#include "../scheduler.h"
#include "../tasks.h"
#include "../syncvars.h"
#include "../../api_lopht.h"
#include <librpi3/arm-config.h>
#include <librpi3/svc.h>
Scheduler__scheduler_mem mem;
Scheduler__scheduler_out out;
#define NB_TASKS_CONST 5
#define NB_PROC_CONST  3

int task_end[NB_TASKS_CONST];
static void delay_visual(void){
    for (volatile unsigned long long i = 0; i < 500000000ULL; i++) {
        __asm__("nop");
    }
}
void global_init(void){
    Scheduler__scheduler_reset(&mem);
    for (int i = 0; i < NB_TASKS_CONST; i++) task_end[i] = 0;
    loc_pc_0 = -1;
    loc_pc_1 = -1;
    loc_pc_2 = -1;
}

static inline void run_task(int tid){
    if (tid == 0) task0();
    else if (tid == 1) task1();
    else if (tid == 2) task2();
    else if (tid == 3) task3();
    else if (tid == 4) task4();
}

void mif_entry_point_cpu0(void){
    static int prev_task_run[NB_PROC_CONST]={
        NB_TASKS_CONST, NB_TASKS_CONST, NB_TASKS_CONST
    };

    for (;;){
        for (int i=0;i<NB_TASKS_CONST;i++)task_end[i] = 0;
        for (int p=0;p<NB_PROC_CONST;p++) {
            int tid=prev_task_run[p];
            if (tid>=0 && tid<NB_TASKS_CONST) task_end[tid]= 1;
        }
        Scheduler__scheduler_step(task_end,&out,&mem);
        UPDATE_CPU(loc_pc_0,1);
        run_task(out.task_run[0]);
        WAIT_CPU(loc_pc_1,2);
        WAIT_CPU(loc_pc_2,2);
        for (int p = 0;p<NB_PROC_CONST; p++)
            prev_task_run[p] = out.task_run[p];
        loc_pc_1 =-1;
        loc_pc_2 =-1;
        UPDATE_CPU(loc_pc_0,-1);
        delay_visual();
    }
}
