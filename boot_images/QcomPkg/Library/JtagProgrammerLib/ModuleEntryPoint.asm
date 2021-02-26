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
// 2014/11/06 bn      Initial revision
//
//=============================================================================

  IMPORT  jsdcc_init
  EXPORT  _JtagProgrammerEntryPoint
        
  PRESERVE8
  AREA    JtagProgrammerEntryPoint, CODE, READONLY
  

_JtagProgrammerEntryPoint

  b       jsdcc_init

ShouldNeverGetHere
  /* should never return */
  b       ShouldNeverGetHere
  END

