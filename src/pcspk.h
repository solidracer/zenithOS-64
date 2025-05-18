#ifndef _pcspk_h
#define _pcspk_h

#include "pit.h"

#define SPKR_CTL 0x61

void stop_beep(void);
void beep(uint32_t freq, uint32_t timems);

#endif
