/**
 * @file UsbMsdSm.h
 *
 * @brief USB device state machine
 *
 * This file defines the external interface to a state machine that manages the
 * USB device and transfer states.
 *
 * Copyright (c) 2014,2016 by Qualcomm Technologies, Inc. All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/25/16   ck      Separate EDK2 undefined USB data structure to EFIUsbEx.h
 10/14/14   ck      Initial Version. Introduce state machine and remove global vars
 =============================================================================*/

#ifndef __USBE_MSD_SM_H__
#define __USBE_MSD_SM_H__

#include <Protocol/EFIUsbfnIo.h> // USBfnIo protocol handle
#include <Protocol/EFIUsbEx.h>   // USB data structure
#include "UsbMsdXfer.h"          // XFER_TYPE

/**
 * @brief Number of events supported by the state machine
 *
 * The Usbfn Device state machine uses all the UsbfnIo event handler messages
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
#define DEV_NUM_EVENTS                      EfiUsbMsgBusEventSpeed
#define DEV_EVENT_OFFSET                    1
#define DEV_GET_EVENT_IDX(msg)              ((msg) - DEV_EVENT_OFFSET)

/**
* @brief max allowed usb device serial number size in bytes
*/
#define USB_MAX_SERIAL_NUMBER_SIZE_IN_BYTES 128


/**
 * @brief Usbfn Device states
 */
typedef enum DEV_STATE {

  // Actual state machine states
  DEV_STATE_DETACHED,           ///< Cable detached
  DEV_STATE_ATTACHED,           ///< Cable attached
  DEV_STATE_DEFAULT_ADDRESSED,  ///< USB default/addressed states
  DEV_STATE_CONFIGURED,         ///< Device configured
  DEV_STATE_SUSPENDED,          ///< Bus suspended
  DEV_STATE_CBW,
  DEV_STATE_SEND_TO_HOST,
  DEV_STATE_RCV_FROM_HOST,
  DEV_STATE_CSW,

  DEV_NUM_STATES,               ///< Number of states

  // Meta-states for state management
  DEV_STATE_UNINITIALIZED,      ///< Invalid state to transition from on initialization
  DEV_STATE_SAME,               ///< Indicates no state change when returned from transition function

  DEV_INITIAL_STATE = DEV_STATE_DETACHED  ///< State to enter on initialization

} DEV_STATE;


/*
 * Forward declaration of state machine structures for state functions
 */
typedef struct DEV_STATE_MACHINE DEV_STATE_MACHINE;
typedef struct DEV_STATE_DATA    DEV_STATE_DATA;


/**
 * @brief State entry function prototype
 *
 * A state's entry function is called upon entry into that state.  If a state
 * is to have an entry function (they are allowed to be NULL), it must be of
 * this type.
 *
 * @param[in] DevSm         Usbfn Device state machine instance
 * @param[in] PrevState     State we are coming from
 */
typedef VOID
(*DEV_STATE_ENTRY_FUNC) (
  IN  DEV_STATE_MACHINE       *DevSm,
  IN  DEV_STATE               PrevState
  );


/**
 * @brief State event function prototype
 *
 * Each state has a pointer to a function of this type for each of the defined
 * events.  The event function of a state is invoked on reception of the event
 * when in that state and is responsible for returning the next state to
 * transition to.  Event functions are allowed to be NULL, and if they are a
 * state transition will not occur for that event in that state.  The same
 * event function can be assigned to multiple events and/or to multiple states.
 *
 * @param[in] DevSm         Usbfn Device state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @retval Next state to transition into
 */
typedef DEV_STATE
(*DEV_STATE_EVENT_FUNC) (
  IN  DEV_STATE_MACHINE          *DevSm,
  IN  EFI_USBFN_MESSAGE           Message,
  IN  UINTN                       PayloadSize,
  IN  EFI_USBFN_MESSAGE_PAYLOAD  *Payload
  );


/**
 * @brief State data structure
 *
 * Contains name of state and pointers to state functions.
 */
struct DEV_STATE_DATA {
  CHAR8 CONST                 *Name;
  DEV_STATE                   State;
  DEV_STATE_ENTRY_FUNC        Entry;
  DEV_STATE_EVENT_FUNC        Event[DEV_NUM_EVENTS];
};


/**
 * @brief State Machine structure
 *
 * Contains array of states and data used only by the state machine functions.
 */
struct DEV_STATE_MACHINE {
  // state data
  DEV_STATE_DATA                         States[DEV_NUM_STATES];
  DEV_STATE                              CurrentState;
  DEV_STATE                              PreviousState;
  // state machine data
  EFI_USBFN_IO_PROTOCOL                 *UsbfnIo;
  UINT16                                 UsbDevStatus;
  EFI_USBFN_PORT_TYPE                    PortType;
  EFI_USB_BUS_SPEED                      BusSpeed;
  UINT8                                  BusAddress;
  DEV_STATE                              ResumeState;
  UINT8                                  SerialNumber[USB_MAX_SERIAL_NUMBER_SIZE_IN_BYTES];
  UINTN                                  SerialNumberLen;
  BOOLEAN                                RcvcMsdClsReq;
  // transfers
  XFER_TYPE                              CtrlXfer;    // Control
  XFER_TYPE                              BulkRxXfer;  // Rx Buff
  XFER_TYPE                              BulkTxXfer;  // Tx Buffer
  XFER_TYPE                              CBW;         // Command Block Wrapper
  XFER_TYPE                              CSW;         // Command Status Wrapper
};


/**
 * @brief Initialize the state machine
 *
 * Initialize the state machine structure and call the initial
 * state's entry function.
 *
 * @param[in] DevSm         Usbfn Device state machine instance
 * @param[in] UsbfnIo       UsbfnIo protocol instance
 *
 * @retval EFI_SUCCESS      State machine ready to process events
 * @retval Others           An error occurred preventing initialization
 */
EFI_STATUS
DevSmInitialize (
  IN DEV_STATE_MACHINE                 *DevSm,
  IN EFI_USBFN_IO_PROTOCOL             *UsbfnIo
  );


/**
 * @brief Cleanup the state machine
 *
 * @param[in] DevSm         Usbfn Device state machine instance
 * 
 * Free any memory allocated.
 */
VOID
DevSmCleanup (
  IN DEV_STATE_MACHINE *DevSm
  );


/**
 * @brief Send a UsbfnIo event to the state machine
 *
 * Advance the state machine based on the event.  If the current state does not
 * have an event function for the current event, no action is taken.  If the
 * event function exists it will be called and return the next state.  If the
 * returned next state is the current state or the DEV_STATE_SAME value, no
 * further action is taken (i.e. the current state's entry function is not
 * invoked again).  If the returned state is not the same, its entry function is
 * called if it exists.
 *
 * @param[in] DevSm         Usbfn Device state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @retval EFI_SUCCESS  The event was successfully processed
 * @retval Others       An error occurred processing the event
 */
EFI_STATUS
DevSmProcessEvent (
  IN  DEV_STATE_MACHINE          *DevSm,
  IN  EFI_USBFN_MESSAGE           Message,
  IN  UINTN                       PayloadSize,
  IN  EFI_USBFN_MESSAGE_PAYLOAD  *Payload
  );


/**
 * @brief Get the current state
 *
 * @param[in] DevSm         Usbfn Device state machine instance
 *
 * @retval Current state (member of DEV_EVENT enumeration)
 */
DEV_STATE
DevSmGetCurrentState (
  IN DEV_STATE_MACHINE *DevSm
  );


/**
 * @brief Get the name of a state
 *
 * @param[in] DevSm         Usbfn Device state machine instance
 * @param[in] State         DEV_STATE to retrieve name of
 *
 * @retval ASCII string representation of State
 */
CHAR8 CONST *
DevSmGetStateName (
  IN  DEV_STATE_MACHINE      *DevSm,
  IN  DEV_STATE               State
  );


/**
 * @brief Get the name of an event
 *
 * @param[in] Msg           EFI_USBFN_MESSAGE value to retrieve name of
 *
 * @retval ASCII string representation of Msg
 */
CHAR8 CONST *
DevSmGetEventName (
  IN  EFI_USBFN_MESSAGE       Msg
  );

#endif /* __USBE_MSD_SM_H__ */
