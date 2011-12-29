#ifndef X86TERM_STDLIB_H
#define X86TERM_STDLIB_H
typedef unsigned long size_t;
#define NULL (0x0)
void *malloc(size_t size);
void free(void *ptr);
#endif
