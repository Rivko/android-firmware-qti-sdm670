/** @file CprhDxe.c

  This file implements CPRH EFI protocol interface.

  Copyright (c) 2017-2018, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  reserved. 

**/

/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include <Library/CPR.h>

/*=========================================================================
      Globals and prototypes
==========================================================================*/

/**
 * Entry point for the CPR DXE driver.
 */
EFI_STATUS EFIAPI CPRDxeEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS  Status = EFI_SUCCESS;
 
  DALSYS_InitMod(NULL);
      
  //To enable aging calibration for GPU CPR, we need to enable GPU power domain.
  //using clock protocol to get GPU power domain info
  cpr_get_gpu_power_domain_info();
	
  //Initialize CPR
  cpr_init();

  return Status;
}
