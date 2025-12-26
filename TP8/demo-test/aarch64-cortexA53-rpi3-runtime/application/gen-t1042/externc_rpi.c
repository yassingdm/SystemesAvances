#include "externc.h"

#include <librpi3/svc.h>
#include <librpi3/stdio.h>

void Externc__deadline_miss_log_step(int date, int task_id, Externc__deadline_miss_log_out* out) {
    (void)date;
    (void)task_id;
    (void)out;
    console_puts(0, "DEADLINE MISS!\n");
}
