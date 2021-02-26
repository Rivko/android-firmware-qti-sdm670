/**
 * @file UsbDevSm.c
 *
 * @brief USB device state machine
 *
 * This file implements the external interface to the state machine
 * which manages the USB device and transfer states.
 *
 * Copyright (c) 2014, 2017 by QUALCOMM Technologies, Inc. All Rights Reserved.
 */ 
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/01/17   pm      Port from Boot.xf.1.4, Fix control transfers issue in middle of bulk data transfers
 12/08/14   ck      Fix incorrect LunNum after unmount
 10/14/14   ck      Initial Version. Introduce state machine and remove global vars
 =============================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>

#include "UsbMsdSm.h"         // header
#include "UsbMsdSmImpl.h"     // state entry & transition function implementation
#include "UsbMsdDesc.h"       // usb descriptors
#include "UsbMsdScsi.h"       // SCSI commands
#include "UsbMsdUtility.h"    // FNC_*_MSG

/**
 * @brief The Usbfn state machine instance.
 *
 * Definition of the state machine structure and each state's name and function
 * pointer mapping.  The state definitions here must be in the same order as
 * the state enum and the event functions in each state must be in the same
 * order as the state machine events.
 */
static CONST DEV_STATE_MACHINE UsbfnDevSmTemplate = {
  // State array
  {
    {
      "DEV_STATE_DETACHED",
      DEV_STATE_DETACHED,
      Dev_Detached_Entry,
      {
        Dev_EventError,
        Dev_EventError,
        Dev_EventError,
        Dev_EventError,
        Dev_Detached_Attach,
        Dev_EventError,
        Dev_EventError,
        Dev_EventError,
        Dev_EventError
      }
    },
    {
      "DEV_STATE_ATTACHED",
      DEV_STATE_ATTACHED,
      Dev_Attached_Entry,
      {
        Dev_EventError,
        Dev_Attached_EpStatusChangedRx,
        Dev_EventError,
        Dev_Common_Detach,
        Dev_EventError,
        Dev_Common_Reset,
        Dev_Common_Suspend,
        Dev_EventError,
        Dev_Common_Speed
      }
    },
    {
      "DEV_STATE_DEFAULT_ADDRESSED",
      DEV_STATE_DEFAULT_ADDRESSED,
      NULL,
      {
        Dev_Common_SetupPkt,
        Dev_Common_EpStatusChangedRx,
        Dev_Common_EpStatusChangedTx,
        Dev_Common_Detach,
        Dev_EventError,
        Dev_Common_Reset,
        Dev_Common_Suspend,
        Dev_EventError,
        Dev_Common_Speed
      }
    },
    {
      "DEV_STATE_CONFIGURED",
      DEV_STATE_CONFIGURED,
      NULL,
      {
        Dev_Configured_SetupPkt,
        Dev_Common_EpStatusChangedRx,
        Dev_Common_EpStatusChangedTx,
        Dev_Common_Detach,
        Dev_EventError,
        Dev_Common_Reset,
        Dev_Common_Suspend,
        Dev_EventError,
        Dev_EventError
      }
    },
    {
      "DEV_STATE_SUSPENDED",
      DEV_STATE_SUSPENDED,
      Dev_Suspended_Entry,
      {
        Dev_Configured_SetupPkt,
        Dev_Suspended_EpStatusChangedRx,
        Dev_Suspended_EpStatusChangedTx,
        Dev_Common_Detach,
        Dev_EventError,
        Dev_Common_Reset,
        Dev_EventError,
        Dev_Suspended_Resume,
        NULL
      }
    },
    {
      "DEV_STATE_CBW",
      DEV_STATE_CBW,
      Dev_CBW_Entry,
      {
        Dev_Configured_SetupPkt,
        Dev_CBW_EpStatusChangedRx,
        Dev_Common_EpStatusChangedTx,
        Dev_EventError,
        Dev_EventError,
        Dev_EventError,
        Dev_Common_Suspend,
        Dev_EventError,
        NULL
      }
    },
    {
      "DEV_STATE_SEND_TO_HOST",
      DEV_STATE_SEND_TO_HOST,
      NULL,
      {
        Dev_Configured_SetupPkt,
        Dev_EpStatusChangedRx,
        Dev_EpStatusChangedTx,
        Dev_EventError,
        Dev_EventError,
        Dev_EventError,
        Dev_Common_Suspend,
        Dev_EventError,
        NULL
      }
    },
    {
      "DEV_STATE_RCV_FROM_HOST",
      DEV_STATE_RCV_FROM_HOST,
      NULL,
      {
        Dev_Configured_SetupPkt,
        Dev_EpStatusChangedRx,
        Dev_EpStatusChangedTx,
        Dev_EventError,
        Dev_EventError,
        Dev_EventError,
        Dev_Common_Suspend,
        Dev_EventError,
        NULL
      }
    },    
    {
      "DEV_STATE_SEND_CSW",
      DEV_STATE_CSW,
      NULL,
      {
        Dev_Configured_SetupPkt,
        Dev_EpStatusChangedRx,
        Dev_CSW_EpStatusChangedTx,
        Dev_EventError,
        Dev_EventError,
        Dev_EventError,
        Dev_Common_Suspend,
        Dev_EventError,
        NULL
      }
    } 
  },
  // Current State
  DEV_STATE_UNINITIALIZED,
  // Previous State
  DEV_STATE_UNINITIALIZED,
  // State machine data
  NULL,                 // UsbfnIo
  0,                    // UsbDevStatus
  EfiUsbUnknownPort,    // PortType 
  UsbBusSpeedUnknown,   // BusSpeed
  0,                    // BusAddress
  DEV_INITIAL_STATE,    // ResumeState
  {0},                  // SerialNumber
  0,                    // SerialNumberLen
  FALSE,                // RcvcMsdLunQuery
  {0},                  // CtrlXfer
  {0},                  // MsdBulkOut
  {0},                  // MsdBulkIn
  {0},                  // CBW
  {0},                  // CSW
};


//
// Prepare the the software before writing RS bit in HW, which
// starts the HW for USB enumeration
//
STATIC
EFI_STATUS
SetDeviceSerialNumber(
  IN DEV_STATE_MACHINE * UsbDevSm
)
{

  EFI_STATUS              Status            = EFI_SUCCESS;
  EFI_USBFN_IO_PROTOCOL  *UsbfnIo           = NULL;  

  ARG_CHK(UsbDevSm);

  UsbfnIo = UsbDevSm->UsbfnIo;
  UsbDevSm->SerialNumberLen                 = USB_MAX_SERIAL_NUMBER_SIZE_IN_BYTES;

  // Query the device serial number
  gBS->SetMem(UsbDevSm->SerialNumber, USB_MAX_SERIAL_NUMBER_SIZE_IN_BYTES, 0);
  Status = UsbfnIo->GetDeviceInfo(
    UsbfnIo,
    EfiUsbDeviceInfoSerialNumber,
    &UsbDevSm->SerialNumberLen,
    UsbDevSm->SerialNumber);
  ERR_CHK("Failed to query serial number: %r", Status);
  DBG(EFI_D_INFO, "Serial Number: %s, Len %d", UsbDevSm->SerialNumber, UsbDevSm->SerialNumberLen);

ON_EXIT:
  // not able to query from USBFn, set a constant value
  if (EFI_ERROR(Status)) {
    gBS->CopyMem(UsbDevSm->SerialNumber, (VOID *)StrSerialDescriptor, sizeof(StrSerialDescriptor));
    UsbDevSm->SerialNumberLen = sizeof(StrSerialDescriptor);
  }  
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
DevSmInitialize(
  IN DEV_STATE_MACHINE     *UsbDevSm,
  IN EFI_USBFN_IO_PROTOCOL *UsbfnIo
  )
{
  EFI_STATUS    Status     = EFI_SUCCESS;
  
  FNC_ENTER_MSG();

  ARG_CHK(UsbfnIo);

  gBS->CopyMem(UsbDevSm, (VOID *) &UsbfnDevSmTemplate, sizeof(UsbfnDevSmTemplate));

  // initialize the state machine structure
  UsbDevSm->UsbfnIo                = UsbfnIo;
 
  // initialize and allocate control and bulk transfer buffers
  UsbDevSm->CtrlXfer.EndpointIndex = USBMSD_CONTROL_EP;
  UsbDevSm->CtrlXfer.XferState     = XFER_FREE;
  UsbDevSm->CtrlXfer.BufferSize    = USBDEV_CONTROL_BUF_SIZE;
  UsbDevSm->CtrlXfer.BufferOffset  = 0;
  UsbDevSm->CtrlXfer.XferSize      = 0;
  Status = UsbfnIo->AllocateTransferBuffer(
      UsbfnIo,
      UsbDevSm->CtrlXfer.BufferSize,
      (void **)&UsbDevSm->CtrlXfer.Buffer);
  ERR_CHK("Failed to allocate control transfer buffer: %r", Status);
  DBG(EFI_D_VERBOSE, "Control transfer buffer = %p", UsbDevSm->CtrlXfer.Buffer);

  UsbDevSm->BulkTxXfer.EndpointIndex       = USBMSD_BULK_EP;
  UsbDevSm->BulkTxXfer.EndpointDirection   = EfiUsbEndpointDirectionDeviceTx;
  UsbDevSm->BulkTxXfer.XferState           = XFER_FREE;
  UsbDevSm->BulkTxXfer.BufferSize          = USBDEV_DEFAULT_BULK_BUF_SIZE;
  UsbDevSm->BulkTxXfer.BufferOffset        = 0;
  UsbDevSm->BulkTxXfer.XferSize            = 0;
  Status = UsbfnIo->AllocateTransferBuffer(
    UsbfnIo,
    UsbDevSm->BulkTxXfer.BufferSize,
    (void **)&UsbDevSm->BulkTxXfer.Buffer);
  ERR_CHK("Failed to allocate BULK TX transfer buffer: %r", Status);
  DBG(EFI_D_VERBOSE, "BULK TX transfer buffer = %p", UsbDevSm->BulkTxXfer.Buffer);

  UsbDevSm->BulkRxXfer = UsbDevSm->BulkTxXfer;
  UsbDevSm->BulkRxXfer.EndpointDirection = EfiUsbEndpointDirectionDeviceRx;
  Status = UsbfnIo->AllocateTransferBuffer(
    UsbfnIo,
    UsbDevSm->BulkRxXfer.BufferSize,
    (void **)&UsbDevSm->BulkRxXfer.Buffer);
  ERR_CHK("Failed to allocate BULK RX transfer buffer: %r", Status);
  DBG(EFI_D_VERBOSE, "BULK OUT transfer buffer = %p", UsbDevSm->BulkRxXfer.Buffer);

  // Init CBW and CSW
  UsbDevSm->CBW.EndpointIndex     = USBMSD_BULK_EP;
  UsbDevSm->CBW.EndpointDirection = EfiUsbEndpointDirectionDeviceRx;
  UsbDevSm->CBW.XferState         = XFER_FREE;
  UsbDevSm->CBW.BufferSize        = QHSUSB_CBW_LEN;
  UsbDevSm->CBW.BufferOffset      = 0;
  UsbDevSm->CBW.XferSize          = QHSUSB_CBW_LEN; 
  Status = UsbfnIo->AllocateTransferBuffer(
    UsbfnIo,
    UsbDevSm->CBW.BufferSize,
    (void **)&UsbDevSm->CBW.Buffer);  
  ERR_CHK("Failed to allocate CBW transfer buffer: %r", Status);
  DBG(EFI_D_VERBOSE, "CBW transfer buffer = %p", UsbDevSm->CBW.Buffer);

  UsbDevSm->CSW.EndpointIndex     = USBMSD_BULK_EP;
  UsbDevSm->CSW.EndpointDirection = EfiUsbEndpointDirectionDeviceTx;
  UsbDevSm->CSW.XferState         = XFER_FREE;
  UsbDevSm->CSW.BufferSize        = QHSUSB_CSW_LEN;
  UsbDevSm->CSW.BufferOffset      = 0;
  UsbDevSm->CSW.XferSize          = QHSUSB_CSW_LEN;
  Status = UsbfnIo->AllocateTransferBuffer(
    UsbfnIo,
    UsbDevSm->CSW.BufferSize,
    (void **)&UsbDevSm->CSW.Buffer);
  ERR_CHK("Failed to allocate CSW transfer buffer: %r", Status);
  DBG(EFI_D_VERBOSE, "CSW transfer buffer = %p", UsbDevSm->CSW.Buffer);
  ((qhsusb_csw_t *) UsbDevSm->CSW.Buffer)->dCSWDataResidue = 0;
  ((qhsusb_csw_t *) UsbDevSm->CSW.Buffer)->bCSWStatus      = CSW_GOOD_STATUS;
  ((qhsusb_csw_t *) UsbDevSm->CSW.Buffer)->dCSWSignature   = QHSUSB_CSW_SIGNATURE;

  // Query and set MSD serial number 
  SetDeviceSerialNumber(UsbDevSm);

  // enter the initial state
  UsbDevSm->CurrentState  = DEV_INITIAL_STATE;
  UsbDevSm->PreviousState = DEV_INITIAL_STATE;
  if (UsbDevSm->States[DEV_INITIAL_STATE].Entry != NULL) {
    UsbDevSm->States[DEV_INITIAL_STATE].Entry(UsbDevSm, DEV_STATE_UNINITIALIZED);
  }

ON_EXIT:
  if (EFI_ERROR(Status)) {
    DevSmCleanup(UsbDevSm);
  }
  FNC_LEAVE_MSG();  
  return Status;
}


/*
 * See header for documentation.
 */
VOID
DevSmCleanup (
  IN DEV_STATE_MACHINE *UsbDevSm
  )
{
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  // cancel and free transfers
  if (NULL != UsbDevSm->CtrlXfer.Buffer) {
    Status = UsbDevSm->UsbfnIo->FreeTransferBuffer(
        UsbDevSm->UsbfnIo,
        (void *)UsbDevSm->CtrlXfer.Buffer);
    WRN_CHK("Failed to free control transfer buffer: %r", Status);
    UsbDevSm->CtrlXfer.Buffer = NULL;
  }

  if (NULL != UsbDevSm->BulkRxXfer.Buffer) {
    Status = UsbDevSm->UsbfnIo->FreeTransferBuffer(
      UsbDevSm->UsbfnIo,
      (void *)UsbDevSm->BulkRxXfer.Buffer);
    WRN_CHK("Failed to free BulkRxXfer transfer buffer: %r", Status);
    UsbDevSm->BulkRxXfer.Buffer = NULL;
  }

  if (NULL != UsbDevSm->BulkTxXfer.Buffer) {
    Status = UsbDevSm->UsbfnIo->FreeTransferBuffer(
      UsbDevSm->UsbfnIo,
      (void *)UsbDevSm->BulkTxXfer.Buffer);
    WRN_CHK("Failed to free BulkTxXfer transfer buffer: %r", Status);
    UsbDevSm->BulkTxXfer.Buffer = NULL;
  }

  if (NULL != UsbDevSm->CBW.Buffer) {
    Status = UsbDevSm->UsbfnIo->FreeTransferBuffer(
      UsbDevSm->UsbfnIo,
      (void *)UsbDevSm->CBW.Buffer);
    WRN_CHK("Failed to free CBW transfer buffer: %r", Status);
    UsbDevSm->CBW.Buffer = NULL;
  }

  if (NULL != UsbDevSm->CSW.Buffer) {
    Status = UsbDevSm->UsbfnIo->FreeTransferBuffer(
      UsbDevSm->UsbfnIo,
      (void *)UsbDevSm->CSW.Buffer);
    WRN_CHK("Failed to free CBW transfer buffer: %r", Status);
    UsbDevSm->CSW.Buffer = NULL;
  }

  FNC_LEAVE_MSG();
}


/*
 * See header for documentation.
 */
EFI_STATUS
DevSmProcessEvent (
  IN  DEV_STATE_MACHINE          *UsbDevSm,
  IN  EFI_USBFN_MESSAGE           Message,
  IN  UINTN                       PayloadSize,
  IN  EFI_USBFN_MESSAGE_PAYLOAD  *Payload
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  DEV_STATE   PrevState, NextState;

  FNC_ENTER_MSG();

  if (NULL == Payload || Message < DEV_EVENT_OFFSET) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // make sure we have a valid event
  if (DEV_GET_EVENT_IDX(Message) >= DEV_NUM_EVENTS) {
    DBG(EFI_D_ERROR, "THIS SHOULDN'T HAPPEN!!");
    goto ON_EXIT;
  }
  DBG(EFI_D_VERBOSE, "Processing event %a in state %a",
      DevSmGetEventName(Message), DevSmGetStateName(UsbDevSm, UsbDevSm->CurrentState));

  PrevState = UsbDevSm->CurrentState;
  UsbDevSm->PreviousState = PrevState;
  if (PrevState >= DEV_NUM_STATES) {
    DBG(EFI_D_ERROR, "Invalid current state!");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  // get the next state from the current state's event function, (if it exists)
  if (UsbDevSm->States[PrevState].Event[DEV_GET_EVENT_IDX(Message)] != NULL) {
    NextState = UsbDevSm->States[PrevState].Event[DEV_GET_EVENT_IDX(Message)](
        UsbDevSm,
        Message,
        PayloadSize,
        Payload);

    // check for no transition
    if ((NextState == PrevState) || (NextState == DEV_STATE_SAME)) {
      DBG(EFI_D_VERBOSE, "Not changing state");
      goto ON_EXIT;
    }

    // check for valid next state (we already know it's not the current state)
    if (NextState >= DEV_NUM_STATES) {
      DBG(EFI_D_ERROR, "Invalid next state returned, not changing state!");
      Status = EFI_DEVICE_ERROR;
      goto ON_EXIT;
    }

    // update the current state first in case the entry function wants to use it
    UsbDevSm->CurrentState = NextState;

    // finally call the next state's entry function (if it exists)
    if (UsbDevSm->States[NextState].Entry != NULL) {
      UsbDevSm->States[NextState].Entry(UsbDevSm, PrevState);
    }

    DBG(EFI_D_VERBOSE, "Transitioned from %a to %a",
      DevSmGetStateName(UsbDevSm, PrevState), DevSmGetStateName(UsbDevSm, NextState));
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
DEV_STATE
DevSmGetCurrentState (
  IN DEV_STATE_MACHINE *UsbDevSm
  )
{
  return UsbDevSm->CurrentState;
}


/*
 * See header for documentation.
 */
CHAR8 CONST *
DevSmGetStateName (
  IN  DEV_STATE_MACHINE      *UsbDevSm, 
  IN  DEV_STATE               State
  )
{
  if (State >= DEV_NUM_STATES) {
    return NULL;
  }

  return UsbDevSm->States[State].Name;
}


/*
 * See header for documentation.
 */
CHAR8 CONST *
DevSmGetEventName (
  IN  EFI_USBFN_MESSAGE       Msg
  )
{
  switch (Msg) {
    case EfiUsbMsgNone:
      return "EfiUsbMsgNone";
    case EfiUsbMsgSetupPacket:
      return "EfiUsbMsgSetupPacket";
    case EfiUsbMsgEndpointStatusChangedRx:
      return "EfiUsbMsgEndpointStatusChangedRx";
    case EfiUsbMsgEndpointStatusChangedTx:
      return "EfiUsbMsgEndpointStatusChangedTx";
    case EfiUsbMsgBusEventDetach:
      return "EfiUsbMsgBusEventDetach";
    case EfiUsbMsgBusEventAttach:
      return "EfiUsbMsgBusEventAttach";
    case EfiUsbMsgBusEventReset:
      return "EfiUsbMsgBusEventReset";
    case EfiUsbMsgBusEventSuspend:
      return "EfiUsbMsgBusEventSuspend";
    case EfiUsbMsgBusEventResume:
      return "EfiUsbMsgBusEventResume";
    case EfiUsbMsgBusEventSpeed:
      return "EfiUsbMsgBusEventSpeed";
    default:
      return "Unknown Usbfn Message";
  }
}

