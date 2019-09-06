#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>
void set_timer_freq(uint32_t hz);
void set_time_slice(uint32_t millis);
#endif
