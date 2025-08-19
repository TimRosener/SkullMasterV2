# SkullMasterV2 Troubleshooting Guide

## Quick Diagnostic Checklist

Before detailed troubleshooting, verify:
- [ ] Power supplies connected and correct voltage
- [ ] DMX cable connected with proper termination
- [ ] DIP switches set to valid DMX address
- [ ] Status LED showing some activity
- [ ] No burning smell or excessive heat
- [ ] All ground connections secure

## Common Issues and Solutions

### System Won't Start

#### Symptom: No LEDs, no servo movement, no serial output

**Possible Causes & Solutions:**

1. **No Power**
   - Check 5V supply with multimeter
   - Verify USB cable if powered via USB
   - Check power supply polarity
   - Test with different power source

2. **RP2040 Not Programmed**
   - Enter bootloader (hold BOOTSEL + reset)
   - Should appear as USB drive
   - Re-upload firmware

3. **Damaged Board**
   - Check for shorts with multimeter
   - Look for burnt components
   - Test with minimal connections

#### Symptom: Status LED stuck on blue

**Cause**: Boot sequence not completing

**Solutions:**
- Check serial output for error messages
- Verify FreeRTOS is included in build
- Ensure both cores are starting
- Check for semaphore deadlock

Debug code to add:
```cpp
void setup() {
    Serial.begin(115200);
    Serial.println("Core 0 starting...");
    // Add after each major step
    Serial.println("Checkpoint 1 reached");
}
```

### DMX Issues

#### Symptom: Red flashing status LED

**Cause**: No valid DMX signal detected

**Solutions:**

1. **Check DMX Wiring**
   ```
   Verify at DMX connector:
   - Pin 1: Ground
   - Pin 2: Data- (Cold)
   - Pin 3: Data+ (Hot)
   - 120Ω between pins 2 & 3
   ```

2. **Test MAX485 Circuit**
   - Measure 5V at MAX485 pin 8
   - Check RE and DE pins are grounded
   - Verify RO connects to GPIO 1

3. **DMX Signal Issues**
   - Use DMX tester to verify signal
   - Check DMX controller output
   - Try different DMX universe
   - Reduce cable length

4. **Software Configuration**
   ```cpp
   // Enable DMX debugging
   systemState.setDebugLevel(DebugLevelDMX);
   
   // Check serial output for:
   // "Bad DMX Frame" - Start code not 0
   // "No DMX" - Timeout exceeded
   ```

#### Symptom: DMX received but wrong channels respond

**Cause**: Incorrect address configuration

**Solutions:**
1. Calculate correct DIP switch settings:
   ```
   DMX Address 17:
   Binary: 00010001
   Switches: 1=ON, 5=ON, others OFF
   ```

2. Verify in debug output:
   ```cpp
   Serial.printf("DMX Address: %d\n", 
                 systemState.getDMXAddress());
   ```

### Servo Problems

#### Symptom: Servo not moving at all

**Diagnostics & Solutions:**

1. **Check Servo Power**
   - Measure voltage at servo connector
   - Should be 5-6V typically
   - Check current capacity (1A per servo minimum)

2. **Verify Servo Wiring**
   ```
   Standard Servo Connector:
   - Brown/Black: Ground
   - Red: Power (5-6V)
   - Orange/Yellow/White: Signal
   ```

3. **Test Servo Directly**
   ```cpp
   // Bypass motion engine for testing
   void testServo(int pin) {
       RP2040_PWM* test = new RP2040_PWM(pin, 50, 7.5);
       test->setPWM();
       delay(1000);
       test->setPWM(pin, 50, 5.0);  // Min position
       delay(1000);
       test->setPWM(pin, 50, 10.0); // Max position
   }
   ```

4. **Check Licensing**
   ```cpp
   // Verify servo is enabled
   if (!C1_config_R[servo_num].licensed) {
       Serial.println("Servo not licensed!");
   }
   ```

#### Symptom: Servo jittering or unstable

**Causes & Solutions:**

1. **Power Supply Issues**
   - Add capacitors (1000µF) near servos
   - Use separate servo power supply
   - Check for voltage drops under load

2. **PWM Signal Problems**
   ```cpp
   // Adjust PWM parameters
   #define SERVO_FREQ 50.0f  // Try 40-60Hz
   #define MIN_PWM 500       // Adjust for your servo
   #define MAX_PWM 2500      // Standard range
   ```

3. **Mechanical Issues**
   - Check for binding in mechanism
   - Verify servo arm attachment
   - Look for excessive load

#### Symptom: Servo moves to wrong positions

**Solutions:**

1. **Calibrate PWM Range**
   ```cpp
   // Fine-tune for your servos
   #define JAW_SERVO_MINPWM 544   // Actual measured
   #define JAW_SERVO_MAXPWM 2400  // Actual measured
   ```

2. **Adjust Degree Mapping**
   ```cpp
   // Correct physical range
   #define JAW_SERVO_MINDEG 0    // Fully closed
   #define JAW_SERVO_MAXDEG 90   // Fully open
   ```

### LED/NeoPixel Issues

#### Symptom: No LED output at all

**Solutions:**

1. **Check Connections**
   - Verify 5V and GND to NeoPixel
   - Check data line to GPIO 28
   - Add 470Ω resistor on data line

2. **Test with Simple Code**
   ```cpp
   void testLEDs() {
       pixels.begin();
       pixels.clear();
       pixels.setPixelColor(0, 255, 0, 0); // Red
       pixels.show();
   }
   ```

3. **Verify Pixel Count**
   ```cpp
   #define NUMPIXELS 7  // Match actual strip
   ```

#### Symptom: Wrong colors or flickering

**Solutions:**

1. **Check Color Order**
   ```cpp
   // Try different orders
   Adafruit_NeoPixel pixels(NUMPIXELS, PIN, 
                            NEO_GRB + NEO_KHZ800);
   // Or NEO_RGB, NEO_BGR, etc.
   ```

2. **Power Supply**
   - Ensure adequate current (60mA per pixel max)
   - Add capacitor at strip power input
   - Use shorter data wire

### Performance Issues

#### Symptom: Delayed servo response

**Diagnostics:**
```cpp
// Measure loop timing
unsigned long loopStart = millis();
// ... loop code ...
unsigned long loopTime = millis() - loopStart;
if (loopTime > 20) {
    Serial.printf("Slow loop: %lu ms\n", loopTime);
}
```

**Solutions:**

1. **Reduce Debug Output**
   ```cpp
   systemState.setDebugLevel(DebugLevelNone);
   ```

2. **Optimize Motion Calculations**
   ```cpp
   // Reduce unnecessary calculations
   if (abs(target - current) < 1) {
       return; // Skip tiny movements
   }
   ```

3. **Adjust Update Rates**
   ```cpp
   #define SERVO_UPDATE_RATE 30  // Try 30ms vs 20ms
   ```

#### Symptom: Servos move too slowly

**Solutions:**

1. **Increase Velocity Limits**
   ```cpp
   #define JAW_SERVO_MAXVEL 360  // Degrees/second
   ```

2. **Adjust Acceleration**
   ```cpp
   #define JAW_SERVO_MAXACC 720  // Deg/sec²
   ```

3. **Disable Smoothing**
   ```cpp
   C1_config_R[i].smooth = false; // Instant movement
   ```

## Debug Mode Usage

### Enabling Debug Output

```cpp
// Via serial command (implement handler)
void processSerialCommand(char cmd) {
    switch(cmd) {
        case '0': systemState.setDebugLevel(0); break;
        case '1': systemState.setDebugLevel(1); break;
        case '2': systemState.setDebugLevel(2); break;
        // etc.
    }
}
```

### Debug Level Outputs

**Level 1 - Startup**
```
Rose&Swan
Pirate Servo DMX Version Alpha 3.1
Core Zero: Starting, Debug Level:1
Core Zero: DMX Start Addr:17
Core One: Starting Setup
```

**Level 2 - DMX**
```
DMX Address:17  Frame Status:0  
1:127  2:64  3:255  4:0  5:128
```

**Level 3 - Servo**
```
CurrentPosition:45, TargetPosition:90, 
MaxDegrees:135, MinDegrees:45
```

**Level 4 - Model**
```
Servo 0, Target:90, Position:67, 
Velocity:120, Status:1, DutyCycle:7.5
```

**Level 5 - Power**
```
Low:0,Current:850,high:50
```

**Level 6 - Pixel**
```
RGB Mode Pixel:6 Brightness:255 
R:255 G:128 B:0
```

## Error Codes

### DMX Errors
- **Frame Error**: Start byte not 0x00
- **Timeout Error**: No packet for 200ms
- **Address Error**: Requested address > 512

### Servo Errors
- **Not Licensed**: Servo disabled in config
- **PWM Error**: State machine allocation failed
- **Range Error**: Position outside limits

### System Errors
- **Boot Timeout**: Core synchronization failed
- **Semaphore Error**: Lock acquisition timeout
- **Memory Error**: Allocation failed (rare)

## Testing Procedures

### Basic Functionality Test

```cpp
void runDiagnostics() {
    Serial.println("=== DIAGNOSTICS START ===");
    
    // Test each servo
    for (int i = 0; i < NUM_SERVO_PINS; i++) {
        Serial.printf("Testing Servo %d...\n", i);
        C1_run_R[i].settargetPos(C1_config_R[i].minDeg);
        delay(1000);
        C1_run_R[i].settargetPos(C1_config_R[i].maxDeg);
        delay(1000);
    }
    
    // Test LEDs
    Serial.println("Testing LEDs...");
    for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, 255, 0, 0);
        pixels.show();
        delay(200);
    }
    pixels.clear();
    pixels.show();
    
    // Test DMX
    Serial.println("Waiting for DMX...");
    for (int i = 0; i < 50; i++) {
        if (checkDMX()) {
            Serial.println("DMX OK!");
            break;
        }
        delay(100);
    }
    
    Serial.println("=== DIAGNOSTICS END ===");
}
```

### Stress Testing

```cpp
void stressTest() {
    Serial.println("Starting stress test...");
    
    unsigned long errors = 0;
    unsigned long cycles = 0;
    
    while (true) {
        // Rapid servo movements
        for (int i = 0; i < NUM_SERVO_PINS; i++) {
            int pos = random(C1_config_R[i].minDeg, 
                           C1_config_R[i].maxDeg);
            C1_run_R[i].settargetPos(pos);
        }
        
        // Random LED colors
        pixels.setPixelColor(EYESPIXEL, 
                            random(256), 
                            random(256), 
                            random(256));
        pixels.show();
        
        // Check for issues
        if (!checkDMX()) errors++;
        
        cycles++;
        if (cycles % 1000 == 0) {
            Serial.printf("Cycles: %lu, Errors: %lu\n", 
                         cycles, errors);
        }
        
        delay(10);
    }
}
```

## Recovery Procedures

### Factory Reset

```cpp
void factoryReset() {
    Serial.println("FACTORY RESET");
    
    // Reset all servos to center
    for (int i = 0; i < NUM_SERVO_PINS; i++) {
        C1_run_R[i].settargetPos(90);
        C1_config_R[i].ServoStartDeg = 90;
    }
    
    // Clear DMX buffer
    memset(bufferDmx, 0, sizeof(bufferDmx));
    
    // Reset system state
    systemState.setMode(RunModeDMX);
    systemState.setDebugLevel(DebugLevelNone);
    systemState.setDMXAddress(1);
    
    // Clear LEDs
    pixels.clear();
    pixels.show();
    
    Serial.println("Reset complete - restart required");
    while(1); // Force restart
}
```

### Safe Mode

```cpp
void enterSafeMode() {
    Serial.println("SAFE MODE");
    
    // Disable all servos
    for (int i = 0; i < NUM_SERVO_PINS; i++) {
        servoInstance[i]->disablePWM();
    }
    
    // Set status LED to yellow
    pixels.setPixelColor(STATUSPIXEL, 255, 255, 0);
    pixels.show();
    
    // Wait for serial commands only
    while (true) {
        if (Serial.available()) {
            char cmd = Serial.read();
            if (cmd == 'R') {
                // Reset command
                NVIC_SystemReset();
            }
        }
        delay(100);
    }
}
```

## Contact Support

If problems persist after troubleshooting:

1. **Document the Issue**
   - Software version
   - Hardware configuration
   - Steps to reproduce
   - Debug output logs

2. **Check Resources**
   - GitHub issues page
   - Hardware manufacturer docs
   - Arduino forum

3. **Prepare Information**
   - Wiring diagram
   - Config file changes
   - Video of behavior
   - Multimeter readings