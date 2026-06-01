#include "frame_exporter.h"

#include <fstream>
#include <iomanip>
#include <sstream>

namespace lunanet::gateway3 {

namespace {

// Packs a bit vector into bytes, MSB first. 6000 bits → 750 bytes, no padding.
std::vector<uint8_t> PackBits(const std::vector<uint8_t>& bits) {
    const int n = static_cast<int>(bits.size());
    const int n_bytes = (n + 7) / 8;
    std::vector<uint8_t> bytes(n_bytes, 0);
    for (int i = 0; i < n; ++i) {
        if (bits[i] & 1u) {
            bytes[i / 8] |= static_cast<uint8_t>(0x80u >> (i % 8));
        }
    }
    return bytes;
}

}  // namespace

bool ExportFrameBinary(const std::vector<uint8_t>& frame,
                       const std::string& output_path,
                       std::string* error_message) {
    if (frame.empty()) {
        if (error_message) *error_message = "Frame is empty";
        return false;
    }

    const auto bytes = PackBits(frame);

    // Binary mode prevents Windows from converting '\n' bytes inside the bitstream
    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        if (error_message) *error_message = "Failed to open: " + output_path;
        return false;
    }

    out.write(reinterpret_cast<const char*>(bytes.data()),
              static_cast<std::streamsize>(bytes.size()));
    if (!out) {
        if (error_message) *error_message = "Write failed: " + output_path;
        return false;
    }

    return true;
}

bool ExportFrameCsv(const std::vector<uint8_t>& frame,
                    const std::string& output_path,
                    std::string* error_message) {
    if (frame.empty()) {
        if (error_message) *error_message = "Frame is empty";
        return false;
    }

    std::ofstream out(output_path);
    if (!out) {
        if (error_message) *error_message = "Failed to open: " + output_path;
        return false;
    }

    for (const uint8_t sym : frame) {
        out << static_cast<int>(sym & 1u) << '\n';
    }

    if (!out) {
        if (error_message) *error_message = "Write failed: " + output_path;
        return false;
    }

    return true;
}

std::string ExportFrameHex(const std::vector<uint8_t>& frame) {
    if (frame.empty()) return {};

    const auto bytes = PackBits(frame);
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (const uint8_t b : bytes) {
        oss << std::setw(2) << static_cast<unsigned>(b);
    }
    return oss.str();
}

}  // namespace lunanet::gateway3
