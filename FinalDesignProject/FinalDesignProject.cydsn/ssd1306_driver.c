/* ========================================
 * 
 * LICENSE INFO:
 *
 * This is documentation for Adafruit's SSD1306 library for monochrome
 * OLED displays: http://www.adafruit.com/category/63_98
 *
 * These displays use I2C or SPI to communicate. I2C requires 2 pins
 * (SCL+SDA) and optionally a RESET pin. SPI requires 4 pins (MOSI, SCK,
 * select, data/command) and optionally a reset pin. Hardware SPI or
 * 'bitbang' software SPI are both supported.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a
 * href="https://github.com/adafruit/Adafruit-GFX-Library"> Adafruit_GFX</a>
 * being present on your system. Please make sure you have installed the latest
 * version before using this library.
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries, with
 * contributions from the open source community.
 *
 * @section license License
 *
 * BSD license, all text above, and the splash screen included below,
 * must be included in any redistribution.
 *
 * Modified by Josh Wild
 *
 * ========================================
*/

/*!
 */

#include "project.h"
#include "ssd1306_driver.h"
#include "stdbool.h"
#include "stdio.h"
#include "malloc.h"

#define FONT_WIDTH 5
#define FONT_HEIGHT 7

const uint8_t font[][5] = {
    // '0' 48 - Index 0
    {0x3E,0x51,0x49,0x45,0x3E},
    // '1' 49 - Index 1
    {0x00,0x42,0x7F,0x40,0x00},
    // '2' 50 - Index 2
    {0x42,0x61,0x51,0x49,0x46},
    // '3' 51 - Index 3
    {0x21,0x41,0x45,0x4B,0x31},
    // '4' 52 - Index 4
    {0x18,0x14,0x12,0x7F,0x10},
    // '5' 53 - Index 5
    {0x27,0x45,0x45,0x45,0x39},
    // '6' 54 - Index 6
    {0x3C,0x4A,0x49,0x49,0x30},
    // '7' 55 - Index 7
    {0x01,0x71,0x09,0x05,0x03},
    // '8' 56 - Index 8
    {0x36,0x49,0x49,0x49,0x36},
    // '9' 57 - Index 9
    {0x06,0x49,0x49,0x29,0x1E},

    // 'A' 65 - Index 10
    {0x7E,0x11,0x11,0x11,0x7E},
    // 'B' 66 - Index 11
    {0x7F,0x49,0x49,0x49,0x36},
    // 'C' 67 - Index 12
    {0x3E,0x41,0x41,0x41,0x22},
    // 'D' 68 - Index 13
    {0x7F,0x41,0x41,0x22,0x1C},
    // 'E' 69 - Index 14
    {0x7F,0x49,0x49,0x49,0x41},
    // 'F' 70 - Index 15
    {0x7F,0x09,0x09,0x09,0x01},
    // 'G' 71 - Index 16
    {0x3E,0x41,0x49,0x49,0x7A},
    // 'H' 72 - Index 17
    {0x7F,0x08,0x08,0x08,0x7F},
    // 'I' 73 - Index 18
    {0x00,0x41,0x7F,0x41,0x00},
    // 'J' 74 - Index 19
    {0x20,0x40,0x41,0x3F,0x01},
    // 'K' 75 - Index 20
    {0x7F,0x08,0x14,0x22,0x41},
    // 'L' 76 - Index 21
    {0x7F,0x40,0x40,0x40,0x40},
    // 'M' 77 - Index 22
    {0x7F,0x02,0x0C,0x02,0x7F},
    // 'N' 78 - Index 23
    {0x7F,0x04,0x08,0x10,0x7F},
    // 'O' 79 - Index 24
    {0x3E,0x41,0x41,0x41,0x3E},
    // 'P' 80 - Index 25
    {0x7F,0x09,0x09,0x09,0x06},
    // 'Q' 81 - Index 26
    {0x3E,0x41,0x51,0x21,0x5E},
    // 'R' 82 - Index 27
    {0x7F,0x09,0x19,0x29,0x46},
    // 'S' 83 - Index 28
    {0x46,0x49,0x49,0x49,0x31},
    // 'T' 84 - Index 29
    {0x01,0x01,0x7F,0x01,0x01},
    // 'U' 85 - Index 30
    {0x3F,0x40,0x40,0x40,0x3F},
    // 'V' 86 - Index 31
    {0x1F,0x20,0x40,0x20,0x1F},
    // 'W' 87 - Index 32
    {0x7F,0x20,0x18,0x20,0x7F},
    // 'X' 88 - Index 33
    {0x63,0x14,0x08,0x14,0x63},
    // 'Y' 89 - Index 34
    {0x03,0x04,0x78,0x04,0x03},
    // 'Z' 90 - Index 35
    {0x61,0x51,0x49,0x45,0x43},

    // 'a' 97 - Index 36
    {0x20,0x54,0x54,0x54,0x78},
    // 'b' 98 - Index 37
    {0x7F,0x48,0x44,0x44,0x38},
    // 'c' 99 - Index 38
    {0x38,0x44,0x44,0x44,0x20},
    // 'd' 100 - Index 39
    {0x38,0x44,0x44,0x48,0x7F},
    // 'e' 101 - Index 40
    {0x38,0x54,0x54,0x54,0x18},
    // 'f' 102 - Index 41
    {0x08,0x7E,0x09,0x01,0x02},
    // 'g' 103 - Index 42
    {0x0C,0x52,0x52,0x52,0x3E},
    // 'h' 104 - Index 43
    {0x7F,0x08,0x04,0x04,0x78},
    // 'i' 105 - Index 44
    {0x00,0x44,0x7D,0x40,0x00},
    // 'j' 106 - Index 45
    {0x20,0x40,0x44,0x3D,0x00},
    // 'k' 107 - Index 46
    {0x7F,0x10,0x28,0x44,0x00},
    // 'l' 108 - Index 47
    {0x00,0x41,0x7F,0x40,0x00},
    // 'm' 109 - Index 48
    {0x7C,0x04,0x18,0x04,0x78},
    // 'n' 110 - Index 49
    {0x7C,0x08,0x04,0x04,0x78},
    // 'o' 111 - Index 50
    {0x38,0x44,0x44,0x44,0x38},
    // 'p' 112 - Index 51
    {0x7C,0x14,0x14,0x14,0x08},
    // 'q' 113 - Index 52
    {0x08,0x14,0x14,0x18,0x7C},
    // 'r' 114 - Index 53
    {0x7C,0x08,0x04,0x04,0x08},
    // 's' 115 - Index 54
    {0x48,0x54,0x54,0x54,0x20},
    // 't' 116 - Index 55
    {0x04,0x3F,0x44,0x40,0x20},
    // 'u' 117 - Index 56
    {0x3C,0x40,0x40,0x20,0x7C},
    // 'v' 118 - Index 57
    {0x1C,0x20,0x40,0x20,0x1C},
    // 'w' 119 - Index 58
    {0x3C,0x40,0x30,0x40,0x3C},
    // 'x' 120 - Index 59
    {0x44,0x28,0x10,0x28,0x44},
    // 'y' 121 - Index 60
    {0x0C,0x50,0x50,0x50,0x3C},
    // 'z' 122 - Index 61
    {0x44,0x64,0x54,0x4C,0x44},

    // ' ' (space) 32 - Index 62
    {0x00,0x00,0x00,0x00,0x00},
    // '!' 33 - Index 63
    {0x00,0x00,0x5F,0x00,0x00},
    // '.' 46 - Index 64
    {0x00,0x60,0x60,0x00,0x00},
    // ',' 44 - Index 65
    {0x00,0x80,0x60,0x00,0x00},
    // ':' 58 - Index 66
    {0x00,0x36,0x36,0x00,0x00},
    // ';' 59 - Index 67
    {0x00,0x80,0x36,0x36,0x00},
    // '?' 63 - Index 68
    {0x02,0x01,0x51,0x09,0x06},
    // '-' 45 - Index 69
    {0x08,0x08,0x08,0x08,0x08},
    // '_' 95 - Index 70
    {0x80,0x80,0x80,0x80,0x80},
    // '/' 47 - Index 71
    {0x60,0x10,0x08,0x04,0x03},
    // '+' 43 - Index 72
    {0x08,0x08,0x3E,0x08,0x08},
    // '=' 61 - Index 73
    {0x14,0x14,0x14,0x14,0x14},
    // '*' 42 - Index 74
    {0x14,0x08,0x3E,0x08,0x14},
    // '%' 37 - Index 75
    {0x62,0x64,0x08,0x13,0x23},
    // '(' 40 - Index 76
    {0x00,0x1C,0x22,0x41,0x00},
    // ')' 41 - Index 77
    {0x00,0x41,0x22,0x1C,0x00},
    // '[' 91 - Index 78
    {0x00,0x7F,0x41,0x41,0x00},
    // ']' 93 - Index 79
    {0x00,0x41,0x41,0x7F,0x00},
    // '"' 34 - Index 80
    {0x03,0x00,0x03,0x00,0x00},
    // '\'' 39 - Index 81
    {0x00,0x03,0x00,0x00,0x00},
    // '#' 35 - Index 82
    {0x14,0x7F,0x14,0x7F,0x14},
    // '$' 36 - Index 83
    {0x24,0x2A,0x7F,0x2A,0x12},
    // '&' 38 - Index 84
    {0x36,0x49,0x55,0x22,0x50},
    // '@' 64 - Index 85
    {0x3E,0x41,0x5D,0x55,0x1E},
};

uint8_t buffer[SSD1306_WIDTH * (SSD1306_HEIGHT / 8)];  // Display buffer
uint8_t contrast = 0x8F;

// SOME DEFINES AND STATIC VARIABLES USED INTERNALLY -----------------------

#define pgm_read_byte(addr)                                                    \
  (*(const unsigned char *)(addr)) ///< PROGMEM workaround for non-AVR

#define ssd1306_swap(a, b)                                                     \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

// LOW-LEVEL UTILS ---------------------------------------------------------

/*!
    @brief Issue single command to SSD1306 using I2C.
    @param c the command character to send to the display. Refer to ssd1306 data sheet for commands
    @return None (void).
    @note
*/
void SSD1306_SendCommand(uint8_t cmd) {
    I2C_I2CMasterSendStart(OLED_I2C_ADDR, 0, TIMEOUT_TIME);
    I2C_I2CMasterWriteByte(I2C_COMMAND_BYTE, TIMEOUT_TIME);
    I2C_I2CMasterWriteByte(cmd, TIMEOUT_TIME);
    I2C_I2CMasterSendStop(TIMEOUT_TIME);
}

/*!
    @brief Issue list of commands to SSD1306, same rules as above.
    @param c - pointer to list of commands
    @param n - number of commands in the list
    @return None (void).
    @note
*/
void SSD1306_SendCommandList(const uint8_t *cmds, uint8_t n) {
    I2C_I2CMasterSendStart(OLED_I2C_ADDR, 0, TIMEOUT_TIME);
    I2C_I2CMasterWriteByte(I2C_COMMAND_BYTE, TIMEOUT_TIME);
    for (uint8_t i = 0; i < n; i ++) {
        I2C_I2CMasterWriteByte(cmds[i], TIMEOUT_TIME);
    }
    I2C_I2CMasterSendStop(TIMEOUT_TIME);
}

/*!
    @brief Issue array of data to SSD1306, same rules as above.
    @param data - pointer to array of data
    @param len - length of data array
    @return None (void).
    @note
*/
void SSD1306_SendData(const uint8_t *data, uint16_t len) {
    I2C_I2CMasterSendStart(OLED_I2C_ADDR, 0, TIMEOUT_TIME);
    I2C_I2CMasterWriteByte(I2C_DATA_BYTE, TIMEOUT_TIME);
    for (uint16_t i = 0; i < len; i ++) {
        I2C_I2CMasterWriteByte(data[i], TIMEOUT_TIME);
    }
    I2C_I2CMasterSendStop(TIMEOUT_TIME);
}

/*!
    @brief  Clear contents of display buffer (set all pixels to off).
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void clearDisplay(void) {
    UART_UartPutString("Clearing Display\n\r");
  memset(buffer, 0, SSD1306_WIDTH * ((SSD1306_HEIGHT + 7) / 8));
}

/*!
    @brief  Dim the display.
    @param  dim - true to enable lower brightness mode, false for full brightness.
    @return None (void).
    @note   This has an immediate effect on the display, no need to call the display() function -- buffer contents are not changed.
*/
void dim(bool dim) {
  // the range of contrast to too small to be really useful; it is useful to dim the display
  SSD1306_SendCommand(SSD1306_SETCONTRAST);
  SSD1306_SendCommand(dim ? 0 : contrast);
}

// ALLOCATE & INIT DISPLAY -------------------------------------------------

/*!
    @brief  Allocate RAM for image buffer, initialize peripherals and pins.
    @return true on successful allocation/init, false otherwise.
            Well-behaved code should check the return value before
            proceeding.
    @note   MUST call this function before any drawing or updates!
*/
bool init(void) {    
    UART_UartPutString("Starting init\n\r");

    clearDisplay();
    UART_UartPutString("Cleared Display\n\r");
    
    I2C_Start();
    CyDelay(1000);
    I2C_I2CMasterClearStatus();
    
    static const uint8_t init_cmds[] = {
        SSD1306_DISPLAYOFF,           // 0xAE
        SSD1306_SETDISPLAYCLOCKDIV, 0x80, // 0xD5, 0x80
        SSD1306_SETMULTIPLEX, 0x1F,  // 0xA8, 31 (32 rows)
        SSD1306_SETDISPLAYOFFSET, 0x00, // 0xD3, 0 offset
        SSD1306_SETSTARTLINE, // 0x40
        SSD1306_CHARGEPUMP, 0x14,    // 0x8D, enable charge pump
        SSD1306_MEMORYMODE, 0x00,    // 0x20, horizontal addressing
        SSD1306_SEGREMAP | 0x01,     // 0xA1, segment remap
        SSD1306_COMSCANDEC,           // 0xC8, COM scan dec
        SSD1306_SETCOMPINS, 0x02,    // 0xDA, COM pins config
        SSD1306_SETCONTRAST, 0x8F,   // 0x81, contrast
        SSD1306_SETPRECHARGE, 0xF1,  // 0xD9, precharge
        SSD1306_SETVCOMDETECT, 0x40, // 0xDB, VCOM detect
        SSD1306_DISPLAYALLON_RESUME,  // 0xA4
        SSD1306_NORMALDISPLAY,        // 0xA6
        SSD1306_DEACTIVATE_SCROLL,    // 0x2E
        SSD1306_DISPLAYON             // 0xAF
    };
    
    SSD1306_SendCommandList(init_cmds, sizeof(init_cmds));
    UART_UartPutString("Sent Commands\n\r");
    
    return true; // Success
}

// DRAWING FUNCTIONS -------------------------------------------------------

/*!
    @brief  Set/clear/invert a single pixel. This is also invoked by the
            Adafruit_GFX library in generating many higher-level graphics
            primitives.
    @param  x
            Column of display -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @param  color
            Pixel color, one of: SSD1306_BLACK, SSD1306_WHITE or
            SSD1306_INVERSE.
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x >= 0) && (x < SSD1306_WIDTH) && (y >= 0) && (y < SSD1306_HEIGHT)) {
        // Pixel is in-bounds.    
        switch (color) {
            case SSD1306_WHITE:
                buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y & 7));
                break;
            case SSD1306_BLACK:
                buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y & 7));
                break;
            case SSD1306_INVERSE:
                buffer[x + (y / 8) * SSD1306_WIDTH] ^= (1 << (y & 7));
                break;
        }
    }
}

/*!
    @brief  Draw a horizontal line with a width and color. Used by public
   methods drawFastHLine,drawFastVLine
        @param x
                   Leftmost column -- 0 at left to (screen width - 1) at right.
        @param y
                   Row of display -- 0 at top to (screen height -1) at bottom.
        @param w
                   Width of line, in pixels.
        @param color
               Line color, one of: SSD1306_BLACK, SSD1306_WHITE or
   SSD1306_INVERSE.
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  if ((y >= 0) && (y < SSD1306_HEIGHT)) { // Y coord in bounds?
    if (x < 0) {                  // Clip left
      w += x;
      x = 0;
    }
    if ((x + w) > SSD1306_WIDTH) { // Clip right
      w = (SSD1306_WIDTH - x);
    }
    if (w > 0) { // Proceed only if width is positive
      uint8_t *pBuf = &buffer[(y / 8) * SSD1306_WIDTH + x];
      uint8_t mask = 1 << (y & 7);
    
      switch (color) {
        case SSD1306_WHITE:
            while (w--) {
                *pBuf++ |= mask;
            };
            break;
        case SSD1306_BLACK:
            mask = ~mask;
            while (w--) {
                *pBuf++ &= mask;
            };
            break;
        case SSD1306_INVERSE:
            while (w--) {
                *pBuf++ ^= mask;
            };
            break;
      }
    }
  }
}

/*!
    @brief  Draw a vertical line with a width and color. Used by public method
   drawFastHLine,drawFastVLine
        @param x
                   Leftmost column -- 0 at left to (screen width - 1) at right.
        @param __y
                   Row of display -- 0 at top to (screen height -1) at bottom.
        @param __h height of the line in pixels
        @param color
                   Line color, one of: SSD1306_BLACK, SSD1306_WHITE or
   SSD1306_INVERSE.
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void drawFastVLine(int16_t x, int16_t __y, int16_t __h, uint16_t color) {
  if ((x >= 0) && (x < SSD1306_WIDTH)) { // X coord in bounds?
    if (__y < 0) {               // Clip top
      __h += __y;
      __y = 0;
    }
    if ((__y + __h) > SSD1306_HEIGHT) { // Clip bottom
      __h = (SSD1306_HEIGHT - __y);
    }
    if (__h > 0) { // Proceed only if height is now positive
      // this display doesn't need ints for coordinates,
      // use local byte registers for faster juggling
      uint8_t y = __y, h = __h;
      uint8_t *pBuf = &buffer[(y / 8) * SSD1306_WIDTH + x];

      // do the first partial byte, if necessary - this requires some masking
      uint8_t mod = (y & 7);
      if (mod) {
        // mask off the high n bits we want to set
        mod = 8 - mod;
        // note - lookup table results in a nearly 10% performance
        // improvement in fill* functions
        // uint8_t mask = ~(0xFF >> mod);
        static const uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};
        uint8_t mask = premask[mod];
        
        // adjust the mask if we're not going to reach the end of this byte
        if (h < mod)
          mask &= (0XFF >> (mod - h));

        switch (color) {
            case SSD1306_WHITE:
                *pBuf |= mask;
                break;
            case SSD1306_BLACK:
                *pBuf &= ~mask;
                break;
            case SSD1306_INVERSE:
                *pBuf ^= mask;
                break;
        }
        pBuf += SSD1306_WIDTH;
      }

      if (h >= mod) { // More to go?
        h -= mod;
        // Write solid bytes while we can - effectively 8 rows at a time
        if (h >= 8) {
          if (color == SSD1306_INVERSE) {
            // separate copy of the code so we don't impact performance of
            // black/white write version with an extra comparison per loop
            do {
              *pBuf ^= 0xFF; // Invert byte
              pBuf += SSD1306_WIDTH; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          } else {
            // store a local value to work with
            uint8_t val = (color != SSD1306_BLACK) ? 255 : 0;
            do {
              *pBuf = val;   // Set byte
              pBuf += SSD1306_WIDTH; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          }
        }

        if (h) { // Do the final partial byte, if necessary
          mod = h & 7;
          // this time we want to mask the low bits of the byte,
          // vs the high bits we did above
          // uint8_t mask = (1 << mod) - 1;
          // note - lookup table results in a nearly 10% performance
          // improvement in fill* functions
          static const uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F};
          uint8_t mask = postmask[mod];
          switch (color) {
            case SSD1306_WHITE:
                *pBuf |= mask;
                break;
            case SSD1306_BLACK:
                *pBuf &= ~mask;
                break;
            case SSD1306_INVERSE:
                *pBuf ^= mask;
                break;
          }
        }
      }
    } // endif positive height
  } // endif x in bounds
}

/*!
    @brief  Return color of a single pixel in display buffer.
    @param  x
            Column of display -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @return true if pixel is set (usually SSD1306_WHITE, unless display invert
   mode is enabled), false if clear (SSD1306_BLACK).
    @note   Reads from buffer contents; may not reflect current contents of
            screen if display() has not been called.
*/
bool getPixel(int16_t x, int16_t y) {
    if ((x >= 0) && (x < SSD1306_WIDTH) && (y >= 0) && (y < SSD1306_HEIGHT)) {
        return (buffer[x + (y / 8) * SSD1306_WIDTH] & (1 << (y & 7)));
    }
    return false; // Pixel out of bounds
}

void drawChar(int16_t x, int16_t y, char c, uint16_t color) {
    int font_index = -1;
    
    // Map character to font array index
    if (c >= '0' && c <= '9') {
        font_index = c - '0';  // 0-9 (indices 0-9)
    } else if (c >= 'A' && c <= 'Z') {
        font_index = 10 + (c - 'A');  // A-Z (indices 10-35)
    } else if (c >= 'a' && c <= 'z') {
        font_index = 36 + (c - 'a');  // a-z (indices 36-61)
    } else {
        // Special characters
        switch(c) {
            case ' ':  font_index = 62; break;
            case '!':  font_index = 63; break;
            case '.':  font_index = 64; break;
            case ',':  font_index = 65; break;
            case ':':  font_index = 66; break;
            case ';':  font_index = 67; break;
            case '?':  font_index = 68; break;
            case '-':  font_index = 69; break;
            case '_':  font_index = 70; break;
            case '/':  font_index = 71; break;
            case '+':  font_index = 72; break;
            case '=':  font_index = 73; break;
            case '*':  font_index = 74; break;
            case '%':  font_index = 75; break;
            case '(':  font_index = 76; break;
            case ')':  font_index = 77; break;
            case '[':  font_index = 78; break;
            case ']':  font_index = 79; break;
            case '"':  font_index = 80; break;
            case '\'': font_index = 81; break;
            case '#':  font_index = 82; break;
            case '$':  font_index = 83; break;
            case '&':  font_index = 84; break;
            case '@':  font_index = 85; break;
            default:   return;  // Unsupported character
        }
    }
    
    if (font_index < 0) return;  // Invalid character
    
    const uint8_t *bitmap = font[font_index];
    
    for (uint8_t i = 0; i < FONT_WIDTH; i++) {
        uint8_t col = bitmap[i];
        for (uint8_t j = 0; j < FONT_HEIGHT; j++) {
            if (col & (1 << j)) {
                drawPixel(x + i, y + j, color);
            } else {
                drawPixel(x + i, y + j, SSD1306_BLACK);
            }
        }
    }
}

void drawString(int16_t x, int16_t y, const char *str, uint16_t color) {
    while (*str) {
        drawChar(x, y, *str, color);
        x += FONT_WIDTH + 1;
        str++;
    }
}

/*!
    @brief  Get base address of display buffer for direct reading or writing.
    @return Pointer to an unsigned 8-bit array, column-major, columns padded
            to full byte boundary if needed.
*/
uint8_t* getBuffer(void) { return buffer; }

// REFRESH DISPLAY ---------------------------------------------------------

/*!
    @brief  Push data currently in RAM to SSD1306 display.
    @return None (void).
    @note   Drawing operations are not visible until this function is
            called. Call after each graphics command, or after a whole set
            of graphics commands, as best needed by one's own application.
*/
void refreshDisplay(void) {
    static const uint8_t dlist1[] = {
        SSD1306_PAGEADDR,
        0,                   // Page start address
        0xFF,                // Page end (not really, but works here)
        SSD1306_COLUMNADDR,
        0,
        SSD1306_WIDTH - 1
    }; // Column start address
    
    SSD1306_SendCommandList(dlist1, sizeof(dlist1));
    
    uint16_t count = SSD1306_WIDTH * ((SSD1306_HEIGHT + 7) / 8);
    uint8_t *ptr = buffer;
    
    while (count > 0) {
        uint16_t chunk = (count > I2C_CHUNK_SIZE) ? I2C_CHUNK_SIZE : count;
        
        I2C_I2CMasterSendStart(OLED_I2C_ADDR, 0, TIMEOUT_TIME);
        I2C_I2CMasterWriteByte(I2C_DATA_BYTE, TIMEOUT_TIME);
        
        for (uint16_t i = 0; i < chunk; i ++) {
            I2C_I2CMasterWriteByte(*ptr++, TIMEOUT_TIME);
        }
        
        I2C_I2CMasterSendStop(TIMEOUT_TIME);
        count -= chunk;
    }

}

/* [] END OF FILE */
