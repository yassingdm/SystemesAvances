#include <stdint.h>
#include <librpi3/mailbox.h>
#include <librpi3/speed.h>

//=============================================================
// Routines for getting/setting CPU/SoC/etc. speeds.
// They use mailboxes to access the VideoCore.
//=============================================================

//-------------------------------------------------------------
// Obtain the max possible ARM CPU speed.
uint32_t arm_getmaxspeed (void) {
  uint32_t Buffer[5] = { 0 };
  if(mailbox_tag_message(&Buffer[0],
			 5, // Number of arguments.
			 MAILBOX_TAG_GET_MAX_CLOCK_RATE,
			 8, // ?
			 8, // ?
			 CLK_ARM_ID, // ARM clock Id
			 0  // Need to reserve space for the response.
			    // Value 0 means no new tag.
			 )
     ) {
    return Buffer[4] ;
  } else {
    return UINT32_MAX ;
  }
}

//-------------------------------------------------------------
// Set the ARM CPU speed. We will usually fix it to its maximum.
// By default, ARM speed can vary, but we need to fix it
// (assuming it remains as fixed), both for real time, and because
// the baud rate of the mini-UART (which we must use) is computed
// from it.
bool arm_setspeed (uint32_t speed) {
  uint32_t Buffer[5] = { 0 };
  if (mailbox_tag_message(&Buffer[0],
			  5,
			  MAILBOX_TAG_SET_CLOCK_RATE,
			  8, // ?
			  8, // ?
			  CLK_ARM_ID, // ARM clock Id
			  speed
			  )
      ) {
    return true ;
  } else {
    return false;
  }
}

//-------------------------------------------------------------
//
uint32_t soc_getspeed (void) {
  uint32_t Buffer[5] = { 0 };
  mailbox_tag_message(&Buffer[0],
		      5,
		      MAILBOX_TAG_GET_CLOCK_RATE,
		      8, // ?
		      8, // ?
		      CLK_CORE_ID, // SOC core clock ID
		      0 // Need to reserve space for the response.
			// Value 0 means no new tag.
		      );
  return Buffer[4] ;
}
