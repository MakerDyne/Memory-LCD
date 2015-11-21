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
  spiSetup();
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
  
  spiRestore();
}


void MemoryLCD::end() {
  if(enablePWM) {
    TIMSK0 |= (1 << OCIE0A); // TODO disable this bit! (this line just copied from begin())
  }
}


void MemoryLCD::displayOnLcd(const uint8_t * const data, const uint8_t lineNumber, const uint8_t numLines) {
  spiSetup();
  // this implementation writes multiple lines that are CONSECUTIVE (although they don't have to be (if this
  // funcion was appropriately modified), as an address is given for every line, not just the first in the sequence)
  // data for all lines should be stored in a single array
  const uint8_t * linePtr = data;
  SPI.setBitOrder(MSBFIRST);
  digitalWrite(SCS, HIGH);
  delayMicroseconds(SCS_HIGH_DELAY);
  SPI.transfer(0b10000000);
  for(byte x=lineNumber+1; x<lineNumber+numLines+1; x++) {
    SPI.setBitOrder(LSBFIRST);	// only the line number goes LSBFIRST for some reason
    SPI.transfer(x);
    SPI.setBitOrder(MSBFIRST);
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
  spiRestore();
}


void MemoryLCD::clearDisplay() {
  spiSetup();
  SPI.setBitOrder(MSBFIRST);
  digitalWrite(SCS, HIGH);
  delayMicroseconds(SCS_HIGH_DELAY);
  SPI.transfer(0b00100000);
  SPI.transfer(0x00);
  delayMicroseconds(SCS_LOW_DELAY);
  digitalWrite(SCS, LOW);
  delayMicroseconds(INTERFRAME_DELAY);
  spiRestore();
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
  spiSetup();
  SPI.setBitOrder(MSBFIRST);
  vcomByte ^= (0b01000000);
  digitalWrite(SCS, HIGH);
  delayMicroseconds(SCS_HIGH_DELAY);
  SPI.transfer(vcomByte);
  SPI.transfer(0x00);
  delayMicroseconds(SCS_LOW_DELAY);
  digitalWrite(SCS, LOW);
  delayMicroseconds(10);
  spiRestore();
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
 */

// Setup SPI registers for use with Memory LCD
void MemoryLCD::spiSetup() {
  savedSpiControlRegister = SPCR;
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);  // remember to change to LSBFIRST for the address chars!
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV4);	// this is the conservative setting
  // Datasheet says SPI clock must have <1MHz frequency (SPI_CLOCK_DIV16)
  // but it may work at 2MHz (SPI_CLOCK_DIV8) or even 4MHz (SPI_CLOCK_DIV4)
}


// Return SPI registers to state they were in before the Memory LCD library ued the SPI bus
void MemoryLCD::spiRestore() {
  SPCR = savedSpiControlRegister;
}