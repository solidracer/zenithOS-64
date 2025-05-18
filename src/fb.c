#include "fb.h"

framebuffer_t framebuffer;

void init_fb(uint32_t *fb, int width, int height, int stride) {
    framebuffer.fb = fb;
    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.stride = stride;
}

void draw_pixel(int x, int y, pixel_t color) {
    if (x >= framebuffer.width || y >= framebuffer.height || color == NO_COLOR) return;
    *DRAW_OFFSET(x, y) = color;
}

void draw_pixel_scaled(int x, int y, int scale, pixel_t color) {
    for (int r = 0;r<scale;r++)
        for (int c = 0;c<scale;c++)
            draw_pixel(x + c, y + r, color);
}
