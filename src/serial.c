#include "serial.h"
#include "termhandlers.h"

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

    /* TODO: check the PIC mapping and apply the appropriate offset instead of hardcoding it. */
    x86event_register(port->irq+0x20, serial_handleinterrupt, (void*)port);
    term_puts("Serial port initialized.\r\n");
}

int serial_handleinterrupt(registers_t state, void* voidport)
{
    uint8_t b;
    serialint_t intcfg;
    linestatus_t linestatus;
    serial_t* port = (serial_t*)voidport;

    linestatus = (linestatus_t)inb(port->combase+5);
    while (linestatus.rx_rdy) /* Received data */
    {
        b = inb(port->combase);
        ringbuf_pushback(port->in_buf, b);
        linestatus = (linestatus_t)inb(port->combase+5);
    }

    while (linestatus.tx_rdy & !ringbuf_isempty(port->out_buf))
    {
        b = ringbuf_popfront(port->out_buf);
        outb(port->combase, b);
        if (ringbuf_isempty(port->out_buf))
        {
            intcfg = serial_getint(port);
            intcfg.tx_rdy = 0;
            serial_setint(port, intcfg);
        } 
        linestatus = (linestatus_t)inb(port->combase+5);
    }
    return -1; /* We can't know that the interrupt ONLY referred to us. */
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
void serial_putc(serial_t* port, char c)
{
    return;
}

int serial_getc(serial_t* port)
{
    if (ringbuf_isempty(port->in_buf))
        return -1;
    return (int)ringbuf_popfront(port->in_buf);

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
