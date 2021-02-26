/** @file PdcDxe.c

  This file inits PDC driver.

  Copyright (c) 2017, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  reserved.

**/

/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PdcLib.h>


/*=========================================================================
      Globals and prototypes
==========================================================================*/

/**
 * Entry point for the Pdc DXE driver.
 */
EFI_STATUS EFIAPI PdcDxeEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS status;
  /* Initalize PDC */
  status = PdcTargetInit();

  return status;
}