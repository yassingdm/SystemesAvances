	//===================================================
	// Interrupt handler macro to be placed in the vector
	// tables. Mostly relies on:
	//   ARM Cortex-A Series
	//   Programmer's Guide for ARMv8-A
	//   Section 10.5
	//
	// This stub performs a partial context save:
	// - The full set of 31 GP registers x0-x30. I save
	//   all of them because I may not return as from a
	//   function call, and thus I cannot rely on the
	//   ABI rules on register restoration.
	// - I do not save the SP, SPSR, and ELR of the caller.
	//   This can be done in the C handler, after
	//   decoding the interrupt and its source.
	// - I also don't save the FPU registers, assuming
	//   the FPU is not used in EL2 and EL3.
	//
	// Saved context is placed on stack - this is the only
	// way to have different storage for the 4 cores
	// without making lots of tests. And efficiency is
	// key.
	//
	// When entering the C function, this stub sets only one
	// argument (in x0) -- the address of the saved context.
	//
	// The C function must never return (noreturn function).
	// Note that it does not perform the context restoration
	// (this one is later in the file).
	//===================================================
	.macro c_handler name cfunc
	.balign 4	
	.globl \cfunc     //imported
	
	.globl \name      //exported
	.type \name, %function
\name:
	stp	x29, x30, [sp, #-16]!
	stp	x18, x19, [sp, #-16]!
	stp	x16, x17, [sp, #-16]!
	stp	x14, x15, [sp, #-16]!
	stp	x12, x13, [sp, #-16]!
	stp	x10, x11, [sp, #-16]!
	stp	x8 , x9 , [sp, #-16]!
	stp	x6 , x7 , [sp, #-16]!
	stp	x4 , x5 , [sp, #-16]!
	stp	x2 , x3 , [sp, #-16]!
	stp	x0 , x1 , [sp, #-16]!

	// The single argument is the SP, which now holds the
	// pointer to the stored registers.
	mov	x0, sp
	// Call the handler
	bl	\cfunc
	// If the handler function terminates, go to fast 
	// context restore
	b	context_restore_partial
	.ltorg
	.size \name, .-\name
	.endm

	//***************************************************
	// Here stop macros, starts the actual code
	//***************************************************
	
	//===================================================
	// The various handlers I will use
	.section ".vector_irq", "ax", %progbits
	c_handler sync_handler_asm    sync_handler_c
	c_handler irq_handler_asm     irq_handler_c
	c_handler fiq_handler_asm     fiq_handler_c
	c_handler serror_handler_asm  serror_handler_c

	//===================================================
	// Fast context restoration, assuming that no changes
	// were made to SPSR, ELR, saved SP, and that the
	// saved context is on stack.
	.globl context_restore_partial
	.type context_restore_partial, %function
context_restore_partial:
	ldp	x0 , x1 ,  [sp], #16
	ldp	x2 , x3 ,  [sp], #16
	ldp	x4 , x5 ,  [sp], #16
	ldp	x6 , x7 ,  [sp], #16
	ldp	x8 , x9 ,  [sp], #16
	ldp	x10, x11,  [sp], #16
	ldp	x12, x13,  [sp], #16
	ldp	x14, x15,  [sp], #16
	ldp	x16, x17,  [sp], #16
	ldp	x18, x19,  [sp], #16
	ldp 	x29, x30,  [sp], #16
	eret
	.ltorg
	.size context_restore_partial, .-context_restore_partial
