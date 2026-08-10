#include "bch_codec.h"

#include <algorithm>
#include <cmath>

namespace lunanet::gateway2 {

namespace {

/**
 * Runs the 8-stage LFSR using the LSIS Figure 7/8 Fibonacci topology.
 * Feedback taps are stages {1,4,5,6,7,8} (bits {0,3,4,5,6,7} => mask 0xF9).
 *
 * The LFSR output is the MSB of the register at each step.
 */
std::array<uint8_t, kBchLfsrOutputSymbols> LfsrEncode(uint8_t initial_state) {
    std::array<uint8_t, kBchLfsrOutputSymbols> output{};
    uint8_t reg = initial_state;

    for (int i = 0; i < kBchLfsrOutputSymbols; ++i) {
        const uint8_t output_bit = (reg >> 7) & 1;
        output[i] = output_bit;

        // Fibonacci feedback: XOR configured taps from current register,
        // then shift left and inject feedback at stage 1 (bit0).
        uint8_t feedback = 0;
        for (int bit = 0; bit < kBchLfsrStages; ++bit) {
            if ((kBchFeedbackTapMask & static_cast<uint8_t>(1u << bit)) != 0u) {
                feedback ^= static_cast<uint8_t>((reg >> bit) & 0x1u);
            }
        }

        reg = static_cast<uint8_t>((reg << 1) & 0xFFu);
        reg = static_cast<uint8_t>(reg | feedback);
    }

    return output;
}

}  // namespace

std::vector<uint8_t> BchEncode(uint16_t sb1_data) {
    // Mask to 9 bits
    sb1_data &= 0x1FF;

    // MSB is bit 8 of the 9-bit field (FID high bit)
    const uint8_t msb = static_cast<uint8_t>((sb1_data >> 8) & 1);

    // 8 LSBs loaded into LFSR: bit 1 into stage 8, bit 8 into stage 1
    // The spec says "bit 1 first" into the register, so bit 1 goes to the
    // highest stage (MSB). The hex representation in the CSV has the 8 LSBs
    // as-is, with bit 8 being the MSB of the byte.
    const uint8_t lfsr_init = static_cast<uint8_t>(sb1_data & 0xFF);

    const auto lfsr_output = LfsrEncode(lfsr_init);

    std::vector<uint8_t> result;
    result.reserve(kBchEncodedSymbols);

    // Prepend the MSB
    result.push_back(msb);

    // Append 51 symbols, each XOR'd with the MSB
    for (int i = 0; i < kBchLfsrOutputSymbols; ++i) {
        result.push_back(static_cast<uint8_t>(lfsr_output[i] ^ msb));
    }

    return result;
}

std::array<std::array<uint8_t, kBchLfsrOutputSymbols>, 256> BchGenerateCodebook() {
    std::array<std::array<uint8_t, kBchLfsrOutputSymbols>, 256> codebook{};

    for (int data = 0; data < 256; ++data) {
        codebook[data] = LfsrEncode(static_cast<uint8_t>(data));
    }

    return codebook;
}

int BchDecodeSoft(const std::vector<double>& soft_symbols) {
    if (soft_symbols.size() != kBchEncodedSymbols) {
        return -1;
    }

    // Per the LSIS spec decoder description:
    //   1. Strip the first symbol (MSB indicator)
    //   2. Correlate the remaining 51 symbols against all 256 MSB=0
    //      codeword hypotheses
    //   4. Max |correlation| → 8 LSBs; sign → MSB

    static const auto codebook = BchGenerateCodebook();

    // Correlate raw inner symbols (indices 1..51) against the MSB=0 codebook.

    double best_abs = -1.0;
    double best_raw = 0.0;
    int best_data = 0;

    for (int data = 0; data < 256; ++data) {
        double correlation = 0.0;
        for (int i = 0; i < kBchLfsrOutputSymbols; ++i) {
            // soft_symbols[i+1] because index 0 is the prepended MSB
            const double soft = soft_symbols[i + 1];
            const double magnitude = std::abs(soft);
            // Sign agreement: soft > 0 means received bit=0
            const bool received_zero = (soft >= 0.0);
            const bool codeword_zero = (codebook[data][i] == 0);

            if (received_zero == codeword_zero) {
                correlation += magnitude;
            } else {
                correlation -= magnitude;
            }
        }

        const double abs_corr = std::abs(correlation);
        if (abs_corr > best_abs) {
            best_abs = abs_corr;
            best_raw = correlation;
            best_data = data;
        }
    }

    // MSB decision per spec: positive correlation => 0, otherwise 1.
    // Deliberate tie-break: correlation == 0.0 resolves to MSB=1, matching
    // gateway5::DecodeSb1BchSoft's identical rule.
    const uint8_t decoded_msb = (best_raw > 0.0) ? 0 : 1;

    // Reconstruct the 9-bit SB1 value
    return (decoded_msb << 8) | best_data;
}

}  // namespace lunanet::gateway2
