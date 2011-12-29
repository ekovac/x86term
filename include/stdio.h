#ifndef X86TERM_STDIO_H
#define X86TERM_STDIO_H
typedef void FILE;
FILE *stdin, *stderr, *stdout;
int fprintf(FILE *stream, const char *format, ...);
#endif
