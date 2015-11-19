#include "MemoryLCD.h"
#include "Linebuffer.h"
#include <SPI.h>

// ---------------------------------------------------------------------------
// Breadboard wiring requirements:
// ---------------------------------------------------------------------------
/*

DO NOT USE PIN 10! It needs to be set as, and remain as, an output for the 
SD card library to work (apparently). See the Arduino SD card examples for
this warning.

Pin connections between Arduino and Memory LCD
Arduino - Memory LCD
  8     -    DISP
  9     -    EXTCOMIN
  7    -    SCS
  11    -    SI
  13    -    SCLK
  Vin   -    5V
  GND   -    GND

Pin connections between Arduino and SD card reader
Arduino - SC Card Reader
  11    -    MOSI
  12    -    MISO
  13    -    CLK
  10     -    CS
  5V    -    5V
  GND   -    GND

Tested on an Arduino Duemilanove
Card reader used is an 'Adafruit 5V ready MicroSD Breakout Board+'
*/

// ---------------------------------------------------------------------------
// Variables for Arduino pin assignments: DUEMILANOVE
// ---------------------------------------------------------------------------

//Memory LCD / SD card reader shared pins
const byte SPI_MOSI      = 11; //  11/51  // Do not change - hardware SPI MOSI pin   // "SI" on Memory LCD breakout, "MOSI" on SD card breakout 
const byte SPI_SCLK      = 13; //  13/52  // Do not change - hardware SPI CLK pin    // "SCLK" on Memory LCD breakout, "CLK" on SD card breakout

// Memory LCD exclusive pins
const byte LCD_EXTCOMIN  = 9;
const byte LCD_DISP      = 8;             // Can use any pin                         // Memory LCD Display on/off pin
const byte LCD_SCS       = 7;             // Can use any pin except dedicated SPI SS pin  // Memory LCD SPI chip select pin

// SD card reader exclusive pins
const byte SPI_MISO      = 12; //  12/50  // Do not change - hardware SPI MISO pin  //  "DO" on SC card breakout
const byte SD_SCS        = 10;            // Use dedicated SPI SS pin to be safe    // "CS" on SD card breakout

// Offical CS pin that must be left as output for SD card lib to work
const byte CS            = 53; //  10/53  // switch between pin 10/53 for duemilanove/mega

// ---------------------------------------------------------------------------
// Variables assignments & initialisations:
// ---------------------------------------------------------------------------


// Create linebuffer to write display data to
ColourLinebuffer lBuffer;

// Memory LCD related variables
MemoryLCD memLcd(LCD_SCS, SPI_MOSI, SPI_SCLK, LCD_DISP, LCD_EXTCOMIN, true);
char * dynamicFramebuffer;
byte linesInFramebuffer;
int lcdWidth;
int lcdHeight;

// Memory LCD interrupt service routine for hardware EXTCOMIN toggle
SIGNAL(TIMER0_COMPA_vect) 
{
  memLcd.hardToggleVCOM();
}


/*
 * Demo related variables
 */
byte numRepetitions = 4;
boolean toggle = true;


void setup() {  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // wait for serial port to connect. Needed for Arduino Leonardo only
  while (!Serial) {
    ;
  }
  
  // Initialise Memory LCD
  // clearDisplay() is important to erase the random pixel pattern displayed when power is first applied to the LCD.
  memLcd.begin();
  memLcd.clearDisplay();
  lcdWidth = memLcd.getLcdWidth();
  lcdHeight = memLcd.getLcdHeight();
    
  // Wait a while (optional - allows time to open serial console, if needed)
  delay(2000);
} // end of setup  


void loop(void) {

}