#include <stdio.h>

#include "threads.h"
#include "variables.h"
#include "scheduler.h"
#include "scheduler_data.h"
#include "externc.h"
#include "api_lopht.h"

void global_init(void)
{
    Scheduler__scheduler_reset(&g_sched_mem);

    for (int i = 0; i < Scheduler_data__ntasks; i++) {
        g_task_end[i] = 0;
    }

    loc_pc_0 = -1;
    loc_pc_1 = -1;
    loc_end  = -1;
}

void mif_entry_point_cpu0(void)
{
    Scheduler__scheduler_step(g_task_end, &g_sched_out, &g_sched_mem);

    for (int i = 0; i < Scheduler_data__ntasks; i++) {
        g_task_end[i] = 0;
    }

    UPDATE_CPU(loc_pc_0, 1);

    int tid0 = g_sched_out.task_run[0];

    switch (tid0) {
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
            // Debug éventuel
            printf("Erreur: task_run[0] = %d\n", tid0);
            break;
    }

    WAIT_CPU(loc_pc_1, 2);

    loc_pc_1 = -1;
    
    UPDATE_CPU(loc_pc_0, -1);
}
