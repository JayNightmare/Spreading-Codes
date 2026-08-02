#include "frame_exporter.h"

#include <algorithm>
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

// Writes a little-endian 32-bit unsigned integer.
void WriteU32Le(std::ofstream& out, uint32_t value) {
    char bytes[4] = {
        static_cast<char>(value & 0xFFu),
        static_cast<char>((value >> 8) & 0xFFu),
        static_cast<char>((value >> 16) & 0xFFu),
        static_cast<char>((value >> 24) & 0xFFu),
    };
    out.write(bytes, sizeof(bytes));
}

// Writes a little-endian 64-bit unsigned integer.
void WriteU64Le(std::ofstream& out, uint64_t value) {
    char bytes[8];
    for (int i = 0; i < 8; ++i) {
        bytes[i] = static_cast<char>((value >> (8 * i)) & 0xFFu);
    }
    out.write(bytes, sizeof(bytes));
}

uint32_t ReadU32Le(const unsigned char* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

uint64_t ReadU64Le(const unsigned char* p) {
    uint64_t value = 0;
    for (int i = 0; i < 8; ++i) {
        value |= static_cast<uint64_t>(p[i]) << (8 * i);
    }
    return value;
}

constexpr char kFrameMagic[8] = {'L', 'S', 'I', 'S', 'A', 'F', 'S', '\0'};
constexpr std::size_t kFrameHeaderSize = 64;

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

bool ExportFrameRaw(const std::vector<uint8_t>& frame,
                    const std::string& output_path,
                    std::string* error_message) {
    if (frame.empty()) {
        if (error_message) *error_message = "Frame is empty";
        return false;
    }

    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        if (error_message) *error_message = "Failed to open: " + output_path;
        return false;
    }

    // Write one byte per symbol — values are 0x00 or 0x01.
    out.write(reinterpret_cast<const char*>(frame.data()),
              static_cast<std::streamsize>(frame.size()));
    if (!out) {
        if (error_message) *error_message = "Write failed: " + output_path;
        return false;
    }

    return true;
}

bool ExportFrameStandard(const std::vector<uint8_t>& frame,
                         uint32_t prn,
                         uint64_t timestamp_unix,
                         const std::string& output_path,
                         std::string* error_message) {
    if (frame.empty()) {
        if (error_message) *error_message = "Frame is empty";
        return false;
    }

    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        if (error_message) *error_message = "Failed to open: " + output_path;
        return false;
    }

    out.write(kFrameMagic, sizeof(kFrameMagic));
    WriteU32Le(out, 1);  // version
    WriteU32Le(out, static_cast<uint32_t>(frame.size()));
    WriteU32Le(out, prn);
    WriteU64Le(out, timestamp_unix);
    const char reserved[36] = {0};
    out.write(reserved, sizeof(reserved));

    out.write(reinterpret_cast<const char*>(frame.data()),
              static_cast<std::streamsize>(frame.size()));

    if (!out) {
        if (error_message) *error_message = "Write failed: " + output_path;
        return false;
    }
    return true;
}

bool ImportFrameStandard(const std::string& input_path,
                         std::vector<uint8_t>* out_frame,
                         FrameFileHeader* out_header,
                         std::string* error_message) {
    std::ifstream in(input_path, std::ios::binary);
    if (!in) {
        if (error_message) *error_message = "Failed to open: " + input_path;
        return false;
    }

    std::vector<unsigned char> header(kFrameHeaderSize);
    in.read(reinterpret_cast<char*>(header.data()), static_cast<std::streamsize>(kFrameHeaderSize));
    if (!in || static_cast<std::size_t>(in.gcount()) != kFrameHeaderSize) {
        if (error_message) *error_message = "File is shorter than the " +
            std::to_string(kFrameHeaderSize) + "-byte header: " + input_path;
        return false;
    }

    if (!std::equal(std::begin(kFrameMagic), std::end(kFrameMagic), header.begin())) {
        if (error_message) *error_message = "Magic mismatch: not a valid LSISAFS frame file";
        return false;
    }

    FrameFileHeader parsed;
    parsed.version = ReadU32Le(&header[8]);
    parsed.frame_length = ReadU32Le(&header[12]);
    parsed.prn = ReadU32Le(&header[16]);
    parsed.timestamp_unix = ReadU64Le(&header[20]);

    std::vector<uint8_t> payload(parsed.frame_length);
    if (parsed.frame_length > 0) {
        in.read(reinterpret_cast<char*>(payload.data()),
                static_cast<std::streamsize>(parsed.frame_length));
        if (!in || static_cast<uint32_t>(in.gcount()) != parsed.frame_length) {
            if (error_message) *error_message = "Payload shorter than header's frame_length (" +
                std::to_string(parsed.frame_length) + " symbols): " + input_path;
            return false;
        }
    }

    if (out_header) *out_header = parsed;
    if (out_frame) *out_frame = std::move(payload);
    return true;
}

}  // namespace lunanet::gateway3
