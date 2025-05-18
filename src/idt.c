#include "idt.h"

ALIGNED(0x10) static IDT64_t IDT[IDT_SIZE] = {0};
static IDTR64_t IDTR;

void set_idt_gate(int n, void *f, int flags) {
    size_t addr = TOADDR(f);
    IDT64_t *gate = IDT + n;
    gate->offset_low = addr & 0xFFFF;
    gate->selector = 0x08;
    gate->ist = 0;
    gate->attr = flags;
    gate->offset_mid = (addr >> 16) & 0xFFFF;
    gate->offset_high = (addr >> 32) & 0xFFFFFFFF;
    gate->zero = 0;
}

void init_idt(void) {
    IDTR.base = TOADDR(IDT);
    IDTR.size = (IDT_SIZE * sizeof(IDT64_t)) - 1;

    asm volatile("lidt %0" :: "m"(IDTR));
}
