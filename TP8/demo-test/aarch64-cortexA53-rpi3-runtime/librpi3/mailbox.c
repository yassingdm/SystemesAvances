#include <stdint.h>
#include <stdarg.h>
#include <librpi3/assert.h>
#include <librpi3/bcm2837.h>
#include <librpi3/mailbox.h>

//================================================================
// Mailbox programming registers.
// From:
// - BCM2835 ARM Peripheral manual
//================================================================

//----------------------------------------------------------------
// The general Raspberry Pi mailbox registers, which
// define the interaction between ARM and VideoCore.
//
// Their specification can be found here:
//   https://github.com/raspberrypi/firmware/wiki/Mailboxes
//----------------------------------------------------------------
struct __attribute__((__packed__, aligned(4))) MailBoxRegisters {
  // Mailbox 0 - from VC to ARM
  const uint32_t Read0; // 0x00 Read data (from VC to ARM)
  uint32_t Unused[3];   // 0x04-0x0F
  uint32_t Peek0;       // 0x10
  uint32_t Sender0;     // 0x14
  uint32_t Status0;     // 0x18 Status of VC to ARM
  uint32_t Config0;     // 0x1C
  
  // Mailbox 1 - from ARM to VC
  uint32_t Write1;      // 0x20 Write data (from ARM to VC)
  uint32_t Unused2[3];  // 0x24-0x2F
  uint32_t Peek1;       // 0x30
  uint32_t Sender1;     // 0x34
  uint32_t Status1;     // 0x38 Status of ARM to VC
  uint32_t Config1;     // 0x3C 
};

//----------------------------------------------------------------
// Mailbox status register values indicating that the mailbox
// is either empty or full. 
// Each mailbox is an 8-deep FIFO of 32-bit words.
//----------------------------------------------------------------
#define MAIL_EMPTY 0x40000000
#define MAIL_FULL  0x80000000




//----------------------------------------------------------------
// Enumerated mailbox power block ID
// https://github.com/raspberrypi/firmware/wiki/Mailboxes
//----------------------------------------------------------------
typedef enum {
  PB_SDCARD = 0x0, // Mailbox Tag Channel SD Card power block 
  PB_UART0 = 0x1,  // Mailbox Tag Channel UART0 power block 
  PB_UART1 = 0x2,  // Mailbox Tag Channel UART1 power block 
  PB_USBHCD = 0x3, // Mailbox Tag Channel USB_HCD power block 
  PB_I2C0 = 0x4,   // Mailbox Tag Channel I2C0 power block 
  PB_I2C1 = 0x5,   // Mailbox Tag Channel I2C1 power block 
  PB_I2C2 = 0x6,   // Mailbox Tag Channel I2C2 power block 
  PB_SPI = 0x7,    // Mailbox Tag Channel SPI power block 
  PB_CCP2TX = 0x8, // Mailbox Tag Channel CCP2TX power block 
} MB_POWER_ID;



//----------------------------------------------------------------
// Static consistency tests on register layout
static_assert(sizeof(struct MailBoxRegisters) == 0x40,
	      "Structure MailBoxRegisters should be 0x40 bytes in size");


//================================================================
// Map the layouts at the good memory addresses.
// The addresses are defined in bcm2837.h
//================================================================
#define MAILBOX ((volatile struct MailBoxRegisters*)RPi_MAILBOX_BASE)


//================================================================
// Sending a message on a given channel
// RETURN: true for success
//================================================================
bool mailbox_write (MAILBOX_CHANNEL channel, uint32_t message)  {
  uint32_t value; // Temporary read value
  if (channel > MB_CHANNEL_GPU) return false; // Channel error
  message &= ~(0xF); // Make sure 4 low channel bits are clear 
  message |= channel; // OR the channel bits to the value 
  do {
    value = MAILBOX->Status1; // Read mailbox1 status from GPU 
  } while ((value & MAIL_FULL) != 0); // Make sure arm mailbox is not full
  MAILBOX->Write1 = message; // Write value to mailbox
  return true; // Write success
}

//================================================================
// Blocking read of one message on the given channel.
// RETURN: The read value for success, 0xFEEDDEAD for failure.
//================================================================
uint32_t mailbox_read (MAILBOX_CHANNEL channel) {
  uint32_t value; // Temporary read value
  if (channel > MB_CHANNEL_GPU) return 0xFEEDDEAD; // Channel error
  do {
    do {
      value = MAILBOX->Status0; // Read mailbox0 status
    } while ((value & MAIL_EMPTY) != 0); // Wait for data in mailbox
    value = MAILBOX->Read0; // Read the mailbox 
  } while ((value & 0xF) != channel); // We have response back
  value &= ~(0xF); // Lower 4 low channel bits are not part of message
  return value; // Return the value
}

//================================================================
// Post and execute the given variadic data onto the tags channel.
// You must provide the correct number of response uint32_t
// variables and a pointer to the response buffer. You nominate
// the number of data uint32_t for the call and fill the variadic
// data in. If you do not want the response data back the use
// NULL for response_buf pointer.
//
// Inputs:
// - response_buf - Pointer to response buffer 
// - data_count   - Number of uint32_t data following
// - variadic uint32_t values for call
// RETURN: True for success and the response data will be set
// with data False for failure and the response buffer is
// untouched.
//================================================================
bool mailbox_tag_message (uint32_t* response_buf,
			  uint8_t data_count,
			  ...) {
  // On the stack, to allow multi-thread execution, but aligned
  uint32_t __attribute__((aligned(16))) message[32];
  
  // Build the message
  {
    // Size of message needed, in bytes
    message[0] = (data_count + 3) * 4;
    // Request processing of a command
    message[1] = 0;
    {
      va_list list;
      // Start variadic argument
      va_start(list, data_count);
      for (int i = 0; i < data_count; i++) {
	// Fetch next variadic
	message[2 + i] = va_arg(list, uint32_t);
      }
      // variadic cleanup
      va_end(list);
    }
    // Set end pointer to zero
    message[data_count + 2] = 0;
  }
  
  // Write message to mailbox
  {
    // Convert the address into a videocore address.
    // Note the double cast, needed to shut up the
    // compiler.
    uint32_t vc_addr =
      ARMaddrToGPUaddr((uint32_t)(uint64_t)message) ;
    mailbox_write(
		  // Channel 8 - send data to video core
		  MB_CHANNEL_TAGS,
		  vc_addr
		  );
  }
  
  // Wait for response from videocore
  mailbox_read(MB_CHANNEL_TAGS);

  // Decode the response
  {
    // Check if the code is 0x80000000, meaning
    // success, or not
    if (message[1] == 0x80000000) {
      if (response_buf) {
	// If buffer NULL used then don't want response
	for (int i = 0; i < data_count; i++) {
	  // Transfer out each response message
	  response_buf[i] = message[2 + i]; 
	}
      }
      return true; // message success
    }
    return false; // Message failed
  }
}

