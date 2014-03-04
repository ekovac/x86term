#ifndef X86TERM_SERIAL_H
#define X86TERM_SERIAL_H
#include "base.h"
#include "stdint.h"
#include "ringbuf.h"
#include "events.h"
#define COM1 (0x3F8)
#define COM2 (0x2F8)
#define COM1IRQ (4)
#define COM2IRQ (3)

typedef union {
	struct {
		unsigned char rx_rdy:1;
		unsigned char tx_rdy:1;
		unsigned char line:1;
		unsigned char modem:1;
		unsigned char sleep:1;
		unsigned char lowpower:1;
	};
	unsigned char byte_repr;
} serialint_t;

typedef union {
	struct {
		unsigned char rx_rdy:1;
		unsigned char e_overrun:1;
		unsigned char e_parity:1;
		unsigned char e_framing:1;
		unsigned char e_break:1;
		unsigned char tx_rdy:1;
		unsigned char holding:1;
		unsigned char e_fifo:1;
	};
	unsigned char byte_repr;
} linestatus_t;

typedef union {
	struct {
		unsigned char dtr:1;
		unsigned char rts:1;
		unsigned char aux1:1;
		unsigned char aux2:1;
		unsigned char loopback:1;
	};
	unsigned char byte_repr;
} modemcontrol_t;

typedef union {
	struct {
		unsigned char data_bits:2;
		unsigned char stop_bits:1;
		unsigned char parity:3;
		unsigned char brk:1;
		unsigned char divlatch:1;
	} ;
	unsigned char byte_repr;
} linecontrol_t;

typedef struct {
	unsigned short speed;
	char data_bits; /* Valid values: 5-9; defau5lt is 8 */
	enum {
		P_NONE = 0x0,
		P_ODD = 0x1,
		P_EVEN = 0x3,
		P_MARK = 0x5,
		P_SPACE = 0x7
	} parity; /* default is P_NONE */
	enum {
		STOP_ONEBIT,
		STOP_MOREBITS
	} stop_bits;
} serialconfig_t;

extern const serialconfig_t DEFAULT_SERIALCONFIG;

typedef struct {
	short combase;
	short irq; 
	serialconfig_t config;
	ringbuf_t* in_buf;
	ringbuf_t* out_buf;
	char exception; /* Exception flag; set on state change. */
} serial_t;

void serial_init(serial_t* port, short combase, short irq, serialconfig_t config);
void serial_applycfg(serial_t* port); /* Apply port->config's contents to the UART. */
void serial_reset(serial_t* port);
void serial_putc(serial_t* port, char c);
int serial_getc(serial_t* port);
void serial_puts(serial_t* port, char *s);
void serial_setint(serial_t* port, serialint_t value);
int serial_handleinterrupt(registers_t state, void* voidport);

serialint_t serial_getint(serial_t* port);
#endif
