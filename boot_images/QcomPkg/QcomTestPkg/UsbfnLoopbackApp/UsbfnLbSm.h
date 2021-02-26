/**
 * @file UsbfnLbSm.h
 *
 * @brief Loopback USB device state machine
 *
 * This file defines the external interface to a state machine that manages the
 * USB loopback device and transfer states.
 *
 * Copyright (c) 2014 by QUALCOMM Technologies, Inc. All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/20/14   ml      Migrate to official SuperSpeed support
 02/06/14   ml      Initial implementation
 =============================================================================*/

#ifndef _USBFN_LB_SM_H_
#define _USBFN_LB_SM_H_


#include "UsbfnLoopback.h"


/**
 * @brief Number of events supported by the state machine
 *
 * The Usbfn Loopback state machine uses all the UsbfnIo event handler messages
 * except the "None" message to advance the state machine. Note that this
 * introduces an offset of 1 between the event handler functions and the
 * message value. The following events are thus supported.
 *
 *   EfiUsbMsgSetupPacket
 *   EfiUsbMsgEndpointStatusChangedRx
 *   EfiUsbMsgEndpointStatusChangedTx
 *   EfiUsbMsgBusEventDetach
 *   EfiUsbMsgBusEventAttach
 *   EfiUsbMsgBusEventReset
 *   EfiUsbMsgBusEventSuspend
 *   EfiUsbMsgBusEventResume
 *   EfiUsbMsgBusEventSpeed
 */
#define ULB_NUM_EVENTS    EfiUsbMsgBusEventSpeed
#define ULB_EVENT_OFFSET  1
#define ULB_GET_EVENT_IDX(msg)    ((msg) - ULB_EVENT_OFFSET)


/**
 * @brief Usbfn Loopback states
 */
typedef enum ULB_STATE {

  // Actual state machine states
  ULB_STATE_DETACHED,           ///< Cable detached
  ULB_STATE_ATTACHED,           ///< Cable attached
  ULB_STATE_DEFAULT_ADDRESSED,  ///< USB default/addressed states
  ULB_STATE_CONFIGURED,         ///< Device configured
  ULB_STATE_LOOPBACK,           ///< Loopback test active
  ULB_STATE_SUSPENDED,          ///< Bus suspended

  ULB_NUM_STATES,               ///< Number of states

  // Meta-states for state management
  ULB_STATE_UNINITIALIZED,      ///< Invalid state to transition from on initialization
  ULB_STATE_SAME,               ///< Indicates no state change when returned from transition function

  ULB_INITIAL_STATE = ULB_STATE_DETACHED  ///< State to enter on initialization

} ULB_STATE;


/*
 * Forward declaration of state machine structures for state functions
 */
typedef struct ULB_STATE_MACHINE ULB_STATE_MACHINE;
typedef struct ULB_STATE_DATA    ULB_STATE_DATA;


/**
 * @brief State entry function prototype
 *
 * A state's entry function is called upon entry into that state.  If a state
 * is to have an entry function (they are allowed to be NULL), it must be of
 * this type.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] PrevState     State we are coming from
 */
typedef VOID
(*ULB_STATE_ENTRY_FUNC) (
  IN  ULB_STATE_MACHINE       *Ulb,
  IN  ULB_STATE               PrevState
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
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state to transition into
 */
typedef ULB_STATE
(*ULB_STATE_EVENT_FUNC) (
  IN  ULB_STATE_MACHINE           *Ulb,
  IN  EFI_USBFN_MESSAGE           Message,
  IN  UINTN                       PayloadSize,
  IN  EFI_USBFN_MESSAGE_PAYLOAD   *Payload
  );


/**
 * @brief State data structure
 *
 * Contains name of state and pointers to state functions.
 */
struct ULB_STATE_DATA {
  CHAR8 CONST                 *Name;
  ULB_STATE                   State;
  ULB_STATE_ENTRY_FUNC        Entry;
  ULB_STATE_EVENT_FUNC        Event[ULB_NUM_EVENTS];
};


/**
 * @brief State Machine structure
 *
 * Contains array of states and data used only by the state machine functions.
 */
struct ULB_STATE_MACHINE {

  // state data
  ULB_STATE_DATA                  States[ULB_NUM_STATES];
  ULB_STATE                       CurrentState;

  // state machine data
  EFI_USBFN_IO_PROTOCOL           *UsbfnIo;
  EFI_USB_DEVICE_INFO             *DeviceInfo;
  EFI_USB_SUPERSPEED_DEVICE_INFO  *SSDeviceInfo;
  BOOLEAN                         RxUsbfnZlt;
  BOOLEAN                         TxUsbfnZlt;
  UINTN                           MaxTransferSize;
  UINT16                          UsbDevStatus;
  EFI_USBFN_PORT_TYPE             PortType;
  EFI_USB_BUS_SPEED               BusSpeed;
  UINT8                           BusAddress;
  ULB_STATE                       ResumeState;
  XFER_TYPE                       CtrlXfer;
  BOOLEAN                         RcvLbTestConfig;
  LB_TEST_CONFIG                  LbTestConfig;
  LB_TEST_DATA                    LbTestData;
};


/**
 * @brief Global loopback state machine instance
 */
extern ULB_STATE_MACHINE UsbfnLbSm;


/**
 * @brief Initialize the state machine
 *
 * Initialize the state machine structure and call the initial
 * state's entry function.
 *
 * @param[in] UsbfnIo       UsbfnIo protocol instance
 * @param[in] DeviceInfo    Loopback USB 2.0 descriptor tree
 * @param[in] SSDeviceInfo  Loopback USB 3.0 descriptor tree
 * @param[in] UsbfnZlt      TRUE:  Use Usbfn Endpoint Policy ZLT
 *                          FALSE: Use apllication driven ZLT
 *
 * @return EFI_SUCCESS      State machine ready to process events
 * @return Others           An error occurred preventing initialization
 */
EFI_STATUS
UlbSmInitialize (
  IN EFI_USBFN_IO_PROTOCOL          *UsbfnIo,
  IN EFI_USB_DEVICE_INFO            *DeviceInfo,
  IN EFI_USB_SUPERSPEED_DEVICE_INFO *SSDeviceInfo,
  IN BOOLEAN                        UsbfnZlt
  );


/**
 * @brief Cleanup the state machine
 *
 * Stop any outstanding operations and free any memory allocated.
 */
VOID
UlbSmCleanup (
  VOID
  );


/**
 * @brief Send a UsbfnIo event to the state machine
 *
 * Advance the state machine based on the event.  If the current state does not
 * have an event function for the current event, no action is taken.  If the
 * event function exists it will be called and return the next state.  If the
 * returned next state is the current state or the ULB_STATE_SAME value, no
 * further action is taken (i.e. the current state's entry function is not
 * invoked again).  If the returned state is not the same, its entry function is
 * called if it exists.
 *
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return EFI_SUCCESS  The event was successfully processed
 * @return Others       An error occurred processing the event
 */
EFI_STATUS
UlbSmProcessEvent (
  IN  EFI_USBFN_MESSAGE           Message,
  IN  UINTN                       PayloadSize,
  IN  EFI_USBFN_MESSAGE_PAYLOAD   *Payload
  );


/**
 * @brief Get the current state
 *
 * @return Current state (member of ULB_EVENT enumeration)
 */
ULB_STATE
UlbSmGetCurrentState (
  VOID
  );


/**
 * @brief Get the name of a state
 *
 * @param[in] State         ULB_STATE to retrieve name of
 *
 * @return ASCII string representation of State
 */
CHAR8 CONST *
UlbSmGetStateName (
  IN  ULB_STATE               State
  );


/**
 * @brief Get the name of an event
 *
 * @param[in] Msg           EFI_USBFN_MESSAGE value to retrieve name of
 *
 * @return ASCII string representation of Msg
 */
CHAR8 CONST *
UlbSmGetEventName (
  IN  EFI_USBFN_MESSAGE       Msg
  );


#endif /* _USBFN_LB_SM_H_ */
