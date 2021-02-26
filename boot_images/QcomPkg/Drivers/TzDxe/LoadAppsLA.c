/** @file LoadAppsLA.c
  Implementation of modules to load hlos specific applications 

  Copyright (c) 2011-2016, Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 07/12/16   rj       Created the file. Moved function from TzeLoader.c
=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Library/QcomLib.h>
#include <Library/UefiLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/TzRuntimeLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/EfiFileLib.h>
#include <Library/ArmLib.h>
#include <Library/PcdLib.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include "QcomLib.h"
#include <Protocol/EFITzeLoader.h>
#include <Library/QcomBaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/EFIScm.h>
#include <Include/scm_sip_interface.h>
#include <Include/scm_qsee_interface.h>
#include <Library/UefiCfgLib.h>
#include <Library/BootConfig.h>
#include <TzeLoaderDxe.h>
#include <QseeComDxe.h>


  /**
  Loads all Secure Apps at bootup
  
  @param  Path            Directory containging ACPI table files
  @param  pPartitionGuid  GUID eg:  TzAppsPartitionType
  @param AppIdPtr         out  if success holds app id on return
   
  @return EFI_SUCCESS if successful
**/
 
EFI_STATUS
LoadSecureApps(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN EFI_HANDLE ImageHandle,
   IN EFI_SYSTEM_TABLE *SystemTable,
   OUT UINT32                         *AppIdPtr
)
{
  EFI_STATUS              Status = EFI_SUCCESS;

  // init QSEECOM protocol
  Status = QseeComProtocolInit(ImageHandle, SystemTable);
  if (Status != EFI_SUCCESS)
  {
    return Status;
  }
  return Status;
}
