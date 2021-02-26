//------------------------------------------------------------------------------ 
//
// Copyright (c) 2010-2015, Qualcomm Technologies Inc. All rights reserved.
// Portions Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.
//
// This program and the accompanying materials
// are licensed and made available under the terms and conditions of the BSD License
// which accompanies this distribution.  The full text of the license may be found at
// http://opensource.org/licenses/bsd-license.php
//  
// THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
// WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//------------------------------------------------------------------------------

//=============================================================================
//                              EDIT HISTORY
//
//
// when       who     what, where, why
// --------   ---     ---------------------------------------------------------
// 09/01/15   vk      Clean up and set stack once
// 06/22/15   vk      Workaround for HYP crash, sync to .S for AARCH64
// 12/10/14   bh      Invalidate TLB earlier, leave data cache after stack setup
// 12/03/14   bh      Invalidate TLB
// 10/08/13   vk      Pass correct stackbase to CEntrypoint
// 09/18/14   na      Invalidate cache prior to enabling it
// 02/01/13   vk      Change deprecated | to :OR:
// 11/02/12   vk      Add release info 
// 11/16/11   jz      Updated to not pass memory base and size
// 05/25/11   niting  Changed alignment settings
// 11/02/10   niting  Initial revision
//
//=============================================================================

#include <AsmMacroIoLib.h>
#include <Library/PcdLib.h>
#include <AutoGen.h>

  INCLUDE AsmMacroIoLib.inc
  
  IMPORT  CEntryPoint
  IMPORT  _ReleaseInfo

  IMPORT  ArmDisableInterrupts
  IMPORT  ArmDisableCachesAndMmu
  IMPORT  ArmInvalidateInstructionCache
  IMPORT  ArmInvalidateTlb

  IMPORT  ArmInvalidateDataCache
  IMPORT  ArmEnableInstructionCache
  IMPORT  ArmEnableDataCache
  IMPORT  InitStackCanary
  
  EXPORT  _ModuleEntryPoint
        
  PRESERVE8
  AREA    ModuleEntryPoint, CODE, READONLY

  /* Entry Point */
_ModuleEntryPoint
  mov r0, #0

  /* First ensure all interrupts are disabled */
  bl ArmDisableInterrupts

  /* Ensure that the MMU and caches are off */
  bl ArmDisableCachesAndMmu
  
  /* Invalidate Instruction Cache and TLB */ 
  bl ArmInvalidateInstructionCache

  /* Setup stack */
  LoadConstantToReg (FixedPcdGet32(PcdPrePiStackBase) ,r0)    // stack base arg0  
  LoadConstantToReg (FixedPcdGet32(PcdPrePiStackSize) ,r1)    // stack size arg1  
  add     r4, r0, r1

  /* Enter SVC mode and set up SVC stack pointer */
  mov     r5,#0x13 :OR: 0x80 :OR: 0x40
  msr     CPSR_c,r5
  mov     r13,r4


  bl ArmInvalidateDataCache
  bl ArmInvalidateTlb

  /* Enable NEON register in case folks want to use them for optimizations (CopyMem) */
  mrc     p15, 0, r0, c1, c0, 2
  orr     r0, r0, #0x00f00000   // Enable VPF access (V* instructions)
  mcr     p15, 0, r0, c1, c0, 2
  mov     r0, #0x40000000       // Set EN bit in FPEXC
#if 0
  msr     FPEXC,r0
#endif     

  /* Fill vector table with branchs to current pc (jmp $) */
  LoadConstantToReg (FixedPcdGet32(PcdCpuVectorBaseAddress) ,r0)
  ldr     r1, ShouldNeverGetHere
  movs    r2, #0
FillVectors
  str     r1, [r0, r2]
  adds    r2, r2, #4
  cmp     r2, #32
  bne     FillVectors

  /* Write Vector base address to VBAR */
  LoadConstantToReg (FixedPcdGet32(PcdCpuVectorBaseAddress) ,r0)
  mcr     p15, 0, r0, c12, c0, 0
  isb                               /* Sync changes to control registers */

  bl ArmInvalidateDataCache
  bl ArmEnableInstructionCache
  bl ArmEnableDataCache

  // Initialize Stack Canary
  bl InitStackCanary
  
  /* before we call C code, lets setup the stack pointer in internal RAM */
  LoadConstantToReg (FixedPcdGet32(PcdPrePiStackBase) ,r0)    // stack base arg0  
  LoadConstantToReg (FixedPcdGet32(PcdPrePiStackSize) ,r1)    // stack size arg1

  /* Call C entry point */
  blx     CEntryPoint                                         /* Assume C code is thumb */

ShouldNeverGetHere
  /* _CEntryPoint should never return */
  b       ShouldNeverGetHere
  b       _ReleaseInfo
  END


