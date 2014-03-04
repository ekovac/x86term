#ifndef X86TERM_TERMHANDLERS_H
#define X86TERM_TERMHANDLERS_H
#include "vterm.h"
#include "ringbuf.h"

extern VTerm* vterm;
void term_handlescancode(char);
void term_handleserial(char);
void term_puts(char* str);
void init_vterm(void);

#endif
