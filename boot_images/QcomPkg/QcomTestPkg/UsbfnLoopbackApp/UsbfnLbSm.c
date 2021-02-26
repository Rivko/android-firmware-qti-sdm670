/**
 * @file UsbfnLbSm.c
 *
 * @brief Loopback USB device state machine
 *
 * This file implements the external interface to the loopback state machine
 * which manages the USB device and transfer states.
 *
 * Copyright (c) 2014 by QUALCOMM Technologies, Inc. All Rights Reserved.
 */ 
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/20/14   ml      Migrate to official SuperSpeed support
 09/17/14   ck      Remove hard-coded max transfer size
 08/21/14   ck      Move device qualifier descriptor to Usb.h and minor fixes
 06/05/14   cching Temporary workaround for MSM 8994 to fix enumeration issue
 02/06/14   ml       Initial implementation
 =============================================================================*/


#include "UsbfnLbSm.h"
#include "UsbfnLbSmImpl.h"
#include "UsbfnLoopback.h"


/**
 * @brief The Usbfn Loopback state machine instance.
 *
 * Definition of the state machine structure and each state's name and function
 * pointer mapping.  The state definitions here must be in the same order as
 * the state enum and the event functions in each state must be in the same
 * order as the state machine events.
 */
ULB_STATE_MACHINE   UsbfnLbSm = {
  // State array
  {
    {
      "ULB_STATE_DETACHED",
      ULB_STATE_DETACHED,
      Ulb_Detached_Entry,
      {
        Ulb_EventError,
        Ulb_EventError,
        Ulb_EventError,
        Ulb_EventError,
        Ulb_Detached_Attach,
        Ulb_EventError,
        Ulb_EventError,
        Ulb_EventError,
        Ulb_EventError
      }
    },
    {
      "ULB_STATE_ATTACHED",
      ULB_STATE_ATTACHED,
      Ulb_Attached_Entry,
      {
        Ulb_EventError,
        Ulb_EventError,
        Ulb_EventError,
        Ulb_Common_Detach,
        Ulb_EventError,
        Ulb_Common_Reset,
        Ulb_Common_Suspend,
        Ulb_EventError,
        Ulb_Common_Speed
      }
    },
    {
      "ULB_STATE_DEFAULT_ADDRESSED",
      ULB_STATE_DEFAULT_ADDRESSED,
      NULL,
      {
        Ulb_Common_SetupPkt,
        Ulb_Common_EpStatusChangedRx,
        Ulb_Common_EpStatusChangedTx,
        Ulb_Common_Detach,
        Ulb_EventError,
        Ulb_Common_Reset,
        Ulb_Common_Suspend,
        Ulb_EventError,
        Ulb_Common_Speed
      }
    },
    {
      "ULB_STATE_CONFIGURED",
      ULB_STATE_CONFIGURED,
      Ulb_Configured_Entry,
      {
        Ulb_Configured_SetupPkt,
        Ulb_Common_EpStatusChangedRx,
        Ulb_Common_EpStatusChangedTx,
        Ulb_Common_Detach,
        Ulb_EventError,
        Ulb_Common_Reset,
        Ulb_Common_Suspend,
        Ulb_EventError,
        Ulb_EventError
      }
    },
    {
      "ULB_STATE_LOOPBACK",
      ULB_STATE_LOOPBACK,
      Ulb_Loopback_Entry,
      {
        Ulb_Common_SetupPkt,
        Ulb_Loopback_EpStatusChangedRx,
        Ulb_Loopback_EpStatusChangedTx,
        Ulb_Common_Detach,
        Ulb_EventError,
        Ulb_Common_Reset,
        Ulb_Common_Suspend,
        Ulb_EventError,
        Ulb_EventError
      }
    },
    {
      "ULB_STATE_SUSPENDED",
      ULB_STATE_SUSPENDED,
      Ulb_Suspended_Entry,
      {
        Ulb_EventError,
        Ulb_EventError,
        Ulb_EventError,
        Ulb_Common_Detach,
        Ulb_EventError,
        Ulb_Common_Reset,
        Ulb_EventError,
        Ulb_Suspended_Resume,
        NULL
      }
    },
  },
  // Current State
  ULB_STATE_UNINITIALIZED,

  // State machine data
  NULL,                 // UsbfnIo
  NULL,                 // DeviceInfo
  NULL,                 // SSDeviceInfo
  FALSE,                // RxUsbfnZlt
  FALSE,                // TxUsbfnZlt
  0,                    // MaxTransferSize
  0,                    // UsbDevStatus
  EfiUsbUnknownPort,    // PortType
  UsbBusSpeedUnknown,   // BusSpeed
  0,                    // BusAddress
  ULB_INITIAL_STATE,    // ResumeState
  {0},                  // CtrlXfer
  FALSE,                // RcvLbTestConfig
  {0},                  // LbTestConfig
  {0},                  // LbTestData
};


/**
 * @brief Signifies the timeout of a loopback test.
 *
 * Forces a transition of the state machine from loopback to configured to
 * allow a new test to start.
 *
 * @param[in] Event         Event that triggered the callback
 * @param[in] Context       NULL
 */
STATIC
VOID
UlbSmLoopbackTimeout (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
  ULB_STATE   PrevState;
  ULB_STATE   NextState;
  FNC_ENTER_MSG();

  // force transition back to configured state if we timed out in loopback
  PrevState = UsbfnLbSm.CurrentState;
  if (PrevState != ULB_STATE_LOOPBACK) {
    goto ON_EXIT;
  }

  PRINT(APL_ERROR, "Loopback test timed out!");

  // cancel transfers
  (void)AbortXfer(UsbfnLbSm.UsbfnIo, &UsbfnLbSm.LbTestData.Xfer);

  // update the current state first
  NextState = ULB_STATE_CONFIGURED;
  UsbfnLbSm.CurrentState = NextState;

  // call the next state's entry function (if it exists)
  if (UsbfnLbSm.States[NextState].Entry != NULL) {
    UsbfnLbSm.States[NextState].Entry(&UsbfnLbSm, PrevState);
  }

  PRINT(APL_VERB, "Transitioned from %a to %a",
      UlbSmGetStateName(PrevState), UlbSmGetStateName(NextState));

ON_EXIT:
  FNC_LEAVE_MSG();
}


/*
 * See header for documentation.
 */
EFI_STATUS
UlbSmInitialize (
  IN EFI_USBFN_IO_PROTOCOL          *UsbfnIo,
  IN EFI_USB_DEVICE_INFO            *DeviceInfo,
  IN EFI_USB_SUPERSPEED_DEVICE_INFO *SSDeviceInfo,
  IN BOOLEAN                        UsbfnZlt
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN       Buffer;
  UINTN       BufferSize;

  FNC_ENTER_MSG();

  if (NULL == UsbfnIo || NULL == DeviceInfo || NULL == SSDeviceInfo) {
    PRINT(APL_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // initialize the state machine structure
  UsbfnLbSm.UsbfnIo      = UsbfnIo;
  UsbfnLbSm.DeviceInfo   = DeviceInfo;
  UsbfnLbSm.SSDeviceInfo = SSDeviceInfo;
  UsbfnLbSm.RxUsbfnZlt   = FALSE;
  UsbfnLbSm.TxUsbfnZlt   = FALSE;

  // query and set the MaxTransferSize as appropriate
  BufferSize = sizeof(UsbfnLbSm.MaxTransferSize);
  Status = UsbfnIo->GetEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      EfiUsbPolicyMaxTransactionSize,
      &BufferSize,
      &UsbfnLbSm.MaxTransferSize);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to query MaxTransferSize: %r", Status);
    goto ON_EXIT;
  }
  if (UsbfnLbSm.MaxTransferSize > USBLB_MAX_TRANSFER_SIZE) {
    UsbfnLbSm.MaxTransferSize = USBLB_MAX_TRANSFER_SIZE;
  }

  // initialize and allocate control transfer buffer
  UsbfnLbSm.CtrlXfer.EndpointIndex = USBLB_CONTROL_EP;
  UsbfnLbSm.CtrlXfer.XferState     = XFER_FREE;
  UsbfnLbSm.CtrlXfer.BufferSize    = USBLB_CONTROL_BUF_SIZE;
  UsbfnLbSm.CtrlXfer.BufferOffset  = 0;
  UsbfnLbSm.CtrlXfer.XferSize      = 0;
  Status = UsbfnIo->AllocateTransferBuffer(
      UsbfnIo,
      UsbfnLbSm.CtrlXfer.BufferSize,
      (void **)&UsbfnLbSm.CtrlXfer.Buffer);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to allocate control transfer buffer: %r", Status);
    goto ON_EXIT;
  }
  PRINT(APL_VVERB, "Control transfer buffer = %p", UsbfnLbSm.CtrlXfer.Buffer);

  // initialize and allocate loopback transfer buffer
  UsbfnLbSm.LbTestData.Xfer.EndpointIndex = USBLB_BULK_EP;
  UsbfnLbSm.LbTestData.Xfer.XferState     = XFER_FREE;
  UsbfnLbSm.LbTestData.Xfer.BufferSize    = UsbfnLbSm.MaxTransferSize;
  UsbfnLbSm.LbTestData.Xfer.BufferOffset  = 0;
  UsbfnLbSm.LbTestData.Xfer.XferSize      = 0;
  do {
    // halve buffer size on allocation failure
    if (EFI_OUT_OF_RESOURCES == Status) {
      UsbfnLbSm.LbTestData.Xfer.BufferSize /= 2;
    }
    Status = UsbfnIo->AllocateTransferBuffer(
        UsbfnIo,
        UsbfnLbSm.LbTestData.Xfer.BufferSize,
        (void **)&UsbfnLbSm.LbTestData.Xfer.Buffer);
  } while (EFI_OUT_OF_RESOURCES == Status);

  // other allocation error
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to allocate loopback data transfer buffer: %r", Status);
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "Loopback buffer size: %d", UsbfnLbSm.LbTestData.Xfer.BufferSize);
  PRINT(APL_VVERB, "Loopback data transfer buffer = %p", UsbfnLbSm.LbTestData.Xfer.Buffer);

  // create timer to timeout of loopback state
  Status = gBS->CreateEvent(
      EVT_TIMER | EVT_NOTIFY_SIGNAL,
      TPL_CALLBACK,
      UlbSmLoopbackTimeout,
      NULL,
      &UsbfnLbSm.LbTestData.TimeoutTimer);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to create timer: %r", Status);
    goto ON_EXIT;
  }

  // set ZLT Endpoint Policy on bulk eps if desired and supported
  if (UsbfnZlt) {

    // query ZLT support for TX
    Buffer = 0;
    BufferSize = sizeof(Buffer);
    Status = UsbfnIo->GetEndpointPolicy(
        UsbfnIo,
        USBLB_BULK_EP,
        EfiUsbEndpointDirectionDeviceTx,
        EfiUsbPolicyZeroLengthTerminationSupport,
        &BufferSize,
        &Buffer);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to query ZLT support on bulk IN ep: %r", Status);
      goto ON_EXIT;
    }
    PRINT(APL_VERB, "ZLT support on bulk IN ep: 0x%x", Buffer);

    // enable ZLT for TX if supported
    if (TRUE == Buffer) {
      BufferSize = sizeof(BOOLEAN);
      Status = UsbfnIo->SetEndpointPolicy(
          UsbfnIo,
          USBLB_BULK_EP,
          EfiUsbEndpointDirectionDeviceTx,
          EfiUsbPolicyZeroLengthTermination,
          BufferSize,
          &Buffer);
      if (EFI_ERROR(Status)) {
        PRINT(APL_ERROR, "Failed to enable ZLT on bulk IN ep: %r", Status);
        goto ON_EXIT;
      }
      UsbfnLbSm.TxUsbfnZlt = TRUE;
      PRINT(APL_ALWAYS, "Using Usbfn driven ZLT on bulk IN ep");
    } else {
      PRINT(APL_ALWAYS, "Using application driven ZLT on bulk IN ep");
    }

    // query ZLT support for RX
    Buffer = 0;
    BufferSize = sizeof(Buffer);
    Status = UsbfnIo->GetEndpointPolicy(
        UsbfnIo,
        USBLB_BULK_EP,
        EfiUsbEndpointDirectionDeviceRx,
        EfiUsbPolicyZeroLengthTerminationSupport,
        &BufferSize,
        &Buffer);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to query ZLT support on bulk OUT ep: %r", Status);
      goto ON_EXIT;
    }
    PRINT(APL_VERB, "ZLT support on bulk OUT ep: 0x%x", Buffer);

    // enable ZLT for RX if supported
    if (TRUE == Buffer) {
      BufferSize = sizeof(BOOLEAN);
      Status = UsbfnIo->SetEndpointPolicy(
          UsbfnIo,
          USBLB_BULK_EP,
          EfiUsbEndpointDirectionDeviceRx,
          EfiUsbPolicyZeroLengthTermination,
          BufferSize,
          &Buffer);
      if (EFI_ERROR(Status)) {
        PRINT(APL_ERROR, "Failed to enable ZLT on bulk OUT ep: %r", Status);
        goto ON_EXIT;
      }
      UsbfnLbSm.RxUsbfnZlt = TRUE;
      PRINT(APL_ALWAYS, "Using Usbfn driven ZLT on bulk OUT ep");
    } else {
      PRINT(APL_ALWAYS, "Using application driven ZLT on bulk OUT ep");
    }
  } else {
    PRINT(APL_ALWAYS, "Using application driven ZLT on bulk eps");
  }

  // enter the initial state
  UsbfnLbSm.CurrentState = ULB_INITIAL_STATE;
  if (UsbfnLbSm.States[ULB_INITIAL_STATE].Entry != NULL) {
    UsbfnLbSm.States[ULB_INITIAL_STATE].Entry(&UsbfnLbSm, ULB_STATE_UNINITIALIZED);
  }

ON_EXIT:
  if (EFI_ERROR(Status)) {
    UlbSmCleanup();
  }
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
VOID
UlbSmCleanup (
  VOID
  )
{
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  // close the loopback timer
  gBS->CloseEvent(UsbfnLbSm.LbTestData.TimeoutTimer);

  // cancel and free control transfer
  if (NULL != UsbfnLbSm.CtrlXfer.Buffer) {

    Status = AbortXfer(UsbfnLbSm.UsbfnIo, &UsbfnLbSm.CtrlXfer);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to abort control transfer: %r", Status);
    }

    Status = UsbfnLbSm.UsbfnIo->FreeTransferBuffer(
        UsbfnLbSm.UsbfnIo,
        (void *)UsbfnLbSm.CtrlXfer.Buffer);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to free control transfer buffer: %r", Status);
    }
  }

  // cancel and free bulk transfer
  if (NULL != UsbfnLbSm.LbTestData.Xfer.Buffer) {

    Status = AbortXfer(UsbfnLbSm.UsbfnIo, &UsbfnLbSm.LbTestData.Xfer);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to abort loopback data transfer: %r", Status);
    }

    Status = UsbfnLbSm.UsbfnIo->FreeTransferBuffer(
        UsbfnLbSm.UsbfnIo,
        (void *)UsbfnLbSm.LbTestData.Xfer.Buffer);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "Failed to free loopback data transfer buffer: %r", Status);
    }
  }

  FNC_LEAVE_MSG();
}


/*
 * See header for documentation.
 */
EFI_STATUS
UlbSmProcessEvent (
  IN  EFI_USBFN_MESSAGE           Message,
  IN  UINTN                       PayloadSize,
  IN  EFI_USBFN_MESSAGE_PAYLOAD   *Payload
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  ULB_STATE   PrevState, NextState;

  FNC_ENTER_MSG();

  if (NULL == Payload || Message < ULB_EVENT_OFFSET) {
    PRINT(APL_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // make sure we have a valid event
  if (ULB_GET_EVENT_IDX(Message) >= ULB_NUM_EVENTS) {
    PRINT(APL_ERROR, "THIS SHOULDN'T HAPPEN!!");
    goto ON_EXIT;
  }
  PRINT(APL_VERB, "Processing event %a in state %a",
      UlbSmGetEventName(Message), UlbSmGetStateName(UsbfnLbSm.CurrentState));

  PrevState = UsbfnLbSm.CurrentState;
  if (PrevState >= ULB_NUM_STATES) {
    PRINT(APL_ERROR, "Invalid current state!");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  // get the next state from the current state's event function, (if it exists)
  if (UsbfnLbSm.States[PrevState].Event[ULB_GET_EVENT_IDX(Message)] != NULL) {
    NextState = UsbfnLbSm.States[PrevState].Event[ULB_GET_EVENT_IDX(Message)](
        &UsbfnLbSm,
        Message,
        PayloadSize,
        Payload);

    // check for no transition
    if ((NextState == PrevState) || (NextState == ULB_STATE_SAME)) {
      PRINT(APL_VERB, "Not changing state");
      goto ON_EXIT;
    }

    // check for valid next state (we already know it's not the current state)
    if (NextState >= ULB_NUM_STATES) {
      PRINT(APL_ERROR, "Invalid next state returned, not changing state!");
      Status = EFI_DEVICE_ERROR;
      goto ON_EXIT;
    }

    // update the current state first in case the entry function wants to use it
    UsbfnLbSm.CurrentState = NextState;

    // finally call the next state's entry function (if it exists)
    if (UsbfnLbSm.States[NextState].Entry != NULL) {
      UsbfnLbSm.States[NextState].Entry(&UsbfnLbSm, PrevState);
    }

    PRINT(APL_ALWAYS, "Transitioned from %a to %a",
        UlbSmGetStateName(PrevState), UlbSmGetStateName(NextState));
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
ULB_STATE
UlbSmGetCurrentState (
  VOID
  )
{
  return UsbfnLbSm.CurrentState;
}


/*
 * See header for documentation.
 */
CHAR8 CONST *
UlbSmGetStateName (
  IN  ULB_STATE               State
  )
{
  if (State >= ULB_NUM_STATES) {
    return NULL;
  }

  return UsbfnLbSm.States[State].Name;
}


/*
 * See header for documentation.
 */
CHAR8 CONST *
UlbSmGetEventName (
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

