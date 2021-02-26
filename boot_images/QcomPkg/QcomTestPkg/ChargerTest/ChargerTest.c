/** @file ChargerTest.c
   
  Tests for Charger

  Copyright (c) 2011-2015, 2017, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 03/20/17   va      Updated App for charger ex 
 09/29/14   va      Updated App for supporting arguments
 08/27/14   va      Updated Asciiprint to DEBUG 
 04/29/13   yg      Add back Stall
 04/04/13   yg      Some cleanup
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 02/02/12   sm      Replaced DEBUG messages with AsciiPrint
 06/06/11   dy      New File

=============================================================================*/

#include "ChargerTest.h"
#include <Protocol/EFIChargerEx.h>
#include "TestInterface.h"


/* Sleep without burning CPU cycles */
EFI_STATUS
WaitForEventTimeout (IN UINT32   Timeoutms)
{
  UINTN           Index;
  EFI_STATUS      Status = EFI_NOT_READY;
  EFI_EVENT       TimerEvent;
  EFI_EVENT       WaitList[3];
  UINT64          TimeoutVal;
  EFI_INPUT_KEY   KeyVal;

  if (Timeoutms == 0)
    return EFI_INVALID_PARAMETER;

  /* Create a Timer event  */
  Status = gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);

  if (Status != EFI_SUCCESS)
    return Status;

  /* Convert to 100ns units */
  TimeoutVal = Timeoutms * 10000;

  /* Set Timer event */
  gBS->SetTimer (TimerEvent, TimerRelative, TimeoutVal);

  /* Wait for some event or the timer */
  WaitList[0] = TimerEvent;
  WaitList[1] = gST->ConIn->WaitForKey;
  Status = gBS->WaitForEvent (2, WaitList, &Index);
  if (Index == 1)
    gST->ConIn->ReadKeyStroke (gST->ConIn, &KeyVal);
  gBS->CloseEvent (TimerEvent);

  return Status;
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
ChargerTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_CHARGER_EX_PROTOCOL  *ChargerExProtocol = NULL;
  /* Parameters to get Charger Ex status */
  BOOLEAN bOffModeCharging = FALSE;
  EFI_CHARGER_EX_FLASH_INFO        FlashInfo = {0};

#if 0 
  /* Act on cmd line arguments if fails continue for charge */
  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);


  if(Argc > 0 )//Act only if arguments are present
  {
    DEBUG((EFI_D_WARN, "Setting Arguments \r\n"));

    Status = ChargerTestH_ProcessCommand(Argc, Argv, &TargetSOC, &displayStatus);

  }else
  {
    DEBUG((EFI_D_WARN,"Arguments not provided \r\n"));
  }
#endif

  /* Start Charger test now */
  TEST_START("ChargerTest: Charger Ex");

  /* Locate Charger Ex Protocol */
  Status = gBS->LocateProtocol(
                  &gChargerExProtocolGuid,
                  NULL,
                  (VOID**) &ChargerExProtocol );
  
  if ((Status != EFI_SUCCESS) || (NULL == ChargerExProtocol)) 
  {
    DEBUG((EFI_D_WARN,"ERROR: ChargerTest - Failed to locate Charger Protocol! Error Code = 0x%08X\r\n", Status));
    return Status;
  }

  DEBUG((EFI_D_WARN,"ChargerTest: Charger Ex Protocol located \n"));

  /* Get Off Mode charging status */
  Status = ChargerExProtocol->IsOffModeCharging(&bOffModeCharging);
  if (Status != EFI_SUCCESS) 
  {
    DEBUG((EFI_D_WARN,"ERROR: ChargerTest - Failed to Off Mode Charging Error Code: %r \r\n", Status));
  }
  else
  {
    DEBUG((EFI_D_WARN,"ChargerTest: Off Mode charging status = %d \n", bOffModeCharging));
  }

  /* Get SW Image flash Status */
  Status = ChargerExProtocol->IsPowerOk(EFI_CHARGER_EX_POWER_FLASH_BATTERY_VOLTAGE_TYPE, &FlashInfo);
  if (Status != EFI_SUCCESS) 
  {
    DEBUG((EFI_D_WARN,"ERROR: ChargerTest - Failed to Flash Battery voltage Error Code = %r \r\n", Status));
  }
  else
  {
    DEBUG((EFI_D_WARN,"ChargerTest: SW can Flash = %d Batt current volt = %dmV Required = %dmV \r\n",
          FlashInfo.bCanFlash, FlashInfo.BattCurrVoltage, FlashInfo.BattRequiredVoltage));
  }

  if(EFI_SUCCESS == Status)
  {
    TestStatus("ChargerTest: Charger Ex", Status);
  }
  else//ChargerCompletionToken error
  {
    TestStatus("ChargerTest: Charger Ex", Status);
  }
  
  TEST_STOP("ChargerTest: Charger Ex");

  ReadAnyKey (NULL, 0);

  return EFI_SUCCESS;
}


