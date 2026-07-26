#pragma once
#ifndef LUNANET_GATEWAY5_CRC_VALIDATOR_H
#define LUNANET_GATEWAY5_CRC_VALIDATOR_H

#include <cstdint>
#include <string>
#include <vector>

namespace lunanet::gateway5 {

enum class SubframeCrcType {
    Sb2,
    Sb3,
    Sb4,
};

struct SubframeCrcVerdict {
    bool valid = false;
    int total_bits = 0;
    int data_bits = 0;
    uint32_t computed_crc = 0;
    uint32_t received_crc = 0;
    std::string error;
};

struct FrameCrcVerdict {
    SubframeCrcVerdict sb2;
    SubframeCrcVerdict sb3;
    SubframeCrcVerdict sb4;
    bool frame_accepted = false;
};

/**
 * Validate CRC-24Q for one decoded subframe systematic vector.
 *
 * Input bits must be Stage 5 output (systematic bits `s`) with SB3/SB4 filler
 * already stripped. This function splits [data+spare][24-bit CRC], recomputes
 * CRC over data+spare, and compares against the received CRC field.
 */
SubframeCrcVerdict ValidateSubframeCrc(const std::vector<uint8_t>& decoded_systematic_bits,
                                       SubframeCrcType type);

/**
 * Validate CRC for SB2/SB3/SB4 and produce a frame-level accept/reject verdict.
 * Frame is accepted only if all three CRC checks pass.
 */
FrameCrcVerdict ValidateFrameCrc(const std::vector<uint8_t>& sb2_systematic_bits,
                                 const std::vector<uint8_t>& sb3_systematic_bits,
                                 const std::vector<uint8_t>& sb4_systematic_bits);

/**
 * CRC-24Q sanity helper: computes CRC over byte data interpreted MSB-first.
 */
uint32_t ComputeCrc24QOverBytesMsbFirst(const std::string& ascii_bytes);

}  // namespace lunanet::gateway5

#endif  // LUNANET_GATEWAY5_CRC_VALIDATOR_H
