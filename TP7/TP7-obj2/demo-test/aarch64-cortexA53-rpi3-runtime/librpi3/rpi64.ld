INCLUDE app-specific.ld

OUTPUT_ARCH(aarch64)
ENTRY(ENTRY_POINT)

/* Specify the memory areas */
MEMORY
{
	/* 128MB which is half memory on smallest memory PI, 
	 * assumes GPU has other half */
	RAM (xrw)      : ORIGIN = 0x0 , LENGTH = 0x8000000 
}

SECTIONS
{
	/* First, the init section, at the entry point address.
	 * I assume the BOOT_ADDR is aligned on 16 bytes, as
	 * explained in required.h. */
	.init BOOT_ADDR : {
		__start__ = . ;
		KEEP(*(.init))
	} > RAM

	/* Next we put the rest of the executable code. */
	.text : {
		. = ALIGN(4);
		*(.text .text.*)
		*(.vector*)
	} > RAM

	/* Allocated data sections and BSS */
	.rodata : {
       		. = ALIGN(4);
	        *(.rodata .rodata.*)
   	} > RAM
	.data : {
		. = ALIGN(4);
		*(.data .data.*)
	} > RAM
	.bss : {
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss .bss.*)
 		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
   	} > RAM

	.stacks : {
		     /* Stack must be aligned on 16 byte boundary
		      * according to AAPCS64 call standard */
        	     . = ALIGN(16);
        	     . = . + STACK_SIZE ;
        	     __stack_core0 = .;
        	     . = . + STACK_SIZE ;
        	     __stack_core1 = .;
        	     . = . + STACK_SIZE ;
        	     __stack_core2 = .;
        	     . = . + STACK_SIZE ;
        	     __stack_core3 = .;
    	} > RAM
	/* End of linker allocation. Beyond this point I can 
         * allocate whatever I want. I align this on 16 bytes
         * so that I can use it as stack. */
	 . = ALIGN(16);
	__end__ = . ;
	

	/*
	 * Finally comes everything else. A fun trick here is to put all other 
	 * sections into this section, which will be discarded by default.
	 */
	/DISCARD/ : {
		*(*)
	}
}
