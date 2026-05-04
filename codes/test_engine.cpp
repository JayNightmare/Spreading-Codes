#include "spreading_codes.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace {

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

bool Expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << std::endl;
        return false;
    }
    return true;
}

std::string Trim(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])) != 0) {
        ++start;
    }

    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
        --end;
    }

    return value.substr(start, end - start);
}

std::string ExtractHexToken(const std::string& line) {
    const std::string trimmed = Trim(line);
    if (trimmed.empty()) {
        return {};
    }

    if (trimmed.find("GoldCode2046") != std::string::npos ||
        trimmed.find("Weil1500") != std::string::npos ||
        trimmed == "[" || trimmed == "]" || trimmed == "];" || trimmed == "\\") {
        return {};
    }

    std::string hex;
    hex.reserve(trimmed.size());
    for (const char ch : trimmed) {
        if (std::isxdigit(static_cast<unsigned char>(ch)) != 0) {
            hex.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
        }
    }

    return hex;
}

bool LoadReferenceHexLines(const std::filesystem::path& path,
                           std::vector<std::string>* out,
                           std::string* error_message) {
    if (out == nullptr) {
        if (error_message != nullptr) {
            *error_message = "LoadReferenceHexLines received null output";
        }
        return false;
    }

    std::ifstream input(path);
    if (!input) {
        if (error_message != nullptr) {
            *error_message = "Failed to open reference file: " + path.string();
        }
        return false;
    }

    out->clear();
    std::string line;
    while (std::getline(input, line)) {
        const std::string token = ExtractHexToken(line);
        if (!token.empty()) {
            out->push_back(token);
        }
    }

    if (out->size() != static_cast<size_t>(lunanet::MAX_PRNS)) {
        if (error_message != nullptr) {
            *error_message = "Expected 210 reference rows in " + path.string() + ", got " +
                             std::to_string(out->size());
        }
        return false;
    }

    return true;
}

size_t FirstMismatchIndex(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    const size_t limit = std::min(a.size(), b.size());
    for (size_t i = 0; i < limit; ++i) {
        if (a[i] != b[i]) {
            return i;
        }
    }
    return limit;
}

template <typename Generator>
bool ValidateAgainstAnnex3(const std::string& label,
                           const std::vector<std::string>& reference_hex,
                           size_t expected_chips,
                           Generator generator) {
    int mismatches = 0;
    bool printed_header = false;

    for (int prn = 1; prn <= lunanet::MAX_PRNS; ++prn) {
        const std::vector<uint8_t> expected = lunanet::hex_to_vector(reference_hex[prn - 1], expected_chips);
        const std::vector<uint8_t> actual = generator(prn);

        if (expected.size() != expected_chips || actual.size() != expected_chips || expected != actual) {
            ++mismatches;
            if (mismatches <= 5) {
                if (!printed_header) {
                    std::cerr << "Reference mismatches for " << label << ":" << std::endl;
                    printed_header = true;
                }
                const size_t mismatch_index = FirstMismatchIndex(actual, expected);
                std::cerr << "  PRN " << prn << " mismatch at chip " << mismatch_index
                          << " (actual_len=" << actual.size() << ", expected_len=" << expected.size() << ")"
                          << std::endl;
            }
        }
    }

    if (mismatches == 0) {
        std::cout << label << " reference validation: PASS (210/210)" << std::endl;
        return true;
    }

    std::cerr << label << " reference validation: FAIL (" << mismatches << "/210 mismatched)" << std::endl;
    return false;
}

}  // namespace

int main(int argc, char** argv) {
    lunanet::initialize_engine();

    const std::string config_path = FindConfigPath((argc > 1) ? argv[1] : nullptr);
    std::string config_error;
    if (!lunanet::load_spreading_code_config(config_path, &config_error)) {
        std::cerr << "FAIL: Could not load spreading code config: " << config_error << std::endl;
        return 1;
    }

    bool ok = true;
    ok &= Expect(std::string(lunanet::hello_moon()) == "Hello Moon", "hello_moon response");

    const std::vector<uint8_t> gold = lunanet::generate_gold_code(1);
    ok &= Expect(gold.size() == lunanet::GOLD_CODE_LENGTH, "Gold code length for PRN 1");

    const std::vector<uint8_t> weil_primary = lunanet::generate_weil_primary(1);
    ok &= Expect(weil_primary.size() == static_cast<size_t>(lunanet::WEIL_PRIMARY_PRIME + lunanet::EXPANSION_LENGTH),
                 "Weil primary length for PRN 1");

    const std::vector<uint8_t> weil_tertiary = lunanet::generate_weil_tertiary(1);
    ok &= Expect(weil_tertiary.size() == lunanet::WEIL_TERTIARY_LENGTH, "Weil tertiary length for PRN 1");

    const size_t afs_q_cap = lunanet::get_afs_q_max_chips();
    const std::vector<uint8_t> afs_q = lunanet::generate_afs_q(1);
    ok &= Expect(!afs_q.empty(), "AFS-Q generated for PRN 1");
    if (afs_q_cap > 0) {
        ok &= Expect(afs_q.size() <= afs_q_cap, "AFS-Q respects configured max chips");
    }

    const std::vector<uint8_t> afs_i_repeat = lunanet::generate_afs_i(1);
    ok &= Expect(afs_i_repeat == gold, "AFS-I deterministic repeatability");

    const std::vector<uint8_t> invalid_gold = lunanet::generate_gold_code(211);
    ok &= Expect(invalid_gold.empty(), "Invalid PRN returns empty sequence");

    const std::map<int, std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> all_codes =
        lunanet::generate_all_spreading_codes();
    ok &= Expect(all_codes.size() == static_cast<size_t>(lunanet::MAX_PRNS), "Batch generation for all PRNs");

    if (!all_codes.empty()) {
        const auto it = all_codes.find(1);
        ok &= Expect(it != all_codes.end(), "PRN 1 exists in batch output");
        if (it != all_codes.end()) {
            ok &= Expect(it->second.first.size() == lunanet::GOLD_CODE_LENGTH, "Batch AFS-I length");
            ok &= Expect(!it->second.second.empty(), "Batch AFS-Q non-empty");
        }
    }

    std::cout << "Sample PRN 1 Gold HEX[24]: " << lunanet::chips_to_hex(gold, 24) << std::endl;
    std::cout << "Sample PRN 1 Weil Primary HEX[24]: " << lunanet::chips_to_hex(weil_primary, 24) << std::endl;
    std::cout << "Sample PRN 1 Weil Tertiary HEX[24]: " << lunanet::chips_to_hex(weil_tertiary, 24) << std::endl;

    const std::filesystem::path repo_root = std::filesystem::path(config_path).parent_path().parent_path();
    const std::filesystem::path annex3_txt_dir = repo_root / "PRN" / "annex3" / "txt";

    std::vector<std::string> annex3_gold_hex;
    std::vector<std::string> annex3_weil_primary_hex;
    std::vector<std::string> annex3_weil_tertiary_hex;
    std::string annex_error;

    ok &= LoadReferenceHexLines(annex3_txt_dir / "006_GoldCode2046hex210prns.txt", &annex3_gold_hex, &annex_error);
    if (!ok) {
        std::cerr << "FAIL: " << annex_error << std::endl;
    }

    annex_error.clear();
    ok &= LoadReferenceHexLines(annex3_txt_dir / "007_l1cp_hex210prns.txt", &annex3_weil_primary_hex, &annex_error);
    if (!ok && !annex_error.empty()) {
        std::cerr << "FAIL: " << annex_error << std::endl;
    }

    annex_error.clear();
    ok &= LoadReferenceHexLines(annex3_txt_dir / "008_Weil1500hex210prns.txt", &annex3_weil_tertiary_hex, &annex_error);
    if (!ok && !annex_error.empty()) {
        std::cerr << "FAIL: " << annex_error << std::endl;
    }

    if (ok) {
        ok &= ValidateAgainstAnnex3(
            "Gold",
            annex3_gold_hex,
            lunanet::GOLD_CODE_LENGTH,
            [](int prn) { return lunanet::generate_gold_code(prn); });

        ok &= ValidateAgainstAnnex3(
            "Weil Primary",
            annex3_weil_primary_hex,
            static_cast<size_t>(lunanet::WEIL_PRIMARY_PRIME + lunanet::EXPANSION_LENGTH),
            [](int prn) { return lunanet::generate_weil_primary(prn); });

        ok &= ValidateAgainstAnnex3(
            "Weil Tertiary",
            annex3_weil_tertiary_hex,
            lunanet::WEIL_TERTIARY_LENGTH,
            [](int prn) { return lunanet::generate_weil_tertiary(prn); });
    }

    if (!ok) {
        return 1;
    }

    std::cout << "Gateway 1 modular foundation checks passed." << std::endl;
    return 0;
}
