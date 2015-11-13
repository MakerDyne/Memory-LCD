#ifndef _LINEBUFFER_LIB_H
#define _LINEBUFFER_LIB_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class Linebuffer {
public:     
  Linebuffer();
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

#endif