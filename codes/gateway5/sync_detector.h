#pragma once
#ifndef LUNANET_GATEWAY5_SYNC_DETECTOR_H
#define LUNANET_GATEWAY5_SYNC_DETECTOR_H

#include <cstddef>
#include <vector>

namespace lunanet::gateway5 {

// Default PSR (peak-to-sidelobe ratio) threshold for declaring a detection.
// Not specified by the LSIS-AFS spec, so this was derived empirically
// (see sync_detector_test.cpp's Monte Carlo test) rather than assumed from
// generic GNSS-acquisition literature: for this 68-symbol pattern against
// random BPSK background data, the achievable PSR ceiling is much lower
// than typical GNSS acquisition PSR values (~2-10), because random data
// bits alone -- with zero channel noise -- already produce sidelobes
// large enough to cap noiseless PSR around ~2.4-2.7. A threshold of 1.35
// clears the spec's >99% detection target at SNR=3dB (99.4% measured over
// 2000 trials) while keeping the false-alarm rate on pure-noise input low
// (~0.55% measured over 2000 trials). Exposed as a parameter so it can be
// re-tuned once real operational/hardware data is available.
constexpr double kDefaultSyncPsrThreshold = 1.35;

// Exclusion window (in symbols) around the peak, inside which candidate
// offsets are NOT considered when searching for the "next-highest sidelobe"
// used in the PSR calculation. One full pattern length (68) is used so the
// sidelobe search isn't contaminated by the main peak's own correlation
// shoulder.
constexpr int kSyncSidelobeExclusion = 68;

struct FrameSyncResult {
    bool detected = false;
    std::size_t frame_offset = 0;
    double peak_correlation = 0.0;
    double psr = 0.0;
};

/**
 * Computes the raw correlation of the 68-symbol BPSK sync reference
 * pattern (BuildSyncReferenceSymbols) against `received` at every valid
 * offset (0 .. received.size() - 68).
 *
 * Exposed separately from DetectFrameSync so tests can validate the
 * self-correlation property mandated by the spec before trusting the
 * detector on real data, and so callers can inspect the full correlation
 * profile if needed.
 *
 * @param received  Despread, symbol-rate soft values to search.
 * @return One correlation value per valid offset, or empty if `received`
 *         is shorter than the 68-symbol pattern.
 */
std::vector<double> ComputeSyncCorrelation(const std::vector<double>& received);

/**
 * Slides the 68-symbol BPSK sync reference pattern across `received` and
 * locates the frame-start offset via peak-to-sidelobe-ratio (PSR)
 * detection, per LSIS-AFS Table 8 / Sec 2.3 (Gateway 5 Stage 1).
 *
 * `received` must already be despread, symbol-rate soft values (sign = bit
 * guess, magnitude = confidence) -- the same convention ExtractFrameSymbols
 * consumes. This function does NOT despread raw I/Q samples (see FSD-5.3,
 * not yet implemented) and does not perform sub-symbol/Doppler
 * interpolation (that applies to upstream chip-level tracking, not this
 * stage's integer symbol-offset output).
 *
 * An absolute correlation threshold is deliberately not used: the spec
 * explicitly calls this out as unreliable when channel power varies.
 *
 * @param received       Despread, symbol-rate soft values to search.
 * @param psr_threshold  Minimum PSR to declare a detection.
 * @return FrameSyncResult with detected=false if `received` is shorter
 *         than the 68-symbol pattern, or if no candidate offset clears
 *         psr_threshold.
 */
FrameSyncResult DetectFrameSync(const std::vector<double>& received,
                                 double psr_threshold = kDefaultSyncPsrThreshold);

}  // namespace lunanet::gateway5

#endif  // LUNANET_GATEWAY5_SYNC_DETECTOR_H
