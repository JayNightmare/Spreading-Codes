#include "signal_exporter.h"

#include <cstring>
#include <fstream>

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

}  // namespace lunanet::gateway4
