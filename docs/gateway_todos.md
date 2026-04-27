# Gateway Todo Tracker

Last updated: 2026-04-27 (Session 3 - Build Recovery & Compilation Fixed)

## Gateway 1 - Spreading Code Module (LSIS-AFS)

### Completed

- ✅ C++ core spreading code engine scaffold is in place (`codes/spreading_codes.cpp`, `codes/spreading_codes.h`).
- ✅ Gold code data tables for PRN 1-210 are embedded (`PRN_INIT_G2`, `PRN_G2_DELAY`).
- ✅ Gold code generation is implemented for valid PRN range 1-210 (`generate_gold_code`).
- ✅ Weil parameter tables from Appendix D (primary) and E (tertiary) are now added as C++ structures and arrays.
- ✅ Primary and tertiary Weil sequence generation functions are implemented (`generate_weil_primary`, `generate_weil_tertiary`).
- ✅ AFS-I and AFS-Q generation entry points exist (`generate_afs_i`, `generate_afs_q`).
- ✅ Batch generator API for all 210 PRNs added (`generate_all_spreading_codes()`).
- ✅ **Test executable builds and runs successfully** (outputs all 210 PRN codes without errors).
- ✅ Build system is configured with CMake for shared library + test executable (`CMakeLists.txt`).
- ✅ Spec parameter CSV reference tables are available under `docs/spec_tables/`.
- ✅ Secondary code mappings corrected per Table 10 (S1=[0,1,1,1], S3=[1,1,0,1]).
- ✅ Type mismatch issues resolved: all function signatures and test code now use consistent `std::vector<uint8_t>` types.
- ✅ WeilTertiaryParams array initialization fixed (removed excess initializers).

### Backlog

**Spec Alignment Issues** (Priority: HIGH - Critical for correct signal generation)

- **Gold code polynomial constants**: Current implementation initializes LFSR with 11-bit register length but tap feedback masks may not align with spec. Header comments reference 11-bit polynomials (`g1(x) = x^11 + x^2 + 1`, `g2(x) = x^11 + x^8 + x^5 + x^2 + 1`), need to validate feedback mask hex values against Appendix C. **Current output**: PRN 1 first 20 chips are `1 1 0 1 0 1 1 1 0 1 0 0 1 1 1 1 0 1 0 0` (needs spot-check against reference `0x5D6430`).

- **Weil code implementation incomplete**: Structures and arrays now defined (Appendix D/E), but generator functions still only compute Legendre sequences. Need to:
     - Implement W(t;k) = L(t) XOR L((t+k) mod prime) with PRN-specific k from Appendix D/E tables.
     - Add insertion_index_p handling for primary codes (expansion block placement).
     - Validate tertiary code construction with appended zero.

- **AFS-Q assembly is concatenation placeholder**: Current generates 11,730 chips by simple concatenation. Spec requires modulo-2 XOR interleaving of primary, expansion, secondary, and tertiary components per technical guide. **Current output**: 11,730 chips (wrong structure, needs proper XOR assembly).

**Architecture Enhancements** (Priority: Medium - Needed for production readiness)

- Replace hard-coded signal/code constants with config-driven ingestion (guide recommends external config parsing from JSON/CSV).
- Build Python orchestration/API bridge layer called out by the implementation guide (Section 3: Module Interface and API Definition).
- Add VOLK/FFTW integration for Stage 2 accumulation and performance path (currently no vectorization).
- Frame/sync/time-system pipeline dependencies from spec tables (Table 12, Table 14, time system parameters) need integration.

### Required Todos (Gate Exit Criteria)

**Phase 1A: Runnable Test Framework** ✅ **COMPLETE**

- ✅ Executable test harness that generates all 210 PRN codes and validates basic structure.
- ✅ Batch generation API returns map<PRN, (AFS-I, AFS-Q) pair>.
- ✅ Build compiles cleanly on macOS with CMake + Clang/C++17.

**Phase 1B: Spec Compliance Validation** (🔴 BLOCKED - Must resolve before I/Q file generation)

- Validate Gold code polynomial constants and feedback masks against Appendix C. Current test shows first 20 bits of PRN 1; need to compare against known reference (user provided `0x5D6430` for first 24 chips). **Recommendation:** Create spot-check test file with hex reference vectors from spec appendices.

- Validate secondary code mappings against Table 10 (now correct: S1=[0,1,1,1], S3=[1,1,0,1]).

- Implement proper Weil code generation:
     - Update `generate_weil_primary(prn)` to use `WEIL_PRIMARY_PARAMS[prn-1].weil_index_k` and apply XOR shift.
     - Update `generate_weil_tertiary(prn)` to use `WEIL_TERTIARY_PARAMS[prn-1].weil_index_k` and append zero.
     - Add expansion block insertion at `insertion_index_p` for primary codes.

- Rewrite `generate_afs_q()` to use modulo-2 XOR instead of concatenation:
     - Interleave primary (10,223 chips), secondary (repeated 4x), expansion (7 chips), tertiary (1,500 chips).
     - Final length should be spec-compliant (currently 11,730 from concat; XOR interleaving may differ).

- Add unit tests to verify:
     - PRN bounds (1-210) and error handling for invalid PRN.
     - Deterministic code repeatability (same PRN always produces identical output).
     - Sequence lengths per spec.
     - Known-reference spot checks against specification vectors (using hex reference tables).

**Phase 1C: Binary I/Q File Generation** (Gate exit requirement)

- Implement baseline binary I/Q file generation path for BPSK(1) validation output (unmodulated PN sequence).
- Support output formats from `file_format_headers.csv` (e.g., IQSG, IQS2, LSIS headers).
- Implement BER/SNR validation reporting framework; gate requires **BER = 0.0 for unmodulated PN baseline**.

**Phase 1D: Signal Validation** (Gate validation)

- Add spectrum validation workflow (GQRX/SDR#) and store evidence artifacts.
- Confirm 68-symbol sync word (Table 12) integration and 12-second frame handling logic.
- Add CI checks that build and run tests on supported platforms (Linux/macOS/Windows).

### Notes

**Build Status (Session 3 - Current)**

- ✅ **BUILD RECOVERY COMPLETE**: Rolled back incomplete patch, fixed type mismatches (uint8_t vs int), corrected WeilTertiaryParams initializers.
- ✅ **COMPILATION SUCCESSFUL**: All files compile cleanly. `liblunanet_spreading_codes.dylib` and `test_engine` executable generated.
- ✅ **TEST EXECUTION SUCCESSFUL**: `./build/bin/test_engine` runs without errors, generates codes for all 210 PRNs.
- ⚠️ **SPEC ALIGNMENT PENDING**: Algorithm implementations run but output still needs validation against reference hex vectors (e.g., user reference `0x5D6430` for PRN 1).

**Session 3 Actions Completed**

1. Fixed WeilTertiaryParams array initialization (removed excess `, 0` from all 210 entries).
2. Updated all function signatures to use `std::vector<uint8_t>` consistently across header/implementation/test.
3. Converted test_engine.cpp variable types and casts for uint8_t output.
4. Clean rebuild with CMake: all targets compile and link successfully.
5. Test harness runs end-to-end without crashes.

**Known Spec Misalignments Requiring Action (Session 4 Priority)**

1. **Gold code verification**: Output structure is correct (2046 chips) but mathematical correctness needs spot-check against spec reference vectors.
2. **Weil codes**: Parameter tables now available; implementation needs to be updated to use PRN-specific k values and apply XOR operation.
3. **AFS-Q assembly**: Currently concatenates; needs modulo-2 XOR per spec signal composition equations.

**If team defines Gateway 2+ scope**: Add new sections below using same three-block format (Completed, Backlog, Required Todos).
