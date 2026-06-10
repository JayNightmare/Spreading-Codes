#include "iq_generator.h"

#include "bpsk_modulator.h"
#include "signal_config.h"

namespace lunanet::gateway4 {

IqSignal GenerateIq(const std::vector<uint8_t>& afs_i_chips,
                    const std::vector<uint8_t>& afs_q_chips,
                    const IqConfig& config,
                    std::string* error_message) {
    if (config.sample_rate_hz <= 0 ||
        config.sample_rate_hz % kAfsQChipRateHz != 0) {
        if (error_message) {
            *error_message = "Sample rate " + std::to_string(config.sample_rate_hz) +
                " Hz must be a positive multiple of the AFS-Q chip rate (" +
                std::to_string(kAfsQChipRateHz) + " Hz)";
        }
        return {};
    }
    if (afs_i_chips.empty() || afs_q_chips.empty()) {
        if (error_message) *error_message = "Empty chip stream(s)";
        return {};
    }

    // Both channels must span the same time. AFS-Q runs at 5× the AFS-I chip
    // rate, so it must carry exactly 5× the chips (LSIS Table 7).
    if (afs_q_chips.size() != afs_i_chips.size() * static_cast<size_t>(kQOverIChipRatio)) {
        if (error_message) {
            *error_message = "Channel length mismatch: AFS-Q has " +
                std::to_string(afs_q_chips.size()) + " chips, expected 5× AFS-I (" +
                std::to_string(afs_i_chips.size() * kQOverIChipRatio) + ")";
        }
        return {};
    }

    // Samples per chip for each channel (integer by construction above).
    const int q_oversample = config.sample_rate_hz / kAfsQChipRateHz;
    const int i_oversample = q_oversample * kQOverIChipRatio;  // samples per AFS-I chip

    const size_t num_samples = afs_q_chips.size() * static_cast<size_t>(q_oversample);

    IqSignal out;
    out.sample_rate_hz = config.sample_rate_hz;
    out.i.reserve(num_samples);
    out.q.reserve(num_samples);

    // Zero-order hold: sample n maps to chip index floor(n / samples_per_chip).
    // Dividing the sample index avoids any 64-bit overflow from rate products.
    for (size_t n = 0; n < num_samples; ++n) {
        const size_t q_index = n / static_cast<size_t>(q_oversample);
        const size_t i_index = n / static_cast<size_t>(i_oversample);
        out.i.push_back(BpskMap(afs_i_chips[i_index]));
        out.q.push_back(BpskMap(afs_q_chips[q_index]));
    }

    return out;
}

}  // namespace lunanet::gateway4
