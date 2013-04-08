#include "pic.h"
/* Ganked from osdev */
/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8 and 70, as configured by default */


void pic_getmap(int* offset1, int* offset2)
{
    // TODO   
} 
/*
arguments:
    offset1 - vector offset for master PIC
        vectors on the master become offset1..offset1+7
    offset2 - same for slave PIC: offset2..offset2+7
*/

void pic_remap(int offset1, int offset2)
{
    unsigned char a1, a2;
 
    a1 = inb(PIC1_DATA);                        // save masks
    a2 = inb(PIC2_DATA);
 
    outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, offset1);                 // define the PIC vectors
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();
    outb(PIC1_DATA, 4);                       // continue initialization sequence
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();
 
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
 
    outb(PIC1_DATA, a1);   // restore saved masks.
    outb(PIC2_DATA, a2);
}

void pic_add_mask(uint16_t mask) {
    pic_set_mask(pic_get_mask() | mask);
}

void pic_del_mask(uint16_t mask) {   
    pic_set_mask(pic_get_mask() & ~(mask));
}

void pic_set_mask(uint16_t mask) {
    uint8_t pic1_val = mask & 0xFF;
    uint8_t pic2_val = (mask >> 8) & 0xFF;
    outb(PIC1_DATA, pic1_val);
    outb(PIC2_DATA, pic2_val);
}

uint16_t pic_get_mask() {
    uint16_t pic1_val, pic2_val;
    uint16_t result;

    pic1_val = inb(PIC1_DATA);
    pic2_val = inb(PIC2_DATA);

    result = pic2_val << 8 | pic1_val;

    return result;
}
void pic_acknowledge() {
    outb(0x20, 0x20);
}