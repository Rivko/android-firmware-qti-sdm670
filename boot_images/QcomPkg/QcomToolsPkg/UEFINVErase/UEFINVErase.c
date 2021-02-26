/** @file UEFINVErase.c
   
  UEFI NV Erase Application. 
  
  Copyright (c) 2012, 2015, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
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
 10/26/15   bh      Fix copyright info
 09/14/15   bh      Remove hard-coded block size (UFS support)
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 09/17/12   yg      Use ReadAnyKey API
 07/17/12   niting  Closed desync event upon success
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 05/17/12   niting  Initial Revision
 
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/BlockIo.h>
#include <Library/QcomLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

/* Set/Clear masked bits */
#define CLEAR_MASK(x,mask)                ((x) & (~(mask)))
#define SET_MASK(x,mask)                  ((x) | (mask))

extern EFI_GUID gUEFIBSNVVariableGPTPartitionGuid;

extern EFI_GUID gEfiEventFirmwareDesyncGuid;
STATIC EFI_EVENT FirmwareDesyncEvent = NULL;

/** 
   
  Does nothing. Just a placeholder

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
DisableSyncEventHandler (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  AsciiPrint("Disabling Syncing of variables\n");
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DisableSyncMain ( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  BOOLEAN EventCreated = FALSE;

  do
  {
    /* Create event to disable sync */
    AsciiPrint("Creating disable sync event\n");
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    DisableSyncEventHandler,
                    NULL,
                    &gEfiEventFirmwareDesyncGuid,
                    &FirmwareDesyncEvent
                    );
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Failed to create disable sync event\n");
      break;
    }

    EventCreated = TRUE;
    AsciiPrint("Signaling disable sync event\n");

    Status = gBS->SignalEvent(FirmwareDesyncEvent);

    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Failed to signal disable sync event\n");
      break;
    }

    AsciiPrint ("Successfully disabled syncing of UEFI variables.\n\n");  
  } while (0);

  if (EventCreated)
  {
    Status = gBS->CloseEvent(FirmwareDesyncEvent);
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Failed to close disable sync event\n");
    }
  }

  return Status;
}

/**
  Entry point for UEFI NV Erase. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS           Erase successfully.
  @retval EFI_DEVICE_ERROR      Failed to erase 
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for 
                                the buffers
**/
EFI_STATUS
EFIAPI
UEFINVEraseMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS               Status;
  UINT32                   Attribs = 0;
  EFI_BLOCK_IO_PROTOCOL    *BlkIo;
  UINT32                   MaxHandles, PartitionSize;
  PartiSelectFilter HandleFilter;
  HandleInfo        HandleInfoList[16];
  UINT32            HandleErrors = 0;
  UINT32                   *PartitionBuffer;

  do
  {
    Status = DisableSyncMain();
    if (EFI_ERROR (Status))
    {
      break;
    }

    Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  
    HandleFilter.RootDeviceType = NULL;
    if (HandleFilter.RootDeviceType)
      Attribs = SET_MASK(Attribs, BLK_IO_SEL_MATCH_ROOT_DEVICE);
    else
      Attribs = CLEAR_MASK(Attribs, BLK_IO_SEL_MATCH_ROOT_DEVICE);
  
    HandleFilter.PartitionType = &gUEFIBSNVVariableGPTPartitionGuid;
    if (HandleFilter.PartitionType)
      Attribs = SET_MASK(Attribs, (BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID | BLK_IO_SEL_PARTITIONED_GPT));
    else
      Attribs = CLEAR_MASK(Attribs, (BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID | BLK_IO_SEL_PARTITIONED_GPT));
  
    MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  
    AsciiPrint ("Searching for UEFI Boot Services NV partition.\n\n");  
    Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
  
    if ((Status != EFI_SUCCESS) || (MaxHandles == 0))
      break;
  
    if (MaxHandles > 1)
    {
      AsciiPrint ("ERROR: Found more than one matching partition handle.\n");
      Status = EFI_INVALID_PARAMETER;
      HandleErrors++;
      break;
    }
  
    AsciiPrint ("UEFI Boot Services NV partition found.\n\n");  
    BlkIo = HandleInfoList[0].BlkIo;
  
    PartitionSize = (UINT32)(BlkIo->Media->LastBlock + 1) * BlkIo->Media->BlockSize;
    PartitionBuffer = AllocatePool (PartitionSize);
  
    if (PartitionBuffer == NULL)
    {  
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }
  
    /* Clear partition buffer */
    SetMem(PartitionBuffer, PartitionSize, 0x00);

    AsciiPrint ("Attempting to clear UEFI Boot Services NV partition.\n\n");  
    Status = BlkIo->WriteBlocks (BlkIo, BlkIo->Media->MediaId, 0, 
                                 PartitionSize, PartitionBuffer);
    if (EFI_ERROR (Status))
    {
      FreePool (PartitionBuffer);
      AsciiPrint ("ERROR: Could not write to partition.\n");
      Status = EFI_DEVICE_ERROR;
      break;
    }

    FreePool (PartitionBuffer);

    AsciiPrint ("Successfully cleared UEFI Boot Services NV partition.\n\n");  

  } while (0);
  
  AsciiPrint ("\r\nPress any key to exit...\n");
  ReadAnyKey (NULL, 0);

  return Status;
}

