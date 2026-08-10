#pragma once
#ifndef LUNANET_GATEWAY4_BPSK_MODULATOR_H
#define LUNANET_GATEWAY4_BPSK_MODULATOR_H

#include <cstdint>
#include <string>
#include <vector>

namespace lunanet::gateway4 {

// Maps a single logic-level chip to its BPSK signal level (LSIS-150, Table 8):
//   logic 0 → +1.0,  logic 1 → -1.0.
// Any non-zero input is treated as logic 1.
float BpskMap(uint8_t logic_level);

// Maps a logic-level chip stream to BPSK signal levels (±1.0), element-wise.
std::vector<float> BpskModulate(const std::vector<uint8_t>& chips);

// Modulates the AFS-I data channel: each navigation data symbol is modulo-2
// added (XOR) onto one full primary code epoch (LSIS-160). The result is the
// logic-level AFS-I chip stream prior to BPSK mapping.
//
// For symbol k, emits primary_code[i] XOR data_symbols[k] for i in [0, 2046).
// The output length is data_symbols.size() * 2046.
//
// @param primary_code   One Gold primary code epoch (must be 2046 chips).
// @param data_symbols   Navigation data symbols (one bit per symbol).
// @param error_message  Optional error string on failure.
// @return Logic-level AFS-I chip stream, or empty on error.
std::vector<uint8_t> ModulateAfsIData(const std::vector<uint8_t>& primary_code,
                                      const std::vector<uint8_t>& data_symbols,
                                      std::string* error_message = nullptr);

}  // namespace lunanet::gateway4

#endif  // LUNANET_GATEWAY4_BPSK_MODULATOR_H
