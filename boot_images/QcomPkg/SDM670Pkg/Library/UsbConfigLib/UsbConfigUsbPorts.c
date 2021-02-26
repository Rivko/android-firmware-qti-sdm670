/** @file \UsbConfigUsbPorts.c

  Implement USB Type-C related functionality for the platform.

  Copyright (c) 2016 QUALCOMM Technologies Inc. All rights reserved.
**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 06/14/17   amitg    Cleanup
 04/04/17   pm       Added check for USB connector type in UsbConfigPortsQueryConnectionChange
 03/16/17   pm       Changed DBG Log level from WARN to VERBOSE for RUMI Platform 
                     in UsbConfigPortsQueryConnectionChange API  
 03/06/17   amitg    EUD Support
 02/03/17   pm       Port from Boot.xf.1.4
 12/20/16   sm       Edited input parameter list for UsbCGetConnectState and 
                     UsbCGetPortState PmicUsbProtocol API
 09/22/16   amitg    Napali Pre-Si Updates
 03/31/16   ck       Only free UsbPortHandle if allocated
 02/18/16   ck       Init Version
=============================================================================*/

#include "UsbConfigUsbPortsPrivate.h"

BOOLEAN SimulateDetach = FALSE;
BOOLEAN SimulateHostCableAttach = FALSE;
BOOLEAN SimulatePeriCableAttach = FALSE;

/* Check UsbConfigUsbPortsPublic.h for description. */
EFI_STATUS
UsbConfigPortsInit(
  IN  UINTN      CoreNum,
  OUT VOID     **UsbPortHandle
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_PORT *Port;

  if (!UsbPortHandle) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters: UsbPortHandle %p", UsbPortHandle);
  }

  Port = AllocateZeroPool(sizeof(QCOM_USB_PORT));
  if (!Port) {
    Status = EFI_OUT_OF_RESOURCES;
    ERR_CHK("Out of resources");
  }

  Port->Message = UsbCfgPortMsgNone;
  Port->ConnectState = USBC_STATE_DETACHED;
  Port->ConnectModeStatus = EFI_PM_USB_TYPEC_CONNECT_MODE_NONE;

  *UsbPortHandle = Port;

ON_EXIT:
  return Status;
}

/* Check UsbConfigUsbPortsPublic.h for description. */
EFI_STATUS
UsbConfigPortsDeinit(
  IN VOID     *UsbPortHandle
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  if (UsbPortHandle)
    FreePool(UsbPortHandle);
  return Status;
}


/**
@Brief - Print the fields in EFI_PM_USB_TYPEC_PORT_STATUS

@param [in]     PrintFlag          Debug level flag
@param [in]     PortStatus         An instance of EFI_PM_USB_TYPEC_PORT_STATUS


**/
__inline
static
VOID 
UsbConfigPortsPrintPortStatus (
  IN UINT32                               PrintFlag,
  IN EFI_PM_USB_TYPEC_PORT_STATUS         *PortStatus
)
{
  DEBUG((PrintFlag,
    "cc_out_sts 0x%x  dfp_curr_adv 0x%x  ufp_conn_type 0x%x",
    PortStatus->cc_out_sts,
    PortStatus->dfp_curr_adv,
    PortStatus->ufp_conn_type));
    
  DEBUG((PrintFlag,
    "  vbus_sts 0x%x  vbus_err_sts 0x%x  debounce_done_sts 0x%x",
    PortStatus->vbus_sts,
    PortStatus->vbus_err_sts,
    PortStatus->debounce_done_sts));

  DEBUG((PrintFlag,
    "  vconn_oc_sts 0x%x  ccout_out_hiz_sts 0x%x\n",
    PortStatus->vconn_oc_sts,
    PortStatus->ccout_out_hiz_sts));
}


/**
Query USB connection change.

The function generates messages of UsbCfgPortMsgPartnerAttach and
UsbCfgPortMsgPartnerDetach if the cable attach / detach happens.

@param [in]     UsbPort         The port to be queried
@param [out]    Message         The message generated based for attach / detach
@param [in,out] PayloadSize     The size of Payload associated with the message
@param [out]    Payload         The payload associated with the message.

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.
**/
static
EFI_STATUS
UsbConfigPortsQueryConnectionChange(
  IN     QCOM_USB_PORT                   *UsbPort,
  OUT    USB_CFG_PORT_MESSAGE            *Message,
  IN OUT UINTN                           *PayloadSize,
  OUT    USB_TYPEC_PORT_MESSAGE_PAYLOAD  *Payload
  ) 
{
  BOOLEAN                       VbusStatus = FALSE;
  EFI_STATUS Status = EFI_SUCCESS;
  //Initialize Message value to UsbCfgPortMsgNone 
  *Message = UsbCfgPortMsgNone;

  // Since there is no Type-C support for Pre-SI,
  // assume port in UFP mode and it is always attached.
  if ((PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI) ||
    (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_VIRTIO) ||
    (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_UNKNOWN)) {
    DBG(EFI_D_VERBOSE, "Pre-SI: Assume UFP and attached");

    if (UsbPort->ConnectState == USBC_STATE_DETACHED) {
      UsbPort->ConnectState = USBC_STATE_ATTACHED;
      *Message = UsbCfgPortMsgPartnerAttach;
      *PayloadSize = sizeof(USB_TYPEC_PORT_MODE);
      Payload->UsbTypeCPortMode = USB_TYPEC_PORT_MODE_UFP;
      // set to CC1 for RUMI
      UsbPort->PortStatus.cc_out_sts = EFI_PM_USB_TYPEC_CC_OUT_CC1;
    }
    goto ON_EXIT;
  }

  // Make sure USB Type-C Protocol is available
  if (NULL == gPmicUsbProtocol) {
    Status = EFI_NOT_FOUND;
    ERR_CHK("Failed to open USB Type-C Protocol");
  }

  //Get USB connector type (Type-C or Micro USB)
  Status = gPmicUsbProtocol->UsbCGetPortState(&UsbPort->PortStatus);
  ERR_CHK("Failed to get UsbCGetPortState with error: %r", Status);
  //DBG(EFI_D_INFO, "USB Connector Type: %d", UsbPort->PortStatus.usb_connector);
  
  if (EFI_PM_USB_CONNECTOR_TYPEC == UsbPort->PortStatus.usb_connector) {
    Status = gPmicUsbProtocol->UsbCGetConnectState(&UsbPort->ConnectModeStatus);
    ERR_CHK("Failed to get UsbCGetConnStateStatus");
    DBG(EFI_D_INFO, "ConnectModeStatus: %d", UsbPort->ConnectModeStatus);

    DBG(EFI_D_INFO, "ConnectModeStatus: %d", UsbPort->ConnectModeStatus);
    if (SimulateDetach) {
      UsbPort->ConnectModeStatus = EFI_PM_USB_TYPEC_CONNECT_MODE_NONE;
    }

    if (SimulateHostCableAttach) {
      UsbPort->ConnectModeStatus = EFI_PM_USB_TYPEC_CONNECT_MODE_DFP;
    }

    if (SimulatePeriCableAttach) {
      UsbPort->ConnectModeStatus = EFI_PM_USB_TYPEC_CONNECT_MODE_UFP;
    }

    if (SimulateHostCableAttach && SimulatePeriCableAttach) {
      ERR_CHK("Cannot simulate host and device at the same time");
    }
  
    switch (UsbPort->ConnectModeStatus) {
    case EFI_PM_USB_TYPEC_CONNECT_MODE_NONE:
      if (UsbPort->ConnectState == USBC_STATE_DETACHED)
        goto ON_EXIT;

      DBG(EFI_D_INFO, "USB Type-C Device Mode(None)");
      *Message = UsbCfgPortMsgPartnerDetach;
      Payload->UsbTypeCPortMode = USB_TYPEC_PORT_MODE_NONE;
      *PayloadSize = sizeof(USB_TYPEC_PORT_MODE);
    break;

    case EFI_PM_USB_TYPEC_CONNECT_MODE_UFP:
      if (UsbPort->ConnectState == USBC_STATE_ATTACHED)
        goto ON_EXIT;

      DBG(EFI_D_INFO, "USB Type-C Device Mode(UFP mode)");
      *Message = UsbCfgPortMsgPartnerAttach;
      Payload->UsbTypeCPortMode = USB_TYPEC_PORT_MODE_UFP;
      *PayloadSize = sizeof(USB_TYPEC_PORT_MODE);
    break;

    case EFI_PM_USB_TYPEC_CONNECT_MODE_DFP:
      if (UsbPort->ConnectState == USBC_STATE_ATTACHED)
        goto ON_EXIT;

    DBG(EFI_D_INFO, "USB Type-C Host Mode(DFP mode)");
      *Message = UsbCfgPortMsgPartnerAttach;
      Payload->UsbTypeCPortMode = USB_TYPEC_PORT_MODE_DFP;
      *PayloadSize = sizeof(USB_TYPEC_PORT_MODE);
    break;

    case EFI_PM_USB_TYPEC_CONNECT_MODE_INVALID:
      DBG(EFI_D_ERROR, "Invalid Typec-C connect mode detected");
      Payload->UsbTypeCPortMode = USB_TYPEC_PORT_MODE_INVALID;
      goto ON_EXIT;
	break;

	default:
      DBG(EFI_D_ERROR, "Unkown Typec-C connect mode detected");
      Payload->UsbTypeCPortMode = USB_TYPEC_PORT_MODE_INVALID;
      goto ON_EXIT;
    }
  }
  else if (EFI_PM_USB_CONNECTOR_MICRO_USB == UsbPort->PortStatus.usb_connector) {
    // Non-TypeC Method of Detection 
    Status = gPmicUsbProtocol->UsbinValid(&VbusStatus);
    ERR_CHK("Failed to get VbusStatus");
    DBG(EFI_D_INFO, "VbusStatus: %d", VbusStatus);

    if (VbusStatus) {
      if (UsbPort->ConnectState == USBC_STATE_DETACHED) {
        UsbPort->ConnectState = USBC_STATE_ATTACHED;
        *Message = UsbCfgPortMsgPartnerAttach;
        *PayloadSize = sizeof(USB_TYPEC_PORT_MODE);
        Payload->UsbTypeCPortMode = USB_TYPEC_PORT_MODE_UFP;
        UsbPort->ConnectModeStatus = EFI_PM_USB_TYPEC_CONNECT_MODE_UFP;
      }
	}
	else {
      if (UsbPort->ConnectState == USBC_STATE_ATTACHED) {
        UsbPort->ConnectState = USBC_STATE_DETACHED;
        *Message = UsbCfgPortMsgPartnerDetach;
        *PayloadSize = sizeof(USB_TYPEC_PORT_MODE);
        //Payload->UsbTypeCPortMode = USB_TYPEC_PORT_MODE_UFP;
        UsbPort->ConnectModeStatus = EFI_PM_USB_TYPEC_CONNECT_MODE_NONE;
      }
    }
  }
  else { /* usb_connector is equal to EFI_PM_USB_CONNECTOR_INVALID */
    Status = EFI_INVALID_PARAMETER; 
    ERR_CHK("Invalid USB connector type");	
  }
  if (*Message == UsbCfgPortMsgNone)
    goto ON_EXIT;
  // If we reach here, it means the connection status has been changed.
  if (*Message == UsbCfgPortMsgPartnerAttach) {
    UsbPort->ConnectState = USBC_STATE_ATTACHED;
  }
  else {
    UsbPort->ConnectState = USBC_STATE_DETACHED;
  }

  UsbConfigPortsPrintPortStatus(EFI_D_INFO, &UsbPort->PortStatus);

ON_EXIT:
  return Status;
}


/* Check UsbConfigUsbPortsPublic.h for description. */
EFI_STATUS
UsbConfigPortsEventHandler(
  IN     VOID                            *UsbPortHandle,
  OUT    USB_CFG_PORT_MESSAGE            *Message,
  IN OUT UINTN                           *PayloadSize,
  OUT    USB_TYPEC_PORT_MESSAGE_PAYLOAD  *Payload
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_PORT *UsbPort = (QCOM_USB_PORT *)UsbPortHandle;

  FNC_ENTER_MSG();

  if (!UsbPortHandle || !Message || !PayloadSize || !Payload) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters: UsbPortHandle %p, Message %p, PayloadSize %p, Payload %p",
      UsbPortHandle,
      Message,
      PayloadSize,
      Payload);
  }

  *Message = UsbCfgPortMsgNone;

  // Check if cable connection / disconnect event should be generated.
  Status = UsbConfigPortsQueryConnectionChange(
    UsbPort,
    Message,
    PayloadSize,
    Payload);

  WRN_CHK("UsbConfigPortsQueryPortConnectionUpdate failed %r", Status);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Check UsbConfigUsbPortsPublic.h for description. */
EFI_STATUS
UsbPortGetConnectionMode(
  IN  VOID                *UsbPortHandle,
  OUT USB_TYPEC_PORT_MODE *UsbPortMode
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_PORT *UsbPort = (QCOM_USB_PORT *) UsbPortHandle;

  FNC_ENTER_MSG();

  if (!UsbPortMode || !UsbPortHandle) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters: UsbPortHandle %p, PortMode %p",
      UsbPortHandle,
      UsbPortMode);
  }

  switch (UsbPort->ConnectModeStatus) {
  case EFI_PM_USB_TYPEC_CONNECT_MODE_UFP:
    *UsbPortMode = USB_TYPEC_PORT_MODE_UFP;
    break;
  case EFI_PM_USB_TYPEC_CONNECT_MODE_DFP:
    *UsbPortMode = USB_TYPEC_PORT_MODE_DFP;
    break;
  case EFI_PM_USB_TYPEC_CONNECT_MODE_NONE:
    *UsbPortMode = USB_TYPEC_PORT_MODE_NONE;
    break;
  default:
    *UsbPortMode = USB_TYPEC_PORT_MODE_INVALID;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Check UsbConfigUsbPortsPublic.h for description. */
EFI_STATUS
UsbPortGetRtConnectionMode(
  IN  VOID                *UsbPortHandle,
  OUT USB_TYPEC_PORT_MODE *UsbPortMode
  )
{
  EFI_STATUS                           Status            = EFI_SUCCESS;
  EFI_PM_USB_TYPEC_CONNECT_MODE_STATUS ConnectModeStatus = EFI_PM_USB_TYPEC_CONNECT_MODE_NONE;

  FNC_ENTER_MSG();

  if (!UsbPortMode || !UsbPortHandle) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters: UsbPortHandle %p, PortMode %p",
      UsbPortHandle,
      UsbPortMode);
  }

  if (NULL == gPmicUsbProtocol) {
    // Find the PMIC USB charger protocol
    Status = gBS->LocateProtocol(&gQcomPmicUsbProtocolGuid, NULL, (void**)&gPmicUsbProtocol);
    if (EFI_ERROR(Status))
    {
      gPmicUsbProtocol = NULL;
      WRN_CHK("Failed to open PMIC USB protocol Status =  (0x%x)", Status);
      goto ON_EXIT;
    }
  }

  Status = gPmicUsbProtocol->UsbCGetConnectState(&ConnectModeStatus);
  ERR_CHK("Failed to get UsbCGetConnStateStatus");
  DBG(EFI_D_INFO, "ConnectModeStatus: %d", ConnectModeStatus);
  
  switch (ConnectModeStatus) {
  case EFI_PM_USB_TYPEC_CONNECT_MODE_UFP:
    *UsbPortMode = USB_TYPEC_PORT_MODE_UFP;
    break;
  case EFI_PM_USB_TYPEC_CONNECT_MODE_DFP:
    *UsbPortMode = USB_TYPEC_PORT_MODE_DFP;
    break;
  case EFI_PM_USB_TYPEC_CONNECT_MODE_NONE:
    *UsbPortMode = USB_TYPEC_PORT_MODE_NONE;
    break;
  default:
    *UsbPortMode = USB_TYPEC_PORT_MODE_INVALID;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* Check UsbConfigUsbPortsPublic.h for description. */
EFI_STATUS
UsbPortGetCcOutStatus(
  IN  VOID                    *UsbPortHandle,
  OUT USB_TYPEC_CC_OUT_STATUS *CcOutStatus
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_PORT *UsbPort = (QCOM_USB_PORT *)UsbPortHandle;

  FNC_ENTER_MSG();

  if (!UsbPortHandle || !CcOutStatus) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters: UsbPortHandle %p, CcOutStatus %p",
      UsbPortHandle,
      CcOutStatus);
  }

  switch (UsbPort->PortStatus.cc_out_sts)
  {
  case EFI_PM_USB_TYPEC_CC_OUT_RESERVED:
    *CcOutStatus = USB_TYPEC_CC_OUT_RESERVED;
    break;
  case EFI_PM_USB_TYPEC_CC_OUT_CC1:
    *CcOutStatus = USB_TYPEC_CC_OUT_CC1;
    break;
  case EFI_PM_USB_TYPEC_CC_OUT_CC2:
    *CcOutStatus = USB_TYPEC_CC_OUT_CC2;
    break;
  case EFI_PM_USB_TYPEC_CC_OUT_OPEN:
    *CcOutStatus = USB_TYPEC_CC_OUT_OPEN;
    break;
  case EFI_PM_USB_TYPEC_CC_OUT_INVALID:
    *CcOutStatus = USB_TYPEC_CC_OUT_INVALID;
  default:
    *CcOutStatus = USB_TYPEC_CC_OUT_INVALID;
    break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}
