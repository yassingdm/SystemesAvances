#include <stdint.h>
#include <librpi3/mmu.h>

//==========================================================================
// INITIALIZE THE MMU in EL2
// - Use pages set up using function init_page_table_el2
void mmu_init_el2(void) {
  asm volatile("dsb sy");

  /* Set the memattrs values into mair_el1*/
  mair_reg_t mair;
  mair.Raw64 = 0 ;
  // The following encodings are defined in mmu-pagetable.h
  mair.Attr[MT_NORMAL]        = MT_NORMAL_ATTR ;
  mair.Attr[MT_NORMAL_NC]     = MT_NORMAL_NC_ATTR ;
  mair.Attr[MT_DEVICE_NGNRNE] = MT_DEVICE_NGNRNE_ATTR ;
  asm volatile ("msr mair_el2, %0" : : "r" (mair.Raw64));

  // Bring page table online and execute memory barrier.
  asm volatile ("msr ttbr0_el2, %0" : : "r" ((uint64_t)el2_l1));
  asm volatile("isb");

  // Specify mapping characteristics in translate control register of EL2
  tcr_el2_reg_t tcr_el2 ;
  // Start by resetting all to 0
  tcr_el2.Raw64  =  0 ;
  // RES1 set to 1
  tcr_el2.RES1_0 =  1 ;
  tcr_el2.RES1_1 =  1 ;
  //
  tcr_el2.T0SZ   = 32 ;
  tcr_el2.IRGN0  =  1 ;
  tcr_el2.ORGN0  =  1 ;
  tcr_el2.SH0    =  RAM_SH_INNER ;
  tcr_el2.TBI    =  1 ;
  // And now set it
  asm volatile ("msr tcr_el2, %0; isb" : : "r" (tcr_el2));

  // Set up SCTLR_EL2
  sctlr_reg_t  sctlr_el2 = read_sctlr_el2() ;
  sctlr_el2.I = 1 ;
  sctlr_el2.C = 1 ;
  sctlr_el2.M = 1 ;    
  sctlr_el2.SA = 1 ;    
  sctlr_el2.A = 1 ;
  write_sctlr_el2(sctlr_el2) ;
}

//==========================================================================
// INITIALIZE THE MMU in EL1
// - Use pages set up using function init_page_table_el1
void mmu_init_el1(void) {
  asm volatile("dsb sy");

  /* Set the memattrs values into mair_el1*/
  mair_reg_t mair;
  mair.Raw64 = 0 ;
  // The following encodings are defined in mmu-pagetable.h
  mair.Attr[MT_NORMAL]        = MT_NORMAL_ATTR ;
  mair.Attr[MT_NORMAL_NC]     = MT_NORMAL_NC_ATTR ;
  mair.Attr[MT_DEVICE_NGNRNE] = MT_DEVICE_NGNRNE_ATTR ;
  asm volatile ("msr mair_el1, %0" : : "r" (mair));

  // Bring page tables online and execute memory barrier
  asm volatile ("msr ttbr0_el1, %0" : : "r" ((uint64_t)el1_l1));
  asm volatile ("msr ttbr1_el1, %0" : : "r" ((uint64_t)el1_l1));
  asm volatile("isb");

  // Specify mapping characteristics in translate control register of EL1
  tcr_el1_reg_t tcr_el1 ;
  // Start by resetting all to 0
  tcr_el1.Raw64  =  0 ;
  // Granule configuration:
  // - TG0=0, meaning 4kB granule for the VAs we use
  // - TG1=0b10 is invalid, but since it's never used we don't care.
  tcr_el1.TG1   = 0b10 ;
  tcr_el1.SH1   = RAM_SH_INNER ;
  tcr_el1.ORGN1 = 0b01 ;
  tcr_el1.IRGN1 = 0b01 ;
  tcr_el1.T1SZ  =   32 ;
  tcr_el1.SH0   = RAM_SH_INNER ;
  tcr_el1.ORGN0 = 0b01 ;
  tcr_el1.IRGN0 = 0b01 ;
  tcr_el1.T0SZ  =   32 ;
  asm volatile ("msr tcr_el1, %0; isb" : : "r" (tcr_el1));

  // Set up SCTLR_EL1
  sctlr_el1_reg_t  sctlr_el1 = read_sctlr_el1() ;
  // RES1
  sctlr_el1.RES1_0 = 1 ;
  sctlr_el1.RES1_1 = 1 ;
  sctlr_el1.RES1_2 = 1 ;
  // Config
  sctlr_el1.I = 1 ;
  sctlr_el1.C = 1 ;
  sctlr_el1.M = 1 ;    
  sctlr_el1.SA = 1 ;    
  sctlr_el1.SA0 = 1 ;    
  sctlr_el1.A = 1 ;
  // Set
  write_sctlr_el1(sctlr_el1) ;
}
