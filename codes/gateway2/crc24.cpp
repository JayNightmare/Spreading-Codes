#include "crc24.h"

namespace lunanet::gateway2 {

uint32_t Crc24Compute(const std::vector<uint8_t>& data) {
    uint32_t crc = kCrc24Init;

    for (const uint8_t bit : data) {
        crc <<= 1;
        crc |= (bit & 1);

        if ((crc & 0x1000000) != 0) {
            crc ^= kCrc24Poly;
        }
    }

    // Process 24 zero bits (CRC register flush)
    for (int i = 0; i < 24; ++i) {
        crc <<= 1;
        if ((crc & 0x1000000) != 0) {
            crc ^= kCrc24Poly;
        }
    }

    return crc & 0xFFFFFF;
}

void Crc24Append(std::vector<uint8_t>& data) {
    const uint32_t crc = Crc24Compute(data);

    for (int i = 23; i >= 0; --i) {
        data.push_back(static_cast<uint8_t>((crc >> i) & 1));
    }
}

bool Crc24Verify(const std::vector<uint8_t>& data_with_crc) {
    if (data_with_crc.size() < 24) {
        return false;
    }

    // Recompute CRC over the data portion (excluding last 24 bits)
    const size_t data_len = data_with_crc.size() - 24;
    std::vector<uint8_t> data_only(data_with_crc.begin(),
                                   data_with_crc.begin() + static_cast<ptrdiff_t>(data_len));
    const uint32_t computed = Crc24Compute(data_only);

    // Extract the received CRC from the last 24 bits
    uint32_t received = 0;
    for (size_t i = data_len; i < data_with_crc.size(); ++i) {
        received = (received << 1) | (data_with_crc[i] & 1);
    }

    return computed == received;
}

}  // namespace lunanet::gateway2
