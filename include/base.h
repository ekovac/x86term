#ifndef IOBASE_H
#define IOBASE_H
#include "types.h"
void* memset(void* s, int c, size_t n);
void outb( unsigned short port, unsigned char val );
void io_wait( void );
unsigned char inb( unsigned short port );
#endif
