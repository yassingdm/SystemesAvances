#ifndef SVC_H
#define SVC_H

#include <stdint.h>
#include <librpi3/arm.h>
#include <librpi3/vector-handlers.h>
#include <librpi3/semaphore.h>

#define SVC_RAM_BASE 0x20000000

struct SVCExchangeRAM{
  // Upon SVC call, core I may place an address,
  // and upon return I may get a return value.
  volatile uint32_t Addr[4] ;
} ;

#define SVC_RAM ((volatile struct SVCExchangeRAM*)SVC_RAM_BASE)

typedef enum {
  SVC_CONSOLE_PRINT = 0,
  SVC_GET_CPUID = 1,
} SVC_call_type ;

//----------------------------------------------------
// This semaphore controls printing using the console.
// As soon as caches and coherency are started,
// concurrent prints from the various cores must use
// this semaphore.
// Initially 0 - set through BSS
extern semaphore_t console_sem ;

//
void svc_call(uint32_t cpuid, SVC_call_type type, void*addr) ;
//
__attribute__((always_inline))
inline void console_puts(uint32_t cpuid,char*str) {
  svc_call(cpuid,SVC_CONSOLE_PRINT,(void*)str);
}
__attribute__((always_inline))
inline uint32_t get_cpuid() {
  uint64_t tmp ;
  svc_call(0xABCD,SVC_GET_CPUID,(void*)&tmp);
  return (uint32_t)tmp ;
}


//
void svc_init(void) ;

//
void svc_handler(aarch64_context_partial*pcontext,
		 uint32_t imm16) ;

#endif
