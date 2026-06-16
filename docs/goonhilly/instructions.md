# Goonhilly Mid-Project Workshop: Step-by-Step Instructions

This guide provides the exact commands needed to build the LSIS-AFS command-line tool and execute the validation scenarios required for the workshop interoperability checks (Levels 0 through 2).

> [!NOTE]
> Our team is participating as **Encoder Only** for this workshop. We will supply the reference outputs, and other teams will cross-decode our signals.

## Prerequisites

- CMake (3.15 or newer)
- A C++17 compatible compiler (GCC, Clang, or MSVC)
- Python 3 (for running the test engine or viewer scripts, though the core CLI is pure C++)

## Step 1: Building the CLI

The project uses CMake to configure and build the static libraries and the final `goon` executable.

```bash
# Navigate to the project root
cd "Spreading Codes"

# Create the build directory and configure the project
mkdir -p build && cd build
cmake ..

# Build the CLI executable (and all dependent gateway libraries)
cmake --build . --target goon

# Install it globally so it can be used anywhere
sudo cmake --install . --prefix /usr/local
```

After building and installing, you can use the `goon` command from anywhere.

## Step 2: Level 0 - Spreading Code Generation

For Level 0, we must generate the raw hex outputs of the spreading codes for all 210 PRNs. This validates the LFSR configurations, Weil code indices, and Legendre sequences.

```bash
# Run from anywhere
goon generate-codes --output codes.txt
```

**Expected Result:** A file named `codes.txt` containing 210 lines, where each line contains exactly 512 hexadecimal characters. This file should be submitted to the CI pipeline or shared with other teams for static analysis.

## Step 3: Level 1 - Frame Encoding

Level 1 validates the Forward Error Correction (FEC) and framing logic (BCH, CRC-24Q, LDPC, and block interleaving). We encode a canonical test case to produce a 6000-byte raw frame.

```bash
# Run from anywhere
goon encode --format frame \
  --prn 1 \
  --fid 0 \
  --toi 42 \
  --wn 100 \
  --itow 250 \
  --output frame.bin
```

**Expected Result:** A file named `frame.bin` of exactly 6000 bytes. Each byte represents one symbol (0x00 or 0x01). The first 68 bytes will perfectly match the LSIS-AFS sync pattern (`CC63F74536F49E04A`).

## Step 4: Level 2 - I/Q Signal Generation

Level 2 is the final encoder output: an interleaved float32 I/Q baseband signal representing exactly 12 seconds of transmission.

The workshop contract requires a sample rate of 1.023 MHz (the AFS-I chip rate).

```bash
# Run from anywhere
goon encode --format iq32 \
  --prn 1 \
  --fid 0 \
  --toi 42 \
  --wn 100 \
  --itow 250 \
  --rate 1023000 \
  --output signal.iq32
```

**Expected Result:** A large binary file (`signal.iq32`) containing interleaved 32-bit floating-point values (I, Q, I, Q...).

- **Sample Rate:** 1.023 MHz
- **Duration:** 12 seconds
- **File Size:** Exactly 98,208,000 bytes (~93 MB)

Share `signal.iq32` with the teams implementing the Gateway 5 (Decoder) pipeline for cross-decode verification.

---

## Troubleshooting

- **Sample Rate Errors:** If you try to run the `iq32` command with an arbitrary sample rate, the CLI will reject it. The sample rate *must* be a positive integer multiple of 1,023,000 Hz to ensure mathematically perfect zero-order hold alignment without phase drift.
- **Linker Warnings:** When building, you may see a warning about a duplicate `liblunanet_gateway2.a`. This is a benign CMake transitive linking warning and can be safely ignored.
