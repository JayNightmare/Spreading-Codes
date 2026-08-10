#include "gateway2/interleaver.h"
#include "gateway5/deinterleaver.h"

#include <cstdint>
#include <iostream>
#include <vector>

namespace {

bool TestRejectsInvalidLength() {
    const std::vector<double> bad(10, 0.0);
    const auto flat = lunanet::gateway5::DeinterleaveSoft(bad);
    if (!flat.empty()) {
        std::cerr << "FAIL [invalid-length]: expected empty flat output\n";
        return false;
    }

    const auto split = lunanet::gateway5::DeinterleaveToSubframes(bad);
    if (!split.sb2.empty() || !split.sb3.empty() || !split.sb4.empty()) {
        std::cerr << "FAIL [invalid-length]: expected empty split outputs\n";
        return false;
    }

    return true;
}

bool TestSplitSizesAndBoundaries() {
    std::vector<double> deinterleaved(lunanet::gateway5::kStage4InterleavedSymbols, 0.0);
    for (int i = 0; i < lunanet::gateway5::kStage4InterleavedSymbols; ++i) {
        deinterleaved[static_cast<std::size_t>(i)] = static_cast<double>(i);
    }

    const auto split = lunanet::gateway5::SplitDeinterleavedSubframes(deinterleaved);
    if (static_cast<int>(split.sb2.size()) != lunanet::gateway5::kStage4Sb2Symbols ||
        static_cast<int>(split.sb3.size()) != lunanet::gateway5::kStage4Sb3Symbols ||
        static_cast<int>(split.sb4.size()) != lunanet::gateway5::kStage4Sb4Symbols) {
        std::cerr << "FAIL [split-size]: unexpected SB2/SB3/SB4 sizes\n";
        return false;
    }

    if (split.sb2.front() != 0.0 || split.sb2.back() != 2399.0) {
        std::cerr << "FAIL [split-sb2]: boundary mismatch\n";
        return false;
    }
    if (split.sb3.front() != 2400.0 || split.sb3.back() != 4139.0) {
        std::cerr << "FAIL [split-sb3]: boundary mismatch\n";
        return false;
    }
    if (split.sb4.front() != 4140.0 || split.sb4.back() != 5879.0) {
        std::cerr << "FAIL [split-sb4]: boundary mismatch\n";
        return false;
    }

    return true;
}

bool TestMandatoryIdentityWithGateway2Interleave() {
    constexpr int kN = lunanet::gateway5::kStage4InterleavedSymbols;
    constexpr int kBits = 13;  // 2^13 = 8192 > 5880

    // Reconstruct gateway2::Interleave permutation exactly using bit planes,
    // then test our soft deinterleaver on integer labels 0..5879.
    std::vector<std::vector<uint8_t>> plane_outputs(
        kBits, std::vector<uint8_t>(static_cast<std::size_t>(kN), 0));

    for (int bit = 0; bit < kBits; ++bit) {
        std::vector<uint8_t> plane_input(static_cast<std::size_t>(kN), 0);
        for (int i = 0; i < kN; ++i) {
            plane_input[static_cast<std::size_t>(i)] =
                static_cast<uint8_t>((i >> bit) & 0x1);
        }

        const auto interleaved_plane = lunanet::gateway2::Interleave(plane_input);
        if (interleaved_plane.size() != static_cast<std::size_t>(kN)) {
            std::cerr << "FAIL [identity]: gateway2 Interleave returned bad size\n";
            return false;
        }
        plane_outputs[static_cast<std::size_t>(bit)] = interleaved_plane;
    }

    std::vector<double> interleaved_labels(static_cast<std::size_t>(kN), 0.0);
    std::vector<int> seen(static_cast<std::size_t>(kN), 0);

    for (int j = 0; j < kN; ++j) {
        int src_index = 0;
        for (int bit = 0; bit < kBits; ++bit) {
            src_index |= (static_cast<int>(plane_outputs[static_cast<std::size_t>(bit)]
                                          [static_cast<std::size_t>(j)])
                          << bit);
        }

        if (src_index < 0 || src_index >= kN) {
            std::cerr << "FAIL [identity]: reconstructed source index out of range\n";
            return false;
        }

        ++seen[static_cast<std::size_t>(src_index)];
        interleaved_labels[static_cast<std::size_t>(j)] = static_cast<double>(src_index);
    }

    for (int i = 0; i < kN; ++i) {
        if (seen[static_cast<std::size_t>(i)] != 1) {
            std::cerr << "FAIL [identity]: permutation reconstruction not one-to-one\n";
            return false;
        }
    }

    const auto deinterleaved = lunanet::gateway5::DeinterleaveSoft(interleaved_labels);
    if (deinterleaved.size() != static_cast<std::size_t>(kN)) {
        std::cerr << "FAIL [identity]: deinterleaved size mismatch\n";
        return false;
    }

    for (int i = 0; i < kN; ++i) {
        if (deinterleaved[static_cast<std::size_t>(i)] != static_cast<double>(i)) {
            std::cerr << "FAIL [identity]: mismatch at index " << i
                      << ", got " << deinterleaved[static_cast<std::size_t>(i)] << '\n';
            return false;
        }
    }

    return true;
}

}  // namespace

int main() {
    bool ok = true;

    if (TestMandatoryIdentityWithGateway2Interleave()) {
        std::cout << "PASS: Interleave(0..5879) then Stage 4 soft deinterleave is identity\n";
    } else {
        ok = false;
    }

    if (TestSplitSizesAndBoundaries()) {
        std::cout << "PASS: SB2/SB3/SB4 split is 2400/1740/1740 with correct boundaries\n";
    } else {
        ok = false;
    }

    if (TestRejectsInvalidLength()) {
        std::cout << "PASS: invalid interleaved input length is rejected\n";
    } else {
        ok = false;
    }

    return ok ? 0 : 1;
}
