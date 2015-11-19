#include "Linebuffer.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/*
 * MonoLinebuffer class methods
 */


MonoLinebuffer::MonoLinebuffer() {
  //  
}


bool MonoLinebuffer::writePixelToBuffer(unsigned int pixelPosition, bool isWhite) {
  if(pixelPosition < lcdPixelWidth) {
    unsigned int currentByte = pixelPosition/8;
    byte bitWithinByte = 7 - (pixelPosition%8);
    if(isWhite) {
      buffer[currentByte] |= (1 << bitWithinByte);
    }
    else {
      buffer[currentByte] &= ~(1 << bitWithinByte);
    }    
    return true;
  }
  else {
    return false;
  }
  // original non-portable Arduino bit setting function
  // bitWrite(buffer[pixelPosition/8], 7 - pixelPosition%8, isWhite);
}


bool MonoLinebuffer::writeByteToBuffer(byte bytePosition, byte byteContents) {
  if(bytePosition < bufferSize) {
    buffer[bytePosition] = byteContents;
    return true;
  }
  else {
    return false;
  }
}


void MonoLinebuffer::clearBuffer() {
  for(byte i=0; i<lcdPixelWidth/8; i++) {
    buffer[i] = 0xFF;
  }
}


void MonoLinebuffer::setBufferBlack() {
  for(byte i=0; i<lcdPixelWidth/8; i++) {
    buffer[i] = 0x00;
  }
}
  

void MonoLinebuffer::setBufferWhite() {
  clearBuffer();
}


const char * const MonoLinebuffer::getPointerToBuffer() {
  return pBuffer;
}


/* 
 * ColourLinebuffer class methods
 */


ColourLinebuffer::ColourLinebuffer() {
  //  
}


bool ColourLinebuffer::writePixelToBuffer(unsigned int pixelPosition, colour c) {
  if((c < 8) && (pixelPosition < lcdPixelWidth)) {
    unsigned int currentBit;	// for a 128px LCD, value should be in range 0-383
    unsigned int currentByte;	// for a 128px LCD, value should be in range 0-127
    byte bitWithinByte;	// position of currentBit within the currentByte, value in range 0-7
    for(byte i=0; i<3; i++) {
      currentBit = (pixelPosition*3) + i;
      currentByte = currentBit/8;
      bitWithinByte = 7-(currentBit%8);
      if( (c & (1 < i)) == 0 ) {
	// set bit to zero
	buffer[currentByte] &= ~(1 << bitWithinByte);
      }
      else {
	// set bit to one
	buffer[currentByte] |= (1 << bitWithinByte);
      }
    }
    return true;
  }
  else {
    return false;
  }
}


bool ColourLinebuffer::writePixelToBuffer(unsigned int pixelPosition, bool isRed, bool isGreen, bool isBlue) {
  if(pixelPosition < lcdPixelWidth) {
    byte RGB = 0;
    if(isBlue)
      RGB = 1;
    if(isGreen)
      RGB |= (1 << 1);
    if(isRed)
      RGB |= (1 << 2);
    return writePixelToBuffer(pixelPosition, (colour)RGB);
  }
  else {
    return false;
  }
}


// Commented out pending deletion as pixels cross over byte boundaries. 
// bool ColourLinebuffer::writeByteToBuffer(byte bytePosition, byte byteContents) {
//   if(bytePosition < bufferSize) {
//     buffer[bytePosition] = byteContents;
//     return true;
//   }
//   else {
//     return false;
//   }
// }


void ColourLinebuffer::clearBuffer() {
  for(byte i=0; i<lcdPixelWidth/8; i++) {
    buffer[i] = 0xFF;
  }
}


void ColourLinebuffer::setBufferColour(colour c) {
  // TODO: individually Set the first 8 pixels (24 bits, 3 bytes) 
  // then copy the whole bytes to the rest of the linebuffer for speed?
  for(unsigned int i=0; i<lcdPixelWidth; i++) {
    writePixelToBuffer(i, c);
  }
}
  

const char * const ColourLinebuffer::getPointerToBuffer() {
  return pBuffer;
}