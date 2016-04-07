#ifndef _MEMORY_LCD_LIB_H
#define _MEMORY_LCD_LIB_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <SPI.h>

// Memory LCD pixel dimensions - ALTER ACCORDING TO YOUR PARTICULAR LCD MODEL
#define LCDWIDTH		(128)
#define LCDHEIGHT		(128)

// Delay constants for LCD timing   // (Datasheet values)
#define PWRUP_DISP_DELAY	40  // (>30us)
#define PWRUP_EXTCOMIN_DELAY	40  // (>30us)
#define SCS_HIGH_DELAY		3   // (>3us)
#define SCS_LOW_DELAY		1   // (>1us)
#define INTERFRAME_DELAY	1   // (>1us)

class MemoryLCD {
  public:
    enum displayType {
      mono = 1,
      colour = 3,
      color = 3
    };
    MemoryLCD(const uint8_t SCSpin, const uint8_t SIpin, const uint8_t SCLKpin, const uint8_t DISPpin, const uint8_t EXTCOMINpin, const boolean useEXTCOMIN, displayType d);
    void begin();
    void end();
    // Write data direct to screen
    void displayOnLcd(const uint8_t * const data, const uint8_t lineNumber, const uint8_t numLines = 1);
    // clear functions
    void clearDisplay();
    // turn display on/off
    void turnOff();
    void turnOn();
    // return display parameters
    int getLcdWidth() const;
    int getLcdHeight() const;
    // VCOM control - NOT YET PROPERLY IMPLEMENTED
    void softToggleVCOM();
    void hardToggleVCOM();
  private:
    // variables
    uint8_t commandByte;
    uint8_t vcomByte;
    const uint8_t DISP;
    const uint8_t SCS;
    const uint8_t SI;
    const uint8_t SCLK;
    const uint8_t EXTCOMIN;
    const byte colourDepth;
    const boolean enablePWM;
    boolean EXTCOMIN_PIN_STATE;
    unsigned int pwm_interrupt_counter;
    uint8_t lineBuffer[LCDWIDTH/8];
    uint8_t savedSpiControlRegister; // AVR SPI regsiter handling
    SPISettings lcdSpiSettings;
    // member functions
    const uint8_t reverseBitOrder(const uint8_t b);
};

#endif
