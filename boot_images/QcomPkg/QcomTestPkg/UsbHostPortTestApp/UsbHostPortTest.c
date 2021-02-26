/** @file UsbHostPortTest.c

  User program for USB High Speed Host Mode Electrical Testing.
  Testing procedures for host mode electrical testing can
  be found in EHCI 1.0 spect section 4.14  "Port Test Modes".
  ChipIdea "CI13320A USB 2.0 High-Speed USB Controller
  Proramming Guide" has details on register settings
  for the testing.

  The host mode electrical test has six sections:

    1. Host Hi-Speed Signal Quality
    2. Host Test J/K, SE0_NAK
    3. Host Disconnect Detect
    4. Host Suspend/Resume Timing
    5. Host CHIRP Timing
    6. Host Controller Packet Parameters

  Section 1-3 involve direct manipulation of HC register bits and are implemented
  through an extenstion QCOM_USB_PROTOCOL_HOST_PORT_TEST to the existing
  QCOM_USB_PROTOCOL interface.

  Section 4-5 leverages EFI_USB2_HC_PROTOCOL interface implemented in EhciDxe.

  Section 6 requires the transactions to obtain device descriptor be broken down
  with user-controlled intervals. This feature is implemented through enhancements
  in EhciDxe (EhciUrb.c).

  Section 4-6 leverages a new protocol QCOM_USB_PORT_TEST_PROTOCOL to get access
  to features in UsbBusDxe.

  Copyright (c) 2010-2017, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/04/17   gk	    UEFI USB compliance test changes
 11/01/13   ar      Add use of TestInterface banners
 06/18/13   mliss   Updated to support multiple controller instances
 04/30/12   wufan   Use UsbPortTestProtocol to pass data btw EHCI and user app.
 12/08/11   wufan   CLI interface for HS USB host mode electrical testing.

=============================================================================*/
//
// Includes
//
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/QcomLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/EFIUsbConfig.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIUsb2HostController.h>
#include <Protocol/EFIUsbPortTestHelper.h>
#include <Protocol/EFIUsbPortTest.h>
#include <Protocol/LoadedImage.h>
#include "Drivers/XhciDxe/XhciPortTest.h"


//
// Miscellaneous Definitions
//

#define TEST_NAME "UsbHostPortTest"
#define QCOM_UEFI_USB_ECT_VER    L"2.0"


extern EFI_GUID gQcomTokenSpaceGuid;
extern EFI_GUID gEfiLoadedImageProtocolGuid;


//
// Structure for grouping sets of related protocols
//
typedef struct USB_HOST_PORT_TEST_CONTROLLER {
  EFI_HANDLE                          Handle;
  EFI_USB2_HC_PROTOCOL                *Usb2Hc;
  QCOM_USB_PORT_TEST_PROTOCOL         *PortTest;
  QCOM_USB_PORT_TEST_HELPER_PROTOCOL  *PortTestHelper;
  BOOLEAN                             HasHub;
  UINTN                               HubPortCount;
} USB_HOST_PORT_TEST_CONTROLLER;


//
// Command line options.
//
typedef enum {
  CMD_TEST_DISABLE          = 0,
  CMD_TEST_J_STATE          = 1,    // Host Test J/K, SE0_NAK
  CMD_TEST_K_STATE          = 2,    // Host Test J/K, SE0_NAK
  CMD_TEST_SE0_NAK          = 3,    // Host Test J/K, SE0_NAK
  CMD_TEST_PACKET           = 4,    // Host Hi-Speed Signal Quality
  CMD_TEST_FORCE_ENABLE     = 5,    // Host Disconnect Detect
  CMD_SS_COMPLIANCE_ENABLE  = 6,    // SS compliance enable
  CMD_SUSPEND_ROOTPORT      = 10,   // Host Suspend/Resume Timing
  CMD_RESUME_ROOTPORT       = 11,   // Host Suspend/Resume Timing
  CMD_GET_DESCRIPTOR        = 12,   // Host Controller Packet Parameters
  CMD_SUSPEND_RESET         = 13,   // Host CHIRP Timing
  CMD_HUB_TEST_J_STATE      = 101,  // Hub Host Test J
  CMD_HUB_TEST_K_STATE      = 102,  // Hub Host Test K
  CMD_HUB_TEST_SE0_NAK      = 103,  // Hub Host Test SE0_NAK
  CMD_HUB_TEST_PACKET       = 104,  // Hub Host Hi-Speed Signal Quality
  CMD_HUB_TEST_FORCE_ENABLE = 105,  // Hub Host Disconnect Detect
} CLI_CMD;


//
// Application data
//
typedef struct USB_HOST_PORT_TEST_APP_DATA {
  EFI_HANDLE                      ImageHandle;
  USB_HOST_PORT_TEST_CONTROLLER   *Controllers;
  UINTN                           ControllerCount;
  UINTN                           ControllerId;
  UINTN                           HubPort;
  UINTN                           TestMode;
} USB_HOST_PORT_TEST_APP_DATA;


/**
  Listing of application version and command line options.

  @return none

**/
STATIC
VOID
Usage (
  IN  USB_HOST_PORT_TEST_APP_DATA     *Data
  )
{
  UINTN i, ControllerCount;

  // check param, but can't fail printing usage
  if (NULL == Data) {
    ControllerCount = 0;
  } else {
    ControllerCount = Data->ControllerCount;
  }

  // print usage header
  Print(L"High-speed USB Host Mode Electrical Test Tool version %s\n"
        L"Usage: UsbHostPortTestApp \"CONTROLLER_ID TEST_MODE_ID [HUB_PORT_ID]\"\n",
        QCOM_UEFI_USB_ECT_VER );
  Print(L"  Controllers:\n"
        L"    ID  Description\n");

  // print info about available controllers
  for (i = 0; i < ControllerCount; ++i) {
    Print(L"    %2d  %s\n",
          i,
          Data->Controllers[i].PortTest->Identify(Data->Controllers[i].PortTest));

    if (Data->Controllers[i].HasHub) {
      Print(L"          Has %d port hub (1 based index)\n", Data->Controllers[i].HubPortCount);
    }
  }

  // print the test modes
  Print(L"  Test Modes:\n"
        L"    ID  Description\n"
        L"     0  TEST_DISABLE\n"
        L"     1  TEST_J_STATE\n"
        L"     2  TEST_K_STATE\n"
        L"     3  TEST_SE0/NAK\n"
        L"     4  TEST_PACKET\n"
        L"     5  TEST_FORCE_ENABLE\n"
        L"     6  SS compliance enable: detach cable before running command\n"
        L"    10  Suspend\n"
        L"    11  Resume\n"
        L"    12  Single step get device descriptor (HC packet parameters)\n"
        L"    13  Suspend+reset (Chirp timing)\n");
  Print(L"   101  TEST_J_STATE (Hub)\n"
        L"   102  TEST_K_STATE (Hub)\n"
        L"   103  TEST_SE0/NAK (Hub)\n"
        L"   104  TEST_PACKET (Hub)\n"
        L"   105  TEST_FORCE_ENABLE (Hub)\n");
}


/**
  Wait for KB input.

  @param Key   the key user pressed

  @retval EFI_SUCCESS  pressed key retrived

**/
STATIC
EFI_STATUS
PortTestGetKey (
  OUT EFI_INPUT_KEY         *Key
  )
{
  EFI_STATUS                      Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *SimpleTextIn = NULL;
  UINTN                           EventIndex;

  Status = gBS->LocateProtocol (
            &gEfiSimpleTextInProtocolGuid,
            NULL,
            (VOID **)&SimpleTextIn );
  if (EFI_ERROR (Status)) {
    Print ( L"failed to locate simple text input protocol\n" );
    return Status;
  }

  Status = gBS->WaitForEvent ( 1, &SimpleTextIn->WaitForKey, &EventIndex );
  if (EFI_ERROR (Status)) {
    Print ( L"failed to wait for key event\n" );
    return Status;
  }

  Status = SimpleTextIn->ReadKeyStroke ( SimpleTextIn, Key );
  if (EFI_ERROR (Status)) {
    Print ( L"failed to wait for key event\n");
    return Status;
  }
  return EFI_SUCCESS;
}


/**
  Get Hub Port number. Return 0 if transfer failed.

  @param[in]  Usb2Hc        Usb host controller protocol.
  @param[out] HubPortCount  Number of ports in hub.

  @retval   EFI_SUCCESS             parameter parsed OK
  @retval   EFI_INVALID_PARAMETER   Bad host controller protocol.

**/
STATIC
EFI_STATUS
GetHubPortCount (
  IN  EFI_USB2_HC_PROTOCOL       *Usb2Hc,
  OUT UINTN                      *HubPortCount
  )
{
  EFI_STATUS                   Status = EFI_SUCCESS;
  UINT32                       TransferResult;
  EFI_USB_DEVICE_REQUEST       Request;
  CHAR8                        Data[64];
  UINTN                        DataLength = sizeof (Data);

  if ( Usb2Hc == NULL || HubPortCount == NULL ) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem ( Data, sizeof (Data) );
  *HubPortCount = 0;

  Request.RequestType = 0xA0;     //GET_HUB_DESCRIPTOR
  Request.Request     = 0x06;     //GET_DESCRIPTOR
  Request.Value       = 0x2900;   //HUB_DESCRIPTOR type
  Request.Length      = DataLength;
  Request.Index       = 0x0;

  Status = Usb2Hc->ControlTransfer (
            Usb2Hc,
            1,                  //Address
            EFI_USB_SPEED_HIGH, //Speed
            16,                 //Max Packet Length
            &Request,           //Request
            EfiUsbDataIn,       //TransferDirection
            Data,               //Data
            &DataLength,        //DataLength
            1000,               //Timeout in millisec
            NULL,               //Translator
            &TransferResult     //TransferResult
            );

  if ( Status == EFI_SUCCESS && TransferResult == EFI_USB_NOERROR ) {
    *HubPortCount = Data[2]; //the 3rd byte is the port count of the hub
  }

  return Status;
}


/**
  Get Hub Port Status.

  @param[in]  Usb2Hc         Usb host controller protocol.
  @param[in]  HubPort        The hub port to query status.
  @param[out] HubPortStatus  Number of ports in hub.

  @retval   EFI_SUCCESS             parameter parsed OK
  @retval   EFI_INVALID_PARAMETER   Bad host controller protocol.

**/
STATIC
EFI_STATUS
GetHubPortStatus (
  IN  EFI_USB2_HC_PROTOCOL       *Usb2Hc,
  IN  UINTN                      HubPort,
  OUT UINTN                      *HubPortStatus
  )
{
  EFI_STATUS                   Status = EFI_SUCCESS;
  UINT32                       TransferResult;
  EFI_USB_DEVICE_REQUEST       Request;
  CHAR8                        Data[64];
  UINTN                        DataLength = 4;

  if ( Usb2Hc == NULL || HubPortStatus == NULL ) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem ( Data, sizeof (Data) );

  Request.RequestType = 0xA3;      //GET_PORT_STATUS
  Request.Request     = 0x0;       //GET_STATUS
  Request.Value       = 0x0;
  Request.Length      = DataLength;
  Request.Index       = HubPort;   //Hub Port

  Status = Usb2Hc->ControlTransfer (
            Usb2Hc,
            1,                  //Address
            EFI_USB_SPEED_HIGH, //Speed
            16,                 //Max Packet Length
            &Request,           //Request
            EfiUsbDataIn,       //TransferDirection
            Data,               //Data
            &DataLength,        //DataLength
            1000,               //Timeout in millisec
            NULL,               //Translator
            &TransferResult     //TransferResult
            );

  if ( Status == EFI_SUCCESS && TransferResult == EFI_USB_NOERROR ) {
    *HubPortStatus = *(UINTN*)Data;
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}


/**
  Execute commands for HSUSB hub testing. This feature is limited to HSUSB2 (8960 Liquid v3.2+).

  @param [in]  Data              Application data pointer

  @retval   EFI_SUCCESS             parameter parsed OK
  @retval   EFI_INVALID_PARAMETER   Wrong port number, or host controller not available.

**/
STATIC
EFI_STATUS
HubTestCmd (
  IN  USB_HOST_PORT_TEST_APP_DATA     *Data
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  USB_HOST_PORT_TEST_CONTROLLER   *Controller;
  UINT32                          TransferResult;
  EFI_USB_DEVICE_REQUEST          Request;
  UINT32                          Selector = 0;
  UINTN                           DataLength = 0, i, PortStatus = 0, TestModeMask = 1<<11;

  if (NULL == Data || FALSE == Data->Controllers[Data->ControllerId].HasHub) {
    Print (L"ERROR: HubTestCmd - invalid parameter\n");
    return EFI_INVALID_PARAMETER;
  }

  Controller = &Data->Controllers[Data->ControllerId];

  // translate hub command
  switch (Data->TestMode) {
    case CMD_HUB_TEST_J_STATE:      Selector = 1; break;
    case CMD_HUB_TEST_K_STATE:      Selector = 2; break;
    case CMD_HUB_TEST_SE0_NAK:      Selector = 3; break;
    case CMD_HUB_TEST_PACKET:       Selector = 4; break;
    case CMD_HUB_TEST_FORCE_ENABLE: Selector = 5; break;
    default:
        break;
  }

  if (Selector == 0) {
    Print ( L"ERROR: Invalid command option\n" );
    return EFI_INVALID_PARAMETER;
  }

  // set power to all ports
  for (i = 1; i <= Controller->HubPortCount; i++) {

    Request.RequestType = 0x23;
    Request.Request     = 0x03;   //SET_FEATURE
    Request.Value       = 0x08;   //PORT_POWER
    Request.Index       = i;      //Port
    Request.Length      = 0x0;

    Status = Controller->Usb2Hc->ControlTransfer (
              Controller->Usb2Hc,
              1,                  //Address
              EFI_USB_SPEED_HIGH, //Speed
              16,                 //Max Packet Length
              &Request,           //Request
              EfiUsbNoData,       //TransferDirection
              NULL,               //Data
              &DataLength,        //DataLength
              1000,               //Timeout in millisec
              NULL,               //Translator
              &TransferResult     //TransferResult
              );

    if (EFI_ERROR(Status) || TransferResult != EFI_USB_NOERROR) {
      Print (L"ERROR: USB transfer error 0x%x with hub port %d\n", TransferResult, i);
    }
  }

  // clear port connection
  for (i = 1; i <= Controller->HubPortCount; i++) {

    Request.RequestType = 0x23;
    Request.Request     = 0x01;   //CLEAR_FEATURE
    Request.Value       = 0x10;   //C_PORT_CONNECTION
    Request.Index       = i;      //Port
    Request.Length      = 0x0;

    Status = Controller->Usb2Hc->ControlTransfer (
              Controller->Usb2Hc,
              1,                  //Address
              EFI_USB_SPEED_HIGH, //Speed
              16,                 //Max Packet Length
              &Request,           //Request
              EfiUsbNoData,       //TransferDirection
              NULL,               //Data
              &DataLength,        //DataLength
              1000,               //Timeout in millisec
              NULL,               //Translator
              &TransferResult     //TransferResult
              );

    if (EFI_ERROR(Status) || TransferResult != EFI_USB_NOERROR) {
      Print (L"ERROR: USB transfer error 0x%x with hub port %d\n", TransferResult, i);
    }

    //Get Port Stutus
    GetHubPortStatus (Controller->Usb2Hc, i, &PortStatus);
  }

  // suspend all ports except the testing port
  for (i = 1; i <= Controller->HubPortCount; i++) {

    if (i == Data->HubPort) {
      continue;
    }

    Request.RequestType = 0x23;
    Request.Request     = 0x03;   //SET_FEATURE
    Request.Value       = 0x02;   //Suspend
    Request.Index       = i;
    Request.Length      = 0x0;

    Status = Controller->Usb2Hc->ControlTransfer (
              Controller->Usb2Hc,
              1,                  //Address
              EFI_USB_SPEED_HIGH, //Speed
              16,                 //Max Packet Length
              &Request,           //Request
              EfiUsbNoData,       //TransferDirection
              NULL,               //Data
              &DataLength,        //DataLength
              1000,               //Timeout in millisec
              NULL,               //Translator
              &TransferResult     //TransferResult
              );

    if (EFI_ERROR(Status) || TransferResult != EFI_USB_NOERROR) {
      Print (L"ERROR: USB transfer error 0x%x with hub port %d\n", TransferResult, i);
    }
  }

  // set the testing port to test mode
  {
    Request.RequestType = 0x23;
    Request.Request     = 0x03;    //SET_FEATURE
    Request.Value       = 0x15 ;   //PORT_TEST
    Request.Index       = (Selector << 8) + Data->HubPort;
    Request.Length      = 0x0;

    Status = Controller->Usb2Hc->ControlTransfer (
              Controller->Usb2Hc,
              1,                  //Address
              EFI_USB_SPEED_HIGH, //Speed
              16,                 //Max Packet Length
              &Request,           //Request
              EfiUsbNoData,       //TransferDirection
              NULL,               //Data
              &DataLength,        //DataLength
              1000,               //Timeout in millisec
              NULL,               //Translator
              &TransferResult     //TransferResult
              );

    if (EFI_ERROR(Status) || TransferResult != EFI_USB_NOERROR) {
      Print (L"ERROR: USB transfer error 0x%x with hub port %d\n", TransferResult, Data->HubPort);
    }

    Status = GetHubPortStatus (Controller->Usb2Hc, Data->HubPort, &PortStatus);
    if (EFI_ERROR(Status)) {
      Print (L"ERROR: Failed to get hub port %d status %r\n", Data->HubPort, Status );
    } else {
      Print (L"Hub port %d status (0x%x), test mode %d\n",
             Data->HubPort,
             PortStatus,
             PortStatus & TestModeMask ? 1 : 0
             );
    }
  }

  return Status;
}


/**
  Get device descriptor in steps.

  Host mode electrical testing for packet parameters requires an interval between
  each transaction of the Get Dev Desc control transfer. The interval is introduced
  by EHCI driver (EhcCreateQtds() in EhciDxe/EhciUrb.c) or XHCI driver. Ehci driver builds
  a list of QTDs but only one of them is transmitted at a time. XHCI driver similarly builds
  TRBs for the desired stage of transfer. The one to be transmitted is determined by
  USB_PORT_TEST_PARAM_QTD_INDEX, which is updated in this function when user presses enter.

  @param [in]  Data              Application data pointer

  @retval EFI_SUCCESS  All transactions done successfully
  @retval EFI_INVALID_PARAMETER  Either Device or UsbPortTest is NULL.

**/
STATIC
EFI_STATUS
GetDevDescriptorByStep (
  IN  USB_HOST_PORT_TEST_APP_DATA     *Data
  )
{
  EFI_INPUT_KEY                   Key;
  EFI_STATUS                      Status = EFI_SUCCESS;
  INTN                            Step = 0, MaxStep = 2;
  USB_HOST_PORT_TEST_CONTROLLER   *Controller;

  //only used in the call of GetCapability
  UINT8                         Speed, Port, Is64;

  if (NULL == Data) {
    Print (L"ERROR: GetDescriptorByStep - invalid parameter\n");
    return EFI_INVALID_PARAMETER;
  }

  Controller = &Data->Controllers[Data->ControllerId];

  // Disable periodic schedule (if not supported still returns success)
  Status = Controller->PortTest->RunPortTest (
            Controller->PortTest,
            USB_PORT_TEST_DISABLE_PERIODIC,
            NULL );
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Failed to disable periodic schedule\n");
    goto Exit_0;
  }

  while ( Step <= MaxStep) {
    Print (L"Press Enter to send next control transfer stage\n");

    //
    // Update Qtd Index in ECHI or Trb Index in XHCI
    // HC driver will transit the right stage as indicated in step
    //
    Status = Controller->PortTest->SetParam (
              Controller->PortTest,
              USB_PORT_TEST_PARAM_QTD_INDEX,
              Step );
    if (EFI_ERROR (Status)) {
      Print (L"ERROR: Failed to set Qtd Index for USB Port Test.\n" );
      goto Exit_1;
    }

    if (Step == 0) {
      // for an update of the UsbPortTestIndex in EchiDxe
      Controller->Usb2Hc->GetCapability (Controller->Usb2Hc, &Speed, &Port, &Is64);
    }

    while (1) {
      Status = PortTestGetKey (&Key);
      if (EFI_ERROR (Status)) {
        Print (L"ERROR: Failed to get key stroke.\n");
        goto Exit_1;
      }
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        Print (L"%c", Key.UnicodeChar);
        break;
      }
    }

    Controller->PortTestHelper->GetDeviceDescriptor (
        Controller->PortTestHelper);
    Step += 1;
  }

  Print ( L"Press any key to exit\n" );
  PortTestGetKey ( &Key );

Exit_1:
  // clear the Qtd Index
  Status = Controller->PortTest->SetParam (
            Controller->PortTest,
            USB_PORT_TEST_PARAM_QTD_INDEX,
            -1 );
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Failed to clear Qtd Index for USB Port Test.\n");
    goto Exit_1;
  }

  // re-enable the periodic schedule
  Status = Controller->PortTest->RunPortTest (
            Controller->PortTest,
            USB_PORT_TEST_ENABLE_PERIODIC,
            NULL );
  if ( EFI_ERROR (Status) ) {
    Print (L"ERROR: Failed to enable periodic schedule.\n");
  }

  // for an update of the UsbPortTestIndex in EchiDxe
  Controller->Usb2Hc->GetCapability (Controller->Usb2Hc, &Speed, &Port, &Is64);

Exit_0:
  return Status;
}


/**
  PortTestCmd.

  This function handles the commands not prefixed with CMD_TEST. These
  comands do not translate to EHCI port test modes defined EHCI/ChipIdea spec.

  @param [in]  Data              Application data pointer

  @retval EFI_SUCCESS            All transactions done successfully .
  @retval EFI_INVALID_PARAMETER  Data is NULL.
  @retval Others                 Failure in called function.
**/
STATIC
EFI_STATUS
PortTestCmd (
  IN  USB_HOST_PORT_TEST_APP_DATA     *Data
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  UINT8                           DeviceNumber = 0, ParentPort = 0;
  USB_HOST_PORT_TEST_CONTROLLER   *Controller;

  if (NULL == Data) {
    return EFI_INVALID_PARAMETER;
  }

  Controller = &Data->Controllers[Data->ControllerId];

  // Get the connected device info
  Status = Controller->PortTestHelper->GetDeviceInfo (
            Controller->PortTestHelper,
            &DeviceNumber,
            &ParentPort );

  if (EFI_ERROR (Status)) {
    Print (L"ERROR: Failed to obtain device info (%r). Is device connected?\n", Status);
    return Status;
  }

  // Perform the test
  switch (Data->TestMode) {
    case CMD_SUSPEND_ROOTPORT:
      Status = Controller->Usb2Hc->SetRootHubPortFeature (
                Controller->Usb2Hc,
                ParentPort,
                EfiUsbPortSuspend );
      break;
    case CMD_RESUME_ROOTPORT:
      Status = Controller->Usb2Hc->ClearRootHubPortFeature (
                Controller->Usb2Hc,
                ParentPort,
                EfiUsbPortSuspend );
      break;
    case CMD_GET_DESCRIPTOR:
      Status = GetDevDescriptorByStep (Data);
      break;
    case CMD_SUSPEND_RESET:
      //This command option is for the testing of chirp timing.
      //The reset itself is enough to produce the chirp. Doing the suspend first
      //because Agilent scope relies on the falling edge of D+ as the trigger.
      Status = Controller->Usb2Hc->SetRootHubPortFeature (
                Controller->Usb2Hc,
                ParentPort,
                EfiUsbPortSuspend );
      if (EFI_ERROR (Status)) {
        Print (L"ERROR: Suspend failed\n");
      } else {
        gBS->Stall ( 25000 );
        Status = Controller->Usb2Hc->SetRootHubPortFeature (
                  Controller->Usb2Hc,
                  ParentPort,
                  EfiUsbPortReset );
      }
      break;
    default:
      break;
  }

  if (EFI_ERROR (Status)) {
    Print (L"ERROR: PortTestCmd failed with Status: %r\n", Status);
    return Status;
  }

  return Status;
}


/**
  Run disconnect detect testing. The test has two steps:
  1. when the scope is ready and fixture has 525mv input, press enter
     and this function should print "Device still connected"
  2. toggle the switch in fixture and press enter again, the function
     should print "disconnect detected"

  @param   UsbPortTest   USB config

  @retval EFI_SUCCESS            test completed
  @retval EFI_INVALID_PARAMETER  invalid UsbPortTest

**/
STATIC
EFI_STATUS
DetectDisconnect (
  IN QCOM_USB_PORT_TEST_PROTOCOL *UsbPortTest
  )
{
  EFI_INPUT_KEY   Key;
  EFI_STATUS      Status = EFI_SUCCESS;
  UINTN           Data = 0;
  UINTN           Count = 0;

  if (UsbPortTest == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  while ( Count < 2 ) {
    Print ( L"Press any key to check disconnect Status\n" );

    PortTestGetKey ( &Key );

    Status = UsbPortTest->RunPortTest ( UsbPortTest,
                                        USB_PORT_TEST_GET_DISCONNECT_STATUS,
                                        &Data );

    if ( Data == 0 ) {
      Print ( L"Disconnect detected\n" );
    } else {
      Print ( L"Device still connected\n" );
    }
    Count += 1;
  }
  return Status;
}


/**
  Retrieve the command line and parse the 1st parameter into a number.

  @param[in,out] Data     Application data pointer

  @retval   EFI_SUCCESS             parameter parsed OK
  @retval   EFI_INVALID_PARAMETER   Less then 2 parameters.

**/
STATIC
EFI_STATUS
ParseCommandLine (
  IN OUT USB_HOST_PORT_TEST_APP_DATA  *Data
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINTN         Argc;
  CHAR8         **Argv;

  // Validate parameter
  if (NULL == Data || NULL == Data->ImageHandle) {
    Print(L"ERROR: PareCommandLine - invalid parameter\n");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // Get the command line arguments
  Status = GetCmdLineArgs(Data->ImageHandle, &Argc, &Argv);
  if (EFI_ERROR(Status)) {
    Print(L"ERROR: failed to parse command line arguments\n");
    goto ON_EXIT;
  }
  if (Argc < 2){
    Print(L"ERROR: at least 2 arguments required (%d supplied)\n", Argc);
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  //
  // First argument is Controller index (CONTROLLER_ID from usage)
  // Second argument is test mode command (TEST_MODE_ID from usage)
  // Third argument is hub port number (if using hub test mode)
  //
  // This function will return 0 if there is an invalid decimal string to parse,
  // so we will unknowingly move on if that is the case.
  //
  Data->ControllerId = AsciiStrDecimalToUintn(Argv[0]);
  Data->TestMode     = AsciiStrDecimalToUintn(Argv[1]);

  // Validate arguments in range
  if (Data->ControllerId >= Data->ControllerCount) {
    Print(L"ERROR: supplied CONTROLLER_ID (%d) out of range\n", Data->ControllerId);
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  switch (Data->TestMode) {
    case CMD_TEST_DISABLE:
    case CMD_TEST_J_STATE:
    case CMD_TEST_K_STATE:
    case CMD_TEST_SE0_NAK:
    case CMD_TEST_PACKET:
    case CMD_TEST_FORCE_ENABLE:
    case CMD_SUSPEND_ROOTPORT:
    case CMD_RESUME_ROOTPORT:
    case CMD_GET_DESCRIPTOR:
    case CMD_SUSPEND_RESET:
	case CMD_SS_COMPLIANCE_ENABLE:
      break;
    case CMD_HUB_TEST_J_STATE:
    case CMD_HUB_TEST_K_STATE:
    case CMD_HUB_TEST_SE0_NAK:
    case CMD_HUB_TEST_PACKET:
    case CMD_HUB_TEST_FORCE_ENABLE:
      // Check third param if using hub test mode
      if (Argc < 3) {
        Print(L"ERROR: must supply HUB_PORT_ID with hub test modes\n");
        Status = EFI_INVALID_PARAMETER;
        goto ON_EXIT;
      }
      if (!Data->Controllers[Data->ControllerId].HasHub) {
        Print(L"ERROR: controller %d has no hub\n", Data->ControllerId);
        Status = EFI_INVALID_PARAMETER;
        goto ON_EXIT;
      }
      Data->HubPort = AsciiStrDecimalToUintn(Argv[2]);
      if (Data->HubPort == 0 ||
          Data->HubPort > Data->Controllers[Data->ControllerId].HubPortCount) {
        Print(L"ERROR: supplied HUB_PORT_ID (%d) out of range\n", Data->HubPort);
        Status = EFI_INVALID_PARAMETER;
        goto ON_EXIT;
      }
      break;
    default:
      Print(L"ERROR: supplied TEST_MODE_ID (%d) not valid\n", Data->TestMode);
      Status = EFI_INVALID_PARAMETER;
      goto ON_EXIT;
  }

ON_EXIT:
  return Status;
}


/**
  @brief Close all opened protocols

  This function is the inverse of FindAvailableControllers.  It loops through all
  open protocols in the various controllers found and closes them before
  freeing the allocated memory.

  @param [in]  ImageHandle      Handle of the application image
  @param [in]  TestPorts        Pointer to Controller array
  @param [in]  ControllerCount  Number of elements in TestPorts
**/
STATIC
VOID
CloseAndFreeControllers (
  IN  EFI_HANDLE                    ImageHandle,
  IN  USB_HOST_PORT_TEST_CONTROLLER   *TestPorts,
  IN  UINTN                         ControllerCount
  )
{
  EFI_STATUS  Status;
  UINTN       i;

  if (NULL == ImageHandle || NULL == TestPorts) {
    Print(L"ERROR: CloseAndFreeControllers - invalid parameter\n");
    return;
  }

  for (i = 0; i < ControllerCount; ++i) {

    // Close Usb2Hc protocol
    if (NULL != TestPorts[i].Usb2Hc) {
      Status = gBS->CloseProtocol (
                TestPorts[i].Handle,
                &gEfiUsb2HcProtocolGuid,
                ImageHandle,
                TestPorts[i].Handle
                );
      if (EFI_ERROR(Status)) {
        Print(L"ERROR: failed to close Usb2Hc protocol on Controller[%d]\n", i);
      }
    }

    // Close PortTest protocol
    if (NULL != TestPorts[i].Usb2Hc) {
      Status = gBS->CloseProtocol (
                TestPorts[i].Handle,
                &gQcomUsbPortTestProtocolGuid,
                ImageHandle,
                TestPorts[i].Handle
                );
      if (EFI_ERROR(Status)) {
        Print(L"ERROR: failed to close PortTest protocol on Controller[%d]\n", i);
      }
    }

    // Close PortTestHelper protocol
    if (NULL != TestPorts[i].Usb2Hc) {
      Status = gBS->CloseProtocol (
                TestPorts[i].Handle,
                &gQcomUsbPortTestHelperProtocolGuid,
                ImageHandle,
                TestPorts[i].Handle
                );
      if (EFI_ERROR(Status)) {
        Print(L"ERROR: failed to close PortTestHelper protocol on Controller[%d]\n", i);
      }
    }
  } // end for TestPorts

  // free the Controller array that was allocated in FindAvailableControllers
  FreePool(TestPorts);
}


/**
  @brief Find all protocols needed for testing

  The Usb2Hc, UsbPortTest and UsbPortTestHelper protocols are needed for the
  various test commands supported by this application. Since a single host
  controller driver implementation (ehci/xhci) will provide instances of the
  Usb2Hc and UsbPortTest protocols and the UsbBus instance for that host
  controller will provide a UsbPortTestHelper, there may be multiple
  instances of each in the system. This function finds all such instances
  and provides a protocol mapping allowing the test code to be sure it is using
  all the protocol instancess that pertain to the same physical port.

  @param [in,out] Data          Application data pointer

  @return EFI_SUCCESS   At least 1 instance of all required protocols was found
  @return Others        Failed to find at least 1 instance of all protocols
**/
STATIC
EFI_STATUS
FindAvailableControllers (
  IN OUT USB_HOST_PORT_TEST_APP_DATA  *Data
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_HANDLE                    *Handles;
  UINTN                         NumHandles = 0;
  USB_HOST_PORT_TEST_CONTROLLER *PossibleControllers = NULL;
  UINTN                         ControllersFound = 0;
  UINTN                         HubPortCount;
  UINTN                         i, j;

  // validate parameters
  if (NULL == Data || NULL == Data->ImageHandle) {
    Print(L"ERROR: FindAvailableTestProtocols - invalid parameter\n");
    return EFI_INVALID_PARAMETER;
  }

  Data->Controllers     = NULL;
  Data->ControllerCount = 0;

  // Find all instances of the Usb2Hc protocol first
  Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiUsb2HcProtocolGuid,
            NULL,
            &NumHandles,
            &Handles
            );
  if (EFI_ERROR(Status)) {
    Print (L"ERROR: failed to find any host controllers\n");
    goto ON_EXIT;
  }

  // allocate Controller array, may not need all elements
  PossibleControllers = AllocateZeroPool(sizeof(USB_HOST_PORT_TEST_CONTROLLER) * NumHandles);
  if (NULL == PossibleControllers) {
    Print(L"ERROR: failed to allocate possible Controller array\n");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  // Loop through each handle to see if it has all the protocols we need
  for (i = 0; i < NumHandles; ++i) {
    EFI_GUID  **ProtocolBuffer;
    UINTN     NumProtocols      = 0;
    BOOLEAN   HasPortTest       = FALSE;
    BOOLEAN   HasPortTestHelper = FALSE;

    // Get the list of protocols installed on this handle
    Status = gBS->ProtocolsPerHandle(
              Handles[i],
              &ProtocolBuffer,
              &NumProtocols
              );
    if (EFI_ERROR(Status)) {
      Print(L"ERROR: no protocols installed on handle[%d] (0x%x)\n", i, Handles[i]);
      continue;
    }

    // Loop through protocols checking for those of interest (we already know it supports Usb2Hc)
    for (j = 0; j < NumProtocols; ++j) {

      // Check for UsbPortTestProtocol
      if (CompareGuid(ProtocolBuffer[j], &gQcomUsbPortTestProtocolGuid)) {
        HasPortTest = TRUE;
        continue;
      }

      // Check for UsbPortTestHelperProtocol
      if (CompareGuid(ProtocolBuffer[j], &gQcomUsbPortTestHelperProtocolGuid)) {
        HasPortTestHelper = TRUE;
      }
    } // end for protocols

    // Free protocol buffer allocated by gBS->ProtocolsPerHandle
    FreePool(ProtocolBuffer);

    // Open protocols if all are supported by this handle
    if (HasPortTest && HasPortTestHelper) {

      PossibleControllers[ControllersFound].Handle = Handles[i];

      // Open Usb2Hc
      Status = gBS->OpenProtocol(
                Handles[i],
                &gEfiUsb2HcProtocolGuid,
                (VOID **)&PossibleControllers[ControllersFound].Usb2Hc,
                Data->ImageHandle,
                Handles[i],
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
      if (EFI_ERROR(Status)) {
        Print(L"ERROR: failed to open Usb2Hc protocol\n");
        goto ON_EXIT;
      }

      // Open UsbPortTest
      Status = gBS->OpenProtocol(
                Handles[i],
                &gQcomUsbPortTestProtocolGuid,
                (VOID **)&PossibleControllers[ControllersFound].PortTest,
                Data->ImageHandle,
                Handles[i],
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
      if (EFI_ERROR(Status)) {
        Print(L"ERROR: failed to open UsbPortTest protocol\n");
        goto ON_EXIT;
      }

      // Open UsbPortTestHelper
      Status = gBS->OpenProtocol(
                Handles[i],
                &gQcomUsbPortTestHelperProtocolGuid,
                (VOID **)&PossibleControllers[ControllersFound].PortTestHelper,
                Data->ImageHandle,
                Handles[i],
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
      if (EFI_ERROR(Status)) {
        Print(L"ERROR: failed to open PortTestHelper protocol\n");
        goto ON_EXIT;
      }

      // Query for a hub attached to root port that could be used for testing
      Status = GetHubPortCount(PossibleControllers[ControllersFound].Usb2Hc, &HubPortCount);
      if (EFI_ERROR(Status) || HubPortCount == 0) {
        Status = EFI_SUCCESS;
        PossibleControllers[ControllersFound].HasHub       = FALSE;
        PossibleControllers[ControllersFound].HubPortCount = 0;
      } else {
        PossibleControllers[ControllersFound].HasHub       = TRUE;
        PossibleControllers[ControllersFound].HubPortCount = HubPortCount;
      }

      // Increment number of valid controllers found
      ControllersFound++;
    }
  } // end for handles

  // Free handle buffer allocated by gBS->LocateHandleBuffer
  FreePool(Handles);

  // copy over found TestPorts
  if (ControllersFound == NumHandles) {

    // all handles had all protocols, return PossibleControllers
    Data->Controllers     = PossibleControllers;
    Data->ControllerCount = ControllersFound;
    PossibleControllers     = NULL;

  } else if (ControllersFound > 0) { // found at least 1 Controller

    // allocate new Controller array
    Data->Controllers = AllocateZeroPool(sizeof(USB_HOST_PORT_TEST_CONTROLLER) * ControllersFound);
    if (NULL == Data->Controllers) {
      Print(L"ERROR: failed to allocate Controller array\n");
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }

    // copy over found TestPorts
    CopyMem(Data->Controllers, PossibleControllers, (sizeof(USB_HOST_PORT_TEST_CONTROLLER) * ControllersFound));
    Data->ControllerCount = ControllersFound;

  } else { // no TestPorts were found

    Status = EFI_NOT_FOUND;
  }

ON_EXIT:
  // clean up on error
  if (EFI_ERROR(Status)) {

    // close opened protocols and free temp array
    if (NULL != PossibleControllers) {
      CloseAndFreeControllers(Data->ImageHandle, PossibleControllers, NumHandles);
      PossibleControllers = NULL;
    }
    Data->Controllers = NULL;
    Data->ControllerCount = 0;
  }
  // always free temp array (if not already done)
  if (NULL != PossibleControllers) {
    FreePool(PossibleControllers);
  }
  return Status;
}


/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  The function has implemented two main functionalities

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UsbHostPortTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  USB_HOST_PORT_TEST_APP_DATA     AppData;
  USB_HOST_PORT_TEST_CONTROLLER   *ActiveController;

  TEST_START( TEST_NAME );

  // Initialize application data
  AppData.ImageHandle     = ImageHandle;
  AppData.Controllers     = NULL;
  AppData.ControllerCount = 0;
  AppData.ControllerId    = (UINTN)-1;
  AppData.HubPort         = (UINTN)-1;
  AppData.TestMode        = (UINTN)-1;

  // Find TestPorts before processing command line so we can print them
  Status = FindAvailableControllers(&AppData);
  if (EFI_ERROR(Status)) {
    Print(L"ERROR: failed to find any controllers '%r'\n", Status);
    goto ON_EXIT;
  }

  // Parse command line parameter
  Status = ParseCommandLine(&AppData);
  if (EFI_ERROR(Status)) {
    Usage(&AppData);
    goto ON_EXIT;
  }

  ActiveController = &AppData.Controllers[AppData.ControllerId];

  Print(L"Using controller %d (%s) for test mode %d\n",
      AppData.ControllerId,
      ActiveController->PortTest->Identify(ActiveController->PortTest),
      AppData.TestMode );

  // Perform the requested operation
  switch (AppData.TestMode) {

    // Test modes supported by PortTest protocol
    case CMD_TEST_DISABLE:
    case CMD_TEST_J_STATE:
    case CMD_TEST_K_STATE:
    case CMD_TEST_SE0_NAK:
    case CMD_TEST_PACKET:
    case CMD_TEST_FORCE_ENABLE:
    case CMD_SS_COMPLIANCE_ENABLE:
      Status = ActiveController->PortTest->RunPortTest (
                ActiveController->PortTest,
                (QCOM_USB_PORT_TEST_MODE)AppData.TestMode,
                NULL );
      if (EFI_ERROR (Status)) {
        Print ( L"ERROR: Port test failed: %r\n", Status);
        goto ON_EXIT;
      }

      if (AppData.TestMode == CMD_TEST_FORCE_ENABLE) {
        Status = DetectDisconnect (ActiveController->PortTest);
        if (EFI_ERROR(Status)) {
          Print(L"ERROR: DetectDisconnect failed: %r\n", Status);
          goto ON_EXIT;
        }
      }
      break;

    // Other test modes handled by this application
    case CMD_SUSPEND_ROOTPORT:
    case CMD_RESUME_ROOTPORT:
    case CMD_GET_DESCRIPTOR:
    case CMD_SUSPEND_RESET:
      Status = PortTestCmd (&AppData);
      if (EFI_ERROR(Status)) {
        Print(L"ERROR: Port test failed: %r\n", Status);
        goto ON_EXIT;
      }
      break;

    // Hub test modes handled by this application
    case CMD_HUB_TEST_J_STATE:
    case CMD_HUB_TEST_K_STATE:
    case CMD_HUB_TEST_SE0_NAK:
    case CMD_HUB_TEST_PACKET:
    case CMD_HUB_TEST_FORCE_ENABLE:
      Status = HubTestCmd (&AppData);
      if (EFI_ERROR(Status)) {
        Print(L"ERROR: Hub test failed: %r\n", Status);
        goto ON_EXIT;
      }
      break;

    default:
      Print(L"Unknown command %d\n", AppData.TestMode);
      Usage (&AppData);
      break;
  }

ON_EXIT:

  // free controller
  if (NULL != AppData.Controllers) {
    CloseAndFreeControllers(ImageHandle, AppData.Controllers, AppData.ControllerCount);
  }

  TestStatus( TEST_NAME, Status );
  TEST_STOP( TEST_NAME );

  return Status;
}

