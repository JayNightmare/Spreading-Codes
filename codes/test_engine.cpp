/**
 * Test program for LunaNet Gateway 1 Spreading Code Engine
 */

#include <iostream>
#include <vector>
#include "spreading_codes.h"

int main() {
    std::cout << "Testing LunaNet Gateway 1 Spreading Code Engine" << std::endl;
    
    // Initialize engine
    lunanet::initialize_engine();
    
    // Test hello function
    std::cout << lunanet::hello_moon() << std::endl;
    
    // Test version
    std::cout << "Engine Version: " << lunanet::get_version() << std::endl;
    
    // Test error handling
    std::cout << "Last Error: '" << lunanet::get_last_error() << "'" << std::endl;
    
    // Generate a Gold code for PRN 1
    std::vector<uint8_t> gold_code = lunanet::generate_gold_code(1);
    if (!gold_code.empty()) {
        std::cout << "Generated Gold code for PRN 1, length: " << gold_code.size() << " chips" << std::endl;
        std::cout << "First 20 chips: ";
        for (size_t i = 0; i < 20 && i < gold_code.size(); i++) {
            std::cout << (int)gold_code[i] << " ";
        }
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
    std::vector<uint8_t> afs_q = lunanet::generate_afs_q(1, 0); // PRN 1, variant S0
    if (!afs_q.empty()) {
        std::cout << "Generated AFS-Q code for PRN 1 variant S0, length: " << afs_q.size() << " chips" << std::endl;
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

    std::cout << "\nTest completed successfully!" << std::endl;
    return 0;
}