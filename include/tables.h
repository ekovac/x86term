#ifndef X86TERM_TABLES_H
#define X86TERM_TABLES_H
#include "types.h"
void gdt_flush(void* gdt_desc);
void idt_flush(void* idt_desc);
#endif
