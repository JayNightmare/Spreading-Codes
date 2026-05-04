#include "c_api.h"

#include "spreading_codes.h"
#include "gateway2/bch_codec.h"
#include "gateway2/crc24.h"

#include <algorithm>
#include <cstring>

namespace {

int CopyToBuffer(const std::vector<uint8_t>& src, uint8_t* out_buf, int out_capacity) {
    if (out_buf == nullptr || out_capacity <= 0) return 0;
    const int count = std::min(static_cast<int>(src.size()), out_capacity);
    std::memcpy(out_buf, src.data(), count);
    return count;
}

}  // namespace

extern "C" {

void lunanet_init(void) {
    lunanet::initialize_engine();
}

int lunanet_load_config(const char* config_path) {
    if (config_path == nullptr) return -1;
    std::string error;
    return lunanet::load_spreading_code_config(config_path, &error) ? 0 : -1;
}

const char* lunanet_last_error(void) {
    return lunanet::get_last_error();
}

int lunanet_generate_gold(int prn, uint8_t* out_buf, int out_capacity) {
    const auto code = lunanet::generate_gold_code(prn);
    return CopyToBuffer(code, out_buf, out_capacity);
}

int lunanet_generate_weil_primary(int prn, uint8_t* out_buf, int out_capacity) {
    const auto code = lunanet::generate_weil_primary(prn);
    return CopyToBuffer(code, out_buf, out_capacity);
}

int lunanet_generate_weil_tertiary(int prn, uint8_t* out_buf, int out_capacity) {
    const auto code = lunanet::generate_weil_tertiary(prn);
    return CopyToBuffer(code, out_buf, out_capacity);
}

int lunanet_generate_afs_i(int prn, uint8_t* out_buf, int out_capacity) {
    const auto code = lunanet::generate_afs_i(prn);
    return CopyToBuffer(code, out_buf, out_capacity);
}

int lunanet_generate_afs_q(int prn, int max_chips, uint8_t* out_buf, int out_capacity) {
    const auto code = lunanet::generate_afs_q(prn, static_cast<size_t>(max_chips));
    return CopyToBuffer(code, out_buf, out_capacity);
}

int lunanet_gold_code_length(void) {
    return lunanet::GOLD_CODE_LENGTH;
}

int lunanet_weil_primary_length(void) {
    return lunanet::WEIL_PRIMARY_PRIME + lunanet::EXPANSION_LENGTH;
}

int lunanet_weil_tertiary_length(void) {
    return lunanet::WEIL_TERTIARY_LENGTH;
}

int lunanet_bch_encode(int sb1_data, uint8_t* out_buf, int out_capacity) {
    const auto encoded = lunanet::gateway2::BchEncode(static_cast<uint16_t>(sb1_data));
    return CopyToBuffer(encoded, out_buf, out_capacity);
}

uint32_t lunanet_crc24(const uint8_t* data, int length) {
    if (data == nullptr || length <= 0) return 0;
    const std::vector<uint8_t> v(data, data + length);
    return lunanet::gateway2::Crc24Compute(v);
}

}  // extern "C"
