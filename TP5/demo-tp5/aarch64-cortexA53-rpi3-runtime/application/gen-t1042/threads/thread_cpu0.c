#include "api_lopht.h"
#include "syncvars.h"
#include "nodes.h"
#include "variables.h"
#include "threads.h"

void global_init(void){
}

void mif_entry_point_cpu0(void){
	g_step(z,&y);

	WAIT_CPU(loc_pc_1, 1);
	h_step(x,y,&z);
	UPDATE_CPU(loc_pc_0, 1);

	WAIT_CPU(loc_pc_1, 2);
	loc_pc_1 = -1 ;
	UPDATE_CPU(loc_pc_0, -1);
}
