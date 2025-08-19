#include "DmxInput.h"


//**********************************************************************************
// Set Voltaget and Current Sense Constance
//#define SERVO_VOLT_LOW 0
//#define SERVO_VOLT_HIGH 1192
#define SERVO_CURRENT_LOW 0
#define SERVO_CURRENT_HIGH 2000

int demoTimer = 0;
int demoTimerDelay = 5;
int dipSwitchTimer = 0;
int dipSwitchDelay = 0;

class System {
public:
  int boot;
  int mode;
  int debug;
  int debugServo;
  int servoDebugSampleRate;
  int servoDebugLastSample;
  int demoTimer;
  int demoTimerDelay;
  int DMXdipSwitchTimer;
  int DMXdipSwitchDelay;
  int RMdipSwitchTimer;
  int RMdipSwitchDelay;
  int servoVoltage;
  int servoCurrent;
  int startDMX;
  int startDMXEyes;
  int DMXLastPacketTimeStamp;
  int DMXPacketAgeLimit;


public:
  System() {
    boot = 0;   // 0 = Core0 Setup, 1 = Core 1 Setup, 2 = Core 0 start, 3 = Core 0 Start, 5 = Run.
    mode = 0;   // 0 = DMX, 1 = Serial, 2 = Program, 3 = XXX, 4 = Demo Mode
    debug = 2;  // 0 = none, 1 = boot info Only, 2 = DebugLevelDMX 1, 3 = Debug servo 2, 4 = Model 5, 5= voltage and current, 6 Eye Pixel Debug
    debugServo = 0;
    servoDebugSampleRate = 200;
    servoDebugLastSample = 0;
    demoTimer = 0;
    demoTimerDelay = 50;
    DMXdipSwitchTimer = 0;
    DMXdipSwitchDelay = 500;
    RMdipSwitchTimer = 0;
    RMdipSwitchDelay = 500;
    servoVoltage = 0;
    servoCurrent = 0;
    startDMX = 0;
    startDMXEyes = 0;
    DMXLastPacketTimeStamp = 0;
    DMXPacketAgeLimit = 200;
  }

public:

  void setDMXLastPacketTimeStamp(int i) {
    DMXLastPacketTimeStamp = i;
  }

  int getDMXLastPacketTimeStamp() {
    return DMXLastPacketTimeStamp;
  }

  void setDMXPacketAgeLimit(int i) {
    DMXPacketAgeLimit = i;
  }

  int getDMXPacketAgeLimit() {
    return DMXPacketAgeLimit;
  }


  void setBootLevel(byte Level) {
    boot = Level;
  }

  void setMode(byte Level) {
    system_state_lock();
    mode = Level;
    system_state_unlock();
  }

  void setDebugLevel(byte Level) {
    system_state_lock();
    debug = Level;
    system_state_unlock();
  }

  void setDebugServo(byte Level) {
    system_state_lock();
    debugServo = Level;
    system_state_unlock();
  }

  void setServoVoltage(int voltage) {
    system_state_lock();
    servoVoltage = voltage;
    system_state_unlock();
  }

  void setServoCurrent(int current) {
    system_state_lock();
    servoCurrent = current;
    system_state_unlock();
  }

  void setDemoTimerDelay(int i) {
    system_state_lock();
    demoTimerDelay = i;
    system_state_unlock();
  }

  void setDemoTimer(int i) {
    system_state_lock();
    demoTimer = i;
    system_state_unlock();
  }

  void setDMXDipSwitchTimer(int i) {
    system_state_lock();
    DMXdipSwitchTimer = i;
    system_state_unlock();
  }

  void setDMXDipSwitchDelay(int i) {
    system_state_lock();
    DMXdipSwitchDelay = i;
    system_state_unlock();
  }

  void setRMDipSwitchTimer(int i) {
    system_state_lock();
    RMdipSwitchTimer = i;
    system_state_unlock();
  }

  void setRMDipSwitchDelay(int i) {
    system_state_lock();
    RMdipSwitchDelay = i;
    system_state_unlock();
  }

  void setDMXAddress(int i) {
    system_state_lock();
    startDMX = i;
    system_state_unlock();
  }

  void setDMXAddressEyes(int i) {
    system_state_lock();
    startDMXEyes = i;
    system_state_unlock();
  }

  int getBootLevel() {
    return boot;
  }

  int getMode() {
    return mode;
  }

  int getDebugLevel() {
    return debug;
  }

  int getDebugServo() {
    return debugServo;
  }

  int getServoVoltage() {
    return servoVoltage;
  }

  int getServoCurrent() {
    return servoCurrent;
  }

  int getDemoTimer() {
    return demoTimer;
  }

  int getDemoTimerDelay() {
    return demoTimerDelay;
  }

  int getDMXDipSwitchTimer() {
    return dipSwitchTimer;
  }

  int getRMDipSwitchDelay() {
    return RMdipSwitchDelay;
  }

  int getRMDipSwitchTimer() {
    return RMdipSwitchTimer;
  }

  int getDMXDipSwitchDelay() {
    return dipSwitchDelay;
  }

  int getDMXAddress() {
    return startDMX;
  }

  // Check to see if enough time has gone by to sample debug stats.
  bool timeToSample() {
    if (millis() > servoDebugLastSample + servoDebugSampleRate) {
      servoDebugLastSample = millis();
      return true;
    }
    return false;
  }

  bool timeToSampleDMX() {
    if (millis() > DMXdipSwitchTimer + DMXdipSwitchDelay) {
      DMXdipSwitchTimer = millis();
      return true;
    }
    return false;
  }

  bool timeToSampleRM() {
    if (millis() > RMdipSwitchTimer + RMdipSwitchDelay) {
      RMdipSwitchTimer = millis();
      return true;
    }
    return false;
  }

};

System systemState;

enum recordID {
  DataExchange = 1,
  SystemChange = 2
};
enum SystemChange {
  SystemChangeBoot = 1,
  SystemChangeRun = 2,
  SystemChangeDebug = 3
};

enum bootMode {
  BootModeCore0A = 0,
  BootModeCore1A = 1,
  BootModeCore0B = 2,
  BootModeRun = 3
};

enum RunMode {
  RunModeDMX = 0,
  RunModeSerial = 1,
  RunModeProgram = 2,
  RunModePause = 3,
  RunModeDemo = 4
};

enum debugLevel {
  DebugLevelNone = 0,
  DebugLevelStartup = 1,
  DebugLevelDMX = 2,
  DebugLevelServo = 3,
  DebugLevelModel = 4,
  DebugLevelVoltCurrent = 5,
  DebugLevelServoFull = 5,
  DebugLevelPixel = 6

};

//**********************************************************************************
// Status Light Configuration
class StatusLight {
public:
  int red;
  int green;
  int blue;
  int rate;
  int updateTimer;

  bool timeToUpdateStatus() {
    if (millis() > updateTimer + rate) {
      updateTimer = millis();
      return true;
    }
    return false;
  }

};

StatusLight statusLight[14] = {
  // r,g,b,rate         (Milliseconds)

  // Boot Color Codes
  { 1, 0, 0, 500, 0 },  // 0 - Boot Start
  { 1, 0, 1, 50, 0 },   // 1 - Boot Setting up Servo's

  // System Run Modes
  { 0, 0, 1, 300, 0 },  // 2 - DMX Recieve Mode
  { 1, 0, 0, 600, 0 },   // 3 - No DMX Connection
  { 0, 1, 0, 50, 0 },   // 4 - USB Recieve Mode
  { 1, 0, 0, 100, 0 },   // 5 - No USB Connection
  { 1, 1, 1, 50, 0 },   // 6 - Demo Mode

  // Servo Run Modes
  { 1, 0, 1, 100, 0 },  // 7 - Servo Startup
  { 0, 1, 0, 100, 0 },  // 8 - Servo Startup Success
  { 0, 0, 0, 100, 0 },  // 9 - Servo Not licensed
  { 1, 1, 0, 100, 0 },  // 10 - Servo Movement
  { 0, 1, 0, 100, 0 },  // 11 - Servo Still
  { 1, 0, 0, 100, 0 },  // 12 = Servo PWM Disabled
  { 0, 0, 0, 100, 0 }   // 13 - Status Light Off
};

#define STATUS_BOOT 0
#define STARTING_SERVOS 1

#define STATUS_DMX_RECIEVE 2
#define STATUS_DMX_BAD 3
#define STATUS_USB_RECIEVE 4
#define STATUS_USB_BAD 5
#define STATUS_DEMO_MODE 6

#define SERVO_STATUS_STARTUP 7
#define SERVO_START_SUCSESS 8
#define SERVO_STATUS_NOTLICENSED 9
#define SERVO_STATUS_MOVEMENT 10
#define SERVO_STATUS_STILL 11
#define SERVO_STATUS_PWM_DISABLED 12

#define STATUS_OFF 13




//**********************************************************************************
// Status Light Configuration

class Hardware {
public:
  int pin;
  int StateMachine;

public:
  Hardware(int p, int M) {
    pin = p;
    StateMachine = M;
  }

  int getServoPin() {
    return pin;
  }
  int getStateMachine() {
    return StateMachine;
  }
};

std::array<Hardware, NUM_SERVO_PINS> hardware = { Hardware(SERVO_PIN_1, 1), Hardware(SERVO_PIN_2, 1), Hardware(SERVO_PIN_3, 2), Hardware(SERVO_PIN_4, 2), Hardware(SERVO_PIN_5, 3), Hardware(SERVO_PIN_6, 3), Hardware(SERVO_PIN_7, 4), Hardware(SERVO_PIN_8, 4), Hardware(SERVO_PIN_9, 5), Hardware(SERVO_PIN_10, 5), Hardware(SERVO_PIN_11, 6), Hardware(SERVO_PIN_12, 6) };

//**********************************************************************************
// Eyes NeoPixel Configuration

class NeoPixelDMX {
public:
  int mode;
  int brightness;
  int red;
  int green;
  int blue;
  int lastUpdate;
  int updateRate;

public:

  void setmode(int i) {
    neopixel_DMX_lock();
    mode = i;
    neopixel_DMX_unlock();
  }

  void setbrightness(int i) {
    neopixel_DMX_lock();
    brightness = i;
    neopixel_DMX_unlock();
  }

  void setred(int i) {
    neopixel_DMX_lock();
    red = i;
    neopixel_DMX_unlock();
  }

  void setgreen(int i) {
    neopixel_DMX_lock();
    green = i;
    neopixel_DMX_unlock();
  }

  void setblue(int i) {
    neopixel_DMX_lock();
    blue = i;
    neopixel_DMX_unlock();
  }

  void setlastUpdate(int i) {
    neopixel_DMX_lock();
    lastUpdate = i;
    neopixel_DMX_unlock();
  }

  void setupdateRate(int i) {
    neopixel_DMX_lock();
    updateRate = i;
    neopixel_DMX_unlock();
  }

  int getmode() {
    return mode;
  }

  int getbrightness() {
    return brightness;
  }

  int getred() {
    return red;
  }

  int getgreen() {
    return green;
  }

  int getblue() {
    return mode;
  }

  int getlastUpdate() {
    return lastUpdate;
  }

  int getupdateRate() {
    return updateRate;
  }
};

NeoPixelDMX eyeDmx = { 0, 0, 0, 0, 0, 0, 40 };

class NeoPixelEyes {
public:
  int baseColor;
  int flickerColor;
  int blackColor;
  int flickerDelay;
  int flickerRange;
  int brightness;        //
  int baseColorTime;     // portion total of all time paramaters that of time that base color is picked,
  int flickerColorTime;  // portion total of all time paramaters that of time that Flicker color is picked,
  int blackColorTime;    // portion total of all time paramaters that of time that Black color is picked,
  int dmxStart;
  int dmxEnd;
  int mode;

public:

  void setBaseColor(int i) {
    neopixel_eye_lock();
    baseColor = i;
    neopixel_eye_unlock();
  }

  void setflickerColor(int i) {
    neopixel_eye_lock();
    flickerColor = i;
    neopixel_eye_unlock();
  }

  void setblackColor(int i) {
    neopixel_eye_lock();
    blackColor = i;
    neopixel_eye_unlock();
  }

  void setflickerDelay(int i) {
    neopixel_eye_lock();
    flickerDelay = i;
    neopixel_eye_unlock();
  }

  void setflickerRange(int i) {
    neopixel_eye_lock();
    flickerRange = i;
    neopixel_eye_unlock();
  }

  void setbrightness(int i) {
    neopixel_eye_lock();
    brightness = i;
    neopixel_eye_unlock();
  }

  void setbaseColorTime(int i) {
    neopixel_eye_lock();
    baseColorTime = i;
    neopixel_eye_unlock();
  }

  void setflickerColorTime(int i) {
    neopixel_eye_lock();
    flickerColorTime = i;
    neopixel_eye_unlock();
  }

  void setblackColorTime(int i) {
    neopixel_eye_lock();
    blackColorTime = i;
    neopixel_eye_unlock();
  }

  void setdmxStart(int i) {
    neopixel_eye_lock();
    dmxStart = i;
    neopixel_eye_unlock();
  }

  void setdmxEnd(int i) {
    neopixel_eye_lock();
    dmxEnd = i;
    neopixel_eye_unlock();
  }

  int getbaseColor() {
    return baseColor;
  }

  int getflickerColor() {
    return flickerColor;
  }

  int getblackColor() {
    return blackColor;
  }

  int getflickerDelay() {
    return flickerDelay;
  }

  int getflickerRange() {
    return flickerRange;
  }

  int getbrightness() {
    return brightness;
  }

  int getbaseColorTime() {
    return baseColorTime;
  }

  int getflickerColorTime() {
    return flickerColorTime;
  }

  int getblackColorTime() {
    return blackColorTime;
  }

  int getdmxStart() {
    return dmxStart;
  }

  int getdmxEnd() {
    return dmxEnd;
  }
};

NeoPixelEyes eyeLight[14] = {
  { 0xFF0000, 0xFF0000, 0xFF0000, 40, 50, 255, 30, 30, 30, 11, 20 },  //Red Fire
  { 0x0000FF, 0x0000FF, 0x0000FF, 20, 50, 255, 30, 30, 10, 21, 30 },  //Blue Fire
  { 0x00FF00, 0x00FF00, 0x00FF00, 40, 50, 255, 30, 30, 10, 31, 40 },  //Green Fire
  { 0xFF00FF, 0xFF00FF, 0x000000, 40, 50, 255, 30, 30, 30, 41, 50 },  //Purple Fire
  { 0xFF8000, 0xFF8000, 0xFF8000, 40, 50, 255, 30, 30, 30, 51, 60 },  //Orange Fire
  { 0xFFFF00, 0xFFFF00, 0x000000, 40, 50, 255, 30, 30, 30, 61, 70 },  //yellow Fire
  { 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 5, 50, 255, 30, 30, 30, 71, 80 },   //White 
  { 0xFF1000, 0xFF3000, 0xC00000, 40, 50, 255, 30, 30, 30, 81, 90 },  //Red Fire
  { 0x000080, 0x0000C0, 0x0000FF, 20, 50, 255, 30, 30, 10, 91, 100 },  //Blue Fire
  { 0x008000, 0x00C000, 0x00FF00, 40, 50, 255, 30, 30, 10, 101, 110 },  //Green Fire
  { 0x800080, 0x4D004D, 0x000000, 40, 50, 255, 30, 30, 30, 111, 120},  //Purple Fire
  { 0xFFA500, 0x996300, 0x000000, 40, 50, 255, 30, 30, 30, 121, 130 },  //Orange Fire
  { 0xFFFF00, 0xFFA500, 0xFF8000, 40, 50, 255, 30, 30, 30, 131, 140 },  //Yellow Fire
  { 0xFFFFFF, 0x000000, 0x9999FF, 5, 50, 255, 30, 30, 30, 141, 150 },   //White Lighting

};

#define REDFIREEYES 0
#define BLUEFIREEYES 1
#define GREENFIREEYES 2



int eyeColorProfile = 3;
int lastEyeFlicker = 0;
int FlickerDelay = random(0, eyeLight[0].flickerDelay);

//**********************************************************************************
// Servo Profile Information
class ServoConfig {
public:
  bool licensed;           // Servo Status true=active
  int servoNum;            // Servo Number, Used for Queue comunications bettween Cores
  char servoUserName[40];  // User definable Servo Name
  char servoPN[40];        // Servo Part Number, for Future Use
  bool analog;             // true=analog, false=digital
  float freq;              // Servo Operating Freq. hz.
  float servoMinDeg;       // Servo Absolutate min Capability in Degrees
  float servoMaxDeg;       // Servo Absolute Max Capability in Degrees
  float minPWM;            // Servo absolute Min Pulse witdh
  float maxPWM;            // Servo absolute Max Pulse Witdh
  float minDeg;            // Min desried Degrees of Travel
  float maxDeg;            // Max desired Degrees of Travel
  float ServoStartDeg;     // Servo Starting Postion Degrees, Servo is intilized to this postion recomend that it is the rest postion of the servo if under load
  float maxVel;            // Servo Max velocity in degrees per second
  bool smooth;             // turn smoothing enginer on
  float maxAcc;            // Max Accerlation degrees per second squared
  float maxDec;            // Max deceleration degrees per second squared at end of Move
  int servoSleepTimer;     // Servo will be disconnect if no move after sleep timer has expired, (seconds 0= no sleep)


  ServoConfig(int i) {
    licensed = true;
    if (i >= NUM_LIC_SERVOS) {
      licensed = false;
    }
    servoNum = i;
    strcpy(servoUserName, "Generic Servo");
    strcpy(servoPN, "Generic Part Number");
    analog = true;
    freq = 50;
    servoMinDeg = 0;
    servoMaxDeg = 180;
    minPWM = 670;
    maxPWM = 2200;
    minDeg = 0;
    maxDeg = 360;
    ServoStartDeg = 90;
    maxVel = 10000;
    smooth = true;
    maxAcc = 1000;  //46100
    maxDec = 1000;
    servoSleepTimer = SERVO_QUIESE_TIMER;
  }

  public:

  bool isServoLicensed(){ 
    return licensed;
  }

};

std::array<ServoConfig, NUM_SERVO_PINS> C1_config_R = { ServoConfig(0), ServoConfig(1), ServoConfig(2), ServoConfig(3), ServoConfig(4), ServoConfig(5), ServoConfig(6), ServoConfig(7), ServoConfig(8), ServoConfig(9), ServoConfig(10), ServoConfig(11) };





//**********************************************************************************
// Servo Profile Information
class runData {
public:
  int servoNum;       // 0-15, 100 = mode or debug update
  bool active;        // Is servo active, servo was active within the sleep period unless sleep period is zero, then servo is forced active all the time
  float curentPos;    // current Postion of the Servo; degrees
  float targetPos;    // target POsition of the Servo; degrees
  float previousPos;  // Previous postion of Servo; degrees
  int lastMove;       // Last time the servo moved (CPU mills since start)
  int servoSleepTimer;
  bool PwmEnabled;
  

  runData(int i) {
    servoNum = i;
    active = true;
    curentPos = 0;
    targetPos = C1_config_R[i].ServoStartDeg;
    previousPos = 0;
    lastMove = 0;
    servoSleepTimer = C1_config_R[i].servoSleepTimer;
    PwmEnabled = false;
  }

public:

  bool isServoActiveRemove() {
    return true;
  }
  
  bool isServoActive() {

  if (curentPos == targetPos && lastMove + servoSleepTimer < millis() ) {
      return false;
    }
      if ( curentPos != targetPos ) lastMove = millis(); 
      return true;
    }

  void setservoNum(int i) {
    run_Data_lock();
    servoNum = i;
    run_Data_unlock();
  }

  void setactive(bool i) {
    run_Data_lock();
    active = i;
    run_Data_unlock();
  }

  void setcurentPos(int i) {
    run_Data_lock();
    curentPos = i;
    run_Data_unlock();
  }

  void settargetPos(int i) {
    run_Data_lock();
    targetPos = i;
    run_Data_unlock();
  }

  void setpreviousPos(int i) {
    run_Data_lock();
    previousPos = i;
    run_Data_unlock();
  }

  void setlastMove(int i) {
    run_Data_lock();
    lastMove = i;
    run_Data_unlock();
  }

  int getservoNum() {
    return servoNum;
  }

  bool getactive() {
    return active;
  }

  int getcurentPos() {
    return curentPos;
  }

  int gettargetPos() {
    return targetPos;
  }

  int getpreviousPos() {
    return previousPos;
  }

  int getlastMove() {
    return lastMove;
  }
};

std::array<runData, NUM_SERVO_PINS> C1_run_R = { runData(0), runData(1), runData(2), runData(3), runData(4), runData(5), runData(6), runData(7), runData(8), runData(9), runData(10), runData(11) };

//**********************************************************************************
//Function Linking Information

float getDutyCycle(int i);
bool readDMX();
void setServoPositions();
bool checkDMX();
void statusLed(int i);
void servoTracker(int i);
uint32_t getBit(uint32_t n, int k);                                                       // Read a bit from an Integer   - PopFifo.h
int readRecordID(uint32_t marker);                                                        // Read Record Indetifier and return value - PopFifo.h
int readSystemState(uint32_t marker);                                                     // Read System Satus Info and return value - PopFifo.h
int readVariableType(uint32_t marker);                                                    // Read variable type and return value - PopFifo.h
int readCharSize(uint32_t marker);                                                        // Read char type size and return value - PopFifo.h
int readArrayIdent(uint32_t marker);                                                      // Read char type size and return value - PopFifo.h
int readElement(uint32_t marker);                                                         // Read char element - PopFifo.h
int readRow(uint32_t marker);                                                             // Read row - PopFifo.h
int createSystemStateID(int state);                                                       // Create a System State Record and push to fifo queue - PopFifo.h
int createDataRecord(int type, char size, int arrayID, int element, int row);             // Create a data type ID record and push to fifo queue - PopFifo.h
uint32_t setBit(uint32_t n, int k);                                                       // set bit - PopFifo.h
uint32_t getBit(uint32_t n, int k);                                                       // set get bit - PopFifo.h
void sendChar(char charData[], int charDataSize, int arrayId, int elementId, int rowId);  // check for valod Record ID - PopFifo.h
void sendChar(char charData[], int charDataSize, int arrayId, int elementId, int rowId);  // Send Char - PopFifo.h
void recChar(char* buffer, int size);                                                     // Recieve Char - PopFifo.h
void sendInt(uint32_t value, int arrayID, int elementID, int rowID);                      // Send int to the other core - PopFifo.h
uint32_t recieveInt();                                                                    // Recieve Int - PopFifo.h
void sendFloat(float value, int arrayID, int elementID, int rowID);                       // Send Float to other core - PopFifo.h
float receiveFloat();                                                                     // Recieve Float - PopFifo.h
void sendBool(bool value, int arrayID, int elementID, int rowID);                         // Send bool to other core - PopFifo.h
bool recieveBool();                                                                       // Recieve bool - PopFifo.h
bool recieveRecord();                                                                     // Recieve Record - PopFifo.h
void servoTrackerLight();
