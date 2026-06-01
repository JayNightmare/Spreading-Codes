#pragma once
#ifndef LUNANET_GATEWAY3_SUBFRAME4_BUILDER_H
#define LUNANET_GATEWAY3_SUBFRAME4_BUILDER_H

#include <cstdint>
#include <string>
#include <vector>

#include "gateway2/ldpc_encoder.h"

namespace lunanet::gateway3 {

// SB4: Network access (LunaNet Network Access Information, etc.)
// 846 data bits → +24-bit CRC → 870 bits → LDPC (with 10 filler bits) → 1740 symbols.
// Identical encoding pipeline to SB3; differs only in message type definitions.
constexpr int kSb4DataBits = 846;
constexpr int kSb4TotalBits = 870;
constexpr int kSb4Symbols = 1740;

struct Subframe4Data {
    uint8_t type;                       // Message type (kSb34TypeFieldBits wide, per frame_config.h)
    std::vector<uint8_t> payload_bits;  // Message payload (bit vector)
    // Remaining space to kSb4DataBits filled with spare bits: alternating 0/1 from 0.
};

// Packs SB4 fields into an 846-bit vector (without CRC). Exported for testing.
std::vector<uint8_t> PackSubframe4(const Subframe4Data& data);

// Full pipeline: pack → CRC-24 → LDPC encode (870→1740, 10 filler bits) → 1740 symbols.
// Returns empty on error; sets *error_message if non-null.
std::vector<uint8_t> BuildSubframe4(const Subframe4Data& data,
                                    const lunanet::gateway2::LdpcMatrices& matrices,
                                    std::string* error_message = nullptr);

}  // namespace lunanet::gateway3

#endif  // LUNANET_GATEWAY3_SUBFRAME4_BUILDER_H
