# SkullMasterV2 Design Decisions

## Overview

This document captures the key design decisions made during the development of SkullMasterV2, explaining the rationale behind architectural choices, trade-offs considered, and lessons learned from previous versions.

## Platform Selection

### Decision: RP2040 over ESP32

**Chosen**: Raspberry Pi RP2040 (Dual Cortex-M0+)

**Alternatives Considered**:
- ESP32 (Dual Xtensa LX6)
- STM32F4 (Single Cortex-M4)
- Arduino Mega 2560 (ATmega2560)

**Rationale**:
1. **Dual-core architecture** enables separation of real-time servo control from DMX processing
2. **PIO state machines** provide hardware-accurate PWM without CPU overhead
3. **Deterministic timing** - No WiFi/Bluetooth interrupts like ESP32
4. **Cost-effective** - Pico boards under $10
5. **Strong community support** with arduino-pico core

**Trade-offs**:
- Lost WiFi/Bluetooth capabilities (not needed for DMX)
- Lower clock speed than STM32F4 (acceptable for our needs)
- Less RAM than ESP32 (264KB sufficient)

## Software Architecture

### Decision: Dual-Core Task Separation

**Design**:
- Core 0: DMX reception, system state, user input
- Core 1: Servo control, motion profiling, LED effects

**Rationale**:
1. **Isolation of critical tasks** - Servo jitter eliminated
2. **Simplified debugging** - Clear responsibility boundaries
3. **Better resource utilization** - Parallel processing
4. **Failure isolation** - DMX loss doesn't affect servo smoothness

**Implementation**:
```cpp
// Core 0 focuses on external communication
void loop() {
    checkDMX();
    readDipSwitches();
    updateSystemState();
}

// Core 1 focuses on real-time control
void loop1() {
    calculateMotion();
    updateServos();
    updateLEDs();
}
```

### Decision: Static Memory Allocation

**Chosen**: All memory allocated at compile time

**Rationale**:
1. **Predictable behavior** - No runtime allocation failures
2. **No heap fragmentation** - Critical for long-running systems
3. **Faster execution** - No malloc/free overhead
4. **Easy memory analysis** - Know usage at compile time

**Implementation**:
```cpp
// Static allocation examples
uint8_t bufferDmx[513];  // Fixed DMX buffer
Derivs_Limiter DL[5];    // Fixed servo count
RP2040_PWM* servoInstance[5];  // Pointers allocated once
```

## Communication Protocols

### Decision: DMX512 as Primary Protocol

**Rationale**:
1. **Industry standard** for theatrical lighting
2. **Robust** - Differential signaling, isolated
3. **Real-time** - Consistent 44Hz update rate
4. **Simple** - No handshaking or acknowledgments
5. **Compatible** with existing lighting consoles

**Implementation Details**:
- Hardware UART for reliability
- Asynchronous reception to prevent blocking
- Frame validation (start code = 0x00)
- Timeout detection (200ms)

### Decision: Binary DIP Switches for Addressing

**Chosen**: 9-bit binary DIP switch array

**Alternatives Considered**:
- Rotary BCD switches
- Software configuration via serial
- EEPROM storage

**Rationale**:
1. **Field configurable** without computer
2. **Visible configuration** - No guessing
3. **Survives power cycles** - Hardware state
4. **No EEPROM wear** concerns
5. **Immediate changes** - No reboot needed

## Motion Control

### Decision: Trapezoidal Velocity Profiles

**Chosen**: Acceleration-limited motion with trapezoidal velocity

**Alternatives Considered**:
- Direct position mapping (instant)
- S-curve profiles
- Sinusoidal smoothing

**Rationale**:
1. **Mechanical sympathy** - Reduces servo wear
2. **Natural movement** - Looks more lifelike
3. **Configurable dynamics** - Per-axis tuning
4. **Computational efficiency** - Simple math
5. **Predictable timing** - Easy to choreograph

**Implementation**:
```cpp
// Configurable per servo
#define JAW_SERVO_MAXVEL 180   // deg/sec
#define JAW_SERVO_MAXACC 360   // deg/sec²
```

### Decision: Servo Sleep Mode

**Innovation**: Disable PWM when servo stationary

**Rationale**:
1. **Power savings** - Significant in multi-servo systems
2. **Reduced heat** - Servos run cooler
3. **Less noise** - Eliminates holding current buzz
4. **Extended lifespan** - Less continuous stress

**Trade-off**: Small wake-up delay (mitigated by predictive wake)

## User Interface

### Decision: NeoPixel Status System

**Design**: 7-pixel strip showing system and servo status

**Layout**:
- Pixel 0: System status
- Pixels 1-5: Individual servo status
- Pixel 6: Eye effect output

**Rationale**:
1. **Rich information** - Multiple states via color
2. **At-a-glance monitoring** - No terminal needed
3. **Diagnostic value** - See issues immediately
4. **Production friendly** - Works in dark theaters

**Color Coding**:
- Blue: Booting/Setup
- Green: Normal operation
- Red: Error/No signal
- Yellow: Movement active
- Off: Sleep mode

## Safety & Reliability

### Decision: Hardware Enable Pin

**Design**: GPIO-controlled output enable

**Rationale**:
1. **Emergency stop** capability
2. **Safe startup** - Outputs disabled during boot
3. **External interlock** possible
4. **Power sequencing** control

### Decision: Semaphore-Protected Shared Data

**Implementation**: FreeRTOS semaphores for all shared variables

**Rationale**:
1. **Thread safety** - Prevent race conditions
2. **Data integrity** - Atomic updates
3. **Predictable behavior** - No corruption
4. **Standard pattern** - Well-understood

**Example**:
```cpp
void setTargetPos(int pos) {
    xSemaphoreTake(runDataLock, 100);
    targetPos = pos;
    xSemaphoreGive(runDataLock);
}
```

## Performance Optimizations

### Decision: Compile-Time Configuration

**Approach**: #define constants vs runtime variables

**Rationale**:
1. **Compiler optimization** - Dead code elimination
2. **Constant folding** - Math at compile time
3. **Smaller binary** - Unused features removed
4. **Better performance** - No indirection

### Decision: Selective Debug Output

**Design**: Multiple debug levels, compile-time selectable

**Rationale**:
1. **Production performance** - Zero overhead when disabled
2. **Detailed diagnostics** - When needed
3. **Selective focus** - Debug specific subsystems
4. **Serial bandwidth** management

## Theatrical Considerations

### Decision: Location-Based Configuration

**Innovation**: Different DMX mappings per physical location

**Rationale**:
1. **Show flexibility** - Same hardware, different scenes
2. **Quick changes** - DIP switch selectable
3. **No reprogramming** - Field configurable
4. **Multiple installations** - One firmware serves all

### Decision: Pirate-Themed Effects

**Implementation**: Custom eye flicker patterns

**Patterns**:
- Fire effects (red/orange)
- Ghost effects (green/cyan)
- Lightning (white/blue)
- Custom color modes

**Rationale**:
1. **Thematic consistency** - Matches show aesthetic
2. **Dramatic impact** - Enhances performance
3. **DMX controllable** - Lighting designer friendly
4. **Smooth transitions** - Professional appearance

## Lessons Learned

### From Version 1.0
- Single-core insufficient for smooth operation
- ESP32 WiFi interrupts caused servo jitter
- Dynamic memory allocation led to crashes

### From Version 2.0
- PIO-based PWM superior to software PWM
- Acceleration limiting essential for realism
- Status indication critical for troubleshooting

### From Version 3.0
- FreeRTOS provides better structure than super-loop
- Compile-time configuration reduces complexity
- Hardware enable pin prevents damage during development

## Future Considerations

### Potential Enhancements
1. **RDM Support** - Remote configuration via DMX
2. **Choreography Storage** - Onboard sequence playback
3. **Current Monitoring** - Per-servo protection
4. **Position Feedback** - Closed-loop control
5. **Art-Net/sACN** - Ethernet control option

### Scalability Options
1. **Daisy-chain Communication** - Multi-skull synchronization
2. **Wireless DMX** - Reduce cabling
3. **SD Card Logging** - Performance analytics
4. **Web Configuration** - If Pico W used

## Design Validation

### Testing Methodology
1. **Bench Testing** - Individual servo verification
2. **Integration Testing** - Full system DMX control
3. **Stress Testing** - 24-hour continuous operation
4. **Field Testing** - Live show performance
5. **EMI Testing** - Interference immunity

### Success Metrics
- Zero servo jitter during normal operation
- <20ms DMX to movement latency
- 99.9% uptime in production
- No thermal issues after 8-hour shows
- Field-configurable without tools

## Conclusion

The SkullMasterV2 design successfully balances performance, reliability, and usability for theatrical animatronics. The dual-core architecture, hardware PWM, and careful separation of concerns create a robust platform that has proven reliable in demanding show environments. The focus on compile-time configuration and static allocation ensures predictable behavior, while the comprehensive status system enables quick troubleshooting.

Key success factors:
- Clear separation of real-time and communication tasks
- Hardware-accelerated PWM for jitter-free operation
- Comprehensive but non-intrusive debugging capabilities
- Field-friendly configuration without computers
- Theatrical-specific features and effects

This design serves as a solid foundation for future animatronic projects and demonstrates the effectiveness of the RP2040 platform for real-time control applications.