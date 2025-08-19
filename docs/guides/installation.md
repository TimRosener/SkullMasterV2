# SkullMasterV2 Installation Guide

## Prerequisites

### Hardware Requirements

#### Minimum Requirements
- RP2040-based microcontroller board (Raspberry Pi Pico or compatible)
- 5V power supply for logic (minimum 500mA)
- Servo power supply (5-7.4V, current depends on servo specifications)
- DMX512 interface circuit (MAX485 or similar)
- 5x servo motors (standard 50Hz PWM compatible)
- NeoPixel LED strip (minimum 7 pixels, WS2812B compatible)
- 12x DIP switches or jumpers

#### Recommended Components
- Raspberry Pi Pico W (for future wireless features)
- 5V 3A power supply for logic and LEDs
- 6V 10A power supply for servos
- High-torque metal gear servos (20kg-cm minimum)
- Capacitors: 1000µF on servo power, 100µF on logic power
- DMX isolation circuit for production environments

### Software Requirements

#### Development Environment
- Arduino IDE 2.0+ or Arduino CLI
- Git (for version control)
- Serial terminal program (for debugging)

#### Required Arduino Cores
```bash
# Add board manager URL
arduino-cli config add board_manager.additional_urls \
  https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

# Update index
arduino-cli core update-index

# Install RP2040 core with FreeRTOS
arduino-cli core install rp2040:rp2040
```

#### Required Libraries
```bash
# Install via Arduino CLI
arduino-cli lib install "Adafruit NeoPixel"
arduino-cli lib install "RP2040_PWM"
arduino-cli lib install "Pico-DMX"
```

## Hardware Setup

### Pin Connections

#### Core Pin Assignments (from RS5Hardware.h)
```
Function          | GPIO Pin | Notes
------------------|----------|-------------------------
DMX Input         | GPIO 1   | UART0 RX
Jaw Servo         | GPIO 4   | PWM2 A
Yaw Servo         | GPIO 5   | PWM2 B
Pitch Servo       | GPIO 6   | PWM3 A
Roll Servo        | GPIO 7   | PWM3 B
Eye Servo         | GPIO 8   | PWM4 A
NeoPixel Data     | GPIO 28  | Any GPIO works
Enable Output     | GPIO 3   | High = outputs enabled

DIP Switches (DMX Address):
Bit 1 (LSB)       | GPIO 9   | Internal pullup
Bit 2             | GPIO 10  | Internal pullup
Bit 4             | GPIO 11  | Internal pullup
Bit 8             | GPIO 12  | Internal pullup
Bit 16            | GPIO 13  | Internal pullup
Bit 32            | GPIO 14  | Internal pullup
Bit 64            | GPIO 15  | Internal pullup
Bit 128           | GPIO 16  | Internal pullup
Bit 256 (MSB)     | GPIO 17  | Internal pullup

Run Mode Switches:
Mode Bit 1        | GPIO 18  | Internal pullup
Mode Bit 2        | GPIO 19  | Internal pullup
Mode Bit 4        | GPIO 20  | Internal pullup
```

### DMX Interface Circuit

#### Basic MAX485 Circuit
```
DMX XLR Pin 1 (GND)  ────────── GND
DMX XLR Pin 2 (Data-) ───┐
                         ├──── MAX485 Pin 6 (A)
DMX XLR Pin 3 (Data+) ───┤
                         └──── MAX485 Pin 7 (B)

MAX485 Connections:
Pin 1 (RO)  ──── GPIO 1 (UART RX)
Pin 2 (RE)  ──── GND (Receive Enable)
Pin 3 (DE)  ──── GND (Driver Disable)
Pin 4 (DI)  ──── Not Connected
Pin 5 (GND) ──── GND
Pin 8 (VCC) ──── 5V

Add 120Ω termination resistor between Data+ and Data-
```

### Power Supply Wiring

#### Dual Supply Configuration (Recommended)
```
Logic Supply (5V 3A):
    ├── RP2040 VCC
    ├── NeoPixel VCC
    ├── MAX485 VCC
    └── 1000µF capacitor to GND

Servo Supply (6V 10A):
    ├── Servo 1 Power
    ├── Servo 2 Power
    ├── Servo 3 Power
    ├── Servo 4 Power
    ├── Servo 5 Power
    └── 1000µF capacitor to GND

Common Ground:
    All grounds must be connected together
```

### DIP Switch Wiring

Switches connect GPIO to GND when ON (closed):
```
GPIO Pin ──┬── 10kΩ ──── 3.3V (external pullup optional)
           │
           └── Switch ─── GND
```

Internal pullups are enabled in software, external pullups optional for noise immunity.

## Software Installation

### Step 1: Clone Repository
```bash
git clone https://github.com/your-org/SkullMasterV2.git
cd SkullMasterV2
```

### Step 2: Configure Build Environment
```bash
# Verify installation
arduino-cli core list
# Should show: rp2040:rp2040

arduino-cli lib list
# Should show required libraries
```

### Step 3: Compile Firmware
```bash
# Compile for RP2040
arduino-cli compile --fqbn rp2040:rp2040:rpipico \
  --build-property "build.flags.cpp=-DDEBUG_LEVEL=1" \
  SkullMasterV2.ino
```

### Step 4: Upload Firmware

#### Method 1: USB Upload
```bash
# Connect USB cable
# Hold BOOTSEL button while plugging in USB
# Release BOOTSEL after connection

# Upload
arduino-cli upload -p /dev/cu.usbmodem* \
  --fqbn rp2040:rp2040:rpipico \
  SkullMasterV2.ino
```

#### Method 2: UF2 File Copy
```bash
# Enter bootloader mode (hold BOOTSEL during power-on)
# RP2040 appears as mass storage device

# Copy UF2 file
cp build/rp2040.rp2040.rpipico/SkullMasterV2.ino.uf2 \
  /Volumes/RPI-RP2/
```

## Initial Configuration

### Step 1: Set DMX Address

Use DIP switches to set binary DMX address:
```
Example: DMX Address 17
Binary: 000010001
Switches: 9=OFF, 8-2=OFF, 1=ON, 5=ON
```

### Step 2: Select Run Mode

Configure location-specific settings:
```
Mode 0 (000): Botany Bay     - DMX Eyes at 401
Mode 1 (001): Swan Island    - DMX Eyes at 409
Mode 2 (010): Port of Destiny - DMX Eyes at 417
Mode 3 (011): Rum Island     - DMX Eyes at 425
Mode 4 (100): Tortuga        - DMX Eyes at 433
```

### Step 3: Connect Serial Monitor (Optional)

For debugging:
```bash
# Connect at 115200 baud
arduino-cli monitor -p /dev/cu.usbmodem* -b 115200
```

### Step 4: Verify Operation

1. **Power LED Check**: Status pixel should show blue during boot
2. **DMX Reception**: Green flash indicates valid DMX signal
3. **Servo Test**: Send DMX value 127 (middle position) to all channels
4. **Eye Test**: Send DMX to eye channels for color verification

## Servo Calibration

### Finding Servo Limits

1. **Enter Debug Mode**: Set debug level to 3 via serial
2. **Test Each Servo**: Send DMX 0 and 255 to find physical limits
3. **Record Values**: Note degree ranges for each axis

### Updating Configuration

Edit `RS5Hardware.h`:
```cpp
// Example: Jaw servo calibration
#define JAW_SERVO_MINDEG 45    // Physical minimum
#define JAW_SERVO_MAXDEG 135   // Physical maximum
#define JAW_SERVO_MINPWM 500   // Microseconds at min
#define JAW_SERVO_MAXPWM 2500  // Microseconds at max
```

### Motion Profile Tuning

Adjust acceleration for smooth movement:
```cpp
#define JAW_SERVO_MAXVEL 200   // Degrees per second
#define JAW_SERVO_MAXACC 500   // Deg/sec²
#define JAW_SERVO_MAXDEC 500   // Deg/sec²
```

## Troubleshooting

### No DMX Reception
- Check MAX485 wiring and power
- Verify DMX cable polarity
- Ensure 120Ω termination installed
- Check debug output for frame errors

### Servo Not Moving
- Verify servo power supply voltage and current
- Check PWM pin connections
- Confirm servo is licensed in configuration
- Test with Demo Mode (no DMX required)

### Status LED Issues
- Blue stuck: Boot sequence incomplete
- Red flashing: No DMX signal
- No LEDs: Check NeoPixel power and data connections

### Compilation Errors
```bash
# FreeRTOS.h not found
# Solution: Install arduino-pico core, not mbed_rp2040

# PWM_Generic_Debug.h not found
# Solution: Normal, file is optional

# Multiple definition errors
# Solution: Clean build directory
arduino-cli cache clean
```

### Upload Failures
- Ensure BOOTSEL pressed during connection
- Try different USB cable/port
- Check for existing serial connections
- Reset board and retry

## Performance Optimization

### Reducing Latency
1. Decrease `DMXPacketAgeLimit` for faster timeout
2. Increase servo update rate (reduce delays)
3. Optimize motion profiles for your mechanics

### Power Saving
1. Enable servo sleep mode when stationary
2. Reduce NeoPixel brightness
3. Implement selective servo updates

### Debug Output
Minimize serial output in production:
```cpp
// In setup()
systemState.setDebugLevel(DebugLevelNone);
```

## Safety Checklist

- [ ] Emergency stop switch wired to ENABLE_OUTPUT pin
- [ ] Current limiting on servo power supply
- [ ] Mechanical stops on all axes
- [ ] Proper grounding and shielding
- [ ] DMX isolation in place
- [ ] Servo power separate from logic power
- [ ] Thermal management for high-duty cycle operation
- [ ] Physical guards around moving parts