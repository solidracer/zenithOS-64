#ifndef _term_h
#define _term_h

#include <stdarg.h>
#include "font.h"

typedef struct {
    int x, y, maxx, maxy;
    uint32_t fg, bg, initfg;
} terminal_t;

extern terminal_t term;

void init_term(pixel_t fg, pixel_t bg);

void term_clear(void);
void term_putc(unsigned char c);
void term_puts(const char *str);
void term_vprintf(const char *fmt, va_list args);
void term_printf(const char *fmt, ...);
void term_panicf(const char *fmt, ...);

#endif
