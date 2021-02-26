/** @file OSConfigDxe.c
  Installs required OS Configuration Data in System Table

  Copyright (c) 2011, 2014 Qualcomm Technologies, Inc. All rights reserved.<BR>

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/09/14   vanim   Add support for updating RFS entries in DSDT.
 06/30/11   niting  Initial version.

=============================================================================*/
#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/RFSLib.h>

typedef struct _OS_PLATFORM_TABLE {
    UINT32 Version;
    CHAR16 ConfigItem0[32]; // Null Terminated String
    CHAR16 ConfigItem1[32]; // Null Terminated String
} OS_PLATFORM_TABLE;


extern EFI_GUID gEfiOSConfigGuid;

CONST CHAR16* ConfigItem0 = (CHAR16*)PcdGetPtr (PcdOSConfigItem0);
CONST CHAR16* ConfigItem1 = (CHAR16*)PcdGetPtr (PcdOSConfigItem1);

/**
  Initialize the OS Configuration Data

  @param  ImageHandle   of the loaded driver
  @param  SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS           Protocol registered
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      Hardware problems

**/
EFI_STATUS
OSConfigDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  OS_PLATFORM_TABLE  *OSConfigTable;

  //
  // Allocate OS Configuration Table.
  //
  OSConfigTable = AllocateZeroPool (sizeof (OS_PLATFORM_TABLE));
  if (OSConfigTable == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  //
  // Update table values.
  //
  OSConfigTable->Version = 0;
  CopyMem (OSConfigTable->ConfigItem0,
           ConfigItem0,
           sizeof(CHAR16)*StrLen(ConfigItem0));
  CopyMem (OSConfigTable->ConfigItem1,
           ConfigItem1,
           sizeof(CHAR16)*StrLen(ConfigItem1));

  //
  // Install the Table.
  //
  Status = gBS->InstallConfigurationTable (&gEfiOSConfigGuid, OSConfigTable);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }

  /* Call the RFS library function to register setVariableCallback functions
   * for updating AML variables in DSDT.
   */
  Status = RFSLib_AMLVariableRegister ();

Error:
  return Status;
}

