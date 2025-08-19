# SkullMasterV2 🎭💀

[![Version](https://img.shields.io/badge/version-3.1.0--alpha-blue.svg)](https://github.com/your-org/SkullMasterV2/releases)
[![Platform](https://img.shields.io/badge/platform-RP2040-purple.svg)](https://www.raspberrypi.org/products/rp2040/)
[![License](https://img.shields.io/badge/license-CC--BY--NC--4.0-yellow.svg)](LICENSE)
[![DMX512](https://img.shields.io/badge/protocol-DMX512-green.svg)](https://en.wikipedia.org/wiki/DMX512)

Professional animatronic skull controller for theatrical productions, featuring DMX512 control, 5-axis servo movement, and synchronized LED effects. Designed for Pirates of the Caribbean themed attractions and Halloween shows.

## 🎯 Features

- **5-Axis Servo Control**: Jaw, Yaw, Pitch, Roll, and Eye movements
- **DMX512 Professional Lighting Control**: Industry-standard theatrical protocol
- **Dual-Core Architecture**: Separation of real-time control and communication
- **Motion Profiling**: Smooth, lifelike movements with acceleration control
- **LED Eye Effects**: Programmable fire, ghost, and lightning effects
- **Field Configurable**: DIP switch addressing, no computer required
- **Multiple Show Modes**: Location-specific configurations for different scenes
- **Status Monitoring**: NeoPixel indicators for system and servo status
- **Power Management**: Automatic servo sleep mode to reduce power consumption

## 🚀 Quick Start

### Hardware Requirements
- RP2040-based board (Raspberry Pi Pico or compatible)
- 5× servo motors (standard 50Hz PWM)
- DMX512 interface circuit (MAX485 or similar)
- NeoPixel LED strip (7+ pixels)
- 5V logic power supply
- 5-7.4V servo power supply

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/your-org/SkullMasterV2.git
   cd SkullMasterV2
   ```

2. **Install Arduino CLI and required cores**
   ```bash
   # Add RP2040 board support
   arduino-cli config add board_manager.additional_urls \
     https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   
   arduino-cli core update-index
   arduino-cli core install rp2040:rp2040
   ```

3. **Install required libraries**
   ```bash
   arduino-cli lib install "Adafruit NeoPixel"
   arduino-cli lib install "RP2040_PWM"
   arduino-cli lib install "Pico-DMX"
   ```

4. **Compile and upload**
   ```bash
   arduino-cli compile --fqbn rp2040:rp2040:rpipico SkullMasterV2.ino
   arduino-cli upload -p /dev/cu.usbmodem* --fqbn rp2040:rp2040:rpipico SkullMasterV2.ino
   ```

## 📖 Documentation

Comprehensive documentation is available in the [`docs/`](docs/) directory:

- [**Installation Guide**](docs/guides/installation.md) - Detailed setup instructions
- [**Configuration Guide**](docs/guides/configuration.md) - Servo calibration and tuning
- [**API Reference**](docs/api/reference.md) - Complete code documentation
- [**Architecture Overview**](docs/design/architecture.md) - System design details
- [**Troubleshooting**](docs/guides/troubleshooting.md) - Common issues and solutions

## 🏗️ Project Structure

```
SkullMasterV2/
├── SkullMasterV2.ino    # Main Arduino sketch
├── RS5DMX.h             # DMX and system configuration
├── RS5DualCore.h        # Inter-core communication
├── RS5Hardware.h        # Pin mappings and hardware config
├── ServoDriver.h        # RP2040 PWM driver
├── ServoEngine.h        # Motion profiling engine
├── docs/                # Documentation
│   ├── api/            # API reference
│   ├── design/         # Architecture documentation
│   └── guides/         # User guides
└── .github/            # GitHub templates and workflows
```

## Getting Started

### Hardware Requirements
- RP2040-based development board
- DMX receiver circuit (RS485 transceiver)
- Up to 12 RC servos
- NeoPixel strip (8 pixels)
- 12-position DIP switch array
- 5V power supply capable of servo current requirements

### Pin Connections
```
DMX Input:        GPIO 18
NeoPixel Data:    GPIO 25
Enable Output:    GPIO 17
DMX Address:      GPIO 5-13 (9 bits)
Run Mode:         GPIO 2-4 (3 bits)
Servo 1 (Jaw):    GPIO 24
Servo 2 (Yaw):    GPIO 23
Servo 3 (Pitch):  GPIO 22
Servo 4 (Roll):   GPIO 21
Servo 5 (Eyes):   GPIO 20
Servos 6-12:      GPIO 19,14-16,26-28
```

## Installation

1. Install Arduino IDE with RP2040 board support
2. Install required libraries:
   - Adafruit NeoPixel
   - RP2040_PWM
   - DmxInput
3. Clone this repository
4. Open SkullMasterV2.ino in Arduino IDE
5. Select your RP2040 board and port
6. Upload the sketch

## Configuration

### DMX Address Setting
Set the 9-bit DIP switches to configure the base DMX address (1-512).

### Run Mode Selection (3-bit switches)
- 000: Botnay Bay (DMX 494)
- 001: Swan Island (DMX 496)
- 010: Port of Destiny (DMX 500)
- 011: Rum Island (DMX 498)
- 100: Tortuga (DMX 68)

### DMX Channel Map
Starting from base address:
- Channel 0: Jaw servo position (0-255 maps to configured angle range)
- Channel 1: Yaw servo position
- Channel 2: Pitch servo position
- Channel 3: Roll servo position
- Channel 4: Eye servo position
- Channel 5-6: Eye LED control (mode/brightness)

## Usage

### DMX Control Mode
1. Connect DMX signal to the controller
2. Set DIP switches for desired DMX address
3. System automatically responds to DMX commands
4. Status LED shows:
   - Green: DMX signal good
   - Red: No DMX signal
   - Blue: Boot/setup mode

### Demo Mode
If no DMX signal is present, the system enters demo mode with automatic servo sweeping and eye effects.

### Servo Motion Profiles
Each servo has configurable:
- Maximum velocity (degrees/second)
- Maximum acceleration/deceleration
- Min/max angle limits
- PWM frequency and pulse width calibration

## API Reference

### Core System Classes

#### System (RS5DMX.h)
Manages global system state, boot sequence, and mode selection.

#### Derivs_Limiter (ServoEngine.h)
Advanced motion control with velocity and acceleration limiting for smooth, realistic movements.

#### Hardware Configuration (RS5Hardware.h)
Pin definitions and servo default parameters.

## Contributing

This project is part of the Rose&Swan theatrical prop control ecosystem. For contributions or bug reports, please contact the development team.

## License

Copyright Rose&Swan 2022-2025 - All Rights Reserved  
CC BY-NC 4.0 (Non-Commercial Use Only)