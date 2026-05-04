# LunaNet AFS Spreading Codes & Signal Encoder

Implementation of the LunaNet Augmented Forward Signal (AFS) spreading code generators and forward error correction (FEC) encoders, built to the **LSIS-AFS Volume A** specification.

**735/735 tests passing** across 10 validation suites.

---

## Features

### Gateway 1 — Spreading Code Generation
- **Gold Code Generator** — 11-stage Fibonacci LFSR pair (G1/G2), 2046-chip sequences for 210 PRNs. Validated against Annex 3 reference vectors.
- **Weil Primary Code Generator** — Legendre-sequence-based Weil construction over GF(10223) with 7-bit insertion expansion to 10230 chips.
- **Weil Tertiary Code Generator** — Weil construction over GF(1499) producing 1500-chip tertiary codes.
- **Tiered AFS-Q Constructor** — Three-tier modular code: primary ⊕ secondary ⊕ tertiary per LSIS §2.3.
- **Table 11 Node Assignments** — 12 LNSP interim test node configurations with secondary code cycling (S0→S3).

### Gateway 2 — Forward Error Correction
- **BCH(51,8) Encoder/Decoder** — 8-stage LFSR with generator polynomial 763₈. Includes soft-decision decoder via exhaustive 256-codeword correlation.
- **CRC-24Q** — Polynomial `0x864CFB` (GPS CNAV compatible). Compute, append, and verify.
- **LDPC Rate-1/2 Encoder** — Dense GF(2) submatrix encoding (A, B⁻¹, C, D) for SB2 (1200→2400) and SB3/SB4 (870→1740). Matrices loaded from Annex 1 CSV files.
- **Block Interleaver** — 60×98 write-row/read-column interleaver for SB2+SB3+SB4 concatenation (5880 symbols).

### Cross-Language Bridge
- **C API** — `extern "C"` DLL shim (`c_api.h`) exporting all generators and FEC functions for FFI access.
- **Python Bridge** — Zero-dependency ctypes wrapper (`lunanet.py`) with auto-DLL discovery and type-safe prototypes.
- **I/Q Signal Generator** — BPSK(1) baseband generator outputting float32 binary and CSV. Mapping: `0 → +1.0`, `1 → -1.0`.

---

## Project Structure

```
Spreading-Codes/
├── CMakeLists.txt                  # Build system (C++17, MSVC/GCC/Clang)
├── config/
│   └── spreading_codes_config.ini  # Runtime configuration
├── codes/
│   ├── spreading_codes.h/.cpp      # Public C++ API
│   ├── c_api.h/.cpp                # C-linkage DLL exports
│   ├── test_engine.cpp             # Orchestration harness (10 suites)
│   ├── gateway1/                   # Spreading code generators
│   │   ├── gold_code_generator.*   #   Gold code (2046 chips)
│   │   ├── weil_code_generator.*   #   Weil primary/tertiary
│   │   ├── tiered_code_generator.* #   AFS-Q three-tier construction
│   │   ├── spreading_config.*      #   Table loading & configuration
│   │   └── gui/                    #   Tkinter report viewer
│   ├── gateway2/                   # FEC encoding
│   │   ├── bch_codec.*             #   BCH(51,8) encoder + soft decoder
│   │   ├── crc24.*                 #   CRC-24Q
│   │   ├── ldpc_encoder.*          #   LDPC rate-1/2 encoder
│   │   └── interleaver.*           #   60×98 block interleaver
│   ├── testing/                    # Test framework
│   │   ├── test_reporter.*         #   Markdown + JUnit XML output
│   │   ├── test_validators.*       #   Validation primitives
│   │   └── test_annex3_loader.*    #   Annex 3 hex reference parser
│   └── python/                     # Python bridge layer
│       ├── lunanet.py              #   ctypes wrapper
│       ├── iq_generator.py         #   BPSK(1) I/Q signal generation
│       └── test_bridge.py          #   Bridge smoke test
├── docs/                           # Spec tables, FAQ, requirements
├── Validation/
│   ├── annex3/                     # Reference vectors (txt + csv)
│   └── reports/                    # Timestamped test reports
└── CHANGELOG.md
```

---

## Build

**Requirements:** CMake 3.16+, C++17 compiler (MSVC 2019+, GCC 9+, Clang 10+).

```bash
cmake -B build -S .
cmake --build build --config Release
```

Outputs:
- `build/bin/Release/lunanet_spreading_codes.dll` — shared library with C API
- `build/bin/Release/test_engine.exe` — validation harness

---

## Validation

Run the full test suite (735 tests across 10 suites):

```bash
./build/bin/Release/test_engine.exe config/spreading_codes_config.ini
```

```
Smoke reference validation:            PASS (12/12)
Annex3/Gold reference validation:      PASS (210/210)
Annex3/Weil Primary reference validation: PASS (210/210)
Annex3/Weil Tertiary reference validation: PASS (210/210)
Table11/Assignments reference validation: PASS (60/60)
Gateway2/BCH reference validation:     PASS (10/10)
Gateway2/CRC24 reference validation:   PASS (4/4)
Gateway2/Interleaver reference validation: PASS (4/4)
Gateway2/LDPC reference validation:    PASS (12/12)
Performance reference validation:      PASS (3/3)

Gateway 1 modular foundation checks passed. (735/735 tests)
```

Reports are written to `Validation/reports/YYYY-MM-DD/HH-MM-SS.{md,xml}`.

### Report Viewer GUI

```bash
python codes/gateway1/gui/report_viewer.py
```

Dark-themed Tkinter viewer with color-coded pass/fail rows, suite/status filtering, and auto-discovery of timestamped reports.

---

## Python Usage

```python
from codes.python.lunanet import LunaNet

ln = LunaNet("config/spreading_codes_config.ini")

# Generate spreading codes
gold = ln.generate_gold(1)           # 2046-chip Gold code
weil_p = ln.generate_weil_primary(1) # 10230-chip Weil primary
weil_t = ln.generate_weil_tertiary(1)# 1500-chip Weil tertiary
afs_q = ln.generate_afs_q(1)        # Tiered AFS-Q code

# FEC encoding
bch = ln.bch_encode(0x0A5)          # BCH(51,8) → 52 symbols
crc = ln.crc24([1, 0, 1, 1])       # CRC-24Q → 24-bit value
```

### I/Q Signal Generation

```bash
python codes/python/iq_generator.py \
    --config config/spreading_codes_config.ini \
    --prn 1 \
    --output Validation/iq_output \
    --format both
```

Outputs `prn001_afs_i.bin`, `prn001_afs_q.bin`, `prn001_iq_interleaved.bin`, and `prn001_iq.csv`.

---

## Performance

Full PRN generation pipeline (Gold + Weil Primary + Weil Tertiary + AFS-Q) completes in **< 0.5 ms per PRN**, well under the SC-1.7 requirement of < 1 second.

| Operation | Time |
|---|---|
| Gold code (2046 chips) | ~0.05 ms |
| Weil Primary (10230 chips) | ~0.1 ms |
| AFS-Q tiered (1 epoch) | ~0.3 ms |
| LDPC SB2 encode (1200→2400) | < 100 ms |

---

## Specification Compliance

| Requirement | Status | Tests |
|---|---|---|
| SC-1.1 Gold code generation | ✅ | 210/210 |
| SC-1.2 Weil primary generation | ✅ | 210/210 |
| SC-1.3 Weil tertiary generation | ✅ | 210/210 |
| SC-1.6 Table 11 node assignments | ✅ | 60/60 |
| SC-1.7 Performance (< 1s/PRN) | ✅ | 3/3 |
| FEC-2.1 BCH(51,8) encoder | ✅ | 10/10 |
| FEC-2.3 LDPC rate-1/2 encoder | ✅ | 12/12 |
| FEC-2.5 CRC-24Q | ✅ | 4/4 |
| FEC-2.7 Block interleaver | ✅ | 4/4 |
| SG-4.3 BPSK(1) I/Q mapping | ✅ | verified |

---

## Roadmap

- [x] Gateway 1 — Spreading code generation (210 PRNs, all types)
- [x] Gateway 1C — Table 11 interim code validation
- [x] Gateway 2 — FEC encoding (BCH, CRC-24, LDPC, interleaver)
- [x] Python bridge layer (ctypes + C API)
- [x] BPSK(1) I/Q signal generation
- [ ] Gateway 3 — Frame assembly (sync + SB1 + interleaved SB2-4)
- [ ] End-to-end pipeline (data → FEC → frame → I/Q)

---

## License

Part of the KURE-x-Tech LunaNet competition entry.