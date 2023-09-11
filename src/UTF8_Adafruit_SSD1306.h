#ifndef _UTF8_Adafruit_SSD1306_H_
#define _UTF8_Adafruit_SSD1306_H_

// Inject UTF8 print support to Adafruit_SSD1306.
// Ref: https://github.com/adafruit/Adafruit_SSD1306

#include "Arduino.h"
#include <Adafruit_SSD1306.h>

#include "UTF8_Core/UTF8_Common.h"
#include "UTF8_Core/UTF8_FontDataType.h"
#include "UTF8_Core/UTF8_Write.h"
#include "UTF8_Core/UTF8_Decode.h"
#include "UTF8_Core/UTF8_BitstreamReader.h"
#include "UTF8_Core/UTF8_FontReader.h"

class UTF8_Adafruit_SSD1306 : public Adafruit_SSD1306, private UTF8_Write {
#include "UTF8_Core/UTF8_InjectCore.h"
public:
  // Redefine the constructor with the same parameters as Adafruit_SSD1306

  // NEW CONSTRUCTORS -- recommended for new projects
  UTF8_Adafruit_SSD1306(uint8_t w, uint8_t h, TwoWire *twi = &Wire,
                   int8_t rst_pin = -1, uint32_t clkDuring = 400000UL,
                   uint32_t clkAfter = 100000UL)
    : Adafruit_SSD1306(w, h, twi, rst_pin, clkDuring, clkAfter)
    , UTF8_ExtensionCore_Constructor_Initializer
    {};
  UTF8_Adafruit_SSD1306(uint8_t w, uint8_t h, int8_t mosi_pin, int8_t sclk_pin,
                   int8_t dc_pin, int8_t rst_pin, int8_t cs_pin)
    : Adafruit_SSD1306(w, h, mosi_pin, sclk_pin, dc_pin, rst_pin, cs_pin)
    , UTF8_ExtensionCore_Constructor_Initializer
    {};
  UTF8_Adafruit_SSD1306(uint8_t w, uint8_t h, SPIClass *spi, int8_t dc_pin,
                   int8_t rst_pin, int8_t cs_pin, uint32_t bitrate = 8000000UL)
    : Adafruit_SSD1306(w, h, spi, dc_pin, rst_pin, cs_pin, bitrate)
    , UTF8_ExtensionCore_Constructor_Initializer
    {};

  // DEPRECATED CONSTRUCTORS - for back compatibility, avoid in new projects
  UTF8_Adafruit_SSD1306(int8_t mosi_pin, int8_t sclk_pin, int8_t dc_pin,
                   int8_t rst_pin, int8_t cs_pin)
    : Adafruit_SSD1306(mosi_pin, sclk_pin, dc_pin, rst_pin, cs_pin)
    , UTF8_ExtensionCore_Constructor_Initializer
    {};
  UTF8_Adafruit_SSD1306(int8_t dc_pin, int8_t rst_pin, int8_t cs_pin)
    : Adafruit_SSD1306(dc_pin, rst_pin, cs_pin)
    , UTF8_ExtensionCore_Constructor_Initializer
    {};
  UTF8_Adafruit_SSD1306(int8_t rst_pin = -1)
    : Adafruit_SSD1306(rst_pin)
    , UTF8_ExtensionCore_Constructor_Initializer
    {};

}; // class UTF8_Adafruit_SSD1306

#endif // _UTF8_Adafruit_SSD1306_H_
