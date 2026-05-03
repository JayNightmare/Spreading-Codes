/**
 * 1. Converts binary chip arrays into formatted hexadecimal strings for CSV validation.
 * 2. Replaces the invalid AFS-Q sequence concatenation with a chip-by-chip modulo-2
 * addition (XOR) aligning the primary, secondary, and tertiary codes.
 */

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "spreading_codes.h"

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

    static std::string trim(const std::string& input) {
        size_t start = 0;
        while (start < input.size() && std::isspace(static_cast<unsigned char>(input[start]))) {
            ++start;
        }
        size_t end = input.size();
        while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1]))) {
            --end;
        }
        return input.substr(start, end - start);
    }

    static int hex_to_value(char ch) {
        if (ch >= '0' && ch <= '9') return ch - '0';
        if (ch >= 'A' && ch <= 'F') return 10 + (ch - 'A');
        if (ch >= 'a' && ch <= 'f') return 10 + (ch - 'a');
        return -1;
    }

    std::vector<uint8_t> hex_to_vector(const std::string& hex, size_t num_chips = 0) {
        std::vector<uint8_t> chips;
        chips.reserve(hex.size() * 4);

        for (char ch : hex) {
            if (std::isspace(static_cast<unsigned char>(ch))) continue;
            const int value = hex_to_value(ch);
            if (value < 0) continue;
            for (int bit = 3; bit >= 0; --bit) {
                chips.push_back(static_cast<uint8_t>((value >> bit) & 1));
            }
        }

        if (num_chips > 0 && chips.size() > num_chips) {
            chips.resize(num_chips);
        }

        return chips;
    }

    bool load_reference_hex_file(const std::string& path,
                                 size_t expected_chips,
                                 std::vector<std::vector<uint8_t>>& out,
                                 std::string* error_message) {
        std::ifstream file(path);
        if (!file) {
            if (error_message) {
                *error_message = "Failed to open reference file: " + path;
            }
            return false;
        }

        out.assign(MAX_PRNS, {});
        std::string line;
        size_t line_no = 0;
        size_t loaded = 0;

        while (std::getline(file, line)) {
            ++line_no;
            std::string trimmed = trim(line);
            if (trimmed.empty() || trimmed[0] == '#') continue;

            const size_t comma = trimmed.find(',');
            if (comma == std::string::npos) {
                continue;
            }

            const std::string prn_text = trim(trimmed.substr(0, comma));
            const std::string hex_text = trim(trimmed.substr(comma + 1));

            int prn = 0;
            try {
                prn = std::stoi(prn_text, nullptr, 10);
            } catch (...) {
                continue;
            }

            if (prn < 1 || prn > MAX_PRNS) {
                continue;
            }

            std::vector<uint8_t> chips = hex_to_vector(hex_text, expected_chips);
            if (expected_chips > 0 && chips.size() < expected_chips) {
                if (error_message) {
                    *error_message = "Reference row too short at line " + std::to_string(line_no);
                }
                return false;
            }

            out[prn - 1] = std::move(chips);
            ++loaded;
        }

        size_t missing = 0;
        for (const auto& entry : out) {
            if (entry.empty()) {
                ++missing;
            }
        }

        if (missing > 0) {
            if (error_message) {
                *error_message = "Reference file missing PRNs (loaded " + std::to_string(loaded) + ")";
            }
            return false;
        }

        return true;
    }

} // namespace lunanet
