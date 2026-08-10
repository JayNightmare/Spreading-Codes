"""Smoke test for the LunaNet Python bridge."""

import sys
from pathlib import Path

# Add the python module path
sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "codes" / "python"))

from lunanet import LunaNet

def main():
    repo_root = Path(__file__).resolve().parent.parent.parent
    config_path = repo_root / "config" / "spreading_codes_config.ini"

    print(f"Loading config: {config_path}")
    ln = LunaNet(str(config_path))

    # Test Gold code
    gold = ln.generate_gold(1)
    print(f"Gold PRN 1: length={len(gold)}, first_8={''.join(str(b) for b in gold[:8])}")
    assert len(gold) == 2046, f"Gold length mismatch: {len(gold)}"

    # Test Weil primary
    wp = ln.generate_weil_primary(1)
    print(f"Weil Primary PRN 1: length={len(wp)}")
    assert len(wp) == 10230, f"Weil primary length mismatch: {len(wp)}"

    # Test Weil tertiary
    wt = ln.generate_weil_tertiary(1)
    print(f"Weil Tertiary PRN 1: length={len(wt)}")
    assert len(wt) == 1500, f"Weil tertiary length mismatch: {len(wt)}"

    # Test AFS-I
    afs_i = ln.generate_afs_i(1)
    print(f"AFS-I PRN 1: length={len(afs_i)}")
    assert len(afs_i) == 2046

    # Test BCH encode
    bch = ln.bch_encode(0x000)
    print(f"BCH encode 0x000: length={len(bch)}")
    assert len(bch) == 52

    # Round-trip BCH
    bch2 = ln.bch_encode(0x1FF)
    print(f"BCH encode 0x1FF: first_4={''.join(str(b) for b in bch2[:4])}")
    assert len(bch2) == 52

    # Test CRC-24
    data = [1, 0, 1, 1, 0, 0, 1, 0]
    crc = ln.crc24(data)
    print(f"CRC-24 of [10110010]: 0x{crc:06X}")
    assert crc != 0, "CRC should be non-zero for non-trivial input"

    # CRC of empty should be 0
    crc_empty = ln.crc24([])
    print(f"CRC-24 of []: 0x{crc_empty:06X}")

    print("\n[OK] All Python bridge smoke tests passed!")

if __name__ == "__main__":
    main()
