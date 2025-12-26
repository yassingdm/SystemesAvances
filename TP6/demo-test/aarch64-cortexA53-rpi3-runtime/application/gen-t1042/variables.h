#ifndef VARIABLES_H
#define VARIABLES_H

#include "scheduler.h"
#include "scheduler_data.h"
#include <stdint.h>

#define NB_TASKS 2  // !!!! A changer soi-même sinon ERREUR

// Variables du scheduler
extern Scheduler__scheduler_mem g_sched_mem;
extern Scheduler__scheduler_out g_sched_out;
extern int g_task_end[NB_TASKS];

// Variables de synchro
extern volatile int64_t loc_pc_0;
extern volatile int64_t loc_pc_1;
extern volatile int64_t loc_end;

#endif

