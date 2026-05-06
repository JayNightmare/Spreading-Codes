<!-- markdownlint-disable MD032 -->

# Gateway 1 Review: Current Spreading Codes Implementation

## 1. Request and Success Criteria

Manager request: review Gateway 1 and produce a document that explains the current spreading-codes implementation.

Success criteria used for this review:

- Explain how code is configured and loaded.
- Explain how each Gateway 1 code family is generated (Gold, Weil primary, Weil tertiary, AFS-I, AFS-Q).
- Explain how generation is exposed through public APIs.
- Identify what is validated by tests today.
- Separate facts, assumptions, open questions, and recommendations.

## 2. Scope and Evidence

Files reviewed (primary):

- codes/gateway1/spreading_config.h
- codes/gateway1/spreading_config.cpp
- codes/gateway1/gold_code_generator.h
- codes/gateway1/gold_code_generator.cpp
- codes/gateway1/weil_code_generator.h
- codes/gateway1/weil_code_generator.cpp
- codes/gateway1/tiered_code_generator.h
- codes/gateway1/tiered_code_generator.cpp
- codes/spreading_codes.h
- codes/spreading_codes.cpp
- codes/c_api.h
- codes/c_api.cpp
- config/spreading_codes_config.ini
- docs/spec_tables/table_10_secondary_codes.csv
- docs/spec_tables/table_11_code_assignments.csv
- codes/test_engine.cpp
- codes/testing/test_validators.h
- codes/testing/test_validators.cpp

## 3. Architecture Summary

Gateway 1 logic is implemented as four focused modules:

- Configuration/table loading in `spreading_config.*`.
- Gold code generation in `gold_code_generator.*`.
- Weil (primary and tertiary) generation in `weil_code_generator.*`.
- Tiered composition (AFS-I and AFS-Q) in `tiered_code_generator.*`.

A top-level runtime facade in `codes/spreading_codes.cpp` holds loaded tables and exposes generation entry points (`generate_gold_code`, `generate_weil_primary`, `generate_weil_tertiary`, `generate_afs_i`, `generate_afs_q`).

C-callable wrappers in `codes/c_api.cpp` export these through `extern "C"` functions.

Evidence:

- `SpreadingSpecTables` and constants are defined in `codes/gateway1/spreading_config.h:12` and `codes/gateway1/spreading_config.h:25`.
- Top-level gateway entry points are in `codes/spreading_codes.cpp:120`, `codes/spreading_codes.cpp:136`, `codes/spreading_codes.cpp:148`, `codes/spreading_codes.cpp:160`, `codes/spreading_codes.cpp:172`.
- C API exports are declared in `codes/c_api.h:44`, `codes/c_api.h:49`, `codes/c_api.h:65` and implemented in `codes/c_api.cpp:37`, `codes/c_api.cpp:42`, `codes/c_api.cpp:57`.

## 4. Configuration and Table Loading Flow

### 4.1 Runtime config input

`load_spreading_code_config(...)` calls `gateway1::LoadSpreadingConfig(...)` and stores loaded tables/paths in process-global engine state.

Evidence:

- Config load entry point: `codes/spreading_codes.cpp:86`.
- Loaded state assignment: `codes/spreading_codes.cpp:97` and `codes/spreading_codes.cpp:99`.

### 4.2 Parser behavior

`spreading_config.cpp` provides lightweight parsers for:

- Key-value INI-like config (`ParseKeyValueFile`).
- CSV tables (`ReadCsvFile`, `SplitCsvLine`, header lookup by case-insensitive key).

Behavior includes:

- Trim whitespace.
- Skip blank/comment lines (`#`).
- Pad short CSV rows to header width.

Evidence:

- CSV splitter: `codes/gateway1/spreading_config.cpp:35`.
- Strict integer and size parsing: `codes/gateway1/spreading_config.cpp:59`, `codes/gateway1/spreading_config.cpp:79`.
- CSV file reader and header check: `codes/gateway1/spreading_config.cpp:116`, `codes/gateway1/spreading_config.cpp:156`.
- Comment/blank filtering: `codes/gateway1/spreading_config.cpp:138`, `codes/gateway1/spreading_config.cpp:187`.
- Case-insensitive header matching: `codes/gateway1/spreading_config.cpp:99`.

### 4.3 Tables loaded

`LoadSpreadingConfig` resolves `spec_tables_dir` and loads:

- Appendix C Gold delays.
- Appendix D Weil primary parameters.
- Appendix E Weil tertiary parameters.
- Table 10 secondary 4-chip sequences.
- Optional Table 11 per-PRN secondary assignments.

Evidence:

- Load sequence: `codes/gateway1/spreading_config.cpp:505`, `codes/gateway1/spreading_config.cpp:508`, `codes/gateway1/spreading_config.cpp:511`, `codes/gateway1/spreading_config.cpp:514`, `codes/gateway1/spreading_config.cpp:528`.
- Optional Table 11 behavior if file missing: `codes/gateway1/spreading_config.cpp:415`.

### 4.4 Defaults and caps

If Table 11 is absent or partial, PRNs are initialized to one default secondary code from config (`afs_q_secondary_code_id`, default `S0`).

The runtime AFS-Q output cap (`afs_q_max_chips`) is parsed from config and stored in tables.

Evidence:

- Config defaults in repo config: `config/spreading_codes_config.ini:13`, `config/spreading_codes_config.ini:16`.
- Initialization before overrides: `codes/gateway1/spreading_config.cpp:408`, `codes/gateway1/spreading_config.cpp:527`.
- Cap parsing: `codes/gateway1/spreading_config.cpp:532`, `codes/gateway1/spreading_config.cpp:533`.

### 4.5 Completeness checks

For Gold/Weil tables, loader enforces all PRNs 1..210 are present; missing entries fail fast.

Evidence:

- Gold missing PRN error: `codes/gateway1/spreading_config.cpp:255`.
- Weil primary missing PRN error: `codes/gateway1/spreading_config.cpp:302`.
- Weil tertiary missing PRN error: `codes/gateway1/spreading_config.cpp:341`.
- Secondary codes S0..S3 required: `codes/gateway1/spreading_config.cpp:399`.
- Final readiness gate: `codes/gateway1/spreading_config.cpp:453`, `codes/gateway1/spreading_config.cpp:552`.

## 5. Generation Algorithms (Current Implementation)

### 5.1 Gold code (AFS-I primary)

Implementation details:

- Two 11-stage LFSRs, both initialized to `0x7FF`.
- Feedback masks: G1 `0x402`, G2 `0x492`.
- G2 is pre-advanced by `(2047 - g2_delay) % 2047`.
- Output length is 2046 chips.
- Each chip is `g1_next XOR g2_next`.

Evidence:

- LFSR constants and masks: `codes/gateway1/gold_code_generator.cpp:7`, `codes/gateway1/gold_code_generator.cpp:8`, `codes/gateway1/gold_code_generator.cpp:9`, `codes/gateway1/gold_code_generator.cpp:10`.
- G2 advance formula: `codes/gateway1/gold_code_generator.cpp:63`.
- XOR combine loop: `codes/gateway1/gold_code_generator.cpp:71`.
- PRN range guard: `codes/gateway1/gold_code_generator.cpp:40`.

### 5.2 Weil primary

Implementation details:

- Uses Legendre sequence over prime 10223.
- Computes Weil chips as `L[t] XOR L[(t+k) mod 10223]`.
- Inserts fixed 7-chip expansion sequence `{0,1,1,0,1,0,0}` at insertion index `p` (1-based handling is explicitly documented in code comments).
- Result length is 10230 chips (`10223 + 7`).

Evidence:

- Legendre symbol and sequence generation: `codes/gateway1/weil_code_generator.cpp:11`, `codes/gateway1/weil_code_generator.cpp:31`.
- Primary generator and k/p validity checks: `codes/gateway1/weil_code_generator.cpp:76`, `codes/gateway1/weil_code_generator.cpp:96`.
- Expansion sequence and insertion points: `codes/gateway1/weil_code_generator.cpp:104`, `codes/gateway1/weil_code_generator.cpp:109`, `codes/gateway1/weil_code_generator.cpp:120`.

### 5.3 Weil tertiary

Implementation details:

- Uses Legendre sequence over prime 1499.
- Computes 1499 Weil chips with same XOR shift structure.
- Appends explicit terminal zero to produce 1500 chips.

Evidence:

- Tertiary PRN parameter and validation: `codes/gateway1/weil_code_generator.cpp:145`, `codes/gateway1/weil_code_generator.cpp:146`.
- Terminal zero append: `codes/gateway1/weil_code_generator.cpp:161`.

### 5.4 Legendre caching

Implementation details:

- Legendre sequences are cached in a global cache keyed by prime.
- Cache access/clear guarded by mutex.
- `ClearLegendreCache()` is exposed for explicit memory cleanup.

Evidence:

- Cache class: `codes/gateway1/weil_code_generator.cpp:43`.
- Mutex guard points: `codes/gateway1/weil_code_generator.cpp:46`, `codes/gateway1/weil_code_generator.cpp:55`.
- Cache clear API: `codes/gateway1/weil_code_generator.cpp:72`.

### 5.5 Tiered AFS assembly

`GenerateAfsI(prn)` is currently a direct alias to Gold code generation.

`GenerateAfsQ(prn, max_chips)` currently composes chips as:

AFS-Q[i] = Primary[i mod P] XOR Secondary[floor(i/P) mod S] XOR Tertiary[floor(i/(P*S)) mod T]

Where current lengths are:

- P = 10230 (Weil primary)
- S = 4 (secondary)
- T = 1500 (tertiary)

So full tiered period is `10230 * 4 * 1500 = 61,380,000` chips, then optionally truncated by `max_chips`.

Evidence:

- AFS-I passthrough: `codes/gateway1/tiered_code_generator.cpp:11`, `codes/gateway1/tiered_code_generator.cpp:12`.
- AFS-Q dependencies: `codes/gateway1/tiered_code_generator.cpp:22`, `codes/gateway1/tiered_code_generator.cpp:27`.
- Secondary assignment lookup and validation: `codes/gateway1/tiered_code_generator.cpp:32`, `codes/gateway1/tiered_code_generator.cpp:33`.
- Full-length and truncation computation: `codes/gateway1/tiered_code_generator.cpp:42`, `codes/gateway1/tiered_code_generator.cpp:43`, `codes/gateway1/tiered_code_generator.cpp:45`.
- Per-chip XOR composition: `codes/gateway1/tiered_code_generator.cpp:50`, `codes/gateway1/tiered_code_generator.cpp:51`, `codes/gateway1/tiered_code_generator.cpp:52`, `codes/gateway1/tiered_code_generator.cpp:53`.

## 6. Public Runtime Behavior

### 6.1 Config must be loaded first

All top-level generation functions check `EnsureConfigLoaded()` and fail (empty vector + last error) if config was not loaded.

Evidence:

- Guard function and error set: `codes/spreading_codes.cpp:32`, `codes/spreading_codes.cpp:34`.
- Guard use in all generators: `codes/spreading_codes.cpp:121`, `codes/spreading_codes.cpp:137`, `codes/spreading_codes.cpp:149`, `codes/spreading_codes.cpp:161`, `codes/spreading_codes.cpp:173`.

### 6.2 AFS-Q cap behavior

If caller passes `max_chips == 0`, top-level API substitutes configured cap (`afs_q_max_chips`). Current checked-in config cap is 40920 chips.

Evidence:

- Effective limit selection: `codes/spreading_codes.cpp:177`, `codes/spreading_codes.cpp:178`.
- Current config value: `config/spreading_codes_config.ini:16`.

Operational implication:

- Default `generate_afs_q(prn)` returns at most 40920 chips in this repo config, not the full 61,380,000-chip period.

### 6.3 C API bridge

C API forwards to C++ API and copies generated vectors into caller buffers. For AFS-Q, C API converts `int max_chips` to `size_t`.

Evidence:

- C wrappers: `codes/c_api.cpp:37`, `codes/c_api.cpp:42`, `codes/c_api.cpp:47`, `codes/c_api.cpp:52`, `codes/c_api.cpp:57`.
- Cast site: `codes/c_api.cpp:58`.

## 7. Validation Coverage in Current Test Harness

### 7.1 Smoke checks

Smoke suite checks:

- Gold/Weil primary/Weil tertiary lengths.
- AFS-Q non-empty and cap-respecting behavior.
- AFS-I equals Gold.
- Invalid PRN rejection.
- Batch generation for all PRNs.

Evidence:

- Smoke function: `codes/test_engine.cpp:45`.
- AFS-Q cap assertion: `codes/test_engine.cpp:76`.
- AFS-I equals Gold: `codes/test_engine.cpp:84`.
- Invalid PRN empty: `codes/test_engine.cpp:87`.
- Batch generation assertion: `codes/test_engine.cpp:92`.

### 7.2 Annex 3 vector validation

Harness loads three Annex3 files and runs per-PRN equality checks for Gold, Weil primary, and Weil tertiary.

Evidence:

- Annex3 loader flow: `codes/test_engine.cpp:112`, `codes/test_engine.cpp:122`, `codes/test_engine.cpp:130`, `codes/test_engine.cpp:138`.
- Per-PRN suite runner usage: `codes/test_engine.cpp:145`, `codes/test_engine.cpp:151`, `codes/test_engine.cpp:158`.
- Validator mismatch reporting: `codes/testing/test_validators.cpp:24`, `codes/testing/test_validators.cpp:54`.

### 7.3 Table 11 checks

Table 11 validation currently builds an internal 12-node expected set (identity PRN mapping, S0..S3 cycling, phase offset 0), then verifies assignment and first-epoch tiered structure.

Evidence:

- Table11 test function: `codes/test_engine.cpp:192`.
- Hardcoded 1..12 loop and cycling assumptions: `codes/test_engine.cpp:209`, `codes/test_engine.cpp:214`, `codes/test_engine.cpp:216`.
- Structural tiered check logic: `codes/testing/test_validators.cpp:203`, `codes/testing/test_validators.cpp:236`.

## 8. Facts, Assumptions, Open Questions, Recommendations

## Facts

- Gateway 1 supports 210 PRNs with constants centralized in config headers (`kMaxPrns=210`, Gold length 2046, Weil primary prime 10223, Weil tertiary prime 1499, tertiary length 1500, expansion length 7).
     - Evidence: `codes/gateway1/spreading_config.h:12`, `codes/gateway1/spreading_config.h:13`, `codes/gateway1/spreading_config.h:14`, `codes/gateway1/spreading_config.h:15`, `codes/gateway1/spreading_config.h:16`, `codes/gateway1/spreading_config.h:17`.
- Secondary code definitions in spec tables are exactly S0=1110, S1=0111, S2=1011, S3=1101.
     - Evidence: `docs/spec_tables/table_10_secondary_codes.csv:3`, `docs/spec_tables/table_10_secondary_codes.csv:4`, `docs/spec_tables/table_10_secondary_codes.csv:5`, `docs/spec_tables/table_10_secondary_codes.csv:6`.
- The default runtime config chooses `S0` as fallback secondary code and caps AFS-Q at 40920 chips.
     - Evidence: `config/spreading_codes_config.ini:13`, `config/spreading_codes_config.ini:16`.
- Table 11 file is optional in loader; if absent, default secondary assignment remains in effect for all PRNs.
     - Evidence: `codes/gateway1/spreading_config.cpp:415`, `codes/gateway1/spreading_config.cpp:527`.

## Assumptions

- This review assumes the checked-in `config/spreading_codes_config.ini` is representative of normal runtime usage, so default AFS-Q behavior is interpreted using `afs_q_max_chips=40920`.
- This review assumes Annex3 reference files under `Validation/annex3/txt` are the intended source of truth for Gold and Weil sequence correctness in current CI/local validation flows.

## Open Questions

- Should default `generate_afs_q(prn)` behavior represent a full 12-second composite period (61,380,000 chips) or remain capped by config for memory/performance? Current implementation favors cap-first behavior.
     - Evidence: `codes/gateway1/tiered_code_generator.cpp:43`, `codes/spreading_codes.cpp:178`, `config/spreading_codes_config.ini:16`.
- Should Table 11 test expectations be read directly from `docs/spec_tables/table_11_code_assignments.csv` rather than hardcoded for nodes 1..12?
     - Evidence: `docs/spec_tables/table_11_code_assignments.csv:2`, `codes/test_engine.cpp:209`.
- Should the C API reject negative `max_chips` explicitly before casting to `size_t`?
     - Evidence: `codes/c_api.cpp:57`, `codes/c_api.cpp:58`.

## Recommendations

- Decide and document intended default AFS-Q API contract (capped epoch vs full period) in public docs and API comments, since it materially affects output length and downstream consumers.
- Align Table 11 validation with table-driven data loading from CSV to prevent test drift if assignments change.
- Consider adding explicit C API validation for `max_chips < 0` to avoid ambiguous behavior from signed-to-unsigned conversion.
- Keep current strong fail-fast config checks (missing PRN rows, missing secondary rows), as they reduce silent misconfiguration risk.

## 9. Practical “How It Works” Summary

For one PRN at runtime:

1. `load_spreading_code_config(...)` loads and validates all required tables and limits.
2. `generate_afs_i(prn)` returns Gold chips for that PRN.
3. `generate_afs_q(prn, max_chips)` builds tiered chips from Weil primary + secondary + Weil tertiary with modular indexing.
4. Top-level wrappers track last error string; C API mirrors these generators for external consumers.

This is the current implemented Gateway 1 spreading-code pipeline in the repository.
