#include "gateway5/frame_synchronizer.h"
#include "gateway3/sync_pattern.h"
#include "gateway4/bpsk_modulator.h"

namespace lunanet::gateway5 {
    std::vector<float> BuildSyncReferenceSymbols() {
        //uint8_t = unsigned integer 8 bits Used for store num between 0-255
        const std::vector<uint8_t> sync_bits = lunanet::gateway3::BuildSyncPattern();
        //Use the BPSK modulator to convert the sync bits into BPSK signal levels
        //0→+1.0
        //1→-1.0
        return lunanet::gateway4::BpskModulate(sync_bits);
    }

// Stage 1 frame synchronization implementation will be added here.

}  // namespace lunanet::gateway5