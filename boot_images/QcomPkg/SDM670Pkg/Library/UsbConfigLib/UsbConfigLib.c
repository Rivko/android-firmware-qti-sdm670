/** @file \UsbConfigLib.c
 * The file implemented the target specific USB configuration APIs used
 * by the UsbConfigDxe driver.
 * Copyright (c) 2015-2017 QUALCOMM Technologies Inc. All rights reserved.
 */

/*=============================================================================
                              EDIT HISTOYR


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 09/11/17   ay       UEFI USB compliance test changes
 07/19/17   amitg    Updated Core Count
 06/14/17   amitg    Cleanup
 04/26/17   amitg    Remove Power down sequence of primary core in Exit Boot Services
 04/19/17   amitg    Sec Core Updates, Primary Core enable pmic call
 03/22/17   pm       Add Sec core, Remove UsbCSetPortRole
 03/21/17   amitg    SDM845 UEFI USB Updates
 03/06/17   amitg    EUD Support
 02/03/17   pm       Vbus Enablement in USB Config Protocol
 02/02/17   pm       Port from Boot.xf.1.4
 01/19/17   pm       Fix KW issue
 05/13/16   ck       honor static configuration in stop controller
 05/06/16   ck       Temp workaround by enable VBUS asap
 04/12/16   ck       Add the support to USB init protocol
 03/31/16   ck       Fix incorrect param check
 02/23/16   ck       Isolate USB Core and Port data to its own module
 01/22/16   ck       Support on demand USBFn configuration
 01/14/16   ck       Fix incorrect call to enter lpm in exit UEFI service
 10/26/15   ck       Init 8998 Pre-Si support
=============================================================================*/

//
// Includes
//
#include "UsbConfigLibPrivate.h"
#include <Library/SerialPortShLib.h>

extern EFI_PLATFORMINFO_PLATFORM_INFO_TYPE    PlatformInfo;

//
// For each USB controller in the system, an USB_CONFIG_DEVICE is created to 
// hold the controller handles and configuration data associated with the
// current USB configuration.
// A dummy core is always appended at the end of the device array.
//
static USB_CONFIG_DEVICE gUsbConfigDevice[] = {
  { USB_CONFIG_DEV_SIGNATURE,     //signature
    NULL,                         //host handle
    NULL,                         //peripheral handle
    NULL,                         //QCOM_USB_CORE_INTERFACE Primary 
    NULL,                         //QCOM_USB_PORT handle Primary
    {0}                           //QCOM_USB_CONFIG_PROTOCOL
  }, // Primary
  { USB_CONFIG_DEV_SIGNATURE,     //signature
    NULL,                         //host handle
    NULL,                         //peripheral handle
    NULL,                         //QCOM_USB_CORE_INTERFACE Secondary 
    NULL,                         //QCOM_USB_PORT handle Secondary
    {0}                           //QCOM_USB_CONFIG_PROTOCOL
  }, // secondray
  { USB_CONFIG_DEV_SIGNATURE,     //signature
    NULL,                         //host handle
    NULL,                         //peripheral handle
    NULL,                         //QCOM_USB_CORE_INTERFACE dummy
    NULL,                         //QCOM_USB_PORT handle dummy
    {0}                           //QCOM_USB_CONFIG_PROTOCOL
  } //Dummy used in case no valid cores are initialized
};


/**
  Get the charger port type

  @param [in] CoreNum            Core number
  @param [Out] ChargerPortType    Charger port type

  @retval EFI_SUCCESS            Charger port type is retrieved correct
  @retval Others                 Operation failed.
**/
EFI_STATUS
GetChargerPortType(
  IN  QCOM_USB_CORE_NUM CoreNum,
  OUT EFI_PM_USB_CHGR_PORT_TYPE *ChargerPortType)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8   UefiVar;
  UINTN   UefiVarSize;
  
  // Check if the UEFI variable is set to enable device mode compliance testing
  // Override charger port type to SDP for compliance testing
  UefiVarSize = sizeof(UefiVar);
  Status = gRT->GetVariable(L"UsbfnForceComplianceMode", &gQcomTokenSpaceGuid, NULL, &UefiVarSize, &UefiVar);
  if (!EFI_ERROR(Status) && (UefiVar != 0)) {
    DBG(EFI_D_WARN, "USB Device mode compliance override set");
    *ChargerPortType = EFI_PM_USB_CHG_PORT_SDP;
    goto ON_EXIT;
  }

  *ChargerPortType = EFI_PM_USB_CHG_PORT_INVALID;

  // Get the PMIC USB charger protocol if not already available.
  if (NULL == gPmicUsbProtocol) {
    Status = gBS->LocateProtocol(&gQcomPmicUsbProtocolGuid, NULL, (void**)&gPmicUsbProtocol);
    if (EFI_ERROR(Status))
    {
      gPmicUsbProtocol = NULL;
      DBG(EFI_D_ERROR, "Failed to open PMIC USB protocol Status =  (0x%x)", Status);
      goto ON_EXIT;
    }
  }

  // ChargerPort function now guarantees that BC 1.2 charger detection finishes
  // before the function returns.
  Status = gPmicUsbProtocol->ChargerPort(ChargerPortType);
  if (EFI_ERROR(Status))
  {
    DBG(EFI_D_ERROR, "Failed to call PMIC ChargerPort Status =  (0x%x)", Status);
    goto ON_EXIT;
  }

ON_EXIT:
  return Status;
}


/**
  Returns core number for a given core type

  @param [in]  CoreType           USB core type.
  @param [out] CoreNum            Core number

  @retval EFI_SUCCESS             CoreNum is valid
  @retval EFI_INVALID_PARAMETER   CoreNum is NULL
  @retval EFI_UNSUPPORTED         Unsupported CoreType.
**/
EFI_STATUS
EFIAPI
GetCoreNum (
  IN  QCOM_USB_CONFIG_CORE_TYPE   CoreType,
  OUT QCOM_USB_CORE_NUM          *CoreNum
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if (NULL == CoreNum) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("GetCoreNum: Error - CoreNum is NULL");
  }

  switch (CoreType)
  {
    case USB_CONFIG_SSUSB1:
      *CoreNum = USB_CORE_0;
    break;	
	case USB_CONFIG_SSUSB2:
	  *CoreNum = USB_CORE_1;
	break;  
    default:
      DBG(EFI_D_ERROR, "GetCoreNum: Error - unsupported USB core type 0x%x", CoreType);
      Status = EFI_UNSUPPORTED;
      *CoreNum = (QCOM_USB_CORE_NUM) USB_CORE_MAX_NUM_SDM_670;
    break;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Returns base address of the specified USB core.

  @param [in]  CoreType           USB core type.
  @param [out] BaseAddr           Register set base address output.

  @retval EFI_SUCCESS             BaseAddr is valid
  @retval EFI_INVALID_PARAMETER   BaseAddr is NULL
  @retval EFI_UNSUPPORTED         Unsupported CoreType.
**/
EFI_STATUS
EFIAPI
GetCoreBaseAddr (
  IN  QCOM_USB_CONFIG_CORE_TYPE   CoreType,
  OUT UINTN                       *BaseAddr
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE_NUM   CoreNum;
  QCOM_USB_CORE_INTERFACE *UsbCoreIfc;

  FNC_ENTER_MSG ();

  if (NULL == BaseAddr) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("GetCoreBaseAddr: Error - BaseAddr is NULL");
  }

  if (CoreType >= USB_CONFIG_CORE_MAX) {
    Status = EFI_UNSUPPORTED;
    ERR_CHK("GetCoreBaseAddr: Error - unsupported USB core type 0x%x",CoreType);
  }

  Status = GetCoreNum(CoreType, &CoreNum);
  ERR_CHK ("failed to get USB core number");
  if (CoreNum >= USB_CORE_MAX_NUM_SDM_670) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("CoreNum out of range");
  }
  
  UsbCoreIfc = gUsbConfigDevice[CoreNum].UsbCoreInterface;
  Status = UsbCoreIfc->GetBaseAddress(UsbCoreIfc, BaseAddr);
  ERR_CHK("UsbCoreIfc->GetBaseAddress failed with status %r", Status);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Initializes the specified USB core for in host or device mode.

  @param [in] Mode              Host or device mode
  @param [in] CoreNum           Index of core supporting Mode to initialize

  @retval EFI_SUCCESS           USB initialized successfully.
  @retval EFI_UNSUPPORTED       Unsupported ConfigType.
**/
EFI_STATUS
EFIAPI
ConfigUsb (
  IN QCOM_USB_MODE_TYPE         Mode,
  IN QCOM_USB_CORE_NUM          CoreNum
  )
{
  EFI_STATUS                  Status   = EFI_SUCCESS;
  QCOM_USB_CORE_INTERFACE     *UsbCoreIfc;

  FNC_ENTER_MSG ();

  if (CoreNum > USB_CORE_1)
  {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK ("ConfigUsb: Error - Invalid CoreNum passed: %d", CoreNum);
  }

  DBG(EFI_D_INFO,"CoreNum %d, Mode %d", CoreNum, Mode);

  UsbCoreIfc = gUsbConfigDevice[CoreNum].UsbCoreInterface;

  USB_ASSERT_RETURN ((NULL != UsbCoreIfc), EFI_DEVICE_ERROR);
  
  // initialize the core
  USB_ASSERT_RETURN ((NULL != UsbCoreIfc->InitCommon), EFI_DEVICE_ERROR);
  Status = UsbCoreIfc->InitCommon(UsbCoreIfc, FALSE);
  ERR_CHK ("ConfigUsb: Error - failed to perform common initialization for USB core %d: %r",
      CoreNum, Status);

  if (Mode == USB_DEVICE_MODE_SS) {
    USB_ASSERT_RETURN ((NULL != UsbCoreIfc->InitDevice), EFI_DEVICE_ERROR);
    Status = UsbCoreIfc->InitDevice(UsbCoreIfc);
    ERR_CHK ("ConfigUsb: Error - failed to perform device initializaition for USB core %d: %r",
        CoreNum, Status);
  } else {
    USB_ASSERT_RETURN ((NULL != UsbCoreIfc->InitHost), EFI_DEVICE_ERROR);
    Status = UsbCoreIfc->InitHost(UsbCoreIfc);
    ERR_CHK ("ConfigUsb: Error - failed to perform host initializaition for USB core %d: %r",
        CoreNum, Status);
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Resets the USB core and reinitializes in the appropriate mode (host or device).

  @param [in] ConfigType        Configuration type
  @param [in] DeviceMode        Reinitialize in host or device mode

  @retval EFI_SUCCESS           USB reset successfully
  @retval Others                Error resetting core

**/
EFI_STATUS
EFIAPI
ResetUsb (
  IN QCOM_USB_CONFIG_CORE_TYPE  CoreType,
  IN BOOLEAN                    DeviceMode
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE_INTERFACE *UsbCoreIfc;
  QCOM_USB_CORE_NUM CoreNum;

  FNC_ENTER_MSG ();

  if (CoreType >= USB_CONFIG_CORE_MAX) {
    Status = EFI_UNSUPPORTED;
    ERR_CHK("ResetUsb: Error - unsupported USB core 0x%x", CoreType);
  }

  Status = GetCoreNum(CoreType, &CoreNum);
  ERR_CHK ("ResetUsb: Error - failed to get USB core number");
  if (CoreNum >= USB_CORE_MAX_NUM_SDM_670) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("CoreNum out of range");
  }
  
  UsbCoreIfc = gUsbConfigDevice[CoreNum].UsbCoreInterface;

  // reset the core
  USB_ASSERT_GOTO_SETSTS ((NULL != UsbCoreIfc->Reset), ON_EXIT,
                          EFI_DEVICE_ERROR, Status);
  Status = UsbCoreIfc->Reset(UsbCoreIfc);
  ERR_CHK ("ResetUsb: Error - failed to reset USB core %d: %r", CoreType, Status);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Enter low power mode for USB Core

  @param  [in] Mode             Host or device mode
  @param  [in] CoreNum          Index of core to be entered in low power mode

  @retval EFI_SUCCESS           USB core entered LPM successfully.
  @retval EFI_INVALID_PARAMETER Unsupported Core
  @retval Other                 Operation failed
**/

EFI_STATUS
EFIAPI
EnterLPM (
  IN QCOM_USB_MODE_TYPE         Mode,
  IN QCOM_USB_CORE_NUM          CoreNum
  )
{
  QCOM_USB_CORE_INTERFACE *UsbCoreIfc;
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  UsbCoreIfc = gUsbConfigDevice[CoreNum].UsbCoreInterface;

  if (CoreNum > USB_CORE_1)
  {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK ("EnterLPM: Error - Invalid core passed: %d", CoreNum);
  }

  Status = UsbCoreIfc->CoreEnterLPM(UsbCoreIfc);
  ERR_CHK ("EnterLPM: Error - failed to enter low power mode for USB coreNum %d: %r", CoreNum, Status);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Exits low power mode for USB Core

  @param [in] Mode                    Host or device mode
  @param [in] CoreNum                 Index of core supporting Mode to initialize

  @retval EFI_SUCCESS                 USB core exited LPM successfully.
  @retval EFI_INVALID_PARAMETER       Unsupported Core
  @retval Other                       Operation failed
**/

EFI_STATUS
EFIAPI
ExitLPM (
  IN QCOM_USB_MODE_TYPE         Mode,
  IN QCOM_USB_CORE_NUM          CoreNum
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE_INTERFACE *UsbCoreIfc;
 
  FNC_ENTER_MSG ();

  UsbCoreIfc = gUsbConfigDevice[CoreNum].UsbCoreInterface;

  if (CoreNum > USB_CORE_1)
  {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("ExitLPM: Error - Invalid core passed: %d", CoreNum);
  }

  Status = UsbCoreIfc->CoreExitLPM(UsbCoreIfc);
  ERR_CHK ("ExitLPM: Error - failed to exit low power mode for USB coreNum %d: %r", CoreNum, Status);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Return appropriate core type describing the ChipIdea core to be used in
  device mode. The Synopsys Usbfn driver will use a separate function.

  @param [out]  CoreType         Configuration type.

  @retval EFI_SUCCESS            Success
  @retval EFI_UNSUPPORTED        Unsupported ConfigType.

**/
EFI_STATUS
EFIAPI
GetUsbFnConfig (
  OUT QCOM_USB_CONFIG_CORE_TYPE *CoreType
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Return appropriate core type for each platform

  @param [out] CoreType          Configuration type.

  @retval EFI_SUCCESS            Success
  @retval EFI_UNSUPPORTED        Unsupported ConfigType.

**/
EFI_STATUS
EFIAPI
GetSSUsbFnConfig (
  OUT QCOM_USB_CONFIG_CORE_TYPE *CoreType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();
  
  if (NULL == CoreType) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("GetSSUsbFnConfig: Error - CoreType is NULL");
  }

  *CoreType = USB_CONFIG_SSUSB1;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Return appropriate core type to use for the specified host mode and index.

  @param [in]  Mode             USB_HOST_MODE_EHCI or USB_HOST_MODE_XHCI
  @param [in]  CoreNum          Core number supporting Mode
  @param [out] CoreType         Core type fitting above parameters

  @retval EFI_SUCCESS           CoreType contains a valid core
  @retval EFI_UNSUPPORTED       Unsupported combination of parameters

**/
EFI_STATUS
EFIAPI
GetUsbHostConfig (
  IN  QCOM_USB_MODE_TYPE        Mode,
  IN  QCOM_USB_CORE_NUM         CoreNum,
  OUT QCOM_USB_CONFIG_CORE_TYPE *CoreType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();
  
  if (NULL == CoreType) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("GetUsbHostConfig: Error - CoreType is NULL");
  }

  if (USB_CORE_0 != CoreNum) {
    Status = EFI_INVALID_PARAMETER;
    *CoreType = USB_CONFIG_CORE_MAX;
    ERR_CHK("GetUsbHostConfig: Error - CoreNum is %d, but expect USB_CORE_0", CoreNum);
  }

  *CoreType = USB_CONFIG_SSUSB1;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

/**
  UsbConfigInit does HW/driver initialization that's not dependent on other driver requests.

  @retval EFI_SUCCESS            Success.
  @retval EFI_DEVICE_ERROR       Failure

**/
EFI_STATUS
EFIAPI
UsbConfigInit (
  VOID
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT8                         UefiVar;
  UINTN                         UefiVarSize;
  UINTN                         Index;
  USB_TYPEC_PORT_MODE           PortMode = USB_TYPEC_PORT_MODE_DFP;
  
  FNC_ENTER_MSG ();
  
  // Check if there's already a stack running on the core. 
  // If InitUsbControllerOnBoot is one, this should not happen, and we should log the error message.
  // If InitUsbControllerOnBoot is zero, this may happen, and it depends on the client application implementation. 
  if (gUsbConfigDevice[USB_CORE_MAX_NUM_SDM_670].PeripheralHandle != NULL)
  {
    UINT32 PrintFlag = EFI_D_INFO;
    if (FeaturePcdGet(InitUsbControllerOnBoot)) {
      PrintFlag = EFI_D_ERROR;
    }
    Status = EFI_SUCCESS;
    DBG(PrintFlag, "USBConfigInit called more than once");
    goto ON_EXIT;
  }  

  // Allocate a handle w/ invalid core config to be used by clients to start a controller in case where no valid USB controller handles exist
  gUsbConfigDevice[USB_CORE_MAX_NUM_SDM_670].UsbConfig = gUsbConfigTemplate;
  gUsbConfigDevice[USB_CORE_MAX_NUM_SDM_670].UsbConfig.coreNum = (QCOM_USB_CORE_NUM)USB_CORE_MAX_NUM_SDM_670;
  gUsbConfigDevice[USB_CORE_MAX_NUM_SDM_670].UsbConfig.modeType = USB_INVALID_MODE;
  gUsbConfigDevice[USB_CORE_MAX_NUM_SDM_670].HostHandle = NULL;
  gUsbConfigDevice[USB_CORE_MAX_NUM_SDM_670].PeripheralHandle = NULL;
  // Install the protocol on PeripheralHandle, this is used as an indicator that
  // the UsbConfigInit has been called. 
  Status = gBS->InstallMultipleProtocolInterfaces(&gUsbConfigDevice[USB_CORE_MAX_NUM_SDM_670].PeripheralHandle,
    &gQcomUsbConfigProtocolGuid, &gUsbConfigDevice[USB_CORE_MAX_NUM_SDM_670].UsbConfig,
    NULL);
  ERR_CHK("UsbConfigInit: Error - Failed to install USB_CONFIG protocol");

  // Initializes UsbConfigDevice for the USB controllers in the system
  for (Index = 0; Index < USB_CORE_MAX_NUM_SDM_670; Index++) {

    gUsbConfigDevice[Index].UsbConfig = gUsbConfigTemplate;
    gUsbConfigDevice[Index].UsbConfig.coreNum = (QCOM_USB_CORE_NUM)Index;
    gUsbConfigDevice[Index].UsbConfig.modeType = USB_INVALID_MODE;
    gUsbConfigDevice[Index].HostHandle = NULL;
    gUsbConfigDevice[Index].PeripheralHandle = NULL;

    Status = UsbConfigCoresInit(
      gUsbConfigDevice[Index].UsbConfig.coreNum,
      &gUsbConfigDevice[Index].UsbCoreInterface);
    ERR_CHK("UsbConfigUsbCoresInit failed on Core %d, %r", 
      gUsbConfigDevice[Index].UsbConfig.coreNum, 
      Status);

    Status = UsbConfigPortsInit(
      gUsbConfigDevice[Index].UsbConfig.coreNum,
      &gUsbConfigDevice[Index].UsbPortHandle);
    ERR_CHK("UsbConfigPortsInit failed on Core %d, %r",
      gUsbConfigDevice[Index].UsbConfig.coreNum,
      Status);
  }

  // Check if we have Clamshell platform
  // We only support host mode for this platform
  if (IS_CLS_PLAT(PlatformInfo.platform)) {
  	
	  // Check if the UEFI variable is set to enable device mode compliance testing
      UefiVarSize = sizeof(UefiVar);
      Status = gRT->GetVariable(L"UsbfnForceComplianceMode", &gQcomTokenSpaceGuid, NULL, &UefiVarSize, &UefiVar);
      if (!EFI_ERROR(Status) && (UefiVar != 0)) {
      	DBG(EFI_D_WARN, "USB Device mode compliance override set");
     	// Enable static configuration for Device Mode
      	gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.Enabled = TRUE;
      	gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.UsbMode = USB_DEVICE_MODE_SS;
      	gUsbConfigDevice[USB_CORE_0_SDM_670].UsbCoreInterface->ModeType = USB_DEVICE_MODE;
      }
      else
      {
      	// Enable static configuration for Host Mode
      	gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.Enabled = TRUE;
      	gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.UsbMode = USB_HOST_MODE_XHCI;
      	gUsbConfigDevice[USB_CORE_0_SDM_670].UsbCoreInterface->ModeType = USB_HOST_MODE;

      	// Start Controller in host mode
      	DBG(EFI_D_WARN, "Start Controller");
      	Status = UsbStartController((QCOM_USB_CORE_NUM)USB_CORE_0_SDM_670, USB_HOST_MODE_XHCI);
      	ERR_CHK("Start controller failed with error: %r on core %d", Status, USB_CORE_0_SDM_670);
      }
  	}
  else {
    // Check if host mode is supported in UEFI.
    if (!FeaturePcdGet(HostModeSupported)) {
      gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.Enabled = TRUE;
      gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.UsbMode = USB_DEVICE_MODE_SS;
    } 
    else {
	  UINT64 StartTime, ElapsedTime, WaitTitme;
   
      // when PMIC change the port type to DRP, it stores the start time.
      // we need to ensure that at least TYPE_C_PORT_ROLE_CHANGE_MAX_TIME_IN_MS ms
      // has passed before calling UsbPortGetRtConnectionMode.
      Status = gPmicUsbProtocol->GetPortRoleSetTime(&StartTime);
      if (EFI_ERROR(Status)) {
        // wait for TYPE_C_PORT_ROLE_CHANGE_MAX_TIME_IN_MS ms as StartTime
        // cannot be determined;
        DBG(EFI_D_WARN, "GetPortRoleSetTime failed %r. Wait for %d ms", Status, TYPE_C_PORT_ROLE_CHANGE_MAX_TIME_IN_MS);
        gBS->Stall(TYPE_C_PORT_ROLE_CHANGE_MAX_TIME_IN_MS*1000);
        Status = EFI_SUCCESS;
      }
      else {
        ElapsedTime = GetTimerCountms() - StartTime;
        DBG(EFI_D_INFO, "ElapsedTime %d ms", ElapsedTime);
        if (ElapsedTime < TYPE_C_PORT_ROLE_CHANGE_MAX_TIME_IN_MS) {
          WaitTitme = TYPE_C_PORT_ROLE_CHANGE_MAX_TIME_IN_MS - ElapsedTime;
          // wait for WaitTitme;
          DBG(EFI_D_INFO, "Wait for %d ms", WaitTitme);
          gBS->Stall(WaitTitme * 1000);
        }	
      }
      // If the user plugs in the host cable during the boot time, configure the 
      // USB hardware in the host mode
      Status = UsbPortGetRtConnectionMode(gUsbConfigDevice[USB_CORE_0_SDM_670].UsbPortHandle, &PortMode);
      ERR_CHK(" Cannot Get Connection Mode for Core %d: %r", USB_CORE_0_SDM_670, Status);
      if (PortMode == USB_TYPEC_PORT_MODE_DFP) {
        DBG(EFI_D_WARN, "Enable Host Mode");
        gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.Enabled = TRUE;
        gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.UsbMode = USB_HOST_MODE_XHCI;
        gUsbConfigDevice[USB_CORE_0_SDM_670].UsbCoreInterface->ModeType = USB_HOST_MODE;
      }
      else {
        // Check if the UEFI variable is set to enable the host mode
        // if UsbHostPrimaryPort is set to host override, force host mode
        UefiVarSize = sizeof(UefiVar);
        Status = gRT->GetVariable(L"UsbHostPrimaryPort", &gQcomTokenSpaceGuid, NULL, &UefiVarSize, &UefiVar);
        if (!EFI_ERROR(Status) && (UefiVar != 0)) {
          DBG(EFI_D_WARN, "Host mode override set");
          gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.Enabled = TRUE;
          gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.UsbMode = USB_HOST_MODE_XHCI;
          gUsbConfigDevice[USB_CORE_0_SDM_670].UsbCoreInterface->ModeType = USB_HOST_MODE;
        }
      }
      if (gUsbConfigDevice[USB_CORE_0_SDM_670].UsbCoreInterface->ModeType == USB_HOST_MODE) {
        //Software based Vbus Enablement always
        Status = gPmicUsbProtocol->EnableOtg(FALSE);
        WRN_CHK("EnableOtg failed: %r", Status);
      }	  
    }
    
    // Create a timer to poll USB Type-C CC Status
    Status = gBS->CreateEvent(
      EVT_TIMER | EVT_NOTIFY_SIGNAL,
      TPL_CALLBACK,
      UsbPortStateChgCb,
      NULL,
      &PortStateChgEvt
      );
    ERR_CHK("Failed to create USB Type-C Event with error: %r", Status);

    // Signal the event immediately.
    gBS->SignalEvent(PortStateChgEvt);

    // Set the polling period for the USB Type-C periodic event callback
    Status = gBS->SetTimer(
      PortStateChgEvt,
      TimerPeriodic,
      USB_PORT_POLL_INTERVAL
      );
    ERR_CHK("Failed to set polling period for USB Type-C Event with error: %r", Status);
  }
ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/** 
  @brief Perform the clean up tasks for exiting boot service.

  When the UsbConfigExitBootServiceEvent is invoked in the UsbConfigDxe to prepare
  the lib for exiting the boot service.
  Unreleased resources, if any, allocated within the lib should now be freed.

  @retval EFI_SUCCESS            The operation succeeded.
  @retval EFI_UNSUPPORTED        The operation failed.

**/
EFI_STATUS
EFIAPI
UsbConfigLibExitBootService (
  VOID
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;

  QCOM_USB_CORE_INTERFACE *UsbCoreIfc;

  UINTN Index;
  
  FNC_ENTER_MSG ();

  if (PortStateChgEvt) {
    gBS->CloseEvent(PortStateChgEvt);
    PortStateChgEvt = NULL;
  }

  UsbCoreIfc = gUsbConfigDevice[USB_CORE_0_SDM_670].UsbCoreInterface;

  // if HostModeSupported is FALSE, the UEFI image is used for the OS where HS
  // PHY can be configured entirely in the HLOS. We simply put the PHY in the
  // Non-Drive Mode.
  if (!FeaturePcdGet(HostModeSupported)) {
    UsbCoreIfc->SetNonDrivingMode(UsbCoreIfc, TRUE);
  }
  else {
    // if HostModeSupported is TRUE, the UEFI image is used for the OS where the
    // debugger software does not re-initialize the HS PHY in general.
    // If the current mode in UEFI is host mode, nothing needs to be done.
    // else we need to not only power down the PHY but also configure it.
    if (gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.Enabled == TRUE &&
        gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.UsbMode == USB_HOST_MODE_XHCI) {
      DBG(EFI_D_INFO, "Host mode: Don't power down the HS PHY");
    }
    else {
      DBG(EFI_D_INFO, "Peripheral mode: power down the HS PHY");
      //UsbCoreIfc->InitHsPhyPwrDwn(UsbCoreIfc);
    }
    // Force debugger on secondary USB port for windows
    // Todo - Remove this once MSFT resolves in their code
    ConfigUsb (USB_DEVICE_MODE_SS, USB_CORE_1);
  }


  for (Index = 0; Index < USB_CORE_MAX_NUM_SDM_670; Index++) {
    UsbConfigCoresDeinit(gUsbConfigDevice[Index].UsbCoreInterface);
    UsbConfigPortsDeinit(gUsbConfigDevice[Index].UsbPortHandle);
  }

  FNC_LEAVE_MSG ();
  return Status;
}


/**

@brief Stop the USB controller.

@param [in] CoreNum            The assigned core number for the USB controller.
@param [in] ModeType           The current USB mode the USB controller operates on.

@retval EFI_SUCCESS            The operation succeeded.
@retval EFI_UNSUPPORTED        The operation failed.

**/
EFI_STATUS
EFIAPI UsbStopController(
  IN   QCOM_USB_CORE_NUM          CoreNum,
  IN   QCOM_USB_MODE_TYPE         ModeType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if ((CoreNum > USB_CORE_1) || (ModeType >= USB_INVALID_MODE))
  {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("UsbStopController: Error - Core %d. ModeType %d. current mode:", CoreNum, ModeType);
  }

  // If the client does not specify the HOST or Device mode type,
  // switch it to the appropriated one for the core
  if (ModeType == USB_HOST_MODE)
    ModeType = USB_HOST_MODE_XHCI;
  else if (ModeType == USB_DEVICE_MODE)
    ModeType = USB_DEVICE_MODE_SS;

  // If static configuration is enabled, use the static configuration value.
  if (gUsbConfigDevice[CoreNum].EnableStaticCfg.Enabled)
    ModeType = gUsbConfigDevice[CoreNum].EnableStaticCfg.UsbMode;

  // USB3.0 primary supports XHCI and device mode only.
  if ((ModeType != USB_HOST_MODE_XHCI) && (ModeType != USB_DEVICE_MODE_SS)) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK ("Error - Invalid ModeType %d", ModeType);
  }

  // check if the mode to be stopped is the one set before
  if (gUsbConfigDevice[CoreNum].UsbConfig.modeType != ModeType) {
    Status = EFI_NOT_STARTED;
    ERR_CHK("Error - Unable to stop controller on core %d (curr mode %d, target mode %d)",
      CoreNum,
      gUsbConfigDevice[CoreNum].UsbConfig.modeType,
      ModeType);
  }

  // Stop Peripheral/Host Stack
  DBG(EFI_D_INFO, "Stopping Core %d in mode: %d ", CoreNum, ModeType);
  if (ModeType == USB_DEVICE_MODE_SS) {
    ARG_CHK(gUsbConfigDevice[CoreNum].PeripheralHandle);

    Status = gBS->DisconnectController(gUsbConfigDevice[CoreNum].PeripheralHandle, NULL, NULL);
    ERR_CHK("Failed to disconnect QcomUsbConfig protocol peripheral handle with Error: %r", Status);
    gUsbConfigDevice[CoreNum].PeripheralHandle = NULL;
  }
  else {
    ARG_CHK(gUsbConfigDevice[CoreNum].HostHandle);

    Status = gBS->DisconnectController(gUsbConfigDevice[CoreNum].HostHandle, NULL, NULL);
    ERR_CHK("Failed to disconnect QcomUsbConfig protocol host handle with error: %r", Status);
    gUsbConfigDevice[CoreNum].HostHandle = NULL;
  }

  //clear out device struct
  gUsbConfigDevice[CoreNum].UsbConfig.modeType = USB_INVALID_MODE;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**

@brief Start the USB controller.

@param [in] CoreNum            The assigned core number for the USB controller.
@param [in] ModeType           The current USB mode the USB controller operates on.

@retval EFI_SUCCESS            The operation succeeded.
@retval EFI_UNSUPPORTED        The operation failed.

**/
EFI_STATUS
EFIAPI UsbStartController(
  IN   QCOM_USB_CORE_NUM          CoreNum,
  IN   QCOM_USB_MODE_TYPE         ModeType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  DBG(EFI_D_INFO, "CoreNum %d, ModeType %d", CoreNum, ModeType);

  if ((CoreNum > USB_CORE_1) || (ModeType >= USB_INVALID_MODE))
  {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK ("UsbStartController: Error - Core %d. ModeType %d. current mode:", CoreNum, ModeType);
  }

  // If the client does not specify the HOST or Device mode type,
  // switch it to the appropriated one for the core
  if (ModeType == USB_HOST_MODE)
    ModeType = USB_HOST_MODE_XHCI;
  else if (ModeType == USB_DEVICE_MODE)
    ModeType = USB_DEVICE_MODE_SS;

  //USB3.0 primary supports XHCI and device mode only (HS/SS)
  if ((CoreNum == USB_CORE_0) && (ModeType != USB_HOST_MODE_XHCI) && (ModeType != USB_DEVICE_MODE_SS))
  {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK ("UsbStartController: Error - attempt to set primary core to ModeType %d", ModeType); 
  }

  // If static configuration is enabled, use the static configuration value.
  if (gUsbConfigDevice[CoreNum].EnableStaticCfg.Enabled) {
    if (ModeType != gUsbConfigDevice[CoreNum].EnableStaticCfg.UsbMode) {
      DBG(EFI_D_WARN, "Static Config Mode (%d) overrides Client Selected Mode (%d)",
          gUsbConfigDevice[CoreNum].EnableStaticCfg.UsbMode,
          ModeType);
    }
    ModeType = gUsbConfigDevice[CoreNum].EnableStaticCfg.UsbMode;
  }

  //if core configured in same mode, ignore duplicate request
  if (gUsbConfigDevice[CoreNum].UsbConfig.modeType == ModeType)
  {
   DBG(EFI_D_WARN, "CoreNum %d, ModeType %d - Duplicate Request", CoreNum, ModeType);
    Status = EFI_SUCCESS;
    goto ON_EXIT;
  }
  else
  {
    // if the current usb mode is valid, stop the current one
    if (gUsbConfigDevice[CoreNum].UsbConfig.modeType != USB_INVALID_MODE) {
      Status = UsbStopController(CoreNum, gUsbConfigDevice[CoreNum].UsbConfig.modeType);
      ERR_CHK("UsbStopController: Error - Unable to stop controller on core %d, Status =  (0x%x)", CoreNum, Status);
    }
  }

  DBG(EFI_D_INFO, "Starting Core %d in mode: %d", CoreNum, ModeType);

  gUsbConfigDevice[CoreNum].UsbConfig.coreNum = CoreNum;
  gUsbConfigDevice[CoreNum].UsbConfig.modeType = ModeType;

  // Install/Connect Peripheral/Host Handle
  if (ModeType == USB_DEVICE_MODE_SS)
  {
    if (gUsbConfigDevice[CoreNum].PeripheralHandle == NULL) {
      //if handle is NULL, new one is allocated
      Status = gBS->InstallMultipleProtocolInterfaces(&gUsbConfigDevice[CoreNum].PeripheralHandle,
        &gQcomUsbConfigProtocolGuid, &gUsbConfigDevice[CoreNum].UsbConfig, NULL);
      ERR_CHK("Failed to install USB_CONFIG protocol (Peripheral Client Handle) with error: %r", Status);
    }

    Status = gBS->ConnectController(gUsbConfigDevice[CoreNum].PeripheralHandle, NULL, NULL, TRUE);
    ERR_CHK("Failed to connect QcomUsbConfig protocol peripheral handle with error: %r", Status);
  }
  else {
    if (gUsbConfigDevice[CoreNum].HostHandle == NULL) {
      //if handle is NULL, new one is allocated
      Status = gBS->InstallMultipleProtocolInterfaces(&gUsbConfigDevice[CoreNum].HostHandle,
        &gQcomUsbConfigProtocolGuid, &gUsbConfigDevice[CoreNum].UsbConfig, NULL);
      ERR_CHK("Failed to install USB_CONFIG protocol (Host Client Handle) with error: %r", Status);
    }
    Status = gBS->ConnectController(gUsbConfigDevice[CoreNum].HostHandle, NULL, NULL, TRUE);
    ERR_CHK("Failed to connect QcomUsbConfig protocol host handle with error: %r", Status);
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  This routine toggles the USB mode configuration on the core specified by CoreNum

  @param [in] CoreNum         The assigned core number for the USB controller.

  @retval EFI_SUCCESS         The operation succeeded.
  @retval EFI_UNSUPPORTED     The operation failed.

**/
EFI_STATUS
EFIAPI UsbToggleControllerMode(
  IN   QCOM_USB_CORE_NUM          CoreNum
  )
{
  EFI_STATUS             Status       = EFI_SUCCESS;
  QCOM_USB_MODE_TYPE     CurrModeType = USB_INVALID_MODE;
  QCOM_USB_MODE_TYPE     NextModeType = USB_INVALID_MODE;

  FNC_ENTER_MSG ();

  if (CoreNum > USB_CORE_1) {
   Status = EFI_INVALID_PARAMETER;
   ERR_CHK ( "UsbToggleControllerMode: Error - Invalid CoreNum %d", CoreNum);
  }

  if (gUsbConfigTemplate.modeType == USB_INVALID_MODE) {
    Status = EFI_NOT_READY;
    ERR_CHK ( "UsbToggleControllerMode: Error - ModeType is USB_INVALID_MODE");
  }

  // store the current mode type. UsbStopController will set the
  // gUsbConfigTemplate.modeType to USB_INVALID_MODE
  CurrModeType = gUsbConfigTemplate.modeType;

  // The new mode on the core:
  // For Sdm845, only switch from USB_DEVICE_MODE_SS to USB_HOST_MODE_XHCI
  switch (CurrModeType) {
    case USB_DEVICE_MODE_SS:
    NextModeType = USB_HOST_MODE_XHCI;
    break;
    case USB_DEVICE_MODE_HS:
    case USB_HOST_MODE_XHCI:
    case USB_HOST_MODE_EHCI:
    NextModeType = USB_INVALID_MODE;
    break;
    default:
    NextModeType = USB_INVALID_MODE;
  }

  if (NextModeType == USB_INVALID_MODE) {
    Status = EFI_UNSUPPORTED;
    ERR_CHK("Error - Setting USB_INVALID_MODE (on core %d) is not supported", CoreNum);
  }

  DBG(EFI_D_VERBOSE, "Starting toggling controller mode on core %d from mode %d to %d", CoreNum, CurrModeType, NextModeType);

  Status = UsbStopController(CoreNum, CurrModeType);
  ERR_CHK ("UsbToggleControllerMode: Error - Unable to stop controller on core %d, Status =  (%r)", CoreNum, Status);

  Status = UsbStartController(CoreNum, NextModeType);
  ERR_CHK ("UsbToggleControllerMode: Error - Unable to start controller on core %d, Status =  (%r)", CoreNum, Status);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Returns maximum number of cores supported in the host mode

  @param [in]  Mode              USB mode
  @param [in]  MaxCoreNum        Number of cores supported

  @retval EFI_SUCCESS            The operation succeeded.
  @retval EFI_UNSUPPORTED        The operation failed.

**/
EFI_STATUS
EFIAPI
GetUsbMaxHostCoreNum(
  IN  QCOM_USB_MODE_TYPE  Mode,
  OUT UINT8               *MaxCoreNum
  )
{
  // for Sdm845, the function is not used.
  // if a use case shows up, the function needs to be
  // revisited and provide the actual implementation. 
  return EFI_UNSUPPORTED;
}


/**
@summary
Return the total number of USB cores in the system

@param [out] Count          Pointer to the count of the system USB cores.

@retval EFI_SUCCESS         The operation succeeded.
@retval Others              The operation failed.

**/
EFI_STATUS
UsbConfigLibGetCoreCount(
  IN   UINT8          *Count
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!Count) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  *Count = USB_CORE_MAX_NUM_SDM_670;

ON_EXIT:
  return Status;
}


/**
@summary
Return the total number of USB cores in the system

@param [in]  CoreNum        Core number
@param [out] ModeType       Pointer to the supported mode type

@retval EFI_SUCCESS         The operation succeeded.
@retval Other               The operation failed.

**/
EFI_STATUS
UsbConfigLibGetSupportedMode(
  IN  QCOM_USB_CORE_NUM          CoreNum,
  OUT QCOM_USB_MODE_TYPE         *ModeType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (CoreNum > USB_CORE_1 || !ModeType) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  *ModeType = USB_DEVICE_MODE | USB_HOST_MODE;

ON_EXIT:
  return Status;
}

/**
@summary
Update the lane number.

The function updates the lane number in the software. The new value is not updated
to the hardware in the function. 

@param [in]  CoreNum        Core number

@retval EFI_SUCCESS         The operation succeeded.
@retval Other               The operation failed.

**/
STATIC
EFI_STATUS
UpdateLaneNumber(
  IN QCOM_USB_CORE_NUM    CoreNum)
{
  EFI_STATUS              Status;
  UINT8                   LaneNum;
  USB_TYPEC_CC_OUT_STATUS CcStatus;
  QCOM_USB_CORE_INTERFACE *UsbCoreIfc;
  
  if (CoreNum >= USB_CORE_MAX_NUM_SDM_670) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCoreIfc = gUsbConfigDevice[CoreNum].UsbCoreInterface;

  // Get the CC status from PMIC
  Status = UsbPortGetCcOutStatus(gUsbConfigDevice[CoreNum].UsbPortHandle, &CcStatus);
  WRN_CHK("UsbPortGetCcOutStatus failed with status %r. Assume CC1", Status);

  // decide the Lane number based on the CC status
  if (CcStatus == USB_TYPEC_CC_OUT_CC1)
    LaneNum = USB_SS_PHY_LANE_A;
  else if (CcStatus == USB_TYPEC_CC_OUT_CC2)
    LaneNum = USB_SS_PHY_LANE_B;
  else {
    LaneNum = USB_SS_PHY_LANE_A;
    WRN_CHK("CC out status is neither CC1 or CC2, but %d. Default to CC1", CcStatus);
  }
  
  DBG (EFI_D_INFO,"LaneNum = %d", LaneNum);

  // Update the lane number
  Status = UsbCoreIfc->SetPhyLane(UsbCoreIfc, LaneNum);
  ERR_CHK("Set CC line failed with error: %r on core %d", Status, CoreNum);

ON_EXIT:
  return Status;
}

/**
 Handle Attach or Detach event

@param [in]  CoreNum       The core for which the event is.
@param [in]  Message       Attach or Detach message
@param [in]  PortMode      The current mode of the port.

@retval EFI_SUCCESS        The operation succeeded
@retval Other              The operation failed.
**/
STATIC
EFI_STATUS
HandlePortPartnerXtach(
  IN QCOM_USB_CORE_NUM    CoreNum,
  IN USB_CFG_PORT_MESSAGE Message,
  IN USB_TYPEC_PORT_MODE  PortMode)
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_CORE_INTERFACE *UsbCoreIfc;
  QCOM_USB_MODE_TYPE UsbCfgMode;
  EFI_PM_USB_CHGR_PORT_TYPE ChargerType = EFI_PM_USB_CHG_PORT_INVALID;

  FNC_ENTER_MSG();

  // Set the charger type to SDP for RUMI
  if ((PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI)) {
    DBG(EFI_D_INFO, "Set Charger Type to SDP for RUMI");
    ChargerType = EFI_PM_USB_CHG_PORT_SDP;
    PortMode = USB_TYPEC_PORT_MODE_UFP;
  } 

  if ((CoreNum >= USB_CORE_MAX_NUM_SDM_670) ||
      (Message != UsbCfgPortMsgPartnerAttach && Message != UsbCfgPortMsgPartnerDetach)) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  UsbCoreIfc = gUsbConfigDevice[CoreNum].UsbCoreInterface;

  if (Message == UsbCfgPortMsgPartnerAttach) { //Attach

    DBG(EFI_D_INFO, "Cable Attach");

    // Update the LaneNumber variable in software.
    Status = UpdateLaneNumber(CoreNum);
    WRN_CHK ("UpdateLaneNumber failed on Core %d: %r", Status, CoreNum);

    // Upon cable attach, we need to consider whether to reset the USB hardware
    // If IS_HARDWARE_CONFIUGRED is false, do nothing as the USB hardware will be
    // configured later. When the UsbConfigDxe is first loaded, 
    // IS_HARDWARE_CONFIUGRED(modeType) returns false as modeType remains in 
    // USB_INVALID_MODE until that StartController function is invoked.
    // For LA, StartController is invoked by the fastboot App.
    // For WP, StartController is invoked later in this function.
    if (IS_HARDWARE_CONFIUGRED(gUsbConfigDevice[CoreNum].UsbConfig.modeType)) {
      // If we are here, it indicates the USB stack is loaded. We do a full reinit if the
      // following conditions are met.
      // (1) The device is configured for the peripheral use case.
      //   Host mode requires XHCIDxe changes which may not worth the effort as
      //   the USB host mode use case is limited. 
      //   The reinit will also bring HS PHY out of non-driving mode.
      // (2) The local port is UFP.
      //   Even if the USB hardware is configured as the peripheral mode, it does not prevent the USER
      //   to plug in a host cable. In that case, PMI charger detection is also not needed.
      // (3) The charger type is SDP or CDP
      //   If we do not need to enumerate the USB device, HS PHY can stay in the power down state.
      if (IS_USB_PERIPHERAL_MODE(gUsbConfigDevice[CoreNum].UsbConfig.modeType))
    { 
    Status = GetChargerPortType(CoreNum, &ChargerType);
        if (EFI_ERROR(Status)) { // cannot get charger type, assume invalid
          WRN_CHK("Cannot Get Charger Type");
        }
        if (IS_PORT_UFP(PortMode) && IS_SDP_CDP(ChargerType)) {
      // For the Device Mode, reinit ONLY the USB Link and PHY. Keep the clocks and power rails on.
      Status = UsbCoreIfc->InitCommon(UsbCoreIfc,TRUE);
      ERR_CHK ("ConfigUsb: Error - failed to perform common initialization for USB core %d: %r", CoreNum, Status);
          
      Status = UsbCoreIfc->InitDevice(UsbCoreIfc);
      ERR_CHK ("ConfigUsb: Error - failed to perform device initializaition for USB core %d: %r", CoreNum, Status);
    }
    }
      // for host mode, we still need to reset the SS PHY in order to update the lane number
      else if (IS_USB_HOST_MODE(gUsbConfigDevice[CoreNum].UsbConfig.modeType))
    {
    if (!FeaturePcdGet(HighSpeedOnlySupported)) 
    {
      Status = UsbCoreIfc->ResetSSPhy(UsbCoreIfc);
      ERR_CHK("SS PHY Reset failed with status %r", Status);
    }
    }
      else {
        DBG(EFI_D_WARN, "UsbModeType: %d",gUsbConfigDevice[CoreNum].UsbConfig.modeType);
      }
    }

    // For WP, call StartController automatically here as there is no client to
    // perform the task.
    if (FeaturePcdGet(InitUsbControllerOnBoot)) {
      BOOLEAN StCtrler = FALSE;
      // if static cfg for the host mode is set, always start the controller.
      if (gUsbConfigDevice[CoreNum].EnableStaticCfg.Enabled == TRUE &&
          gUsbConfigDevice[USB_CORE_0_SDM_670].EnableStaticCfg.UsbMode == USB_HOST_MODE_XHCI) {
        StCtrler = TRUE;
      // USB Init protocol is going to initialize the port for device mode
      }

      // Start controller in Peripheral mode as default.
      // If gUsbConfigDevice[CoreNum].EnableStaticCfg.Enabled is TRUE, the mode
      // defined by gUsbConfigDevice[CoreNum].EnableStaticCfg.UsbMode will override
      // the one we set for UsbCfgMode.
      if (StCtrler) {
        UsbCfgMode = USB_DEVICE_MODE_SS;
        DBG(EFI_D_WARN, "Start Controller");
        Status = UsbStartController(CoreNum, UsbCfgMode);
        ERR_CHK("Start controller failed with error: %r on core %d", Status, CoreNum);
      }
    }
  }
  else { // Detach
    // do nothing as dynamic role switching based on CC status is not supported.
    DBG(EFI_D_WARN, "Cable Detach");

    // if the device is in the peripheral mode, put the HS PHY into non-driving
    // after detach.
    if (IS_USB_PERIPHERAL_MODE(gUsbConfigDevice[CoreNum].UsbConfig.modeType)) {
      // after cable detach, put the HS into non-driving
      UsbCoreIfc->SetNonDrivingMode(UsbCoreIfc, TRUE);
    }
  }
ON_EXIT:
  if (Status != EFI_SUCCESS)
    DBG(EFI_D_ERROR, "Error encountered in UsbPortStateCbGetCcStatus");

  FNC_LEAVE_MSG();
  return Status;
}


// Check UsbConfigLibPrivate.h for the description
STATIC
VOID
EFIAPI
UsbPortStateChgCb(
  IN EFI_EVENT       Event,
  IN VOID           *Context
  )
{
  EFI_STATUS Status;
  UINTN Index;
  USB_CFG_PORT_MESSAGE Message;
  UINTN PayloadSize;
  USB_TYPEC_PORT_MESSAGE_PAYLOAD Payload;

  FNC_ENTER_MSG();

  // Poll only for Core 0
  for (Index = 0; Index < USB_CORE_MAX_NUM_SDM_670; Index++) {
    Status = UsbConfigPortsEventHandler(
      gUsbConfigDevice[Index].UsbPortHandle,
      &Message,
      &PayloadSize,
      &Payload);

    switch (Message) {
    case UsbCfgPortMsgPartnerAttach:
    case UsbCfgPortMsgPartnerDetach:
      Status = HandlePortPartnerXtach(Index, Message, Payload.UsbTypeCPortMode);
      WRN_CHK("HandlePortPartnerXtach failed %r", Status);
    default:
      break;
    }
  }

  FNC_LEAVE_MSG();
}

/**
Return connection state for USB Type-C Port

@param [in]  CoreType      The assigned the core type for the controller.
@param [out] IsConnected   Is the port partner connected.

@retval EFI_SUCCESS   The operation succeeded
@retval Other         The operation failed.

**/
EFI_STATUS
EFIAPI
GetUsbFnConnStatus(
  IN  QCOM_USB_CONFIG_CORE_TYPE    CoreType,
  OUT BOOLEAN                     *IsConnected
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USB_TYPEC_PORT_MODE PortMode;
  QCOM_USB_CORE_NUM CoreNum;

  FNC_ENTER_MSG();

  if (NULL == IsConnected || CoreType != USB_CONFIG_SSUSB1) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters: CoreType %d, IsUFPMOD %p", CoreType, IsConnected);
  }

  Status = GetCoreNum(CoreType, &CoreNum);
  ERR_CHK(" Cannot locate CoreNum for CoreType %d: %r", CoreType, Status);
  if (CoreNum >= USB_CORE_MAX_NUM_SDM_670) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("CoreNum out of range");
  }

  // Check the mode of the port
  Status = UsbPortGetConnectionMode(gUsbConfigDevice[CoreNum].UsbPortHandle, &PortMode);
  ERR_CHK(" Cannot Get Connection Mode for Core %d: %r", CoreNum, Status);

  *IsConnected = (PortMode == USB_TYPEC_PORT_MODE_UFP || PortMode == USB_TYPEC_PORT_MODE_DFP) ? TRUE : FALSE;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

/**
Returns the USB Vbus State for the Core

@param [in]  CoreNum      Assigned CoreNumber
@param [out] VbusStatus   Vbus State

@retval EFI_SUCCESS      The operation succeeded
@retval  EFI_UNSUPPORTED Unsupported API
@retval Other            The operation failed.

**/

EFI_STATUS
EFIAPI
GetUsbVbusState (
  IN  QCOM_USB_CORE_NUM          CoreNum,
  OUT UINT32                    *VbusStatus
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  QCOM_USB_CORE_INTERFACE *UsbCoreIfc;

  FNC_ENTER_MSG ();
   
  // For Clamshell platform Vbus state cannot be queried by PMIC
  if (IS_CLS_PLAT(PlatformInfo.platform)) {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  if (CoreNum > USB_CORE_1)
  {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK ("GetUsbVbusState: Error - Invalid CoreNum passed: %d", CoreNum);
  }
   
  DBG(EFI_D_INFO,"CoreNum %d", CoreNum);
   
  UsbCoreIfc = gUsbConfigDevice[CoreNum].UsbCoreInterface;
   
  USB_ASSERT_RETURN ((NULL != UsbCoreIfc), EFI_DEVICE_ERROR);
   
  // Enable the Vbus for the Core
  USB_ASSERT_RETURN ((NULL != UsbCoreIfc->GetVbusStatus), EFI_DEVICE_ERROR);
  Status = UsbCoreIfc->GetVbusStatus(UsbCoreIfc, VbusStatus);
  ERR_CHK ("ConfigUsb: Error - failed to get the Vbus state for the USB core %d: %r",
      CoreNum, Status);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

/**
Enables the USB Vbus for the core

@param [in]  CoreNum      Assigned CoreNumber


@retval EFI_SUCCESS     The operation succeeded
@retval EFI_UNSUPPORTED Unsupported API
@retval Other           The operation failed.

**/

EFI_STATUS
EFIAPI
UsbEnableVbus (
  IN  QCOM_USB_CORE_NUM          CoreNum
  )
{

  EFI_STATUS              Status = EFI_SUCCESS;
  QCOM_USB_CORE_INTERFACE *UsbCoreIfc;
 
  FNC_ENTER_MSG ();
 
   // For Clamshell platform Vbus state cannot be enabled by PMIC
  if (IS_CLS_PLAT(PlatformInfo.platform)) {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  if (CoreNum > USB_CORE_1)
  {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK ("ConfigUsb: Error - Invalid CoreNum passed: %d", CoreNum);
  }
 
  DBG(EFI_D_INFO,"CoreNum %d", CoreNum);
 
  UsbCoreIfc = gUsbConfigDevice[CoreNum].UsbCoreInterface;
 
  USB_ASSERT_RETURN ((NULL != UsbCoreIfc), EFI_DEVICE_ERROR);
 
 // Enable the Vbus for the Core
 USB_ASSERT_RETURN ((NULL != UsbCoreIfc->EnableVbus), EFI_DEVICE_ERROR);
 Status = UsbCoreIfc->EnableVbus(UsbCoreIfc);
 ERR_CHK ("ConfigUsb: Error - failed to enable the Vbus for the USB core %d: %r",
     CoreNum, Status);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}
