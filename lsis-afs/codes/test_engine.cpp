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
    std::vector<int> gold_code = lunanet::generate_gold_code(1);
    if (!gold_code.empty()) {
        std::cout << "Generated Gold code for PRN 1, length: " << gold_code.size() << " chips" << std::endl;
        std::cout << "First 20 chips: ";
        for (int i = 0; i < 20 && i < gold_code.size(); i++) {
            std::cout << gold_code[i] << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "Error generating Gold code: " << lunanet::get_last_error() << std::endl;
    }
    
    // Test Weil code generation
    std::vector<int> weil_primary = lunanet::generate_weil_primary();
    if (!weil_primary.empty()) {
        std::cout << "Generated Weil primary code, length: " << weil_primary.size() << " chips" << std::endl;
    } else {
        std::cout << "Error generating Weil primary code: " << lunanet::get_last_error() << std::endl;
    }
    
    // Test AFS-I generation
    std::vector<int> afs_i = lunanet::generate_afs_i(1);
    if (!afs_i.empty()) {
        std::cout << "Generated AFS-I code for PRN 1, length: " << afs_i.size() << " chips" << std::endl;
    } else {
        std::cout << "Error generating AFS-I code: " << lunanet::get_last_error() << std::endl;
    }
    
    // Test AFS-Q generation
    std::vector<int> afs_q = lunanet::generate_afs_q(1, 0); // PRN 1, variant S0
    if (!afs_q.empty()) {
        std::cout << "Generated AFS-Q code for PRN 1 variant S0, length: " << afs_q.size() << " chips" << std::endl;
    } else {
        std::cout << "Error generating AFS-Q code: " << lunanet::get_last_error() << std::endl;
    }
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}