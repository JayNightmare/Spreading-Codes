#include "gateway3/sync_pattern.h"
#include "gateway5/frame_synchronizer.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

int main() {
    const std::vector<uint8_t> bits =
        lunanet::gateway3::BuildSyncPattern();

    const std::vector<float> symbols =
        lunanet::gateway5::BuildSyncReferenceSymbols();

    // 1. Check the length of the sync pattern and symbols(is it 68bits?)
    if (bits.size() != 68) {
        std::cerr << "FAIL: bit length = "
                  << bits.size()
                  << ", expected 68\n";
        return 1;
    }

    if (symbols.size() != 68) {
        std::cerr << "FAIL: symbol length = "
                  << symbols.size()
                  << ", expected 68\n";
        return 1;
    }

    // 2. Check all 68 bits and symbols against the expected values
    const std::array<uint8_t, 68> expected_bits = {
        1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0,
        0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
        0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1,
        0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0,
        1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0, 1, 0
    };

    // 3. Check all 68 symbols against the expected values after converted to BPSK mapping
    const std::array<float, 68> expected_symbols = {
        -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f,
        +1.0f, +1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, -1.0f,
        +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f,
        +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f,
        -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f,
        +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f
    };

    for (std::size_t i = 0; i < expected_bits.size(); ++i) {
        if (bits[i] != expected_bits[i]) {
            std::cerr << "FAIL: bit mismatch at index "
                      << i << '\n';
            return 1;
        }

        if (symbols[i] != expected_symbols[i]) {
            std::cerr << "FAIL: symbol mismatch at index "
                      << i << '\n';
            return 1;
        }
    }

    std::cout << "PASS: sync pattern has 68 bits\n";
    std::cout << "PASS: pattern is MSB-first\n";
    std::cout << "PASS: BPSK mapping is correct\n";
    std::cout << "PASS: all 68 symbols match the specification\n";

    return 0;
}