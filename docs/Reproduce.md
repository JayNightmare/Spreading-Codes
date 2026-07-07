# Reproduce Build and Validation

This document provides reproducible build and validation commands for Windows and Linux.

## Scope

- Build all targets using CMake
- Run the full validation suite
- Run per-gateway validation (Gateway 1 to Gateway 4)

## Prerequisites

### Windows

- CMake 3.16+
- Visual Studio 2019+ with C++ workload (or Ninja + MSVC/Clang toolchain)

### Linux

- CMake 3.16+
- C++17 compiler (GCC 9+ or Clang 10+)
- Build tools (`make` or `ninja`)

## Repository Root

Run all commands from the repository root:

```powershell
# Windows PowerShell
cd <path>\Spreading-Codes
```

```bash
# Linux
cd /path/to/Spreading-Codes
```

## Build (Windows)

### Option A: Visual Studio Generator (multi-config)

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Expected key outputs:

- `build/bin/Release/test_engine.exe`
- `build/bin/Release/goon.exe`
- `build/bin/Release/lunanet_spreading_codes.dll`

### Option B: Ninja (single-config)

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Expected key outputs:

- `build/bin/test_engine.exe`
- `build/bin/goon.exe`
- `build/bin/lunanet_spreading_codes.dll`

## Build (Linux)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Expected key outputs:

- `build/bin/test_engine`
- `build/bin/goon`
- `build/lib/liblunanet_spreading_codes.so`

## Validation (Windows)

### Full Validation via test_engine

Visual Studio generator:

```powershell
.\build\bin\Release\test_engine.exe config\spreading_codes_config.ini
```

Ninja generator:

```powershell
.\build\bin\test_engine.exe config\spreading_codes_config.ini
```

### Gateway-Scoped Validation via test_engine

Visual Studio generator:

```powershell
.\build\bin\Release\test_engine.exe config\spreading_codes_config.ini --gateway gateway1
.\build\bin\Release\test_engine.exe config\spreading_codes_config.ini --gateway gateway2
.\build\bin\Release\test_engine.exe config\spreading_codes_config.ini --gateway gateway3
.\build\bin\Release\test_engine.exe config\spreading_codes_config.ini --gateway gateway4
```

Ninja generator:

```powershell
.\build\bin\test_engine.exe config\spreading_codes_config.ini --gateway gateway1
.\build\bin\test_engine.exe config\spreading_codes_config.ini --gateway gateway2
.\build\bin\test_engine.exe config\spreading_codes_config.ini --gateway gateway3
.\build\bin\test_engine.exe config\spreading_codes_config.ini --gateway gateway4
```

### CTest Targets

```powershell
ctest --test-dir build --output-on-failure
ctest --test-dir build -R gateway1_validation --output-on-failure
ctest --test-dir build -R gateway2_validation --output-on-failure
ctest --test-dir build -R gateway3_validation --output-on-failure
ctest --test-dir build -R gateway4_validation --output-on-failure
```

## Validation (Linux)

### Full Validation via test_engine

```bash
./build/bin/test_engine config/spreading_codes_config.ini
```

### Gateway-Scoped Validation via test_engine

```bash
./build/bin/test_engine config/spreading_codes_config.ini --gateway gateway1
./build/bin/test_engine config/spreading_codes_config.ini --gateway gateway2
./build/bin/test_engine config/spreading_codes_config.ini --gateway gateway3
./build/bin/test_engine config/spreading_codes_config.ini --gateway gateway4
```

### CTest Targets

```bash
ctest --test-dir build --output-on-failure
ctest --test-dir build -R gateway1_validation --output-on-failure
ctest --test-dir build -R gateway2_validation --output-on-failure
ctest --test-dir build -R gateway3_validation --output-on-failure
ctest --test-dir build -R gateway4_validation --output-on-failure
```

## Reports

Validation reports are written under:

- `Validation/reports/YYYY-MM-DD/HH-MM-SS.md`
- `Validation/reports/YYYY-MM-DD/HH-MM-SS.xml`

Gateway-filtered runs use the suffix `_gatewayX` in the filename.

## Troubleshooting

- If you get command not found or exit code 127 on Linux, verify you are using the correct binary path:
  - Multi-config build trees may place binaries in `build/bin/Release/`
  - Single-config Linux/Ninja builds usually place binaries in `build/bin/`
- If CTest cannot find tests, re-run CMake configure:

```bash
cmake -S . -B build
```

- If shared library loading fails on Linux, ensure the binary and shared library were built in the same build tree and run from repository root.
