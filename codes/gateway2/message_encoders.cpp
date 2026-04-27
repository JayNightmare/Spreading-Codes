#include "message_encoders.h"

namespace lunanet::gateway2 {

std::vector<uint8_t> bch_encode(const std::vector<uint8_t>& message_bits) {
    return message_bits;
}

std::vector<uint8_t> crc24_encode(const std::vector<uint8_t>& message_bits) {
    return message_bits;
}

std::vector<uint8_t> ldpc_encode(const std::vector<uint8_t>& message_bits) {
    return message_bits;
}

const char* get_version() {
    return "gateway2-scaffold-0.1";
}

} // namespace lunanet::gateway2