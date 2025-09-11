#pragma once

#include <stdint.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

typedef enum {
    FONT_SMALL = 1,
    FONT_MEDIUM = 2,
    FONT_LARGE = 3
} font_size_t;

void display_init(void);
void display_clear(void);
void display_draw_char(int x, int y, char c, font_size_t size);
void display_draw_string(int x, int y, const char *str, font_size_t size);
void display_flush(void);
int display_char_width(font_size_t size);
int display_line_height(font_size_t size);

