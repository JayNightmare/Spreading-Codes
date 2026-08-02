#include "sync_pattern.h"

namespace lunanet::gateway3 {

namespace {

uint8_t HexNibble(char c) {
    if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
    if (c >= 'A' && c <= 'F') return static_cast<uint8_t>(c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return static_cast<uint8_t>(c - 'a' + 10);
    return 0;
}

}  // namespace

std::vector<uint8_t> BuildSyncPattern() {
    // 17 nibbles × 4 bits = 68 bits — not byte-aligned, so parsed nibble-by-nibble.
    // Whole-byte reads would produce 4 spurious zero bits from the 9th byte.
    static const char kSyncHex[] = "CC63F74536F49E04A";

    std::vector<uint8_t> bits;
    bits.reserve(kSyncPatternSymbols);

    for (int i = 0; kSyncHex[i] != '\0'; ++i) {
        const uint8_t nibble = HexNibble(kSyncHex[i]);
        bits.push_back((nibble >> 3) & 1);
        bits.push_back((nibble >> 2) & 1);
        bits.push_back((nibble >> 1) & 1);
        bits.push_back((nibble >> 0) & 1);
    }

    // Guard against kSyncHex ever being edited to a different length: the
    // rest of the codebase (frame_synchronizer, frame_sync_test) assumes
    // exactly kSyncPatternSymbols bits and has no other way to detect drift.
    if (bits.size() != static_cast<std::size_t>(kSyncPatternSymbols)) {
        return {};
    }

    return bits;
}

}  // namespace lunanet::gateway3
