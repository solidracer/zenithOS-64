#ifndef _pit_h
#define _pit_h

#include "pic.h"
#include "idt.h"

#define PIT_CH0 0x40
#define PIT_CH1 0x41
#define PIT_CH2 0x42
#define PIT_CMD 0x43

void init_pit(uint32_t freq);

void sleep(uint32_t ms);

#endif
