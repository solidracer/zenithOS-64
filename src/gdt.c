#include "gdt.h"

static SD32_t GDT[GDT_SIZE] = {0};
GDTR64_t GDTR;

extern void flush_gdt(void);

static void set_segment(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    SD32_t *sd = GDT + index;

    sd->base_low = base & 0xFFFF;
    sd->limit_low = limit & 0xFFFF;

    sd->base_middle = (base >> 16) & 0xFF;
    sd->limit_high = (limit >> 16) & 0xF;

    sd->base_high = (base >> 24) & 0xFF;

    sd->access_byte = access;

    sd->flags = flags;
}

void init_gdt(void) {
    /* NULL SD */
    set_segment(0, 0x0, 0x0, 0x00, 0x0);
    /* KERNEL CODE SD */
    set_segment(1, 0x0, 0xFFFFF, 0x9A, 0xA);
    /* KERNEL DATA SD */
    set_segment(2, 0x0, 0xFFFFF, 0x92, 0xC);

    GDTR.base = TOADDR(GDT);
    GDTR.size = (GDT_SIZE * sizeof(SD32_t)) - 1;

    flush_gdt();
}
