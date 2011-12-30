/* All ganked from OSDEV */
#include "base.h"
#include "stdlib.h"

inline void outb( unsigned short port, unsigned char val )
{
    __asm__ volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}
inline unsigned char inb( unsigned short port )
{
    unsigned char ret;
    __asm__ volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}
inline void io_wait( void )
{
    // port 0x80 is used for 'checkpoints' during POST.
    // The Linux kernel seems to think it is free for use :-/
    __asm__ volatile( "outb %%al, $0x80"
                  : : "a"(0) );
}
