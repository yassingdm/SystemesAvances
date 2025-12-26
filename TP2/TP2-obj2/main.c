#include <stdio.h>
#include "hs_handler.h"

int main(void) {
    Hs_handler__main_mem mem;
    Hs_handler__main_out out;

    Hs_handler__main_reset(&mem);

    for (;;) {
        Hs_handler__main_step(&out, &mem);
    }

    return 0;
}
