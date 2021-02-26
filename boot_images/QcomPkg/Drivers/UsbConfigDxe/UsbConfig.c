/** @file UsbConfig.c

  QCOM_USB_CONFIG_PROTOCOL driver.

  Copyright (c) 2010-2017 Qualcomm Technologies, Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 02/02/17   pm       Port from Boot.xf.1.2.2, Vbus Enablement in USB Config Protocol
 04/12/16   ck       Add the support for Usb init protocol
 02/23/16   ck       Add the support to query connection state for Type-C port
 01/22/16   ck       Support on demand USBFn configuration
 07/07/15   ck       Add support to query max cores
 02/27/15   amitg    UEFI Low Power Mode Updates
 02/05/15   al       Changing TPL_CALLBACK to TPL_CALLBACK
 11/19/14   ck       Add toggle USB mode support
 11/07/14   ar       Add support for EHCI host mode
 10/22/14   amitg    UEFI LPM support
 08/23/13   mliss    Added GetSSUsbfnConfig
 08/03/12   amitg    Ported Changes for 8974
 06/01/12   kameya   Support for multiple USB cores in host mode in UEFI.
 03/30/11   sniedzie Created from PlatConfig.
 05/11/11   sbryksin Added platform/core type selection for UsbFn
 08/11/11   sbryksin Changes for 8960v2 w/ HSIC
 11/02/11   sbryksin Added initial APQ8064 revision
 12/07/31   amitg    Added initial MSM8974 revision
=============================================================================*/

#include "UsbConfigPrivate.h"

//
// global variables shared with other modules
//
EFI_PLATFORMINFO_PLATFORM_INFO_TYPE   PlatformInfo;
EFIChipInfoVersionType                chipVersion;
QCOM_USB_CONFIG_PROTOCOL              gUsbConfigTemplate = {
    QCOM_USB_CONFIG_PROTOCOL_REVISION,
    UsbConfigGetCoreBaseAddr,
    UsbConfigConfigUsb,
    UsbConfigResetUsb,
    UsbConfigGetUsbFnConfig,
    UsbConfigGetSSUsbFnConfig,
    UsbConfigGetConnStatus,
    UsbConfigGetUsbHostConfig,
    UsbConfigGetUsbMaxHostCoreNum,
    NULL,                          // UsbConfigExitBootService
    UsbConfigStartController,
    UsbConfigStopController,
    UsbConfigEnterLPM,
    UsbConfigExitLPM,
    UsbConfigToggleMode,
    UsbConfigGetCoreCount,
    UsbConfigGetSupportedMode,
    USB_CORE_0,
    USB_DEVICE_MODE_SS,
	UsbConfigGetVbusState,
    UsbConfigEnableVbus
};


//
// static variables
//
static EFI_EVENT UsbConfigToggleUsbModeEvt     = NULL;
static EFI_EVENT UsbConfigExitBootServiceEvent = NULL;
static EFI_EVENT UsbConfigInitUsbEvt           = NULL;


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigGetCoreBaseAddr (
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CONFIG_CORE_TYPE  CoreType,
  OUT  UINTN                      *BaseAddr
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = GetCoreBaseAddr (CoreType, BaseAddr);
  WRN_CHK ("failed to get USB base address");

  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigConfigUsb (
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_MODE_TYPE         Mode,
  IN  QCOM_USB_CORE_NUM          CoreNum
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = ConfigUsb (Mode, CoreNum);
  WRN_CHK ("failed to configure USB");

  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigResetUsb (
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_CONFIG_CORE_TYPE  CoreType,
  IN  BOOLEAN                    DeviceMode
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = ResetUsb (CoreType, DeviceMode);
  WRN_CHK ("failed to reset USB");

  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigGetUsbMaxHostCoreNum (
   IN  QCOM_USB_CONFIG_PROTOCOL  *This, 
   IN  QCOM_USB_MODE_TYPE        Mode,
   OUT UINT8                     *MaxCoreNum
   )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG();

  Status = GetUsbMaxHostCoreNum(Mode, MaxCoreNum);

  FNC_LEAVE_MSG();
  return Status; 
}

/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigGetVbusState (
   IN  QCOM_USB_CONFIG_PROTOCOL  *This, 
   IN  QCOM_USB_CORE_NUM         CoreNum,
   OUT UINT32                   *VbusStatus

   )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG();

  Status = GetUsbVbusState(CoreNum, VbusStatus);

  FNC_LEAVE_MSG();
  return Status; 
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigEnableVbus (
   IN  QCOM_USB_CONFIG_PROTOCOL  *This, 
   IN  QCOM_USB_CORE_NUM         CoreNum
   )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG();

  Status = UsbEnableVbus(CoreNum);

  FNC_LEAVE_MSG();
  return Status; 
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigGetUsbFnConfig (
   IN  QCOM_USB_CONFIG_PROTOCOL   *This, 
   OUT QCOM_USB_CONFIG_CORE_TYPE  *CoreType 
   )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = GetUsbFnConfig (CoreType);

  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigGetSSUsbFnConfig (
   IN  QCOM_USB_CONFIG_PROTOCOL   *This, 
   OUT QCOM_USB_CONFIG_CORE_TYPE  *CoreType 
   )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = GetSSUsbFnConfig (CoreType);

  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigGetConnStatus (
   IN  QCOM_USB_CONFIG_PROTOCOL   *This, 
   IN  QCOM_USB_CONFIG_CORE_TYPE   CoreType,
   OUT BOOLEAN                    *IsConnectionEstablished
   )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = GetUsbFnConnStatus(CoreType, IsConnectionEstablished);
  ERR_CHK("Failed to get GetSSUsbFnConnStatus with Error: %r", Status);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
EFIAPI
UsbConfigGetUsbHostConfig (
   IN  QCOM_USB_CONFIG_PROTOCOL   *This, 
   IN  QCOM_USB_MODE_TYPE         Mode,
   IN  QCOM_USB_CORE_NUM          CoreNum,
   OUT QCOM_USB_CONFIG_CORE_TYPE  *CoreType 
   )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = GetUsbHostConfig (Mode, CoreNum, CoreType);

  FNC_LEAVE_MSG ();
  return Status;
}


/*
* See header for documentation.
*/
EFI_STATUS
EFIAPI UsbConfigStartController(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CORE_NUM          CoreNum,
  IN   QCOM_USB_MODE_TYPE         ModeType
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG();

  Status = UsbStartController(CoreNum, ModeType);
  ERR_CHK("Failed to start controller");

ON_EXIT:

  FNC_LEAVE_MSG();
  return Status;
}


/*
* See header for documentation.
*/
EFI_STATUS
EFIAPI UsbConfigStopController(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CORE_NUM          CoreNum,
  IN   QCOM_USB_MODE_TYPE         ModeType
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG();

  Status = UsbStopController(CoreNum, ModeType);
  ERR_CHK("Failed to start controller");

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
* See header for documentation.
*/
EFI_STATUS
EFIAPI
UsbConfigEnterLPM (
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_MODE_TYPE         Mode,
  IN  QCOM_USB_CORE_NUM          CoreNum
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = EnterLPM (Mode, CoreNum);
  WRN_CHK ("failed to enter LPM");

  FNC_LEAVE_MSG ();
  return Status;
}

/*
* See header for documentation.
*/
EFI_STATUS
EFIAPI
UsbConfigExitLPM (
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_MODE_TYPE         Mode,
  IN  QCOM_USB_CORE_NUM          CoreNum
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = ExitLPM (Mode, CoreNum);
  WRN_CHK ("failed to exit LPM");

  FNC_LEAVE_MSG ();
  return Status;
}


/*
* See header for documentation.
*/
EFI_STATUS
EFIAPI
UsbConfigToggleMode(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  IN   QCOM_USB_CORE_NUM           CoreNum
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  Status = UsbToggleControllerMode(CoreNum);
  ERR_CHK("Failed to toggle controller mode: %r", Status);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
* See header for documentation.
*/
EFI_STATUS
EFIAPI
UsbConfigGetCoreCount(
  IN   QCOM_USB_CONFIG_PROTOCOL   *This,
  OUT  UINT8                      *Count
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();
  
  if (!This || !Count) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  Status = UsbConfigLibGetCoreCount(Count);
  ERR_CHK("Failed to get core count: %r", Status);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
* See header for documentation.
*/
EFI_STATUS
EFIAPI
UsbConfigGetSupportedMode(
  IN  QCOM_USB_CONFIG_PROTOCOL   *This,
  IN  QCOM_USB_CORE_NUM          CoreNum,
  OUT QCOM_USB_MODE_TYPE         *ModeType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  if (!This || !ModeType) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  Status = UsbConfigLibGetSupportedMode(CoreNum, ModeType);
  ERR_CHK("UsbConfigLibGetSupportedMode: %r", Status);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief The ExitBootSerivce event callback function 
 *
 * @param  Event                   Pointer to this event
 * @param  Context                 Event handler private data
 */
VOID
EFIAPI
UsbConfigExitBootService (
  IN EFI_EVENT  Event,
  IN VOID       *Context
   )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG ();

  Status = UsbConfigLibExitBootService();
  WRN_CHK ("Failed to do USB exit");

  if (UsbConfigExitBootServiceEvent) {
    gBS->CloseEvent(UsbConfigExitBootServiceEvent);
    UsbConfigExitBootServiceEvent = NULL;
  } 

  if (UsbConfigToggleUsbModeEvt) {
    gBS->CloseEvent(UsbConfigToggleUsbModeEvt);
    UsbConfigToggleUsbModeEvt = NULL;
  } 

  FNC_LEAVE_MSG ();
}


/**
 * @brief Toggle the USB primary port mode, triggered by gEfiEventToggleUsbModeGuid
 *
 * @param  Event                   Pointer to this event
 * @param  Context                 Event handler private data
 */
VOID
EFIAPI
UsbConfigToggleUsbModeCb (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{  
  EFI_STATUS Status = EFI_SUCCESS;
  
  FNC_ENTER_MSG ();

  // Toggle the USB mode on primary port.
  Status = UsbConfigToggleMode(&gUsbConfigTemplate, USB_CORE_0);  

  if (EFI_ERROR(Status)) {    
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
}


/**
 * @brief Start USB controller, triggered by gInitUsbControllerGuid
 *
 * @param  Event                   Pointer to this event
 * @param  Context                 Event handler private data
 */
VOID
EFIAPI
UsbConfigStartControllerCb (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS Status;
  
  FNC_ENTER_MSG ();
  
  Status = UsbStartController(
    USB_CORE_0,
    USB_DEVICE_MODE_SS);

  ERR_CHK("UsbStartController failed %r", Status);
  
ON_EXIT:
  FNC_LEAVE_MSG ();
  return;
}


/**
  Entry point for EFI drivers.

  @param  ImageHandle       EFI_HANDLE.
  @param  SystemTable       EFI_SYSTEM_TABLE.

  @return EFI_SUCCESS       Success.
          EFI_DEVICE_ERROR  Fail.

**/
EFI_STATUS
EFIAPI
UsbConfigDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  EFI_PLATFORMINFO_PROTOCOL  *hPlatformInfoProtocol = NULL;
  EFI_CHIPINFO_PROTOCOL      *hChipInfoProtocol     = NULL;
  
  FNC_ENTER_MSG ();

  //get platform info
  Status = gBS->LocateProtocol (
            &gEfiPlatformInfoProtocolGuid,
            NULL,
            (VOID **) &hPlatformInfoProtocol
            );
  ERR_CHK ("Failed to get gEfiPlatformInfoProtocolGuid");

  Status = hPlatformInfoProtocol->GetPlatformInfo(hPlatformInfoProtocol, &PlatformInfo);
  ERR_CHK ("Failed to get PlatformInfo");

  //Get chip version
  Status = gBS->LocateProtocol (&gEfiChipInfoProtocolGuid, NULL, (VOID**)&hChipInfoProtocol);
  ERR_CHK ( "Failed to install ChipInfo protocol");
  
  Status = hChipInfoProtocol->GetChipVersion(hChipInfoProtocol, 
                                             &chipVersion);
  ERR_CHK ("Failed to get chip version");
 
  //Register for boot services exit event.
  Status = gBS->CreateEventEx(
    EVT_NOTIFY_SIGNAL, 
    TPL_CALLBACK, 
    UsbConfigExitBootService,
    NULL,
    &gEfiEventExitBootServicesGuid,
    &UsbConfigExitBootServiceEvent);

  ERR_CHK ("Failed to register for boot services exit event");

  // Register for Toggle Usb Mode event
  Status = gBS->CreateEventEx(
    EVT_NOTIFY_SIGNAL,
    TPL_CALLBACK,
    UsbConfigToggleUsbModeCb,
    NULL, 
    &gEfiEventToggleUsbModeGuid,
    &UsbConfigToggleUsbModeEvt);

  ERR_CHK ("Failed to Create ToggleUsbModeEvent. %r", Status);
  
  // UsbConfigInit no longer triggers the hardware configuration
  // by default but only to set up few data structures needed
  // later for USB hardware initialization.
  Status = UsbConfigInit();
  ERR_CHK("UsbConfigInit failed, %r", Status);
  
  // Register for Config USB Event
  Status = gBS->CreateEventEx(
    EVT_NOTIFY_SIGNAL,
    TPL_CALLBACK,
    UsbConfigStartControllerCb,
    NULL,
    &gInitUsbControllerGuid,
    &UsbConfigInitUsbEvt);
  ERR_CHK("Failed to Create ConfigUsbEvent. %r", Status);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

