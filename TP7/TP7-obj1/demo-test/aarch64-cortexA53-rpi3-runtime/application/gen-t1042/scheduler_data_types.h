/* --- Generated the 1/12/2025 at 15:50 --- */
/* --- heptagon compiler, version 1.05.00 (compiled sun. nov. 23 20:57:45 CET 2025) --- */
/* --- Command line: /home/mrwnito/.opam/heptagon-4.03/bin/heptc -target c scheduler_data.ept externc.epi scheduler.ept --- */

#ifndef SCHEDULER_DATA_TYPES_H
#define SCHEDULER_DATA_TYPES_H

#include "stdbool.h"
#include "assert.h"
#include "pervasives.h"
typedef struct Scheduler_data__task_attributes {
  int period;
  int capacity;
  int deadline;
  int first_start;
} Scheduler_data__task_attributes;

static const int Scheduler_data__ntasks = 2;

static const int Scheduler_data__nproc = 2;

static const Scheduler_data__task_attributes Scheduler_data__tasks[2] = {
{5, 1, 5, 0}, {7, 1, 7, 0}};

typedef enum {
  Scheduler_data__Running,
  Scheduler_data__Ready,
  Scheduler_data__Waiting
} Scheduler_data__task_state;

Scheduler_data__task_state Scheduler_data__task_state_of_string(char* s);

char* string_of_Scheduler_data__task_state(Scheduler_data__task_state x,
                                           char* buf);

typedef struct Scheduler_data__task_status {
  Scheduler_data__task_state status;
  int current_proc;
  int current_deadline;
  int left;
} Scheduler_data__task_status;

typedef struct Scheduler_data__scheduler_state {
  int current_date;
  Scheduler_data__task_status tasks[2];
} Scheduler_data__scheduler_state;

static const Scheduler_data__scheduler_state Scheduler_data__init_sstate = {
-1,
{{Scheduler_data__Waiting, Scheduler_data__nproc, 0, 0},
 {Scheduler_data__Waiting, Scheduler_data__nproc, 0, 0}}};

#endif // SCHEDULER_DATA_TYPES_H
