/** @file PxebootTest.c
   
  Tests for PXE boot.

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/27/12   pbi     Updated for timetick changes
 04/10/12   jz      Fixed klockwork warnings
 01/20/12   jz      Created
=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>

#include <Protocol/PxeBaseCode.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/EfiFileLib.h>

#include "TimetickUEFI.h"

static Timetick_timer_Type Timer;

VOID
EFIAPI
CalculateTime (
  uint32 TotalTicks,
  UINT64 *Seconds
  )
{
  uint32 TimetickFreq = 0;
  
  /* Convert ticks to micro seconds */
  Timetick_GetFreq(Timer, &TimetickFreq);
  *Seconds = MultU64x32(TotalTicks, 1000000);
  *Seconds = DivU64x32(*Seconds, TimetickFreq);
  *Seconds /= 1000000;
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
PxebootTestEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_PXE_BASE_CODE_PROTOCOL  *Pxe;
  EFI_HANDLE                  LoadImageHandle = NULL;
  EFI_OPEN_FILE               *FileHandle;
  CHAR8                       *FilePath= "LoadFile0:";
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  UINTN                       ExitDataSize;
  CHAR16                      *ExitData;
  uint32                      StartTick, EndTick, TotalTicks;
  UINT64                      Seconds;

  TEST_START("PXE BOOT");

  if(PcdGet32(PcdQTimerEnabled))
  {
    Timer = TIMETICK_QTIMER;
  }
  else
  {
    Timer = TIMETICK_TIMER_ADGT;
  }
  /* Enable TimeTick for collecting performance data */
  Timetick_Init();
  Timetick_Enable(Timer, TRUE);

  // Get the tick count at the start of the event
  Timetick_GetCount(Timer, &StartTick);

  FileHandle = EfiOpen (FilePath, EFI_FILE_MODE_READ, 0);
  if (FileHandle == NULL) {
    return EFI_INVALID_PARAMETER; 
  }

  DevicePath = FileHandle->DevicePath;
  if (DevicePath != NULL) {
    // check for device path form: blk, fv, fs, and loadfile
    Status = gBS->LoadImage (TRUE, ImageHandle, DevicePath, NULL, 0, &LoadImageHandle);
  }

  // Get the tick count at the end of the event
  Timetick_GetCount(Timer, &EndTick);

  // Calculate the number of ticks spent in the event
  TotalTicks = EndTick - StartTick;

  CalculateTime(TotalTicks, &Seconds);
    
  if (EFI_ERROR(Status))
  {
    AsciiPrint("Error Status: %d\n", Status);
  }
  else
  {
    AsciiPrint("Time taken: %ld\n", Seconds);
  }

  if (!EFI_ERROR (Status)) {
    // Transfer control to the EFI image we loaded with LoadImage()
    Status = gBS->StartImage (LoadImageHandle, &ExitDataSize, &ExitData);
  }
  
  EfiClose (FileHandle);

  TestStatus("PXE BOOT", Status);
  TEST_STOP("PXE BOOT");
  return Status;
}
