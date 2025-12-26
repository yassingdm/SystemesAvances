#include <stdio.h>
#include "print.h"


void Print__print_fast_reset(Print__print_fast_mem *self) {
    (void)self;
}
void Print__print_fast_step(int idx, int x_in, int y_out,
                            Print__print_fast_out *out) {
    (void)out;
    printf("[Fast]    idx=%d | x_in=%d -> y_out=%d\n", idx, x_in, y_out);
    fflush(stdout);
}


void Print__print_gnc_reset(Print__print_gnc_mem *self) {
    (void)self;
}
void Print__print_gnc_step(int idx, int y_in, int x_out,
                           Print__print_gnc_out *out) {
    (void)out;
    printf("[GNC]     idx=%d | y_in=%d -> x_out=%d\n", idx, y_in, x_out);
    fflush(stdout);
}


void Print__print_thermal_reset(Print__print_thermal_mem *self) {
    (void)self;
}
void Print__print_thermal_step(int idx,
                               Print__print_thermal_out *out) {
    (void)out;
    printf("[Thermal] idx=%d\n", idx);
    fflush(stdout);
}