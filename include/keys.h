#ifndef KEYS_H
#define KEYS_H
#include "stdint.h"
typedef struct KBSTATE_T {
    /* Not a lot here yet, but it'll get more interesting when I start paying attention to the clock and keyrepeat */
    char mods;
} kbstate_t;
extern kbstate_t keyboard;
char scan2byte(char code, char mods);
char handle_keypress(char scancode);
#endif
