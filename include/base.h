#ifndef IOBASE_H
#define IOBASE_H
void outb( unsigned short port, unsigned char val );
void io_wait( void );
unsigned char inb( unsigned short port );
void kpanic(char* msg, void* addr);
#endif
