#include "MemoryLCD.h"
#include <SPI.h>

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

MemoryLCD::MemoryLCD(const uint8_t SCSpin, const uint8_t SIpin, const uint8_t SCLKpin, const uint8_t DISPpin, const uint8_t EXTCOMINpin, const boolean useEXTCOMIN, displayType d)
		    : SCS(SCSpin), 
		      SI(SIpin),
		      SCLK(SCLKpin), 
		      DISP(DISPpin), 
		      EXTCOMIN(EXTCOMINpin), 
		      enablePWM(useEXTCOMIN),
		      colourDepth(d),
		      EXTCOMIN_PIN_STATE(false), 
		      pwm_interrupt_counter(0), 
		      commandByte(0b10000000),
		      vcomByte(0b01000000),
		      savedSpiControlRegister(SPCR) {
  // Delay for MemoryLCD to reach 5V
  delayMicroseconds(800);  // minimum 750us
}


void MemoryLCD::begin(void) {
  // setup timer for EXTCOMIN signal
  if(enablePWM) {
    pinMode(EXTCOMIN, OUTPUT);
    digitalWrite(EXTCOMIN, LOW);
    // New interrupt based EXCOMIN toggle, ARDUINO SPECIFIC
    TIMSK0 |= (1 << OCIE0A);
  }
  else {
    // TODO: setup timer driven interrupt instead?
  }

  // setup SPI
  SPI.begin();
  // Datasheet says SPI clock must have <1MHz frequency (SPI_CLOCK_DIV16)
  // but it may work at 2MHz (SPI_CLOCK_DIV8) or even 4MHz (SPI_CLOCK_DIV4)
  lcdSpiSettings = SPISettings(SPI_CLOCK_DIV4, MSBFIRST, SPI_MODE0);
  // Set pin modes
  pinMode(SCS, OUTPUT);
  pinMode(SI, OUTPUT);
  pinMode(SCLK, OUTPUT);
  digitalWrite(SCS, LOW);
  digitalWrite(SI, LOW);
  digitalWrite(SCLK, LOW);
  if(DISP != 255) {
    pinMode(DISP, OUTPUT);
    digitalWrite(DISP, LOW);
  }
  if(enablePWM) {
    pinMode(EXTCOMIN, OUTPUT);
//     digitalWrite(EXTCOMIN, LOW);
  }
  
  // Memory LCD startup sequence  
  digitalWrite(DISP, HIGH);
  delayMicroseconds(PWRUP_DISP_DELAY);

  pinMode(EXTCOMIN, OUTPUT);
  delayMicroseconds(PWRUP_EXTCOMIN_DELAY);
}


void MemoryLCD::end() {
  if(enablePWM) {
    TIMSK0 |= (1 << OCIE0A); // TODO disable this bit! (this line just copied from begin())
  }
  SPI.end();
}


void MemoryLCD::displayOnLcd(const uint8_t * const data, const uint8_t lineNumber, const uint8_t numLines) {
  SPI.beginTransaction(lcdSpiSettings);
  // this implementation writes multiple lines that are CONSECUTIVE (although they don't have to be (if this
  // funcion was appropriately modified), as an address is given for every line, not just the first in the sequence)
  // data for all lines should be stored in a single array
  const uint8_t * linePtr = data;
  digitalWrite(SCS, HIGH);
  delayMicroseconds(SCS_HIGH_DELAY);
  SPI.transfer(0b10000000);
  for(byte x=lineNumber+1; x<lineNumber+numLines+1; x++) {
    SPI.transfer(reverseBitOrder(x)); // only the line number goes LSBFIRST for some reason
   for(byte y=0; y<(LCDWIDTH*colourDepth)/8; y++) {
     SPI.transfer(*(linePtr++));
   }
    SPI.transfer(0x00);
    linePtr = data;
  }
  SPI.transfer(0x00);  // trailing padding chars
  delayMicroseconds(SCS_LOW_DELAY);
  digitalWrite(SCS, LOW);
  delayMicroseconds(INTERFRAME_DELAY);
  SPI.endTransaction();
}


void MemoryLCD::clearDisplay() {
  SPI.beginTransaction(lcdSpiSettings);
  digitalWrite(SCS, HIGH);
  delayMicroseconds(SCS_HIGH_DELAY);
  SPI.transfer(0b00100000);
  SPI.transfer(0x00);
  delayMicroseconds(SCS_LOW_DELAY);
  digitalWrite(SCS, LOW);
  delayMicroseconds(INTERFRAME_DELAY);
  SPI.endTransaction();
}


// won't work if DISP pin is not used
void MemoryLCD::turnOff() {
  if(DISP != 255)
    digitalWrite(DISP, HIGH);
}


// won't work if DISP pin is not used
void MemoryLCD::turnOn() {
  if(DISP != 255)
    digitalWrite(DISP, LOW);
}


int MemoryLCD::getLcdWidth() const {
  return LCDWIDTH;
}


int MemoryLCD::getLcdHeight() const {
  return LCDHEIGHT;
}
    

void MemoryLCD::softToggleVCOM() {
  SPI.beginTransaction(lcdSpiSettings);
  vcomByte ^= (0b01000000);
  digitalWrite(SCS, HIGH);
  delayMicroseconds(SCS_HIGH_DELAY);
  SPI.transfer(vcomByte);
  SPI.transfer(0x00);
  delayMicroseconds(SCS_LOW_DELAY);
  digitalWrite(SCS, LOW);
  delayMicroseconds(10);
  SPI.endTransaction();
}


// Recommended to be used in conjunction with an interrupt service routine (see below hardToggleVCOM method for code) 
void MemoryLCD::hardToggleVCOM() {
  if(enablePWM) {
    if(++pwm_interrupt_counter > 243) {  // 243 gives 2Hz EXTCOMIN rate
      EXTCOMIN_PIN_STATE = !EXTCOMIN_PIN_STATE;
      digitalWrite(EXTCOMIN, EXTCOMIN_PIN_STATE);
      pwm_interrupt_counter = 0;
    } 
  }
}
// paste into main sketch file (See demo.ino for example of it in use)
// SIGNAL(TIMER0_COMPA_vect) 
// {
//   lcd.hardToggleVCOM();
// }

/*
 * Added these functions after I found the SD library to be somewhat tempremental...
 * Trying to play nicely with other devices on the SPI bus that might use different settings
 * The aim of these functions is to save the existing state of the SPI Control Register
 * before any function in the Memory LCD alters them, and then to restore the existing
 * SPI Control Register settings when each Memory LCD function finishes.
 * 
 * Made obsolete in Arduino 1.6.6 due to Arduino's own implementation of SPI transactions
 */


// Made obsolete in Arduino 1.6.6 due to Arduino's own implementation of SPI transactions
// Setup SPI registers for use with Memory LCD
// void MemoryLCD::spiSetup() {
//   savedSpiControlRegister = SPCR;
//   SPI.begin();
//   SPI.setBitOrder(MSBFIRST);  // remember to change to LSBFIRST for the address chars!
//   SPI.setDataMode(SPI_MODE0);
//   SPI.setClockDivider(SPI_CLOCK_DIV4);	// this is the conservative setting
//   // Datasheet says SPI clock must have <1MHz frequency (SPI_CLOCK_DIV16)
//   // but it may work at 2MHz (SPI_CLOCK_DIV8) or even 4MHz (SPI_CLOCK_DIV4)
// }


// Made obsolete in Arduino 1.6.6 due to Arduino's own implementation of SPI transactions
// Return SPI registers to state they were in before the Memory LCD library ued the SPI bus
// void MemoryLCD::spiRestore() {
//   SPCR = savedSpiControlRegister;
// }


// Code to reverse bit order of a singe 8-bit byte
// Copied from user "sth" on StackOverflow
// http://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
const uint8_t MemoryLCD::reverseBitOrder(const uint8_t b) {
   uint8_t temp = b;
   temp = (temp & 0xF0) >> 4 | (temp & 0x0F) << 4;
   temp = (temp & 0xCC) >> 2 | (temp & 0x33) << 2;
   temp = (temp & 0xAA) >> 1 | (temp & 0x55) << 1;
   return temp;
}
