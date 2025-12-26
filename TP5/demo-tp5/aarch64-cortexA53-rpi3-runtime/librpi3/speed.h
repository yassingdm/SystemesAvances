#ifndef SPEED_H
#define SPEED_H

#include <stdint.h>

//=============================================================
// Routines for getting/setting CPU/SoC/etc. speeds.
// They use mailboxes to access the VideoCore.
//=============================================================

// Obtain the max possible ARM CPU speed.
uint32_t arm_getmaxspeed (void) ;

// Set the ARM CPU speed. We will usually fix it to its maximum.
// By default, ARM speed can vary, but we need to fix it
// (assuming it remains as fixed), both for real time, and because
// the baud rate of the mini-UART (which we must use) is computed
// from it.
bool arm_setspeed (uint32_t speed) ;

// Get the SoC speed.
uint32_t soc_getspeed (void) ;

#endif
