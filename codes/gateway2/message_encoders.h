#ifndef LUNANET_GATEWAY2_MESSAGE_ENCODERS_H
#define LUNANET_GATEWAY2_MESSAGE_ENCODERS_H

#include <cstdint>
#include <string>
#include <vector>

namespace lunanet::gateway2 {

std::vector<uint8_t> bch_encode(const std::vector<uint8_t>& message_bits);
std::vector<uint8_t> crc24_encode(const std::vector<uint8_t>& message_bits);
std::vector<uint8_t> ldpc_encode(const std::vector<uint8_t>& message_bits);
const char* get_version();

} // namespace lunanet::gateway2

#endif // LUNANET_GATEWAY2_MESSAGE_ENCODERS_H