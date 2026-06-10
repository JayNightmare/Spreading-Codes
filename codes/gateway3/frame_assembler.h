#pragma once
#ifndef LUNANET_GATEWAY3_FRAME_ASSEMBLER_H
#define LUNANET_GATEWAY3_FRAME_ASSEMBLER_H

#include <cstdint>
#include <string>
#include <vector>

#include "gateway2/ldpc_encoder.h"
#include "subframe2_builder.h"
#include "subframe3_builder.h"
#include "subframe4_builder.h"

namespace lunanet::gateway3 {

// 500 symbols/sec × 12 seconds = 6000 symbols.
// Layout: [Sync:68][SB1:52][Interleaved(SB2+SB3+SB4):5880]
constexpr int kFrameSymbols = 6000;

// Pre-loaded LDPC matrix sets for both subframe types.
struct FrameMatrices {
    lunanet::gateway2::LdpcMatrices sb2;   // For SB2 (1200-bit input → 2400 symbols)
    lunanet::gateway2::LdpcMatrices sb34;  // For SB3 and SB4 (870-bit input → 1740 symbols)
};

// All inputs for one complete navigation frame.
struct FrameInput {
    uint8_t fid;       // Frame ID (2 bits, 0–3)
    uint8_t toi;       // Time of Interval (7 bits, 0–99)
    Subframe2Data sb2;
    Subframe3Data sb3;
    Subframe4Data sb4;
};

// Assembles a complete 6000-symbol navigation frame (FID=0 layout):
//   1. Sync pattern (68 sym, uncoded)
//   2. SB1 via BCH(51,8) (52 sym)
//   3. SB2 via CRC-24 + LDPC (2400 sym)
//   4. SB3 via CRC-24 + LDPC (1740 sym)
//   5. SB4 via CRC-24 + LDPC (1740 sym)
//   6. Interleave SB2+SB3+SB4 (60×98 block)
//   7. Frame = sync + SB1 + interleaved(SB2+SB3+SB4)
//
// Returns empty on error; sets *error_message if non-null.
std::vector<uint8_t> AssembleFrame(const FrameInput& input,
                                   const FrameMatrices& matrices,
                                   std::string* error_message = nullptr);

// Loads the four LDPC submatrices (A, B^-1, C, D) for both SB2 and SB3/SB4
// from Validation/annex3/csv/ using the committed dense-matrix CSV files.
//
// @param annex3_csv_dir  Path to the directory containing the 004*.csv files.
// @param out             Output matrix sets.
// @param error_message   Optional error message on failure.
bool LoadFrameMatrices(const std::string& annex3_csv_dir,
                       FrameMatrices* out,
                       std::string* error_message = nullptr);

}  // namespace lunanet::gateway3

#endif  // LUNANET_GATEWAY3_FRAME_ASSEMBLER_H
