typedef struct {
    unsigned char data;
    unsigned char attr;
} VideoCell;
#include "gdt.h"
extern uint64_t* gdt;
extern void (*flush_segments)(void);
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
void print(char* str)
{
    unsigned char c;
    unsigned char attr = 0x07;
    while ((c = *(str++)))
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
char hexits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
void print_byte(unsigned char val)
{
    char str[3];
    str[2] = 0x00;
    str[1] = hexits[val & 0x0F];
    str[0] = hexits[(val & 0xF0) >> 4];
    print(str);
}
void print_bytes(void* addr, short len)
{
    int i;
    char* b_addr = (char*)addr;
    print("0x");
    for (i=0; i<len; i++)
    {
        print_byte(b_addr[i]);
    }
    return;
}
void display_init(void* mbd, unsigned int magic)
{
    if ( magic != 0x2BADB002 )
    {
        term_erase(0,0,80,25);
        print("Something went horribly wrong with multiboot.\n");
    }
    /* Dummy for now. */
    state.rows = 25;
    state.columns = 80;
    state.cur_row = 0;
    state.cur_column = 0;
    state.vidram = VIDEORAM;
}
void* p_gdtr;
void load_gdt(void* loc, short length)
{
    struct GDTR {
        short length;
        void* loc;
    } gdtr;
    gdtr.loc = loc;
    gdtr.length = length - 1;
    p_gdtr = &gdtr;
    __asm__("lgdt p_gdtr");
    flush_segments();
}
void kmain( void* mbd, unsigned int magic )
{
    display_init(mbd, magic);
    term_erase(0, 0, state.columns, state.rows);
    print("Hello World!\nThis is my first operating system.\n");
    gdt[0] = create_descriptor(0, 0, 0);
    gdt[1] = create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL0));
    gdt[2] = create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL0));
    load_gdt(gdt, 3);
    /* Print a letter to screen to see everything is working: */
    uint32_t test = 0xDEADBEEF;
    print("We made it here without exploding.\n");
    print_bytes(&test, sizeof(test));
}

