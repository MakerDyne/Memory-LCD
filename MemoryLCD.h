#ifndef _MEMORY_LCD_LIB_H
#define _MEMORY_LCD_LIB_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

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
    MemoryLCD(const unsigned char SCSpin, const unsigned char SIpin, const unsigned char SCLKpin, const unsigned char DISPpin, const unsigned char EXTCOMINpin, const boolean useEXTCOMIN);
    void begin();
    void end();
    // Write data direct to screen
    void displayOnLcd(const char * data, const unsigned char lineNumber, const unsigned char numLines);
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
    char commandByte;
    char vcomByte;
    const char DISP;
    const char SCS;
    const char SI;
    const char SCLK;
    const char EXTCOMIN;
    const boolean enablePWM;
    boolean EXTCOMIN_PIN_STATE;
    unsigned int pwm_interrupt_counter;
    char lineBuffer[LCDWIDTH/8];
    char savedSpiControlRegister; // AVR SPI regsiter handling
    // member functions
    void spiSetup();
    void spiRestore();
    
};

#endif