#include "termhandlers.h"
#include "keys.h"
#include "display.h"

ringbuf_t serial_inbuf, serial_outbuf, kb_inbuf;

VTerm* vterm;
VTermScreen* vscreen;
VTermScreenCallbacks vtsc;
/* Callbacks that we'll need  */
int term_damage(VTermRect rect, void* user)
{
    /* TODO: Reimplement in a less slow-ass way */
    VTermScreenCell cell;
    VTermPos pos;
    int row, col;
    for (row = rect.start_row; row < rect.end_row; row++)
    for (col = rect.start_col; col < rect.end_col; col++)
    {
        pos.col = col;
        pos.row = row;
        vterm_screen_get_cell(vscreen, pos, &cell);
        display_set_cursor(col, row);
        /* TODO: flatten RGB back down to text curs */
        if (!cell.attrs.reverse)
            display_set_pen(0x07);
        else 
            display_set_pen(0x70);
        display_set_cell(cell.chars[0]);
    }
    return 1;
}
int term_movecursor(VTermPos pos, VTermPos oldpos, int visible, void* user)
{
    /* User-visible cursor, not internal */
    return 0;
}
int term_moverect(VTermRect dest, VTermRect src, void* user)
{
    /* TODO: Implement efficient moverect */
    return 0;
}
int term_settermprop(VTermProp prop, VTermValue *val, void* user)
{
    /* All unnecessary visible crap, don't worry about it yet. */
    return 1;
}
void term_handlescancode(char scancode)
{
    vterm_input_push_char(vterm, 0, scan2byte(scancode, 0));
    return;
}
void term_handleserial(char serialbyte)
{
    vterm_push_bytes(vterm, &serialbyte, 1);
    return;    
}
void init_vterm(void)
{
    vterm = vterm_new(disp.height, disp.width);
    vscreen = vterm_obtain_screen(vterm);

    vtsc.damage = term_damage;
    vtsc.moverect = term_moverect;
    vtsc.movecursor = term_movecursor;
    vtsc.settermprop = term_settermprop;
    vtsc.setmousefunc = NULL;
    vtsc.bell = NULL;
    vtsc.resize = NULL;

    vterm_screen_set_callbacks(vscreen, &vtsc, NULL);

    ringbuf_init(&serial_inbuf);
    ringbuf_init(&serial_outbuf);
    ringbuf_init(&kb_inbuf);
    return;
}
