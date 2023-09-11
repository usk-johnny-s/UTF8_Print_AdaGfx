// Bitstream Reader.

#include "Arduino.h"
#include "UTF8_Common.h"
#include "UTF8_BitstreamReader.h"

uint8_t UTF8_BitstreamReader::readbits_u8(uint8_t bits) {
  if (nullptr == bitstream_bytearray_pos) return 0;
  uint8_t ret = 0;
  uint8_t ret_bits = 0;
  while(ret_bits < bits) {
    if ((0==readbuf_bits)&&(ret_bits<bits)) {
      // Read new byte from bitstream
      readbuf = pgm_read_byte(bitstream_bytearray_pos++);
      readbuf_bits = 8;
    }
    if ((bits-ret_bits) >= readbuf_bits) {
      ret |= ((uint8_t)((readbuf>>(8-readbuf_bits))&((1<<(readbuf_bits))-1))<<(ret_bits));
      ret_bits += readbuf_bits;
      readbuf_bits = 0;
    } else {
      ret |= ((uint8_t)((readbuf>>(8-readbuf_bits))&((1<<(bits-ret_bits))-1))<<(ret_bits));
      readbuf_bits -= bits-ret_bits;
      ret_bits = bits;
    }
  }
  return ret;
}

uint16_t UTF8_BitstreamReader::readbits_u16(uint8_t bits) {
  if (nullptr == bitstream_bytearray_pos) return 0;
  uint16_t ret = 0;
  uint8_t ret_bits = 0;
  while(ret_bits < bits) {
    if ((0==readbuf_bits)&&(ret_bits<bits)) {
      // Read new byte from bitstream
      readbuf = pgm_read_byte(bitstream_bytearray_pos++);
      readbuf_bits = 8;
    }
    if ((bits-ret_bits) >= readbuf_bits) {
      ret |= ((uint16_t)((readbuf>>(8-readbuf_bits))&((1<<(readbuf_bits))-1))<<(ret_bits));
      ret_bits += readbuf_bits;
      readbuf_bits = 0;
    } else {
      ret |= ((uint16_t)((readbuf>>(8-readbuf_bits))&((1<<(bits-ret_bits))-1))<<(ret_bits));
      readbuf_bits -= bits-ret_bits;
      ret_bits = bits;
    }
  }
  return ret;
}

uint32_t UTF8_BitstreamReader::readbits_u32(uint8_t bits) {
  if (nullptr == bitstream_bytearray_pos) return 0;
  uint32_t ret = 0;
  uint8_t ret_bits = 0;
  while(ret_bits < bits) {
    if ((0==readbuf_bits)&&(ret_bits<bits)) {
      // Read new byte from bitstream
      readbuf = pgm_read_byte(bitstream_bytearray_pos++);
      readbuf_bits = 8;
    }
    if ((bits-ret_bits) >= readbuf_bits) {
      ret |= ((uint32_t)((readbuf>>(8-readbuf_bits))&((1<<(readbuf_bits))-1))<<(ret_bits));
      ret_bits += readbuf_bits;
      readbuf_bits = 0;
    } else {
      ret |= ((uint32_t)((readbuf>>(8-readbuf_bits))&((1<<(bits-ret_bits))-1))<<(ret_bits));
      readbuf_bits -= bits-ret_bits;
      ret_bits = bits;
    }
  }
  return ret;
}

uint8_t UTF8_BitstreamReader::skipToZeroBit() {
  if (nullptr == bitstream_bytearray_pos) return 0;
  uint8_t ret = 0;
  while(true) {
    if (0==readbuf_bits) {
      // Read new byte from bitstream
      readbuf = pgm_read_byte(bitstream_bytearray_pos++);
      readbuf_bits = 8;
    }
    bool isZeroBit = (0==(readbuf&(0x01<<(8-readbuf_bits))));
    readbuf_bits -= 1;
    if (isZeroBit) {
      break;
    }
    ret += 1;
  }
  return ret;
}

