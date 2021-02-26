// ****************************************************************************
// QUALCOMM Proprietary Design Data
// Copyright (c) 2014, Qualcomm Technologies Incorporated. All rights reserved.
// ****************************************************************************
// ================================================================================
//                                 EDIT HISTORY
// 
// ================================================================================
// when       who          what, where, why
// --------   ---          --------------------------------------------------------
// 12/15/15   plc          call mmu_invalidate_tlb to perform TLBI in EL context
// 02/11/15   kpa          use 64 bit regs to zero init pimem
// 10/28/14   tomwang      Initial Revision
// ================================================================================
#include "../../../../XBLLoader/boot_cache_mmu.h"
//====================================================================================================
// void __burstClear(uint32 *source, uint32 num_words)//
// Register usage:
// Passed parameters:
//   X0: uint32 *source
//   X1: uint32 num_bytes 
//====================================================================================================
  .global  __burstClear
  .global  __pimem_scrub
  
__burstClear:
      add     x4, x3, #64
      mov     x5, #0
      lsr     x2, x2, #7      // divide size by 128
      str x5, [x0]
      
      DSB      SY
      STP      x30, x0,  [sp, #-16]! // Preserve LR
      bl       mmu_invalidate_tlb               // All stage 1 translations 
      LDP      x30, x0,  [sp], #16 // Recover LR for valid return
      DSB      SY
      ISB      SY
      
      str     x1, [x0]        // write PT entry to map previously unmapped page/block, TLBI is not necessary
      dsb     sy
loop:
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128    
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128    
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128    
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128    
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dc zva  ,x3             // 64 byte write of zeros
      dc zva  ,x4             // 64 byte write of zeros
      add     x3, x3, #128
      add     x4, x4, #128
      dmb ish
      add     x5, x5, #32
      cmp     x5, x2
      bne     loop
      dsb     sy
      isb
      nop                     
      nop
      nop
      nop
      nop
      ret
     

__pimem_scrub:

  SUB     sp, sp, #0x20 // save 3 registers

  // Save passed and working registers
  stp     x2, x3,    [sp, #0]
  str     x4, [sp, #0x10]
  
  mov     x2, #0 // used as counter
  mov     x3, #0 // used for pair loading
  mov     x4, #0 // used for pair loading
  
read_128bites_loop:
  ldp x3, x4, [x0], #0x10
  ldp x3, x4, [x0], #0x10

  ldp x3, x4, [x0], #0x10
  ldp x3, x4, [x0], #0x10

  ldp x3, x4, [x0], #0x10
  ldp x3, x4, [x0], #0x10

  ldp x3, x4, [x0], #0x10
  ldp x3, x4, [x0], #0x10
  
  dsb sy
  isb
  dmb ish

  add     x2, x2, #0x80 // increment by 128 bytes
  cmp     x1, x2 // compare counter against input num bytes
  
  b.ne read_128bites_loop
  
  //Restore passed and working registers
  ldr     x4, [sp, #0x10]
  ldp     x2, x3,   [sp, #0x00]
  
  add     sp, sp, #0x20
  ret  
  .end
