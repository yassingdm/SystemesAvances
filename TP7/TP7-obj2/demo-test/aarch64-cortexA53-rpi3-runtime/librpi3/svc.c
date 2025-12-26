#include <librpi3/svc.h>
#include <librpi3/uart-mini.h>

//----------------------------------------------------
// This semaphore controls printing using the console.
// As soon as caches and coherency are started,
// concurrent prints from the various cores must use
// this semaphore.
// Initially 0 - set through BSS
semaphore_t console_sem ;

void svc_handler(aarch64_context_partial*pcontext,
		 uint32_t imm16) {
  switch(imm16) {
  case SVC_CONSOLE_PRINT:
    {
      char* str = (char*)((uint64_t)SVC_RAM->Addr[GetCoreID()]) ;
      semaphore_get(&console_sem) ;
      /*
      switch(GetCoreID()){
      case 0:
	miniuart_puts("0\n") ;
	break ;
      case 1:
	miniuart_puts("1\n") ;
	break ;
      case 2:
	miniuart_puts("2\n") ;
	break ;
      case 3:
	miniuart_puts("3\n") ;
	break ;
      default:
	miniuart_puts("X\n") ;
	break ;
      }
      */
      miniuart_puts(str) ;
      semaphore_release(&console_sem) ;
    }
    break ;
  case SVC_GET_CPUID:
    pcontext->x[0] = GetCoreID() ;
    break ;
  default:
    semaphore_get(&console_sem) ;
    miniuart_puts("SVC/HVC handler called with unknown code.\n") ;
    semaphore_release(&console_sem) ;    
    break ;
  }
}

void svc_call(uint32_t cpuid, SVC_call_type type, void*addr) {
  switch(type) {
  case SVC_CONSOLE_PRINT:
    // CPUID must have a correct value
    SVC_RAM->Addr[cpuid] = (uint64_t)addr ;
    asm volatile("svc #0") ;
    break ;
  case SVC_GET_CPUID:
    // CPUID value is not used, value is returned in the
    // pointed address (which must be a pointer to a
    // uint64_t
    {
      uint64_t tmp ;
      asm volatile("svc #1;"
		   "mov %[val], x0;"
		   :[val]"=r"(tmp)::) ;
      *((uint64_t*)addr) = tmp ;
    }
    break ;
  default:
    // Do nothing
    break ;
  }
}
