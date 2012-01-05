#include "display.h"
#include "base.h"

display_state_t disp;

uint8_t rgb2vga(uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t color = 0;
    if (red > 128) color |= 0x4;
    if (green > 128) color |= 0x2;
    if (blue > 128) color |= 0x1;
    
    if (red > 250 || green > 250 || blue > 250) color |= 0x08;
    return color;
}
void display_init(uint8_t width, uint8_t height)
{
    disp.width = width;
    disp.height = height;
    disp.cursor_x = 0;
    disp.cursor_y = 0;
    disp.pen = 0x07;
    return;
}

void display_erase(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    int i,j;
    video_cell_t cell;
    cell.data = ' ';
    cell.attr = disp.pen;

    /* Make sure it's in bounds. */
    if (x1 > disp.width || x2 > disp.width || y1 > disp.height || y2 > disp.height)
        return;

    for (i = x1; i < x2; i++) for (j = y1; j < y2; j++) VIDEOAT(i,j) = cell;
    return;
}

void display_clear(void)
{
    display_erase(0, 0, disp.width, disp.height);
    display_set_cursor(0,0);
    return;
}

void display_set_pen(uint8_t attr)
{
    disp.pen = attr;
    return;
}
void display_set_cursor(uint8_t x, uint8_t y)
{
    unsigned short position = (y * 80) + x;
    disp.cursor_x = x;
    disp.cursor_y = y;

    outb(0x3D4, 0x0F);
    outb(0x3D5, position & 0xFF);
    outb(0x3D4, 0x0E);
    outb(0x3D5, (position >> 8) & 0xFF);
    return;
}
void display_put(uint8_t col, uint8_t row, uint8_t c, uint8_t attr)
{
    video_cell_t cell;
    cell.data = c;
    cell.attr = attr;
    VIDEOAT(col, row) = cell;
}
void display_copycell(uint8_t destcol, uint8_t destrow, uint8_t srccol, uint8_t srcrow)
{
    VIDEOAT(destcol, destrow) = VIDEOAT(srccol, srcrow);
}

void display_set_cell(char c)
{
    video_cell_t cell;
    cell.data = c;
    cell.attr = disp.pen;
    VIDEOAT(disp.cursor_x, disp.cursor_y) = cell;
    return;
}

void putc(char c)
{
    if (c == '\n')
    {
        disp.cursor_y++;
        disp.cursor_x = 0;
    }
    else
    {
        display_set_cell(c);
        disp.cursor_y = disp.cursor_y + (disp.cursor_x + 1)/disp.width;
        disp.cursor_x = (disp.cursor_x + 1) % disp.width;
    }
    if (disp.cursor_y >= disp.height)
    {
        /* TODO scroll */
        disp.cursor_y = 0;
    }
    
    return;
}

char* hexits = "0123456789ABCDEF";
void put_byte(uint8_t b)
{
    char left, right;
    right = hexits[0x0F & b];
    left = hexits[(0xF0 & b) >> 4];
    putc(left); putc(right);
}

void put_bytes(uint8_t* bytes, int len)
{
    int i;
    putc('0'); putc('x');
    for (i = len-1; i >= 0; i--)
    {
        put_byte(bytes[i]);
    }
}

void puts(char* s)
{
    char c;
    while ((c = *(s++)))
    {
        putc(c);
    }
    return;
}
