/** @file UsbfnChgSrvTest.c

 Test app for EFIUsbfnChgSrv Protocol

  Copyright (c) 2016, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 05/23/16   ck      Init Check-in

 =============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Protocol/EFIUsbfnChgSrv.h>
#include <Protocol/EFIUsbInitProtocol.h>
#include "UsbError.h"

// Terminate output with a newline, and eliminate the need for double parens.
#define DBG(EfiD, Str, args...) \
  AsciiPrint ("%a: " Str "\n",__FUNCTION__, ##args);

extern EFI_GUID                    gEfiUsbfnChgSrvProtocolGuid;
extern EFI_GUID                    gEfiSimpleTextInProtocolGuid;
extern EFI_GUID                    gEfiUsbInitProtocolGuid;
static EFI_USBFN_CHG_SRV_PROTOCOL *UsbfnChgSrvProtocol;


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
UefiMain( IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{  
  EFI_STATUS                       Status             = EFI_SUCCESS;
  EFI_USBFN_CHG_SRV_CHARGER_TYPE   ChgType            = USB_CHARGER_UNKNOWN;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *SimpleTextIn       = NULL;
  EFI_INPUT_KEY                    Key;
  UINT32                           StartTime          = 0;
  UINT32                           EndTime            = 0;
  EFI_USB_INIT_PROTOCOL           *UsbInitProtocol    = NULL;
  UINT8                            CoreCount;
  USB_CORE_INFO                    CoreInfo;  

  TEST_START("UsbfnChgSrvTest");

  // Locate the simple text input protocol.
  Status = gBS->LocateProtocol(
    &gEfiSimpleTextInProtocolGuid,
    NULL,
    (VOID **)&SimpleTextIn);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to locate simple text input protocol: %r", Status);
    goto ON_EXIT;
  }
  
  // Locate UsbInitProtocol
  Status = gBS->LocateProtocol(
    &gEfiUsbInitProtocolGuid,
    NULL,
    (VOID **)&UsbInitProtocol);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to locate UsbInitProtocol: %r", Status);
    UsbInitProtocol = NULL;
  }

  if (UsbInitProtocol) {
    Status = UsbInitProtocol->QueryCoreCount(UsbInitProtocol, &CoreCount);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to QueryCoreCount: %r", Status);
      goto ON_EXIT;
    }

    Status = UsbInitProtocol->QueryCoreInfo(UsbInitProtocol, NULL, &CoreInfo);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to QueryCoreInfo: %r", Status);
      goto ON_EXIT;
    }    

    if (!(CoreInfo.ModeType & EFI_USB_PERIPHERAL_MODE)) {
      DBG(EFI_D_ERROR, "Usb Core does not support device mode");
      goto ON_EXIT;
    }

    // Don't use CoreInfo->ModeType as the 3rd parameters to the StartCore 
    // function as it is an ORed value. Choose the mode to be initialized.  
    Status = UsbInitProtocol->StartController(UsbInitProtocol, CoreInfo.CoreNum, EFI_USB_PERIPHERAL_MODE);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to StartCore: %r", Status);
      goto ON_EXIT;
    }
    
    // Locate the USB Chg Srv protocol.
    Status = gBS->LocateProtocol(&gEfiUsbfnChgSrvProtocolGuid, NULL,
      (VOID **) &UsbfnChgSrvProtocol);
    USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);  

    // start enumeration
    StartTime = GetTimerCountms();
    Status = UsbfnChgSrvProtocol->StartController(UsbfnChgSrvProtocol);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Unable to start controller %r", Status);
      goto ON_EXIT;
    }
    EndTime = GetTimerCountms();

    // get the charger type
    Status = UsbfnChgSrvProtocol->GetChargerType(UsbfnChgSrvProtocol, &ChgType);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Unable to get charger type %r", Status);
      goto ON_EXIT;
    }
    DBG(EFI_D_ERROR, "Charger Type %d. Det Time %d", ChgType, EndTime-StartTime);

    DBG(EFI_D_ERROR, "Press any key to quit...");
    for (;;) {
      // Check for a key press to quit the application
      Status = SimpleTextIn->ReadKeyStroke(SimpleTextIn, &Key);
      if (!EFI_ERROR(Status)) {
        break;
      }
    }

    // stop the enumeration.
    Status = UsbfnChgSrvProtocol->StopController(UsbfnChgSrvProtocol);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Unable to get charger type %r", Status);
      goto ON_EXIT;
    }
    
    Status = UsbInitProtocol->StopController(UsbInitProtocol, CoreInfo.CoreNum, EFI_USB_PERIPHERAL_MODE);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to StartCore: %r", Status);
      goto ON_EXIT;
    }
    
  }

ON_EXIT:
  TestStatus("UsbfnChgSrvTest", Status);
  TEST_STOP("UsbfnChgSrvTest");
  return Status;
}
