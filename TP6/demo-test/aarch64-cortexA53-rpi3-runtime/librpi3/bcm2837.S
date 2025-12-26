.section .text.ARMaddrToGPUaddr, "ax", %progbits
.balign	4
.globl ARMaddrToGPUaddr;
.type ARMaddrToGPUaddr, %function
ARMaddrToGPUaddr:
	orr	w0, w0, #0xC0000000
	ret
.balign	4
.ltorg
.size	ARMaddrToGPUaddr, .-ARMaddrToGPUaddr
