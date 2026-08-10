#ifndef LUNANET_GATEWAY2_INTERLEAVER_H
#define LUNANET_GATEWAY2_INTERLEAVER_H

#include <cstdint>
#include <vector>

namespace lunanet::gateway2 {

constexpr int kInterleaverRows = 60;
constexpr int kInterleaverCols = 98;
constexpr int kInterleaverSize = kInterleaverRows * kInterleaverCols;  // 5880

/**
 * Block interleaver: write row-wise, read column-wise.
 * Input: 5880 symbols (SB2[2400] + SB3[1740] + SB4[1740]).
 *
 * @param input  Exactly kInterleaverSize symbols.
 * @return Interleaved symbol vector, or empty on size mismatch.
 */
std::vector<uint8_t> Interleave(const std::vector<uint8_t>& input);

/**
 * Block deinterleaver: write column-wise, read row-wise.
 * Reverses the interleaving operation.
 *
 * @param input  Exactly kInterleaverSize interleaved symbols.
 * @return Deinterleaved symbol vector, or empty on size mismatch.
 */
std::vector<uint8_t> Deinterleave(const std::vector<uint8_t>& input);

}  // namespace lunanet::gateway2

#endif  // LUNANET_GATEWAY2_INTERLEAVER_H
