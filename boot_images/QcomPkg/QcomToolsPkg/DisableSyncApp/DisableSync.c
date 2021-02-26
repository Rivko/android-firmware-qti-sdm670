/** @file DisableSync.c
   
  Disables syncing of variables.

  Copyright (c) 2012, 2015, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
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


 when       who       what, where, why
 --------   ---       -----------------------------------------------------------
 07/13/15   al        Changing TPL_NOTIFY to TPL_CALLBACK
 12/02/13   c_mvanim  Updated DisableSyncTest App 
 07/03/12   aus       Replaced DEBUG with AsciiPrint
 01/20/12   niting    Created

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>

extern EFI_GUID gEfiEventFirmwareDesyncGuid;
extern EFI_GUID gQcomTokenSpaceGuid;
STATIC EFI_EVENT FirmwareDesyncEvent = NULL;


/** 
   
  Does nothing. Just a placeholder

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
DisableSyncEventHandler (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  AsciiPrint("Disabling Syncing of variables\n");
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
DisableSyncMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8      ArgValue = 0;
  UINTN      ArgSize = 0; 
  ArgSize = sizeof(ArgValue);
  
  
  /* Check for Var1 */
  Status = gRT->GetVariable (L"Var1", &gQcomTokenSpaceGuid,
                             NULL, &ArgSize, &ArgValue);
    
  /* Var1 is found. Hence this has to be the second run of this app */  	
  if ((Status == EFI_SUCCESS) && (ArgValue == 5)) {
	/* Now check for Var2 */
	Status = gRT->GetVariable (L"Var2", &gQcomTokenSpaceGuid,
                               NULL, &ArgSize, &ArgValue);
    ASSERT_EFI_ERROR (Status);
								   
	if( ArgValue == 5) {
	   /* Var2 found to hold the value as expected*/
	   AsciiPrint("Var2 is not updated after disable sync event as expected \n");
	   /* Delete Var1, Var2 variables from storage before exit*/
	   Status = gRT->SetVariable (L"Var1",&gQcomTokenSpaceGuid,
                                 (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                                  0, NULL);
	   ASSERT_EFI_ERROR (Status);
	   Status = gRT->SetVariable (L"Var2",&gQcomTokenSpaceGuid,
                                 (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
								  0, NULL);
   	   ASSERT_EFI_ERROR (Status);
	   Status = EFI_SUCCESS;
	   TestStatus("DisableSync",Status);
	} 
	else {
	   /* Var2 is found to have a different value */
	   AsciiPrint("Var2 is updated even after disable sync event \n");
	   /* Delete Var1, Var2 variables from storage before exit */
	   Status = gRT->SetVariable (L"Var1",&gQcomTokenSpaceGuid,
                                 (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                                  0, NULL);
	   ASSERT_EFI_ERROR (Status);
	   Status = gRT->SetVariable (L"Var2",&gQcomTokenSpaceGuid,
                                 (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
								  0, NULL);
   	   ASSERT_EFI_ERROR (Status);
	   Status = EFI_NOT_FOUND;
	   TestStatus("DisableSync",Status);
	}
	
    RemoveFromBootOrder (L"DisableSync Test");
	TEST_STOP("DisableSync");
	return Status;
  }
	
  /* Var1 is not found. Hence this has to be the first run of this app */  	 
  /* Create Var1 variable and set it to 5 */
  TEST_START("DisableSync");
  AsciiPrint("Set Var1 to value 5 \n");
  ArgValue = 5;	
  Status = gRT->SetVariable (L"Var1",&gQcomTokenSpaceGuid,
                            (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                             ArgSize, &ArgValue);					   
  ASSERT_EFI_ERROR (Status);
	
  /* Create Var2 variable and set it to 5 */
  AsciiPrint("Set Var2 to value 5\n");
  Status = gRT->SetVariable (L"Var2",&gQcomTokenSpaceGuid,
                            (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                             ArgSize, &ArgValue);
  ASSERT_EFI_ERROR (Status);
     
  /* Adding this App image to Bootorder variable before reset */
  Status = AddDevPathToBootOrder (ImageHandle, NULL,
                                  L"DisableSync Test", NULL, 0, 0);
  ASSERT_EFI_ERROR (Status);	
  
  /* Create event to disable sync */
  AsciiPrint("Creating disable sync event\n");
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  DisableSyncEventHandler,
                  NULL,
                  &gEfiEventFirmwareDesyncGuid,
                  &FirmwareDesyncEvent
                  );
  ASSERT_EFI_ERROR (Status);

  AsciiPrint("Signaling disable sync event\n");
  Status = gBS->SignalEvent(FirmwareDesyncEvent);
  ASSERT_EFI_ERROR (Status);
  
  /* Set Var2 variable to 8 after disabling sync of variables */
  AsciiPrint("Set Var2 to value 8 \n");
  ArgValue = 8;
  Status = gRT->SetVariable (L"Var2",&gQcomTokenSpaceGuid,
                            (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                             ArgSize, &ArgValue);
  ASSERT_EFI_ERROR (Status);						  
  
  AsciiPrint("Resetting the device...\n");
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  return Status;
}
