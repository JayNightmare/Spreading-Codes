#ifndef LUNANET_GATEWAY2_BCH_CODEC_H
#define LUNANET_GATEWAY2_BCH_CODEC_H

#include <array>
#include <cstdint>
#include <vector>

namespace lunanet::gateway2 {

constexpr int kBchInputBits = 9;
constexpr int kBchLfsrStages = 8;
constexpr int kBchEncodedSymbols = 52;
constexpr int kBchLfsrOutputSymbols = 51;

/**
 * BCH(51,8) generator polynomial 763 (octal) = 0x1F3.
 * x^8 + x^7 + x^6 + x^5 + x + 1
 */
constexpr uint8_t kBchGeneratorPoly = 0xF3;  // 8-bit feedback mask (excludes x^8 tap)

/**
 * Encodes a 9-bit SB1 field (FID[2] + TOI[7]) using BCH(51,8).
 *
 * Algorithm per LSIS §2.4.2.1:
 *   1. Load 8 LSBs (bits 1-8) into an 8-stage LFSR as initial state.
 *   2. Shift the LFSR 51 times to produce 51 encoded symbols.
 *   3. XOR the MSB (bit 0) with all 51 symbols.
 *   4. Prepend the MSB → 52 symbols total.
 *
 * @param sb1_data  9-bit value: bits[8:7] = FID, bits[6:0] = TOI.
 * @return 52-element vector of symbols (0 or 1).
 */
std::vector<uint8_t> BchEncode(uint16_t sb1_data);

/**
 * Generates all 256 BCH(51,8) codewords for the MSB=0 case.
 * Used for soft-decision decoding via exhaustive correlation.
 *
 * @return 256-element array, each containing 51 encoded symbols.
 */
std::array<std::array<uint8_t, kBchLfsrOutputSymbols>, 256> BchGenerateCodebook();

/**
 * Soft-decision BCH(51,8) decoder via exhaustive correlation.
 *
 * @param soft_symbols  52 received soft-decision values (sign/magnitude).
 *                      Positive → bit 0, Negative → bit 1.
 * @return Decoded 9-bit SB1 value, or -1 on failure.
 */
int BchDecodeSoft(const std::vector<double>& soft_symbols);

}  // namespace lunanet::gateway2

#endif  // LUNANET_GATEWAY2_BCH_CODEC_H
