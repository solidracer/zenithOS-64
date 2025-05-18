#include "pit.h"

static int count = 0;

__attribute__((interrupt, target("general-regs-only")))
void timer_handler(void *arg) {
    UNUSED(arg);
    if (count)
        count--;
    send_eoi_pic(0);
}

void sleep(uint32_t ms) {
    if (ms < 10) return;
    count = ms / 10;
    while (count) asm volatile("hlt");
}

void init_pit(uint32_t freq) {
    uint32_t rval;

    if (freq <= 18)
        rval = 0x10000;
    else if (freq >= 1193182)
        rval = 1;
    else
        rval = 1193182 / freq;

    outb(PIT_CMD, 0x34);
    outb(PIT_CH0, rval & 0xFF);
    outb(PIT_CH0, (rval >> 8) & 0xFF);

    set_idt_gate(IRQ0, timer_handler, IDT_INTGATE);
    irq_clear_mask(0);
}
