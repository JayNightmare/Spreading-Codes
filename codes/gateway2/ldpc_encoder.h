#ifndef LUNANET_GATEWAY2_LDPC_ENCODER_H
#define LUNANET_GATEWAY2_LDPC_ENCODER_H

#include <cstdint>
#include <string>
#include <vector>

namespace lunanet::gateway2 {

/**
 * Dense binary matrix stored row-major as a flat vector of uint8_t (0 or 1).
 */
struct BinaryMatrix {
    int rows = 0;
    int cols = 0;
    std::vector<uint8_t> data;  // rows * cols elements, row-major

    uint8_t Get(int r, int c) const { return data[r * cols + c]; }
    void Set(int r, int c, uint8_t v) { data[r * cols + c] = v; }
};

/**
 * LDPC submatrix set for one subframe type (SB2 or SB3/SB4).
 */
struct LdpcMatrices {
    BinaryMatrix a;      // A submatrix
    BinaryMatrix b_inv;  // B^-1 submatrix
    BinaryMatrix c;      // C submatrix
    BinaryMatrix d;      // D submatrix
};

/**
 * LDPC encoding parameters for a specific subframe.
 */
struct LdpcParams {
    int info_bits;       // Total info bits (including filler): 1200 for SB2, 880 for SB3/SB4
    int filler_bits;     // Filler bits to append: 0 for SB2, 10 for SB3/SB4
    int data_bits;       // Actual data bits: 1200 for SB2, 870 for SB3/SB4
    int puncture_z2;     // 2*Z systematic bits to puncture: 240 for SB2, 176 for SB3/SB4
    int output_symbols;  // Broadcast symbols: 2400 for SB2, 1740 for SB3/SB4
};

constexpr LdpcParams kLdpcSb2 = {1200, 0, 1200, 240, 2400};
constexpr LdpcParams kLdpcSb34 = {880, 10, 870, 176, 1740};

/**
 * Loads a dense binary matrix from a CSV file (comma-separated 0/1 values).
 *
 * @param path           Path to the CSV file.
 * @param out            Output matrix.
 * @param error_message  Optional error message on failure.
 * @return true on success.
 */
bool LoadBinaryMatrixCsv(const std::string& path,
                         BinaryMatrix* out,
                         std::string* error_message = nullptr);

/**
 * Loads the full set of LDPC submatrices for a subframe type.
 *
 * @param a_path      Path to A matrix CSV.
 * @param b_inv_path  Path to B^-1 matrix CSV.
 * @param c_path      Path to C matrix CSV.
 * @param d_path      Path to D matrix CSV.
 * @param out         Output matrix set.
 * @param error_message  Optional error message.
 * @return true on success.
 */
bool LoadLdpcMatrices(const std::string& a_path,
                      const std::string& b_inv_path,
                      const std::string& c_path,
                      const std::string& d_path,
                      LdpcMatrices* out,
                      std::string* error_message = nullptr);

/**
 * Encodes a data vector using LDPC rate 1/2.
 *
 * Algorithm per LSIS §2.4.3.1.2:
 *   p1 = B^-1 · A · s^T   (mod 2)
 *   p2 = C · s^T + D · p1  (mod 2)
 *   Broadcast = (s* ; p*) with puncturing applied.
 *
 * @param data_bits    Input data bits (without filler). Length must match params.data_bits.
 * @param matrices     Pre-loaded LDPC submatrices.
 * @param params       Encoding parameters for the subframe type.
 * @param error_message  Optional error message.
 * @return Encoded symbols for broadcast (params.output_symbols), or empty on error.
 */
std::vector<uint8_t> LdpcEncode(const std::vector<uint8_t>& data_bits,
                                const LdpcMatrices& matrices,
                                const LdpcParams& params,
                                std::string* error_message = nullptr);

}  // namespace lunanet::gateway2

#endif  // LUNANET_GATEWAY2_LDPC_ENCODER_H
