#include <stddef.h>
#include <librpi3/arm.h>
#include <librpi3/arm-config.h>
#include <librpi3/required.h>
#include <librpi3/uart-mini.h>
#include <librpi3/stdio.h>
#include <librpi3/bcm2837.h>



//==============================================================
// Set up the BSS - must be called by one core before all
// other are started.
void bss_init(void) {
  volatile uint32_t* ptr ;
  for(ptr = __bss_start__; ptr < __bss_end__; ptr++) {
    *ptr = 0 ;
  }
}


//==============================================================
// Timer configuration must be done in EL2, on all cores.
// Part of it will be executed only on Core 0.
void arm_timer_config_el2(void) {
  //(TODO)
  
  // Make the virtual and physical timers coincide
  write_cntvoff_el2(0) ;

  // Give EL1 and EL0 access to timer registers
  cnthctl_el2_reg_t cnthctl_el2 ;
  cnthctl_el2.Raw64 = 0 ;
  cnthctl_el2.EL1PCTEN = 1 ;
  cnthctl_el2.EL1PCEN = 1 ;
  write_cnthctl_el2(cnthctl_el2) ;
  
  // Give EL0 access to timer registers.
  // Is this needed?
  cntkctl_el1_reg_t cntkctl_el1 ;
  cntkctl_el1.Raw64 = 0 ;
  cntkctl_el1.EL0PCTEN = 1 ;
  cntkctl_el1.EL0VCTEN = 1 ;
  write_cntkctl_el1(cntkctl_el1) ;

  if(GetCoreID() == 0) {
    // If mailbox 0 of Core 1 is written, the core receives an IRQ
    int cpuid ;
    for(cpuid=1;cpuid<4;cpuid++) {
      LOC_PERIPH->CoreMailboxIntControl[cpuid].Raw32 = 0 ;
      LOC_PERIPH->CoreMailboxIntControl[cpuid].Mailbox0_IRQ = 1 ;
    }

    /*
    // We are in NS EL1 so enable IRQ to core0 that level
    for(cpuid=0;cpuid<4;cpuid++) {
      if(cpuid == 5) {
	LOC_PERIPH->CoreTimerIntControl[cpuid].Raw32 = 0 ;
	LOC_PERIPH->CoreTimerIntControl[cpuid].nCNTPNSIRQ_IRQ = 1;
      } else {
	// Make sure no IRQ or FIQ is routed to cores different from 0
	LOC_PERIPH->CoreTimerIntControl[cpuid].Raw32 = 0 ;
      }
    }
    
    // It's already set if starting in EL2, but I still
    // set it for safety. With this value, the divider is 1.
    LOC_PERIPH->TimerPrescaler = 0x40000000 ;
    */

    // I currently only use the LocalTimer. Only this one is
    // configured. 
    // Route timer interrupt to IRQ on core 0
    LOC_PERIPH->TimerRouting.Routing = LOCALTIMER_TO_CORE0_IRQ ;
    // Set period. The base is 38.4 MHz, so 2^26 gives
    // about two seconds.
    LOC_PERIPH->TimerControlStatus.ReloadValue =
      /* Given the base frequency of 38.4MHz, one second requires 
       * exactly 38.4*10^6 ticks */ 38400000 ;
    // Timer IRQ
    LOC_PERIPH->TimerControlStatus.IntEnable = 1;
    // Clear interrupt
    LOC_PERIPH->TimerClearReload.IntClear = 1;
    // Reload now
    LOC_PERIPH->TimerClearReload.Reload = 1;
    // Enable timer
    LOC_PERIPH->TimerControlStatus.TimerEnable = 1;
  }
}

//==============================================================
// Configure EL2. Must be done at EL3 or EL2.
void arm_config_el2(uint64_t*vector_table,uint32_t*stack_pointer) {
  // Set up EL2 identification registers
  write_vpidr_el2(read_midr_el1()) ;
  write_vmpidr_el2(read_mpidr_el1()) ;
  
  // 
  reset_hstr_el2() ;

  // Set up CPTR_EL2
  cptr_el2_reg_t cptr_el2 ;
  // RES0/RES1 set-up
  cptr_el2.Raw64 = 0 ;
  cptr_el2.RES1_0 = 0xff ; 
  cptr_el2.RES1_1 = 1 ;
  cptr_el2.RES1_2 = 3 ;
  // Traps to EL2 SVE instructions executed at EL2,
  // EL1, or EL0 (and other instructions that access
  // SVE registers executed at these levels)
  // NOTE: SVE = Scalable Vector Extension.
  cptr_el2.TZ = 1 ;
  // All other traps are not set
  write_cptr_el2(cptr_el2) ;

  // Tell EL2 that lower level is AArch64
  hcr_el2_reg_t hcr_el2 ;
  hcr_el2.Raw64 = 0 ;
  hcr_el2.RW = 1 ;  // Make sure EL1 is 64 bits
  hcr_el2.FMO = 1 ; // FIQs are all routed to EL2
  hcr_el2.IMO = 1 ; // IRQs are all routed to EL2 (TODO)
  hcr_el2.AMO = 1 ; // Physical SErrors are all routed to EL2 (TODO?)
  write_hcr_el2(hcr_el2) ;

  // Set up the vector table, if one was provided
  if(vector_table != NULL) {
    write_vbar_el2((uint64_t)vector_table) ;
  }

  // If a stack pointer has been provided, set it
  if(stack_pointer != NULL) {
    write_sp_el2((uint64_t)stack_pointer) ;
  }
}

//==============================================================
// Configure EL1. Must be done at EL3, EL2, or EL1.
void arm_config_el1(uint64_t*vector_table,uint32_t*stack_pointer) {
  // Set up CPACR_EL1
  cpacr_el1_reg_t cpacr_el1 ;
  // RES0 set-up
  cpacr_el1.Raw64 = 0 ;
  // Remove all traps on FP, SIMD, and SVE (but SVE
  // will be trapped on EL2, so it's OK). (TODO?)
  cpacr_el1.FPEN = 3 ;
  write_cpacr_el1(cpacr_el1) ;

  // Set up the vector table, if one was provided
  if(vector_table != NULL) {
    write_vbar_el1((uint64_t)vector_table) ;
  }

  // If a stack pointer has been provided, set it
  if(stack_pointer != NULL) {
    write_sp_el1((uint64_t)stack_pointer) ;
  }
}

//==============================================================
// Branch from EL2 to EL1, to a specific address 
__attribute__((noreturn))
void branch_el2_to_el1(uint64_t addr) {
  // Set the processor status after return
  spsr_reg_t spsr_el2 ;
  spsr_el2.Raw64 = 0 ;
  spsr_el2.M03 = EL1h ; // Return to EL1
  spsr_el2.F = 1 ; // FIQ masked
  spsr_el2.I = 1 ; // IRQ masked
  spsr_el2.A = 1 ; // SError masked
  spsr_el2.D = 1 ; // debug exceptions masked
  write_spsr_el2(spsr_el2) ;

  // Set the return address 
  write_elr_el2((uint64_t)addr) ;

  // Trigger return to EL1
  asm volatile("eret");

  // Failsafe
  for(;;) ;
}

//==============================================================
// Branch from EL1 to EL0, to a specific address
__attribute__((noreturn))
void branch_el1_to_el0(uint64_t addr,uint64_t stack_addr) {
  // Set the processor status after return.
  // Interrupts are allowed here.
  spsr_reg_t spsr_el1 ;
  spsr_el1.Raw64 = 0 ;
  spsr_el1.M03 = EL0t ;
  write_spsr_el1(spsr_el1) ;
  
  // Set the return address 
  write_elr_el1(addr) ;
  // Set the stack
  write_sp_el0(stack_addr) ;
  
  // Trigger return to EL0, and place the CPUID into X0
  asm volatile("mov x0, %0;"
	       "eret;"
	       ::"r"(GetCoreID())) ;
  
  // Failsafe
  for(;;) ;
}

//==============================================================
// Branch from EL1 to EL0, to a specific address
__attribute__((noreturn))
void branch_el2_to_el0(uint64_t addr,uint64_t stack_addr,uint64_t el2_sp) {
  // Set the processor status after return.
  // Interrupts are allowed here.
  spsr_reg_t spsr_el2 ;
  spsr_el2.Raw64 = 0 ;
  spsr_el2.M03 = EL0t ;
  write_spsr_el2(spsr_el2) ;
  
  // Set the return address 
  write_elr_el2(addr) ;
  // Set the stack
  write_sp_el0(stack_addr) ;
  
  // Trigger return to EL0, and place the CPUID into X0
  DMB(sy) ;
  asm volatile("mov sp, %[el2sp];"
	       "mov x0, %[cpuid];"
	       "eret;"
	       ::
		[cpuid]"r"(GetCoreID()),
		[el2sp]"r"(el2_sp) ) ;
  
  // Failsafe
  for(;;) ;
}

//==============================================================
// Very useful delay function. It is not calibrated on a
// time base, but it allows to take some time when it's
// needed. It's not optimized, to make for more delay...
__attribute__((optimize("-O0")))
void delay(uint32_t delay) {
  int i ;
  for(i=0;i<delay;i++) ;
}

