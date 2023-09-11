// This decodes UTF8 code to UTF(UTF32) code.
#include "Arduino.h"
#include "UTF8_Common.h"
#include "UTF8_Write.h"
#include "UTF8_Decode.h"

#define UTF8_CODE_LENGTH_MAX 6
static const uint32_t printFontUTF8_marker[UTF8_CODE_LENGTH_MAX - 1] PROGMEM = {
//0x00000000UL, // 1byte code: 0000 0000 0000 0000 0000 0000 0aaa aaaa
  0x00003080UL, // 2byte code: 0000 0000 0000 0000 0011 0bbb Bbaa aaaa
  0x000E2080UL, // 3byte code: 0000 0000 0000 1110 ccCc bbbb Bbaa aaaa
  0x03C82080UL, // 4byte code: 0000 0011 110d Dddd ccCc bbbb Bbaa aaaa
  0xFA082080UL, // 5byte code: 1111 10Ee dddd Ddcc ccCc bbbb Bbaa aaaa
  0x82082080UL  // 6byte code: Ffee eeEe dddd Ddcc ccCc bbbb Bbaa aaaa
};

static const uint32_t printFontUTF8_overlong[UTF8_CODE_LENGTH_MAX - 1] PROGMEM = {
//0x00000000UL, // 1byte code: 0000 0000 0000 0000 0000 0000 0aaa aaaa
  0xFFFFFFC0UL, // 2byte code: 0000 0000 0000 0000 0011 0bbb Bbaa aaaa
  0xFFFFF000UL, // 3byte code: 0000 0000 0000 1110 ccCc bbbb Bbaa aaaa
  0xFFFC0000UL, // 4byte code: 0000 0011 110d Dddd ccCc bbbb Bbaa aaaa
  0xFF000000UL, // 5byte code: 1111 10Ee dddd Ddcc ccCc bbbb Bbaa aaaa
  0xC0000000UL  // 6byte code: Ffee eeEe dddd Ddcc ccCc bbbb Bbaa aaaa
};

// input UTF8 code strem
size_t UTF8_Decode::write(uint8_t in_c) {
  if ((0x80 > in_c)||(0xC0 <= in_c)) {
    if (0 != utf8_code_count) {
      // Detect illegal code sequence.
      if (nullptr != pUTF8_Write) (void)(pUTF8_Write->illegalCode)();
    }
    utf8_code_count = 0;
  }
  if (0 == utf8_code_count) {  // Get code length by 1st byte.
    utf8_code_length = 
    (0x80 > in_c)?1:((0xC0 <= in_c)&&(0xE0 > in_c))?2:((0xE0 <= in_c)&&(0xF0 > in_c))?3:((0xF0 <= in_c)&&(0xF8 > in_c))?4:((0xF8 <= in_c)&&(0xFC > in_c))?5:((0xFC <= in_c)&&(0xFE > in_c))?6:0;
    utf32_buf = 0;
  }
  if (0 == utf8_code_length) {
    // Detect non UTF-8 code. (0xFE or 0xFF)
    if (nullptr != pUTF8_Write) (void)(pUTF8_Write->illegalCode)();
    utf8_code_count = 0;
  } else if (1 == utf8_code_length) {
    // Put 7bit character.
    if (nullptr != pUTF8_Write) (void)(pUTF8_Write->writeUTF)((uint32_t)in_c);
    utf8_code_count = 0;
  } else {
    utf32_buf = (utf32_buf << 6);
    utf32_buf ^= in_c;
    utf8_code_count++;
    if (utf8_code_length == utf8_code_count) {
      utf32_buf ^= pgm_read_dword(printFontUTF8_marker + (utf8_code_length - 2));  // Flip marker bits
      if ( 0 == (utf32_buf & pgm_read_dword(printFontUTF8_overlong + (utf8_code_length - 2)))) {
        // Detect overlong coding.
        if (nullptr != pUTF8_Write) (void)(pUTF8_Write->illegalCode)();
      } else {
        // Put UTF-32 Character
        if (nullptr != pUTF8_Write) (void)(pUTF8_Write->writeUTF)(utf32_buf);
      }
      utf8_code_count = 0;
    }
  }
  return 1;
}

// flush decoder
void UTF8_Decode::flush() {
  if (0 != utf8_code_count) {
    if (nullptr != pUTF8_Write) (void)(pUTF8_Write->illegalCode)();
  }
  utf8_code_count = 0;
}
