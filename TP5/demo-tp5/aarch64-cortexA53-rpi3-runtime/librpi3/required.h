#ifndef REQUIRED_H
#define REQUIRED_H

//==============================================================
// These symbols must be defined by the user code when some
// services are needed. Definition can be done either using
// C variables/functions, or inside the linker script.
//
// Not all definitions are always needed:
// - Stack definitions and entry point are needed when using
//   the assembly boot code boot.S.
// - BSS definitions are needed when the application has a
//   BSS that needs setting using routine bss_init from
//   arm-config.[ch].
//==============================================================

//--------------------------------------------------------------
// Stack and entry point definitions
//--------------------------------------------------------------

// These symbols must be defined by the linker
// script or in C. They are the stacks of the
// 4 cores, to be set when starting the code.
extern uint32_t __attribute__((aligned(16))) __stack_core0[] ;
extern uint32_t __attribute__((aligned(16))) __stack_core1[] ;
extern uint32_t __attribute__((aligned(16))) __stack_core2[] ;
extern uint32_t __attribute__((aligned(16))) __stack_core3[] ;

// These two symbols mark the start and the end of the code
// and data allocated by the linker. I align them both
// on 16 bytes to facilitate allocation of objects around,
// such as stacks.
extern uint32_t __attribute__((aligned(16))) __start__[] ;
extern uint32_t __attribute__((aligned(16))) __end__[] ;

// The C code must define this function to which control
// is given after assembly initializations.
__attribute__((noreturn))
extern void _c_start(void) ;

//--------------------------------------------------------------
// BSS definitions
//--------------------------------------------------------------

// These symbols must be defined by the linker
// script or in C. They are the start and end of
// the BSS, allowing its initialization when code
// is started.
extern uint32_t __attribute__((aligned(4))) __bss_start__[] ;
extern uint32_t __attribute__((aligned(4))) __bss_end__[] ;


#endif
