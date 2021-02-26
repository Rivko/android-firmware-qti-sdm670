/** @file FBPTTest.c
   
  Test for UEFI FBPT support.

  Copyright (c) 2012,2014, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 10/27/14   jb      Change FBPT base to uint64
 05/08/14   vk      Add 64 bit support  
 07/17/12   vishalo Read FBPTBase from PCD, cleanup
 07/06/12   vishalo Initial Revision
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TestInterface.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/ProcAsmLib.h>
#include <Library/FBPT.h>

STATIC UINTN FBPTPayLoadMemoryBase;

/**
  Initializes global FBPTMemoryBase for handlers
  Must be called once before handlers update entries

  @retval EFI_SUCCESS  FBPTMemoryBase initialized     
**/

EFI_STATUS  
InitFBPTPayLoadMemBase(VOID)
{
  FBPTPayLoadMemoryBase =  (UINTN)PcdGet64(PcdFBPTPayloadBaseOffset);
  return EFI_SUCCESS;
}

EFI_STATUS
CallExitBS(VOID)
{
  EFI_STATUS              Status;
  UINTN                   MemoryMapSize = 0;
  EFI_MEMORY_DESCRIPTOR   *MemoryMap = NULL;
  UINTN                   MapKey = 0;
  UINTN                   DescriptorSize = 0;
  UINT32                  DescriptorVersion = 0;
  UINTN                   Pages;

  do {
    Status = gBS->GetMemoryMap (
                    &MemoryMapSize,
                    MemoryMap,
                    &MapKey,
                    &DescriptorSize,
                    &DescriptorVersion
                    );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Pages = EFI_SIZE_TO_PAGES (MemoryMapSize) + 1;
      MemoryMap = AllocatePages (Pages);
    
      /*
       * Get System MemoryMap
       */
      Status = gBS->GetMemoryMap (
                      &MemoryMapSize,
                      MemoryMap,
                      &MapKey,
                      &DescriptorSize,
                      &DescriptorVersion
                      );
      //AsciiPrint( "ExitBootServices ...\n"));
      /* Don't do anything between the GetMemoryMap() and ExitBootServices() */
      if (!EFI_ERROR (Status)) {
        Status = gBS->ExitBootServices (gImageHandle, MapKey);
        if (EFI_ERROR (Status)) {
          FreePages (MemoryMap, Pages);
          MemoryMap = NULL;
          MemoryMapSize = 0;
        }
      }
    }
  } while (EFI_ERROR (Status));

  return Status;
}

EFI_STATUS
FBPTTest(VOID)
{
  STATIC volatile UefiPerfRecord_t *UefiPerfRecord;
  STATIC UINT32 NotFirstRun; 
  
  InitFBPTPayLoadMemBase();
  
  UefiPerfRecord =  (UefiPerfRecord_t*) FBPTPayLoadMemoryBase;

  if(UefiPerfRecord == NULL)
    return EFI_UNSUPPORTED;

  if(NotFirstRun == 0) 
  {
     /*FBPT Header */
    AsciiPrint("PMon Cycle Count                                            = %d\n", ReadCycleCntReg() );
    AsciiPrint( "UefiPerfRecord->UefiFBPTHeader.Signature[0]                = %c\n", UefiPerfRecord->UefiFBPTHeader.Signature[0] );
    AsciiPrint( "UefiPerfRecord->UefiFBPTHeader.Signature[1]                = %c\n", UefiPerfRecord->UefiFBPTHeader.Signature[1] );
    AsciiPrint( "UefiPerfRecord->UefiFBPTHeader.Signature[2]                = %c\n", UefiPerfRecord->UefiFBPTHeader.Signature[2] );
    AsciiPrint( "UefiPerfRecord->UefiFBPTHeader.Signature[3]                = %c\n", UefiPerfRecord->UefiFBPTHeader.Signature[3] );
    AsciiPrint( "UefiPerfRecord->UefiFBPTHeader.Length                      = %d\n\n", UefiPerfRecord->UefiFBPTHeader.Length );
  
    /* Perf Data Record */
    AsciiPrint( "UefiPerfRecord->UefiFBBPDataRecord.PerformanceRecordType   = %d\n", UefiPerfRecord->UefiFBBPDataRecord.PerformanceRecordType);
    AsciiPrint( "UefiPerfRecord->UefiFBBPDataRecord.RecordLength            = %d\n", UefiPerfRecord->UefiFBBPDataRecord.RecordLength);
    AsciiPrint( "UefiPerfRecord->UefiFBBPDataRecord.Revision                = %d\n", UefiPerfRecord->UefiFBBPDataRecord.Revision);
    AsciiPrint( "UefiPerfRecord->UefiFBBPDataRecord.Reserved                = %d\n", UefiPerfRecord->UefiFBBPDataRecord.Reserved);
  
    AsciiPrint( "UefiPerfRecord->UefiFBBPDataRecord.ResetEnd                = %016ld\n", UefiPerfRecord->UefiFBBPDataRecord.ResetEnd);
    AsciiPrint( "UefiPerfRecord->UefiFBBPDataRecord.OSLoaderLoadImageStart  = %016ld\n", UefiPerfRecord->UefiFBBPDataRecord.OSLoaderLoadImageStart);
    AsciiPrint( "UefiPerfRecord->UefiFBBPDataRecord.OSLoaderStartImageStart = %016ld\n", UefiPerfRecord->UefiFBBPDataRecord.OSLoaderStartImageStart);
    AsciiPrint( "UefiPerfRecord->UefiFBBPDataRecord.ExitBootServicesEntry   = %016ld\n", UefiPerfRecord->UefiFBBPDataRecord.ExitBootServicesEntry);
    AsciiPrint( "UefiPerfRecord->UefiFBBPDataRecord.ExitBootServicesExit    = %016ld\n", UefiPerfRecord->UefiFBBPDataRecord.ExitBootServicesExit);

    AsciiPrint( "PMon Cycle Count                                           = %d\n", ReadCycleCntReg() );
    NotFirstRun = 1;
  }

  return EFI_SUCCESS;
}



/**
  Entry point for the application

  @param[in] ImageHandle    Image handle 
  @param[in] SystemTable    Pointer to the System Table
  
  @retval EFI_SUCCESS       Execution successful
  @retval other             Error occurred

**/
EFI_STATUS
EFIAPI
FBPTTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  volatile UINTN i = 0xFFFFFFFF;
  TEST_START("FBPTTest");
  Status = FBPTTest();
  if(Status != EFI_SUCCESS) {
    goto ExitError;
  }

  AsciiPrint("Calling ExitBootServices ....\n\r"); 

  Status = CallExitBS();
  if(Status != EFI_SUCCESS) {
    goto ExitError;
  }
  
  Status = FBPTTest();
  if(Status != EFI_SUCCESS) {
    AsciiPrint( "Error setting FBPT \n\r" );
    TestStatus("FBPTTest", Status);
    TEST_STOP("FBPTTest");
    goto ExitError;
  }

  /* Loop for a while, before calling reset */
  while(i--);

  /* Do a cold reset */
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

#if 0
  TestStatus("FBPTTest", Status);
  TEST_STOP("FBPTTest");
#endif  
  
  return EFI_SUCCESS;
ExitError:
  return EFI_UNSUPPORTED;
}
