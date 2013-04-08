#include "serial.h"

const serialconfig_t DEFAULT_SERIALCONFIG = {
        .speed = 38400, 
        .data_bits = 8, 
        .parity = P_NONE, 
        .stop_bits = STOP_ONEBIT};
/* Ugly utility functions */


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
    /* Register interrupt handlers */
    x86event_register(irq, serial_handleinterrupt, (void*)port);
}

int serial_handleinterrupt(registers_t state, void* voidport)
{
    serial_t* port = (serial_t*)voidport;
    term_puts("Stuff from serial port!");
    return 0;
}

void serial_applycfg(serial_t* port)
{
    /* Compute the LCR value. */
    serialconfig_t cfg = port->config;

    linecontrol_t lcr;
    lcr.data_bits = cfg.data_bits - 5;
    lcr.stop_bits = (cfg.stop_bits == STOP_MOREBITS);
    lcr.parity = cfg.parity;

    short divisor = 115200/cfg.speed;

    outb(port->combase + 1, 0x00); /* Disable all interrupts */
    outb(port->combase + 3, 0x80); /* Prepare to set baud */
    outb(port->combase + 0, divisor & 0x00FF); /* Set low part of divisor. */
    outb(port->combase + 1, (divisor & 0xFF00) >> 8 ); /* Set high part of divisor. */
    outb(port->combase + 3, lcr.byte_repr); /* Set the line protocol */

}

serialint_t serial_getint(serial_t* port)
{
    serialint_t intcfg;
    intcfg.byte_repr = inb(port->combase+1);
    return intcfg;
}

void serial_setint(serial_t* port, serialint_t value)
{
    outb(port->combase+1, value.byte_repr);
}
/*
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
*/