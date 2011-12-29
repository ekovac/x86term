#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include <stdarg.h>
#define __unused __attribute__((unused))
extern void* heapbase;
int fprintf(__unused FILE* stream, __unused const char* format, ...)
{
    return 0;
}
static void* heaptop = NULL;
void *malloc(size_t size)
{
    if (!heaptop) heaptop = heapbase;
    void* newmem;
    newmem = heaptop;
    heaptop += size;
    return newmem;
}
void free(__unused void *ptr)
{
    return; /* Herp derp */
}

void* memset(void* s, int c, size_t n)
{   
    /* TODO: Optimize */
    unsigned int i;
    char* vp = (char*)s;
    for (i = 0; i < n; i++)
        vp[i] = (char)c;
    return s;
}
void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned int i;
    char *cdest, *csrc;
    cdest = (char*)dest;
    csrc = (char*)src;
    for (i = 0; i < n; i++)
        cdest[i] = csrc[i];
    return dest;
}
void *memmove(void *dest, const void *src, size_t n)
{
    int i;
    char *cdest, *csrc;
    cdest = (char*)dest;
    csrc = (char*)src;
    if (dest < src) 
        for (i=0; i < (int)n; i++) cdest[i] = csrc[i];
    else if (dest > src) 
        for (i=n-1; i>=0; i--) cdest[i] = csrc[i];
    return dest;
}
char *strncpy(char *dest, const char *src, size_t n)
{
    unsigned int i;
    for (i=0; i < n; i++)
    {
        dest[i] = src[i];
        if (src[i] == '\0') break;
    }
    return dest;
}
int strncmp(const char *s1, const char *s2, size_t n)
{
    unsigned int i;
    for (i = 0; i < n; i++)
        if ((s1[i] != s2[i]) || (s1[i] == 0)) return s1[i]-s2[i];
    return 0;
}

