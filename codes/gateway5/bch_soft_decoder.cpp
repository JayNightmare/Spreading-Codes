#include "gateway5/bch_soft_decoder.h"

#include "gateway2/bch_codec.h"

#include <array>
#include <cmath>

namespace lunanet::gateway5 {

namespace {

const std::array<std::array<uint8_t, gateway2::kBchLfsrOutputSymbols>, 256>&
GetBchHypotheses() {
    // The 256 MSB=0 BCH hypotheses are constant, so cache once.
    static const auto codebook = gateway2::BchGenerateCodebook();
    return codebook;
}

}  // namespace

int DecodeSb1BchSoft(const std::vector<double>& sb1_soft) {
    if (sb1_soft.size() != static_cast<std::size_t>(kStage3Sb1SoftSymbols)) {
        return -1;
    }

    const auto& hypotheses = GetBchHypotheses();

    double best_abs_corr = -1.0;
    double best_raw_corr = 0.0;
    int best_data = 0;

    for (int data = 0; data < 256; ++data) {
        double corr = 0.0;

        for (int i = 0; i < kStage3InnerSoftSymbols; ++i) {
            const double soft = sb1_soft[static_cast<std::size_t>(i + 1)];
            const double magnitude = std::abs(soft);

            // LSIS sign convention: positive soft => bit guess 0.
            const bool received_zero = (soft >= 0.0);
            const bool hypothesis_zero = (hypotheses[data][i] == 0);

            corr += (received_zero == hypothesis_zero) ? magnitude : -magnitude;
        }

        const double abs_corr = std::abs(corr);
        if (abs_corr > best_abs_corr) {
            best_abs_corr = abs_corr;
            best_raw_corr = corr;
            best_data = data;
        }
    }

    const int decoded_msb = (best_raw_corr > 0.0) ? 0 : 1;
    return (decoded_msb << 8) | best_data;
}

uint64_t PackBch52MsbFirst(const std::vector<uint8_t>& symbols) {
    if (symbols.size() != static_cast<std::size_t>(kStage3Sb1SoftSymbols)) {
        return 0;
    }

    uint64_t packed = 0;
    for (const uint8_t bit : symbols) {
        packed <<= 1;
        packed |= static_cast<uint64_t>(bit & 0x1u);
    }
    return packed;
}

std::vector<double> UnpackBch52ToSoft(uint64_t packed) {
    std::vector<double> soft(kStage3Sb1SoftSymbols, 0.0);

    for (int i = 0; i < kStage3Sb1SoftSymbols; ++i) {
        const int bit_index_from_lsb = (kStage3Sb1SoftSymbols - 1) - i;
        const uint8_t bit = static_cast<uint8_t>((packed >> bit_index_from_lsb) & 0x1u);
        soft[static_cast<std::size_t>(i)] = (bit == 0u) ? +1.0 : -1.0;
    }

    return soft;
}

}  // namespace lunanet::gateway5
