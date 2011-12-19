typedef struct {
    unsigned char data;
    unsigned char attr;
} VideoCell;
#define VIDEORAM ((unsigned char*)0xb8000)
volatile struct DISPLAY_STATE {
    unsigned short rows, columns;
    unsigned short cur_row, cur_column;
    volatile unsigned char* vidram;
} state;
void term_putchar(unsigned char x, unsigned char y, unsigned char c, unsigned char attr)
{
    state.vidram[y*state.columns*2+x*2] = c;
    state.vidram[y*state.columns*2+x*2+1] = attr;
}
void advance_cur(unsigned short amount)
{
    state.cur_row = (state.cur_row + ((state.cur_column + amount) / state.columns)) % state.rows;
    state.cur_column = ((state.cur_column + amount) % state.columns);
    return;
}
void term_erase(unsigned char x1, unsigned char y1, unsigned char width, unsigned char height)
{
    unsigned char x, y;
    for (x = x1; x <= x1+width; x++)
    for (y = y1; y <= y1+height; y++)
        term_putchar(x,y, 0x00, 0x07);
    return;
}
void dumb_print(char* str)
{
    unsigned char c;
    unsigned char attr = 0x07;
    while (c = *(str++))
    {
        if (c == '\n') 
        {
            state.cur_row++;
            state.cur_column = 0;    
        }
        else 
        {
            term_putchar(state.cur_column,state.cur_row, c, attr);
            advance_cur(1);
        }
    }
    return;
}

void display_init(void* mbd, unsigned int magic)
{
    if ( magic != 0x2BADB002 )
    {
        term_erase(0,0,80,25);
        dumb_print("Something went horribly wrong with multiboot.\n");
    }
    /* Dummy for now. */
    state.rows = 25;
    state.columns = 80;
    state.cur_row = 0;
    state.cur_column = 0;
    state.vidram = VIDEORAM;
}
void kmain( void* mbd, unsigned int magic )
{
    display_init(mbd, magic);
    term_erase(0, 0, state.columns, state.rows);
    dumb_print("Hello World!\nThis is my first operating system.\n");


    /* Print a letter to screen to see everything is working: */
}

