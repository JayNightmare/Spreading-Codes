# LSIS-AFS Implementation: Architecture & Approach

This document explains the software architecture, the rationale behind our design decisions, and how the various "Gateways" connect to form the complete LSIS-AFS encoding pipeline.

## Engineering Approach

Our implementation strategy was guided by three core principles:

1. **Modular, Pure Functional Core:** We built the system as a series of isolated, stateless C++ libraries. The generators and encoders take raw inputs and return exactly formatted vectors. There is no hidden global state, making the system highly testable and trivially parallelizable.
2. **Test-Driven Traceability:** Every component was built directly against the LSIS-AFS Volume A specification. The validation harness continuously cross-checks outputs against Annex 3 vectors (including full 210/210 PRN compliance for Gold and Weil code families).
3. **Mathematical Precision over Approximations:** Rather than using arbitrary resampling filters that might introduce phase drift over a 12-second transmission, our I/Q generation uses deterministic rational-index mapping ($C = \lfloor n \cdot R_{chip} / R_{sample} \rfloor$). This guarantees mathematically perfect zero-order hold alignment for both the AFS-I and AFS-Q channels across the entire 12-second navigation frame.

---

## The Gateway Pipeline

The LSIS-AFS specification divides the system into functional "Gateways". We have fully implemented Gateways 1 through 4 for the encode/transmit side and added partial Gateway 5 foundations for the decode side.

### Gateway Responsibilities

| Gateway | Name              | Primary Responsibility                                                                | Key Components Implemented                                                                                    |
| ------- | ----------------- | ------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| **1**   | Spreading Codes   | Generate the high-speed cryptographic sequences used to spread the signal.            | - Gold Code LFSR<br>- Legendre Sequence Cache<br>- Weil Primary/Tertiary Generators<br>- Tiered Code Assembly |
| **2**   | FEC Encoding      | Add redundancy to the navigation data so the receiver can correct errors.             | - BCH(51,8) Encoder/Decoder<br>- CRC-24Q Generator<br>- Rate-1/2 LDPC Encoder<br>- 60×98 Block Interleaver    |
| **3**   | Frame Assembly    | Pack the encoded data into the 6000-symbol, 12-second navigation frame.               | - Sync Pattern Prepending<br>- SB1 / SB2 / SB3 / SB4 Builders<br>- Frame Concatenation                        |
| **4**   | Signal Generation | Convert the digital symbols and spreading codes into an analog-ready baseband signal. | - AFS-I Data Modulator<br>- BPSK Mapper<br>- Arbitrary-Rate I/Q Multiplexer                                   |
| **5**   | Sync & Decoding (Partial) | Start the receive-side inverse chain from frame location to soft metrics.       | - Sync Reference Symbol Builder<br>- 6000-Symbol Region Extractor (68/52/5880)<br>- LLR Conversion Helper      |

Current Gateway 5 gaps: robust noisy-stream frame sync detector, de-spreading integration, LDPC decode-chain integration, and BER/sync reliability qualification.

---

## Architecture Flow

The following diagram illustrates how data flows through the gateways to become an RF-ready baseband signal.

![LSIS-AFS Architecture](/docs/goonhilly/figures/LSIS-AFS-Architecture.png)

---

## Design Rationale: I/Q Sample Generation

One of the most complex challenges in Gateway 4 is merging the AFS-I channel (which runs at 1.023 MHz) and the AFS-Q channel (which runs at 5.115 MHz) into a single, perfectly time-aligned I/Q sample stream.

### The Problem with Interpolation

Standard SDR approaches often use interpolating FIR filters (like Root-Raised Cosine) to match sample rates. However, because the AFS-Q channel has a massive 61,380,000 chip period, tiny floating-point rounding errors in time-steps or filter phases can cause the I and Q channels to slowly drift out of alignment by the end of the 12-second frame. This would ruin the delicate sync expected by the decoder.

### Our Solution: Rational Index Mapping

Instead of continuous time simulation, we treat time as discrete mathematical steps. The sample rate is allowed to be any positive integer multiple of the AFS-I chip rate (1.023 MHz).

For every single output sample `n`, we calculate the exact chip index mathematically:

```cpp
// i_rate = 1,023,000
// q_rate = 5,115,000
// sr = output sample rate

const size_t i_index = n * i_rate / sr;
const size_t q_index = (n * q_rate) / sr; // 64-bit math to prevent overflow

out.i.push_back(BpskMap(afs_i_chips[i_index]));
out.q.push_back(BpskMap(afs_q_chips[q_index]));
```

**Why this matters:**

1. **Perfect Sync:** By tying both indices directly to the global sample index `n`, it is mathematically impossible for the two channels to drift apart.
2. **Decimation by Design:** The Goonhilly Workshop expects an I/Q file at `1.023 MHz`. At this rate, the AFS-I channel has 1 sample per chip, but the AFS-Q channel has 1 sample per 5 chips. The rational index map elegantly handles this by deterministically decimating the AFS-Q channel (essentially "sampling" the Q-chip at that exact instantaneous moment).
3. **Performance:** Integer multiplication and division are significantly faster than running 12 million samples through a polyphase interpolation filter, allowing us to generate the 93 MB signal file in a fraction of a second.
