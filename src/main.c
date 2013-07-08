#include "stdlib.h"
#include "display.h"
#include "segments.h"
#include "interrupts.h"
#include "pic.h"
#include "serial.h"
#include "termhandlers.h"
#include "events.h"
#include "timer.h"

int tmpkbd(registers_t state, void* user_data)
{
    term_puts("Keyboard interrupt!\r\n");
    inb(0x60);
    return 1;
}

void kmain(__unused void* mbd, __unused unsigned int magic)
{
    init_gdt();
    init_idt();
    x86event_init();
    pic_remap(0x20, 0x28);
    display_init(80,25);
    pic_mask_all();
    pic_del_mask( 1<<COM1IRQ );
    pic_del_mask( 1<<1 );
    init_vterm();
    
    serial_t port;
    serial_init(&port, COM1, COM1IRQ, DEFAULT_SERIALCONFIG);
    serialint_t rxrdy = {.rx_rdy = 1};
    serial_setint(&port, rxrdy);
    //timer_config(0, 1000);

    term_puts("x86term - terminal emulator for bare x86 PCs\r\n"
              "(C) 2012 Philip \"digitalfox\" Kovac\r\n\r\n");


    x86event_register(1+0x20, &tmpkbd, NULL); // Bogus keyboard handler.
     
    //__asm__("int 0x21");
    //int a=0, b=0, c;
    //c=a/b;
    while (1)
    {
        __asm__("sti");
        __asm__("hlt");
    }


    return;
}
