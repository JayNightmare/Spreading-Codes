# KURE — LSIS-AFS Feedback

## Level 0: Gold Codes

**Status**: FAIL (format issue)

Your `codes.txt` has 211 lines instead of 210, and includes formatting from a Python/JSON array.

### Problem

Your file looks like:

```txt
GoldCode2046 = [ \
"17590C193EB85C40...",
"130BFFDA4199FCFE...",
...
"290F86BE376A91DF..."]
```

### Required format

Plain text, 210 lines, no headers, no quotes, no array syntax:

```txt
17590C193EB85C40632F5B9919BEEB8D...
130BFFDA4199FCFE84D51D04CC16A7A2...
...
```

Each line is exactly 512 hex characters (uppercase or lowercase), terminated by a newline. No other content.

### Fix

Strip the header line, remove all quote characters, commas, brackets, and backslashes. The actual hex values appear correct (PRN 1 starts `17590C`), so it's purely a formatting issue.

---

## Level 1: Frame Encoding

**Status**: FAIL (wrong TOI value)

Your frame encodes **TOI=62** instead of the required **TOI=42**.

### Expected parameters (from onboarding guide)

```txt
PRN=1, FID=0, TOI=42, WN=100, ITOW=250
```

Your SB1 data bits decode as FID=0, TOI=62 — the encoding pipeline itself appears correct (BCH parity is consistent with your data bits), you just used the wrong TOI input.

### Fix

Regenerate with `--toi 42` (or equivalent in your implementation).

---

## Level 2: Signal

**Status**: FAIL (wrong TOI value)

Same issue — your signal decodes to TOI=58 (possibly a different run than the frame, which has TOI=62). Both should be TOI=42.

### Fix

Regenerate with the canonical parameters:

```txt
--prn 1 --fid 0 --toi 42 --wn 100 --itow 250 --format iq32 --rate 1023000
```

---

## What's Correct

- Gold code values are correct (PRN 1 = `17590C...`)
- BCH encoder is working properly
- Frame structure (sync pattern, symbol format) is correct
- Signal format (98,208,000 bytes, float32 LE I/Q) is correct
- All CRCs pass when decoded

The only issues are file formatting (codes.txt) and using wrong input values (TOI).
