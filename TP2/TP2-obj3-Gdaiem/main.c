#include <stdio.h>
#include "hs_handler_merge.h"

int main(void) {
    Hs_handler_merge__main_mem mem;
    Hs_handler_merge__main_out out;

    Hs_handler_merge__main_reset(&mem);

    for (;;) {
        Hs_handler_merge__main_step(&out, &mem);
    }

    return 0;
}
