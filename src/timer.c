#include "timer.h"
#include "display.h"
#define PIT_IO (0x40)
#define SPEAKER_IO (0x42)
#define PITMODE_IO (0x43)
#define PORTB_IO (0x61)
#define BASE_FREQ (1193180)
uint32_t beep_remaining = 0;
void timer_handler(void)
{
    if (beep_remaining > 0) 
    {
        beep_remaining--;
        if (beep_remaining == 0)
            beep_disable();
    }
    /* TODO: Cursor blink. */
    outb(0x20, 0x20); /* Acknowledge interrupt. */
    return;
}
/* http://wiki.osdev.org/PC_Speaker */
void timer_config(uint8_t pit_channel, uint32_t freq_hz)
{
    if (pit_channel > 2) return;
        /* bogus pit channel, silently do nothing. */
    uint16_t freq_div = BASE_FREQ / freq_hz;
    outb(PITMODE_IO, 0xb6);
    outb(PIT_IO+pit_channel, (uint8_t)freq_div);
    outb(PIT_IO+pit_channel, (uint8_t)freq_div >> 8);
    return;
}
void beep_enable(void)
{
    uint8_t tmp = inb(PORTB_IO) | 3;
    outb(PORTB_IO, tmp);
    return;
}
void beep_disable(void)
{
    uint8_t tmp = inb(PORTB_IO) & (0xFF ^ 3);
    outb(PORTB_IO, tmp);
    return;
}
void beep_beep(uint32_t freq_hz, uint16_t time_ms)
{
    beep_remaining = time_ms;
    timer_config(2, freq_hz);
    beep_enable();
}
