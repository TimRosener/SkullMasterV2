# SkullMasterV2 API Reference

## Core Classes

### System Class
**File**: RS5DMX.h  
**Purpose**: Central system state management and configuration

```cpp
class System {
public:
    // Boot sequence control
    void setBootLevel(byte Level);
    int getBootLevel();
    
    // Operating modes
    void setMode(byte Level);
    int getMode();
    
    // Debug configuration
    void setDebugLevel(byte Level);
    int getDebugLevel();
    void setDebugServo(byte Level);
    int getDebugServo();
    
    // DMX configuration
    void setDMXAddress(int i);
    int getDMXAddress();
    void setDMXAddressEyes(int i);
    
    // Power monitoring
    void setServoVoltage(int voltage);
    int getServoVoltage();
    void setServoCurrent(int current);
    int getServoCurrent();
    
    // Timing utilities
    bool timeToSample();
    bool timeToSampleDMX();
    bool timeToSampleRM();
};
```

**Global Instance**: `System systemState`

**Usage Example**:
```cpp
// Set debug level
systemState.setDebugLevel(DebugLevelDMX);

// Check DMX address
int addr = systemState.getDMXAddress();

// Timing check
if (systemState.timeToSample()) {
    // Perform periodic task
}
```

---

### ServoConfig Class
**File**: RS5DMX.h  
**Purpose**: Servo configuration parameters

```cpp
class ServoConfig {
public:
    bool licensed;           // Servo enabled
    int servoNum;           // Servo ID (0-4)
    char servoUserName[40]; // Display name
    float freq;             // PWM frequency (Hz)
    float minDeg;           // Minimum angle
    float maxDeg;           // Maximum angle
    float minPWM;           // Min pulse width (µs)
    float maxPWM;           // Max pulse width (µs)
    float maxVel;           // Max velocity (deg/s)
    float maxAcc;           // Max acceleration (deg/s²)
    float maxDec;           // Max deceleration (deg/s²)
    bool smooth;            // Enable smoothing
    int servoSleepTimer;    // Idle timeout (ms)
    
    bool isServoLicensed();
};
```

**Global Array**: `std::array<ServoConfig, NUM_SERVO_PINS> C1_config_R`

---

### runData Class
**File**: RS5DMX.h  
**Purpose**: Runtime servo state tracking

```cpp
class runData {
public:
    int servoNum;        // Servo ID
    bool active;         // Currently active
    float curentPos;     // Current position (degrees)
    float targetPos;     // Target position (degrees)
    float previousPos;   // Previous position
    int lastMove;        // Last movement time (ms)
    bool PwmEnabled;     // PWM output state
    
    // Thread-safe setters
    void setcurentPos(int i);
    void settargetPos(int i);
    void setpreviousPos(int i);
    
    // Getters
    int getcurentPos();
    int gettargetPos();
    bool isServoActive();
};
```

**Global Array**: `std::array<runData, NUM_SERVO_PINS> C1_run_R`

---

### Derivs_Limiter Class
**File**: ServoEngine.h  
**Purpose**: Motion profile generation with acceleration limiting

```cpp
class Derivs_Limiter {
public:
    Derivs_Limiter(float maxVelocity, 
                   float maxAcceleration,
                   float maxDeceleration, 
                   float initialPosition,
                   float initialVelocity = 0,
                   float initialAcceleration = 0,
                   bool printStuff = false,
                   bool asymmetric = false,
                   float minLimit = -INFINITY,
                   float maxLimit = INFINITY);
    
    void setTarget(float target);
    void calc();
    float getPosition();
    float getVelocity();
    float getAcceleration();
    bool isPosAtTarget();
};
```

**Usage Example**:
```cpp
// Initialize motion profile
Derivs_Limiter DL[NUM_SERVO_PINS];
DL[0] = Derivs_Limiter(180, 360, 360, 90);

// Update position
DL[0].setTarget(targetAngle);
DL[0].calc();
float currentPos = DL[0].getPosition();
```

---

### RP2040_PWM Class
**File**: ServoDriver.h  
**Purpose**: Hardware PWM generation for servo control

```cpp
class RP2040_PWM {
public:
    RP2040_PWM(uint8_t pin, float freq, float dutyCycle);
    
    bool setPWM();
    bool setPWM(uint8_t pin, float frequency, float dutyCycle);
    bool setPWM_Period(uint8_t pin, uint32_t period, float dutyCycle);
    bool setPWM_manual(uint8_t pin, uint16_t top, uint8_t div, 
                      float dutyCycle, bool phaseCorrect = false);
    
    float getActualFreq();
    float getDutyCycle();
    void enablePWM();
    void disablePWM();
};
```

---

### DmxInput Class
**File**: DmxInput.h (Pico-DMX Library)  
**Purpose**: DMX512 reception

```cpp
class DmxInput {
public:
    void begin(uint8_t pin, uint16_t start_channel, 
              uint16_t num_channels);
    void read_async(volatile uint8_t* buffer);
    bool read(volatile uint8_t* buffer);
    unsigned long latest_packet_timestamp();
    uint16_t latest_packet_size();
    bool is_connected();
};
```

**Global Instance**: `DmxInput dmxInput`

---

## Core Functions

### Setup Functions

#### setup()
**File**: SkullMasterV2.ino  
**Core**: 0  
**Purpose**: Initialize Core 0 systems

```cpp
void setup() {
    // Initialize semaphores
    // Configure serial port
    // Set up DIP switches
    // Start DMX receiver
    // Begin boot sequence
}
```

#### setup1()
**File**: SkullMasterV2.ino  
**Core**: 1  
**Purpose**: Initialize Core 1 systems

```cpp
void setup1() {
    // Wait for Core 0
    // Configure servo defaults
    // Initialize NeoPixels
    // Start servo state machines
    // Begin motion profiles
}
```

---

### Main Loop Functions

#### loop()
**File**: SkullMasterV2.ino  
**Core**: 0  
**Purpose**: Core 0 main loop

```cpp
void loop() {
    // Read DIP switches
    // Check DMX signal
    // Update system state
    // Process DMX data
}
```

#### loop1()
**File**: SkullMasterV2.ino  
**Core**: 1  
**Purpose**: Core 1 main loop

```cpp
void loop1() {
    // Calculate servo positions
    // Update PWM outputs
    // Control LED effects
    // Monitor servo status
}
```

---

### DMX Functions

#### checkDMX()
**Returns**: bool - true if valid DMX signal  
**Purpose**: Verify DMX signal presence

```cpp
bool checkDMX() {
    // Check packet age
    // Return signal status
}
```

#### readDMX()
**Returns**: bool - true if successful  
**Purpose**: Process DMX data into servo targets

```cpp
bool readDMX() {
    // Validate start code
    // Map DMX to servo positions
    // Update eye settings
}
```

#### ReadDmxDipSwitches()
**Purpose**: Read DIP switches for DMX configuration

```cpp
void ReadDmxDipSwitches() {
    // Read 9-bit DMX address
    // Read 3-bit run mode
    // Update system state
}
```

---

### Servo Control Functions

#### setServoPositions()
**Purpose**: Calculate and apply servo movements

```cpp
void setServoPositions() {
    // For each servo:
    //   Set target position
    //   Calculate motion profile
    //   Update current position
    //   Apply PWM signal
}
```

#### getDutyCycle(int i)
**Parameters**: i - servo index  
**Returns**: float - duty cycle percentage  
**Purpose**: Convert position to PWM duty cycle

```cpp
float getDutyCycle(int i) {
    // Map degrees to PWM range
    // Calculate duty cycle
    // Return percentage
}
```

#### servoMonitor()
**Purpose**: Update servo status indicators

```cpp
void servoMonitor() {
    // Check each servo
    // Update status LED
    // Detect movement/sleep
}
```

---

### LED Effect Functions

#### flickerEyes(int profile)
**Parameters**: profile - effect preset index  
**Purpose**: Generate eye flicker effects

```cpp
void flickerEyes(int i) {
    // Select color based on timing
    // Apply flicker pattern
    // Update brightness
}
```

#### setEyeColor()
**Purpose**: Direct RGB control of eye LEDs

```cpp
void setEyeColor() {
    // Read RGB values from DMX
    // Apply brightness
    // Update pixel
}
```

#### statusLed(int status)
**Parameters**: status - status code  
**Purpose**: Update system status LED

```cpp
void statusLed(int i) {
    // Look up color code
    // Set pixel color
}
```

#### sendPixelFrame()
**Purpose**: Update NeoPixel display

```cpp
void sendPixelFrame() {
    // Check update timer
    // Send pixel data
}
```

---

### Utility Functions

#### floatMap()
**Parameters**: 
- x - input value
- in_min, in_max - input range
- out_min, out_max - output range
**Returns**: float - mapped value
**Purpose**: Map value between ranges

```cpp
float floatMap(float x, float in_min, float in_max, 
               float out_min, float out_max);
```

---

## Data Structures

### StatusLight Structure
**Purpose**: LED status configuration

```cpp
class StatusLight {
    int red, green, blue;  // Color values
    int rate;              // Flash rate (ms)
    int updateTimer;       // Last update time
    
    bool timeToUpdateStatus();
};
```

### NeoPixelDMX Structure
**Purpose**: DMX-controlled LED settings

```cpp
class NeoPixelDMX {
    int mode;         // Effect mode
    int brightness;   // Global brightness
    int red, green, blue;  // RGB values
    int updateRate;   // Refresh rate
};
```

### Hardware Structure
**Purpose**: Pin and state machine mapping

```cpp
class Hardware {
    int pin;           // GPIO pin
    int StateMachine;  // PWM state machine
    
    int getServoPin();
    int getStateMachine();
};
```

---

## Enumerations

### Boot Modes
```cpp
enum bootMode {
    BootModeCore0A = 0,   // Core 0 setup
    BootModeCore1A = 1,   // Core 1 setup
    BootModeCore0B = 2,   // Core 0 start
    BootModeRun = 3       // Running
};
```

### Run Modes
```cpp
enum RunMode {
    RunModeDMX = 0,       // DMX control
    RunModeSerial = 1,    // Serial control
    RunModeProgram = 2,   // Programmed sequence
    RunModePause = 3,     // Paused
    RunModeDemo = 4       // Demo mode
};
```

### Debug Levels
```cpp
enum debugLevel {
    DebugLevelNone = 0,        // No debug
    DebugLevelStartup = 1,     // Boot only
    DebugLevelDMX = 2,         // DMX frames
    DebugLevelServo = 3,       // Servo tracking
    DebugLevelModel = 4,       // Motion model
    DebugLevelVoltCurrent = 5, // Power monitoring
    DebugLevelPixel = 6        // LED debugging
};
```

---

## Thread Safety

### Semaphores
```cpp
SemaphoreHandle_t systemStateLock;   // System state
SemaphoreHandle_t neoPixelEyesLock;  // Eye LEDs
SemaphoreHandle_t neoPixelDMXLock;   // DMX LEDs
SemaphoreHandle_t runDataLock;       // Servo data
```

### Lock Functions
```cpp
void system_state_lock();
void system_state_unlock();
void neopixel_eye_lock();
void neopixel_eye_unlock();
void neopixel_DMX_lock();
void neopixel_DMX_unlock();
void run_Data_lock();
void run_Data_unlock();
```

---

## Constants

### Pin Definitions (RS5Hardware.h)
```cpp
#define DMX_PIN 1              // DMX input
#define SERVO_PIN_1 4          // Jaw
#define SERVO_PIN_2 5          // Yaw
#define SERVO_PIN_3 6          // Pitch
#define SERVO_PIN_4 7          // Roll
#define SERVO_PIN_5 8          // Eyes
#define PIN 28                 // NeoPixel data
#define ENABLEOUTPUT_PIN 3     // Output enable
```

### Configuration
```cpp
#define NUM_SERVO_PINS 12      // Total servo slots
#define NUM_LIC_SERVOS 5       // Active servos
#define NUMPIXELS 7            // LED count
#define SOFTWARE_VERSION "Alpha 3.1"
```

### DMX Addresses
```cpp
#define EYEDMX_BOTNAYBAY 401
#define EYEDMX_SWANISLAND 409
#define EYEDMX_PORTOFDESTINY 417
#define EYEDMX_RUMISLAND 425
#define EYEDMX_TORTUGA 433
```

---

## Error Handling

### Error Detection
- DMX timeout: 200ms without valid packet
- Servo range: Enforced in ServoConfig
- PWM allocation: Check return values
- Semaphore timeout: 100ms max wait

### Error Recovery
- DMX loss: Hold last position or safe mode
- Servo fault: Disable PWM output
- Boot failure: Restart sequence
- Memory error: System reset

---

## Performance Metrics

### Update Rates
- Servo PWM: 50Hz (20ms)
- Motion calculation: 50Hz
- DMX check: 100Hz (10ms)
- LED update: 30Hz (33ms)
- Status update: 10Hz (100ms)

### Latency
- DMX to servo: <20ms
- Button to action: <50ms
- Effect change: <100ms

### Resource Usage
- RAM: ~24KB of 264KB
- Flash: ~94KB of 2MB
- CPU Core 0: ~20%
- CPU Core 1: ~40%