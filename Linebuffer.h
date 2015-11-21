#ifndef _LINEBUFFER_LIB_H
#define _LINEBUFFER_LIB_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/*
 * File contains two classes implementing linebuffers for the Sharp Memory LCD
 * MonoLinebuffer for all monochrome Memory LCDs
 * ColourLinebuffer for the single model of RGB colour Memory LCD
 * 
 * ATTENTION: The class MonoLinebuffer requires editing before use in order
 * to match it to the particular LCD model being used
 */

class MonoLinebuffer {
public:     
  MonoLinebuffer();
  bool writePixelToBuffer(unsigned int pixelPosition, bool isWhite);
  bool writeByteToBuffer(uint8_t bytePosition, uint8_t byteContents);
  void clearBuffer();
  void setBufferBlack();
  void setBufferWhite();
  const uint8_t * const getPointerToBuffer();

private:
/*
 * START EDITING - COMMENT OR UNCOMMENT TO SUIT YOUR MODEL OF MEMORY LCD
 * 
 * Array size 12 for a 96 pixel wide LCD
 * Array size 16 for a 128 pixel wide LCD
 * Array size 50 for a 400 pixel wide LCD
*/

//   unsigned int lcdPixelWidth = 96
//   uint8_t bufferSize = 12;
//   uint8_t buffer[12]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//
  unsigned int lcdPixelWidth = 128;
  uint8_t bufferSize = 16;
  uint8_t buffer[16]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//
//   unsigned int lcdPixelWidth = 400;
//   uint8_t bufferSize = 50;
//   uint8_t buffer[50]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
// 			  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
// 			  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
// 			  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
// 			  0xFF, 0xFF};                  

/*
 * END EDITING - COMMENT OR UNCOMMENT TO SUIT YOUR MODEL OF MEMORY LCD
*/  
  uint8_t * const pBuffer = buffer; 
};


class ColourLinebuffer {
public:     
  enum colour {	// RGB
    black = 0,	// 000
    blue,	// 001
    green,	// 010
    cyan,	// 011
    red,	// 100
    magenta,	// 101
    yellow,	// 110
    white 	// 111
  };
  ColourLinebuffer();
  bool writePixelToBuffer(unsigned int pixelPosition, colour c);
  bool writePixelToBuffer(unsigned int pixelPosition, bool isRed, bool isGreen, bool isBlue);
  void clearBuffer();
  void setBufferColour(colour c);
  const uint8_t * const getPointerToBuffer();
private:
  static const unsigned int lcdPixelWidth = 128;
  static const uint8_t bufferSize = 48;
  uint8_t buffer[bufferSize] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
			     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };                  

/*
 * END EDITING - COMMENT OR UNCOMMENT TO SUIT YOUR MODEL OF MEMORY LCD
*/  
  uint8_t * const pBuffer = buffer; 
};

#endif