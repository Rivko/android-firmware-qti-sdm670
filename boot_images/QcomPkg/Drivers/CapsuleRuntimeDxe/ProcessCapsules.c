/** @file
  BDS routines to handle capsules.
 
  Copyright (c) 2012-2013, Qualcomm Technologies Inc. All rights reserved.
Portions Copyright (c) 2004 - 2009, Intel Corporation. All 
rights 
reserved.<BR> This program and the accompanying materials 
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
 11/30/14   bh      fix 64-bit compiler errors
 09/02/14   vk      Return if memory allocation fails, for KW
 05/07/13   vk      Return if memory allocation fails, for KW
 07/26/12   niting  Reset age of CapsuleGuidTable if any updates are made
 05/15/12   niting  Fixed alignment for capsule offset table.
 03/13/12   niting  Added firmware update support.
 02/06/12   jdt     Initial revision

=============================================================================*/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CapsuleLib.h>
#include <Library/QcomLib.h>
#include <Library/UefiRuntimeLib.h>

#include "CommonHeader.h"
#include "CapsuleService.h"

STATIC UINT8 OverflowBuffer[CAPSULE_UPDATE_TABLE_BLOCK_SIZE];

/* CapsuleBuffer is the output of CapsuleDataCoalesce */
EFI_STATUS
EFIAPI
PersistCoalescedCapsules (
  IN VOID    *CapsuleBuffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  UINT32                  *MemLoc;
  UINT32                  ImgSize;
  CAPSULE_GUID_ENTRY      *GuidEntry;
  CAPSULE_GUID_TABLE      *CapsuleGuidTable;
  UINT32                  FreeBlockLocation;
  UINT32                  CapsuleSize;
  UINT8                   *OverflowLocation = NULL;
  UINT32                  OverflowSize = 0;
  UINT32                  OverflowBlock = 0;
  UINT32                  Index;

  CAPSULE_OFFSET_TABLE      *CapsuleOffsetTable;
  UINT32                     CapsuleOffsetByte;
  EFI_CAPSULE_BUFFER_HEADER *CapsuleBufferHeader;
  UINT8                     *CapsuleBufferData;
  EFI_CAPSULE_HEADER        *CapsuleHeader;
  UINT32                    CapsuleTotalNumber;

  do {
  
    if ((CapsuleBlkIo == NULL) || EfiAtRuntime())
    {
      Status = EFI_UNSUPPORTED;
      break;
    }

    CapsuleBufferHeader = (EFI_CAPSULE_BUFFER_HEADER*)CapsuleBuffer;
    if(CapsuleBufferHeader == NULL)
    {
      return EFI_INVALID_PARAMETER;
    }

    if(CapsuleBufferHeader->PrivateData.Signature != EFI_CAPSULE_PEIM_PRIVATE_DATA_SIGNATURE)
    {
      return EFI_INVALID_PARAMETER;
    }

    CapsuleBufferData = &CapsuleBufferHeader->CapsuleData[0];

    // Get CapsuleTotalNumber from CapsuleOffsetTable - align to UINT32
    CapsuleOffsetByte = ALIGN_VALUE(CapsuleBufferHeader->PrivateData.CapsuleSize,
                                    sizeof(UINT64));
    CapsuleOffsetTable = (CAPSULE_OFFSET_TABLE*)(&CapsuleBufferData[CapsuleOffsetByte]);

    CapsuleTotalNumber = CapsuleOffsetTable->CapsuleCount;

    if (CapsuleTotalNumber == 0) {
      //
      // We didn't find a hob, so had no errors.
      //
      return EFI_SUCCESS;
    }

    CapsuleBufferHeader = (EFI_CAPSULE_BUFFER_HEADER*)CapsuleBuffer;

    // Validate signature
    if (CapsuleBufferHeader->PrivateData.Signature != EFI_CAPSULE_PEIM_PRIVATE_DATA_SIGNATURE)
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }
  
    //////////////////////////////////////////////////////////
    /// Read and Initialize Capsule Guid Table
    //////////////////////////////////////////////////////////

    // Read Capsule Guid Table from storage
    ImgSize = CapsuleBlkIo->Media->BlockSize;
    Status = gBS->AllocatePool(EfiBootServicesData,
                               ImgSize,
                               (VOID**)&MemLoc);
    if (Status != EFI_SUCCESS)
      return Status;
  
    Status = CapsuleBlkIo->ReadBlocks (CapsuleBlkIo, 
                                       CapsuleBlkIo->Media->MediaId, 
                                       0, 
                                       ImgSize, 
                                       MemLoc);
    if (EFI_ERROR (Status))
    {  
      break;
    }
  
    CapsuleGuidTable = (CAPSULE_GUID_TABLE*)MemLoc;
  
    Status = ValidateCapsuleGuidTable(CapsuleGuidTable);
    if (EFI_ERROR (Status))
    {
      // Reinitialize partition
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
      CapsuleGuidTable->Crc32 = 0;

      // Clear out all Guid entries
      SetMem(&(CapsuleGuidTable->CapsuleGuidEntries[0]), 
             sizeof(CapsuleGuidTable->CapsuleGuidEntries),
             0x0);
      Status = CalculateCrc32(CapsuleGuidTable, 
                              sizeof(CAPSULE_GUID_TABLE), 
                              &(CapsuleGuidTable->Crc32));
      if (EFI_ERROR (Status))
      {  
        break;
      }
    }
  
    // Confirm block sizes match
    if (CapsuleGuidTable->BlockSize != CAPSULE_UPDATE_TABLE_BLOCK_SIZE)
    {
      Status = EFI_UNSUPPORTED;
      break;
    }

    //////////////////////////////////////////////////////////
    /// For each capsule, find a valid slot in the Capsule
    /// Guid Table and ensure enough space is present in the
    /// peristent storage. Proceed to store the capsule followed
    /// by an update to the Capsule Guid Table
    //////////////////////////////////////////////////////////
    for(Index = 0; Index < CapsuleTotalNumber; Index++)
    {
      // Get pointer to capsule and size of capsule
      CapsuleHeader = (EFI_CAPSULE_HEADER*)((UINT8*)CapsuleBufferData +
                                            CapsuleOffsetTable->OffsetArray[Index]);

      // Get Size for capsule
      CapsuleSize = CapsuleHeader->CapsuleImageSize;

      // Find next valid slot that can fit CapsuleBuffer
      if ((CapsuleGuidTable->GuidCount < CAPSULE_GUID_ENTRIES_MAX) && 
          (CapsuleGuidTable->FreeBlocks*CAPSULE_UPDATE_TABLE_BLOCK_SIZE >= CapsuleSize))
      {    
        // Set offset/size in header block for that slot
        FreeBlockLocation = CapsuleGuidTable->BlockCount - CapsuleGuidTable->FreeBlocks;
        GuidEntry = &(CapsuleGuidTable->CapsuleGuidEntries[CapsuleGuidTable->GuidCount]);
        GuidEntry->BlockOffset = FreeBlockLocation;
        GuidEntry->ImageSize = CapsuleSize;
    
        // Write capsule into partition
        OverflowSize = (GuidEntry->ImageSize)%CAPSULE_UPDATE_TABLE_BLOCK_SIZE;
        if (OverflowSize != 0)
        {
          // Copy over remaining non-aligned bytes
          OverflowLocation = (UINT8*)CapsuleHeader + 
                             (GuidEntry->ImageSize - OverflowSize);
          ZeroMem(&(OverflowBuffer[0]), CAPSULE_UPDATE_TABLE_BLOCK_SIZE);
          CopyMem(&(OverflowBuffer[0]), OverflowLocation, OverflowSize);
          OverflowBlock = GuidEntry->BlockOffset + 
                          (GuidEntry->ImageSize >> CAPSULE_UPDATE_TABLE_BLOCK_SHIFT);
          Status = CapsuleBlkIo->WriteBlocks (CapsuleBlkIo, 
                                              CapsuleBlkIo->Media->MediaId, 
                                              OverflowBlock, 
                                              CAPSULE_UPDATE_TABLE_BLOCK_SIZE, 
                                              &(OverflowBuffer[0]));
          if (Status != EFI_SUCCESS)
          {
            break;
          }
        }

        // Write capsule image (except overflow)
        Status = CapsuleBlkIo->WriteBlocks (CapsuleBlkIo, 
                                            CapsuleBlkIo->Media->MediaId, 
                                            GuidEntry->BlockOffset, 
                                            GuidEntry->ImageSize - OverflowSize, 
                                            CapsuleHeader);
        if (Status != EFI_SUCCESS)
        {
          break;
        }

        // Update Capsule Guid Table
        // Update free blocks by number of blocks taken by the capsule
        // Add one block in case ImageSize is not block aligned
        CapsuleGuidTable->FreeBlocks -= 
          (GuidEntry->ImageSize >> CAPSULE_UPDATE_TABLE_BLOCK_SHIFT) + 
          (((GuidEntry->ImageSize%CAPSULE_UPDATE_TABLE_BLOCK_SIZE) > 0)?1:0);
        CapsuleGuidTable->GuidCount++;

        CapsuleGuidTable->Age = 0;

        // Set validity for that capsule slot by setting Guid
        CopyGuid(&(GuidEntry->Guid), &(CapsuleHeader->CapsuleGuid));

        // Calculate CRC of header block
        CapsuleGuidTable->Crc32 = 0;
        Status = CalculateCrc32(CapsuleGuidTable, 
                                sizeof(CAPSULE_GUID_TABLE), 
                                &(CapsuleGuidTable->Crc32)); 
        if (Status != EFI_SUCCESS)
        {
          break;
        }

        // Write back Capsule Guid Table
        Status = CapsuleBlkIo->WriteBlocks (CapsuleBlkIo, 
                                            CapsuleBlkIo->Media->MediaId, 
                                            0, 
                                            ImgSize, 
                                            CapsuleGuidTable);
        if (Status != EFI_SUCCESS)
        {
          break;
        }

      }
      else
      {  
        Status = EFI_OUT_OF_RESOURCES;
        break;
      }
    }

    if (Status != EFI_SUCCESS)
    {
      break;
    }
  
  } while (0);

  return Status;
}

/**

  This routine is called to process capsules which were 
  pushed by the previous boot. 
   
  Capsules with flag CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE set 
  will be installed to the system table, and those without will 
  be processed by ProcessCapsuleImage. 
   

  @param CapsuleBuffer            The Capsule Buffer to process. 
  @retval EFI_INVALID_PARAMETER   boot mode is not correct for an update
  @retval EFI_SUCCESS             There is no error when processing capsule

**/
EFI_STATUS
EFIAPI
ProcessCoalescedCapsules (
  VOID *CapsuleBuffer
  )
{
  EFI_STATUS                  Status;
  EFI_CAPSULE_HEADER          *CapsuleHeader;
  UINT32                      Size;
  UINT32                      CapsuleNumber;
  UINT32                      CapsuleTotalNumber;
  EFI_CAPSULE_TABLE           *CapsuleTable;
  UINT32                      Index;
  UINT32                      CacheIndex;
  UINT32                      CacheNumber;
  VOID                        **CapsulePtr;
  VOID                        **CapsulePtrCache;
  EFI_GUID                    *CapsuleGuidCache; 
  UINTN                        CurrentCapsuleOffset;
  CAPSULE_OFFSET_TABLE        *CapsuleOffsetTable;
  UINT32                       CapsuleOffsetByte;


  EFI_CAPSULE_BUFFER_HEADER      *CapsuleBufferHeader;
  UINT8                         *CapsuleBufferData;

  UINT64                        *CapsuleOffsetArray;

  CapsuleNumber = 0;
  CapsuleTotalNumber = 0;
  CacheIndex   = 0;
  CacheNumber  = 0;
  CapsulePtr        = NULL;
  CapsulePtrCache   = NULL;
  CapsuleGuidCache  = NULL;

  Status = EFI_SUCCESS;

  //Validate Capsule Table
  CapsuleBufferHeader = (EFI_CAPSULE_BUFFER_HEADER*)CapsuleBuffer;
  if(CapsuleBufferHeader == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if(CapsuleBufferHeader->PrivateData.Signature != EFI_CAPSULE_PEIM_PRIVATE_DATA_SIGNATURE)
  {
    return EFI_INVALID_PARAMETER;
  }

  CapsuleBufferData = &CapsuleBufferHeader->CapsuleData[0];

  // Get CapsuleTotalNumber from CapsuleOffsetTable - align to UINT32
  CapsuleOffsetByte = ALIGN_VALUE(CapsuleBufferHeader->PrivateData.CapsuleSize,
                                  sizeof(UINT64));
  CapsuleOffsetTable = (CAPSULE_OFFSET_TABLE*)(&CapsuleBufferData[CapsuleOffsetByte]);

  CapsuleTotalNumber = CapsuleOffsetTable->CapsuleCount;

  if (CapsuleTotalNumber == 0) {
    //
    // We didn't find a hob, so had no errors.
    //
    return EFI_SUCCESS;
  }
  
  //
  // Init temp Capsule Data table.
  //
  CapsulePtr       = (VOID **) AllocateZeroPool (sizeof (VOID *) * CapsuleTotalNumber);
  ASSERT (CapsulePtr != NULL);
  if (CapsulePtr == NULL)
    return EFI_OUT_OF_RESOURCES;

  CapsulePtrCache  = (VOID **) AllocateZeroPool (sizeof (VOID *) * CapsuleTotalNumber);
  ASSERT (CapsulePtrCache != NULL);
  if (CapsulePtrCache == NULL)
    return EFI_OUT_OF_RESOURCES;

  CapsuleGuidCache = (EFI_GUID *) AllocateZeroPool (sizeof (EFI_GUID) * CapsuleTotalNumber);
  ASSERT (CapsuleGuidCache != NULL);
  if (CapsuleGuidCache == NULL)
    return EFI_OUT_OF_RESOURCES;

  //
  // Find all capsule images in CapsuleBuffer.
  //
  CapsuleOffsetArray = CapsuleOffsetTable->OffsetArray;

  for(CapsuleNumber = 0; CapsuleNumber < CapsuleTotalNumber; CapsuleNumber++) 
  {
    CurrentCapsuleOffset = CapsuleOffsetArray[CapsuleNumber];
    CapsulePtr[CapsuleNumber] = (VOID*) &CapsuleBufferData[CurrentCapsuleOffset];
  }

  //
  //Check the capsule flags,if contains CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE, install
  //capsuleTable to configure table with EFI_CAPSULE_GUID
  //

  //
  // Capsules who have CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE always are used for operating
  // System to have information persist across a system reset. EFI System Table must 
  // point to an array of capsules that contains the same CapsuleGuid value. And agents
  // searching for this type capsule will look in EFI System Table and search for the 
  // capsule's Guid and associated pointer to retrieve the data. Two steps below describes
  // how to sorting the capsules by the unique guid and install the array to EFI System Table. 
  // Firstly, Loop for all coalesced capsules, record unique CapsuleGuids and cache them in an 
  // array for later sorting capsules by CapsuleGuid.
  //
  for (Index = 0; Index < CapsuleTotalNumber; Index++) {
    CapsuleHeader = (EFI_CAPSULE_HEADER*) CapsulePtr [Index];
    if ((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) != 0) {
      //
      // For each capsule, we compare it with known CapsuleGuid in the CacheArray.
      // If already has the Guid, skip it. Whereas, record it in the CacheArray as 
      // an additional one.
      //
      CacheIndex = 0;
      while (CacheIndex < CacheNumber) {
        if (CompareGuid(&CapsuleGuidCache[CacheIndex],&CapsuleHeader->CapsuleGuid)) {
          break;
        }
        CacheIndex++;
      }
      if (CacheIndex == CacheNumber) {
        CopyMem(&CapsuleGuidCache[CacheNumber++],&CapsuleHeader->CapsuleGuid,sizeof(EFI_GUID));
      }
    }
  }

  //
  // Secondly, for each unique CapsuleGuid in CacheArray, gather all coalesced capsules
  // whose guid is the same as it, and malloc memory for an array which preceding
  // with UINT32. The array fills with entry point of capsules that have the same
  // CapsuleGuid, and UINT32 represents the size of the array of capsules. Then install
  // this array into EFI System Table, so that agents searching for this type capsule
  // will look in EFI System Table and search for the capsule's Guid and associated
  // pointer to retrieve the data.
  //
  CacheIndex = 0;
  while (CacheIndex < CacheNumber) {
    CapsuleNumber = 0;  
    for (Index = 0; Index < CapsuleTotalNumber; Index++) {
      CapsuleHeader = (EFI_CAPSULE_HEADER*) CapsulePtr [Index];
      if ((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) != 0) {
        if (CompareGuid (&CapsuleGuidCache[CacheIndex], &CapsuleHeader->CapsuleGuid)) {
          //
          // Cache Caspuleheader to the array, this array is uniqued with certain CapsuleGuid.
          //
          CapsulePtrCache[CapsuleNumber++] = (VOID*)CapsuleHeader;
        }
      }
    }
    if (CapsuleNumber != 0) {
      Size = sizeof(EFI_CAPSULE_TABLE) + (CapsuleNumber - 1) * sizeof(VOID*);  
      CapsuleTable = AllocateRuntimePool (Size);
      ASSERT (CapsuleTable != NULL);
      if (CapsuleTable == NULL)
      {
        return EFI_OUT_OF_RESOURCES;
      }
      CapsuleTable->CapsuleArrayNumber =  CapsuleNumber;
      CopyMem(&CapsuleTable->CapsulePtr[0], CapsulePtrCache, CapsuleNumber * sizeof(VOID*));
      // TODO: Need to use EfiGetSystemConfigurationTable to make sure the table has not already
      // been installed (i.e. with the same GUID). If so - appending is needed.
      Status = gBS->InstallConfigurationTable (&CapsuleGuidCache[CacheIndex], (VOID*)CapsuleTable);
      ASSERT_EFI_ERROR (Status);
    }
    CacheIndex++;
  }

  //
  // Besides ones with CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE flag, all capsules left are
  // recognized by platform with CapsuleGuid. For general platform driver, UpdateFlash 
  // type is commonly supported, so here only deal with encapsuled FVs capsule. Additional
  // type capsule transaction could be extended. It depends on platform policy.
  //
  for (Index = 0; Index < CapsuleTotalNumber; Index++) {
    CapsuleHeader = (EFI_CAPSULE_HEADER*) CapsulePtr [Index];
    if ((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) == 0) {
      //
      // Call capsule library to process capsule image.
      //
      ProcessCapsuleImageEx(CapsuleHeader, 0, NULL);
    }
  }
  
  //
  // Free the allocated temp memory space.
  //
  FreePool (CapsuleGuidCache);
  FreePool (CapsulePtrCache);
  FreePool (CapsulePtr);

  return Status;
}

