#ifndef EXTERNC_H
#define EXTERNC_H

#include <stdio.h>
#include <stdlib.h>

#include "scheduler_types.h"
#include "scheduler_data_types.h"


typedef struct {
  int _dummy;
} Externc__deadline_miss_log_out;


typedef struct {
  int _dummy;
} Externc__print_scheduler_state_out;


void Externc__deadline_miss_log_step(int date,
                                     int task_id,
                                     Externc__deadline_miss_log_out* out);

void Externc__print_scheduler_state_step(struct Scheduler_data__scheduler_state s,
                                         Externc__print_scheduler_state_out* out);

void task0(void);
void task1(void);

#endif /* EXTERNC_H */