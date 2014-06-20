#ifndef X86TERM_EVENTS_H
#define X86TERM_EVENTS_H
#include "interrupts.h"

#define X86EVENT_HANDLER_COUNT (16)
#define X86EVENT_INTERRUPT_COUNT (32)

typedef int (*eventhandler_t)(registers_t state, void* user_data);

typedef struct { 
	eventhandler_t handler;
	void* user_data;
} handlerentry_t;

void x86event_init();
int x86event_register(short int_no, eventhandler_t handler_f, void* user_data);
int x86event_deregister(short int_no, eventhandler_t handler_f, void* user_data); /* Linear time */
void x86event_fire(registers_t state);

#endif