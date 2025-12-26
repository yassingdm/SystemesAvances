/* --- Generated the 23/11/2025 at 17:56 --- */
/* --- heptagon compiler, version 1.05.00 (compiled mon. sep. 15 11:46:4 CET 2025) --- */
/* --- Command line: /Users/omarvatchagaev/.opam/default/bin/heptc -target c ../scheduler_types.ept --- */

#ifndef SCHEDULER_TYPES_TYPES_H
#define SCHEDULER_TYPES_TYPES_H

#include "stdbool.h"
#include "assert.h"
#include "pervasives.h"
typedef struct Scheduler_types__task_attributes {
  int period;
  int capacity;
  int deadline;
  int first_start;
} Scheduler_types__task_attributes;

static const int Scheduler_types__ntasks = 2;

static const int Scheduler_types__nproc = 2;

static const int Scheduler_types__int_max = 99999999;

static const Scheduler_types__task_attributes Scheduler_types__tasks[2] = {
{5, 1, 5, 0}, {7, 1, 7, 0}};

typedef enum {
  Scheduler_types__Running,
  Scheduler_types__Ready,
  Scheduler_types__Waiting
} Scheduler_types__task_state;

Scheduler_types__task_state Scheduler_types__task_state_of_string(char* s);

char* string_of_Scheduler_types__task_state(Scheduler_types__task_state x,
                                            char* buf);

typedef struct Scheduler_types__task_status {
  Scheduler_types__task_state status;
  int current_proc;
  int current_deadline;
} Scheduler_types__task_status;

typedef struct Scheduler_types__scheduler_state {
  int current_date;
  Scheduler_types__task_status tasks[2];
} Scheduler_types__scheduler_state;

static const Scheduler_types__scheduler_state Scheduler_types__init_sstate = {
-1,
{{Scheduler_types__Waiting, Scheduler_types__nproc, 0},
 {Scheduler_types__Waiting, Scheduler_types__nproc, 0}}};

#endif // SCHEDULER_TYPES_TYPES_H
