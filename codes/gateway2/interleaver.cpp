#include "interleaver.h"

namespace lunanet::gateway2 {

std::vector<uint8_t> Interleave(const std::vector<uint8_t>& input) {
    if (input.size() != kInterleaverSize) {
        return {};
    }

    std::vector<uint8_t> output(kInterleaverSize);

    // Write row-wise (input is already row-wise), read column-wise
    for (int col = 0; col < kInterleaverCols; ++col) {
        for (int row = 0; row < kInterleaverRows; ++row) {
            output[col * kInterleaverRows + row] = input[row * kInterleaverCols + col];
        }
    }

    return output;
}

std::vector<uint8_t> Deinterleave(const std::vector<uint8_t>& input) {
    if (input.size() != kInterleaverSize) {
        return {};
    }

    std::vector<uint8_t> output(kInterleaverSize);

    // Reverse: write column-wise, read row-wise
    for (int row = 0; row < kInterleaverRows; ++row) {
        for (int col = 0; col < kInterleaverCols; ++col) {
            output[row * kInterleaverCols + col] = input[col * kInterleaverRows + row];
        }
    }

    return output;
}

}  // namespace lunanet::gateway2
