#ifndef _fb_h
#define _fb_h

#include "def.h"

typedef struct {
    uint32_t *fb;
    int width, height, stride;
} framebuffer_t;

typedef uint32_t pixel_t;

extern framebuffer_t framebuffer;

#define NO_COLOR (pixel_t)-1
#define PACK_COLOR(r, g, b) (((r) << 16) | ((g) << 8) | (b))
#define DRAW_OFFSET(x, y) (framebuffer.fb + (y) * framebuffer.stride + (x))

void init_fb(uint32_t *fb, int width, int height, int stride);

void draw_pixel(int x, int y, pixel_t color);
void draw_pixel_scaled(int x, int y, int scale, pixel_t color);

#endif
