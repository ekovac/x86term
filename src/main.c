#include "stdlib.h"
#include "display.h"
#include "segments.h"
#include "interrupts.h"
#include "pic.h"
#include "serial.h"
#include "termhandlers.h"
#include "timer.h"

void kmain(__unused void* mbd, __unused unsigned int magic)
{
    init_gdt();
    init_idt();
    PIC_remap(0x20, 0x28);
    init_vterm();
    timer_config(0, 1000);
    for (i=0; i<8; i++) IRQ_set_mask(i);
    IRQ_clear_mask(0);
    IRQ_clear_mask(1);
    if (combase == COM2)
        IRQ_clear_mask(3);
    if (combase == COM1)
        IRQ_clear_mask(4);
    serial_set_baud(combase, 9600);
    serial_reset(combase);
    serial_rxint(combase, 1);
    term_puts("x86term - terminal emulator for bare x86 PCs\r\n"
              "(C) 2012 Philip \"digitalfox\" Kovac\r\n\r\n");
    while(1) /* Main control loop */
    {
        __asm__("sti"); 
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
        if (vterm_output_bufferlen(vterm) != 0)
        {
            vterm_output_bufferread(vterm, &data, 1);
            __asm__("cli");
            ringbuf_pushback(&serial_outbuf, data);
            /* Enable transmit interrupt */
            serial_txint(combase, 1);
            __asm__("sti");
        }
        if (ringbuf_isempty(&serial_inbuf) 
            && ringbuf_isempty(&kb_inbuf) 
            && vterm_output_bufferlen(vterm) == 0)
        {
            __asm__("hlt"); /* Nothing to do, wait for next interrupt. */
        }
    }
    return;
}
