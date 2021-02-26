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
 06/28/16   na      Change all mass storage references to dload
 06/15/16   bd      Remove offlinecrashdump library dependency from QcomLib 
 06/08/16   sj      removed the mass storage cookie references
 07/10/15   ao      changed isDloadCookieSet() to use TzFastcall() 
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
#include <Library/UefiCfgLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/OfflineCrashDump.h>
#include <Library/SerialPortShLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TzRuntimeLib.h>
#include <Protocol/EFIScm.h>
#include <Include/scm_sip_interface.h>
#include <Library/BaseLib.h>
#include <Library/CookieLib.h>

EFI_STATUS
EFIAPI
GetMemoryCaptureMode(
  UINT32 *MemoryCaptureMode
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 SharedIMEMBaseAddr = 0;
  UINT32 MemoryCaptureModeOffset = 0;
  volatile UINT32* mMemoryCaptureMode = NULL;
  if (MemoryCaptureMode == NULL)
    return EFI_INVALID_PARAMETER;

  Status = GetConfigValue ("SharedIMEMBaseAddr", &SharedIMEMBaseAddr);
  if ((Status != EFI_SUCCESS) || (SharedIMEMBaseAddr == 0))
  {
    DEBUG ((EFI_D_ERROR, "SharedIMEMBaseAddr not found in uefiplat.cfg\r\n"));
    return Status;
  }

  Status = GetConfigValue ("MemoryCaptureModeOffset", &MemoryCaptureModeOffset);
  if ((Status != EFI_SUCCESS) || (MemoryCaptureModeOffset == 0))
  {
    DEBUG ((EFI_D_ERROR, "MemoryCaptureModeOffset not found in uefiplat.cfg\r\n"));
    return Status;
  }

  mMemoryCaptureMode = (UINT32* )((UINTN)SharedIMEMBaseAddr + MemoryCaptureModeOffset);
  *MemoryCaptureMode = *mMemoryCaptureMode;
  return Status;
}

EFI_STATUS
EFIAPI
GetMemoryCaptureModeRuntime(
  UINTN MemoryCaptureModeAddr,
  UINT32 *MemoryCaptureMode
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  if ((MemoryCaptureMode == NULL) || (MemoryCaptureModeAddr == 0))
    return EFI_INVALID_PARAMETER;

  *MemoryCaptureMode = MmioRead32(MemoryCaptureModeAddr);
  return Status;
}

EFI_STATUS
SetMemoryCaptureMode(
  UINT32 Value
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 SharedIMEMBaseAddr = 0;
  UINT32 MemoryCaptureModeOffset = 0;
  volatile UINT32* MemoryCaptureMode = NULL;

  Status = GetConfigValue ("SharedIMEMBaseAddr", &SharedIMEMBaseAddr);
  if ((Status != EFI_SUCCESS) || (SharedIMEMBaseAddr == 0))
  {
    DEBUG ((EFI_D_ERROR, "SharedIMEMBaseAddr not found in uefiplat.cfg\r\n"));
    return Status;
  }

  Status = GetConfigValue ("MemoryCaptureModeOffset", &MemoryCaptureModeOffset);
  if ((Status != EFI_SUCCESS) || (MemoryCaptureModeOffset == 0))
  {
    DEBUG ((EFI_D_ERROR, "MemoryCaptureModeOffset not found in uefiplat.cfg\r\n"));
    return Status;
  }

  MemoryCaptureMode = (UINT32* )((UINTN)SharedIMEMBaseAddr + MemoryCaptureModeOffset);
  *MemoryCaptureMode = Value;
  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SetMemoryCaptureModeRuntime(
  UINTN MemoryCaptureModeAddr,
  UINT32 Value
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (MemoryCaptureModeAddr == 0)
    return EFI_INVALID_PARAMETER;

  MmioWrite32(MemoryCaptureModeAddr, Value);
  
  return Status;
}

BOOLEAN
EFIAPI
IsMemoryCaptureModeValid(
  UINT32 MemoryCaptureMode
  )
{
  if ((MemoryCaptureMode == OFFLINE_CRASH_DUMP_DISABLE) || 
      (MemoryCaptureMode == OFFLINE_CRASH_DUMP_LEGACY_ENABLE) || 
      (MemoryCaptureMode == OFFLINE_CRASH_DUMP_ENABLE))
    return TRUE;
  else
    return FALSE;
}

EFI_STATUS
EFIAPI
GetAbnormalResetOccurred(
  UINT32 *AbnormalResetOccurred
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 SharedIMEMBaseAddr = 0;
  UINT32 AbnormalResetOccurredOffset = 0;
  volatile UINT32* mAbnormalResetOccurred = NULL;

  if (AbnormalResetOccurred == NULL)
    return EFI_INVALID_PARAMETER;

  Status = GetConfigValue ("SharedIMEMBaseAddr", &SharedIMEMBaseAddr);
  if ((Status != EFI_SUCCESS) || (SharedIMEMBaseAddr == 0))
  {
    DEBUG ((EFI_D_ERROR, "SharedIMEMBaseAddr not found in uefiplat.cfg\r\n"));
    return Status;
  }

  Status = GetConfigValue ("AbnormalResetOccurredOffset", &AbnormalResetOccurredOffset);
  if ((Status != EFI_SUCCESS) || (AbnormalResetOccurredOffset == 0))
  {
    DEBUG ((EFI_D_ERROR, "AbnormalResetOccurredOffset not found in uefiplat.cfg\r\n"));
    return Status;
  }

  mAbnormalResetOccurred = (UINT32* )((UINTN)SharedIMEMBaseAddr + AbnormalResetOccurredOffset);
  *AbnormalResetOccurred = *mAbnormalResetOccurred;

  return Status;
}

EFI_STATUS
SetAbnormalResetOccurred(
  UINT32 Value
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 SharedIMEMBaseAddr = 0;
  UINT32 AbnormalResetOccurredOffset = 0;
  volatile UINT32* AbnormalResetOccurred = NULL;

  Status = GetConfigValue ("SharedIMEMBaseAddr", &SharedIMEMBaseAddr);
  if ((Status != EFI_SUCCESS) || (SharedIMEMBaseAddr == 0))
  {
    DEBUG ((EFI_D_ERROR, "SharedIMEMBaseAddr not found in uefiplat.cfg\r\n"));
    return Status;
  }

  Status = GetConfigValue ("AbnormalResetOccurredOffset", &AbnormalResetOccurredOffset);
  if ((Status != EFI_SUCCESS) || (AbnormalResetOccurredOffset == 0))
  {
    DEBUG ((EFI_D_ERROR, "AbnormalResetOccurredOffset not found in uefiplat.cfg\r\n"));
    return Status;
  }

  AbnormalResetOccurred = (UINT32* )((UINTN)SharedIMEMBaseAddr + AbnormalResetOccurredOffset);
  *AbnormalResetOccurred = Value;
  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SetAbnormalResetOccurredRuntime(
  UINTN AbnormalResetOccurredAddr,
  UINT32 Value
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (AbnormalResetOccurredAddr == 0)
    return EFI_INVALID_PARAMETER;
 
  MmioWrite32(AbnormalResetOccurredAddr, Value);

  return Status;
}

BOOLEAN
EFIAPI
IsAbnormalResetOccurredValid(
  UINT32 AbnormalResetOccurred
  )
{
  if ((AbnormalResetOccurred == ABNORMAL_RESET_DISABLE) || (AbnormalResetOccurred == ABNORMAL_RESET_ENABLE))
    return TRUE;
  else
    return FALSE;
}

VOID
EFIAPI
PrintOfflineCrashDumpValues ( VOID )
{
#ifdef ENABLE_OCD_DEBUG
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 MemoryCaptureMode = 0;
  UINT32 AbnormalResetOccurred = 0;
  BOOLEAN DLOADCookieSet = FALSE;

  GetMemoryCaptureMode(&MemoryCaptureMode);

  GetAbnormalResetOccurred(&AbnormalResetOccurred);

  DLOADCookieSet = IsDLOADCookieSet();

  DEBUG((EFI_D_ERROR, "************Printing Offline Crash Dump Values**************\r\n"));

  DEBUG((EFI_D_ERROR, "  Offline Crash Dump Values:\r\n"));

  Status = GetMemoryCaptureMode(&MemoryCaptureMode);
  if (Status == EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "    MemoryCaptureMode: 0x%08x\r\n", MemoryCaptureMode));
  }

  Status = GetAbnormalResetOccurred(&AbnormalResetOccurred);
  if (Status == EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "    AbnormalResetOccurred: 0x%08x\r\n", AbnormalResetOccurred));
  }

  DLOADCookieSet = IsDLOADCookieSet();
  DEBUG((EFI_D_ERROR, "    Is DLOADCookieSet?: %a\r\n", (DLOADCookieSet == TRUE) ? "Set" : "Cleared" ));
  DEBUG((EFI_D_ERROR, "************************************************************\r\n"));
#endif
}

