#ifndef _ps2_h
#define _ps2_h

#include "pic.h"
#include "idt.h"

#define PS2_CMD 0x64
#define PS2_DAT 0x60

#define KEY_MASK 0x7FFF
#define RELEASE_MASK 0x8000

#define KEYBOARD_BUF_LEN 64

typedef enum {
    KEY_CAPSLOCK = 128,
    KEY_LSHIFT,
    KEY_RSHIFT,
    KEY_LCTRL,
    KEY_LALT,
    KEY_NUMLOCK,
    KEY_KP7,
    KEY_KP4,
    KEY_KP1,
    KEY_KP8,
    KEY_KP5,
    KEY_KP2,
    KEY_KP0,
    KEY_KPSTAR,
    KEY_KP9,
    KEY_KP6,
    KEY_KP3,
    KEY_KPDOT,
    KEY_KPMIN,
    KEY_KPPLUS,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_SCROLLLOCK,
    KEY_BCKSPACE,
    KEY_TAB,
    KEY_ENTER,
    KEY_ESC,
    KEY_RALT,
    KEY_RCTRL,
    KEY_INSERT,
    KEY_DEL,
    KEY_LARROW,
    KEY_HOME,
    KEY_END,
    KEY_UARROW,
    KEY_DARROW,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
    KEY_RARROW,
    KEY_KPSLASH,
    KEY_KPENTER
} key_t;

typedef struct {
    volatile uint16_t buffer[KEYBOARD_BUF_LEN];
    volatile unsigned char tail, head;
} keyboard_t;

extern keyboard_t keyboard;

#define RESET() outb(PS2_CMD, 0xFE)

int kb_buffer_push(uint16_t k);
int kb_buffer_pop(uint16_t *k);

int init_ps2(void);

#endif
