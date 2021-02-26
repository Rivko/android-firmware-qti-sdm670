/** @file WatchdogTimerTest.c
   
  Test application for Watchdog timer.

  Copyright (c) 2011, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 07/30/11   shirleyy Adding TestInterface Library
 01/13/11   niting  Initial revision

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>

#include <Protocol/WatchdogTimer.h>

VOID
EFIAPI
WatchdogTimerTestHandler(
  IN UINT64  Time
)
{
  AsciiPrint("Watchdog Timer Expired: %ld\nResetting...", Time);
  TestStatus("WatchdogTimer",EFI_SUCCESS);
  TEST_STOP("WatchdogTimer");
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
WatchdogTimerTest (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_WATCHDOG_TIMER_ARCH_PROTOCOL *WDT = NULL;
  UINT64 TimerPeriod;

  do {
    TEST_START("WatchdogTimer");
    AsciiPrint("Start of Watchdog Timer Test\n");
  
    Status = gBS->LocateProtocol(&gEfiWatchdogTimerArchProtocolGuid,
                                 NULL,
                                 (VOID**) &WDT);
    
    if (Status != EFI_SUCCESS)
    {
      TestStatus("WatchdogTimer",Status);
      break;
    }
  
    AsciiPrint("Register Watchdog Timer Handler:\n");
    Status = WDT->RegisterHandler(WDT,
                                  WatchdogTimerTestHandler);
    if (Status != EFI_SUCCESS)
    {
      TestStatus("WatchdogTimer",Status);
      break;
    }

    AsciiPrint("Get Watchdog Timer:\n");
    Status = WDT->GetTimerPeriod(WDT, &TimerPeriod);
    if (Status != EFI_SUCCESS)
    {
      TestStatus("WatchdogTimer",Status);
	  break;
    }
    AsciiPrint("Watchdog Timer Period = %ld\n", TimerPeriod);

    AsciiPrint("Set Watchdog Timer:\n");
    /* Set watchdog timer to 3 seconds */
    Status = WDT->SetTimerPeriod(WDT, 30000000);
    if (Status != EFI_SUCCESS)
    {
      TestStatus("WatchdogTimer",Status);
	  break;
    }
  
    AsciiPrint("Get Watchdog Timer:\n");
    WDT->GetTimerPeriod(WDT, &TimerPeriod);
    AsciiPrint("Watchdog Timer Period = %ld\n", TimerPeriod);

    AsciiPrint("Waiting until watchdog expires...\n");
    CpuDeadLoop();
  
  } while (0);

  AsciiPrint("Error occurred, should never reach here\n");
  TEST_STOP("WatchdogTimer");

  return Status;
}
