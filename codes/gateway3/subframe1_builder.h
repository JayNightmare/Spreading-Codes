#pragma once
#ifndef LUNANET_GATEWAY3_SUBFRAME1_BUILDER_H
#define LUNANET_GATEWAY3_SUBFRAME1_BUILDER_H

#include <cstdint>
#include <vector>

namespace lunanet::gateway3 {

// SB1 output is 52 symbols: 1 MSB prepended + 51 BCH-encoded symbols.
constexpr int kSb1Symbols = 52;

// Encodes SB1 per LSIS-AFS §2.4.2.1.
// Packs [FID(2 MSBs) | TOI(7 LSBs)] into a 9-bit word, then applies BCH(51,8).
//
// @param fid  Frame ID, 0–3 (2 bits used)
// @param toi  Time of Interval, 0–99 (7 bits used)
// @return     52-element symbol vector (0 or 1), or empty on error.
std::vector<uint8_t> BuildSubframe1(uint8_t fid, uint8_t toi);

}  // namespace lunanet::gateway3

#endif  // LUNANET_GATEWAY3_SUBFRAME1_BUILDER_H
