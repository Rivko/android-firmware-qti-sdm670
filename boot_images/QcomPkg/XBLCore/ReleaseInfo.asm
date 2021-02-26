//------------------------------------------------------------------------------ 
//
// Copyright (c) 2012-2013, Qualcomm Technologies Inc. All rights reserved.
//
//------------------------------------------------------------------------------

//=============================================================================
//                              EDIT HISTORY
//
//
// when       who     what, where, why
// --------   ---     ---------------------------------------------------------
// 10/23/13   niting  Updated UefiInfoBlockPtr location
// 12/10/12   yg      Add UefiInfoBlk and Format
// 11/02/12   vk      Initial revision
//
//============================================================================
  
#include <ReleaseInfo.h>
#define LINE_FEED  0x0A

  INCLUDE AsmMacroIoLib.inc
  EXPORT  _ReleaseInfo
  EXPORT  _UefiInfoBlockPtr
  EXPORT  _UefiRelInfoString
        
  IMPORT  _ModuleEntryPoint

  AREA    ReleaseInfo, CODE, READONLY, ALIGN=4

_ReleaseInfo
  b _ModuleEntryPoint

_UefiInfoBlockPtr
  DCD 0

_UefiRelInfoString
  DCB REL_LABEL_FIELD
  DCB REL_LABEL, LINE_FEED

  DCB REL_DATE_FIELD
  DCB REL_DATE,LINE_FEED

  DCB BUILD_VER_FIELD
  DCB BUILD_VER1, BUILD_VER2, BUILD_VER3, BUILD_VER4, LINE_FEED

  DCB BUILD_DATE_FIELD
  DCB BUILD_DATE, LINE_FEED

  DCB BUILD_TIME_FIELD
  DCB BUILD_TIME, LINE_FEED
  DCB 0
  
  ALIGN 16, 0

  END

