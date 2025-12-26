#ifndef MMU2_H
#define MMU2_H

#include <stdint.h>
#include <librpi3/assert.h>
#include <librpi3/arm.h>

//================================================================
// OVERALL VIEW ON MMU CONFIGURATION
//----------------------------------------------------------------
//
// We assume we set up through Raspberry Pi configuration
// file the separation of the 1Go of RAM into 768 Mo of RAM
// dedicated to ARM CPU use and 256 Mo for VideoCore (VC). The
// latter is divided into 240 Mo of RAM for exclusive VC use
// and 16 Mo of memory-mapped system peripherals.
//
// Thus, the memory map is:
//   [0x0000_0000 - 0x3000_0000) - RAM for ARM CPU use
//   [0x3000_0000 - 0x3f00_0000) - RAM for VideoCore
//   [0x3f00_0000 - 0x4000_0000) - System peripherals
//   [0x4000_0000 - 0x4020_0000) - ARM peripherals
//
// Encoding the addresses is done under the following
// hypotheses:
// - Granule = 4kB, meaning that each page has 512 entries
// - Output address sizes at each level are 32-bit. This is
//   set using TCR_ELx.{I}PS, cf. "Output address size", page
//   2400 of ARMv8 ARM.
// - Input address size is also 32-bit. This is achieved using
//   the TSZ0 and TSZ1 fields set to 32, cf. "Input address size",
//   p. 2401.
// - Since we have to set up some "upper" addresses, we will
//   simply place here a copy of the lower table. This means
//   upper addresses are identical to lower ones, modulo the
//   initial 1s.
//
// Under these hypotheses, addresses are decoded as follows:
// - bits [63:32] are never used
// - bits [31:30] are used as L1 index
// - bits [29:21] are used as L2 index or transmitted from VA
//   if L1 is a page descriptor
// - bits [20:12] are used as L3 index of transmitted from VA
//   if L1 or L2 are page descriptors
// - bits [11:0] are transmitted from VA
//
// The particular encoding we use:
// - L1 encoding:
//   * Tables have size 4 (but are still aligned on 4ko, if they
//     contain non-invalid entries).
//   * For the EL3, EL2, and lower EL1/EL0 addresses,
//     L1 descriptors of indices 0 and 1 are table indices and
//     those of index 2 and 3 are invalid.
//   * For the upper EL1/EL0 addresses, all L1 indices
//     are invalid.
// - L2 encoding:
//   * Tables have size 512 (aligned on 4ko)
//   * The table pointed from the L1 descriptor of index 0 has
//     the following descriptors:
//     >   2 for privileged code    =   4 Mo
//     > 382 for EL0 code           = 764 Mo
//     > 120 for VideoCore RAM      = 240 Mo
//     >   8 for system peripherals =  16 Mo
//   * The table pointed from the L1 descriptor of index 1 has
//     the following descriptors:
//     >   1 for ARM peripherals    =   2 Mo
//     > 511 invalid
//
// We do not enable Stage 2 decoding, even in EL0/EL1. To do
// this, we set HCR_EL2.VM=0, so that the output of Stage 1
// is the PA.
//================================================================


//================================================================
// Access permissions.
//----------------------------------------------------------------
// For **table** descriptors, they set up limits on 
// subsequent levels of lookup, cf. Table D5-28 of ARMv8 ARM.
//
// The field APTable has 2 bits and is set according to type
// aptable below.
//
// In all ELs they are set to 0 (APTABLE_NOEFFECT), so that
// the permissions are set through page descriptors (the
// AP field).
//
//----------------------------------------------------------------
// For **page** descriptors in EL1/EL0 translation tables.
// According to ARMv8 ARM, page 2462:
//   "For a translation regime that applies to EL0 and a
//    higher Exception level, if the value of the AP[2:1]
//    bits is 0b01, permitting write access from EL0, then
//    the PXN field is treated as if it has the value 1,
//    regardless of its actual value."
// Then, simply referring to the permission bits tables
// separately is not sufficient, and one must use 
// Table D5-30.
//
// From this table, I isolate a few configurations I will
// use:
// - Memory that can be RWX at EL0 but only RW at EL1:
//   UXN = 0, PXN = 0, AP = 0b01
// - Memory that can be RWX at EL1 but inaccessible at EL0:
//   UXN = 1, PXN = 0, AP = 0b00
// - Memory that can be RW, but not X at both EL1 and EL0:
//   UXN = 1, PXN = 0, AP = 0b01
// - Memory that can be RW at EL1 but no access at EL0:
//   UXN = 1, PXN = 1, AP = 0b00
// In all cases, I set SCTLR_ELx.WXN=0 (no need to change
// it dynamically).
//
//----------------------------------------------------------------
// For **page** descriptors in EL1/EL0 translation tables.
//
// Access permissions for the EL2 and EL3 translation tables.
// Taken from table D5-31:
// - Memory that can be RWX:
//   XN = 0, AP=0b00
// - Memory that can be RW:
//   XN = 1, AP=0b00
// At EL2 and EL3 PXN is always 0. In all cases, I set
// SCTLR_ELx.WXN=0.
//================================================================



//================================================================
// APTable field possible values. Only APTABLE_NOEFFECT is used in
// the current configuration.
//================================================================
typedef enum {
  // No effect
  APTABLE_NOEFFECT          = 0b00,
  // Access at EL0 not permitted, regardless of
  // permissions in subsequent levels of lookup
  APTABLE_NO_EL0            = 0b01,
  // Write access not permitted, at any Exception
  // level, regardless of permissions in subsequent
  // levels of lookup
  APTABLE_NO_WRITE          = 0b10,
  // Write access not permitted,at any Exception
  // level, Read access not permitted at EL0.
  APTABLE_NO_WRITE_EL0_READ = 0b11 
} aptable ;


//================================================================
// Application-specific types of memory I'm going to use
// on the Raspberry Pi. I use macrodefinitions (as opposed to
// constants) in order to be able to include them in the
// header file.
//================================================================

// Normal memory, used only by the ARM cores
#define MT_NORMAL_ATTR \
  (mair_attr){				  \
    .OuterPolicy        = WB_NON_TRANS,   \
    .OuterReadAllocate  = 1,              \
    .OuterWriteAllocate = 1,              \
    .InnerPolicy        = WB_NON_TRANS,   \
    .InnerReadAllocate  = 1,              \
    .InnerWriteAllocate = 1               \
  }
// VideoCore memory - For now I set it as RW uncached, but
// I could actually simply set it as inaccessible
#define MT_NORMAL_NC_ATTR \
  (mair_attr){				  \
    .OuterPolicy        = NC_OR_WB_TRANS, \
    .OuterReadAllocate  = 0,              \
    .OuterWriteAllocate = 0,              \
    .InnerPolicy        = NC_OR_WB_TRANS, \
    .InnerReadAllocate  = 0,              \
    .InnerWriteAllocate = 0               \
  }
// Device memory, used for both system devices and local
// devices
#define MT_DEVICE_NGNRNE_ATTR \
  (mair_attr){				  \
    .IsNormalMemory     = 0,              \
    .DeviceType         = nGnRnE          \
  }

// These 3 sets of memory attributes will be assigned
// indices from 0 to 2 in MAIR registers, as follows:
typedef enum {
  MT_DEVICE_NGNRNE = 0,
  MT_NORMAL_NC     = 1,
  MT_NORMAL        = 2,
  // Attributes 3-7 are not used
} attr_ndx ;


//================================================================
// Data structure for manipulating page table descriptors,
// under the 4ko granule hypothesis.
//
// Note the way page and table descriptors are multiplexed
// on the same union.
//
// The names of the fields are those of the ARMv8 ARM, with
// the exception of "Valid", "Table", and the names of
// the address components.
//
// All RES0 fields are safely set to 0 if Raw64 is set to 0
// in the beginning of the manipulation. IGNORE fields must
// never be used.
//
// References:
// - lower and upper block attributes, see section D5.3.3.
// - Stage 2 table descriptor for 4k granule, cf. Fig. D5-15
// - 
//================================================================
typedef union {
  // General attributes of a descriptor
  struct __attribute__((packed)) {
    // 1 -> valid descriptor, 0 -> invalid
    uint64_t Valid      :  1 ;
    // 1 -> table descriptor, 0 -> page descriptor
    uint64_t Table      :  1 ; 
    uint64_t IGNORE0    : 10 ;
    // Address of either:
    // - next-level table (aligned on 4ko)
    // - page (aligned on 4ko for L3, on 2Mo for L2, on 1Go for L1)
    uint64_t Address    : 20 ; 
    uint64_t IGNORE1    : 32 ;
  } ;
  // Split the address into levels, for decoding
  struct __attribute__((packed)) {
    uint64_t VAAddress  : 12 ; // Always from VA
    uint64_t L3Address  :  9 ; // Decode in L3, otherwise from VA
    uint64_t L2Address  :  9 ; // Decode in L2, otherwise from VA
    uint64_t L1Address  :  2 ; // Decode in L1
    uint64_t IGNORE6    : 32 ;
  } ;
  // Stage 1 table descriptor attributes
  struct __attribute__((packed)) {
    uint64_t IGNORE2    : 59 ;
    uint64_t PXNTable   :  1 ;
    uint64_t XNTable    :  1 ;
    aptable  APTable    :  2 ;
    uint64_t NSTable    :  1 ;
  } ;
  // Stage 1, L1/L2 page descriptor attributes (not L3)
  struct __attribute__((packed)) {
    uint64_t IGNORE3    :  2 ;
    attr_ndx AttrIndx   :  3 ;
    uint64_t NS         :  1 ;
    uint64_t AP         :  2 ;
    ram_sh   SH         :  2 ;
    uint64_t AF         :  1 ; // Access flag. Always 1 for us.
    uint64_t nG         :  1 ;
    uint64_t IGNORE4    : 39 ;
    uint64_t DBM        :  1 ;
    uint64_t Contiguous :  1 ;
    // Privileged execute-never. Only valid when stage 1
    // supports two VA ranges (i.e. for EL0/EL1).
    uint64_t PXN        :  1 ;
    // Execute-never/Unprivileged execute-never. It's
    // XN at EL2/EL3 and UXN at EL0/EL1.
    uint64_t XN         :  1 ;
    uint64_t IGNORE5    :  9 ;
  } ;
  uint64_t Raw64 ;
} descriptor ;


//----------------------------------------------------------------
// Static consistency tests on descriptor size
static_assert(sizeof(descriptor) == 0x08,
	      "descriptor type should be 8 bytes in size");

#endif
