#ifndef X86TERM_STDLIB_H
#define X86TERM_STDLIB_H
typedef unsigned long size_t;
#define __unused __attribute__((unused))
#define NULL (0x0)
int abs(int i);
void *malloc(size_t size);
void free(void *ptr);
#endif
