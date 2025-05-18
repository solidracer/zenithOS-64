#include "pic.h"

void disable_all_pic(void) {
    outb(PIC1_DAT, 0xFF);
    outb(PIC2_DAT, 0xFF);
}

void enable_all_pic(void) {
    outb(PIC1_DAT, 0x00);
    outb(PIC2_DAT, 0x00);
}

void send_eoi_pic(uint8_t irq) {
    if (irq >= 8)
        outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

void remap_pic(void) {
    outb(PIC1_CMD, 0x11);
    io_wait();
    outb(PIC2_CMD, 0x11);
    io_wait();
    outb(PIC1_DAT, 0x20);
    io_wait();
    outb(PIC2_DAT, 0x28);
    io_wait();
    outb(PIC1_DAT, 0x04);
    io_wait();
    outb(PIC2_DAT, 0x02);
    io_wait();

    outb(PIC1_DAT, 0x01);
    io_wait();
    outb(PIC2_DAT, 0x01);
    io_wait();

    disable_all_pic();
}

void irq_set_mask(uint8_t irq) {
    uint16_t port;

    if (irq < 8)
        port = PIC1_DAT;
    else {
        port = PIC2_DAT;
        irq -= 8;
    }

    outb(port, inb(port) | (1 << irq));
}

void irq_clear_mask(uint8_t irq) {
    uint16_t port;

    if (irq < 8)
        port = PIC1_DAT;
    else {
        port = PIC2_DAT;
        irq -= 8;
    }

    outb(port, inb(port) & ~(1 << irq));
}
