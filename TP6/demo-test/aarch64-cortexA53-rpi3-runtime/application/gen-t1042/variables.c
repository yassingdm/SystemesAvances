#include "variables.h"


Scheduler__scheduler_mem g_sched_mem;
Scheduler__scheduler_out g_sched_out;

int g_task_end[NB_TASKS];

volatile int64_t loc_pc_0 = -1;
volatile int64_t loc_pc_1 = -1;
volatile int64_t loc_end  = -1;
