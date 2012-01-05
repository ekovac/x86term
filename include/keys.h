#ifndef KEYS_H
#define KEYS_H
#include "stdint.h"
typedef struct KBSTATE_T {
    /* Not a lot here yet, but it'll get more interesting when I start paying attention to the clock and keyrepeat */
    char mods;
    int extended;
} kbstate_t;
typedef struct {
    enum { KEY_NONE, KEY_ASCII, KEY_SPECIAL, KEY_MOD } type;
    union {
      struct {
        char lower, upper;
      } ascii;
      int value;
      int modifier;
    } v;
} key_t;
extern kbstate_t keyboard;
key_t *lookup_keypress(unsigned char scancode);
#endif
