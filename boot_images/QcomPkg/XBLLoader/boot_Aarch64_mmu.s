//============================================================================
//
//             Arm V8 Aarch64 architecture assembly language MMU support
//
// GENERAL DESCRIPTION
//   This file contains the Arm V8 Aarch64 hierarchichal cache and mmu control    
//                                functions.
//
// Copyright 2013-2015,2017 by Qualcomm Technologies, Incorporated.  
//                           All Rights Reserved 
//============================================================================

//============================================================================
//
//                       EDIT HISTORY FOR MODULE
//
// This section contains comments describing changes made to the module.
// Notice that changes are listed in reverse chronological order.
//
//
// when      who     what, where, why
// --------  -----   ---------------------------------------------------------
// 06/29/17  yps     Added mmu_inval_trans_tbl_entry
// 06/15/17  yps     Added flush cache regions for internal memory before mmu disable/enable
// 05/02/17  yps     Added sp backup and restor apis
// 01/30/17  digant  only disable/enable mmu api + ttbr1 disable
// 11/17/15  kedara  cleanup code
// 11/09/15  plc     Update functions to run in EL1 or EL3
// 10/22/15  plc     Update SCTLR read/write and TLB invalidate functions 
//                   to make them Exception-level aware, and add push/pop
//                   macros to be used throughout all maintenance routines
// 12/17/14  ck      Increased MMU physical address size to 4TB
// 10/29/14  kedara  Added mmu_reconfigure to support 64 bit bootup.
// 08/21/14  kedara  Update mmu_get_page_table_base to use TTBR0_EL3
// 07/24/14  kedara  Update mmu_flush_cache to push/pop higher registers.
// 05/31/14  plc     Change L1 Pagetable to device shared (strongly ordered)
// 05/21/14  plc     Clean up linker warnings
// 05/05/14  plc     Remove bitshift on initial table MAIR attributes
// 04/10/14  plc     Implement armv8 cache maintenance routines
// 04/03/14  plc     Fix TLBI calls, and setup MMU for EL1 as well as EL3
// 03/25/14  plc     Enable MMU, A64 conversion for most maintenance asm
// 01/16/14  plc     Added initial routines for creating page tables
// 11/01/13  plc     Initial revision for 64-bit MSM8994
//============================================================================


//============================================================================
//                             MODULE DEFINES
//============================================================================


//============================================================================
//                             MODULE INCLUDES
//============================================================================
#include "boot_page_table_armv8.h"

//============================================================================
//                             MODULE IMPORTS
//============================================================================
#ifdef BOOT_USES_L2_DCACHE     
   .extern  l2_cache_flush_and_disable
   .extern  l2_cache_flush
   .extern  l2_ranged_cache_invalidate
   .extern  l2_ranged_cache_flush
   .extern  l2_ranged_cache_clean
#endif   
//============================================================================
//                             MODULE EXPORTS
//============================================================================
   .global  cache_mmu_disable
   .global  cache_mmu_init_page_table
   .global  mmu_reconfigure
   .global  mmu_invalidate_data_cache
   .global  mmu_invalidate_instruction_cache
   .global  mmu_enable_instruction_cache
   .global  mmu_flush_cache
   .global  dcache_clean_all
   .global  dcache_clean_region
   .global  dcache_inval_region
   .global  dcache_flush_region
   .global  cache_set_memory_barrier_location
   .global  memory_barrier 
   .global  mmu_invalidate_tlb
   .global  boot_disable_mmu
   .global  boot_enable_mmu
   .global  boot_enable_dcache   
   .global  mmu_relocate_page_table_base
   .global  mmu_flush_cache_and_disable
   .global  mmu_set_dacr
   .global  mmu_get_page_table_base
   .global  mmu_set_page_table_base
   .global  boot_backup_sp
   .global  boot_restore_sp
   .global  boot_flush_regions_disable_dcache  
   .global  mmu_inval_trans_tbl_entry
   .global  boot_is_icache_enabled
   .global  boot_is_dcache_enabled
//============================================================================
//                             MODULE EQUATES
//============================================================================
.set SCTLR_EL3_REG_DEFAULT, 0x0

.set MAIR_EL3_ATTR, MMU_PAGETABLE_MAIR_7 | MMU_PAGETABLE_MAIR_6 |  MMU_PAGETABLE_MAIR_5 | MMU_PAGETABLE_MAIR_4 | MMU_PAGETABLE_MAIR_3 | MMU_PAGETABLE_MAIR_2 | MMU_PAGETABLE_MAIR_1 | MMU_PAGETABLE_MAIR_0

.set TT_S1_TABLE,  0x00000000000000003    // NSTable=0, PXNTable=0, UXNTable=0, APTable=0

// TT block entries templates   (L1 and L2, NOT L3)
// Assuming table contents:
// 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
// 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
// 2 = b00000000 = Device-nGnRnE
.set TT_S1_FAULT,           0x0
.set TT_S1_NORMAL_NO_CACHE, 0x00000000000000401    // Index = 0, AF=1
.set TT_S1_NORMAL_WBWA,     0x00000000000000405    // Index = 1, AF=1
.set TT_S1_DEVICE_nGnRnE,   0x00000000000000409    // Index = 2, AF=1

.set TT_S1_UXN,  (1 << 54)
.set TT_S1_PXN,  (1 << 53)
.set TT_S1_nG ,  (1 << 11)
.set TT_S1_NS,   (1 << 5)

.set TT_S1_NON_SHARED,   (0 << 8)               // Non-shareable
.set TT_S1_INNER_SHARED, (3 << 8)               // Inner-shareable
.set TT_S1_OUTER_SHARED, (2 << 8)               // Outer-shareable
      
//============================================================================
//
//                         MEMORY BARRIER WORD POINTER
//
// The memory barrier word must exist in non-cached region of memory.
//============================================================================

  .section CACHE_MMU_ZI, "aw", %nobits

g_memory_barrier_ptr_addr = .

// This should be initialized to the actual SMEM memory barrier, otherwise NULL
.type g_memory_barrier_ptr, @object
g_memory_barrier_ptr:
   .space 4
      
//============================================================================
//
//                        MACROS AND FUNCTIONS
//
//============================================================================
  .section ARM_MMU, "ax", %progbits
  .p2align 8
   
   //CODE

//============================================================================
// MACRO mmu_disable_i_and_d_cache
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Disables the Data Cache
//   Does a read modify write, so no other bits in the control register are
//   affected
//============================================================================
   .macro   mmu_disable_i_and_d_cache
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions
   DSB      SY
   ISB      SY
   MOV      x2, #1
   BL       sctlr_read
   BIC      x1, x1, x2, lsl #2  // C bit - Disable data cache (SCTLR_EL*.C)
   BIC      x1, x1, x2, lsl #12 // I bit - Disable intruction cache (SCTLR_EL*.I)
   BL       sctlr_write
   DSB      SY
   ISB      SY
   .endm
 
//============================================================================
// FUNCTION mmu_set_page_table_base
//
// ARGS
//   base is as follows:
// 
//  bit  Name    Function
// ---------------------------------------------------
//  0 - C bit - Page table walk is Inner Cacheable? 
//  1 - S bit - Page table walk is Shareable?
//  2 - IMP bit - Implementation defined (Parity enabled for tablewalk)
// 3-4 - RGN bits- Outer cachable attributes for page table walking:
//                 00=Normal memory, Outer Noncacheable
//                 01=Normal memory, Outer Write-Back, write allocate
//                 10=Normal memory, Outer Write-Through, no allocate on write
//                 11=Normal memory, Outer Write-Back, no allocate on write
//  5 - Not Outer Shareable bit. (ARMv7)
//
//  6 - Combines with bit 0 to be IRGN
//
// IRGN [6,0] when multiprocessing extensions are implemented
//      0b00 Normal memory, Inner Non-cacheable
//      0b01 Normal memory, Inner Write-Back Write-Allocate Cacheable
//      0b10 Normal memory, Inner Write-Through Cacheable
//      0b11 Normal memory, Inner Write-Back no Write-Allocate Cacheable
//  
//14-31 - Pointer to level one translation table
//
//
// DESCRIPTION
//  Sets the base address of the mmu page table.
//============================================================================
   .type mmu_set_page_table_base, @function
mmu_set_page_table_base:
   MSR     TTBR0_EL1, x0
   ret


//============================================================================
// MACRO mmu_enable
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Enables the mmu
//   Does a read modify write, so no other bits in the control register are
//   affected
//============================================================================
   .macro   mmu_enable
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   DSB      SY
   ISB      SY
   MOV      x2, #1
   BL       sctlr_read
   ORR      x1, x1, x2, lsl #0  // Set M bit 
   ORR      x1, x1, x2, lsl #2  // Enable data cache (SCTLR_EL*.C)
   ORR      x1, x1, x2, lsl #12 // Enable intruction cache (SCTLR_EL3*.I)
   BL       sctlr_write
   DSB      SY
   ISB      SY
   .endm

//============================================================================
// MACRO mmu_disable
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Disables the mmu
//   Does a read modify write, so no other bits in the control register are
//   affected
//============================================================================
   .macro   mmu_disable
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   DSB      SY
   ISB      SY
   BL       sctlr_read
   MOV      x2, #1
   BIC      x1, x1, x2, lsl #0  // Clear M bit
   BIC      x1, x1, x2, lsl #2  // Clear data cache (SCTLR_EL*.C)
   BIC      x1, x1, x2, lsl #12 // Clear intruction cache (SCTLR_EL*.I)
   BL       sctlr_write
   DSB      SY
   ISB      SY
   .endm
   
//============================================================================
// MACRO mmu_align_fault_enable
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Enables address alignment fault checking
//   Does a read modify write, so no other bits in the control register are
//   affected
//============================================================================
   .macro   mmu_align_fault_enable
   MOV      x2, #1
   BL       sctlr_read
   ORR      x1, x1, x2, lsl #1 // Set A bit - Enable alignment fault check
   BL       sctlr_write
   .endm

//============================================================================
// MACRO mmu_set_dom_access
//
// ARGS
//   permissions
//     Contains 16 2 bit Domain fields
//     
//      00 = No Access - Any access to this domain causes a domain fault
//      01 = Client - Accesses checked against access permissions in TLB entry
//      10 = Reserved - Any access to this domain causes a domain fault
//      11 = Master - No checking, no permission fault can be generated
//
//  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//  |D15|D14|D13|D12|D11|D10|D09|D08|D07|D06|D05|D04|D03|D02|D01|D00|
//  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//   struct 
//   {
//      D15_Domain int:2
//      D14_Domain int:2
//      D13_Domain int:2
//      D12_Domain int:2
//      D11_Domain int:2
//      D10_Domain int:2
//      D9_Domain int:2
//      D8_Domain int:2
//      D7_Domain int:2
//      D6_Domain int:2
//      D5_Domain int:2
//      D4_Domain int:2
//      D3_Domain int:2
//      D2_Domain int:2
//      D1_Domain int:2
//      D0_Domain int:2
//   }
//
// DESCRIPTION
//   Sets the Access permisions for the 16 domains.
//
//============================================================================
   .macro   mmu_set_dom_access permisions
   MOV     w0,   \permissions
   MSR     DACR32_EL2, w0
   .endm

//============================================================================
// MACRO push_registers
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Pushes all of the cpu registers to the stack
//============================================================================
   .macro   push_registers
   STP     x0, x1,   [sp, #-16]!
   STP     x2, x3,   [sp, #-16]!
   STP     x4, x5,   [sp, #-16]!
   STP     x6, x7,   [sp, #-16]!
   STP     x8, x9,   [sp, #-16]!
   STP     x10, x11, [sp, #-16]!
   STP     x12, x13, [sp, #-16]!
   STP     x14, x15, [sp, #-16]!
   STP     x16, x17, [sp, #-16]!
   STP     x18, x19, [sp, #-16]!
   STP     x20, x21, [sp, #-16]!
   STP     x22, x23, [sp, #-16]!
   STP     x24, x25, [sp, #-16]!
   STP     x26, x27, [sp, #-16]!
   STP     x28, x29, [sp, #-16]!
   STP     x30, x0,  [sp, #-16]! // SP must be 16 byte aligned, so store x0 again  
   .endm
   
//============================================================================
// MACRO pop_registers
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Pushes all of the cpu registers to the stack
//============================================================================
   .macro   pop_registers
   LDP     x30, x0,  [sp], #16   // x0 completes the pair, so sp is 16 byte aligned
   LDP     x28, x29, [sp], #16
   LDP     x26, x27, [sp], #16
   LDP     x24, x25, [sp], #16  
   LDP     x22, x23, [sp], #16   
   LDP     x20, x21, [sp], #16  
   LDP     x18, x19, [sp], #16  
   LDP     x16, x17, [sp], #16
   LDP     x14, x15, [sp], #16   
   LDP     x12, x13, [sp], #16 
   LDP     x10, x11, [sp], #16  
   LDP     x8, x9,   [sp], #16   
   LDP     x6, x7,   [sp], #16  
   LDP     x4, x5,   [sp], #16  
   LDP     x2, x3,   [sp], #16
   LDP     x0, x1,   [sp], #16
   .endm
//============================================================================
// MACRO dcache_disable
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Disables the dcache
//   Does a read modify write, so no other bits in the control register are
//   affected
//============================================================================
.macro dcache_disable
   DSB      SY
   ISB      SY
   BL       sctlr_read
   MOV      x2, #1
   BIC      x1, x1, x2, lsl #2  // Clear C bit
   BL       sctlr_write
   DSB      SY
   ISB      SY
   .endm
//============================================================================

//============================================================================
// MACRO dcache_enable
//
// DESCRIPTION -
//   Enable the dcache
//   Does a read modify write, so no other bits in the control register are
//   affected
//============================================================================  
.macro dcache_enable
   DSB      SY
   ISB      SY
   MOV      x2, #1
   BL       sctlr_read
   ORR      x1, x1, x2, lsl #2  // Set C bit 
   BL       sctlr_write
   DSB      SY
   ISB      SY
   .endm
//============================================================================
//============================================================================
// sctlr_read
//
// ARGS
//   NONE
//
// RETURN
//   X1: Contains value in SCTLR register for current exception level
//
// DESCRIPTION
//   Reads the SCTLR in context for current exception level, either EL1 or EL3 
//============================================================================
.type sctlr_read, @function
sctlr_read:
   // Check the current Exception Level
   //----------------------------------
   MRS    x1, CurrentEL
   LSR    x1, x1, #2   
   CMP    x1, #3
   BGE    sctlr_read_el3
   MRS    x1, SCTLR_EL1
   B      sctlr_read_end
sctlr_read_el3:
   MRS    x1, SCTLR_EL3
sctlr_read_end:   
   ret
   
//============================================================================
// sctlr_write
//
// ARGS
//   X1: Contains value to be written to SCTLR for current exception level
//
// DESCRIPTION
//   Writes X1 to the SCTLR of current exception level, either EL1 or EL3 
//============================================================================
.type sctlr_write, @function
sctlr_write:
   // Check the current Exception Level
   //----------------------------------
   MRS    x27, CurrentEL
   LSR    x27, x27, #2   
   CMP    x27, #3
   BGE    sctlr_write_el3
   MSR    SCTLR_EL1, x1
   B      sctlr_write_end
sctlr_write_el3:
   MSR    SCTLR_EL3, x1
sctlr_write_end:   
   ret

//============================================================================
// mmu_invalidate_tlb
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Invalidates the entire Translation Look-aside Buffer (TLB) for EL1 or EL3 
//   unified operation (Data and Instruction). Invalidates all unlocked 
//   entries in the TLB. Causes the prefetch buffer to be flushed. All 
//   following instructions are fetched after the TLB invalidation.
//   We should do this before we enable the MMU.
//============================================================================
.type mmu_invalidate_tlb, @function
mmu_invalidate_tlb:
   DSB      SY
   ISB      SY
   // Check the current Exception Level
   //----------------------------------
   MRS      x15, CurrentEL
   LSR      x15, x15, #2
   CMP      x15, #3
   BGE      tlbi_el3
   // Invalidate the EL1 TLBs
   // ------------------------
   TLBI     VMALLE1     // All stage 1 translations used at EL1 
   B        tlbi_end
tlbi_el3:
   // Invalidate the EL3 TLBs
   // ------------------------
   TLBI     ALLE3       // All stage 1 translations used at EL3 
tlbi_end:   
   DSB      SY
   ISB      SY
   ret   
   
//============================================================================
// mmu_invalidate_tlb_el1_el3
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Invalidates the entire Translation Look-aside Buffer(TLB) for EL1 and EL3 
//   unified operation (Data and Instruction). Invalidates all unlocked 
//   entries in the TLB. Causes the prefetch buffer to be flushed. All 
//   following instructions are fetched after the TLB invalidation.
//   We should do this before we enable the MMU.
//============================================================================
.type mmu_invalidate_tlb_el1_el3, @function
mmu_invalidate_tlb_el1_el3:
   DSB      SY
   ISB      SY
   // Invalidate the EL1 TLBs
   // ------------------------
   TLBI     VMALLE1     // All stage 1 translations used at EL1 
   DSB      SY
   ISB      SY
   // Invalidate the EL3 TLBs
   // ------------------------
   TLBI     ALLE3       // All stage 1 translations used at EL3  
   DSB      SY
   ISB      SY
   ret  
   
//============================================================================
// mmu_invalidate_instruction_cache
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Invalidate the entire intruction cache.  This command will invalidate
//   all instruction caches to Point of Unification.
//   We need to do this before we enable the mmu
//============================================================================
.type mmu_invalidate_instruction_cache, @function
mmu_invalidate_instruction_cache:
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   DSB      SY
   ISB      SY
   IC IALLU
   DSB      SY
   ISB      SY
   ret

//============================================================================
// mmu_invalidate_data_cache
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Invalidate the entire data cache and the unified cache.  This command will 
//   invalidate any data cached.  It also flushes the branch target cache.
//   We need to do this before we enable the mmu
//   Note that registers such as CLIDR_EL1, CSSELR_EL1, etc. are 32-bits, 
//   though armv8 requires MRS/MSR instruction to pass 64-bit Xt register.
//   So we will read and write 64-bits, and operate on 32-bits.
//============================================================================
.type mmu_invalidate_data_cache, @function
mmu_invalidate_data_cache:
  push_registers
   
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions   	
  DSB     SY                   // ensure ordering with previous memory accesses
  MRS     x0, CLIDR_EL1        // Read CLIDR_EL1 into Xt (but uses 32 bits)
  ANDS    w3, w0, #0x7000000   // extract LoC from clidr  
  ORR     w3, wzr, w3, lsr #23 // left align LoC bit field in bits 3:0, and 
                               // increment loop +=2 as cssr reg level bits 3:1
  BEQ     finished             // if LoC is 0, then no need to clean
  MOV     w10, #0              // start invalidate at cache level 0
.loop1:
  ADD     w2, w10, w10, lsr #1  // calculate 3x cur_level, as type field=3bits
  LSR     w1, w0, w2            // extract cache type bits from clidr_el1    
  AND     w1, w1, #7            // mask of the bits for current cache only
  CMP     w1, #2                // see if we have Dcache at this level
  BLT     skip                  // skip if no cache, or just i-cache
  MSR     CSSELR_EL1, x10       // select current cache level in CSSELR_EL1
                                // x10 is passed but CSSELR_EL1 is 32bits 
  ISB     SY                    // isb to sych the new CCSIDR_EL1
  MRS     x1, CCSIDR_EL1        // read the new csidr 
  AND     w2, w1, #7            // extract the length of the cache lines
  ADD     w2, w2, #4            // add 4 (line length offset)
  MOV     w4, #0x3ff            // mask off 'associativity' bits of CCSIDR_EL1
  ANDS    w4, w4, w1, lsr #3    // find maximum number on the way size
  CLZ     w5, w4                // find bit position of way size increment
  MOV     w7, #0x7fff           // mask off 'NumSets' bits of CCSIDR_EL1
  ANDS    w7, w7, w1, lsr #13   // extract max number of the index size
loop2:
  MOV     w9, w4                // create working copy of max way size
loop3:
  LSL     w28, w9, w5           // factor way and cache number into w11
  ORR     w11, w10, w28
  LSL     w29, w7, w2           // factor index number into w11
  ORR     w11, w11, w29 
  DC      ISW, x11              // invalidate by set/way, pass x11 as DC ISW 
                                // is a 64-bit operation, though b32:63 are RES
  SUBS    w9, w9, #1            // decrement the way
  BGE     loop3
  SUBS    w7, w7, #1            // decrement the index
  BGE     loop2
skip:
  ADD     w10, w10, #2          // increment cache number
  CMP     w3, w10
  BGT     .loop1
finished:
  MOV     w10, #0               // swith back to cache level 0
  MSR     CSSELR_EL1, x10       // select current cache level in CSSELR_EL1
  DSB     SY
  ISB     SY
  pop_registers
  ret

//============================================================================
// mmu_enable_instruction_cache
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Enables the level one and level two intruction cache by setting the I1 and I2 bit 
//   in the ACTLR register. The mmu remains disabled
//============================================================================
.type mmu_enable_instruction_cache, @function
mmu_enable_instruction_cache:
  push_registers
  DSB     SY
  ISB     SY
  MOV     x2, #1
  BL      sctlr_read
  ORR     x1, x1, x2, lsl #12   // Enable intruction cache (SCTLR_EL3I)
  BL      sctlr_write
  DSB     SY
  ISB     SY
  pop_registers
  ret

//============================================================================
// cache_mmu_disable
//
// ARGS
//   
//
//  DESCRIPTION -
//        Disables the MMU and cache.
//           1- flushes out cache and write buffer
//              also disable data and instruction cache
//           2- disables MMU
//           3- invalidates TLB
//
//        The page table is unaffected
//============================================================================
.type cache_mmu_disable, @function
cache_mmu_disable:
  push_registers
  BL      mmu_flush_cache_and_disable
  MOV     x0, #0    
  mmu_disable
  bl      mmu_invalidate_tlb
  pop_registers
  ret


   
//============================================================================
// memory_barrier
//
// PROTOTYPE
//   void memory_barrier(void)//
//
// ARGS
//
// DESCRIPTION
//   Creates a memory barrier
//
// DEPENDENCIES
//============================================================================
.type memory_barrier, @function
memory_barrier:
   DSB     SY
   ret

//=======================================================================
// cache_set_memory_barrier_location
//
// DESCRIPTION
//   Sets the location of the actual memory barrier in SMEM
//
// DEPENDENCIES
//   cache_set_memory_barrier_location() must be called with a pointer to
//   non-buffered or non-cached memory before any call to memory_barrier(
//   for the instruction and data streams to be syncronized by
//   the memory_barrier() operation.
//=======================================================================
.type cache_set_memory_barrier_location, @function
cache_set_memory_barrier_location:
  ldr    x1, .LC
  ldr     x1, [x1]
  str     x0,  [x1]
  ret

.LC:
   .xword g_memory_barrier_ptr_addr
//============================================================================
// dcache_clean_all
//
// DESCRIPTION -
//   Flush the whole D-cache.
//   Note that registers such as CLIDR_EL1, CSSELR_EL1, etc. are 32-bits, 
//   though armv8 requires MRS/MSR instruction to pass 64-bit Xt register.
//   So we will read and write 64-bits, and operate on 32-bits.
//============================================================================  
.type dcache_clean_all, @function
dcache_clean_all:
  DSB     SY                   // ensure ordering with previous memory accesses
  MRS     x0, CLIDR_EL1        // Read CLIDR_EL1 into Xt
  ANDS    w3, w0, #0x7000000   // extract LoC from clidr  
  ORR     w3, wzr, w3, lsr #23 // left align loc bit field in bits 3:0, and 
                               // increment loop +=2 as cssr reg level bits 3:1
  BEQ     finished1            // if LoC is 0, then no need to clean
  MOV     w10, #0              // start clean at cache level 0
loop4:
  ADD     w2, w10, w10, lsr #1  // calculate 3x cur_level, as type field=3bits
  LSR     w1, w0, w2            // extract cache type bits from clidr_el1        
  AND     w1, w1, #7            // mask of the bits for current cache only
  CMP     w1, #2                // see if we have Dcache at this level
  BLT     skip1                 // skip if no cache, or just i-cache
  MSR     CSSELR_EL1, x10       // select current cache level in CSSELR_EL1
  ISB     SY                    // isb to sych the new CCSIDR_EL1
  MRS     x1, CCSIDR_EL1        // read the new csidr
  AND     w2, w1, #7            // extract the length of the cache lines
  ADD     w2, w2, #4            // add 4 (line length offset)
  MOV     w4, 0x3ff            // mask off 'associativity' bits of CCSIDR_EL1
  ANDS    w4, w4, w1, lsr #3    // find maximum number on the way size
  CLZ     w5, w4                // find bit position of way size increment
  MOV     w7, 0x7fff           // mask off 'NumSets' bits of CCSIDR_EL1
  ANDS    w7, w7, w1, lsr #13   // extract max number of the index size
loop5:
  MOV     w9, w4                // create working copy of max way size
loop6:
  LSL     w28, w9, w5           // factor way and cache number into w11
  ORR     w11, w10, w28
  LSL     w29, w7, w2           // factor index number into w11
  ORR     w11, w11, w29 
  DC      CISW, x11             // clean & invalidate by set/way
  SUBS    w9, w9, #1            // decrement the way
  BGE     loop6
  SUBS    w7, w7, #1            // decrement the index
  BGE     loop5
skip1:
  ADD     w10, w10, #2          // increment cache number
  CMP     w3, w10
  BGT     loop4
finished1:
  MOV     w10, #0               // swith back to cache level 0
  MSR     CSSELR_EL1, x10       // select current cache level in CSSELR_EL1
  DSB     SY
  ISB     SY
  ret
  
//============================================================================
// dcache_flush_region_without_stack_use
//
// PROTOTYPE
//   void dcache_flush_region_without_stack_use(void *addr, unsigned int length)//
//
// ARGS
//   void *addr            - Start address of memory region
//   unsigned int length   - Length of memory region
//
// DESCRIPTION
//   Cleans and invalidates a memory region in the cache. Please note that the
//   data in the cache would be written back to main memory if dirty and the 
//   region invalidated. So any further access to the data would result in a 
//   cache-miss.   
//============================================================================

.type dcache_flush_region_without_stack_use, @function
dcache_flush_region_without_stack_use:
  MOV     x4, x0                // copy addr
  MOV     x5, x1                // copy size

  // If the size is invalid, then there is no flushing required
  CMP     x1, #0
  BLE     flush_done
  
  ADD     x1, x0, x1            // get the end address
  SUB     x1, x1, #1            // Adjust the end address to correct value.

//  dcache_line_size x2, x3      // x2 holds cache line size
  MRS      x3, CTR_EL0
  // read CTR bits 16 through 19 for DMINLINE
  ORR      x3, xzr, x3, lsr #16 // 
  ANDS     x3, x3, #0xF         // cache line size encoding, 4 bits.
  BNE      size_offset
  MOV      x2, #0               // for the unlikely event that DMINLINE is 0
  B        do_lsl
size_offset:  
  MOV      x2, #4               // size offset. (DMINLINE is log2 words)
do_lsl:  
  LSL      x2, x2, x3           // actual cache line size in $reg. 64 bytes 
                                // for Krait and 32 bytes for Scorpion.
  SUB     x3, x2, #1            // x3 holds alignment mask
  
  // Get the cache line aligned start address
  BIC     x0, x0, x3            // x0 holds cache aligned start address
  
  // Get the cache line aligned end address and flush the line 
  BIC     x1, x1, x3            // x1 holds cache aligned end address
  DC      CIVAC, x1             // clean & invalidate D / U line 
  DSB     ISH
  // Execute the loop until we have flushed all the lines between start and end	
flush_1oop:  
  DC      CIVAC, x0             // clean & invalidate D / U line
  DSB     ISH
  ADD     x0, x0, x2
  CMP     x0, x1
  BCS     flush_done
  B       flush_1oop
  
flush_done:  
  DSB     SY                    // ensure all memory operations are complete

#ifdef BOOT_USES_L2_DCACHE   	
  // L2 cache flush
  MOV     x0, x4                // copy addr to x0
  MOV     x1, x5                // copy size to x1
  bl      l2_ranged_cache_flush
#endif

  DSB     SY
  ISB     SY

  ret
//============================================================================
// dcache_flush_region
//
// PROTOTYPE
//   void dcache_flush_region(void *addr, unsigned int length)//
//
// ARGS
//   void *addr            - Start address of memory region
//   unsigned int length   - Length of memory region
//
// DESCRIPTION
//   Cleans and invalidates a memory region in the cache. Please note that the
//   data in the cache would be written back to main memory if dirty and the 
//   region invalidated. So any further access to the data would result in a 
//   cache-miss.   
//============================================================================
.type dcache_flush_region, @function
dcache_flush_region:
  push_registers
  BL dcache_flush_region_without_stack_use
  pop_registers
  ret

//============================================================================
// dcache_inval_region
//
// PROTOTYPE
//   void dcache_inval_region(void *addr, unsigned int length)//
//
// ARGS
//   void *addr            - Start address of memory region
//   unsigned int length   - Length of memory region
//
// DESCRIPTION
//   Invalidate a memory region in the cache. Please note that the data in 
//   the cache will not be written back to the main memory. However, any 
//   further accesses to data in this region would result in a cache-miss.   
//============================================================================
.type dcache_inval_region, @function
dcache_inval_region:
  push_registers
  
  MOV     x4, x0                      // copy start addr
  MOV     x5, x1                      // copy size	

  // If the size is invalid, then there is no invalidation required
  CMP     x1, #0
  BLE     inval_done

  ADD     x1, x0, x1                  // get the end address  
  SUB     x1, x1, #1                  // Adjust the end address to correct value.
  MOV     x6, x1                      // copy end address
  
  //dcache_line_size x2, x3            // x2 holds cache line size
  MRS      x3, CTR_EL0
  // read CTR bits 16 through 19 for DMINLINE
  ORR      x3, xzr, x3, lsr #16 // 
  ANDS     x3, x3, #0xF         // cache line size encoding, 4 bits.
  BNE      size_offset1
  MOV      x2, #0               // for the unlikely event that DMINLINE is 0
  B        do_lsl1
size_offset1:
  MOV      x2, #4               // size offset. (DMINLINE is log2 words)
do_lsl1:
  LSL      x2, x2, x3           // actual cache line size in $reg. 64 bytes 
                                // for Krait and 32 bytes for Scorpion.
  SUB     x3, x2, #1                  // x3 holds alignment mask

  // Get the cache line aligned start address and flush the line	
  BIC     x0, x0, x3                  // x0 holds cache aligned start address
  CMP     x0, x4                      // compare if start address is already cache line aligned
  BEQ     skip_c_and_i_start
  DC      CIVAC, x0                   // clean & invalidate D / U line only if not cache aligned

skip_c_and_i_start:  
   // Get the cache line aligned end address and flush the line		
  BIC     x1, x1, x3                  // x1 holds cache aligned end address
  CMP     x1, x6                      // compare if end address is already cache line aligned
  BEQ     skip_c_and_i_end
  DC      CIVAC, x1                   // clean & invalidate D / U line only if not cache aligned
  B       inval_loop
skip_c_and_i_end:
  DC      IVAC, x1                    // invalidate D / U line if cache aligned  
   // Execute the loop until we have invalidated all the lines between start and end	
inval_loop:
  DC      IVAC, x0                    // invalidate D / U line
  ADD     x0, x0, x2
  CMP     x0, x1
  BCS    inval_done
  B      inval_loop
  
inval_done:	
  DSB    SY                           // ensure all memory operations are complete

#ifdef BOOT_USES_L2_DCACHE   	
  // L2 cache inval
  MOV    x0, x4                       // copy addr to r0
  MOV    x1, x5                       // copy size to r1
  BL     l2_ranged_cache_invalidate	
#endif

  DSB    SY
  ISB    SY
  
  pop_registers
  ret

//============================================================================
// dcache_clean_region
//
// PROTOTYPE
//   void dcache_clean_region(void *addr, unsigned int length)//
//
// ARGS
//   void *addr            - Start address of memory region
//   unsigned int length   - Length of memory region
//
// DESCRIPTION
//   Cleans a memory region in the cache. Please note that this will write 
//   back any data that is dirty but it will not invalidate the cache region. 
//   So any further accesses to data in this region would result in a cache-hit.   
//============================================================================
.type dcache_clean_region, @function
dcache_clean_region:
  push_registers

  MOV     x4, x0                      // copy addr
  MOV     x5, x1                      // copy size

  // If the size is invalid, then there is no clean required
  CMP     x1, #0
  BLE     clean_done

  ADD     x1, x0, x1                  // get the end address  
  SUB     x1, x1, #1                  // Adjust the end address to correct value.
  
  //dcache_line_size x2, x3            // x2 holds cache line size
  MRS      x3, CTR_EL0
  // read CTR bits 16 through 19 for DMINLINE
  ORR      x3, xzr, x3, lsr #16 // 
  ANDS     x3, x3, #0xF         // cache line size encoding, 4 bits.
  BNE      size_offset2
  MOV      x2, #0               // for the unlikely event that DMINLINE is 0
  B        do_lsl2
size_offset2:
  MOV      x2, #4               // size offset. (DMINLINE is log2 words)
do_lsl2:
  LSL      x2, x2, x3           // actual cache line size in $reg. 64 bytes 
                                // for Krait and 32 bytes for Scorpion.
  SUB     x3, x2, #1                  // x3 holds alignment mask
  
    
  // Get the cache line aligned start address
  BIC     x0, x0, x3                  // x0 holds cache aligned start address
  
  // Get the cache line aligned end address and clean the line	
  BIC     x1, x1, x3                  // x1 holds cache aligned end address
  DC      CVAC, x1                    // clean D / U line	
  
  // Execute the loop until we have cleaned all the lines between start and end
clean_loop:
  DC      CVAC, x0                    // clean D / U line
  ADD     x0, x0, x2
  CMP     x0, x1
  BCS     clean_done 
  B       clean_loop
  
clean_done: 
  DSB     SY                          // ensure all memory operations are complete

#ifdef BOOT_USES_L2_DCACHE   	
  // L2 cache clean
  MOV     x0, x4                      // copy addr to x0
  MOV     x1, x5                      // copy size to x1
  BL      l2_ranged_cache_clean	
#endif

  DSB     SY
  ISB     SY
  
  pop_registers
  ret

//============================================================================
// mmu_flush_cache
//
// DESCRIPTION -
//   Flush the entire cache system.
//   The data cache flush is now achieved using atomic clean / invalidates
//   working outwards from L1 cache. This is done using Set/Way based cache
//   maintainance instructions.
//   The instruction cache can still be invalidated back to the point of
//   unification in a single instruction.
//============================================================================  
.type mmu_flush_cache, @function
mmu_flush_cache:
  push_registers

  // Call memory barrier operations to ensure completion of all cache 
  // maintenance & branch predictor operations appearing in program 
  // order, before these instructions 
  DSB     SY
  ISB     SY
  BL      dcache_clean_all
  MOV     x0, #0
  IC      IALLU                 // I+BTB cache invalidate
  DSB     SY
  ISB     SY

#ifdef BOOT_USES_L2_DCACHE      
  // Flush L2 cache
  BL      l2_cache_flush
  DSB     SY
  ISB     SY
#endif

  pop_registers
  ret

//============================================================================
// mmu_flush_cache_and_disable
//
//
// DESCRIPTION -
//  Flush the entire cache system.
//  The data cache flush is now achieved using atomic clean / invalidates
//  working outwards from L1 cache. This is done using Set/Way based cache
//  maintainance instructions.
//  The instruction cache can still be invalidated back to the point of
//  unification in a single instruction.
//  Immediately after flushing data cache, the caches are disabled.
//
//============================================================================	
.type mmu_flush_cache_and_disable, @function
mmu_flush_cache_and_disable:
  push_registers

  // Call memory barrier operations to ensure completion of all cache 
  // maintenance & branch predictor operations appearing in program 
  // order, before these instructions 
  DSB     SY
  ISB     SY
  bl dcache_clean_all
  // need to disable caches here. else the restoring of regs [ldmfd]
  // caches those addresses and cache does not remain flushed.
  mmu_disable_i_and_d_cache
  
  // Invalidate cache after disabling it. This ensures no valid cache lines
  // remain in disabled cache.  
  MOV     x0, #0
  IC IALLU                      // I+BTB cache invalidate
  DSB     SY
  ISB     SY
  
#ifdef BOOT_USES_L2_DCACHE
  // Flush L2 cache and disable it
  BL      l2_cache_flush_and_disable
  DSB     SY
  ISB     SY
#endif

  pop_registers
  ret
  

//============================================================================
// cache_mmu_init_page_table
//  INPUT       - PAGE_TABLE_BASE ADDRESS     
//  DESCRIPTION -
//        Initializes the ARM MMU.
//                1- sets a default value in the control register
//                2- sets the FSCE PID register to be flat mapping 
//                3- sets the physical address of the page table
//                4- populates the page_table with section entries
//                5- sets the domain access permisions
//                6- flushes out cache and write buffer
//                7- enables the mmu
//============================================================================
.type cache_mmu_init_page_table, @function
cache_mmu_init_page_table:
   // Save the registers first
   push_registers

   // Save x0 containing page-table-base
   MOV      x27, x0

   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 

   DSB      SY
   ISB      SY
   MRS      x1, SCTLR_EL3
   AND      x1, x1, #0xFFFFFFFFFFFFFFFD     // Clear A bit so misaligned accesses do not trigger exception
   MSR      SCTLR_EL3, x1
   MRS      x1, SCTLR_EL1
   AND      x1, x1, #0xFFFFFFFFFFFFFFFD     // Clear A bit so misaligned accesses do not trigger exception
   MSR      SCTLR_EL1, x1
   DSB      SY
   ISB      SY
   BL       mmu_invalidate_data_cache
   BL       mmu_invalidate_instruction_cache
   BL       mmu_invalidate_tlb_el1_el3
   
   // Restore page-table-base to x0
   MOV      x0, x27
   
   // set page table base for both EL3 and EL1, so EL1 can be checked 
   // regardless of whether we are modifying page table before or after TZ
   AND      x0, x0, #0x0000FFFFFFFFFFFF
   MSR      TTBR0_EL3, x0 //configure base PT to passed in "mmu_l1_pagetable"
   MSR      TTBR0_EL1, x0 //configure base PT to passed in "mmu_l1_pagetable"
   
   // Set up memory attributes
   // -------------------------
   MOVZ     x1, (MAIR_EL3_ATTR >> 48), LSL #48
   MOVK     x1, (MAIR_EL3_ATTR >> 32) & 0xffff, LSL #32
   MOVK     x1, (MAIR_EL3_ATTR >> 16) & 0xffff, LSL #16
   MOVK     x1, MAIR_EL3_ATTR  & 0xffff

   MSR      MAIR_EL3, x1
   MSR      MAIR_EL1, x1
   
   // Set up TCR_EL3 (translation control register)
   // ---------------------------------------------
   // Set size fields so that first level is 1, not 0
   // TBI, bit [20]      Top Byte ignored = 0
   // PS, bits [18:16]   Physical address Size = 011 (42 bits, 4TB)   
   // TG0, bits [15:14]  Granule size = 00 (4KByte)
   // SH0, bits [13:12]  Shareability for memory associated with translation
   //                    table walks using TTBR0_EL3 = 11 (Shareable)
   // ORGN0 bits [11:10] Outer cacheability for memory associated with 
   //                    translation table walks using TTBR0_EL3 = 00 (NC)
   // IRGN0 bits [9:8] 	Inner cacheability for memory associated with
   //                    translation table walks using TTBR0_EL3 = 00 (NC)
   // T0SZ, bits [5:0]   The size offset of the memory region addressed by 
   //                    TTBR0_EL3.  Size is 2^(64-T0SZ) bytes   = 0x19
   //                    which is minimum required size for starting at level1
   // 0x0000000000030025
   MOVZ     x1, 0x0003, LSL #16
   MOV      x2, 0x0019
   ORR      x1, x1, x2
   MSR      TCR_EL3, x1
   MSR      TCR_EL1, x1

   // The ISB is needed to ensure the changes to system context are seen before
   // the write of the SCTLR_EL3.M to enable the MMU.

   ISB

   // Generate TTBR0 L1: 4 1GB entries covering DDR
   // ---------------------------------------------

   //table[0] = 0x0000,0000,0000,0000 - 0x0000,0000,3FFF,FFFF   - declare entire retire region as Normal region
   MOV      x1, x0  // Address of L1 table, save in X1

   MOV      x0, MMU_MAIR_DEVICE_nGnRE// Entry template, normal, non-cacheable, non-shared
   MOV      x2, MMU_PAGETABLE_BLOCK_DESCRIPTOR
   ORR      x0, x0, x2
   ORR      x0, x0, #MMU_PAGETABLE_ACCESS_FLAG_SET 
   STR      x0, [x1]                              // Write template into entry table[0]

   
   //table[1] = 0x0000,0000,4000,0000 - 0x0000,0000,7FFF,FFFF   - declare entire retire region as Normal region
   MOV      x0, MMU_MAIR_DEVICE_nGnRE// Entry template, normal, non-cacheable, non-shared
   MOV      x2, MMU_PAGETABLE_BLOCK_DESCRIPTOR
   ORR      x0, x0, #0x40000000                     // Address of 2nd block
   ORR      x0, x0, x2
   ORR      x0, x0, #MMU_PAGETABLE_ACCESS_FLAG_SET   
   STR      x0, [x1, #8]                          // Write template into entry table[0]
   
   //table[2] = 0x0000,0000,8000,0000 - 0x0000,0000,BFFF,FFFF   - declare entire retire region as Normal region
   MOV      x0, MMU_MAIR_DEVICE_nGnRE// Entry template, normal, non-cacheable, non-shared
   MOV      x2, MMU_PAGETABLE_BLOCK_DESCRIPTOR
   ORR      x0, x0, #0x80000000                     // Address of 3rd block
   ORR      x0, x0, x2
   ORR      x0, x0, #MMU_PAGETABLE_ACCESS_FLAG_SET   
   STR      x0, [x1, #16]                         // Write template into entry table[0]
   
   //table[3] = 0x0000,0000,C000,0000 - 0x0000,0000,FFFF,FFFF   - declare entire retire region as Normal region
   MOV      x0, MMU_MAIR_DEVICE_nGnRE// Entry template, normal, non-cacheable, non-shared
   MOV      x2, MMU_PAGETABLE_BLOCK_DESCRIPTOR
   ORR      x0, x0, #0xC0000000                     // Address of 4th block
   ORR      x0, x0, x2
   ORR      x0, x0, #MMU_PAGETABLE_ACCESS_FLAG_SET   
   STR      x0, [x1, #24]                         // Write template into entry table[0]   

   // REVISIT: will we need to call "mmu_enable" later, after OCMEM region added?
   mmu_enable

   // Restore the LR before returning
   pop_registers
   ret

//============================================================================
// mmu_reconfigure
//  INPUT       - None
//  DESCRIPTION -
//        Initializes the ARM MMU, depending on current EL.
//                1- sets a default value in the control register
//                2- sets the memory attribute MAIR register
//
//  NOTE: It is expected mmu_invalidate_tlb shall be called immediately 
//        after this api
//
//============================================================================
.type mmu_reconfigure, @function   
mmu_reconfigure: 
   // Check the current Exception Level
   //----------------------------------
   MRS    x1, CurrentEL
   LSR    x1, x1, #2   
   CMP    x1, #3
   BGE    mmu_reconfigure_el3
   // Set up TCR_EL1 (translation control register) to 
   // -----------------------------------------------------------
   // Set size fields so that first level is 1, not 0
   // TBI1, bit [38]     Top Byte ignored (TTBR1) = 0
   // TBI0, bit [37]     Top Byte ignored (TTBR0) = 0
   // AS,   bit [36]     The upper 8 bits of TTBR0_EL1 and TTBR1_EL1 are ignored by hardware
   //                    for every purpose except reading back the register = 0
   // IPS, bits [34:32]  Physical address Size = 001 (36 bits, 64GB)   
   // TG1, bits [31:30]  TTBR1_EL1 Granule size = 00 (implementation defined)
   // SH1, bits [29:28]  Shareability for memory associated with translation 
   //                    table walks using TTBR1_EL1 = 00 (non-Shareable)
   // ORGN1 bits [27:26] Outer cacheability for memory associated with 
   //                    translation table walks using TTBR1_EL1 = 00 (NC)
   // IRGN1 bits [25:24] Inner cacheability for memory associated with 
   //                    translation table walks using TTBR1_EL1 = 00 (NC)
   // EPD1, bit [23]     Translation table walk disable using TTBR1_EL1 = 0 (perform walks)
   // A1, bit [22]       Selects whether TTBR0_EL1 or TTBR1_EL1 defines the ASID = 0 (TTBR0_EL1)
   // T1SZ, bits [21:16] The size offset of the memory region addressed by 
   //                    TTBR1_EL1.  Size is 2^(64-T0SZ) bytes   = 0x0 (unused)  
   // TG0, bits [15:14]  Granule size = 00 (4KByte)
   // SH0, bits [13:12]  Shareability for memory associated with translation
   //                    table walks using TTBR0_EL1 = 00 (Non-Shareable)
   // ORGN0 bits [11:10] Outer cacheability for memory associated with 
   //                    translation table walks using TTBR0_EL1 = 00 (NC)
   // IRGN0 bits [9:8] 	 Inner cacheability for memory associated with
   //                    translation table walks using TTBR0_EL1 = 00 (NC)
   // EPD0, bit [7]      Translation table walk disable using TTBR0_EL1 = 0 (perform walks)
   // T0SZ, bits [5:0]   The size offset of the memory region addressed by 
   //                    TTBR0_EL1.  Size is 2^(64-T0SZ) bytes   = 0x1C
   //                    which is minimum required size for starting at level1   

   MOV      x1, #0x001C
   ORR      x1, x1, #0x800000
   MOVK     x1, 0x1, LSL #32   
   MSR      TCR_EL1, x1

   // Set up memory attributes
   // -------------------------
   MOVZ     x1, (MAIR_EL3_ATTR >> 48), LSL #48
   MOVK     x1, (MAIR_EL3_ATTR >> 32) & 0xffff, LSL #32
   MOVK     x1, (MAIR_EL3_ATTR >> 16) & 0xffff, LSL #16
   MOVK     x1, MAIR_EL3_ATTR  & 0xffff

   MSR      MAIR_EL1, x1   
   B      mmu_reconfigure_end
mmu_reconfigure_el3:
   // Set up TCR_EL3 (translation control register) to 0x0001301C
   // -----------------------------------------------------------
   // Set size fields so that first level is 1, not 0
   // TBI, bit [20]      Top Byte ignored = 0
   // PS, bits [18:16]   Physical address Size = 001 (36 bits, 64GB)   
   // TG0, bits [15:14]  Granule size = 00 (4KByte)
   // SH0, bits [13:12]  Shareability for memory associated with translation
   //                    table walks using TTBR0_EL3 = 00 (Non-Shareable)
   // ORGN0 bits [11:10] Outer cacheability for memory associated with 
   //                    translation table walks using TTBR0_EL3 = 00 (NC)
   // IRGN0 bits [9:8] 	Inner cacheability for memory associated with
   //                    translation table walks using TTBR0_EL3 = 00 (NC)
   // T0SZ, bits [5:0]   The size offset of the memory region addressed by 
   //                    TTBR0_EL3.  Size is 2^(64-T0SZ) bytes   = 0x1C
   //                    which is minimum required size for starting at level1

   MOV      x1, #0x001C
   MOVK     x1, 0x1, LSL #16
   MSR      TCR_EL3, x1

   // Set up memory attributes
   // -------------------------
   MOVZ     x1, (MAIR_EL3_ATTR >> 48), LSL #48
   MOVK     x1, (MAIR_EL3_ATTR >> 32) & 0xffff, LSL #32
   MOVK     x1, (MAIR_EL3_ATTR >> 16) & 0xffff, LSL #16
   MOVK     x1, MAIR_EL3_ATTR  & 0xffff

   MSR      MAIR_EL3, x1
mmu_reconfigure_end: 
   ret
   
//============================================================================


//============================================================================
// mmu_relocate_page_table_base
//
//// ARGS
//   unsigned long *dst_base  - destination page table base
//
// DESCRIPTION -
//  Relocate the base(TTBR0) of page table, all attributes of TTBR0 will be 
//  perserved
//
//============================================================================
.type mmu_relocate_page_table_base, @function
mmu_relocate_page_table_base:

  push_registers
  DSB     SY
  ISB     SY
  // Check the current Exception Level
  //----------------------------------
  MRS    x1, CurrentEL
  LSR    x1, x1, #2   
  CMP    x1, #3
  BGE    relocate_pt_base_el3
  MSR    TTBR0_EL1, X0         // set new TTBR0_EL1 location
  DSB    SY
  ISB    SY
  TLBI   VMALLE1               // Invalidate all EL1 entries  
  B      relocate_pt_base_end
relocate_pt_base_el3:
  MSR    TTBR0_EL3, x0         // set new TTBR0_EL3 location
  DSB    SY
  ISB    SY  
  TLBI   ALLE3                 // Invalidate all EL3 entries  
relocate_pt_base_end:     
                               // ARMv7 does not define any branch predictor maintenance instructions 
                               // for AArch64 state, so BPIALL has been removed.
  DSB    SY
  ISB    SY
  pop_registers
  ret

   
//============================================================================
// mmu_get_page_table_base
//
//// RETURN
//   unsigned long  -  page table base
//
// DESCRIPTION -
//  Return the base address of page table, all attributes of TTBR0 will be 
//  cleared
//
//============================================================================
.type mmu_get_page_table_base, @function
mmu_get_page_table_base:
   // Check the current Exception Level
   //----------------------------------
   MRS    x0, CurrentEL
   LSR    x0, x0, #2   
   CMP    x0, #3
   BGE    get_pt_base_el3
   MRS    x0, TTBR0_EL1
   B      get_pt_base_end
get_pt_base_el3:
   MRS    x0, TTBR0_EL3
get_pt_base_end:   
   ret
   
   
//============================================================================
// mmu_set_dacr
//
// PROTOTYPE
//   void mmu_set_dacr(uint32 dacr_value)//
//
// ARGS
//   unsigned int dacr_value   - Value to be written to DACR
//   r0 register shall contain the argument
//
// DESCRIPTION
//   Configure DACR, Domain Access Control Register.
//   
//============================================================================    
.type mmu_set_dacr, @function
mmu_set_dacr:
  // Configure Domain access control register register DACR to have appropriate 
  // permissions for different domains.
  // Check the current Exception Level
  //----------------------------------
   MRS    x1, CurrentEL
   LSR    x1, x1, #2   
   CMP    x1, #3
   BGE    set_dacr32_el2
   MOV    x1, #0x00300000 // Sets bits 21, 20 so that SIMD access do not cause exception
   MSR    CPACR_EL1, x1
   B      set_dacr32_end
set_dacr32_el2:
   MSR    DACR32_EL2, x0    // Write DACR
   DSB    SY
   ISB    SY
set_dacr32_end:    
   ret

//============================================================================

//============================================================================
// boot_enable_mmu
//
// ARGS
//  ptr:  internal memory table address, used for flush cache 
//  size: internal memory table size
//
// DESCRIPTION
//   Enables the mmu
//============================================================================
.type boot_enable_mmu, @function
boot_enable_mmu:
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   push_registers
   DSB      SY
   ISB      SY
   MOV      x2, #1
   BL       sctlr_read
   ORR      x1, x1, x2, lsl #0  // Set M bit 
   BL       sctlr_write
   DSB      SY
   ISB      SY
   pop_registers
   ret


//============================================================================
// boot_disable_mmu
//
// ARGS
//  ptr:  internal memory table address, used for flush cache 
//  size: internal memory table size
//
// DESCRIPTION
//   Disables the mmu
//============================================================================
.type boot_disable_mmu, @function
boot_disable_mmu:
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   push_registers
   DSB      SY
   ISB      SY
   BL       sctlr_read
   MOV      x2, #1
   BIC      x1, x1, x2, lsl #0  // Clear M bit
   BL       sctlr_write
   DSB      SY
   ISB      SY
   pop_registers
   ret
//=================================================================================   

//============================================================================
// boot_backup_sp
// PROTOTYPE
//   void boot_backup_sp(void* pstack_sp)//
// ARGS
//  void* pstack_sp     --point to back-up address of stack pointer
//
// DESCRIPTION
//   backup stack point
//============================================================================
.type boot_backup_sp, @function
boot_backup_sp:
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   DSB      SY
   ISB      SY
   MOV      X1, SP
   STR      X1, [X0]
   DSB      SY
   ISB      SY
   ret
//=================================================================================   
//============================================================================
// boot_restore_sp
// PROTOTYPE
//   void boot_restore_sp(void* pstack_sp)//
// ARGS
//   void* pstack_sp     --point to backup address of stack pointer
//
// DESCRIPTION
//   restor stack point
//============================================================================
.type boot_restore_sp, @function
boot_restore_sp:
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   DSB      SY
   ISB      SY
   LDR      X1, [X0]
   MOV      SP,  X1
   DSB      SY
   ISB      SY
   ret
//=================================================================================

//============================================================================
// boot_enable_dcache
//
// ARGS
// None
//
// DESCRIPTION
//   Enable the dcache
//============================================================================   
.type boot_dcache_enable, @function
boot_enable_dcache:
   push_registers
   dcache_enable
   pop_registers
   ret
   
//============================================================================
// boot_flush_regions_disable_dcache
//
// PROTOTYPE
//   void boot_flush_regions_disable_dcache(void *addr, uint64 length)//
//
// ARGS
//   void *ptr            - Start address of memory regions
//   uint64 size          - Length of memory regions
//
// DESCRIPTION
//   Cleans and invalidate memory regions in the cache, and then disable cache.
//   Please note that the data in the cache would be written back to main 
//   memory if dirty and the region invalidated. So if cache enabled, any 
//   further access to the data would result in a cache-miss.   
//============================================================================

.type boot_flush_regions_disable_dcache, @function
boot_flush_regions_disable_dcache:
   push_registers
   MOV      x10, x0   //store internal mem table address to x10
   MOV      x11, x1   //store internal mem table size to x11
flush_cache:
   CMP      x11, #0   //determine whether the size is 0
   BEQ      flush_end
   LDR      x0, [x10] //get internal mem address to x0
   ADD      x10, x10, #8
   LDR      x1, [x10] //get internal mem size to x1
   ADD      x10, x10, #8
   SUB      x11, x11, #1 //table size subtract 1 
   BL dcache_flush_region_without_stack_use
   B       flush_cache
flush_end:
   dcache_disable
   pop_registers
   ret
//============================================================================
// mmu_inval_trans_tbl_entry
//
// ARGS
//   void *ttptr            - the translation table entry address
//
// DESCRIPTION
//   Replace the old translation table entry with invalid entry, and excute a DSB instruction
//============================================================================
.type mmu_inval_trans_tbl_entry, @function
mmu_inval_trans_tbl_entry:
   DSB      SY
   ISB      SY
   // Replace the old translation table entry with invalid entry
   MOV      x15, #0
   STR      x15, [x0]
   DSB      ISHST
   ISB      SY
   ret   

//============================================================================
// boot_is_icache_enabled
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Check if I cache is enabled
//============================================================================
.type boot_is_icache_enabled, @function
boot_is_icache_enabled:
   STP     x30, x29, [sp, #-16]!   
   BL      sctlr_read
   MOV     x0, #1
   AND     x0, x1, x0, lsl #12 // Check I bit 
   LDP     x30, x29, [sp], #16 
   ret
//=================================================================================   

//============================================================================
// boot_is_dcache_enabled
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Check if D cache is enabled
//============================================================================
.type boot_is_dcache_enabled, @function
boot_is_dcache_enabled:
   STP     x30, x29, [sp, #-16]!
   BL      sctlr_read
   MOV     x0, #1
   AND     x0, x1, x0, lsl #2  // Check C bit 
   LDP     x30, x29, [sp], #16   
   ret
//=================================================================================      
   .end
