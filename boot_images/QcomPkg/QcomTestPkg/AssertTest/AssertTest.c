/** @file
   
  ASSERT() to test crashdump 

  Copyright (c) 2016 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/21/16   ai      Initial version 

=============================================================================*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  /* Start 30sec watchdog */
  gBS->SetWatchdogTimer(30, 0, 0, NULL);

  ASSERT(0==1);

  return EFI_SUCCESS;
}
