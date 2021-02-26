/** @file
  Capsule Library instance that implements firmware update.

  Copyright (c) 2012,2015, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  Portions Copyright (c) 2007 - 2013, Intel Corporation. All rights reserved.
  This program and the accompanying materials are licensed and made available 
  under the terms and conditions of the BSD License which accompanies this 
  distribution.  The full text of the license may be found at 
  http://opensource.org/licenses/bsd-license.php 

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/05/15   jb      Add FMP Support
 07/26/12   niting  Reset age of CapsuleGuidTable if any updates are made
 03/17/12   niting  Fixed deletion of capsules by only allowing one outstanding 
                    GUID event at anytime and deleting others as soon as 
                    another is installed
 03/13/12   niting  Initial revision

=============================================================================*/
#include <Uefi.h>
#include <Library/CapsuleLib.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Guid/FmpCapsule.h>
#include <Protocol/FirmwareManagement.h>
#include <Library/GenericBdsLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePath.h>
#include <Library/MemoryAllocationLib.h>
#include "CommonHeader.h"

/**
 * This table stores the list of persisted capsules that would be removed
 * once they have been consumed successfully.
 */
typedef struct {
  /// 
  /// Event corresponding to the GUID
  /// 
  EFI_EVENT           Event;

  /// 
  /// GUID value for the entry
  /// 
  EFI_GUID            Guid;

  /// 
  /// Keeps track of how many repeated entries are present.
  /// This will allow if multiple capsules with the same GUID are present
  /// and allow for each of them to be deleted.
  /// 
  UINT32              EntryCount;

  /// 
  /// Capsule flags corresponding to the GUID
  /// 
  UINT32              CapsuleFlags;

  ///
  /// Capsule Offset in storage
  ///
  EFI_LBA             CapsuleLBAOffset;

}EVENT_GUID_ENTRY;

/**
 * This table stores the list of persisted capsules that would be removed
 * once they have been consumed successfully.
 */
typedef struct {
  ///
  /// Number of valid event GUID entries
  ///
  UINT32              GuidCount;

  ///
  /// Entries for each Guid in the Capsule Guid Table.
  /// GuidCount in Header determines the number of valid entries
  /// in this array.
  ///
  EVENT_GUID_ENTRY    GuidEntries[CAPSULE_GUID_ENTRIES_MAX];

} EVENT_GUID_TABLE;

/* Stores the events to signal when capsules are consumed */
STATIC EVENT_GUID_TABLE EventGuidTable = {0};
EFI_BLOCK_IO_PROTOCOL *CapsuleBlkIo = NULL;
BOOLEAN CapsulePartitionFlag = FALSE;

/**
  Function indicate the current completion progress of the firmware
  update. Platform may override with own specific progress function.

  @param  Completion    A value between 1 and 100 indicating the current completion progress of the firmware update

  @retval EFI_SUCESS    Input capsule is a correct FMP capsule.
**/
EFI_STATUS
EFIAPI
Update_Image_Progress (
   IN UINTN Completion
)
{
  return EFI_SUCCESS;
}


/**
  Validate Fmp capsules layout.

  @param  CapsuleHeader    Points to a capsule header.

  @retval EFI_SUCESS                     Input capsule is a correct FMP capsule.
  @retval EFI_INVALID_PARAMETER  Input capsule is not a correct FMP capsule.
**/
EFI_STATUS
ValidateFmpCapsule (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER       *FmpCapsuleHeader;
  UINT8                                        *EndOfCapsule;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *ImageHeader;
  UINT8                                        *EndOfPayload;
  UINT64                                       *ItemOffsetList;
  UINT32                                       ItemNum;
  UINTN                                        Index;

  FmpCapsuleHeader = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER *) ((UINT8 *) CapsuleHeader + CapsuleHeader->HeaderSize);
  EndOfCapsule     = (UINT8 *) CapsuleHeader + CapsuleHeader->CapsuleImageSize;

  if (FmpCapsuleHeader->Version > EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION) {
    return EFI_INVALID_PARAMETER;
  }
  ItemOffsetList = (UINT64 *)(FmpCapsuleHeader + 1);

  ItemNum = FmpCapsuleHeader->EmbeddedDriverCount + FmpCapsuleHeader->PayloadItemCount;

  if (ItemNum == FmpCapsuleHeader->EmbeddedDriverCount) {
    //
    // No payload element 
    //
    if (((UINT8 *)FmpCapsuleHeader + ItemOffsetList[ItemNum - 1]) < EndOfCapsule) {
      return EFI_SUCCESS;
    } else {
      return EFI_INVALID_PARAMETER;
    }
  }

  if (FmpCapsuleHeader->PayloadItemCount != 0) {
    //
    // Check if the last payload is within capsule image range
    //
    ImageHeader  = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *)((UINT8 *)FmpCapsuleHeader + ItemOffsetList[ItemNum - 1]);
    EndOfPayload = (UINT8 *)(ImageHeader + 1) + ImageHeader->UpdateImageSize + ImageHeader->UpdateVendorCodeSize;
  } else {
    //
    // No driver & payload element in FMP
    //
    EndOfPayload = (UINT8 *)(FmpCapsuleHeader + 1);
  }

  if (EndOfPayload != EndOfCapsule) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // All the address in ItemOffsetList must be stored in ascending order
  //
  if (ItemNum >= 2) {
    for (Index = 0; Index < ItemNum - 1; Index++) {
      if (ItemOffsetList[Index] >= ItemOffsetList[Index + 1]) {
        return EFI_INVALID_PARAMETER;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Process Firmware management protocol data capsule.  

  @param  CapsuleHeader         Points to a capsule header.

  @retval EFI_SUCESS            Process Capsule Image successfully.
  @retval EFI_UNSUPPORTED       Capsule image is not supported by the firmware.
  @retval EFI_VOLUME_CORRUPTED  FV volume in the capsule is corrupted.
  @retval EFI_OUT_OF_RESOURCES  Not enough memory.
**/
EFI_STATUS
ProcessFmpCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  EFI_STATUS                                    Status;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER        *FmpCapsuleHeader;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER  *ImageHeader;
  EFI_HANDLE                                    ImageHandle;
  UINT64                                        *ItemOffsetList;
  UINT32                                        ItemNum;
  UINTN                                         Index;
  UINTN                                         ExitDataSize;
  EFI_HANDLE                                    *HandleBuffer;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL              *Fmp;
  UINTN                                         NumberOfHandles;
  UINTN                                         DescriptorSize;
  UINT8                                         FmpImageInfoCount;
  UINT32                                        FmpImageInfoDescriptorVer;
  UINTN                                         ImageInfoSize;
  UINT32                                        PackageVersion;
  CHAR16                                        *PackageVersionName;
  CHAR16                                        *AbortReason;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *FmpImageInfoBuf;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *TempFmpImageInfo;
  UINTN                                         DriverLen;
  UINTN                                         Index1;
  UINTN                                         Index2;
  MEMMAP_DEVICE_PATH                            MemMapNode;
  EFI_DEVICE_PATH_PROTOCOL                      *DriverDevicePath;

  Status           = EFI_SUCCESS;
  HandleBuffer     = NULL;
  ExitDataSize     = 0;
  DriverDevicePath = NULL;

  FmpCapsuleHeader = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER *) ((UINT8 *) CapsuleHeader + CapsuleHeader->HeaderSize);

  if (FmpCapsuleHeader->Version > EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION) {
    return EFI_INVALID_PARAMETER;
  }
  ItemOffsetList = (UINT64 *)(FmpCapsuleHeader + 1);

  ItemNum = FmpCapsuleHeader->EmbeddedDriverCount + FmpCapsuleHeader->PayloadItemCount;

  //
  // capsule in which driver count and payload count are both zero is not processed.
  //
  if (ItemNum == 0) {
    return EFI_SUCCESS;
  }

  //
  // 1. ConnectAll to ensure 
  //    All the communication protocol required by driver in capsule installed 
  //    All FMP protocols are installed
  //
  BdsLibConnectAll();


  //
  // 2. Try to load & start all the drivers within capsule 
  //
  SetDevicePathNodeLength (&MemMapNode.Header, sizeof (MemMapNode));
  MemMapNode.Header.Type     = HARDWARE_DEVICE_PATH;
  MemMapNode.Header.SubType  = HW_MEMMAP_DP;
  MemMapNode.MemoryType      = EfiBootServicesCode;
  MemMapNode.StartingAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)CapsuleHeader;
  MemMapNode.EndingAddress   = (EFI_PHYSICAL_ADDRESS)(UINTN)((UINT8 *)CapsuleHeader + CapsuleHeader->CapsuleImageSize - 1);

  DriverDevicePath = AppendDevicePathNode (NULL, &MemMapNode.Header);
  if (DriverDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < FmpCapsuleHeader->EmbeddedDriverCount; Index++) {
    if (FmpCapsuleHeader->PayloadItemCount == 0 && Index == (UINTN)FmpCapsuleHeader->EmbeddedDriverCount - 1) {
      //
      // When driver is last element in the ItemOffsetList array, the driver size is calculated by reference CapsuleImageSize in EFI_CAPSULE_HEADER
      //
      DriverLen = CapsuleHeader->CapsuleImageSize - CapsuleHeader->HeaderSize - (UINTN)ItemOffsetList[Index];
    } else {
      DriverLen = (UINTN)ItemOffsetList[Index + 1] - (UINTN)ItemOffsetList[Index];
    }

    Status = gBS->LoadImage(
                    FALSE,
                    gImageHandle,
                    DriverDevicePath,
                    (UINT8 *)FmpCapsuleHeader + ItemOffsetList[Index],
                    DriverLen,
                    &ImageHandle
                    );
    if (EFI_ERROR(Status)) {
      goto EXIT;
    }

    Status = gBS->StartImage(
                    ImageHandle, 
                    &ExitDataSize, 
                    NULL
                    );
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Driver Return Status = %r\n", Status));
      goto EXIT;
    }
  }

  //
  // Connnect all again to connect drivers within capsule 
  //
  if (FmpCapsuleHeader->EmbeddedDriverCount > 0) {
    BdsLibConnectAll();
  }

  //
  // 3. Route payload to right FMP instance
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareManagementProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );

  if (!EFI_ERROR(Status)) {
    for(Index1 = 0; Index1 < NumberOfHandles; Index1++) {
      Status = gBS->HandleProtocol(
                      HandleBuffer[Index1],
                      &gEfiFirmwareManagementProtocolGuid,
                      (VOID **)&Fmp
                      );
      if (EFI_ERROR(Status)) {
        continue;
      }

      ImageInfoSize = 0;
      Status = Fmp->GetImageInfo (
                      Fmp,
                      &ImageInfoSize,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL
                      );
      if (Status != EFI_BUFFER_TOO_SMALL) {
        continue;
      }

      FmpImageInfoBuf = NULL;
      FmpImageInfoBuf = AllocateZeroPool (ImageInfoSize);
      if (FmpImageInfoBuf == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto EXIT;
      }

      PackageVersionName = NULL;
      Status = Fmp->GetImageInfo (
                      Fmp,
                      &ImageInfoSize,               // ImageInfoSize
                      FmpImageInfoBuf,              // ImageInfo
                      &FmpImageInfoDescriptorVer,   // DescriptorVersion
                      &FmpImageInfoCount,           // DescriptorCount
                      &DescriptorSize,              // DescriptorSize
                      &PackageVersion,              // PackageVersion
                      &PackageVersionName           // PackageVersionName
                      );

      //
      // If FMP GetInformation interface failed, skip this resource
      //
      if (EFI_ERROR(Status)) {
        FreePool(FmpImageInfoBuf);
        continue;
      }

      if (PackageVersionName != NULL) {
        FreePool(PackageVersionName);
      }

      TempFmpImageInfo = FmpImageInfoBuf;
      for (Index2 = 0; Index2 < FmpImageInfoCount; Index2++) {
        //
        // Check all the payload entry in capsule payload list 
        //
        for (Index = FmpCapsuleHeader->EmbeddedDriverCount; Index < ItemNum; Index++) {
          ImageHeader  = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *)((UINT8 *)FmpCapsuleHeader + ItemOffsetList[Index]);
          if (CompareGuid(&ImageHeader->UpdateImageTypeId, &TempFmpImageInfo->ImageTypeId) &&
              ImageHeader->UpdateImageIndex == TempFmpImageInfo->ImageIndex) {
            AbortReason = NULL;
            if (ImageHeader->UpdateVendorCodeSize == 0) {
              Status = Fmp->SetImage(
                              Fmp,
                              TempFmpImageInfo->ImageIndex,           // ImageIndex
                              (UINT8 *)(ImageHeader + 1),             // Image
                              ImageHeader->UpdateImageSize,           // ImageSize
                              NULL,                                   // VendorCode
                              Update_Image_Progress,                  // Progress
                              &AbortReason                            // AbortReason
                              );
            } else {
              Status = Fmp->SetImage(
                              Fmp,
                              TempFmpImageInfo->ImageIndex,                                          // ImageIndex
                              (UINT8 *)(ImageHeader + 1),                                            // Image
                              ImageHeader->UpdateImageSize,                                          // ImageSize
                              (UINT8 *)((UINT8 *) (ImageHeader + 1) + ImageHeader->UpdateImageSize), // VendorCode
                              Update_Image_Progress,                                                 // Progress
                              &AbortReason                                                           // AbortReason
                              );
            }
            if (AbortReason != NULL) {
              DEBUG ((EFI_D_ERROR, "%s\n", AbortReason));
              FreePool(AbortReason);
            }
          }
        }
        //
        // Use DescriptorSize to move ImageInfo Pointer to stay compatible with different ImageInfo version
        //
        TempFmpImageInfo = (EFI_FIRMWARE_IMAGE_DESCRIPTOR *)((UINT8 *)TempFmpImageInfo + DescriptorSize);
      }
      FreePool(FmpImageInfoBuf);
    }
  }

EXIT:

  if (HandleBuffer != NULL) {
    FreePool(HandleBuffer);
  }

  if (DriverDevicePath != NULL) {
    FreePool(DriverDevicePath);
  }

  return Status;
}

EFI_STATUS
EFIAPI
ValidateCapsuleGuidTable (
  CAPSULE_GUID_TABLE *CapsuleGuidTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Crc32 = 0;
  do
  {
    if (CapsuleGuidTable == NULL)
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    Crc32 = CapsuleGuidTable->Crc32;
    CapsuleGuidTable->Crc32  = 0;
    Status = CalculateCrc32(CapsuleGuidTable, 
                            sizeof(CAPSULE_GUID_TABLE), 
                            &(CapsuleGuidTable->Crc32));
    if (EFI_ERROR (Status))
    {  
      break;
    }
  
    if ((Crc32 != CapsuleGuidTable->Crc32) ||
        (CapsuleGuidTable->Signature != CAPSULE_UPDATE_TABLE_SIGNATURE))
    {
      // restore old CRC value
      CapsuleGuidTable->Crc32 = Crc32;
      Status = EFI_NOT_FOUND;
      break;
    }
  } while (0);

  return Status;
}

/**
  Checks whether the Update Capsule partition exists.
   
  @param  BlkIo                 On output, pointer to the pointer of the Block 
                                I/O protocol that corresponds to the Update
                                Capsule Partition if it exists.
  @retval TRUE                  Update Capsule partition does exist.
  @retval FALSE                 Update Capsule partition does not exist.

**/
BOOLEAN
EFIAPI
UpdateCapsulePartitionExists (
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Locate capsule partition if it exists; if not return EFI_NOT_FOUND
  /* Ideally we should not find more than 1 handle */
  HandleInfo              HandleInfoList[4];
  UINT32                  MaxHandles = 0;
  PartiSelectFilter       HandleFilter;
  UINT32                  Attribs = 0;

  if (CapsulePartitionFlag == TRUE)
  {
    return (CapsuleBlkIo != NULL);
  }

  /* Select the BlkIo handle that represents the partition by the
   * referenced GUID type in GPT partition on Non removable media
   * ie eMMC device */
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.PartitionType = &gUpdateCapsulePartitionGuid;
  HandleFilter.VolumeName = 0;

  do{
    Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
  
    if (Status != EFI_SUCCESS)
      break;
  
    /* Having more than one partition is considered ambiguity, so return
     * invalid param */
    if (MaxHandles > 1)
    {  
      Status = EFI_INVALID_PARAMETER;
      break;
    }
  
    if (MaxHandles == 0)
    {
      DEBUG ((EFI_D_INFO, "Update Capsule partition not found\n"));
      Status = EFI_NOT_FOUND;
      break;
    }

    if (HandleInfoList[0].BlkIo->Media->BlockSize != CAPSULE_UPDATE_TABLE_BLOCK_SIZE)
    {
      // Confirm block size of media matches supported block size
      Status = EFI_UNSUPPORTED;
      break;
    }  
  }while (0);
  
  CapsulePartitionFlag = TRUE;
  if (Status != EFI_SUCCESS)
  {
    CapsuleBlkIo = NULL;
    return FALSE;
  }
  else
  {
    CapsuleBlkIo = HandleInfoList[0].BlkIo;
    return TRUE;
  }
}

EFI_STATUS
EFIAPI
DeletePersistedCapsule(
  EVENT_GUID_ENTRY    *EventGuidEntry
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  UINT32             *MemLoc = NULL;
  UINT32             ImgSize = 0;
  CAPSULE_GUID_TABLE *CapsuleGuidTable = NULL;
  UINT32             Index = 0;

  do
  {
    if (!UpdateCapsulePartitionExists())
    {
      Status = EFI_UNSUPPORTED;
      break;
    }
    /* Get the Capsule Guid Table - stored in first block */
    ImgSize = CapsuleBlkIo->Media->BlockSize;
    Status = gBS->AllocatePool(EfiBootServicesData,
                               ImgSize,
                               (VOID**)&MemLoc);

    if (Status != EFI_SUCCESS)
      break;

    // Load the Capsule Guid Table into memory
    Status = CapsuleBlkIo->ReadBlocks (CapsuleBlkIo, 
                                       CapsuleBlkIo->Media->MediaId, 
                                       0, 
                                       ImgSize, 
                                       MemLoc);
    if (EFI_ERROR (Status))
      break;

    CapsuleGuidTable = (CAPSULE_GUID_TABLE*)MemLoc;
    Status = ValidateCapsuleGuidTable(CapsuleGuidTable);
    if (EFI_ERROR (Status))
      break;

    // Find Guid entry corresponding to capsule
    for (Index = 0; Index < CapsuleGuidTable->GuidCount; Index++)
    {
      if ((CompareGuid(&(CapsuleGuidTable->CapsuleGuidEntries[Index].Guid), 
                      &(EventGuidEntry->Guid)) == TRUE) &&
          (EventGuidEntry->CapsuleLBAOffset == CapsuleGuidTable->CapsuleGuidEntries[Index].BlockOffset) &&
          (EventGuidEntry->EntryCount > 0))
      {
        // Check if last entry in list;
        // Don't update CapsuleGuidTable->FreeBlocks unless all entries are 
        // deleted. Once all capsules are deleted it will clear out the entire 
        // table and thus free up the partition. If a new capsule is inserted,
        // it will be allocated from the remaining FreeBlocks and not reallocate
        // from the blocks taken from the deleted capsule.
        if (Index == CapsuleGuidTable->GuidCount - 1)
        {
          // If last entry, zero out last entry
          SetMem(&(CapsuleGuidTable->CapsuleGuidEntries[Index].Guid), 
                 sizeof(CAPSULE_GUID_ENTRY), 
                 0x0);
          CapsuleGuidTable->GuidCount--;
        }
        else
        {
          // If not last entry, shift all entries up one
          CopyMem(&(CapsuleGuidTable->CapsuleGuidEntries[Index]),
                  &(CapsuleGuidTable->CapsuleGuidEntries[Index + 1]),
                  (CapsuleGuidTable->GuidCount - Index - 1)*sizeof(CAPSULE_GUID_ENTRY));
    
          // Zero out last entry
          SetMem(&(CapsuleGuidTable->CapsuleGuidEntries[CapsuleGuidTable->GuidCount - 1]), 
                 sizeof(CAPSULE_GUID_ENTRY), 
                 0x0);
          CapsuleGuidTable->GuidCount--;
        }

        // Decrease the number of entries for the given GUID
        EventGuidEntry->EntryCount--;
        CapsuleGuidTable->Age = 0;
      }
    }

    if (CapsuleGuidTable->GuidCount == 0)
    {
      // Reinitialize Capsule Guid Table 
      // This effectively deletes all capsules in the partition
      CapsuleGuidTable->Signature = CAPSULE_UPDATE_TABLE_SIGNATURE;
      CapsuleGuidTable->Revision = CAPSULE_UPDATE_TABLE_REVISION;
      CapsuleGuidTable->GuidCount = 0; 
      CapsuleGuidTable->BlockSize = CAPSULE_UPDATE_TABLE_BLOCK_SIZE;
      CapsuleGuidTable->BlockCount = CapsuleBlkIo->Media->LastBlock;
      CapsuleGuidTable->FreeBlocks = CapsuleBlkIo->Media->LastBlock - 1;
      CapsuleGuidTable->Age = 0;
      CapsuleGuidTable->Reserved[0] = 0;
      CapsuleGuidTable->Reserved[1] = 0;
      CapsuleGuidTable->Reserved[2] = 0;

      // Clear out all Guid entries
      SetMem(&(CapsuleGuidTable->CapsuleGuidEntries[0]), 
             sizeof(CapsuleGuidTable->CapsuleGuidEntries),
             0x0);
    }

    CapsuleGuidTable->Crc32 = 0;
    Status = CalculateCrc32(CapsuleGuidTable, 
                            sizeof(CAPSULE_GUID_TABLE), 
                            &(CapsuleGuidTable->Crc32));
    if (EFI_ERROR (Status))
      break;

    // Copy back Capsule Guid Table to persisted storage
    Status = CapsuleBlkIo->WriteBlocks (CapsuleBlkIo, 
                                        CapsuleBlkIo->Media->MediaId, 
                                        0, 
                                        ImgSize, 
                                        MemLoc);
    if (EFI_ERROR (Status))
      break;

  }while(0);

  if (Status == EFI_SUCCESS)
  {
    gBS->FreePool(MemLoc);
  }

  return Status;
}

/**
  Callback to delete the Capsule from the persistent storage and the System 
  Table. Only delete capsules from System Table if the 
  CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE is not set. 

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
DeleteCapsule (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  BOOLEAN EventGuidFound = FALSE;
  UINT32 Index = 0;

  do
  {
    // Find event that corresponds to the one that was signaled
    for (Index = 0; Index < EventGuidTable.GuidCount; Index++)
    {
      if (Event == EventGuidTable.GuidEntries[Index].Event)
      {
        EventGuidFound = TRUE;
        break;
      }
    }

    // Check if persistence is supported or Event not found
    if ((CapsuleBlkIo == NULL) || (Index == EventGuidTable.GuidCount))
    {
      break;
    }
  
    // Delete that Guid from the persisted storage if it is present in 
    // the storage
    DeletePersistedCapsule(&(EventGuidTable.GuidEntries[Index]));  
  } while(0);

  if (EventGuidFound == TRUE)
  {
    if ((EventGuidTable.GuidEntries[Index].CapsuleFlags & 
        CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) == 0)
    {
      // Delete the GUID from the System Table if 
      // CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE is not set
      gBS->InstallConfigurationTable(&(EventGuidTable.GuidEntries[Index].Guid), 
                                     NULL);
    }

    if (Index == EventGuidTable.GuidCount - 1)
    {
      // If last entry, zero out last entry
      SetMem(&(EventGuidTable.GuidEntries[Index]),
             sizeof(EVENT_GUID_ENTRY), 
             0x0);
      EventGuidTable.GuidEntries[Index].Event = 0;
    }
    else
    {
      // If not last entry, copy last entry to current Index entry
      CopyMem(&(EventGuidTable.GuidEntries[Index]),
              &(EventGuidTable.GuidEntries[EventGuidTable.GuidCount - 1]),
              sizeof(EVENT_GUID_ENTRY));

      // Zero out last entry
      SetMem(&(EventGuidTable.GuidEntries[EventGuidTable.GuidCount - 1]), 
             sizeof(EVENT_GUID_ENTRY), 
             0x0);
    }

    EventGuidTable.GuidCount--;

    // Close the event to avoid multiple signals
    gBS->CloseEvent(Event);
  }
}

/**
  The firmware checks whether the capsule image is supported 
  by the CapsuleGuid in CapsuleHeader or other specific information in capsule image.
  
  @param  CapsuleHeader    Point to the UEFI capsule image to be checked.
  
  @retval EFI_UNSUPPORTED  Input capsule is not supported by the firmware.
**/
EFI_STATUS
EFIAPI
SupportCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  if (CompareGuid (&gEfiFmpCapsuleGuid, &CapsuleHeader->CapsuleGuid))
  {
    //
    // Check layout of FMP capsule
    //
    return ValidateFmpCapsule(CapsuleHeader);
  }
  // Capsules that do not have CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE are supported at boot time
  else if (((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) == 0) && 
      EfiAtRuntime())
  {
    return EFI_UNSUPPORTED;
  }
  // Capsules that do not have CAPSULE_FLAGS_PERSIST_ACROSS_RESET are not supported
  else if ((CapsuleHeader->Flags & CAPSULE_FLAGS_PERSIST_ACROSS_RESET) == 0)
  {
    return EFI_UNSUPPORTED;
  }
  else
  {
    return EFI_SUCCESS;
  }
}

/**
  The firmware specific implementation processes the capsule image
  if it recognized the format of this capsule image.
  
  @param  CapsuleHeader    Point to the UEFI capsule image to be processed. 
  @param  CapsuleLBAOffset Offset of the capsule in storage, if 0 capsule did
                           not come from storage
  @param  ResetNeeded      Returns if a process capsule that needs a reset was
                           processed
   
  @retval EFI_UNSUPPORTED  Capsule image is not supported by the firmware.
**/
EFI_STATUS
EFIAPI
ProcessCapsuleImageEx (
  IN EFI_CAPSULE_HEADER *CapsuleHeader,
  IN EFI_LBA             CapsuleLBAOffset,
  OUT BOOLEAN            *ResetNeeded OPTIONAL
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Size = 0;
  EFI_CAPSULE_TABLE *CapsuleTable = NULL;
  UINT32 Index = 0;

  do
  {
    //
    // Check FMP capsule layout
    //
    if (CompareGuid (&gEfiFmpCapsuleGuid, &CapsuleHeader->CapsuleGuid))
    {
      EVENT_GUID_ENTRY Entry;
      Status = ValidateFmpCapsule(CapsuleHeader);
      if (EFI_ERROR(Status)) {
        return Status;
      }

      //
      // Press EFI FMP Capsule
      //
      Status = ProcessFmpCapsuleImage(CapsuleHeader);

      //
      // Delete Capsule
      //
      Entry.EntryCount = 1;
      CopyGuid(&(Entry.Guid), &(CapsuleHeader->CapsuleGuid));
      Entry.CapsuleLBAOffset = CapsuleLBAOffset;
      Entry.CapsuleFlags = CapsuleHeader->Flags;
      DeletePersistedCapsule(&Entry);
      if (NULL != ResetNeeded)
      {
        *ResetNeeded = TRUE;
      }
      break;
    }
    if (((CapsuleHeader->Flags & CAPSULE_FLAGS_PERSIST_ACROSS_RESET) == 0) || 
        EfiAtRuntime())
    {
      // Do not support if CAPSULE_FLAGS_PERSIST_ACROSS_RESET is not set
      Status = EFI_UNSUPPORTED;
      break;
    }
    else
    {
      // Put all capsules in System Table
      // Assumption: The capsule passed in was in the persisted capsule partition and 
      // would need to be removed from the System Table before ExitBootServices is called
      // when booting to HLOS. However this is not enforced.
      Size = sizeof(EFI_CAPSULE_TABLE) + sizeof(VOID*);
      if ((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) != 0)
      {
        // If required in system table then allocate for runtime
        Status = gBS->AllocatePool(EfiRuntimeServicesData,
                                   Size,
                                   (VOID**)&CapsuleTable);
      }
      else
      {
        // If not required in system table then allocate for boottime
        Status = gBS->AllocatePool(EfiBootServicesData,
                                   Size,
                                   (VOID**)&CapsuleTable);
      }
      if (Status != EFI_SUCCESS)
      {
        break;
      }
      CapsuleTable->CapsuleArrayNumber = 1;
      CopyMem(&CapsuleTable->CapsulePtr[0], (VOID*)&CapsuleHeader, sizeof(VOID*));
      // TODO: Need to use EfiGetSystemConfigurationTable to make sure the table has not already
      // been installed (i.e. with the same GUID). If so - appending is needed.
      // For now, the capsule passed in will overwrite what is the System Table already
      Status = gBS->InstallConfigurationTable (&(CapsuleHeader->CapsuleGuid), 
                                               (VOID*)CapsuleTable);
      if (Status != EFI_SUCCESS)
      {
        break;
      }
  
      // See if GUID entry exists already in the EventGuidTable
      for (Index = 0; Index < EventGuidTable.GuidCount; Index++)
      {
        if ((CompareGuid(&(CapsuleHeader->CapsuleGuid), 
                        &(EventGuidTable.GuidEntries[Index].Guid)) == TRUE) &&
            (CapsuleLBAOffset == EventGuidTable.GuidEntries[Index].CapsuleLBAOffset))
        {
          break;
        }
      }

      // If GUID is not already registered in the EventGuidTable, Index will equal
      // EventGuidTable.GuidCount 
      if (Index == EventGuidTable.GuidCount)
      {
        // Mark GUID for deletion from System Table on Exit Boot Services or when signaled
        // Create event for deletion from EventGuidTable and persistence
        Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    DeleteCapsule,
                    NULL,
                    &(CapsuleHeader->CapsuleGuid),
                    &(EventGuidTable.GuidEntries[Index].Event)
                    );
        if (Status != EFI_SUCCESS)
        {
          break;
        }
        CopyGuid(&(EventGuidTable.GuidEntries[Index].Guid),
                 &(CapsuleHeader->CapsuleGuid));
        EventGuidTable.GuidEntries[Index].CapsuleLBAOffset = 
          CapsuleLBAOffset;
        EventGuidTable.GuidEntries[Index].CapsuleFlags = 
          CapsuleHeader->Flags;
        EventGuidTable.GuidEntries[Index].EntryCount = 1;
        EventGuidTable.GuidCount++;
      }
      else
      {
        // GUID already exists in EventGuidTable - just keep track by 
        // incrementing the count
        EventGuidTable.GuidEntries[Index].EntryCount++;
      }
    }
  } while (0);

  return Status;
}

