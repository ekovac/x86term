#include "ringbuf.h"
#include "display.h"
void ringbuf_init(ringbuf_t* buf)
{
    buf->front = 0;
    buf->back = 0;
    return;
}

void ringbuf_pushback(ringbuf_t* buf, uint8_t b)
{
    buf->data[buf->back] = b;
    if (++(buf->back) == RINGBUF_LEN) buf->back = 0;
    if (buf->back == buf->front) kpanic("Ring buffer overflowed.", buf);
    return;
}
uint8_t ringbuf_popfront(ringbuf_t* buf)
{
    uint8_t b = buf->data[buf->front];
    if (++(buf->front) == RINGBUF_LEN) buf->front = 0; 
    return b;
}
uint8_t ringbuf_isempty(ringbuf_t* buf)
{
    return buf->front == buf->back;
}
