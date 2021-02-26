/** @file QcomFwUpdateApp.c
   
  Main Dispatcher for firmware updates of Windows-on-Snapdragon
  (WoS) 

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved. 
   

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/07/02    Mic         moved from OSAppPkg to QComPkg

=============================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FwCommonLib.h>
#include <Uefi/UefiBaseType.h>

EFI_STATUS
EmergencyDownloadMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{

  EFI_STATUS Status ;
  Status = InvalidateAllPrimaryGPT(TRUE, TRUE) ;

  if(EFI_ERROR(Status))
  {   
    return Status;
  }

  Status = InvalidateAllBackUpGPT(TRUE, TRUE);
        
  return Status;
    
}
