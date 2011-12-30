#include "ringbuf.h"

void ringbuf_init(ringbuf_t* buf)
{
    buf->front = 0;
    buf->back = 0;
    buf->len = 0;
    return;
}

void ringbuf_pushback(ringbuf_t* buf, uint8_t b)
{
    buf->data[buf->back] = b;
    if (++(buf->back) == RINGBUF_LEN) buf->back = 0;
    buf->len++;
    return;
}
uint8_t ringbuf_popfront(ringbuf_t* buf)
{
    uint8_t b = buf->data[buf->front];
    if (++(buf->front) == RINGBUF_LEN) buf->front = 0; 
    buf->len--;
    return b;
}
uint8_t ringbuf_isempty(ringbuf_t* buf)
{
    return !(buf->len);
}
