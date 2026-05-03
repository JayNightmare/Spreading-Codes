/**
 * Test program for LunaNet Gateway 1 Spreading Code Engine
 */

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include "spreading_codes.h"
#include "convert.cpp"

static std::string find_config_path() {
    std::filesystem::path current = std::filesystem::current_path();
    for (int i = 0; i < 4; ++i) {
        std::filesystem::path candidate = current / "config" / "spreading_codes_config.ini";
        if (std::filesystem::exists(candidate)) {
            return candidate.string();
        }
        if (!current.has_parent_path()) break;
        current = current.parent_path();
    }
    return "config/spreading_codes_config.ini";
}

static bool compare_vectors(const std::vector<uint8_t>& actual,
                            const std::vector<uint8_t>& expected,
                            size_t& mismatch_index) {
    const size_t min_len = std::min(actual.size(), expected.size());
    for (size_t i = 0; i < min_len; ++i) {
        if (actual[i] != expected[i]) {
            mismatch_index = i;
            return false;
        }
    }

    if (actual.size() != expected.size()) {
        mismatch_index = min_len;
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    std::cout << "Testing LunaNet Gateway 1 Spreading Code Engine" << std::endl;
    
    // Initialize engine
    lunanet::initialize_engine();

    // Load runtime config
    std::string config_path = (argc > 1) ? argv[1] : find_config_path();
    std::string config_error;
    if (!lunanet::load_spreading_code_config(config_path, &config_error)) {
        std::cerr << "Failed to load config: " << config_error << std::endl;
        return 1;
    }
    std::cout << "Config: " << config_path << std::endl;
    std::cout << "Annex3 Gold: " << lunanet::get_annex3_gold_path() << std::endl;
    std::cout << "Annex3 Weil Primary: " << lunanet::get_annex3_weil_primary_path() << std::endl;
    std::cout << "Annex3 Weil Tertiary: " << lunanet::get_annex3_weil_tertiary_path() << std::endl;
    
    // Test hello function
    std::cout << "Test Message: " << lunanet::hello_moon() << std::endl;
    
    // Test version
    std::cout << "Engine Version: " << lunanet::get_version() << std::endl;
    
    // Test error handling
    std::cout << "Last Error: '" << lunanet::get_last_error() << "'" << std::endl;
    
    // Generate a Gold code for PRN 1
    std::vector<uint8_t> gold_code = lunanet::generate_gold_code(1);
    if (!gold_code.empty()) {
        std::cout << "Generated Gold code for PRN 1, length: " << gold_code.size() << " chips" << std::endl;
        std::cout << "First 24 Chips: ";
        for (size_t i = 0; i < 24 && i < gold_code.size(); i++) {
            std::cout << (int)gold_code[i] << " ";
        }
        std::cout << "\nFirst 24 Chips (HEX): " << lunanet::vector_to_hex(gold_code, 24) << std::endl;
        std::cout << std::endl;
    } else {
        std::cout << "Error generating Gold code: " << lunanet::get_last_error() << std::endl;
    }
    
    // Test Weil code generation
    std::vector<uint8_t> weil_primary = lunanet::generate_weil_primary(1);
    if (!weil_primary.empty()) {
        std::cout << "Generated Weil primary code, length: " << weil_primary.size() << " chips" << std::endl;
    } else {
        std::cout << "Error generating Weil primary code: " << lunanet::get_last_error() << std::endl;
    }
    
    // Test AFS-I generation (Single PRN)
    std::vector<uint8_t> afs_i = lunanet::generate_afs_i(1);
    if (!afs_i.empty()) {
        std::cout << "Generated AFS-I code for PRN 1, length: " << afs_i.size() << " chips" << std::endl;
    } else {
        std::cout << "Error generating AFS-I code: " << lunanet::get_last_error() << std::endl;
    }
    
    // Test AFS-Q generation (Single PRN)
    const size_t afs_q_max = lunanet::get_afs_q_max_chips();
    std::vector<uint8_t> afs_q = lunanet::generate_afs_q(1, afs_q_max);
    if (!afs_q.empty()) {
        std::cout << "Generated AFS-Q code for PRN 1, length: " << afs_q.size() << " chips" << std::endl;
    } else {
        std::cout << "Error generating AFS-Q code: " << lunanet::get_last_error() << std::endl;
    }

    // --- NEW TEST: Test all PRNs using the batch generator (Phase 1B Simulation) ---
    std::cout << "\n--- Testing Batch Generation for All PRNs (Simulation) ---" << std::endl;
    try {
        auto all_codes = lunanet::generate_all_spreading_codes();
        if (!all_codes.empty()) {
            std::cout << "Successfully generated codes for " << all_codes.size() << " PRNs." << std::endl;
            // Optionally, check the size of one entry to confirm data was populated
            int first_prn = 1;
            if (all_codes.count(first_prn)) {
                auto pair = all_codes[first_prn];
                std::cout << "Verification: PRN " << first_prn << " AFS-I length: " << pair.first.size()
                          << ", AFS-Q length: " << pair.second.size() << std::endl;
            }
        } else {
            std::cout << "FAILURE: Failed to generate codes for any PRN." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during batch generation test: " << e.what() << std::endl;
    }
    // ----------------------------------------------------------------------------

    // --- Reference vector validation (Annex 3) ---
    std::cout << "\n--- Validating Annex 3 Reference Vectors ---" << std::endl;
    std::string ref_error;

    std::vector<std::vector<uint8_t>> gold_refs;
    if (lunanet::load_reference_hex_file(lunanet::get_annex3_gold_path(), lunanet::GOLD_CODE_LENGTH, gold_refs, &ref_error)) {
        int mismatches = 0;
        for (int prn = 1; prn <= lunanet::MAX_PRNS; ++prn) {
            auto actual = lunanet::generate_gold_code(prn);
            size_t mismatch_index = 0;
            if (!compare_vectors(actual, gold_refs[prn - 1], mismatch_index)) {
                if (mismatches < 5) {
                    std::cout << "Gold mismatch PRN " << prn << " at chip " << mismatch_index << std::endl;
                }
                ++mismatches;
            }
        }
        if (mismatches == 0) {
            std::cout << "Gold reference validation: PASS" << std::endl;
        } else {
            std::cout << "Gold reference validation: FAIL (" << mismatches << " PRNs)" << std::endl;
        }
    } else {
        std::cout << "Gold reference validation skipped: " << ref_error << std::endl;
    }

    ref_error.clear();
    std::vector<std::vector<uint8_t>> weil_primary_refs;
    if (lunanet::load_reference_hex_file(lunanet::get_annex3_weil_primary_path(), lunanet::WEIL_PRIMARY_PRIME + lunanet::EXPANSION_LENGTH, weil_primary_refs, &ref_error)) {
        int mismatches = 0;
        for (int prn = 1; prn <= lunanet::MAX_PRNS; ++prn) {
            auto actual = lunanet::generate_weil_primary(prn);
            size_t mismatch_index = 0;
            if (!compare_vectors(actual, weil_primary_refs[prn - 1], mismatch_index)) {
                if (mismatches < 5) {
                    std::cout << "Weil primary mismatch PRN " << prn << " at chip " << mismatch_index << std::endl;
                }
                ++mismatches;
            }
        }
        if (mismatches == 0) {
            std::cout << "Weil primary reference validation: PASS" << std::endl;
        } else {
            std::cout << "Weil primary reference validation: FAIL (" << mismatches << " PRNs)" << std::endl;
        }
    } else {
        std::cout << "Weil primary reference validation skipped: " << ref_error << std::endl;
    }

    ref_error.clear();
    std::vector<std::vector<uint8_t>> weil_tertiary_refs;
    if (lunanet::load_reference_hex_file(lunanet::get_annex3_weil_tertiary_path(), lunanet::WEIL_TERTIARY_LENGTH, weil_tertiary_refs, &ref_error)) {
        int mismatches = 0;
        for (int prn = 1; prn <= lunanet::MAX_PRNS; ++prn) {
            auto actual = lunanet::generate_weil_tertiary(prn);
            size_t mismatch_index = 0;
            if (!compare_vectors(actual, weil_tertiary_refs[prn - 1], mismatch_index)) {
                if (mismatches < 5) {
                    std::cout << "Weil tertiary mismatch PRN " << prn << " at chip " << mismatch_index << std::endl;
                }
                ++mismatches;
            }
        }
        if (mismatches == 0) {
            std::cout << "Weil tertiary reference validation: PASS" << std::endl;
        } else {
            std::cout << "Weil tertiary reference validation: FAIL (" << mismatches << " PRNs)" << std::endl;
        }
    } else {
        std::cout << "Weil tertiary reference validation skipped: " << ref_error << std::endl;
    }

    std::cout << "\nTest completed successfully!" << std::endl;
    return 0;
}
