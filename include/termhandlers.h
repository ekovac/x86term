#ifndef X86TERM_TERMHANDLERS_H
#define X86TERM_TERMHANDLERS_H
#include "vterm.h"
#include "ringbuf.h"

extern ringbuf_t serial_inbuf, serial_outbuf, kb_inbuf;
extern VTerm* vterm;
void term_handlescancode(char);
void term_handleserial(char);
void init_vterm(void);

#endif
