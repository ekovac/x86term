#include ""

typedef struct {
	ringbuf_t* in_buf,
	ringbuf_t* out_buf
} keyboard_t;

void keyboard_init(keyboard_t* kb);
