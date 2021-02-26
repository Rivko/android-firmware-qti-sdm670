/**
 * @file UsbfnDwc3Ch9.c
 *
 * @brief Chapter 9 functionality
 *
 * This file orchestrates attach/detach proceessing and various parts of the
 * enumeration process.
 *
 * @copyright Copyright (C) 2013-2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
  EDIT HISTORY


  when       who     what, where, why
  --------   ---     -----------------------------------------------------------
  09/04/17   gk      UEFI USB compliance test changes  
  02/01/17   pm      Port from Boot.xf.1.4
  01/18/17   pm      Enhance debug message and conditionally fix the STALL issue
  01/17/17	 pm	     Fix ch9 test failure
  06/20/16   ck      Add support to put the PHY in and out of the Non-Driving mode
  06/16/16   ck      Enable SuperSpeed
  02/23/16   ck      Use UsbConfig for cable detection.
  01/22/16   ck      Port the fix for SetConfig usage.  
  10/26/15   ck      Assume cable connection for pre-si platforms
  11/20/14   ml      Migrate to official SuperSpeed support
  10/07/14   wufan   Add support of HS Test Mode for compliance testing 
  09/12/14   ck      Allow application to be restarted
  06/09/14   ck      Set VBUS override before sending the client detach MSG
  06/04/14   amitg   MSM 8994 BU Updates
  04/25/14   amitg   Charger and Cable Detection Updates for MSM 8994 (Elessar)
  03/14/14   amitg   Updates for 8994
  06/10/13   mliss   Initial rework for Synopsys Usbfn driver
==============================================================================*/

/*
 * Include Files
 */
#include "UsbfnDwc3Ch9.h"
#include "UsbfnDwc3Impl.h"
#include "UsbfnDwc3Util.h"
#include "DwcHwio.h"

// The max number of trials to get a charger type
#define MAX_CHARGER_TYPE_READ_ATTEMPTS	100


/**
 * Set maximum power to be drawn from USB host.
 *
 * Called every time when power allowed to be drawn changes.
 * Call charger driver with updated value
 *
 * @param Usbfn                 Usbfn Device
 * @param MaxCurrent            Maximum current to draw from Vbus, ma
 */
STATIC VOID
UsbfnDwcCoreSetMaxCurrent (
  IN USBFN_DEV                  *Usbfn,
  IN INTN                       MaxCurrent
  )
{
   Usbfn->MaxCurrent = MaxCurrent;
   DEBUG(( EFI_D_INFO, "UsbfnDwcCoreSetMaxCurrent: max current = %d\r\n", MaxCurrent));
}


/**
 * @brief Set interface and alternate setting
 *
 * Locates the interface with the given values and configures the hardware
 * endpoints with settings from descriptors.
 *
 * @param [in] Usbfn            Usbfn Device
 * @param [in] IfcNum           Interface Number
 * @param [in] Alt              Alternate setting
 *
 * @retval EFI_SUCCESS    Interface found and endpoints configured
 * @retval EFI_NOT_FOUND  Interface with given values not found
 */
STATIC
EFI_STATUS
UsbfnDwcCoreSetInterface (
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      IfcNum,
  IN UINT8                      Alt
  )
{
  EFI_STATUS  status = EFI_NOT_FOUND;
  UINTN       ifcCount, epCount, i, j;

  FNC_ENTER_MSG();

  if (UsbBusSpeedSuper == Usbfn->Speed) {
    EFI_USB_SUPERSPEED_INTERFACE_INFO *ifcInfo;
    ifcCount = Usbfn->SSDeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->NumInterfaces;

    for (i = 0; i < ifcCount; i++) {
      ifcInfo = Usbfn->SSDeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[i];

      // check for matching interface
      if (ifcInfo->InterfaceDescriptor->InterfaceNumber == IfcNum &&
          ifcInfo->InterfaceDescriptor->AlternateSetting == Alt) {
        epCount = ifcInfo->InterfaceDescriptor->NumEndpoints;

        DBG(EFI_D_INFO, "Found interface %d alternate %d", IfcNum, Alt);

        // configure endpoints in found interface
        for (j = 0; j < epCount; j++) {
          EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR *epDesc = ifcInfo->EndpointDescriptorTable[j];
          UINT8 ep = USB_EP_FROM_ADDR(epDesc->EndpointDescriptor->EndpointAddress);
          UINT8 dir = USB_EPDIR_FROM_ADDR(epDesc->EndpointDescriptor->EndpointAddress);
          UINT8 epType = USB_EPTYPE_FROM_ATTR(epDesc->EndpointDescriptor->Attributes);

          UsbfnDwcConfigEndpoints(
              Usbfn,
              ep,
              dir,
              epType,
              UsbEpMaxPacketSize[epType][Usbfn->Speed],
              FALSE,
              epDesc->EndpointCompanionDescriptor->MaxBurst,
              epDesc->EndpointDescriptor->Interval
              );
        }

        status = EFI_SUCCESS;
        goto ON_EXIT;
      }
    }
  } else { // speed < super
    EFI_USB_INTERFACE_INFO *ifcInfo;
    ifcCount = Usbfn->DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->NumInterfaces;

    for (i = 0; i < ifcCount; i++) {
      ifcInfo = Usbfn->DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[i];

      // check for matching interface
      if (ifcInfo->InterfaceDescriptor->InterfaceNumber == IfcNum &&
          ifcInfo->InterfaceDescriptor->AlternateSetting == Alt) {
        epCount = ifcInfo->InterfaceDescriptor->NumEndpoints;

        DBG(EFI_D_INFO, "Found interface %d alternate %d", IfcNum, Alt);

        // configure endpoints in found interface
        for (j = 0; j < epCount; j++) {
          EFI_USB_ENDPOINT_DESCRIPTOR *epDesc = ifcInfo->EndpointDescriptorTable[j];
          UINT8 ep = USB_EP_FROM_ADDR(epDesc->EndpointAddress);
          UINT8 dir = USB_EPDIR_FROM_ADDR(epDesc->EndpointAddress);
          UINT8 epType = USB_EPTYPE_FROM_ATTR(epDesc->Attributes);

          UsbfnDwcConfigEndpoints(
              Usbfn,
              ep,
              dir,
              epType,
              UsbEpMaxPacketSize[epType][Usbfn->Speed],
              FALSE,
              0,
              epDesc->Interval
              );
        }

        status = EFI_SUCCESS;
        goto ON_EXIT;
      }
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return status;
}


/**
 * @brief Set new configuration value
 *
 * Set a new non-zero configuration value from the deconfigured state, or
 * deconfigure the device by setting NewConfig to 0.
 *
 * @param [in] Usbfn            Usbfn Device
 * @param [in] NewConfig        Configuration value to set
 *
 * @retval EFI_SUCCESS    Set new configuration
 * @retval others         Failed to set new configuration
 */
STATIC
EFI_STATUS
UsbfnDwcCoreSetConfig (
  IN USBFN_DEV                  *Usbfn,
  IN UINT8                      NewConfig
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_USB_CONFIG_INFO *ConfigInfo;
  UINTN               i;

  FNC_ENTER_MSG();

  if (NewConfig != Usbfn->ConfigValue) {

    if (NewConfig != 0) { // configured

      // only need descriptor components of info structs which share the same location
      if (UsbBusSpeedSuper == Usbfn->Speed) {
        ConfigInfo = (EFI_USB_CONFIG_INFO *)Usbfn->SSDeviceInfo->ConfigInfoTable[0];
      } else {
        ConfigInfo = Usbfn->DeviceInfo->ConfigInfoTable[0];
      }

      // only support 1 config, make sure value matches
      if (NewConfig != ConfigInfo->ConfigDescriptor->ConfigurationValue) {
        Status = EFI_NOT_FOUND;
        DBG(EFI_D_ERROR, "Failed to find requested configuration %d", NewConfig);
		 goto ON_EXIT;
      }

      Status = UsbfnDwcSetConfigIfc(Usbfn);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "UsbfnDwcSetConfigIfc failed %r", Status);
        goto ON_EXIT;
      }

      // interface alternate settings not supported (all use 0)
      for (i = 0; i < ConfigInfo->ConfigDescriptor->NumInterfaces; i++) {
        UINT8 ifcNum = ConfigInfo->InterfaceInfoTable[i]->InterfaceDescriptor->InterfaceNumber;
        Status = UsbfnDwcCoreSetInterface(Usbfn, ifcNum, 0);
        if (EFI_ERROR(Status)) {
          DBG(EFI_D_ERROR, "Failed to set interface %d", ifcNum);
          goto ON_EXIT;
        }
      }

      // apply changes
      Usbfn->ConfigValue = NewConfig;
      Usbfn->IsSuspended = FALSE;

      //
      // bMaxPower is in units of 2ma when in HS mode and
      // 8mA when in SS mode - see USB 3.0 spec #9.6.3
      //
      if (Usbfn->Speed == UsbBusSpeedSuper) {
        UsbfnDwcCoreSetMaxCurrent(Usbfn, Usbfn->SSDeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->MaxPower * 8);
      } else {
        UsbfnDwcCoreSetMaxCurrent(Usbfn, Usbfn->DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->MaxPower * 2);
      }
    } else { // deconfigured
      Usbfn->ConfigValue = 0;
      UsbfnDwcCoreSetMaxCurrent(Usbfn, 0);
      UsbfnDwcCancelActiveTransfers(Usbfn);
      UsbfnDwcClearAllStallEndpoints(Usbfn);
      DwcDeconfigureNonCtrlEPs(&Usbfn->DwcDevice);
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Handle a setup packet directed at the device.
 *
 * @param  [in]  Usbfn           Usbfn device instance pointer
 * @param  [in]  Req             A received setup packet
 */
STATIC
VOID
UsbfnDwcCoreHandleSetupDev (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USB_DEVICE_REQUEST     *Req
  )
{
  EFI_STATUS  status;
  UINT32      TestMode = USB_TEST_DISABLE;

  FNC_ENTER_MSG();

  switch (Req->Request) {

    case USB_REQ_SET_ADDRESS:
      if (0 == Usbfn->ConfigValue) {
        Usbfn->Address = (UINT8)(Req->Value & 0xff);
        DBG(EFI_D_INFO, "New Address is %d", Usbfn->Address);
        UsbfnDwcHandleSetAddress(Usbfn);
      } else {
        DBG(EFI_D_ERROR, "SetAddress received in Configured state");
      }
      break;

    case USB_REQ_SET_CONFIG:
      status = UsbfnDwcCoreSetConfig(Usbfn, (UINT8)(Req->Value & 0xff));
      if (EFI_ERROR(status)) {
        DBG(EFI_D_ERROR, "Failed to set configuration %d", Req->Value & 0xff);
      } else {
        DBG(EFI_D_INFO, "Configuration %d set", Usbfn->ConfigValue);
      }
      break;

    case USB_REQ_SET_SEL:
      DBG(EFI_D_INFO, "USB_REQ_SET_SEL");

      // set flag to store the values returned in data stage
      Usbfn->StoreSELData = TRUE;
      break;

    case USB_REQ_SET_FEATURE:
      DBG(EFI_D_INFO, "USB_REQ_SET_FEATURE");

      if (Req->Value == USB_FEATURE_U1_ENABLE) {
        DBG (EFI_D_INFO, "U1_ENABLE");
        UsbfnDwcHandleU1Enable(Usbfn);

      } else if (Req->Value == USB_FEATURE_U2_ENABLE) {
        DBG(EFI_D_INFO, "U2_ENABLE");
        UsbfnDwcHandleU2Enable(Usbfn);
        
      } else if (Req->Value == USB_FEATURE_HS_TEST_MODE) {
        DBG(EFI_D_INFO, "HS_TEST_MODE");
        TestMode = Req->Index >> 8;
        if (USB_TEST_MODE_MAX > TestMode) {
           Usbfn->HsTestMode = (USB_TEST_MODE)TestMode ;            
        } else {
           DBG(EFI_D_WARN, "SET_FEATURE test mode out of range %d", TestMode);
        }

      } else {
        DBG(EFI_D_WARN, "Unhandled SET_FEATURE %d", Req->Value);
      }
      break;
  }
  UsbfnSetupDevCb(Usbfn, Req);

  FNC_LEAVE_MSG();
}


/**
 * @brief Handle a setup packet directed at an interface.
 *
 * @param  [in]  Usbfn           Usbfn device instance pointer
 * @param  [in]  Req             A received setup packet
 */
STATIC VOID
UsbfnDwcCoreHandleSetupIfc (
  IN USBFN_DEV                  *Usbfn,
  IN EFI_USB_DEVICE_REQUEST     *Req
  )
{
  FNC_ENTER_MSG();

  switch (Req->Request) {
    case USB_REQ_SET_INTERFACE:
      if (0 != Usbfn->ConfigValue) {
        UsbfnDwcCoreSetInterface(
	      Usbfn,
	      (UINT8)(Req->Index & 0xff),
	      (UINT8)(Req->Value & 0xff)
	      );
      }
    break;
  }
  UsbfnSetupIfcCb(Usbfn, Req);

  FNC_LEAVE_MSG();
}


/*******************************************************************************
 * Externalized Function Definitions
 ******************************************************************************/

/*
 * See header for documentation.
 */
VOID
UsbfnDwcCoreInit (
  IN USBFN_DEV                  *Usbfn
  )
{
  /* device is in "default" state - not configured, no address assigned */
  Usbfn->Enabled = TRUE;
  Usbfn->Speed = UsbBusSpeedUnknown;
  Usbfn->Address = 0;
  Usbfn->ConfigValue = 0;
  UsbfnDwcCoreSetMaxCurrent(Usbfn, 0);
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcCoreShutdown (
  IN USBFN_DEV                  *Usbfn
  )
{
  UINT8         Ep, Dir;
  LIST_ENTRY   *ListEntry   = NULL;
  FNC_ENTER_MSG();

  DBG(EFI_D_INFO, "Usbfn Shutting Down...");

  Usbfn->Enabled = FALSE;

  // perform software disconnect if the cable is still attached
  if (Usbfn->IsAttached) {

    //
    // there won't be a disconnect event in the case of a device initiated
    // disconnect, set to true to prevent spinning forever
    //
    Usbfn->DisconnectEvtPosted = TRUE;
    UsbfnDwcCoreCableStatus(Usbfn, FALSE);
  }
  
  Usbfn->ConfigValue = 0;
  Usbfn->IsAttached  = 0;
  Usbfn->Speed       = UsbBusSpeedUnknown;
  Usbfn->Address     = 0;

  // remove every message from UsbfnMessages
  // and put it back to UsbfnMessagePool 
  while (!IsListEmpty(&Usbfn->UsbfnMessages)) {
    ListEntry = GetFirstNode(&Usbfn->UsbfnMessages);
    RemoveEntryList(ListEntry);
    InsertTailList(&Usbfn->UsbfnMessagePool, ListEntry);
  }

  // Put every URB in the ActiveTransfers and PendingXfer
  // back to UrbPool
  if (Usbfn->SetupPkt) {
    for (Ep = 0; Ep < USBFN_ENDPOINT_NUMBER_MAX; Ep++) {
      for (Dir = 0; Dir < USBFN_ENDPOINT_DIR_MAX; Dir++) {        
        while (!IsListEmpty(&Usbfn->ActiveTransfers[Ep][Dir])) {
          ListEntry = GetFirstNode(&Usbfn->ActiveTransfers[Ep][Dir]);
          RemoveEntryList(ListEntry);
          InsertTailList(&Usbfn->UrbPool, ListEntry);          
        }
        while (!IsListEmpty(&Usbfn->PendingXfer[Ep][Dir])) {
          ListEntry = GetFirstNode(&Usbfn->PendingXfer[Ep][Dir]);
          RemoveEntryList(ListEntry);
          InsertTailList(&Usbfn->UrbPool, ListEntry);
        }
      }
    }
  }

  FNC_LEAVE_MSG();
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcCorePoll (
  IN USBFN_DEV                  *Usbfn
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  BOOLEAN IsConnectionEstablished = TRUE;

  if(!Usbfn) {
    DBG(EFI_D_ERROR, "Invalid parameter passed to the function");
    goto ON_EXIT;
  }

  if(!Usbfn->Enabled) {
    DBG(EFI_D_ERROR, "Usbfn is not enabled");
    goto ON_EXIT;
  }

  //For primary core in non-pre-si platforms, detect cable status through PMIC
  //interface
  if ((Usbfn->CoreType == USB_CONFIG_SSUSB1) && 
      !(Usbfn->PlatType.platform == EFI_PLATFORMINFO_TYPE_UNKNOWN ||
        Usbfn->PlatType.platform == EFI_PLATFORMINFO_TYPE_RUMI    ||
        Usbfn->PlatType.platform == EFI_PLATFORMINFO_TYPE_VIRTIO))
  {
    // Get cable state
    Status = Usbfn->UsbConfigProtocol->GetUsbFnConnStatus(Usbfn->UsbConfigProtocol,
      Usbfn->CoreType, &IsConnectionEstablished);
    if(EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR,"Usbfn->UsbConfigProtocol unbale to get GetSSUsbFnConnStatus with error: %r\r\n", Status);
      goto ON_EXIT;
    }
  }
  else
  {
    IsConnectionEstablished = TRUE;
  }
  
  if(Usbfn->IsAttached) {
    // Cable/Host partner has been removed
    if(!IsConnectionEstablished) {      
      DBG(EFI_D_INFO, "Cable removed, IsConnectionEstablished = %d \r \n", IsConnectionEstablished);
      UsbfnDwcCoreCableStatus(Usbfn, FALSE);
    } 
    else if(DWC_HWIO_INF(Usbfn->DwcUsb3RegBase, DCTL, RUN_STOP)) {
      // poll for events only if controller is running
      UsbfnDwcEventHandler(Usbfn);
    }
  } 
  else {
    // Cable/Host partner has been attached
    if(IsConnectionEstablished) {      
      DBG(EFI_D_INFO,"Cable connected, IsConnectionEstablished = %d \r \n", IsConnectionEstablished);
      UsbfnDwcCoreCableStatus(Usbfn, TRUE);
    }
  }

ON_EXIT:
  return;
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcCoreCableStatus (
  IN USBFN_DEV                  *Usbfn,
  IN BOOLEAN                    IsAttached
  )
{
  EFI_STATUS  Status    = EFI_SUCCESS;
  UINTN       Cnt       = 0;
  UINT8       UefiVar;
  UINTN       UefiVarSize;
  BOOLEAN     IsCompliance    = FALSE;
  

  FNC_ENTER_MSG();
  Usbfn->IsAttached = IsAttached;

  if(TRUE == IsAttached) {
  	

	// Check if the UEFI variable is set to enable device mode compliance testing
    // Override charger port type to SDP for compliance testing
    UefiVarSize = sizeof(UefiVar);
    Status = gRT->GetVariable(L"UsbfnForceComplianceMode", &gQcomTokenSpaceGuid, NULL, &UefiVarSize, &UefiVar);
    if (!EFI_ERROR(Status) && (UefiVar != 0)) {
      DBG(EFI_D_WARN, "USB Device mode compliance override set");
      IsCompliance = TRUE;
    }
  
    // 1. Run charger detection upon cable attach.
    //    RS bit will also be set if the port type is either CDP or SDP
    DBG(EFI_D_INFO, "cable attached");

    //For primary core in non-pre-si platforms, detect cable status through PMIC 
    //interface
    if ((Usbfn->CoreType == USB_CONFIG_SSUSB1) &&
        !(Usbfn->PlatType.platform == EFI_PLATFORMINFO_TYPE_UNKNOWN ||
          Usbfn->PlatType.platform == EFI_PLATFORMINFO_TYPE_RUMI    ||
          Usbfn->PlatType.platform == EFI_PLATFORMINFO_TYPE_VIRTIO  ||
          IsCompliance))
    {
      // We wait to make sure PMIC APSD has completed
      do {
        // Charger detection for synopsys core through PMIC
        Status = UsbfnDwc3GetChargerPortType(0, &Usbfn->ChgPortType);
        if(EFI_ERROR(Status)) {
          DBG(EFI_D_ERROR,"PmicSmbchgProtocol->ChargerPort() failed with error: %r. Assume SDP\r", Status);
          Usbfn->ChgPortType = DCI_DWC_CHG_PORT_SDP;
          break;
        }
        
        // if the port type is still invalid, give PMIC HW some more time
        if (DCI_DWC_CHG_PORT_UNKNOWN == Usbfn->ChgPortType) {
          gBS->Stall(10 * 1000);
        }
        Cnt++;
      } while((DCI_DWC_CHG_PORT_UNKNOWN == Usbfn->ChgPortType) && (Cnt < MAX_CHARGER_TYPE_READ_ATTEMPTS));
    }
    else
    {    
      Usbfn->ChgPortType = DCI_DWC_CHG_PORT_SDP;
    }

    // handle different port types
    switch (Usbfn->ChgPortType) {
      case DCI_DWC_CHG_PORT_SDP:
      case DCI_DWC_CHG_PORT_CDP:

        // Connect the device to the USB bus. - Vbus indication
        dci_connect(Usbfn->Usb30Qscratch);

        // Initialize the controller
        Status = UsbfnDwcCoreInitImpl(Usbfn);
        if (EFI_ERROR(Status)) {
          DBG(EFI_D_ERROR, "UsbfnDwcCoreInitImpl failed with error: %r\r", Status);
          goto ON_EXIT;
        }

        // Start the controller if we're connected to a host
        DBG(EFI_D_INFO, "starting controller");
        DWC_HWIO_OUTF(Usbfn->DwcUsb3RegBase, DCTL, RUN_STOP, 0x1U);
        break;
      case DCI_DWC_CHG_PORT_DCP:
        UsbfnDwcCoreSetMaxCurrent(Usbfn, 1500);
        break;
      case DCI_DWC_CHG_PORT_INVALID:
        UsbfnDwcCoreSetMaxCurrent(Usbfn, 500);
        break;
      default:
        DBG(EFI_D_ERROR, "Unknown Port type %d\r\n", Usbfn->ChgPortType);
    }
  } else {
    DBG(EFI_D_INFO, "cable detached");

    dci_disconnect (Usbfn->Usb30Qscratch);

    // shut down the controller on disconnect
    Status = UsbfnDwcHandleDisconnect(Usbfn);
    if (EFI_ERROR(Status)) {
      //
      // When this happens, the controller is actually stopped and the charger
      // detection never fails. Just print a warning here and do nothing.
      //
      DBG(EFI_D_ERROR, "Failed to process cable disconnect");
    }    

    // upon cable detach, set max current to zero (R/S bit will already be clear)    
    Usbfn->ConfigValue = 0;
    Usbfn->Address = 0;
    Usbfn->IsSuspended = FALSE;
    UsbfnDwcCoreSetMaxCurrent(Usbfn, 0);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcCorePortStatusChanged (
  IN USBFN_DEV                  *Usbfn,
  IN BOOLEAN                    IsSuspended
  )
{
  // Act only when suspend/resume status changes
  if (Usbfn->IsSuspended == IsSuspended) {
    return;
  }

  FNC_ENTER_MSG();

  if (IsSuspended) { // suspend

    // draw no current in suspended state
    UsbfnDwcCoreSetMaxCurrent(Usbfn, 0);

    // notify suspend
    UsbfnBusEventCb(Usbfn, EfiUsbMsgBusEventSuspend);

  } else { // resume

    //
    // suspend can happen when the device is not configured
    // if we resume back a configured state, set max current based on link speed
    // otherwise, set it to zero
    //
    if (0 != Usbfn->ConfigValue) {
      //
      // bMaxPower is in units of 2ma when in HS mode and
      // 8mA when in SS mode - see USB 3.0 spec #9.6.3
      //
      if (Usbfn->Speed == UsbBusSpeedSuper) {
        UsbfnDwcCoreSetMaxCurrent(Usbfn, Usbfn->SSDeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->MaxPower * 8);
      } else {
        UsbfnDwcCoreSetMaxCurrent(Usbfn, Usbfn->DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->MaxPower * 2);
      }
    } else {
      UsbfnDwcCoreSetMaxCurrent(Usbfn, 0);
    }

    UsbfnBusEventCb(Usbfn, EfiUsbMsgBusEventResume);
  }

  Usbfn->IsSuspended = IsSuspended;
  FNC_LEAVE_MSG();
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcCoreReset (
  IN USBFN_DEV                  *Usbfn
  )
{
  FNC_ENTER_MSG();

  Usbfn->ConfigValue = 0;
  Usbfn->Address     = 0;

  /*
   * Reset EP0 software state
   * Hardware has reset EP0 transfers, reflect this in software.
   */
  UsbfnBusEventCb(Usbfn, EfiUsbMsgBusEventReset);

  FNC_LEAVE_MSG();
}


/*
 * See header for documentation.
 */
VOID
UsbfnDwcCoreProcessSetupPkt (
  IN  USBFN_DEV                 *Usbfn,
  IN  VOID                      *Buffer,
  OUT BOOLEAN                   *HasDataStage,
  OUT DWC_ENDPOINT_DIR          *DataStageDir
  )
{
  EFI_USB_DEVICE_REQUEST    *Req;

  FNC_ENTER_MSG();

  ASSERT(Usbfn && Buffer && HasDataStage && DataStageDir);

  Req = (EFI_USB_DEVICE_REQUEST*)Buffer;

  DBG(EFI_D_INFO,"Req->RequestType 0x%02x",Req->RequestType);
  DBG(EFI_D_INFO,"Req->Request     0x%02x",Req->Request);
  DBG(EFI_D_INFO,"Req->Value       0x%04x",Req->Value);
  DBG(EFI_D_INFO,"Req->Index       0x%04x",Req->Index);
  DBG(EFI_D_INFO,"Req->Length      0x%04x",Req->Length);

  *HasDataStage = (Req->Length > 0) ? TRUE : FALSE;
  *DataStageDir = (Req->RequestType & USB_SETUP_RT_DIR_MASK) ? DWC_EP_DIR_TX : DWC_EP_DIR_RX;

  // Handle only the standard request. For other type of request,
  // just pass the setup pkt to the upper layer.

  if (((Req->RequestType >> USB_SETUP_RT_TYPE_POS) & USB_SETUP_RT_TYPE_SEED) != USB_SETUP_RT_TYPE_STD) {
    DEBUG((EFI_D_INFO, "Non-Standard Request Received. Pass to Client to Handle it. \n"));
    UsbfnSetupDevCb(Usbfn, Req);

  } else {

    // Determine recipient
    switch ((Req->RequestType >> USB_SETUP_RT_RCP_POS) & USB_SETUP_RT_RCP_SEED) {

      case USB_SETUP_RT_RCP_DEV:
        UsbfnDwcCoreHandleSetupDev(Usbfn, Req);
        break;

      case USB_SETUP_RT_RCP_IFC:
        UsbfnDwcCoreHandleSetupIfc(Usbfn, Req);
        break;

      default:
        UsbfnSetupDevCb(Usbfn, Req);
        break;
    }
  }

  FNC_LEAVE_MSG();
}


