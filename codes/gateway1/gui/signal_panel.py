"""LunaNet — Signal Generation panel (Gateway 4).

An interactive Tkinter panel that generates BPSK baseband I/Q samples from the
LunaNet spreading codes and previews / exports them. Wraps Gateway 4's signal
model:

  * AFS-I (data, in-phase):  Gold primary, BPSK(1) @ 1.023 Mchip/s, 500 sym/s
  * AFS-Q (pilot, quadrature): tiered Weil, BPSK(5) @ 5.115 Mchip/s
  * Logic→signal mapping (LSIS Table 8): logic 0 → +1.0, logic 1 → -1.0

Generation and export reuse ``codes/python/iq_generator.py`` so the GUI and the
command-line tool stay in lockstep. The C/C++ engine is reached through the
``LunaNet`` ctypes bridge.

This module is consumed by ``report_viewer.py`` as a notebook tab, but is fully
self-contained: it owns its widgets and lazily loads the native library on the
first generate so the rest of the app keeps working if the build is missing.
"""

from __future__ import annotations

import sys
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, ttk
from typing import Any, Optional

# Reuse the command-line I/Q generator and the ctypes bridge so the GUI never
# re-implements the signal model.
_PYTHON_DIR = Path(__file__).resolve().parents[2] / "python"
if str(_PYTHON_DIR) not in sys.path:
    sys.path.insert(0, str(_PYTHON_DIR))

# ── Signal constants (LSIS-AFS Table 7 / Table 9), mirrored from gateway4 ─────
AFS_I_CHIP_RATE_HZ = 1_023_000
AFS_Q_CHIP_RATE_HZ = 5_115_000
SYMBOL_RATE_HZ = 500
AFS_I_PRIMARY_CHIPS = 2046
AFS_Q_PRIMARY_CHIPS = 10230
AFS_Q_SECONDARY_CHIPS = 4
AFS_Q_TERTIARY_CHIPS = 1500
AFS_Q_TIERED_CHIPS = AFS_Q_PRIMARY_CHIPS * AFS_Q_SECONDARY_CHIPS * AFS_Q_TERTIARY_CHIPS
FRAME_DURATION_SEC = 12

# Upper bound on AFS-Q chips generated interactively. The full 12 s tiered code
# is 61.38 M chips; that is what the CLI is for. Keep the GUI responsive.
MAX_PREVIEW_Q_CHIPS = 1_000_000
DEFAULT_Q_CHIPS = AFS_Q_PRIMARY_CHIPS  # one primary epoch (2 ms)

# Number of leading chips drawn in the waveform preview per channel.
PREVIEW_CHIPS = 48


class SignalGenerationPanel(ttk.Frame):
    """Interactive Gateway 4 baseband I/Q generation panel."""

    def __init__(self, parent: tk.Misc, palette: dict[str, str], repo_root: Path) -> None:
        super().__init__(parent, style="Dark.TFrame")

        self._c = palette
        self._repo_root = Path(repo_root)
        self._config_path = self._repo_root / "config" / "spreading_codes_config.ini"

        self._bridge: Any = None     # Lazily constructed LunaNet instance.
        self._iqgen: Any = None      # Lazily imported iq_generator module.
        self._i_samples: list[float] = []
        self._q_samples: list[float] = []
        self._prn = 1

        self._ensure_styles()
        self._build_controls()
        self._build_stats()
        self._build_preview()
        self._build_status()

    # ── Lazy native-bridge loading ────────────────────────────────────────

    def _load_engine(self) -> Optional[str]:
        """Load the iq_generator module and the LunaNet bridge.

        Returns an error string on failure, or ``None`` on success.
        """
        if self._bridge is not None and self._iqgen is not None:
            return None
        try:
            import iq_generator  # type: ignore  # noqa: WPS433 (lazy import)
            from lunanet import LunaNet  # type: ignore
        except Exception as exc:  # pragma: no cover - import-time failure path
            return f"Could not import signal tooling: {exc}"

        if not self._config_path.exists():
            return f"Config not found: {self._config_path}"

        try:
            self._bridge = LunaNet(self._config_path)
        except Exception as exc:
            self._bridge = None
            return (
                "Could not load the native LunaNet library. Build it first "
                f"(cmake --build build). Details: {exc}"
            )

        self._iqgen = iq_generator
        return None

    # ── Style helpers ─────────────────────────────────────────────────────

    def _ensure_styles(self) -> None:
        """Configure the dark widget styles this panel relies on (idempotent)."""
        s = ttk.Style(self)
        c = self._c
        s.configure("Signal.TLabel", background=c["BG_DARK"], foreground=c["FG_SECONDARY"],
                    font=("Segoe UI", 10))
        s.configure("SignalValue.TLabel", background=c["BG_SURFACE"], foreground=c["FG_PRIMARY"],
                    font=("Consolas", 10))

    def _new_spinbox(self, parent: tk.Misc, var: tk.StringVar, from_: int, to: int,
                     width: int) -> tk.Spinbox:
        c = self._c
        return tk.Spinbox(
            parent, from_=from_, to=to, textvariable=var, width=width,
            bg=c["BG_SURFACE"], fg=c["FG_PRIMARY"], buttonbackground=c["BG_ELEVATED"],
            insertbackground=c["FG_PRIMARY"], highlightthickness=1,
            highlightbackground=c["BORDER_CLR"], highlightcolor=c["ACCENT"],
            relief=tk.FLAT, font=("Consolas", 10), justify=tk.RIGHT,
        )

    # ── Controls ──────────────────────────────────────────────────────────

    def _build_controls(self) -> None:
        bar = ttk.Frame(self, style="Dark.TFrame")
        bar.pack(fill=tk.X, padx=4, pady=(10, 6))

        ttk.Label(bar, text="PRN", style="Section.TLabel").pack(side=tk.LEFT, padx=(0, 6))
        self._prn_var = tk.StringVar(value="1")
        self._prn_spin = self._new_spinbox(bar, self._prn_var, 1, 210, width=5)
        self._prn_spin.pack(side=tk.LEFT, padx=(0, 16))

        ttk.Label(bar, text="AFS-Q CHIPS", style="Section.TLabel").pack(side=tk.LEFT, padx=(0, 6))
        self._qchips_var = tk.StringVar(value=str(DEFAULT_Q_CHIPS))
        self._qchips_spin = self._new_spinbox(
            bar, self._qchips_var, AFS_Q_PRIMARY_CHIPS, MAX_PREVIEW_Q_CHIPS, width=10)
        self._qchips_spin.pack(side=tk.LEFT, padx=(0, 16))

        ttk.Label(bar, text="FORMAT", style="Section.TLabel").pack(side=tk.LEFT, padx=(0, 6))
        self._format_var = tk.StringVar(value="both")
        self._format_combo = ttk.Combobox(
            bar, textvariable=self._format_var, state="readonly", width=8,
            values=["binary", "csv", "both"],
        )
        self._format_combo.pack(side=tk.LEFT, padx=(0, 16))

        self._generate_btn = ttk.Button(
            bar, text="⚡  Generate", style="Accent.TButton", command=self._on_generate)
        self._generate_btn.pack(side=tk.LEFT, padx=(0, 8))

        self._export_btn = ttk.Button(
            bar, text="⭳  Export…", style="Accent.TButton", command=self._on_export)
        self._export_btn.pack(side=tk.LEFT)
        self._export_btn.state(["disabled"])

    # ── Stats readout ─────────────────────────────────────────────────────

    def _build_stats(self) -> None:
        c = self._c
        frame = ttk.Frame(self, style="Dark.TFrame")
        frame.pack(fill=tk.X, padx=4, pady=(2, 6))

        ttk.Label(frame, text="SIGNAL PARAMETERS", style="Section.TLabel").pack(anchor=tk.W)

        self._stats = tk.Text(
            frame, height=9, wrap=tk.WORD, relief=tk.FLAT,
            bg=c["BG_SURFACE"], fg=c["FG_PRIMARY"], insertbackground=c["FG_PRIMARY"],
            font=("Consolas", 10), padx=10, pady=8, highlightthickness=1,
            highlightbackground=c["BORDER_CLR"],
        )
        self._stats.pack(fill=tk.X, pady=(4, 0))
        self._stats.configure(state=tk.DISABLED)
        self._set_stats(self._idle_stats_text())

    def _idle_stats_text(self) -> str:
        return (
            "No signal generated yet.\n\n"
            f"AFS-I (data) :  Gold {AFS_I_PRIMARY_CHIPS} chips/epoch · BPSK(1) · "
            f"{AFS_I_CHIP_RATE_HZ/1e6:.3f} Mchip/s · {SYMBOL_RATE_HZ} sym/s\n"
            f"AFS-Q (pilot):  tiered Weil · BPSK(5) · {AFS_Q_CHIP_RATE_HZ/1e6:.3f} Mchip/s\n"
            "BPSK mapping :  logic 0 → +1.0, logic 1 → -1.0  (LSIS Table 8)\n"
            f"Full frame   :  {FRAME_DURATION_SEC} s  ·  full AFS-Q tiered code = "
            f"{AFS_Q_TIERED_CHIPS:,} chips"
        )

    # ── Waveform preview ──────────────────────────────────────────────────

    def _build_preview(self) -> None:
        c = self._c
        frame = ttk.Frame(self, style="Dark.TFrame")
        frame.pack(fill=tk.BOTH, expand=True, padx=4, pady=(2, 6))

        ttk.Label(frame, text="WAVEFORM PREVIEW  (first 48 chips)",
                  style="Section.TLabel").pack(anchor=tk.W)

        self._canvas = tk.Canvas(
            frame, bg=c["BG_SURFACE"], height=240, highlightthickness=1,
            highlightbackground=c["BORDER_CLR"],
        )
        self._canvas.pack(fill=tk.BOTH, expand=True, pady=(4, 0))
        self._canvas.bind("<Configure>", lambda _evt: self._draw_preview())

    # ── Status bar ────────────────────────────────────────────────────────

    def _build_status(self) -> None:
        self._status = ttk.Label(self, text="Ready", style="Status.TLabel")
        self._status.pack(fill=tk.X, padx=4, pady=(0, 6))

    # ── Actions ───────────────────────────────────────────────────────────

    def _read_int(self, var: tk.StringVar, lo: int, hi: int, default: int) -> int:
        try:
            value = int(var.get())
        except (TypeError, ValueError):
            return default
        return max(lo, min(hi, value))

    def _on_generate(self) -> None:
        self._set_status("Loading engine…")
        self.update_idletasks()

        err = self._load_engine()
        if err is not None:
            self._set_status(err)
            return

        prn = self._read_int(self._prn_var, 1, 210, 1)
        q_chips = self._read_int(
            self._qchips_var, AFS_Q_PRIMARY_CHIPS, MAX_PREVIEW_Q_CHIPS, DEFAULT_Q_CHIPS)

        self._generate_btn.state(["disabled"])
        self._set_status(f"Generating I/Q for PRN {prn} ({q_chips:,} AFS-Q chips)…")
        self.update_idletasks()

        try:
            i_samples, q_samples = self._iqgen.generate_iq(self._bridge, prn, q_chips)
        except Exception as exc:
            self._set_status(f"Generation failed: {exc}")
            self._generate_btn.state(["!disabled"])
            return

        self._i_samples = i_samples
        self._q_samples = q_samples
        self._prn = prn

        self._set_stats(self._result_stats_text(prn, i_samples, q_samples))
        self._draw_preview()
        self._export_btn.state(["!disabled"])
        self._generate_btn.state(["!disabled"])
        self._set_status(
            f"Generated PRN {prn}: AFS-I {len(i_samples):,} samples, "
            f"AFS-Q {len(q_samples):,} samples. Ready to export.")

    def _on_export(self) -> None:
        if not self._i_samples or not self._q_samples:
            self._set_status("Nothing to export — generate a signal first.")
            return

        default_dir = self._repo_root / "Validation" / "iq_output"
        default_dir.mkdir(parents=True, exist_ok=True)
        chosen = filedialog.askdirectory(
            title="Choose I/Q output directory", initialdir=str(default_dir))
        if not chosen:
            self._set_status("Export cancelled.")
            return

        out_dir = Path(chosen)
        prefix = f"prn{self._prn:03d}"
        fmt = self._format_var.get()
        written: list[str] = []

        try:
            if fmt in ("binary", "both"):
                i_path = out_dir / f"{prefix}_afs_i.bin"
                q_path = out_dir / f"{prefix}_afs_q.bin"
                iq_path = out_dir / f"{prefix}_iq_interleaved.bin"
                self._iqgen.write_float32_binary(self._i_samples, i_path)
                self._iqgen.write_float32_binary(self._q_samples, q_path)
                n = min(len(self._i_samples), len(self._q_samples))
                self._iqgen.write_interleaved_iq(
                    self._i_samples[:n], self._q_samples[:n], iq_path)
                written += [i_path.name, q_path.name, iq_path.name]
            if fmt in ("csv", "both"):
                csv_path = out_dir / f"{prefix}_iq.csv"
                n = min(len(self._i_samples), len(self._q_samples))
                self._iqgen.write_csv(self._i_samples[:n], self._q_samples[:n], csv_path)
                written.append(csv_path.name)
        except OSError as exc:
            self._set_status(f"Export failed: {exc}")
            return

        self._set_status(f"Exported {len(written)} file(s) to {out_dir}: {', '.join(written)}")

    # ── Rendering helpers ─────────────────────────────────────────────────

    def _set_status(self, text: str) -> None:
        self._status.configure(text=text)

    def _set_stats(self, text: str) -> None:
        self._stats.configure(state=tk.NORMAL)
        self._stats.delete("1.0", tk.END)
        self._stats.insert(tk.END, text)
        self._stats.configure(state=tk.DISABLED)

    def _result_stats_text(self, prn: int, i_samples: list[float],
                           q_samples: list[float]) -> str:
        i_dur_ms = len(i_samples) / AFS_I_CHIP_RATE_HZ * 1e3
        q_dur_ms = len(q_samples) / AFS_Q_CHIP_RATE_HZ * 1e3
        return (
            f"PRN          :  {prn}\n"
            f"AFS-I (data) :  {len(i_samples):,} chips · {AFS_I_CHIP_RATE_HZ/1e6:.3f} "
            f"Mchip/s · {i_dur_ms:.3f} ms · BPSK(1)\n"
            f"AFS-Q (pilot):  {len(q_samples):,} chips · {AFS_Q_CHIP_RATE_HZ/1e6:.3f} "
            f"Mchip/s · {q_dur_ms:.3f} ms · BPSK(5)\n"
            f"Symbol rate  :  {SYMBOL_RATE_HZ} sym/s  ·  "
            f"{AFS_I_PRIMARY_CHIPS} AFS-I chips per data symbol\n"
            f"BPSK levels  :  logic 0 → +1.0, logic 1 → -1.0  (LSIS Table 8)\n"
            f"Sample values:  I,Q ∈ {{-1.0, +1.0}} (float32 on export)\n"
            f"Full frame   :  {FRAME_DURATION_SEC} s = {AFS_Q_TIERED_CHIPS:,} AFS-Q chips "
            f"(tertiary code → frame start, LSIS-220)"
        )

    def _draw_preview(self) -> None:
        canvas = self._canvas
        canvas.delete("all")
        c = self._c

        width = canvas.winfo_width()
        height = canvas.winfo_height()
        if width < 10 or height < 10:
            return

        if not self._i_samples or not self._q_samples:
            canvas.create_text(
                width / 2, height / 2, fill=c["FG_MUTED"],
                font=("Segoe UI", 11),
                text="Generate a signal to preview the BPSK waveform")
            return

        self._draw_channel(canvas, "AFS-I", self._i_samples, c["ACCENT"],
                           y_top=18, y_bottom=height / 2 - 8, width=width)
        self._draw_channel(canvas, "AFS-Q", self._q_samples, c["ACCENT_YELLOW"],
                           y_top=height / 2 + 18, y_bottom=height - 18, width=width)

    def _draw_channel(self, canvas: tk.Canvas, label: str, samples: list[float],
                      color: str, y_top: float, y_bottom: float, width: float) -> None:
        c = self._c
        left = 70.0
        right = width - 16.0
        if right <= left:
            return

        y_high = y_top + 6
        y_low = y_bottom - 6
        y_mid = (y_high + y_low) / 2

        # Zero baseline and channel label.
        canvas.create_line(left, y_mid, right, y_mid, fill=c["BORDER_CLR"], dash=(2, 3))
        canvas.create_text(8, y_mid, anchor=tk.W, fill=color,
                           font=("Segoe UI Semibold", 10), text=label)
        canvas.create_text(right, y_top, anchor=tk.NE, fill=c["FG_MUTED"],
                           font=("Consolas", 8), text="+1")
        canvas.create_text(right, y_bottom, anchor=tk.SE, fill=c["FG_MUTED"],
                           font=("Consolas", 8), text="-1")

        n = min(PREVIEW_CHIPS, len(samples))
        if n < 2:
            return
        step = (right - left) / n

        # Build a stepped (sample-and-hold) BPSK waveform: +1.0 → y_high, -1.0 → y_low.
        points: list[float] = []
        for idx in range(n):
            y = y_high if samples[idx] > 0 else y_low
            x0 = left + idx * step
            x1 = left + (idx + 1) * step
            points.extend([x0, y, x1, y])
        canvas.create_line(*points, fill=color, width=2)
