# Changelog

## Unreleased
### Fixed
- **Gold Code Validation**: Corrected LFSR G2 initialization by computationally advancing the sequence `2047 - D_k` steps to correctly realize the telecommunications mathematical delay $G_2(t - D_k)$. LFSR now perfectly matches Annex3 test vectors for all 210 PRNs.
- **Generator Alignment**: Resolved mathematical dissonance by restoring the left-shifting Fibonacci implementation that correctly aligns with the characteristic polynomial specification.
