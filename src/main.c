#include "stdlib.h"
#include "display.h"
#include "segments.h"
#include "interrupts.h"
#include "pic.h"
#include "serial.h"
#include "termhandlers.h"
#include "timer.h"
static uint8_t times_through = 0;
static short combase = COM1;
void kb_handler(void)
{
    ringbuf_pushback(&kb_inbuf, inb(0x60));
    outb(0x20, 0x20);
    return;
}
void exception_handler(registers_t regs)
{
    static int count = 3;
    puts("CPU exception: ");
    put_bytes((uint8_t*)&(regs.int_no), sizeof(int)); puts(" "); 
    put_bytes((uint8_t*)&(regs.err_code), sizeof(int)); puts("\n");
    puts("Executing instruction: ");
    put_bytes((uint8_t*)&(regs.eip), sizeof(int)); puts("\n");
    count--;
    if (count == 0) __asm__("hlt");

    return;
}
void serial_handler(void)
{
    uint8_t b;
    while (inb(combase+5) & 0x1) /* Received data */
    {
        ringbuf_pushback(&serial_inbuf, inb(combase));
    }
    while (inb(combase+5) & 0x20 && !ringbuf_isempty(&serial_outbuf)) /* Ready to transmit */
    {
        b = ringbuf_popfront(&serial_outbuf);
        outb(combase, b);
        if (ringbuf_isempty(&serial_outbuf)) serial_txint(combase, 0); /* Disable the tx interrupt if the ringbuf is empty. */
    }
    outb(0x20, 0x20); /* Acknowledge interrupt */
    return;
}
void parse_args(void* mbd, int* port, int* baud)
{
    uint32_t flags = mbd;
    if (flags & (1 << 1))
    {
        /* cmdline is specified. */
        
    }
    else
    {
        *port = 1;
        *baud = 9600;
    }
    if (flags & (1 << 11))
    {
        /* vbe info is specified. */
    }

}
void kmain(__unused void* mbd, __unused unsigned int magic)
{
    int i;
    int8_t data, rdy;
    init_gdt();
    display_init(80, 25);
    display_clear();
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
        times_through++;
        if (ringbuf_isempty(&serial_inbuf) 
            && ringbuf_isempty(&kb_inbuf) 
            && vterm_output_bufferlen(vterm) == 0)
        {
            __asm__("hlt"); /* Nothing to do, wait for next interrupt. */
        }
    }
    return;
}
