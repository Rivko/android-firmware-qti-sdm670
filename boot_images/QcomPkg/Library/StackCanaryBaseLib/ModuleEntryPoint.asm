//------------------------------------------------------------------------------ 
//
// Copyright (c) 2015, Qualcomm Technologies Inc. All rights reserved.
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
// 08/28/15   aus     Initial revision
//
//=============================================================================

#include <AsmMacroIoLib.h>
#include <Library/PcdLib.h>
#include <AutoGen.h>
 
  INCLUDE AsmMacroIoLib.inc
 
  IMPORT  _ModuleEntryPoint
  IMPORT  InitStackCanary
  EXPORT  _QCModuleEntry
       
  PRESERVE8
  AREA    QCModuleEntry, CODE, READONLY
 
// Entry Point 
_QCModuleEntry
  // If r0 is NULL, it is SEC stage. Stack canary is initialized after stack is setup
  cmp r0, #0
  beq entry_point

  // Save and restore passed parameters before initialization of stack canary
  push {r0-r3,r14}
  bl InitStackCanary 
  pop {r0-r3,r14}

entry_point
  b _ModuleEntryPoint
END

