;============================================================================
;
;             
; GENERAL DESCRIPTION
;   This file contains the A53 hierarchichal cache and mmu control functions.
;
;============================================================================

#ifndef BOOT_A53_MMU_H
#define BOOT_A53_MMU_H

/*===========================================================================
 
                           Boot A53 MMU
 
GENERAL DESCRIPTION
   A53 architecture assembly language MMU support.
   
 
Copyright 2011, 2013, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when      who       what, where, why
--------  -----     ------------------------------------------------------- 
10/05/15  ck        Corrected copyright
12/06/13  ck        Initial creation from boot_krait_mmu.h
05/20/11  kpa       Initial Creation

============================================================================*/

#ifdef _ARM_ASM_

;============================================================================
;
;                        MACROS AND FUNCTIONS
;
;============================================================================

;============================================================================
; MACRO mmu_enable_i_and_d_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables the Cache
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_enable_i_and_d_cache
   ; Call memory barrier operations to ensure completion of all cache
   ; maintenance & branch predictor operations appearing in program 
   ; order, before these instructions
   mdsb
   misb
   mrc     p15, 0, r0, c1, c0, 0   ;  Read System Control register   
   orr     r0, r0, # (1 << 2 )     ;  C bit - Enable Data Caches
   orr     r0, r0, # (1 << 12 )    ;  I bit - Enable Instruction Caches
   mcr     p15, 0, r0, c1, c0, 0   ;  Write System Control register
   mdsb
   misb
   MEND

#endif /* _ARM_ASM_ */

#endif /* BOOT_A53_MMU_H */

