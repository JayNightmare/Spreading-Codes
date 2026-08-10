#include "iq_generator.h"

#include "bpsk_modulator.h"
#include "signal_config.h"

namespace lunanet::gateway4 {

IqSignal GenerateIq(const std::vector<uint8_t>& afs_i_chips,
                    const std::vector<uint8_t>& afs_q_chips,
                    const IqConfig& config,
                    std::string* error_message) {
    if (config.sample_rate_hz <= 0 ||
        config.sample_rate_hz % kAfsIChipRateHz != 0) {
        if (error_message) {
            *error_message = "Sample rate " + std::to_string(config.sample_rate_hz) +
                " Hz must be a positive multiple of the AFS-I chip rate (" +
                std::to_string(kAfsIChipRateHz) + " Hz)";
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

    // Duration in AFS-I chips. Total samples = afs_i_chips × (rate / i_rate).
    const size_t samples_per_i_chip =
        static_cast<size_t>(config.sample_rate_hz / kAfsIChipRateHz);
    const size_t num_samples = afs_i_chips.size() * samples_per_i_chip;

    IqSignal out;
    out.sample_rate_hz = config.sample_rate_hz;
    out.i.reserve(num_samples);
    out.q.reserve(num_samples);

    // Rational chip-index mapping: chip(n) = floor(n * chip_rate / sample_rate).
    // For I: chip_i = floor(n / samples_per_i_chip) — always integer ratio.
    // For Q: chip_q = floor(n * kAfsQChipRateHz / sample_rate). Use 64-bit to
    // avoid overflow when sample_rate × chip_count exceeds 32-bit range.
    const auto i_rate = static_cast<int64_t>(kAfsIChipRateHz);
    const auto q_rate = static_cast<int64_t>(kAfsQChipRateHz);
    const auto sr = static_cast<int64_t>(config.sample_rate_hz);

    for (size_t n = 0; n < num_samples; ++n) {
        const size_t i_index = n * static_cast<size_t>(i_rate) / static_cast<size_t>(sr);
        const size_t q_index = static_cast<size_t>(static_cast<int64_t>(n) * q_rate / sr);
        out.i.push_back(BpskMap(afs_i_chips[i_index]));
        out.q.push_back(BpskMap(afs_q_chips[q_index]));
    }

    return out;
}

}  // namespace lunanet::gateway4

