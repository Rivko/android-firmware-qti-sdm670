/**
 * @file UsbfnLoopback.c
 *
 * Usbfn Test Application entry point.
 *
 * Performs UsbfnIo protocol validation tests and provides a data loopback USB
 * interface for performance testing.
 *
 * Copyright (c) 2010-2016, QUALCOMM Technologies Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/23/16   ck      Fix the issue of using un-initialized variable
 04/18/16   ck      Support USB Init Protocol
 01/22/16   ck      Support on deman USBFn configuration. 
 10/26/15   ck      Assume cable connection for pre-si platforms
 01/20/15   ck      Remove deprecated functions
 11/20/14   ml      Migrate to official SuperSpeed support
 02/06/14   ml      Organization, more thorough protocol tests, support new host tool
 11/01/13   ar      Add use of TestInterface banners
 08/29/13   mliss   Minor updates to more gracefully handle detach and exit
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 07/08/11   cching  Simplify the implementation, add few more test functions, and correct data transfer
 06/24/11   cching  Port UsbfnLoopback.c to UEFI

=============================================================================*/

#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <Library/TestInterface.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Protocol/EFIPlatformInfo.h>

#include "UsbfnLoopback.h"
#include "UsbfnDesc.h"
#include "UsbfnLbSm.h"
#include "UsbfnIoTest.h"


// global application verbosity level
INTN gAppPrintLvl = APL_ALWAYS;
extern EFI_GUID   gInitUsbControllerGuid;
extern EFI_GUID   gEfiUsbInitProtocolGuid;

/**
 * Dummy function needed for event notification callback 
 */
VOID
DummyNotify (
	IN EFI_EVENT Event,
	IN VOID *Context
	) 
{ 
  ; 
}


/**
 * Print the application usage information.
 */
STATIC
VOID
PrintUsage(
  VOID
  )
{
  // AsciiPrint doesn't support large enough string for all of help
  AsciiPrint("Usage: UsbfnTest [-hlzvv]\n"
             "\n");
  AsciiPrint("  Test the UsbfnIo protocol\n"
             "\n");
  AsciiPrint("  Performs tests to verify correct operation of various protocol interfaces, then\n"
             "  optionally enters loopback mode. While in loopback mode, throughput tests can be\n");
  AsciiPrint("  performed using the 'usbtp' host tool. Pressing any key will exit loopback mode\n"
             "  back to the shell. A single argument consisting of an optional leading dash (-)\n");
  AsciiPrint("  and one or more switches is supported (order is irrelevant).\n"
             "\n");
  AsciiPrint("  Switch  Description\n"
             "    h     This help\n");
  AsciiPrint("    l     Enter loopback mode after protocol tests\n"
             "    z     Use Usbfn driven ZLT (if supported) instead of application driven ZLT\n");
  AsciiPrint("    2     Use ConfigureEnableEndpoints instead of ConfigureEnableEndpointsEx\n");
  AsciiPrint("    v     Increase verbosity, two supported\n"
             "\n");
  AsciiPrint("  Each message printed by this application is prefixed with the message log level\n"
             "  in square brackets. The integer levels are defined as follows:\n");
  AsciiPrint("    [%d] Error\n", APL_ERROR);
  AsciiPrint("    [%d] Warning\n", APL_WARN);
  AsciiPrint("    [%d] Default, always printed\n", APL_ALWAYS);
  AsciiPrint("    [%d] Verbose\n", APL_VERB);
  AsciiPrint("    [%d] Very verbose\n\n", APL_VVERB);
}


/*
 * See header for documentation.
 */
EFI_STATUS
IssueXfer (
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
  )
{
  EFI_STATUS  Status     = EFI_SUCCESS;
  UINTN       BufferSize = 0;
  UINT8       *Buffer    = NULL;
  FNC_ENTER_MSG();

  if (NULL == UsbfnIo || NULL == Xfer) {
    PRINT(APL_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // verify Xfer is not active
  if (XFER_PENDING_COMPLETE == Xfer->XferState) {
    PRINT(APL_ERROR, "Transfer on EP 0x%02x active, not submitting",
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction));
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  Buffer = Xfer->Buffer;
  Buffer += Xfer->BufferOffset;

  BufferSize = Xfer->XferSize - Xfer->BufferOffset;

  Status = UsbfnIo->Transfer(
      UsbfnIo,
      Xfer->EndpointIndex,
      Xfer->Direction,
      &BufferSize,
      Buffer);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to submit transfer on EP 0x%02x: %r",
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction),
        Status);
    goto ON_EXIT;
  }

  PRINT(APL_VERB, "Transfer submitted on EP 0x%02x, pending completion",
      ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction));
  Xfer->XferState = XFER_PENDING_COMPLETE;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
AbortXfer (
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  if (NULL == UsbfnIo || NULL == Xfer) {
    PRINT(APL_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // Do nothing if the transfer hasn't been submitted
  if (Xfer->XferState != XFER_PENDING_COMPLETE) {
    PRINT(APL_VERB, "Transfer on EP 0x%02x not active, nothing to do",
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction));
    goto ON_EXIT;
  }

  Status = UsbfnIo->AbortTransfer(
      UsbfnIo,
      Xfer->EndpointIndex,
      Xfer->Direction);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to abort transfer on EP 0x%02x: %r",
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction),
        Status);
  }

  PRINT(APL_VERB, "Transfer on EP 0x%02x aborted, free",
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction));
  Xfer->XferState = XFER_FREE;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * The user Entry Point for Application.
 *
 * This function performs the following:
 *
 * 1. Process command line argument(s)
 * 2. Open UsbfnIo protocol and perform interface functionality tests
 * 3. Data loopback mode (loop until user input provided):
 *    - Fetch UsbfnIo events
 *    - Pass events to state machine which is responsible for all USB
 *      enumeration and data transfer handling
 *
 * @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 * @param[in] SystemTable    A pointer to the EFI System Table.
 *
 * @retval EFI_SUCCESS       All UsbfnIo tests passed.
 * @retval other             Error occured while testing UsbfnIo.
 */
EFI_STATUS
EFIAPI
UsbfnLoopbackMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                      Status        = EFI_SUCCESS;
  EFI_STATUS                      TstStatus     = EFI_SUCCESS;
  EFI_USBFN_IO_PROTOCOL           *UsbfnIo      = NULL;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *SimpleTextIn = NULL;
  EFI_HANDLE                      *Handles      = NULL;
  UINTN                           NumHandles    = 0;
  BOOLEAN                         UsbfnZlt      = FALSE;
  BOOLEAN                         DoLoopback    = FALSE;
  BOOLEAN                         Usb2Only      = FALSE;
  UINTN                           Argc;
  CHAR8                           **Argv;
  EFI_USBFN_MESSAGE               Msg;
  EFI_USBFN_MESSAGE_PAYLOAD       Payload;
  UINTN                           PayloadSize;
  EFI_INPUT_KEY                   Key;
  EFI_PLATFORMINFO_PROTOCOL       *PlatformInfoProtocol;
  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE PlatformInfo;
  EFI_EVENT                       UsbConfigEvt;
  EFI_USB_INIT_PROTOCOL           *UsbInitProtocol = NULL;
  UINT8                           CoreCount;
  USB_CORE_INFO                   CoreInfo;
  UINT8                           CoreNum;
  STATIC CONST UINT8              USB_CORE_ZERO = 0;

  // process command line args, if any
  Status = GetCmdLineArgs(ImageHandle, &Argc, &Argv);
  if (EFI_ERROR(Status)) {
    PRINT(APL_WARN, "Failed to get command line args: %r", Status);
  } else if (Argc > 0) {

    INTN i = 0;
    CHAR8 *c = Argv[i];

    // parse command line switches
    while (*c != '\0') {

      if (*c == '-') { // no action, just continue processing arg

      } else if (*c == 'h') { // application help
        PrintUsage();
        return EFI_SUCCESS;

      } else if (*c == 'l') { // enter loopback mode
        DoLoopback = TRUE;

      } else if (*c == 'z') { // use Usbfn driven ZLT
        UsbfnZlt = TRUE;

      } else if (*c == '2') { // only support USB 2.0
        Usb2Only = TRUE;

      } else if (*c == 'v') { // increase verbosity once per 'v'
        gAppPrintLvl++;

      } else { // invalid argument, try next one
        i++;
        if (Argc > i) {
          c = Argv[i];
          continue;
        } else { // treat unsupported arguments as warning
          PRINT(APL_WARN, "Unsupported switch '%c' found (-h for help)", c);
          break;
        }
      }
      c++;
    }
  }

  TEST_START(TEST_NAME);

  PRINT(APL_ALWAYS, "Current log level: %d", gAppPrintLvl);

  // Locate UsbInitProtocol
  Status = gBS->LocateProtocol(
    &gEfiUsbInitProtocolGuid,
    NULL,
    (VOID **)&UsbInitProtocol);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to locate UsbInitProtocol: %r", Status);
    UsbInitProtocol = NULL;
  }

  if (UsbInitProtocol) {
    Status = UsbInitProtocol->QueryCoreCount(UsbInitProtocol, &CoreCount);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to QueryCoreCount: %r", Status);
      goto ON_EXIT;
    }

    Status = UsbInitProtocol->QueryCoreInfo(UsbInitProtocol, NULL, &CoreInfo);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to QueryCoreInfo: %r", Status);
      goto ON_EXIT;
    }

    CoreNum = USB_CORE_ZERO;
    Status = UsbInitProtocol->QueryCoreInfo(UsbInitProtocol, &CoreNum, &CoreInfo);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to QueryCoreInfo: %r", Status);
      goto ON_EXIT;
    }

    if (!(CoreInfo.ModeType & EFI_USB_PERIPHERAL_MODE)) {
      PRINT(APL_ERROR, "Usb Core does not support device mode");
      goto ON_EXIT;
    }

    // Don't use CoreInfo->ModeType as the 3rd parameters to the StartCore 
    // function as it is an ORed value. Choose the mode to be initialized.  
    Status = UsbInitProtocol->StartController(UsbInitProtocol, CoreInfo.CoreNum, EFI_USB_PERIPHERAL_MODE);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to StartCore: %r", Status);
      goto ON_EXIT;
    }
  }
  else {
    Status = gBS->CreateEventEx(
      EVT_NOTIFY_SIGNAL,
      TPL_CALLBACK,
      DummyNotify,
      NULL,
      &gInitUsbControllerGuid,
      &UsbConfigEvt
      );

    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "CreateEventEx failed: %r", Status);
      goto ON_EXIT;
    }
    gBS->SignalEvent(UsbConfigEvt);
    gBS->CloseEvent(UsbConfigEvt);
  }

  // Locate all instances of UsbfnIo protocol
  Status = gBS->LocateHandleBuffer(
      ByProtocol,
      &gEfiUsbfnIoProtocolGuid,
      NULL,
      &NumHandles,
      &Handles);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to locate UsbfnIo protocol: %r", Status);
    goto ON_EXIT;
  }

  // Verify that exactly 1 instance exists
  if (NumHandles != 1) {
    PRINT(APL_ERROR, "Expecting 1 instance of UsbfnIo protocol, found %d", NumHandles);
    goto ON_EXIT;
  }

  // Open the protocol exclusively
  Status = gBS->OpenProtocol(
      Handles[0], // there is only one
      &gEfiUsbfnIoProtocolGuid,
      (VOID **)&UsbfnIo,
      ImageHandle,
      Handles[0],
      EFI_OPEN_PROTOCOL_EXCLUSIVE);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to open UsbfnIo protocol: %r", Status);
    UsbfnIo = NULL; // make sure NULL to prevent accidental attempt to close
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "UsbfnIo Protocol Revision: 0x%08x", UsbfnIo->Revision);

  // workaround the KW warning.
  if (UsbfnIo == NULL)
    goto ON_EXIT;

  // Before doing anything, call StopController
  Status = UsbfnIo->StopController(UsbfnIo);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  // Test Get Endpoint Policies
  Status = TestUsbfnIoGetEndpointPolicy(UsbfnIo);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TestUsbfnIoGetEndpointPolicy failed: %r", Status);
  }

  // Test Set Endpoint Policies
  Status = TestUsbfnIoSetEndpointPolicy(UsbfnIo);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TestUsbfnIoSetEndpointPolicy failed: %r", Status);
  }

  // Locate the simple text input protocol.
  Status = gBS->LocateProtocol(
      &gEfiSimpleTextInProtocolGuid,
      NULL,
      (VOID **)&SimpleTextIn);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to locate simple text input protocol: %r", Status);
    goto ON_EXIT;
  }

  // Start the USB controller
  Status = UsbfnIo->StartController(UsbfnIo);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to Start USB Controller: %r", Status);
    goto ON_EXIT;
  }

  // Copy configuration and set RS
  if (Usb2Only) {
    PRINT(APL_ALWAYS, "Using ConfigureEnableEndpoints");
    Status = UsbfnIo->ConfigureEnableEndpoints(
        UsbfnIo,
        (EFI_USB_DEVICE_INFO *)&DeviceInfo);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to configure and enable endpoints: %r", Status);
      goto ON_EXIT;
    }
  } else {
    PRINT(APL_ALWAYS, "Using ConfigureEnableEndpointsEx");
    Status = UsbfnIo->ConfigureEnableEndpointsEx(
        UsbfnIo,
        (EFI_USB_DEVICE_INFO *)&DeviceInfo,
        (EFI_USB_SUPERSPEED_DEVICE_INFO *)&SSDeviceInfo);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to configure and enable endpoints: %r", Status);
      goto ON_EXIT;
    }
  }

  // Test the device information query functions
  Status = TestUsbfnIoQueryDeviceInformation(UsbfnIo);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TestUsbfnIoQueryDeviceInformation failed: %r", Status);
  }

  // Loopback mode
  if (DoLoopback) {

    // Initialize loopback device state machine
      Status = UlbSmInitialize(
          UsbfnIo,
          (EFI_USB_DEVICE_INFO *)&DeviceInfo,
          (EFI_USB_SUPERSPEED_DEVICE_INFO *)&SSDeviceInfo,
          UsbfnZlt);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to initialize loopback state machine: %r", Status);
      goto ON_EXIT;
    }

    // Loopback main loop
    PRINT(APL_ALWAYS, "Loopback mode entered. Press any key to quit...");
    // Fetch the platform info
    Status = gBS->LocateProtocol(&gEfiPlatformInfoProtocolGuid, NULL,
      (VOID **)&PlatformInfoProtocol);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to Locate gEfiPlatformInfoProtocolGuid: %r", Status);
    }

    Status = PlatformInfoProtocol->GetPlatformInfo(PlatformInfoProtocol,
      &PlatformInfo);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to GetPlatformInfo: %r", Status);
    }

    for (;;) {
      // Check for a key press
      // seeing the issue with Virtio that even no key is not pressed,
      // the ReadKeyStroke indicates some key is pressed.
      Status = SimpleTextIn->ReadKeyStroke(SimpleTextIn, &Key);
      if (!EFI_ERROR(Status) && 
          !((PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI)    || 
            (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_VIRTIO)  || 
            (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_UNKNOWN))){
        PRINT(APL_ERROR, "Key is pressed. End the app");
        break;
      }

      // Always poll for events
      PayloadSize = sizeof(Payload);
      Status = UsbfnIo->EventHandler(UsbfnIo, &Msg, &PayloadSize, &Payload);
      if (EFI_ERROR(Status)) {
        PRINT(APL_ERROR, "UsbfnIo EventHandler failed: %r", Status);
        break;
      }

      // Pass meaningful events to state machine
      if (Msg != EfiUsbMsgNone) {
        Status = UlbSmProcessEvent(Msg, PayloadSize, &Payload);
        if (EFI_ERROR(Status)) {
          PRINT(APL_ERROR, "State machine failed to process event: %r", Status);
          break;
        }
      }
    }

    // Free state machine resources
    UlbSmCleanup();

  } else {
    PRINT(APL_ALWAYS, "Loopback mode not entered (use -h for help)");
  }

ON_EXIT:
  TstStatus = Status;

  // No cleanup unless the protocol was opened successfully
  if (NULL != UsbfnIo) {

    // Stop controller before exiting
    Status = UsbfnIo->StopController(UsbfnIo);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to Stop USB Controller: %r", Status);
    }

    // Close the UsbfnIo protocol
    Status = gBS->CloseProtocol(
        Handles[0],
        &gEfiUsbfnIoProtocolGuid,
        ImageHandle,
        Handles[0]
        );
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "failed to close UsbfnIo protocol: %r", Status);
    }

    // Free the handle buffer allocated by gBS->LocateHandleBuffer
    FreePool(Handles);

    if (UsbInitProtocol) {
      Status = UsbInitProtocol->StopController(UsbInitProtocol, CoreInfo.CoreNum, EFI_USB_PERIPHERAL_MODE);
      if (EFI_ERROR(Status)) {
        PRINT(APL_ERROR, "Failed to StartCore: %r", Status);
        goto ON_EXIT;
      }
    }
  }


  TestStatus(TEST_NAME, TstStatus);
  TEST_STOP(TEST_NAME);

  return EFI_SUCCESS;
}

