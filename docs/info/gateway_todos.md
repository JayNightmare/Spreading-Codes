# Gateway Todo Tracker

Last updated: 2026-07-17

## Status Snapshot

- Gateway 1: Complete and Annex3-validated.
- Gateway 2: Complete for encode pipeline (BCH/CRC/LDPC encode/interleaver).
- Gateway 3: Complete frame assembly pipeline.
- Gateway 4: Complete baseband generation pipeline.
- Gateway 5: Partial foundations implemented (sync reference, frame slicing, LLR utility, standalone tests).
- Gateway 6: Not started in this repository.

## Gateway 5 Priority Backlog

### In place

- Sync reference symbol construction from fixed 68-symbol SP.
- 6000-symbol frame region extraction (SP=68, SB1=52, interleaved=5880).
- LLR helper with LSIS sign convention.
- Standalone tests: `gateway5_frame_sync_test`, `gateway5_symbol_extractor_test`.

### Remaining for Gateway 5 completion

- Implement robust frame synchronization detector over noisy and offset streams.
- Integrate de-spreading path to produce soft symbols from received I/Q.
- Integrate LDPC decode chain for SB2/SB3/SB4 and connect CRC gates.
- Add full-frame decode acceptance/rejection flow and telemetry.
- Add BER and sync-reliability benchmarking for target thresholds.

## Cross-Gateway Next Steps

- Gateway 5 -> Gateway 6 handoff: decoded bit payload contracts for parser inputs.
- Gateway 6 implementation: SB1-SB4 field parsing and ToT computation.
- End-to-end round-trip qualification: encode -> channel/noise -> decode -> parse.

## External References

- Gateway 5 design/scaffold notes: <https://github.com/KURE-x-Tech/Asteria-Knowledge-Base-G5-share>
- Note: the external knowledge bank is guidance and is not auto-integrated into this repository build.
