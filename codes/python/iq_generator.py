"""BPSK(1) I/Q signal generator for AFS validation.

Generates binary I/Q sample files from spreading codes using the BPSK mapping:
  Logic 0 → +1.0
  Logic 1 → -1.0

Output formats:
  - float32 binary (interleaved I/Q or separate I and Q files)
  - CSV (for analysis)

Usage:
    python iq_generator.py --config ../../config/spreading_codes_config.ini --prn 1 --output output/
"""

from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path
from typing import Optional

sys.path.insert(0, str(Path(__file__).resolve().parent))
from lunanet import LunaNet


def bpsk_map(chips: list[int]) -> list[float]:
    """Map binary chips to BPSK baseband: 0 → +1.0, 1 → -1.0."""
    return [1.0 if c == 0 else -1.0 for c in chips]


def write_float32_binary(samples: list[float], path: Path) -> None:
    """Write samples as raw float32 binary."""
    with open(path, "wb") as f:
        for s in samples:
            f.write(struct.pack("<f", s))


def write_interleaved_iq(
    i_samples: list[float],
    q_samples: list[float],
    path: Path,
) -> None:
    """Write interleaved I/Q pairs as float32: [I0, Q0, I1, Q1, ...]."""
    with open(path, "wb") as f:
        for i_val, q_val in zip(i_samples, q_samples):
            f.write(struct.pack("<ff", i_val, q_val))


def write_csv(
    i_samples: list[float],
    q_samples: list[float],
    path: Path,
) -> None:
    """Write I/Q samples as CSV: index, I, Q."""
    with open(path, "w") as f:
        f.write("index,I,Q\n")
        for idx, (i_val, q_val) in enumerate(zip(i_samples, q_samples)):
            f.write(f"{idx},{i_val:.1f},{q_val:.1f}\n")


def generate_iq(
    ln: LunaNet,
    prn: int,
    max_q_chips: int = 0,
) -> tuple[list[float], list[float]]:
    """Generate I and Q channel BPSK baseband samples for a given PRN.

    Args:
        ln: Initialized LunaNet bridge instance.
        prn: PRN number (1-210).
        max_q_chips: Maximum AFS-Q chips (0 = full length).

    Returns:
        Tuple of (I_samples, Q_samples) as float lists.
    """
    afs_i_chips = ln.generate_afs_i(prn)
    afs_q_chips = ln.generate_afs_q(prn, max_q_chips)

    i_samples = bpsk_map(afs_i_chips)
    q_samples = bpsk_map(afs_q_chips)

    return i_samples, q_samples


def main():
    parser = argparse.ArgumentParser(description="Generate BPSK(1) I/Q sample files")
    parser.add_argument("--config", required=True, help="Path to spreading_codes_config.ini")
    parser.add_argument("--prn", type=int, required=True, help="PRN number (1-210)")
    parser.add_argument("--output", required=True, help="Output directory")
    parser.add_argument("--max-q-chips", type=int, default=10230,
                        help="Max AFS-Q chips (default: 10230 = 1 primary epoch)")
    parser.add_argument("--format", choices=["binary", "csv", "both"], default="both",
                        help="Output format (default: both)")
    parser.add_argument("--dll", default=None, help="Path to DLL (auto-detect if omitted)")
    args = parser.parse_args()

    output_dir = Path(args.output)
    output_dir.mkdir(parents=True, exist_ok=True)

    print(f"Initializing LunaNet engine...")
    ln = LunaNet(args.config, dll_path=args.dll)

    print(f"Generating I/Q for PRN {args.prn} (max_q_chips={args.max_q_chips})...")
    i_samples, q_samples = generate_iq(ln, args.prn, args.max_q_chips)

    prefix = f"prn{args.prn:03d}"
    print(f"  AFS-I: {len(i_samples)} samples")
    print(f"  AFS-Q: {len(q_samples)} samples")

    if args.format in ("binary", "both"):
        # Separate I and Q files
        i_path = output_dir / f"{prefix}_afs_i.bin"
        q_path = output_dir / f"{prefix}_afs_q.bin"
        write_float32_binary(i_samples, i_path)
        write_float32_binary(q_samples, q_path)
        print(f"  Binary I: {i_path} ({i_path.stat().st_size} bytes)")
        print(f"  Binary Q: {q_path} ({q_path.stat().st_size} bytes)")

        # Interleaved I/Q (uses min length of I and Q)
        min_len = min(len(i_samples), len(q_samples))
        iq_path = output_dir / f"{prefix}_iq_interleaved.bin"
        write_interleaved_iq(i_samples[:min_len], q_samples[:min_len], iq_path)
        print(f"  Interleaved I/Q: {iq_path} ({iq_path.stat().st_size} bytes)")

    if args.format in ("csv", "both"):
        min_len = min(len(i_samples), len(q_samples))
        csv_path = output_dir / f"{prefix}_iq.csv"
        write_csv(i_samples[:min_len], q_samples[:min_len], csv_path)
        print(f"  CSV: {csv_path}")

    print(f"\n[OK] I/Q generation complete for PRN {args.prn}")


if __name__ == "__main__":
    main()
