#include <zenithBoot.h>
#include "term.h"

#include "memutil.h"

#include "gdt.h"
#include "pic.h"
#include "idt.h"
#include "pit.h"
#include "pcspk.h"
#include "ps2.h"

extern zenith_boot_info_t info;
extern zenith_memory_map_t memmap;

static const char *logo =
"+==============================================================================+\n"
"|'########:'########:'##::: ##:'####:'########:'##::::'##::'#######:::'######::|\n"
"|..... ##:: ##.....:: ###:: ##:. ##::... ##..:: ##:::: ##:'##.... ##:'##... ##:|\n"
"|:::: ##::: ##::::::: ####: ##:: ##::::: ##:::: ##:::: ##: ##:::: ##: ##:::..::|\n"
"|::: ##:::: ######::: ## ## ##:: ##::::: ##:::: #########: ##:::: ##:. ######::|\n"
"|:: ##::::: ##...:::: ##. ####:: ##::::: ##:::: ##.... ##: ##:::: ##::..... ##:|\n"
"|: ##:::::: ##::::::: ##:. ###:: ##::::: ##:::: ##:::: ##: ##:::: ##:'##::: ##:|\n"
"| ########: ########: ##::. ##:'####:::: ##:::: ##:::: ##:. #######::. ######::|\n"
"|........::........::..::::..::....:::::..:::::..:::::..:::.......::::......:::|\n"
"+==============================================================================+\n";

#define SHELL_PROMPT "\ew#>\en"

#define DRAW_CURSOR() draw_glyph(0xFF, term.x * FONT_WIDTH * FONT_SCALE, term.y * FONT_HEIGHT * FONT_SCALE, FONT_SCALE, term.fg, term.bg);
#define ERASE_CURSOR() draw_glyph(' ', term.x * FONT_WIDTH * FONT_SCALE, term.y * FONT_HEIGHT * FONT_SCALE, FONT_SCALE, term.fg, term.bg);

extern char high_entry[];

void kmain(void) {
    if (info.pixel_format != PF_BGRR_32BBP)
        return;

    init_gdt();
    remap_pic();
    init_pit(100);
    TRYCALL(init_ps2);
    init_idt();
    init_fb(info.fb, info.width, info.height, info.ppsl);
    init_term(PACK_COLOR(170, 170, 170), 0x0);
    asm volatile("sti");

    uint64_t mem = 0;

    for (unsigned int i = 0;i<memmap.entries;i++) {
        efi_memory_descr_t *entry = (efi_memory_descr_t*)(TOADDR(memmap.map) + i * memmap.desc_size);
        if (entry->type == 7)
            mem += entry->numpage * 0x1000;
    }

    mem /= 1024 * 1024;

    term_printf("%s\nWelcome to \ewzen\x18thOS!\en\nresolution: %dx%d (characters)\nresolution: %d (%d) x %d (pixels)\nusable memory: %u MiB\narch: x86-64\n\nengineered with chaos by \ewsolidracer\en\n\n", logo, term.maxx, term.maxy, framebuffer.width, framebuffer.stride, framebuffer.height, mem);

    term_printf("%s ", SHELL_PROMPT);
    DRAW_CURSOR();
    char buffer[77];
    int i = 0;
    for (;;) {
        uint16_t k;
        while (kb_buffer_pop(&k))
            asm volatile("hlt");
        if (!(k & RELEASE_MASK)) {
            ERASE_CURSOR();
            if (k == KEY_ENTER || k == KEY_KPENTER) {
                buffer[i] = '\0';
                term_putc('\n');
                if (i == 5 && !memcmp(buffer, "clear", 5))
                    term_clear();
                else if (i == 4 && !memcmp(buffer, "info", 4))
                    term_printf("%s\n\ewzen\x18thOS\en v0.0.0 made by solidracer\n\narch: x86-64\nresolution: %dx%d (characters)\nresolution: %dx%d (pixels)\n", logo, term.maxx, term.maxy, framebuffer.width, framebuffer.height);
                else if (i == 5 && !memcmp(buffer, "reset", 5))
                    RESET();
                else if (i == 4 && !memcmp(buffer, "beep", 4))
                    beep(800, 400);
                else if (!memcmp(buffer, "echo", 4) && i >= 4)
                    term_printf("%s\n", i>5?buffer + 5 : buffer + 4);
                else if (i == 5 && !memcmp(buffer, "sleep", 5))
                    sleep(1000);
                else if (i == 3 && !memcmp(buffer, "mem", 3))
                    term_printf("entry: \ea%p\en\nkmain: \eg%p\en\nframebuffer: \ey%p\en\ninfo (low): \em%p\en\n", high_entry, kmain, framebuffer.fb, &info);
                else if (i == 4 && !memcmp(buffer, "help", 4))
                    term_printf("minimal shell for zenithOS\ncommands: clear, info, reset, echo, help, beep, sleep, mem\n\nwritten by \ewsolidracer\en\n");
                else if (i > 0) {
                    term_printf("\ererror:\en unknown command `\ew%s\en'\n", buffer);
                    beep(200, 100);
                }
                i = 0;
                term_printf("%s ", SHELL_PROMPT);
            }
            else if (k == KEY_BCKSPACE && i > 0) {
                i--;
                draw_glyph(' ', --term.x * FONT_WIDTH * FONT_SCALE, term.y * FONT_HEIGHT * FONT_SCALE, FONT_SCALE, term.fg, term.bg);
            }
            else if (i < 76 && k < 127 && k >= 32) {
                buffer[i++] = k;
                term_putc(k);
            }
            DRAW_CURSOR();
        }
    }
}
