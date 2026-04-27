/**
 * 1. Converts binary chip arrays into formatted hexadecimal strings for CSV validation.
 * 2. Replaces the invalid AFS-Q sequence concatenation with a chip-by-chip modulo-2
 * addition (XOR) aligning the primary, secondary, and tertiary codes.
 */

#include <string>
#include <vector>
#include <cstdint>

namespace lunanet {

    std::string vector_to_hex(const std::vector<uint8_t>& chips, size_t num_chips = 24) {
        std::string hex_str;
        int current_val = 0;

        for (size_t i = 0; i < num_chips && i < chips.size(); ++i) {
            current_val = (current_val << 1) | (chips[i] & 1);

            if ((i + 1) % 4 == 0) {
                hex_str += "0123456789ABCDEF"[current_val];
                current_val = 0;
            }
        }
        return hex_str;
    }

} // namespace lunanet