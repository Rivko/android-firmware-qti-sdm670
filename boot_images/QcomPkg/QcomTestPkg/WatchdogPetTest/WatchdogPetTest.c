/** @file WatchdogPetTest.c
  Test application for Watchdog timer pet.
  
  Copyright (c) 2017, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 07/25/17   c_mku   Initial revision to demonstrate watchdog pet.

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/WatchdogLib.h>
#define MAX_ITERATIONS 10

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
WatchdogPetTest (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT8 i=0;
  EFI_STATUS Status = EFI_SUCCESS;
  
  TEST_START("WatchDogPetTest");  
  /* Disable crash dump watchdog  */
  Status = gBS->SetWatchdogTimer(0, 0, 0, NULL);
  if(Status != EFI_SUCCESS)
  {
    goto Exit;
  }
  AsciiPrint("Start : Disable Watchdog\n");
  WatchDogDisable();
  AsciiPrint("Finish : Disable Watchdog\n");
  Status = WatchDogSetBiteTimer(SEC_TO_MSEC(2));
  if(Status != EFI_SUCCESS)
  {
    goto Exit;
  }
  AsciiPrint("BiteTime Register value set to 2 Seconds\n");
  ResetWatchDogCounter();
  AsciiPrint("Start : Enable Watchdog\n");
  WatchDogEnable();
  AsciiPrint("Finish : Enable Watchdog\n");
  do{
    for(i=0;i<MAX_ITERATIONS;i++)
    {
      AsciiPrint("==========================================================================\n");
      AsciiPrint(" Iteration : %d \n",i);
      AsciiPrint("Start : Reset Watchdog Counter\n");
      ResetWatchDogCounter();
      AsciiPrint("Finish : Reset Watchdog Counter\n");
      gBS->Stall(EFI_TIMER_PERIOD_SECONDS(1));
      AsciiPrint("==========================================================================\n");
    }
    Status = WatchDogSetBiteTimer(SEC_TO_MSEC(2));
    if(Status != EFI_SUCCESS)
    {
      goto Exit;
    }
    AsciiPrint("Stall for 5 second,Expect crash after this\n");
    gBS->Stall(EFI_TIMER_PERIOD_SECONDS(5));
  }while(0);
  
  Exit:
	  TestStatus("WatchDogPetTest", EFI_DEVICE_ERROR);
	  TEST_STOP("WatchDogPetTest");
	  return EFI_DEVICE_ERROR;
}