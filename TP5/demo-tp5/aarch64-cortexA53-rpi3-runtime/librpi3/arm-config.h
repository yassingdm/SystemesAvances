#ifndef ARM_CONFIG_H
#define ARM_CONFIG_H

#include <stdint.h>

//==============================================================
// Set up the BSS - must be called by one core before all
// other are started.
void bss_init(void) ;

//==============================================================
// Configure everything except the MMU and timers
void arm_config_el2(uint64_t*vector_table,uint32_t*stack_pointer) ;
void arm_config_el1(uint64_t*vector_table,uint32_t*stack_pointer) ;

//==============================================================
// Timer configuration must be done in EL2
void arm_timer_config_el2() ;

//==============================================================
// Branch from One EL to a lower one

// Branching from EL2 to EL1h while keeping interrupts
// (DAIF) masked.
__attribute__((noreturn))
void branch_el2_to_el1(uint64_t addr) ;
// Branching from EL1 to EL0. No interrupt is masked.
__attribute__((noreturn))
void branch_el1_to_el0(uint64_t addr,uint64_t stack_addr) ;
// Branching from EL2 to EL0. No interrupt is masked.
__attribute__((noreturn))
void branch_el2_to_el0(uint64_t addr,uint64_t stack_addr,uint64_t el2_sp) ;
  
//==============================================================
// Very useful delay function
void delay(uint32_t delay) ;

#endif
