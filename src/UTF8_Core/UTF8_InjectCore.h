// This is the core of the UTF8 extension of the Adafruit_GFX library.
// This is included and used from the wrapper class.
//  (This cannot compile and execut alone.)

#define UTF8_ExtensionCore_Constructor_Initializer \
    _UTF8_Decode(this)

private:
  UTF8_Decode _UTF8_Decode;
  UTF8_FontReader _UTF8_FontReader;

  // drawChar/writeChar is for drawing font-bitstream.
  void drawChar(int16_t x, int16_t y, UTF8_FontReader &rUTF8_FontReader, uint16_t color,
                uint16_t bg, uint8_t size_x, uint8_t size_y)
  {
      type_font_box_pos fbox[4];
      type_font_box_pos mbox[4];
      rUTF8_FontReader.getFbox(fbox);
      rUTF8_FontReader.getMbox(mbox);
      startWrite();
      for (type_font_box_pos fy = fbox[1];fy < fbox[3];fy++) {
        for (type_font_box_pos fx = fbox[0];fx < fbox[2];fx++) {
          bool maskBit = false;
          if ((fx >= mbox[0])&&(fx < mbox[2])&&(fy >= mbox[1])&&(fy < mbox[3])) {
            // Get maskBit when iniside of mbox
            maskBit = rUTF8_FontReader.getBit();
          }
          if (maskBit) {
            if (size_x == 1 && size_y == 1) { // Not scaled (default size)
              writePixel(x + fx, y + fy, color);
            } else { // Scaled
              writeFillRect(x + fx * size_x, y + fy * size_y, size_x, size_y, color);
            }
          } else if (bg != color) {
            if (size_x == 1 && size_y == 1) { // Not scaled (default size)
              writePixel(x + fx, y + fy, bg);
            } else { // Scaled
              writeFillRect(x + fx * size_x, y + fy * size_y, size_x, size_y, bg);
            }
          }
        }
      }
      endWrite();
  }
  void writeChar(UTF8_FontReader &rUTF8_FontReader)
  {
      type_font_box_pos fbox[4];
      _UTF8_FontReader.getFbox(fbox);
      if (wrap && ((cursor_x + textsize_x * fbox[2]) > _width)) {
        cursor_x = 0;
        cursor_y += (int16_t)textsize_y * _UTF8_FontReader.getBoxY_Max();
      }
      drawChar(cursor_x, cursor_y, rUTF8_FontReader, textcolor, textbgcolor, textsize_x, textsize_y);
      cursor_x += (int16_t)textsize_x * fbox[2];
  }

  // UTF8_Write interface is for callback for Decoder.
  using UTF8_Write::writeUTF;
  virtual size_t writeUTF(uint32_t utf) {
    if (utf == (uint32_t)'\n') {
      cursor_x = 0;
      cursor_y += (int16_t)textsize_y * _UTF8_FontReader.getBoxY_Max();
    } else if (utf != (uint32_t)'\r') {
      _UTF8_FontReader.seek(utf);
      writeChar(_UTF8_FontReader);
    }
    return 1;
  }

  using UTF8_Write::illegalCode;
  virtual size_t illegalCode() {
    _UTF8_FontReader.seek_Tofu();
    writeChar(_UTF8_FontReader);
    return 1;
  }

public:
  // setHalf2Full is set enable / disable Hankaku to Zenkaku conversion.
  void setHalf2Full(bool enable)
  {
      _UTF8_FontReader.setHalf2Full(enable);
  }

  // drawChar(unsigned char c) inherited via Adafrut_GFX is delegated to drawChar(uint32 utf)
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg, uint8_t size)
  {
    drawChar(x, y, (uint32_t)c, color, bg, size, size);
  }
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg, uint8_t size_x, uint8_t size_y)
  {
    drawChar(x, y, (uint32_t) c, color, bg, size_x, size_y);
  }
  void drawChar(int16_t x, int16_t y, uint32_t utf, uint16_t color,
                uint16_t bg, uint8_t size)
  {
      drawChar(x, y, utf, color, bg, size, size);
  }
  void drawChar(int16_t x, int16_t y, uint32_t utf, uint16_t color,
                uint16_t bg, uint8_t size_x, uint8_t size_y)
  {
      _UTF8_FontReader.seek(utf);
      drawChar(x, y, _UTF8_FontReader, color, bg, size_x, size_y);
  }

    // Print::write() and Print::flush() inherited via Adafrut_GFX are delegated to UTF8_Decode::write() and UTF8_Decode::flush()
  using Print::write;
  virtual size_t write(uint8_t c)
  {
    _UTF8_Decode.write(c);
  }
  using Print::flush;
  virtual void flush()
  {
    _UTF8_Decode.flush();
  }