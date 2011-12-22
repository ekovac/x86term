typedef struct {
    unsigned char data;
    unsigned char attr;
} VideoCell;
static unsigned long long gdt[5] = {
0x0000000000000000,
0x00CF9A000000FFFF,
0x00CF92000000FFFF,
0x00CFFA000000FFFF,
0x00CFF2000000FFFF};
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef struct IDTDescr{
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t zero;      // unused, set to 0
   uint8_t type_attr; // type and attributes, see below
   uint16_t offset_2; // offset bits 16..31
} IDTEntry;
IDTEntry interrupt_list[16];
extern void (*set_lidt)(void*, int);
extern void (*set_gdt)(void*, int);
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
void print_bytes_big(void* addr, short len)
{
    int i;
    char* b_addr = (char*)addr;
    print("0x");
    for (i=len-1; i>=0; i--)
    {
        print_byte(b_addr[i]);
    }
    return;
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
void interrupt_handler(void)
{
    return;
}
void kmain( void* mbd, unsigned int magic )
{
    int i;
    display_init(mbd, magic);
    term_erase(0, 0, state.columns, state.rows);
    print("Hello World!\nThis is my first operating system.\n");
    for (i=0; i<5; i++)
    {
        print("\n");
        print_bytes_big(gdt+i, sizeof(gdt[i]));
    }
    set_gdt(gdt, 5);
    print("\nWe made it here without exploding.\n");
    /* Print a letter to screen to see everything is working: */
}

