/**
 * LunaNet Gateway 1 - Spreading Code Generator Implementation
 * 
 * This file implements the AFS (AltBOC Forward link Spreading) codes
 * for the LunaNet navigation system.
 */

#include "spreading_codes.h"
#include <stdexcept>
#include <cstring>
#include <algorithm>

namespace lunanet {

// PRN initialization vectors for G2 register (from CSV data)
// G2_Initialization_HEX values converted to decimal
const uint16_t PRN_INIT_G2[] = {
    // PRN 1-10
    0x514, 0x59E, 0x49A, 0x346, 0x788, 0x341, 0x170, 0x7AB, 0x301, 0x32E,
    // PRN 11-20
    0x4DE, 0x290, 0x4ED, 0x506, 0x30E, 0x230, 0x270, 0x380, 0x2DC, 0x5C8,
    // PRN 21-30
    0x61E, 0x720, 0x548, 0x770, 0x32F, 0x600, 0x635, 0x487, 0x730, 0x4BE,
    // PRN 31-40
    0x55F, 0x775, 0x424, 0x603, 0x450, 0x0AB, 0x316, 0x44F, 0x477, 0x76F,
    // PRN 41-50
    0x1F3, 0x189, 0x6BE, 0x6EB, 0x336, 0x650, 0x590, 0x74F, 0x294, 0x03F,
    // PRN 51-60
    0x5AB, 0x4F2, 0x647, 0x494, 0x521, 0x53D, 0x4DF, 0x4A2, 0x2C3, 0x15F,
    // PRN 61-70
    0x3A2, 0x209, 0x329, 0x0A9, 0x532, 0x733, 0x697, 0x6DD, 0x2FC, 0x471,
    // PRN 71-80
    0x428, 0x706, 0x52F, 0x77F, 0x526, 0x7FB, 0x220, 0x13C, 0x65F, 0x7E3,
    // PRN 81-90
    0x254, 0x444, 0x041, 0x542, 0x6EF, 0x4F6, 0x708, 0x45F, 0x654, 0x659,
    // PRN 91-100
    0x486, 0x72A, 0x191, 0x68D, 0x0CD, 0x5FE, 0x604, 0x282, 0x36D, 0x231,
    // PRN 101-110
    0x655, 0x1FF, 0x6FB, 0x41F, 0x6AC, 0x026, 0x298, 0x612, 0x100, 0x2C7,
    // PRN 111-120
    0x534, 0x7BE, 0x7DA, 0x510, 0x39A, 0x786, 0x354, 0x5A4, 0x1A4, 0x529,
    // PRN 121-130
    0x4CE, 0x5F2, 0x280, 0x30D, 0x621, 0x572, 0x455, 0x4C5, 0x51E, 0x676,
    // PRN 131-140
    0x53C, 0x726, 0x410, 0x544, 0x430, 0x3AC, 0x468, 0x389, 0x7DE, 0x3CD,
    // PRN 141-150
    0x7FD, 0x0BB, 0x600, 0x6F9, 0x61A, 0x7EA, 0x798, 0x75A, 0x307, 0x5D1,
    // PRN 151-160
    0x541, 0x689, 0x71E, 0x330, 0x498, 0x377, 0x664, 0x476, 0x6BB, 0x55E,
    // PRN 161-170
    0x340, 0x440, 0x1DE, 0x650, 0x302, 0x32C, 0x2CD, 0x50A, 0x31A, 0x459,
    // PRN 171-180
    0x750, 0x5D2, 0x4F3, 0x779, 0x538, 0x719, 0x146, 0x1E4, 0x5E3, 0x554,
    // PRN 181-190
    0x584, 0x288, 0x226, 0x594, 0x4D4, 0x48C, 0x020, 0x44E, 0x43A, 0x6F0,
    // PRN 191-200
    0x7C5, 0x331, 0x7BD, 0x443, 0x46A, 0x758, 0x357, 0x22C, 0x385, 0x643,
    // PRN 201-210
    0x648, 0x4D3, 0x282, 0x530, 0x760, 0x6E4, 0x485, 0x679, 0x112, 0x2DE
};

// G2 delay values in chips (from CSV column G2_Delay_chips)
const uint16_t PRN_G2_DELAY[] = {
    // PRN 1-10
    1845, 1071, 170, 2035, 1214, 1292, 1284, 1894, 1537, 735,
    // PRN 11-20
    561, 1789, 1453, 196, 1040, 326, 1787, 982, 1030, 1380,
    // PRN 21-30
    1932, 1188, 390, 714, 303, 1001, 707, 1984, 139, 182,
    // PRN 31-40
    1891, 1247, 1434, 2000, 1843, 865, 616, 514, 449, 1173,
    // PRN 41-50
    24, 1383, 1940, 1594, 1765, 752, 145, 1615, 1666, 1372,
    // PRN 51-60
    1634, 1068, 1181, 879, 1153, 1621, 927, 1848, 402, 413,
    // PRN 61-70
    1090, 657, 609, 1547, 370, 271, 1353, 635, 299, 697,
    // PRN 71-80
    152, 678, 1329, 15, 1974, 1884, 1868, 277, 302, 9,
    // PRN 81-90
    603, 1583, 848, 1234, 1568, 510, 1303, 1921, 823, 1187,
    // PRN 91-100
    1299, 824, 672, 2034, 1388, 13, 223, 1840, 1161, 1132,
    // PRN 101-110
    365, 2, 924, 1373, 959, 220, 1542, 188, 264, 453,
    // PRN 111-120
    68, 715, 75, 1095, 938, 1316, 394, 1156, 166, 969,
    // PRN 121-130
    269, 179, 957, 400, 625, 1513, 1796, 100, 1660, 1454,
    // PRN 131-140
    1613, 1064, 844, 518, 320, 661, 2031, 694, 1143, 1167,
    // PRN 141-150
    1885, 833, 1601, 903, 399, 1896, 899, 133, 556, 331,
    // PRN 151-160
    198, 212, 1024, 1070, 1972, 1573, 884, 1177, 1691, 533,
    // PRN 161-170
    480, 751, 447, 734, 973, 857, 1767, 1548, 1876, 614,
    // PRN 171-180
    1017, 1978, 275, 1141, 1252, 1952, 1714, 1067, 557, 522,
    // PRN 181-190
    1159, 545, 1580, 610, 935, 1134, 780, 691, 1038, 1418,
    // PRN 191-200
    295, 916, 1654, 624, 706, 1033, 1633, 790, 1451, 1300,
    // PRN 201-210
    459, 106, 861, 1541, 114, 1381, 1945, 1069, 242, 356
};

// Static variables for error handling
static char last_error[256] = "";
static const char* version = "1.0.0";

void initialize_engine() {
    // Initialize any required resources
    std::memset(last_error, 0, sizeof(last_error));
}

const char* hello_moon() {
    return "Hello from LunaNet Gateway 1 - Spreading Code Engine!";
}

const char* get_last_error() {
    return last_error;
}

const char* get_version() {
    return version;
}

// Helper function to set error message
static void set_error(const char* msg) {
    std::strncpy(last_error, msg, sizeof(last_error) - 1);
    last_error[sizeof(last_error) - 1] = '\0';
}

// Linear Feedback Shift Register (LFSR) implementation
class LFSR {
private:
    uint16_t state;
    uint16_t feedback_mask;
    int length;
    
public:
    LFSR(uint16_t initial_state, uint16_t feedback_mask, int reg_length = 10)
        : state(initial_state), feedback_mask(feedback_mask), length(reg_length) {}
    
    // Generate next bit and update state
    int next_bit() {
        // Output is the LSB (least significant bit)
        int output = state & 1;
        
        // Calculate feedback
        int feedback = 0;
        uint16_t masked_state = state & feedback_mask;
        
        // Count parity of masked state
        for (int i = 0; i < length; i++) {
            feedback ^= (masked_state >> i) & 1;
        }
        
        // Shift right and insert feedback at MSB
        state = (state >> 1) | (feedback << (length - 1));
        
        return output;
    }
    
    // Reset to initial state
    void reset(uint16_t initial_state) {
        state = initial_state;
    }
    
    // Get current state
    uint16_t get_state() const {
        return state;
    }
};

// Legendre symbol computation for Weil code generation
static int legendre_symbol(int a, int p) {
    if (a == 0) return 0;
    if (p == 2) return a & 1; // For p=2, return a mod 2
    
    // Euler's criterion: a^((p-1)/2) mod p
    long long result = 1;
    long long base = a % p;
    long long exponent = (p - 1) / 2;
    
    while (exponent > 0) {
        if (exponent & 1) {
            result = (result * base) % p;
        }
        base = (base * base) % p;
        exponent >>= 1;
    }
    
    if (result == p - 1) return -1;
    return result;
}

// Generate Gold code for a given PRN
std::vector<int> generate_gold_code(int prn) {
    if (prn < 1 || prn > MAX_PRNS) {
        set_error("PRN must be between 1 and 210");
        return {};
    }
    
    // G1 uses fixed initialization (all ones)
    LFSR g1(0x3FF, G1_TAP_FEEDBACK); // 10-bit register initialized to 1023 (all ones)
    
    // G2 uses PRN-specific initialization
    uint16_t g2_init = PRN_INIT_G2[prn - 1]; // Adjust for 0-based indexing
    LFSR g2(g2_init, G2_TAP_FEEDBACK);
    
    // Apply G2 delay - pre-shift G2 by the specified number of chips
    uint16_t g2_delay = PRN_G2_DELAY[prn - 1];
    for (uint16_t i = 0; i < g2_delay; i++) {
        g2.next_bit();
    }
    
    std::vector<int> gold_code;
    gold_code.reserve(GOLD_CODE_LENGTH);
    
    // Generate 2046 chips (short cycle, resetting at chip 2046)
    for (int i = 0; i < GOLD_CODE_LENGTH; i++) {
        int g1_bit = g1.next_bit();
        int g2_bit = g2.next_bit();
        
        // Gold code is XOR of G1 and G2
        gold_code.push_back(g1_bit ^ g2_bit);
    }
    
    return gold_code;
}

// Generate Weil code (primary, length 10223)
std::vector<int> generate_weil_primary() {
    std::vector<int> weil_code;
    weil_code.reserve(LEGENDRE_PRIME);
    
    // Generate Legendre sequence for prime 10223
    // Values are +1 for quadratic residue, -1 for non-residue, 0 for 0
    // We convert to binary: 1 -> 1, 0 or -1 -> 0
    for (int n = 0; n < LEGENDRE_PRIME; n++) {
        int symbol = legendre_symbol(n, LEGENDRE_PRIME);
        // Convert Legendre symbol to binary: 1 -> 1, (0 or -1) -> 0
        weil_code.push_back((symbol == 1) ? 1 : 0);
    }
    
    return weil_code;
}

// Generate shortened Weil code (tertiary, length 1500)
std::vector<int> generate_weil_tertiary() {
    // Generate full Weil code first
    std::vector<int> full_weil = generate_weil_primary();
    
    // Take first 1500 chips
    std::vector<int> tertiary_code(full_weil.begin(), 
                                   full_weil.begin() + LEGENDRE_SHORT);
    
    return tertiary_code;
}

// Generate the complete AFS-I data channel (Gold codes)
std::vector<int> generate_afs_i(int prn) {
    return generate_gold_code(prn);
}

// Generate the complete AFS-Q pilot channel (Weil codes with assembly)
std::vector<int> generate_afs_q(int prn, int variant) {
    // Generate primary Weil code (10223 chips)
    std::vector<int> primary = generate_weil_primary();
    
    // Generate tertiary Weil code (1500 chips)
    std::vector<int> tertiary = generate_weil_tertiary();
    
    // Fixed 7-bit expansion block: 0110100
    std::vector<int> expansion = {0, 1, 1, 0, 1, 0, 0};
    
    // Secondary code based on variant (example: S0 = 1110)
    std::vector<int> secondary;
    switch (variant) {
        case 0:  // S0 variant
            secondary = {1, 1, 1, 0};
            break;
        case 1:  // S1 variant
            secondary = {1, 1, 0, 1};
            break;
        case 2:  // S2 variant
            secondary = {1, 0, 1, 1};
            break;
        case 3:  // S3 variant
            secondary = {0, 1, 1, 1};
            break;
        default:
            secondary = {1, 1, 1, 0}; // Default to S0
            break;
    }
    
    // Assemble the final signal:
    // [Primary][Expansion][Secondary][Tertiary] using modulo-2 addition
    // For simplicity in this implementation, we'll concatenate them
    // In a real implementation, this would involve proper interleaving and modulo-2 addition
    
    std::vector<int> afs_q;
    afs_q.reserve(primary.size() + expansion.size() + secondary.size() + tertiary.size());
    
    // Concatenate all components
    afs_q.insert(afs_q.end(), primary.begin(), primary.end());
    afs_q.insert(afs_q.end(), expansion.begin(), expansion.end());
    afs_q.insert(afs_q.end(), secondary.begin(), secondary.end());
    afs_q.insert(afs_q.end(), tertiary.begin(), tertiary.end());
    
    return afs_q;
}

} // namespace lunanet 