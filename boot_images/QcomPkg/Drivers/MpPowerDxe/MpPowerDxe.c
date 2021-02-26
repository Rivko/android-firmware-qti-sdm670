/** @file
  Implements the multi-processor power support

  Copyright (c) 2015-17, Qualcomm Technologies,Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Drivers/MpPowerDxe/MpPowerDxe.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

=============================================================================*/

#include <Uefi.h>
#include <Guid/EventGroup.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/DebugLib.h>

#include <Library/MpPowerLib.h>


/** Globals **/
UINT32    NumCpus =  0;
BOOLEAN   PsciCompliant = FALSE;

EFI_STATUS
MpPowerInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTablef
  )
{
  EFI_STATUS  Status;
  
  Status = GetConfigValue ("NumActiveCores", &NumCpus);
  if ((Status != EFI_SUCCESS) || (NumCpus == 0))
  {
    Status = GetConfigValue ("NumCpus", &NumCpus);
    if ((Status != EFI_SUCCESS) || (NumCpus == 0))
    {
      DEBUG ((EFI_D_WARN, "NumCpus not found in uefiplat.cfg. Defaulting to 1.\r\n"));
      // Default to 1
      NumCpus = 1;
    }
  }

  Status = MpPowerInit();
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "Chipset-specific initialization failed.\r\n"));
    return Status;
  }

  /* Multicluster enablement */
  Status = MpPower(NumCpus);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "Chipset-specific Exit BS initialization failed.\r\n"));
    return Status;
  }

  return EFI_SUCCESS;

}
