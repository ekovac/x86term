#ifndef X86_TIMER_H
#define X86_TIMER_H
#include "base.h"
#include "stdint.h"
extern uint32_t beep_remaining;
void timer_config(uint8_t pit_channel, uint32_t freq_hz);
void beep_enable(void);
void beep_disable(void);
void beep_beep(uint32_t freq_hz, uint16_t time_ms);
#endif
