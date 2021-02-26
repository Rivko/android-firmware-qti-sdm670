/** @file CacheTest.c
   
  Profile Cache

  Copyright (c) 2016-2017, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 03/01/17   vk      Print absolute time
 12/20/16   vk      Initial Revision

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ArmLib.h>
#include <Library/CacheMaintenanceLib.h>

#include <Library/QcomBaseLib.h>
#include <Library/TestInterface.h>

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
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  #define WRITE_SIZE 350
  #define RUN_COUNT_SMALL 100
  #define RUN_COUNT_BIG   1000

  UINT32 startTime = 0, endTime = 0;

  EFI_STATUS  Status = EFI_SUCCESS;
  VOID* TestBuffer = NULL;
  UINT32 i = 0;

  TestBuffer = AllocatePages(SIZE_16MB/EFI_PAGE_SIZE);
  if (TestBuffer == NULL)
    return EFI_OUT_OF_RESOURCES;

#if 0  
  ArmCleanInvadlidateDataCache();
  ArmCleanInvadlidateInstructionCache();

  SetMem (TestBuffer, SIZE_1MB, 0);
#endif

  LogMsg("Start Invalidate ");
  startTime = GetTimerCountus();
  for (i = 0; i < RUN_COUNT_SMALL; i++)
  {
    SetMem (TestBuffer, WRITE_SIZE, i);     	  
    WriteBackInvalidateDataCacheRange (TestBuffer, WRITE_SIZE);
  }
  endTime = GetTimerCountus();
  LogMsg("End   Invalidate ");
  
  DEBUG((EFI_D_WARN, "Time:%d uS, count: %d\n\n", endTime-startTime, RUN_COUNT_SMALL));
  
  LogMsg("Start Invalidate ");
  startTime = GetTimerCountus();
  for (i = 0; i < RUN_COUNT_BIG; i++)
  {
    SetMem ( (VOID*)((UINTN)TestBuffer+SIZE_8MB), WRITE_SIZE, i);     	  
    WriteBackInvalidateDataCacheRange ( (VOID*)((UINTN)TestBuffer+SIZE_8MB), WRITE_SIZE);
  }
  endTime = GetTimerCountus();
  LogMsg("End   Invalidate ");

  DEBUG((EFI_D_WARN, "Time:%d uS, count: %d\n", endTime-startTime, RUN_COUNT_BIG));

  return Status;
}

