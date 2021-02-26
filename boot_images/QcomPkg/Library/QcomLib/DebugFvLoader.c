/** @file
  This file includes methods for loading and decompression of FV

  Copyright (c) 2013 - 2014, 2017 Qualcomm Technologies, Inc. All Rights Reserved.
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
 09/08/17   kpa      relocate common api's to MountFvUtils.c
 09/08/17   bd       Support for authenticated FV loading
 08/12/16   bh       Get RAM loaded FV address from memmap
 06/22/16   bh       Add tools FV mounting from different lib
 07/09/15   bh       Remove assert
 12/11/14   bh       Moved location to QcomLib
 10/14/14   jb       Change FD PCD to 64bit
 04/03/14   vk       Fix 64 bit compiler warnings
 01/06/14   aus      Fixed 64 bit program header format
 11/28/13   aus      Added 64 bit elf support
 11/01/13   aus      Added support to load debug FV is uefi is an elf image
 11/01/13   vk       Fix 64 bit compiler warnings
 05/08/13   aus      Read FV info from RAM and if that fails read from EMMC
 04/08/13   aus      Created new file
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
#include <Library/MountFvUtils.h>

#define FV_INFO_SECTOR_MAGIC_NUMBER 0x462E562E // F.V.
#define MAX_FV_COUNT                8

// 
// MBN header structure
//  
typedef struct
{
  UINT32 ImageId;           /* Identifies the type of image this header
                               represents (OEM SBL, AMSS, Apps boot loader,
                               etc.). */
  UINT32 HeaderVersion;     /* Header version number. */
  UINT32 ImageSrc;          /* Location of image in flash: Address of
                               image in NOR or page/sector offset to image
                               from page/sector 0 in NAND/SUPERAND. */
  UINT32 ImageDest;         /* Pointer to location to store image in RAM.
                               Also, entry point at which image execution
                               begins. */
  UINT32 ImageSize;         /* Size of complete image in bytes */
  UINT32 CodeSize;          /* Size of code region of image in bytes */
  UINT32 SignaturePtr;      /* Pointer to images attestation signature */
  UINT32 SignatureSize;     /* Size of the attestation signature in
                               bytes */
  UINT32 CertChainPtr;      /* Pointer to the chain of attestation
                               certificates associated with the image. */
  UINT32 CertChainSize;     /* Size of the attestation chain in bytes */

} ImageHeaderType;

//
// ELF header structure
//

//
// Fields in the e_ident array.  The ELFINFO_*_INDEX macros are 
// indices into the array.  The macros under each ELFINFO_* macro
// is the values the byte may have.  
//
#define ELFINFO_MAGIC_SIZE    16
#define ELFINFO_MAG0_INDEX    0     /* File identification byte 0 index */
#define ELFINFO_MAG0          0x7f  /* Magic number byte 0              */
#define ELFINFO_MAG1_INDEX    1     /* File identification byte 1 index */
#define ELFINFO_MAG1         'E'    /* Magic number byte 1              */
#define ELFINFO_MAG2_INDEX    2     /* File identification byte 2 index */
#define ELFINFO_MAG2         'L'    /* Magic number byte 2              */
#define ELFINFO_MAG3_INDEX    3     /* File identification byte 3 index */
#define ELFINFO_MAG3         'F'    /* Magic number byte 3              */
#define ELFINFO_CLASS_INDEX   4     /* File class byte index            */
#define ELF_CLASS_32          1     /* 32-bit objects                   */
#define ELF_CLASS_64          2     /* 64-bit objects                   */
#define ELFINFO_VERSION_INDEX 6     /* File version byte index          */
#define ELF_VERSION_CURRENT   1     /* Current version                  */

typedef struct
{
  UINT8  e_ident[ELFINFO_MAGIC_SIZE]; /* Magic number and other info       */
  UINT16 e_type;                      /* Object file type                  */
  UINT16 e_machine;                   /* Architecture                      */
  UINT32 e_version;                   /* Object file version               */
  UINTN  e_entry;                     /* Entry point virtual address       */
  UINTN  e_phoff;                     /* Program header table file offset  */
  UINTN  e_shoff;                     /* Section header table file offset  */
  UINT32 e_flags;                     /* Processor-specific flags          */
  UINT16 e_ehsize;                    /* ELF header size in bytes          */
  UINT16 e_phentsize;                 /* Program header table entry size   */
  UINT16 e_phnum;                     /* Program header table entry count  */
  UINT16 e_shentsize;                 /* Section header table entry size   */
  UINT16 e_shnum;                     /* Section header table entry count  */
  UINT16 e_shstrndx;                  /* Section header string table index */
} Elf_Ehdr;

//
// Program segment header.  
//
typedef struct
{
  UINT32 p_type;    /* Segment type */
  UINT32 p_offset;  /* Segment file offset */
  UINTN  p_vaddr;   /* Segment virtual address */
  UINTN  p_paddr;   /* Segment physical address */
  UINTN  p_filesz;  /* Segment size in file */
  UINTN  p_memsz;   /* Segment size in memory */
  UINTN  p_flags;   /* Segment flags */
  UINTN  p_align;   /* Segment alignment */
} Elf32_Phdr;


typedef struct
{
  UINT32 p_type;    /* Segment type */
  UINT32 p_flags;   /* Segment flags */  
  UINTN  p_offset;  /* Segment file offset */
  UINTN  p_vaddr;   /* Segment virtual address */
  UINTN  p_paddr;   /* Segment physical address */
  UINTN  p_filesz;  /* Segment size in file */
  UINTN  p_memsz;   /* Segment size in memory */
  UINTN  p_align;   /* Segment alignment */
} Elf64_Phdr;

// 
// FV info structure
//  
typedef struct
{
  UINT32 MagicNumber;                  /* Magic Number FV_INFO_SECTOR_MAGIC_NUMBER */
  UINT32 Version;                      /* Structure version number. */
  UINT32 NumberOfFvs;                  /* Number of FVs */
  UINT32 FvSizeInBytes [MAX_FV_COUNT];  /* Size of each FV that is appended */
} FvInfoSectorType;

// UEFI Partition GUID
EFI_GUID UefiPartitionGuid = { 0x400FFDCD, 0x22E0, 0x47E7, { 0x9A, 0x23, 0xF1, 0x6E, 0xD9, 0x38, 0x23, 0x88 } };

STATIC HandleInfo HandleInfoList[2];
STATIC UINT32 MaxHandles;
STATIC PartiSelectFilter HandleFilter;

/**
  This function verifies the ELF Header by sanity checking various fields
  within the ELF Header structure 

  @retval TRUE      If ELF Header is valid
  @retval FALSE     If ELF header is invalid

**/
BOOLEAN
VerifyElfHeader(Elf_Ehdr *ehdr)
{
  return ( ehdr->e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
           ehdr->e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
           ehdr->e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
           ehdr->e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
	       ehdr->e_ehsize == sizeof(Elf_Ehdr) &&
           ehdr->e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
          ((ehdr->e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32 && 
		    ehdr->e_phentsize == sizeof(Elf32_Phdr)) || 
	       (ehdr->e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_64 &&  
		    ehdr->e_phentsize == sizeof(Elf64_Phdr))) );
} 

/**
  Load the Debug FV image by parsing the MBN header to locate the end of the UEFI image. 
  Parse the FV info sector to obtain the number of FVs appended. Then load and process 
  every FV that is appended.

  @retval EFI_NOT_FOUND         Couldn't locate the MBN imformation
  @retval EFI_OUT_OF_RESOURCES  Out of memory.
  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_INVALID_PARAMETER Invalid parameter.

**/
EFI_STATUS
LoadDebugFv( VOID )
{
  UINT32                  Attribs = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_BLOCK_IO_PROTOCOL   *BlkIo;
  ImageHeaderType         *MbnHeaderPtr = NULL;
  FvInfoSectorType        *FvInfoPtr = NULL;
  UINT8                   *SectorBuff = NULL, *FvBuffer = NULL, *TempFvBuffer;
  UINT32                  FvInfoSector, FvSectorStart, BlkSize = 0;
  UINT32                  CurrentFvCount, FvTotalSizeInBytes = 0;
  UINTN                   TempRamBuff = 0;
  BOOLEAN                 ReadFromRAM = TRUE;
  UINT32                  HeaderSize = 0, ImageSize = 0;
  Elf_Ehdr                *ElfHeaderPtr = NULL;
  Elf32_Phdr              *Phdr32Ptr = NULL;
  Elf64_Phdr              *Phdr64Ptr = NULL;
  
  // Select the BlkIo handle that represents the partition by the
  // referenced GUID type in GPT partition on Non removable media
  // ie eMMC device 
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.PartitionType = &UefiPartitionGuid;
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
  if (Status != EFI_SUCCESS)
  {
    goto LoadFvImageExit;
  }

  // Having more than one partition is considered ambiguity, return error 
  if (MaxHandles > 1) { 
    Status = EFI_NO_MAPPING;
    goto LoadFvImageExit;
  }
  
  // If no partition is found, return error
  if (MaxHandles == 0) { 
    Status = EFI_NOT_FOUND;
    goto LoadFvImageExit;
  }  
  
  // Get the Block IO protocol instance from the handle 
  BlkIo = HandleInfoList[0].BlkIo;
  BlkSize = BlkIo->Media->BlockSize;
  
  SectorBuff = AllocatePool(BlkSize);
  if (SectorBuff == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto LoadFvImageExit;
  }
  
  // Get the image header to know the end of the UEFI image 
  Status = BlkIo->ReadBlocks (BlkIo, BlkIo->Media->MediaId, 0, BlkSize, SectorBuff);
  if (Status != EFI_SUCCESS)
  {
    goto LoadFvImageExit;
  }

  // Check if UEFI image is an ELF image
  ElfHeaderPtr = (Elf_Ehdr *)SectorBuff;

  if (VerifyElfHeader(ElfHeaderPtr)) 
  {
     // UEFI compiled as ELF image 
	 // The program header corresponding to the actual data segment is the last one. Hence 
	 // we get the headersize to point to the last program header.
     HeaderSize = sizeof(Elf_Ehdr) + ((ElfHeaderPtr->e_phnum-1) * ElfHeaderPtr->e_phentsize);  	 
	 
     // If it is a 32 bit ELF header
	 if (ElfHeaderPtr->e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32) {	 
        Phdr32Ptr = (Elf32_Phdr *) &SectorBuff[HeaderSize];
        ImageSize = Phdr32Ptr->p_memsz + Phdr32Ptr->p_offset;	 
	    HeaderSize = 0;		
	 } 
	 
	 // Else if it is a 64 bit ELF header
	 else {
        Phdr64Ptr = (Elf64_Phdr *) &SectorBuff[HeaderSize];
        ImageSize = Phdr64Ptr->p_memsz + Phdr64Ptr->p_offset;	 
	    HeaderSize = 0;
	 }
  }
  else
  {
     // UEFI compiled as MBN image
     MbnHeaderPtr = (ImageHeaderType *)SectorBuff;
     HeaderSize = sizeof(ImageHeaderType);
     ImageSize = MbnHeaderPtr->ImageSize;
  }
  
  // ImageSize will point to the end of the image. It also points to the beginning of FV information table. 
  // Get the FV information table 
  FvInfoSector = DivU64x32(ImageSize, BlkSize);
  
  if (((ImageSize + HeaderSize)%BlkSize) != 0)
     FvInfoSector += 1;

  //  FV info sector format  
  //===========================================================================
  // magic number: uint32
  // version     : uint32
  // num of FVs  : uint32
  // 
  // FV0 size: 32 bits 
  // FV1 size: 32 bits
  // ....
  // 
  // Sector aligned FV0 data : user defined length 
  // Sector aligned FV1 data : user defined length 
  // ....
  //
  // ==========================================================================
  
  // Try reading the FV info from RAM 
  if (ReadFromRAM) {
      TempRamBuff = (UINTN)PcdGet64(PcdEmbeddedFdBaseAddress) + (UINTN)PcdGet64(PcdEmbeddedFdSize); 
      FvInfoPtr = (FvInfoSectorType *)TempRamBuff;
  
      // Validate the FV info sector 
      // Check Magic number and total number of FVs appended
      if ((FvInfoPtr->MagicNumber != FV_INFO_SECTOR_MAGIC_NUMBER) ||
          (FvInfoPtr->NumberOfFvs >= MAX_FV_COUNT)){
         // Read from RAM failed, hence read from EMMC
         ReadFromRAM = FALSE;
      }  
   }
   
   // Read from EMMC in case read from RAM fails
   if (!ReadFromRAM) {
      Status = BlkIo->ReadBlocks (BlkIo, BlkIo->Media->MediaId, FvInfoSector, BlkSize, SectorBuff);
      if (Status != EFI_SUCCESS) {
         goto LoadFvImageExit;
      }  
  
      FvInfoPtr = (FvInfoSectorType *)SectorBuff;
  
      // Validate the FV info sector 
      // Magic number check 
      if (FvInfoPtr->MagicNumber != FV_INFO_SECTOR_MAGIC_NUMBER) {
         Status = EFI_NOT_FOUND;  
         goto LoadFvImageExit;
      }  
  
      // Check total number of FVs appended
      if (FvInfoPtr->NumberOfFvs >= MAX_FV_COUNT) {
         Status = EFI_INVALID_PARAMETER;  
         goto LoadFvImageExit;
      }  
  }
  
  // Get the total size of all the FVs that need to be read
  CurrentFvCount = 0;
  while (FvInfoPtr->NumberOfFvs != CurrentFvCount) {
    FvTotalSizeInBytes += FvInfoPtr->FvSizeInBytes[CurrentFvCount];
    CurrentFvCount++;
  }

  // Read the FV
  FvBuffer = AllocatePool(FvTotalSizeInBytes);
  if (FvBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto LoadFvImageExit;
  }
  
  if (ReadFromRAM) {
    // FV info sector is sector aligned. Appended FVs start at the sector boundary.
    TempRamBuff += BlkSize;  
    CopyMem (FvBuffer, (VOID *) TempRamBuff, FvTotalSizeInBytes);
  }
  else {
    // FV info sector is sector aligned. Appended FVs start at the sector boundary.
    FvSectorStart = FvInfoSector + 1;  
    Status = BlkIo->ReadBlocks (BlkIo, BlkIo->Media->MediaId, FvSectorStart, FvTotalSizeInBytes, FvBuffer);
    if (Status != EFI_SUCCESS) {
      goto LoadFvImageExit;
    }
  }
  
  // Process each FV 
  CurrentFvCount = 0;
  TempFvBuffer = FvBuffer;
  while (FvInfoPtr->NumberOfFvs != CurrentFvCount) {
    Status = ProcessFvImageFile (TempFvBuffer);
    if (Status != EFI_SUCCESS)
    {
      goto LoadFvImageExit;
    }

    // Script aligns the FVs to sectors and the final size of FV (including the padding to make it sector aligned)
    // is written into the info sector header. Thus the FvSizeInBytes is expected to be sector aligned. 
    // Hence we can obtain the pointer to the next FV by incrementing the current FV Buffer pointer 
    // by the FV size mentioned in the header. 
    TempFvBuffer = TempFvBuffer + FvInfoPtr->FvSizeInBytes[CurrentFvCount];
    CurrentFvCount++; 
  }

LoadFvImageExit:  
  if (SectorBuff != NULL)
    FreePool (SectorBuff);

  if (FvBuffer != NULL)
    FreePool (FvBuffer);

  return Status;
}

EFI_STATUS ReadFromToolsPartition (VOID **ReadBuffer)
{
  EFI_STATUS             Status = EFI_NOT_FOUND;
  CHAR8                  ToolsFvPartition[MAX_PARTI_NAME_LEN];
  UINTN                  Size = MAX_PARTI_NAME_LEN;
  CHAR16                 *PartitionLabel = NULL;
  UINTN                  ImageSize = 0;
  
  Status = GetConfigString ("ToolsFvPartition", ToolsFvPartition, &Size);
  if (Status == EFI_SUCCESS)
  {
    PartitionLabel = AllocatePool(Size);
    AsciiStrToUnicodeStr(ToolsFvPartition, PartitionLabel);
  }
  else
    PartitionLabel = L"toolsfv";
  
  Status = ReadFromPartition (PartitionLabel, ReadBuffer, &ImageSize);
  
  return Status;
}

#define TOOLS_FV_FILENAME "tools.fv"

EFI_STATUS MountFvFromBuffer (VOID *Buffer, EFI_GUID *DesiredGuid)
{
  EFI_STATUS                  Status;
  VOID                       *FvBuffer;
  VOID                       *FvBufferTry;
  UINTN                       FvSize, SectionUiSize;
  EFI_USER_INTERFACE_SECTION *SectionUi;

  FvBuffer = NULL;
  FvBufferTry = NULL;
  FvSize = 0;
  SectionUi = NULL;
  SectionUiSize = 0;

  Status = ProcessFvImageFile (Buffer);
  if (EFI_ERROR(Status))
    DEBUG((EFI_D_WARN, "Could not process FV in buffer\n"));

  //check if tools fv is mounted
  if (GuidedFvIsMounted(DesiredGuid))
    return EFI_SUCCESS;

  BdsConnectAllDrivers ();

  // Search for Tools FV in all mounted FVs
  Status = LoadFileFromGuidedFv(NULL,
                                TOOLS_FV_FILENAME,
                                (UINT8 **) &FvBuffer,
                                &FvSize);

  if (EFI_ERROR(Status) || (FvBuffer == NULL))
  {
    DEBUG((EFI_D_WARN, "%a not found in any loaded FVs\n", TOOLS_FV_FILENAME));
    return Status;
  }

  SectionUi = (EFI_USER_INTERFACE_SECTION *) FvBuffer;

  while ((SectionUi->CommonHeader.Type != EFI_SECTION_FIRMWARE_VOLUME_IMAGE) &&
         (SectionUi->CommonHeader.Type != 0))
  {
    if (SectionUi->CommonHeader.Type == EFI_SECTION_USER_INTERFACE)
      SectionUiSize = sizeof(*SectionUi) + StrSize(SectionUi->FileNameString);
    else 
      SectionUiSize = EXTRACT_SECTION_UI_SIZE(SectionUi);

    SectionUi = (EFI_USER_INTERFACE_SECTION *) ((UINTN) SectionUi + SectionUiSize);
  }

  FvBufferTry = (VOID *) ((UINTN) SectionUi + SectionUiSize);

  Status = ProcessFvImageFile(FvBufferTry);
  if (EFI_ERROR(Status))
    DEBUG((EFI_D_WARN, "Could not mount tools FV from FV\n"));

  return Status;
}

/* Load the debug FV from the partition into memory and mount */
EFI_STATUS LoadDebugToolsFv (VOID)
{
  EFI_STATUS Status = EFI_NOT_FOUND;
  VOID *ReadBuffer = NULL;
  MemRegionInfo RamFv; 

  Status = GetMemRegionInfoByName("FV Region", &RamFv);
  if (Status == EFI_SUCCESS)
  {
    Status = MountFvFromBuffer ( (VOID *) RamFv.MemBase, &gToolsFvGuid);
    if (Status == EFI_SUCCESS)
    {
      DEBUG((EFI_D_ERROR, "FV mounted from memory\n"));
      return Status;
    }
  }

  Status = ReadFromToolsPartition (&ReadBuffer);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, "Failed to read from specified tools partition"));
    return Status;
  }

  Status = MountFvFromBuffer (ReadBuffer, &gToolsFvGuid);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, "Failed to mount FV from partition\n"));
    return Status;
  }

  BdsConnectAllDrivers ();
  DEBUG((EFI_D_ERROR, "Successfully loaded Tools FV\n"));
  return EFI_SUCCESS;
}

