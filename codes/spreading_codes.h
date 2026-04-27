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
#include <map>
#include <array>

namespace lunanet {

// Constants for Gold Code generation (11-bit LFSRs per LSIS-AFS Appendix C)
// Generator polynomials: g1(x) = x^11 + x^2 + 1, g2(x) = x^11 + x^8 + x^5 + x^2 + 1
constexpr int LFSR_LENGTH = 11;         // 11-bit shift registers
constexpr uint16_t G1_INIT = 0x7FF;    // All 1s for 11-bit: 2047 decimal
constexpr int G1_TAP_FEEDBACK = 0x204;  // Taps for G1: bits 2, 9 (0-indexed in 11-bit)
constexpr int G2_TAP_FEEDBACK = 0x3A6;  // Taps for G2: bits 1, 2, 5, 7, 8, 9
constexpr int GOLD_CODE_LENGTH = 2046;  // 2^11 - 1 shortened cycle length
constexpr int MAX_PRNS = 210;

// Weil code parameters (indices for PRN-specific generation)
// Primary: W(t;k) = L(t) XOR L((t+k) mod 10223)
constexpr int WEIL_PRIMARY_PRIME = 10223;
// Tertiary: W(t;k) = L(t) XOR L((t+k) mod 1499)
constexpr int WEIL_TERTIARY_PRIME = 1499;
constexpr int WEIL_TERTIARY_LENGTH = 1500;  // 1499 + 1 appended zero

// Weil code parameters per Appendix D/E
// Expansion block is fixed: [0, 1, 1, 0, 1, 0, 0] (7 chips, inserted at PRN-specific index)
constexpr int EXPANSION_LENGTH = 7;

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
std::vector<uint8_t> generate_gold_code(int prn);

// Generate Legendre sequence (used by Weil code construction)
std::vector<uint8_t> generate_legendre_sequence(int prime);

// Generate PRN-specific Weil primary code (Appendix D)
// W(t;k) = L(t) XOR L((t+k) mod 10223) with expansion inserted at index p
std::vector<uint8_t> generate_weil_primary(int prn);

// Generate PRN-specific Weil tertiary code (Appendix E)
// W(t;k) = L(t) XOR L((t+k) mod 1499) with 0 appended at end
std::vector<uint8_t> generate_weil_tertiary(int prn);

// Generate the complete AFS-I data channel (Gold codes)
std::vector<uint8_t> generate_afs_i(int prn);

// Generate the complete AFS-Q pilot channel (Weil codes with assembly)
std::vector<uint8_t> generate_afs_q(int prn, int variant);

// Generate spreading codes for all 210 PRNs (Phase 1B batch generator)
// Returns map: PRN -> (AFS-I vector, AFS-Q vector pair) with uint8_t storage
std::map<int, std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> generate_all_spreading_codes();

// Get the last error message
const char* get_last_error();

// Get library version
const char* get_version();

} // namespace lunanet

#endif // LUNANET_SPREADING_CODES_H