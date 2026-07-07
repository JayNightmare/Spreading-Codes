| LSIS-AFS |     | Competition |     | —   | Technical | FAQ |     |
| -------- | --- | ----------- | --- | --- | --------- | --- | --- |
This document clarifies common implementation questions and potential ambiguities in the LSIS-
AFSVolumeAspecification. Itisintendedtohelpcontestantsavoidcommonpitfallswithoutgiving
| away the  | engineering |            | work. |           |              |     |     |
| --------- | ----------- | ---------- | ----- | --------- | ------------ | --- | --- |
| Spreading | Code        | Generation |       |           |              |     |     |
| Q1: What  | are         | the Gold   | code  | generator | polynomials? |     |     |
The Gold code uses two 11-stage Fibonacci LFSRs. The circuit is shown in Figure C-1 of the
| specification, |             | but for | clarity: |         |          |     |     |
| -------------- | ----------- | ------- | -------- | ------- | -------- | --- | --- |
| • G1           | polynomial: |         | x¹¹ +    | x² + 1  |          |     |     |
| • G2           | polynomial: |         | x¹¹ +    | x⁸ + x⁵ | + x² + 1 |     |     |
Both LFSRs shift left (stage 1 → 2 → … → 11). The output chip is taken from stage 11 (the MSB).
| Feedback | is XORed |     | into stage | 1.  |     |     |     |
| -------- | -------- | --- | ---------- | --- | --- | --- | --- |
Q2: How do I use the G2 initialization vectors from Tables C-1 through C-5?
The tables provide both a "G2 Delay" and a "G2 Initialization" value for each PRN. These are
| equivalent | representations: |     |     |     |     |     |     |
| ---------- | ---------------- | --- | --- | --- | --- | --- | --- |
• The delay is the number of chips G2 would advance from all-1s before code generation
begins.
• The initialization vector is the resulting G2 register state after that delay.
Youcaneither: 1. StartG2atall-1sandadvanceitbythedelayvalue,OR2. Loadtheinitialization
| vector | directly | into G2 | as the | starting | state. |     |     |
| ------ | -------- | ------- | ------ | -------- | ------ | --- | --- |
Important: The delay-to-init mapping uses the reciprocal polynomial form (a right-shift LFSR with
tapsatbitpositions0,2,5,8). Onceyouhavetheinitstate,generatechipsusingtheleft-shiftform
| described | above. |     |     |     |     |     |     |
| --------- | ------ | --- | --- | --- | --- | --- | --- |
Q3: What does "I11 is the MSB and I1 is the LSB" mean for the initialization vector?
The hex value in the table (e.g., 0x514 for PRN 1) represents the 11-bit register state where: - Bit
10 (MSB of the hex value) = stage I11 - Bit 0 (LSB of the hex value) = stage I1
| Load this        | directly | into | your             | 11-bit | register. |          |           |
| ---------------- | -------- | ---- | ---------------- | ------ | --------- | -------- | --------- |
| Q4: How          | is the   | Weil | code constructed |        | from the  | Legendre | sequence? |
| The construction |          | has  | three            | steps: |           |          |           |
1. Generate Legendre sequence L(t) of length p (10223 for primary, 1499 for tertiary)
2. Compute Weil sequence: W(t) = L(t) XOR L((t + k) mod p), where k is the Weil index from
| Appendix |     | D/E |     |     |     |     |     |
| -------- | --- | --- | --- | --- | --- | --- | --- |
3. Insert (not replace) 7 bits at position p to produce the final code of length p+7
1

The 7 inserted bits for the primary code are derived from the Legendre sequence at the insertion
| point.   | After insertion, | all      | subsequent |     | bits shift | right by | 7 positions. |
| -------- | ---------------- | -------- | ---------- | --- | ---------- | -------- | ------------ |
| Q5: What | is the           | Legendre | sequence?  |     |            |          |              |
The Legendre sequence L(t) for t = 0, 1, …, p-1 is defined using the Legendre symbol: - L(0) =
0 (by convention, since 0 is neither a residue nor non-residue) - L(t) = 0 if t is a quadratic residue
| mod p | - L(t) = 1 if | t is a | quadratic | non-residue |     | mod p |     |
| ----- | ------------- | ------ | --------- | ----------- | --- | ----- | --- |
A quadratic residue can be computed via Euler's criterion: t^((p-1)/2) mod p. If the result is 1, t is
| a QR;    | if p-1, it is | a non-residue. |     |        |     |     |     |
| -------- | ------------- | -------------- | --- | ------ | --- | --- | --- |
| Q6: What | are the       | secondary      |     | codes? |     |     |     |
Per Table 10, the four secondary codes are fixed 4-chip sequences: - S0: 1, 1, 1, 0 - S1: 0, 1, 1,
| 1 - S2: | 1, 0, 1, 1   | - S3: | 1, 1, 0, | 1     |           |     |     |
| ------- | ------------ | ----- | -------- | ----- | --------- | --- | --- |
| These   | are assigned | to    | LNSP     | nodes | per Table | 11. |     |
Frame Encoding
| Q7: How | does BCH(51,8) |     | encoding |     | work | for Subframe | 1?  |
| ------- | -------------- | --- | -------- | --- | ---- | ------------ | --- |
The 9-bit SB1 field contains FID (2 bits) and TOI (7 bits). Encoding proceeds as:
| 1. Take | the 8 | LSBs | (bits 1-8) | of  | the 9-bit | field |     |
| ------- | ----- | ---- | ---------- | --- | --------- | ----- | --- |
2. Encode using the BCH generator polynomial 763 (octal) via an 8-stage LFSR
| 3. This    | produces | 51   | encoded | symbols |         |                |     |
| ---------- | -------- | ---- | ------- | ------- | ------- | -------------- | --- |
| 4. Take    | the MSB  | (bit | 9) and  | XOR     | it with | all 51 symbols |     |
| 5. Prepend | the      | MSB  | as the  | first   | symbol  |                |     |
Result: 52 symbols total (1 prepended MSB + 51 modified encoded symbols).
| Q8: What | does the | BCH | decoder | look | like? |     |     |
| -------- | -------- | --- | ------- | ---- | ----- | --- | --- |
The decoder uses soft-decision correlation: 1. Strip the first symbol (the MSB indicator) 2. XOR
theMSBvaluebackoutoftheremaining51softsymbols3. Correlateagainstall256possibleBCH
codewords(forthe8databits)4. Selectthecodewordwithmaximumcorrelation5. Combinewith
| the MSB   | to recover | the | 9-bit        | SB1 field |     |     |     |
| --------- | ---------- | --- | ------------ | --------- | --- | --- | --- |
| Q9: Which | symbols    | get | interleaved? |           |     |     |     |
The 60×98 block interleaver applies only to the concatenation of encoded SB2 + SB3 + SB4: -
SB2: 2400 symbols - SB3: 1740 symbols - SB4: 1740 symbols - Total: 5880 symbols = 60 rows
× 98 columns
The sync pattern (68 symbols) and SB1 (52 symbols) are NOT interleaved.
Final frame structure: SP(68) || SB1(52) || Interleaved(5880) = 6000 symbols.
2

Q10: How does the interleaver work?
• Write symbols row-wise: fill row 0 left-to-right, then row 1, etc.
• Read symbols column-wise: read column 0 top-to-bottom, then column 1, etc.
The deinterleaver reverses this: write column-wise, read row-wise.
Q11: What is the CRC-24 polynomial?
The CRC-24 used in LSIS-AFS is CRC-24Q (the same as used in GPS CNAV). The generator
polynomial is:
g(X) = (1 + X) · P(X)
Where P(X) = X²³ + X¹⁷ + X¹³ + X¹² + X¹¹ + X⁹ + X⁸ + X⁷ + X⁵ + X³ + 1
In hex (25 bits, MSB first): 0x1864CFB
The CRC guards all data in SB2, SB3, and SB4 including spare bits.
Q12: How does LDPC encoding work?
The LDPC code is rate 1/2 using quasi-cyclic structure. Key points:
• TheAnnex1CSVfilescontainshiftcoefficientsforaQC-LDPCcode,notexplicitHmatrices
• Each entry is a circular shift value for a Z×Z identity submatrix (-1 means zero submatrix)
• The lifting factor Z is derived from the codeword length and base graph dimensions
• SB2: 1200 information bits → 2400 encoded symbols
• SB3/SB4: 870 information bits (846 data + 24 CRC) → 1740 encoded symbols
• SB3/SB4 use 10 filler bits (zeros) to reach the required input length
For decoding, min-sum or sum-product belief propagation with 20-50 iterations is sufficient.
Signal Generation
Q13: What is the BPSK mapping?
Per Table 8: - Logic 0 → +1.0 - Logic 1 → -1.0
This applies to both data symbols and spreading code chips.
Q14: How do symbols and chips align for AFS-I?
Each AFS-I data symbol spans exactly one Gold code period: - Gold code: 2046 chips at 1.023
Mchip/s = 2 ms per code period - Symbol rate: 500 symbols/s = 2 ms per symbol - One symbol ×
one code period = 2046 chips
The baseband signal for one symbol is: BPSK(symbol) × BPSK(code_chip) for each chip.
3

Q15: How does frame timing work?
• Frame duration: 6000 symbols × 2 ms/symbol = 12 seconds
• Block interval: 1200 seconds (100 frames)
• ITOW: 504 block intervals per week
• Week: 604800 seconds
TimeofTransmission: ToT=WN×604800+ITOW×1200+TOI×12(secondssinceLRTepoch)
Q16: How does the tertiary code align with frames?
Thetertiarycode(1500chips)startsattheframeboundary: -Chip0ofthetertiarycodecoincides
with the first chip of the sync pattern - The tertiary code period (1500 chips × secondary period)
spans exactly one frame (12 seconds)
Frame Synchronization and Decoding
Q17: What is the sync pattern?
The 68-symbol synchronization pattern in hexadecimal is:
0xCC63F74536F49E04A
This is 17 hex nibbles = 68 bits. Read MSB-first: the first transmitted symbol corresponds to the
MSB of 0xC (which is 1).
The sync pattern is transmitted uncoded (no FEC) at the start of every frame.
Q18: How does frame synchronization work?
Correlatethereceivedsymbolstreamwiththeknown68-symbolsyncpattern. Thecorrelationpeak
indicates the frame start. For coded signals (after despreading), this is a straightforward sliding
dot product.
Detection threshold should be set to achieve > 99% detection probability at SNR > 0 dB.
Q19: What LLR convention should I use for soft decoding?
There is no single "correct" convention — just be consistent. Common choices: - Positive LLR =
bit more likely to be 0 - Negative LLR = bit more likely to be 1
Or the reverse. What matters is that your BPSK demapper, deinterleaver, and LDPC decoder all
agree on the same convention.
4

Data Formats and Bit Ordering
Q20: What is the bit ordering convention?
Perthespecification: -MSB-firstbitorderingwithinbytes-MSB-firstbyteorderingwithinmulti-byte
fields - The "first" bit transmitted is the MSB
Q21: How are spare bits filled?
Spare bits in subframes are filled with alternating 0/1 starting with 0, per the specification.
Common Mistakes
1. Interleavingtheentireframe—OnlySB2+SB3+SB4(5880symbols)areinterleaved,notSP
or SB1.
2. Wrong Gold code polynomial — The PDF text extraction often garbles the mathematical
formulas. Use the polynomials stated in Q1 above.
3. Forgetting the BCH MSB step — The MSB XOR and prepend step is easy to miss. Without
it, your 52-symbol output will be wrong.
4. LDPC matrix format confusion — The Annex1 files are shift coefficients for QC-LDPC, not
dense matrices.
5. Off-by-one in Weil insertion — The insertion adds 7 bits, shifting everything after position
p. The final code is length 10230 (not 10223).
6. CRCscope—CRC-24coversALLdatabitsinthesubframeincludingsparebits,notjustthe
"useful" data.
7. Signal level mapping — Logic 0 maps to +1.0 (not -1.0). This is the opposite of some other
GNSS systems.
8. Tertiary code length — The Weil tertiary sequence is 1499 chips from the Legendre/Weil
construction, then a zero is appended to make 1500 chips total.
Errata
Appendix D — Weil Primary Parameters
PRN 62 in the appendix_d_weil_primary_params.csv file has an incorrect insertion index.
The correct value is: - PRN 62: k=2360, p=1622 (not p=6284, which is PRN 63's value)
This has been confirmed against the Annex3 reference codes.
Resources
5

| Resource |             |         | Description |               |                      |
| -------- | ----------- | ------- | ----------- | ------------- | -------------------- |
| LSIS-AFS | Volume      | A (PDF) | Primary     | specification |                      |
| Annex1   | (CSV files) |         | LDPC matrix | shift         | coefficients         |
| Annex3   | (hex files) |         | Reference   | spreading     | codes for validation |
docs/spec_tables/ Machine-readable parameter tables extracted from
the spec
Still Stuck?
If you encounter an ambiguity not covered here: 1. Check the specification figures (especially
Figures4,6,9,C-1)2. UsetheAnnex3referencecodestovalidateyourspreadingcodegenerators
3. Buildincrementally—validateeachcomponentbeforeintegrating4. Theround-triptest(encode
| → decode | → verify) | is your best debugging | tool |     |     |
| -------- | --------- | ---------------------- | ---- | --- | --- |
6
