#pragma once
#ifndef LUNANET_GATEWAY3_SUBFRAME2_BUILDER_H
#define LUNANET_GATEWAY3_SUBFRAME2_BUILDER_H

#include <cstdint>
#include <string>
#include <vector>

#include "gateway2/ldpc_encoder.h"

namespace lunanet::gateway3 {

// SB2: Clock & Ephemeris, broadcast every frame.
// 1176 data bits → +24-bit CRC → 1200 bits → LDPC → 2400 symbols.
constexpr int kSb2DataBits = 1176;
constexpr int kSb2TotalBits = 1200;
constexpr int kSb2Symbols = 2400;

struct Subframe2Data {
    uint16_t wn;                        // Week Number (13 bits, 0–8191)
    uint16_t itow;                      // Integer Time of Week (9 bits, 0–511)
    uint8_t toi;                        // Time of Interval (7 bits, 0–99)
    std::vector<uint8_t> payload_bits;  // CED + Health/Safety + Time Conversions (bit vector)
    // Remaining space to kSb2DataBits filled with spare bits: alternating 0/1 from 0.
    // kSb2SpareBits ({LSIS-TBC-2022}) controls how many spare bits to expect;
    // PackSubframe2 always fills to exactly kSb2DataBits regardless.
};

// Packs SB2 fields into a 1176-bit vector (without CRC). Exported for testing.
std::vector<uint8_t> PackSubframe2(const Subframe2Data& data);

// Full pipeline: pack → CRC-24 → LDPC encode → 2400 symbols.
// Returns empty on error; sets *error_message if non-null.
std::vector<uint8_t> BuildSubframe2(const Subframe2Data& data,
                                    const lunanet::gateway2::LdpcMatrices& matrices,
                                    std::string* error_message = nullptr);

}  // namespace lunanet::gateway3

#endif  // LUNANET_GATEWAY3_SUBFRAME2_BUILDER_H
