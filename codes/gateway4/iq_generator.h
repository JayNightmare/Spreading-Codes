#pragma once
#ifndef LUNANET_GATEWAY4_IQ_GENERATOR_H
#define LUNANET_GATEWAY4_IQ_GENERATOR_H

#include <cstdint>
#include <string>
#include <vector>

#include "signal_config.h"

namespace lunanet::gateway4 {

// Configuration for I/Q baseband sample generation.
struct IqConfig {
    // Output sample rate. Must be a positive integer multiple of the AFS-Q chip
    // rate so both channels resample to an integer number of samples per chip.
    int sample_rate_hz = kDefaultSampleRateHz;
};

// Generated complex baseband signal. I and Q always have equal length and are
// sampled at sample_rate_hz.
struct IqSignal {
    std::vector<float> i;    // In-phase (AFS-I, data channel), BPSK ±1.0
    std::vector<float> q;    // Quadrature (AFS-Q, pilot channel), BPSK ±1.0
    int sample_rate_hz = 0;
};

// Generates complex baseband I/Q samples from the two channel chip streams.
//
// The AFS-I and AFS-Q streams must cover the same time span. Because AFS-Q runs
// at exactly 5× the AFS-I chip rate (LSIS Table 7), this requires
// afs_q_chips.size() == 5 * afs_i_chips.size(). Each channel is resampled to the
// common output rate by zero-order hold, preserving code/data synchronization:
//   - AFS-I symbol boundaries fall on Gold primary epochs (LSIS-160),
//   - AFS-Q tertiary code start aligns with the frame start (LSIS-220).
//
// @param afs_i_chips   Data-modulated AFS-I logic chips (see ModulateAfsIData).
// @param afs_q_chips   AFS-Q tiered pilot logic chips (5× the AFS-I length).
// @param config        Sample-rate configuration.
// @param error_message Optional error string on failure.
// @return I/Q signal, or an empty signal (i/q empty) on error.
IqSignal GenerateIq(const std::vector<uint8_t>& afs_i_chips,
                    const std::vector<uint8_t>& afs_q_chips,
                    const IqConfig& config,
                    std::string* error_message = nullptr);

}  // namespace lunanet::gateway4

#endif  // LUNANET_GATEWAY4_IQ_GENERATOR_H
