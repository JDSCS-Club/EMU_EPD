/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#ifndef _Adafruit_SSD1306_H_
#define _Adafruit_SSD1306_H_


//#ifdef __cplusplus
// extern "C" {
//#endif
   
#include <stdint.h>     //  int16_t, uint16_t, ...

//#include "stm32f1xx_hal.h"

//#include "i2c.h"
   
//#ifdef __cplusplus
//}
//#endif

#if defined(__SAM3X8E__)
 typedef volatile RwReg PortReg;
 typedef uint32_t PortMask;
 #define HAVE_PORTREG
#elif defined(ARDUINO_ARCH_SAMD)
// not supported
#elif defined(ESP8266) || defined(ESP32) || defined(ARDUINO_STM32_FEATHER) || defined(__arc__)
  typedef volatile uint32_t PortReg;
  typedef uint32_t PortMask;
#elif defined(__AVR__)
  typedef volatile uint8_t PortReg;
  typedef uint8_t PortMask;
  #define HAVE_PORTREG
#else
  // chances are its 32 bit so assume that
  typedef volatile uint32_t PortReg;
  typedef uint32_t PortMask;
#endif

#ifdef __cplusplus

#include <Adafruit_GFX.h>

#endif

#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define SSD1306_I2C_ADDRESS   0x3C<<1 // 011110+SA0+RW - 0x3C or 0x3D
// Address for 128x32 is 0x3C
// Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)

/*=========================================================================
    SSD1306 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
    Select the appropriate display below to create an appropriately
    sized framebuffer, etc.

    SSD1306_128_64  128x64 pixel display

    SSD1306_128_32  128x32 pixel display

    SSD1306_96_16

    -----------------------------------------------------------------------*/
//   #define SSD1306_128_64
   #define SSD1306_128_32
//   #define SSD1306_96_16
/*=========================================================================*/

#if defined SSD1306_128_64 && defined SSD1306_128_32
  #error "Only one SSD1306 display can be specified at once in SSD1306.h"
#endif
#if !defined SSD1306_128_64 && !defined SSD1306_128_32 && !defined SSD1306_96_16
  #error "At least one SSD1306 display must be specified in SSD1306.h"
#endif

#if defined SSD1306_128_64
  #define SSD1306_LCDWIDTH                  128
  #define SSD1306_LCDHEIGHT                 64
#endif
#if defined SSD1306_128_32
  #define SSD1306_LCDWIDTH                  128
  #define SSD1306_LCDHEIGHT                 32
#endif
#if defined SSD1306_96_16
  #define SSD1306_LCDWIDTH                  96
  #define SSD1306_LCDHEIGHT                 16
#endif

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

#ifdef __cplusplus

class Adafruit_SSD1306 : public Adafruit_GFX {
 public:
  Adafruit_SSD1306(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS);
  Adafruit_SSD1306(int8_t DC, int8_t RST, int8_t CS);
  Adafruit_SSD1306(int8_t RST = -1);

  void begin(uint8_t switchvcc = SSD1306_SWITCHCAPVCC, uint8_t i2caddr = SSD1306_I2C_ADDRESS, bool reset=true);
  void ssd1306_command(uint8_t c);
  void ssd1306_data(uint16_t a);

  void clearDisplay(void);
  void invertDisplay(uint8_t i);
  void display();

  void startscrollright(uint8_t start, uint8_t stop);
  void startscrollleft(uint8_t start, uint8_t stop);

  void startscrolldiagright(uint8_t start, uint8_t stop);
  void startscrolldiagleft(uint8_t start, uint8_t stop);
  void stopscroll(void);

  void dim(bool dim);

  virtual void drawPixel(int16_t x, int16_t y, uint16_t color);

  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

  void debug( int bEnable ){ dbg = bEnable; }
  void printf(const char* format);
  
 private:
  int8_t _i2caddr, _vccstate, sid, sclk, dc, rst, cs;
  int8_t dbg;
  void fastSPIwrite(uint8_t c);
  void memset(uint8_t buffer[], uint16_t start_address, uint16_t length);

#if defined(_WIN32)
  inline void drawFastVLineInternal( int16_t x, int16_t y, int16_t h, uint16_t color );
  inline void drawFastHLineInternal( int16_t x, int16_t y, int16_t w, uint16_t color );
#else
  inline void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
  inline void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));
#endif

};

#endif

//========================================================================

#ifdef __cplusplus
extern "C" {
#endif

uint8_t     *GetLCDFrameBuffer( void );

extern int g_bInitLCD;

int     LCDInit             ( void );

int     LCDCheck            ( void );

void    LCDBattery          ( int nLevel ); //  0 ~ 6   ( Battery ) - 0:x / 6:충전중
void    LCDRSSI             ( int nLevel ); //  0 ~ 5   ( RSSI(수신감도) : Received Signal Strength Indicator )
void    LCDLight            ( int bOnOff ); //  0(Off) / 1(On) ( Light On/Off )
void    LCDSpeaker          ( int nLevel ); //  0(Off) / 1(On) ( Speaker On/Off )

void    LCDMenu             ( void );       //  LCD Menu

void    LCDMenuUpDown       ( int nUpDown ); //  0( Off ) / 1( ▲Up ) / 2( ▼Down ) / 3( ▲Up/▼Down ) 

void    LCDSetCursor        ( int x, int y );  		//  Setting LCD Cursor

void    LCDEnableDebug		( int bEnable );  		//  print LCD Display

void    LCDPrintf           ( char *str );  		//  print LCD Display
void    LCDPrintfXY         ( int x, int y, char *str );  //  print LCD Display

void    LCDClear            ( void );  //  Clear LCD Display
void    LCDClearTitle       ( void );
void    LCDClearMain        ( void );


void    LCDDrawRect         ( int x, int y, int w, int h, int color );  //  Clear LCD Display

void    testdrawline        ( void );
void    testdrawrect        ( void );
void    testfillrect        ( void );
void    testdrawcircle      ( void );
void    testdrawroundrect   ( void );
void    testfillroundrect   ( void );
void    testdrawtriangle    ( void );
void    testfilltriangle    ( void );
void    testdrawbitmap      ( const uint8_t *bitmap, uint8_t w, uint8_t h );


void    OLED_Test           ( void );   // Adafruit_SSD1306 &display );


#ifdef __cplusplus
}
#endif

//========================================================================

#endif /* _Adafruit_SSD1306_H_ */
