#include "subframe1_builder.h"

#include "gateway2/bch_codec.h"

namespace lunanet::gateway3 {

std::vector<uint8_t> BuildSubframe1(uint8_t fid, uint8_t toi) {
    // Pack per spec: bit 0 (MSB) = FID high bit, bits 1-8 = FID low + TOI
    // Integer layout: bits[8:7] = FID, bits[6:0] = TOI
    const uint16_t sb1_data =
        static_cast<uint16_t>(((fid & 0x3u) << 7) | (toi & 0x7Fu));
    return lunanet::gateway2::BchEncode(sb1_data);
}

}  // namespace lunanet::gateway3
