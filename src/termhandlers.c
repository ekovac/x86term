#include "termhandlers.h"
#include "keys.h"
#include "display.h"
#include "stdlib.h"
#include "timer.h"
#include "ringbuf.h"
#include <string.h>

VTermScreen* vscreen;
/* Callbacks that we'll need  */
int term_damage(VTermRect rect, __unused void* user)
{
    /* TODO: Reimplement in a less slow-ass way */
    VTerm* vterm = user;
    VTermScreenCell cell;
    VTermPos pos;
    uint8_t fg, bg, color;
    int row, col;
    for (row = rect.start_row; row < rect.end_row; row++)
    for (col = rect.start_col; col < rect.end_col; col++)
    {
        pos.col = col;
        pos.row = row;
        vterm_screen_get_cell(vscreen, pos, &cell);
        fg = rgb2vga(cell.fg.red, cell.fg.green, cell.fg.blue);
        bg = rgb2vga(cell.bg.red, cell.bg.green, cell.bg.blue);

        if (cell.attrs.reverse)
            color = bg | (fg << 4);
        else
            color = fg | (bg << 4);
        display_put(col, row, cell.chars[0], color);
    }
    return 1;
}
int term_movecursor(VTermPos pos, __unused VTermPos oldpos, __unused int visible, __unused void* user)
{
    /* User-visible cursor, not internal */
    display_set_cursor(pos.col, pos.row);
    return 1;
}
int term_moverect(VTermRect dest, VTermRect src, __unused void* user)
{
    int downward  = dest.start_row - src.start_row;
    int rightward = dest.start_col - src.start_col;
    for(int row = downward <= 0 ? dest.start_row : dest.end_row-1;
            row >= dest.start_row && row < dest.end_row;
            downward <= 0 ? row++ : row--) {
        for(int col = rightward <= 0 ? dest.start_col : dest.end_col-1;
                col >= dest.start_col && col < dest.end_col;
                rightward <= 0 ? col++ : col--) {
            display_copycell(col, row, col - rightward, row - downward);
        }
    }
    return 1;
}
int term_settermprop(__unused VTermProp prop, __unused VTermValue *val, __unused void* user)
{
    /* All unnecessary visible crap, don't worry about it yet. */
    return 1;
}
int term_bell(__unused void* user)
{
    beep_beep(600,200);
    return 0;
}
void term_handlescancode(char scancode)
{
    key_t *key = lookup_keypress(scancode);
    if(!key)
        return;
    switch(key->type) {
    case KEY_NONE:
    case KEY_MOD:
        break;
    case KEY_ASCII:
        vterm_input_push_char(vterm, keyboard.mods & ~VTERM_MOD_SHIFT,
                keyboard.mods & VTERM_MOD_SHIFT ? key->v.ascii.upper : key->v.ascii.lower);
        break;
    case KEY_SPECIAL:
        vterm_input_push_key(vterm, keyboard.mods, key->v.value);
        break;
    }
    return;
}
void term_handleserial(char serialbyte)
{
    vterm_push_bytes(vterm, &serialbyte, 1);
    return;    
}
void term_puts(char* str)
{   
    vterm_push_bytes(vterm, str, strlen(str));
    return; 
}
static VTermScreenCallbacks vtsc =
{
    .damage = &term_damage,
    .moverect = &term_moverect,
    .movecursor = &term_movecursor,
    .settermprop = &term_settermprop,
    .setmousefunc = NULL,
    .bell = &term_bell,
    .resize = NULL,
};
void init_vterm(void)
{
    vterm = vterm_new(disp.height, disp.width);

    vscreen = vterm_obtain_screen(vterm);
    VTerm* vterm;
    vterm_screen_set_callbacks(vscreen, &vtsc, vterm);
    vterm_screen_enable_altscreen(vscreen, 1);
    vterm_screen_reset(vscreen, 1);
    
    return;
}
