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
  bool writeByteToBuffer(byte bytePosition, byte byteContents);
  void clearBuffer();
  void setBufferBlack();
  void setBufferWhite();
  const char * const getPointerToBuffer();

private:
/*
 * START EDITING - COMMENT OR UNCOMMENT TO SUIT YOUR MODEL OF MEMORY LCD
 * 
 * Array size 12 for a 96 pixel wide LCD
 * Array size 16 for a 128 pixel wide LCD
 * Array size 50 for a 400 pixel wide LCD
*/

//   unsigned int lcdPixelWidth = 96
//   char bufferSize = 12;
//   char buffer[12]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//
  unsigned int lcdPixelWidth = 128;
  char bufferSize = 16;
  char buffer[16]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//
//   unsigned int lcdPixelWidth = 400;
//   char bufferSize = 50;
//   char buffer[50]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
// 			  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
// 			  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
// 			  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
// 			  0xFF, 0xFF};                  

/*
 * END EDITING - COMMENT OR UNCOMMENT TO SUIT YOUR MODEL OF MEMORY LCD
*/  
  char * const pBuffer = buffer; 
};


class ColourLinebuffer {
public:     
  enum colour {	// RGB
    black,	// 000
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
//  bool writeByteToBuffer(byte bytePosition, byte byteContents);
  void clearBuffer();
  void setBufferColour(colour c);
  const char * const getPointerToBuffer();
private:
  static const unsigned int lcdPixelWidth = 128;
  static const byte bufferSize = 48;
  char buffer[bufferSize] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
			     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };                  

/*
 * END EDITING - COMMENT OR UNCOMMENT TO SUIT YOUR MODEL OF MEMORY LCD
*/  
  char * const pBuffer = buffer; 
};

#endif