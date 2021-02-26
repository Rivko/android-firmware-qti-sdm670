/**
 * @file EraseTest.c
 *
 * Test block erase protocol
 *  
 * Copyright (c) 2016, Qualcomm Technologies, Inc. All rights reserved.
 */

/*=============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/14/16   bh      Initial revision

 ============================================================================*/

#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/EFIEraseBlock.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TestInterface.h>
#include <Library/MemoryAllocationLib.h>

extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiEmmcBootPartition1Guid;
extern EFI_GUID gEfiEmmcBootPartition2Guid;
extern EFI_GUID gEfiEmmcGppPartition1Guid;
extern EFI_GUID gEfiSdRemovableGuid;
extern EFI_GUID gEfiUfsLU0Guid;
extern EFI_GUID gEfiUfsLU1Guid;
extern EFI_GUID gEfiUfsLU2Guid;
extern EFI_GUID gEfiUfsLU3Guid;
extern EFI_GUID gEfiUfsLU4Guid;
extern EFI_GUID gEfiUfsLU5Guid;
extern EFI_GUID gEfiUfsLU6Guid;
extern EFI_GUID gEfiUfsLU7Guid;

#define MAX_NUM_PARTITIONS      128 
#define MAX_ROOT_PARTITIONS     16
#define MAX_NUM_HANDLES         128
#define PARTITION_NAME_LENGTH   32 
#define WIDECHAR_TO_CHAR_MASK   0x00ff

typedef struct _ROOT_PARTI {
   CONST CHAR8*  Name;
   EFI_GUID*     Guid;
}RootPartitionInfoType;

RootPartitionInfoType  RootPartitionInfo[] = 
{
  {"eMMC User",      &gEfiEmmcUserPartitionGuid},
  {"eMMC Boot1",     &gEfiEmmcBootPartition1Guid},
  {"eMMC Boot2",     &gEfiEmmcBootPartition2Guid},
  {"eMMC GPP",       &gEfiEmmcGppPartition1Guid},
  {"SDCard",         &gEfiSdRemovableGuid},
  {"UFS-LUN 0",      &gEfiUfsLU0Guid},
  {"UFS-LUN 1",      &gEfiUfsLU1Guid},
  {"UFS-LUN 2",      &gEfiUfsLU2Guid},
  {"UFS-LUN 3",      &gEfiUfsLU3Guid},
  {"UFS-LUN 4",      &gEfiUfsLU4Guid},
  {"UFS-LUN 5",      &gEfiUfsLU5Guid},
  {"UFS-LUN 6",      &gEfiUfsLU6Guid},
  {"UFS-LUN 7",      &gEfiUfsLU7Guid},
};

STATIC HandleInfo RootHandleInfoList[16];
STATIC HandleInfo HandleInfoList[MAX_NUM_HANDLES];                   

STATIC UINTN      NumRootPartitions;


EFI_STATUS
EFIAPI
EraseTestEntryPoint ( IN EFI_HANDLE         ImageHandle,
                      IN EFI_SYSTEM_TABLE   *SystemTabloe)
{
  UINT32                Attribs = 0;
  UINT32                MaxHandles;
  UINT32                MaxRootHandles;
  EFI_STATUS            Status;
  PartiSelectFilter     HandleFilter;
  EFI_PARTITION_ENTRY*  PartEntry;
  EFI_BLOCK_IO_PROTOCOL *BlkIo;
  UINT16                i, j;
  EFI_ERASE_BLOCK_PROTOCOL *EraseBlock;
  EFI_ERASE_BLOCK_TOKEN *Token = NULL;
  BOOLEAN               CheckOnes = FALSE;
  UINTN                 Index;
  UINTN                 EventIndex  = 0;

  UINT8 *OriginalBuffer;
  UINT8 *ReadBuffer;
  UINTN PartitionSize = 0;

  TEST_START("EraseTest");
  
  HandleFilter.PartitionType  = 0; 
  HandleFilter.VolumeName     = 0;
  MaxRootHandles              = sizeof(RootHandleInfoList) / sizeof(*RootHandleInfoList);

  NumRootPartitions = sizeof(RootPartitionInfo)/sizeof(*RootPartitionInfo);
  ASSERT (NumRootPartitions < MAX_ROOT_PARTITIONS);

  /* Seach by GUID */
  Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE; 

  for (i = 0; i < NumRootPartitions; ++i)
  { 
    /* Find Root Partitions First */
    Attribs |= BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY;
    HandleFilter.RootDeviceType = RootPartitionInfo[i].Guid;
    GetBlkIOHandles(Attribs, &HandleFilter, RootHandleInfoList, &MaxRootHandles);

    if (MaxRootHandles == 0)
      continue;

    Status = gBS->HandleProtocol (
        RootHandleInfoList[0].Handle,
        &gEfiEraseBlockProtocolGuid,
        (VOID **) &EraseBlock);
    
    if (EFI_ERROR(Status))
      continue;
    
    /* Remove Root-only attribute to find subpartitions */
    Attribs   &= ~BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY;
    HandleFilter.RootDeviceType = RootPartitionInfo[i].Guid;
    MaxHandles = sizeof(HandleInfoList) / sizeof(*HandleInfoList); 
    GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);

    /* check if any subpartitions are found. 1 means only found root */
    if (MaxHandles == 1)
      continue;

    /* Find The Names of each subpartitions */
    for(j = 0; j < MaxHandles; j++)
    {
      Status = gBS->HandleProtocol(HandleInfoList[j].Handle, 
                                   &gEfiEraseBlockProtocolGuid, 
                                   (VOID **) &EraseBlock);
      if (EFI_ERROR (Status))
           continue;  /* Root Partition Found, Skip iteration */

      Status = gBS->HandleProtocol(HandleInfoList[j].Handle, 
                                   &gEfiPartitionRecordGuid, 
                                   (VOID **) &PartEntry);
      if (EFI_ERROR (Status))
           continue;  /* Root Partition Found, Skip iteration */

      BlkIo = HandleInfoList[j].BlkIo;

      // Allocate memory to read full partition
      PartitionSize = (BlkIo->Media->LastBlock + 1) * (BlkIo->Media->BlockSize);
      DEBUG((EFI_D_ERROR, "---------------------\n"));
      DEBUG((EFI_D_ERROR, "Erasing \"%s\": 0x%x Bytes\n",PartEntry->PartitionName, PartitionSize));

      OriginalBuffer = AllocatePool(PartitionSize);
      if (OriginalBuffer == NULL)
      {
        DEBUG((EFI_D_ERROR, "Partition too large, skipping\n"));
        continue;
      }
      ReadBuffer = AllocatePool(PartitionSize);
      if (ReadBuffer == NULL)
      {
        DEBUG((EFI_D_ERROR, "Partition too large, skipping\n"));
        continue;
      }

      // Read original buffer 
      Status = BlkIo->ReadBlocks(BlkIo,
                                 BlkIo->Media->MediaId,
                                 0,
                                 PartitionSize,
                                 (VOID *) OriginalBuffer);
      if (EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "Could not read original buffer\n"));
        goto Exit;
      }
      
      // Erase partition
      Status = EraseBlock->EraseBlocks(BlkIo,
                                       BlkIo->Media->MediaId,
                                       0,
                                       Token,
                                       PartitionSize);
      
      if (Token)
        gBS->WaitForEvent(1, &Token->Event, &EventIndex);
  
      if (EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "Could not erase partition\n"));
        goto Exit;
      }

      
      // Read partition to verify it was erased
      Status = BlkIo->ReadBlocks(BlkIo,
                                 BlkIo->Media->MediaId,
                                 0,
                                 PartitionSize,
                                 (VOID *) ReadBuffer);
      if (EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "Could not read erased partition\n"));
        goto Exit;
      }


      //Check that partition was erased
      CheckOnes = FALSE;
      for (Index = 0; Index < PartitionSize; Index++){
        if (ReadBuffer[Index] != 0)
        {
          CheckOnes = TRUE;
          break;
        }
      }
      if (CheckOnes)
      {
        for (Index = 0; Index < PartitionSize; Index++)
        {
          if (ReadBuffer[Index] != 1)
          {
            DEBUG((EFI_D_ERROR, "Partition not erased\n"));
            goto Exit;
          }
        }
      }


      // Replace original contents
      Status = BlkIo->WriteBlocks(BlkIo,
                                 BlkIo->Media->MediaId,
                                 0,
                                 PartitionSize,
                                 (VOID *) OriginalBuffer);
      if (EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "Could not restore contents to partition\n"));
        goto Exit;
      }

      DEBUG((EFI_D_ERROR, "%s: SUCCESS\n", PartEntry->PartitionName));

      FreePool(OriginalBuffer);
      FreePool(ReadBuffer);

    }         
  }

  TEST_STOP("EraseTest");

  return EFI_SUCCESS;

  Exit:
    FreePool(OriginalBuffer);
    FreePool(ReadBuffer);
    TEST_STOP("EraseTest");
    return Status;
}

