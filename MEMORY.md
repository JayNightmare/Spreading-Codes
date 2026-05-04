# Project Memory

## Current State

- **Validation**: All PRN codes (Gold, Weil Primary, Weil Tertiary) are fully passing 210/210 compliance with Annex3 validation vectors. Table 11 interim code assignments (12 LNSP nodes) validated with 60/60 tests covering secondary code cycling, PRN identity, and tiered AFS-Q construction.
- **Test Framework**: Modular test engine (`codes/testing/`) with structured reporting - outputs console summary, markdown report, and JUnit XML for CI integration. 735 tests across 10 suites (Smoke, Annex3/Gold, Annex3/Weil Primary, Annex3/Weil Tertiary, Table11/Assignments, Gateway2/BCH, Gateway2/CRC24, Gateway2/Interleaver, Gateway2/LDPC, Performance). Reports auto-generated to `Validation/reports/YYYY-MM-DD/HH-MM-SS.{md,xml}`.
- **Gateway 2 (FEC Encoding)**: Complete. BCH(51,8) encoder/decoder, CRC-24Q, LDPC rate-1/2 encoder (SB2 + SB3/SB4), and 60×98 block interleaver all implemented and tested.
- **Python Bridge**: Zero-dependency ctypes wrapper (`codes/python/lunanet.py`) over C-linkage DLL API (`codes/c_api.h`). Exposes all code generators, BCH encoding, and CRC-24 to Python. Smoke-tested.
- **I/Q Generation**: BPSK(1) baseband signal generator (`codes/python/iq_generator.py`) outputs binary float32 and CSV I/Q files. Mapping: 0→+1.0, 1→-1.0.
- **Report Viewer**: Tkinter GUI (`codes/gateway1/gui/report_viewer.py`) with dark theme, color-coded pass/fail table rows, suite/status filtering, and auto-discovery of timestamped reports.
- **Performance**: Legendre sequence caching eliminates redundant computation in Weil generators. Full PRN generation (Gold + Weil Primary + Weil Tertiary + AFS-Q) completes in < 0.5 ms per PRN - well under the SC-1.7 requirement of < 1 second.
- **Gold Code Generator**: Fixed the `lunanet::gateway1` Gold code generators to match the Annex3 reference specification. The $G_2$ delay is applied by computationally advancing the sequence by $2047 - D_k$ relative to $G_1$, using a standard left-shifting Fibonacci LFSR algorithm.

## Architecture

- `codes/testing/test_reporter.h/.cpp` - TestReporter with pass/fail/skip tracking, markdown + JUnit XML output.
- `codes/testing/test_validators.h/.cpp` - Reusable validation primitives (Annex3 suite, length, bounds, equality, Table11).
- `codes/testing/test_annex3_loader.h/.cpp` - Annex3 hex reference file parser.
- `codes/test_engine.cpp` - Orchestration harness with 9 test suites.
- `codes/gateway1/weil_code_generator.cpp` - Thread-safe `LegendreCache` (module-level static) for Weil generators.
- `codes/gateway1/gui/report_viewer.py` - Tkinter report viewer GUI (dark theme, summary + detail tables).
- `codes/gateway1/gui/report_parser.py` - JUnit XML / markdown parser (no GUI dependency).
- `codes/gateway2/bch_codec.h/.cpp` - BCH(51,8) encoder with LFSR + soft-decision decoder.
- `codes/gateway2/crc24.h/.cpp` - CRC-24Q compute/append/verify (polynomial 0x864CFB).
- `codes/gateway2/interleaver.h/.cpp` - 60×98 block interleaver/deinterleaver.
- `codes/gateway2/ldpc_encoder.h/.cpp` - LDPC rate-1/2 encoder with dense GF(2) matrix ops and CSV loader.
- `codes/c_api.h/.cpp` - C-linkage DLL shim for ctypes/FFI access.
- `codes/python/lunanet.py` - Zero-dependency Python wrapper over the C API.
- `codes/python/iq_generator.py` - BPSK(1) I/Q signal generator (float32 binary + CSV export).

## Active Tasks (Completed)

- Exhaustive validation of the Gold code LFSR configuration.
- Resolution of Gold Code reference validation failures (210/210 passing now).
- Complete synchronization of C++ output with Annex3 documentation for `006_GoldCode2046hex210prns.txt`.
- Modularized test engine into `codes/testing/` with split validation, reporting, and harness layers.
- Added Legendre sequence caching in Weil generators - eliminates ~420 redundant computations per batch.
- Performance benchmark suite validating SC-1.7 (< 1 second per PRN).
- Tkinter report viewer GUI with dark theme, color-coded results, suite/status filtering.
- Timestamped report output to `Validation/reports/YYYY-MM-DD/HH-MM-SS.*`.

- Table 11 interim code assignment validation (SC-1.6) — 60/60 tests covering all 12 LNSP nodes.
- BCH(51,8) encoder/decoder (FEC-2.1/2.2) — 10/10 tests, round-trip verified.
- CRC-24Q compute/verify (FEC-2.5/2.6) — 4/4 tests.
- Block interleaver 60×98 (FEC-2.7) — 4/4 tests.
- LDPC rate-1/2 encoder (FEC-2.3) — SB2 (1200→2400) and SB3/SB4 (870→1740), 12/12 tests.
- Python ctypes bridge — C API DLL shim + Python wrapper, smoke-tested.
- BPSK(1) I/Q generator — binary float32 and CSV export, verified ±1.0 mapping.

## Next Steps

- Frame assembly (Gateway 3) — sync pattern + SB1 + interleaved(SB2+SB3+SB4) = 6000 symbols.
- Full end-to-end encode pipeline: data → FEC → frame → I/Q.
