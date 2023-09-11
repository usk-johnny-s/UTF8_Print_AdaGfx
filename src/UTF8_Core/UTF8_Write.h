#ifndef _UTF8_WRITE_H
#define _UTF8_WRITE_H

// UTF writer interface for Decoder

class UTF8_Write {
public:
  virtual size_t (writeUTF)(uint32_t utf) = 0;
  virtual size_t (illegalCode)() = 0;
}; // class UTF8_Write

#endif // _UTF8_WRITE_H
