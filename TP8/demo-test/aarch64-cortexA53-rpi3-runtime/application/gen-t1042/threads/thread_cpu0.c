#include "../syncvars.h"
#include "../scheduler.h"
#include "../tasks.h"
#include "../../api_lopht.h"
#include <librpi3/arm-config.h>
#include <librpi3/svc.h>

Scheduler__scheduler_mem mem;
Scheduler__scheduler_out out;
#define NB_TASKS_CONST 2
int task_end[NB_TASKS_CONST];
void global_init() {
    Scheduler__scheduler_reset(&mem);
    for(int i=0; i<NB_TASKS_CONST; i++) {
        task_end[i] = 0;
    }
}
void mif_entry_point_cpu0(void) {
    for(int i=0; i<NB_TASKS_CONST; i++) {
        task_end[i] = 1;
    }
    Scheduler__scheduler_step(task_end, &out, &mem);
    
    UPDATE_CPU(loc_pc_0, 1);
    
    int task_to_run = out.task_run[0];
    if (task_to_run == 0) {
        task0();
    } else if (task_to_run == 1) {
        task1();
    }
    WAIT_CPU(loc_pc_1, 2);
    loc_pc_1 = -1;
    UPDATE_CPU(loc_pc_0, -1);
}
