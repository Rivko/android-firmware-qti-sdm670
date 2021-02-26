/** @file \UsbConfigUsbPortsPublic.h

  The public header file for UsbConfigUsbCore.c
  Copyright (c) 2016-2017 QUALCOMM Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 02/03/17   pm       Port from Boot.xf.1.4
 02/18/16   ck       Init check-in
=============================================================================*/

#ifndef _USB_CONFIG_USB_PORTS_PUB_H_
#define _USB_CONFIG_USB_PORTS_PUB_H_

#include "UsbConfigLibPublic.h"

typedef struct _QCOM_USB_PORT_INTERFACE QCOM_USB_PORT_INTERFACE;

//
// The possible USB Type-C port mode.
// 
typedef enum _USB_TYPEC_PORT_MODE
{
  USB_TYPEC_PORT_MODE_NONE,
  USB_TYPEC_PORT_MODE_UFP,
  USB_TYPEC_PORT_MODE_DFP,
  USB_TYPEC_PORT_MODE_INVALID
} USB_TYPEC_PORT_MODE;


//
// The possible USB Type-C CC out status.
// 
typedef enum _USB_TYPEC_CC_OUT_STATUS
{
  USB_TYPEC_CC_OUT_RESERVED,
  USB_TYPEC_CC_OUT_CC1,
  USB_TYPEC_CC_OUT_CC2,
  USB_TYPEC_CC_OUT_OPEN,
  USB_TYPEC_CC_OUT_INVALID
} USB_TYPEC_CC_OUT_STATUS;

// 
// Payload for the current message.
//
typedef union _USB_TYPEC_PORT_MESSAGE_PAYLOAD
{
  USB_TYPEC_PORT_MODE UsbTypeCPortMode;
} USB_TYPEC_PORT_MESSAGE_PAYLOAD;

// 
// Event message.
// 
typedef enum _USB_CFG_PORT_MESSAGE
{
  UsbCfgPortMsgNone = 0,
  /**< No messages. */
  UsbCfgPortMsgPartnerDetach,
  /**< Detach event was signaled. */
  UsbCfgPortMsgPartnerAttach,
  /**< Attach event was signaled. */
} /** @cond */USB_CFG_PORT_MESSAGE/** @endcond */;

//
// USB Type-C Port Connection State
//
typedef enum {
  USBC_STATE_INVALID = 0,   // Invalid Connection State
  USBC_STATE_DETACHED,      // Detached Connection State
  USBC_STATE_ATTACHED,      // Attached connection State
  USBC_STATE_MAX
} USB_CFG_PORT_CONNECT_STATE;

/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

/**
@brief Initialize a USB Type-C port specified by the CoreNum.

The client shall call the function to get the USB port handle before calling 
any other public interfaces exposed by the module. 

@param [in]  CoreNum          The core number associated with the port to be 
                              initialized. 
@param [out] UsbPortHandle    The USB port handle associated with the port

@retval EFI_SUCCESS           The operation succeeded.
@retval Others                The operation failed.
**/
EFI_STATUS
UsbConfigPortsInit(
  IN  UINTN      CoreNum,
  OUT VOID     **UsbPortHandle
  );

  
/**
@brief De-initialize a USB Type-C port.

After calling the function, the client shall not invoke any other APIs 
exposed by the module. 

@param [in] UsbPortHandle    The USB port handle associated with the port

@retval EFI_SUCCESS           The operation succeeded.
@retval Others                The operation failed.
**/
EFI_STATUS
UsbConfigPortsDeinit(
  IN VOID     *UsbPortHandle
  );

  
/**
@brief Event handler function for a USB port.

The client calls the function periodically to obtain the new Type-C related events 
associated with the USB port. 

@param [in]     UsbPort         The handle of the port
@param [out]    Message         The message generated based on the event
@param [in,out] PayloadSize     The size of Payload associated with the message
@param [out]    Payload         The payload associated with the message.

@retval EFI_SUCCESS       The operation succeeded.
@retval Others            The operation failed.
**/
EFI_STATUS
UsbConfigPortsEventHandler(
  IN     VOID                            *UsbPortHandle,
  OUT    USB_CFG_PORT_MESSAGE            *Message,
  IN OUT UINTN                           *PayloadSize,
  OUT    USB_TYPEC_PORT_MESSAGE_PAYLOAD  *Payload
  );


/**
@brief Return the real-time connection state for an USB Type-C port. 

The function returns the real-time value of the type-c port mode value 
associated with the UsbPortHandle.

@param [in]  UsbPortHandle    The handle of the USB port
@param [out] UsbPortMode      The latest cached USB_TYPEC_PORT_MODE value

@retval EFI_SUCCESS           The operation succeeded.
@retval Others                The operation failed.
**/
EFI_STATUS
UsbPortGetRtConnectionMode(
  IN  VOID                *UsbPortHandle,
  OUT USB_TYPEC_PORT_MODE *UsbPortMode
  );


/**
@brief Return the cached connection state for an USB Type-C port. 

The function returns the cached value of the type-c port mode 
value associated with the UsbPortHandle. The value is only
updated, if any, when the client calls the EventHandler, which
queries the hardware for the update.

@param [in]  UsbPortHandle    The handle of the USB port
@param [out] UsbPortMode      The latest cached USB_TYPEC_PORT_MODE value

@retval EFI_SUCCESS           The operation succeeded.
@retval Others                The operation failed.
**/
EFI_STATUS
UsbPortGetConnectionMode(
  IN  VOID                *UsbPortHandle,
  OUT USB_TYPEC_PORT_MODE *UsbPortMode
  );


/**
@brief Return the cached CC out status for an USB Type-C port.

The function returns the cached value of the type-c port CC out
status associated with the UsbPortHandle. The value is only
updated, if any, when the client calls the EventHandler, which 
queries the hardware for the update.

@param [in]  UsbPortHandle    The handle of the USB port
@param [out] CcOutStatus      The CC out status returned by the hardware

@retval EFI_SUCCESS           The operation succeeded.
@retval Others                The operation failed.
**/
EFI_STATUS
UsbPortGetCcOutStatus(
  IN  VOID                    *UsbPortHandle,
  OUT USB_TYPEC_CC_OUT_STATUS *CcOutStatus
  );
#endif /* _USB_CONFIG_USB_PORTS_PUB_H_ */
