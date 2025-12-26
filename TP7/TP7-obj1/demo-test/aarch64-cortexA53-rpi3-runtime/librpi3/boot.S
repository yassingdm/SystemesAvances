	//=====================================================
	// Minimal, universal assembly entry point.
	// It only sets up the stacks of the cores
	// before giving control to the C entry point.
	//
	// This code is not EL-specific. It can be called
	// at any EL provided that the stack symbols
	// and the C entry point symbol are defined.
	//
	// Note that the code does not even disable the
	// interrupts. If required, you can do it the first
	// thing in C.
	//
	// File boot.h contains declarations of objects used
	// in assembly. They must be defined at some point
	// (in C or linker script).
	//=====================================================
	
	.section ".init", "ax", %progbits
	.balign	4
	.globl _start
	
_start:	
	// Read core id
	mrs	x6, mpidr_el1
	ands	x6, x6, #0x3
	beq	core0stack
	cmp 	x6, #1
	beq 	core1stack
	cmp 	x6, #2
	beq 	core2stack
	cmp 	x6, #3
	beq 	core3stack
dead:
	wfe
	b	dead
	
core0stack:
	ldr 	x2, =__stack_core0
	b	setstack

core1stack:
	ldr 	x2, =__stack_core1
	b	setstack

core2stack:
	ldr 	x2, =__stack_core2
	b	setstack

core3stack:
	ldr 	x2, =__stack_core3
	b	setstack

setstack:
	// Set up the EL2 stack
	mov	sp, x2
	
	// Minimal setup is finished, ready to enter C domain 
	b	_c_start

	.balign	4
	.ltorg	/* Tell assembler ltorg data for code above can go here */
