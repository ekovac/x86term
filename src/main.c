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
    pic_remap(0x20, 0x28);
    display_init(80,25);
    pic_set_mask( (1<<COM1IRQ) & (1<<1) );
    init_vterm();
    serial_t port;
    serial_init(&port, COM1, COM1IRQ, DEFAULT_SERIALCONFIG);
    //timer_config(0, 1000);
    term_puts("x86term - terminal emulator for bare x86 PCs\r\n"
              "(C) 2012 Philip \"digitalfox\" Kovac\r\n\r\n");
    return;
}
