#pragma once
#ifndef LUNANET_GATEWAY5_BCH_SOFT_DECODER_H
#define LUNANET_GATEWAY5_BCH_SOFT_DECODER_H

#include <cstdint>
#include <vector>

namespace lunanet::gateway5 {

constexpr int kStage3Sb1SoftSymbols = 52;
constexpr int kStage3InnerSoftSymbols = 51;

/**
 * Gateway 5 Stage 3: exhaustive ML BCH soft-decode for SB1.
 *
 * Input must be the 52 signed soft values from ExtractedFrame.sb1:
 *   - sb1_soft[0] = prepended MSB symbol
 *   - sb1_soft[1..51] = 51 BCH inner symbols
 *
 * Decoder rule:
 *   1. Enumerate all 256 MSB=0 BCH hypotheses (cached).
 *   2. Correlate each hypothesis with the 51 inner soft symbols:
 *      add |s| for sign match, subtract |s| for sign mismatch.
 *   3. Choose max |correlation|.
 *   4. MSB = 0 if best correlation > 0, else MSB = 1.
 *
 * @param sb1_soft 52 signed soft values.
 * @return Decoded 9-bit FID+TOI (bits[8:7]=FID, bits[6:0]=TOI), or -1 on
 *         invalid input size.
 */
int DecodeSb1BchSoft(const std::vector<double>& sb1_soft);

/**
 * Packs a BCH symbol vector (MSB-first) into a 52-bit integer.
 *
 * @param symbols 52 binary symbols.
 * @return Packed 52-bit value in the low bits of uint64_t, or 0 on invalid
 *         input.
 */
uint64_t PackBch52MsbFirst(const std::vector<uint8_t>& symbols);

/**
 * Unpacks a 52-bit integer (MSB-first) into signed soft values.
 *
 * Bit mapping uses logic 0 -> +1.0 and logic 1 -> -1.0.
 */
std::vector<double> UnpackBch52ToSoft(uint64_t packed);

}  // namespace lunanet::gateway5

#endif  // LUNANET_GATEWAY5_BCH_SOFT_DECODER_H
