#ifndef BOOT_H
#define BOOT_H

// Entry point function for all cores
extern void _start(void) ;

// At this address are found 3 uint64_t function pointers.
// They are set to 0 by the Raspberry Pi 3 boot code,
// when not booting in EL3. When one is set to a non-zero
// value, control is given to this address on the
// corresponding core (SPIN_ADDR[0] = Core 1,
// SPIN_ADDR[1] = Core 2, SPIN_ADDR[2] = Core 3).
#define SPIN_ADDR ((volatile uint64_t*)0xE0)

#endif
