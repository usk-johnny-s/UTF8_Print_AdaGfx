#ifndef _UTF8_BITSTREAM_READER_H
#define _UTF8_BITSTREAM_READER_H

// Bitstream Reader.

class UTF8_BitstreamReader {
public:
  UTF8_BitstreamReader()
  : bitstream_bytearray_pos(nullptr)
  , readbuf(0)
  , readbuf_bits(0)
  {
  };
  UTF8_BitstreamReader(const uint8_t* in_bitstream_bytearray_pos)
  : bitstream_bytearray_pos(in_bitstream_bytearray_pos)
  , readbuf(0)
  , readbuf_bits(0)
  {
  };
  UTF8_BitstreamReader(const uint8_t* in_bitstream_bytearray_pos,const uint32_t bitstream_bit_pos)
  : bitstream_bytearray_pos(in_bitstream_bytearray_pos)
  , readbuf(0)
  , readbuf_bits(0)
  {
    uint32_t byte_pos = bitstream_bit_pos >> 3;
    uint32_t bit_pos = bitstream_bit_pos & 0x07;
    bitstream_bytearray_pos = in_bitstream_bytearray_pos + byte_pos;
    if (0 != bit_pos) {
      (void)readbits_u8(bit_pos);
    }
  };
  uint8_t readbits_u8(uint8_t bits);
  uint16_t readbits_u16(uint8_t bits);
  uint32_t readbits_u32(uint8_t bits);
  uint8_t skipToZeroBit();

private:
  uint8_t* bitstream_bytearray_pos;
  uint8_t readbuf;
  uint8_t readbuf_bits;
}; // class UTF8_BitstreamReader

#endif // _UTF8_BITSTREAM_READER_H
