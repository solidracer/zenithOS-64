#include "ps2.h"

keyboard_t keyboard;

static int buffer_full(void) {
    return ((keyboard.tail + 1) & (KEYBOARD_BUF_LEN-1)) == keyboard.head;
}

static int buffer_empty(void) {
    return keyboard.tail == keyboard.head;
}

int kb_buffer_push(uint16_t k) {
    if (buffer_full())
        return 1;
    keyboard.buffer[keyboard.tail] = k;
    keyboard.tail = (keyboard.tail + 1) & (KEYBOARD_BUF_LEN - 1);
    return 0;
}

int kb_buffer_pop(uint16_t *k) {
    if (buffer_empty())
        return 1;
    *k = keyboard.buffer[keyboard.head];
    keyboard.buffer[keyboard.head] = 0;
    keyboard.head = (keyboard.head + 1) & (KEYBOARD_BUF_LEN - 1);
    return 0;
}

static const unsigned char set1_normal[] = {
    [0x29] = '`', [0x02] = '1', [0x03] = '2', 
    [0x04] = '3', [0x05] = '4', [0x06] = '5',
    [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0', [0x0C] = '-',
    [0x0D] = '=',
    [0x0E] = KEY_BCKSPACE,
    [0x0F] = KEY_TAB,
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e',
    [0x13] = 'r', [0x14] = 't', [0x15] = 'y',
    [0x16] = 'u', [0x17] = 'i', [0x18] = 'o',
    [0x19] = 'p', [0x1A] = '[', [0x1B] = ']',
    [0x3A] = KEY_CAPSLOCK,
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd',
    [0x21] = 'f', [0x22] = 'g', [0x23] = 'h',
    [0x24] = 'j', [0x25] = 'k', [0x26] = 'l',
    [0x27] = ';', [0x28]= '\'',
    [0x1C] = KEY_ENTER,
    [0x2A] = KEY_LSHIFT,
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c',
    [0x2F] = 'v', [0x30] = 'b', [0x31] = 'n',
    [0x32] = 'm', [0x33] = ',', [0x34] = '.',
    [0x35] = '/',
    [0x36] = KEY_RSHIFT,
    [0x1D] = KEY_LCTRL,
    [0x38] = KEY_LALT,
    [0x39] = ' ',
    [0x45] = KEY_NUMLOCK,
    [0x47] = KEY_KP7,
    [0x4B] = KEY_KP4,
    [0x4F] = KEY_KP1,
    [0x48] = KEY_KP8,
    [0x4C] = KEY_KP5,
    [0x50] = KEY_KP2,
    [0x52] = KEY_KP0,
    [0x37] = KEY_KPSTAR,
    [0x49] = KEY_KP9,
    [0x4D] = KEY_KP6,
    [0x51] = KEY_KP3,
    [0x53] = KEY_KPDOT,
    [0x4A] = KEY_KPMIN,
    [0x4E] = KEY_KPPLUS,
    [0x01] = KEY_ESC,
    [0x2B] = '\\',
    [0x3B] = KEY_F1,
    [0x3C] = KEY_F2,
    [0x3D] = KEY_F3,
    [0x3E] = KEY_F4,
    [0x3F] = KEY_F5,
    [0x40] = KEY_F6,
    [0x41] = KEY_F7,
    [0x42] = KEY_F8,
    [0x43] = KEY_F9,
    [0x44] = KEY_F10,
    [0x57] = KEY_F11,
    [0x58] = KEY_F12,
    [0x46] = KEY_SCROLLLOCK,
};

static const unsigned char set1_shift[] = {
    [0x29] = '~', [0x02] = '!', [0x03] = '@',
    [0x04] = '#', [0x05] = '$', [0x06] = '%',
    [0x07] = '^', [0x08] = '&', [0x09] = '*',
    [0x0A] = '(', [0x0B] = ')', [0x0C] = '_',
    [0x0D] = '+', [0x10] = 'Q', [0x11] = 'W',
    [0x12] = 'E', [0x13] = 'R', [0x14] = 'T',
    [0x15] = 'Y', [0x16] = 'U', [0x17] = 'I',
    [0x18] = 'O', [0x19] = 'P', [0x1A] = '{',
    [0x1B] = '}', [0x1E] = 'A', [0x1F] = 'S',
    [0x20] = 'D', [0x21] = 'F', [0x22] = 'G',
    [0x23] = 'H', [0x24] = 'J', [0x25] = 'K',
    [0x26] = 'L', [0x27] = ':', [0x28]= '\"',
    [0x1C] = KEY_ENTER,
    [0x2C] = 'Z', [0x2D] = 'X', [0x2E] = 'C',
    [0x2F] = 'V', [0x30] = 'B', [0x31] = 'N',
    [0x32] = 'M', [0x33] = '<', [0x34] = '>',
    [0x35] = '?', [0x2B] = '|'
};

static const unsigned char set1_ext[] = {
    [0x38] = KEY_RALT,
    [0x1D] = KEY_RCTRL,
    [0x52] = KEY_INSERT,
    [0x53] = KEY_DEL,
    [0x4B] = KEY_LARROW,
    [0x47] = KEY_HOME,
    [0x4F] = KEY_END,
    [0x48] = KEY_UARROW,
    [0x50] = KEY_DARROW,
    [0x49] = KEY_PAGEUP,
    [0x51] = KEY_PAGEDOWN,
    [0x4D] = KEY_RARROW,
    [0x35] = KEY_KPSLASH,
    [0x1C] = KEY_KPENTER
};

__attribute__((interrupt, target("general-regs-only")))
void keyboard_handler(void *arg) {
    UNUSED(arg);
    uint8_t k = inb(PS2_DAT);
    char release = 0;
    static char extended = 0;
    static char capslock = 0, lshift = 0, rshift = 0;

    uint16_t key = 0;

    if (k == 0xE0)
        extended = 1;
    else {
        if (k & 0x80) {
            k &= ~0x80;
            release = 1;
        }
        
        if (extended && k < 84)  key = set1_ext[k];
        else if (k < 54 && ((lshift | rshift) ^ capslock) && set1_shift[k]) key = set1_shift[k];
        else if (k < 89) key = set1_normal[k];
        
        if (!key)
            goto end;
        else if (key == KEY_CAPSLOCK) capslock ^= !release;
        else if (key == KEY_LSHIFT) lshift = !release;
        else if (key == KEY_RSHIFT) rshift = !release;

        /* make it ready to send */
        key &= KEY_MASK;
        if (release)
            key |= RELEASE_MASK;

        kb_buffer_push(key);

        end:
        extended = 0;
    }

    send_eoi_pic(1);
}

/* a few helper functions */

static void wait_output_full(void) {
    while (!(inb(0x64) & 0x01)) asm volatile("nop");
}

static void wait_input_empty(void) {
    while (inb(0x64) & 0x02) asm volatile("nop");
}

static uint8_t read_data(void) {
    wait_output_full();
    return inb(PS2_DAT);
}

static void flush(void) {
    while (inb(PS2_CMD) & 0x01)
        UNUSED(inb(PS2_DAT));
}

static void write_data(uint16_t port, uint8_t data) {
    wait_input_empty();
    outb(port, data);
}

int init_ps2(void) {
    keyboard.head = keyboard.tail = 0;

    write_data(PS2_CMD, 0xAD);

    flush();

    write_data(PS2_CMD, 0x20);

    uint8_t cb = read_data();

    cb = 0b01010101;

    write_data(PS2_CMD, 0xAA);
    if (read_data() != 0x55)
        return 1;

    write_data(PS2_CMD, 0x60);
    write_data(PS2_DAT, cb);

    write_data(PS2_CMD, 0xAB);
    if (read_data() != 0x00)
        return 1;

    write_data(PS2_DAT, 0xFF);
    if (read_data() != 0xFA)
        return 1;
    if (read_data() != 0xAA)
        return 1;

    set_idt_gate(IRQ0 + 1, keyboard_handler, IDT_INTGATE);
    irq_clear_mask(1);

    write_data(PS2_CMD, 0xAE);

    return 0;
}
