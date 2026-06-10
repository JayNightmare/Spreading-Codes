#pragma once
#ifndef LUNANET_GATEWAY3_FRAME_CONFIG_H
#define LUNANET_GATEWAY3_FRAME_CONFIG_H

namespace lunanet::gateway3 {

// {LSIS-TBC-2022}: SB2 spare bit count, positioned before CRC, guarded by CRC.
// Default per spec v1.0. Change to match interoperability partner's resolution
// without a rewrite — the total (1176 bits before CRC) stays fixed.
constexpr int kSb2SpareBits = 0;

// {LSIS-TBC-2023}: SB3/SB4 type field width. Must be 4 or 6.
// 4-bit type → 842 payload bits; 6-bit type → 840 payload bits.
// Total before CRC stays fixed at 846 bits.
constexpr int kSb34TypeFieldBits = 4;

static_assert(kSb34TypeFieldBits == 4 || kSb34TypeFieldBits == 6,
              "SB3/SB4 type field must be 4 or 6 bits");

constexpr int kSb34PayloadBits = 846 - kSb34TypeFieldBits;  // 842 or 840

}  // namespace lunanet::gateway3

#endif  // LUNANET_GATEWAY3_FRAME_CONFIG_H
