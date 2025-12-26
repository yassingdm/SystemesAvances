#include <stdio.h>

#include "threads.h"
#include "variables.h"
#include "scheduler.h"
#include "scheduler_data.h"
#include "externc.h"
#include "api_lopht.h"

void mif_entry_point_cpu1(void)
{
    WAIT_CPU(loc_pc_0, 1);

    int tid1 = g_sched_out.task_run[1];

    switch (tid1) {
        case 0:
            task0();
            g_task_end[0] = 1;
            break;

        case 1:
            task1();
            g_task_end[1] = 1;
            break;

        case Scheduler_data__ntasks:
            break;

        default:
            printf("Erreur: task_run[1] = %d\n", tid1);
            break;
    }

    UPDATE_CPU(loc_pc_1, 2);

    WAIT_END(loc_pc_0);
}
