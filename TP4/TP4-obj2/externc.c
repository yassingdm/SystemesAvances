#include <stdio.h>
#include "externc.h"

void Externc__deadline_miss_log_step(int date, int task_id,
                                     Externc__deadline_miss_log_out* out)
{
  fprintf(stderr, "[MISS] tâche %d : deadline à t=%d\n", task_id, date);
  
  if (out) out->_dummy = 0;
}

void Externc__print_scheduler_state_step(struct Scheduler_data__scheduler_state s,
                                         Externc__print_scheduler_state_out* out)
{
  printf("t=%d | ", s.current_date);
  
  for (int i = 0; i < Scheduler_data__ntasks; ++i) {
    const struct Scheduler_data__task_status ts = s.tasks[i];
    const char* st =
      (ts.status == Scheduler_data__Running) ? "Run" :
      (ts.status == Scheduler_data__Ready)   ? "Ready" :
      "Wait";
    
    printf("T%d[%s,dl=%d,left=%d] ", i, st, ts.current_deadline, ts.left);
  }
  
  printf("\n");
  
  if (out) out->_dummy = 0;
}