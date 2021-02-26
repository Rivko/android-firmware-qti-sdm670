/**
 * @defgroup CtrlSm Control Transfer State Machine
 *
 * @brief State machine to manage the flow of control transfers
 *
 * Implementation of a state machine to manage the control transfer programming
 * model as described in the Synopsys Databook, section 8.4.
 */
/**
 * @file UsbfnCtrlSm.h
 *
 * @ingroup CtrlSm
 *
 * @brief Control transfer state machine for Synopsys core
 *
 * This file defines the external interface to a state machine that manages the
 * control transfers for the Synopsys core as defined in the Synopsys Databook,
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

#ifndef _USBFN_CTRL_SM_H_
#define _USBFN_CTRL_SM_H_

// for event structures
#include "UsbfnDwc3.h"

/**
 * @brief State machine event enum which maps to common layer events
 */
typedef enum _USM_EVENT {
  USM_XFER_COMPLETE_EV, ///< XferComplete Endpoint Event
  USM_XFER_NOTREADY_EV, ///< XferNotReady Endpoint Event
  USM_RESET_EV,         ///< USB Reset Device Event
  USM_DISCONNECT_EV,    ///< Disconnect Device Event
  USM_CMD_COMPLETE_EV,  ///< Endpoint Command Complete Event
  USM_SET_STALL_EV,     ///< Client initiated stall
  USM_NUM_EVENTS        ///< Number of events
} USM_EVENT;


/**
 * @brief Control state machine states
 */
typedef enum _USM_STATE {

  // Actual state machine states
  USM_STATE_CTRL_SETUP,     ///< Control Setup State
  USM_STATE_WAIT_HOST_2,    ///< 2-Stage Wait for Host State
  USM_STATE_CTRL_STATUS_2,  ///< 2-Stage Status State
  USM_STATE_CTRL_DATA,      ///< Control Data State
  USM_STATE_WAIT_HOST_3,    ///< 3-Stage Wait for Host State
  USM_STATE_CTRL_STATUS_3,  ///< 3-Stage Status State
  USM_STATE_SET_STALL,      ///< Stall State
  USM_STATE_BYPASS_STATUS,  ///< Bypass Status State

  USM_NUM_STATES,           ///< Number of states

  // Meta-states for state management
  USM_STATE_UNINITIALIZED,  ///< Invalid state to transition from on initialization
  USM_STATE_SAME,           ///< Indicates no state change when returned from transition function

  USM_INITIAL_STATE = USM_STATE_CTRL_SETUP  ///< State to enter on initialization

} USM_STATE;




/*
 * Forward declaration of state machine structures for state functions
 */
typedef struct _USM_STATE_MACHINE USM_STATE_MACHINE;
typedef struct _USM_STATE_DATA    USM_STATE_DATA;


/**
 * @brief State entry function prototype
 *
 * A state's entry function is called upon entry into that state.  If a state
 * is to have an entry function (they are allowed to be NULL), it must be of
 * this type.
 *
 * @param Usm           Usbfn State Machine instance
 * @param PrevState     State we are coming from
 */
typedef VOID
(*USM_STATE_ENTRY_FUNC) (
  IN  USM_STATE_MACHINE       *Usm,
  IN  USM_STATE               PrevState
  );


/**
 * @brief State event function prototype
 *
 * Each state has a pointer to a function of this type for each of the defined
 * events.  The event function of a state is invoked on recption of the event
 * when in that state and is responsible for returning the next state to
 * transition to.  Event functions are allowed to be NULL, and if they are a
 * state transition will not occur for that event in that state.  The same
 * event function can be assigned to multiple events and/or to multiple states.
 *
 * Event functions for RESET and DISCONNECT events only need to perform state
 * specific actions (ex. end transfer and transition to CTRL_SETUP).  Actions
 * that need to be performed on RESET or DISCONNECT that are common to all
 * states are performed outside the state machine implementation.
 *
 * @param Usm           Usbfn State Machine instance
 * @param Evt           Common layer event structure
 *
 * @return Next state to transition into
 */
typedef USM_STATE
(*USM_STATE_EVENT_FUNC) (
  IN  USM_STATE_MACHINE       *Usm,
  IN  DWC_EVT                 *Evt
  );


/**
 * @brief State data structure
 *
 * Contains name of state and pointers to state functions.
 */
struct _USM_STATE_DATA {
  CHAR8 CONST                 *Name;
  USM_STATE                   State;
  USM_STATE_ENTRY_FUNC        Entry;
  USM_STATE_EVENT_FUNC        Event[USM_NUM_EVENTS];
};


/**
 * @brief State Machine structure
 *
 * Contains array of states and data used only by the state machine functions.
 */
struct _USM_STATE_MACHINE {

  // state data
  USM_STATE_DATA              States[USM_NUM_STATES];
  USM_STATE                   CurrentState;

  // state machine data
  USBFN_DEV                   *Usbfn;
  BOOLEAN                     HasDataStage;
  DWC_ENDPOINT_DIR            DataDir;
  BOOLEAN                     DataStageSubmitted;
};



/**
 * @brief Initialize the state machine
 *
 * Initialize the state machine structure and call the initial
 * state's entry function.
 *
 * @param Usbfn         USBFN_DEV instance (to use in state functions)
 *
 * @return EFI_SUCCESS  State machine ready to process events
 * @return Others       An error occurred preventing initialization
 */
EFI_STATUS
UsmInitialize (
  IN  USBFN_DEV               *Usbfn
  );


/**
 * @brief Send an external event to the state machine
 *
 * Advance the state machine based on the event.  If the current state does not
 * have an event function for the current event, no action is taken.  If the
 * event function exists it will be called and return the next state.  If the
 * returned next state is the current state or the USM_STATE_SAME value, no
 * further action is taken (ie. the current state's entry function is not
 * invoked again).  If the returned state is not the same, its entry function is
 * called if it exists.
 *
 * @param Evt           The common layer event to process
 *
 * @return EFI_SUCCESS  The event was successfully processed
 * @return Others       An error occurred processing the event
 */
EFI_STATUS
UsmProcessEvent (
  IN  DWC_EVT                 *Evt
  );


/**
 * @brief Externally originated stall on control endpoint
 *
 * Advance the state machine according to a stall condition generated by the
 * client driver's processing of a setup packet or data stage control transfer.
 */
EFI_STATUS
UsmExternalSetStall (
  VOID
  );


/**
 * @brief Get the current state
 *
 * @return Current state (member of USM_EVENT enumeration)
 */
USM_STATE
UsmGetCurrentState (
  VOID
  );


/**
 * @brief Get the name of a state
 *
 * @param State         USM_STATE to retrieve name of
 *
 * @return ASCII string representation of State
 */
CHAR8 CONST *
UsmGetStateName (
  IN  USM_STATE               State
  );


/**
 * @brief Get the name of an event
 *
 * @param Event         USM_EVENT to retrieve name of
 *
 * @return ASCII string representation of Event
 */
CHAR8 CONST *
UsmGetEventName (
  IN  USM_EVENT               Event
  );


#endif /* _USBFN_CTRL_SM_H_ */
