#include "gold_code_generator.h"

namespace lunanet::gateway1 {

namespace {

constexpr int kLfsrLength = 11;
static_assert(kLfsrLength > 0 && kLfsrLength < 16,
              "kLfsrLength must fit within the 16-bit LFSR state register");

// Period of the maximal-length LFSR (2^n - 1). The Gold code (kGoldCodeLength)
// is this period shortened by one chip.
constexpr int kGoldCodePeriod = (1 << kLfsrLength) - 1;

constexpr uint16_t kG1Init = 0x7FF;
constexpr uint16_t kG1FeedbackMask = 0x402;
constexpr uint16_t kG2FeedbackMask = 0x492;

class Lfsr {
public:
    Lfsr(uint16_t initial_state, uint16_t feedback_mask)
        : state_(initial_state), feedback_mask_(feedback_mask) {}

    uint8_t NextBit() {
        const uint8_t output = static_cast<uint8_t>((state_ >> (kLfsrLength - 1)) & 1U);

        const uint16_t masked_state = static_cast<uint16_t>(state_ & feedback_mask_);
        uint8_t feedback = 0;
        for (int i = 0; i < kLfsrLength; ++i) {
            feedback ^= static_cast<uint8_t>((masked_state >> i) & 1U);
        }

        state_ = static_cast<uint16_t>(((state_ << 1) & ((1 << kLfsrLength) - 1)) | feedback);
        return output;
    }

private:
    uint16_t state_;
    uint16_t feedback_mask_;
};

}  // namespace

std::vector<uint8_t> GenerateGoldCode(int prn, const SpreadingSpecTables& tables, std::string* error_message) {
    if (prn < 1 || prn > kMaxPrns) {
        if (error_message != nullptr) {
            *error_message = "PRN must be in range [1, 210]";
        }
        return {};
    }

    if (tables.g2_delays.size() != static_cast<size_t>(kMaxPrns)) {
        if (error_message != nullptr) {
            *error_message = "Gold table not loaded";
        }
        return {};
    }

    const int g2_delay = tables.g2_delays[prn - 1];
    if (g2_delay < 0) {
        if (error_message != nullptr) {
            *error_message = "Missing G2 delay for PRN " + std::to_string(prn);
        }
        return {};
    }

    if (g2_delay >= kGoldCodePeriod) {
        if (error_message != nullptr) {
            *error_message = "G2 delay " + std::to_string(g2_delay) +
                " out of range [0, " + std::to_string(kGoldCodePeriod - 1) +
                "] for PRN " + std::to_string(prn);
        }
        return {};
    }

    Lfsr g1(kG1Init, kG1FeedbackMask);
    Lfsr g2(kG1Init, kG2FeedbackMask);

    const int advance_amount = (kGoldCodePeriod - g2_delay) % kGoldCodePeriod;
    for (int i = 0; i < advance_amount; ++i) {
        static_cast<void>(g2.NextBit());
    }

    std::vector<uint8_t> gold;
    gold.reserve(kGoldCodeLength);

    for (int i = 0; i < kGoldCodeLength; ++i) {
        const uint8_t chip = static_cast<uint8_t>(g1.NextBit() ^ g2.NextBit());
        gold.push_back(chip);
    }

    return gold;
}

}  // namespace lunanet::gateway1
