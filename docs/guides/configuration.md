# SkullMasterV2 Configuration Guide

## Overview

This guide covers all configuration options for the SkullMasterV2 animatronic controller, including servo calibration, DMX mapping, motion profiles, and special effects.

## Configuration Files

### Primary Configuration Locations

1. **RS5Hardware.h** - Hardware pin mappings and servo parameters
2. **RS5DMX.h** - System settings and DMX configuration
3. **Compile-time settings** - Debug levels and features

## Servo Configuration

### Servo Parameter Structure

Each servo is configured with these parameters in `RS5Hardware.h`:

```cpp
struct ServoConfigC1 {
    int servoNum;           // Servo identifier (0-4)
    char* servoUserName;    // Human-readable name
    int pin;                // GPIO pin number
    float freq;             // PWM frequency (typically 50Hz)
    int ServoStartDeg;      // Starting position in degrees
    int minDeg;             // Minimum allowed angle
    int maxDeg;             // Maximum allowed angle
    int minPWM;             // PWM pulse width at minDeg (µs)
    int maxPWM;             // PWM pulse width at maxDeg (µs)
    int servoMinDeg;        // Physical servo minimum (usually 0)
    int servoMaxDeg;        // Physical servo maximum (usually 180)
    float maxVel;           // Maximum velocity (deg/sec)
    float maxAcc;           // Maximum acceleration (deg/sec²)
    float maxDec;           // Maximum deceleration (deg/sec²)
    bool smooth;            // Enable acceleration limiting
    bool licensed;          // Enable this servo
    int servoSleepTimer;    // Idle timeout (ms)
};
```

### Individual Servo Settings

#### Jaw Servo Configuration
```cpp
// Physical limits - adjust based on your mechanism
#define JAW_SERVO_MINDEG 65      // Closed position
#define JAW_SERVO_MAXDEG 90      // Open position
#define JAW_START_POS 75         // Neutral position

// PWM calibration - standard servo timing
#define JAW_SERVO_MINPWM 600     // Pulse width at min angle
#define JAW_SERVO_MAXPWM 2400    // Pulse width at max angle

// Motion profile - smooth jaw movement
#define JAW_SERVO_MAXVEL 180     // Max speed (deg/sec)
#define JAW_SERVO_MAXACC 360     // Acceleration (deg/sec²)
#define JAW_SERVO_MAXDEC 360     // Deceleration (deg/sec²)

// Power management
#define JAW_START_SLEEP 5000     // Sleep after 5 seconds idle
```

#### Yaw Servo Configuration (Left-Right)
```cpp
#define YAW_SERVO_MINDEG 45      // Full right
#define YAW_SERVO_MAXDEG 135     // Full left
#define YAW_START_POS 90         // Center/forward

#define YAW_SERVO_MINPWM 600
#define YAW_SERVO_MAXPWM 2400

#define YAW_SERVO_MAXVEL 120     // Slower for smooth panning
#define YAW_SERVO_MAXACC 240
#define YAW_SERVO_MAXDEC 240

#define YAW_START_SLEEP 10000    // 10 seconds idle timeout
```

#### Pitch Servo Configuration (Up-Down)
```cpp
#define PITCH_SERVO_MINDEG 60    // Looking down
#define PITCH_SERVO_MAXDEG 120   // Looking up
#define PITCH_START_POS 90       // Level

#define PITCH_SERVO_MINPWM 600
#define PITCH_SERVO_MAXPWM 2400

#define PITCH_SERVO_MAXVEL 120
#define PITCH_SERVO_MAXACC 240
#define PITCH_SERVO_MAXDEC 240

#define PITCH_START_SLEEP 10000
```

#### Roll Servo Configuration (Tilt)
```cpp
#define ROLL_SERVO_MINDEG 70     // Tilt right
#define ROLL_SERVO_MAXDEG 110    // Tilt left
#define ROLL_START_POS 90        // Level

#define ROLL_SERVO_MINPWM 600
#define ROLL_SERVO_MAXPWM 2400

#define ROLL_SERVO_MAXVEL 90     // Slower for effect
#define ROLL_SERVO_MAXACC 180
#define ROLL_SERVO_MAXDEC 180

#define ROLL_START_SLEEP 15000   // 15 seconds idle
```

#### Eye Servo Configuration
```cpp
#define EYE_SERVO_MINDEG 0       // Eyes closed
#define EYE_SERVO_MAXDEG 90      // Eyes wide open
#define EYE_START_POS 45         // Half open

#define EYE_SERVO_MINPWM 600
#define EYE_SERVO_MAXPWM 2400

#define EYE_SERVO_MAXVEL 360     // Fast eye movement
#define EYE_SERVO_MAXACC 720
#define EYE_SERVO_MAXDEC 720

#define EYE_START_SLEEP 3000     // Quick sleep for power save
```

### Calibration Process

#### Step 1: Find Physical Limits
```cpp
// Temporarily set wide limits for testing
#define TEST_SERVO_MINDEG 0
#define TEST_SERVO_MAXDEG 180

// Send DMX values 0 and 255 to find actual range
// Record positions where mechanical limits are reached
```

#### Step 2: Measure PWM Timing
```cpp
// Use oscilloscope or logic analyzer if available
// Standard servos: 500-2500µs range
// Digital servos: May use 600-2400µs
// Continuous rotation: 1500µs = stop
```

#### Step 3: Tune Motion Profiles
```cpp
// Start with conservative values
#define INITIAL_MAXVEL 60
#define INITIAL_MAXACC 120

// Gradually increase until movement is optimal
// Watch for: Overshooting, vibration, stalling
```

## DMX Configuration

### DMX Address Mapping

#### Base Address Setting
The base DMX address is set via DIP switches:
```cpp
// Binary representation on switches
// Example: Address 17 = 00010001 binary
// Switch 1 = ON (1), Switch 5 = ON (16)
```

#### Channel Assignments
Starting from base address:
```cpp
Channel Offset | Function
---------------|------------------
0              | Jaw position
1              | Yaw position
2              | Pitch position
3              | Roll position
4              | Eye position
5              | Eye brightness/mode
6              | Eye effect selection
```

### Location-Based Configuration

Different locations use different eye DMX addresses:
```cpp
// Set via Run Mode DIP switches
#define EYEDMX_BOTNAYBAY 401      // Mode 000
#define EYEDMX_SWANISLAND 409     // Mode 001
#define EYEDMX_PORTOFDESTINY 417  // Mode 010
#define EYEDMX_RUMISLAND 425      // Mode 011
#define EYEDMX_TORTUGA 433        // Mode 100
```

### DMX Timing Parameters
```cpp
// In RS5DMX.h
#define DMX_PACKET_AGE_LIMIT 200  // Max ms between packets
#define DMX_BREAK_THRESHOLD 88    // µs for break detection
#define DMX_MAB_TIME 12           // µs mark after break
```

## LED Eye Configuration

### Eye Effect Profiles

Define custom eye effects in `RS5Hardware.h`:

```cpp
class eyeLightFlicker {
    int dmxStart;        // DMX value range start
    int dmxEnd;          // DMX value range end
    uint32_t baseColor;  // Primary color (0xRRGGBB)
    uint32_t flickerColor; // Secondary color
    uint32_t blackColor;   // Dark/off color
    int baseColorTime;     // Time weight for base
    int flickerColorTime;  // Time weight for flicker
    int blackColorTime;    // Time weight for black
    int flickerDelay;      // Max ms between changes
};
```

### Predefined Effects

```cpp
// Fire effect
eyeLightFlicker FIRE_EFFECT = {
    10, 20,           // DMX 10-20
    0xFF4500,         // Orange base
    0xFF0000,         // Red flicker
    0x000000,         // Black
    10, 5, 2,         // Time weights
    50                // Fast flicker
};

// Ghost effect
eyeLightFlicker GHOST_EFFECT = {
    21, 30,           // DMX 21-30
    0x00FF00,         // Green base
    0x00FFFF,         // Cyan flicker
    0x000000,         // Black
    8, 4, 3,          // Time weights
    100               // Medium flicker
};

// Lightning effect
eyeLightFlicker LIGHTNING = {
    31, 40,           // DMX 31-40
    0x0000FF,         // Blue base
    0xFFFFFF,         // White flash
    0x000000,         // Black
    5, 2, 10,         // Mostly dark
    25                // Very fast
};
```

### RGB Direct Control Mode

When DMX eye mode value < 10:
```cpp
// Channels from eye base address:
// +0: Brightness (0-255)
// +1: Mode selector (<10 for RGB)
// +2: Red value (0-255)
// +3: Green value (0-255)
// +4: Blue value (0-255)
```

## Motion Profile Configuration

### Smooth vs Instant Movement

```cpp
// In servo configuration
bool smooth = true;   // Enable acceleration limits
bool smooth = false;  // Instant position changes

// Smooth movement uses maxVel, maxAcc, maxDec
// Instant movement ignores acceleration limits
```

### Advanced Motion Settings

```cpp
// Jerk limiting (future implementation)
#define MAX_JERK 1000  // deg/sec³

// Position filtering
#define POSITION_DEADBAND 2  // Ignore changes < 2 degrees

// Velocity scaling by distance
#define VELOCITY_SCALE_FACTOR 0.8  // Slow down for small moves
```

## System Configuration

### Debug Levels

```cpp
enum debugLevel {
    DebugLevelNone = 0,      // Production mode
    DebugLevelStartup = 1,   // Boot messages only
    DebugLevelDMX = 2,       // DMX frame debugging
    DebugLevelServo = 3,     // Servo position tracking
    DebugLevelModel = 4,     // Motion calculations
    DebugLevelPower = 5,     // Voltage/current monitoring
    DebugLevelPixel = 6      // LED debugging
};
```

### Timing Configuration

```cpp
// Update rates (milliseconds)
#define SERVO_UPDATE_RATE 20      // 50Hz servo refresh
#define STATUS_UPDATE_RATE 100    // Status LED update
#define DMX_CHECK_RATE 10         // DMX timeout check
#define DIP_SWITCH_SCAN_RATE 500  // DIP switch polling

// Timeouts
#define SERVO_SLEEP_TIMEOUT 5000  // Default idle timeout
#define DMX_TIMEOUT 200           // No signal timeout
#define BOOT_TIMEOUT 5000         // Boot sequence limit
```

### Memory Allocation

```cpp
// Buffer sizes
#define DMX_BUFFER_SIZE 513       // Start code + 512 channels
#define NUM_SERVO_PINS 5          // Maximum servos
#define NUM_LIC_SERVOS 5          // Licensed servos
#define NUMPIXELS 7               // Status + 5 servos + eyes
```

## Status Indicator Configuration

### Status LED Colors

```cpp
// System status colors (NeoPixel 0)
#define STATUS_BOOT         Color(0, 0, 255)    // Blue
#define STATUS_DMX_GOOD     Color(0, 255, 0)    // Green
#define STATUS_DMX_BAD      Color(255, 0, 0)    // Red
#define STATUS_DEMO_MODE    Color(255, 255, 0)  // Yellow
#define STATUS_ERROR        Color(255, 0, 255)  // Magenta

// Servo status colors (NeoPixels 1-5)
#define SERVO_STATUS_STARTUP      Color(0, 0, 255)
#define SERVO_STATUS_MOVING       Color(0, 255, 0)
#define SERVO_STATUS_STILL        Color(0, 64, 0)
#define SERVO_STATUS_SLEEPING     Color(0, 0, 0)
#define SERVO_STATUS_ERROR        Color(255, 0, 0)
#define SERVO_STATUS_NOTLICENSED  Color(64, 0, 64)
```

### Flash Patterns

```cpp
// Flash timing (milliseconds)
struct StatusPattern {
    int onTime;
    int offTime;
    int repeatCount;
};

StatusPattern DMX_LOST = {100, 100, 3};    // Fast triple flash
StatusPattern BOOT_PROGRESS = {500, 500, 0}; // Slow continuous
StatusPattern ERROR_CRITICAL = {50, 50, 10}; // Rapid flashing
```

## Performance Tuning

### Optimization Priorities

```cpp
// Set performance mode
#define PERFORMANCE_MODE 1  // 0=Balanced, 1=Speed, 2=Smooth

#if PERFORMANCE_MODE == 1
    #define SERVO_UPDATE_PRIORITY 1
    #define DMX_PROCESS_PRIORITY 2
    #define LED_UPDATE_PRIORITY 3
#endif
```

### Core Affinity

```cpp
// Task assignment
#define DMX_CORE 0           // DMX on Core 0
#define SERVO_CORE 1         // Servos on Core 1
#define AFFINITY_STRICT true // Enforce core assignment
```

## Safety Configuration

### Limit Protection

```cpp
// Hardware protection
#define ENABLE_SERVO_LIMITS true
#define SERVO_LIMIT_MARGIN 5     // Degrees from physical limit

// Software protection
#define ENABLE_WATCHDOG false    // Not yet implemented
#define WATCHDOG_TIMEOUT 1000    // Milliseconds

// Current limiting
#define MAX_TOTAL_CURRENT 5000   // mA for all servos
#define PER_SERVO_CURRENT 1000   // mA per servo max
```

### Emergency Stop

```cpp
// E-stop configuration
#define ESTOP_PIN GPIO3          // Emergency stop input
#define ESTOP_ACTIVE_LOW true    // Ground to trigger
#define ESTOP_LATCH true         // Require reset after trigger
```

## Configuration Validation

### Compile-Time Checks

```cpp
// Validate servo ranges
#if JAW_SERVO_MINDEG >= JAW_SERVO_MAXDEG
    #error "Jaw servo min must be less than max"
#endif

// Validate PWM frequencies
#if PWM_FREQUENCY > 1000 || PWM_FREQUENCY < 40
    #error "PWM frequency out of range"
#endif

// Validate memory usage
#if (NUM_SERVO_PINS * sizeof(ServoConfigC1)) > 10000
    #error "Servo configuration too large"
#endif
```

### Runtime Validation

```cpp
// In setup()
void validateConfiguration() {
    // Check servo parameters
    for (int i = 0; i < NUM_SERVO_PINS; i++) {
        if (C1_config_R[i].minDeg >= C1_config_R[i].maxDeg) {
            Serial.printf("ERROR: Servo %d invalid range\n", i);
        }
        if (C1_config_R[i].freq < 40 || C1_config_R[i].freq > 400) {
            Serial.printf("ERROR: Servo %d invalid frequency\n", i);
        }
    }
    
    // Check DMX configuration
    if (systemState.getDMXAddress() > 507) {
        Serial.println("ERROR: DMX address too high");
    }
}
```

## Best Practices

### Configuration Management

1. **Version Control**: Always commit configuration changes
2. **Documentation**: Comment all non-obvious values
3. **Backup**: Keep known-good configurations
4. **Testing**: Test changes incrementally
5. **Validation**: Use compile and runtime checks

### Servo Tuning Order

1. Set mechanical limits first
2. Calibrate PWM timing
3. Start with slow speeds
4. Increase acceleration gradually
5. Fine-tune sleep timeouts

### DMX Setup Order

1. Verify electrical connections
2. Set base address via DIP switches
3. Configure location mode
4. Test with DMX controller
5. Validate all channels

### Performance Optimization Order

1. Profile current performance
2. Identify bottlenecks
3. Adjust priorities
4. Optimize critical paths
5. Validate improvements