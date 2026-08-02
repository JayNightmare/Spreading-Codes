#include "gateway2/bch_codec.h"
#include "gateway5/bch_soft_decoder.h"

#include <cstdint>
#include <iostream>
#include <vector>

namespace {

bool TestMlDecodeFromEncodedSymbols() {
    constexpr uint16_t kSb1 = 0x045;

    const std::vector<uint8_t> encoded = lunanet::gateway2::BchEncode(kSb1);
    if (encoded.size() != static_cast<std::size_t>(lunanet::gateway5::kStage3Sb1SoftSymbols)) {
        std::cerr << "FAIL [ml-decode]: encoded size mismatch\n";
        return false;
    }

    std::vector<double> soft(encoded.size(), 0.0);
    for (std::size_t i = 0; i < encoded.size(); ++i) {
        // Add varying magnitudes to ensure metric uses confidence values.
        const double mag = 0.5 + static_cast<double>((i % 7) + 1) * 0.25;
        soft[i] = (encoded[i] == 0u) ? +mag : -mag;
    }

    const int decoded = lunanet::gateway5::DecodeSb1BchSoft(soft);
    if (decoded != static_cast<int>(kSb1)) {
        std::cerr << "FAIL [ml-decode]: decoded 0x" << std::hex << decoded
                  << ", expected 0x" << kSb1 << std::dec << "\n";
        return false;
    }

    return true;
}

bool TestVectorValidation() {
    constexpr uint16_t kSb1 = 0x045;
    constexpr uint64_t kExpectedPacked = 0x229f61dbb84a0ULL;

    const auto encoded = lunanet::gateway2::BchEncode(kSb1);
    const auto packed_opt = lunanet::gateway5::PackBch52MsbFirst(encoded);

    if (!packed_opt.has_value()) {
        std::cerr << "FAIL [vector encode]: PackBch52MsbFirst rejected a valid 52-symbol input\n";
        return false;
    }

    const uint64_t packed = *packed_opt;
    if (packed != kExpectedPacked) {
        std::cerr << "FAIL [vector encode]: encode(0x045) packed = 0x" << std::hex
                  << packed << ", expected 0x" << kExpectedPacked << std::dec << "\n";
        return false;
    }

    const auto soft = lunanet::gateway5::UnpackBch52ToSoft(kExpectedPacked);
    const int decoded = lunanet::gateway5::DecodeSb1BchSoft(soft);
    if (decoded != static_cast<int>(kSb1)) {
        std::cerr << "FAIL [vector decode]: decode(0x229f61dbb84a0) = 0x" << std::hex
                  << decoded << ", expected 0x" << kSb1 << std::dec << "\n";
        return false;
    }

    return true;
}

bool TestInvalidLengthRejected() {
    const std::vector<double> bad(10, 1.0);
    const int decoded = lunanet::gateway5::DecodeSb1BchSoft(bad);
    if (decoded != -1) {
        std::cerr << "FAIL [invalid-length]: expected -1, got " << decoded << "\n";
        return false;
    }
    return true;
}

bool TestPackInvalidSizeRejected() {
    // 0 is a valid all-zero codeword, so the invalid-size case must be
    // distinguishable via std::nullopt rather than a 0 return value.
    const std::vector<uint8_t> too_short(10, 0u);
    const auto packed_opt = lunanet::gateway5::PackBch52MsbFirst(too_short);
    if (packed_opt.has_value()) {
        std::cerr << "FAIL [pack-invalid-length]: expected std::nullopt, got a value\n";
        return false;
    }
    return true;
}

}  // namespace

int main() {
    bool ok = true;

    if (TestMlDecodeFromEncodedSymbols()) {
        std::cout << "PASS: exhaustive ML decode recovers SB1=0x045 from 52 soft symbols\n";
    } else {
        ok = false;
    }

    if (TestVectorValidation()) {
        std::cout << "PASS: encode(0x045) == 0x229f61dbb84a0\n";
        std::cout << "PASS: decode(0x229f61dbb84a0) == 0x045\n";
    } else {
        ok = false;
    }

    if (TestInvalidLengthRejected()) {
        std::cout << "PASS: invalid SB1 soft vector size is rejected\n";
    } else {
        ok = false;
    }

    if (TestPackInvalidSizeRejected()) {
        std::cout << "PASS: PackBch52MsbFirst rejects invalid input size via std::nullopt\n";
    } else {
        ok = false;
    }

    return ok ? 0 : 1;
}
