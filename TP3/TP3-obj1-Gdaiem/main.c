#include <stdio.h>
#include <unistd.h>
#include "gnc.h"

int main(void){
    Gnc__main_mem m;
    Gnc__main_reset(&m);
    Gnc__main_out out;

    for(;;){
        Gnc__main_step(&out, &m);
        sleep(1);
    }
    return 0;
}