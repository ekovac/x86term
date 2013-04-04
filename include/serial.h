#ifndef X86TERM_SERIAL_H
#define X86TERM_SERIAL_H
#include "base.h"
#include "stdint.h"
#include "ringbuf.h"
#define COM1 (0x3F8)
#define COM2 (0x2F8)
#define COM1IRQ (0x)



typedef struct {
	short speed;
	char data_bits; /* Valid values: 5-9; default is 8 */
	enum {
		P_NONE,
		P_ODD,
		P_EVEN,
		P_MARK,
		P_SPACE
	} parity; /* default is P_NONE */
	char stop_bits;
} serialconfig_t;

extern serialconfig_t DEFAULT_SERIALCONFIG;

typedef struct {
	short combase;
	short irq; 
	serialconfig_t config;
	ringbuf_t* in_buf;
	ringbuf_t* out_buf;
	char exception; /* Exception flag; set on state change. */
} serial_t;
void serial_init(serial_t* port, short combase, short irq, serialconfig_t config);
int serial_applycfg(serial_t* port); /* Apply port->config's contents to the UART. */
void serial_retrievecfg(serial_t* port); /* Store the UART's config into port->config */
void serial_reset(serial_t* port);
void serial_putc(serial_t* port, char c);
void serial_getc(serial_t* port, char* c);
void serial_puts(serial_t* port, char *s);
void serial_txint(serial_t* port, char c);
void serial_rxint(serial_t* port, char c);
#endif
