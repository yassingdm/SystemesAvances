#ifndef BCM2837_H
#define BCM2837_H

#include <stdint.h>

//==============================================================
// The base address of the memory-mapped devices on the RPi3
#define RPi_IO_BASE          0x3F000000

//==============================================================
// The offsets of various SoC memory-mapped devices w.r.t.
// the RPi_IO_BASE. They are the same in RPi1, RPi2, and Rpi3,
// being defined in:
// - BCM2835 ARM Peripheral manual
//==============================================================

// Mailbox registers base
#define RPi_MAILBOX_BASE    (RPi_IO_BASE + 0x0000B880)
// GPIO registers base
#define RPi_GPIO_BASE       (RPi_IO_BASE + 0x00200000)
// AUX control base (needed to enable miniuart)
#define RPi_AUX_BASE        (RPi_IO_BASE + 0x00215000)
// Mini UART base
#define RPi_MINIUART_BASE   (RPi_IO_BASE + 0x00215040)

//==============================================================
// The base address of the local peripheral devices of the
// Raspberry Pi 3.
#define LOCAL_PERIPH_BASE          0x40000000



//==============================================================
// Convert addresses between ARM and GPU
volatile uint32_t ARMaddrToGPUaddr(volatile uint32_t addr) ;


//==============================================================
// Local ARM peripherals
// Source: QA7_rev3.4.pdf, page 7
//==============================================================

//--------------------------------------------------------------
// Local timer interrupt routing
//--------------------------------------------------------------
typedef union {
  struct  __attribute__((__packed__)) {
    enum {
      LOCALTIMER_TO_CORE0_IRQ = 0,
      LOCALTIMER_TO_CORE1_IRQ = 1,
      LOCALTIMER_TO_CORE2_IRQ = 2,
      LOCALTIMER_TO_CORE3_IRQ = 3,
      LOCALTIMER_TO_CORE0_FIQ = 4,
      LOCALTIMER_TO_CORE1_FIQ = 5,
      LOCALTIMER_TO_CORE2_FIQ = 6,
      LOCALTIMER_TO_CORE3_FIQ = 7,
    } Routing                       : 3;
    unsigned unused                 :29;
  };
  uint32_t Raw32;
} timer_route_reg_t;

//--------------------------------------------------------------
// Local timer control and status
//
// There is a single local timer which can generate interrupts.
// The local timer ALWAYS gets its timing pulses from the
// Crystal clock. You get a 'timing pulse' every clock EDGE.
// Thus a 19.2 MHz crystal gives 38.4 M pulses/second.
//
//--------------------------------------------------------------
typedef union {
  struct  __attribute__((__packed__)) {
    unsigned ReloadValue : 28;
    unsigned TimerEnable :  1;
    unsigned IntEnable   :  1;
    unsigned unused      :  1;
    unsigned IntPending  :  1;
  };
  uint32_t Raw32;
} timer_ctrl_status_reg_t;

//--------------------------------------------------------------
// Local timer clear and reload
//--------------------------------------------------------------
typedef union {
  struct  __attribute__((__packed__)) {
    unsigned unused   : 30;
    unsigned Reload   :  1;
    unsigned IntClear :  1;
  };
  uint32_t Raw32;
} timer_clr_reload_reg_t;

//--------------------------------------------------------------
// For the signification of the various CNTXXX, see here:
//   https://developer.arm.com/documentation/ddi0500/j/Generic-Timer/Generic-Timer-functional-description
//--------------------------------------------------------------
typedef union {
  struct  __attribute__((__packed__))  {
    unsigned nCNTPSIRQ_IRQ  : 1;	
    unsigned nCNTPNSIRQ_IRQ : 1;
    unsigned nCNTHPIRQ_IRQ  : 1;	
    unsigned nCNTVIRQ_IRQ   : 1;	
    unsigned nCNTPSIRQ_FIQ  : 1;	
    unsigned nCNTPNSIRQ_FIQ : 1;
    unsigned nCNTHPIRQ_FIQ  : 1;	
    unsigned nCNTVIRQ_FIQ   : 1;	
    unsigned reserved       :24;	
  };
  uint32_t Raw32;
} generic_timer_int_ctrl_reg_t;


//--------------------------------------------------------------
// 
//--------------------------------------------------------------
typedef union {
  struct  __attribute__((__packed__))  {
    unsigned Mailbox0_IRQ  : 1;
    unsigned Mailbox1_IRQ  : 1;
    unsigned Mailbox2_IRQ  : 1;
    unsigned Mailbox3_IRQ  : 1;
    unsigned Mailbox0_FIQ  : 1;
    unsigned Mailbox1_FIQ  : 1;
    unsigned Mailbox2_FIQ  : 1;
    unsigned Mailbox3_FIQ  : 1;
    unsigned reserved : 24;
  };
  uint32_t Raw32;
} mailbox_int_ctrl_reg_t;

//--------------------------------------------------------------
// Core interrupt source
//--------------------------------------------------------------
typedef union {
  struct  __attribute__((__packed__)) {
    unsigned CNTPSIRQ   : 1;
    unsigned CNTPNSIRQ  : 1;
    unsigned CNTHPIRQ   : 1;
    unsigned CNTVIRQ    : 1;
    unsigned Mbox0      : 1;
    unsigned Mbox1      : 1;
    unsigned Mbox2      : 1;
    unsigned Mbox3      : 1;
    unsigned GPU        : 1;
    unsigned PMU        : 1;
    unsigned AXI        : 1; // Only possible on Core0, IRQ
    unsigned LocalTimer : 1;
    unsigned RES0       :20;
  };
  uint32_t Raw32;	
} core_int_source_reg_t;

//--------------------------------------------------------------
// Timer value.
// - When TimerLS is read, it also forces a store of TimerMS,
//   which is provided the next time TimerMS is read.
// - When TimerLS is written, the value is put on hold, and
//   is written when TimerMS is written.
// TimerLS operation must always precede TimerMS operation of
// the same kind. Performing only a TimerMS operation makes no
// sense.
//--------------------------------------------------------------

//--------------------------------------------------------------
// Timer prescaler.
// Must be <= 2^31.
//
// timer_frequency = ((2^31)/prescaler) * input_frequency
//--------------------------------------------------------------

//--------------------------------------------------------------
// Timer control register
//--------------------------------------------------------------
typedef union {
  struct __attribute__((__packed__)) {
    unsigned RES0_0       : 8;
    // 0 -> APB clock,  1 -> crystal clock
    unsigned CLK_SRC      : 1;
    // 1 -> increment clock by 2, instead of by 1
    unsigned CLK_BY_2     : 1;
    unsigned RES0_1       :22;
  };
  uint32_t Raw32;	
} timer_ctrl_reg_t;

//--------------------------------------------------------------
// 
//--------------------------------------------------------------
struct __attribute__((__packed__, aligned(4))) LocalPeripherals {
  volatile timer_ctrl_reg_t TimerControl ;                      //0x00
  volatile uint32_t UNUSED0 ;                                   //0x04
  volatile uint32_t TimerPrescaler ;                            //0x08
  volatile uint32_t GPUInterruptsRouting ;                      //0x0C
  // Performance Monitor Interrupts routing - PMIR
  volatile uint32_t PMIR_set ;                                  //0x10
  volatile uint32_t PMIR_clear ;                                //0x14
  volatile uint32_t UNUSED1 ;                                   //0x18
  // Core timer access
  volatile uint32_t TimerLS ;                                   //0x1C
  volatile uint32_t TimerMS ;                                   //0x20
  // Local timer: Routing to IRQs and FIQs.
  volatile timer_route_reg_t TimerRouting;                      //0x24
  volatile uint32_t UNUSED2;                                    //0x28
  volatile uint32_t AXIOutstandingCounters;                     //0x2C
  volatile uint32_t AXIOutstandingIRQ;                          //0x30
  // Local timer: control and status
  volatile timer_ctrl_status_reg_t TimerControlStatus;          //0x34
  // Local timer: write flags
  volatile timer_clr_reload_reg_t TimerClearReload;             //0x38
  volatile uint32_t UNUSED3;                                    //0x3C
  // Per-core interrupt control.
  volatile generic_timer_int_ctrl_reg_t CoreTimerIntControl[4]; //0x40
  volatile mailbox_int_ctrl_reg_t  CoreMailboxIntControl[4];    //0x50
  // Determine the source of an interrupt
  volatile const core_int_source_reg_t CoreIRQSource[4];        //0x60
  volatile const core_int_source_reg_t CoreFIQSource[4];        //0x70
  // Local mailbox access. The protocol is as follows:
  // - [Write function] When writing v32 on MBoxWR[i][j], 
  //   the bits that are 1 in v32 are positioned to 1 in
  //   mailbox j of core i.
  // - [Read function] Reading from MBoxRC[i][j] gives 
  //   the current value of mailbox j of core i.
  // - [Clear function] Writing v32 on MBoxRC[i][j] will 
  //   set to 0 all the mailbox bits that are 1 in v32.
  // - [Interrupt] While the value of mailbox j of 
  //   core i is not 0, the mailbox will generate an 
  //   interrupt to core i (but one has to enable these
  //   interrupts to be able to receive them).
  volatile uint32_t MBoxWR[4][4];                               //0x80
  volatile uint32_t MBoxRC[4][4];                               //0xC0
};

#define LOC_PERIPH ((volatile struct LocalPeripherals*)LOCAL_PERIPH_BASE)


#endif
