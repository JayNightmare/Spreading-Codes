# Build & Run Instructions

## Prerequisites

macOS (Homebrew):

```bash
xcode-select --install
brew install cmake ninja python
```

Ubuntu/Debian (APT):

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build python3 python3-pip python3-tk
```

Python packages:

```bash
# No third-party pip packages are required for core build/tests/examples.
python3 --version
```

## Build

From repository root:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Alternative multi-config build command (works across generators):

```bash
cmake -S . -B build
cmake --build build --config Release
```

## Run Tests

Run CTest target registered by CMake:

```bash
ctest --test-dir build --output-on-failure
```

Run the full validation harness directly:

```bash
./build/bin/test_engine config/spreading_codes_config.ini
```

## Run Examples

Python bridge smoke test:

```bash
python3 codes/python/test_bridge.py
```

Generate example BPSK I/Q outputs for PRN 1:

```bash
python3 codes/python/iq_generator.py \
  --config config/spreading_codes_config.ini \
  --prn 1 \
  --output Validation/iq_output \
  --format both
```

Optional GUI report viewer:

```bash
python3 codes/gateway1/gui/report_viewer.py
```
