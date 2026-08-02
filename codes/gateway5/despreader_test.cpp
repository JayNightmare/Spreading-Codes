#include "gateway5/despreader.h"

#include "gateway1/gold_code_generator.h"
#include "gateway1/spreading_config.h"
#include "gateway4/bpsk_modulator.h"

#include <cmath>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

// Locates config/spreading_codes_config.ini relative to this source file,
// matching the pattern used elsewhere in gateway5 tests (e.g.
// crc_validation_test.cpp) for locating repo-relative reference data.
std::string FindConfigPath() {
    const std::filesystem::path repo_root =
        std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();
    return (repo_root / "config" / "spreading_codes_config.ini").string();
}

bool LoadTables(lunanet::gateway1::SpreadingSpecTables* tables, std::string* error) {
    lunanet::gateway1::Annex3Paths annex3;
    return lunanet::gateway1::LoadSpreadingConfig(FindConfigPath(), tables, &annex3, error);
}

// Builds a noiseless AFS-I chip-rate BPSK stream for the given PRN and data
// symbols, mirroring gateway4::ModulateAfsIData + BpskMap exactly (i.e. the
// same process gateway4::GenerateIq performs at its default sample rate).
std::vector<double> BuildAfsIChipStream(int prn,
                                        const lunanet::gateway1::SpreadingSpecTables& tables,
                                        const std::vector<uint8_t>& data_symbols) {
    std::string err;
    const auto primary_code = lunanet::gateway1::GenerateGoldCode(prn, tables, &err);
    const auto logic_chips = lunanet::gateway4::ModulateAfsIData(primary_code, data_symbols, &err);
    const auto bpsk_chips = lunanet::gateway4::BpskModulate(logic_chips);
    return std::vector<double>(bpsk_chips.begin(), bpsk_chips.end());
}

bool TestNoiselessRoundTripAtZeroPhase(const lunanet::gateway1::SpreadingSpecTables& tables) {
    const int prn = 1;
    const std::vector<uint8_t> data_symbols = {0, 1, 1, 0, 0, 1, 0, 1, 1, 1};
    const auto chip_stream = BuildAfsIChipStream(prn, tables, data_symbols);

    const auto result = lunanet::gateway5::DespreadAfsI(chip_stream, prn, tables);
    if (!result.locked) {
        std::cerr << "FAIL [zero-phase]: expected lock, correlation=" << result.lock_correlation << "\n";
        return false;
    }
    if (result.code_phase != 0) {
        std::cerr << "FAIL [zero-phase]: expected code_phase=0, got " << result.code_phase << "\n";
        return false;
    }
    if (result.symbols.size() != data_symbols.size()) {
        std::cerr << "FAIL [zero-phase]: expected " << data_symbols.size() << " symbols, got "
                  << result.symbols.size() << "\n";
        return false;
    }
    for (size_t k = 0; k < data_symbols.size(); ++k) {
        const bool recovered_bit = result.symbols[k] < 0.0;  // BpskMap: logic1 -> negative
        if (recovered_bit != static_cast<bool>(data_symbols[k])) {
            std::cerr << "FAIL [zero-phase]: symbol " << k << " mismatch\n";
            return false;
        }
        if (result.symbols[k] < 0.99 && result.symbols[k] > -0.99 &&
            std::fabs(std::fabs(result.symbols[k]) - 1.0) > 1e-9) {
            std::cerr << "FAIL [zero-phase]: symbol " << k << " magnitude not ~1.0: "
                      << result.symbols[k] << "\n";
            return false;
        }
    }
    return true;
}

bool TestNoiselessRoundTripWithUnknownPhase(const lunanet::gateway1::SpreadingSpecTables& tables) {
    const int prn = 3;
    const std::vector<uint8_t> data_symbols = {1, 0, 0, 1, 0, 1, 1, 0};
    const auto pattern = BuildAfsIChipStream(prn, tables, data_symbols);

    // Prepend an arbitrary chip-count of unrelated noise-like BPSK values so
    // the true code phase (mod 2046) is neither 0 nor known in advance,
    // exercising the phase-search loop rather than the trivial phase=0 case.
    std::mt19937 rng(0xACE1u);
    std::uniform_real_distribution<double> noise(-1.0, 1.0);
    std::vector<double> stream(777, 0.0);
    for (auto& v : stream) v = (noise(rng) >= 0.0) ? 1.0 : -1.0;
    const std::size_t expected_phase = stream.size() % lunanet::gateway5::kDespreadChipsPerSymbol;
    stream.insert(stream.end(), pattern.begin(), pattern.end());

    const auto result = lunanet::gateway5::DespreadAfsI(stream, prn, tables);
    if (!result.locked) {
        std::cerr << "FAIL [unknown-phase]: expected lock, correlation=" << result.lock_correlation << "\n";
        return false;
    }
    if (result.code_phase != expected_phase) {
        std::cerr << "FAIL [unknown-phase]: expected code_phase=" << expected_phase
                  << ", got " << result.code_phase << "\n";
        return false;
    }
    // The recovered symbol stream starts at code_phase, so the FIRST
    // recovered symbol corresponds to noise (not data) unless expected_phase
    // happens to be 0 -- just confirm the correct data pattern appears
    // somewhere in the tail with the right sign pattern.
    bool found_prefix_match = false;
    for (std::size_t start = 0; start + data_symbols.size() <= result.symbols.size(); ++start) {
        bool all_match = true;
        for (std::size_t k = 0; k < data_symbols.size() && all_match; ++k) {
            const bool recovered_bit = result.symbols[start + k] < 0.0;
            if (recovered_bit != static_cast<bool>(data_symbols[k])) all_match = false;
        }
        if (all_match) {
            found_prefix_match = true;
            break;
        }
    }
    if (!found_prefix_match) {
        std::cerr << "FAIL [unknown-phase]: recovered symbols never match the original data pattern\n";
        return false;
    }
    return true;
}

bool TestWrongPrnFailsToLock(const lunanet::gateway1::SpreadingSpecTables& tables) {
    const int true_prn = 5;
    const int wrong_prn = 6;
    const std::vector<uint8_t> data_symbols = {0, 1, 0, 1, 1, 0};
    const auto chip_stream = BuildAfsIChipStream(true_prn, tables, data_symbols);

    const auto result = lunanet::gateway5::DespreadAfsI(chip_stream, wrong_prn, tables);
    if (result.locked) {
        std::cerr << "FAIL [wrong-prn]: expected no lock, but locked with correlation="
                  << result.lock_correlation << "\n";
        return false;
    }
    return true;
}

bool TestRejectsShortInput(const lunanet::gateway1::SpreadingSpecTables& tables) {
    const std::vector<double> too_short(100, 1.0);
    std::string error;
    const auto result = lunanet::gateway5::DespreadAfsI(too_short, 1, tables, 0.5, &error);
    if (result.locked || error.empty()) {
        std::cerr << "FAIL [short-input]: expected graceful rejection with an error message\n";
        return false;
    }
    return true;
}

}  // namespace

int main() {
    lunanet::gateway1::SpreadingSpecTables tables;
    std::string load_error;
    if (!LoadTables(&tables, &load_error)) {
        std::cerr << "FAIL [setup]: could not load spreading config: " << load_error << "\n";
        return 1;
    }

    bool ok = true;

    if (TestNoiselessRoundTripAtZeroPhase(tables)) {
        std::cout << "PASS: noiseless round-trip recovers data symbols exactly at code_phase=0\n";
    } else {
        ok = false;
    }

    if (TestNoiselessRoundTripWithUnknownPhase(tables)) {
        std::cout << "PASS: code-phase search locates the true phase and recovers data symbols\n";
    } else {
        ok = false;
    }

    if (TestWrongPrnFailsToLock(tables)) {
        std::cout << "PASS: despreading against the wrong PRN correctly fails to lock\n";
    } else {
        ok = false;
    }

    if (TestRejectsShortInput(tables)) {
        std::cout << "PASS: input shorter than one code period is rejected gracefully\n";
    } else {
        ok = false;
    }

    return ok ? 0 : 1;
}
