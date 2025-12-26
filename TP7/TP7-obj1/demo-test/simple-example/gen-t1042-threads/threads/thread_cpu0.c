#include "api_lopht.h"
#include "syncvars.h"
#include "nodes.h"
#include "variables.h"
#include "threads.h"


__attribute__((always_inline))
inline void mif_reinit(void) {
	/* Update MIF counter */
	mif_cnt = (mif_cnt+1)%1 ;
	/* Reset sync vars of other cores */
	loc_pc_1 = -1 ;
	loc_pc_2 = -1 ;
	/* Reset the local sync var and propagate changes */
	UPDATE_CPU(loc_pc_0,-1) ;
}
/* CYCLE FUNCTIONS */
SECTION_ATTR(".text.cpu0_period1_phase0")
__attribute__((noinline))
void cpu0_period1_phase0(void) {
	UPDATE_CPU(loc_pc_0,0);
	// Prefetch operations go here
	// Waiting for synchronizations, if any
	// Release date wait goes here (if any)
	g_step(z_1, &y);
	// Deadline check (if any) goes here

	UPDATE_CPU(loc_pc_0,1000);
	// Prefetch operations go here
	// Waiting for synchronizations, if any
	WAIT_CPU(loc_pc_1,10000);
	// Release date wait goes here (if any)
	h_step(x, y, &z_1);
	// Deadline check (if any) goes here

}


SECTION_ATTR(".text.cpu0")
__attribute__((noinline))
void global_init(void) {
	/* Initialize state variables */
	z_1 = 123;
	/* Initialize synchronization variables */
	mif_cnt = -1 ;// make sure first MIF's id is 0
	mif_reinit() ;
}
SECTION_ATTR(".text.cpu0")
__attribute__((noinline,noreturn))
void mif_end_point0 (void){
	/* Notify other cores this core finished */
	UPDATE_CPU(loc_pc_0,10000);
	/* Wait until other cores complete the MIF */
	WAIT_CPU(loc_pc_1,10000);
	WAIT_CPU(loc_pc_2,10000);
	/* Update sync vars for next MIF */
	mif_reinit();
	/* Wait until interrupted. */
	for(;;); // Could use a WFE statement
}


SECTION_ATTR(".text.cpu0")
void mif_entry_point_cpu0(void){
	cpu0_period1_phase0();
	/* MIF is now completed */
	mif_end_point0();
}
