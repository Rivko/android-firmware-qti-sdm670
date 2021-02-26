/** @file UsbfnChgDxe.c

  This file contains the entry point of the dxe driver.

  Copyright (c) 2016-2017 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 02/01/17	pm		 Port from Boot.xf.1.4, correct debug message level
 05/23/16   ck       Init version
 =============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/QcomBaseLib.h>
#include <Protocol/EFIUsbfnIo.h>
#include "UsbfnChgDesc.h"

/*=========================================================================
      Macro
==========================================================================*/
/* USB endpoint index for control transfers (both IN/OUT used) */
#define USB_CTRL_EP_TX_IDX          0x80
#define USB_CTRL_EP_RX_IDX          0x00
#define USB_CTRL_XFER_MAX_BUF_SIZ   512

// Get Descript Type
#define USB_VALUE_TO_DESC_TYPE(value) ((value) >> 8)

#define USB_SETUP_RT_TYPE_SHFT        5
#define USB_SETUP_RT_TYPE_BMSK        (0x3 << USB_SETUP_RT_TYPE_SHFT)
#define USB_SETUP_RT_TYPE(bmRT)       (((bmRT) & USB_SETUP_RT_TYPE_BMSK) >> USB_SETUP_RT_TYPE_SHFT)
#define USB_SETUP_RT_TYPE_STD         0
#define USB_SETUP_RT_TYPE_CLS         1
#define USB_SETUP_RT_TYPE_VND         2

#define USBFN_EVT_POLLING_INTERVAL       100000
#define USBFN_EVT_MAX_WAIT_TIME_IN_MS    5000

/*=========================================================================
       Global Variables
==========================================================================*/
extern EFI_GUID gEfiEventExitBootServicesGuid;
extern EFI_GUID gEfiUsbfnChgSrvProtocolGuid;
extern EFI_GUID gEfiUsbDeviceProtocolGuid;
extern EFI_GUID gEfiUsbDeviceOemDataChargerTypeGuid;
extern EFI_GUID gEfiUsbDeviceOemDataSetupPacketGuid;
extern EFI_GUID gEfiUsbDeviceOemDataMaxPktSizeGuid;

/*=========================================================================
       Static Variables
==========================================================================*/
static USB_DEVICE_DESCRIPTOR_SET gDescSet;

STATIC
CONST
struct {
  EFI_USB_BOS_DESCRIPTOR                BosDescriptor;
  EFI_USB_USB_20_EXTENSION_DESCRIPTOR   Usb2ExtDescriptor;
  EFI_USB_SUPERSPEED_USB_DESCRIPTOR     SsUsbDescriptor;
} BinaryObjectStore = {
  // BOS Descriptor
  {
    sizeof(EFI_USB_BOS_DESCRIPTOR),               // Descriptor Size
    USB_DESC_TYPE_BOS,                            // Descriptor Type
    sizeof(BinaryObjectStore),                    // Total Length
    2                                             // Number of device capabilities
  },
  // USB2 Extension Desc
  {
    sizeof(EFI_USB_USB_20_EXTENSION_DESCRIPTOR),  // Descriptor Size
    USB_DESC_TYPE_DEVICE_CAPABILITY,              // Device Capability Type descriptor
    USB_DEV_CAP_TYPE_USB_20_EXTENSION,            // USB 2.0 Extension Capability Type
    0x6                                           // Supported device level features
  },
  // Super Speed Device Capability Desc
  {
    sizeof(EFI_USB_SUPERSPEED_USB_DESCRIPTOR),    // Descriptor Size
    USB_DESC_TYPE_DEVICE_CAPABILITY,              // Device Capability Type descriptor
    USB_DEV_CAP_TYPE_SUPERSPEED_USB,              // SuperSpeed Device Capability Type
    0x00,                                         // Supported device level features
    0x0E,                                         // Speeds Supported by the device: SS, HS and FS
    0x01,                                         // Functionality support
    0x07,                                         // U1 Device Exit Latency
    0x65                                          // U2 Device Exit Latency
  }
};

/*=========================================================================
       Forward Declaration
==========================================================================*/

/*=========================================================================
Functions
==========================================================================*/

/**
* @brief Print GUID in text
*
* @param[in]  Guid                  GUID to be printed
* @param[in]  DebugLevel            UEFI Debug Level 
*/
VOID
UsbfnChgSrvPrintGuid(
  IN EFI_GUID  Guid,
  IN UINT32 DebugLevel
)
{
  UINTN i = 0;
  DEBUG((DebugLevel, "{0x%08x,0x%04x,0x%04x,{", Guid.Data1, Guid.Data2, Guid.Data3));


  for (i = 0; i < 8; i++)
  {
    DEBUG((DebugLevel, "0x%02x", Guid.Data4[i]));
    if (i != 7)
    {
      DEBUG((DebugLevel, ","));
    }
  }
  DEBUG((DebugLevel, "}}\n"));
}


/**
* @brief Driver exit function that is called on gBS->ExitBootServices()
*
* Disconnects from host if a connection is still active and frees memory.
*
* @param[in]  Event                   Pointer to this event
* @param[in]  Context                 Event handler private data
*/
VOID
EFIAPI
UsbfnChgSrvExitBootService(
  IN EFI_EVENT  Event,
  IN VOID       *Context
)
{
  USBFN_CHG_DEV *UsbfnChgDev = (USBFN_CHG_DEV *)Context;

  if (!Context) {
    DBG(EFI_D_ERROR, "Context cannot be NULL");
    goto ON_EXIT;
  }

  // Close ExitBootServiceEvent
  if (UsbfnChgDev->ExitBootServiceEvent) {
    gBS->CloseEvent(UsbfnChgDev->ExitBootServiceEvent);
    UsbfnChgDev->ExitBootServiceEvent = NULL;
  }
  
  // Close EventHandlerTimer
  if (UsbfnChgDev->EventHandlerTimer) {
    gBS->CloseEvent(UsbfnChgDev->EventHandlerTimer);
    UsbfnChgDev->EventHandlerTimer = NULL;
  }

  // Free the charger device instance
  FreePool(UsbfnChgDev);
 
ON_EXIT:
  return;
}


/**
* @brief Handle the charger detection result
*
* The function converts the EFI_USBFN_PORT_TYPE to EFI_USBFN_CHARGER_TYPE and
* store the test result in the charger device instance.
*
* @param[in]  UsbfnChgDev             An instance to UsbfnChgDev
* @param[in]  PortType                Charger port type in EFI_USBFN_PORT_TYPE
*/
EFI_STATUS
HandleOemDataCharger(
  IN USBFN_CHG_DEV       *UsbfnChgDev,
  IN EFI_USBFN_PORT_TYPE  PortType
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!UsbfnChgDev) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  switch (PortType) {
  case EfiUsbStandardDownstreamPort:
    UsbfnChgDev->ChargerType = USB_CHARGER_SDP;
    break;
  case EfiUsbDedicatedChargingPort:
    UsbfnChgDev->ChargerType = USB_CHARGER_DCP;
    break;
  case EfiUsbChargingDownstreamPort:
    UsbfnChgDev->ChargerType = USB_CHARGER_CDP;
    break;
    // If it is not SDP, DCP or CDP assume IDCP
  case EfiUsbInvalidDedicatedChargingPort:
    UsbfnChgDev->ChargerType = USB_CHARGER_IDCP;
  case EfiUsbUnknownPort:
    UsbfnChgDev->ChargerType = USB_CHARGER_IDCP;
    break;
  default:
    UsbfnChgDev->ChargerType = USB_CHARGER_IDCP;
  }
  
  UsbfnChgDev->ChgDevState = CHG_DEV_ST_CHARGER;
ON_EXIT:
  return Status;
}


/**
* Handles setup packets containing class requests.
*
* @param[in]  UsbfnChgDev  An instance of USBFN_CHG_DEV
* @param[in]  Req          Setup packet data
*
* @retval EFI_SUCCESS           The control request is handled
* @retval EFI_UNSUPPORTED       The control request is not supported.
* @retval Others                Error processing request
*/
STATIC
EFI_STATUS
ProcessCtrlRequest(
  IN  USBFN_CHG_DEV           *UsbfnChgDev,
  IN  EFI_USB_DEVICE_REQUEST  *Req
)
{
  EFI_STATUS                Status            = EFI_SUCCESS;
  EFI_USB_DEVICE_PROTOCOL  *UsbDeviceProtocol = NULL;
  UINTN                     XferLen           = 0;

  if (!UsbfnChgDev || !Req){
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  
  DBG(EFI_D_INFO,
    "SetupPacket:\n"
    "  bmRequestType: 0x%02x\n"
    "  bRequest:      0x%02x\n"
    "  wValue:      0x%04x\n"
    "  wIndex:      0x%04x\n"
    "  wLength:     0x%04x",
    Req->RequestType,
    Req->Request,
    Req->Value,
    Req->Index,
    Req->Length);

  UsbDeviceProtocol = UsbfnChgDev->UsbDeviceProtocol;

  // Switch on setup request
  switch (Req->Request) {

  case EFI_USB_SET_IDLE_REQUEST:    
    XferLen = 0;
    Status  = UsbDeviceProtocol->Send(USB_CTRL_EP_TX_IDX, XferLen, UsbfnChgDev->CtrlXferBuffer);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Unable to send status for SET_IDEL, %r", Status);
    }
    break;

  case USB_DEV_GET_DESCRIPTOR:    
    if (USB_VALUE_TO_DESC_TYPE(Req->Value) == USB_DESC_TYPE_REPORT) {
      XferLen = Req->Length < sizeof (ReportDescriptor) ? Req->Length : sizeof(ReportDescriptor);
      Status = UsbDeviceProtocol->Send(USB_CTRL_EP_TX_IDX, XferLen, ReportDescriptor);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Unable to send HID report descriptor %r", Status);
      }
    }
    break;  
  default:        
    Status = EFI_UNSUPPORTED;
  }

ON_EXIT:
  if (EFI_ERROR(Status) && Req) {
    DBG(EFI_D_ERROR,
      "SetupPacket:\n"
      "  bmRequestType: 0x%02x\n"
      "  bRequest:      0x%02x\n"
      "  wValue:      0x%04x\n"
      "  wIndex:      0x%04x\n"
      "  wLength:     0x%04x",
      Req->RequestType,
      Req->Request,
      Req->Value,
      Req->Index,
      Req->Length);
  }
  return Status;
}


/**
* Handle transfer complete event
*
* @param[in]  UsbfnChgDev       An instance of USBFN_CHG_DEV
* @param[in]  EventData         Event data associated with the transfer complete event
*
* @retval EFI_SUCCESS           The operation succeeded. 
* @retval Others                The operation failed.
*/
EFI_STATUS
HandleTransferNotification(
  IN USBFN_CHG_DEV         *UsbfnChgDev,
  IN USB_DEVICE_EVENT_DATA *EventData
)
{
  EFI_STATUS               Status            = EFI_SUCCESS;
  EFI_USB_DEVICE_PROTOCOL *UsbDeviceProtocol = NULL;
  UINTN                    XferSize          = 0;

  if (!UsbfnChgDev || !EventData) {
    DBG(EFI_D_ERROR, "Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;    
    goto ON_EXIT;
  }

  UsbDeviceProtocol = UsbfnChgDev->UsbDeviceProtocol;

  // if xfer is not successful, just return
  if (EventData->TransferOutcome.Status != UsbDeviceTransferStatusCompleteOK){
    DBG(EFI_D_ERROR, "Xfer on EP 0x%x failed with status %d",
      EventData->TransferOutcome.EndpointIndex,
      EventData->TransferOutcome.Status);
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  } 

  if ((EventData->TransferOutcome.EndpointIndex == USB_CTRL_EP_RX_IDX) &&
    (EventData->TransferOutcome.BytesCompleted == 0)) {    
    DBG(EFI_D_INFO, "Dev Recv Status Pkg from Host");
  }
  else if ((EventData->TransferOutcome.EndpointIndex == USB_CTRL_EP_TX_IDX) &&
    (EventData->TransferOutcome.BytesCompleted == 0)) {    
    DBG(EFI_D_INFO, "Host Recv Status Pkg from Device");
  }
  else {
    if (EventData->TransferOutcome.EndpointIndex == USB_CTRL_EP_RX_IDX) {
      DBG(EFI_D_INFO, "Data Stage finished (H->D), Prepare to send status to host");
      //Data Stage finished (H->D), Prepare to send status to host
      Status = UsbDeviceProtocol->Send(USB_CTRL_EP_TX_IDX, XferSize, UsbfnChgDev->CtrlXferBuffer);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Unable to send status");
        goto ON_EXIT;
      }      
    }
    else {
      //Data Stage finished (D->H), Prepare to recv status from host
      DBG(EFI_D_INFO, "Data Stage finished (D->H), Prepare to recv status from host");
      Status = UsbDeviceProtocol->Send(USB_CTRL_EP_RX_IDX, XferSize, UsbfnChgDev->CtrlXferBuffer);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Unable to receieve status");
        goto ON_EXIT;
      }
    }
  }

ON_EXIT:
  return Status;
}


/**
* Event handler function to process the events from USB Device Protocol
*
* @param[in]  UsbfnChgDev       An instance of USBFN_CHG_DEV
* @param[in]  Event             Event returned by USB Device Protocol
* @param[in]  PayloadSize       The size of the payload associated with Payload
* @param[in]  Payload           The payload associated with the Event
*
* @retval EFI_SUCCESS           The operation succeeded.
* @retval Others                The operation failed.
*/
EFI_STATUS
DeviceEventHandler(
  IN USBFN_CHG_DEV         *UsbfnChgDev,
  IN USB_DEVICE_EVENT       Event,
  IN UINTN                  PayloadSize,
  IN USB_DEVICE_EVENT_DATA *Payload
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  switch (Event)
  {
  case UsbDeviceEventOemEvent:
    // charger type is detected
    if (CompareGuid(&Payload->OemData.DataGuid, &gEfiUsbDeviceOemDataChargerTypeGuid)){
      DBG(EFI_D_INFO, "Chg Det Event");      
      Status = HandleOemDataCharger(UsbfnChgDev, *(EFI_USBFN_PORT_TYPE*)Payload->OemData.Context);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Failed to get charger type: %r", Status);
      }
    }
    else if (CompareGuid(&Payload->OemData.DataGuid, &gEfiUsbDeviceOemDataSetupPacketGuid)){
      DBG(EFI_D_INFO, "Recv Setup Pkt Event");
      Status = ProcessCtrlRequest(UsbfnChgDev, (EFI_USB_DEVICE_REQUEST *)Payload->OemData.Context);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Failed to process cls request: %r", Status);
        Status = UsbfnChgDev->UsbDeviceProtocol->SetEndpointStallState(USB_CTRL_EP_RX_IDX, TRUE);
        if (EFI_ERROR(Status)) {
          DBG(EFI_D_ERROR, "Failed to stall control ep: %r", Status);
        }
      }
    }
    else if (CompareGuid(&Payload->OemData.DataGuid, &gEfiUsbDeviceOemDataMaxPktSizeGuid)){
      UINTN MaxPktSize = (Payload->OemData.Context) ? *((UINT16 *)(Payload->OemData.Context)) : 0;
      DBG(EFI_D_INFO, "Max Pkt Size %d", MaxPktSize);

      if (!Payload->OemData.Context) {
        Status = EFI_DEVICE_ERROR;
        break;
      }
      
      // low,    full,   high,   super
      // 0,      64,     512,    1024
      if (MaxPktSize == 1024)
        UsbfnChgDev->DeviceSpeed = USB_CHG_SPD_SUPER;
      else if (MaxPktSize == 512)
        UsbfnChgDev->DeviceSpeed = USB_CHG_SPD_HIGH;
      else if (MaxPktSize == 64)
        UsbfnChgDev->DeviceSpeed = USB_CHG_SPD_FULL;
      else 
        UsbfnChgDev->DeviceSpeed = USB_CHG_SPD_LOW;
      
      UsbfnChgDev->ChgDevState = CHG_DEV_ST_SPEED;
    }
    else {
      DEBUG((EFI_D_INFO, "Unhandled Device OemEvent "));
      UsbfnChgSrvPrintGuid(Payload->OemData.DataGuid, EFI_D_INFO);
    }
    if (Payload->OemData.Context)
        FreePool(Payload->OemData.Context);
    break;
  case UsbDeviceEventDeviceStateChange:
    if (UsbDeviceStateConnected == Payload->DeviceState) {
      DBG(EFI_D_INFO, "Device Enumeration Event");
      UsbfnChgDev->ChgDevState = CHG_DEV_ST_ENUM;      
    }
    else if (UsbDeviceStateDisconnected == Payload->DeviceState) {
      DBG(EFI_D_INFO, "Cable Disconnected");
      UsbfnChgDev->ChgDevState = CHG_DEV_ST_DISCONNECT;
      UsbfnChgDev->ChargerType = USB_CHARGER_UNKNOWN;      
    }
    else {
      DBG(EFI_D_ERROR, "USB Device Protocol reports invalid State");
    }
    break;
  case UsbDeviceEventTransferNotification:
    DBG(EFI_D_INFO, "Transfer Complete Event", Status);
    Status = HandleTransferNotification(UsbfnChgDev, Payload);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to handle xfer complete: %r", Status);
      Status = UsbfnChgDev->UsbDeviceProtocol->SetEndpointStallState(USB_CTRL_EP_RX_IDX, TRUE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Failed to stall control ep: %r", Status);
      }
    }
    break;
  default:
    DBG(EFI_D_ERROR, "Unhandled Device Event %d", Event);
  }

  return Status;
}

/**
* @brief Event callback function to handle events from Usb Device Protocol. 
*
* @param[in]  Event                   Pointer to this event
* @param[in]  Context                 Event handler private data
*/
VOID
EventHandlerTimeNotifyFunc(
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
)
{
  EFI_STATUS                      Status            = EFI_SUCCESS;
  USBFN_CHG_DEV                  *UsbfnChgDev       = (USBFN_CHG_DEV *)Context;
  USB_DEVICE_EVENT                Msg               = UsbDeviceEventNoEvent;
  USB_DEVICE_EVENT_DATA           Payload;
  UINTN                           PayloadSize       = 0;

  if (!Context) {
    DBG(EFI_D_ERROR, "Context cannot be NULL");
    goto ON_EXIT;
  }
 
  // There should be no event to be handled by the user. 
  // However, HandleEvent function should be called periodically
  // so the USB hw event can be handled by the underlying drivers
  Status = UsbfnChgDev->UsbDeviceProtocol->HandleEvent(&Msg, &PayloadSize, &Payload);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Device Protocol HandleEvent failed %r", Status);
    goto ON_EXIT;
  }

  if (Msg != UsbDeviceEventNoEvent) {
    DeviceEventHandler(UsbfnChgDev, Msg, PayloadSize, &Payload);
  } 
 
ON_EXIT:
  return;
}


/**
* @brief Start the process of charger detection and device enumeration.
* 
* The function blocks until one of the followings happen:
* 1. Device enumeration
* 2. CDP, DCP, IDCP is detected. 
* 3. Cable disconnect
* 4. USBFN_EVT_MAX_WAIT_TIME_IN_MS has passed
*
* @param[in]  UsbfnChgDev       An instance of USBFN_CHG_DEV
*
* @retval EFI_SUCCESS           The operation succeeded.
* @retval Others                The operation failed.
*/
EFI_STATUS
UsbfnChgEnumDetectChgType(
  IN USBFN_CHG_DEV *UsbfnChgDev
)
{
  EFI_STATUS                      Status            = EFI_SUCCESS;
  USB_DEVICE_EVENT                Msg               = UsbDeviceEventNoEvent;
  USB_DEVICE_EVENT_DATA           Payload;
  UINTN                           PayloadSize       = 0;  
  UINT32                          TimeStart, TimeEnd, TimeInv;

  TimeStart = GetTimerCountms();

  for (;;) {
    Status = UsbfnChgDev->UsbDeviceProtocol->HandleEvent(&Msg, &PayloadSize, &Payload);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "DeviceProtocol HandleEvent failed %r", Status);
      continue;
    }

    if (Msg != UsbDeviceEventNoEvent) {
      DeviceEventHandler(UsbfnChgDev, Msg, PayloadSize, &Payload);
    }
    
    // Exit the loop for the following reasons:
    // 1. USB Speed is determined for SDP and CDP
    if ((UsbfnChgDev->ChgDevState == CHG_DEV_ST_SPEED) && 
        (UsbfnChgDev->ChargerType == USB_CHARGER_SDP || UsbfnChgDev->ChargerType == USB_CHARGER_CDP)) {
      break;
    }
    
    // 2. If the charger type is not SDP or CDP, exit once reaching the state of CHG_DEV_ST_CHARGER
    if ((UsbfnChgDev->ChgDevState == CHG_DEV_ST_CHARGER) &&
        (UsbfnChgDev->ChargerType != USB_CHARGER_SDP && UsbfnChgDev->ChargerType != USB_CHARGER_CDP)) {
      break;      
    }
    
    // 3. If cable disconnect
    if (UsbfnChgDev->ChgDevState == CHG_DEV_ST_DISCONNECT)
      break;
    
    TimeEnd = GetTimerCountms();

    // The time wraps around. Since TimeStart is very close to the point
    // where it wraps, simply set TimeInv to TimeEnd. 
    if (TimeEnd < TimeStart) {
      TimeStart = TimeEnd;
      TimeInv   = TimeEnd;
    }
    else {
      TimeInv = TimeEnd - TimeStart;
    }
    if (TimeInv >= USBFN_EVT_MAX_WAIT_TIME_IN_MS) {
      break;
    }
  }
  return Status;
}


/**
@ brief Return detected charger type.

@param[in]  This           Pointer to the EFI_USBFN_CHG_SRV_PROTOCOL instance.
@param[out] Type           Pointer to the detected charger type

@see    EFIUsbfnChgSrv.h

@return EFI_SUCCESS       The operation succeeded.
@return Others            The operation failed.
**/
EFI_STATUS
EFIAPI 
UsbfnChgGetChargerType(
  IN   EFI_USBFN_CHG_SRV_PROTOCOL     *This,
  OUT  EFI_USBFN_CHG_SRV_CHARGER_TYPE *Type
)
{
  EFI_STATUS                      Status       = EFI_SUCCESS;
  USBFN_CHG_DEV                  *UsbfnChgDev  = NULL;

  if (!This || !Type) {
    DBG(EFI_D_ERROR, "EFI_USBFN_CHG_SRV_PROTOCOL is NULL");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  UsbfnChgDev = USBFN_CHG_DEV_FROM_THIS(This);

  if (!UsbfnChgDev) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  *Type = UsbfnChgDev->ChargerType;
ON_EXIT:
  return Status;
}


/**
@ brief Return detected device speed.

@param[in]  This           Pointer to the EFI_USBFN_CHG_SRV_PROTOCOL instance.
@param[out] Speed          Pointer to the detected speed type

@see    EFIUsbfnChgSrv.h

@return EFI_SUCCESS       The operation succeeded.
@return Others            The operation failed.
**/
EFI_STATUS
EFIAPI 
UsbfnChgGetSpeed(
  IN   EFI_USBFN_CHG_SRV_PROTOCOL     *This,
  OUT  EFI_USBFN_CHG_SRV_SPEED_TYPE   *Speed
)
{
  EFI_STATUS                      Status       = EFI_SUCCESS;
  USBFN_CHG_DEV                  *UsbfnChgDev  = NULL;

  if (!This || !Speed) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  UsbfnChgDev = USBFN_CHG_DEV_FROM_THIS(This);

  if (!UsbfnChgDev) {
    DBG(EFI_D_ERROR, "UsbFnChgDev is NULL");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  *Speed = UsbfnChgDev->DeviceSpeed;
ON_EXIT:
  return Status;
}


/**
@ brief Start the USB controller for charger detection and enumeration.

The function starts the USB controller through USB Device Protocol. The function 
performs charger detection and/or USB enumeration within max time of 
USBFN_EVT_MAX_WAIT_TIME_IN_MS before returning the control back to the caller.
A periodical timer is scheduled to handle any event from the USB Device Protocol. 

@param[in]  This              An instance of EFI_USBFN_CHG_SRV_PROTOCOL pointer.

@see    UsbfnChgEnumDetectChgType
@see    EFIUsbfnChgSrv.h

@return EFI_SUCCESS       The operation succeeded. 
@return Others            The operation failed. 
**/
EFI_STATUS
EFIAPI
UsbfnChgStartController(
  IN   EFI_USBFN_CHG_SRV_PROTOCOL   *This
)
{
  EFI_STATUS                      Status            = EFI_SUCCESS;
 
  USBFN_CHG_DEV                  *UsbfnChgDev       = NULL;
  EFI_USB_DEVICE_PROTOCOL        *UsbDeviceProtocol = NULL;
  USB_DEVICE_DESCRIPTOR          *DevDesc           = NULL;
  VOID                          **Descriptors       = NULL;   
  USB_DEVICE_DESCRIPTOR          *SSDevDesc         = NULL;
  VOID                          **SSDescriptors     = NULL;

  if (!This) {
    DBG(EFI_D_ERROR, "EFI_USBFN_CHG_SRV_PROTOCOL is NULL");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  UsbfnChgDev = USBFN_CHG_DEV_FROM_THIS(This);

  if (!UsbfnChgDev) {
    DBG(EFI_D_ERROR, "UsbFnChgDev is NULL");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  UsbDeviceProtocol = UsbfnChgDev->UsbDeviceProtocol;

  Status = UsbDeviceProtocol->AllocateTransferBuffer(UsbfnChgDev->CtrlXferBufferSize, &UsbfnChgDev->CtrlXferBuffer);
  if (Status != EFI_SUCCESS) {
    DBG(EFI_D_ERROR, "Cannot create control buffer");
    goto ON_EXIT;
  }

  Status = BuildDefaultDescriptors(&DevDesc, &Descriptors, &SSDevDesc, &SSDescriptors);
  if (Status != EFI_SUCCESS)
  {
    DBG(EFI_D_ERROR, "BuildDefaultDesc failed");
    goto ON_EXIT;
  }
  
  gDescSet.DeviceDescriptor = DevDesc;
  gDescSet.Descriptors = Descriptors;
  gDescSet.SSDeviceDescriptor = SSDevDesc;
  gDescSet.SSDescriptors = SSDescriptors;
  gDescSet.DeviceQualifierDescriptor = &DeviceQualifier;
  gDescSet.BinaryDeviceOjectStore =  (VOID *) &BinaryObjectStore;
  gDescSet.StringDescriptorCount = USBFN_CHG_DEV_NUM_STR_DESC;
  gDescSet.StringDescritors = StrDescriptors;

  Status = UsbDeviceProtocol->StartEx(&gDescSet);

  UsbfnChgEnumDetectChgType(UsbfnChgDev);

  // Enumeration and Charger detection are done.
  // Before return the control to the caller, schedule a periodical
  // timer to call HandleEvent
  Status = gBS->CreateEvent(
    EVT_TIMER | EVT_NOTIFY_SIGNAL,
    TPL_CALLBACK,
    EventHandlerTimeNotifyFunc,
    UsbfnChgDev,
    &UsbfnChgDev->EventHandlerTimer);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to create timer: %r", Status);
    UsbfnChgDev->EventHandlerTimer = NULL;
    goto ON_EXIT;
  }
    
  Status = gBS->SetTimer(
    UsbfnChgDev->EventHandlerTimer,
    TimerPeriodic,
    USBFN_EVT_POLLING_INTERVAL);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Fail to start polling timer %r", Status);
    goto ON_EXIT;
  } 

ON_EXIT: 
  FreeDefaultDescriptors(DevDesc, Descriptors, SSDescriptors);
  return Status;
}


/**
@ brief Stop the USB controller.

The function stops the controller and the background polling timer.

@param[in]  This              An instance of EFI_USBFN_CHG_SRV_PROTOCOL pointer.

@see    EFIUsbfnChgSrv.h

@return EFI_SUCCESS       The operation succeeded.
@return Others            The operation failed.
**/
EFI_STATUS
EFIAPI
UsbfnChgStopController(
  IN   EFI_USBFN_CHG_SRV_PROTOCOL   *This
)
{
  EFI_STATUS                      Status            = EFI_SUCCESS;  
  USBFN_CHG_DEV                  *UsbfnChgDev       = NULL;
  EFI_USB_DEVICE_PROTOCOL        *UsbDeviceProtocol = NULL;  

  if (!This) {
    DBG(EFI_D_ERROR, "EFI_USBFN_CHG_SRV_PROTOCOL is NULL");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  UsbfnChgDev = USBFN_CHG_DEV_FROM_THIS(This);

  if (!UsbfnChgDev) {
    DBG(EFI_D_ERROR, "UsbFnChgDev is NULL");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  UsbDeviceProtocol = UsbfnChgDev->UsbDeviceProtocol;

  // Close EventHandlerTimer
  if (UsbfnChgDev->EventHandlerTimer) {
    gBS->CloseEvent(UsbfnChgDev->EventHandlerTimer);
    UsbfnChgDev->EventHandlerTimer = NULL;
  }
  
  // De-allocate Buffer
  if (UsbfnChgDev->CtrlXferBuffer) {
    UsbfnChgDev->UsbDeviceProtocol->FreeTransferBuffer(UsbfnChgDev->CtrlXferBuffer);
    UsbfnChgDev->CtrlXferBuffer = NULL;
  }

  // Stop the controller
  Status = UsbDeviceProtocol->Stop();
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Unable to stop controller");    
  }

  UsbfnChgDev->ChargerType = USB_CHARGER_UNKNOWN;
  UsbfnChgDev->ChgDevState = CHG_DEV_ST_UNINIT;

ON_EXIT:  
  return Status;
}


/**
* @brief Create and initialize a USBFN_CHG_DEV instance
*
* @retval New instance on success
* @retval NULL on failure
*/
USBFN_CHG_DEV *
CreateUsbfnChgDevInstance(
  VOID
)
{
  EFI_STATUS              Status      = EFI_SUCCESS;
  USBFN_CHG_DEV          *UsbfnChgDev = NULL;

  // Allocate the software device structure
  UsbfnChgDev = (USBFN_CHG_DEV *)AllocateZeroPool(sizeof(USBFN_CHG_DEV));
  if (!UsbfnChgDev) {
    DBG(EFI_D_ERROR, "memory allocation failed for UsbfnChgDev");
    goto ON_EXIT;
  }

  UsbfnChgDev->Signature = USBFN_CHG_DEV_SIGNATURE;

  // Init Protocol
  UsbfnChgDev->UsbfnChgSrvProtocol.Revision        = EFI_USB_DEVICE_PROTOCOL_REVISION;
  UsbfnChgDev->UsbfnChgSrvProtocol.StartController = UsbfnChgStartController;
  UsbfnChgDev->UsbfnChgSrvProtocol.StopController  = UsbfnChgStopController;
  UsbfnChgDev->UsbfnChgSrvProtocol.GetChargerType  = UsbfnChgGetChargerType;
  UsbfnChgDev->UsbfnChgSrvProtocol.GetSpeed        = UsbfnChgGetSpeed;
  UsbfnChgDev->CtrlXferBuffer = NULL;
  UsbfnChgDev->CtrlXferBufferSize = USB_CTRL_XFER_MAX_BUF_SIZ;
  UsbfnChgDev->DeviceSpeed  = USB_CHG_SPD_UNKNOWN;
  UsbfnChgDev->ChargerType  = USB_CHARGER_UNKNOWN;
  UsbfnChgDev->ChgDevState  = CHG_DEV_ST_UNINIT;  

  Status = gBS->LocateProtocol(
    &gEfiUsbDeviceProtocolGuid,
    NULL,
    (VOID **)&UsbfnChgDev->UsbDeviceProtocol);
  if (Status != EFI_SUCCESS) {
    DBG(EFI_D_ERROR, "Cannot locate UsbDevice Protocol");
    goto ON_EXIT;
  }

  // Register for boot services exit event.
  Status = gBS->CreateEventEx(
    EVT_NOTIFY_SIGNAL,
    TPL_CALLBACK,
    UsbfnChgSrvExitBootService,
    UsbfnChgDev,
    &gEfiEventExitBootServicesGuid,
    &UsbfnChgDev->ExitBootServiceEvent);
    
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "UsbfnChgEntryPoint: ExitBootServiceEvent creation failed %r", Status);
    UsbfnChgDev->ExitBootServiceEvent = NULL;
    goto ON_EXIT;
  } 

ON_EXIT:
  if (EFI_ERROR(Status)){
    FreePool(UsbfnChgDev);
    UsbfnChgDev = NULL;
  }
  return UsbfnChgDev;
}


/**
  UsbfnChg Entry point

  @param[in]  ImageHandle       EFI_HANDLE.
  @param[in]  SystemTable       EFI_SYSTEM_TABLE.

  @return EFI_SUCCESS       Success.
**/
EFI_STATUS
EFIAPI
UsbfnChgEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS     Status      = EFI_SUCCESS;
  USBFN_CHG_DEV *UsbfnChgDev = NULL;

  UsbfnChgDev = CreateUsbfnChgDevInstance();

  if (!UsbfnChgDev) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  Status = gBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
    &gEfiUsbfnChgSrvProtocolGuid,
    &UsbfnChgDev->UsbfnChgSrvProtocol,
    NULL);

  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to install UsbfnChgSrv protocol: %r", Status);
    goto ON_EXIT;
  }

ON_EXIT:
  return Status;
}

