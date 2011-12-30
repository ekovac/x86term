#include "display.h"
#include "segments.h"
#include "interrupts.h"
#include "pic.h"
#include "serial.h"
#include "termhandlers.h"
void kb_handler(void)
{
    ringbuf_pushback(&kb_inbuf, inb(0x60));
    outb(0x20, 0x20);
    return;
}
void serial_handler(void)
{
    uint8_t b;
    if (inb(COM1+5) & 0x1) /* Received data */
    {
        ringbuf_pushback(&serial_inbuf, inb(COM1));
    }
    else if (inb(COM1+5) & 0x20) /* Ready to transmit */
    {
        if (!ringbuf_isempty(&serial_outbuf))
        {
            b = ringbuf_popfront(&serial_outbuf);
            outb(COM1, b);
            if (ringbuf_isempty(&serial_outbuf)) serial_txint(0); /* Disable the tx interrupt if the ringbuf is empty. */
        }
    }
    outb(0x20, 0x20); /* Acknowledge interrupt */
    return;
}
void kmain(void* mbd, unsigned int magic)
{
    int i;
    uint8_t data, rdy;
    display_init(80, 25);
    display_clear();
    init_gdt();
    init_idt();
    PIC_remap(0x20, 0x28);
    for (i=0; i<8; i++) IRQ_set_mask(i);
    IRQ_clear_mask(1);
    IRQ_clear_mask(4);
    serial_set_baud(9600);
    serial_reset();
    serial_rxint(1);
    while(1) /* Main control loop */
    {
        
        /* Check if we have KB input */ 
        rdy = 0;
        __asm__("cli");
        if (!ringbuf_isempty(&kb_inbuf))
        {
            data = ringbuf_popfront(&kb_inbuf);
            rdy = 1;
        }
        __asm__("sti");
        if (rdy) term_handlescancode(data);

        /* Check if we have serial input */
        rdy = 0;
        __asm__("cli");
        if (!ringbuf_isempty(&serial_inbuf))
        {
            data = ringbuf_popfront(&serial_inbuf);
            rdy = 1;
        }
        __asm__("sti");
        if (rdy) term_handleserial(data);
        
        /* Check if we have stuff to output to write to serial */
        if (vterm_output_bufferlen(vterm) > 0)
        {
            vterm_output_bufferread(vterm, &data, 1);
            __asm__("cli");
            ringbuf_pushback(&serial_outbuf, data);
            /* Enable transmit interrupt */
            serial_txint(1);
            __asm__("sti");
        }
    }
    return;
}
