#ifndef __QCOM_DPP_DPP_UTIL_H_
#define __QCOM_DPP_DPP_UTIL_H_
/** @file DppUtil.h
   
  Qualcomm Dpp tool application

  Copyright (c) 2010-2013, Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/16/13   mic     Initial version
=============================================================================*/
#include <Uefi.h>
#include <Protocol/EFIDpp.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include "../../Drivers/DppDxe/Dpp.h"

EFI_STATUS 
DppGvariableInit(VOID);

EFI_STATUS
DppItemRead (
  IN  CHAR16       *Type,
  IN  CHAR16       *Name,
  OUT UINT8       **Buff,
  OUT UINT32       *Size
);

EFI_STATUS
DppItemWrite (
  IN  CHAR16       *Type,
  IN  CHAR16       *Name,
  IN UINT8         *Buff,
  IN UINT32         Size
);

EFI_STATUS
DppItemDelete (
  IN  CHAR16       *Type,
  IN  CHAR16       *Name
);

EFI_STATUS
DppErase (void);

EFI_STATUS
ListAllDppItems(VOID);

#endif
