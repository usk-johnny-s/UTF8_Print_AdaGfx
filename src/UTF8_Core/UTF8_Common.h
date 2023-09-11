#ifndef _UTF8_COMMON_H
#define _UTF8_COMMON_H

// Common definitions.

#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else // __AVR__ , defined(ESP8266) || defined(ESP32)
#ifdef PROGMEM
#undef PROGMEM
#endif // PROGMEM
#define PROGMEM
#ifdef pgm_read_byte
#undef pgm_read_byte
#endif // pgm_read_byte
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#ifdef pgm_read_word
#undef pgm_read_word
#endif // pgm_read_word
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
#ifdef pgm_read_dword
#undef pgm_read_dword
#endif // pgm_read_dword
#define pgm_read_dword(addr) (*(const uint32_t *)(addr))
#endif // __AVR__ , defined(ESP8266) || defined(ESP32)

#endif // _UTF8_COMMON_H