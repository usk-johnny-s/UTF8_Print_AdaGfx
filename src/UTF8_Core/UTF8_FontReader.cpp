// Font bitstream reader.

#include "Arduino.h"
#include "UTF8_Common.h"
#include "UTF8_FontDataType.h"
#include "UTF8_BitstreamReader.h"
#include "UTF8_FontReader.h"
#include "UTF8_FontData.h"

void UTF8_FontReader::seek(uint32_t utf)
{
  if (enableHalf2Full) {
    utf = convHarf2Full(utf);
  }
  type_font_glyphbit_pos glyphbit_pos = 0;
  if (utf <= 0xffff) {
    // This program supports only 16bit character code range.
    type_font_charatable_pos charatable_pos = 0;
    type_font_charatable_pos numchara;
    type_font_glyphbit_pos glyph_length;

    // scan font_entrylist_bitstream
    UTF8_BitstreamReader entrylist(font_entrylist_bitstream);
    for (type_font_num_entrylist entrylist_ix = 0; entrylist_ix < FONT_NUM_ENTRYLIST; entrylist_ix++) {
      numchara = entrylist.readbits_u16(FONT_BITS_NUMCHARA);
      glyph_length = entrylist.readbits_u8(FONT_BITS_GLYPTHLENGTH);
      // scan font_charatable
      for (type_font_charatable_pos charatable_ix = 0; charatable_ix < numchara; charatable_ix++) {
          uint16_t readchara = pgm_read_word(font_charatable+charatable_pos+charatable_ix);
          if (utf == readchara) {
            // character code found in font_charatable
            glyphbit_pos += charatable_ix * glyph_length;
            _glyph_bitstream = UTF8_BitstreamReader(font_glyph_bitstream,glyphbit_pos);
            getBox(_fbox);
            getBox(_mbox);
            return;
          }
      }
      charatable_pos += numchara;
      glyphbit_pos += numchara * glyph_length;
    }
  }
  // print TOFU if character code not in font data.
  seek_Tofu();
}

void UTF8_FontReader::seek_Tofu()
{
  type_font_glyphbit_pos glyphbit_pos = 0;
  glyphbit_pos = font_tofu_glyphbit_pos;
  _glyph_bitstream = UTF8_BitstreamReader(font_glyph_bitstream,glyphbit_pos);
  getBox(_fbox);
  getBox(_mbox);
}

type_font_box_pos UTF8_FontReader::getBoxX_Max() const {
  return FONT_BOX_X_MAX;
};

type_font_box_pos UTF8_FontReader::getBoxY_Max() const {
  return FONT_BOX_Y_MAX;
};

void UTF8_FontReader::getBox(type_font_box_pos *pos)
{
  uint8_t skiplen = _glyph_bitstream.skipToZeroBit();
  uint8_t value_offset = 0;
  for (uint8_t ix = 0; ix < skiplen; ix ++) {
    uint8_t t_bitlen = pgm_read_byte(font_boxlist_id_bitlen+ix);
    value_offset += (1 << t_bitlen);
  }
  uint8_t bitlen = pgm_read_byte(font_boxlist_id_bitlen+skiplen);
  uint8_t value_bits = _glyph_bitstream.readbits_u8(bitlen);
  uint32_t value = (uint16_t)value_offset + (uint16_t)value_bits;
  UTF8_BitstreamReader _boxtable_bitstream(font_boxtable_bitstream,value * FONT_BITS_BOXVALUE * 4);
  pos[0] = _boxtable_bitstream.readbits_u8(FONT_BITS_BOXVALUE);
  pos[1] = _boxtable_bitstream.readbits_u8(FONT_BITS_BOXVALUE);
  pos[2] = _boxtable_bitstream.readbits_u8(FONT_BITS_BOXVALUE);
  pos[3] = _boxtable_bitstream.readbits_u8(FONT_BITS_BOXVALUE);
}

bool UTF8_FontReader::getBit()
{
  uint8_t value_bit = _glyph_bitstream.readbits_u8(1);
  return (0 != value_bit);
}

static const uint8_t _ConvKataknaLowByte[] PROGMEM = {
   0x02,0x0C,0x0D,0x01,0xFB,0xF2,0xA1,0xA3,0xA5,0xA7,0xA9,0xE3,0xE5,0xE7,0xC3,0xFC,
   0xA2,0xA4,0xA6,0xA8,0xAA,0xAB,0xAD,0xAF,0xB1,0xB3,0xB5,0xB7,0xB9,0xBB,0xBD,0xBF,
   0xC1,0xC4,0xC6,0xC8,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD2,0xD5,0xD8,0xDB,0xDE,0xDF,
   0xE0,0xE1,0xE2,0xE4,0xE6,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEF,0xF3,0x9B,0x9C  
};

uint32_t UTF8_FontReader::convHarf2Full(uint32_t utf)
{
  if (0x0000FF9F >= utf) { // 半角絞り込み
    uint16_t utf16 = (uint16_t)utf;
    if (0xFF61 <= utf16) { // カタカナ(FF61-FF9F)
      return 0x00003000UL | pgm_read_byte(_ConvKataknaLowByte - 0xFF61 + utf16);
    } else if (0x007E >= utf16) {
      uint8_t utf8 = (uint8_t)utf;
      if (0x20 <= utf8) { // 英数記号(20-7E)
        switch(utf8) {
          case 0x20:
            return 0x00003000UL;  // スペース
        }
        return (0x0000FF01UL - 0x00000021UL) + utf8;
      }
    } else if (0x00A5 == utf16) { // 円記号(A5)
        return 0x0000FFE5UL;
    }
  }
  return utf;
}

