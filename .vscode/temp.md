# Technical Implementation Guide: Programming the LSIS-AFS Spreading Code Gateway

## 1. Strategic Objectives of Gateway 1: The Spreading Code Milestone

The transition from the Gateway 0 architectural blueprint to the Gateway 1 implementation phase marks the shift from conceptual design to the generation of executable Signal-In-Space (SiS). The Spreading Code module serves as the foundational "first gate" in this evolution, acting as the primary translator of LunaNet Interoperability Specifications (LNIS) into functional code. Precision at this stage is non-negotiable; we are moving from high-level documentation to the concrete engineering required to produce the 2492.028 MHz center frequency carrier and the mandatory Right-Hand Circular Polarization (RHCP) required for lunar navigation.

As defined in the technical specifications for the Lunar Augmented Navigation Service (LANS), the specific goal of this module is the generation of In-phase (I) and Quadrature (Q) pseudo-random noise (PN) sequences. These sequences are the pulse of our dual-channel architecture, where total signal power is divided between the data and pilot channels. By generating these codes at a precise rate of 1.023 Mcps, we enable cislunar receivers to achieve time-of-flight measurements and signal tracking at a received power level of -163 dBW. This implementation bridges the gap between theoretical interoperability and the rigid physical constraints of the lunar environment, providing the structural groundwork for a sustainable Position, Navigation, and Timing (PNT) ecosystem.

## 2. Evaluative Analysis of the Technology Stack and Library Selection

The technology stack for Gateway 1 is mandated by Technical Constraints TC-1 through TC-4. This specific selection ensures that our implementation is POSIX-compliant, cross-platform (Linux/Windows), and capable of the real-time performance necessary for high-rate sample generation.

| Component       | Language/Library | "So What?" (SDR Competitive Impact)                                                                                                                                       |
| --------------- | ---------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Core Engine     | C++              | Mandatory for low-level I/Q sample generation. Provides the deterministic performance required for 1.023 Mcps spreading and real-time bit manipulation.                   |
| Orchestration   | Python           | Facilitates high-level orchestration and rapid unit testing, ensuring the framework remains an open and accessible technical resource for the scientific community.       |
| Vector Kernels  | VOLK             | Critical for Stage 2 signal accumulation. Uses SIMD (Single Instruction, Multiple Data) optimization to handle the computational overhead of composite signal generation. |
| Transform Ops   | FFTW             | The "Fastest Fourier Transform in the West" is the recommended library for efficient frequency-domain processing within the SDR pipeline.                                 |
| GNSS Foundation | GPS-SDR-SIM      | Leveraging this open-source foundation prevents "rebuilding the wheel" for basic GNSS functions, allowing us to focus on unique 12-second AFS frame structures.           |

This stack moves us beyond proprietary "black box" solutions. By utilizing C++ for the heavy lifting and Python for management, the LSIS-AFS gateway is optimized for deployment on everything from standard workstations to embedded ARM-based cislunar payloads.

## 3. Architectural Integration: The C++ and Python Hybrid Framework

To ensure the LSIS-AFS implementation can adapt as NASA, ESA, and JAXA finalize specific message formats, we employ a "Provider Plugin" pattern. This pattern decouples the high-level message framing from the underlying physical layer modulation engine. This modularity is essential for maintaining a stable Signal-In-Space while the contents of the Clock and Ephemeris Data (CED) evolve.

The interface between the Python orchestration layer and the C++ processing engine is governed by the "Module Interface and API Definition Document." Developers must implement the following parameters within this API:

- Data Symbol Sequence Rate: Fixed at 500 sps (symbols per second).
- Synchronization Word: Implementation of the 68-symbol sync word to allow for low-complexity receiver acquisition.
- Frame and Block Structure: Handling for the 12-second AFS frame and the 20-minute block interval data.
- Time System Mapping: Logic to trace LunaNet Reference Time (LRT) to Universal Coordinated Time (UTC).

The logic of the modular pipeline is dictated by the mathematical foundation of the AFS signal s(t):

$$s(t) = \sqrt{2P*I} D*{AFS-I}(t)$$
$$C*{AFS-I}(t)$$
$$\cos(2\pi f_c t + \theta) + \sqrt{2P_Q}$$
$$C*{AFS-Q}(t)$$
$$\sin(2\pi f_c t + \theta)$$

In this equation, $P*I$ and $P_Q$ represent the received power levels (nominally -163 dBW), and $D*{AFS-I}(t)$ represents the 500 sps data symbol sequence. The spreading codes, $C*{AFS-I}(t)$ and $C*{AFS-Q}(t)$, must be computed in parallel before being modulated onto the 2492.028 MHz carrier (f_c). This workflow ensures that code generation and message framing remain independent until final baseband accumulation.

## 4. Developer’s Step-by-Step Programming Guide for Spreading Codes

Coding the spreading module requires extreme precision to maintain the 1.023 Mcps PN code rate and the 2492.028 MHz center frequency. Follow this five-step programming workflow:

1. Repository Initialization: Establish a Git-based directory structure that separates the Spreading Module from the Message Module (LDPC/Framing) and the Baseband Module (Modulation).
2. Reference Data Ingestion: Per TC-3, develop a JSON configuration parser to ingest signal constants from LNIS Annex 1 and Annex 3. Hard-coding signal parameters in C++ is prohibited to ensure future flexibility.
3. PN Sequence Generation: Program the I and Q PN codes by implementing the shift-register logic defined by the LNIS polynomials. The generator must output chips at the mandatory 1.023 Mcps rate.
4. Two-Stage Parallelization: Implement the two-stage processing logic required for SDR efficiency.
      - Stage 1: Execute satellite-specific processing threads to generate independent PN sequences and 12-second message frames.
      - Stage 2: Utilize VOLK kernels for the composite accumulation stage, summing independent streams into a single binary I/Q signal.
5. Mock-up Validation: Generate a baseline BPSK(1) I/Q file without navigation data. Perform a visual spectrum validation using GQRX or SDR# to confirm the presence of the BPSK(1) main lobe and the integrity of the output file format.

All code must be validated against Signal-In-Space Error (SISE) requirements to ensure the generated waveform is compliant with LunaNet standards.

## 5. Verification, Validation, and Risk Mitigation

A rigorous Verification and Validation (V&V) strategy is the only defense against "cascading errors." If spreading codes are misaligned at this stage, subsequent LDPC encoding and frame synchronization will fail.

The following critical technical risks must be mitigated through direct developer commands:

1. Computational Load: High-rate I/Q generation can saturate CPU resources.

- Mitigation Command: Mandate the use of VOLK for SIMD optimization in the Stage 2 accumulation pipeline.

2. Standards Drift: Message formats for different agencies (NASA vs. ESA) may change.

- Mitigation Command: Use the "Provider Plugin" design to isolate frame definitions from the core modulation engine.

3. Signal Weakness: Lunar orbits (ELFO) introduce extreme Doppler shifts and signal attenuation.

- Mitigation Command: Implement hierarchical secondary codes on the Pilot channel specifically to facilitate deep-space signal acquisition.

Success Criteria for Gateway 1 The Spreading Code gateway is successful when the system generates a baseline binary I/Q file that passes unit tests within the CI/CD pipeline. Success is measured by the ability to calculate a stable Signal-to-Noise Ratio (SNR) and a 0.0 Bit Error Rate (BER) on the unmodulated PN sequence. Completing these requirements establishes the definitive structural groundwork for a sustainable and interoperable lunar ecosystem.
