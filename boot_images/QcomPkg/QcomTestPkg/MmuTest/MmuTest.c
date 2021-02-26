/** @file MmuTest.c
   
  Test MMU 

  Copyright (c) 2016-2018 Qualcomm Technologies Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/27/17   vk      Add uncached allocate test
 08/31/16   vk      Initial rev

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/TestInterface.h>
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/ArmLib.h>
#include <Library/ArmMmuLib.h>
#include <Library/CacheMaintenanceLib.h>

/* 1. Allocate aligned 8MB, -> Block entries
 * 2. Set entire region as executable
 * 3. Set 1 page in middle not executable
 * 4. Set starting 2MB as executable
 * 5. Set entire region as executable
 * After each step we write to entire region to verify it is still accessible
 */
EFI_STATUS
TestSetClearNoExec(VOID)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN SzBytes = SIZE_8MB;
  UINTN SzPages = SzBytes/EFI_PAGE_SIZE;
  VOID* TestBuffer = NULL;

  TestBuffer = AllocateAlignedPages(SzPages, SIZE_8MB);

  if (TestBuffer == NULL)
  {
    DEBUG ((EFI_D_ERROR, "Memory Allocation failed !\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Status = ArmClearMemoryRegionNoExec ((UINT64)TestBuffer,  SzBytes);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "XN: 0x%016lx--0x%016lx [Failed]\n", (UINT64)TestBuffer, SzBytes));
  }

  SetMem (TestBuffer, SzBytes,1);

  Status = ArmSetMemoryRegionNoExec ((UINT64)TestBuffer + EFI_PAGE_SIZE, EFI_PAGE_SIZE);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "XN: 0x%016lx--0x%016lx [Failed]\n", (UINT64)TestBuffer + EFI_PAGE_SIZE, EFI_PAGE_SIZE));
  }

  SetMem (TestBuffer, SzBytes, 2);

  Status = ArmClearMemoryRegionNoExec ((UINT64)TestBuffer,  SIZE_2MB);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "XN: 0x%016lx--0x%016lx [Failed]\n", (UINT64)TestBuffer, SIZE_2MB));
  }

  SetMem (TestBuffer, SzBytes, 3);

  Status = ArmClearMemoryRegionNoExec ((UINT64)TestBuffer,  SzBytes);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "XN: 0x%016lx--0x%016lx [Failed]\n", (UINT64)TestBuffer, SzBytes));
  }

  SetMem (TestBuffer, SzBytes, 4);

  return EFI_SUCCESS;
}

/* 1. Allocate aligned 8MB, -> Block entries
 * 2. Set entire region as uncached
 * 3. Set 1 page in middle as write back
 * 4. Set entire 8MB as uncached  
 * After each step we write to entire region to verify iut is still accessible
 */
EFI_STATUS
TestBlockPageBlockConv(VOID)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN SzBytes = SIZE_8MB;
  UINTN SzPages = SzBytes/EFI_PAGE_SIZE;
  EFI_PHYSICAL_ADDRESS TestBuffer = 0;
  VOID *p;

  p = AllocateAlignedPages(SzPages, SIZE_8MB);
  if (p == NULL)
  {
    DEBUG ((EFI_D_ERROR, "Memory Allocation failed !\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  TestBuffer = (EFI_PHYSICAL_ADDRESS) p;

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to allocate memory\n"));
    return Status;
  }

  SetMem (p, SzBytes, 0);

  Status = gDS->SetMemorySpaceAttributes (TestBuffer, SzBytes, EFI_MEMORY_WC);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to set memory attributes !\n"));
    return Status;
  }

  SetMem (p, SzBytes, 1);
  Status = gDS->SetMemorySpaceAttributes (TestBuffer + EFI_PAGE_SIZE, EFI_PAGE_SIZE,  EFI_MEMORY_WB);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to set memory attributes !\n"));
    return Status;
  }

  SetMem (p, SzBytes, 2);

  Status = gDS->SetMemorySpaceAttributes (TestBuffer, SzBytes, EFI_MEMORY_WC);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to set memory attributes !\n"));
    return Status;
  }

  SetMem (p, SzBytes, 3);

  return EFI_SUCCESS;
}


/* 1. Allocate aligned 8MB, -> Block entries
 * 2. Set entire region as uncached
 * 3. Set 1MB ++ 2MB as write back
 * After each step we write to entire region to verify iut is still accessible
 */
EFI_STATUS
TestBlockMissingAlignCheck(VOID)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN SzBytes = SIZE_8MB;
  UINTN SzPages = SzBytes/EFI_PAGE_SIZE;
  EFI_PHYSICAL_ADDRESS TestBuffer = 0;
  VOID *p;

  p = AllocateAlignedPages(SzPages, SIZE_8MB);
  if (p == NULL)
  {
    DEBUG ((EFI_D_ERROR, "Memory Allocation failed !\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  TestBuffer = (EFI_PHYSICAL_ADDRESS) p;

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to allocate memory\n"));
    return Status;
  }

  SetMem (p, SzBytes, 0);

  Status = gDS->SetMemorySpaceAttributes (TestBuffer, SzBytes, EFI_MEMORY_WC);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to set memory attributes !\n"));
    return Status;
  }

  SetMem (p, SzBytes, 1);
  Status = gDS->SetMemorySpaceAttributes (TestBuffer + SIZE_1MB, SIZE_2MB,  EFI_MEMORY_WB);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to set memory attributes !\n"));
    return Status;
  }

  SetMem (p, SzBytes, 2);

  return EFI_SUCCESS;
}

#define ALLOC_SIZE      1024
#define ITERATION_COUNT 2000
EFI_STATUS
UncachedAllocTest (VOID)
{
  VOID *p= NULL;
  UINT32 i = 0;

  for (i = 0; i < ITERATION_COUNT; i++)
  {
    p = UncachedAllocatePool (ALLOC_SIZE);
    if (p == NULL)
    {
      DEBUG ((EFI_D_ERROR, "Memory Allocation failed !\n"));
      return EFI_OUT_OF_RESOURCES;
    }

    SetMem (p, ALLOC_SIZE, 4);
    UncachedFreePool(p);
  }

  for (i = 0; i < ITERATION_COUNT; i++)
  {
    p = UncachedAllocatePool (ALLOC_SIZE*5);
    if (p == NULL)
    {
      DEBUG ((EFI_D_ERROR, "Memory Allocation failed !\n"));
      return EFI_OUT_OF_RESOURCES;
    }
    
    SetMem (p, ALLOC_SIZE, 5);
    UncachedFreePool(p);
  }

  for (i = 0; i < ITERATION_COUNT; i++)
  {
    p = UncachedAllocatePool (ALLOC_SIZE*2);
    if (p == NULL)
    {
      DEBUG ((EFI_D_ERROR, "Memory Allocation failed !\n"));
      return EFI_OUT_OF_RESOURCES;
    }

    SetMem (p, ALLOC_SIZE, 6);
    UncachedFreePool(p);
  }
 
  return EFI_SUCCESS; 
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{

  DEBUG ((EFI_D_ERROR, "Test  : SetClearNoExec\n"));
  TestSetClearNoExec();
  DEBUG ((EFI_D_ERROR, "Result: PASS\n"));

  DEBUG ((EFI_D_ERROR, "Test  : BlockPageBlockConv\n"));
  TestBlockPageBlockConv();
  DEBUG ((EFI_D_ERROR, "Result: PASS\n"));

  DEBUG ((EFI_D_ERROR, "Test  : TestBlockMissingAlignCheck\n"));	
  TestBlockMissingAlignCheck();
  DEBUG ((EFI_D_ERROR, "Result: PASS\n"));
  
  DEBUG ((EFI_D_ERROR, "Test  : UncachedAllocTest\n"));	
  UncachedAllocTest();
  DEBUG ((EFI_D_ERROR, "Result: PASS\n"));
  
  return EFI_SUCCESS;
}

