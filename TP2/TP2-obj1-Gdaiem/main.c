#include <stdio.h>
#include <stdbool.h>
#include "rcounter.h"

int main(void) {
    Rcounter__rcounter_mem mem;
    Rcounter__rcounter_out out;

    Rcounter__rcounter_reset(&mem);

    for (;;) {
        int rst_in = 0;
        printf("rst (0/1) ? ");
        fflush(stdout);
        if (scanf("%d", &rst_in) != 1) return 0;

        Rcounter__rcounter_step((rst_in != 0), &out, &mem);

        printf("cnt = %d\n", out.cnt);
    }
    return 0;
}
