#pragma once
#ifndef LUNANET_GATEWAY4_SIGNAL_EXPORTER_H
#define LUNANET_GATEWAY4_SIGNAL_EXPORTER_H

#include <string>

#include "iq_generator.h"

namespace lunanet::gateway4 {

// Writes interleaved little-endian float32 I/Q pairs: [I0, Q0, I1, Q1, ...].
// File size is signal.i.size() * 2 * sizeof(float) bytes.
//
// @return true on success; sets *error_message on failure.
bool ExportIqBinary(const IqSignal& signal,
                    const std::string& output_path,
                    std::string* error_message = nullptr);

// Writes I/Q samples as CSV with a header line "index,I,Q", one sample per row.
//
// @return true on success; sets *error_message on failure.
bool ExportIqCsv(const IqSignal& signal,
                 const std::string& output_path,
                 std::string* error_message = nullptr);

}  // namespace lunanet::gateway4

#endif  // LUNANET_GATEWAY4_SIGNAL_EXPORTER_H
