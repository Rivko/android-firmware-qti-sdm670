#ifndef RPMH_UEFI_INTERNAL_H
#define RPMH_UEFI_INTERNAL_H

/*===========================================================================
                             rpmh_uefi_internal.h

DESCRIPTION:
  Interface for interacting with TCSs. This includes managing TCS state, 
  configuring TCSs, and handling TCS completion.  

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>




//CopyMem is overlap-safe in UEFI:
#define memmove(dst, src, size) CopyMem(dst, src, size)

#endif // #define RPMH_UEFI_INTERNAL_H

