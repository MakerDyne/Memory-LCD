#include "Linebuffer.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

MonoLinebuffer::MonoLinebuffer() {
  //  
}


bool MonoLinebuffer::writePixelToBuffer(unsigned int pixelPosition, bool isWhite) {
  if(pixelPosition < lcdPixelWidth) {
    bitWrite(buffer[pixelPosition/8], 7 - pixelPosition%8, isWhite);
    return true;
  }
  else {
    return false;
  }
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
