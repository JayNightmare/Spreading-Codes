#include "ldpc_encoder.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <sstream>

namespace lunanet::gateway2 {

namespace {

std::string Trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

/**
 * GF(2) matrix-vector multiply: result = M · v (mod 2).
 * M is (rows x cols), v is (cols x 1).
 *
 * Precondition: v.size() == m.cols and m.data.size() == m.rows * m.cols.
 * Callers must validate these dimensions before invoking (see LdpcEncode).
 */
std::vector<uint8_t> GF2MatVecMul(const BinaryMatrix& m,
                                   const std::vector<uint8_t>& v) {
    assert(static_cast<int>(v.size()) == m.cols &&
           "GF2MatVecMul: vector length must equal matrix column count");
    assert(m.data.size() == static_cast<size_t>(m.rows) * m.cols &&
           "GF2MatVecMul: matrix data size must equal rows * cols");
    std::vector<uint8_t> result(m.rows, 0);
    for (int r = 0; r < m.rows; ++r) {
        uint8_t acc = 0;
        const int row_offset = r * m.cols;
        for (int c = 0; c < m.cols; ++c) {
            acc ^= (m.data[row_offset + c] & v[c]);
        }
        result[r] = acc;
    }
    return result;
}

/**
 * GF(2) vector XOR: result = a ^ b (element-wise mod 2).
 *
 * Precondition: a.size() == b.size(). Callers must validate equal lengths
 * before invoking (see LdpcEncode).
 */
std::vector<uint8_t> GF2VecXor(const std::vector<uint8_t>& a,
                                const std::vector<uint8_t>& b) {
    assert(a.size() == b.size() &&
           "GF2VecXor: operand vectors must have equal length");
    std::vector<uint8_t> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] ^ b[i];
    }
    return result;
}

}  // namespace

bool LoadBinaryMatrixCsv(const std::string& path,
                         BinaryMatrix* out,
                         std::string* error_message) {
    if (out == nullptr) {
        if (error_message) *error_message = "Null output pointer";
        return false;
    }

    std::ifstream file(path);
    if (!file) {
        if (error_message) *error_message = "Failed to open: " + path;
        return false;
    }

    // Sanity bound on total element count, guarding against runaway allocation
    // from a corrupt or malformed CSV. Far larger than any real LDPC submatrix.
    constexpr size_t kMaxMatrixElements = 64u * 1024u * 1024u;  // 64M cells.

    std::vector<uint8_t> data;  // Flat row-major buffer, filled in a single pass.
    std::string line;
    int rows = 0;
    int expected_cols = -1;
    size_t line_number = 0;

    while (std::getline(file, line)) {
        ++line_number;
        const std::string trimmed = Trim(line);
        if (trimmed.empty()) continue;  // Tolerate blank separator lines.

        std::istringstream ss(trimmed);
        std::string token;
        int row_cols = 0;

        while (std::getline(ss, token, ',')) {
            const std::string t = Trim(token);
            if (t.size() != 1 || (t[0] != '0' && t[0] != '1')) {
                if (error_message) {
                    *error_message = "Invalid cell \"" + t + "\" on line " +
                        std::to_string(line_number) + " in " + path +
                        " (expected a single binary digit, 0 or 1)";
                }
                return false;
            }

            if (data.size() >= kMaxMatrixElements) {
                if (error_message) {
                    *error_message = "Matrix in " + path + " exceeds the maximum "
                        "supported size of " + std::to_string(kMaxMatrixElements) +
                        " cells";
                }
                return false;
            }
            data.push_back(static_cast<uint8_t>(t[0] - '0'));
            ++row_cols;
        }

        if (row_cols == 0) {
            if (error_message) {
                *error_message = "Malformed row with no valid cells on line " +
                    std::to_string(line_number) + " in " + path;
            }
            return false;
        }

        if (expected_cols < 0) {
            expected_cols = row_cols;
        } else if (row_cols != expected_cols) {
            if (error_message) {
                *error_message = "Row " + std::to_string(rows) +
                    " has " + std::to_string(row_cols) + " cols, expected " +
                    std::to_string(expected_cols) + " in " + path;
            }
            return false;
        }

        ++rows;
    }

    if (rows == 0) {
        if (error_message) *error_message = "Empty matrix file: " + path;
        return false;
    }

    out->rows = rows;
    out->cols = expected_cols;
    out->data = std::move(data);

    return true;
}

bool LoadLdpcMatrices(const std::string& a_path,
                      const std::string& b_inv_path,
                      const std::string& c_path,
                      const std::string& d_path,
                      LdpcMatrices* out,
                      std::string* error_message) {
    if (out == nullptr) {
        if (error_message) *error_message = "Null output pointer";
        return false;
    }

    if (!LoadBinaryMatrixCsv(a_path, &out->a, error_message)) return false;
    if (!LoadBinaryMatrixCsv(b_inv_path, &out->b_inv, error_message)) return false;
    if (!LoadBinaryMatrixCsv(c_path, &out->c, error_message)) return false;
    if (!LoadBinaryMatrixCsv(d_path, &out->d, error_message)) return false;

    return true;
}

std::vector<uint8_t> LdpcEncode(const std::vector<uint8_t>& data_bits,
                                const LdpcMatrices& matrices,
                                const LdpcParams& params,
                                std::string* error_message) {
    // Validate parameter self-consistency before allocating anything. This guards
    // against negative or mismatched fields that would otherwise drive oversized
    // allocations (e.g. excessive filler) or out-of-bounds indexing downstream.
    if (params.data_bits < 0 || params.filler_bits < 0 || params.info_bits < 0 ||
        params.puncture_z2 < 0 || params.output_symbols < 0) {
        if (error_message) *error_message = "LDPC params contain negative fields";
        return {};
    }
    if (params.data_bits + params.filler_bits != params.info_bits) {
        if (error_message) {
            *error_message = "LDPC params inconsistent: data_bits + filler_bits != info_bits";
        }
        return {};
    }
    if (params.puncture_z2 > params.data_bits) {
        if (error_message) {
            *error_message = "LDPC params inconsistent: puncture_z2 exceeds systematic length";
        }
        return {};
    }

    // Validate input length
    if (static_cast<int>(data_bits.size()) != params.data_bits) {
        if (error_message) {
            *error_message = "Input length " + std::to_string(data_bits.size()) +
                ", expected " + std::to_string(params.data_bits);
        }
        return {};
    }

    // Build the systematic vector s (info_bits long, with filler appended)
    std::vector<uint8_t> s = data_bits;
    for (int i = 0; i < params.filler_bits; ++i) {
        s.push_back(0);  // Filler bits are zeros
    }

    if (static_cast<int>(s.size()) != params.info_bits) {
        if (error_message) {
            *error_message = "Systematic vector size mismatch after filler";
        }
        return {};
    }

    // Validate matrix dimensions. The c.rows == d.rows constraint guarantees that
    // the GF2VecXor(cs, dp1) below operates on equal-length operands.
    if (matrices.a.cols != params.info_bits ||
        matrices.b_inv.rows != matrices.a.rows ||
        matrices.b_inv.cols != matrices.b_inv.rows ||
        matrices.c.cols != params.info_bits ||
        matrices.c.rows != matrices.d.rows ||
        matrices.d.cols != matrices.b_inv.rows) {
        if (error_message) {
            *error_message = "Matrix dimension mismatch for given params";
        }
        return {};
    }

    // p1 = B^-1 · A · s^T  (mod 2)
    const auto as = GF2MatVecMul(matrices.a, s);
    const auto p1 = GF2MatVecMul(matrices.b_inv, as);

    // p2 = C · s^T + D · p1  (mod 2)
    const auto cs = GF2MatVecMul(matrices.c, s);
    const auto dp1 = GF2MatVecMul(matrices.d, p1);
    const auto p2 = GF2VecXor(cs, dp1);

    // Full codeword: (s ; p1 ; p2)
    // Now apply puncturing per LSIS §2.4.3.1.2:
    //   s* = systematic with first puncture_z2 bits removed and filler removed
    //   p* = (p1 ; p2) trimmed to reach output_symbols total

    std::vector<uint8_t> output;
    output.reserve(params.output_symbols);

    // Append s* (systematic portion with first 2Z bits punctured and filler removed)
    const int sys_start = params.puncture_z2;
    const int sys_end = params.info_bits - params.filler_bits;
    for (int i = sys_start; i < sys_end; ++i) {
        output.push_back(s[i]);
    }

    // How many parity symbols do we need to reach output_symbols?
    const int sys_broadcast = sys_end - sys_start;
    const int parity_needed = params.output_symbols - sys_broadcast;

    // Append parity from p1 first, then p2
    int parity_added = 0;
    for (size_t i = 0; i < p1.size() && parity_added < parity_needed; ++i) {
        output.push_back(p1[i]);
        ++parity_added;
    }
    for (size_t i = 0; i < p2.size() && parity_added < parity_needed; ++i) {
        output.push_back(p2[i]);
        ++parity_added;
    }

    if (static_cast<int>(output.size()) != params.output_symbols) {
        if (error_message) {
            *error_message = "Output size " + std::to_string(output.size()) +
                ", expected " + std::to_string(params.output_symbols);
        }
        return {};
    }

    return output;
}

}  // namespace lunanet::gateway2
