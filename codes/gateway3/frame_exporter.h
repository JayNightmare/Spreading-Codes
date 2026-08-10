#pragma once
#ifndef LUNANET_GATEWAY3_FRAME_EXPORTER_H
#define LUNANET_GATEWAY3_FRAME_EXPORTER_H

#include <cstdint>
#include <string>
#include <vector>

namespace lunanet::gateway3 {

// Exports a 6000-symbol frame (each element 0 or 1) as packed uint8 bytes.
// 6000 bits / 8 = 750 bytes, MSB of each byte first, no padding needed.
// Returns false and sets *error_message on failure.
bool ExportFrameBinary(const std::vector<uint8_t>& frame,
                       const std::string& output_path,
                       std::string* error_message = nullptr);

// Exports a frame as CSV with one symbol per line ("0" or "1").
// Returns false and sets *error_message on failure.
bool ExportFrameCsv(const std::vector<uint8_t>& frame,
                    const std::string& output_path,
                    std::string* error_message = nullptr);

// Returns the frame as a lowercase hex string of packed bytes (750 chars = 375 hex pairs).
// Returns empty string on error.
std::string ExportFrameHex(const std::vector<uint8_t>& frame);

// Exports frame symbols as raw bytes, one byte per symbol (0x00 or 0x01).
// Output is exactly frame.size() bytes — 6000 for a standard frame.
// This matches the workshop interop CI contract ("exactly 6000 bytes").
bool ExportFrameRaw(const std::vector<uint8_t>& frame,
                    const std::string& output_path,
                    std::string* error_message = nullptr);

// Metadata carried in the standardized final-submission interop frame file
// header (see the LSIS-AFS interoperability testing document's "Frame
// Export Format"). Distinct from the Goonhilly-workshop-specific headerless
// contract that ExportFrameRaw/ExportFrameBinary implement above -- both
// formats are kept side by side since they serve different test harnesses.
struct FrameFileHeader {
    uint32_t version = 1;
    uint32_t frame_length = 0;   // Number of symbols (6000 for a standard frame).
    uint32_t prn = 0;
    uint64_t timestamp_unix = 0;
};

// Writes the standardized interop frame file: a 64-byte header (magic
// "LSISAFS\0", version, frame_length, PRN, Unix timestamp, reserved),
// followed by `frame.size()` raw symbol bytes (0x00/0x01). All numeric
// header fields are little-endian.
//
// @return true on success; sets *error_message on failure.
bool ExportFrameStandard(const std::vector<uint8_t>& frame,
                         uint32_t prn,
                         uint64_t timestamp_unix,
                         const std::string& output_path,
                         std::string* error_message = nullptr);

// Reads a standardized interop frame file written by ExportFrameStandard
// (or another spec-compliant implementation). Validates the "LSISAFS\0"
// magic and that the symbol payload length matches the header's
// frame_length field.
//
// @return true on success (out_frame/out_header populated); false with
//         *error_message set on a magic mismatch, truncated file, or
//         length mismatch.
bool ImportFrameStandard(const std::string& input_path,
                         std::vector<uint8_t>* out_frame,
                         FrameFileHeader* out_header,
                         std::string* error_message = nullptr);

}  // namespace lunanet::gateway3

#endif  // LUNANET_GATEWAY3_FRAME_EXPORTER_H
