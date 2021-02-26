/*********************************************************************
 * @file OfflineCrashDumpLib.c
 *
 * @brief Offline Crash Dump support
 *
 * Copyright (c) 2013-2016 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

 when       who     what, where, why
 --------   ----    ---------------------------------------------------
 08/11/16   bh      Move cookie logic to new lib
 06/28/16   na      Remove second parameter from ClearDLOADCookieRuntime()
 05/27/15   ao      update dload cookie set/clr/isSet 
 10/22/14   aus     Added support for EDL mode
 10/03/14   na      Fixing size of input address for 64-bit
 08/15/14   sm      Switched to new SCM API
 04/22/14   aus     Call into TZ to write to the DLOAD cookie address
 04/10/14   niting  Fixed warnings
 03/12/14   aus     Mass storage mode support
 04/15/13   yg      Remove Stalls
 04/03/13   niting  Moved offline crash dump support into OfflineCrashDumpLib 
 03/23/13   niting  Initial revision.
========================================================================*/

#include <Uefi.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
GetMemoryCaptureMode(
  UINT32 *MemoryCaptureMode
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetMemoryCaptureModeRuntime(
  UINTN MemoryCaptureModeAddr,
  UINT32 *MemoryCaptureMode
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
SetMemoryCaptureMode(
  UINT32 Value
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SetMemoryCaptureModeRuntime(
  UINTN MemoryCaptureModeAddr,
  UINT32 Value
  )
{
  return EFI_SUCCESS;
}

BOOLEAN
EFIAPI
IsMemoryCaptureModeValid(
  UINT32 MemoryCaptureMode
  )
{
  return FALSE;
}

EFI_STATUS
EFIAPI
GetAbnormalResetOccurred(
  UINT32 *AbnormalResetOccurred
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
SetAbnormalResetOccurred(
  UINT32 Value
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SetAbnormalResetOccurredRuntime(
  UINTN AbnormalResetOccurredAddr,
  UINT32 Value
  )
{
  return EFI_SUCCESS;
}

BOOLEAN
EFIAPI
IsAbnormalResetOccurredValid(
  UINT32 AbnormalResetOccurred
  )
{
    return FALSE;
}

VOID
EFIAPI
PrintOfflineCrashDumpValues ( VOID )
{
}

