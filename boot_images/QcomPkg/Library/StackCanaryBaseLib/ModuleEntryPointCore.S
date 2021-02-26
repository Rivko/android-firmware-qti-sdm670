//------------------------------------------------------------------------------ 
//
//  Copyright (c) 2015-2016, Qualcomm Technologies Inc. All rights reserved.
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
// 06/28/16   vk      Enable TZ call
// 10/27/15   vk      Initial revision
//
//============================================================================


.text
.align 3

GCC_ASM_IMPORT (_ModuleEntryPoint)
GCC_ASM_IMPORT (InitStackCanary)

GCC_ASM_EXPORT (_QCModuleEntry)

ASM_PFX(_QCModuleEntry):
  cmp x0, #0
  beq entry_point
  
  stp x3,  x2,  [sp, #-0x10]!
  stp x1,  x0,  [sp, #-0x10]!
  stp x9,  x8,  [sp, #-0x10]!
  stp x30, x29, [sp, #-0x10]!
  bl InitStackCanary
  ldp x30, x29, [SP],#0x10
  ldp x9,  x8,  [sp],#0x10
  ldp x1,  x0,  [SP],#0x10
  ldp x3,  x2,  [SP],#0x10

ASM_PFX(entry_point): 
  b _ModuleEntryPoint
 
