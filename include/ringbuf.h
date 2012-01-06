#ifndef X86TERM_RINGBUF_H
#define X86TERM_RINGBUF_H
#include "stdint.h"
#define RINGBUF_LEN (64) 
typedef struct RINGBUF_STRUCT {
    uint8_t data[RINGBUF_LEN];
    size_t front, back;
} ringbuf_t;

void ringbuf_init(ringbuf_t* buf);
uint8_t ringbuf_popfront(ringbuf_t* buf);
void ringbuf_pushback(ringbuf_t* buf, uint8_t b);
uint8_t ringbuf_isempty(ringbuf_t* buf);
#endif
