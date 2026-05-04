"""LunaNet Python bridge — ctypes wrapper over the C API DLL.

Provides zero-dependency access to Gateway 1 spreading code generators
and Gateway 2 FEC encoders from Python.

Usage:
    from lunanet import LunaNet
    ln = LunaNet("path/to/config.ini")
    gold = ln.generate_gold(1)
"""

from __future__ import annotations

import ctypes
import os
import platform
import sys
from pathlib import Path
from typing import Optional


def _find_dll(search_dirs: Optional[list[Path]] = None) -> Path:
    """Locate the lunanet_spreading_codes shared library."""
    system = platform.system()
    if system == "Windows":
        names = ["lunanet_spreading_codes.dll"]
    elif system == "Darwin":
        names = ["liblunanet_spreading_codes.dylib"]
    else:
        names = ["liblunanet_spreading_codes.so"]

    candidates = search_dirs or []

    # Add default search paths relative to this file
    module_dir = Path(__file__).resolve().parent
    repo_root = module_dir.parent.parent
    candidates.extend([
        repo_root / "build" / "bin" / "Release",
        repo_root / "build" / "bin" / "Debug",
        repo_root / "build" / "bin",
        repo_root / "build" / "lib",
        repo_root / "build",
    ])

    for directory in candidates:
        for name in names:
            candidate = directory / name
            if candidate.exists():
                return candidate

    raise FileNotFoundError(
        f"Could not find lunanet shared library. Searched: {[str(d) for d in candidates]}"
    )


class LunaNet:
    """Python bridge to the LunaNet C API via ctypes."""

    GOLD_CODE_LENGTH = 2046
    WEIL_PRIMARY_LENGTH = 10230
    WEIL_TERTIARY_LENGTH = 1500
    BCH_ENCODED_LENGTH = 52

    def __init__(
        self,
        config_path: str | Path,
        dll_path: Optional[str | Path] = None,
    ):
        if dll_path is not None:
            lib_path = Path(dll_path)
        else:
            lib_path = _find_dll()

        # On Windows, add DLL directory to search path
        if platform.system() == "Windows":
            dll_dir = str(lib_path.parent)
            os.add_dll_directory(dll_dir)

        self._lib = ctypes.CDLL(str(lib_path))
        self._setup_prototypes()

        self._lib.lunanet_init()

        config_str = str(Path(config_path).resolve())
        result = self._lib.lunanet_load_config(config_str.encode("utf-8"))
        if result != 0:
            error = self._lib.lunanet_last_error()
            raise RuntimeError(f"Failed to load config: {error.decode() if error else 'unknown error'}")

    def _setup_prototypes(self):
        """Declare ctypes function signatures for type safety."""
        lib = self._lib

        lib.lunanet_init.argtypes = []
        lib.lunanet_init.restype = None

        lib.lunanet_load_config.argtypes = [ctypes.c_char_p]
        lib.lunanet_load_config.restype = ctypes.c_int

        lib.lunanet_last_error.argtypes = []
        lib.lunanet_last_error.restype = ctypes.c_char_p

        lib.lunanet_generate_gold.argtypes = [
            ctypes.c_int, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int
        ]
        lib.lunanet_generate_gold.restype = ctypes.c_int

        lib.lunanet_generate_weil_primary.argtypes = [
            ctypes.c_int, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int
        ]
        lib.lunanet_generate_weil_primary.restype = ctypes.c_int

        lib.lunanet_generate_weil_tertiary.argtypes = [
            ctypes.c_int, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int
        ]
        lib.lunanet_generate_weil_tertiary.restype = ctypes.c_int

        lib.lunanet_generate_afs_i.argtypes = [
            ctypes.c_int, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int
        ]
        lib.lunanet_generate_afs_i.restype = ctypes.c_int

        lib.lunanet_generate_afs_q.argtypes = [
            ctypes.c_int, ctypes.c_int,
            ctypes.POINTER(ctypes.c_uint8), ctypes.c_int
        ]
        lib.lunanet_generate_afs_q.restype = ctypes.c_int

        lib.lunanet_gold_code_length.argtypes = []
        lib.lunanet_gold_code_length.restype = ctypes.c_int

        lib.lunanet_weil_primary_length.argtypes = []
        lib.lunanet_weil_primary_length.restype = ctypes.c_int

        lib.lunanet_weil_tertiary_length.argtypes = []
        lib.lunanet_weil_tertiary_length.restype = ctypes.c_int

        lib.lunanet_bch_encode.argtypes = [
            ctypes.c_int, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int
        ]
        lib.lunanet_bch_encode.restype = ctypes.c_int

        lib.lunanet_crc24.argtypes = [
            ctypes.POINTER(ctypes.c_uint8), ctypes.c_int
        ]
        lib.lunanet_crc24.restype = ctypes.c_uint32

    def _generate_code(self, func, prn: int, length: int) -> list[int]:
        """Call a code generation function and return the result as a list."""
        buf = (ctypes.c_uint8 * length)()
        count = func(prn, buf, length)
        if count <= 0:
            raise RuntimeError(f"Code generation failed for PRN {prn}")
        return list(buf[:count])

    def generate_gold(self, prn: int) -> list[int]:
        """Generate a Gold code (2046 chips) for the given PRN (1-210)."""
        return self._generate_code(self._lib.lunanet_generate_gold, prn, self.GOLD_CODE_LENGTH)

    def generate_weil_primary(self, prn: int) -> list[int]:
        """Generate a Weil primary code (10230 chips) for the given PRN."""
        return self._generate_code(self._lib.lunanet_generate_weil_primary, prn, self.WEIL_PRIMARY_LENGTH)

    def generate_weil_tertiary(self, prn: int) -> list[int]:
        """Generate a Weil tertiary code (1500 chips) for the given PRN."""
        return self._generate_code(self._lib.lunanet_generate_weil_tertiary, prn, self.WEIL_TERTIARY_LENGTH)

    def generate_afs_i(self, prn: int) -> list[int]:
        """Generate an AFS-I code (= Gold code) for the given PRN."""
        return self._generate_code(self._lib.lunanet_generate_afs_i, prn, self.GOLD_CODE_LENGTH)

    def generate_afs_q(self, prn: int, max_chips: int = 0) -> list[int]:
        """Generate an AFS-Q tiered code for the given PRN."""
        capacity = max_chips if max_chips > 0 else (self.WEIL_PRIMARY_LENGTH * 4 * self.WEIL_TERTIARY_LENGTH)
        buf = (ctypes.c_uint8 * capacity)()
        count = self._lib.lunanet_generate_afs_q(prn, max_chips, buf, capacity)
        if count <= 0:
            raise RuntimeError(f"AFS-Q generation failed for PRN {prn}")
        return list(buf[:count])

    def bch_encode(self, sb1_data: int) -> list[int]:
        """BCH(51,8) encode a 9-bit SB1 value. Returns 52 symbols."""
        buf = (ctypes.c_uint8 * self.BCH_ENCODED_LENGTH)()
        count = self._lib.lunanet_bch_encode(sb1_data, buf, self.BCH_ENCODED_LENGTH)
        if count <= 0:
            raise RuntimeError(f"BCH encode failed for SB1={sb1_data:#05x}")
        return list(buf[:count])

    def crc24(self, data: list[int]) -> int:
        """Compute CRC-24Q over a bit vector. Returns 24-bit CRC value."""
        arr = (ctypes.c_uint8 * len(data))(*data)
        return self._lib.lunanet_crc24(arr, len(data))
