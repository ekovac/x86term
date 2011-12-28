#include "pic.h"
#include "types.h"
#include "keys.h"
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

struct IDTDescr{
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t zero;      // unused, set to 0
   uint8_t type_attr; // type and attributes, see below
   uint16_t offset_2; // offset bits 16..31
} __attribute__((packed));
typedef struct IDTDescr IDTEntry;
IDTEntry interrupt_list[256];
/*
extern void (*simple_isr)(void);
extern void (*set_lidt)(void*, short);
extern void (*set_gdt)(void*, short);
*/
extern void dontcare_isr(void);
extern void enable_interrupts(void);
extern void disable_interrupts(void);
extern void kb_isr(void);
extern void serial_isr(void);
extern void set_lidt(void*, short);
extern void set_gdt(void*, short);
#define VIDEORAM ((unsigned char*)0xb8000)
volatile struct DISPLAY_STATE {
    unsigned short rows, columns;
    unsigned short cur_row, cur_column;
    volatile unsigned char* vidram;
} state;
volatile struct KB_STATE {
    char mods;
} kb_state;
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
void term_clear()
{
    term_erase(0,0,state.columns, state.rows);
    state.cur_row = 0;
    state.cur_column = 0;
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
#define PORT 0x3F8
void serial_init()
{
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x0C);    // Set divisor to 12 (lo byte,) 9600 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // RTS/DSR set    
   outb(PORT + 1, 0x01);    // Enable interrupts
}
char xlate_scans(char byte)
{
    char up;
    char key;
    up = byte & 0x80;
    key = byte & ~0x80;
    /* Handle modifiers */
    if (key == 0x2A || key == 0x36)
    {
        if (!up)
            kb_state.mods |= 0x01;
        else
            kb_state.mods &= ~0x01;
        return 0x00;
    }
    else if (key == 0x1D)
    {
        if (!up)
            kb_state.mods |= 0x02;
        else
            kb_state.mods &= ~0x02;
        return 0x00;
    }
    else if (key == 0x38)
    {
        if (!up)
            kb_state.mods |= 0x04;
        else
            kb_state.mods &= ~0x04;
        return 0x00;
    }
    else /* It's a single-strike key. */
    {
        return scan2byte(key, kb_state.mods);
    }
}
void print_mods()
{
    if (kb_state.mods & 0x01)
        print(" [SHIFT] ");
    if (kb_state.mods & 0x02)
        print(" [CTRL] ");
    if (kb_state.mods & 0x04)
        print(" [ALT] ");
}
void interrupt_handler(char irq)
{   
    term_clear();
    char inval;
    char fmt[4] = {'\'', '\0', '\'', '\0'};
    print("Received interrupt: ");
    print_byte(irq); print("\n");
    if (irq == 1)
    {
        print("Read KB byte: ");
        inval = inb(0x60);
        print_byte(inval); print("\n");
        fmt[1] = xlate_scans(inval);
        print(fmt); 
        print("\n");
        print_mods();
        
    }
    if (irq == 4)
    {
        print("Read serial byte: ");
        print_byte(inb(PORT));
        print("\n");
    }
    outb(0x20, 0x20);
    return;
}
void split_addr(void* addr, uint16_t* top, uint16_t* bottom)
{
    *bottom = ((long)addr & 0x0000FFFF);
    *top = ((long)addr & 0xFFFF0000) >> 16;
    return;
}
void interrupt_init(void)
{
    int i;
    uint16_t offset_top, offset_bottom;
    /* Zero out the interrupt table. */
    split_addr(dontcare_isr, &offset_top, &offset_bottom);
    for (i=0; i<32; i++)
    {
        interrupt_list[i].offset_1 = offset_bottom;
        interrupt_list[i].offset_2 = offset_top;
        interrupt_list[i].selector = 0x08;
        interrupt_list[i].zero = 0;
        interrupt_list[i].type_attr = 0x8E; 
    }
    for (i=32; i<256; i++)
    {
        interrupt_list[i].type_attr = 0x00;
    }
    split_addr(kb_isr, &offset_top, &offset_bottom);
    interrupt_list[0x21].offset_1 = offset_bottom;
    interrupt_list[0x21].offset_2 = offset_top;
    interrupt_list[0x21].selector = 0x08;
    interrupt_list[0x21].zero = 0;
    interrupt_list[0x21].type_attr = 0b10001110;
    interrupt_list[0x24] = interrupt_list[1];
    split_addr(serial_isr, &offset_top, &offset_bottom);
    interrupt_list[0x24].offset_1 = offset_bottom;
    interrupt_list[0x24].offset_2 = offset_top;
    return;
}
void kmain( void* mbd, unsigned int magic )
{
    int i;
    kb_state.mods = 0;
    display_init(mbd, magic);
    term_erase(0, 0, state.columns, state.rows);
    print("Hello World!\nThis is my first operating system.\n");
    serial_init();
    interrupt_init();
    set_gdt(gdt, 4*8+1);
    set_lidt(interrupt_list, 256*sizeof(IDTEntry));
    PIC_remap(0x20, 0x28);
    for (i=0; i<8; i++) IRQ_set_mask(i);
    IRQ_clear_mask(1);
    IRQ_clear_mask(4);
    print("\nWe made it here without exploding.\n");
    enable_interrupts();
    while (1)
    {
        asm("xchg %eax, %eax");    
    } 
    /* Print a letter to screen to see everything is working: */
    return;
}

