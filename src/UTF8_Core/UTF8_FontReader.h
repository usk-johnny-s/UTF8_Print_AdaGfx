#ifndef _UTF8_FONT_READER_H
#define _UTF8_FONT_READER_H

// Font bitstream reader.

class UTF8_FontReader {
public:
  void seek(uint32_t utf);
  void seek_Tofu();
  type_font_box_pos getBoxX_Max() const;
  type_font_box_pos getBoxY_Max() const;
  void getFbox(type_font_box_pos *pos)
  {
    memcpy(pos,_fbox,sizeof(_fbox));
  }
  void getMbox(type_font_box_pos *pos)
  {
    memcpy(pos,_mbox,sizeof(_mbox));
  }
  bool getBit();
  void setHalf2Full(bool enable)
  {
    enableHalf2Full = enable;
  }

private:
  bool enableHalf2Full = false;
  UTF8_BitstreamReader _glyph_bitstream;
  type_font_box_pos _fbox[4];
  type_font_box_pos _mbox[4];
  void getBox(type_font_box_pos *pos);
  uint32_t convHarf2Full(uint32_t utf);
}; // class UTF8_FontReader

#endif // _UTF8_FONT_READER_H
