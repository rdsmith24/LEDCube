#include <Arduino.h>
#include "LEDCube.h"
#include "megaAVR_TimerInterrupt.h"
// SPI Library used to clock data out to the shift registers
#include <SPI.h>

#define TIMER1_FREQUENCY (float)(8000.0f)

// **************************************************************************************************************
// Start Arduino setup section                                                                                  *
// **************************************************************************************************************
void setup()
{
#if DO_DEBUG
  Serial.begin(115200);
  // Wait for serial line to get setup
  while (!Serial)
    ;

  // Startup message
  Serial.print(F("\nStarting LEDCube on "));
  Serial.println(BOARD_NAME);
  Serial.print(F("CPU Frequency = "));
  Serial.print(F_CPU / 1000000);
  Serial.println(F(" MHz"));
  Serial.print(F("TCB Clock Frequency = "));
#if USING_16MHZ
  Serial.println(F("16MHz for highest accuracy"));
#elif USING_8MHZ
  Serial.println(F("8MHz for very high accuracy"));
#else
  Serial.println(F("250KHz for lower accuracy but longer time"));
#endif
#endif

  // Initialize ITimer1
  ITimer1.init();

  // Attach interrupt handler function
  if (ITimer1.attachInterrupt(TIMER1_FREQUENCY, ledUpdateInterruptHandler))
#if DO_DEBUG
  {
    Serial.print(F("Starting  ITimer1 OK, millis() = "));
    Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
#endif

  // Setup SPI transmission parameters
  SPI.setBitOrder(MSBFIRST);           // Most Significant Bit First
  SPI.setDataMode(SPI_MODE0);          // Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2); // Run the data in at 16MHz/2 - 8MHz

  // Setup the anode array, this is what's written to the anode shift register, to enable each level
  anode[0] = B00000001;
  anode[1] = B00000010;
  anode[2] = B00000100;
  anode[3] = B00001000;
  anode[4] = B00010000;
  anode[5] = B00100000;
  anode[6] = B01000000;
  anode[7] = B10000000;

  // Finally, set up the Outputs
  pinMode(latch_pin, OUTPUT); // Latch
  pinMode(data_pin, OUTPUT);  // MOSI DATA
  pinMode(clock_pin, OUTPUT); // SPI Clock
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(blank_pin, OUTPUT);

  // start up the SPI library
  SPI.begin();

}

// **************************************************************************************************************
// End Arduino setup section                                                                                    *
// **************************************************************************************************************

// **************************************************************************************************************
// Start Arduino looping section                                                                                *
// **************************************************************************************************************
void loop()
{
  testCube();
}

// **************************************************************************************************************
// End Arduino looping section                                                                                  *
// **************************************************************************************************************

// **************************************************************************************************************
// Start interrupt handler function                                                                             *
// **************************************************************************************************************
void ledUpdateInterruptHandler(void)
{
  /*
  static bool toggle1 = false;
  static bool started = false;

  if (!started)
  {
    started = true;
    pinMode(blank_pin, OUTPUT);
  }

  // Make the blank_pin HIGH, then make it low to turn on the LEDs with new data
  PORTD |= 1 << blank_pin;
  PORTD &= ~(toggle1 << blank_pin); // Blank pin LOW to turn on the LEDs with the new data
  // digitalWrite(blank_pin, toggle1);
  toggle1 = !toggle1;
*//////////////////////////////////////////

  // This routine is called in the background automatically at frequency set by OCR1A
  // In this code, I set OCR1A to 30, so this is called every 124us, giving each level in the cube 124us of ON time
  // There are 8 levels, so we have a maximum brightness of 1/8, since the level must turn off before the next level is turned on
  // The frequency of the multiplexing is then 124us*8=992us, or 1/992us= about 1kHz

  PORTD |= 1 << blank_pin; // The first thing we do is turn all of the LEDs OFF, by writing a 1 to the blank pin
  // Note, in my bread-boarded version, I was able to move this way down in the cube, meaning that the OFF time was minimized
  // do to signal integrity and parasitic capcitance, my rise/fall times, required all of the LEDs to first turn off, before updating
  // otherwise you get a ghosting effect on the previous level

  // This is 4 bit 'Bit ngle Modulation' or BAM, There are 8 levels, so when a '1' is written to the color brightness,
  // each level will have a chance to light up for 1 cycle, the BAM bit keeps track of which bit we are modulating out of the 4 bits
  // Bam counter is the cycle count, meaning as we light up each level, we increment the BAM_Counter
  if (BAM_Counter == 8)
    BAM_Bit++;
  else if (BAM_Counter == 24)
    BAM_Bit++;
  else if (BAM_Counter == 56)
    BAM_Bit++;

// Here is where we increment the BAM counter
  BAM_Counter++; 

  // The BAM bit will be a value from 0-3, and only shift out the arrays corresponding to that bit, 0-3
  // Here's how this works, each case is the bit in the Bit angle modulation from 0-4,
  // Next, it depends on which level we're on, so the byte in the array to be written depends on which level, but since each level contains 64 LED,
  // we only shift out 8 bytes for each color
  switch (BAM_Bit)
  {
  case 0:
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(red0[shift_out]);
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(green0[shift_out]);
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(blue0[shift_out]);
    break;
  case 1:
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(red1[shift_out]);
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(green1[shift_out]);
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(blue1[shift_out]);
    break;
  case 2:
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(red2[shift_out]);
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(green2[shift_out]);
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(blue2[shift_out]);
    break;
  case 3:
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(red3[shift_out]);
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(green3[shift_out]);
    for (shift_out = level; shift_out < level + 8; shift_out++)
      SPI.transfer(blue3[shift_out]);
    // Here is where the BAM_Counter is reset back to 0, it's only 4 bit, but since each cycle takes 8 counts,
    //, it goes 0 8 16 32, and when BAM_counter hits 64 we reset the BAM
    if (BAM_Counter == 120)
    {
      BAM_Counter = 0;
      BAM_Bit = 0;
    }
    break;
  } // switch_case

  SPI.transfer(anode[anodelevel]); // finally, send out the anode level byte

  PORTD |= 1 << latch_pin;    // Latch pin HIGH
  PORTD &= ~(1 << latch_pin); // Latch pin LOW
  PORTD &= ~(1 << blank_pin); // Blank pin LOW to turn on the LEDs with the new data

  anodelevel++;      // inrement the anode level
  level = level + 8; // increment the level variable by 8, which is used to shift out data, since the next level woudl be the next 8 bytes in the arrays

  if (anodelevel == 8) // go back to 0 if max is reached
    anodelevel = 0;
  if (level == 64) // if you hit 64 on level, this means you just sent out all 63 bytes, so go back
    level = 0;

 // moved down here so outputs are all off until the first call of this function
  //pinMode(blank_pin, OUTPUT); // moved down here so outputs are all off until the first call of this function
  DDRD |= _BV (blank_pin);  //faster version of line above
}

// **************************************************************************************************************
// End interrupt handler function                                                                               *
// **************************************************************************************************************

// **************************************************************************************************************
// Start function to draw LED with color                                                                        *
// **************************************************************************************************************
void drawLED(int level, int row, int column, byte red, byte green, byte blue)
{
  static int whichbyte;
  static int wholebyte;

  // This routine is how LEDs are updated, with the inputs for the LED location and its R G and B brightness levels

  // First make sure everything is in proper range
  // This code is ugly but it runs three times faster than inline function to bounds check
  // level >= 0 <= 7
  // row >= 0 <= 7
  // column >=0 <= 7
  // red >= 0 <= 15
  // green >= 0 <= 15
  // blue  >= 0 <= 15

  if (level < 0)
    level = 0;
  if (level > 7)
    level = 7;
  if (row < 0)
    row = 0;
  if (row > 7)
    row = 7;
  if (column < 0)
    column = 0;
  if (column > 7)
    column = 7;
  if (red < 0)
    red = 0;
  if (red > 15)
    red = 15;
  if (green < 0)
    green = 0;
  if (green > 15)
    green = 15;
  if (blue < 0)
    blue = 0;
  if (blue > 15)
    blue = 15;

  // There are 512 LEDs in the cube, so when we write to level 2, column 5, row 4, that needs to be translated into a number from 0 to 511
  whichbyte = int(((level * 64) + (row * 8) + column) / 8);

  // The first level LEDs are first in the sequence, then 2nd level, then third, and so on
  // the (level*64) is what indexes the level's starting place, so level 0 are LEDs 0-63, level 1 are LEDs 64-127, and so on

  // The column counts left to right 0-7 and the row is back to front 0-7
  // This means that if you had level 0, row 0, the bottom back row would count from 0-7,

  // so if you looked down on the cube, and only looked at the bottom level
  //  00 01 02 03 04 05 06 07
  //  08 09 10 11 12 13 14 15
  //  16 17 18 19 20 21 22 23
  //  24 25 26 27 28 29 30 31
  //  32 33 34 35 36 37 38 39
  //  40 41 42 43 44 45 46 47
  //  48 49 50 51 52 53 54 55
  //  56 57 58 59 60 61 62 63

  // Then, if you incremented the level, the top right of the grid above would start at 64
  // The reason for doing this, is so you don't have to memorize a number for each LED, allowing you to use level, row, column

  // Now, what about the divide by 8 in there?
  //...well, we have 8 bits per byte, and we have 64 bytes in memory for all 512 bits needed for each LED, so
  // we divide the number we just found by 8, and take the integer of it, so we know which byte, that bit is located
  // confused? that's ok, let's take an example, if we wanted to write to the LED to the last LED in the cube, we would write a 7, 7, 7
  //  giving (7*64)+(7*8)=7 = 511, which is right, but now let's divide it by 8, 511/8 = 63.875, and take the int of it so, we get 63,
  // this is the last byte in the array, which is right since this is the last LED

  // This next variable is the same thing as before, but here we don't divide by 8, so we get the LED number 0-511
  wholebyte = (level * 64) + (row * 8) + column;
  // This will all make sense in a sec

  // This is 4 bit color resolution, so each color contains x4 64 byte arrays, explanation below:
  bitWrite(red0[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 0));
  bitWrite(red1[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 1));
  bitWrite(red2[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 2));
  bitWrite(red3[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 3));

  bitWrite(green0[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 0));
  bitWrite(green1[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 1));
  bitWrite(green2[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 2));
  bitWrite(green3[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 3));

  bitWrite(blue0[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 0));
  bitWrite(blue1[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 1));
  bitWrite(blue2[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 2));
  bitWrite(blue3[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 3));

  // Are you now more confused?  You shouldn't be!  It's starting to make sense now.  Notice how each line is a bitWrite, which is,
  // bitWrite(the byte you want to write to, the bit of the byte to write, and the 0 or 1 you want to write)
  // This means that the 'whichbyte' is the byte from 0-63 in which the bit corresponding to the LED from 0-511
  // Is making sense now why we did that? taking a value from 0-511 and converting it to a value from 0-63, since each LED represents a bit in
  // an array of 64 bytes.
  // Then next line is which bit 'wholebyte-(8*whichbyte)'
  // This is simply taking the LED's value of 0-511 and subracting it from the BYTE its bit was located in times 8
  // Think about it, byte 63 will contain LEDs from 504 to 511, so if you took 505-(8*63), you get a 1, meaning that,
  // LED number 505 is is located in bit 1 of byte 63 in the array

  // is that it?  No, you still have to do the bitRead of the brightness 0-15 you are trying to write,
  // if you wrote a 15 to RED, all 4 arrays for that LED would have a 1 for that bit, meaning it will be on 100%
  // This is why the four arrays read 0-4 of the value entered in for RED, GREEN, and BLUE
  // hopefully this all makes some sense?
}
// **************************************************************************************************************
// End drawLED                                                                                                  *
// **************************************************************************************************************

// **************************************************************************************************************
// Start function test LEDs                                                                                     *
// **************************************************************************************************************
void testCube(void)
{
  for (int level = 0; level <= maxLevel; level++)
  {
    for (int row = 0; row <= maxRow; row++)
    {
      for (int column = 0; column <= maxColumn; column++)
      {
        for (byte red = 0; red <= maxColor; red++)
        {
          for (byte green = 0; green <= maxColor; green++)
          {
            for (byte blue = 0; blue <= maxColor; blue++)
            {
              drawLED(level, row, column, red, green, blue);
            }
          }
        }
      }
    }
  }
}