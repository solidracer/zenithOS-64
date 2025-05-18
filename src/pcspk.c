#include "pcspk.h"

static void play_sound(uint32_t freq) {
    uint32_t rval;

    if (freq <= 18)
        rval = 0x10000;
    else if (freq >= 1193182)
        rval = 1;
    else
        rval = 1193182 / freq;

    outb(PIT_CMD, 0xb6);
    outb(PIT_CH2, rval & 0xFF);
    outb(PIT_CH2, (rval >> 8) & 0xFF);

    uint8_t val = inb(SPKR_CTL);
    if (val != (val | 3)) {
        outb(SPKR_CTL, val | 3);
    }
}


void stop_beep(void) {
    uint8_t val = inb(SPKR_CTL) & 0xFC;
    outb(SPKR_CTL, val);
}

void beep(uint32_t freq, uint32_t timems) {
    play_sound(freq);
    sleep(timems);
    stop_beep();
}
