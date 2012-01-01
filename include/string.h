#ifndef X86TERM_STRING_H
#define X86TERM_STRING_H
#include "stdlib.h"
#include <stdarg.h>
void* memset(void* s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
int vsnprintf(char *str, size_t size, const char *format, ...);
#endif
