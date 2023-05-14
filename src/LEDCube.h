//
// Created by Roger Smith on 5/5/23.
//

#ifndef LEDCHECK_LEDCHECK_H
#define LEDCHECK_LEDCHECK_H


// **************************************************************************************************************
// Start Section Interrupt                                                                                      *
// This section is to setup defines & fuctions to access the timer interrupt library                            *
// **************************************************************************************************************
#if !(defined(__AVR_ATmega4809__) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_AVR_NANO_EVERY) ||  \
      defined(ARDUINO_AVR_ATmega4809) || defined(ARDUINO_AVR_ATmega4808) || defined(ARDUINO_AVR_ATmega3209) || \
      defined(ARDUINO_AVR_ATmega3208) || defined(ARDUINO_AVR_ATmega1609) || defined(ARDUINO_AVR_ATmega1608) || \
      defined(ARDUINO_AVR_ATmega809) || defined(ARDUINO_AVR_ATmega808))
#error This is designed only for Arduino or MegaCoreX megaAVR board! Please check your Tools->Board setting
#endif

// These define's must be placed at the beginning before #include "megaAVR_TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0

// Select USING_16MHZ     == true for  16MHz to Timer TCBx => shorter timer, but better accuracy
// Select USING_8MHZ      == true for   8MHz to Timer TCBx => shorter timer, but better accuracy
// Select USING_250KHZ    == true for 250KHz to Timer TCBx => shorter timer, but better accuracy
// Not select for default 250KHz to Timer TCBx => longer timer,  but worse accuracy
#define USING_16MHZ true
#define USING_8MHZ false
#define USING_250KHZ false

// Select the timers you're using, here ITimer1
#define USE_TIMER_0 false
#define USE_TIMER_1 true
#define USE_TIMER_2 false
#define USE_TIMER_3 false

// **************************************************************************************************************
// End Section Interrupt                                                                                        *
// **************************************************************************************************************

// **************************************************************************************************************
// General defines, global variables and function prototypes                                                    *
// **************************************************************************************************************

// flag here to turn on serial debugging
#define DO_DEBUG false



#define latch_pin 2  // can use any pin you want to latch the shift registers
#define blank_pin 4  // same, can use any pin you want for this, just make sure you pull up via a 1k to 5V
#define data_pin 11  // used by SPI, must be pin 11
#define clock_pin 13 // used by SPI, must be 13

// Global Variables
// These variables are used by multiplexing and Bit Angle Modulation Code
int shift_out;
byte anode[8]; // byte to write to the anode shift register, 8 of them, shifting the ON level in each byte in the array

// This is how the brightness for every LED is stored,
// Each LED only needs a 'bit' to know if it should be ON or OFF, so 64 Bytes gives you 512 bits= 512 LEDs
// Since we are modulating the LEDs, using 4 bit resolution, each color has 4 arrays containing 64 bits each
byte red0[64], red1[64], red2[64], red3[64];
byte blue0[64], blue1[64], blue2[64], blue3[64];
byte green0[64], green1[64], green2[64], green3[64];

int level = 0;                // keeps track of which level we are shifting data to
int anodelevel = 0;           // this increments through the anode levels
int BAM_Bit, BAM_Counter = 0; // Bit Angle Modulation variables to keep track of things
unsigned long start;          // for a millis timer to cycle through the animations



// **************************************************************************************************************
// End Section General defines, global variables and function prototypes                                        *
// **************************************************************************************************************


const int maxLevel = 0;
const int maxRow = 0;
const int maxColumn = 7;
const byte maxColor = 15;

// Function prtotypes
void rainVersionTwo();
void folder();
void sinwaveTwo();
void clean();
void bouncyvTwo();
void color_wheelTWO();
void harlem_shake();
void drawLED(int level, int row, int column, byte red, byte green, byte blue);
void ledUpdateInterruptHandler(void);
void testCube(void);

#endif //LEDCHECK_LEDCHECK_H