/** @file BootModeMgmtProtocolTest.c
   
  Tests for BootModeMgmt protocol

  Copyright (c) 2014, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 12/09/14   lm     Initial revision

=============================================================================*/
#include <stdio.h>
#include <string.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/EFIBootModeMgmt.h>
#include <Library/TestInterface.h>

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
BootModeMgmtProtocolTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_BOOT_MODE_MGMT_PROTOCOL *BootModeMgmtProtocol = NULL;
  UINT32 BootMode = 0, BootModeNew = 0, BootModeOrig = 0;
  UINT32 ProfileNameElements = 32, ProfileNameElementsNew = 32, ProfileNameElementsOrig = 32;
  CHAR16 ProfileName[32] = L"Factory";
  CHAR16 ProfileNameNew[32];
  CHAR16 ProfileNameOrig[32];

  TEST_START("BootModeMgmtProtocolTest");

  /*
   * Load BootModeMgmtProtocol protocol
   */
  Status = gBS->LocateProtocol(&gEfiBootModeMgmtProtocolGuid,
                               NULL,
							   (VOID**) &BootModeMgmtProtocol);
  if (Status != EFI_SUCCESS)
  {
     AsciiPrint ("FW Boot Mode Management protocol is not found\n");
	 Status = EFI_SUCCESS;
     goto endOfTest;
  }
  
  AsciiPrint ("BootModeMgmtProtocolTestMain:Boot Mode Management protocol loaded\n");

  /* 
   * Get the values in the Boot Mode partition
   */
  Status = BootModeMgmtProtocol->GetBootModeInfo(BootModeMgmtProtocol, &BootModeOrig, &ProfileNameElementsOrig, ProfileNameOrig);
  if (Status == EFI_NOT_FOUND)
  {
     AsciiPrint ("Boot Mode partition Info not found on the device\n");
	 Status = EFI_SUCCESS;
     goto endOfTest;
  }
  
  else if (Status == EFI_VOLUME_CORRUPTED)
  {
     AsciiPrint ("Boot Mode partition Info does not contain valid data\n");
	 /* Initialize the values */
	 BootModeOrig = 0x0;
     ProfileNameElementsOrig = 0x0;
	 memset(ProfileNameOrig,'\0',sizeof(CHAR16)*32);
  }
  
  else if (Status == EFI_BUFFER_TOO_SMALL)
  {
     AsciiPrint ("Boot Mode partition emmc elements are greater in size than the buffer passed\n");
     Status = EFI_BUFFER_TOO_SMALL;
	 goto endOfTest;	 
  }
  
  else 
  {
     AsciiPrint ("Got values  BootModeOrig = %x,ProfileNameElementsOrig = %x,ProfileNameOrig = %s \n",BootModeOrig,ProfileNameElementsOrig,ProfileNameOrig);    
  }
  
  /* 
   * Set the values in the Boot Mode partition and read them back to check that the Set was 
   * successful.
   */  
  BootMode = 0;  
  ProfileNameElements = 8;
    
  AsciiPrint ("Setting values BootMode = %x,ProfileNameElements = %x,ProfileName = %s\n",BootMode,ProfileNameElements,ProfileName);  
  Status = BootModeMgmtProtocol->SetBootModeInfo(BootModeMgmtProtocol, &BootMode, &ProfileNameElements, ProfileName);
  if (Status != EFI_SUCCESS)
  {
     AsciiPrint ("ERROR: BootModeMgmtProtocolTestMain:Boot Mode Management protocol EFI_Set_Boot_Mode_Info failed\n");
     goto endOfTest;
  } 
  
  /* 
   * Read the values from the Boot Mode partition
   */
  Status = BootModeMgmtProtocol->GetBootModeInfo(BootModeMgmtProtocol, &BootModeNew, &ProfileNameElementsNew, ProfileNameNew);
  if (Status != EFI_SUCCESS)
  {
     AsciiPrint ("ERROR: BootModeMgmtProtocolTestMain:Boot Mode Management protocol EFI_Get_Boot_Mode_Info failed\n");
     goto endOfTest;
  }
  
  AsciiPrint ("Read back values BootModeNew = %x,ProfileNameElementsNew = %x,ProfileNameNEw = %s \n",BootModeNew,ProfileNameElementsNew,ProfileNameNew);  
  
  if ((BootMode != BootModeNew) || (ProfileNameElements != ProfileNameElementsNew))
  {
     AsciiPrint ("ERROR: Falied to set the values in the Boot Mode partition\n");
	 Status = EFI_VOLUME_CORRUPTED;
     goto endOfTest;     
  }
 
  /* 
   * Set the values back to the original values so that the partition is not corrupted.
   */  
  BootMode = BootModeOrig;
  ProfileNameElements = ProfileNameElementsOrig;
  CopyMemS (ProfileName, sizeof(ProfileNameOrig), ProfileNameOrig, sizeof(ProfileNameOrig));
 
  AsciiPrint ("Setting values BootMode = %x,ProfileNameElements = %x,ProfileName = %s\n",BootMode,ProfileNameElements,ProfileName);  
  Status = BootModeMgmtProtocol->SetBootModeInfo(BootModeMgmtProtocol, &BootMode, &ProfileNameElements, ProfileName);
  if (Status != EFI_SUCCESS)
  {
     AsciiPrint ("ERROR: BootModeMgmtProtocolTestMain:Boot Mode Management protocol EFI_Set_Boot_Mode_Info failed\n");
     goto endOfTest;
  }   
  
endOfTest:
  TestStatus("BootModeMgmtProtocolTest",Status);
  TEST_STOP("BootModeMgmtProtocolTest");
  
  return EFI_SUCCESS;
 
}

