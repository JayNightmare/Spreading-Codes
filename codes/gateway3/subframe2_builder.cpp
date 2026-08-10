#include "subframe2_builder.h"

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

std::vector<uint8_t> PackSubframe2(const Subframe2Data& data) {
    std::vector<uint8_t> bits;
    bits.reserve(kSb2DataBits);

    AppendBits(bits, data.wn & 0x1FFFu, 13);   // WN: 13 bits
    AppendBits(bits, data.itow & 0x1FFu, 9);    // ITOW: 9 bits
    AppendBits(bits, data.toi & 0x7Fu, 7);      // TOI: 7 bits

    // Payload (CED + Health/Safety + Time Conversions), truncated if oversized
    for (uint8_t b : data.payload_bits) {
        if (static_cast<int>(bits.size()) >= kSb2DataBits) break;
        bits.push_back(b & 1u);
    }

    // Spare bits fill remaining space; alternating 0/1 starting with 0
    AppendSpareBits(bits, kSb2DataBits);

    return bits;
}

std::vector<uint8_t> BuildSubframe2(const Subframe2Data& data,
                                    const lunanet::gateway2::LdpcMatrices& matrices,
                                    std::string* error_message) {
    auto bits = PackSubframe2(data);

    lunanet::gateway2::Crc24Append(bits);  // 1176 → 1200 bits

    if (static_cast<int>(bits.size()) != kSb2TotalBits) {
        if (error_message) *error_message = "SB2 bit count after CRC: " +
            std::to_string(bits.size()) + " (expected 1200)";
        return {};
    }

    return lunanet::gateway2::LdpcEncode(
        bits, matrices, lunanet::gateway2::kLdpcSb2, error_message);
}

}  // namespace lunanet::gateway3
