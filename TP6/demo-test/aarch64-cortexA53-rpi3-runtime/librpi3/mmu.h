#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <librpi3/assert.h>
#include <librpi3/mmu-private.h>

extern descriptor el1_l1[4] ;
extern descriptor el2_l1[4] ;

extern uint32_t table_loaded;

void init_page_table_el1 (void);
void init_page_table_el2 (void);

void mmu_init_el1 (void);
void mmu_init_el2 (void);

#endif
