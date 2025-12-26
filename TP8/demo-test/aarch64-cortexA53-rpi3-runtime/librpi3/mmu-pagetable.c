#include <stdint.h>
#include <librpi3/mmu-private.h>

//=============================================================
// Stage 1 (unique stage) traslation tables for EL2 and EL3
// Controlled by EL2 or EL3.
// L2 table for the first 1Go
__attribute__((aligned(4096))) descriptor el2_l2_0[512] ;
// L2 table for the second 1Go, which contains some peripherals
__attribute__((aligned(4096))) descriptor el2_l2_1[512] ;
// L1 table. This one goes to TTBR0_EL2 and possibly
// TTBR0_EL3.
__attribute__((aligned(4096))) descriptor el2_l1[4] ;


//=============================================================
//
void init_page_table_el2 (void) {
  uint64_t i ;
  
  //---------------------------------------------------------
  // EL2/EL3 translation table
  {// L1 page table, set up into TTBR0
    // Normally it's taken care of by BSS initialization
    for(i=0;i<4;i++) el2_l1[i].Raw64 = 0 ;
    // Descriptor 0 - first 1Go of memory space
    el2_l1[0].Valid = 1 ;
    el2_l1[0].Table = 1 ;
    el2_l1[0].Address = ((uint64_t)el2_l2_0) >> 12 ;
    el2_l1[0].NSTable = 1 ; // Next level is non-secure. Is this needed ?
    // Descriptor 1 - second 1Go of memory space
    el2_l1[1].Valid = 1 ;
    el2_l1[1].Table = 1 ;
    el2_l1[1].Address = ((uint64_t)el2_l2_1) >> 12 ;
    el2_l1[1].NSTable = 1 ; // Next level is non-secure. Is this needed ?
    // Descriptors 2 and 3 already have their Valid bit
    // set to 0, so they are invalid.
  }
  {// L2 pages
    // Normally it's taken care of by BSS initialization,
    // but since this is a very sensitive phase I prefer
    // to do it again.
    for(i=0;i<512;i++) {
      el2_l2_0[i].Raw64 = 0 ;
      el2_l2_1[i].Raw64 = 0 ;
    }
    // 8 Mo of memory in full RWX access (for privileged
    // code), cached 
    for(i=0;i<4;i++) {
      el2_l2_0[i].Valid    = 1 ;
      el2_l2_0[i].Table    = 0 ; // Page descriptor
      el2_l2_0[i].AttrIndx = MT_NORMAL ;
      el2_l2_0[i].AF = 1 ;
      el2_l2_0[i].SH = RAM_SH_INNER ;
      el2_l2_0[i].Address  = (i<<9) ;      
    }
    // 760 Mo of memory in RW access, cached. Execution is
    // not permitted to avoid execution of unprivileged
    // code.
    for(;i<384;i++) {
      el2_l2_0[i].Valid    = 1 ;
      el2_l2_0[i].Table    = 0 ; // Page descriptor
      el2_l2_0[i].AttrIndx = MT_NORMAL ;
      el2_l2_0[i].AF = 1 ;
      el2_l2_0[i].XN = 1 ;       // Execute never
      el2_l2_0[i].SH = RAM_SH_INNER ;
      el2_l2_0[i].Address  = (i<<9) ;      
    }
    // 240 Mo of VideoCore RAM, only kernel access, uncached,
    // never execute.
    for(;i<504;i++) {
      el2_l2_0[i].Valid = 1 ;
      el2_l2_0[i].Table = 0 ; // Page descriptor
      el2_l2_0[i].AttrIndx = MT_NORMAL_NC ;
      el2_l2_0[i].AF = 1 ;
      el2_l2_0[i].XN = 1 ;       // Execute never
      el2_l2_0[i].Address = (i<<9) ;
    }    
    // 16 Mo of system peripherals. Device, never execute,
    // uncached.
    for(;i<512;i++) {
      el2_l2_0[i].Valid = 1 ;
      el2_l2_0[i].Table = 0 ; // Page descriptor
      el2_l2_0[i].AttrIndx = MT_DEVICE_NGNRNE ;
      el2_l2_0[i].AF = 1 ;
      el2_l2_0[i].XN = 1 ;       // Execute never
      el2_l2_0[i].Address = (i<<9) ;
    }
    // 2 Mo of local peripherals. Device, never execute,
    // uncached.
    {
      el2_l2_1[0].Valid = 1 ;
      el2_l2_1[0].Table = 0 ; // Page descriptor
      el2_l2_1[0].AttrIndx = MT_DEVICE_NGNRNE ;
      el2_l2_1[0].AF = 1 ;
      el2_l2_1[0].XN = 1 ;       // Execute never
      el2_l2_1[0].Address = (i<<9) ;
    }
    // Remaining elements of el2_l2_1 are Invalid.
  }
}


//=============================================================
// Stage 1 translation tables for EL1/EL0
// L2 table for the first 1Go
__attribute__((aligned(4096))) descriptor el1_l2_0[512] ;
// L2 table for the second 1Go, which contains some peripherals
__attribute__((aligned(4096))) descriptor el1_l2_1[512] ;
// L1 table, used for both lower and upper addresses. This one
// goes to TTBR0_EL1 and TTBR1_EL1
__attribute__((aligned(4096))) descriptor el1_l1  [4] ;

void init_page_table_el1 (void) {
  uint64_t i ;  
  //---------------------------------------------------------
  // EL1/EL0 translation table
  {// L1 page table, set up into TTBR0 (and also in TTBR1)
    // Normally it's taken care of by BSS initialization
    for(i=0;i<4;i++) el1_l1[i].Raw64 = 0 ;
    // Descriptor 0 - first 1Go of memory space
    el1_l1[0].Valid = 1 ;
    el1_l1[0].Table = 1 ;
    el1_l1[0].Address = ((uint64_t)el1_l2_0) >> 12 ;
    el1_l1[0].NSTable = 1 ; // Next level is non-secure. Is this needed ?
    // Descriptor 1 - second 1Go of memory space
    el1_l1[1].Valid = 1 ;
    el1_l1[1].Table = 1 ;
    el1_l1[1].Address = ((uint64_t)el1_l2_1) >> 12 ;
    el1_l1[1].NSTable = 1 ; // Next level is non-secure. Is this needed ?
    // Descriptors 2 and 3 already have their Valid bit
    // set to 0, so they are invalid.
  }
  {// L2 pages
    // Normally it's taken care of by BSS initialization,
    // but since this is a very sensitive phase I prefer
    // to do it again.
    for(i=0;i<512;i++) {
      el1_l2_0[i].Raw64 = 0 ;
      el1_l2_1[i].Raw64 = 0 ;
    }
    // 8 Mo of cached memory in:
    // - full RWX access for EL1
    // - no access for EL0
    for(i=0;i<4;i++) {
      el1_l2_0[i].Valid    = 1 ;
      el1_l2_0[i].Table    = 0 ; // Page descriptor
      el1_l2_0[i].AttrIndx = MT_NORMAL ;
      el1_l2_0[i].AF = 1 ;      
      el1_l2_0[i].SH = RAM_SH_INNER ;
      el1_l2_0[i].Address  = (i<<9) ;
      // Permissions, cf. mmu-private.h
      el1_l2_0[i].XN  = 1 ;
      el1_l2_0[i].PXN = 0 ;
      el1_l2_0[i].AP  = 0b00 ;
    }
    // 760 Mo of memory in RW access, cached. Execution is
    // not permitted to avoid execution of unprivileged
    // code.
    for(;i<384;i++) {
      el1_l2_0[i].Valid    = 1 ;
      el1_l2_0[i].Table    = 0 ; // Page descriptor
      el1_l2_0[i].AttrIndx = MT_NORMAL ;
      el1_l2_0[i].AF = 1 ;
      el1_l2_0[i].XN = 1 ;       // Execute never
      el1_l2_0[i].SH = RAM_SH_INNER ;
      el1_l2_0[i].Address  = (i<<9) ;      
      // Permissions, cf. mmu-private.h
      el1_l2_0[i].XN  = 0 ;
      el1_l2_0[i].PXN = 0 ;
      el1_l2_0[i].AP  = 0b01 ;
    }
    // 240 Mo of VideoCore RAM, only privileged access, uncached,
    // never execute.
    for(;i<504;i++) {
      el1_l2_0[i].Valid = 1 ;
      el1_l2_0[i].Table = 0 ; // Page descriptor
      el1_l2_0[i].AttrIndx = MT_NORMAL_NC ;
      el1_l2_0[i].AF = 1 ;
      el1_l2_0[i].XN = 1 ;       // Execute never
      el1_l2_0[i].Address = (i<<9) ;
      // Permissions, cf. mmu-private.h
      el1_l2_0[i].XN  = 1 ;
      el1_l2_0[i].PXN = 1 ;
      el1_l2_0[i].AP  = 0b00 ;
    }    
    // 16 Mo of system peripherals. Device, never execute,
    // uncached. For now, grant access to both EL0 and EL1.
    for(;i<512;i++) {
      el1_l2_0[i].Valid = 1 ;
      el1_l2_0[i].Table = 0 ; // Page descriptor
      el1_l2_0[i].AttrIndx = MT_DEVICE_NGNRNE ;
      el1_l2_0[i].AF = 1 ;
      el1_l2_0[i].XN = 1 ;       // Execute never
      el1_l2_0[i].Address = (i<<9) ;
      // Permissions, cf. mmu-private.h
      el1_l2_0[i].XN  = 1 ;
      el1_l2_0[i].PXN = 0 ;
      el1_l2_0[i].AP  = 0b01 ;
    }
    // 2 Mo of local peripherals. Device, never execute,
    // uncached. For now, grant access to both EL0 and EL1.
    {
      el1_l2_1[0].Valid = 1 ;
      el1_l2_1[0].Table = 0 ; // Page descriptor
      el1_l2_1[0].AttrIndx = MT_DEVICE_NGNRNE ;
      el1_l2_1[0].AF = 1 ;
      el1_l2_1[0].XN = 1 ;       // Execute never
      el1_l2_1[0].Address = (i<<9) ;
      // Permissions, cf. mmu-private.h
      el1_l2_1[0].XN  = 1 ;
      el1_l2_1[0].PXN = 0 ;
      el1_l2_1[0].AP  = 0b01 ;
    }
    // Remaining elements of el2_l2_1 are Invalid.
  }
}


