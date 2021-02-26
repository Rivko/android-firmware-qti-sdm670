//------------------------------------------------------------------------------ 
//
// Copyright (c) 2014, Qualcomm Technologies Inc. All rights reserved.
//
//------------------------------------------------------------------------------

//=============================================================================
//                              EDIT HISTORY
//
// when       who     what, where, why
// --------   ---     ---------------------------------------------------------
// 03/10/14   niting  Initial revision
//
//=============================================================================
#include <Library/PcdLib.h>
#include <AutoGen.h>

  IMPORT  XBLRamDumpMain
  EXPORT  _ModuleEntryPoint
        
  PRESERVE8
  AREA    ModuleEntryPoint, CODE, READONLY
  

_ModuleEntryPoint

  b       XBLRamDumpMain

ShouldNeverGetHere
  /* sbl1_entry should never return */
  b       ShouldNeverGetHere
  END

