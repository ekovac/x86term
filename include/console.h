#ifndef X86TERM_CONSOLE_H
#define X86TERM_CONSOLE_H

#include "vterm.h"

typedef struct {
    VTerm* vterm;
    keyboard_t* kbd;
    serial_t* port;
    char active;

}


#endif