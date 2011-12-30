#ifndef X86TERM_TABLES_H
#define X86TERM_TABLES_H
#include "stdint.h"
void gdt_flush(uint32_t gdt_desc);
void idt_flush(uint32_t idt_desc);
#endif
