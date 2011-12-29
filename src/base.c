/* All ganked from OSDEV */
#include "base.h"
void* memset(void* s, int c, size_t n)
{
    char* sp = (char*)s;
    int i;
    /* TODO Optimize */
    for (i = 0; i < n; i++)
        sp[i] = (char)c;
}
inline void outb( unsigned short port, unsigned char val )
{
    asm volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}
inline unsigned char inb( unsigned short port )
{
    unsigned char ret;
    asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}
inline void io_wait( void )
{
    // port 0x80 is used for 'checkpoints' during POST.
    // The Linux kernel seems to think it is free for use :-/
    asm volatile( "outb %%al, $0x80"
                  : : "a"(0) );
}
