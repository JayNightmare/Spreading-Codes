#pragma once
#ifndef LUNANET_GATEWAY3_SYNC_PATTERN_H
#define LUNANET_GATEWAY3_SYNC_PATTERN_H

#include <cstdint>
#include <vector>

namespace lunanet::gateway3 {

// Sync pattern is 68 bits (17 hex nibbles), uncoded. No FEC applied.
// Per LSIS-AFS §2.4: hex CC63F74536F49E04A, MSB first.
constexpr int kSyncPatternSymbols = 68;

// Returns a 68-element bit vector (MSB first) parsed nibble-by-nibble
// from the 17-nibble sync hex. Tightly packed: no byte-alignment padding.
std::vector<uint8_t> BuildSyncPattern();

}  // namespace lunanet::gateway3

#endif  // LUNANET_GATEWAY3_SYNC_PATTERN_H
