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
MonoLinebuffer lBuffer;

// Memory LCD related variables
MemoryLCD memLcd(LCD_SCS, SPI_MOSI, SPI_SCLK, LCD_DISP, LCD_EXTCOMIN, true);
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
static const float sineDegrees[]=
{0.0000, 0.0175, 0.0349, 0.0523, 0.0698, 0.0872, 0.1045, 0.1219, 0.1392, 0.1564, 0.1736, 0.1908,
0.2079, 0.2250, 0.2419, 0.2588, 0.2756, 0.2924, 0.3090, 0.3256, 0.3420, 0.3584, 0.3746, 0.3907, 
0.4067, 0.4226, 0.4384, 0.4540, 0.4695, 0.4848, 0.5000, 0.5150, 0.5299, 0.5446, 0.5592, 0.5736, 
0.5878, 0.6018, 0.6157, 0.6293, 0.6428, 0.6561, 0.6691, 0.6820, 0.6947, 0.7071, 0.7193, 0.7314, 
0.7431, 0.7547, 0.7660, 0.7771, 0.7880, 0.7986, 0.8090, 0.8192, 0.8290, 0.8387, 0.8480, 0.8572, 
0.8660, 0.8746, 0.8829, 0.8910, 0.8988, 0.9063, 0.9135, 0.9205, 0.9272, 0.9336, 0.9397, 0.9455, 
0.9511, 0.9563, 0.9613, 0.9659, 0.9703, 0.9744, 0.9781, 0.9816, 0.9848, 0.9877, 0.9903, 0.9925, 
0.9945, 0.9962, 0.9976, 0.9986, 0.9994, 0.9998, 1.0000, 0.9998, 0.9994, 0.9986, 0.9976, 0.9962, 
0.9945, 0.9925, 0.9903, 0.9877, 0.9848, 0.9816, 0.9781, 0.9744, 0.9703, 0.9659, 0.9613, 0.9563, 
0.9511, 0.9455, 0.9397, 0.9336, 0.9272, 0.9205, 0.9135, 0.9063, 0.8988, 0.8910, 0.8829, 0.8746, 
0.8660, 0.8572, 0.8480, 0.8387, 0.8290, 0.8192, 0.8090, 0.7986, 0.7880, 0.7771, 0.7660, 0.7547, 
0.7431, 0.7314, 0.7193, 0.7071, 0.6947, 0.6820, 0.6691, 0.6561, 0.6428, 0.6293, 0.6157, 0.6018, 
0.5878, 0.5736, 0.5592, 0.5446, 0.5299, 0.5150, 0.5000, 0.4848, 0.4695, 0.4540, 0.4384, 0.4226, 
0.4067, 0.3907, 0.3746, 0.3584, 0.3420, 0.3256, 0.3090, 0.2924, 0.2756, 0.2588, 0.2419, 0.2250, 
0.2079, 0.1908, 0.1736, 0.1564, 0.1392, 0.1219, 0.1045, 0.0872, 0.0698, 0.0523, 0.0349, 0.0175, 
0.0000, -0.0175, -0.0349, -0.0523, -0.0698, -0.0872, -0.1045, -0.1219, -0.1392, -0.1564, -0.1736, 
-0.1908, -0.2079, -0.2250, -0.2419, -0.2588, -0.2756, -0.2924, -0.3090, -0.3256, -0.3420, -0.3584, 
-0.3746, -0.3907, -0.4067, -0.4226, -0.4384, -0.4540, -0.4695, -0.4848, -0.5000, -0.5150, -0.5299, 
-0.5446, -0.5592, -0.5736, -0.5878, -0.6018, -0.6157, -0.6293, -0.6428, -0.6561, -0.6691, -0.6820, 
-0.6947, -0.7071, -0.7193, -0.7314, -0.7431, -0.7547, -0.7660, -0.7771, -0.7880, -0.7986, -0.8090, 
-0.8192, -0.8290, -0.8387, -0.8480, -0.8572, -0.8660, -0.8746, -0.8829, -0.8910, -0.8988, -0.9063, 
-0.9135, -0.9205, -0.9272, -0.9336, -0.9397, -0.9455, -0.9511, -0.9563, -0.9613, -0.9659, -0.9703, 
-0.9744, -0.9781, -0.9816, -0.9848, -0.9877, -0.9903, -0.9925, -0.9945, -0.9962, -0.9976, -0.9986, 
-0.9994, -0.9998, -1.0000, -0.9998, -0.9994, -0.9986, -0.9976, -0.9962, -0.9945, -0.9925, -0.9903, 
-0.9877, -0.9848, -0.9816, -0.9781, -0.9744, -0.9703, -0.9659, -0.9613, -0.9563, -0.9511, -0.9455, 
-0.9397, -0.9336, -0.9272, -0.9205, -0.9135, -0.9063, -0.8988, -0.8910, -0.8829, -0.8746, -0.8660, 
-0.8572, -0.8480, -0.8387, -0.8290, -0.8192, -0.8090, -0.7986, -0.7880, -0.7771, -0.7660, -0.7547, 
-0.7431, -0.7314, -0.7193, -0.7071, -0.6947, -0.6820, -0.6691, -0.6561, -0.6428, -0.6293, -0.6157, 
-0.6018, -0.5878, -0.5736, -0.5592, -0.5446, -0.5299, -0.5150, -0.5000, -0.4848, -0.4695, -0.4540, 
-0.4384, -0.4226, -0.4067, -0.3907, -0.3746, -0.3584, -0.3420, -0.3256, -0.3090, -0.2924, -0.2756, 
-0.2588, -0.2419, -0.2250, -0.2079, -0.1908, -0.1736, -0.1564, -0.1392, -0.1219, -0.1045, -0.0872, 
-0.0698, -0.0523, -0.0349, -0.0175 };


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
//   // print sinewave
//   float increment = 360.00/lcdWidth;  // one number MUST have a decimal point here!
//   int sinValue = 0;
//   //for(float theta=0; theta<50.26; theta += 0.1256) {
//   for(int theta=0; theta<211; theta +=30) {
//     for(int y=0; y<lcdHeight; y++) {
//         for(int x=0; x<lcdWidth; x++) {
//         //sinValue = (sin(theta+(x*increment))*lcdHeight/2)+lcdHeight/2;
//         sinValue = ( sinLookupDegrees( theta+(x*increment) ) * lcdHeight/2 ) + lcdHeight/2;
//         if(sinValue >= y && y > lcdHeight/2) {
// 	  writePixelToLineBuffer(lineBuffer,x, 0);
//           writePixelToLineBuffer(lineBuffer, x, 0);
//         }
//         if(sinValue <= y && y <= lcdHeight/2) {


//           writePixelToLineBuffer(lineBuffer,x, 0);
//         }
//       }
//       memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), y);
//       lBuffer.clearBuffer();
//     }
//     delay(100);
//     memLcd.clearDisplay();
//   }
//   
//   
//   // print expanding and contracting circles
//   int originX = lcdWidth/2;
//   int originY = lcdHeight/2;
//   int expandingCircleRadius = (lcdHeight/2)*0.9;
//   for(int repeat = 0; repeat < 2; repeat++) {
//     for(int radius = 5; radius < expandingCircleRadius; radius++) {
//       for(int y = originY - radius; y <= originY; y++) {
//         // need to calculate left and right limits of the circle
//         float theta = acos(float(abs(originY-y))/float(radius));
//         theta -= 1.5708;
//         int xLength = cos(theta)*float(radius);
//         for(int x = originX - xLength; x <= originX; x++) {
//           writePixelToLineBuffer(lineBuffer, x, 0);
// 	  writePixelToLineBuffer(lineBuffer, originX + (originX - x), 0);
//         }
//         memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), y, 1);
// 	memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), originY + (originY - y));
//         lBuffer.clearBuffer();
//       }
//     }
//     for(int radius = expandingCircleRadius; radius > 2; radius--) {
//       for(int y = originY - radius; y <= originY; y++) {
//         // need to calculate left and right limits of the circle
//         float theta = acos(float(abs(originY-y))/float(radius));
//         theta -= 1.5708;
//         int xLength = cos(theta)*float(radius);
//         for(int x = originX - xLength; x <= originX ; x++) {
//           writePixelToLineBuffer(lineBuffer, x, 0);
// 	  writePixelToLineBuffer(lineBuffer, originX + (originX - x), 0);
//         }
//         memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), y);
// 	memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), originY + (originY - y));
//         lBuffer.clearBuffer();
//       }
//       memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), originY+radius);
//       memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), originY-radius);
//     }
//     memLcd.clearDisplay();
//     lBuffer.clearBuffer();
//   }
//   
//   
//   // print circling circle
//   numRepetitions = 4;
//   int sweepRadius = (lcdHeight/2)*0.8;
//   int sweepOriginX = lcdWidth/2;
//   int sweepOriginY = lcdHeight/2;
//   int circleRadius = 0.7*((lcdHeight/2)-sweepRadius);
//   for(float rads=0; rads<6.2824*numRepetitions; rads += 0.04) {
//     // calculate circle centre
//     int circleOriginX = sweepOriginX + cos(rads)*sweepRadius;
//     int circleOriginY = sweepOriginY + sin(rads)*sweepRadius;
//     // draw circle about the centre
//     for(int y = circleOriginY - circleRadius; y <= circleOriginY; y++) {
//       // need to calculate left and right limits of the circle
//       float theta = acos(float(abs(circleOriginY-y))/float(circleRadius));
//       theta -= 1.5708;
//       int xLength = cos(theta)*float(circleRadius);
//       for(int x = circleOriginX - xLength; x <= circleOriginX; x++) {
//         writePixelToLineBuffer(lineBuffer, x, 0);
// 	writePixelToLineBuffer(lineBuffer, circleOriginX + (circleOriginX - x), 0);
//       }
//       memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), y);
//       memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), circleOriginY + (circleOriginY - y));
//       lBuffer.clearBuffer();
//     }
//     delay(15);
//     memLcd.clearDisplay();
//   }
//   memLcd.clearDisplay();
//   lBuffer.clearBuffer();
  
  
  // print triangles
  numRepetitions = 4;
  toggle = false;
  for(byte i=0; i< numRepetitions; i++) {
    for(int y=0; y<lcdHeight; y++) {
      for(int x=0; x<y+((lcdWidth-lcdHeight)/2); x++) {
	lBuffer.writePixelToBuffer(x, toggle);
      }
      memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), y);
      delay(5);
    }
    for(int y=lcdHeight; y>=0; y--) {
      for(int x=lcdWidth; x>y+((lcdWidth-lcdHeight)/2); x--) {
	lBuffer.writePixelToBuffer(x, toggle);
      }
      memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), y);
      delay(5);
    }
    if(toggle)
      toggle = false;
    else
      toggle =  true;
  }
  memLcd.clearDisplay();
  lBuffer.clearBuffer();
  
  
  // Print chequerboard patterns
  numRepetitions = 8;
  for(byte i=0; i<numRepetitions; i++) {
    for(int y=1; y<(lcdHeight+1); y++) {
      for(int x=0; x <lcdWidth/8; x++) {
	if(toggle) {
	  lBuffer.writeByteToBuffer(x, 0xFF);
	  toggle = false;
	} else {
	  lBuffer.writeByteToBuffer(x, 0x00);
	  toggle = true;
	}
      }
      memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), y-1);
      lBuffer.clearBuffer();
      if((y%8) == 0)
	if(toggle)
	  toggle = false;
	else
	  toggle = true;
    }
    delay(500);
    if(toggle)
      toggle = false;
    else
      toggle = true;
  }
  lBuffer.clearBuffer();
  memLcd.clearDisplay();
  
  
//   // print scrolling diagonal lines
//   numRepetitions = 32;
//   toggle = true;
//   const byte lineWidth = 8;
//   for(int i=0; i<numRepetitions; i++) {
//     // work only on the mimumum screen area for a complete repeating pattern
//     for(int y=0; y<(lineWidth*2)+1; y++) {
//       for(int x=0; x<(lineWidth*2)+1; x++) {
// 	writePixelToLineBuffer(lineBuffer, x, toggle);
// 	if((x-y+1+i)%lineWidth == 0 && x != 2*lineWidth) {
// 	  if(toggle)
// 	    toggle = false;
// 	  else
// 	    toggle = true;
// 	}
//       // now horizontally copy the repeating pattern to the rest of the screen
//       }for(byte i=3; i<=lcdWidth/8; i++) {
// 	if(i%2 == 0) 
// 	  memLcd.copyByteWithinLineBuffer(2,i);
// 	else
// 	  memLcd.copyByteWithinLineBuffer(1,i);
//       }
//       // now horizontally copy the repeating pattern to the rest of the screen
//       for(int i=y; i<=lcdHeight; i+=(2*lineWidth))
// 	memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), i);
//       lBuffer.clearBuffer();
//       //if((lcdWidth-y+1+i)%8 == 0) {
//       if((y+i)%8 == 0) {
// 	if(toggle)
// 	  toggle = false;
// 	else
// 	  toggle = true;
//       }
//     }
//     delay(200);
//     memLcd.clearDisplay();
//   }
//   lBuffer.clearBuffer();
//   memLcd.clearDisplay();
  
  
  // print black and white alternating screens
  // demonstrates use of writeByteToLineBuffer and writeLineBufferToDisplay
  numRepetitions = 4;
  for(byte i=0; i<numRepetitions; i++) {
    byte colour = 0x00;
    if(i%2 == 0)
      lBuffer.setBufferBlack();
    else
      lBuffer.setBufferWhite();
    for(byte y=0; y<lcdHeight; y++) {
      memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), y);
      delay(5);
    }
  }
  lBuffer.clearBuffer();
  memLcd.clearDisplay();
  
  
  // print horizontal line descending down the screen
  byte lineThickness = 8;
  for(int i=0; i<numRepetitions; i++) {
    for(byte y=0; y<lcdHeight+lineThickness+1; y++) {  // lcdHeight+10 to give the line some thickness
      int blackLine = y;
      int whiteLine = y - lineThickness;
      if(whiteLine > 0 && whiteLine < lcdHeight+1) {
        lBuffer.setBufferWhite();
	memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), whiteLine);
      }
      if(blackLine > 0 && blackLine < lcdHeight+1) {
        lBuffer.setBufferBlack();
	memLcd.displayOnLcd(lBuffer.getPointerToBuffer(), blackLine);
      }
      delay(5);
    }
  }
  // make sure you clear both the buffer and display before moving on to a new sequence
  lBuffer.clearBuffer();
  memLcd.clearDisplay();
}


float sinLookupDegrees(int degree) {
  return sineDegrees[degree%360];
}