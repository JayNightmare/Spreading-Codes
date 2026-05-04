LSIS-AFS Competition — Submission Report
Replaceallplaceholdertextin[brackets]withyourteam'sinformation.
Delete any sections marked (optional) if not applicable. Submit this
file as SUBMISSION.md in the root of your repository.
Team Information
Field Details
Team Name [Your team name]
Members [Name 1, Name 2, ...]
Institution / Affiliation [University, company, or independent]
Contact Email [Primary contact email]
Repository URL [Link to your private repo]
Implementation Overview
Language & Technology Stack
Component Choice
Primary Language [e.g. C, Rust, Python, Go, Java]
Build System [e.g. CMake, Cargo, Make, Gradle]
Key Libraries [List any third-party libraries used]
Test Framework [e.g. CTest, pytest, Go test, JUnit]
Platform(s) Tested [e.g. Linux x86_64, macOS ARM64, Windows]
Architecture Summary
[2-3 paragraphs describing your system architecture, module structure, and key
design decisions. Include a diagram if helpful.]
Build & Run Instructions
Prerequisites
[List all dependencies and how to install them.]
[package manager install commands, e.g.:]
[apt install ... / brew install ... / pip install ...]

Build
[exact build commands]

Run Tests
[exact test commands]

Run Examples
[exact commands to run key examples]

Gateway Status
Mark each gateway as Complete, Partial, or Not Attempted. Provide a brief
note for partial implementations.

| Gateway | Status | Notes |
| --- | --- | --- |
| 0 — Design & Architecture | [ ] Complete / [ ] Partial / [ ] Not Attempted | |
| 1 — Spreading Code Generation | [ ] Complete / [ ] Partial / [ ] Not Attempted | |
| 2 — Forward Error Correction | [ ] Complete / [ ] Partial / [ ] Not Attempted | |
| 3 — Navigation Message Framing | [ ] Complete / [ ] Partial / [ ] Not Attempted | |
| 4 — Baseband Signal Generation | [ ] Complete / [ ] Partial / [ ] Not Attempted | |
| 5 — Frame Sync & Decoding | [ ] Complete / [ ] Partial / [ ] Not Attempted | |
| 6 — Message Parsing | [ ] Complete / [ ] Partial / [ ] Not Attempted | |
| 7 — Integration & Validation | [ ] Complete / [ ] Partial / [ ] Not Attempted | |
| 8 — Documentation & Examples | [ ] Complete / [ ] Partial / [ ] Not Attempted | |

Validation Results

Gateway 1: Spreading Codes

| Check | Pass/Fail | Evidence |
| --- | --- | --- |
| Gold codes match Annex3 (all 210 PRNs) | | [test output or file reference] |
| Weil primary codes match Annex3 (all 210 PRNs) | | |
| Weil tertiary codes match Annex3 (all 210 PRNs) | | |
| Secondary codes match Table 10 | | |
| Tiered codes maintain coherency | | |
| Code lengths match Table 9 | | |
| Generation time < 1s per PRN | | |

Gateway 2: Forward Error Correction

| Check | Pass/Fail | Evidence |
| --- | --- | --- |
| BCH encoder produces valid 52-symbol codewords | | |
| LDPC encoder produces valid codewords | | |
| LDPC puncturing handled correctly | | |
| CRC-24 matches specification | | |
| Interleaver pattern validated | | |
| Round-trip encode/decode recovers data | | |
| Encoding time < 100ms per frame | | |
| Decoding time < 1s per frame | | |
| BER < 10^-5 at SNR > 0 dB | | |

Gateway 3: Navigation Message Framing

| Check | Pass/Fail | Evidence |
| --- | --- | --- |
| Frame structure matches Figure 9 | | |
| Symbol counts: 68 + 52 + 5880 = 6000 | | |
| Bit allocations match spec tables | | |
| Frame duration is 12 seconds | | |

Gateway 4: Baseband Signal Generation

| Check | Pass/Fail | Evidence |
| --- | --- | --- |
| I/Q samples correctly formatted | | |
| AFS-I chip rate: 1.023 Mchip/s | | |
| AFS-Q chip rate: 5.115 Mchip/s | | |
| Symbol rate: 500 symbols/s (AFS-I) | | |
| Code synchronization correct | | |
| Signal duration: 12 seconds | | |

Gateway 5: Frame Sync & Decoding

| Check | Pass/Fail | Evidence |
| --- | --- | --- |
| Frame sync detection > 99% at SNR > 0 dB | | |
| Decoders recover original data correctly | | |
| CRC validation catches errors | | |
| LDPC converges in < 50 iterations | | |
| Decode time < 1s per frame | | |

Gateway 6: Message Parsing

| Check | Pass/Fail | Evidence |
| --- | --- | --- |
| All subframes parse correctly | | |
| WN, ITOW, TOI fields extracted | | |
| Time of transmission calculated accurately | | |
| All message types handled | | |

Gateway 7: Integration & Validation

| Check | Pass/Fail | Evidence |
| --- | --- | --- |
| Round-trip recovers data with 100% accuracy | | |
| All 12 interim test codes working | | |
| Process 12s frames in < 1 second | | |
| All "shall" requirements verified | | |

Performance Benchmarks

| Metric | Your Result | Target |
| --- | --- | --- |
| Code generation (per PRN) | [time] | < 1 second |
| Frame encoding (per frame) | [time] | < 100 ms |
| Frame decoding (per frame) | [time] | < 1 second |
| Real-time factor | [x real-time] | > 1x |
| BER at SNR 0 dB | [value] | < 10^-5 |
| Frame sync reliability | [%] | > 99% |
| Test coverage | [%] | > 90% |

[Describe your benchmarking methodology: hardware, OS, compiler flags, number of runs, etc.]

Interoperability (optional)

| Partner Team | Test Performed | Result |
| --- | --- | --- |
| [Team name] | [e.g. Decoded their signal file] | [Pass/Fail + notes] |
| [Team name] | [e.g. They decoded our signal] | [Pass/Fail + notes] |

[Describe the interop testing process and any issues encountered.]

Test Summary

| Category | Total | Passing | Failing | Skipped |
| --- | --- | --- | --- | --- |
| Unit tests | | | | |
| Integration tests | | | | |
| Compliance tests | | | | |
| Performance tests | | | | |

[Paste or reference your test output here]

Known Limitations

[List any known issues, incomplete features, or deviations from the specification.]

Innovation & Extras (optional)

[Describe any additional features, optimizations, visualizations, or novel approaches your team implemented beyond the base requirements.]

File Manifest

| Path | Description |
| --- | --- |
| README.md | Project overview and quick start |
| SUBMISSION.md | This submission report |
| src/ | Source code |
| tests/ | Test suite |
| docs/ | Documentation |
| examples/ | Usage examples |
| [other] | [description] |

Self-Assessment

| Category (Points) | Self-Score | Justification |
| --- | --- | --- |
| Correctness (40) | /40 | |
| Performance (20) | /20 | |
| Completeness (20) | /20 | |
| Code Quality (10) | /10 | |
| Innovation & Extras (10) | /10 | |
| **Total** | **/100** | |

Additional Notes (optional)

[Any other information you'd like the judges to know.]
