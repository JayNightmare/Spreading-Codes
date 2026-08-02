#include "signal_exporter.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iterator>

namespace lunanet::gateway4 {

namespace {

// Serializes one float as 4 little-endian bytes, independent of host byte order.
void WriteLittleEndianFloat(std::ofstream& out, float value) {
    uint32_t bits = 0;
    static_assert(sizeof(bits) == sizeof(value), "float must be 32 bits");
    std::memcpy(&bits, &value, sizeof(bits));
    char bytes[4] = {
        static_cast<char>(bits & 0xFFu),
        static_cast<char>((bits >> 8) & 0xFFu),
        static_cast<char>((bits >> 16) & 0xFFu),
        static_cast<char>((bits >> 24) & 0xFFu),
    };
    out.write(bytes, sizeof(bytes));
}

float ReadLittleEndianFloat(const unsigned char* p) {
    uint32_t bits = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
                    (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
    float value = 0.0f;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

void WriteU32Le(std::ofstream& out, uint32_t value) {
    char bytes[4] = {
        static_cast<char>(value & 0xFFu),
        static_cast<char>((value >> 8) & 0xFFu),
        static_cast<char>((value >> 16) & 0xFFu),
        static_cast<char>((value >> 24) & 0xFFu),
    };
    out.write(bytes, sizeof(bytes));
}

uint32_t ReadU32Le(const unsigned char* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

void WriteF64Le(std::ofstream& out, double value) {
    uint64_t bits = 0;
    static_assert(sizeof(bits) == sizeof(value), "double must be 64 bits");
    std::memcpy(&bits, &value, sizeof(bits));
    char bytes[8];
    for (int i = 0; i < 8; ++i) {
        bytes[i] = static_cast<char>((bits >> (8 * i)) & 0xFFu);
    }
    out.write(bytes, sizeof(bytes));
}

double ReadF64Le(const unsigned char* p) {
    uint64_t bits = 0;
    for (int i = 0; i < 8; ++i) {
        bits |= static_cast<uint64_t>(p[i]) << (8 * i);
    }
    double value = 0.0;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

constexpr char kIqMagic[8] = {'L', 'S', 'I', 'S', 'I', 'Q', '\0', '\0'};
constexpr std::size_t kIqHeaderSize = 128;
constexpr char kSampleFormatFloat32[16] = {'f', 'l', 'o', 'a', 't', '3', '2'};

}  // namespace

bool ExportIqBinary(const IqSignal& signal,
                    const std::string& output_path,
                    std::string* error_message) {
    if (signal.i.empty() || signal.i.size() != signal.q.size()) {
        if (error_message) *error_message = "Signal is empty or I/Q length mismatch";
        return false;
    }

    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        if (error_message) *error_message = "Failed to open: " + output_path;
        return false;
    }

    for (size_t n = 0; n < signal.i.size(); ++n) {
        WriteLittleEndianFloat(out, signal.i[n]);
        WriteLittleEndianFloat(out, signal.q[n]);
    }

    if (!out) {
        if (error_message) *error_message = "Write failed: " + output_path;
        return false;
    }

    return true;
}

bool ExportIqCsv(const IqSignal& signal,
                 const std::string& output_path,
                 std::string* error_message) {
    if (signal.i.empty() || signal.i.size() != signal.q.size()) {
        if (error_message) *error_message = "Signal is empty or I/Q length mismatch";
        return false;
    }

    std::ofstream out(output_path);
    if (!out) {
        if (error_message) *error_message = "Failed to open: " + output_path;
        return false;
    }

    out << "index,I,Q\n";
    for (size_t n = 0; n < signal.i.size(); ++n) {
        out << n << ',' << signal.i[n] << ',' << signal.q[n] << '\n';
    }

    if (!out) {
        if (error_message) *error_message = "Write failed: " + output_path;
        return false;
    }

    return true;
}

bool ExportIqBinaryStandard(const IqSignal& signal,
                           uint32_t prn,
                           const std::string& output_path,
                           std::string* error_message) {
    if (signal.i.empty() || signal.i.size() != signal.q.size()) {
        if (error_message) *error_message = "Signal is empty or I/Q length mismatch";
        return false;
    }
    if (signal.sample_rate_hz <= 0) {
        if (error_message) *error_message = "Signal has an invalid sample rate";
        return false;
    }

    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        if (error_message) *error_message = "Failed to open: " + output_path;
        return false;
    }

    const double duration_sec =
        static_cast<double>(signal.i.size()) / static_cast<double>(signal.sample_rate_hz);

    out.write(kIqMagic, sizeof(kIqMagic));
    WriteU32Le(out, 1);  // version
    WriteF64Le(out, static_cast<double>(signal.sample_rate_hz));
    WriteF64Le(out, duration_sec);
    WriteU32Le(out, prn);
    out.write(kSampleFormatFloat32, sizeof(kSampleFormatFloat32));
    const char reserved[80] = {0};
    out.write(reserved, sizeof(reserved));

    for (size_t n = 0; n < signal.i.size(); ++n) {
        WriteLittleEndianFloat(out, signal.i[n]);
        WriteLittleEndianFloat(out, signal.q[n]);
    }

    if (!out) {
        if (error_message) *error_message = "Write failed: " + output_path;
        return false;
    }
    return true;
}

bool ImportIqBinaryStandard(const std::string& input_path,
                           IqSignal* out_signal,
                           IqFileHeader* out_header,
                           std::string* error_message) {
    std::ifstream in(input_path, std::ios::binary);
    if (!in) {
        if (error_message) *error_message = "Failed to open: " + input_path;
        return false;
    }

    std::vector<unsigned char> header(kIqHeaderSize);
    in.read(reinterpret_cast<char*>(header.data()), static_cast<std::streamsize>(kIqHeaderSize));
    if (!in || static_cast<std::size_t>(in.gcount()) != kIqHeaderSize) {
        if (error_message) *error_message = "File is shorter than the " +
            std::to_string(kIqHeaderSize) + "-byte header: " + input_path;
        return false;
    }

    if (!std::equal(std::begin(kIqMagic), std::end(kIqMagic), header.begin())) {
        if (error_message) *error_message = "Magic mismatch: not a valid LSISIQ signal file";
        return false;
    }

    IqFileHeader parsed;
    parsed.version = ReadU32Le(&header[8]);
    parsed.sample_rate_hz = ReadF64Le(&header[12]);
    parsed.duration_sec = ReadF64Le(&header[20]);
    parsed.prn = ReadU32Le(&header[28]);

    const bool is_float32 = std::equal(std::begin(kSampleFormatFloat32),
                                       std::end(kSampleFormatFloat32),
                                       header.begin() + 32);
    if (!is_float32) {
        if (error_message) *error_message = "Unsupported sample format (only float32 is supported)";
        return false;
    }

    std::vector<unsigned char> payload((std::istreambuf_iterator<char>(in)),
                                       std::istreambuf_iterator<char>());
    constexpr std::size_t kBytesPerSample = 2 * sizeof(float);  // I and Q
    if (payload.size() % kBytesPerSample != 0) {
        if (error_message) *error_message = "Sample payload length is not a multiple of " +
            std::to_string(kBytesPerSample) + " bytes (malformed I/Q pair): " + input_path;
        return false;
    }

    IqSignal signal;
    signal.sample_rate_hz = static_cast<int>(parsed.sample_rate_hz);
    const std::size_t num_samples = payload.size() / kBytesPerSample;
    signal.i.reserve(num_samples);
    signal.q.reserve(num_samples);
    for (std::size_t n = 0; n < num_samples; ++n) {
        signal.i.push_back(ReadLittleEndianFloat(&payload[n * kBytesPerSample]));
        signal.q.push_back(ReadLittleEndianFloat(&payload[n * kBytesPerSample + sizeof(float)]));
    }

    if (out_header) *out_header = parsed;
    if (out_signal) *out_signal = std::move(signal);
    return true;
}

}  // namespace lunanet::gateway4
