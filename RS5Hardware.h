//**********************************************************************************
// NeoPixel Hardware Definitions
#define PIN 25            // GPIO PIN for Status Pizel
#define NUMPIXELS 8       // Number of Pixels on Status String
#define STATUSPIXEL 0     // Postion of status pixel on string
#define EYESPIXEL 7       // PIXEL for EYES

//**********************************************************************************
// setup DMX Recieve
#define DMX_PIN 18          // DMX Recieve Pin 

//**********************************************************************************
// setup DMX Address Set
#define DMX_BIT_1_PIN 13   // Define DMX Address Input for Bit 1
#define DMX_BIT_2_PIN 12    // Define DMX Address Input for Bit 2
#define DMX_BIT_4_PIN 11    // Define DMX Address Input for Bit 3
#define DMX_BIT_8_PIN 10    // Define DMX Address Input for Bit 4
#define DMX_BIT_16_PIN 9   // Define DMX Address Input for Bit 5
#define DMX_BIT_32_PIN 8   // Define DMX Address Input for Bit 6
#define DMX_BIT_64_PIN 7   // Define DMX Address Input for Bit 7
#define DMX_BIT_128_PIN 6  // Define DMX Address Input for Bit 8
#define DMX_BIT_256_PIN 5  // Define DMX Address Input for Bit 9

//**********************************************************************************
// Run Mode Bits PIN
#define RUNMODE_BIT_1_PIN 4
#define RUNMODE_BIT_2_PIN 3
#define RUNMODE_BIT_4_PIN 2

// Switch 12,11,10 set the island that the Skull is on for Eye color control. 
#define EYEDMX_BOTNAYBAY      494 // Dip Switch 000 494 for BotnayBay 68 for Vignet
#define EYEDMX_SWANISLAND     496 // Dip Switch 001
#define EYEDMX_PORTOFDESTINY  500 // Dip Switch 010
#define EYEDMX_RUMISLAND      498 // Dip Switch 011
#define EYEDMX_TORTUGA        68  // Dip Switch 100

//**********************************************************************************
// Enable Servo and Neopixel Output Pin
#define ENABLEOUTPUT_PIN      17

//**********************************************************************************
// Servo Hardware and movement limits setup

// Servo PWM Pins
#define NUM_SERVO_PINS      6
#define NUM_LIC_SERVOS      6
#define SERVO_QUIESE_TIMER  2000
#define DEBUG_SERVO         1

#define SERVO_PIN_1     24
#define SERVO_PIN_2     23
#define SERVO_PIN_3     22
#define SERVO_PIN_4     21
#define SERVO_PIN_5     20
#define SERVO_PIN_6     19
#define SERVO_PIN_7     14
#define SERVO_PIN_8     15
#define SERVO_PIN_9     16
#define SERVO_PIN_10    26
#define SERVO_PIN_11    27
#define SERVO_PIN_12    28

// JAW SERVO Defaults
#define JAW_SERVO_POS     0
#define JAW_SERVO_MAXACC  10000
#define JAW_SERVO_MAXDEC  10000
#define JAW_SERVO_MAXVEL  290
#define JAW_SERVO_MAXDEG  80
#define JAW_SERVO_MINDEG  0
#define JAW_START_POS    1
#define JAW_START_SLEEP   1000

// Yaw Servo Defaults
#define YAW_SERVO_POS 1
#define YAW_SERVO_MAXACC  2000
#define YAW_SERVO_MAXDEC  1000
#define YAW_SERVO_MAXVEL  290
#define YAW_SERVO_MAXDEG  180
#define YAW_SERVO_MINDEG  0
#define YAW_START_POS     90
#define YAW_START_SLEEP   1000

// Pitch Servo Defaults
#define PITCH_SERVO_POS 2
#define PITCH_SERVO_MAXACC  10000
#define PITCH_SERVO_MAXDEC  10000
#define PITCH_SERVO_MAXVEL  290
#define PITCH_SERVO_MAXDEG  180
#define PITCH_SERVO_MINDEG  0
#define PITCH_START_POS     90
#define PITCH_START_SLEEP   1000

// Roll Servo Defaults
#define ROLL_SERVO_POS 3
#define ROLL_SERVO_MAXACC  10000
#define ROLL_SERVO_MAXDEC  10000
#define ROLL_SERVO_MAXVEL  290
#define ROLL_SERVO_MAXDEG  180
#define ROLL_SERVO_MINDEG  0
#define ROLL_START_POS     90
#define ROLL_START_SLEEP   1000

// Eye Servo Defaults
#define EYE_SERVO_POS 4
#define EYE_SERVO_MAXACC  10000
#define EYE_SERVO_MAXDEC  10000
#define EYE_SERVO_MAXVEL  290
#define EYE_SERVO_MAXDEG  180
#define EYE_SERVO_MINDEG  0
#define EYE_START_POS     90
#define EYE_START_SLEEP   1000

