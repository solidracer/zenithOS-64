#ifndef _idt_h
#define _idt_h

#include "def.h"

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} PACKED IDT64_t;

typedef struct {
    uint16_t size;
    uint64_t base;
} PACKED IDTR64_t;

#define IDT_SIZE 256

#define IDT_INTGATE 0x8E
#define IDT_TRPGATE 0x8F

void set_idt_gate(int n, void *f, int flags);
void init_idt(void);

#endif
