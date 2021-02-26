/** @file UnitTest.c
   
  Generic Unit Tests, this can also be used as regression test.

  Copyright (c) 2011-2012, 2019 by Qualcomm Technologies, Inc. All Rights Reserved
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
 01/31/13   vk      Fix warning
 01/10/13   niting  Removed data manager
 04/10/12   jz      Fixed klockwork warnings
 11/04/11   yg      Created

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFITzeLoader.h>

#define RUN_TEST_AND_REPORT(test_fn,namestring)         \
      do {                                              \
        EFI_STATUS   Status;                            \
                                                        \
        TEST_START(namestring);                         \
        Status = test_fn (ImageHandle, SystemTable);    \
        TestStatus(namestring, Status);                 \
        TEST_STOP(namestring);                          \
      }while (0)

EFI_GUID TzExecPartitionType = { 0x69B4201F, 0xA5AD, 0x45EB, { 0x9F, 0x49, 0x45, 0xB3, 0x8C, 0xCD, 0xAE, 0xF5}};

extern EFI_GUID gEfiEmmcUserPartitionGuid;
static PartiSelectFilter HandleFilter;
static HandleInfo HandleInfoList[16];

static 
EFI_STATUS
TestGetBlkIoHandle (  
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  UINT32                   Attribs = 0;
  UINT32                   MaxHandles, MaxBlocks;
  EFI_BLOCK_IO_PROTOCOL    *BlkIo;
  EFI_STATUS               Status;
  
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE;
//  Attribs |= BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.PartitionType = 0;
  HandleFilter.RootDeviceType = &gEfiEmmcUserPartitionGuid;
//  HandleFilter.RootDeviceType = &gEfiSdRemovableGuid
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);

  if ((Status != EFI_SUCCESS) || (MaxHandles == 0))
    return EFI_NOT_FOUND;

  /* Get the Block IO protocol instance from the handle */
  BlkIo = HandleInfoList[0].BlkIo;

  ASSERT_EFI_ERROR (Status);

  MaxBlocks = (UINT32)BlkIo->Media->LastBlock;

  AsciiPrint ("Max Blocks : %X", MaxBlocks);

  return EFI_SUCCESS;
}

#if 0
static 
EFI_BLOCK_IO_PROTOCOL  *
GetRootDevHandle (void){
  UINT32                   Attribs = 0;
  UINT32                   MaxHandles, MaxBlocks;
  EFI_BLOCK_IO_PROTOCOL    *BlkIo;
  EFI_STATUS               Status;
  
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
//  Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE;
  Attribs |= BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.PartitionType = 0;
  HandleFilter.VolumeName = 0;
  HandleFilter.RootDeviceType = &gEfiEmmcUserPartitionGuid;
//  HandleFilter.RootDeviceType = &gEfiSdRemovableGuid

  Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);

  if ((Status != EFI_SUCCESS) || (MaxHandles == 0))
    return NULL;

  /* Get the Block IO protocol instance from the handle */
  return HandleInfoList[0].BlkIo;
}
#endif

EFI_STATUS
TestTZeLoader (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS  Status;
  EFI_TZE_LOADER_PROTOCOL *TzeLoader;
  UINT32 AppID = 0;

  Status = gBS->LocateProtocol (&gEfiTzeLoaderProtocolGuid, NULL, (VOID**)&TzeLoader);
  if (Status == EFI_SUCCESS) {
   Status = TzeLoader->LoadImageFromFile (TzeLoader, "fs0:\\easecapp.mbn", &AppID);
   if (Status != 0)
     Status = TzeLoader->LoadImageFromFile (TzeLoader, "fs1:\\easecapp.mbn", &AppID);
     if (Status != 0)
       Status = TzeLoader->LoadImageFromPartition (TzeLoader, &TzExecPartitionType, &AppID);
  }
  return Status;
}

#if 0
typedef struct {
   DataStoreHandle  Handle;
   UINT8*           DataStorePtr;
   UINT32           BlkCnt;
}DataStoreDetailsType;

static DataStoreDetailsType DHandles[6];

static int TableNotify (VOID* Ctxt, DataStoreHandle Handle, TableType TableID)
{
  UINT8* DataStorePtr;
  UINT32 Offset, BlkCnt;

  Offset = (UINT32)Ctxt;

  if (Offset >= 6)
    return -1;

  if (DataStoreGetBuffer (Handle, &DataStorePtr, &BlkCnt) < 0)
    return -1;

  DHandles[Offset].Handle = Handle;
  DHandles[Offset].DataStorePtr = DataStorePtr;
  DHandles[Offset].BlkCnt = BlkCnt;

  return 0;
}

static UINT8 SyncDataBuffer[256 * 1024];
static EFI_BLOCK_IO_PROTOCOL  *RootBlkIo = NULL;

static VOID ChangeData (UINT32 HandleOffset)
{
  int i;
  UINT32 TblSize, Token, *DPtr = DHandles[HandleOffset].DataStorePtr;

  for (i = 0; i < DHandles[HandleOffset].BlkCnt * BLOCK_SIZE / 4; ++i)
  {
    DPtr[i] = (HandleOffset << 24) | (DPtr[i] + 1);
  }
}

static VOID SyncTestTable (UINT32 TableID)
{
  UINT32 TblSize, Token, *DPtr;
  DataPartitionRecord PhyParams;
  int i;

  /* Now get the sync data to sync */
  TblSize = sizeof(SyncDataBuffer);
  DPtr = SyncDataBuffer;
  for (i = 0; i< TblSize / 4; ++i, ++DPtr)
    *DPtr = 0;

  GetTableSyncData (TableID, SyncDataBuffer, &TblSize, &PhyParams, &Token, NULL, 0);

  if (RootBlkIo == NULL)
    RootBlkIo = GetRootDevHandle ();

  if (RootBlkIo)
  {
  i = RootBlkIo->WriteBlocks (RootBlkIo, RootBlkIo->Media->MediaId, PhyParams.SectorStart, TblSize, SyncDataBuffer);

//  UpdateSyncResult (Token, ResultPkt, PktSize, 0);  // for RPMB
  UpdateSyncResult (Token, NULL, 0, i);   // For non RPMB
  }
}

EFI_STATUS
TestVarTables (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  int i;

//  RuntimeDriverInitializeCrc32Table ();

  /* Register for the table callback */
  DataStoreRegisterTableCallback (TableNotify, (VOID*)0, TABLE_ID_BS_NV_S);
  DataStoreRegisterTableCallback (TableNotify, (VOID*)1, TABLE_ID_BS_NV_NS);
  DataStoreRegisterTableCallback (TableNotify, (VOID*)2, TABLE_ID_RT_NV_S);
  DataStoreRegisterTableCallback (TableNotify, (VOID*)3, TABLE_ID_RT_NV_NS);
  DataStoreRegisterTableCallback (TableNotify, (VOID*)4, TABLE_ID_FTPM);

  /* Load all the tables and register them to  */
  InitializeDataTables ();

  for (i=0; i<10; ++i)
  {
    ChangeData (1); ChangeData (1); ChangeData (1); 
    SyncTestTable (1);
    ChangeData (1); ChangeData (1); 
    SyncTestTable (1);

    ChangeData (2); ChangeData (2); 
    SyncTestTable (2);
    ChangeData (2); ChangeData (2); ChangeData (2); ChangeData (2); 
    SyncTestTable (2);

    ChangeData (1); ChangeData (1); 
    SyncTestTable (1);
    ChangeData (2); 
    SyncTestTable (2);
  }

  return Status;
}
#endif 

EFI_STATUS
TestTemplate (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
          
  return Status;
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
UnitTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
#if 0
  RUN_TEST_AND_REPORT (TestVarTables, "TZ Variable Tables Test");
#endif

  RUN_TEST_AND_REPORT (TestGetBlkIoHandle, "BlkIo Handle Select Test");

  RUN_TEST_AND_REPORT (TestTZeLoader, "TZ Exec Loader Test");
 
  return EFI_SUCCESS;
}
