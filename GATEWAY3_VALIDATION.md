# Gateway 3: Navigation Message Framing Validation & Build Instructions

## Overview

Gateway 3 implements the **navigation message framing layer** for LunaNet, responsible for assembling complete 12-second navigation frames according to LSIS Section 2.4. The implementation constructs frames by concatenating four subframes (SB1 through SB4) with appropriate error correction coding:

- **Subframe 1 (SB1):** Frame identification and timing metadata encoded via BCH(51,8) code
- **Subframe 2 (SB2):** Clock and ephemeris data protected by CRC-24 and rate-1/2 LDPC code
- **Subframe 3 (SB3):** Variable broadcast messages (orbit almanac, coordinate transformations) with CRC-24 and LDPC protection
- **Subframe 4 (SB4):** Network access information and parameters, encoded identically to SB3

The gateway validates the complete encoding pipeline, symbol counts, bit allocations per specification tables, and frame timing requirements. Validation also confirms proper handling of interleaving, error detection codes, and forward error correction matrix operations.

## Build Requirements

### Prerequisites

- **CMake** 3.15+ ([Download](https://cmake.org/download/))
- **C++ Compiler** (one of):
  - GCC 9+ (Linux/macOS)
  - Clang 10+ (Linux/macOS)
  - MSVC 2019+ (Windows)
- **Standard Libraries** (C++17)

### Build Instructions

```bash
# From repository root
cd Spreading-Codes

# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compile
cmake --build build --config Release

# Run Gateway 3 tests
./build/bin/test_engine config/spreading_codes_config.ini --gateway gateway3
```

## Expected Test Output

When all tests pass, you should see output like:

```
Validation scope: gateway3

[Gateway3/Frame] ✓ Load LDPC matrices
[Gateway3/Sync] ✓ Length = 68 symbols
[Gateway3/Sync] ✓ All symbols binary
[Gateway3/Sync] ✓ First byte matches 0xCC (1100 1100)
[Gateway3/Sync] ✓ Last nibble matches 0xA (1010)
[Gateway3/SB1] ✓ Length = 52 symbols
[Gateway3/SB1] ✓ Round-trip FID=0 TOI=0
[Gateway3/SB1] ✓ Round-trip FID=1 TOI=50
[Gateway3/SB1] ✓ Round-trip FID=3 TOI=99
[Gateway3/SB1] ✓ Round-trip FID=2 TOI=1
[Gateway3/SB2] ✓ Pack = 1176 bits
[Gateway3/SB2] ✓ CRC-24 verifies after append
[Gateway3/SB2] ✓ LDPC encode → 2400 symbols
[Gateway3/SB2] ✓ All encoded symbols binary
[Gateway3/SB3] ✓ Pack = 846 bits
[Gateway3/SB3] ✓ CRC-24 verifies after append
[Gateway3/SB3] ✓ LDPC encode → 1740 symbols
[Gateway3/SB4] ✓ Pack = 846 bits
[Gateway3/SB4] ✓ CRC-24 verifies after append
[Gateway3/SB4] ✓ LDPC encode → 1740 symbols
[Gateway3/Frame] ✓ Full frame = 6000 symbols
[Gateway3/Performance] ✓ Frame assembly < 100ms
[Gateway3/Frame] ✓ Sync pattern not interleaved (frame[0..67] intact)
[Gateway3/Frame] ✓ All 6000 frame symbols binary
[Gateway3/Frame] ✓ Export binary (750 bytes)
[Gateway3/Frame] ✓ Export CSV (6000 lines)
[Gateway3/Frame] ✓ Hex export = 1500 chars (750 packed bytes)
[Gateway3/Frame] ✓ Frame duration = 12 seconds
[Gateway3/SB2] ✓ Bit allocation = 1176 bits (13+9+7+1147)
[Gateway3/SB2] ✓ WN field (bits 0-12) encodes correctly
[Gateway3/SB2] ✓ ITOW field (bits 13-21) encodes correctly
[Gateway3/SB2] ✓ TOI field (bits 22-28) encodes correctly
[Gateway3/SB3] ✓ Bit allocation = 846 bits (type + payload)
[Gateway3/SB3] ✓ Type field (bits 0-3 or 0-5) encodes correctly
[Gateway3/SB4] ✓ Bit allocation = 846 bits (type + payload)

PASS: All tests passed!
```

## Validation Checklist

The following requirements are validated by the test suite:

### ✅ Frame Structure (Figure 9)

- Sync pattern: 68 symbols (0xCCA for start, ends with 0xA)
- SB1 (header): 52 symbols (BCH encoded FID + TOI)
- SB2+SB3+SB4 interleaved: 5880 symbols
- **Total: 6000 symbols**

### ✅ Bit Allocations (Tables 14, 18, 19, 20)

**SB2 (Clock & Ephemeris):** 1176 bits

- WN (Week Number): 13 bits
- ITOW (Integer Time of Week): 9 bits
- TOI (Time of Interval): 7 bits
- Payload (CED + Health + Time Conv): 1147 bits

**SB3 (Variable Data):** 846 bits

- Type field: 4 or 6 bits (configurable)
- Payload: 840-842 bits

**SB4 (Network Access):** 846 bits

- Type field: 4 or 6 bits
- Payload: 840-842 bits

### ✅ Frame Duration

- 6000 symbols @ 500 symbols/sec = **12 seconds**

### ✅ Encoding Pipeline

- SB1: BCH(51,8) encoding → 52 symbols
- SB2: Payload → CRC-24 → LDPC(1/2) → 2400 symbols
- SB3/4: Payload → CRC-24 → LDPC(1/2) → 1740 symbols each
- All with interleaving & error correction

## Generated Artifacts

After successful test run, the following files are created in system temp directory:

- `gw3_test_frame.bin` — Binary frame (750 bytes = 6000 bits packed)
- `gw3_test_frame.csv` — CSV format (6000 lines, one symbol per line)
- (Hex format printed to console: 1500 characters)

## Success Criteria (All ✅)

- [x] Frame structure matches Figure 9
- [x] Symbol counts: 68 + 52 + 5880 = 6000
- [x] Bit allocations match specification tables
- [x] Frame duration is exactly 12 seconds
- [x] All encoding pipelines functional
- [x] Frame assembly < 100ms performance
- [x] All export formats working

## Troubleshooting

### Build Fails: "CMakeLists.txt not found"

```bash
# Make sure you're in the repository root
cd Spreading-Codes
ls CMakeLists.txt  # Should exist
```

### Build Fails: "C++ compiler not found"

Install a C++17 compatible compiler:

- **Windows:** [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)
- **macOS:** `xcode-select --install`
- **Linux:** `sudo apt-get install build-essential cmake`

### Tests Fail: "LDPC matrices not found"

The tests load LDPC matrices from `Validation/annex3/csv/`. Ensure:

```bash
ls Validation/annex3/csv/004a*.csv  # Should exist
```

### Tests Fail: "Spreading code config not found"

```bash
ls config/spreading_codes_config.ini  # Should exist
```

## Next Steps

Once Gateway 3 validation passes:

1. ✅ **Gateway 3 Complete** — Commit validation proof
2. → **Gateway 4:** Baseband Signal Generation (I/Q modulators)
3. → **Gateway 5-6:** Decoding pipeline
4. → **Gateway 7-8:** Integration & documentation

---

**To complete Gateway 3:**

1. Build the project following instructions above
2. Run the test suite
3. Verify all tests pass
4. Keep test output as validation artifact
5. Commit the proof of success

Gateway 3 is production-ready once this validation completes. 🎯
