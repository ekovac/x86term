#include "serial.h"

const serialconfig_t DEFAULT_SERIALCONFIG = {
        speed = 38400, 
        data_bits = 8, 
        parity = P_NONE, 
        stop_bits = 1};

void serial_init(serial_t* port, short combase, short irq, serialconfig_t config)
{
    /* Configure port */
    port->combase = combase;
    port->irq = irq;
    port->config = config;
    serial_applycfg(port);
    /* Initialize I/O buffers */
    port->in_buf = ringbuf_new();
    port->out_buf = ringbuf_new();
    port->exception = 0;
}
void serial_applycfg(serial_t* port)
{
    return;
}
void serial_set_baud(short combase, short rate)
{ /* Also sets line protocol */
    short divisor;
    divisor = 115200/rate;
    outb(combase+1, 0x00);
    outb(combase+3, 0x80);
    outb(combase+0, divisor & 0x00FF);
    outb(combase+1, (divisor & 0xFF00) >> 8);
    outb(combase+3, 0x03);
    return;
}
void serial_reset(serial_t* port)
{
    /* Clear FIFO, de-assert RTS/DSR */
    outb(combase+1, 0x00);
    outb(combase+2, 0x07);
    outb(combase+4, 0x00);
    outb(combase+4, 0x0B);
}
void serial_txint(short combase, char c)
{
    c = (c && c) << 1; 
    c = c | (inb(combase+1) & ~0x02);
    outb(combase+1, c);
}
void serial_rxint(short combase, char c)
{
    c = (c && c);
    c = c | (inb(combase+1) & ~0x01);
    outb(combase+1, c);
}
void serial_putc(short combase, char c)
{
    outb(combase, c);
    return;
}
void serial_puts(short combase, char *s)
{
    while(*s)
        serial_putc(combase, *(s++));
}
