# Minerva: Efficient ML Model Updates for Deeply Embedded Microcontrollers

Minerva is an end-to-end system for enabling efficient, state-preserving ML model updates on deeply embedded microcontrollers (MCUs). Instead of performing full device firmware updates (DFUs) every time an ML model changes, Minerva isolates model code and weights into **capsules** -- fixed memory regions with a stable `predict()` entrypoint. This allows lightweight, non-disruptive updates (weights-only, operators-only, or full capsule) without rebooting the device.

**Paper:** *Efficient ML Model Updates for Deeply Embedded Microcontrollers* (EuroSys 2025, Submission #622)

## Table of Contents

- [Overview](#overview)
- [Repository Structure](#repository-structure)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Getting Started](#getting-started)
  - [1. Install the Toolchain](#1-install-the-toolchain)
  - [2. Clone and Set Up the Repository](#2-clone-and-set-up-the-repository)
  - [3. Build a Capsule Application](#3-build-a-capsule-application)
  - [4. Flash to Hardware](#4-flash-to-hardware)
- [How Capsule Updates Work](#how-capsule-updates-work)
- [Applications](#applications)
- [Update Types](#update-types)
- [Reproducing Results](#reproducing-results)
  - [With Hardware](#with-hardware)
  - [Without Hardware](#without-hardware)
- [Evaluation Logs](#evaluation-logs)
- [BLE DFU Baseline](#ble-dfu-baseline)
- [HTTP FOTA Baseline](#http-fota-baseline)
- [Expected Evaluation Time](#expected-evaluation-time)
- [License](#license)

## Overview

On resource-constrained MCUs (e.g., ARM Cortex-M4 on nRF52840 with 256 KB RAM and 1 MB Flash), updating an ML model traditionally requires a full firmware update -- reflashing the entire application binary, rebooting the device, and losing runtime state. This is costly in terms of bandwidth, downtime, and energy.

Minerva introduces **capsules**: linker-isolated memory regions that contain only the ML model's inference code (operators) and weights. The key properties are:

1. **Fixed entrypoint:** The `predict()` function always resides at a known address. Application code calls it without needing to know what model is inside.
2. **Linker-based isolation:** A custom linker script (`link.ld`) places capsule code in `.container-code` and capsule data in `.container-data` sections, separate from the main application.
3. **In-place swap:** The `switch_container()` function copies new capsule contents into the designated memory region at runtime -- no reboot required.
4. **Integrity verification:** SHA-256 hashes are computed and verified before applying an update (`check_hash()`).
5. **Selective updates:** Only the changed portion (weights, operators, or both) needs to be transmitted, dramatically reducing update size.

## Repository Structure

```
minerva/
├── container-update/              # Core capsule update framework
│   ├── container-update.h         #   switch_container() and check_hash() implementation
│   ├── link.ld                    #   Linker script defining capsule memory regions
│   ├── gen-byte-array.py          #   Extracts capsule sections from ELF into C byte arrays
│   ├── sha256.h                   #   SHA-256 implementation for integrity checks
│   ├── checksum.h                 #   Generated hash values for verification
│   ├── contents.h                 #   Generated capsule contents (byte arrays)
│   ├── hash.c                     #   Hash computation utility
│   └── run.sh                     #   End-to-end build-and-flash script
│
├── improved-octo-doodle/          # Main project with capsule-enabled applications
│   └── src/
│       ├── container-update/      #   Shared copy of capsule framework
│       └── apps/                  #   ML application examples
│           ├── farmbeats/         #     Agricultural sensor classification
│           ├── gesturepod/        #     Gesture recognition
│           ├── Powerblade/        #     Power monitoring
│           ├── spektacom/         #     Cricket bat sensor analytics
│           ├── container/         #     Basic UART + LED demo
│           ├── diff/              #     Differential update demo
│           └── esp/               #     UART echo with predictor
│
├── ble_application_update/        # BLE OTA DFU baseline (Nordic Secure Bootloader)
│   ├── secure_bootloader/         #   Bootloader variants for nRF52832/nRF52840
│   └── README.md                  #   Setup walkthrough
│
├── http_application_update/       # HTTP FOTA baseline (Zephyr + nRF9160)
│   ├── src/main.c                 #   Capsule-only download
│   ├── src/main2.c                #   Full DFU download
│   └── README.rst                 #   Setup instructions
│
├── evaluation_logs/               # Raw timing logs from hardware experiments
├── design.md                      # Design philosophy notes
└── makefile-hierarchy.md          # Build system documentation
```

## Hardware Requirements

### Required for full reproduction (flashing + timing)

| Component | Details |
|-----------|---------|
| **Development Board** | Nordic nRF52840 DK (PCA10056) -- recommended, or nRF52832 DK (PCA10040) |
| **Debugger** | SEGGER J-Link (integrated on Nordic DKs) |
| **USB Cable** | Micro-USB for power and J-Link connection |

### Optional

| Component | Details |
|-----------|---------|
| **nRF9160 DK** | Required only for HTTP FOTA baseline experiments |
| **USB Power Meter** | For energy consumption measurements |
| **Android Phone** | For BLE DFU baseline testing (with nRF Connect app) |

## Software Requirements

| Tool | Version | Purpose |
|------|---------|---------|
| `arm-none-eabi-gcc` | 7.x+ | ARM cross-compiler toolchain |
| `python3` | 3.8+ | Capsule extraction scripts |
| `make` | GNU Make | Build system |
| Nordic nRF5 SDK | v15.3.0 | Device HAL and drivers |
| `nrfjprog` | Latest | Flash programming (from nRF Command Line Tools) |
| J-Link Software | Latest | Debugger interface |
| `nrfutil` | 6.0+ | DFU package generation (BLE baseline only) |

### Installing the ARM toolchain

**macOS:**
```bash
brew install armmbed/formulae/arm-none-eabi-gcc
```

**Ubuntu/Debian:**
```bash
sudo apt-get install gcc-arm-none-eabi
```

**Or download directly:**
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm

### Installing Nordic tools

1. Download the nRF5 SDK v15.3.0 from https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK
2. Install nRF Command Line Tools from https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Command-Line-Tools

## Getting Started

### 1. Install the Toolchain

Ensure `arm-none-eabi-gcc`, `make`, `python3`, `nrfjprog`, and J-Link software are installed and on your `PATH`:

```bash
arm-none-eabi-gcc --version
python3 --version
nrfjprog --version
```

### 2. Clone and Set Up the Repository

```bash
git clone https://github.com/ShishirPatil/minerva-eurosys-artifact.git
cd minerva-eurosys-artifact
```

Ensure the nRF5 SDK is available. The build system expects it at the path configured in `improved-octo-doodle/src/nrf52x-base/`. Check that `SDK_ROOT` points to your SDK installation (see `makefile-hierarchy.md` for details).

### 3. Build a Capsule Application

Each application in `improved-octo-doodle/src/apps/` follows the same workflow. We'll use **farmbeats** as an example:

```bash
cd improved-octo-doodle/src/apps/farmbeats
```

**Option A: Full automated build + flash** (requires hardware):
```bash
../../container-update/run.sh
```

This script performs the complete capsule update workflow:
1. Builds the application with the **new** model/utils (capsule contents)
2. Extracts the capsule sections from the ELF into C byte arrays (`contents.h`)
3. Rebuilds the application with the **old** model/utils (base firmware) embedding the new capsule
4. Flashes the firmware to the device

**Option B: Build only** (no hardware required):
```bash
# Build with the current model
make

# Inspect the generated binary
arm-none-eabi-size _build/*.elf
arm-none-eabi-objdump -h _build/*.elf    # View section layout

# Extract capsule sections
python3 container-update/gen-byte-array.py \
    _build/*.elf \
    container-update/contents.h \
    .container-code \
    .container-data
```

### 4. Flash to Hardware

With a Nordic DK connected via USB:

```bash
make flash
```

This uses `nrfjprog` and J-Link to program the device. After flashing, open a serial terminal (115200 baud) to observe predictions:

```bash
# macOS
screen /dev/tty.usbmodem* 115200

# Linux
screen /dev/ttyACM0 115200
```

You should see output like:
```
Predicted Class: 12
Predicted Class: 12
SWITCHING
Predicted Class: 7      # <-- different class after capsule swap
```

## How Capsule Updates Work

### Memory Layout

The custom linker script (`container-update/link.ld`) partitions memory:

```
Flash (0x00000000 - 0x000FF000): Main application firmware
    ├── .text           Application code
    ├── .rodata         Constants
    └── .data           Initialized globals

RAM (0x20000000+):
    ├── CONTAINER_CODE (0x20000000, 4 KB):  ML inference operators (predict(), etc.)
    ├── CONTAINER_DATA (0x20001000, 64 KB): ML model weights
    └── General RAM:     Application heap/stack
```

### Update Flow

```
1. Train new model          (host machine)
2. Convert to C arrays      (model.h / utils.h)
3. Build capsule ELF        (make)
4. Extract capsule bytes    (gen-byte-array.py → contents.h + checksum.h)
5. Embed in firmware        (rebuild with old model + new capsule as payload)
6. Transmit update payload  (BLE / HTTP / UART — only capsule bytes)
7. On device:
   a. check_hash()          Verify SHA-256 integrity
   b. switch_container()    memcpy new code/data into capsule region
   c. predict() now runs    the new model — no reboot needed
```

### Key Functions

- **`switch_container()`** (`container-update.h`): Copies new capsule contents into the reserved memory region using `memcpy`. Validates integrity via SHA-256 before applying.
- **`check_hash()`**: Compares SHA-256 hash of the incoming capsule against the expected hash to ensure integrity.
- **`gen-byte-array.py`**: Extracts `.container-code` and `.container-data` sections from a compiled ELF and converts them to C byte arrays for embedding.

## Applications

Each application demonstrates capsule updates with a different ML model:

| Application | Domain | Model Type | Description |
|-------------|--------|------------|-------------|
| **farmbeats** | Agriculture | Neural Network | Classifies agricultural sensor readings |
| **gesturepod** | HCI | Neural Network | Recognizes hand gestures from IMU data |
| **Powerblade** | Energy | Neural Network | Power monitoring and classification |
| **spektacom** | Sports | 2-layer NN | Cricket bat impact analytics |

Each application directory contains:
- `main.c` -- Application entry point with capsule swap logic
- `model.h` / `utils.h` -- Current model weights and inference operators
- `models/` -- Old and new model variants for A/B testing
- `utils/` -- Old and new operator implementations
- `Makefile` -- Build configuration targeting nRF52840
- `container-update/` -- Local capsule update headers

## Update Types

Minerva supports three granularities of capsule update:

| Update Type | What Changes | Typical Size | Use Case |
|-------------|-------------|-------------|----------|
| **Weights-only** | Model parameters in `.container-data` | Small (KB) | Retraining, fine-tuning, federated learning |
| **Operators-only** | Inference code in `.container-code` | Small (KB) | Bug fixes, operator optimizations |
| **Full capsule** | Both code and data sections | Medium (KB) | Architecture change, new model family |
| **Full DFU** (baseline) | Entire firmware image | Large (100s KB) | Major application changes |

## Reproducing Results

### With Hardware

1. **Build and flash each application** using `run.sh` in each app directory.
2. **Measure capsule size vs. full DFU size:**
   ```bash
   # Capsule size (code + data sections only)
   arm-none-eabi-objdump -h _build/*.elf | grep container

   # Full firmware size
   arm-none-eabi-size _build/*.elf
   ```
3. **Measure update latency:** The applications toggle a GPIO pin around `switch_container()`. Use an oscilloscope or logic analyzer on the configured PIN to measure swap time.
4. **Compare against BLE DFU baseline:** Follow instructions in `ble_application_update/README.md` to set up the Nordic Secure Bootloader and measure full DFU time.
5. **Compare against HTTP FOTA baseline:** Follow instructions in `http_application_update/README.rst` for nRF9160-based comparisons.

### Without Hardware

Even without a physical device, you can reproduce the core size and payload comparisons:

```bash
cd improved-octo-doodle/src/apps/farmbeats

# 1. Build with old model
cp models/old_model.h model.h
cp utils/old_utils.h utils.h
make
arm-none-eabi-size _build/*.elf          # Full firmware size
make clean

# 2. Build with new model
cp models/new_model.h model.h
cp utils/new_utils.h utils.h
make
arm-none-eabi-size _build/*.elf          # Full firmware size (for DFU)

# 3. Extract capsule (new model only)
python3 container-update/gen-byte-array.py \
    _build/*.elf \
    container-update/contents.h \
    .container-code .container-data

# 4. Compare sizes
wc -c container-update/contents.h        # Capsule payload size
ls -la _build/*.hex                      # Full DFU image size
```

Repeat for `gesturepod`, `Powerblade`, and `spektacom` to reproduce all size comparison data.

Pre-collected evaluation logs are available in `evaluation_logs/` for reference. See [Evaluation Logs](#evaluation-logs) below for details on how to interpret them.

## Evaluation Logs

The `evaluation_logs/` directory contains raw serial logs captured from hardware experiments. These logs record the complete device output during capsule updates and full DFU baselines, and serve as the ground truth for the timing and size measurements reported in the paper.

### Directory Layout

Logs are organized by `<application>_<update-type>/`, with 5 independent trials per configuration:

```
evaluation_logs/
├── farmbeats_weights/          # Weights-only capsule update
│   ├── trial1.txt ... trial5.txt
├── farmbeats_function/         # Operators-only capsule update
│   ├── trial1.txt ... trial5.txt
├── farmbeats_weight+function/  # Full capsule update (weights + operators)
│   ├── trial1.txt ... trial5.txt
├── farmbeats-full_dfu/         # Full DFU baseline
│   └── trial1.txt
├── gesturepod-weights/         # (same structure for gesturepod)
├── gesturepod-function/
├── gesturepod-function+weight/
├── gesturepod-full_dfu/
├── powerblade-weight/          # (same structure for powerblade)
├── powerblade-function/
├── powerblade-function+weight/
├── powerblade-full_dfu/
├── spektacom_weight/           # (same structure for spektacom)
├── spectacom_function/
├── spectacom_weight+function/
└── wasted_time_logs/           # Download-only timing at various payload sizes
    ├── download-5.2kb.txt
    ├── download-6.2kb.txt
    ├── download-16.2kb.txt
    ├── download-36kb.txt
    ├── download-40.375kb.txt
    ├── download-90kb.txt
    ├── download-200kb.txt
    └── full_dfu_spektacom_213.996kb.txt
```

### Log Format

Each trial log is a timestamped serial capture from the nRF9160 modem. The format is:

```
<ISO-8601 timestamp> DEBUG modem << <device output>
```

A typical **capsule update** log (e.g., `farmbeats_weights/trial1.txt`) shows:

```
2019-12-13T06:31:26.659Z DEBUG modem << ***** Booting Zephyr OS build v2.0.99-ncs1 *****
2019-12-13T06:31:26.676Z DEBUG modem << Initialized bsdlib
2019-12-13T06:31:32.499Z DEBUG modem << LTE Link Connected!
2019-12-13T06:31:32.629Z DEBUG modem << Predicted Class: 12        # Prediction BEFORE update
2019-12-13T06:31:39.528Z DEBUG modem << Connected to turbomemory1.s3-us-west-1.amazonaws.com
2019-12-13T06:31:39.543Z DEBUG modem << Downloading: app_update.bin [0]
2019-12-13T06:31:41.167Z DEBUG modem << Downloaded 4096/5470 bytes (74%)
2019-12-13T06:31:41.193Z DEBUG modem << Downloaded 5470/5470 bytes (100%)
2019-12-13T06:31:41.216Z DEBUG modem << Download complete
2019-12-13T06:31:41.234Z DEBUG modem << Predicted Class: 12        # Prediction AFTER update (no reboot)
```

A **full DFU** log (e.g., `farmbeats-full_dfu/trial1.txt`) shows additional overhead:

```
...
2019-12-13T01:56:06.139Z DEBUG modem << Download complete
2019-12-13T01:56:06.243Z DEBUG modem << MCUBoot image upgrade scheduled. Reset the device to apply
2019-12-13T01:56:10.387Z DEBUG modem << Starting bootloader       # Device reboots
2019-12-13T01:56:10.407Z DEBUG modem << Swap type: test
2019-12-13T01:56:41.719Z DEBUG modem << Bootloader chainload...    # ~31s MCUboot swap
2019-12-13T01:56:42.245Z DEBUG modem << SPM: prepare to jump to Non-Secure image.
2019-12-13T01:56:42.251Z DEBUG modem << Initializing bsdlib        # Full re-initialization
2019-12-13T01:56:46.911Z DEBUG modem << Predicted Class: 12        # First prediction after reboot
```

### How to Extract Timing Measurements

The key metrics can be derived from the timestamps:

1. **Download time:** Difference between the `Downloading:` and `Download complete` timestamps.
   - Capsule update example: `5470 bytes` downloaded in ~2 seconds
   - Full DFU example: `264536 bytes` downloaded in ~83 seconds

2. **Total update latency (capsule):** Difference between `Downloading:` and the post-update `Predicted Class:` line. For capsule updates, inference resumes immediately after download -- no reboot.

3. **Total update latency (full DFU):** Includes download time + MCUboot swap time (~25-31 seconds) + re-initialization time (~5 seconds). The device must fully reboot.

4. **Download size:** Shown in the `Downloaded X/Y bytes (100%)` line.

### Comparing Capsule vs. Full DFU (example: farmbeats)

| Metric | Weights-only capsule | Full DFU |
|--------|---------------------|----------|
| Payload size | 5,470 bytes | 264,536 bytes |
| Download time | ~2 s | ~83 s |
| Reboot required | No | Yes (~31 s MCUboot swap) |
| Re-initialization | None | ~5 s (LTE reconnect) |
| **Total update latency** | **~2 s** | **~120 s** |

### Wasted Time Logs

The `wasted_time_logs/` directory isolates download time as a function of payload size, independent of any specific application. These logs measure the raw download overhead from the AWS S3 server over LTE at various payload sizes (5.2 KB to 214 KB), establishing the relationship between update payload size and transfer time.

## BLE DFU Baseline

The `ble_application_update/` directory contains the Nordic Secure Bootloader configuration used as the DFU baseline. See `ble_application_update/README.md` for a complete walkthrough covering:

- Generating signing keys with `nrfutil`
- Building and flashing the secure bootloader
- Programming the SoftDevice (s140 for nRF52840, s132 for nRF52832)
- Generating DFU packages
- Performing OTA updates via the nRF Connect Android app

## HTTP FOTA Baseline

The `http_application_update/` directory contains a Zephyr-based FOTA sample for the nRF9160. See `http_application_update/README.rst` for setup and usage. This baseline downloads firmware images from an AWS S3 bucket over LTE and applies them via MCUboot.

## Expected Evaluation Time

| Step | Time |
|------|------|
| Software setup (toolchain, SDK) | 30-60 min |
| Building firmware and capsules | < 10 min |
| Hardware flashing and experiments | 1-2 hours |
| Full reproduction of all measurements | < 1 day |


## License

This project is released under the MIT License. See `improved-octo-doodle/LICENSE.md`.
