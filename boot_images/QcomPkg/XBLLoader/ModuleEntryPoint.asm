//------------------------------------------------------------------------------ 
//
// Copyright (c) 2014, Qualcomm Technologies Inc. All rights reserved.
//
//------------------------------------------------------------------------------

//=============================================================================
//                              EDIT HISTORY
//
//
// when       who     what, where, why
// --------   ---     ---------------------------------------------------------
// 05/02/14   kedara  Added sbl1_entry api
// 03/10/14   niting  Initial revision
//
//=============================================================================
#include <Library/PcdLib.h>
#include <AutoGen.h>

  IMPORT  sbl1_entry
  EXPORT  _ModuleEntryPoint
        
  PRESERVE8
  AREA    ModuleEntryPoint, CODE, READONLY
  

_ModuleEntryPoint

  b      sbl1_entry

ShouldNeverGetHere
  /* sbl1_entry should never return */
  b       ShouldNeverGetHere
  END

