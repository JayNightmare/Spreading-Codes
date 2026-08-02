#pragma once
#ifndef LUNANET_GATEWAY5_DEINTERLEAVER_H
#define LUNANET_GATEWAY5_DEINTERLEAVER_H

#include <vector>

#include "gateway2/interleaver.h"

namespace lunanet::gateway5 {

// Single source of truth: mirror gateway2's interleaver grid dimensions
// directly instead of redefining the same constants, so a future spec
// change to the grid size only needs to be made in one place.
constexpr int kStage4Rows = lunanet::gateway2::kInterleaverRows;
constexpr int kStage4Cols = lunanet::gateway2::kInterleaverCols;
constexpr int kStage4InterleavedSymbols = lunanet::gateway2::kInterleaverSize;  // 5880

constexpr int kStage4Sb2Symbols = 2400;
constexpr int kStage4Sb3Symbols = 1740;
constexpr int kStage4Sb4Symbols = 1740;

// Keep the SB2/SB3/SB4 partition consistent with the shared interleaver
// total: if the canonical grid size ever changes upstream but this
// partition isn't updated to match, catch it at compile time instead of
// silently truncating/misaligning subframes at runtime.
static_assert(kStage4Sb2Symbols + kStage4Sb3Symbols + kStage4Sb4Symbols ==
                  kStage4InterleavedSymbols,
              "Stage 4 subframe sizes must sum to kStage4InterleavedSymbols");

struct Stage4Subframes {
    std::vector<double> sb2;
    std::vector<double> sb3;
    std::vector<double> sb4;
};

/**
 * Stage 4 deinterleaver for Gateway 5 soft symbols.
 *
 * Mapping mirrors gateway2 block deinterleaver exactly, but values stay soft:
 *   - write interleaved input column-wise into 60x98 grid
 *   - read row-wise to produce deinterleaved stream
 *
 * @param interleaved_soft 5880 soft symbols from ExtractedFrame.interleaved.
 * @return Flat deinterleaved stream, or empty on size mismatch.
 */
std::vector<double> DeinterleaveSoft(const std::vector<double>& interleaved_soft);

/**
 * Splits a flat deinterleaved stream into SB2/SB3/SB4.
 *
 * SB2 = first 2400, SB3 = next 1740, SB4 = final 1740 symbols.
 * Returns empty vectors on size mismatch.
 */
Stage4Subframes SplitDeinterleavedSubframes(const std::vector<double>& deinterleaved_soft);

/**
 * Convenience API: deinterleave 5880 soft symbols and split to SB2/SB3/SB4.
 */
Stage4Subframes DeinterleaveToSubframes(const std::vector<double>& interleaved_soft);

}  // namespace lunanet::gateway5

#endif  // LUNANET_GATEWAY5_DEINTERLEAVER_H
