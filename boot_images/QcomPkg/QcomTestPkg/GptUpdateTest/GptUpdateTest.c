/**
 * @file GptUpdateTest.c
 *
 * GPT partition update test
 *  
 * Copyright (c) 2016, Qualcomm Technologies, Inc. All rights reserved.
 */

 /*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/07/16   bh      Initial revision

=============================================================================*/

#include <Library/QcomLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TestInterface.h>

#define MAX_NUM_PARTIS 128
#define GPT_MAIN_SIZE 34*512
#define GPT_BCKP_SIZE 33*512

CHAR8 org_main[GPT_MAIN_SIZE];
CHAR8 org_bak[GPT_BCKP_SIZE];
extern CHAR8 new_main[GPT_MAIN_SIZE];
extern CHAR8 new_bak[GPT_BCKP_SIZE];

extern EFI_GUID gEfiUfsLU1Guid;
extern EFI_GUID gEfiUfsLU4Guid;
extern EFI_GUID gBlockIoRefreshGuid;
extern EFI_GUID gEfiPlatPartitionTypeGuid;
extern EFI_GUID gEfiSimpleFileSystemProtocolGuid;
extern EFI_GUID gEfiEmmcUserPartitionGuid;

EFI_EVENT BlockIoRefreshEvent;
// EFI_FILE_HANDLE FileHandle;
STATIC HandleInfo HandleInfoList[MAX_NUM_PARTIS];

STATIC
UINTN
CountPartitions (VOID)
{
  EFI_STATUS               Status;  
  UINT32                   MaxHandles;
  PartiSelectFilter        HandleFilter;
  UINT32                   Attribs = 0;
  UINTN                    HandleCount = 0;

  Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE;

  MaxHandles = sizeof(HandleInfoList) / sizeof(*HandleInfoList);
  HandleFilter.PartitionType = 0;
  HandleFilter.VolumeName = 0;
  HandleFilter.RootDeviceType = &gEfiEmmcUserPartitionGuid;

  Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
  if (EFI_ERROR (Status) || MaxHandles == 0)
  {
    MaxHandles = sizeof(HandleInfoList) / sizeof(*HandleInfoList);
    HandleFilter.PartitionType = 0;
    HandleFilter.VolumeName = 0;
    HandleFilter.RootDeviceType = &gEfiUfsLU1Guid;
  
    Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
    if (EFI_ERROR (Status))
      return 0;

    HandleCount += MaxHandles;

    MaxHandles = (sizeof(HandleInfoList) / sizeof(*HandleInfoList));
    HandleFilter.PartitionType = 0;
    HandleFilter.VolumeName = 0;
    HandleFilter.RootDeviceType = &gEfiUfsLU4Guid;
  
    Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
    if (EFI_ERROR (Status))
      return 0;

    HandleCount += MaxHandles;
  }
  else
  {
    HandleCount = MaxHandles;
  }

  return HandleCount;
}

EFI_STATUS
OpenFatFile(
  IN  CHAR16           *Path,
  IN  EFI_GUID         *PartitionType,
  IN  UINT64           OpenMode,
  OUT EFI_FILE_HANDLE  *FileHandle
  )
{
  EFI_STATUS                        Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Volume;
  EFI_FILE_HANDLE                   RootFileHandle;
  UINT32                            MaxHandles;
  UINT32                            Attrib = 0;
  PartiSelectFilter                 HandleFilter;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);

  if ( (Path == NULL) || (PartitionType == NULL) || (FileHandle == NULL) ) 
    return EFI_INVALID_PARAMETER;

  Attrib |= BLK_IO_SEL_PARTITIONED_GPT;
  Attrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attrib |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;
  Attrib |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;

  HandleFilter.PartitionType = PartitionType;
  HandleFilter.RootDeviceType = 0;
  HandleFilter.VolumeName = 0;
  HandleFilter.PartitionLabel = 0;

  // Returns the IO Block handle
  Status = GetBlkIOHandles(Attrib,
                           &HandleFilter,
                           HandleInfoList,
                           &MaxHandles
                           );

  if(!EFI_ERROR(Status)) {
    if(MaxHandles == 0)
      return EFI_NO_MEDIA;

    if(MaxHandles != 1) {
      // Unable to deterministically load from single partition
      DEBUG(( EFI_D_INFO, "Multipe partitions found for file\n"));
      return EFI_LOAD_ERROR;
    }
  }

  // Find the file system interface to the device
  Status = gBS->HandleProtocol(HandleInfoList[0].Handle,
                               &gEfiSimpleFileSystemProtocolGuid,
                               (VOID *) &Volume
                               );
  if(EFI_ERROR(Status))
    return Status;

  // Open the root directory of the volume
  Status = Volume->OpenVolume(Volume,
                              &RootFileHandle
                              );
  if(EFI_ERROR(Status) || (RootFileHandle == NULL))
    return Status;

  Status = RootFileHandle->Open(RootFileHandle,
                                FileHandle,
                                Path,
                                OpenMode,
                                0
                                );
  if(EFI_ERROR(Status))
    *FileHandle = NULL;

  return Status;
}

STATIC VOID GetFatFileHandle(CHAR16 *Path, 
                             EFI_GUID *PartitionType, 
                             OUT EFI_FILE_HANDLE *FileHandle)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = OpenFatFile(Path,
                       PartitionType,
                       EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                       FileHandle);

  if (EFI_ERROR(Status))
    DEBUG((EFI_D_ERROR, "Unable to get file handle, Status=0x%x\n", Status));

}

STATIC EFI_STATUS WriteFatFileHandle(EFI_FILE_HANDLE FileHandle)
{
  EFI_STATUS Status = EFI_SUCCESS;
  CHAR16 *Buffer = L"TestingWrite\n";
  UINTN Length = StrSize(Buffer);
  
  if (!FileHandle)
    return EFI_NOT_FOUND;

  Status = FileHandle->Write(FileHandle,
                             &Length,
                             (UINT8 *) Buffer);
  if (EFI_ERROR(Status))
    DEBUG((EFI_D_ERROR, "Unable to write file handle, Status=0x%x\n", Status));

  return Status;
}

VOID
BlkIoRefDummy (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  return;
}

STATIC VOID UpdateGptPartition (CHAR8 *gpt_main, CHAR8 *gpt_bak, UINT8 DoBackup)
{
  EFI_STATUS  Status;
  UINT32 MaxHandles;
  UINT32 Attribs = 0;
  PartiSelectFilter HandleFilter;
  EFI_BLOCK_IO_PROTOCOL *BlkIo;

  DEBUG((EFI_D_ERROR, "Updating partitions\n"));

  MaxHandles = sizeof(HandleInfoList) / sizeof(*HandleInfoList); 

  Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE;
  Attribs |= BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY;
  
  HandleFilter.PartitionType = 0;
  HandleFilter.VolumeName = 0;
  HandleFilter.RootDeviceType = &gEfiEmmcUserPartitionGuid;
  
  Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoList, &MaxHandles);

  if (EFI_ERROR(Status) || MaxHandles != 1)
  {
    DEBUG((EFI_D_ERROR, "Updating: Couldn't get user partition\n"));
    return;
  }

  BlkIo = HandleInfoList[0].BlkIo;

  if (DoBackup)
  {
    // Read current GPT main and backup sectors
    Status = BlkIo->ReadBlocks(BlkIo,
                               BlkIo->Media->MediaId,
                               0,
                               GPT_MAIN_SIZE,
                               (VOID *) org_main);
    if (EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "Could not save original GPT main header\n"));
      return;
    }

    Status = BlkIo->ReadBlocks(BlkIo,
                               BlkIo->Media->MediaId,
                               BlkIo->Media->LastBlock-32,
                               GPT_BCKP_SIZE,
                               (VOID *) org_bak);
    if (EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "Could not save original GPT backup header\n"));
      return;
    }

  }

  // Write new GPT partition information
  Status = BlkIo->WriteBlocks(BlkIo,
                              BlkIo->Media->MediaId,
                              0,
                              GPT_MAIN_SIZE,
                              (VOID *) gpt_main);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Could not write to storage\n"));
    return;
  }

  Status = BlkIo->WriteBlocks(BlkIo,
                              BlkIo->Media->MediaId,
                              BlkIo->Media->LastBlock-32,
                              GPT_BCKP_SIZE,
                              (VOID *) gpt_bak);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Could not write to storage\n"));
    return;
  }

  // Signal event to refresh block IO handles

  Status = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, 
                              TPL_CALLBACK, 
                              BlkIoRefDummy,
                              NULL, 
                              &gBlockIoRefreshGuid,
                              &BlockIoRefreshEvent);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Could not create event\n"));
    return;
  }
  Status = gBS->SignalEvent(BlockIoRefreshEvent);
  
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Could not create signal\n"));
    return;
  }
}

UINTN
CheckFatFilesystems(VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_HANDLE *HandleBuffer;
  UINTN HandleCount;

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "%d FAT filesystems found, Status: 0x%x\n", HandleCount, Status));
  }
  return HandleCount;
}

#define PERFORM_BACKUP 1
#define SKIP_BACKUP 0

EFI_STATUS
EFIAPI
GptUpdateTestEntryPoint ( IN EFI_HANDLE         ImageHandle,
                          IN EFI_SYSTEM_TABLE   *SystemTabloe)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_FILE_HANDLE FileHandle;
  UINTN      Before = 0, After = 0, FatExists = 0;

  TEST_START("GptUpdateTest");

  FatExists = CheckFatFilesystems();

  if (FatExists)
  {
    GetFatFileHandle(L"TestFile.txt", &gEfiPlatPartitionTypeGuid, &FileHandle);
    WriteFatFileHandle(FileHandle);
  }

  Before = CountPartitions();
  UpdateGptPartition(new_main, new_bak, PERFORM_BACKUP);
  After = CountPartitions();

  DEBUG((EFI_D_ERROR, "Before: %d partitions\nAfter: %d partitions\n", Before, After));
  if (Before != After)
    DEBUG((EFI_D_ERROR, "\nSUCCESS: Expected number of partitions to change\n\n"));
  else
  {
    DEBUG((EFI_D_ERROR, "\nFAILED: Expected number of partitions to change\n\n"));
    TestStatus("GptUpdateTest", EFI_LOAD_ERROR);
    TEST_STOP("GptUpdateTest");
    return EFI_LOAD_ERROR;
  }
  
  if (FatExists)
  {
    Status = WriteFatFileHandle(FileHandle);
    if (Status == EFI_MEDIA_CHANGED)
      DEBUG((EFI_D_ERROR, "\nSUCCESS: Expected stale handle write to fail\n\n"));
    else
    {
      DEBUG((EFI_D_ERROR, "\nFAILED: Stale handle used to write, corruptoin could occur\n\n"));
      TestStatus("GptUpdateTest", EFI_LOAD_ERROR);
      TEST_STOP("GptUpdateTest");
      return EFI_LOAD_ERROR;
    }
  }

  DEBUG((EFI_D_ERROR, "Restoring original GPT"));

  Before = CountPartitions();
  UpdateGptPartition(org_main, org_bak, SKIP_BACKUP);
  After = CountPartitions();

  DEBUG((EFI_D_ERROR, "Before: %d partitions\nAfter: %d partitions\n", Before, After));
  if (Before != After)
    DEBUG((EFI_D_ERROR, "\nSUCCESS: Expected number of partitions to change\n\n"));
  else
  {
    DEBUG((EFI_D_ERROR, "\nFAILED: Expected number of partitions to change\n\n"));
    TestStatus("GptUpdateTest", EFI_LOAD_ERROR);
    TEST_STOP("GptUpdateTest");
    return EFI_LOAD_ERROR;
  }


  TestStatus("GptUpdateTest", EFI_SUCCESS);
  TEST_STOP("GptUpdateTest");

  return EFI_SUCCESS;
}
