#pragma once
#ifndef LUNANET_GATEWAY5_DEINTERLEAVER_H
#define LUNANET_GATEWAY5_DEINTERLEAVER_H

#include <vector>

namespace lunanet::gateway5 {

constexpr int kStage4Rows = 60;
constexpr int kStage4Cols = 98;
constexpr int kStage4InterleavedSymbols = kStage4Rows * kStage4Cols;  // 5880

constexpr int kStage4Sb2Symbols = 2400;
constexpr int kStage4Sb3Symbols = 1740;
constexpr int kStage4Sb4Symbols = 1740;

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
