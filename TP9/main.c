#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>  
#include "square.h"    

int main(void) {
    Square__main_app_mem mem;
    Square__main_app_out out;

    Square__main_app_reset(&mem);

    long cycle = 0;

    for(;;) {
        Square__main_app_step(&out, &mem);

        printf("Cycle %ld : clk = %s\n", cycle, out.clk ? "True" : "False");
        fflush(stdout);

        cycle++;

        usleep(500000);
    }

    return 0;
}