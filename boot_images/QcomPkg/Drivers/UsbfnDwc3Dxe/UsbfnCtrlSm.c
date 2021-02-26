/**
 * @file UsbfnCtrlSm.c
 *
 * @ingroup CtrlSm
 *
 * @brief Synopsys core control transfer state machine external implementation
 *
 * This file implements the external interface to the control state machine
 * which manages control transfers as defined by the Synopsys Databook,
 * section 8.4.
 *
 * @copyright Copyright (c) 2013-2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/12/14   ck      Address pending setup pkt and End Transfer issues
 06/10/13   mliss   Initial implementation
 =============================================================================*/

#include "UsbfnCtrlSm.h"
#include "UsbfnCtrlSmImpl.h"
#include "UsbfnDwc3.h"
#include "UsbfnDwc3Util.h"


/**
 * @brief The usbfn control state machine instance.
 *
 * Definition of the state machine structure and each state's name and function
 * pointer mapping.  The state definitions here must be in the same order as
 * the state enum and the event functions in each state must be in the same
 * order as the event enum.
 */
USM_STATE_MACHINE   UsbfnCtrlSm = {
  // State array
  {
    {
      "USM_STATE_CTRL_SETUP",
      USM_STATE_CTRL_SETUP,
      Usm_CtrlSetup_Entry,
      {
        Usm_CtrlSetup_XferComplete,
        Usm_CtrlSetup_XferNotReady,
        Usm_EventNotHandled,
        Usm_EventNotHandled,
        Usm_EventNotHandled,
        Usm_EventNotHandled
      }
    },
    {
      "USM_STATE_WAIT_HOST_2",
      USM_STATE_WAIT_HOST_2,
      Usm_WaitHost2_Entry,
      {
        Usm_WaitHost2_XferComplete,
        Usm_WaitHost2_XferNotReady,
        Usm_Stall_Reset,
        Usm_NoAction_Disconnect,
        Usm_EventNotHandled,
        Usm_ExternalStall
      }
    },
    {
      "USM_STATE_CTRL_STATUS_2",
      USM_STATE_CTRL_STATUS_2,
      Usm_CtrlStatus2_Entry,
      {
        Usm_CtrlStatus2_XferComplete,
        Usm_CtrlStatus2_XferNotReady,
        Usm_CtrlStatus2_Reset_Disconnect,
        Usm_CtrlStatus2_Reset_Disconnect,
        Usm_EventNotHandled,
        Usm_ExternalStall
      }
    },
    {
      "USM_STATE_CTRL_DATA",
      USM_STATE_CTRL_DATA,
      Usm_CtrlData_Entry,
      {
        Usm_CtrlData_XferComplete,
        Usm_CtrlData_XferNotReady,
        Usm_CtrlData_Reset_Disconnect,
        Usm_CtrlData_Reset_Disconnect,
        Usm_EventNotHandled,
        Usm_ExternalStall
      }
    },
    {
      "USM_STATE_WAIT_HOST_3",
      USM_STATE_WAIT_HOST_3,
      Usm_WaitHost3_Entry,
      {
        Usm_WaitHost3_XferComplete,
        Usm_WaitHost3_XferNotReady,
        Usm_Stall_Reset,
        Usm_NoAction_Disconnect,
        Usm_EventNotHandled,
        Usm_ExternalStall
      }
    },
    {
      "USM_STATE_CTRL_STATUS_3",
      USM_STATE_CTRL_STATUS_3,
      Usm_CtrlStatus3_Entry,
      {
        Usm_CtrlStatus3_XferComplete,
        Usm_CtrlStatus3_XferNotReady,
        Usm_CtrlStatus3_Reset_Disconnect,
        Usm_CtrlStatus3_Reset_Disconnect,
        Usm_EventNotHandled,
        Usm_ExternalStall
      }
    },
    {
      "USM_STATE_SET_STALL",
      USM_STATE_SET_STALL,
      Usm_SetStall_Entry,
      {
        Usm_EventNotHandled,
        Usm_EventNotHandled,
        Usm_SetStall_Reset,
        Usm_NoAction_Disconnect,
        Usm_SetStall_CmdComplete,
        Usm_EventNotHandled
      }
    },
    {
        "USM_STATE_BYPASS_STATUS",
        USM_STATE_BYPASS_STATUS,
        Usm_BypassSts_Entry,
        {
            Usm_EventNotHandled,
            Usm_EventNotHandled,
            Usm_EventNotHandled,
            Usm_EventNotHandled,
            Usm_BypassSts_Handle_PendingSetup,
            Usm_EventNotHandled
        }
    },
  },
  // Current State
  USM_STATE_UNINITIALIZED,

  // USBFN Device pointer
  NULL,
  FALSE,
  DWC_EP_DIR_OUT,
  FALSE
};

/**
 * @brief Worker function to process a state machine event.
 *
 * Function to process all state machine events, invoked internally by
 * external functions UsmProcessEvent and UsmExternalSetStall.
 *
 * @param [in]  UsmEvt        The state machine event to process
 * @param [in]  Evt           The common layer event, if any
 *
 * @return EFI_SUCCESS  The event was successfully processed
 * @return others       An error occurred processing the event
 */
STATIC EFI_STATUS
UsmProcessEventWorker (
  IN  USM_EVENT             UsmEvt,
  IN  DWC_EVT               *Evt
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  USM_STATE   PrevState, NextState;

  FNC_ENTER_MSG();

  // make sure we have a valid event
  if (UsmEvt >= USM_NUM_EVENTS) {
    DBG(EFI_D_ERROR, "THIS SHOULDN'T HAPPEN");
  }
  DBG(EFI_D_INFO, "Processing event %a in state %a",
      UsmGetEventName(UsmEvt), UsmGetStateName(UsbfnCtrlSm.CurrentState));

  PrevState = UsbfnCtrlSm.CurrentState;
  if (PrevState >= USM_NUM_STATES) {
    DBG(EFI_D_ERROR, "Invalid current state!");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  // get the next state from the current state's event function, (if it exists)
  if (UsbfnCtrlSm.States[PrevState].Event[UsmEvt] != NULL) {
    NextState = UsbfnCtrlSm.States[PrevState].Event[UsmEvt](&UsbfnCtrlSm, Evt);

    // check for no transition
    if ((NextState == PrevState) || (NextState == USM_STATE_SAME)) {
      DBG(EFI_D_INFO, "Not changing state");
      goto ON_EXIT;
    }

    // check for valid next state (we already know it's not the current state)
    if (NextState >= USM_NUM_STATES) {
      DBG(EFI_D_ERROR, "Invalid next state returned, not changing state!");
      Status = EFI_DEVICE_ERROR;
      goto ON_EXIT;
    }

    // update the current state first in case the entry function wants to use it
    UsbfnCtrlSm.CurrentState = NextState;

    // finally call the next state's entry function (if it exists)
    if (UsbfnCtrlSm.States[NextState].Entry != NULL) {
      UsbfnCtrlSm.States[NextState].Entry(&UsbfnCtrlSm, PrevState);
    }

    DBG(EFI_D_INFO, "Transitioned from %a to %a",
        UsmGetStateName(PrevState), UsmGetStateName(NextState));
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * External functions for managing the state machine
 */

/*
 * See header for documentation.
 */
EFI_STATUS
UsmInitialize (
  IN  USBFN_DEV               *Usbfn
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  FNC_ENTER_MSG();
  if (NULL == Usbfn) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  // initialize state machine data
  UsbfnCtrlSm.Usbfn              = Usbfn;
  UsbfnCtrlSm.HasDataStage       = FALSE;
  UsbfnCtrlSm.DataDir            = DWC_EP_DIR_IN;

  // enter the initial state
  UsbfnCtrlSm.CurrentState = USM_INITIAL_STATE;
  if (UsbfnCtrlSm.States[USM_INITIAL_STATE].Entry != NULL) {
    UsbfnCtrlSm.States[USM_INITIAL_STATE].Entry(&UsbfnCtrlSm, USM_STATE_UNINITIALIZED);
  }

ON_EXIT:
  FNC_LEAVE_MSG()
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsmProcessEvent (
  IN  DWC_EVT                 *Evt
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  USM_EVENT   UsmEvt = USM_NUM_EVENTS;

  FNC_ENTER_MSG();
  if (NULL == Evt) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  // translate common layer event into USM_EVENT
  if (Evt->Template.IsDeviceEvent) {
    switch (Evt->Device.EventType) {
      case DWC_EVT_DEV_DISCONNECT_EVENT:
        UsmEvt = USM_DISCONNECT_EV;
        break;
      case DWC_EVT_DEV_USB_RESET:
        UsmEvt = USM_RESET_EV;
        break;
      default:
        // warn, but don't return error
        DBG(EFI_D_WARN, "Unhandled DWC device event: %d", Evt->Device.EventType);
        goto ON_EXIT;
    }
  } else {
    switch (Evt->Endpoint.EventType) {
      case DWC_EVT_EP_XFER_COMPLETE:
        UsmEvt = USM_XFER_COMPLETE_EV;
        break;
      case DWC_EVT_EP_XFER_NOT_READY:
        UsmEvt = USM_XFER_NOTREADY_EV;
        break;
      case DWC_EVT_EP_EP_CMD_COMPLETE:
        UsmEvt = USM_CMD_COMPLETE_EV;
        break;
      default:
        // warn, but don't return error
        DBG(EFI_D_WARN, "Unhandled DWC endpoint event: %d", Evt->Endpoint.EventType);
        goto ON_EXIT;
    }
  }

  // call the worker to process the event
  Status = UsmProcessEventWorker(UsmEvt, Evt);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
UsmExternalSetStall (
  VOID
  )
{
  return UsmProcessEventWorker(USM_SET_STALL_EV, NULL);
}

// For CLANG, the inline function usage is documented here:
// http://clang.llvm.org/compatibility.html#inline
// In our case, the functions are not inlined, and it causes
// the missing symbols.

/*
 * See header for documentation.
 */
USM_STATE
UsmGetCurrentState (
  VOID
  )
{
  return UsbfnCtrlSm.CurrentState;
}


/*
 * See header for documentation.
 */
CHAR8 CONST *
UsmGetStateName (
  IN  USM_STATE               State
  )
{
  if (State >= USM_NUM_STATES) {
    return NULL;
  }

  return UsbfnCtrlSm.States[State].Name;
}


/*
 * See header for documentation.
 */
CHAR8 CONST *
UsmGetEventName (
  IN  USM_EVENT               Event
  )
{
  switch (Event) {
    case USM_XFER_COMPLETE_EV:
      return "USM_XFER_COMPLETE_EV";
    case USM_XFER_NOTREADY_EV:
      return "USM_XFER_NOTREADY_EV";
    case USM_RESET_EV:
      return "USM_RESET_EV";
    case USM_DISCONNECT_EV:
      return "USM_DISCONNECT_EV";
    case USM_CMD_COMPLETE_EV:
      return "USM_CMD_COMPLETE_EV";
    case USM_SET_STALL_EV:
      return "USM_SET_STALL_EV";
    default:
      return "Unknown Event";
  }
}
