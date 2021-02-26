/** @file PmicRuntimeLib.c
   
  Library implementation to support SBI Runtime call.

  Copyright (c) 2013, Qualcomm Technologies, Inc. All rights
  reserved. 
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/26/14    al      Updating file and making library common
  10/02/13   al      Initial revision.

=============================================================================*/


#include <Uefi.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include "SpmiBus.h"
#include "SpmiBus_Runtime.h"
#include "pm_version.h"
#include "device_info.h"
#include "pm_comm.h"

/**
  Initialize any infrastructure required for Lib to function.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_STATUS    Returns the status of PmicRuntimeLibInitialize.

**/
EFI_STATUS
EFIAPI
PmicRuntimeLibInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
   EFI_STATUS Status = EFI_SUCCESS; 
   /* Initialize the runtime library */
   Status  = SPMIRuntimeLibInitialize(ImageHandle, SystemTable);
   
   /*err_flag = pm_version_detect();*/

   return Status;
   
}

