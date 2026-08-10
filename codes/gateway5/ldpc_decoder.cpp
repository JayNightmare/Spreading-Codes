#include "gateway5/ldpc_decoder.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace lunanet::gateway5 {
namespace {

struct TannerGraph {
    std::vector<int> edge_var;
    std::vector<int> edge_check;
    std::vector<std::vector<int>> check_edges;
    std::vector<std::vector<int>> var_edges;
};

bool BuildTannerGraph(const lunanet::gateway2::BinaryMatrix& h,
                      TannerGraph* out,
                      std::string* error_message) {
    if (out == nullptr) {
        if (error_message) *error_message = "Null TannerGraph output";
        return false;
    }

    TannerGraph g;
    g.check_edges.assign(static_cast<std::size_t>(h.rows), {});
    g.var_edges.assign(static_cast<std::size_t>(h.cols), {});

    for (int r = 0; r < h.rows; ++r) {
        for (int c = 0; c < h.cols; ++c) {
            if (h.Get(r, c) == 0u) {
                continue;
            }
            const int edge_id = static_cast<int>(g.edge_var.size());
            g.edge_var.push_back(c);
            g.edge_check.push_back(r);
            g.check_edges[static_cast<std::size_t>(r)].push_back(edge_id);
            g.var_edges[static_cast<std::size_t>(c)].push_back(edge_id);
        }
    }

    *out = std::move(g);
    return true;
}

int ComputeSyndromeWeight(const lunanet::gateway2::BinaryMatrix& h,
                          const std::vector<uint8_t>& bits) {
    int non_zero = 0;
    for (int r = 0; r < h.rows; ++r) {
        uint8_t parity = 0;
        const int row_offset = r * h.cols;
        for (int c = 0; c < h.cols; ++c) {
            parity ^= static_cast<uint8_t>(h.data[static_cast<std::size_t>(row_offset + c)] &
                                           bits[static_cast<std::size_t>(c)]);
        }
        if (parity != 0u) {
            ++non_zero;
        }
    }
    return non_zero;
}

}  // namespace

bool BuildParityCheckMatrix(const lunanet::gateway2::LdpcMatrices& matrices,
                            const lunanet::gateway2::BinaryMatrix& b_matrix,
                            lunanet::gateway2::BinaryMatrix* out_h,
                            std::string* error_message) {
    if (out_h == nullptr) {
        if (error_message) *error_message = "Null output parity-check matrix";
        return false;
    }

    const int m1 = matrices.a.rows;
    const int k = matrices.a.cols;
    const int m2 = matrices.c.rows;

    if (b_matrix.rows != m1 || b_matrix.cols != m1) {
        if (error_message) *error_message = "B matrix dimension mismatch";
        return false;
    }
    if (matrices.c.cols != k || matrices.d.rows != m2 || matrices.d.cols != m1) {
        if (error_message) *error_message = "A/C/D matrix dimensions are inconsistent";
        return false;
    }

    const int rows = m1 + m2;
    const int cols = k + m1 + m2;

    lunanet::gateway2::BinaryMatrix h;
    h.rows = rows;
    h.cols = cols;
    h.data.assign(static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols), 0u);

    // Band 1: [A B 0]
    for (int r = 0; r < m1; ++r) {
        for (int c = 0; c < k; ++c) {
            h.Set(r, c, matrices.a.Get(r, c));
        }
        for (int c = 0; c < m1; ++c) {
            h.Set(r, k + c, b_matrix.Get(r, c));
        }
    }

    // Band 2: [C D I]
    for (int r = 0; r < m2; ++r) {
        const int hr = m1 + r;
        for (int c = 0; c < k; ++c) {
            h.Set(hr, c, matrices.c.Get(r, c));
        }
        for (int c = 0; c < m1; ++c) {
            h.Set(hr, k + c, matrices.d.Get(r, c));
        }
        h.Set(hr, k + m1 + r, 1u);
    }

    *out_h = std::move(h);
    return true;
}

std::vector<double> RestorePuncturedCodewordLlrs(
    const std::vector<double>& received_llrs,
    const lunanet::gateway2::LdpcParams& params,
    int p1_bits,
    int p2_bits,
    std::string* error_message) {
    if (params.info_bits < 0 || params.data_bits < 0 || params.filler_bits < 0 ||
        params.puncture_z2 < 0 || p1_bits < 0 || p2_bits < 0) {
        if (error_message) *error_message = "Negative restore dimensions";
        return {};
    }
    if (params.data_bits + params.filler_bits != params.info_bits) {
        if (error_message) *error_message = "Invalid params: data_bits + filler_bits != info_bits";
        return {};
    }
    if (params.puncture_z2 > params.data_bits) {
        if (error_message) *error_message = "Invalid params: puncture_z2 > data_bits";
        return {};
    }

    const int transmitted_sys = params.data_bits - params.puncture_z2;
    const int expected_len = transmitted_sys + (params.output_symbols - transmitted_sys);
    if (static_cast<int>(received_llrs.size()) != expected_len) {
        if (error_message) {
            *error_message = "Received LLR length mismatch: got " +
                std::to_string(received_llrs.size()) + ", expected " +
                std::to_string(expected_len);
        }
        return {};
    }

    const int n = params.info_bits + p1_bits + p2_bits;
    std::vector<double> llr(static_cast<std::size_t>(n), 0.0);

    int rx = 0;

    // Restore transmitted systematic bits (after punctured prefix).
    for (int i = params.puncture_z2; i < params.data_bits; ++i) {
        llr[static_cast<std::size_t>(i)] = received_llrs[static_cast<std::size_t>(rx++)];
    }

    // Filler bits (SB3/SB4) are not transmitted and remain 0.0 erasures.
    // Punctured systematic prefix also remains 0.0 erasures.

    const int parity_received = static_cast<int>(received_llrs.size()) - rx;
    int parity_used = 0;

    // Restore parity in encoder order: p1 then p2; non-transmitted tail stays 0.0.
    for (int i = 0; i < p1_bits && parity_used < parity_received; ++i) {
        llr[static_cast<std::size_t>(params.info_bits + i)] =
            received_llrs[static_cast<std::size_t>(rx++)];
        ++parity_used;
    }
    for (int i = 0; i < p2_bits && parity_used < parity_received; ++i) {
        llr[static_cast<std::size_t>(params.info_bits + p1_bits + i)] =
            received_llrs[static_cast<std::size_t>(rx++)];
        ++parity_used;
    }

    return llr;
}

LdpcDecodeResult DecodeLdpcMinSum(
    const std::vector<double>& received_llrs,
    const lunanet::gateway2::LdpcMatrices& matrices,
    const lunanet::gateway2::BinaryMatrix& b_matrix,
    const lunanet::gateway2::LdpcParams& params,
    int max_iterations,
    double alpha,
    std::string* error_message) {
    LdpcDecodeResult result;

    if (max_iterations <= 0) {
        if (error_message) *error_message = "max_iterations must be > 0";
        return result;
    }
    if (!(alpha > 0.0 && alpha <= 1.0)) {
        if (error_message) *error_message = "alpha must be in (0, 1]";
        return result;
    }

    lunanet::gateway2::BinaryMatrix h;
    if (!BuildParityCheckMatrix(matrices, b_matrix, &h, error_message)) {
        return result;
    }

    const int p1_bits = matrices.a.rows;
    const int p2_bits = matrices.c.rows;

    const auto channel_llr =
        RestorePuncturedCodewordLlrs(received_llrs, params, p1_bits, p2_bits, error_message);
    if (channel_llr.empty()) {
        return result;
    }

    TannerGraph graph;
    if (!BuildTannerGraph(h, &graph, error_message)) {
        return result;
    }

    const int n = h.cols;
    const int edges = static_cast<int>(graph.edge_var.size());

    std::vector<double> q(static_cast<std::size_t>(edges), 0.0);  // v->c
    std::vector<double> r(static_cast<std::size_t>(edges), 0.0);  // c->v
    std::vector<double> posterior(static_cast<std::size_t>(n), 0.0);
    std::vector<uint8_t> hard(static_cast<std::size_t>(n), 0u);

    // Initialization: variable-to-check messages start with channel LLR.
    for (int e = 0; e < edges; ++e) {
        q[static_cast<std::size_t>(e)] = channel_llr[static_cast<std::size_t>(graph.edge_var[e])];
    }

    int final_syndrome = -1;

    for (int iter = 1; iter <= max_iterations; ++iter) {
        // Check node update (normalized min-sum).
        for (int check = 0; check < h.rows; ++check) {
            const auto& ce = graph.check_edges[static_cast<std::size_t>(check)];
            if (ce.empty()) {
                continue;
            }

            int total_sign = 1;
            double min1 = std::numeric_limits<double>::infinity();
            double min2 = std::numeric_limits<double>::infinity();
            int min_edge = -1;

            for (const int edge_id : ce) {
                const double msg = q[static_cast<std::size_t>(edge_id)];
                const double a = std::abs(msg);
                const int s = (msg < 0.0) ? -1 : 1;
                total_sign *= s;

                if (a < min1) {
                    min2 = min1;
                    min1 = a;
                    min_edge = edge_id;
                } else if (a < min2) {
                    min2 = a;
                }
            }

            for (const int edge_id : ce) {
                const double msg = q[static_cast<std::size_t>(edge_id)];
                const int self_sign = (msg < 0.0) ? -1 : 1;
                const int out_sign = total_sign * self_sign;
                const double use_min = (edge_id == min_edge) ? min2 : min1;
                const double mag = alpha * use_min;
                r[static_cast<std::size_t>(edge_id)] = (out_sign < 0) ? -mag : mag;
            }
        }

        // Variable node update with strict extrinsic exclusion.
        for (int var = 0; var < n; ++var) {
            const auto& ve = graph.var_edges[static_cast<std::size_t>(var)];

            double sum_r = 0.0;
            for (const int edge_id : ve) {
                sum_r += r[static_cast<std::size_t>(edge_id)];
            }

            const double post = channel_llr[static_cast<std::size_t>(var)] + sum_r;
            posterior[static_cast<std::size_t>(var)] = post;
            hard[static_cast<std::size_t>(var)] = (post < 0.0) ? 1u : 0u;

            for (const int edge_id : ve) {
                // Exclude destination check's own message (extrinsic update).
                q[static_cast<std::size_t>(edge_id)] = post - r[static_cast<std::size_t>(edge_id)];
            }
        }

        final_syndrome = ComputeSyndromeWeight(h, hard);
        if (final_syndrome == 0) {
            result.converged = true;
            result.iterations = iter;
            break;
        }

        result.iterations = iter;
    }

    result.syndrome_weight = final_syndrome;

    // Return de-fillered data bits for SB3/SB4 and plain data bits for SB2.
    const int data_bits = params.data_bits;
    result.decoded_data_bits.assign(hard.begin(), hard.begin() + data_bits);

    return result;
}

}  // namespace lunanet::gateway5
