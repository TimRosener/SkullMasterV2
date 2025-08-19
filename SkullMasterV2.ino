// ============================================================================
// File: SkullMasterV2.ino
// Project: SkullMasterV2 - DMX512 Animatronic Controller
// Version: 3.1.0-alpha
// Date: 2024-12-27
// Author: Rose&Swan Productions / Tim Rosener
// Description: Main Arduino sketch for dual-core RP2040 animatronic control
//              system with DMX512 interface, 5-axis servo control, and LED
//              effects for theatrical skull props
// License: CC BY-NC 4.0 (Non-Commercial)
// ============================================================================

// ********************************************************************************
// Define include libraries
// ********************************************************************************
#include <arduino.h>            // arduino std library
#include <string.h>             //
#include <Adafruit_NeoPixel.h>  // Neopixel Libary
#include "ServoEngine.h"        // Servo Movement Engine
#include "ServoDriver.h"        // Drive State Machines
#include "DmxInput.h"           // DMX Support -
#include "array"                //
#include "RP2040_PWM.h"         // PIO based PWM on GPIO
#include <hardware/flash.h>     // Flash Drive Tools
#include "RS5DualCore.h"        // multicore data sharing setup
#include "RS5hardware.h"        // Hardware Setup
#include "RS5DMX.h"             // Pirate


// GLOBAL

// ********************************************************************************
// Set SOFTWARE_VERSION Number
// ********************************************************************************
#define SOFTWARE_VERSION "Alpha 3.1"

//**********************************************************************************
// Setup Status Lights and System Status Neopixel
int statusLightTimer = 0;  // Last status change CPU cycles
int status = 0;            // satus holds the current status of Core1
int statusFrameFreq = 10;
int statusFrameLast = 0;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
//**********************************************************************************

//**********************************************************************************
// define state machine instances for servos
RP2040_PWM* servoInstance[NUM_SERVO_PINS];
const float servoStartDC = 7.5f;  // Starting Dutycyle as a precentage
int servoStartDelay = 250;        // Delay in milliseconds bettween starting each servo
//**********************************************************************************

//**********************************************************************************
// setup DMX Recieve
DmxInput dmxInput;                                         //Set up DMX Class
volatile uint8_t bufferDmx[DMXINPUT_BUFFER_SIZE(1, 512)];  // Define DMX duffer (start byte + 512 channels)
//**********************************************************************************

//**********************************************************************************
// Setup Servo Movement Engine
Derivs_Limiter DL[NUM_SERVO_PINS];
//**********************************************************************************

// ********************************************************************************
// initilize and start  Core zero with Servo Maintenance and DMX Recieve system
// ********************************************************************************

// END GLOBAL

//**********************************************************************************
// Core 0 Setup
void setup() {

  // Setup Semaphores for Data Handling bettween cores
  systemStateLock = xSemaphoreCreateMutex();
  neoPixelEyesLock = xSemaphoreCreateMutex();
  neoPixelDMXLock = xSemaphoreCreateMutex();
  runDataLock = xSemaphoreCreateMutex();

  systemState.setBootLevel(0);  // Set Bot Mode to Core Zero Setup

  if (systemState.getDebugLevel() > DebugLevelNone) {

    // Setup Serial Port
    Serial.begin(115200);
    while (!Serial) {
      //delay(1);
    }


    Serial.printf("Rose&Swan\nPirate Servo DMX Version %s\n", SOFTWARE_VERSION);
    Serial.printf("Copyright Rose&Swan 2022(c) All Rights Reserved\n");
    Serial.printf("Core Zero: Starting, Debug Level:%d\n", systemState.getDebugLevel());
  }

  // Set Dip Swtich and Run Mode Dip Switches to inout, and pulled up to 3.3V
  pinMode(DMX_BIT_1_PIN, INPUT_PULLUP);
  pinMode(DMX_BIT_2_PIN, INPUT_PULLUP);
  pinMode(DMX_BIT_4_PIN, INPUT_PULLUP);
  pinMode(DMX_BIT_8_PIN, INPUT_PULLUP);
  pinMode(DMX_BIT_16_PIN, INPUT_PULLUP);
  pinMode(DMX_BIT_32_PIN, INPUT_PULLUP);
  pinMode(DMX_BIT_64_PIN, INPUT_PULLUP);
  pinMode(DMX_BIT_128_PIN, INPUT_PULLUP);
  pinMode(DMX_BIT_256_PIN, INPUT_PULLUP);
  pinMode(RUNMODE_BIT_1_PIN, INPUT_PULLUP);
  pinMode(RUNMODE_BIT_2_PIN, INPUT_PULLUP);
  pinMode(RUNMODE_BIT_4_PIN, INPUT_PULLUP);

  pinMode(ENABLEOUTPUT_PIN, OUTPUT);     // Set the pin as an output
  digitalWrite(ENABLEOUTPUT_PIN, HIGH);  // Set the pin to HIGH


  ReadDmxDipSwitches();
  dmxInput.begin(DMX_PIN, 1, 512);  // Start DMX Reciever
  dmxInput.read_async(bufferDmx);   // Start Asynchronus Read

  // Start DMX Reciver1 & Dip Switch Pins

  if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core Zero: DMX Start Addr:%d\n", systemState.getDMXAddress());
  if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core Zero: Initializing Asynchronus DMX Reciever on Pin %d, DMX Range %d-%d, Buffer Size: %d\n", DMX_PIN, 1, 1, 512, DMXINPUT_BUFFER_SIZE(1, 512));

  // Get Run Mode INformation from Dip Switches
  //getRunMode(); Removed for Pirate Show
  systemState.setBootLevel(1);  // Set Boot Mode to Core One Setup
}
// End Core 0 setup
//**********************************************************************************



//**********************************************************************************
// Core Zero Start
void loop() {

  digitalWrite(ENABLEOUTPUT_PIN, HIGH);  // Set the pin to HIGH

  // Wait for setups to finish
  while (systemState.getBootLevel() != 3) {
    delay(1);
  }

  if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core Zero: Entering Run Mode\n");

  systemState.setBootLevel(4);  // Let Core One Start Loop()


  while (true) {

    //******************************************************************************
    // Check Dip Switches and see if Run Mode has changed
    //getRunMode(); Removed for Custom Pirate version
    //******************************************************************************

    //******************************************************************************
    // Check Dip Switches and see if Run Mode has changed
    ReadDmxDipSwitches();

    //******************************************************************************

    //******************************************************************************
    // Read DMX and set Target Servo Postions
    if (systemState.getMode() == RunModeDMX) {
      if (checkDMX()) {  // Check for DMX signal and set status
        readDMX();
        updateStatusLight(STATUS_DMX_RECIEVE);  // Update Status LED Settings
      } else {
        updateStatusLight(STATUS_DMX_BAD);  // Update Status LED Settings
      }
    }
    //******************************************************************************

    delay(1);
  }
}
// End Core Zero Loop
//**********************************************************************************


// ********************************************************************************
// initilize and start  Core 1 with Servo Maintenance and DMX Recieve system
// ********************************************************************************

//**********************************************************************************
// Start Core 1 Setup
void setup1() {

  // Wait for my turn in Boot Sequence
  while (systemState.getBootLevel() != 1) {
    delay(1);
  }

  //***************************************************
  // Default Servo Settings;

  // setup jaw servo defaults
  C1_config_R[JAW_SERVO_POS].maxAcc = JAW_SERVO_MAXACC;
  C1_config_R[JAW_SERVO_POS].maxDec = JAW_SERVO_MAXDEC;
  C1_config_R[JAW_SERVO_POS].maxVel = JAW_SERVO_MAXVEL;
  C1_config_R[JAW_SERVO_POS].maxDeg = JAW_SERVO_MAXDEG;
  C1_config_R[JAW_SERVO_POS].minDeg = JAW_SERVO_MINDEG;
  C1_config_R[JAW_SERVO_POS].servoSleepTimer = JAW_START_SLEEP;
  C1_run_R[JAW_SERVO_POS].setcurentPos(JAW_START_POS);
  C1_run_R[JAW_SERVO_POS].settargetPos(JAW_START_POS);

  //Setup Yaw Servo Defaults
  C1_config_R[YAW_SERVO_POS].maxAcc = YAW_SERVO_MAXACC;
  C1_config_R[YAW_SERVO_POS].maxDec = YAW_SERVO_MAXDEC;
  C1_config_R[YAW_SERVO_POS].maxVel = YAW_SERVO_MAXVEL;
  C1_config_R[YAW_SERVO_POS].maxDeg = YAW_SERVO_MAXDEG;
  C1_config_R[YAW_SERVO_POS].minDeg = YAW_SERVO_MINDEG;
  C1_config_R[YAW_SERVO_POS].servoSleepTimer = YAW_START_SLEEP;
  C1_run_R[YAW_SERVO_POS].setcurentPos(YAW_START_POS);
  C1_run_R[YAW_SERVO_POS].settargetPos(YAW_START_POS);

  //Setup pitch Servo Defaults
  C1_config_R[PITCH_SERVO_POS].maxAcc = PITCH_SERVO_MAXACC;
  C1_config_R[PITCH_SERVO_POS].maxDec = PITCH_SERVO_MAXDEC;
  C1_config_R[PITCH_SERVO_POS].maxVel = PITCH_SERVO_MAXVEL;
  C1_config_R[PITCH_SERVO_POS].maxDeg = PITCH_SERVO_MAXDEG;
  C1_config_R[PITCH_SERVO_POS].minDeg = PITCH_SERVO_MINDEG;
  C1_config_R[PITCH_SERVO_POS].servoSleepTimer = PITCH_START_SLEEP;
  C1_run_R[PITCH_SERVO_POS].setcurentPos(PITCH_START_POS);
  C1_run_R[PITCH_SERVO_POS].settargetPos(PITCH_START_POS);

  //Setup roll Servo Defaults
  C1_config_R[ROLL_SERVO_POS].maxAcc = ROLL_SERVO_MAXACC;
  C1_config_R[ROLL_SERVO_POS].maxDec = ROLL_SERVO_MAXDEC;
  C1_config_R[ROLL_SERVO_POS].maxVel = ROLL_SERVO_MAXVEL;
  C1_config_R[ROLL_SERVO_POS].maxDeg = ROLL_SERVO_MAXDEG;
  C1_config_R[ROLL_SERVO_POS].minDeg = ROLL_SERVO_MINDEG;
  C1_config_R[ROLL_SERVO_POS].servoSleepTimer = ROLL_START_SLEEP;
  C1_run_R[ROLL_SERVO_POS].setcurentPos(ROLL_START_POS);
  C1_run_R[ROLL_SERVO_POS].settargetPos(ROLL_START_POS);

  //Setup eyes Servo Defaults
  C1_config_R[EYE_SERVO_POS].maxAcc = EYE_SERVO_MAXACC;
  C1_config_R[EYE_SERVO_POS].maxDec = EYE_SERVO_MAXDEC;
  C1_config_R[EYE_SERVO_POS].maxVel = EYE_SERVO_MAXVEL;
  C1_config_R[EYE_SERVO_POS].maxDeg = EYE_SERVO_MAXDEG;
  C1_config_R[EYE_SERVO_POS].minDeg = EYE_SERVO_MINDEG;
  C1_config_R[EYE_SERVO_POS].servoSleepTimer = EYE_START_SLEEP;
  C1_run_R[EYE_SERVO_POS].setcurentPos(EYE_START_POS);
  C1_run_R[EYE_SERVO_POS].settargetPos(EYE_START_POS);


  if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core One: Starting Setup\n");


  //*********************************************************
  // setup status LED and turn on during setup
  if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core One: initializing NeoPixel and Built in LED.\n");

  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)

  // Tell the ouside workd that boot in progress
  statusLed(STATUS_BOOT);
  sendPixelFrame();
  delay(1000);

  // setup servo Movoment Profiles
  if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core One: Initilizing Motility Engine for each servo:\n");
  for (int i = 0; i < NUM_LIC_SERVOS; i++) {
    if (systemState.getDebugLevel() > DebugLevelModel) Serial.printf("Core One: Starting Motility Model for Servo %d, %s\n", C1_config_R[i].servoNum, C1_config_R[i].servoUserName);
    if (!C1_config_R[i].smooth) {
      DL[i] = Derivs_Limiter(C1_config_R[i].maxVel, INFINITY, INFINITY, C1_config_R[i].ServoStartDeg, C1_config_R[i].ServoStartDeg, 0, false, false, -INFINITY, INFINITY);
    } else {
      DL[i] = Derivs_Limiter(C1_config_R[i].maxVel, C1_config_R[i].maxAcc, C1_config_R[i].maxDec, C1_config_R[i].ServoStartDeg, C1_config_R[i].ServoStartDeg, 0, false, false, -INFINITY, INFINITY);
    }
  }

  // Setup Servo State Machines
  statusLed(STARTING_SERVOS);
  sendPixelFrame();
  if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core One: Initilizing State Machines and Starting Servo's in an orderly Manner:\n");
  for (int i = 0; i < NUM_LIC_SERVOS; i++) {
    if (C1_config_R[i].licensed) servoStatusLed(i, SERVO_STATUS_STARTUP);
    if (!C1_config_R[i].licensed) servoStatusLed(i, SERVO_STATUS_NOTLICENSED);
    delay(servoStartDelay / 2);
    if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core One: Starting Servo %d, %s, on Pin %d, state machine %d, start position %d degrees\n", C1_config_R[i].servoNum, C1_config_R[i].servoUserName, hardware[i].getServoPin(), hardware[i].getStateMachine(), C1_config_R[i].ServoStartDeg);

    if (C1_config_R[i].licensed) servoInstance[i] = new RP2040_PWM(hardware[i].getServoPin(), C1_config_R[i].freq, getDutyCycle(i));  // initilize state machine for each servo

    if (C1_config_R[i].licensed) servoInstance[i]->setPWM();
    if (C1_config_R[i].licensed) servoInstance[i]->disablePWM();
    statusLed(STARTING_SERVOS);
    sendPixelFrame();

    if (C1_config_R[i].licensed) servoStatusLed(i, SERVO_START_SUCSESS);
    delay(servoStartDelay / 2);
  }

  if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core One: Finsihed Setting Up:\n");

  systemState.setBootLevel(3);  // Turn over boot control to Core zero loop()
}
// END Core 1 Setup
//**********************************************************************************


//**********************************************************************************
// Core One Start

void loop1() {

  // Enable Servo and NEOPixel Data Ouptut.
  digitalWrite(ENABLEOUTPUT_PIN, HIGH);  // Set the pin to HIGH

  // Wait for setups to finish and start core one
  while (systemState.getBootLevel() != 4) {
    delay(1);
  }
  if (systemState.getDebugLevel() > DebugLevelNone) Serial.printf("Core One: Entering Run Mode\n");  // Debug Ourput - Core 1 begin
  systemState.setBootLevel(5);                                                                       // Let Core One Start Loop()

  // Main Run Loop.
  while (true) {  // Main Loop

    //********************************************************************
    // DMX Run Mode

    if (systemState.getMode() == RunModeDMX) {

      // ******************************************************************************************
      // POP SHOW EYE Managemnet Code

      if (bufferDmx[systemState.startDMXEyes + 1] < 10) {
        // Determine Eye color mode

          setEyeColor();  // set eye color to mode to RGB Mode
      } else {            // Set Preset FLicker Modes

        for (int i = 0; i < (sizeof(eyeLight) / sizeof(eyeLight[0])); i++) {

          if (bufferDmx[systemState.startDMXEyes + 1] >= eyeLight[i].getdmxStart() && bufferDmx[systemState.startDMXEyes + 1] <= eyeLight[i].getdmxEnd()) {

            eyeColorProfile = i;
            break;
          }
        }
        flickerEyes(eyeColorProfile);
      }
      //********************************************************************************************
      
      setServoPositions();  // Calculate next Servo Positions and write to GPIO Pins
      sendPixelFrame();
    }
    //********************************************************************

    //********************************************************************
    // DEMO MODE
    if (systemState.getMode() == RunModeDemo) {
      sweepPos();
      flickerEyes(REDFIREEYES);
      updateStatusLight(STATUS_DEMO_MODE);
      setServoPositions();  // Calculate next Servo Positions and write to GPIO Pins
      sendPixelFrame();
    }
    //********************************************************************


    //********************************************************************
    //  Montior Servo's and set Servo LED

    servoMonitor();

    //********************************************************************
    // Run DEBUG output if needed

    if (systemState.getDebugLevel() == DebugLevelServoFull) servoTracker();
    if (systemState.getDebugLevel() == DebugLevelServo) servoTrackerLite();
    if (systemState.getDebugLevel() == DebugLevelVoltCurrent) CheckCurrent();
    //********************************************************************
  }
}
// End Core One Start
//*********************************************************************************

// ********************************************************************************
// Functions
// ********************************************************************************


// ********************************************************************************
// map functions for floats
float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// ********************************************************************************


// ********************************************************************************
// caluclate Duty Cycle based on servo profile and realtime run data for servo i
float getDutyCycle(int i) {

  // Set Lower PWM Bound
  int PWMLower = floatMap(C1_config_R[i].minDeg, C1_config_R[i].servoMinDeg, C1_config_R[i].servoMaxDeg, C1_config_R[i].minPWM, C1_config_R[i].maxPWM);

  // Set Upper PWM Bound
  int PWMUpper = floatMap(C1_config_R[i].maxDeg, C1_config_R[i].servoMinDeg, C1_config_R[i].servoMaxDeg, C1_config_R[i].minPWM, C1_config_R[i].maxPWM);

  // Calculate DutyCycle
  float dutyCycle = 100 * (floatMap(C1_run_R[i].getcurentPos(), C1_config_R[i].minDeg, C1_config_R[i].maxDeg, PWMLower / ((1 / C1_config_R[i].freq) * 1000000), PWMUpper / ((1 / C1_config_R[i].freq) * 1000000)));

  return dutyCycle;
}
// ********************************************************************************


//**********************************************************************************
// Read DMX Values and put in Servo Profiles
bool readDMX() {

  if (bufferDmx[0] != 0) {
    if (systemState.getDebugLevel() == DebugLevelDMX) Serial.printf("Bad DMX Frame:%d (Position One not equal to zero)\n", bufferDmx[0]);
    return false;
  }

  // Read DMX data and update target postion in run data.
  for (int i = 0; i < NUM_LIC_SERVOS; i++) {
    if (C1_config_R[i].licensed) {
      if (bufferDmx[0] == 0) C1_run_R[i].settargetPos(floatMap(bufferDmx[i + systemState.getDMXAddress()], 0, 255, C1_config_R[i].minDeg, C1_config_R[i].maxDeg));
    }
  }

  // read DMX and set Preset/Color/Brightness levels for LED Eyes
  if (bufferDmx[0] == 0) eyeDmx.setmode(bufferDmx[systemState.startDMXEyes]);

  // ************************************************************************************************
  // This section is for Normal Eye Mode and is Commented Out for 2023 POP Show
  // ************************************************************************************************
  //if (bufferDmx[0] == 0) eyeDmx.setbrightness(bufferDmx[systemState.getDMXAddress() + NUM_LIC_SERVOS + 1]);
  //if (bufferDmx[0] == 0) eyeDmx.setred(bufferDmx[systemState.getDMXAddress() + NUM_LIC_SERVOS + 2]);
  //if (bufferDmx[0] == 0) eyeDmx.setgreen(bufferDmx[systemState.getDMXAddress() + NUM_LIC_SERVOS + 3]);
  //if (bufferDmx[0] == 0) eyeDmx.setblue(bufferDmx[systemState.getDMXAddress() + NUM_LIC_SERVOS + 4]);

  if (systemState.getDebugLevel() == DebugLevelDMX) {
    Serial.printf("DMX Address:%d  ", systemState.getDMXAddress());
    Serial.printf("Frame Status:%d  ", bufferDmx[0]);
    for (int i = 0; i <= 512; i++) {
      if (i == systemState.getDMXAddress() || i == systemState.getDMXAddress() + 1 || i == systemState.startDMXEyes || i == systemState.startDMXEyes + 1) {
        Serial.printf("%u:%u  ", i, bufferDmx[i]);
      }
    }
  }
  if (systemState.getDebugLevel() == DebugLevelDMX) Serial.printf("\n");
  return true;
}
// ********************************************************************************


//**********************************************************************************
// Calculate Servo Postions
void setServoPositions() {

  for (int i = 0; i < NUM_LIC_SERVOS; i++) {

    // Check that Servo is Licensed
    if (!C1_config_R[i].licensed) continue;
    if (systemState.getDebugLevel() == DebugLevelModel) Serial.printf("===Servo %d, good License, ", i);

    // Set new Target Postion for Servo
    DL[i].setTarget(C1_run_R[i].gettargetPos());

    if (systemState.getDebugLevel() == DebugLevelModel) Serial.printf("Target:%d, ", int(C1_run_R[i].gettargetPos()));

    // Calculaite New Servo Postions.
    DL[i].calc();
    C1_run_R[i].setcurentPos(DL[i].getPosition());

    if (systemState.getDebugLevel() == DebugLevelModel) Serial.printf("Position:%d, ", int(C1_run_R[i].getcurentPos()));

    //check to see if servo is quite

    if (C1_run_R[i].isServoActive()) {
      if (!C1_run_R[i].PwmEnabled) {
        C1_run_R[i].PwmEnabled = true;
      }
    } else {
      if (C1_run_R[i].PwmEnabled) {
        C1_run_R[i].PwmEnabled = false;
      }
    }
    if (systemState.getDebugLevel() == DebugLevelModel) Serial.printf("Status %d ", int(C1_run_R[i].isServoActive()));


    // Update previos postion and calculate new servo postion
    C1_run_R[i].setpreviousPos(C1_run_R[i].getcurentPos());
    if (systemState.getDebugLevel() == DebugLevelModel) Serial.printf(". Duty Cycle %d", int(getDutyCycle(i)));
    if (C1_run_R[i].PwmEnabled) {
      servoInstance[i]->setPWM(hardware[i].pin, C1_config_R[i].freq, getDutyCycle(i));
    } else {
      servoInstance[i]->setPWM(hardware[i].pin, C1_config_R[i].freq, 0);
    }
  }
  if (systemState.getDebugLevel() == DebugLevelModel) Serial.printf("\n");
  return;
}
// ********************************************************************************


//**********************************************************************************
// check for DMX Data


bool checkDMX() {
  if (millis() > dmxInput.latest_packet_timestamp() + systemState.getDMXPacketAgeLimit()) {
    status = STATUS_DMX_BAD;
    if (systemState.getDebugLevel() == DebugLevelDMX) Serial.printf("No DMX:%d\n", status);
    return false;
  }
  status = STATUS_DMX_RECIEVE;  // Set Status to DMX Normal
  return true;
}
// ********************************************************************************


//**********************************************************************************
// Status Light
void updateStatusLight(int i) {
  if (statusLight[i].timeToUpdateStatus()) {
    if (pixels.getPixelColor(STATUSPIXEL) != 0) {
      Serial.printf("Status !0 Pixel:%d\n",pixels.getPixelColor(STATUSPIXEL));
      statusLed(STATUS_OFF);
    } else {
      Serial.printf("Status = 0 Pixel:%d\n",pixels.getPixelColor(STATUSPIXEL));
      statusLed(i);
    }
  }
  return;
}
// ********************************************************************************


//**********************************************************************************
// update status light
void statusLed(int i) {
  //pixels.clear(); // Set all pixel colors to 'off'
  pixels.setPixelColor(STATUSPIXEL, pixels.Color(statusLight[i].green, statusLight[i].red, statusLight[i].blue));
}
// ********************************************************************************


//**********************************************************************************
// update status light
void servoStatusLed(int x, int i) {
  int s = x + 1;  // offset SPI NeoPixel address by one to account for status light. Servo 0 starts at 1
  pixels.setPixelColor(s, pixels.Color(statusLight[i].green, statusLight[i].red, statusLight[i].blue));
}
// ********************************************************************************


//**********************************************************************************
// Servo Status Monitor
void servoMonitor() {
  for (int i = 0; i < NUM_LIC_SERVOS; i++) {
    if (!C1_config_R[i].isServoLicensed()) {
      servoStatusLed(i, SERVO_STATUS_NOTLICENSED);
      continue;
    }
    if (!C1_run_R[i].isServoActive()) {
      servoStatusLed(i, SERVO_STATUS_PWM_DISABLED);
      continue;
    }
    if (C1_run_R[i].getcurentPos() == C1_run_R[i].gettargetPos()) {
      servoStatusLed(i, SERVO_STATUS_STILL);
      continue;
    }
    servoStatusLed(i, SERVO_STATUS_MOVEMENT);
  }
}
// ********************************************************************************


//**********************************************************************************
// Debug Routine
void servoTrackerLite() {

  if (systemState.timeToSample()) {
    int i = systemState.getDebugServo();
    Serial.printf("CurrentPostion:%d, TargetPosition:%d, MaxDegrees:%d, MinDegrees:%d\n", int(C1_run_R[i].getcurentPos()), int(C1_run_R[i].gettargetPos()), int(C1_config_R[i].maxDeg), int(C1_config_R[i].minDeg));
  }
}
// ********************************************************************************


//**********************************************************************************
// Debug Routine
void servoTracker() {

  if (systemState.timeToSample()) {
    int i = systemState.getDebugServo();
    Serial.printf("CurrentPostion:%d, TargetPosition:%d, MaxDegrees:%d, MaxVelocity:%d, MaxAccelration:%d, MaxDeceleration:%d, Smoothing Mode:%d, MaxPWM:%d, MinPWM:%d, Frequnecy:%d\n",
                  i, C1_run_R[i].getservoNum(), int(C1_run_R[i].getcurentPos()), int(C1_run_R[i].gettargetPos()), int(C1_config_R[i].maxDeg), int(C1_config_R[i].maxVel), int(C1_config_R[i].maxAcc), int(C1_config_R[i].maxDec), C1_config_R[i].smooth, int(C1_config_R[i].maxPWM), int(C1_config_R[i].minPWM), int(C1_config_R[i].freq));
  }
}
// ********************************************************************************


//**********************************************************************************
// Demo Mode move servos to random postions and at random times.
void randPos() {
  if (systemState.timeToSample()) {
    int i = random(0, 4);
    C1_run_R[i].settargetPos(random(C1_config_R[i].minDeg, C1_config_R[i].maxDeg));
    if (random(0, 100) > 90) {
      pixels.setPixelColor(EYESPIXEL, random(150, 255), 24, 0);
    }
  }
}
// ********************************************************************************

//**********************************************************************************
// Read dip Switches and check Run Mode.
void getRunMode() {

  // Get Run Mode
  if (systemState.timeToSampleRM()) {
    int readSwitches = 0;
    int mode = 0;
    if (digitalRead(RUNMODE_BIT_1_PIN) == 0) readSwitches = readSwitches + 1;
    if (digitalRead(RUNMODE_BIT_2_PIN) == 0) readSwitches = readSwitches + 2;
    if (digitalRead(RUNMODE_BIT_4_PIN) == 0) readSwitches = readSwitches + 4;

    // Pirate Show Code
    if (readSwitches <= 5 && readSwitches >= 0) mode = DebugLevelNone;
    if (readSwitches == 5) mode = 1;
    if (readSwitches == 6) mode = 2;
    if (readSwitches == 7) mode = 4;
    if (readSwitches == 8) mode = 4;

    systemState.setMode(mode);
  }


  //Serial.printf("Timer: %d Rate %d\n",systemState.getDipSwitchTimer(), systemState.getDemoTimerDelay());
}
// ********************************************************************************


//**********************************************************************************
// Read dip Switches and check Run Mode.
void ReadDmxDipSwitches() {

  //**********************************************************************************
  // Read dip Switches and and set run mode
  if (systemState.timeToSampleDMX()) {

    // Get DMX Address
    int i = 0;
    if (digitalRead(DMX_BIT_1_PIN) == 0) i = i + 1;
    if (digitalRead(DMX_BIT_2_PIN) == 0) i = i + 2;
    if (digitalRead(DMX_BIT_4_PIN) == 0) i = i + 4;
    if (digitalRead(DMX_BIT_8_PIN) == 0) i = i + 8;
    if (digitalRead(DMX_BIT_16_PIN) == 0) i = i + 16;
    if (digitalRead(DMX_BIT_32_PIN) == 0) i = i + 32;
    if (digitalRead(DMX_BIT_64_PIN) == 0) i = i + 64;
    if (digitalRead(DMX_BIT_128_PIN) == 0) i = i + 128;
    if (digitalRead(DMX_BIT_256_PIN) == 0) i = i + 256;
    systemState.setDMXAddress(i);
    // Get Run Mode, WHich Island am I on
    i = 0;
    if (digitalRead(RUNMODE_BIT_1_PIN) == 0) i = i + 1;
    if (digitalRead(RUNMODE_BIT_2_PIN) == 0) i = i + 2;
    if (digitalRead(RUNMODE_BIT_4_PIN) == 0) i = i + 4;
    if (i == 0) systemState.setDMXAddressEyes(EYEDMX_BOTNAYBAY);
    if (i == 1) systemState.setDMXAddressEyes(EYEDMX_SWANISLAND);
    if (i == 2) systemState.setDMXAddressEyes(EYEDMX_PORTOFDESTINY);
    if (i == 3) systemState.setDMXAddressEyes(EYEDMX_RUMISLAND);
    if (i == 4) systemState.setDMXAddressEyes(EYEDMX_TORTUGA);
  }
}
// ********************************************************************************

//**********************************************************************************
// Check Current Level and Report
void CheckCurrent() {
  if (systemState.timeToSample()) {
    //systemState.setServoVoltage(map(analogRead(28),0,1023,SERVO_VOLT_LOW,SERVO_VOLT_HIGH));
    systemState.setServoCurrent(map(analogRead(29), 0, 1023, SERVO_CURRENT_LOW, SERVO_CURRENT_HIGH));
    Serial.printf("Low:0,Current:%d,high:50\n", systemState.getServoCurrent());
  }
}

//**********************************************************************************
// Demo Mode move servos to back and forth
void sweepPos() {
  if (millis() > demoTimer) {
    demoTimer = demoTimerDelay + millis();
    for (int i = 0; i < NUM_LIC_SERVOS; i++) {
      if (C1_run_R[i].gettargetPos() == C1_run_R[i].getcurentPos()) {
        // Move to Max Degrees if an Minimum Degrees
        if (C1_run_R[i].getcurentPos() == C1_config_R[i].minDeg) C1_run_R[i].settargetPos(C1_config_R[i].maxDeg);
        // Move from Min Degrees if a Max Degrees
        if (C1_run_R[i].getcurentPos() == C1_config_R[i].maxDeg) C1_run_R[i].settargetPos(C1_config_R[i].minDeg);
        // Make sure it does not get stuck at startup ie: Target and Current Postion are equal but not min or max
        if (C1_run_R[i].getcurentPos() != C1_config_R[i].minDeg && C1_run_R[i].curentPos != C1_config_R[i].maxDeg) C1_run_R[i].settargetPos(C1_config_R[i].minDeg);
      }
    }
  }
}
// ********************************************************************************

//**************************************************************************
// Flicker Eyes
void flickerEyes(int i) {
  if (millis() > lastEyeFlicker) {                                         // check if fliker delay has passed
    lastEyeFlicker = random(0, eyeLight[i].getflickerDelay()) + millis();  // Set new Flicker time

    int colorSelect = random(0, eyeLight[i].getflickerColorTime() + eyeLight[i].getflickerColorTime() + eyeLight[i].getblackColorTime());

    if (colorSelect <= eyeLight[i].getbaseColorTime()) {
      pixels.setPixelColor(EYESPIXEL, ((eyeLight[i].getbaseColor() >> 16) & 0xFF), ((eyeLight[i].getbaseColor() >> 8) & 0xFF), ((eyeLight[i].getbaseColor() & 0xFF)));
    }
    if (colorSelect <= eyeLight[i].getflickerColorTime() + eyeLight[i].getflickerColorTime() && colorSelect > eyeLight[i].getbaseColorTime() + 1) {
      pixels.setPixelColor(EYESPIXEL, ((eyeLight[i].getflickerColor() >> 16) & 0xFF), ((eyeLight[i].getflickerColor() >> 8) & 0xFF), ((eyeLight[i].getflickerColor() & 0xFF)));
    }
    if (colorSelect > eyeLight[i].getflickerColorTime() + eyeLight[i].getflickerColorTime() + eyeLight[i].getblackColorTime() && colorSelect > eyeLight[i].getflickerColorTime() + eyeLight[i].getflickerColorTime() + 1) {
      pixels.setPixelColor(EYESPIXEL, ((eyeLight[i].getblackColor() >> 16) & 0xFF), ((eyeLight[i].getblackColor() >> 8) & 0xFF), ((eyeLight[i].getblackColor() & 0xFF)));
    }

    pixels.setBrightness(bufferDmx[systemState.startDMXEyes]);
  }
  if (systemState.getDebugLevel() == DebugLevelPixel) Serial.printf(" flicker Mode Pixel:%d Brightness:%d M:%d Eye DMX Value:%d,%d Mode:%d\n", EYESPIXEL, 255, i, bufferDmx[EYEDMX_BOTNAYBAY], bufferDmx[EYEDMX_BOTNAYBAY + 1], eyeColorProfile);
}

//*************************************************
// Set Eye Color in 
void setEyeColor() {

  if (millis() > eyeDmx.lastUpdate + eyeDmx.updateRate) {

    eyeDmx.lastUpdate = millis();

    uint16_t r = (eyeDmx.getred() * eyeDmx.getbrightness()) / 255;
    uint16_t g = (eyeDmx.getgreen() * eyeDmx.getbrightness()) / 255;
    uint16_t b = (eyeDmx.getblue() * eyeDmx.getbrightness()) / 255;

    if (systemState.getDebugLevel() == DebugLevelPixel) Serial.printf(" RGB Mode Pixel:%d Brightness:%d R:%d G:%d B:%d \n", EYESPIXEL, 255, r, g, b);

    pixels.setPixelColor(EYESPIXEL, r, g, b);
  }
}


// ********************************************************************************
// Send a Frame of Pixel Data
void sendPixelFrame() {
  if (millis() > statusFrameLast + statusFrameFreq) {
    statusFrameLast = millis();
    pixels.show();
  }
}
