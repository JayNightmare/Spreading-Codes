#include "spreading_codes.h"

#include "testing/test_annex3_loader.h"
#include "testing/test_reporter.h"
#include "testing/test_validators.h"

#include "gateway1/spreading_config.h"
#include "gateway2/bch_codec.h"
#include "gateway2/crc24.h"
#include "gateway2/interleaver.h"
#include "gateway2/ldpc_encoder.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

enum class GatewaySelection {
    kAll,
    kGateway1,
    kGateway2,
};

struct RunOptions {
    std::string config_override;
    std::string reports_base_override;
    GatewaySelection gateway = GatewaySelection::kAll;
    bool show_help = false;
    std::string parse_error;
};

std::string ToLowerAscii(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
    return value;
}

bool ParseGatewaySelection(const std::string& value, GatewaySelection* selection) {
    const std::string normalized = ToLowerAscii(value);
    if (normalized == "all") {
        *selection = GatewaySelection::kAll;
        return true;
    }
    if (normalized == "gateway1" || normalized == "g1" || normalized == "1") {
        *selection = GatewaySelection::kGateway1;
        return true;
    }
    if (normalized == "gateway2" || normalized == "g2" || normalized == "2") {
        *selection = GatewaySelection::kGateway2;
        return true;
    }
    return false;
}

std::string GatewayScopeName(GatewaySelection selection) {
    switch (selection) {
        case GatewaySelection::kGateway1: return "gateway1";
        case GatewaySelection::kGateway2: return "gateway2";
        case GatewaySelection::kAll:
        default: return "all";
    }
}

void PrintUsage(std::ostream& out, const char* exe_name) {
    out << "Usage: " << exe_name
        << " [config_path] [reports_base] [--gateway <all|gateway1|gateway2>]" << std::endl;
    out << "       " << exe_name
        << " [--gateway <all|gateway1|gateway2>] [config_path] [reports_base]" << std::endl;
    out << std::endl;
    out << "Options:" << std::endl;
    out << "  --gateway, -g  Validation scope to run (default: all)." << std::endl;
    out << "  --help, -h     Show this help message." << std::endl;
}

RunOptions ParseRunOptions(int argc, char** argv) {
    RunOptions options;
    std::vector<std::string> positional;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            options.show_help = true;
            return options;
        }

        if (arg == "--gateway" || arg == "-g") {
            if (i + 1 >= argc) {
                options.parse_error = "Missing value for --gateway";
                return options;
            }
            const std::string value = argv[++i];
            if (!ParseGatewaySelection(value, &options.gateway)) {
                options.parse_error = "Invalid --gateway value: '" + value + "'";
                return options;
            }
            continue;
        }

        if (arg.rfind("--gateway=", 0) == 0) {
            const std::string value = arg.substr(std::string("--gateway=").size());
            if (!ParseGatewaySelection(value, &options.gateway)) {
                options.parse_error = "Invalid --gateway value: '" + value + "'";
                return options;
            }
            continue;
        }

        if (!arg.empty() && arg[0] == '-') {
            options.parse_error = "Unknown option: " + arg;
            return options;
        }

        positional.push_back(arg);
    }

    if (positional.size() > 2) {
        options.parse_error = "Too many positional arguments";
        return options;
    }

    if (!positional.empty()) {
        options.config_override = positional[0];
    }
    if (positional.size() == 2) {
        options.reports_base_override = positional[1];
    }

    return options;
}

std::string FindConfigPath(const char* override_path) {
    if (override_path != nullptr && std::string(override_path).size() > 0) {
        return override_path;
    }

    std::filesystem::path current = std::filesystem::current_path();
    for (int i = 0; i < 5; ++i) {
        const std::filesystem::path candidate = current / "config" / "spreading_codes_config.ini";
        if (std::filesystem::exists(candidate)) {
            return candidate.string();
        }
        if (!current.has_parent_path()) {
            break;
        }
        current = current.parent_path();
    }

    return "config/spreading_codes_config.ini";
}

void RunSmokeTests(lunanet::testing::TestReporter& reporter) {
    const std::string kSuite = "Smoke";

    lunanet::testing::ValidateCondition(
        kSuite, "hello_moon response",
        std::string(lunanet::hello_moon()) == "Hello Moon",
        "Expected 'Hello Moon'", reporter);

    const std::vector<uint8_t> gold = lunanet::generate_gold_code(1);
    lunanet::testing::ValidateCodeLength(
        kSuite, "Gold code length PRN 1",
        gold, lunanet::GOLD_CODE_LENGTH, reporter);

    const std::vector<uint8_t> weil_primary = lunanet::generate_weil_primary(1);
    lunanet::testing::ValidateCodeLength(
        kSuite, "Weil primary length PRN 1",
        weil_primary,
        static_cast<size_t>(lunanet::WEIL_PRIMARY_PRIME + lunanet::EXPANSION_LENGTH),
        reporter);

    const std::vector<uint8_t> weil_tertiary = lunanet::generate_weil_tertiary(1);
    lunanet::testing::ValidateCodeLength(
        kSuite, "Weil tertiary length PRN 1",
        weil_tertiary, lunanet::WEIL_TERTIARY_LENGTH, reporter);

    const std::vector<uint8_t> afs_q = lunanet::generate_afs_q(1);
    lunanet::testing::ValidateNonEmpty(kSuite, "AFS-Q generated PRN 1", afs_q, reporter);

    const size_t afs_q_cap = lunanet::get_afs_q_max_chips();
    if (afs_q_cap > 0) {
        lunanet::testing::ValidateCondition(
            kSuite, "AFS-Q respects configured max chips",
            afs_q.size() <= afs_q_cap,
            "Got " + std::to_string(afs_q.size()) + " chips, cap is " + std::to_string(afs_q_cap),
            reporter);
    }

    const std::vector<uint8_t> afs_i = lunanet::generate_afs_i(1);
    lunanet::testing::ValidateEquality(
        kSuite, "AFS-I deterministic repeatability", afs_i, gold, reporter);

    lunanet::testing::ValidateBoundsRejection(
        kSuite, "Invalid PRN 211 returns empty",
        [](int prn) { return lunanet::generate_gold_code(prn); }, 211, reporter);

    const auto all_codes = lunanet::generate_all_spreading_codes();
    lunanet::testing::ValidateCondition(
        kSuite, "Batch generation for all PRNs",
        all_codes.size() == static_cast<size_t>(lunanet::MAX_PRNS),
        "Got " + std::to_string(all_codes.size()) + " PRNs", reporter);

    if (!all_codes.empty()) {
        const auto it = all_codes.find(1);
        lunanet::testing::ValidateCondition(
            kSuite, "PRN 1 exists in batch output",
            it != all_codes.end(), "PRN 1 missing from batch", reporter);
        if (it != all_codes.end()) {
            lunanet::testing::ValidateCodeLength(
                kSuite, "Batch AFS-I length",
                it->second.first, lunanet::GOLD_CODE_LENGTH, reporter);
            lunanet::testing::ValidateNonEmpty(
                kSuite, "Batch AFS-Q non-empty",
                it->second.second, reporter);
        }
    }
}

bool RunAnnex3Validation(
    const std::filesystem::path& annex3_dir,
    lunanet::testing::TestReporter& reporter) {

    std::vector<std::string> gold_hex;
    std::vector<std::string> weil_primary_hex;
    std::vector<std::string> weil_tertiary_hex;
    std::string error;

    if (!lunanet::testing::LoadAnnex3HexLines(
            annex3_dir / "006_GoldCode2046hex210prns.txt",
            lunanet::MAX_PRNS, gold_hex, error)) {
        reporter.Record({"Annex3/Gold", "Load reference file",
            lunanet::testing::TestStatus::kFail, error, 0.0});
        return false;
    }

    if (!lunanet::testing::LoadAnnex3HexLines(
            annex3_dir / "007_l1cp_hex210prns.txt",
            lunanet::MAX_PRNS, weil_primary_hex, error)) {
        reporter.Record({"Annex3/Weil Primary", "Load reference file",
            lunanet::testing::TestStatus::kFail, error, 0.0});
        return false;
    }

    if (!lunanet::testing::LoadAnnex3HexLines(
            annex3_dir / "008_Weil1500hex210prns.txt",
            lunanet::MAX_PRNS, weil_tertiary_hex, error)) {
        reporter.Record({"Annex3/Weil Tertiary", "Load reference file",
            lunanet::testing::TestStatus::kFail, error, 0.0});
        return false;
    }

    lunanet::testing::ValidateAnnex3Suite(
        "Annex3/Gold", gold_hex,
        lunanet::GOLD_CODE_LENGTH, lunanet::MAX_PRNS,
        [](int prn) { return lunanet::generate_gold_code(prn); },
        reporter);

    lunanet::testing::ValidateAnnex3Suite(
        "Annex3/Weil Primary", weil_primary_hex,
        static_cast<size_t>(lunanet::WEIL_PRIMARY_PRIME + lunanet::EXPANSION_LENGTH),
        lunanet::MAX_PRNS,
        [](int prn) { return lunanet::generate_weil_primary(prn); },
        reporter);

    lunanet::testing::ValidateAnnex3Suite(
        "Annex3/Weil Tertiary", weil_tertiary_hex,
        lunanet::WEIL_TERTIARY_LENGTH, lunanet::MAX_PRNS,
        [](int prn) { return lunanet::generate_weil_tertiary(prn); },
        reporter);

    return true;
}

void RunPerformanceBenchmarks(lunanet::testing::TestReporter& reporter) {
    using Clock = std::chrono::high_resolution_clock;
    const std::string kSuite = "Performance";

    for (int prn : {1, 105, 210}) {
        const auto start = Clock::now();

        static_cast<void>(lunanet::generate_gold_code(prn));
        static_cast<void>(lunanet::generate_weil_primary(prn));
        static_cast<void>(lunanet::generate_weil_tertiary(prn));
        static_cast<void>(lunanet::generate_afs_q(prn));

        const double elapsed_ms =
            std::chrono::duration<double, std::milli>(Clock::now() - start).count();

        reporter.Record({kSuite,
            "PRN " + std::to_string(prn) + " full generation",
            elapsed_ms < 1000.0
                ? lunanet::testing::TestStatus::kPass
                : lunanet::testing::TestStatus::kFail,
            std::to_string(elapsed_ms) + " ms",
            elapsed_ms});
    }
}

void RunTable11Validation(
    const std::string& config_path,
    lunanet::testing::TestReporter& reporter) {

    lunanet::gateway1::SpreadingSpecTables tables;
    lunanet::gateway1::Annex3Paths annex3;
    std::string error;

    if (!lunanet::gateway1::LoadSpreadingConfig(config_path, &tables, &annex3, &error)) {
        reporter.Record({"Table11/Assignments", "Load config",
            lunanet::testing::TestStatus::kFail, error, 0.0});
        return;
    }

    // Table 11 defines nodes 1-12 with PRN identity mapping and
    // secondary code cycling S0, S1, S2, S3, S0, S1, S2, S3, ...
    std::vector<lunanet::testing::Table11Entry> entries;
    for (int node = 1; node <= 12; ++node) {
        entries.push_back({
            node,
            node,                     // prn_i = node_id
            node,                     // prn_q = node_id
            (node - 1) % 4,           // S0, S1, S2, S3 cycling
            node,                     // tertiary_prn = node_id
            0,                        // phase_offset = 0
        });
    }

    lunanet::testing::ValidateTable11Suite(
        entries,
        tables.secondary_code_by_prn,
        tables.secondary_codes,
        [](int prn) { return lunanet::generate_gold_code(prn); },
        [](int prn) { return lunanet::generate_weil_primary(prn); },
        [](int prn) { return lunanet::generate_weil_tertiary(prn); },
        [](int prn) { return lunanet::generate_afs_q(prn); },
        reporter);
}

void RunGateway2Tests(lunanet::testing::TestReporter& reporter) {
    const std::string kBch = "Gateway2/BCH";
    const std::string kCrc = "Gateway2/CRC24";
    const std::string kIl = "Gateway2/Interleaver";

    // ── BCH(51,8) Tests ─────────────────────────────────────────────────

    // Test: encode produces 52 symbols
    {
        const auto encoded = lunanet::gateway2::BchEncode(0x000);  // FID=0, TOI=0
        lunanet::testing::ValidateCondition(
            kBch, "Encode length (SB1=0x000)",
            encoded.size() == 52,
            "Got " + std::to_string(encoded.size()) + " symbols, expected 52",
            reporter);
    }

    // Test: all encoded symbols are binary
    {
        const auto encoded = lunanet::gateway2::BchEncode(0x0A5);
        bool all_binary = true;
        for (const auto s : encoded) {
            if (s > 1) { all_binary = false; break; }
        }
        lunanet::testing::ValidateCondition(
            kBch, "Symbols are binary (SB1=0x0A5)",
            all_binary, "Non-binary symbol found", reporter);
    }

    // Test: round-trip encode → soft → decode recovers original
    for (uint16_t sb1 : {0x000, 0x001, 0x07F, 0x080, 0x0FF, 0x100, 0x1FF}) {
        const auto encoded = lunanet::gateway2::BchEncode(sb1);

        // Convert to soft decisions: 0 → +1.0, 1 → -1.0
        std::vector<double> soft(encoded.begin(), encoded.end());
        for (auto& s : soft) { s = (s == 0) ? 1.0 : -1.0; }

        const int decoded = lunanet::gateway2::BchDecodeSoft(soft);
        lunanet::testing::ValidateCondition(
            kBch, "Round-trip SB1=0x" +
                ([](uint16_t v) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "%03X", v);
                    return std::string(buf);
                })(sb1),
            decoded == sb1,
            "Decoded 0x" + std::to_string(decoded) +
            ", expected 0x" + std::to_string(sb1),
            reporter);
    }

    // Test: MSB=1 case (FID=2, TOI=0 → SB1=0x100)
    {
        const auto encoded = lunanet::gateway2::BchEncode(0x100);
        lunanet::testing::ValidateCondition(
            kBch, "MSB=1 prepended (SB1=0x100)",
            !encoded.empty() && encoded[0] == 1,
            "First symbol should be 1 for MSB=1", reporter);
    }

    // ── CRC-24 Tests ────────────────────────────────────────────────────

    // Test: CRC of empty data
    {
        std::vector<uint8_t> data;
        const uint32_t crc = lunanet::gateway2::Crc24Compute(data);
        lunanet::testing::ValidateCondition(
            kCrc, "Empty data CRC",
            crc == 0x000000,
            "CRC of empty data should be 0, got 0x" + std::to_string(crc),
            reporter);
    }

    // Test: Append + Verify round-trip
    {
        std::vector<uint8_t> data = {1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1};
        lunanet::gateway2::Crc24Append(data);
        lunanet::testing::ValidateCondition(
            kCrc, "Append grows by 24 bits",
            data.size() == 12 + 24,
            "Size " + std::to_string(data.size()), reporter);

        lunanet::testing::ValidateCondition(
            kCrc, "Verify after append",
            lunanet::gateway2::Crc24Verify(data),
            "CRC verification failed on valid data", reporter);
    }

    // Test: Corruption detection
    {
        std::vector<uint8_t> data = {0, 1, 0, 1, 0, 1, 0, 1};
        lunanet::gateway2::Crc24Append(data);
        data[3] ^= 1;  // Flip one bit
        lunanet::testing::ValidateCondition(
            kCrc, "Detect single bit corruption",
            !lunanet::gateway2::Crc24Verify(data),
            "CRC should fail after corruption", reporter);
    }

    // ── Block Interleaver Tests ──────────────────────────────────────────

    // Test: round-trip
    {
        std::vector<uint8_t> data(lunanet::gateway2::kInterleaverSize);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = static_cast<uint8_t>(i % 2);
        }

        const auto interleaved = lunanet::gateway2::Interleave(data);
        lunanet::testing::ValidateCondition(
            kIl, "Interleave output size",
            interleaved.size() == lunanet::gateway2::kInterleaverSize,
            "Got " + std::to_string(interleaved.size()), reporter);

        const auto deinterleaved = lunanet::gateway2::Deinterleave(interleaved);
        lunanet::testing::ValidateCondition(
            kIl, "Round-trip (interleave→deinterleave)",
            deinterleaved == data,
            "Data mismatch after round-trip", reporter);
    }

    // Test: interleaver rejects wrong size
    {
        std::vector<uint8_t> bad(100, 0);
        lunanet::testing::ValidateCondition(
            kIl, "Reject invalid size",
            lunanet::gateway2::Interleave(bad).empty(),
            "Should return empty for non-5880 input", reporter);
    }

    // Test: interleaving actually permutes data
    {
        std::vector<uint8_t> seq(lunanet::gateway2::kInterleaverSize, 0);
        seq[1] = 1;  // Single non-zero element at position 1
        const auto interleaved = lunanet::gateway2::Interleave(seq);
        lunanet::testing::ValidateCondition(
            kIl, "Permutation changes positions",
            interleaved != seq,
            "Interleaved output should differ from input", reporter);
    }
}

void RunLdpcTests(
    const std::filesystem::path& repo_root,
    lunanet::testing::TestReporter& reporter) {

    const std::string kSuite = "Gateway2/LDPC";
    const std::filesystem::path csv_dir = repo_root / "Validation" / "annex3" / "csv";

    // ── Load SB2 matrices ────────────────────────────────────────────────
    lunanet::gateway2::LdpcMatrices sb2_matrices;
    std::string error;

    using Clock = std::chrono::high_resolution_clock;
    auto load_start = Clock::now();

    const bool sb2_loaded = lunanet::gateway2::LoadLdpcMatrices(
        (csv_dir / "004h_lunanet_sf2_ldpc_submatrix_a_mat.csv").string(),
        (csv_dir / "004i_lunanet_sf2_ldpc_submatrix_b_inv_mat.csv").string(),
        (csv_dir / "004f_lunanet_sf2_ldpc_submatrix_c_mat.csv").string(),
        (csv_dir / "004g_lunanet_sf2_ldpc_submatrix_d_mat.csv").string(),
        &sb2_matrices, &error);

    double load_ms = std::chrono::duration<double, std::milli>(Clock::now() - load_start).count();

    lunanet::testing::ValidateConditionTimed(
        kSuite, "Load SB2 submatrices",
        sb2_loaded, error, load_ms, reporter);

    if (sb2_loaded) {
        // Validate SB2 matrix dimensions
        lunanet::testing::ValidateCondition(
            kSuite, "SB2 A dimensions (480x1200)",
            sb2_matrices.a.rows == 480 && sb2_matrices.a.cols == 1200,
            "A: " + std::to_string(sb2_matrices.a.rows) + "x" + std::to_string(sb2_matrices.a.cols),
            reporter);

        lunanet::testing::ValidateCondition(
            kSuite, "SB2 B_inv dimensions (480x480)",
            sb2_matrices.b_inv.rows == 480 && sb2_matrices.b_inv.cols == 480,
            "B_inv: " + std::to_string(sb2_matrices.b_inv.rows) + "x" + std::to_string(sb2_matrices.b_inv.cols),
            reporter);

        // Encode SB2 test data (all-zeros)
        std::vector<uint8_t> sb2_data(lunanet::gateway2::kLdpcSb2.data_bits, 0);
        auto enc_start = Clock::now();
        const auto sb2_encoded = lunanet::gateway2::LdpcEncode(
            sb2_data, sb2_matrices, lunanet::gateway2::kLdpcSb2, &error);
        double enc_ms = std::chrono::duration<double, std::milli>(Clock::now() - enc_start).count();

        lunanet::testing::ValidateConditionTimed(
            kSuite, "SB2 encode output length (2400)",
            static_cast<int>(sb2_encoded.size()) == lunanet::gateway2::kLdpcSb2.output_symbols,
            "Got " + std::to_string(sb2_encoded.size()) + (error.empty() ? "" : ": " + error),
            enc_ms, reporter);

        // Verify all symbols are binary
        if (!sb2_encoded.empty()) {
            bool all_binary = true;
            for (const auto s : sb2_encoded) {
                if (s > 1) { all_binary = false; break; }
            }
            lunanet::testing::ValidateCondition(
                kSuite, "SB2 encoded symbols are binary",
                all_binary, "Non-binary symbol found", reporter);
        }

        // Encode SB2 with non-zero data
        std::vector<uint8_t> sb2_data2(lunanet::gateway2::kLdpcSb2.data_bits, 0);
        for (size_t i = 0; i < sb2_data2.size(); i += 3) sb2_data2[i] = 1;
        const auto sb2_enc2 = lunanet::gateway2::LdpcEncode(
            sb2_data2, sb2_matrices, lunanet::gateway2::kLdpcSb2, &error);
        lunanet::testing::ValidateCondition(
            kSuite, "SB2 encode non-zero data",
            static_cast<int>(sb2_enc2.size()) == lunanet::gateway2::kLdpcSb2.output_symbols,
            "Got " + std::to_string(sb2_enc2.size()),
            reporter);

        // SB2 performance check (< 100ms)
        lunanet::testing::ValidateCondition(
            kSuite, "SB2 encode < 100ms",
            enc_ms < 100.0,
            std::to_string(enc_ms) + " ms", reporter);
    }

    // ── Load SB3/SB4 matrices ─────────────────────────────────────────────
    lunanet::gateway2::LdpcMatrices sb3_matrices;
    error.clear();

    load_start = Clock::now();
    const bool sb3_loaded = lunanet::gateway2::LoadLdpcMatrices(
        (csv_dir / "004a_lunanet_sf3_ldpc_submatrix_a_mat.csv").string(),
        (csv_dir / "004b_lunanet_sf3_ldpc_submatrix_b_inv_mat.csv").string(),
        (csv_dir / "004d_lunanet_sf3_ldpc_submatrix_c_mat.csv").string(),
        (csv_dir / "004e_lunanet_sf3_ldpc_submatrix_d_mat.csv").string(),
        &sb3_matrices, &error);
    load_ms = std::chrono::duration<double, std::milli>(Clock::now() - load_start).count();

    lunanet::testing::ValidateConditionTimed(
        kSuite, "Load SB3/SB4 submatrices",
        sb3_loaded, error, load_ms, reporter);

    if (sb3_loaded) {
        // Validate SB3 matrix dimensions
        lunanet::testing::ValidateCondition(
            kSuite, "SB3 A dimensions (352x880)",
            sb3_matrices.a.rows == 352 && sb3_matrices.a.cols == 880,
            "A: " + std::to_string(sb3_matrices.a.rows) + "x" + std::to_string(sb3_matrices.a.cols),
            reporter);

        // Encode SB3 test data (870 data bits + 10 filler)
        std::vector<uint8_t> sb3_data(lunanet::gateway2::kLdpcSb34.data_bits, 0);
        auto enc_start = Clock::now();
        const auto sb3_encoded = lunanet::gateway2::LdpcEncode(
            sb3_data, sb3_matrices, lunanet::gateway2::kLdpcSb34, &error);
        double enc_ms = std::chrono::duration<double, std::milli>(Clock::now() - enc_start).count();

        lunanet::testing::ValidateConditionTimed(
            kSuite, "SB3 encode output length (1740)",
            static_cast<int>(sb3_encoded.size()) == lunanet::gateway2::kLdpcSb34.output_symbols,
            "Got " + std::to_string(sb3_encoded.size()) + (error.empty() ? "" : ": " + error),
            enc_ms, reporter);

        // Verify binary
        if (!sb3_encoded.empty()) {
            bool all_binary = true;
            for (const auto s : sb3_encoded) {
                if (s > 1) { all_binary = false; break; }
            }
            lunanet::testing::ValidateCondition(
                kSuite, "SB3 encoded symbols are binary",
                all_binary, "Non-binary symbol found", reporter);
        }

        lunanet::testing::ValidateCondition(
            kSuite, "SB3 encode < 100ms",
            enc_ms < 100.0,
            std::to_string(enc_ms) + " ms", reporter);
    }
}

}  // namespace

int main(int argc, char** argv) {
    const RunOptions options = ParseRunOptions(argc, argv);
    if (!options.parse_error.empty()) {
        std::cerr << "FAIL: " << options.parse_error << std::endl;
        PrintUsage(std::cerr, argv[0]);
        return 1;
    }
    if (options.show_help) {
        PrintUsage(std::cout, argv[0]);
        return 0;
    }

    lunanet::initialize_engine();

    const char* config_override =
        options.config_override.empty() ? nullptr : options.config_override.c_str();
    const std::string config_path = FindConfigPath(config_override);
    std::string config_error;
    if (!lunanet::load_spreading_code_config(config_path, &config_error)) {
        std::cerr << "FAIL: Could not load spreading code config: " << config_error << std::endl;
        return 1;
    }

    lunanet::testing::TestReporter reporter;
    const bool run_gateway1 =
        options.gateway == GatewaySelection::kAll ||
        options.gateway == GatewaySelection::kGateway1;
    const bool run_gateway2 =
        options.gateway == GatewaySelection::kAll ||
        options.gateway == GatewaySelection::kGateway2;

    std::cout << "Validation scope: " << GatewayScopeName(options.gateway) << std::endl;

    const std::filesystem::path repo_root =
        std::filesystem::path(config_path).parent_path().parent_path();
    const std::filesystem::path annex3_txt_dir = repo_root / "Validation" / "annex3" / "txt";

    if (run_gateway1) {
        RunSmokeTests(reporter);

        std::cout << "Sample PRN 1 Gold HEX[24]: "
                  << lunanet::chips_to_hex(lunanet::generate_gold_code(1), 24) << std::endl;
        std::cout << "Sample PRN 1 Weil Primary HEX[24]: "
                  << lunanet::chips_to_hex(lunanet::generate_weil_primary(1), 24) << std::endl;
        std::cout << "Sample PRN 1 Weil Tertiary HEX[24]: "
                  << lunanet::chips_to_hex(lunanet::generate_weil_tertiary(1), 24) << std::endl;

        RunAnnex3Validation(annex3_txt_dir, reporter);
        RunTable11Validation(config_path, reporter);
        RunPerformanceBenchmarks(reporter);
    }

    if (run_gateway2) {
        RunGateway2Tests(reporter);
        RunLdpcTests(repo_root, reporter);
    }

    reporter.PrintSummary(std::cout);

    const std::filesystem::path reports_base =
        options.reports_base_override.empty()
            ? (repo_root / "Validation" / "reports")
            : std::filesystem::path(options.reports_base_override);

    const auto now = std::chrono::system_clock::now();
    const std::time_t now_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
#ifdef _WIN32
    localtime_s(&tm_buf, &now_t);
#else
    localtime_r(&now_t, &tm_buf);
#endif

    std::ostringstream date_ss;
    date_ss << std::put_time(&tm_buf, "%Y-%m-%d");
    std::ostringstream time_ss;
    time_ss << std::put_time(&tm_buf, "%H-%M-%S");

    const std::filesystem::path report_dir = reports_base / date_ss.str();
    std::string report_stem = time_ss.str();
    if (options.gateway != GatewaySelection::kAll) {
        report_stem += "_" + GatewayScopeName(options.gateway);
    }

    if (!reporter.WriteMarkdownReport(report_dir / (report_stem + ".md"))) {
        std::cerr << "Warning: Failed to write markdown report." << std::endl;
    }
    if (!reporter.WriteJUnitXml(report_dir / (report_stem + ".xml"))) {
        std::cerr << "Warning: Failed to write JUnit XML report." << std::endl;
    }

    std::cout << "Reports written to: " << report_dir.string() << std::endl;
    return reporter.AllPassed() ? 0 : 1;
}
