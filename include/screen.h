#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

extern void init_video();
extern void puts(uint8_t* string);
extern void putc(uint8_t character);
extern void puti(uint32_t a);
extern void puth(uint32_t a);
extern void putb(uint32_t a);
extern void putarri(uint32_t* arr, uint32_t n);
extern void printf(uint8_t* str, ...);

#endif
