#include "termhandlers.h"
#include "keys.h"
#include "display.h"
#include "stdlib.h"
ringbuf_t serial_inbuf, serial_outbuf, kb_inbuf;

VTerm* vterm;
VTermScreen* vscreen;
/* Callbacks that we'll need  */
int term_damage(VTermRect rect, __unused void* user)
{
    /* TODO: Reimplement in a less slow-ass way */
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
int term_movecursor(VTermPos pos, VTermPos oldpos, int visible, __unused void* user)
{
    /* User-visible cursor, not internal */
    return 1;
}
int term_moverect(VTermRect dest, VTermRect src, __unused void* user)
{
    /* TODO: Implement efficient moverect */
    return 1;
}
int term_settermprop(VTermProp prop, VTermValue *val, __unused void* user)
{
    /* All unnecessary visible crap, don't worry about it yet. */
    return 1;
}
int term_bell(__unused void* user)
{
    return 1;
}
int term_resize(int rows, int cols, void* user)
{
    return 1;
}
int term_setmousefunc(VTermMouseFunc func, void *data, void *user)
{
    return 1;
}
void term_handlescancode(char scancode)
{
    
    vterm_input_push_char(vterm, keyboard.mods, scan2byte(scancode, 0));
    return;
}
void term_handleserial(char serialbyte)
{
    vterm_push_bytes(vterm, &serialbyte, 1);
    return;    
}
static VTermScreenCallbacks vtsc =
{
    .damage = &term_damage,
    .moverect = &term_moverect,
    .movecursor = &term_movecursor,
    .settermprop = &term_settermprop,
    .setmousefunc = &term_setmousefunc,
    .bell = &term_bell,
    .resize = &term_resize,
};
void init_vterm(void)
{
    vterm = vterm_new(disp.height, disp.width);

    vscreen = vterm_obtain_screen(vterm);
    vterm_screen_set_callbacks(vscreen, &vtsc, NULL);
    vterm_screen_reset(vscreen);
    ringbuf_init(&serial_inbuf);
    ringbuf_init(&serial_outbuf);
    ringbuf_init(&kb_inbuf);
    return;
}
