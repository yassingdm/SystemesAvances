#include "vector-handlers.h"

	//=========================================================
	// The handlers called from here, imported from either C
	// or ASM
	
	// The default error handler.
	// Prints some info and hangs.
	// Written in C, in vector-handlers.c.
	.globl default_handler

	// Handlers for the various interrupt types
	// Defined in assembly, in vector-handlers-asm.S (which
	// call C functions in other files).
	.globl sync_handler_asm
	.globl irq_handler_asm
	.globl fiq_handler_asm
	.globl serror_handler_asm
	
	//=========================================================
	// Macro for the default interrupt vector that calls a nice
	// routine that prints its context and blocks (for debug
	// purposes). Aligns on 0x80, like all vectors.	
	.macro	default context class
	.balign 0x80
	mov	x0, \context
	mov	x1, \class
	b	default_handler
	.endm
	
	//=========================================================
	// Macro for an interrupt vector that simply branches to
	// another routine. A bit like the ARMv6 of the RPi1.
	.macro	vector handler
	.balign 0x80
	b	\handler
	.endm

	//*********************************************************
	// Actual code (not macros) starts here
	//*********************************************************	
	.section ".vector_table", "ax", %progbits
	
	//=========================================================
	// EL3 vector table
	.balign 0x800
	.globl	VectorTableEL3   // exported
	.type VectorTableEL3, %function	
VectorTableEL3:
	// IMPOSSIBLE: Coming from EL3 with SP0
	default CONF_CURR_SP0 TYPE_SYNC
	default CONF_CURR_SP0 TYPE_IRQ
	default CONF_CURR_SP0 TYPE_FIQ
	default CONF_CURR_SP0 TYPE_SERROR
	// IMPOSSIBLE: Coming from EL3 with SP3
	default CONF_CURR_SPx TYPE_SYNC
	default CONF_CURR_SPx TYPE_IRQ
	default CONF_CURR_SPx TYPE_FIQ
	default CONF_CURR_SPx TYPE_SERROR
	// Coming from EL2, EL1 or EL0 using AArch64
	vector	sync_handler_asm   //default CONF_LOWER_A64 TYPE_SYNC
	vector	irq_handler_asm    //default CONF_LOWER_A64 TYPE_IRQ
	vector	fiq_handler_asm    //default CONF_LOWER_A64 TYPE_FIQ
	vector	serror_handler_asm //default CONF_LOWER_A64 TYPE_SERROR
	// IMPOSSIBLE: Coming from EL2, EL1 or EL0 using AArch32
	default CONF_LOWER_A32 TYPE_SYNC
	default CONF_LOWER_A32 TYPE_IRQ
	default CONF_LOWER_A32 TYPE_FIQ
	default CONF_LOWER_A32 TYPE_SERROR
	.balign 0x80 // Make sure no-one fills this gap
	.ltorg
	.size VectorTableEL3, .-VectorTableEL3
	
	//=========================================================
	// EL2 vector table
	.balign 0x800
	.globl	VectorTableEL2   // exported
	.type VectorTableEL2, %function	
VectorTableEL2:
	// IMPOSSIBLE: Coming from EL2 with SP0
	default CONF_CURR_SP0 TYPE_SYNC
	default CONF_CURR_SP0 TYPE_IRQ
	default CONF_CURR_SP0 TYPE_FIQ
	default CONF_CURR_SP0 TYPE_SERROR
	// IMPOSSIBLE: Coming from EL2 with SP2
	default CONF_CURR_SPx TYPE_SYNC
	default CONF_CURR_SPx TYPE_IRQ
	default CONF_CURR_SPx TYPE_FIQ
	default CONF_CURR_SPx TYPE_SERROR
	// Coming from EL1 or EL0 using AArch64
	vector	sync_handler_asm   //default CONF_LOWER_A64 TYPE_SYNC
	vector	irq_handler_asm    //default CONF_LOWER_A64 TYPE_IRQ
	vector	fiq_handler_asm    //default CONF_LOWER_A64 TYPE_FIQ
	vector	serror_handler_asm //default CONF_LOWER_A64 TYPE_SERROR
	// IMPOSSIBLE: Coming from EL1 or EL0 using AArch32
	default CONF_LOWER_A32 TYPE_SYNC
	default CONF_LOWER_A32 TYPE_IRQ
	default CONF_LOWER_A32 TYPE_FIQ
	default CONF_LOWER_A32 TYPE_SERROR
	.balign 0x80 // Make sure no-one fills this gap
	.ltorg
	.size VectorTableEL2, .-VectorTableEL2


	//=========================================================
	// EL1 vector table
	.balign 0x800
	.globl	VectorTableEL1
	.type VectorTableEL1, %function	
VectorTableEL1:
	// IMPOSSIBLE: Coming from EL1 with SP0
	default CONF_CURR_SP0 TYPE_SYNC
	default CONF_CURR_SP0 TYPE_IRQ
	default CONF_CURR_SP0 TYPE_FIQ
	default CONF_CURR_SP0 TYPE_SERROR
	// IMPOSSIBLE: Coming from EL1 with SP1
	default CONF_CURR_SPx TYPE_SYNC
	default CONF_CURR_SPx TYPE_IRQ
	default CONF_CURR_SPx TYPE_FIQ
	default CONF_CURR_SPx TYPE_SERROR
	// Coming from EL0 using AArch64
	vector	sync_handler_asm //default CONF_LOWER_A64 TYPE_SYNC
	default CONF_LOWER_A64 TYPE_IRQ
	default CONF_LOWER_A64 TYPE_FIQ
	default CONF_LOWER_A64 TYPE_SERROR
	// IMPOSSIBLE: Coming from EL0 using AArch32
	default CONF_LOWER_A32 TYPE_SYNC
	default CONF_LOWER_A32 TYPE_IRQ
	default CONF_LOWER_A32 TYPE_FIQ
	default CONF_LOWER_A32 TYPE_SERROR
	.balign 0x80 // Make sure no-one fills this gap
	.ltorg
	.size VectorTableEL1, .-VectorTableEL1
