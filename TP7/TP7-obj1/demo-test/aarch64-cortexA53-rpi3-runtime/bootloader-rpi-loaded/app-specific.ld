/* The entry point of the application */
ENTRY_POINT = _start ;
/* The address where everything is loaded. The entry point
 * must be placed there. */
BOOT_ADDR   = 0x90000 ;
/* The stack size value used for the stacks of the four
 * cores. Only the highest-EL stacks are set this way. */
STACK_SIZE  = 2048 ;
