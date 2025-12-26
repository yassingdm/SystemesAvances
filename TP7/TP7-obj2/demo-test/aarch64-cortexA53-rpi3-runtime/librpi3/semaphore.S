//"========================================================================="
//	semaphore_get -- AARCH64 Pi3 code
//	C Function: "void semaphore_get (uint32_t* sem);"
//	Entry: X0 will have semaphore address value
//	Return: nothing
//"========================================================================="
.section .text.semaphore_get, "ax", %progbits
.balign	4
.globl semaphore_get;
.type semaphore_get, %function
semaphore_get:
	mov   w2, 1
//	.p2align 2 // DPB: why?
.lockloop:
	ldaxr   w1, [x0]       // load-acquire exclusive
	stxr    w3, w2, [x0]   // try to store 1
	cbnz    w3, .lockloop  // check if store succeeded
	cbnz    w1, .lockloop  // check if the initial value was 0
	//dmb 	ish            // memory barrier
	ret
.balign	4
.ltorg
.size	semaphore_get, .-semaphore_get

//"========================================================================="
//	semaphore_release -- Composite Pi1, Pi2 & Pi3 code
//	C Function: "void semaphore_release (uint32_t* sem);"
//	Entry: X0 will have semaphore address value
//	Return: nothing
//"========================================================================="
.section .text.semaphore_release, "ax", %progbits
.balign	4
.globl semaphore_release;
.type semaphore_release, %function
semaphore_release:
	stlrb	wzr, [x0] // store release 0
	dmb 	ish       // memory barrier
	ret
.balign	4
.ltorg
.size	semaphore_release, .-semaphore_release


//"========================================================================="
//	api_stlr - API access to store release
//"========================================================================="
.section .text.api_stlr, "ax", %progbits
.balign	4
.globl api_stlr;
.type api_stlr, %function
api_stlr:
	stlr	x1, [x0]
	// dmb 	ish
	ret
.balign	4
.ltorg
.size	api_stlr, .-api_stlr
	
//"========================================================================="
//	api_ldar - API access to load acquire
//"========================================================================="
.section .text.api_ldar, "ax", %progbits
.balign	4
.globl api_ldar;
.type api_ldar, %function
api_ldar:
	ldar	x0, [x0]
	// dmb 	ish
	ret
.balign	4
.ltorg
.size	api_ldar, .-api_ldar
	

//"========================================================================="
//	get_sp - obtain the stack pointer
//"========================================================================="
.section .text.get_sp, "ax", %progbits
.balign	4
.globl get_sp;
.type get_sp, %function
get_sp:
	mov	x0, sp
	ret
.balign	4
.ltorg
.size	get_sp, .-get_sp
	

	
