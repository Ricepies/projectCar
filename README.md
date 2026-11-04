# ProjectCar (Raspberry Pi Pico 4WD Robot Car)

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL_v3-blue.svg)](LICENSE)

An embedded C++ firmware for a 4-wheel-drive (4WD) robotic car powered by the **Raspberry Pi Pico (RP2040)** and controlled wirelessly via a **PS2 Gamepad**.

---

## 🏎️ Features

- **4-Wheel Independent Drive:** Dual-channel H-Bridge motor control for Front-Left, Front-Right, Rear-Left, and Rear-Right wheels.
- **PS2 Gamepad Integration:** Fast bit-banged SPI communication supporting dual analog joysticks and digital action buttons.
- **Audible Status Feedback:** Active/passive buzzer feedback indicating power-on, motor arming/disarming, and alarms.
- **Modern C++ Structure:** Clean separation of headers (`include/`), implementation (`src/`), standalone demos (`examples/`), and hardware datasheets (`docs/`).

---

## 📌 Pinout & Hardware Connections

### 1. Motors (H-Bridge Drivers)
| Motor | Pin A (Forward PWM) | Pin B (Reverse PWM) |
| :--- | :--- | :--- |
| **Front Left (FL)** | `GP0` | `GP1` |
| **Front Right (FR)** | `GP2` | `GP3` |
| **Rear Left (RL)** | `GP6` | `GP7` |
| **Rear Right (RR)** | `GP4` | `GP5` |

### 2. PS2 Gamepad (Bit-banged SPI)
| PS2 Signal | Pico GPIO | Notes |
| :--- | :--- | :--- |
| **DAT (MOSI)** | `GP12` | Data from controller to Pico |
| **CMD (MISO)** | `GP13` | Command from Pico to controller |
| **SEL (CS)** | `GP14` | Chip select (Active LOW) |
| **CLK (SCK)** | `GP15` | SPI clock line |
| **VCC** | `3.3V` / `5V` | Match receiver specifications |
| **GND** | `GND` | Common ground |

### 3. Peripherals
| Peripheral | Pico GPIO | Notes |
| :--- | :--- | :--- |
| **Buzzer** | `GP16` | Audible status and melody output |

---

## 📁 Project Structure

```text
projectCar/
├── .gitignore                      # Git ignore rules for PlatformIO and OS files
├── platformio.ini                  # PlatformIO multi-environment configuration
├── README.md                       # Project overview and hardware guide
│
├── include/                        # Header declarations (.h)
│   ├── Gamepad.h                   # PS2 controller driver interface
│   ├── GamepadNew.h                # Streamlined single-loop fused PS2 driver
│   ├── Motors.h                    # Motor driver interface
│   └── Types.h                     # Rust/LLVM fixed-width type aliases (i8, u8, i16, etc.)
│
├── src/                            # Firmware implementation (.cpp)
│   ├── Gamepad.cpp                 # PS2 SPI protocol implementation
│   ├── GamepadNew.cpp              # Streamlined single-loop fused PS2 implementation
│   ├── Motors.cpp                  # Motor PWM speed and direction control
│   └── main.cpp                    # Primary car application & loop
│
├── examples/                       # Standalone sketches & test routines
│   ├── adc_resistance_reader.cpp   # ADC voltage-divider resistance reader
│   ├── autonomous_steering.cpp     # Directional movement & turn routines
│   ├── buzzer_melody.cpp           # Passive buzzer musical playback
│   └── legacy_mbot_science_festival.cpp # Historical 2021 mBot Arduino code
│
└── docs/                           # Hardware documentation & references
    ├── hardware/
    │   ├── Pico-R3-Pinout.pdf      # Raspberry Pi Pico pinout diagram
    │   ├── RP2040-datasheet.pdf    # Official RP2040 datasheet
    │   ├── Getting-started-with-pico.pdf
    │   └── PS2通讯说明.pdf         # PS2 protocol documentation
    └── references/
        ├── Arduino Cookbook 3rd Edition.pdf
        └── PS2遥控手柄测试代码（树莓派）/  # Vendor reference test code
```

---

## 🎮 Controls

For an exhaustive line-by-line breakdown of the SPI polling protocol, button mask decoding, and differential joystick mixing, see the [Gamepad Controls Technical Guide](docs/GAMEPAD_CONTROL.md).

| Controller Input | Action |
| :--- | :--- |
| **Start Button** | Arm / Disarm motors (audible high/low tone feedback) |
| **Left Stick (LY, LX)** | Differential forward / reverse / steering drive |
| **Right Stick (RY, RX)** | Alternate steering / spot rotation |

---

## 🛠️ Building & Flashing

This project uses [PlatformIO](https://platformio.org/).

### Command Line
```bash
# Build firmware for Raspberry Pi Pico
pio run -e pico

# Upload firmware to connected Raspberry Pi Pico
pio run -e pico --target upload
```

### Visual Studio Code
1. Open the `projectCar` folder in VS Code.
2. Ensure the **PlatformIO IDE** extension is installed.
3. Click the **Build** (✓) or **Upload** (→) icon in the PlatformIO status bar.

---

## 📄 License

This project is licensed under the terms of the [GNU Affero General Public License v3.0 (AGPL-3.0)](LICENSE).

