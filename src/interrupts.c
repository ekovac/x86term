#include "interrupts.h"
#include "tables.h"
#include "interrupt_handlers.h"
#include "string.h"
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

void init_idt(void)
{
    int i=0;
    idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t)*256); 
    for (i = 0; i < 8; i++) idt_set_gate(i, (uint32_t)cpu_isr, 0x08, 0x8E);

    idt_set_gate(0x21, (uint32_t)kb_isr, 0x08, 0x8E);
    idt_set_gate(0x24, (uint32_t)serial_isr, 0x08, 0x8E);

    idt_flush((uint32_t)&idt_ptr);
    return;
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags;
}
