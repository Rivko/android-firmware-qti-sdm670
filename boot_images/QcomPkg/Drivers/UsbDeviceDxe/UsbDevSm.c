/**
 * @file UsbDevSm.c
 *
 * @brief USB device state machine
 *
 * This file implements the external interface to the state machine
 * which manages the USB device and transfer states.
 *
 * Copyright (c) 2014-2016 by QUALCOMM Technologies, Inc. All Rights Reserved.
 */ 
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/25/16   ck      Define USB STATUS as UINT16
 12/01/15   ck      Add USB SS and USBCV test support
 01/15/15   ck      Added support of USB enumeration for charger app
 09/17/14   ck      Update for the new USB Device header
 08/21/14   ck      Rename/Remove variables and functions.
 06/05/14   cching  Temporary workaround for MSM 8994 to fix enumeration issue
 02/06/14   ml      Initial implementation
 =============================================================================*/


#include "UsbDevSm.h"
#include "UsbDevSmImpl.h"
#include "UsbDeviceDxe.h"
#include "UsbDevUtil.h"


/**
 * @brief The Usbfn state machine instance.
 *
 * Definition of the state machine structure and each state's name and function
 * pointer mapping.  The state definitions here must be in the same order as
 * the state enum and the event functions in each state must be in the same
 * order as the state machine events.
 */
DEV_STATE_MACHINE   UsbfnDevSm = {
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
        Dev_EventError,
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
        Dev_EventError,
        Dev_Common_EpStatusChangedRx,
        Dev_EventError,
        Dev_Common_Detach,
        Dev_EventError,
        Dev_Common_Reset,
        Dev_EventError,
        Dev_Suspended_Resume,
        NULL
      }
    },
  },
  // Current State
  DEV_STATE_UNINITIALIZED,
  // Previous State
  DEV_STATE_UNINITIALIZED,
  // State machine data
  NULL,                     // UsbfnIo
  0,                        // UsbDevStatus
  EfiUsbUnknownPort,        // PortType
  UsbBusSpeedUnknown,       // BusSpeed
  0,                        // BusAddress
  DEV_INITIAL_STATE,        // ResumeState
  {0},                      // CtrlXfer
  {{0},{0},{0},0,0,0,0,0},  // DescriptorSet
  FALSE                     // ClsVndReq
};


/*
 * See header for documentation.
 */
EFI_STATUS
DevSmInitialize (
  IN EFI_USBFN_IO_PROTOCOL      *UsbfnIo,
  IN BOOLEAN                    UsbfnZlt
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  FNC_ENTER_MSG();

  if (NULL == UsbfnIo) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // initialize the state machine structure
  UsbfnDevSm.UsbfnIo                = UsbfnIo;

  // by default, class/vendor requestn handling state is false
  UsbfnDevSm.ClsVndReq              = FALSE;
 
  // initialize and allocate control transfer buffer
  UsbfnDevSm.CtrlXfer.EndpointIndex = USBDEV_CONTROL_EP;
  UsbfnDevSm.CtrlXfer.XferState     = XFER_FREE;
  UsbfnDevSm.CtrlXfer.BufferSize    = USBDEV_CONTROL_BUF_SIZE;
  UsbfnDevSm.CtrlXfer.BufferOffset  = 0;
  UsbfnDevSm.CtrlXfer.XferSize      = 0;
  Status = UsbfnIo->AllocateTransferBuffer(
      UsbfnIo,
      UsbfnDevSm.CtrlXfer.BufferSize,
      (void **)&UsbfnDevSm.CtrlXfer.Buffer);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to allocate control transfer buffer: %r", Status);
    goto ON_EXIT;
  }
  DBG(EFI_D_VERBOSE, "Control transfer buffer = %p", UsbfnDevSm.CtrlXfer.Buffer);

  // enter the initial state
  UsbfnDevSm.CurrentState  = DEV_INITIAL_STATE;
  UsbfnDevSm.PreviousState = DEV_INITIAL_STATE;
  if (UsbfnDevSm.States[DEV_INITIAL_STATE].Entry != NULL) {
    UsbfnDevSm.States[DEV_INITIAL_STATE].Entry(&UsbfnDevSm, DEV_STATE_UNINITIALIZED);
  }

ON_EXIT:
  if (EFI_ERROR(Status)) {
    DevSmCleanup();
  }
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
VOID
DevSmCleanup (
  VOID
  )
{
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  // cancel and free control transfer
  if (NULL != UsbfnDevSm.CtrlXfer.Buffer) {

    Status = AbortXfer(UsbfnDevSm.UsbfnIo, &UsbfnDevSm.CtrlXfer);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to abort control transfer: %r", Status);
    }

    Status = UsbfnDevSm.UsbfnIo->FreeTransferBuffer(
        UsbfnDevSm.UsbfnIo,
        (void *)UsbfnDevSm.CtrlXfer.Buffer);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to free control transfer buffer: %r", Status);
    }
  }

  UsbfnDevSm.ClsVndReq              = FALSE;

  FNC_LEAVE_MSG();
}


/*
 * See header for documentation.
 */
EFI_STATUS
DevSmProcessEvent (
  IN  EFI_USBFN_MESSAGE           Message,
  IN  UINTN                       PayloadSize,
  IN  EFI_USBFN_MESSAGE_PAYLOAD   *Payload
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
      DevSmGetEventName(Message), DevSmGetStateName(UsbfnDevSm.CurrentState));

  PrevState = UsbfnDevSm.CurrentState;
  UsbfnDevSm.PreviousState = PrevState;
  if (PrevState >= DEV_NUM_STATES) {
    DBG(EFI_D_ERROR, "Invalid current state!");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  // get the next state from the current state's event function, (if it exists)
  if (UsbfnDevSm.States[PrevState].Event[DEV_GET_EVENT_IDX(Message)] != NULL) {
    NextState = UsbfnDevSm.States[PrevState].Event[DEV_GET_EVENT_IDX(Message)](
        &UsbfnDevSm,
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
    UsbfnDevSm.CurrentState = NextState;

    // finally call the next state's entry function (if it exists)
    if (UsbfnDevSm.States[NextState].Entry != NULL) {
      UsbfnDevSm.States[NextState].Entry(&UsbfnDevSm, PrevState);
    }

    DBG(EFI_D_VERBOSE, "Transitioned from %a to %a",
        DevSmGetStateName(PrevState), DevSmGetStateName(NextState));
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
  VOID
  )
{
  return UsbfnDevSm.CurrentState;
}


/*
 * See header for documentation.
 */
CHAR8 CONST *
DevSmGetStateName (
  IN  DEV_STATE               State
  )
{
  if (State >= DEV_NUM_STATES) {
    return NULL;
  }

  return UsbfnDevSm.States[State].Name;
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

