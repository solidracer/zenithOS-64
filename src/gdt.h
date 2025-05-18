#ifndef _gdt_h
#define _gdt_h

#include "def.h"

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access_byte;
    uint8_t limit_high : 4;
    uint8_t flags : 4;
    uint8_t base_high;
} PACKED SD32_t;

typedef struct {
    uint16_t size;
    uint64_t base;
} PACKED GDTR64_t;

#define GDT_SIZE 3

extern GDTR64_t GDTR;

void init_gdt(void);

#endif
