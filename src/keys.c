#include "keys.h"
#include "vterm_input.h"
#include "stdlib.h"
kbstate_t keyboard;
#define NOKEY       { .type = KEY_NONE }
#define ASCII(l,u)  { .type = KEY_ASCII,   .v = { .ascii = { .lower = l, .upper = u } } }
#define SPECIAL(s)  { .type = KEY_SPECIAL, .v = { .value = s } }
#define MODIFIER(m) { .type = KEY_MOD,     .v = { .modifier = m } }
/* See also  http://www.quadibloc.com/comp/scan.htm  */
static key_t keys[] = {
    NOKEY,
    /* First row: 0x01 - 0x0e */
    SPECIAL(VTERM_KEY_ESCAPE),
    ASCII('1','!'),
    ASCII('2','@'),
    ASCII('3','#'),
    ASCII('4','$'),
    ASCII('5','%'),
    ASCII('6','^'),
    ASCII('7','&'),
    ASCII('8','*'),
    ASCII('9','('),
    ASCII('0',')'),
    ASCII('-','_'),
    ASCII('=','+'),
    SPECIAL(VTERM_KEY_BACKSPACE),
    /* Second row: 0x0f - 0x1c */
    SPECIAL(VTERM_KEY_TAB),
    ASCII('q','Q'),
    ASCII('w','W'),
    ASCII('e','E'),
    ASCII('r','R'),
    ASCII('t','T'),
    ASCII('y','Y'),
    ASCII('u','U'),
    ASCII('i','I'),
    ASCII('o','O'),
    ASCII('p','P'),
    ASCII('[','{'),
    ASCII(']','}'),
    SPECIAL(VTERM_KEY_ENTER),
    /* Third row: 0x1d - 0x29 */
    MODIFIER(VTERM_MOD_CTRL), /* Ctrl really */
    ASCII('a','A'),
    ASCII('s','S'),
    ASCII('d','D'),
    ASCII('f','F'),
    ASCII('g','G'),
    ASCII('h','H'),
    ASCII('j','J'),
    ASCII('k','K'),
    ASCII('l','L'),
    ASCII(';',':'),
    ASCII('\'','"'),
    ASCII('`','~'),
    /* Fourth row: 0x2a - 0x36 */
    MODIFIER(VTERM_MOD_SHIFT),
    ASCII('\\','|'),
    ASCII('z','Z'),
    ASCII('x','X'),
    ASCII('c','C'),
    ASCII('v','V'),
    ASCII('b','B'),
    ASCII('n','N'),
    ASCII('m','M'),
    ASCII(',','<'),
    ASCII('.','>'),
    ASCII('/','?'),
    MODIFIER(VTERM_MOD_SHIFT),
    /* Others */
    NOKEY, /* PrintScreen */
    /* Fifth row: 0x38 - */
    MODIFIER(VTERM_MOD_ALT),
    ASCII(' ',' '),
};
struct {
    int code;
    key_t key;
} extended_keys[] = {
    { 0x47, SPECIAL(VTERM_KEY_HOME) },
    { 0x48, SPECIAL(VTERM_KEY_UP) },
    { 0x49, SPECIAL(VTERM_KEY_PAGEUP) },
    { 0x4b, SPECIAL(VTERM_KEY_LEFT) },
    { 0x4d, SPECIAL(VTERM_KEY_RIGHT) },
    { 0x4f, SPECIAL(VTERM_KEY_END) },
    { 0x50, SPECIAL(VTERM_KEY_DOWN) },
    { 0x51, SPECIAL(VTERM_KEY_PAGEDOWN) },
    { 0x52, SPECIAL(VTERM_KEY_INS) },
    { 0x53, SPECIAL(VTERM_KEY_DEL) },
};
key_t *lookup_keypress(unsigned char scancode)
{
    if(scancode == 0xE0) {
        keyboard.extended = 1;
        return NULL;
    }

    unsigned char up   = (scancode &  0x80);
    unsigned char code = (scancode & ~0x80);

    int extended = keyboard.extended;
    keyboard.extended = 0;

    key_t *key = NULL;;

    if(!extended) {
        if(code < sizeof(keys)/sizeof(keys[0]))
            key = &keys[code];
    }
    else {
        /* TODO: Since extended_keys is ordered this could be quicker as a
         * binary search */
        for(int i = 0; i < sizeof(extended_keys)/sizeof(extended_keys[0]); i++)
            if(extended_keys[i].code == code) {
                key = &extended_keys[i].key;
                break;
            }
    }

    if(!key)
        return NULL;

    switch(key->type) {
    case KEY_MOD:
        if(!up)
            keyboard.mods |=  key->v.modifier;
        else
            keyboard.mods &= ~key->v.modifier;
        return NULL;
    case KEY_NONE:
        return NULL;
    case KEY_ASCII:
    case KEY_SPECIAL:
        if(!up)
            return key;
        else
            return NULL;
    }

    /* UNREACHED */
    return NULL;
}
