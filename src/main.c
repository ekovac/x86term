#include "display.h"
#include "segments.h"
#include "interrupts.h"
#include "pic.h"
void kb_handler(void)
{
    puts("Keyboard interrupt received!\n");
    outb(0x20, 0x20);
    puts("Interrupt acknowledged!\n");
    return;
}
void serial_handler(void)
{
    puts("Serial interrupt received!\n");
    outb(0x20, 0x20);
    return;
}
void kmain(void* mbd, unsigned int magic)
{
    int i;
    display_init(80, 25);
    display_clear();
    puts("Hello world! This is a cleaner base.\n");
    init_gdt();
    init_idt();
    PIC_remap(0x20, 0x28);
    for (i=0; i<8; i++) IRQ_set_mask(i);
    IRQ_clear_mask(1);
    IRQ_clear_mask(4);
    asm("sti");
    while(1)
    {
        asm("xchg %eax, %eax");
    }
    return;
}
