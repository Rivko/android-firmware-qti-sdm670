/**@file

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/RamDiskLib.h>

#include <IndustryStandard/Mbr.h>

//
// Extract UINT32 from char array
//
#define UNPACK_UINT32(a) (UINT32)( (((UINT8 *) a)[0] <<  0) |    \
                                   (((UINT8 *) a)[1] <<  8) |    \
                                   (((UINT8 *) a)[2] << 16) |    \
                                   (((UINT8 *) a)[3] << 24) )

/**
  Performs additional actions after a PE/COFF image has been loaded and relocated.

  If ImageContext is NULL, then ASSERT().

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that has already been loaded and relocated.

**/
EFI_STATUS
EFIAPI
ValidateRamDisk (
  IN  UINT8 *ImageLocation,
  OUT UINTN *ImageSize
)
{
  EFI_STATUS Status;
  UINTN      Index;
  MASTER_BOOT_RECORD          *ProtectiveMbr = (MASTER_BOOT_RECORD *)(VOID *)ImageLocation;
  EFI_PARTITION_TABLE_HEADER  *PartHdr = (EFI_PARTITION_TABLE_HEADER *)(VOID *)(ImageLocation + 512);

  //
  // Verify that the Protective MBR is valid
  //
  for (Index = 0; Index < MAX_MBR_PARTITIONS; Index++) {
    if (ProtectiveMbr->Partition[Index].BootIndicator == 0x00 &&
        ProtectiveMbr->Partition[Index].OSIndicator == PMBR_GPT_PARTITION &&
        UNPACK_UINT32 (ProtectiveMbr->Partition[Index].StartingLBA) == 1
        ) {
      break;
    }
  }
  if (Index == MAX_MBR_PARTITIONS) {
    Status = EFI_NOT_FOUND;
    goto Done;
  }

  if ((PartHdr->Header.Signature != EFI_PTAB_HEADER_ID) ||
      PartHdr->MyLBA != 1 ||
      (PartHdr->SizeOfPartitionEntry < sizeof (EFI_PARTITION_ENTRY))
      ) {
    DEBUG ((EFI_D_INFO, "Invalid efi partition table header\n"));
    return EFI_NOT_FOUND;
  }

  if (NULL != ImageSize) {
    *ImageSize = (PartHdr->AlternateLBA + 1) * RAMDISK_BLOCK_SIZE;
  }
  Status = EFI_SUCCESS;

Done:
  return Status;
}

