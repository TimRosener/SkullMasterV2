# SkullMasterV2 Architecture Documentation

## System Overview

SkullMasterV2 is a dual-core real-time control system for theatrical animatronics, built on the RP2040 microcontroller. The architecture separates time-critical servo control from DMX communication to ensure smooth, reliable operation.

## Core Architecture

### Dual-Core Design Rationale

The RP2040's dual Cortex-M0+ cores are utilized to separate concerns:

```
┌─────────────────────────────────────────────┐
│                  RP2040                      │
│                                              │
│  ┌──────────────┐      ┌──────────────┐    │
│  │    Core 0    │      │    Core 1    │    │
│  │              │      │              │    │
│  │ DMX Reception│◄────►│Servo Control │    │
│  │ DIP Switches │      │Motion Profile│    │
│  │ System State │      │ LED Effects  │    │
│  └──────────────┘      └──────────────┘    │
│         ▲                      ▲            │
│         └──────Semaphores─────┘            │
└─────────────────────────────────────────────┘
```

### Core 0 Responsibilities

**Purpose**: Handle all external inputs and system state management

**Tasks**:
- DMX512 packet reception and validation
- DIP switch monitoring for addressing
- Run mode determination
- System state updates
- Debug output coordination

**Thread Safety**: 
- Owns system state writes
- Uses FreeRTOS semaphores for data protection
- Runs at lower priority than Core 1

### Core 1 Responsibilities

**Purpose**: Real-time servo control and effects generation

**Tasks**:
- Motion profile calculation
- PWM signal generation
- Servo position updates
- NeoPixel LED control
- Status indicator management
- Power management (servo sleep)

**Thread Safety**:
- Read-only access to DMX data
- Owns servo position state
- Protected pixel buffer access

## Module Architecture

### 1. DMX Input Module (DmxInput)

**Purpose**: Asynchronous DMX512 reception

**Responsibilities**:
- Hardware UART configuration
- Frame synchronization
- Break detection
- Buffer management

**Communication**:
- **Inputs**: DMX512 electrical signal
- **Outputs**: 513-byte buffer (start code + 512 channels)
- **Dependencies**: Hardware UART, DMA

**Memory Usage**:
- Static allocation: 513 bytes buffer
- No dynamic allocation

### 2. Servo Engine (ServoEngine.h)

**Purpose**: Motion profiling and smooth movement

**Responsibilities**:
- Acceleration/deceleration curves
- Velocity limiting
- Position interpolation
- Jerk limitation (optional)

**Communication**:
- **Inputs**: Target positions, motion parameters
- **Outputs**: Current positions, velocities
- **Dependencies**: Math library

**Thread Safety**:
- Each servo has independent motion profile
- No shared state between servos

### 3. Servo Driver (ServoDriver.h)

**Purpose**: Hardware PWM generation using RP2040 PIO

**Responsibilities**:
- PWM frequency configuration
- Duty cycle calculation
- Hardware timer management
- State machine allocation

**Communication**:
- **Inputs**: Servo positions (degrees)
- **Outputs**: PWM signals (GPIO pins)
- **Dependencies**: RP2040 hardware/pwm

**Memory Usage**:
- Static: 8 PWM slice configurations
- Per servo: ~100 bytes

### 4. System State Manager (RS5DMX.h)

**Purpose**: Central system configuration and state

**Class Structure**:
```cpp
class System {
    // Boot sequence control
    int boot;           // Boot stage (0-5)
    
    // Operating modes
    int mode;           // DMX/Demo/Debug
    int debug;          // Debug level (0-6)
    
    // DMX configuration
    int startDMX;       // Base DMX address
    int startDMXEyes;   // Eye effect address
    
    // Timing control
    int DMXPacketAgeLimit;  // Timeout in ms
}
```

**Thread Safety**:
- Protected by systemStateLock semaphore
- Atomic operations for critical values

### 5. Hardware Configuration (RS5Hardware.h)

**Purpose**: Hardware abstraction and pin mapping

**Configuration Arrays**:
```cpp
// Servo configurations
ServoConfigC1 C1_config_R[NUM_SERVO_PINS] = {
    {servoNum, pin, minDeg, maxDeg, ...},
    // ... per servo
};

// Runtime data
servoRunDataC1 C1_run_R[NUM_SERVO_PINS];

// Hardware mapping
RS5HardwareManager hardware[NUM_SERVO_PINS];
```

## Communication Patterns

### Inter-Core Communication

**Semaphore Usage**:
```cpp
SemaphoreHandle_t systemStateLock;    // System state protection
SemaphoreHandle_t neoPixelEyesLock;   // Eye LED data
SemaphoreHandle_t neoPixelDMXLock;    // DMX LED mapping
SemaphoreHandle_t runDataLock;        // Servo runtime data
```

**Data Flow**:
1. Core 0 receives DMX packet
2. Core 0 validates and locks runDataLock
3. Core 0 updates target positions
4. Core 0 releases lock
5. Core 1 reads positions (lock-free read)
6. Core 1 calculates motion profiles
7. Core 1 updates PWM outputs

### DMX Protocol Implementation

**Frame Structure**:
```
[Break][MAB][Start Code][Channel 1-512]
```

**Validation**:
- Start code must be 0x00
- Packet age < 200ms
- Address range validation

### Status Communication

**NeoPixel Protocol**:
- Pixel 0: System status
- Pixels 1-5: Individual servo status
- Pixel 6: Eye effect output

**Status Colors**:
- Green: Normal operation
- Red: Error/No signal
- Blue: Boot/Setup
- Yellow: Movement active
- Off: Sleep mode

## Memory Management

### Static Allocation Strategy

All memory is statically allocated at compile time:

```cpp
// Fixed buffers
uint8_t bufferDmx[513];              // DMX buffer
Derivs_Limiter DL[NUM_SERVO_PINS];   // Motion profiles
RP2040_PWM* servoInstance[5];        // PWM instances
```

### Memory Map

```
┌─────────────────┐ 0x20000000
│   Stack Core 0  │
├─────────────────┤
│   Stack Core 1  │
├─────────────────┤
│  Static Data    │
│  - DMX Buffer   │
│  - Servo Config │
│  - Motion Data  │
├─────────────────┤
│      Heap       │
│   (Minimal)     │
└─────────────────┘ 0x20042000
```

Total RAM Usage: ~24KB of 264KB available

## Performance Characteristics

### Timing Requirements

- **DMX Reception**: 250kbaud, 44Hz update rate
- **Servo Update**: 50Hz PWM, 20ms period
- **Motion Calculation**: <1ms per frame
- **LED Update**: 30Hz for smooth effects

### Latency

- **DMX to Servo**: <20ms typical
- **Position Change**: Governed by acceleration limits
- **Status Update**: <10ms

### CPU Usage

- **Core 0**: ~20% (mostly idle waiting for DMX)
- **Core 1**: ~40% (motion calculations)

## Error Handling

### DMX Errors

**Timeout Handling**:
```cpp
if (millis() > dmxInput.latest_packet_timestamp() + 200) {
    // Enter safe mode
    // Hold last positions or return to home
}
```

### Servo Errors

**Range Protection**:
- Hardware limits enforced in ServoConfig
- Software limits in motion profile
- PWM bounds checking

### System Errors

**Watchdog Timer**: Not implemented (consider adding)
**Stack Overflow**: Protected by FreeRTOS
**Deadlock Prevention**: Timeout on all semaphores

## Extension Points

### Adding New Servo Axes

1. Update NUM_SERVO_PINS in RS5Hardware.h
2. Add configuration to C1_config_R array
3. Assign GPIO pin and PWM slice
4. Update DMX channel mapping

### Custom Effects

1. Add effect profile to eyeLight array
2. Define DMX trigger ranges
3. Implement effect function
4. Update flickerEyes() logic

### Protocol Extensions

1. DMX RDM support (not implemented)
2. Art-Net compatibility (future)
3. sACN/E1.31 support (future)

## Testing Considerations

### Unit Testing Points

- Motion profile calculations
- DMX packet validation
- PWM duty cycle conversion
- Semaphore deadlock prevention

### Integration Testing

- DMX signal loss recovery
- Servo limit testing
- Multi-axis coordination
- Effect transitions

### Performance Testing

- Maximum servo update rate
- DMX packet processing time
- Worst-case latency
- Power consumption profiles