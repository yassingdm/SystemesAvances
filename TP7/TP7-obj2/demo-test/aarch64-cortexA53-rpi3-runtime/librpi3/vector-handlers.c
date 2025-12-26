#include <librpi3/arm.h>
#include <librpi3/uart-mini.h>
#include <librpi3/stdio.h>
#include <librpi3/vector-handlers.h>
#include <librpi3/svc.h>
#include <librpi3/semaphore.h>
#include <librpi3/bcm2837.h>
#include <librpi3/arm-config.h>

//======================================================================
// This is the default interrupt handler, which is an error handler.
// It prints some data and then hangs, hence the "noreturn" attribute.
__attribute__((noreturn))
void default_handler(uint64_t interrupt_arrival,
		     uint64_t interrupt_type) {
  miniuart_puts("default_handler:\n") ;
  switch(interrupt_arrival) {
  case CONF_CURR_SP0:  miniuart_puts("\tFROM_CURRE_A64_SP0\n") ; break ;
  case CONF_CURR_SPx:  miniuart_puts("\tFROM_CURRE_A64_SPx\n") ; break ;
  case CONF_LOWER_A64: miniuart_puts("\tFROM_LOWER_A64_SPx\n") ; break ;
  case CONF_LOWER_A32: miniuart_puts("\tFROM_LOWER_A32_SPx\n") ; break ;
  default: miniuart_puts("\tBAD arrival CONF id\n") ; break ;
  }
  switch(interrupt_type) {
  case TYPE_SYNC:   miniuart_puts("\tTYPE_SYNC\n") ; break ;
  case TYPE_IRQ:    miniuart_puts("\tTYPE_IRQ\n") ; break ;
  case TYPE_FIQ:    miniuart_puts("\tTYPE_FIQ\n") ; break ;
  case TYPE_SERROR: miniuart_puts("\tTYPE_SERROR\n") ; break ;
  default: miniuart_puts("\tBAD interrupt TYPE\n") ; break ;
  }
  char buf[64] ;
  snprintf(buf,63,"\tEL:%u\n",read_current_el()) ;
  miniuart_puts(buf) ;
  snprintf(buf,63,"\tCore:%u\n",GetCoreID()) ;
  miniuart_puts(buf) ;
  miniuart_puts("\tdefault_handler: TERMINATE EXECUTION!\n") ;
  for(;;) ;
}

semaphore_t tmp_print ;

//======================================================================
// Specialized interrupt handlers. They must all have
// type handler (cf. vector-handlers.h).
void sync_handler_c(aarch64_context_partial*pcontext) {
  esr_reg_t esr ;
  switch(read_current_el()) {
  case 1:
    esr = read_esr_el1() ;
    break ;
  case 2:
    esr = read_esr_el2() ;
    break ;
  default:
    goto other ;
  }
  if(esr.EC_H == 0b0101) {
    // SVC or HVC call
    svc_handler(pcontext,esr.Imm16) ;
    // Return to caller
    return ;
  } 
 other:
  {
    semaphore_get(&console_sem) ;

    char buf[64] ;
    esr_reg_t esr ;
    uint64_t far ;  
    snprintf(buf,62,"SYNC handler called on C%din EL%u.\n",
	     GetCoreID(),
	     read_current_el()) ;
    miniuart_puts(buf) ;
    switch(read_current_el()) {
    case 1:
      esr = read_esr_el1() ;
      snprintf(buf,62,"\tESR_EL1=0x%x.\n",esr.Raw32) ;
      miniuart_puts(buf) ;
      far = read_far_el1() ;
      snprintf(buf,62,"\tFAR_EL1=0x%x.\n",far) ;
      miniuart_puts(buf) ;
      far = read_elr_el1() ;
      snprintf(buf,62,"\tELR_EL1=0x%x.\n",far) ;
      miniuart_puts(buf) ;
      break ;
    case 2:
      esr = read_esr_el2() ;
      snprintf(buf,62,"\tESR_EL2=0x%x.\n",esr.Raw32) ;
      miniuart_puts(buf) ;
      far = read_far_el2() ;
      snprintf(buf,62,"\tFAR_EL2=0x%x.\n",far) ;
      miniuart_puts(buf) ;
      far = read_elr_el2() ;
      snprintf(buf,62,"\tELR_EL2=0x%x.\n",far) ;
      miniuart_puts(buf) ;
      break ;
    case 3:
      esr = read_esr_el3() ;
      snprintf(buf,62,"\tESR_EL3=0x%x.\n",esr.Raw32) ;
      miniuart_puts(buf) ;
      far = read_far_el3() ;
      snprintf(buf,62,"\tFAR_EL3=0x%x.\n",far) ;
      miniuart_puts(buf) ;
      far = read_elr_el3() ;
      snprintf(buf,62,"\tELR_EL3=0x%x.\n",far) ;
      miniuart_puts(buf) ;
      break ;
    default: break ;
    }
    
    semaphore_release(&console_sem) ;
  }
}

void irq_handler_c(aarch64_context_partial*pcontext) {
  char buf[64] ; // Used for printing
  uint64_t cpuid = GetCoreID() ;

  if(cpuid == 0) {
    if(LOC_PERIPH->CoreIRQSource[cpuid].LocalTimer) {
      // Normal timer handling

      // Some printing
      snprintf(buf,63,"//=========cycle start============\n") ;
      semaphore_get(&console_sem) ;
      miniuart_puts(buf) ;
      semaphore_release(&console_sem) ;
      
      // Core 0 received a timer IRQ, clear interrupt
      LOC_PERIPH->TimerClearReload.IntClear = 1 ;
      // Make sure the HW protocol finished. Without this code,
      // I can still have the interrupt not cleared, and raised
      // again.
      while(LOC_PERIPH->CoreIRQSource[cpuid].Raw32 != 0) ;
      
      // Write to mailbox 0 of cores 1,2,3 to trigger them
      for(cpuid=1;cpuid<4;cpuid++) {
	LOC_PERIPH->MBoxWR[cpuid][0] = 0x1 ;
      }
      // Launch the thread
      branch_el2_to_el0(// The entry point
			(uint64_t)0x800000,
			// The stack
			0x900000-cpuid*0x1000,
			// The restored stack
			((uint64_t)pcontext)+22*8
			) ;
    }
  } else {
    if(LOC_PERIPH->CoreIRQSource[cpuid].Mbox0) {
      // while(LOC_PERIPH->MBoxRC[cpuid][0] != 0x1) ;
      // Cores 1-3 received a mailbox 0 interrupt. Acknowledge it.
      LOC_PERIPH->MBoxRC[cpuid][0] = 0x1 ;
      // Make sure the HW protocol finished. Without this code,
      // I can still have the interrupt not cleared, and raised
      // again.
      while(LOC_PERIPH->CoreIRQSource[cpuid].Raw32 != 0) ;
      // Launch the thread
      branch_el2_to_el0((uint64_t)0x800000,
			0x900000-cpuid*0x1000,
			((uint64_t)pcontext)+22*8
			) ;
    }
  }
  // This case does not correspond to one normally treated
  // I print it and block execution on this core.
  snprintf(buf,63,"Core %d: IRQ handler called. Source: 0x%x. Stack: 0x%x\n",
	   cpuid,
	   LOC_PERIPH->CoreIRQSource[cpuid],
	   get_sp()) ;
  semaphore_get(&console_sem) ;
  miniuart_puts(buf) ;
  semaphore_release(&console_sem) ;
  for(;;) ;  
}
void fiq_handler_c(void) {
  miniuart_puts("FIQ handler called.\n") ;
}
void serror_handler_c(void) {
  miniuart_puts("SError handler called.\n") ;
}
