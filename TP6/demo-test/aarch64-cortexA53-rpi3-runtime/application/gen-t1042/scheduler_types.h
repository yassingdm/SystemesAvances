/* --- Generated the 19/11/2025 at 19:0 --- */
/* --- heptagon compiler, version 1.05.00 (compiled tue. sep. 23 21:17:51 CET 2025) --- */
/* --- Command line: /home/yassi/.opam/4.14.1/bin/heptc -target c scheduler_data.ept externc.epi scheduler.ept --- */

#ifndef SCHEDULER_TYPES_H
#define SCHEDULER_TYPES_H

#include "stdbool.h"
#include "assert.h"
#include "pervasives.h"
#include "externc_types.h"
#include "scheduler_data_types.h"
typedef struct Scheduler__select_acc {
  int tid;
  int speriod;
} Scheduler__select_acc;

typedef struct Scheduler__aux_type {
  int sel_proc;
  int sel_period;
} Scheduler__aux_type;

#endif // SCHEDULER_TYPES_H
