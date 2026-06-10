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

}  // namespace lunanet::gateway3

#endif  // LUNANET_GATEWAY3_FRAME_EXPORTER_H
