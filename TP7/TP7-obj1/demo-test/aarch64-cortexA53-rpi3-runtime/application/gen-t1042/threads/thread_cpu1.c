#include "../syncvars.h"
#include "../scheduler.h"
#include "../tasks.h"
#include "../../api_lopht.h"
#include <librpi3/arm-config.h>
extern Scheduler__scheduler_out out;
#ifndef WAIT_END
#define WAIT_END(var) while(api_ldar(&var) != -1)
#endif

void mif_entry_point_cpu1(void) {
    WAIT_CPU(loc_pc_0, 1);
    int task_to_run = out.task_run[1];
    if (task_to_run == 0) {
        task0();
    } else if (task_to_run == 1) {
        task1();
    }
    UPDATE_CPU(loc_pc_1, 2);
    WAIT_END(loc_pc_0);
}
