#include "api_lopht.h"
#include "syncvars.h"
#include "nodes.h"
#include "variables.h"
#include "threads.h"


/* CYCLE FUNCTIONS */
SECTION_ATTR(".text.cpu2")
__attribute__((noinline,noreturn))
void mif_end_point2 (void){
	/* Notify other cores this core finished */
	UPDATE_CPU(loc_pc_2,10000);
	/* Wait until interrupted. */
	for(;;); // Could use a WFE statement
}


SECTION_ATTR(".text.cpu2")
void mif_entry_point_cpu2(void){
	/* MIF is now completed */
	mif_end_point2();
}
