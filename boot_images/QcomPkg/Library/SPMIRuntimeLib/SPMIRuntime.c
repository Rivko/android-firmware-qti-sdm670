/** @file SPMIRuntime.c
   
  Library implementation to support SPMI Runtime call.

  Copyright (c) 2012-2016 Qualcomm Technologies Inc.. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 10/03/14   na      Fixing size of puPmicArbSpmiVirtualAddress (must be UINTN)
 05/29/14   sv      Removed SpmiBus_Init() call from SPMIVirtualAddressChangeCallBack. (CR-672559)
 04/10/14   sg      Memory map size corrected.
 06/14/13   unr     Call Timetick_RunTimeInit
 11/02/12   niting  Call SpmiBus_OsGetCoreBaseAddress correctly and map size fix
 08/02/12   unr     Initial revision.

=============================================================================*/

#include <PiDxe.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>

#include <Guid/EventGroup.h>

#include <SpmiBus.h>
#include <SpmiBus_Os.h>
#include <SpmiBus_Runtime.h>

extern EFI_STATUS Timetick_RunTimeInit(void);

/* Round off to page size */
#define ROUND_TO_PAGE(x) (x & (~(EFI_PAGE_SIZE - 1)))
#define SUPPORTED_BUS_COUNT 1

EFI_EVENT SPMIVirtualAddressChangeEvent = NULL;

static UINTN puPmicArbSpmiVirtualAddress[SUPPORTED_BUS_COUNT] = {0};

/**
  Virtual address change notification call back. It converts global pointer
  to virtual address.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
SPMIVirtualAddressChangeCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
    uint32 i;
  /*
   * Convert physical address to Virtual in place
   */

   for(i = 0; i < SUPPORTED_BUS_COUNT; i++)
   {
      gRT->ConvertPointer(0, (VOID**) &puPmicArbSpmiVirtualAddress[i]);
      SpmiBus_OsSetCoreBaseAddress(i, puPmicArbSpmiVirtualAddress[i]);
   }
}

/**
  Initialize any infrastructure required for Lib to function.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SPMIRuntimeLibInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  uint32 i;
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR  auHwioAddr0MemoryDescriptor;

  Status = Timetick_RunTimeInit();
  ASSERT_EFI_ERROR (Status);

  for(i = 0; i < SUPPORTED_BUS_COUNT; i++)
  {
    SpmiBus_OsGetCoreBaseAddress(i, &puPmicArbSpmiVirtualAddress[i]);
    
    if (puPmicArbSpmiVirtualAddress[i] != 0)
    {
      uint32 spmiMemMapSize = 0;
      /*
       * Get the GCD Memory Descriptor specified by auHwioAddr[0] page boundary
       */
      Status = gDS->GetMemorySpaceDescriptor (
                      ROUND_TO_PAGE(puPmicArbSpmiVirtualAddress[i]), 
                      &auHwioAddr0MemoryDescriptor);

      ASSERT_EFI_ERROR (Status);

      SpmiBus_OsGetCoreMemoryMapSize(i, &spmiMemMapSize);
      
      /*
       * Mark the 4KB region as EFI_RUNTIME_MEMORY so the OS
       * will allocate a virtual address range.
       */
      Status = gDS->SetMemorySpaceAttributes (
                      ROUND_TO_PAGE(puPmicArbSpmiVirtualAddress[i]), 
                      (EFI_PAGE_SIZE) * spmiMemMapSize, 
                      auHwioAddr0MemoryDescriptor.Attributes | EFI_MEMORY_RUNTIME);

      ASSERT_EFI_ERROR (Status);
    }
  }
  
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SPMIVirtualAddressChangeCallBack,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &SPMIVirtualAddressChangeEvent
                  );

  ASSERT_EFI_ERROR (Status);

  return Status;
}

