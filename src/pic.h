#ifndef _pic_h
#define _pic_h

#include "io.h"

#define PIC1 0x20
#define PIC2 0xA0

#define PIC1_CMD PIC1
#define PIC1_DAT (PIC1 + 1)

#define PIC2_CMD PIC2
#define PIC2_DAT (PIC2 + 1)

#define PIC_EOI 0x20

void disable_all_pic(void);
void enable_all_pic(void);

void send_eoi_pic(uint8_t irq);

#define IRQ0 0x20

void remap_pic(void);

void irq_set_mask(uint8_t irq);
void irq_clear_mask(uint8_t irq);

#endif
