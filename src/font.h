#ifndef _font_h
#define _font_h

#include "fb.h"

#define FONT_HEIGHT 16
#define FONT_WIDTH 8
#define FONT_SCALE 1

void draw_glyph(unsigned char c, int x, int y, int scale, pixel_t fg, pixel_t bg);

#endif
