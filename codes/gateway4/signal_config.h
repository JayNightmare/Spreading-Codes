#pragma once
#ifndef LUNANET_GATEWAY4_SIGNAL_CONFIG_H
#define LUNANET_GATEWAY4_SIGNAL_CONFIG_H

#include <cstdint>

namespace lunanet::gateway4 {

// ── Chip and symbol rates (LSIS-AFS Table 7) ──────────────────────────────
// AFS-I (data, in-phase):  BPSK(1) at 1.023 Mchip/s, 500 symbols/s.
// AFS-Q (pilot, quadrature): BPSK(5) at 5.115 Mchip/s, no data.
constexpr int kAfsIChipRateHz = 1023000;   // 1.023 Mchip/s
constexpr int kAfsQChipRateHz = 5115000;   // 5.115 Mchip/s
constexpr int kSymbolRateHz   = 500;       // AFS-I data symbol rate

// Q runs at exactly 5× the I chip rate (5.115 / 1.023). Used to keep both
// channels time-aligned when resampling to a common sample rate.
constexpr int kQOverIChipRatio = kAfsQChipRateHz / kAfsIChipRateHz;  // 5

// ── Code lengths (LSIS-AFS Table 9) ───────────────────────────────────────
constexpr int kAfsIPrimaryChips = 2046;    // Gold primary, 2 ms epoch
constexpr int kAfsQPrimaryChips = 10230;   // Weil primary, 2 ms epoch
constexpr int kAfsQSecondaryChips = 4;     // Secondary code, 8 ms period
constexpr int kAfsQTertiaryChips = 1500;   // Weil tertiary, 12 s period

// Full AFS-Q tiered code length = primary × secondary × tertiary. This is one
// tertiary-code period, which equals exactly one 12 s frame (LSIS-220).
constexpr long kAfsQTieredChips =
    static_cast<long>(kAfsQPrimaryChips) * kAfsQSecondaryChips * kAfsQTertiaryChips;  // 61,380,000

// ── Code/data coherency (LSIS-160) ────────────────────────────────────────
// "Number of primary code chips per data symbol = primary chip-rate / symbol-rate".
// For AFS-I this is exactly one Gold code epoch per data symbol.
constexpr int kAfsIChipsPerSymbol = kAfsIChipRateHz / kSymbolRateHz;  // 2046
static_assert(kAfsIChipsPerSymbol == kAfsIPrimaryChips,
              "AFS-I data symbol must span exactly one primary code epoch");

// ── Frame timing (LSIS-310 / Gateway 3) ───────────────────────────────────
constexpr int kFrameSymbols     = 6000;    // 12 s × 500 symbols/s
constexpr int kFrameDurationSec = 12;

// ── BPSK logic-to-signal level assignment (LSIS-150, Table 8) ──────────────
constexpr float kBpskLevelLogic0 = 1.0f;
constexpr float kBpskLevelLogic1 = -1.0f;

// Default I/Q sample rate: the AFS-Q chip rate, giving one sample per Q chip
// and five samples per I chip. Callers may request any positive integer
// multiple of the AFS-Q chip rate (see IqConfig).
constexpr int kDefaultSampleRateHz = kAfsQChipRateHz;

}  // namespace lunanet::gateway4

#endif  // LUNANET_GATEWAY4_SIGNAL_CONFIG_H
