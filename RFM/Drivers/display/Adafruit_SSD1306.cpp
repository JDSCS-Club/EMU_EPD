
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

//#include "glcdfont.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "math.h"               //  min()

#include "PHan_Lib.h"           //  PHan_HanFontLoad()

#if defined(PHANFONT_LOCAL)		//	한글폰트 사용시. 문자코드 변환 사용.
#include "unicode.h"            //  UTF8ToCP949()
#endif	//	defined(PHANFONT_LOCAL)

#if defined(_WIN32)

#include "x86_hal_driver.h"     //  HAL_Delay()

#else   //  STM32

#if defined(STM32F407xx)
#include "stm32f4xx.h"
#elif defined(STM32F207xx)
#include "stm32f2xx.h"
#endif

#endif

#ifndef min
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif


#define SSD1306_CMD   0x00
#define SSD1306_DATA  0x40


#define pgm_read_byte(addr) (*(const unsigned char *)(addr))


uint8_t Buffer_CMD[]  = {SSD1306_CMD, 0x00};
uint8_t Buffer_DATA[129]= {SSD1306_DATA,};

//========================================================================
static uint8_t bufMenu[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] =
//========================================================================
{
    //  128 X 8
    0x00,0x06,0x0A,0xFE,0x0A,0xE6,0x00,0xF0,0x00,0xF8,0x00,0xFC,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x48,
    0xFE,0x82,0xBA,0xBA,0x82,0xBA,0xBA,0x82,0xBA,0xBA,0x82,0xBA,0xBA,0x82,0xFE,0x00,

    //  128 X 8
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    //  128 X 8
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    //  128 X 8
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

// the memory buffer for the LCD
static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] =
{
//  128 X 8
0x00,0x06,0x0A,0xFE,0x0A,0xE6,0x00,0xF0,0x00,0xF8,0x00,0xFC,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x84,0x48,0xFE,0x32,0xB4,0x48,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x06,0x0A,0x12,0x12,0x12,0x12,
0x12,0x0A,0x06,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x86,0x02,0x78,0x84,0xA4,0x68,0x02,0x86,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x7A,0x86,0x84,0xB4,0xA4,0xA6,0x7A,0x0E,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x48,
0xFE,0x82,0xBA,0xBA,0x82,0xBA,0xBA,0x82,0xBA,0xBA,0x82,0xBA,0xBA,0x82,0xFE,0x00,

//  128 X 8
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
0xE0,0xF0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x31,0xF1,0xE1,0x01,0x01,0xE1,
0xF1,0x31,0x31,0x31,0x30,0x30,0x30,0x30,0x30,0x30,0xF0,0xE0,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x01,0x30,0x30,0x30,0x30,0x31,0x31,0x30,0x30,0x30,0x30,0x30,
0xF0,0xF0,0x00,0x00,0x30,0x30,0x30,0x30,0x31,0x31,0x30,0x30,0x30,0x31,0x31,0xF0,
0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//  128 X 8
0x40,0xC0,0xC0,0x00,0xC0,0xC0,0x40,0x00,0x40,0xC0,0x40,0x40,0xC0,0x00,0x40,0xC0,
0x00,0x00,0xC0,0x40,0x00,0x40,0xC0,0x00,0x00,0xC0,0x40,0x00,0x00,0x00,0x00,0x00,
0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,
0xFF,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0xFF,0xFF,0x00,0x00,0x00,0x0E,
0x0E,0x0E,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,
0xFF,0xFF,0x00,0x00,0xE0,0xE0,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7F,
0x7F,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x80,0x80,0x00,0x00,0x80,0x80,
0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x80,

//  128 X 8
0x20,0x3F,0x03,0x3C,0x03,0x3F,0x20,0x00,0x20,0x3F,0x22,0x27,0x30,0x00,0x20,0x3F,
0x03,0x0C,0x3F,0x20,0x00,0x00,0x1F,0x20,0x20,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,
0x3F,0x7F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7F,0x3F,0x00,0x00,0x3F,
0x7F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7F,0x3F,0x00,0x00,0x00,0x07,
0x07,0x07,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,
0x7F,0x7F,0x00,0x00,0x7F,0x7F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,
0x60,0x00,0x00,0x00,0x00,0x00,0x40,0x7F,0x04,0x04,0x7F,0x40,0x00,0x3F,0x40,0x40,
0x40,0x3F,0x00,0x40,0x7F,0x07,0x78,0x07,0x7F,0x40,0x00,0x40,0x7F,0x44,0x4E,0x61
};

#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }

#if defined(_WIN32)
#else       //  STM32

extern I2C_HandleTypeDef hi2c3;

#endif

// the most basic function, set a single pixel
void Adafruit_SSD1306::drawPixel( int16_t x, int16_t y, uint16_t color )
{
    if ( ( x < 0 ) || ( x >= width() ) || ( y < 0 ) || ( y >= height() ) )
        return;

    // check rotation, move pixel around if necessary
    switch ( getRotation() )
    {
    case 1:
        ssd1306_swap( x, y );
        x = WIDTH - x - 1;
        break;
    case 2:
        x = WIDTH - x - 1;
        y = HEIGHT - y - 1;
        break;
    case 3:
        ssd1306_swap( x, y );
        y = HEIGHT - y - 1;
        break;
    }

    // x is which column
    switch ( color )
    {
    case WHITE:   buffer[x + ( y / 8 ) * SSD1306_LCDWIDTH] |= ( 1 << ( y & 7 ) ); break;
    case BLACK:   buffer[x + ( y / 8 ) * SSD1306_LCDWIDTH] &= ~( 1 << ( y & 7 ) ); break;
    case INVERSE: buffer[x + ( y / 8 ) * SSD1306_LCDWIDTH] ^= ( 1 << ( y & 7 ) ); break;
    }
}


// initializer for I2C - we only indicate the reset pin!
Adafruit_SSD1306::Adafruit_SSD1306(int8_t reset) :
Adafruit_GFX(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT)
{
  sclk = dc = cs = sid = -1;
  rst = reset;
}


void Adafruit_SSD1306::begin(uint8_t vccstate, uint8_t i2caddr, bool reset)
{
  _vccstate = vccstate;
  _i2caddr = i2caddr;

  // Init sequence
  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80

  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_command(SSD1306_LCDHEIGHT - 1);

  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_command(0x0);                                   // no offset
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
  if (vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x10); }
  else
    { ssd1306_command(0x14); }
  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(0x00);                                  // 0x0 act like ks0108
  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  ssd1306_command(SSD1306_COMSCANDEC);

 #if defined SSD1306_128_32
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x02);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  ssd1306_command(0x8F);

#elif defined SSD1306_128_64
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x12);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x9F); }
  else
    { ssd1306_command(0xCF); }

#elif defined SSD1306_96_16
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x2);   //ada x12
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x10); }
  else
    { ssd1306_command(0xAF); }

#endif

  ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
  if (vccstate == SSD1306_EXTERNALVCC)
    { ssd1306_command(0x22); }
  else
    { ssd1306_command(0xF1); }
  ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  ssd1306_command(0x40);
  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}


void Adafruit_SSD1306::invertDisplay(uint8_t i)
{
  if (i)
  {
    ssd1306_command(SSD1306_INVERTDISPLAY);
  }
  else
  {
    ssd1306_command(SSD1306_NORMALDISPLAY);
  }
}

void Adafruit_SSD1306::ssd1306_command(uint8_t c)
{
  Buffer_CMD[1] = c;
#if defined(_WIN32)
#else   //  STM32
  while(HAL_I2C_Master_Transmit(&hi2c3,_i2caddr,Buffer_CMD,0x02,0x10) != HAL_OK)
  {

  }
#endif
}

void Adafruit_SSD1306::ssd1306_data(uint16_t a)
{
  uint8_t i;
  
  for(i = 0; i < 128 ; i++)
  {
    Buffer_DATA[i+1] = buffer[a+i];
  }

#if defined(_WIN32)
#else   //  STM32
  while(HAL_I2C_Master_Transmit(&hi2c3,_i2caddr,Buffer_DATA,129,0x10) != HAL_OK)
  {

  }
#endif
}



// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void Adafruit_SSD1306::startscrollright(uint8_t start, uint8_t stop)
{
  ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void Adafruit_SSD1306::startscrollleft(uint8_t start, uint8_t stop)
{
  ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void Adafruit_SSD1306::startscrolldiagright(uint8_t start, uint8_t stop)
{
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void Adafruit_SSD1306::startscrolldiagleft(uint8_t start, uint8_t stop)
{
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void Adafruit_SSD1306::stopscroll(void)
{
  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void Adafruit_SSD1306::dim(bool dim)
{
  uint8_t contrast;

  if (dim)
  {
    contrast = 0; // Dimmed display
  }
  else
  {
    if (_vccstate == SSD1306_EXTERNALVCC)
    {
      contrast = 0x9F;
    }
    else
    {
      contrast = 0xCF;
    }
  }
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);
}

void Adafruit_SSD1306::display(void) 
{
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);   // Column start address (0 = reset)
  ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0); // Page start address (0 = reset)
  #if SSD1306_LCDHEIGHT == 64
    ssd1306_command(7); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 32
    ssd1306_command(3); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 16
    ssd1306_command(1); // Page end address
  #endif
  
  // save I2C bitrate
#ifdef TWBR
  uint8_t twbrbackup = TWBR;
  TWBR = 12; // upgrade to 400KHz!
#endif

//I2C write
  for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i += 128) 
  {
    
    ssd1306_data(i);
  }

#ifdef TWBR
    TWBR = twbrbackup;
#endif
  
}

// clear everything
void Adafruit_SSD1306::clearDisplay(void)
{
  
  memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
}


void Adafruit_SSD1306::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  bool bSwap = false;
  switch(rotation)
  {
    case 0:
      // 0 degree rotation, do nothing
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x
      bSwap = true;
      ssd1306_swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      x -= (w-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
      bSwap = true;
      ssd1306_swap(x, y);
      y = HEIGHT - y - 1;
      y -= (w-1);
      break;
  }

  if(bSwap)
  {
    drawFastVLineInternal(x, y, w, color);
  }
  else
  {
    drawFastHLineInternal(x, y, w, color);
  }
}

void Adafruit_SSD1306::drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  // Do bounds/limit checks
  if(y < 0 || y >= HEIGHT) { return; }

  // make sure we don't try to draw below 0
  if(x < 0)
  {
    w += x;
    x = 0;
  }

  // make sure we don't go off the edge of the display
  if( (x + w) > WIDTH)
  {
    w = (WIDTH - x);
  }

  // if our width is now negative, punt
  if(w <= 0) { return; }

  // set up the pointer for  movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SSD1306_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  register uint8_t mask = 1 << (y&7);

  switch (color)
  {
  case WHITE:         while(w--) { *pBuf++ |= mask; }; break;
    case BLACK: mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
  case INVERSE:         while(w--) { *pBuf++ ^= mask; }; break;
  }
}

void Adafruit_SSD1306::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  bool bSwap = false;
  switch(rotation)
  {
    case 0:
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
      bSwap = true;
      ssd1306_swap(x, y);
      x = WIDTH - x - 1;
      x -= (h-1);
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      y -= (h-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y
      bSwap = true;
      ssd1306_swap(x, y);
      y = HEIGHT - y - 1;
      break;
  }

  if(bSwap)
  {
    drawFastHLineInternal(x, y, h, color);
  }
  else
  {
    drawFastVLineInternal(x, y, h, color);
  }
}


void Adafruit_SSD1306::drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color)
{

  // do nothing if we're off the left or right side of the screen
  if(x < 0 || x >= WIDTH) { return; }

  // make sure we don't try to draw below 0
  if(__y < 0)
  {
    // __y is negative, this will subtract enough from __h to account for __y being 0
    __h += __y;
    __y = 0;

  }

  // make sure we don't go past the height of the display
  if( (__y + __h) > HEIGHT)
  {
    __h = (HEIGHT - __y);
  }

  // if our height is now negative, punt
  if(__h <= 0)
  {
    return;
  }

  // this display doesn't need ints for coordinates, use local byte registers for faster juggling
  register uint8_t y = __y;
  register uint8_t h = __h;


  // set up the pointer for fast movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SSD1306_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  // do the first partial byte, if necessary - this requires some masking
  register uint8_t mod = (y&7);
  if(mod)
  {
    // mask off the high n bits we want to set
    mod = 8-mod;

    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    // register uint8_t mask = ~(0xFF >> (mod));
    static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    register uint8_t mask = premask[mod];

    // adjust the mask if we're not going to reach the end of this byte
    if( h < mod) 
    {
      mask &= (0XFF >> (mod-h));
    }

  switch (color)
    {
    case WHITE:   *pBuf |=  mask;  break;
    case BLACK:   *pBuf &= ~mask;  break;
    case INVERSE: *pBuf ^=  mask;  break;
    }

    // fast exit if we're done here!
    if(h<mod) { return; }

    h -= mod;

    pBuf += SSD1306_LCDWIDTH;
  }


  // write solid bytes while we can - effectively doing 8 rows at a time
  if(h >= 8)
  {
    if (color == INVERSE) 
    {          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
      do
      {
      *pBuf=~(*pBuf);

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1306_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    else {
      // store a local value to work with
      register uint8_t val = (color == WHITE) ? 255 : 0;

      do  {
        // write our value in
      *pBuf = val;

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1306_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    }

  // now do the final partial byte, if necessary
  if(h)
  {
    mod = h & 7;
    // this time we want to mask the low bits of the byte, vs the high bits we did above
    // register uint8_t mask = (1 << mod) - 1;
    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
    register uint8_t mask = postmask[mod];
    switch (color)
    {
      case WHITE:   *pBuf |=  mask;  break;
      case BLACK:   *pBuf &= ~mask;  break;
      case INVERSE: *pBuf ^=  mask;  break;
    }
  }
}

void Adafruit_SSD1306::memset(uint8_t buffer[], uint16_t start_address, uint16_t destination_address)
{
  uint16_t x;
  
  for(x = start_address; x < destination_address; x++)
  {
    buffer[x] = 0x00;
  }
}

const int bitval_r[8 * 2] = {
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x8000,
    0x4000,
    0x2000,
    0x1000,
    0x0800,
    0x0400,
    0x0200,
    0x0100,
};


void Adafruit_SSD1306::printf( const char* _format )
{
    uint16_t i, j;
    uint8_t page;

    int bIsCharHan;

    page = cursor_y / 8;

    PHAN_FONT_OBJ   FontPtr;    //  Font Render Buffer

#if defined(_WIN32) || !defined(PHANFONT_LOCAL)		//	문자코드 변환 사용 X

    const char *format;
    //	Windows에선 Multibyte문자열은 UTF-8파일이라도,
    //		자동으로 CP949로 변환되어 저장됨.
    format = _format;
#else

    //	한글폰트 사용시. 문자코드 변환 사용.
    char format[128];
    memset( (uint8_t *)format, 0, 128 );
    UTF8ToCP949( _format, format );

#endif

    ::printf( "%s - Text:%s\n", __func__, format );

    for ( i = 0; format[i] != '\0'; i++ )
    {
        //  한글체크
        if ( (uint8_t)format[i] > 0x80 )		//	문자.
        {
            bIsCharHan = 1;

            //	한글폰트
            PHan_HanFontLoad( (const uint8_t *)&format[i], &FontPtr );

            for ( j = 0; j < 16; j++ )
            {
                uint16_t	dotByte;
                int bIdx;

                //	2 Byte - 한글
                dotByte = FontPtr.FontBuffer[j * 2];
                dotByte |= ( FontPtr.FontBuffer[( j * 2 ) + 1] ) << 8;

                //  16 X 16 Font ( 한글 )
                for ( bIdx = 0; bIdx < 16; bIdx++ )
                {
                    if ( dotByte & bitval_r[bIdx] )
                    {
                        drawPixel( cursor_x + i * 8 + bIdx, cursor_y + j, 1 );
                    }
                    else
                    {
                        drawPixel( cursor_x + i * 8 + bIdx, cursor_y + j, 0 );
                    }
                }
            }
        }
        else
        {
            bIsCharHan = 0;

            //	영문폰트
            //*
            PHan_EngFontLoad( (const uint8_t*)&format[i], &FontPtr );

            for ( j = 0; j < 16; j++ )
            {
                uint16_t	dotByte;
                int bIdx;

                dotByte = FontPtr.FontBuffer[j];
                dotByte |= ( FontPtr.FontBuffer[j + 1] ) << 8;

                //  8 X 16 Font ( 영문 )
                for ( bIdx = 0; bIdx < 8; bIdx++ )
                {
                    if ( dotByte & bitval_r[bIdx] )
                    {
                        drawPixel( cursor_x + i * 8 + bIdx, cursor_y + j, 1 );
                    }
                    else
                    {
                        drawPixel( cursor_x + i * 8 + bIdx, cursor_y + j, 0 );
                    }
                }
            }

            /*/
            
            for ( j = 0; j < 8; j++ )
            {
                //  8 X 16 Font ( 영문 / 숫자 )
                buffer[cursor_x + page * 128 + i * 8 + j] = FONT_8X16[( format[i] - 32 ) * 16 + j];
                buffer[cursor_x + ( page + 1 ) * 128 + i * 8 + j] = FONT_8X16[( format[i] - 32 ) * 16 + j + 8];
            }
            //  */
        }


        if ( bIsCharHan )
        {
            //  한글인경우 2 Byte
            i++;
        }

    }
}


//========================================================================

#define OLED_RESET 0

Adafruit_SSD1306 display( OLED_RESET );
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char logo16_glcd_bmp[] =
{
0x00,0x00,0x00,0x20,0x00,0x60,0x00,0xE0,0x00,0xE0,0xEE,0xCC,0x7E,0xCE,0x7F,0xDE,
0x7F,0xFF,0x3F,0xFB,0x3B,0x33,0x11,0x27,0x00,0x0E,0x00,0x0E,0x00,0x0C,0x00,0x00
};


//========================================================================
uint8_t * GetLCDFrameBuffer( void )
//========================================================================
{
    //  LCD Frame Buffer
    return buffer;
}

int g_bInitLCD = 0;

//========================================================================
int LCDInit( void )
//========================================================================
{
    display.begin( SSD1306_SWITCHCAPVCC, 0x78 );

    g_bInitLCD = 1;

    return 0;
}

//========================================================================
int LCDCheck( void )
//========================================================================
{
    return 0;
}


//========================================================================
void LCDBattery( int nLevel )
//========================================================================
{
    if ( g_bInitLCD == false ) return;

    //  Size : 17 x 7
    //  Area : ( 110, 1 ) ~ ( 127, 7 )
    //  Level : 0 ~ 6   ( Battery )

    const uint8_t bmpRSSI[7][17] = {
        { 0x3C, 0x24, 0x7F, 0x41, 0x41, 0x41, 0x41, 0x41, 0x55, 0x49, 0x55, 0x41, 0x41, 0x41, 0x41, 0x41, 0x7F },  // -[    X    ] - 방전
        { 0x3C, 0x24, 0x7F, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x7F },  // -[         ]
        { 0x3C, 0x24, 0x7F, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x5D, 0x5D, 0x41, 0x7F },  // -[       0 ]
        { 0x3C, 0x24, 0x7F, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x5D, 0x5D, 0x41, 0x5D, 0x5D, 0x41, 0x7F },  // -[     0 0 ]
        { 0x3C, 0x24, 0x7F, 0x41, 0x41, 0x41, 0x41, 0x5D, 0x5D, 0x41, 0x5D, 0x5D, 0x41, 0x5D, 0x5D, 0x41, 0x7F },  // -[   0 0 0 ]
        { 0x3C, 0x24, 0x7F, 0x41, 0x5D, 0x5D, 0x41, 0x5D, 0x5D, 0x41, 0x5D, 0x5D, 0x41, 0x5D, 0x5D, 0x41, 0x7F },  // -[ 0 0 0 0 ]
        { 0x3C, 0x24, 0x7F, 0x41, 0x41, 0x41, 0x51, 0x49, 0x4D, 0x7F, 0x59, 0x49, 0x45, 0x41, 0x41, 0x41, 0x7F },  // -[    z    ] - 충전중
    };

    if ( nLevel > 6 ) nLevel = 6;

    display.drawBitBuf( 110, 1, bmpRSSI[nLevel], 17, 7, WHITE, BLACK );
    display.display();
}

//========================================================================
void LCDRSSI( int nLevel )
//========================================================================
{
    if ( g_bInitLCD == false ) return;

    //  Area : ( 1, 1 ) ~ ( 11, 7 )
    //  Level : 0 ~ 5   ( RSSI(수신감도) : Received Signal Strength Indicator )

    const uint8_t bmpRSSI[6][11] = {
        { 0x03, 0x05, 0x7F, 0x05, 0x03, 0x50, 0x20, 0x50, 0x00, },      // | x
        { 0x03, 0x05, 0x7F, 0x05, 0x03, 0x00, },                        // | 0
        { 0x03, 0x05, 0x7F, 0x05, 0x73, 0x00, },
        { 0x03, 0x05, 0x7F, 0x05, 0x73, 0x00, 0x78, 0x00, },
        { 0x03, 0x05, 0x7F, 0x05, 0x73, 0x00, 0x78, 0x00, 0x7C},
        { 0x03, 0x05, 0x7F, 0x05, 0x73, 0x00, 0x78, 0x00, 0x7C, 0x00, 0x7E },
    };

    if ( nLevel > 5 ) nLevel = 5;

    display.drawBitBuf( 1, 1, bmpRSSI[nLevel], 11, 7, WHITE, BLACK );
    display.display();
}


//========================================================================
void LCDLight( int bOnOff )
//========================================================================
{
    if ( g_bInitLCD == false ) return;

    //  Area : ( 1, 1 ) ~ ( 11, 7 )
    //  Level : 0(Off) / 1(On) ( Light On/Off )

    const uint8_t bmpLight[2][7] = {
        { 0x00, 0x10, 0xF0, 0xF0, 0xF0, 0x10, 0x00, },      // Light Off
        { 0x02, 0x14, 0xF0, 0xF7, 0xF0, 0x14, 0x02, },      // Light On
    };

    const uint8_t *pBmp = NULL;

    if ( bOnOff )
    {
        //  On
        pBmp = bmpLight[1];
    }
    else
    {
        //  Off
        pBmp = bmpLight[0];
    }

    display.drawBitBuf( 90, 0, pBmp, 7, 8, WHITE, BLACK );
    display.display();
}


//========================================================================
void LCDSpeaker( int nLevel )
//========================================================================
{
    if ( g_bInitLCD == false ) return;

    //  Area : ( 1, 1 ) ~ ( 11, 7 )
    //  Level : 0(Off) / 1(On) ( Light On/Off )

    //  Speaker Icon ( 11 X 9 )
    const uint8_t bmpSpk[4][22] = {
//        { 0x38, 0x38, 0x7C, 0xFE, 0x00, },                                  // Speaker Icon
        { 0x38, 0x38, 0x7C, 0xFE, 0x00, 0x28, 0x10, 0x28, 0x00,  },         // Speaker Off ( 0 )
        { 0x38, 0x38, 0x7C, 0xFE, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  },   // Speaker On ( Level 1 )
        { 0x38, 0x38, 0x7C, 0xFE, 0x00, 0x38, 0x82, 0x7C, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  },   // Speaker On ( Level 2 )
        { 0x38, 0x38, 0x7C, 0xFE, 0x00, 0x38, 0x82, 0x7C, 0x01, 0x82, 0x7C,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,  },   // Speaker On ( Level 3 )
    };

    const uint8_t *pBmp = NULL;

    if ( nLevel < 0 && nLevel > 3 ) nLevel = 0;

    //========================================================================
    pBmp = bmpSpk[nLevel];
    //========================================================================

    display.drawBitBuf( 50, 0, pBmp, 11, 9, WHITE, BLACK );
    display.display();
}


//========================================================================
void LCDMenu( void )
//========================================================================
{
    if ( g_bInitLCD == false ) return;

    memcpy( buffer, bufMenu, sizeof( bufMenu ) );
    display.display();
}


//========================================================================
void    LCDMenuUpDown       ( int nUpDown ) //  0( Off ) / 1( ▲Up ) / 2( ▼Down ) / 3( ▲Up/▼Down ) 
//========================================================================
{
    //  0( Off ) / 1( ▲Up ) / 2(       ) / 3( ▲Up   ) 
    //                           ▼Down        ▼Down 

    if ( g_bInitLCD == false ) return;

    //  Area : ( 2, 16 ) ~ ( 13, 32 )
    //  Level : 0(Off) / 1(On) ( Light On/Off )

    //  Speaker Icon ( 11 X 16 )
    const uint8_t bmpMenuUpDown[4][22] = {
        { 0x00, },                                                                      //  0 ( Off )
        { 0x40, 0x60, 0x70, 0x78, 0x7C, 0x7E, 0x7C, 0x78, 0x70, 0x60, 0x40, 0x00, },    //  1 ( ▲Up )
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
          0x02, 0x06, 0x0E, 0x1E, 0x3E, 0x7E, 0x3E, 0x1E, 0x0E, 0x06, 0x02 },           //  2 ( ▼Down )
        { 0x40, 0x60, 0x70, 0x78, 0x7C, 0x7E, 0x7C, 0x78, 0x70, 0x60, 0x40,             //  3 ( ▲Up )
          0x02, 0x06, 0x0E, 0x1E, 0x3E, 0x7E, 0x3E, 0x1E, 0x0E, 0x06, 0x02 },           //    ( ▼Down )
    };

    const uint8_t* pBmp = NULL;

    if ( nUpDown < 0 && nUpDown > 3 ) nUpDown = 0;

    pBmp = bmpMenuUpDown[nUpDown];

    display.drawBitBuf( 5, 13, pBmp, 11, 16, WHITE, BLACK );
    display.display();
}


//========================================================================
void LCDSetCursor( int x, int y )
//========================================================================
{
    if ( g_bInitLCD == false ) return;

    display.setCursor( x, y );
}

//========================================================================
void LCDPrintf( char * str )
//========================================================================
{
    if ( g_bInitLCD == false ) return;

//    display.clearDisplay();

    //  (0, 11) ~ (128, 32 )
    int16_t x, y;

    display.getCursor( &x, &y );

    display.fillRect( x, 11, 128 - x, 21, 0 );    //  Clear

//    display.setCursor( 28, 8 );
    display.printf( str );
    display.display();
}

//========================================================================
void LCDClear( void )
//========================================================================
{
    if ( g_bInitLCD == false ) return;

    display.clearDisplay();
    display.display();
}

//========================================================================
void LCDClearTitle( void )
//========================================================================
{
    //  Title 영역 Clear
    if ( g_bInitLCD == false ) return;

    display.fillRect( 0, 0, 128, 10, BLACK );
//    display.display();
}

//========================================================================
void LCDClearMain( void )
//========================================================================
{
    //  Main 영역 Clear
    if ( g_bInitLCD == false ) return;

    display.fillRect( 0, 10, 128, 22, BLACK );
//    display.display();
}

//========================================================================
void    LCDDrawRect        ( int x, int y, int w, int h, int color )  //  Clear LCD Display
//========================================================================
{
    display.fillRect( x, y, w, h, color );
    display.display();
}

//========================================================================
void testdrawline( void )
//========================================================================
{
    for ( int16_t i = 0; i < display.width(); i += 4 ) {
        display.drawLine( 0, 0, i, display.height() - 1, WHITE );
        display.display();
    }
    for ( int16_t i = 0; i < display.height(); i += 4 ) {
        display.drawLine( 0, 0, display.width() - 1, i, WHITE );
        display.display();
    }
    HAL_Delay( 250 );

    display.clearDisplay();
    for ( int16_t i = 0; i < display.width(); i += 4 ) {
        display.drawLine( 0, display.height() - 1, i, 0, WHITE );
        display.display();
    }
    for ( int16_t i = display.height() - 1; i >= 0; i -= 4 ) {
        display.drawLine( 0, display.height() - 1, display.width() - 1, i, WHITE );
        display.display();
    }
    HAL_Delay( 250 );

    display.clearDisplay();
    for ( int16_t i = display.width() - 1; i >= 0; i -= 4 ) {
        display.drawLine( display.width() - 1, display.height() - 1, i, 0, WHITE );
        display.display();
    }
    for ( int16_t i = display.height() - 1; i >= 0; i -= 4 ) {
        display.drawLine( display.width() - 1, display.height() - 1, 0, i, WHITE );
        display.display();
    }
    HAL_Delay( 250 );

    display.clearDisplay();
    for ( int16_t i = 0; i < display.height(); i += 4 ) {
        display.drawLine( display.width() - 1, 0, 0, i, WHITE );
        display.display();
    }
    for ( int16_t i = 0; i < display.width(); i += 4 ) {
        display.drawLine( display.width() - 1, 0, i, display.height() - 1, WHITE );
        display.display();
    }
    HAL_Delay( 250 );
}

//========================================================================
void testdrawrect( void )
//========================================================================
{
    for ( int16_t i = 0; i < display.height() / 2; i += 2 ) {
        display.drawRect( i, i, display.width() - 2 * i, display.height() - 2 * i, WHITE );
        display.display();
        HAL_Delay( 1 );
    }
}

//========================================================================
void testfillrect( void )
//========================================================================
{
    uint8_t color = 1;
    for ( int16_t i = 0; i < display.height() / 2; i += 3 ) {
        // alternate colors
        display.fillRect( i, i, display.width() - i * 2, display.height() - i * 2, color % 2 );
        display.display();
        HAL_Delay( 1 );
        color++;
    }
}

//========================================================================
void testdrawcircle( void )
//========================================================================
{
    for ( int16_t i = 0; i < display.height(); i += 2 ) {
        display.drawCircle( display.width() / 2, display.height() / 2, i, WHITE );
        display.display();
        HAL_Delay( 1 );
    }
}

//========================================================================
void testdrawroundrect( void )
//========================================================================
{
    for ( int16_t i = 0; i < display.height() / 2 - 2; i += 2 ) {
        display.drawRoundRect( i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, WHITE );
        display.display();
        HAL_Delay( 1 );
    }
}

//========================================================================
void testfillroundrect( void )
//========================================================================
{
    uint8_t color = WHITE;
    for ( int16_t i = 0; i < display.height() / 2 - 2; i += 2 ) {
        display.fillRoundRect( i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, color );
        if ( color == WHITE ) color = BLACK;
        else color = WHITE;
        display.display();
        HAL_Delay( 1 );
    }
}

//========================================================================
void testdrawtriangle( void )
//========================================================================
{
    for ( int16_t i = 0; i < min( display.width(), display.height() ) / 2; i += 5 ) {
        display.drawTriangle( display.width() / 2, display.height() / 2 - i,
            display.width() / 2 - i, display.height() / 2 + i,
            display.width() / 2 + i, display.height() / 2 + i, WHITE );
        display.display();
        HAL_Delay( 1 );
    }
}

//========================================================================
void testfilltriangle( void )
//========================================================================
{
    uint8_t color = WHITE;
    for ( int16_t i = min( display.width(), display.height() ) / 2; i > 0; i -= 5 ) {
        display.fillTriangle( display.width() / 2, display.height() / 2 - i,
            display.width() / 2 - i, display.height() / 2 + i,
            display.width() / 2 + i, display.height() / 2 + i, WHITE );
        if ( color == WHITE ) color = BLACK;
        else color = WHITE;
        display.display();
        HAL_Delay( 1 );
    }
}

//========================================================================
void testdrawbitmap( const uint8_t *bitmap, uint8_t w, uint8_t h )
//========================================================================
{
    uint8_t icons[NUMFLAKES][3];

    // initialize
    for ( uint8_t f = 0; f < NUMFLAKES; f++ )
    {
        icons[f][XPOS] = rand();
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = rand() + 1;
    }

    while ( 1 )
    {
        // draw each icon
        for ( uint8_t f = 0; f < NUMFLAKES; f++ )
        {
            display.drawBitmap( icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE );
        }
        display.display();
        HAL_Delay( 200 );

        // then erase it + move it
        for ( uint8_t f = 0; f < NUMFLAKES; f++ )
        {
            display.drawBitmap( icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, BLACK );
            // move it
            icons[f][YPOS] += icons[f][DELTAY];
            // if its gone, reinit
            if ( icons[f][YPOS] > display.height() )
            {
                icons[f][XPOS] = rand();
                icons[f][YPOS] = 0;
                icons[f][DELTAY] = rand();
            }
        }
    }
}


//========================================================================


//========================================================================
void OLED_Test( void )  // Adafruit_SSD1306 &display )
//========================================================================
{
    char *str1 = "가나다라";
    char *str2 = "마바사아";

    display.begin( SSD1306_SWITCHCAPVCC, 0x78 );

    //  display.clearDisplay();
    display.display();
    HAL_Delay( 1000 );


    //  while(1);
    display.clearDisplay();
    display.display();


    //Display text
    display.setTextSize( 2 );
    display.setTextColor( WHITE );
    //    display.setCursor( 28, 8 );
    display.setCursor( 0, 0 );
    display.printf( "WaveShare" );

    display.setCursor( 0, 16 );
    display.printf( "[WaveShare] 12345" );

    display.display();
    HAL_Delay( 1000 );

    display.clearDisplay();

    display.setCursor( 0, 0 );

    char *bufKor = str1;

//DEL #if defined(_WIN32)
//DEL     //	Windows에선 Multibyte문자열은 UTF-8파일이라도,
//DEL     //		자동으로 CP949로 변환되어 저장됨.
//DEL     char *bufKor = str1;
//DEL #else
//DEL     char bufKor[32];
//DEL     memset( bufKor, 0, 32 );
//DEL     UTF8ToCP949( str1, bufKor );
//DEL #endif

//    char *strName = GetNextStnName( 4, 0 );

    printf( "bufKor : " );
    for ( int i = 0; i < strlen( bufKor ); i++ )
    {
        printf( "0x%02X ", (uint8_t)bufKor[i] );
    }
    printf( "\n" );

    printf( "%s(%d) \n", __func__, __LINE__ );//, bufKor );

    display.printf( bufKor );
    display.setCursor( 0, 16 );

    bufKor = str2;

//#if defined(_WIN32)
//    //	Windows에선 Multibyte문자열은 UTF-8파일이라도,
//    //		자동으로 CP949로 변환되어 저장됨.
//    bufKor = str2;
//#else
//
//    memset( bufKor, 0, 32 );
//    UTF8ToCP949( str2, bufKor );
//    
//#endif

    printf( "bufKor : " );
    for ( int i = 0; i < strlen( bufKor ); i++ )
    {
        printf( "0x%02X ", (uint8_t)bufKor[i] );
    }
    printf( "\n" );

//    printf( "%s(%d) : %s\n", __func__, __LINE__, bufKor );
    printf( "%s(%d) \n", __func__, __LINE__ );//, bufKor );

    display.printf( bufKor );

    display.display();
    HAL_Delay( 1000 );

#if 0


    display.clearDisplay();
    display.display();

    // draw many lines
    testdrawline();
    display.display();
    HAL_Delay( 2000 );
    display.clearDisplay();

    // draw rectangles
    testdrawrect();
    display.display();
    HAL_Delay( 2000 );
    display.clearDisplay();

    // draw multiple rectangles
    testfillrect();
    display.display();
    HAL_Delay( 2000 );
    display.clearDisplay();

    // draw mulitple circles
    testdrawcircle();
    display.display();
    HAL_Delay( 2000 );
    display.clearDisplay();

    // draw a white circle, 10 pixel radius
    display.fillCircle( display.width() / 2, display.height() / 2, 10, WHITE );
    display.display();
    HAL_Delay( 2000 );
    display.clearDisplay();

    testdrawroundrect();
    HAL_Delay( 2000 );
    display.clearDisplay();

    testfillroundrect();
    HAL_Delay( 2000 );
    display.clearDisplay();

    testdrawtriangle();
    HAL_Delay( 2000 );
    display.clearDisplay();

    testfilltriangle();
    HAL_Delay( 2000 );
    display.clearDisplay();

    // miniature bitmap display
    display.clearDisplay();
    display.drawBitmap( 30, 16, logo16_glcd_bmp, 16, 16, 1 );
    display.display();
    HAL_Delay( 500 );
#endif

}


//========================================================================


