//#include <stddef.h>
#include <librpi3/uart-mini.h>
#include <librpi3/arm.h>
#include <librpi3/arm-config.h>
#include <librpi3/stdio.h>
#include <librpi3/speed.h>
#include <librpi3/required.h>
#include "loader.h"

void cpu0_el2_print_info(void) {
  // Used for printing
  char strbuf[128] ; 
  // Print something to confirm we reached this stage
  miniuart_puts("--------------------------------------------\n");
  miniuart_puts("Loader code started and UART initialized\n") ;
  snprintf(strbuf,127,"Base addr:0x%x EL%d\n",
	   (uint64_t)__start__,
	   read_current_el()) ;
  miniuart_puts(strbuf);
  miniuart_puts("--------------------------------------------\n");
}


void el2_main(void) {
  arm_config_el2(NULL,NULL) ;
  if(GetCoreID() == 0) {
    // The first core. First, do some initialization and
    // print some information on the console.
    bss_init() ;
    // Set the ARM speed to its maximum
    arm_setspeed(arm_getmaxspeed()) ;
    // Init the UART
    miniuart_init(115200) ;
    // Print some info
    cpu0_el2_print_info() ;

    //
    loader_protocol() ;
    
  } else {
    // Error - Cores 1-3 should not pass through the loader
    delay(1000000) ;
    miniuart_puts("ERRXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    for(;;) ;
  }
}

__attribute__((noreturn))
void _c_start(void) {
  el2_main() ;
  for(;;) ;
}
