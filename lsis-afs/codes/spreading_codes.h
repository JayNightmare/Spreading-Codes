/**
 * LunaNet Gateway 1 - Spreading Code Generator
 * 
 * This module implements the AFS (AltBOC Forward link Spreading) codes
 * for the LunaNet navigation system.
 * 
 * Author: LunaNet Gateway 1 Team
 * Version: 1.0.0
 */

#ifndef LUNANET_SPREADING_CODES_H
#define LUNANET_SPREADING_CODES_H

#include <vector>
#include <cstdint>
#include <string>

namespace lunanet {

// Constants for Gold Code generation (G1 and G2 polynomials)
constexpr int G1_POLY = 0x400;      // x^10 + x^3 + 1
constexpr int G2_POLY = 0x400;      // x^10 + x^3 + 1
constexpr int G1_TAP_FEEDBACK = 0x233;  // Taps for G1: 3, 10
constexpr int G2_TAP_FEEDBACK = 0x21D;  // Taps for G2: 3, 4, 6, 8, 9, 10
constexpr int GOLD_CODE_LENGTH = 2046; // Short cycle length (2,046 chips)
constexpr int MAX_PRNS = 210;

// Weil code parameters
constexpr int LEGENDRE_PRIME = 10223;  // Prime for primary Weil code
constexpr int LEGENDRE_SHORT = 1500;   // Shortened tertiary code length
constexpr int EXPANSION_BLOCK = 0x1A; // 7-bit expansion: 0110100 (0x1A in 7-bit)

// PRN initialization vectors for G2 register (from CSV data)
extern const uint16_t PRN_INIT_G2[];

// G2 delay values in chips (from CSV column G2_Delay_chips)
extern const uint16_t PRN_G2_DELAY[];

// Initialize the C++ engine
void initialize_engine();

// Hello function to verify C++/Python bridge
const char* hello_moon();

// Generate Gold code for a given PRN
// Returns a vector of 2046 chips (0 or 1)
std::vector<int> generate_gold_code(int prn);

// Generate Weil code (primary, length 10223)
std::vector<int> generate_weil_primary();

// Generate shortened Weil code (tertiary, length 1500)
std::vector<int> generate_weil_tertiary();

// Generate the complete AFS-I data channel (Gold codes)
std::vector<int> generate_afs_i(int prn);

// Generate the complete AFS-Q pilot channel (Weil codes with assembly)
std::vector<int> generate_afs_q(int prn, int variant);

// Get the last error message
const char* get_last_error();

// Get library version
const char* get_version();

} // namespace lunanet

#endif // LUNANET_SPREADING_CODES_H