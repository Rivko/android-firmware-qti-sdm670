/** @file MemTest.c
   
  Tests for Allocate/Free pages/pool and uncached pages/pool.

  Copyright (c) 2011,2016-2018 Qualcomm Technologies, Inc. All rights reserved.
  Portions Copyright (c) 2007, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Portions (C) Copyright 2015 Hewlett Packard Enterprise Development LP<BR>

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
 09/18/17	mt		Added tests for UncachedAlignedRuntimePool and Pages
 04/28/17   pawans  Added Uefi memory use limit
 01/17/13   vk      Fix warning
 06/27/12   jz      Cleanups
 04/04/12   jz      Initial revision

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/PrintLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Base.h>
 
#define MAX_NUM_LOOP   100
#define MAX_LIMIT_RT_RESERVED_ACPI 260
#define PAGES_TO_MEGABYTES(PageCount) EFI_PAGE_SIZE*PageCount/(1024*1024)  

CHAR8 *gMemMapType[] = {
  "reserved",
  "LoaderCode",
  "LoaderData",
  "BS_code   ",
  "BS_data   ",
  "RT_code   ",
  "RT_data   ",
  "available ",
  "Unusable  ",
  "ACPI_recl ",
  "ACPI_NVS  ",
  "MemMapIO  ",
  "MemPortIO ",
  "PAL_code  "
};


STATIC
EFI_STATUS
TestAllocatePages (
  EFI_ALLOCATE_TYPE AllocateType,
  EFI_MEMORY_TYPE   MemoryType
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  UINTN                Index;
  EFI_PHYSICAL_ADDRESS MemAddress = MAX_ADDRESS;

  for (Index=1; Index<=MAX_NUM_LOOP; Index++)
  {
    Status = gBS->AllocatePages(AllocateType, MemoryType, Index, &MemAddress);
    if (EFI_ERROR(Status))
    {
      AsciiPrint("*** Failed to allocate %d pages, Status: 0x%x\n",
                 Index, Status);
      break;
    }

    gBS->SetMem((VOID *)(UINTN)MemAddress, EFI_PAGES_TO_SIZE(Index), 1 << (Index & 0x7));
    Status = gBS->FreePages(MemAddress, Index);
    if (EFI_ERROR(Status))
    {
      AsciiPrint("*** Failed to free %d pages, Status: 0x%x\n",
                 Index, Status);
      break;
    }
  }

  return Status;
}

STATIC
EFI_STATUS
TestAllocatePool (
  EFI_MEMORY_TYPE   MemoryType
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  UINTN                Index;
  VOID                 *pMemBuffer = NULL;

  for (Index=1; Index<=MAX_NUM_LOOP; Index++)
  {
    Status = gBS->AllocatePool(MemoryType, EFI_PAGES_TO_SIZE(Index), &pMemBuffer);
    if (EFI_ERROR(Status))
    {
      AsciiPrint("*** Failed to allocate pool, size %d, Status: 0x%x\n",
                 EFI_PAGES_TO_SIZE(Index), Status);
      break;
    }

    SetMem(pMemBuffer, EFI_PAGES_TO_SIZE(Index), 1 << (Index & 0x7));
    Status = gBS->FreePool(pMemBuffer);
    if (EFI_ERROR(Status))
    {
      AsciiPrint("*** Failed to free pool, size %d, Status: 0x%x\n",
                 EFI_PAGES_TO_SIZE(Index), Status);
      break;
    }
  }

  return Status;
}

STATIC
EFI_STATUS
TestUncachedAllocatePages (
  VOID
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  VOID                 *pMemBuffer = NULL;
  UINTN                Index;

  for (Index=1; Index<=MAX_NUM_LOOP; Index++)
  {
    pMemBuffer = UncachedAllocatePages(Index);
    if (pMemBuffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      AsciiPrint("*** Failed to allocate %d uncached pages, Status = 0x%x\n",
                 Index, Status);
      break;
    }

    SetMem(pMemBuffer, EFI_PAGES_TO_SIZE(Index), 1 << (Index & 0x7));
    UncachedFreePages(pMemBuffer, Index);
  }

  return Status;
}

STATIC
EFI_STATUS
TestUncachedAllocatePool (
  VOID
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  VOID                 *pMemBuffer = NULL;
  UINTN                Index;

  for (Index=1; Index<=MAX_NUM_LOOP; Index++)
  {
    pMemBuffer = UncachedAllocatePool(EFI_PAGES_TO_SIZE(Index));
    if (pMemBuffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      AsciiPrint("*** Failed to allocate uncached pool, size %d, Status = 0x%x\n",
                 EFI_PAGES_TO_SIZE(Index), Status);
      break;
    }

    SetMem(pMemBuffer, EFI_PAGES_TO_SIZE(Index), 1 << (Index & 0x7));
    UncachedFreePool(pMemBuffer);
  }

  return Status;
}

STATIC
EFI_STATUS
TestUncachedAllocateAlignedRuntimePool(
  VOID 
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN Index;
  VOID *pMemBuffer = NULL;

  for (Index=1; Index<=MAX_NUM_LOOP; Index++)
  {
    pMemBuffer = UncachedAllocateAlignedRuntimePool(EFI_PAGES_TO_SIZE(Index), SIZE_4KB);
    if (pMemBuffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      AsciiPrint("*** Failed to allocate uncached pool, size %d, Status = 0x%x\n",
                  EFI_PAGES_TO_SIZE(Index), Status);
      break;
    }

    else if( ( (int)pMemBuffer & (SIZE_4KB-1) ) != 0x0 )
    {
      Status = EFI_OUT_OF_RESOURCES;

      AsciiPrint("*** Incorrect Alignment of Allocated Pool,should have been 4k, "
                  "size %d, Status = 0x%x\n",EFI_PAGES_TO_SIZE(Index), Status);
      break;
    }

    SetMem(pMemBuffer, EFI_PAGES_TO_SIZE(Index), 1 << (Index & 0x7));
    UncachedFreePool(pMemBuffer); 
  }

  if(Status == EFI_SUCCESS) {
    AsciiPrint("===> Allocate/Free Uncached Aligned Runtime Pool with 4k "
                "alignment, Status: 0x%x\n", Status);
  }

  Status = EFI_SUCCESS;

  for (Index=1; Index<=MAX_NUM_LOOP; Index++)
  {
    pMemBuffer = UncachedAllocateAlignedRuntimePool(EFI_PAGES_TO_SIZE(Index), SIZE_8KB);
    if (pMemBuffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      AsciiPrint("*** Failed to allocate uncached pool, size %d, Status = 0x%x\n",
                  EFI_PAGES_TO_SIZE(Index), Status);
      break;
    }

    else if( ( (int)pMemBuffer & (SIZE_8KB-1) ) != 0x0 )
    {
      Status = EFI_OUT_OF_RESOURCES;

      AsciiPrint("*** Incorrect Alignment of Allocated Pool, should have been 8k,"
                  "size %d, Status = 0x%x\n",  EFI_PAGES_TO_SIZE(Index), Status);
      break;
    }

    SetMem(pMemBuffer, EFI_PAGES_TO_SIZE(Index), 1 << (Index & 0x7));
    UncachedFreePool(pMemBuffer); 
  }

  if(Status == EFI_SUCCESS) {
    AsciiPrint("===> Allocate/Free Uncached Aligned Runtime Pool with 8k "
                "alignment, Status: 0x%x\n", Status);
  }

  return Status;
}

/**
  Sum up the UEFI memory use (BS/RT Code/Data) 

  @return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UefiMemMapCmd (
  )
{

  EFI_STATUS            Status;
  EFI_MEMORY_DESCRIPTOR *MemMap = NULL;
  EFI_MEMORY_DESCRIPTOR *OrigMemMap;
  UINTN                 MemMapSize = 0;
  UINTN                 MapKey;
  UINTN                 DescriptorSize;
  UINT32                DescriptorVersion;
  UINT64                PageCount[EfiMaxMemoryType];
  UINTN                 Index;
  UINTN                 CurrentRow;
  UINT64                TotalPages;
  UINT32                UefiMemUseThreshold = 0;
  ZeroMem (PageCount, sizeof (PageCount));

  AsciiPrint ("EFI Memory Map\n");

  // First call is to figure out how big the buffer needs to be
  Status = gBS->GetMemoryMap (&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    // In case the AllocatPool changes the memory map we added in some extra descriptors
    MemMapSize += (DescriptorSize * 0x100);
    OrigMemMap = MemMap = AllocatePool (MemMapSize);
    if (OrigMemMap != NULL) {
      // 2nd time we get the data
      Status = gBS->GetMemoryMap (&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
      if (!EFI_ERROR (Status)) {
        for (Index = 0, CurrentRow = 0; Index < MemMapSize/DescriptorSize; Index++) {
          PageCount[MemMap->Type % EfiMaxMemoryType] += MemMap->NumberOfPages;
          AsciiPrint("index/type = %d : physical start = %x : %,7ld Pages (%,14ld)\n",MemMap->Type, MemMap->PhysicalStart, MemMap-> NumberOfPages, EFI_PAGE_SIZE * (MemMap-> NumberOfPages));
          MemMap = NEXT_MEMORY_DESCRIPTOR (MemMap, DescriptorSize);
        }
     }
	 
     /*
      uefi BS and RT from index 3 to 6 in gMemMapType
     */
     for (Index = 3, TotalPages = 0; Index <= 6; Index++) {
        AsciiPrint ("\n%a %,7ld Pages (%,14ld)", gMemMapType[Index], PageCount[Index], EFI_PAGE_SIZE * PageCount[Index]);
        // Count total memory
        TotalPages += PageCount[Index];
      }

      AsciiPrint("\nTotal Pages for BS and RT = %llu and memory use = %d\n", TotalPages, PAGES_TO_MEGABYTES(TotalPages));
      
      Status = GetConfigValue ("UefiMemUseThreshold", &UefiMemUseThreshold);
      if (Status != EFI_SUCCESS)
      {
        AsciiPrint("\nUefiMemUseThreshold not specified in uefiplat.cfg\n");
      }
      else if ( PAGES_TO_MEGABYTES(TotalPages) >= UefiMemUseThreshold)
      {
        AsciiPrint ("\nWarning: UEFI memory use : %,ld MB (%,ld KB) exceeds threshold %d MB \n", PAGES_TO_MEGABYTES(TotalPages), 1024 * PAGES_TO_MEGABYTES(TotalPages), UefiMemUseThreshold);
        return EFI_OUT_OF_RESOURCES ;
      }

      /* uefi Reserved is index 0 RT is index 5 and 6 and ACPI is index 9 and 10 in gMemMapType */
      TotalPages = PageCount[0] + PageCount[5] + PageCount[6] + PageCount[9] + PageCount[10];
      AsciiPrint("\nTotal Pages for RT and Reserved = %llu and memory use = %d\n", TotalPages, PAGES_TO_MEGABYTES(TotalPages));

      if ( PAGES_TO_MEGABYTES(TotalPages) >= MAX_LIMIT_RT_RESERVED_ACPI)
      {
        AsciiPrint ("Warning: UEFI memory use of RT and Reserved: %,ld MB (%,ld KB) exceeds threshold %d MB \n", PAGES_TO_MEGABYTES(TotalPages), 1024 * PAGES_TO_MEGABYTES(TotalPages), MAX_LIMIT_RT_RESERVED_ACPI);
        return EFI_OUT_OF_RESOURCES ;
      }

      FreePool ( OrigMemMap );

    }
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
MemTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  
  TEST_START("MemTest");

  AsciiPrint("\nMemory Allocation Tests: 1..%d\n", MAX_NUM_LOOP);

  AsciiPrint("\n   Allocate/Free Pages\n");
  Status = TestAllocatePages(AllocateAnyPages, EfiBootServicesData);
  AsciiPrint("===> AllocateAnyPages, EfiBootServicesData, Status: 0x%x\n", Status);
  if(Status != 0x0) goto failure;
  
  Status = TestAllocatePages(AllocateMaxAddress, EfiBootServicesData);
  AsciiPrint("===> AllocateMaxAddress, EfiBootServicesData, Status: 0x%x\n", Status);
  if(Status != 0x0) goto failure;
  
  Status = TestAllocatePages(AllocateAnyPages, EfiRuntimeServicesData);
  AsciiPrint("===> AllocateAnyPages, EfiRuntimeServicesData, Status: 0x%x\n", Status);
  if(Status != 0x0) goto failure;
  Status = TestAllocatePages(AllocateMaxAddress, EfiRuntimeServicesData);
  AsciiPrint("===> AllocateMaxAddress, EfiRuntimeServicesData, Status: 0x%x\n", Status);
  if(Status != 0x0) goto failure;

  AsciiPrint("\n\n   Allocate/Free Pool\n");
  Status = TestAllocatePool(EfiBootServicesData);
  AsciiPrint("===> EfiBootServicesData, Status: 0x%x\n", Status);
  if(Status != 0x0) goto failure;
  Status = TestAllocatePool(EfiRuntimeServicesData);
  AsciiPrint("===> EfiRuntimeServicesData, Status: 0x%x\n", Status);
  if(Status != 0x0) goto failure;
  
  /* Calling UefiMemMapCmd here as UncachedMemoryAllocationLib will not release memory back until the library destructor is called i.e. when the app exits
	 which is not the case for Simple AllocatePage or AllocatePool API's */
  Status = UefiMemMapCmd(); 
  if(Status != 0x0) goto failure;

  AsciiPrint("\n\nUncached Memory Allocation Tests 1..%d\n\n", MAX_NUM_LOOP);
  
  Status = TestUncachedAllocatePages();  
  AsciiPrint("===> Allocate/Free Uncached Pages, Status: 0x%x\n", Status);
  if(Status != 0x0) goto failure;
  Status = TestUncachedAllocatePool();  
  AsciiPrint("===> Allocate/Free Uncached Pool, Status: 0x%x\n", Status);
  if(Status != 0x0) goto failure;

  Status = TestUncachedAllocateAlignedRuntimePool();
  AsciiPrint("===> Allocate/Free Uncached Aligned Runtime Pool, Status: 0x%x\n", Status);
  if(Status != 0x0) goto failure;
  
 failure:
  TestStatus("MemTest", Status); 
  TEST_STOP("MemTest");
  return Status;
}
