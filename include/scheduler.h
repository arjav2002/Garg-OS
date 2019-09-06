#ifndef SCHEDULER_H
#define SCHEDULER_H

#define PRIORITY0_TIME_SLICE_MILLIS	100
#define PRIORITY1_TIME_SLICE_MILLIS	50
#define PRIORITY2_TIME_SLICE_MILLIS	25
#define PRIORITY3_TIME_SLICE_MILLIS	10

extern void init_scheduler();
extern int start_next_process(regs_t* current_proc_regs);
extern uint32_t add_process(uint32_t stack_size, uint32_t code_size, regs_t registers, uint8_t* code, uint8_t priority_level);

#endif
