#include <librpi3/uart-mini.h>
#include <librpi3/string.h>
#include <librpi3/stdio.h>
#include <librpi3/crc16.h>
#include "magic.h"
#include "loader.h"

// The entry point of the code we are
// loading
typedef void (*entry_point_type)(void) ;

void acknowledge_end() {
  miniuart_putc(rpi_completed) ;
}

//==============================================================
// Loader protocol function 
__attribute__((noreturn))
void loader_protocol(void) {
  // Used for printing
  char strbuf[128] ; 
  
  // Buffer flush and synchronization protocol.
  // Note that it can only be executed once, in the
  // begining!
  {
    char c ;
    int b = 0 ;
    // Wait for the incoming magic word.
    do {
      c = miniuart_getc() ;
      if(c == write_magic[b]) b++ ;
      else b = 0 ;
      miniuart_putc(c) ;
    } while (b<=strnlen(write_magic,4)) ;
    // Send the outgoing magic word.
    miniuart_puts(read_magic) ;
  }
  //
  {
    int strbuf_ptr ;
    for(;;) {
      char data_kind = miniuart_getc() ;
      char c ;
      switch(data_kind) {
      case DATA_STRING:
	{
	  strbuf_ptr = 0 ;
	  do {
	    c = miniuart_getc() ;
	    strbuf[strbuf_ptr] = c ;
	    strbuf_ptr++ ;
	  } while(c != 0) ;
	  miniuart_puts_noend("RECEIVED STRING: ") ;
	  miniuart_puts(strbuf) ;
	  miniuart_puts("\n") ;
	  acknowledge_end() ;
	}
	break ;
	
      case DATA_UINT32:
	{
	  uint32_t res = miniuart_read_uint32() ;
	  snprintf(strbuf,127,"RECEIVED UINT32:0x%x\n",res) ;
	  miniuart_puts(strbuf) ;
	  acknowledge_end() ;
	}
	break ;
	
      case DATA_FILE:
	{
	  uint32_t addr = miniuart_read_uint32() ;
	  uint32_t size = miniuart_read_uint32() ;
	  int i ;
	  unsigned char* caddr = (unsigned char*)(uint64_t)addr ;
	  // This loop **must** be as fast as possible, to avoid timing
	  // problems.
	  for(i=0;i<size;i++) {
	    caddr[i] = miniuart_getc() ;
	  }

	  // Confirm reception
	  { 
	    uint32_t crc = (uint32_t)crc16(caddr,size) ;
	    snprintf(strbuf,127,"RECEIVED FILE: ADDR:0x%x SIZE:0x%x CRC:0x%x\n",
		     addr,
		     size,
		     crc) ;
	    miniuart_puts(strbuf) ;
	  }
	  
	  // Flush the data cache and invalidate the instruction cache
	  // Cf. page 114 of ARMV8 ARM (DDI0487D_a_armv8_arm.pdf)
	  // ARM inline assembly rules here:
	  //   http://www.ethernut.de/en/documents/arm-inline-asm.html
	  {
	    // First, flush all pages from data cache
#define PAGE_SIZE 16
	    for(i=0;i<size;i+=PAGE_SIZE) {
	      asm volatile("mov x0, %[addr];"
			   "dc cvau, x0;"
			   : /* no outputs */
			   :[addr]"r"(&caddr[i])
			   :"x0", "memory") ;
	    }
	    // Then, perform a barrier
	    asm volatile("dsb ish;":::"memory") ;
	    // Invalidate all the instruction cache
	    asm volatile("ic iallu;":::"memory") ;
	    // Flush the instruction pipeline
	    asm volatile("ISB":::"memory") ;
	  }

	  acknowledge_end() ;
	}

	break ;

      case CMD_RUN:
	{
	  uint32_t addr = miniuart_read_uint32() ;
	  snprintf(strbuf,127,"RUN command. Branching to address: 0x%x\n",addr) ;
	  miniuart_puts(strbuf) ;
	  miniuart_puts("-------------------------------------------------\n") ;
	  entry_point_type entry_point = (entry_point_type)(uint64_t)addr ;
	  (*entry_point)() ;
	  // assume it does not return, but protect nonetheless
	  for(;;) ; 
	}
	break ;
	
      default:
	miniuart_puts("UNKNOWN DATA KIND\n") ;
	break ;
      }
    }
  }  
}

