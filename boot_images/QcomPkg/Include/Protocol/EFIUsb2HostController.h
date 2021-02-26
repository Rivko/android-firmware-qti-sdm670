/**
  @file Usb2HostController.h
  @brief EFI_USB2_HC_PROTOCOL as defined in UEFI 2.0.

  The USB Host Controller Protocol is used by code, typically USB bus drivers,
  running in the EFI boot services environment, to perform data transactions over
  a USB bus. In addition, it provides an abstraction for the root hub of the USB bus.

  Portions Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*/
/*=============================================================================
  Copyright (c) 2012, 2014 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 02/05/14   amitg    Async USB Host Stack
 05/01/13    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 09/27/12   llg      (Tech Pubs) Edited/added Doxygen comments and markup.

=============================================================================*/

#ifndef _USB2_HOSTCONTROLLER_H_
#define _USB2_HOSTCONTROLLER_H_

#include <Protocol/EFIUsbIo.h>

/** @ingroup usb2_host_controller_protocol */
#define EFI_USB2_HC_PROTOCOL_GUID \
  { \
    0x3e745226, 0x9818, 0x45b6, {0xa2, 0xac, 0xd7, 0xcd, 0xe, 0x8b, 0xa2, 0xbc } \
  }

/** @cond */
/* Forward reference for pure ANSI compatability */
typedef struct _EFI_USB2_HC_PROTOCOL EFI_USB2_HC_PROTOCOL;
/** @endcond */

/** @ingroup usb2_host_controller_data_types
  USB port status. */
typedef struct {
  UINT16          PortStatus;        /**< Current port status bitmap. */
  UINT16          PortChangeStatus;  /**< Current port change status bitmap.*/
} EFI_USB_PORT_STATUS;

/** @addtogroup usb2_host_controller_constants 
@{ */
/* EFI_USB_PORT_STATUS.PortStatus bit definition */

/** Port status bit definition for connection. */
#define USB_PORT_STAT_CONNECTION    0x0001
/** Port status bit definition for enable. */
#define USB_PORT_STAT_ENABLE        0x0002
/** Port status bit definition for suspend. */
#define USB_PORT_STAT_SUSPEND       0x0004
/** Port status bit definition for overcurrnet. */
#define USB_PORT_STAT_OVERCURRENT   0x0008
/** Port status bit definition for reset. */
#define USB_PORT_STAT_RESET         0x0010
/** Port status bit definition for power. */
#define USB_PORT_STAT_POWER         0x0100
/** Port status bit definition for low speed. */
#define USB_PORT_STAT_LOW_SPEED     0x0200
/** Port status bit definition for high speed. */
#define USB_PORT_STAT_HIGH_SPEED    0x0400
/** Port status bit definition for super speed. */
#define USB_PORT_STAT_SUPER_SPEED   0x0800
/** Port status bit definition for owner. */
#define USB_PORT_STAT_OWNER         0x2000

/* EFI_USB_PORT_STATUS.PortChangeStatus bit definition */

/** Port change status bit definition for connection. */
#define USB_PORT_STAT_C_CONNECTION  0x0001
/** Port change status bit definition for enable. */
#define USB_PORT_STAT_C_ENABLE      0x0002
/** Port change status bit definition for suspend. */
#define USB_PORT_STAT_C_SUSPEND     0x0004
/** Port change status bit definition for overcurrent. */
#define USB_PORT_STAT_C_OVERCURRENT 0x0008
/** Port change status bit definition for reset. */
#define USB_PORT_STAT_C_RESET       0x0010
/** @} */ /* end_addtogroup usb2_host_controller_constants */


/** @ingroup usb2_host_controller_data_types
  USB port feature value. Each value indicates its bit index in the port 
  status bitmap and status change bitmap. When combined, these two bitmaps 
  equal a 32-bit bitmap.
*/
typedef enum {
  EfiUsbPortEnable            = 1,  /**< Enable; value 1. */
  EfiUsbPortSuspend           = 2,  /**< Suspend; value 2. */
  EfiUsbPortReset             = 4,  /**< Reset; value 4. */
  EfiUsbPortPower             = 8,  /**< Power; value 8. */
  EfiUsbPortOwner             = 13, /**< Owner; value 13. */
  EfiUsbPortConnectChange     = 16, /**< Connection change; value 16. */
  EfiUsbPortEnableChange      = 17, /**< Enable change; value 17. */
  EfiUsbPortSuspendChange     = 18, /**< Suspend change; value 18. */
  EfiUsbPortOverCurrentChange = 19, /**< Overcurrent change; value 19. */
  EfiUsbPortResetChange       = 20  /**< Reset change; value 20. @newpage */
} EFI_USB_PORT_FEATURE;

/** @addtogroup usb2_host_controller_constants 
@{ */
/** USB full speed; 12 Mb/sec, USB 1.1 OHCI and UHCI HC. */
#define EFI_USB_SPEED_FULL      0x0000
/** USB low speed; 1 Mb/sec, USB 1.1 OHCI and UHCI HC. */
#define EFI_USB_SPEED_LOW       0x0001
/** USB high speed; 480 Mb/sec, USB 2.0 EHCI HC. */
#define EFI_USB_SPEED_HIGH      0x0002
/** USB super speed; 4.8 Gb/sec, USB 3.0 XHCI HC. */
#define EFI_USB_SPEED_SUPER     0x0003
/** @} */ /* end_addtogroup usb2_host_controller_constants */

/** @ingroup usb2_host_controller_data_types
  Transaction translator.
*/
typedef struct {
  UINT8      TranslatorHubAddress;
  /**< Device address. */
  UINT8      TranslatorPortNumber;
  /**< Port number of the hub to which the device is connected. */
} EFI_USB2_HC_TRANSACTION_TRANSLATOR;


/* Protocol definitions */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_USB2_HC_PROTOCOL_GET_CAPABILITY */
/** @ingroup usb2_hc_get_capability
  @par Summary
  Retrieves the host controller capabilities.

  @param[in]  This           Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[out] MaxSpeed       Host controller data transfer speed.
  @param[out] PortNumber     Number of the root hub ports.
  @param[out] Is64BitCapable TRUE if the controller supports 64-bit memory 
                             addressing; otherwise FALSE.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- NULL parameter was passed. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_GET_CAPABILITY)(
  IN  EFI_USB2_HC_PROTOCOL  *This,
  OUT UINT8                 *MaxSpeed,
  OUT UINT8                 *PortNumber,
  OUT UINT8                 *Is64BitCapable
  );

/** @addtogroup usb2_host_controller_constants
@{ */ 
/** Global reset. */
#define EFI_USB_HC_RESET_GLOBAL             0x0001
/** Host controller reset. */
#define EFI_USB_HC_RESET_HOST_CONTROLLER    0x0002
/** Global reset with debug. */
#define EFI_USB_HC_RESET_GLOBAL_WITH_DEBUG  0x0004
/** Host controller reset with debug. */
#define EFI_USB_HC_RESET_HOST_WITH_DEBUG    0x0008
/** @} */ /* end_addtogroup usb2_host_controller_constants */ 

/* EFI_USB2_HC_PROTOCOL_RESET */
/** @ingroup usb2_hc_reset
  @par Summary
  Provides a software reset for the USB host controller.

  @param[in]  This        Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  Attributes  Bitmask of the reset operation to perform.

  @return
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- Attributes parameter is not valid. 
  @par
  EFI_UNSUPPORTED       -- Type of reset specified by the Attributes parameter 
                           is not currently supported by the host controller 
                           hardware. 
  @par
  EFI_ACCESS_DENIED     -- Reset operation is rejected because the debug port 
                           is being configured and active; only 
                           EFI_USB_HC_RESET_GLOBAL_WITH_DEBUG or 
                           EFI_USB_HC_RESET_HOST_WITH_DEBUG reset attributes 
                           can be used to perform a reset operation for this 
                           host controller. 
  @par
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_RESET)(
  IN EFI_USB2_HC_PROTOCOL   *This,
  IN UINT16                 Attributes
  );

/** @ingroup usb2_host_controller_data_types
  USB host controller state.
*/
typedef enum {
  EfiUsbHcStateHalt,
  /**< Halt state. No USB transactions can occur while in this state. The host
       controller enters this state for three reasons: 
       - After a host controller hardware reset
       - State is explicitly set by the software 
       - State is triggered by a fatal error such as consistency check failure */
  EfiUsbHcStateOperational,
  /**< Operational state. When in this state, the host controller can execute 
       bus traffic. This state must be explicitly set to enable the USB bus 
       traffic. */
  EfiUsbHcStateSuspend,
  /**< Suspend state. No USB transactions can occur while in this state. The 
       host controller enters this state for the following reasons:
       - State is explicitly set by the software
       - State is triggered when there is no bus traffic for 3 microseconds */
/** @cond */
  EfiUsbHcStateMaximum
/** @endcond */
} EFI_USB_HC_STATE;

/* EFI_USB2_HC_PROTOCOL_GET_STATE */
/** @ingroup usb2_hc_get_state
  @par Summary
  Retrieves the current state of the USB host controller.

  @param[in]  This  Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[out] State Pointer to the #EFI_USB_HC_STATE enumeration that
                    indicates the current state of the USB host controller.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- State parameter is NULL. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_GET_STATE)(
  IN        EFI_USB2_HC_PROTOCOL    *This,
  OUT       EFI_USB_HC_STATE        *State
);

/* EFI_USB2_HC_PROTOCOL_SET_STATE */
/** @ingroup usb2_hc_set_state
  @par Summary
  Sets the USB host controller to a specific state.

  @param[in]  This  Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  State State to which the USB host controller is set; see 
                    #EFI_USB_HC_STATE for details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- State parameter is not valid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_SET_STATE)(
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN EFI_USB_HC_STATE        State
  );

/* EFI_USB2_HC_PROTOCOL_CONTROL_TRANSFER */
/** @ingroup usb2_hc_control_transfer
  @par Summary
  Submits a control transfer to a target USB device.

  @param[in]  This                Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  DeviceAddress       Address of the target USB device.
  @param[in]  DeviceSpeed         Device speed.
  @param[in]  MaximumPacketLength Maximum packet size the default control 
                                  transfer endpoint is capable of sending or 
                                  receiving.
  @param[in]  Request             Pointer to the USB device request that is 
                                  sent to the USB device.
  @param[in]  TransferDirection   Specifies the data direction for the transfer. 
                                  There are three values available: 
                                  EfiUsbDataIn, EfiUsbDataOut and EfiUsbNoData.
  @param[in,out] Data             Pointer to the data buffer that is 
                                  transmitted to or received from the USB 
                                  device.
  @param[in,out] DataLength       Data buffer size or the number of transferred 
                                  bytes. 
                                  - On input, the size (in bytes) of the data 
                                    buffer specified by Data
                                  - On output, the number of bytes that were 
                                    actually transferred @tablebulletend
  @param[in]  TimeOut             Maximum time (in milliseconds) the transfer 
                                  is allowed to complete.
  @param[in]  Translator          Pointer to the transaction translator data; 
                                  see #EFI_USB2_HC_TRANSACTION_TRANSLATOR for 
                                  details.
  @param[out] TransferResult      Pointer to the detailed result information 
                                  generated by this control transfer.

  @newpage
  @return
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. 
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. 
  @par
  EFI_TIMEOUT           -- Transfer failed due to timeout. 
  @par
  EFI_DEVICE_ERROR      -- Transfer failed due to a host controller or device 
                           error; the TransferResult parameter has detailed 
                           error information.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_CONTROL_TRANSFER)(
  IN     EFI_USB2_HC_PROTOCOL               *This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              DeviceSpeed,
  IN     UINTN                              MaximumPacketLength,
  IN     EFI_USB_DEVICE_REQUEST             *Request,
  IN     EFI_USB_DATA_DIRECTION             TransferDirection,
  IN OUT VOID                               *Data       OPTIONAL,
  IN OUT UINTN                              *DataLength OPTIONAL,
  IN     UINTN                              TimeOut,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
  OUT    UINT32                             *TransferResult
  );

/* EFI_USB2_HC_PROTOCOL_ASYNC_CONTROL_TRANSFER */
/** @ingroup usb2_hc_async_control_transfer
  @par Summary
  Submits an asynchronous control transfer to a control endpoint of a USB device.

  @param  This                A pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param  DeviceAddress       Represents the address of the target device on the USB.
  @param  DeviceSpeed         Indicates device speed.
  @param  MaximumPacketLength Indicates the maximum packet size the target endpoint is capable of
                                                 sending or receiving.
  @param  Request       Pointer to #EFI_USB_DEVICE_REQUEST that is sent to 
                                        the USB device.
  @param  Data                Array of pointers to the buffers of data that will be transmitted to USB
                              device or received from USB device.
  @param  DataLength          When input, indicates the size, in bytes, of the data buffers specified by
                              Data. When output, indicates the actually transferred data size.
  @param  PollingInterval     Indicates the maximum time, in milliseconds, to wait between checking the
                              transfer status;
  @param  Translator          A pointer to the transaction translator data.
  @param  CallBackFunction    The Callback function. This function is called at the rate specified by
                              PollingInterval.
  @param  Context             The context that is passed to the CallBackFunction. This is an
                              optional parameter and may be NULL..

  @newpage
  @return 
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. 
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. 
  @par
  EFI_TIMEOUT           -- Transfer failed due to timeout. 
  @par
  EFI_DEVICE_ERROR      -- Transfer failed due to a host controller or device 
                           error; the TransferResult parameter has detailed 
                           error information.
*/

typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_ASYNC_CONTROL_TRANSFER)(
  IN     EFI_USB2_HC_PROTOCOL                     *This,
  IN     UINT8                                    DeviceAddress,
  IN     UINT8                                    DeviceSpeed,
  IN     UINTN                                    MaximumPacketLength,
  IN     EFI_USB_DEVICE_REQUEST                   *Request,
  IN     EFI_USB_DATA_DIRECTION                   Direction,
  IN OUT VOID                                     *Data,
  IN OUT UINTN                                    *DataLength,
  IN     UINTN                                    PollingInterval  OPTIONAL,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR       *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK CallBackFunction OPTIONAL,
  IN     VOID                                     *Context         OPTIONAL
  );



/** @ingroup usb2_host_controller_constants
  Maximum number of bulk buffers.
*/ 
#define EFI_USB_MAX_BULK_BUFFER_NUM 10

/* EFI_USB2_HC_PROTOCOL_BULK_TRANSFER */
/** @ingroup usb2_hc_bulk_transfer
  @par Summary
  Submits a bulk transfer to a bulk endpoint of a USB device.

  @param[in]  This                Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  DeviceAddress       Address of the target USB device.
  @param[in]  EndPointAddress     Combination of an endpoint number and an 
                                  endpoint direction of the target USB device.
  @param[in]  DeviceSpeed         Device speed.
  @param[in]  MaximumPacketLength Maximum packet size the target endpoint is 
                                  capable of sending or receiving.
  @param[in]  DataBuffersNumber   Number of data buffers prepared for the 
                                  transfer.
  @param[in,out] Data             Array of pointers to the data buffers that 
                                  are transmitted to or received from the USB 
                                  device.
  @param[in,out] DataLength       Data buffer size or the number of transferred 
                                  bytes. 
                                  - On input, the size (in bytes) of the data 
                                    buffers specified by Data
                                  - On output, the number of bytes that were 
                                    actually transferred @tablebulletend
  @param[in,out] DataToggle       Pointer to the data toggle value.
  @param[in]  TimeOut             Maximum time (in milliseconds) the transfer 
                                  is allowed to complete.
  @param[in]  Translator          Pointer to the transaction translator data; 
                                  see #EFI_USB2_HC_TRANSACTION_TRANSLATOR for 
                                  details.
  @param[out] TransferResult      Pointer to the detailed result information of 
                                  the bulk transfer. 

  @newpage
  @return
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. 
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. 
  @par
  EFI_TIMEOUT           -- Transfer failed due to timeout. 
  @par
  EFI_DEVICE_ERROR      -- Transfer failed due to a host controller or device 
                           error; the TransferResult parameter has detailed 
                           error information.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_BULK_TRANSFER)(
  IN     EFI_USB2_HC_PROTOCOL               *This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              EndPointAddress,
  IN     UINT8                              DeviceSpeed,
  IN     UINTN                              MaximumPacketLength,
  IN     UINT8                              DataBuffersNumber,
  IN OUT VOID                               *Data[EFI_USB_MAX_BULK_BUFFER_NUM],
  IN OUT UINTN                              *DataLength,
  IN OUT UINT8                              *DataToggle,
  IN     UINTN                              TimeOut,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
  OUT    UINT32                             *TransferResult
  );

/* EFI_USB2_HC_PROTOCOL_ASYNC_BULK_TRANSFER */
/** @ingroup usb2_hc_async_bulk_transfer
  @par Summary
  Submits an asynchronous bulk transfer to a bulk endpoint of a USB device.

  @param[in]  This                Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  DeviceAddress       Address of the target USB device.
  @param[in]  EndPointAddress     Combination of an endpoint number and an 
                                  endpoint direction of the target USB device.
  @param[in]  DeviceSpeed         Device speed.
  @param[in]  MaximumPacketLength Maximum packet size the target endpoint is 
                                  capable of sending or receiving.
  @param[in]  DataBuffersNumber   Number of data buffers prepared for the 
                                  transfer.
  @param[in,out] Data             Array of pointers to the data buffers that 
                                  are transmitted to or received from the USB 
                                  device.
  @param[in,out] DataLength       Data buffer size or the number of transferred 
                                  bytes. 
                                  - On input, the size (in bytes) of the data 
                                    buffers specified by Data
                                  - On output, the number of bytes that were 
                                    actually transferred @tablebulletend
  @param[in,out] DataToggle       Pointer to the data toggle value.
  @param[in]  PollingInterval     Interval (in milliseconds) the asynchronous 
                                  bulk transfer is polled.
  @param[in]  CallBackFunction    Callback function; this function is called at 
                                  the rate specified by PollingInterval.
  @param[in]  Context             Data passed to the callback function. This is 
                                  an optional parameter and may be NULL.

  @newpage
  @return 
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. 
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. 
  @par
  EFI_TIMEOUT           -- Transfer failed due to timeout. 
  @par
  EFI_DEVICE_ERROR      -- Transfer failed due to a host controller or device 
                           error; the TransferResult parameter has detailed 
                           error information.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_ASYNC_BULK_TRANSFER)(
  IN     EFI_USB2_HC_PROTOCOL               *This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              EndPointAddress,
  IN     UINT8                              DeviceSpeed,
  IN     UINTN                              MaximumPacketLength,
  IN     UINT8                              DataBuffersNumber,
  IN OUT VOID                               *Data[EFI_USB_MAX_BULK_BUFFER_NUM],
  IN OUT UINTN                              *DataLength,
  IN OUT UINT8                              *DataToggle,
  IN     UINTN                                               PollingInterval  OPTIONAL,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK            CallBackFunction OPTIONAL,
  IN     VOID                                                *Context         OPTIONAL
  );
  
/* EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER */
/* Note: Translator parameter doesn't exist in UEFI2.0 spec, 
   but it will be updated in the following specification version. */
/** @ingroup usb2_hc_async_interrupt_transfer
  @par Summary
  Submits an asynchronous interrupt transfer to an interrupt endpoint of a USB 
  device.

  @param[in]  This                Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  DeviceAddress       Address of the target USB device.
  @param[in]  EndPointAddress     Combination of an endpoint number and an 
                                  endpoint direction of the target USB device.
  @param[in]  DeviceSpeed         Device speed.
  @param[in]  MaximumPacketLength Maximum packet size the target endpoint is 
                                  capable of sending or receiving.
  @param[in]  IsNewTransfer       New transfer flag. 
                                  - If TRUE, an asynchronous interrupt pipe is 
                                    built between the host and the target 
                                    interrupt endpoint 
                                  - If FALSE, the specified asynchronous 
                                    interrupt pipe is cancelled 
                                  - If TRUE and an interrupt transfer exists 
                                    for the target endpoint, 
                                    EFI_INVALID_PARAMETER is returned @tablebulletend
  @param[in,out] DataToggle       Pointer to the data toggle value.
  @param[in]  PollingInterval     Interval (in milliseconds) the asynchronous 
                                  interrupt transfer is polled.
  @param[in]  DataLength          Length of data to be received, at the rate 
                                  specified by PollingInterval, from the target 
                                  asynchronous interrupt endpoint.
  @param[in]  Translator          Pointer to the transaction translator data; 
                                  see #EFI_USB2_HC_TRANSACTION_TRANSLATOR for 
                                  details.
  @param[in]  CallBackFunction    Callback function; this function is called at 
                                  the rate specified by PollingInterval.
  @param[in]  Context             Data passed to the callback function. This is 
                                  an optional parameter and may be NULL.

  @return
  EFI_SUCCESS           -- Function completed successfully or it was cancelled. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER)(
  IN     EFI_USB2_HC_PROTOCOL                                *This,
  IN     UINT8                                               DeviceAddress,
  IN     UINT8                                               EndPointAddress,
  IN     UINT8                                               DeviceSpeed,
  IN     UINTN                                               MaxiumPacketLength,
  IN     BOOLEAN                                             IsNewTransfer,
  IN OUT UINT8                                               *DataToggle,
  IN     UINTN                                               PollingInterval  OPTIONAL,
  IN     UINTN                                               DataLength       OPTIONAL,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR                  *Translator      OPTIONAL,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK             CallBackFunction OPTIONAL,
  IN     VOID                                                *Context         OPTIONAL
  );

/* EFI_USB2_HC_PROTOCOL_SYNC_INTERRUPT_TRANSFER */
/* Note: Translator parameter doesn't exist in UEFI2.0 spec, 
   but it will be updated in the following specification version. */
/** @ingroup usb2_hc_sync_interrupt_transfer
  @par Summary
  Submits a synchronous interrupt transfer to an interrupt endpoint of a USB 
  device.

  @param[in]  This                Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  DeviceAddress       Address of the target USB device.
  @param[in]  EndPointAddress     Combination of an endpoint number and an 
                                  endpoint direction of the target USB device.
  @param[in]  DeviceSpeed         Indicates device speed.
  @param[in]  MaximumPacketLength Maximum packet size the target endpoint is 
                                  capable of sending or receiving.
  @param[in,out] Data             Pointer to the data buffer that is 
                                  transmitted to or received from the USB 
                                  device.
  @param[in,out] DataLength       Data buffer size or the number of transferred 
                                  bytes. 
                                  - On input, the size (in bytes) of the data 
                                    buffer specified by Data
                                  - On output, the number of bytes that were 
                                    actually transferred @tablebulletend
  @param[in,out] DataToggle       Pointer to the data toggle value.
  @param[in]  TimeOut             Maximum time (in milliseconds) the transfer 
                                  is allowed to complete.
  @param[in]  Translator          Pointer to the transaction translator data; 
                                  see #EFI_USB2_HC_TRANSACTION_TRANSLATOR for 
                                  details.
  @param[out] TransferResult      Pointer to the detailed result information of 
                                  the synchronous interrupt transfer.

  @newpage
  @return
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. 
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. 
  @par
  EFI_TIMEOUT           -- Transfer failed due to timeout. 
  @par
  EFI_DEVICE_ERROR      -- Transfer failed due to a host controller or device 
                           error; the TransferResult parameter has detailed 
                           error information.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_SYNC_INTERRUPT_TRANSFER)(
  IN     EFI_USB2_HC_PROTOCOL                        *This,
  IN     UINT8                                       DeviceAddress,
  IN     UINT8                                       EndPointAddress,
  IN     UINT8                                       DeviceSpeed,
  IN     UINTN                                       MaximumPacketLength,
  IN OUT VOID                                        *Data,
  IN OUT UINTN                                       *DataLength,
  IN OUT UINT8                                       *DataToggle,
  IN     UINTN                                       TimeOut,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR          *Translator,
  OUT    UINT32                                      *TransferResult
  );

/** @addtogroup usb2_host_controller_constants 
@{ */ 
/** Maximum number of isochronous buffers. */
#define EFI_USB_MAX_ISO_BUFFER_NUM  7
/** Maximum alternate number of isochronous buffers. */
#define EFI_USB_MAX_ISO_BUFFER_NUM1 2
/** @} */ /* end_addtogroup usb2_host_controller_constants */ 

/* EFI_USB2_HC_PROTOCOL_ISOCHRONOUS_TRANSFER */
/** @ingroup usb2_hc_isochronous_transfer
  @par Summary
  Submits an isochronous transfer to an isochronous endpoint of a USB device.

  @param[in]  This                Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  DeviceAddress       Address of the target USB device.
  @param[in]  EndPointAddress     Combination of an endpoint number and an 
                                  endpoint direction of the target USB device.
  @param[in]  DeviceSpeed         Device speed.
  @param[in]  MaximumPacketLength Maximum packet size the target endpoint is 
                                  capable of sending or receiving.
  @param[in]  DataBuffersNumber   Number of data buffers prepared for the 
                                  transfer.
  @param[in,out] Data             Array of pointers to the data buffers that 
                                  are transmitted to or received from the USB 
                                  device.
  @param[in]  DataLength          Length (in bytes) of the data to be sent to 
                                  or received from the USB device.
  @param[in]  Translator          Pointer to the transaction translator data; 
                                  see #EFI_USB2_HC_TRANSACTION_TRANSLATOR for 
                                  details.
  @param[out] TransferResult      Pointer to the detailed result information of 
                                  the isochronous transfer.

  @return 
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. 
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. 
  @par
  EFI_TIMEOUT           -- Transfer failed due to timeout. 
  @par
  EFI_DEVICE_ERROR      -- Transfer failed due to a host controller or device 
                           error; the TransferResult parameter has detailed 
                           error information.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_ISOCHRONOUS_TRANSFER)(
  IN     EFI_USB2_HC_PROTOCOL               *This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              EndPointAddress,
  IN     UINT8                              DeviceSpeed,
  IN     UINTN                              MaximumPacketLength,
  IN     UINT8                              DataBuffersNumber,
  IN OUT VOID                               *Data[EFI_USB_MAX_ISO_BUFFER_NUM],
  IN     UINTN                              DataLength,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
  OUT    UINT32                             *TransferResult
  );

/* EFI_USB2_HC_PROTOCOL_ASYNC_ISOCHRONOUS_TRANSFER */
/** @ingroup usb2_hc_async_isochronous_transfer
  @par Summary
  Submits a nonblocking isochronous transfer to an isochronous endpoint of a 
  USB device.

  @param[in]  This                Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  DeviceAddress       Address of the target USB device.
  @param[in]  EndPointAddress     Combination of an endpoint number and an 
                                  endpoint direction of the target USB device.
  @param[in]  DeviceSpeed         Device speed.
  @param[in]  MaximumPacketLength Maximum packet size the target endpoint is 
                                  capable of sending or receiving.
  @param[in]  DataBuffersNumber   Number of data buffers prepared for the 
                                  transfer.
  @param[in,out]  Data            Array of pointers to the data buffers that 
                                  are transmitted to or received from the USB 
                                  device.
  @param[in]  DataLength          Length (in bytes) of the data to be sent to 
                                  or received from the USB device.
  @param[in]  Translator          Pointer to the transaction translator data; 
                                  see #EFI_USB2_HC_TRANSACTION_TRANSLATOR for 
                                  details.
  @param[in]  IsochronousCallback Isochronous callback function; this function 
                                  is called if the isochronous transfer is 
                                  completed.
  @param[in]  Context             Data passed to the isochronous callback 
                                  function. This is an optional parameter and 
                                  may be NULL.

  @return
  EFI_SUCCESS     -- Function completed successfully. \n
  EFI_UNSUPPORTED -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_ASYNC_ISOCHRONOUS_TRANSFER)(
  IN     EFI_USB2_HC_PROTOCOL               *This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              EndPointAddress,
  IN     UINT8                              DeviceSpeed,
  IN     UINTN                              MaximumPacketLength,
  IN     UINT8                              DataBuffersNumber,
  IN OUT VOID                               *Data[EFI_USB_MAX_ISO_BUFFER_NUM],
  IN     UINTN                              DataLength,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK    IsochronousCallBack,
  IN     VOID                               *Context OPTIONAL
  );

/* EFI_USB2_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS */
/** @ingroup usb2_hc_get_roothub_port_status
  @par Summary
  Retrieves the current status of a USB root hub port.

  @param[in]  This       Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  PortNumber Specifies the root hub port from which the status is 
                         to be retrieved. This value is zero-based.
  @param[out] PortStatus Pointer to the current port status bits and port 
                         change status bits; see #EFI_USB_PORT_STATUS for 
                         details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- PortNumber parameter is invalid.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS)(
  IN        EFI_USB2_HC_PROTOCOL    *This,
  IN        UINT8                   PortNumber,
  OUT       EFI_USB_PORT_STATUS     *PortStatus
  );

/* EFI_USB2_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE */
/** @ingroup usb2_hc_set_roothub_port_feature
  @par Summary
  Sets a feature for the specified root hub port.

  @param[in]  This        Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  PortNumber  Specifies the root hub port whose feature is 
                          to be set. This value is zero-based.
  @param[in]  PortFeature Indicates the feature selector associated with the 
                          feature set request; see #EFI_USB_PORT_FEATURE for 
                          details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE)(
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN UINT8                   PortNumber,
  IN EFI_USB_PORT_FEATURE    PortFeature
  );

/* EFI_USB2_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE */
/** @ingroup usb2_hc_clear_roothub_port_feature
  @par Summary
  Clears a feature for the specified root hub port.

  @param[in]  This        Pointer to the EFI_USB2_HC_PROTOCOL instance.
  @param[in]  PortNumber  Specifies the root hub port whose feature is 
                          to be cleared. This value is zero-based.
  @param[in]  PortFeature Indicates the feature selector associated with the 
                          feature clear request; see #EFI_USB_PORT_FEATURE for 
                          details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB2_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE)(
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN UINT8                   PortNumber,
  IN EFI_USB_PORT_FEATURE    PortFeature
  );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup usb2_host_controller_protocol
  @par Summary
  This protocol provides USB host controller management, basic 
  data transactions over a USB bus, and USB root hub access. A device driver 
  that wants to manage a USB bus in a system retrieves the EFI_USB2_HC_PROTOCOL 
  instance that is associated with the USB bus to be managed. A device handle 
  for a USB host controller minimally contains an EFI_DEVICE_PATH_PROTOCOL 
  instance and an EFI_USB2_HC_PROTOCOL instance.

  @par Parameters
  @inputprotoparams{usb2_host_controller_proto_params.tex} 
*/
struct _EFI_USB2_HC_PROTOCOL {
  EFI_USB2_HC_PROTOCOL_GET_CAPABILITY              GetCapability;
  EFI_USB2_HC_PROTOCOL_RESET                       Reset;
  EFI_USB2_HC_PROTOCOL_GET_STATE                   GetState;
  EFI_USB2_HC_PROTOCOL_SET_STATE                   SetState;
  EFI_USB2_HC_PROTOCOL_CONTROL_TRANSFER            ControlTransfer;
  EFI_USB2_HC_PROTOCOL_BULK_TRANSFER               BulkTransfer;
  EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER    AsyncInterruptTransfer;
  EFI_USB2_HC_PROTOCOL_SYNC_INTERRUPT_TRANSFER     SyncInterruptTransfer;
  EFI_USB2_HC_PROTOCOL_ISOCHRONOUS_TRANSFER        IsochronousTransfer;
  EFI_USB2_HC_PROTOCOL_ASYNC_ISOCHRONOUS_TRANSFER  AsyncIsochronousTransfer;
  EFI_USB2_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS     GetRootHubPortStatus;
  EFI_USB2_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE    SetRootHubPortFeature;
  EFI_USB2_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE  ClearRootHubPortFeature;

  ///
  /// The major revision number of the USB host controller. The revision information 
  /// indicates the release of the Universal Serial Bus Specification with which the 
  /// host controller is compliant.
  ///
  UINT16                                           MajorRevision;

  ///
  /// The minor revision number of the USB host controller. The revision information 
  /// indicates the release of the Universal Serial Bus Specification with which the 
  /// host controller is compliant.  
  ///
  UINT16                                           MinorRevision;
  UINT32                                           Padding;
  
  EFI_USB2_HC_PROTOCOL_ASYNC_BULK_TRANSFER         AsyncBulkTransfer; // need to address the change
                                                                      // to move out and create new version
                                                                      // of the protocol structure
  EFI_USB2_HC_PROTOCOL_ASYNC_CONTROL_TRANSFER      AsyncControlTransfer; // need to address the change
};

/** @cond */
extern EFI_GUID gEfiUsb2HcProtocolGuid;
/** @endcond */

#endif
