#ifndef VECTOR_HANDLERS_H
#define VECTOR_HANDLERS_H

// First, some constants that are also visible from assembly

// The four types of interrupts
#define TYPE_SYNC   0x10
#define TYPE_IRQ    0x11
#define TYPE_FIQ    0x12
#define TYPE_SERROR 0x13

// The four configurations of an interrupt arrival
#define CONF_CURR_SP0  0x20 // Comes from same EL, handle with SP0
#define CONF_CURR_SPx  0x21 // Comes from same EL, handle with SP of current EL
#define CONF_LOWER_A64 0x22 // Comes from lower EL in AArch64
#define CONF_LOWER_A32 0x23 // Comes from lower EL in AArch32


#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stddef.h>

//==============================================================
// Partial context - only the GP registers
typedef struct __attribute__((__packed__)) {
  uint64_t x[31] ; // Saved in order -- x0 on x[0], x3 on x[3]
} aarch64_context_partial ;

//==============================================================
// Full exec context, including GP registers, SPSR, ELR, and SP.
// FPU registers are not needed because exception code
// does not touch them.
typedef struct __attribute__((__packed__)) {
  aarch64_context_partial cp ;
  uint64_t spsr ;  // Saved processor state reg
  uint64_t elr ;   // Elevated link register
  uint64_t sp ;    // Stack pointer
} aarch64_context ;

//==============================================================
// Vector handlers that receive a context have this signature
typedef void (*handler)(aarch64_context_partial*pcontext) ;

#endif
#endif
