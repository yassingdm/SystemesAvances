#include <stdint.h>
#include <librpi3/assert.h>
#include <librpi3/arm.h>
#include <librpi3/bcm2837.h>
#include <librpi3/gpio.h>
#include <librpi3/mailbox.h>
#include <librpi3/uart-mini.h>



//================================================================
// Auxiliary control programming registers.
// We need them to enable the Mini UART.
// From:
// - BCM2835 ARM Peripheral manual
//================================================================

//----------------------------------------------------------------
// Both AUX control registers have the same bit affectation.
//----------------------------------------------------------------
struct __attribute__((__packed__, aligned(4))) aux_reg {
  uint32_t MINI_UART : 1 ;
  uint32_t SPI1      : 1 ;
  uint32_t SPI2      : 1 ;
  uint32_t RES0      :29 ;
} ;

//----------------------------------------------------------------
// AUX control memory-mapped registers layout
//----------------------------------------------------------------
struct AUXCtrlRegisters {
  // Check IRQ status
  const struct aux_reg AUXIRQ ;
  // Enable/disable devices or read their status
  struct       aux_reg AUXENB ; 
} ;


//================================================================
// MiniUART programming registers.
// From:
// - BCM2835 ARM Peripheral manual
// - Errata: https://elinux.org/BCM2835_datasheet_errata
//================================================================

//----------------------------------------------------------------
// Mini UART IO register
//----------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    volatile unsigned DATA     :  8;
    volatile unsigned reserved : 24;
  };
  volatile uint32_t Raw32;
} mu_io_reg_t;

//----------------------------------------------------------------
// Mini UART INTERRUPT ENABLE register
// PAGE HAS ERRORS: https://elinux.org/BCM2835_datasheet_errata
// It is essentially same as standard 16550 register IER
//----------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    unsigned RXDI     : 1;
    unsigned TXEI     : 1;
    unsigned LSI      : 1;
    unsigned MSI      : 1;
    unsigned reserved :28;
  };
  uint32_t Raw32;
} mu_ie_reg_t;

//----------------------------------------------------------------
// Mini UART INTERRUPT ID register
//----------------------------------------------------------------
typedef union {
  const struct __attribute__((__packed__)) {
    unsigned PENDING          : 1;
    enum {
      MU_NO_INTERRUPTS  = 0,
      MU_TXE_INTERRUPT  = 1,
      MU_RXD_INTERRUPTS = 2,
    } SOURCE                  : 2;
    unsigned reserved_rd      :29;
  };
  struct __attribute__((__packed__)) {
    unsigned unused           : 1;
    unsigned RXFIFO_CLEAR     : 1;
    unsigned TXFIFO_CLEAR     : 1;
    unsigned reserved_wr      :29;
  };
  uint32_t Raw32;
} mu_ii_reg_t;

//----------------------------------------------------------------
// Mini UART LINE CONTROL register
//----------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    unsigned DATA_LENGTH : 1;
    unsigned reserved    : 5;
    unsigned BREAK       : 1;
    unsigned DLAB        : 1;
    unsigned reserved1   :24;
  };
  uint32_t Raw32;
} mu_lcr_reg_t;

//----------------------------------------------------------------
// Mini UART MODEM CONTROL register
//----------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    unsigned reserved  : 1;
    unsigned RTS       : 1;
    unsigned reserved1 :30;
  };
  uint32_t Raw32;
} mu_mcr_reg_t;

//----------------------------------------------------------------
// Mini UART LINE STATUS register
//----------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    unsigned RXFDA     : 1;
    unsigned RXOE      : 1;
    unsigned reserved  : 3;
    volatile unsigned TXFE      : 1;
    unsigned TXIdle    : 1;
    unsigned reserved1 :25;
  };
  uint32_t Raw32;
} mu_lsr_reg_t;

//----------------------------------------------------------------
// Mini UART MODEM STATUS register
//----------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    unsigned reserved  : 4;
    unsigned CTS       : 1;
    unsigned reserved1 :27;
  };
  uint32_t Raw32;
} mu_msr_reg_t;

//----------------------------------------------------------------
// Mini UART SCRATCH register
//----------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    unsigned USER_DATA : 8;
    unsigned reserved  :24;
  };
  uint32_t Raw32;
} mu_scratch_reg_t;

//----------------------------------------------------------------
// Mini UART CONTROL register
//----------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    unsigned RXE            : 1;
    unsigned TXE            : 1;
    unsigned EnableRTS      : 1;
    unsigned EnableCTS      : 1;
    enum {
      FIFOhas3spaces = 0,
      FIFOhas2spaces = 1,
      FIFOhas1spaces = 2,
      FIFOhas4spaces = 3,
    } RTSflowLevel          : 2;
    unsigned RTSassertLevel : 1;
    unsigned CTSassertLevel : 1;
    unsigned reserved       :24;
  };
  uint32_t Raw32;
} mu_cntl_reg_t;

//----------------------------------------------------------------
// Mini UART STATUS register
//----------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    unsigned RXFDA       : 1;
    unsigned TXFE        : 1;
    unsigned RXIdle      : 1;
    unsigned TXIdle      : 1;
    unsigned RXOE        : 1;
    unsigned TXFF        : 1;
    unsigned RTS         : 1;
    unsigned CTS         : 1;
    unsigned TXFCE       : 1;
    unsigned TX_DONE     : 1;
    unsigned reserved    : 6;
    unsigned RXFIFOLEVEL : 4;
    unsigned reserved1   : 4;
    unsigned TXFIFOLEVEL : 4;
    unsigned reserved2   : 4;
  };
  uint32_t Raw32;
} mu_stat_reg_t;

//----------------------------------------------------------------
// Mini UART BAUDRATE register
//----------------------------------------------------------------
typedef union
{
  struct __attribute__((__packed__))
  {
    unsigned DIVISOR  : 16;
    unsigned reserved : 16;
  };
  uint32_t Raw32;
} mu_baudrate_reg_t;

//----------------------------------------------------------------
// Mini UART memory-mapped registers layout
//----------------------------------------------------------------
struct __attribute__((__packed__, aligned(4))) MiniUARTRegisters {
  volatile mu_io_reg_t IO;           // +0x00
  mu_ie_reg_t IER;          // +0x04
  mu_ii_reg_t IIR;          // +0x08
  mu_lcr_reg_t LCR;         // +0x0C
  mu_mcr_reg_t MCR;         // +0x10
  const volatile mu_lsr_reg_t LSR;   // +0x14
  const mu_msr_reg_t MSR;   // +0x18
  mu_scratch_reg_t SCRATCH; // +0x1C
  mu_cntl_reg_t CNTL;       // +0x20
  const mu_stat_reg_t STAT; // +0x24
  mu_baudrate_reg_t BAUD;   // +0x28
};


//----------------------------------------------------------------
// Static consistency tests on register layouts
static_assert(sizeof(struct AUXCtrlRegisters) == 8,
	      "AUXCtrlRegisters should be 8 bytes in size");
static_assert(sizeof(struct MiniUARTRegisters) == 0x2C,
	      "MiniUARTRegisters should be 0x2C bytes in size");


//================================================================
// Map the layouts at the good memory addresses.
// The addresses are defined in bcm2837.h
//================================================================
#define AUXCTRL  ((volatile struct AUXCtrlRegisters*)RPi_AUX_BASE)
#define MINIUART ((volatile struct MiniUARTRegisters*)RPi_MINIUART_BASE)


//================================================================
// MiniUART routines
//================================================================

//----------------------------------------------------------------
// Initialize - return value is "true" upon success,
//              "false" otherwise.
bool miniuart_init(uint32_t baudrate) {
  // Used by mailbox routines
  uint32_t Buffer[5] = { 0 };
  // Get core clock frequency
  mailbox_tag_message(&Buffer[0],
		      5,
		      MAILBOX_TAG_GET_CLOCK_RATE,
		      8,
		      8,
		      CLK_CORE_ID,
		      0);
  // Calculate divisor and check if it's correct
  uint32_t Divisor = (Buffer[4] / (baudrate * 8)) - 1;
  if (Divisor > 0xFFFF) {
    return false; // Invalid baudrate can't set
  }
  
  DMB(sy); // First access to hardware so memory barrier 
  // Enable miniuart
  AUXCTRL->AUXENB.MINI_UART = 1 ;
  DMB(sy); // First access to hardware so memory barrier 
  
  MINIUART->CNTL.RXE = 0; // Disable receiver
  MINIUART->CNTL.TXE = 0; // Disable transmitter

  MINIUART->LCR.DATA_LENGTH = 1;  // Data length = 8 bits
  MINIUART->MCR.RTS = 0;          // Set RTS line high
  MINIUART->IIR.RXFIFO_CLEAR = 1; // Clear RX FIFO
  MINIUART->IIR.TXFIFO_CLEAR = 1; // Clear TX FIFO
  
  MINIUART->BAUD.DIVISOR = Divisor; // Set the divisor
  
  gpio_setup(14, GPIO_ALTFUNC5); // GPIO 14 to ALT FUNC5 mode
  gpio_setup(15, GPIO_ALTFUNC5); // GPIO 15 to ALT FUNC5 mode
  
  MINIUART->CNTL.RXE = 1; // Enable receiver
  MINIUART->CNTL.TXE = 1; // Enable transmitter
  DMB(sy); // Last access to hardware so memory barrier
  return true; // Return success
}

//----------------------------------------------------------------
//
char miniuart_getc (void)
{
  DMB(sy); // First access to hardware so memory barrier 
  while (MINIUART->LSR.RXFDA == 0) {};
  return(MINIUART->IO.DATA);
  DMB(sy); // Last access to hardware so memory barrier
}

//----------------------------------------------------------------
//
void miniuart_putc (const char c)
{
  DMB(sy); // First access to hardware so memory barrier 
  while (MINIUART->LSR.TXFE == 0) {};
  // Only the lower 8 bits are important, but I will set the
  // whole word.
  MINIUART->IO.Raw32 = c; 
  DMB(sy); // Last access to hardware so memory barrier
}

//----------------------------------------------------------------
//
void miniuart_puts_noend (const char *str)
{
  while (*str) {
    if(*str == '\n')
      miniuart_putc('\r');
    miniuart_putc(*str++);
  }
}

//----------------------------------------------------------------
//
void miniuart_puts (const char *str)
{
  miniuart_puts_noend(str) ;
  miniuart_putc(0) ;// trailing \0
}

//----------------------------------------------------------------
//
uint32_t miniuart_read_uint32(void) {
  union {
    char     bytes[4] ;
    uint32_t val ;
  } read_buf ;
  int i ;
  for(i=0;i<4;i++)
    read_buf.bytes[i] = miniuart_getc() ;
  return read_buf.val ;
}
