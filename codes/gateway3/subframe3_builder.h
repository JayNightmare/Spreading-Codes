#pragma once
#ifndef LUNANET_GATEWAY3_SUBFRAME3_BUILDER_H
#define LUNANET_GATEWAY3_SUBFRAME3_BUILDER_H

#include <cstdint>
#include <string>
#include <vector>

#include "gateway2/ldpc_encoder.h"

namespace lunanet::gateway3 {

// SB3: Variable data (Orbit Almanac MSG-G5, Coordinate Frame Conversions, etc.)
// 846 data bits → +24-bit CRC → 870 bits → LDPC (with 10 filler bits) → 1740 symbols.
constexpr int kSb3DataBits = 846;
constexpr int kSb3TotalBits = 870;
constexpr int kSb3Symbols = 1740;

struct Subframe3Data {
    uint8_t type;                       // Message type (kSb34TypeFieldBits wide, per frame_config.h)
    std::vector<uint8_t> payload_bits;  // Message payload (bit vector)
    // Remaining space to kSb3DataBits filled with spare bits: alternating 0/1 from 0.
};

// Packs SB3 fields into an 846-bit vector (without CRC). Exported for testing.
std::vector<uint8_t> PackSubframe3(const Subframe3Data& data);

// Full pipeline: pack → CRC-24 → LDPC encode (870→1740, 10 filler bits) → 1740 symbols.
// Returns empty on error; sets *error_message if non-null.
std::vector<uint8_t> BuildSubframe3(const Subframe3Data& data,
                                    const lunanet::gateway2::LdpcMatrices& matrices,
                                    std::string* error_message = nullptr);

}  // namespace lunanet::gateway3

#endif  // LUNANET_GATEWAY3_SUBFRAME3_BUILDER_H
