/*==============================================================================

FILE:      TimetickUEFI.c

DESCRIPTION: This file implements the routines to read/write to the timetick 
             Qtimer registers. 

===========================================================================
             Copyright (c) 2013, 2015 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/13/15   al       Changing TPL_NOTIFY to TPL_CALLBACK
 03/11/15   vk       Initialize timer 
 06/27/12   pbi      Created for Badger family support.

=============================================================================*/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include "TimetickUEFI.h"
#include "TimerHWIO.h"
#include "HALhwio.h"
#include <Guid/EventGroup.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DxeServicesTableLib.h>

/*===========================================================================

                           DATA DEFINITIONS

============================================================================*/
/* Round off to 4KB pages */
#define ROUND_TO_PAGE(x) (x & 0xfffff000)

static UINTN       QTimerBase; /* Holds the QTimer registers base address*/
static boolean      IsRunTimeInitialized = 0;

EFI_EVENT TimetickVirtualAddressChangeEvent = NULL;

/*=========================================================================
                              FUNCTIONS
==========================================================================*/
/**
  Virtual address change notification call back. It converts global pointer
  to virtual address.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
TimetickVirtualAddressChangeCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
    gRT->ConvertPointer(0, (VOID**)&QTimerBase);
    Timetick_UpdateRunTimeAddress(QTimerBase);
}



/*=============================================================================

FUNCTION Timetick_RunTimeInit
 
=============================================================================*/
EFI_STATUS Timetick_RunTimeInit(void)
{
  EFI_STATUS       Status = EFI_SUCCESS;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR  QTimerBaseMemoryDescriptor;

  if (!IsRunTimeInitialized)
  {
     /* Initialize Timer */
     Timetick_Init();
	 
     /* Get the base addresses for the QTimer Counter */
     QTimerBase = PcdGet32(PcdQTimerV2BasePhys);

     if (QTimerBase != 0)
     {
         /*
          * Get the GCD Memory Descriptor specified by QTimerBase page boundary
          */
         Status = gDS->GetMemorySpaceDescriptor (ROUND_TO_PAGE(QTimerBase), 
                                                 &QTimerBaseMemoryDescriptor);
         ASSERT_EFI_ERROR (Status);
       
         /*
          * Mark the 4KB region as EFI_RUNTIME_MEMORY so the OS
          * will allocate a virtual address range.
          */
         Status = gDS->SetMemorySpaceAttributes (
                         ROUND_TO_PAGE(QTimerBase), 
                         EFI_PAGE_SIZE, 
                         QTimerBaseMemoryDescriptor.Attributes | EFI_MEMORY_RUNTIME);
         ASSERT_EFI_ERROR (Status);
     }


     Status = gBS->CreateEventEx (
                     EVT_NOTIFY_SIGNAL,
                     TPL_CALLBACK,
                     TimetickVirtualAddressChangeCallBack,
                     NULL,
                     &gEfiEventVirtualAddressChangeGuid,
                     &TimetickVirtualAddressChangeEvent
                     );

     ASSERT_EFI_ERROR (Status);
     IsRunTimeInitialized = 1;
  }
  return EFI_SUCCESS;

}/* END Timetick_RunTimeInit */


