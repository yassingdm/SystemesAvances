#include "scheduler.h"
#include "scheduler_types.h"
#include "scheduler_data.h"
#include "scheduler_data_types.h"
#include "externc.h"

int main(void) {
  struct Scheduler_data__scheduler_state s  = Scheduler_data__init_sstate;
  struct Scheduler_data__scheduler_state so;    
  Scheduler__scheduler_out sched_out;           
  Externc__print_scheduler_state_out pout;     
  
  long cycle = 0;
  char line[8];
  
  puts("Ordonnanceur RM");
  while (1) {
    printf("\n[cycle %ld] Appuyez sur Entrée...", cycle++);
    if (!fgets(line, sizeof line, stdin)) break;
    
    Scheduler__scheduler_step(s, &sched_out);
    
    so = sched_out.so;
    
    Externc__print_scheduler_state_step(so, &pout);
    
    s = so;
  }
  return 0;
}