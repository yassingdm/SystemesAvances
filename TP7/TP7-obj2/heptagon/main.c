#include <stdio.h>

#include "scheduler.h"
#include "scheduler_types.h"
#include "scheduler_data.h"
#include "scheduler_data_types.h"

int main(void) {
    Scheduler__scheduler_mem mem;
    Scheduler__scheduler_out out;

    int task_end[Scheduler_data__ntasks];

    Scheduler__scheduler_reset(&mem);

    for (int i = 0; i < Scheduler_data__ntasks; i++)
        task_end[i] = 0;

    long cycle = 0;
    char line[8];

    puts("Ordonnanceur RM (Test PC)");

    while (1) {

        printf("\n[Cycle %ld] Appuyez sur Entrée...", cycle++);
        if (!fgets(line, sizeof line, stdin))
            break;


        Scheduler__scheduler_step(task_end, &out, &mem);

        printf("Tâches lancées : ");

        for (int cpu = 0; cpu < Scheduler_data__nproc; cpu++) {
            int t = out.task_run[cpu];
            if (t == Scheduler_data__ntasks)
                printf("[CPU %d: rien] ", cpu);
            else
                printf("[CPU %d: tache %d] ", cpu, t);
        }
        printf("\n");

        for (int i = 0; i < Scheduler_data__ntasks; i++)
            task_end[i] = 0;
    }

    return 0;
}
