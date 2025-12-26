#include <stdio.h>
#include "externc.h"

void Externc__deadline_miss_log_step(int date, int task_id,
                                     Externc__deadline_miss_log_out* out)
{
  fprintf(stderr, "[MISS] tâche %d : deadline à t=%d\n", task_id, date);
  
  if (out) out->_dummy = 0;
}

void Externc__print_scheduler_state_step(
    struct Scheduler_data__scheduler_state s,
    Externc__print_scheduler_state_out* out)
{
    (void)out; 

    char buf[128];

    snprintf(buf, sizeof buf,
             "RPI://===== cycle (date=%d) =====\n",
             s.current_date);
    console_puts(0, buf);

    for (int tid = 0; tid < Scheduler_data__ntasks; tid++) {
        const struct Scheduler_data__task_status *ts = &s.tasks[tid];

        const char *st =
            (ts->status == Scheduler_data__Running) ? "Run" :
            (ts->status == Scheduler_data__Ready)   ? "Ready" :
                                                      "Wait";

        snprintf(buf, sizeof buf,
                 "RPI:Task %d : status=%s, dl=%d, left=%d, cpu=%d\n",
                 tid, st, ts->current_deadline, ts->left, ts->current_proc);
        console_puts(0, buf);
    }
}
