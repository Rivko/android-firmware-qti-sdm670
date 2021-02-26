/** @file AslrTest.c
   
  ASLR test application

  Copyright (c) 2016, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/08/16   bh      Initial revision.

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AslrLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/ProcAsmLib.h>

#define ASLR_TEST_ALLOC_SIZE   512

EFI_STATUS
EFIAPI
AslrTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  UefiInfoBlkType * IBlk = NULL;
  UINTN StackTop = 0;
  VOID * StackPointer = NULL;
  VOID * UncachedMemPointer = NULL;
  VOID * MemPointer = NULL;

  /* Get pointers */
  StackPointer = GetStackPointer();

  IBlk = GetInfoBlkPtr();
  if (IBlk == NULL)
    return EFI_NOT_READY;

  StackTop = ((UINTN) IBlk->StackBase) + IBlk->StackSize;

  UncachedMemPointer = UncachedAllocatePool(ASLR_TEST_ALLOC_SIZE);
  if (UncachedMemPointer == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  MemPointer = AllocatePool(ASLR_TEST_ALLOC_SIZE);
  if (MemPointer == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  /* Print pointers */
  DEBUG((EFI_D_ERROR, "Stack Top   : 0x%x\n", StackTop));

  DEBUG((EFI_D_ERROR, "Current SP  : 0x%x\n", (UINTN) StackPointer));

  DEBUG((EFI_D_ERROR, "Cached Mem  : 0x%x\n", (UINTN) MemPointer));

  DEBUG((EFI_D_ERROR, "Uncached Mem: 0x%x\n", (UINTN) UncachedMemPointer));

  Status = EFI_SUCCESS;

Exit:
  if (MemPointer)
    FreePool(MemPointer);

  if (UncachedMemPointer)
    UncachedFreePool(UncachedMemPointer);
  
  return Status;
}

