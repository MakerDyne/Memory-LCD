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


bool MonoLinebuffer::writePixelToBuffer(uint16_t pixelPosition, bool isWhite) {
  if(pixelPosition < lcdPixelWidth) {
    uint8_t currentByte = pixelPosition/8;
    uint8_t bitShift = 7 - (pixelPosition%8);
    if(isWhite) {
      buffer[currentByte] |= (1 << bitShift);
    }
    else {
      buffer[currentByte] &= ~(1 << bitShift);
    }    
    return true;
  }
  else {
    return false;
  }
}


bool MonoLinebuffer::writeByteToBuffer(uint8_t bytePosition, uint8_t byteContents) {
  if(bytePosition < bufferSize) {
    buffer[bytePosition] = byteContents;
    return true;
  }
  else {
    return false;
  }
}


void MonoLinebuffer::clearBuffer() {
  for(uint8_t i=0; i<bufferSize; i++) {
    buffer[i] = 0xFF;
  }
}


void MonoLinebuffer::setBufferBlack() {
  for(uint8_t i=0; i<bufferSize; i++) {
    buffer[i] = 0x00;
  }
}
  

void MonoLinebuffer::setBufferWhite() {
  clearBuffer();
}


const uint8_t * const MonoLinebuffer::getPointerToBuffer() {
  return pBuffer;
}


/* 
 * ColourLinebuffer class methods
 */


ColourLinebuffer::ColourLinebuffer() {
  //  
}


bool ColourLinebuffer::writePixelToBuffer(uint16_t pixelPosition, colour c) {
  if((c < 8) && (pixelPosition < lcdPixelWidth)) {
    uint16_t currentBit;	// for a 128px LCD, value should be in range 0-383
    uint16_t currentByte;	// for a 128px LCD, value should be in range 0-127
    uint8_t bitShift;	// position of currentBit within the currentByte, value in range 0-7
    for(uint8_t i=0; i<3; i++) {
      currentBit = (pixelPosition*3) + i;
      currentByte = currentBit/8;
      bitShift = 7-(currentBit%8);
      if( (c & (1 << (2-i))) == 0 ) {
	// set bit to zero
	buffer[currentByte] &= ~(1 << bitShift);
      }
      else {
	// set bit to one
	buffer[currentByte] |= (1 << bitShift);
      }
    }
    return true;
  }
  else {
    return false;
  }
}


bool ColourLinebuffer::writePixelToBuffer(uint16_t pixelPosition, bool isRed, bool isGreen, bool isBlue) {
  if(pixelPosition < lcdPixelWidth) {
    uint8_t RGB = 0;
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


void ColourLinebuffer::clearBuffer() {
  for(uint8_t i=0; i<bufferSize; i++) {
    buffer[i] = 0xFF;
  }
}


void ColourLinebuffer::setBufferColour(colour c) {
  //clearBuffer();
  for(uint16_t i=0; i<8; i++) {
    writePixelToBuffer(i, c);
  }
  for(uint8_t i=3; i<bufferSize; i++) {
    buffer[i] = buffer[i%3];
  }
}

const uint8_t * const ColourLinebuffer::getPointerToBuffer() {
  return pBuffer;
}