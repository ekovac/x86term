#ifndef X86TERM_SERIAL_H
#define X86TERM_SERIAL_H
#include "base.h"
#include "stdint.h"
#define COM1 (0x3F8)
#define COM2 (0x2F8)
void serial_set_baud(short combase, short rate);
void serial_reset(short combase);
void serial_putc(short combase, char c);
void serial_puts(short combase, char *s);
void serial_txint(short combase, char c);
void serial_rxint(short combase, char c);
#endif
