# SkullMasterV2 - Animatronic Skull DMX Controller

**Version**: 3.1.0-alpha  
**Date**: 2024-12-27  
**Status**: Production-Ready

A dual-core RP2040-based animatronic controller for theatrical skull props with DMX512 control, designed for pirate-themed attractions. Controls up to 5 servos for jaw, yaw, pitch, roll, and eye movements with synchronized LED eye effects.

## Project Structure

```
SkullMasterV2/
├── SkullMasterV2.ino      # Main Arduino sketch with dual-core logic
├── RS5DMX.h               # DMX configuration and system state
├── RS5DualCore.h          # Inter-core communication and synchronization
├── RS5Hardware.h          # Hardware pin definitions and servo configurations
├── ServoDriver.h          # PWM driver for RP2040 hardware
├── ServoEngine.h          # Motion profiling and servo control
└── docs/                  # Documentation
    ├── README.md          # This file
    ├── CHANGELOG.md       # Version history
    ├── guides/            # User guides
    ├── design/            # Architecture documentation
    └── api/               # API reference
```

## System Architecture Overview

### Dual-Core Design
- **Core 0**: DMX reception, dip switch monitoring, system state management
- **Core 1**: Servo control, motion profiling, LED effects, status indicators

### Key Components
- **DMX512 Interface**: Receives lighting control commands
- **Servo Control**: 5-axis animatronic movement with motion profiling
- **LED Effects**: Programmable eye colors with flicker effects
- **Status System**: Multi-color NeoPixel status indicators

### Communication
- FreeRTOS semaphores for thread-safe inter-core data sharing
- Asynchronous DMX reception with frame validation
- Real-time servo position updates at configurable rates

## Getting Started

### Quick Start
1. Connect power (5V for logic, appropriate voltage for servos)
2. Connect DMX input to pin defined in RS5Hardware.h
3. Set DMX address using DIP switches
4. Power on - status LED shows boot sequence
5. Send DMX commands to control servos and eyes

### DMX Channel Map
Starting at base DMX address:
- Channel 0: Jaw servo (0-255 maps to min-max degrees)
- Channel 1: Yaw servo
- Channel 2: Pitch servo
- Channel 3: Roll servo
- Channel 4: Eye servo
- Channel 5: Eye LED mode/brightness
- Channel 6: Eye LED color/effect selection

## Installation

### Hardware Requirements
- RP2040-based board (Raspberry Pi Pico compatible)
- 5 servo motors (standard PWM)
- DMX512 receiver circuit
- NeoPixel LED strip (minimum 7 pixels)
- 9 DIP switches for addressing
- 3 DIP switches for mode selection

### Software Requirements
- Arduino IDE 1.8.x or 2.x
- arduino-pico core (Earle Philhower) with FreeRTOS support
- Required libraries:
  - Adafruit NeoPixel (1.15.1+)
  - RP2040_PWM (1.7.0+)
  - Pico-DMX (3.1.0+)

### Compilation
```bash
arduino-cli compile --fqbn rp2040:rp2040:rpipico SkullMasterV2.ino
arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn rp2040:rp2040:rpipico SkullMasterV2.ino
```

## Configuration

### DIP Switch Settings
- **DMX Address (9 switches)**: Binary representation of DMX start address (1-512)
- **Run Mode (3 switches)**:
  - 000: Botany Bay configuration
  - 001: Swan Island configuration
  - 010: Port of Destiny configuration
  - 011: Rum Island configuration
  - 100: Tortuga configuration

### Servo Calibration
Edit servo parameters in RS5Hardware.h:
- Min/Max degrees for each axis
- PWM ranges (typically 500-2500μs)
- Acceleration/deceleration profiles
- Sleep timeout values

## Usage

### Operating Modes
1. **DMX Mode**: Normal operation, responds to DMX commands
2. **Demo Mode**: Automated sweep movements for testing
3. **Debug Modes**: Various diagnostic outputs via serial

### Status LED Indicators
- **Boot**: System initialization
- **Green Flash**: DMX signal received
- **Red Flash**: No DMX signal
- **Individual Servo LEDs**: Show movement/sleep status

### Debug Levels
Set via serial or compile-time:
- 0: No debug output
- 1: Boot information only
- 2: DMX frame debugging
- 3: Servo position tracking
- 4: Motion model debugging
- 5: Current/voltage monitoring
- 6: LED pixel debugging

## API Reference

See [api/reference.md](api/reference.md) for detailed class and function documentation.

## Contributing

This project is maintained by Rose&Swan for theatrical productions. For bug reports or feature requests, contact the development team.

## License

Copyright Rose&Swan 2022-2024. All Rights Reserved.  
This is proprietary software for theatrical animatronic control systems.

## Safety Notice

**WARNING**: This system controls mechanical actuators that can cause injury. Always:
- Implement emergency stop mechanisms
- Use appropriate current limiting
- Test movements at reduced speed first
- Keep clear of moving parts during operation
- Follow theatrical safety protocols