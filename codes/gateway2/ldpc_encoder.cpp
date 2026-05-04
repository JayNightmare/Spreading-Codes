#include "ldpc_encoder.h"

#include <algorithm>
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
 */
std::vector<uint8_t> GF2MatVecMul(const BinaryMatrix& m,
                                   const std::vector<uint8_t>& v) {
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
 */
std::vector<uint8_t> GF2VecXor(const std::vector<uint8_t>& a,
                                const std::vector<uint8_t>& b) {
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

    std::vector<std::vector<uint8_t>> rows_data;
    std::string line;
    int expected_cols = -1;

    while (std::getline(file, line)) {
        const std::string trimmed = Trim(line);
        if (trimmed.empty()) continue;

        std::vector<uint8_t> row;
        std::istringstream ss(trimmed);
        std::string token;

        while (std::getline(ss, token, ',')) {
            const std::string t = Trim(token);
            if (t.empty()) continue;
            row.push_back(static_cast<uint8_t>(t[0] - '0'));
        }

        if (expected_cols < 0) {
            expected_cols = static_cast<int>(row.size());
        } else if (static_cast<int>(row.size()) != expected_cols) {
            if (error_message) {
                *error_message = "Row " + std::to_string(rows_data.size()) +
                    " has " + std::to_string(row.size()) + " cols, expected " +
                    std::to_string(expected_cols) + " in " + path;
            }
            return false;
        }

        rows_data.push_back(std::move(row));
    }

    if (rows_data.empty()) {
        if (error_message) *error_message = "Empty matrix file: " + path;
        return false;
    }

    out->rows = static_cast<int>(rows_data.size());
    out->cols = expected_cols;
    out->data.clear();
    out->data.reserve(static_cast<size_t>(out->rows) * out->cols);

    for (const auto& row : rows_data) {
        out->data.insert(out->data.end(), row.begin(), row.end());
    }

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

    // Validate matrix dimensions
    if (matrices.a.cols != params.info_bits ||
        matrices.b_inv.rows != matrices.a.rows ||
        matrices.b_inv.cols != matrices.b_inv.rows ||
        matrices.c.cols != params.info_bits ||
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
