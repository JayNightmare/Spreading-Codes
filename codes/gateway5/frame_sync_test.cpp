#include "gateway3/sync_pattern.h"
#include "gateway5/frame_synchronizer.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {
// Tolerance for float comparisons below. BuildSyncReferenceSymbols maps
// bits straight to literal +/-1.0f with no accumulated arithmetic, so exact
// equality holds today, but a tolerance guards against a future change
// (e.g. a scaled or filtered reference) silently breaking this test.
constexpr float kSymbolTolerance = 1e-6f;

bool FloatEq(float a, float b) {
    return std::fabs(a - b) <= kSymbolTolerance;
}
}  // namespace

int main() {
    using lunanet::gateway3::kSyncPatternSymbols;

    const std::vector<uint8_t> bits =
        lunanet::gateway3::BuildSyncPattern();

    const std::vector<float> symbols =
        lunanet::gateway5::BuildSyncReferenceSymbols();

    // 1. Check the length of the sync pattern and symbols (is it kSyncPatternSymbols bits?)
    if (bits.size() != static_cast<std::size_t>(kSyncPatternSymbols)) {
        std::cerr << "FAIL: bit length = "
                  << bits.size()
                  << ", expected " << kSyncPatternSymbols << "\n";
        return 1;
    }

    if (symbols.size() != static_cast<std::size_t>(kSyncPatternSymbols)) {
        std::cerr << "FAIL: symbol length = "
                  << symbols.size()
                  << ", expected " << kSyncPatternSymbols << "\n";
        return 1;
    }

    // 2. Check all kSyncPatternSymbols bits and symbols against the expected values
    const std::array<uint8_t, kSyncPatternSymbols> expected_bits = {
        1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0,
        0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
        0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1,
        0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0,
        1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 1, 0, 1, 0
    };

    // 3. Check all kSyncPatternSymbols symbols against the expected values after converted to BPSK mapping
    const std::array<float, kSyncPatternSymbols> expected_symbols = {
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

        if (!FloatEq(symbols[i], expected_symbols[i])) {
            std::cerr << "FAIL: symbol mismatch at index "
                      << i << '\n';
            return 1;
        }
    }

    std::cout << "PASS: sync pattern has " << kSyncPatternSymbols << " bits\n";
    std::cout << "PASS: pattern is MSB-first\n";
    std::cout << "PASS: BPSK mapping is correct\n";
    std::cout << "PASS: all " << kSyncPatternSymbols << " symbols match the specification\n";

    return 0;
}