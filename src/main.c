#include "display.h"
#include "segments.h"
#include "interrupts.h"
#include "pic.h"
#include "serial.h"
#include "keys.h"
void kb_handler(void)
{
    char inval;
    inval = inb(0x60);
    serial_putc(handle_keypress(inval));
    outb(0x20, 0x20);
    return;
}
void serial_handler(void)
{
    char inval;
    inval = inb(COM1);
    putc(inval);
    outb(0x20, 0x20);
    return;
}
void kmain(void* mbd, unsigned int magic)
{
    int i;
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
    asm("sti");
    while(1)
    {
        asm("xchg %eax, %eax");
    }
    return;
}
