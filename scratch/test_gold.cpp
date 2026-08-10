#include <iostream>
#include <vector>
#include <string>
#include "codes/gateway1/gold_code_generator.h"
#include "codes/gateway1/spreading_config.h"
#include "codes/spreading_codes.h"

int main() {
    lunanet::initialize_engine();
    std::string err;
    lunanet::load_spreading_code_config("config/spreading_codes_config.ini", &err);
    
    auto actual1 = lunanet::generate_gold_code(1);
    auto actual2 = lunanet::generate_gold_code(2);
    
    std::cout << "PRN 1: ";
    for(int i=0; i<10; ++i) std::cout << (int)actual1[i];
    std::cout << "\nPRN 2: ";
    for(int i=0; i<10; ++i) std::cout << (int)actual2[i];
    std::cout << "\n";
    
    return 0;
}
