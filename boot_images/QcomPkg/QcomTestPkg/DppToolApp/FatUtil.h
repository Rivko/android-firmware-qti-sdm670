#ifndef __QCOM_DPP_FAT_UTIL_H_
#define __QCOM_DPP_FAT_UTIL_H_

/** @file FatUtil.h
   
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
#include <Protocol/SimpleFileSystem.h>

EFI_STATUS
FatWrite(
    IN EFI_HANDLE               PartHandle,
    IN CHAR16                   *FileName,
    IN VOID                     *Buffer,
    IN OUT UINTN                BufferSize
    );

EFI_STATUS
FatRead(
    IN EFI_HANDLE               PartHandle,
    IN CHAR16                   *FileName,
    OUT VOID                    **Buffer,
    OUT UINTN                   *BufferSize
    );

#endif
