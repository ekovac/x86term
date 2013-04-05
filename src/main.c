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
    term_puts("x86term - terminal emulator for bare x86 PCs\r\n"
              "(C) 2012 Philip \"digitalfox\" Kovac\r\n\r\n");
    return;
}
