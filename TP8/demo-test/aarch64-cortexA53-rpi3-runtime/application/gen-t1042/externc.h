#ifndef EXTERNC_H
#define EXTERNC_H
#include "stdbool.h"
#include "assert.h"
#include "pervasives.h"

typedef struct {
  int _dummy;
} Externc__deadline_miss_log_out;
void Externc__deadline_miss_log_step(int date,
                                     int task_id,
                                     Externc__deadline_miss_log_out* out);
#endif
