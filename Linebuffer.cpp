#include "Linebuffer.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

Linebuffer::Linebuffer() {
  //  
}


bool Linebuffer::writePixelToBuffer(unsigned int pixelPosition, bool isWhite) {
  if(pixelPosition < lcdPixelWidth) {
    bitWrite(buffer[pixelPosition/8], 7 - pixelPosition%8, isWhite);
    return true;
  }
  else {
    return false;
  }
}


bool Linebuffer::writeByteToBuffer(byte bytePosition, byte byteContents) {
  if(bytePosition < bufferSize) {
    buffer[bytePosition] = byteContents;
    return true;
  }
  else {
    return false;
  }
}


void Linebuffer::clearBuffer() {
  for(byte i=0; i<lcdPixelWidth/8; i++) {
    buffer[i] = 0xFF;
  }
}


void Linebuffer::setBufferBlack() {
  for(byte i=0; i<lcdPixelWidth/8; i++) {
    buffer[i] = 0x00;
  }
}
  

void Linebuffer::setBufferWhite() {
  clearBuffer();
}


const char * const Linebuffer::getPointerToBuffer() {
  return pBuffer;
}
