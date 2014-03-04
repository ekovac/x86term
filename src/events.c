#include "events.h"
#include "pic.h"
#include "string.h"
#include "termhandlers.h"
#include "display.h"

static handlerentry_t handlers[X86EVENT_INTERRUPT_COUNT][X86EVENT_HANDLER_COUNT];
void x86event_init()
{
	memset(handlers, 0, sizeof(handlers));
	return;
}

void x86event_unhandled(registers_t state)
{
	static int count = 3;
	puts("Unhandled Interrupt: ");
	put_bytes((uint8_t*)&(state.int_no), sizeof(int)); puts(" ");
	put_bytes((uint8_t*)&(state.err_code), sizeof(int)); puts("\n");
	puts("Executing instruction: ");
	put_bytes((uint8_t*)&(state.eip), sizeof(int)); puts("\n");
	count--;
	if (count == 0) __asm__("hlt");
}
void x86event_fire(registers_t state)
{
	int i;
	int retval;
	handlerentry_t entry;

	__asm__("cli");

	for (i = 0; i < X86EVENT_HANDLER_COUNT; i++)
	{
		entry = handlers[state.int_no][i];
		if (entry.handler == NULL) continue;
		retval = entry.handler(state, entry.user_data);
		if (retval > 0) break;
	}
	
	if (!retval) x86event_unhandled(state);
	else pic_acknowledge();

	__asm__("sti");
	return;
}
/* in an eventhandler_t, returning 0 means completely unhandled, returning -1 means it might be unhandled. Other retvals mean handled. */
int x86event_register(short int_no, eventhandler_t handler_f, void* user_data)
{
	int i;
	handlerentry_t entry;
	for (i = 0; i < X86EVENT_HANDLER_COUNT; i++)
	{
		entry = handlers[int_no][i];
		if (entry.handler == NULL)
		{
			entry.handler = handler_f;
			entry.user_data = user_data;
			handlers[int_no][i] = entry;
			return 0;
		}
	}
	return 1; /* Could not find room */
}

int x86event_deregister(short int_no, eventhandler_t handler_f, void* user_data)
{
	int i;
	handlerentry_t entry;
	for (i = 0; i < X86EVENT_HANDLER_COUNT; i++)
	{
		entry = handlers[int_no][i];
		if (entry.handler == handler_f && entry.user_data == user_data)
		{
			entry.handler = NULL;
			entry.user_data = NULL;
			handlers[int_no][i] = entry;
			return 0;
		}
	}
	return 1; /* Could not find entry to remove */
}

