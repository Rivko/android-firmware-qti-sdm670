/** @file ResetRuntimeDxe.c
   
  Runtime DXE to support Reset Runtime Architecture Protocol.

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Portions Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.

  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/25/16   vk      Add clear API
 03/22/16   vk      Move reset reason protocol implementation to lib
 01/28/16   vk      Add reset reason protocol
 09/18/15   jb      Added serial port flush code on reset
 04/03/13   niting  Added status check for reset initialization failure
 08/30/11   niting  Initial revision.

=============================================================================*/
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/EfiResetSystemLib.h>
#include <Library/SerialPortLib.h>
#include <Library/SerialPortShLib.h>
#include <Library/UefiRuntimeLib.h>

#include <Protocol/Reset.h>
#include <Protocol/EFIResetReason.h>

/**
  Resets the entire platform.

  @param  ResetType             The type of reset to perform.
  @param  ResetStatus           The status code for the reset.
  @param  DataSize              The size, in bytes, of WatchdogData.
  @param  ResetData             For a ResetType of EfiResetCold, EfiResetWarm, or
                                EfiResetShutdown the data buffer starts with a Null-terminated
                                Unicode string, optionally followed by additional binary data.

**/
VOID
EFIAPI
ResetSystemViaLib (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN VOID             *ResetData OPTIONAL
  )
{
  if (!EfiAtRuntime())
  {
    /* Flush the buffer to serial port and enable
	   * synchronous transfer */
    SerialPortFlush ();
    EnableSynchronousSerialPortIO ();
    gBS->Stall(300); /* 300 uSeconds*/
  }
  LibResetSystem (ResetType, ResetStatus, DataSize, ResetData);
  return;
}

STATIC
EFI_STATUS
EFI_Get_Reset_Reason(
   IN EFI_RESETREASON_PROTOCOL *This,
   IN OUT UINT32               *ResetReason,
   IN OUT CHAR16               *ResetReasonString,
   IN OUT UINT32               *ResetReasonStrLen
   )
{
  EFI_STATUS Status;
  Status = LibGetResetReason (ResetReason, ResetReasonString, ResetReasonStrLen);
  return Status;
}

STATIC
EFI_STATUS
EFI_Clear_Reset_Reason(
   IN EFI_RESETREASON_PROTOCOL *This
   )
{
  EFI_STATUS Status;
  Status = LibClearResetReason ();
  return Status;
}

STATIC
EFI_RESETREASON_PROTOCOL ResetReasonProtocol =
{
  EFI_RESETREASON_PROTOCOL_REVISION,
  EFI_Get_Reset_Reason,
  EFI_Clear_Reset_Reason
};


EFI_STATUS
EFIAPI
InitializeReset (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  Status = LibInitializeResetSystem (ImageHandle, SystemTable);
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Failed to initialize reset\r\n"));
    return Status;
  }
  
  SystemTable->RuntimeServices->ResetSystem = ResetSystemViaLib;

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiResetArchProtocolGuid,
                  NULL,
                  NULL
                  );

   Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiResetReasonProtocolGuid, (VOID**) &ResetReasonProtocol, 
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

