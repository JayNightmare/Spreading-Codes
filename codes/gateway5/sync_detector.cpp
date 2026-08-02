#include "gateway5/sync_detector.h"
#include "gateway5/frame_synchronizer.h"

#include <algorithm>
#include <cmath>

namespace lunanet::gateway5 {

std::vector<double> ComputeSyncCorrelation(const std::vector<double>& received) {
    const std::vector<float> reference = BuildSyncReferenceSymbols();
    const std::size_t pattern_len = reference.size();

    if (received.size() < pattern_len) {
        return {};
    }

    const std::size_t num_offsets = received.size() - pattern_len + 1;
    std::vector<double> correlation(num_offsets, 0.0);

    // Brute-force sliding correlation. O(N * 68). The spec explicitly
    // recommends building this simple version first and only reaching for
    // something faster (e.g. FFT-based correlation) if performance testing
    // later shows it's needed.
    for (std::size_t offset = 0; offset < num_offsets; ++offset) {
        double sum = 0.0;
        for (std::size_t k = 0; k < pattern_len; ++k) {
            sum += received[offset + k] * static_cast<double>(reference[k]);
        }
        correlation[offset] = sum;
    }

    return correlation;
}

FrameSyncResult DetectFrameSync(const std::vector<double>& received,
                                 double psr_threshold) {
    FrameSyncResult result;

    const std::vector<double> correlation = ComputeSyncCorrelation(received);
    if (correlation.empty()) {
        return result;  // detected = false
    }

    // Locate the global peak. Signed max (not max-magnitude): given the
    // reference's fixed sign convention, a genuine match produces a
    // strongly POSITIVE spike (aligned terms are all (+-1)*(+-1) = +1), so
    // we're specifically looking for that positive spike rather than any
    // large-magnitude excursion.
    const auto peak_it = std::max_element(correlation.begin(), correlation.end());
    const std::size_t peak_offset =
        static_cast<std::size_t>(peak_it - correlation.begin());
    const double peak_value = *peak_it;
    result.peak_correlation = peak_value;

    // Next-highest sidelobe: the largest-MAGNITUDE correlation outside a
    // kSyncSidelobeExclusion-symbol window centered on the peak. Magnitude
    // (not signed value) because a strong negative excursion in the noise
    // floor is just as relevant to false-alarm risk as a positive one.
    double next_highest_mag = 0.0;
    bool have_sidelobe = false;
    for (std::size_t offset = 0; offset < correlation.size(); ++offset) {
        const long long distance = static_cast<long long>(offset) -
                                    static_cast<long long>(peak_offset);
        if (std::llabs(distance) <= kSyncSidelobeExclusion) {
            continue;
        }
        const double mag = std::fabs(correlation[offset]);
        if (!have_sidelobe || mag > next_highest_mag) {
            next_highest_mag = mag;
            have_sidelobe = true;
        }
    }

    if (!have_sidelobe) {
        // Stream too short to have any offsets outside the exclusion
        // window around the peak -- can't compute a meaningful PSR.
        return result;
    }

    // Guard against a near-zero sidelobe floor, which would make PSR
    // blow up numerically rather than reflecting genuine confidence.
    constexpr double kMinSidelobeFloor = 1e-6;
    const double sidelobe_floor = std::max(next_highest_mag, kMinSidelobeFloor);
    result.psr = std::fabs(peak_value) / sidelobe_floor;

    if (peak_value > 0.0 && result.psr >= psr_threshold) {
        result.detected = true;
        result.frame_offset = peak_offset;
    }

    return result;
}

}  // namespace lunanet::gateway5
