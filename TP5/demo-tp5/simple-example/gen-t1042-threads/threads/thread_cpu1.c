#include "api_lopht.h"
#include "syncvars.h"
#include "nodes.h"
#include "variables.h"
#include "threads.h"


/* CYCLE FUNCTIONS */
SECTION_ATTR(".text.cpu1_period1_phase0")
__attribute__((noinline))
void cpu1_period1_phase0(void) {
	UPDATE_CPU(loc_pc_1,0);
	// Prefetch operations go here
	// Waiting for synchronizations, if any
	// Release date wait goes here (if any)
	f_step(&x);
	// Deadline check (if any) goes here

}

SECTION_ATTR(".text.cpu1")
__attribute__((noinline,noreturn))
void mif_end_point1 (void){
	/* Notify other cores this core finished */
	UPDATE_CPU(loc_pc_1,10000);
	/* Wait until interrupted. */
	for(;;); // Could use a WFE statement
}


SECTION_ATTR(".text.cpu1")
void mif_entry_point_cpu1(void){
	cpu1_period1_phase0();
	/* MIF is now completed */
	mif_end_point1();
}
