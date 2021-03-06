#ifndef X86TERM_DISPLAY_H
#define X86TERM_DISPLAY_H

#include "stdint.h"
typedef struct VIDEO_CELL {
    uint8_t data;
    uint8_t attr;
} video_cell_t;
typedef struct DISPLAY_STATE {
    uint8_t width, height;
    uint8_t cursor_x, cursor_y;
    uint8_t pen;
} display_state_t;

extern display_state_t disp;

#define VIDEOMEM ((video_cell_t*)0xb8000)
#define VIDEOAT(x,y) (VIDEOMEM[(y)*disp.width+(x)])

uint8_t rgb2vga(uint8_t r, uint8_t g, uint8_t b);

void display_init(uint8_t width, uint8_t height);
void display_erase(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void display_clear(void);

void display_set_pen(uint8_t attr);
void display_set_cursor(uint8_t x, uint8_t y);
void display_put(uint8_t col, uint8_t row, uint8_t c, uint8_t attr);
void display_copycell(uint8_t destcol, uint8_t destrow, uint8_t srccol, uint8_t srcrow);
void display_set_cell(char c);
void display_hw_cursor(uint8_t x, uint8_t y, uint8_t visible);

void putc(char c);
void put_byte(uint8_t b);
void put_bytes(uint8_t* bytes, int len);
void puts(char* s);

#endif
