#ifndef SCHEDULER_H
#define SCHEDULER_H

#define PRIORITY0_TIME_SLICE_MILLIS	100
#define PRIORITY1_TIME_SLICE_MILLIS	50
#define PRIORITY2_TIME_SLICE_MILLIS	25
#define PRIORITY3_TIME_SLICE_MILLIS	10

#include "process.h"

extern process* proc_arr;

extern void init_scheduler();
extern int start_next_process(regs_t* current_proc_regs);

#endif
