#ifndef X86TERM_SERIAL_H
#define X86TERM_SERIAL_H
#include "base.h"
#include "stdint.h"
#define COM1 (0x3F8)
#define COM2 (0x2F8)
void serial_set_baud(short rate);
void serial_reset(void);
void serial_putc(char c);
void serial_txint(char c);
void serial_rxint(char c);
#endif
