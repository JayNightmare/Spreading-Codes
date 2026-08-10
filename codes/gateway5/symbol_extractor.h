#pragma once
#ifndef LUNANET_GATEWAY5_SYMBOL_EXTRACTOR_H
#define LUNANET_GATEWAY5_SYMBOL_EXTRACTOR_H

#include <cstddef>
#include <vector>

namespace lunanet::gateway5 {

// Frame layout constants per LSIS-AFS Table 8.
// Total frame: 6000 symbols split into three regions.
constexpr int kFrameSymbols        = 6000;
constexpr int kSpSymbolsStage2     = 68;    // SP  : [0   : 68)
constexpr int kSb1SymbolsStage2    = 52;    // SB1 : [68  : 120)
constexpr int kInterleavedStart    = 120;   // interleaved block starts here
constexpr int kInterleavedSymbols  = 5880;  // [120 : 6000)

// Holds the three symbol regions after slicing the received frame.
// All values are kept signed (soft): sign = best-guess bit, magnitude = confidence.
struct ExtractedFrame {
    std::vector<double> sp;           // 68 soft values  — sync pattern region
    std::vector<double> sb1;          // 52 soft values  — SB1 / BCH region
    std::vector<double> interleaved;  // 5880 soft values — interleaved SB2-SB4 block
};

/**
 * Slice 6000 symbols from `received` beginning at `frame_offset`.
 *
 * The result is split into the three frame regions:
 *   SP               = [0   : 68)   (68 symbols)
 *   SB1              = [68  : 120)  (52 symbols)
 *   interleaved      = [120 : 6000) (5880 symbols)
 *
 * Values are kept as signed soft floats — do NOT hard-slice to 0/1.
 *
 * @param received      Full despread received signal (soft, signed).
 * @param frame_offset  Start index of the frame (from Stage 1 sync detection).
 * @return ExtractedFrame with all three regions populated, or empty vectors
 *         if `received` is too short to hold 6000 symbols from `frame_offset`.
 */
ExtractedFrame ExtractFrameSymbols(const std::vector<double>& received,
                                   std::size_t frame_offset);

/**
 * Convert a vector of despread soft values to log-likelihood ratios (LLRs).
 *
 * For BPSK in AWGN with the LSIS sign convention (logic 0 → +1, logic 1 → −1):
 *
 *   LLR[i] = 2 * r[i] / noise_variance
 *
 * Interpretation:
 *   positive LLR → bit 0,  negative LLR → bit 1,  magnitude = confidence.
 *
 * @param soft_values    Signed received values (output of despreader).
 * @param noise_variance Per-symbol noise variance σ² (must be > 0; throws
 *                       std::invalid_argument otherwise).
 * @return LLR vector of the same length as `soft_values`.
 */
std::vector<double> ComputeLlr(const std::vector<double>& soft_values,
                                double noise_variance);

}  // namespace lunanet::gateway5

#endif  // LUNANET_GATEWAY5_SYMBOL_EXTRACTOR_H
