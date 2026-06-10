#include "subframe4_builder.h"

#include "frame_config.h"
#include "gateway2/crc24.h"
#include "gateway2/ldpc_encoder.h"

namespace lunanet::gateway3 {

namespace {

// Appends 'count' bits from 'value', MSB first.
void AppendBits(std::vector<uint8_t>& bits, uint64_t value, int count) {
    for (int i = count - 1; i >= 0; --i) {
        bits.push_back(static_cast<uint8_t>((value >> i) & 1u));
    }
}

// Alternating 0/1 fill, starting with 0. CRC protects these bits.
void AppendSpareBits(std::vector<uint8_t>& bits, int target) {
    int idx = 0;
    while (static_cast<int>(bits.size()) < target) {
        bits.push_back(static_cast<uint8_t>(idx & 1));
        ++idx;
    }
}

}  // namespace

std::vector<uint8_t> PackSubframe4(const Subframe4Data& data) {
    std::vector<uint8_t> bits;
    bits.reserve(kSb4DataBits);

    // Type field: 4 or 6 bits per {LSIS-TBC-2023}
    AppendBits(bits, data.type, kSb34TypeFieldBits);

    for (uint8_t b : data.payload_bits) {
        if (static_cast<int>(bits.size()) >= kSb4DataBits) break;
        bits.push_back(b & 1u);
    }

    AppendSpareBits(bits, kSb4DataBits);

    return bits;
}

std::vector<uint8_t> BuildSubframe4(const Subframe4Data& data,
                                    const lunanet::gateway2::LdpcMatrices& matrices,
                                    std::string* error_message) {
    auto bits = PackSubframe4(data);

    lunanet::gateway2::Crc24Append(bits);  // 846 → 870 bits

    if (static_cast<int>(bits.size()) != kSb4TotalBits) {
        if (error_message) *error_message = "SB4 bit count after CRC: " +
            std::to_string(bits.size()) + " (expected 870)";
        return {};
    }

    return lunanet::gateway2::LdpcEncode(
        bits, matrices, lunanet::gateway2::kLdpcSb34, error_message);
}

}  // namespace lunanet::gateway3
