#include "serial.h"
void serial_set_baud(short rate)
{ /* Also sets line protocol */
    short divisor;
    divisor = 115200/rate;
    outb(COM1+1, 0x00);
    outb(COM1+3, 0x80);
    outb(COM1+0, divisor & 0x00FF);
    outb(COM1+1, (divisor & 0xFF00) >> 8);
    outb(COM1+3, 0x03);
    outb(COM1+1, 0x01);
    return;
}
void serial_reset(void)
{
    /* Clear FIFO, de-assert RTS/DSR */
    outb(COM1+1, 0x00);
    outb(COM1+2, 0xC7);
    outb(COM1+4, 0x00);
    outb(COM1+4, 0x0B);
    outb(COM1+1, 0x01);
}
void serial_putc(char c)
{
    outb(COM1, c);
    return;
}
