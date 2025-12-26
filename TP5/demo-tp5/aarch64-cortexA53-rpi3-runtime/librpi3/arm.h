#ifndef ARM_REGISTERS_H
#define ARM_REGISTERS_H

#include <stdint.h>

//==============================================================
// Memory barrier routines - inline assembly
#define ISB(option) asm volatile ("isb " #option : : : "memory")
#define DSB(option) asm volatile ("dsb " #option : : : "memory")
#define DMB(option) asm volatile ("dmb " #option : : : "memory")

//==============================================================
// Obtain the current stack pointer
__attribute__((always_inline))
inline uint64_t GetSP(void) {
  uint64_t result ;
  asm volatile("mov %[stack], sp":[stack]"=r"(result)::"memory") ;
  return result ;
}

//==============================================================
// Mask and unmask interrupts
typedef 
union {
  struct __attribute__((__packed__)) {
    uint8_t F      : 1 ;
    uint8_t I      : 1 ;
    uint8_t A      : 1 ;
    uint8_t D      : 1 ;
    uint8_t unused : 4 ;
  } ;
  uint8_t Raw8 ;
} daif_mask ;

__attribute__((always_inline))
inline void MaskAllInterrupts(void) {
  asm volatile ("msr daifset, 0xf;") ;
}
__attribute__((always_inline))
inline void UnmaskAllInterrupts(void) {
  asm volatile ("msr daifclr, 0xf;") ;
}

//==============================================================
// Access to EL2 and EL1 stack pointers
__attribute__((always_inline))
inline void write_sp_el2(uint64_t reg) {
  asm volatile("msr SP_EL2, %[addr]"::[addr]"r"(reg):);
}
__attribute__((always_inline))
inline void write_sp_el1(uint64_t reg) {
  asm volatile("msr SP_EL1, %[addr]"::[addr]"r"(reg):);
}
__attribute__((always_inline))
inline void write_sp_el0(uint64_t reg) {
  asm volatile("msr SP_EL0, %[addr]"::[addr]"r"(reg):);
}


//==============================================================
// Access to core identification registers
__attribute__((always_inline))
inline uint64_t read_mpidr_el1 (void) {
  uint64_t result ;
  asm volatile("mrs %[addr], MPIDR_EL1;"
	       :[addr]"=r"(result)) ;
  return result ;
}
__attribute__((always_inline))
inline uint64_t read_midr_el1 (void) {
  uint64_t result ;
  asm volatile("mrs %[addr], MIDR_EL1;"
	       :[addr]"=r"(result)
	       :// No inputs
	       :"x0") ;
  return result ;
}
__attribute__((always_inline))
inline void write_vpidr_el2(uint64_t reg) {
  asm volatile("msr VPIDR_EL2, %[addr]"::[addr]"r"(reg):);
}
__attribute__((always_inline))
inline void write_vmpidr_el2(uint64_t reg) {
  asm volatile("msr VMPIDR_EL2, %[addr]"::[addr]"r"(reg):);
}


__attribute__((always_inline))
inline uint8_t GetCoreID (void) {
  return read_mpidr_el1() & 0b11 ;
}


//==============================================================
// Access to FAR_EL1 and FAR_EL2
__attribute__((always_inline))
inline uint64_t read_far_el1(void) {
  register uint64_t res ;
  asm volatile("mrs %[addr], FAR_EL1;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline uint64_t read_far_el2(void) {
  register uint64_t res ;
  asm volatile("mrs %[addr], FAR_EL2;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline uint64_t read_far_el3(void) {
  register uint64_t res ;
  asm volatile("mrs %[addr], FAR_EL3;":[addr]"=r"(res)::);
  return res ;
}


//==============================================================
// Structure and access to ESR_EL1, ESR_EL2, and ESR_EL3
typedef union {
  struct __attribute__((__packed__)) {
    unsigned ISS  :25 ;//25
    unsigned IL   : 1 ;//26
    // I split EC in two:
    // - EC_H seems to identify the class of problem
    //   (e.g. all supervisory calls have the same)
    // - EC_L identifies choices, e.g. the type of
    //   supervisory call (e.g. HVC vs. SVC vs. SMC).
    unsigned EC_L : 2 ;
    unsigned EC_H : 4 ;//32
    //    uint64_t RES0 :32 ;//64
  } ;
  struct __attribute__((__packed__)) {
    unsigned Imm16  :16 ;
    unsigned UNUSED :16 ;
  } ;
  uint64_t Raw32;
} esr_reg_t;

__attribute__((always_inline))
inline esr_reg_t read_esr_el1(void) {
  register esr_reg_t res ;
  asm volatile("mrs %[addr], ESR_EL1;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline esr_reg_t read_esr_el2(void) {
  register esr_reg_t res ;
  asm volatile("mrs %[addr], ESR_EL2;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline esr_reg_t read_esr_el3(void) {
  register esr_reg_t res ;
  asm volatile("mrs %[addr], ESR_EL3;":[addr]"=r"(res)::);
  return res ;
}

//==============================================================
// Writing the address of the vector tables into
// registers VBAR_EL1 and VBAR_EL2
__attribute__((always_inline))
inline void write_vbar_el1(uint64_t adr) {
  asm volatile("msr VBAR_EL1, %[addr];"::[addr]"r"(adr):);
}
__attribute__((always_inline))
inline void write_vbar_el2(uint64_t adr) {
  asm volatile("msr VBAR_EL2, %[addr];"::[addr]"r"(adr):);
}
__attribute__((always_inline))
inline void write_vbar_el3(uint64_t adr) {
  asm volatile("msr VBAR_EL3, %[addr];"
	       "isb"::[addr]"r"(adr):"memory");
}


//==============================================================
// Access to CurrentEL
__attribute__((always_inline))
inline uint64_t read_current_el(void) {
  uint64_t res ;
  asm volatile("mrs %[addr], CurrentEL;":[addr]"=r"(res)::);
  return (res>>2) ;
}

//==============================================================
// Structure and access to CPACR_EL1
typedef union {
  struct __attribute__((__packed__)) {
    unsigned RES0_0  :16 ;//16
    unsigned ZEN     : 2 ;//18
    unsigned RES0_1  : 2 ;//20
    unsigned FPEN    : 2 ;//22
    unsigned RES0_2  : 6 ;//28
    unsigned TTA     : 1 ;//29
    uint64_t RES0_3  :35 ;//64
  } ;
  uint64_t Raw64;
} cpacr_el1_reg_t;

__attribute__((always_inline))
inline cpacr_el1_reg_t read_cpacr_el1(void) {
  register cpacr_el1_reg_t res ;
  asm volatile("mrs %[addr], CPACR_EL1;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_cpacr_el1(cpacr_el1_reg_t reg) {
  asm volatile("msr CPACR_EL1, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}


//==============================================================
// Register HSTR_EL2 is not used, because I don't use AArch32.
// I only create a function to set this register to RES0.
__attribute__((always_inline))
inline void reset_hstr_el2(void) {
  asm volatile("msr HSTR_EL2, xzr") ;
}



//==============================================================
// Structure and access to CPTR_EL3
typedef union {
  struct __attribute__((__packed__)) {
    unsigned RES0_0  : 8 ;// 8
    unsigned EZ      : 1 ;// 9
    unsigned RES0_1  : 1 ;//10
    unsigned TFP     : 1 ;//11
    unsigned RES0_2  : 9 ;//20
    unsigned TTA     : 1 ;//21
    unsigned RES0_3  : 9 ;//30
    unsigned TAM     : 1 ;//31
    unsigned TCPAC   : 1 ;//32
    unsigned RES0_4  :32 ;//64
  } ;
  uint64_t Raw64;
} cptr_el3_reg_t;

__attribute__((always_inline))
inline cptr_el3_reg_t read_cptr_el3(void) {
  register cptr_el3_reg_t res ;
  asm volatile("mrs %[addr], CPTR_EL3;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_cptr_el3(cptr_el3_reg_t reg) {
  asm volatile("msr CPTR_EL3, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}

//==============================================================
// Structure and access to CPTR_EL2
typedef union {
  struct __attribute__((__packed__)) {
    unsigned RES1_0  : 8 ;// 8
    unsigned TZ      : 1 ;// 9
    unsigned RES1_1  : 1 ;//10
    unsigned TFP     : 1 ;//11
    unsigned RES0_0  : 1 ;//12
    unsigned RES1_2  : 2 ;//14
    unsigned RES0_1  : 6 ;//20
    unsigned TTA     : 1 ;//21
    unsigned RES0_2  : 9 ;//30
    unsigned TAM     : 1 ;//31
    unsigned TCPAC   : 1 ;//32
    unsigned RES0_3  :32 ;//64
  } ;
  uint64_t Raw64;
} cptr_el2_reg_t;

__attribute__((always_inline))
inline cptr_el2_reg_t read_cptr_el2(void) {
  register cptr_el2_reg_t res ;
  asm volatile("mrs %[addr], CPTR_EL2;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_cptr_el2(cptr_el2_reg_t reg) {
  asm volatile("msr CPTR_EL2, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}



//==============================================================
// Structure and access to HCR_EL2
//
// Attention: field E2H can only be 0 on the RPi3/Cortex A53.
// For instance, if one tries to use TTBR1_EL2, which should
// exist if E2H is set, the compiler throws an error.
typedef union {
  struct __attribute__((__packed__)) {
    unsigned VM      : 1 ;
    unsigned SWIO    : 1 ;
    unsigned PTW     : 1 ;
    unsigned FMO     : 1 ;
    unsigned IMO     : 1 ;
    unsigned AMO     : 1 ;
    unsigned VF      : 1 ;
    unsigned VI      : 1 ;
    unsigned VSE     : 1 ;
    unsigned FB      : 1 ;
    unsigned BSU     : 2 ;
    unsigned DC      : 1 ;
    unsigned TWI     : 1 ;
    unsigned TWE     : 1 ;
    unsigned TID0    : 1 ;
    unsigned TID1    : 1 ;
    unsigned TID2    : 1 ;
    unsigned TID3    : 1 ;
    unsigned TSC     : 1 ;//20
    unsigned TIDCP   : 1 ;
    unsigned TACR    : 1 ;
    unsigned TSW     : 1 ;
    unsigned TPCP    : 1 ;
    unsigned TPU     : 1 ;
    unsigned TTLB    : 1 ;
    unsigned TVM     : 1 ;
    unsigned TGE     : 1 ;
    unsigned TDZ     : 1 ;
    unsigned HCD     : 1 ;
    unsigned TRVM    : 1 ;
    unsigned RW      : 1 ;
    unsigned CD      : 1 ;
    unsigned ID      : 1 ;
    unsigned E2H     : 1 ;
    unsigned TLOR    : 1 ;
    unsigned TERR    : 1 ;
    unsigned TEA     : 1 ;
    unsigned MIOCNCE : 1 ;
    unsigned RES0_1  : 1 ;
    unsigned APK     : 1 ;
    unsigned API     : 1 ;
    unsigned NV      : 1 ;
    unsigned NV1     : 1 ;
    unsigned AT      : 1 ;
    unsigned NV2     : 1 ;
    unsigned FWB     : 1 ;
    unsigned FIEN    : 1 ;//48
    unsigned RES0_2  :16 ;//64
  } ;
  uint64_t Raw64;
} hcr_el2_reg_t;

__attribute__((always_inline))
inline hcr_el2_reg_t read_hcr_el2(void) {
  register hcr_el2_reg_t res ;
  asm volatile("mrs %[addr], HCR_EL2;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_hcr_el2(hcr_el2_reg_t reg) {
  asm volatile("msr HCR_EL2, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}


//==============================================================
// Structure and access to SCR_EL3
//
typedef union {
  struct __attribute__((__packed__)) {
    unsigned NS      : 1 ;
    unsigned IRQ     : 1 ;
    unsigned FIQ     : 1 ;
    unsigned EA      : 1 ;
    unsigned RES1_0  : 2 ;
    unsigned RES0_0  : 1 ;    
    unsigned SMD     : 1 ;
    unsigned HCE     : 1 ;
    unsigned SIF     : 1 ;
    unsigned RW      : 1 ;
    unsigned ST      : 1 ;
    unsigned TWI     : 1 ;
    unsigned TWE     : 1 ;
    unsigned TLOR    : 1 ;
    unsigned TERR    : 1 ;
    unsigned APK     : 1 ;
    unsigned API     : 1 ;
    unsigned EEL2    : 1 ;
    unsigned EASE    : 1 ;
    unsigned NMEA    : 1 ;
    unsigned FIEN    : 1 ;
    uint64_t RES0_1  :42 ;
  } ;
  uint64_t Raw64;
} scr_el3_reg_t;

__attribute__((always_inline))
inline scr_el3_reg_t read_scr_el3(void) {
  register scr_el3_reg_t res ;
  asm volatile("mrs %[addr], SCR_EL3;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_scr_el3(scr_el3_reg_t reg) {
  asm volatile("msr SCR_EL3, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}


//==============================================================
// Access to return address registers ELR_EL3, ELR_EL2,
// and ELR_EL1
__attribute__((always_inline))
inline uint64_t read_elr_el3(void) {
  register uint64_t res ;
  asm volatile("mrs %[addr], ELR_EL3;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_elr_el3(uint64_t reg) {
  asm volatile("msr ELR_EL3, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}
__attribute__((always_inline))
inline uint64_t read_elr_el2(void) {
  register uint64_t res ;
  asm volatile("mrs %[addr], ELR_EL2;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_elr_el2(uint64_t reg) {
  asm volatile("msr ELR_EL2, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}
__attribute__((always_inline))
inline uint64_t read_elr_el1(void) {
  register uint64_t res ;
  asm volatile("mrs %[addr], ELR_EL1;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_elr_el1(uint64_t reg) {
  asm volatile("msr ELR_EL1, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}

//==============================================================
// Processor state
typedef enum {
  EL0t = 0b0000, // EL0 using SP0
  EL1t = 0b0100, // EL1 using SP0
  EL1h = 0b0101, // EL1 using SP1
  EL2t = 0b1000, // EL2 using SP0
  EL2h = 0b1001, // EL2 using SP2
  EL3t = 0b1100, // EL3 using SP0
  EL3h = 0b1101  // EL3 using SP3
} AARCH64_STATE ;

//==============================================================
// Structure and access to SPSR_EL3, SPSR_EL2, and SPSR_EL1
typedef union {
  struct __attribute__((__packed__)) {
    AARCH64_STATE M03    :  4 ; // 4
    unsigned      M4     :  1 ; // 5
    unsigned      RES0_0 :  1 ; // 6
    unsigned      F      :  1 ; // 7
    unsigned      I      :  1 ; // 8
    unsigned      A      :  1 ; // 9
    unsigned      D      :  1 ; //10
    unsigned      RES0_1 : 10 ; //20
    unsigned      IL     :  1 ; //21
    unsigned      SS     :  1 ; //22
    unsigned      PAN    :  1 ; //23
    unsigned      UAO    :  1 ; //24
    unsigned      DIT    :  1 ; //25
    unsigned      RES0_2 :  3 ; //28
    unsigned      V      :  1 ; //29
    unsigned      C      :  1 ; //30
    unsigned      Z      :  1 ; //31
    unsigned      N      :  1 ; //32
    unsigned      RES0_3 : 32 ; //64
  };
  uint64_t Raw64;
} spsr_reg_t;

__attribute__((always_inline))
inline spsr_reg_t read_spsr_el3(void) {
  register spsr_reg_t res ;
  asm volatile("mrs %[addr], SPSR_EL3;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_spsr_el3(spsr_reg_t reg) {
  asm volatile("msr SPSR_EL3, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}
__attribute__((always_inline))
inline spsr_reg_t read_spsr_el2(void) {
  register spsr_reg_t res ;
  asm volatile("mrs %[addr], SPSR_EL2;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_spsr_el2(spsr_reg_t reg) {
  asm volatile("msr SPSR_EL2, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}
__attribute__((always_inline))
inline spsr_reg_t read_spsr_el1(void) {
  register spsr_reg_t res ;
  asm volatile("mrs %[addr], SPSR_EL1;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_spsr_el1(spsr_reg_t reg) {
  asm volatile("msr SPSR_EL1, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}


//==============================================================
// Structure and access to SCTLR_EL3 and SCTLR_EL2
typedef union {
  struct __attribute__((__packed__)) {
    unsigned M      :  1 ; // 1
    unsigned A      :  1 ; // 2
    unsigned C      :  1 ; // 3
    unsigned SA     :  1 ; // 4
    unsigned RES1_1 :  2 ; // 6
    unsigned nAA    :  1 ; // 7
    unsigned RES0_1 :  4 ; //11
    unsigned RES1_2 :  1 ; //12
    unsigned I      :  1 ; //13
    unsigned EnDB   :  1 ; //14
    unsigned RES0_2 :  2 ; //16
    unsigned RES1_3 :  1 ; //17
    unsigned RES0_3 :  1 ; //18
    unsigned RES1_4 :  1 ; //19
    unsigned WXN    :  1 ; //20
    unsigned RES0_4 :  1 ; //21
    unsigned IESB   :  1 ; //22
    unsigned RES1_5 :  2 ; //24
    unsigned RES0_5 :  1 ; //25
    unsigned EE     :  1 ; //26
    unsigned RES0_6 :  1 ; //27
    unsigned EnDA   :  1 ; //28
    unsigned RES1_6 :  2 ; //30
    unsigned EnlB   :  1 ; //31
    unsigned EnlA   :  1 ; //32
    unsigned RES0_7 : 32 ; //64
  };
  uint64_t Raw64;
} sctlr_reg_t;

__attribute__((always_inline))
inline sctlr_reg_t read_sctlr_el3(void) {
  register sctlr_reg_t res ;
  asm volatile("mrs %[addr], SCTLR_EL3;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_sctlr_el3(sctlr_reg_t reg) {
  asm volatile("msr SCTLR_EL3, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}
__attribute__((always_inline))
inline sctlr_reg_t read_sctlr_el2(void) {
  register sctlr_reg_t res ;
  asm volatile("mrs %[addr], SCTLR_EL2;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_sctlr_el2(sctlr_reg_t reg) {
  asm volatile("msr SCTLR_EL2, %[addr];"
	       "isb;"::[addr]"r"(reg):);
}

//==============================================================
// Structure and access to SCTLR_EL1
typedef union {
  struct __attribute__((__packed__)) {
    unsigned M      :  1 ; // 1
    unsigned A      :  1 ; // 2
    unsigned C      :  1 ; // 3
    unsigned SA     :  1 ; // 4
    unsigned SA0    :  1 ; // 5
    unsigned CP15BEN:  1 ; // 6
    unsigned nAA    :  1 ; // 7
    unsigned ITD    :  1 ; // 8
    unsigned SED    :  1 ; // 9
    unsigned UMA    :  1 ; //10    
    unsigned RES0_0 :  1 ; //11
    unsigned RES1_0 :  1 ; //12
    unsigned I      :  1 ; //13
    unsigned EnDB   :  1 ; //14
    unsigned DZE    :  1 ; //15
    unsigned UCT    :  1 ; //16
    unsigned nTWI   :  1 ; //17
    unsigned RES0_1 :  1 ; //18
    unsigned nTWE   :  1 ; //19
    unsigned WXN    :  1 ; //20
    unsigned RES1_1 :  1 ; //21
    unsigned IESB   :  1 ; //22
    unsigned RES1_2 :  1 ; //23
    unsigned SPAN   :  1 ; //24
    unsigned E0E    :  1 ; //25
    unsigned EE     :  1 ; //26
    unsigned UCI    :  1 ; //27
    unsigned EnDA   :  1 ; //28
    unsigned nTLSMD :  1 ; //29
    unsigned LSMAOE :  1 ; //30
    unsigned EnlB   :  1 ; //31
    unsigned EnlA   :  1 ; //32
    unsigned RES0_2 : 32 ; //64
  };
  uint64_t Raw64;
} sctlr_el1_reg_t;

__attribute__((always_inline))
inline sctlr_el1_reg_t read_sctlr_el1(void) {
  register sctlr_el1_reg_t res ;
  asm volatile("mrs %[addr], SCTLR_EL1;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_sctlr_el1(sctlr_el1_reg_t reg) {
  asm volatile("msr SCTLR_EL1, %[addr];"
	       "isb"::[addr]"r"(reg):);
}

//***********************************************************
// Type of register TCR_EL2
typedef union {
  struct __attribute__((__packed__)) {
    uint32_t T0SZ   : 6 ; // 6
    uint32_t RES0_0 : 2 ; // 8
    uint32_t IRGN0  : 2 ; //10
    uint32_t ORGN0  : 2 ; //12
    uint32_t SH0    : 2 ; //14
    uint32_t TG0    : 2 ; //16
    uint32_t PS     : 3 ; //19
    uint32_t RES0_1 : 1 ; //20
    uint32_t TBI    : 1 ; //21
    uint32_t HA     : 1 ; //22
    uint32_t HD     : 1 ; //23
    uint32_t RES1_0 : 1 ; //24
    uint32_t HPD    : 1 ; //25
    uint32_t HWU59  : 1 ; //26
    uint32_t HWU60  : 1 ; //27
    uint32_t HWU61  : 1 ; //28
    uint32_t HWU62  : 1 ; //29
    uint32_t TBID   : 1 ; //30
    uint32_t RES0_2 : 1 ; //31
    uint32_t RES1_1 : 1 ; //32
    uint32_t RES0_3 :32 ; //64
  };
  uint64_t Raw64; // Raw 64bit access
} tcr_el2_reg_t ;

//***********************************************************
// Type of register TCR_EL1
typedef union {
  struct __attribute__((__packed__)) {
    uint32_t T0SZ   : 6 ; // 6
    uint32_t RES0_0 : 1 ; // 7
    uint32_t EPD0   : 1 ; // 8
    uint32_t IRGN0  : 2 ; //10
    uint32_t ORGN0  : 2 ; //12
    uint32_t SH0    : 2 ; //14
    uint32_t TG0    : 2 ; //16
    uint32_t T1SZ   : 6 ; //22
    uint32_t A1     : 1 ; //23
    uint32_t EPD1   : 1 ; //24
    uint32_t IRGN1  : 2 ; //26
    uint32_t ORGN1  : 2 ; //28
    uint32_t SH1    : 2 ; //30
    uint32_t TG1    : 2 ; //32
    uint32_t IPS    : 3 ; //35
    uint32_t RES0_1 : 1 ; //36
    uint32_t AS     : 1 ; //37
    uint32_t TBI0   : 1 ; //38
    uint32_t TBI1   : 1 ; //39
    uint32_t HA     : 1 ; //40
    uint32_t HD     : 1 ; //41
    uint32_t HPD0   : 1 ; //42
    uint32_t HPD1   : 1 ; //43
    uint32_t HWU059 : 1 ; //44
    uint32_t HWU060 : 1 ; //45
    uint32_t HWU061 : 1 ; //46
    uint32_t HWU062 : 1 ; //47
    uint32_t HWU159 : 1 ; //48
    uint32_t HWU160 : 1 ; //49
    uint32_t HWU161 : 1 ; //50
    uint32_t HWU162 : 1 ; //51
    uint32_t TBID0  : 1 ; //52
    uint32_t TBID1  : 1 ; //53
    uint32_t NFD0   : 1 ; //54
    uint32_t NFD1   : 1 ; //55
    uint32_t RES0_2 : 9 ; //64
  };
  uint64_t Raw64; // Raw 64bit access
} tcr_el1_reg_t ;

//***********************************************************
// Memory attributes used in our setting.
// The 64-bit MAIR_ELx registers allow the definition of 8
// sets of appributes, indexed by the AttrIndx field of
// Stage 1 descriptors. Each set of attributes is encoded on
// 8 bits of MAIR_ELx.

// Device types, cf. section B2.7.2 of ARMv8 ARM
typedef enum {
  nGnRnE = 0b0000,// No gathering, no reordering, no early write ack
  nGnRE  = 0b0100,// No gathering, no reordering, early write ack
  nGRE   = 0b1000,// No gathering, reordering, early write ack
  GRE    = 0b1100 // Gathering, reordering, early write ack. Still,
                  // speculative access is forbidden.
} device_type ;

// Cache policies, cf. section D12.2.82
typedef enum {
  WT_TRANS       = 0, // Write-through transient
  NC_OR_WB_TRANS = 1, // Non-cacheable (when RW=0b00)
                      // or write-through transient
  WT_NON_TRANS   = 2, // Write-through non-transient
  WB_NON_TRANS   = 3  // Write-back non-transient  
} cache_policy ;

// The data type itself 
typedef union {
  // Determine if it's memory or a device
  struct __attribute__((packed)) {
    // 0 -> device memory, not 0 -> normal memory
    uint8_t      IsNormalMemory     : 4 ;
    uint8_t      IGNORE0            : 4 ;
  } ;
  // If memory, then decode using this
  struct __attribute__((packed)) {
    cache_policy OuterPolicy        : 2 ;
    uint8_t      OuterReadAllocate  : 1 ;
    uint8_t      OuterWriteAllocate : 1 ;
    cache_policy InnerPolicy        : 2 ;
    uint8_t      InnerReadAllocate  : 1 ;
    uint8_t      InnerWriteAllocate : 1 ;
  } ;
  // If device, then the first 4 bits are 0, the last
  // 4 are decoded using this
  struct __attribute__((packed)) {
    uint8_t      IGNORE1            : 4 ;
    device_type  DeviceType         : 4 ;
  } ;
  uint8_t Raw8 ;
} mair_attr ;

//***********************************************************
// Type of register MAIR_EL1 and MAIR_EL2
// The properties of the various memory types used
// in this program
typedef union {
  struct __attribute__((__packed__)) {
    mair_attr Attr[8] ;
  };
  uint64_t Raw64; // Raw 64bit access
} mair_reg_t ;



//***********************************************************
// Shareability bits for normal memory
typedef enum {
  RAM_SH_NONE    = 0b00, // Non-shareable
  RAM_SH_INVALID = 0b01, // Reserved, unpredictable
  RAM_SH_OUTER   = 0b10, // Outer shareable
  RAM_SH_INNER   = 0b11, // Inner shareable  
} ram_sh ;

//==============================================================
// Structure and access to CNTHCTL_EL2
typedef union {
  struct __attribute__((__packed__)) {
    unsigned EL1PCTEN : 1 ;
    unsigned EL1PCEN  : 1 ;
    unsigned EVNTEN   : 1 ;
    unsigned EVNTDIR  : 1 ;
    unsigned EVNTI    : 4 ;
    uint64_t RES0     :56 ;
  } ;
  uint64_t Raw64;
} cnthctl_el2_reg_t;

__attribute__((always_inline))
inline cnthctl_el2_reg_t read_cnthctl_el2(void) {
  register cnthctl_el2_reg_t res ;
  asm volatile("mrs %[addr], CNTHCTL_EL2;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_cnthctl_el2(cnthctl_el2_reg_t reg) {
  asm volatile("msr CNTHCTL_EL2, %[addr];"::[addr]"r"(reg):);
}

//==============================================================
// Structure and access to CNTKCTL_EL1
typedef union {
  struct __attribute__((__packed__)) {
    unsigned EL0PCTEN : 1 ;
    unsigned EL0VCTEN : 1 ;
    unsigned EVNTEN   : 1 ;
    unsigned EVNTDIR  : 1 ;
    unsigned EVNTI    : 4 ;
    unsigned EL0VTEN  : 1 ;
    unsigned EL0PTEN  : 1 ;
    uint64_t RES0     :54 ;
  } ;
  uint64_t Raw64;
} cntkctl_el1_reg_t;

__attribute__((always_inline))
inline cntkctl_el1_reg_t read_cntkctl_el1(void) {
  register cntkctl_el1_reg_t res ;
  asm volatile("mrs %[addr], CNTKCTL_EL1;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_cntkctl_el1(cntkctl_el1_reg_t reg) {
  asm volatile("msr CNTKCTL_EL1, %[addr];"::[addr]"r"(reg):);
}

//==============================================================
// Access to the counter-timer Virtual Offset register
// CNTVOFF_EL2
__attribute__((always_inline))
inline uint64_t read_cntvoff_el2(void) {
  register uint64_t res ;
  asm volatile("mrs %[addr], CNTVOFF_EL2;":[addr]"=r"(res)::);
  return res ;
}
__attribute__((always_inline))
inline void write_cntvoff_el2(uint64_t val) {
  asm volatile("msr CNTVOFF_EL2, %[addr];"::[addr]"r"(val):);
}


#endif
