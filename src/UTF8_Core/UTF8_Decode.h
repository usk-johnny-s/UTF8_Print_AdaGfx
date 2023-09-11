#ifndef _UTF8_DECODE_H
#define _UTF8_DECODE_H
// This decodes UTF8 code to UTF(UTF32) code.

class UTF8_Decode {
public:
  // UTF8_Decode will call back _pUTF8_Write as each UTF code is decoded.
  UTF8_Decode(UTF8_Write *_pUTF8_Write = nullptr)
    : utf8_code_count(0)
    , utf8_code_length(0)
    , utf32_buf(0)
    , pUTF8_Write(_pUTF8_Write)
    {};

  // input UTF8 code strem
  size_t write(uint8_t);
  // flush decoder
  void flush();

private:
  uint32_t utf32_buf;
  uint8_t utf8_code_count;
  uint8_t utf8_code_length;
  UTF8_Write *pUTF8_Write;
}; // class UTF8_Decode

#endif // _UTF8_DECODE_H
