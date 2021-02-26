//------------------------------------------------------------------------------ 
//
//  Copyright (c) 2014, 2017 Qualcomm Technologies Inc. All rights reserved.
//  Portions Copyright (c) 2011-2013, ARM Limited. All rights reserved.
//
//  This program and the accompanying materials
//  are licensed and made available under the terms and conditions of the BSD License
//  which accompanies this distribution.  The full text of the license may be found at
//  http://opensource.org/licenses/bsd-license.php
//
//  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//------------------------------------------------------------------------------

//=============================================================================
//                              EDIT HISTORY
//
//
// when       who     what, where, why
// --------   ---     ---------------------------------------------------------
// 05/19/17   vk      Add support for EL3 handling in loader for presil
// 05/30/14   kedara  Initial revision
//
//============================================================================
#include <AsmMacroIoLibV8.h>
#include <Chipset/AArch64.h>

.text
.align 3
GCC_ASM_EXPORT (_ModuleEntryPoint)
GCC_ASM_IMPORT (sbl1_entry)
 
#ifdef FEATURE_START_EL3
GCC_ASM_IMPORT (Image$$SBL1_DATA_ZI$$Base)
GCC_ASM_IMPORT (Image$$SBL1_DATA_ZI$$End)

.align 3
.global CNTFRQ 
CNTFRQ:
  .quad 0x124F800

.global sbl1_lel64_sync_addr
sbl1_lel64_sync_addr:
  .quad 0x1483f400

.global pbl_shared_data_pointer
pbl_shared_data_pointer:
  .quad 0x14823188

.global Instr_MovX0_0
Instr_MovX0_0:
  .word 0xD2800000  /* Decode as mov x0, #0*/

.global Instr_eret
Instr_eret:
  .word 0xD69F03E0  /* Decode as eret */
#endif

.align 3
.global 
_ModuleEntryPoint:
  /* Get current EL in x4 */
  EL1_OR_EL2_OR_EL3(x4)
1:
    /* x0 should contain PBL shared data pointer */
    b sbl1_entry
2:  b dead
3:
#ifndef FEATURE_START_EL3
    b dead
#else    
  ldr x0, pbl_shared_data_pointer /* TODO: Done by PBL, remove after PBL is available */

  /* First ensure all interrupts are disabled */
  bl ASM_PFX(ArmDisableInterrupts)

  /* Ensure that the MMU and caches are off */
  bl ASM_PFX(ArmDisableCachesAndMmu)
  
  /* Invalidate Instruction Cache and TLB */ 
  bl ASM_PFX(ArmInvalidateInstructionCache)

  bl ASM_PFX(ArmInvalidateTlb)
  bl ArmWriteCptr

_SetupELx:
  mov x0, #0x30           /* RES1 */
  orr x0, x0, #(1 << 0)   /* Non-secure bit */
  orr x0, x0, #(1 << 8)   /* HVC enable */
  orr x0, x0, #(1 << 10)  /* 64-bit EL2 */
  msr scr_el3, x0

  msr cptr_el3, xzr       /* Disable copro. traps to EL3 */

  ldr x0, CNTFRQ
  msr cntfrq_el0, x0

  msr sctlr_el2, xzr

  /* Now setup our EL1. Controlled by EL2 config on Model */
  mrs x0, hcr_el2         /* Read EL2 Hypervisor configuration Register */
  orr x0, x0, #(1 << 31)  /* Set EL1 to be 64bit */

  /* Send all interrupts to their respective Exception levels for EL2 */
  mov x5, #(1 << 3)
  bic x0, x0, x5          /* Disable virtual FIQ */

  mov x5, #(1 << 4)
  bic x0, x0, x5          /* Disable virtual IRQ */

  mov x5, #(1 << 5)
  bic x0, x0, x5          /* Disable virtual SError and Abort */

  msr hcr_el2, x0         /* Write back our settings */

  /* Enable architected timer access */
  mrs x0, cnthctl_el2
  orr x0, x0, #3          /* Enable EL1 access to timers */
  msr cnthctl_el2, x0

  mrs x0, cntkctl_el1
  orr x0, x0, #3          /* EL0 access to counters */
  msr cntkctl_el1, x0

  /* Set ID regs */
  mrs x0, midr_el1
  mrs x1, mpidr_el1
  msr vpidr_el2, x0
  msr vmpidr_el2, x1

  /* Coprocessor traps. */
  mov x0, #0x33ff
  msr cptr_el2, x0        /* Disable copro. traps to EL2 */

  msr hstr_el2, xzr       /* Disable CP15 traps to EL2 */

  mov x0, #CPACR_CP_FULL_ACCESS
  bl ArmWriteCpacr        /* Disable copro traps to EL1 */
  
  /* Update Vector table to trap and returm SMC call */
  ldr x0, sbl1_lel64_sync_addr
  
  /* Update instruction at sbl1_lel64_syn to mov x0, #0 */
  ldr w1, Instr_MovX0_0
  str w1, [x0]

  /* Update instruction to eret */
  mov x3, #0x4;
  add x0, x0, x3
  ldr w1, Instr_eret
  str w1, [x0]

  /* Update VBAR_EL3 */
  adr x0, Image$$SBL1_VECTOR_TABLE$$Start
  bl ArmWriteVBar

  /* Switch to ELx NS */
  bl ArmDisableAlignmentCheck

  /* Zero init the ZI region */
_ZeroInitZIEl3:  
  adr x0, Image$$SBL1_DATA_ZI$$Base
  adr x1, Image$$SBL1_DATA_ZI$$End
  sub x1, x1, x0
  bl __ZeroInitRange

  adr x0, sbl1_entry /* Load start address for ELx */
  msr elr_el3, x0

EL1:
  mov x1, #0xc5 /* IF, EL1, EL specific SP */
  msr spsr_el3, x1
  mov x0, #0x0  /* Disable MMU, Alignment check etc for EL1 */
  msr sctlr_el1, x0
  ldr x0, pbl_shared_data_pointer
  eret

/* x0 base and x1 size */
__ZeroInitRange:
  /* Zero Init */
  add x2, x0, x1 
  mov x3, x0

  mov v4.d[0], xzr
  mov v4.d[1], xzr
  mov v5.2d, v4.2d 
  mov v6.2d, v4.2d
  mov v7.2d, v4.2d 

_ZeroFill: 
  /* Assumes base is 128-bit aligned, size is a multiple of 64B */
  st4     {v4.2d, v5.2d, v6.2d, v7.2d}, [x3], #64  /* Fill every 64 bytes */
  cmp     x3, x2                                   /* Compare Size */ 
  b.lt     _ZeroFill 
  ret
#endif

dead:  
  b dead                      /* We should never get here */
