#ifndef LUNANET_GATEWAY2_CRC24_H
#define LUNANET_GATEWAY2_CRC24_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace lunanet::gateway2 {

/**
 * CRC-24Q generator polynomial per LSIS §2.4.3.1.3.
 * g(X) = (1+X) · P(X)
 * P(X) = X²³ + X¹⁷ + X¹³ + X¹² + X¹¹ + X⁹ + X⁸ + X⁷ + X⁵ + X³ + 1
 * Full 25-bit polynomial: 0x1864CFB
 */
constexpr uint32_t kCrc24Poly = 0x864CFB;  // 24-bit representation (implicit MSB)
constexpr uint32_t kCrc24Init = 0x000000;

/**
 * Computes CRC-24Q over a bit vector.
 *
 * @param data  Vector of bits (0 or 1), MSB-first ordering.
 * @return 24-bit CRC value.
 */
uint32_t Crc24Compute(const std::vector<uint8_t>& data);

/**
 * Appends CRC-24 as 24 bits to the data vector.
 *
 * @param data  Vector of bits; 24 CRC bits are appended (MSB first).
 */
void Crc24Append(std::vector<uint8_t>& data);

/**
 * Verifies CRC-24 on a data+CRC bit vector.
 *
 * @param data_with_crc  Data bits followed by 24 CRC bits.
 * @return true if CRC is valid (remainder is zero).
 */
bool Crc24Verify(const std::vector<uint8_t>& data_with_crc);

}  // namespace lunanet::gateway2

#endif  // LUNANET_GATEWAY2_CRC24_H
