/** @file TftpTest.c
   
  Tests for image download via TFTP.

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
TftpEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_PXE_BASE_CODE_PROTOCOL            *Pxe;
  EFI_PXE_BASE_CODE_TFTP_OPCODE         Operation;
  VOID                                  *BufferPtr = NULL;
  BOOLEAN                               Overwrite = FALSE;
  UINT64                                BufferSize;
  UINTN                                 BlockSize = 8192; //1428;
  EFI_IP_ADDRESS                        ServerIp;
  UINT8                                 Filename[32];
  EFI_PXE_BASE_CODE_MTFTP_INFO          *Info = NULL;
  BOOLEAN                               DontUseBuffer = FALSE;
  uint32                                StartTick, EndTick, TotalTicks;
  UINT64                                Seconds;
  UINTN                                 WriteBytes, Offset;
  
  TEST_START("TFTP");


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

  Status = gBS->LocateProtocol (&gEfiPxeBaseCodeProtocolGuid, NULL, (VOID **)&Pxe);
  if (EFI_ERROR(Status)) {
    AsciiPrint("Failed to locate PxeBaseCodeProtocol, Status: %d\n", Status);
    return Status;
  }

  Status = Pxe->Start (Pxe, FALSE);
  if (EFI_ERROR(Status) && (Status != EFI_ALREADY_STARTED)) {
    AsciiPrint("Failed to enable PXE, Status: %d\n", Status);
    return Status;
  }

  Status = Pxe->Dhcp(Pxe, TRUE);
  if (EFI_ERROR(Status)) {
    AsciiPrint("Failed to complete DHCP, Status: %d\n", Status);
    return Status;
  }

  CopyMem (&ServerIp, Pxe->Mode->DhcpAck.Dhcpv4.BootpSiAddr, sizeof (EFI_IPv4_ADDRESS));

  CopyMem (Filename, Pxe->Mode->DhcpAck.Dhcpv4.BootpBootFile, sizeof(Filename));
  
  // Get the file size from server
  Operation = EFI_PXE_BASE_CODE_TFTP_GET_FILE_SIZE;
  DontUseBuffer = TRUE;

  Status = Pxe->Mtftp (
                  Pxe,
                  Operation,
                  BufferPtr,
                  Overwrite,
                  &BufferSize,
                  &BlockSize,
                  &ServerIp,
                  Filename,
                  Info,
                  DontUseBuffer
                  );

  if (EFI_ERROR(Status)) {
    AsciiPrint("Failed to get file size, Status: %d\n", Status);
    return Status;
  }

  // Read the file from server
  Operation = EFI_PXE_BASE_CODE_TFTP_READ_FILE;
  DontUseBuffer = FALSE;

  BufferPtr = AllocatePool(BufferSize);
  if (BufferPtr == NULL)
    return RETURN_OUT_OF_RESOURCES;

  // Get the tick count at the start of the event
  Timetick_GetCount(Timer, &StartTick);

  Status = Pxe->Mtftp (
                  Pxe,
                  Operation,
                  BufferPtr,
                  Overwrite,
                  &BufferSize,
                  &BlockSize,
                  &ServerIp,
                  Filename,
                  Info,
                  DontUseBuffer
                  );

  // Get the tick count at the end of the event
  Timetick_GetCount(Timer, &EndTick);

  // Calculate the number of ticks spent in the event
  TotalTicks = EndTick - StartTick;

  CalculateTime(TotalTicks, &Seconds);
    
  if (EFI_ERROR(Status))
  {
    AsciiPrint("TFTP failed, Status: %d\n", Status);
  }
  else
  {
    AsciiPrint("TFTP succeeded, operation: %d, filename: %a, size: %ld, time taken: %ld seconds\n",
               Operation, Filename, BufferSize, Seconds);
  }

  Pxe->Stop(Pxe);

  if (BufferPtr != NULL) {
    FreePool(BufferPtr);
  }

  TestStatus("TFTP", Status);
  TEST_STOP("TFTP");
  return Status;
}
