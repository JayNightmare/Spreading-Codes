#include "gateway5/deinterleaver.h"

namespace lunanet::gateway5 {

std::vector<double> DeinterleaveSoft(const std::vector<double>& interleaved_soft) {
    if (interleaved_soft.size() != static_cast<std::size_t>(kStage4InterleavedSymbols)) {
        return {};
    }

    std::vector<double> deinterleaved(kStage4InterleavedSymbols, 0.0);

    // Intentional duplication of gateway2::Deinterleave mapping, but on
    // signed soft doubles instead of hard bits.
    // Exact inverse of gateway2::Interleave:
    // write column-wise from interleaved stream, then read row-wise.
    for (int row = 0; row < kStage4Rows; ++row) {
        for (int col = 0; col < kStage4Cols; ++col) {
            deinterleaved[row * kStage4Cols + col] =
                interleaved_soft[col * kStage4Rows + row];
        }
    }

    return deinterleaved;
}

Stage4Subframes SplitDeinterleavedSubframes(const std::vector<double>& deinterleaved_soft) {
    if (deinterleaved_soft.size() != static_cast<std::size_t>(kStage4InterleavedSymbols)) {
        return {};
    }

    Stage4Subframes out;
    out.sb2.assign(deinterleaved_soft.begin(),
                   deinterleaved_soft.begin() + kStage4Sb2Symbols);
    out.sb3.assign(deinterleaved_soft.begin() + kStage4Sb2Symbols,
                   deinterleaved_soft.begin() + kStage4Sb2Symbols + kStage4Sb3Symbols);
    out.sb4.assign(deinterleaved_soft.begin() + kStage4Sb2Symbols + kStage4Sb3Symbols,
                   deinterleaved_soft.end());

    return out;
}

Stage4Subframes DeinterleaveToSubframes(const std::vector<double>& interleaved_soft) {
    const auto deinterleaved = DeinterleaveSoft(interleaved_soft);
    if (deinterleaved.empty()) {
        return {};
    }
    return SplitDeinterleavedSubframes(deinterleaved);
}

}  // namespace lunanet::gateway5
