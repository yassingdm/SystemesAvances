#include "../syncvars.h"
#include "../scheduler.h"
#include "../tasks.h"
#include "../syncvars.h"
#include "../../api_lopht.h"
#include <librpi3/arm-config.h>
extern Scheduler__scheduler_out out;
#ifndef WAIT_END
#define WAIT_END(var) while(api_ldar(&var) != -1)
#endif

static inline void run_task(int tid) {
    if (tid == 0) task0();
    else if (tid == 1) task1();
    else if (tid == 2) task2();
    else if (tid == 3) task3();
    else if (tid == 4) task4();
}
void mif_entry_point_cpu1(void) {
    for (;;) {
        WAIT_CPU(loc_pc_0, 1);
        int task_to_run = out.task_run[1];
        run_task(task_to_run);
        UPDATE_CPU(loc_pc_1, 2);
        WAIT_END(loc_pc_0);
    }
}
