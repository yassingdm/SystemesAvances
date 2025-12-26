#include <librpi3/uart-mini.h>
#include <librpi3/semaphore.h>
#include <librpi3/stdio.h>
#include <librpi3/arm-config.h>
#include <librpi3/svc.h>
#include <gen-t1042/threads.h>


// Weak symbols doing nothing for entry points
// of the 4 cores. I can now instantiate as
// many of these symbols as I want as part of
// code generation (1, 2, 3, or 4). If I don't
// generate one, the weak symbol below does its
// magic.
__attribute__((weak))
__attribute__((noreturn))
void mif_entry_point_cpu0(void) {
  for(;;) ;
}
__attribute__((weak))
__attribute__((noreturn))
void mif_entry_point_cpu1(void) {
  for(;;) ;
}
__attribute__((weak))
__attribute__((noreturn))
void mif_entry_point_cpu2(void) {
  for(;;) ;
}
__attribute__((weak))
__attribute__((noreturn))
void mif_entry_point_cpu3(void) {
  for(;;) ;
}


// Global barrier - this value is not zero,
// so it gets initialized as part of program loading
// (.data section), not during .bss init.
volatile uint32_t bss_not_initialized = 1 ;

__attribute__((noreturn))
__attribute__((section(".init")))
void el0_main(uint64_t cpuid) {
  // BSS init - the first one to get the semaphore will
  // do it. Normally, no cache operation is needed, everything
  // is done though semaphores and coherency.
  if((cpuid==0)&&(bss_not_initialized)) {
    bss_init() ;
    asm volatile("dmb sy") ;
    bss_not_initialized = 0 ;
    // Application initialization
    global_init() ;
  }
  while(bss_not_initialized) ;

  // And now, the computation
  /*
  char strbuf[128] ;
  snprintf(strbuf,127,"EL0:Core %d\n",cpuid) ;
  console_puts(cpuid,strbuf) ;
  */
  
  switch(cpuid) {
  case 0:
    mif_entry_point_cpu0() ;
    break ;
  case 1:
    mif_entry_point_cpu1() ;
    break ;
  case 2:
    mif_entry_point_cpu2() ;
    break ;
  case 3:
    mif_entry_point_cpu3() ;
    break ;
  default:
    break ;
  }
  for(;;) ;
}
