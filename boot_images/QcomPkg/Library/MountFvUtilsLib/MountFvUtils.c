/** @file
  This file includes methods for loading and decompression of FV

  Copyright (c) 2017, Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  Portions copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.
  
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php
  
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 09/08/17   kpa      Initial Revision
=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/
#include <Uefi.h>

#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/ArmLib.h>
#include <Library/PcdLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ExtractGuidedSectionLib.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/LzmaDecompress.h>
#include "QcomLib.h"
#include <Library/GuidedFvLib.h>
#include <Library/QcomBdsLib.h>
#include <MemRegionInfo.h>
#include <Library/UefiCfgLib.h>
#include <Library/UefiLib.h>
#include <Library/MountFvUtils.h>


#define FVB_DEVICE_SIGNATURE        SIGNATURE_32('_','F','V','B')
#define GET_OCCUPIED_SIZE(ActualSize, Alignment) \
  (ActualSize) + (((Alignment) - ((ActualSize) & ((Alignment) - 1))) & ((Alignment) - 1))

//
// UEFI Specification structures for FV LBA cache, device path and FVB device format
//  
typedef struct {
  UINTN                       Base;
  UINTN                       Length;
} LBA_CACHE;

typedef struct {
  MEMMAP_DEVICE_PATH          MemMapDevPath;
  EFI_DEVICE_PATH_PROTOCOL    EndDevPath;
} FV_MEMMAP_DEVICE_PATH;

typedef struct {
  MEDIA_FW_VOL_DEVICE_PATH    FvDevPath;
  EFI_DEVICE_PATH_PROTOCOL    EndDevPath;
} FV_PIWG_DEVICE_PATH;

typedef struct {
  UINTN                                 Signature;
  EFI_HANDLE                            Handle;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL    FwVolBlockInstance;
  UINTN                                 NumBlocks;
  LBA_CACHE                             *LbaCache;
  UINT32                                FvbAttributes;
  EFI_PHYSICAL_ADDRESS                  BaseAddress;
  UINT32                                AuthenticationStatus;
} EFI_FW_VOL_BLOCK_DEVICE;

// 
// Global data structures and variables
//
FV_MEMMAP_DEVICE_PATH mFvMemmapDevicePathTemplate = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_MEMMAP_DP,
      {
        (UINT8)(sizeof (MEMMAP_DEVICE_PATH)),
        (UINT8)(sizeof (MEMMAP_DEVICE_PATH) >> 8)
      }
    },
    EfiMemoryMappedIO,
    (EFI_PHYSICAL_ADDRESS) 0,
    (EFI_PHYSICAL_ADDRESS) 0,
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};

FV_PIWG_DEVICE_PATH mFvPIWGDevicePathTemplate = {
  {
    {
      MEDIA_DEVICE_PATH,
      MEDIA_PIWG_FW_VOL_DP,
      {
        (UINT8)(sizeof (MEDIA_FW_VOL_DEVICE_PATH)),
        (UINT8)(sizeof (MEDIA_FW_VOL_DEVICE_PATH) >> 8)
      }
    },
    { 0 }
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};

EFI_FW_VOL_BLOCK_DEVICE  mFwVolBlock = {
  FVB_DEVICE_SIGNATURE,
  NULL,
  NULL,
  {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  0,
  NULL,
  0,
  0,
  0
};

EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL *FwVolBlkProtocol = NULL;

// 
// Extern functions
//
extern RETURN_STATUS EFIAPI LzmaUefiDecompressGetInfo (
  IN  CONST VOID  *Source,
  IN  UINT32      SourceSize,
  OUT UINT32      *DestinationSize,
  OUT UINT32      *ScratchSize
  );
  
extern RETURN_STATUS EFIAPI LzmaUefiDecompress (
  IN CONST VOID  *Source,
  IN UINTN       SourceSize,
  IN OUT VOID    *Destination,
  IN OUT VOID    *Scratch
  );
  
extern RETURN_STATUS EFIAPI LzmaGuidedSectionGetInfo (
  IN  CONST VOID  *InputSection,
  OUT UINT32      *OutputBufferSize,
  OUT UINT32      *ScratchBufferSize,
  OUT UINT16      *SectionAttribute
  );
  
extern RETURN_STATUS EFIAPI LzmaGuidedSectionExtraction (
  IN CONST  VOID    *InputSection,
  OUT       VOID    **OutputBuffer,
  OUT       VOID    *ScratchBuffer,        OPTIONAL
  OUT       UINT32  *AuthenticationStatus
  );

/**
  This routine produces a firmware volume block protocol on a given
  buffer. 
  Function used from /MdeModulePkg/Core/Dxe/FwVolBlock.c

  @param  BaseAddress            base address of the firmware volume image
  @param  Length                 length of the firmware volume image
  @param  ParentHandle           handle of parent firmware volume, if this image
                                 came from an FV image file and section in another firmware
                                 volume (ala capsules)
  @param  AuthenticationStatus   Authentication status inherited, if this image
                                 came from an FV image file and section in another firmware volume.

  @retval EFI_VOLUME_CORRUPTED   Volume corrupted.
  @retval EFI_OUT_OF_RESOURCES   No enough buffer to be allocated.
  @retval EFI_SUCCESS            Successfully produced a FVB protocol on given
                                 buffer.

**/
EFI_STATUS
ProduceFVBProtocolOnBuffer (
  IN EFI_PHYSICAL_ADDRESS   BaseAddress,
  IN UINT64                 Length,
  IN EFI_HANDLE             ParentHandle,
  IN UINT32                 AuthenticationStatus
  )
{
  EFI_FW_VOL_BLOCK_DEVICE       *FvbDev;
  EFI_FIRMWARE_VOLUME_HEADER    *FwVolHeader;
  UINTN                         BlockIndex;
  UINTN                         BlockIndex2;
  UINTN                         LinearOffset;
  UINT32                        FvAlignment;
  EFI_FV_BLOCK_MAP_ENTRY        *PtrBlockMapEntry;
  EFI_STATUS                    Status;

  FvAlignment = 0;
  FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN) BaseAddress;
  //
  // Validate FV Header, if not as expected, return
  //
  if (FwVolHeader->Signature != EFI_FVH_SIGNATURE) {
    return EFI_VOLUME_CORRUPTED;
  }
  //
  // Get FvHeader alignment
  //
  FvAlignment = 1 << ((FwVolHeader->Attributes & EFI_FVB2_ALIGNMENT) >> 16);
  //
  // FvAlignment must be greater than or equal to 8 bytes of the minimum FFS alignment value. 
  //
  if (FvAlignment < 8) {
    FvAlignment = 8;
  }
  if ((UINTN)BaseAddress % FvAlignment != 0) {
    //
    // FvImage buffer is not at its required alignment.
    //
    return EFI_VOLUME_CORRUPTED;
  }
    
  Status = gBS->LocateProtocol(&gEfiFirmwareVolumeBlockProtocolGuid, 
                               NULL,
                               (VOID**) &FwVolBlkProtocol);  
  ASSERT_EFI_ERROR (Status);
  
  CopyMem(&mFwVolBlock.FwVolBlockInstance, FwVolBlkProtocol, sizeof(EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL));

  //
  // Allocate EFI_FW_VOL_BLOCK_DEVICE
  //
  FvbDev = AllocateCopyPool (sizeof (EFI_FW_VOL_BLOCK_DEVICE), &mFwVolBlock);
  if (FvbDev == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }  
  
  FvbDev->BaseAddress   = BaseAddress;
  FvbDev->FvbAttributes = FwVolHeader->Attributes;
  FvbDev->FwVolBlockInstance.ParentHandle = ParentHandle;
  if (ParentHandle != NULL) {
    FvbDev->AuthenticationStatus = AuthenticationStatus;
  }

  //
  // Init the block caching fields of the device
  // First, count the number of blocks
  //
  FvbDev->NumBlocks = 0;
  for (PtrBlockMapEntry = FwVolHeader->BlockMap;
       PtrBlockMapEntry->NumBlocks != 0;
       PtrBlockMapEntry++) {
    FvbDev->NumBlocks += PtrBlockMapEntry->NumBlocks;
  }
  //
  // Second, allocate the cache
  //
  FvbDev->LbaCache = AllocatePool (FvbDev->NumBlocks * sizeof (LBA_CACHE));
  if (FvbDev->LbaCache == NULL) {
    FreePool (FvbDev);
    return EFI_OUT_OF_RESOURCES;
  }
  
  //
  // Last, fill in the cache with the linear address of the blocks
  //
  BlockIndex = 0;
  LinearOffset = 0;
  for (PtrBlockMapEntry = FwVolHeader->BlockMap;
       PtrBlockMapEntry->NumBlocks != 0; PtrBlockMapEntry++) {
    for (BlockIndex2 = 0; BlockIndex2 < PtrBlockMapEntry->NumBlocks; BlockIndex2++) {
      FvbDev->LbaCache[BlockIndex].Base = LinearOffset;
      FvbDev->LbaCache[BlockIndex].Length = PtrBlockMapEntry->Length;
      LinearOffset += PtrBlockMapEntry->Length;
      BlockIndex++;
    }
  }

  //
  // Judge whether FV name guid is produced in Fv extension header
  //
  if (FwVolHeader->ExtHeaderOffset == 0) {
    //
    // FV does not contains extension header, then produce MEMMAP_DEVICE_PATH
    //
    FvbDev->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) AllocateCopyPool (sizeof (FV_MEMMAP_DEVICE_PATH), &mFvMemmapDevicePathTemplate);
    if (FvbDev->DevicePath == NULL) {
      FreePool (FvbDev);
      return EFI_OUT_OF_RESOURCES;
    }
    ((FV_MEMMAP_DEVICE_PATH *) FvbDev->DevicePath)->MemMapDevPath.StartingAddress = BaseAddress;
    ((FV_MEMMAP_DEVICE_PATH *) FvbDev->DevicePath)->MemMapDevPath.EndingAddress   = BaseAddress + FwVolHeader->FvLength - 1;
  } else {
    //
    // FV contains extension header, then produce MEDIA_FW_VOL_DEVICE_PATH
    //
    FvbDev->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) AllocateCopyPool (sizeof (FV_PIWG_DEVICE_PATH), &mFvPIWGDevicePathTemplate);
    if (FvbDev->DevicePath == NULL) {
      FreePool (FvbDev);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyGuid (
      &((FV_PIWG_DEVICE_PATH *)FvbDev->DevicePath)->FvDevPath.FvName, 
      (GUID *)(UINTN)(BaseAddress + FwVolHeader->ExtHeaderOffset)
      );
  }
  
  //
  //
  // Attach FvVolBlock Protocol to new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
             &FvbDev->Handle,
             &gEfiFirmwareVolumeBlockProtocolGuid,     &FvbDev->FwVolBlockInstance,
             &gEfiDevicePathProtocolGuid,              FvbDev->DevicePath,
             NULL
             );

  return Status;
  
}


/**
  Go through the file to search SectionType section,
  when meeting an encapsuled section. 
  Function used from EmbeddedPkg/Library/PrePiLib/FwVol.c
  
  @param  SectionType  - Filter to find only section of this type.
  @param  Section      - From where to search.
  @param  SectionSize  - The file size to search.
  @param  OutputBuffer - Pointer to the section to search.
                      
  @retval EFI_SUCCESS
**/
EFI_STATUS
FfsProcessSection (
  IN EFI_SECTION_TYPE           SectionType,
  IN EFI_COMMON_SECTION_HEADER  *Section,
  IN UINTN                      SectionSize,
  OUT VOID                      **OutputBuffer
  )
{
  EFI_STATUS                              Status;
  UINT32                                  SectionLength = 0;
  UINT32                                  ParsedLength;
  UINT32                                  DstBufferSize = 0;
  VOID                                    *ScratchBuffer;
  UINT32                                  ScratchBufferSize;
  VOID                                    *DstBuffer = NULL;
  UINT16                                  SectionAttribute;
  UINT32                                  AuthenticationStatus;


  *OutputBuffer = NULL;
  ParsedLength  = 0;
  Status        = EFI_NOT_FOUND;
  while (ParsedLength < SectionSize) {
    if (Section->Type == SectionType) {
      *OutputBuffer = (VOID *)(Section + 1);

      return EFI_SUCCESS;
    } else if (Section->Type == EFI_SECTION_GUID_DEFINED) {
    
      if (Section->Type == EFI_SECTION_GUID_DEFINED) {
        Status = ExtractGuidedSectionGetInfo (
                   Section,
                   &DstBufferSize,
                   &ScratchBufferSize,
                   &SectionAttribute
                   );
      }
      
      if (EFI_ERROR (Status)) {
        //
        // GetInfo failed
        //
        DEBUG ((EFI_D_ERROR, "Decompress GetInfo Failed - %r\n", Status));
        return EFI_NOT_FOUND;
      }
      //
      // Allocate scratch buffer
      //
      ScratchBuffer = (VOID *)(UINTN)AllocatePages (EFI_SIZE_TO_PAGES (ScratchBufferSize));
      if (ScratchBuffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      //
      // Allocate destination buffer, extra one page for adjustment 
      //
      DstBuffer = (VOID *)(UINTN)AllocatePages (EFI_SIZE_TO_PAGES (DstBufferSize) + 1);
      if (DstBuffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      //
      // DstBuffer still is one section. Adjust DstBuffer offset, skip EFI section header
      // to make section data at page alignment.
      //
      DstBuffer = (UINT8 *)DstBuffer + EFI_PAGE_SIZE - sizeof (EFI_COMMON_SECTION_HEADER);
      //
      // Call decompress function
      //
      if (Section->Type == EFI_SECTION_GUID_DEFINED) {
        Status = ExtractGuidedSectionDecode (
                    Section,
                    &DstBuffer,
                    ScratchBuffer,
                    &AuthenticationStatus
                    );
      }
      
      if (EFI_ERROR (Status)) {
        //
        // Decompress failed
        //
        DEBUG ((EFI_D_ERROR, "Decompress Failed - %r\n", Status));
        return EFI_NOT_FOUND;
      } else {
        return FfsProcessSection (
                SectionType, 
                DstBuffer, 
                DstBufferSize, 
                OutputBuffer 
                );
       }        
    }

    //
    // Size is 24 bits wide so mask upper 8 bits. 
    // SectionLength is adjusted it is 4 byte aligned.
    // Go to the next section
    //
    SectionLength = *(UINT32 *)Section->Size & 0x00FFFFFF;
    SectionLength = GET_OCCUPIED_SIZE (SectionLength, 4);
    if (SectionLength == 0)
      return EFI_NOT_FOUND;
    
    ParsedLength += SectionLength;
    Section = (EFI_COMMON_SECTION_HEADER *)((UINT8 *)Section + SectionLength);
  }
  
  return EFI_NOT_FOUND;
}

/**
  Process the buffer to obtain the decompressed buffer and use that to produce the 
  FVB protocol on the buffer.
  Parts of the function used from MdeModulePkg/Core/Dxe/Dispatcher/Dispatcher.c
  Parts of the function used from EmbeddedPkg/Library/PrePiLib/FwVol.c

  @param  Buffer                The FV buffer to be processed

  @retval EFI_OUT_OF_RESOURCES  No enough memory or other resource.
  @retval EFI_VOLUME_CORRUPTED  Corrupted volume.
  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_INVALID_PARAMETER Invalid parameters

**/
EFI_STATUS
ProcessFvImageFile (
  IN  VOID                            *Buffer
  )
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  VOID                                *AlignedBuffer = NULL;
  EFI_FIRMWARE_VOLUME_HEADER          *FvHeader;
  UINT32                              FvAlignment;
  VOID                                *OutputBuffer = NULL;
  EFI_FFS_FILE_HEADER                 *FfsFileHeader;
  UINT32                              FileSize;
  EFI_COMMON_SECTION_HEADER           *Section;
  
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // Read the first (and only the first) firmware volume section
  //
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) Buffer;  
  
  // FfsFile Header is located after FV Header
  FfsFileHeader = (EFI_FFS_FILE_HEADER *)( (UINTN)FvHeader + (UINTN)FvHeader->HeaderLength);

  //
  // Size is 24 bits wide so mask upper 8 bits. 
  // Does not include FfsFileHeader header size
  // FileSize is adjusted to FileOccupiedSize as it is 8 byte aligned.
  //
  Section = (EFI_COMMON_SECTION_HEADER *)(FfsFileHeader + 1);
  FileSize = *(UINT32 *)(FfsFileHeader->Size) & 0x00FFFFFF;
  FileSize -= sizeof (EFI_FFS_FILE_HEADER);

  // Decompress the Volume image, if required
  Status = FfsProcessSection (  EFI_SECTION_FIRMWARE_VOLUME_IMAGE,
                                Section,
                                FileSize,
                                &OutputBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  

  if (OutputBuffer != NULL) {
    
    //
    // FvImage should be at its required alignment.
    //
    FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) OutputBuffer;
    //
    // Get FvHeader alignment
    //
    FvAlignment = 1 << ((FvHeader->Attributes & EFI_FVB2_ALIGNMENT) >> 16);
    //
    // FvAlignment must be greater than or equal to 8 bytes of the minimum FFS alignment value. 
    //
    if (FvAlignment < 8) {
      FvAlignment = 8;
    }
    //
    // Allocate the aligned buffer for the FvImage.
    //
    AlignedBuffer = AllocateAlignedPages (EFI_SIZE_TO_PAGES (FvHeader->FvLength), (UINTN) FvAlignment);
    if (AlignedBuffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
    } else {
      //
      // Move FvImage into the aligned buffer and release the original buffer.
      //
      CopyMem (AlignedBuffer, OutputBuffer, FvHeader->FvLength);

      //
      // Produce a FVB protocol for the file
      //
      Status = ProduceFVBProtocolOnBuffer (
                 (EFI_PHYSICAL_ADDRESS) (UINTN) AlignedBuffer,
                 (UINT64)FvHeader->FvLength,
                 NULL,
                 0
                 ); 
    }
  }

  if (EFI_ERROR (Status)) {
    if (AlignedBuffer != NULL) {
      FreeAlignedPages (AlignedBuffer, EFI_SIZE_TO_PAGES (FvHeader->FvLength));
    }

    if(OutputBuffer != NULL) {
      FreePool(OutputBuffer);
    }
  }

  return Status;
}


/**
  Read a FV (firmware volume) from a partition passed in
  as an argument.  
  
  @param[in]  PartitionLabel    Label of the partition from where FV gets loaded
  @param[out] ReadBuffer        Buffer cointaining loaded FV
              ImageSize         Size of data read
              Attrib            Attributes of partitioned to be opened
  
  @retval  EFI_SUCCESS          Partition found and FV is loaded.
  @retval  EFI_NOT_FOUND        Firmware volume not found.
  @retval  EFI_OUT_OF_RESOURCES There were not enough resources to allocate the 
                                output data buffer or complete the operations.
**/   
EFI_STATUS ReadFromPartitionWithAttrib (CHAR16* PartitionLabel, VOID **ReadBuffer, UINTN *ImageSize, UINT32 Attrib)
{
  EFI_STATUS             Status = EFI_NOT_FOUND;
  HandleInfo             HandleInfoList[2];
  PartiSelectFilter      FilterData;
  UINT32                 HandleCount;
  EFI_BLOCK_IO_PROTOCOL *BlkIo;
  UINTN                  ReadSize;

  FilterData.PartitionLabel = PartitionLabel;

  HandleCount = sizeof(HandleInfoList)/sizeof(*HandleInfoList);

  Status = GetBlkIOHandles(Attrib,
                           &FilterData,
                           HandleInfoList,
                           &HandleCount);
  if (EFI_ERROR(Status) || (HandleCount != 1))
  {
    FilterData.RootDeviceType = &gEfiEmmcUserPartitionGuid;
    Status = GetBlkIOHandles(Attrib,
                             &FilterData,
                             HandleInfoList,
                             &HandleCount);
    if (EFI_ERROR(Status) || (HandleCount != 1))
      return EFI_NOT_FOUND;
  }

  BlkIo = HandleInfoList[0].BlkIo;

  ReadSize = (BlkIo->Media->LastBlock+1)*(BlkIo->Media->BlockSize);

  *ReadBuffer = AllocatePages(ReadSize/EFI_PAGE_SIZE);
  if (*ReadBuffer == NULL)
    return EFI_OUT_OF_RESOURCES;

  Status = BlkIo->ReadBlocks(BlkIo,
                             BlkIo->Media->MediaId,
                             0,
                             ReadSize,
                             *ReadBuffer);

  *ImageSize = ReadSize;
  return Status;
}

/**
  Wrapper around ReadFromPartitionWithAttrib to enable setting default
  partition attributes

  Read a FV (firmware volume) from a partition passed in
  as an argument.  
  
  @param[in]  PartitionName    Label of the partition from where FV gets loaded
  @param[out] ReadBuffer        Buffer cointaining loaded FV
  
  @retval  EFI_SUCCESS          Partition found and FV is loaded.
  @retval  EFI_NOT_FOUND        Firmware volume not found.
  @retval  EFI_OUT_OF_RESOURCES There were not enough resources to allocate the 
                                output data buffer or complete the operations.
**/   
EFI_STATUS ReadFromPartition (CHAR16* PartitionLabel, VOID **ReadBuffer, UINTN *ImageSize)
{
  UINT32                 Attrib;

  Attrib = BLK_IO_SEL_MATCH_PARTITION_LABEL;
  Attrib |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;
  Attrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;

  return ReadFromPartitionWithAttrib(PartitionLabel, ReadBuffer, ImageSize, Attrib);
  
}