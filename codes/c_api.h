#ifndef LUNANET_C_API_H
#define LUNANET_C_API_H

#include <cstddef>
#include <cstdint>

#ifdef _WIN32
    #ifdef LUNANET_BUILDING_DLL
        #define LUNANET_API __declspec(dllexport)
    #else
        #define LUNANET_API __declspec(dllimport)
    #endif
#else
    #define LUNANET_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the engine. Must be called before any other function.
 */
LUNANET_API void lunanet_init(void);

/**
 * Load spreading code configuration from an INI file.
 * @return 0 on success, non-zero on error.
 */
LUNANET_API int lunanet_load_config(const char* config_path);

/**
 * Get the last error message. Returns empty string if no error.
 */
LUNANET_API const char* lunanet_last_error(void);

/**
 * Generate a Gold code for the given PRN.
 * @param prn       PRN number (1-210).
 * @param out_buf   Output buffer (caller-allocated, at least out_capacity bytes).
 * @param out_capacity  Capacity of out_buf.
 * @return Number of chips written, or 0 on error.
 */
LUNANET_API int lunanet_generate_gold(int prn, uint8_t* out_buf, int out_capacity);

/**
 * Generate a Weil primary code for the given PRN.
 */
LUNANET_API int lunanet_generate_weil_primary(int prn, uint8_t* out_buf, int out_capacity);

/**
 * Generate a Weil tertiary code for the given PRN.
 */
LUNANET_API int lunanet_generate_weil_tertiary(int prn, uint8_t* out_buf, int out_capacity);

/**
 * Generate an AFS-I code for the given PRN.
 */
LUNANET_API int lunanet_generate_afs_i(int prn, uint8_t* out_buf, int out_capacity);

/**
 * Generate an AFS-Q code for the given PRN.
 * @param max_chips  Maximum chips (0 = full length).
 */
LUNANET_API int lunanet_generate_afs_q(int prn, int max_chips, uint8_t* out_buf, int out_capacity);

/**
 * Get the Gold code length constant.
 */
LUNANET_API int lunanet_gold_code_length(void);

/**
 * Get the Weil primary code length (prime + expansion).
 */
LUNANET_API int lunanet_weil_primary_length(void);

/**
 * Get the Weil tertiary code length.
 */
LUNANET_API int lunanet_weil_tertiary_length(void);

/**
 * BCH(51,8) encode a 9-bit SB1 value.
 * @param sb1_data  9-bit value (FID[2] + TOI[7]).
 * @param out_buf   Output buffer (at least 52 bytes).
 * @return 52 on success, 0 on error.
 */
LUNANET_API int lunanet_bch_encode(int sb1_data, uint8_t* out_buf, int out_capacity);

/**
 * Compute CRC-24Q over a bit vector.
 * @param data     Array of bits (0 or 1).
 * @param length   Number of bits.
 * @return 24-bit CRC value.
 */
LUNANET_API uint32_t lunanet_crc24(const uint8_t* data, int length);

#ifdef __cplusplus
}
#endif

#endif  // LUNANET_C_API_H
