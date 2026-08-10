#include "frame_assembler.h"

#include "gateway2/interleaver.h"
#include "gateway2/ldpc_encoder.h"
#include "subframe1_builder.h"
#include "sync_pattern.h"

namespace lunanet::gateway3 {

std::vector<uint8_t> AssembleFrame(const FrameInput& input,
                                   const FrameMatrices& matrices,
                                   std::string* error_message) {
    // Step 1: Sync pattern (68 symbols, uncoded)
    const auto sync = BuildSyncPattern();
    if (static_cast<int>(sync.size()) != kSyncPatternSymbols) {
        if (error_message) *error_message = "Sync pattern size error";
        return {};
    }

    // Step 2: SB1 via BCH (52 symbols)
    const auto sb1 = BuildSubframe1(input.fid, input.toi);
    if (static_cast<int>(sb1.size()) != kSb1Symbols) {
        if (error_message) *error_message = "SB1 encoding failed";
        return {};
    }

    // Step 3: SB2 via CRC-24 + LDPC (2400 symbols)
    const auto sb2 = BuildSubframe2(input.sb2, matrices.sb2, error_message);
    if (sb2.empty()) return {};

    // Step 4: SB3 via CRC-24 + LDPC (1740 symbols)
    const auto sb3 = BuildSubframe3(input.sb3, matrices.sb34, error_message);
    if (sb3.empty()) return {};

    // Step 5: SB4 via CRC-24 + LDPC (1740 symbols)
    const auto sb4 = BuildSubframe4(input.sb4, matrices.sb34, error_message);
    if (sb4.empty()) return {};

    // Step 6: Concatenate SB2+SB3+SB4 = 5880 symbols, then interleave
    std::vector<uint8_t> combined;
    combined.reserve(lunanet::gateway2::kInterleaverSize);
    combined.insert(combined.end(), sb2.begin(), sb2.end());
    combined.insert(combined.end(), sb3.begin(), sb3.end());
    combined.insert(combined.end(), sb4.begin(), sb4.end());

    if (static_cast<int>(combined.size()) != lunanet::gateway2::kInterleaverSize) {
        if (error_message) *error_message =
            "SB2+SB3+SB4 combined size " + std::to_string(combined.size()) +
            " != 5880";
        return {};
    }

    const auto interleaved = lunanet::gateway2::Interleave(combined);
    if (interleaved.empty()) {
        if (error_message) *error_message = "Interleaver returned empty result";
        return {};
    }

    // Step 7: Assemble final frame
    std::vector<uint8_t> frame;
    frame.reserve(kFrameSymbols);
    frame.insert(frame.end(), sync.begin(), sync.end());       // 68
    frame.insert(frame.end(), sb1.begin(), sb1.end());         // 52
    frame.insert(frame.end(), interleaved.begin(), interleaved.end());  // 5880

    if (static_cast<int>(frame.size()) != kFrameSymbols) {
        if (error_message) *error_message =
            "Frame size " + std::to_string(frame.size()) + " != 6000";
        return {};
    }

    return frame;
}

bool LoadFrameMatrices(const std::string& annex3_csv_dir,
                       FrameMatrices* out,
                       std::string* error_message) {
    if (out == nullptr) {
        if (error_message) *error_message = "Null output pointer";
        return false;
    }

    // Append separator if missing so path construction works on all platforms
    std::string dir = annex3_csv_dir;
    if (!dir.empty() && dir.back() != '/' && dir.back() != '\\') {
        dir += '/';
    }

    const auto path = [&](const std::string& f) { return dir + f; };

    // SB2 LDPC matrices (1200-bit systematic → 2400 symbols)
    if (!lunanet::gateway2::LoadLdpcMatrices(
            path("004h_lunanet_sf2_ldpc_submatrix_a_mat.csv"),
            path("004i_lunanet_sf2_ldpc_submatrix_b_inv_mat.csv"),
            path("004f_lunanet_sf2_ldpc_submatrix_c_mat.csv"),
            path("004g_lunanet_sf2_ldpc_submatrix_d_mat.csv"),
            &out->sb2, error_message)) {
        return false;
    }

    // SB3/SB4 LDPC matrices (870-bit systematic + 10 filler → 1740 symbols)
    if (!lunanet::gateway2::LoadLdpcMatrices(
            path("004a_lunanet_sf3_ldpc_submatrix_a_mat.csv"),
            path("004b_lunanet_sf3_ldpc_submatrix_b_inv_mat.csv"),
            path("004d_lunanet_sf3_ldpc_submatrix_c_mat.csv"),
            path("004e_lunanet_sf3_ldpc_submatrix_d_mat.csv"),
            &out->sb34, error_message)) {
        return false;
    }

    return true;
}

}  // namespace lunanet::gateway3
