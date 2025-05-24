#include "term.h"

#include "memutil.h"

terminal_t term;

static void halt(void) {
    asm volatile("cli");
    for (;;) asm volatile("hlt");
}

void init_term(pixel_t fg, pixel_t bg) {
    term.fg = term.initfg = fg;
    term.bg = bg;
    term.maxx = framebuffer.width / (FONT_WIDTH * FONT_SCALE);
    term.maxy = framebuffer.height / (FONT_HEIGHT * FONT_SCALE);
    term_clear();
}

void term_clear(void) {
    for (int i = 0;i<framebuffer.height;i++)
        for (int j = 0;j<framebuffer.width;j++)
            draw_pixel(j, i, term.bg);
    term.x = term.y = 0;
}

static void scroll(void) {
    int numlines = FONT_HEIGHT * FONT_SCALE;
    uint32_t pixelsize = (framebuffer.height - numlines) * framebuffer.stride;
    memmove(framebuffer.fb, DRAW_OFFSET(0, numlines), pixelsize * sizeof(uint32_t));
    memset(DRAW_OFFSET(0, framebuffer.height - numlines), 0, numlines * framebuffer.stride * sizeof(uint32_t));
}

static void next_row(void) {
    if (term.y + 1 >= term.maxy)
        scroll();
    else
        term.y++;
    term.x = 0;
}

void term_putc(unsigned char c) {
    static int isesc = 0;
    if (isesc) {
        switch(c) {
            case 'r': {
                term.fg = PACK_COLOR(255, 85, 85);
                break;
            }
            case 'g': {
                term.fg = PACK_COLOR(85, 255, 85);
                break;
            }
            case 'y': {
                term.fg = PACK_COLOR(255, 255, 85);
                break;
            }
            case 'm': {
                term.fg = PACK_COLOR(255, 40, 255);
                break;
            }
            case 'b': {
                term.fg = PACK_COLOR(85, 85, 255);
                break;
            }
            case 'a': {
                term.fg = PACK_COLOR(85, 255, 255);
                break;
            }
            case 'n': {
                term.fg = term.initfg;
                break;
            }
            case 'w': {
                term.fg = PACK_COLOR(255, 255, 255);
                break;
            }
            case 'c': {
                term_clear();
                break;
            }
            default: break;
        }
        isesc = 0;
        return;
    }
    switch(c) {
        case '\n': {
            next_row();
            return;
        }
        case '\t': {
            term.x = ALIGN(term.x + 1, 8);
            break;
        }
        case '\r': {
            term.x = 0;
            return;
        }
        case '\b': {
            if (term.x > 0)
                term.x--;
            return;
        }
        case '\e': {
            isesc = 1;
            return;
        }
        default: {
            if (term.x >= term.maxx)
                next_row();
            draw_glyph(c, term.x * FONT_WIDTH * FONT_SCALE, term.y * FONT_HEIGHT * FONT_SCALE, FONT_SCALE, term.fg, term.bg);
            term.x++;
            return;
        }
    }
}

static void print_str(const char *str) {
    while (*str) 
        term_putc(*str++);
}

void term_puts(const char *str) { 
    print_str(str);
    term_putc('\n');
}

static void print_hex(size_t n) {
    static const char digits[] = "0123456789ABCDEF";

    if (!n) {
        term_putc('0');
        return;
    }
    
    unsigned long div = 1;

    while (n / div >= 16)
        div *= 16;

    while (div != 0) {
        unsigned long d = n / div;
        term_putc(digits[d]);
        n %= div;
        div /= 16;
    }
}

static void print_bin(unsigned int n, unsigned int prec) {
    for (int i = prec-1;i>=0;i--)
        term_putc('0' + ((n >> i) & 1));
}

static void print_uint(unsigned int n) {
    unsigned int div = 1;

    while (n / div >= 10)
        div *= 10;

    while (div != 0) {
        unsigned int d = n / div;
        term_putc('0' + d);
        n %= div;
        div /= 10;
    }
}

static void print_int(int n) {
    if (n < 0) {
        term_putc('-');
        n = -n;
    }
    print_uint(n);
}

static void print_float(float f, int prec) {
    if (f<0) {
        term_putc('-');
        f = -f;
    }
    float i = (int)f;
    print_int(i);
    term_putc('.');
    float floating = f - i;
    for (int j = 0;j<prec;j++) floating *= 10;
    print_int(floating);
    term_putc('f');
}

void term_vprintf(const char *fmt, va_list args) {
    while (*fmt) {
        char c = *fmt++;
        if (c == '%') {
            c = *fmt++;
            switch(c) {
                case 's': {
                    char *str = va_arg(args, char*);
                    if (!str) {
                        print_str("(null)");
                        continue;
                    }
                    print_str(str);
                    continue;
                }
                case 'd': {
                    int d = va_arg(args, int);
                    print_int(d);
                    continue;
                }
                case 'u': {
                    unsigned int u = va_arg(args, unsigned int);
                    print_uint(u);
                    continue;
                }
                case 'x': {
                    unsigned int h = va_arg(args, unsigned int);
                    print_hex(h);
                    continue;
                }
                case 'b': {
                    unsigned int d = va_arg(args, unsigned int);
                    unsigned int p = va_arg(args, unsigned int);
                    print_str("0b");
                    print_bin(d, p);
                    continue;
                }
                case 'c': {
                    unsigned char c = va_arg(args, unsigned int);
                    term_putc(c);
                    continue;
                }
                case 'f': {
                    float f = va_arg(args, double);
                    print_float(f, 5);
                    continue;
                }
                case 'p': {
                    void *p = va_arg(args, void*);
                    if (!p) {
                        print_str("(null)");
                        continue;
                    }
                    print_str("0x");
                    print_hex(TOADDR(p));
                    continue;
                }
                default: term_putc('%'); term_putc(c); continue;
            }
        } else term_putc(c);
    }
}

void term_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    term_vprintf(fmt, args);
    va_end(args);
}

void term_panicf(const char *fmt, ...) {
    va_list args;
    term_printf("\erPANIC\en: ");
    va_start(args, fmt);
    term_vprintf(fmt, args);
    va_end(args);
    halt();
}
