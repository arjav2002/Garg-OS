#include "system.h"
#include "screen.h"
#include "timer.h"
#include "scheduler.h"
#include "process_list.h"
#include "paging.h"
#include "system.h"

#include <stdint.h>

uint32_t timer_ticks = 0;
uint32_t freq = 18;
uint32_t time_slice_millis = 0;
extern element* current_element;

void timer_handler(regs_t *r) {
	timer_ticks++;
	if(timer_ticks == time_slice_millis * freq / 1000) {
		set_time_slice(start_next_process(r));
		timer_ticks = 0;
	}
}

void timer_install() {
	irq_install_handler(0, timer_handler);
}

void set_time_slice(uint32_t millis) {
	time_slice_millis = millis;
}

void set_timer_freq(uint32_t hz) {
	if(hz > 11931080/65335)
		return;
	int16_t divisor = 1193180 / hz;
	outb(0x43, 0x36);
	outb(0x40, divisor & 0xFF);
	outb(0x40, divisor >> 8);
	freq = hz;
}
