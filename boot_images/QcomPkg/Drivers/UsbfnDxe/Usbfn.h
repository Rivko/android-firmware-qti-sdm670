/** @file Usbfn.h

  The header file for Usbfn

  Copyright (c) 2010-2011, 2014-2015 Qualcomm Technologies Inc. All rights reserved.
  Portions Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/20/15   ck      Update and clean up EFIUsbfnIo Protocol
 12/12/14   arung   Port UEFI LPM to 8909	
 04/30/14   ar      Cleanup for NDEBUG build  
 06/24/11   cching  port Usbfn to UEFI
=============================================================================*/

#ifndef USBFN_H
#define USBFN_H

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/EFIUsbfnIo.h>
#include <Library/UsbConfigLib.h>
#include <Protocol/EFIPlatformInfo.h>
#include "qhsusb_dcd.h"
#include "UsbError.h"

// device signature and CR record
#define USBFN_DEV_SIGNATURE   SIGNATURE_32 ('u', 's', 'b', 'f')
#define USBFN_FROM_THIS(x)    CR(x, USBFN_DEV, UsbfnIo, USBFN_DEV_SIGNATURE)

// Endpoint related Definitions
#define USB_ENDPOINT_NUMBER_MIN  0x00
#define USB_ENDPOINT_NUMBER_MAX  0x0f
#define USB_ENDPOINT_DIR_OUT     0x00
#define USB_ENDPOINT_DIR_BMSK    USB_ENDPOINT_DIR_IN

// Macro to check address alignment.
#define IS_ALIGNED(Address, Align) ((((UINTN)Address) & (Align-1)) == 0)
// Macro for check whether the address is within lower 32 bit
#define IS_NOT_32_BIT_ADDR(ADDRESS) ((((UINT64) ADDRESS) & (0xFFFFFFFF00000000)) != (UINT64) (0))
// Default Uncached Buffer Alignment : 4K
#define USBFN_DEFAULT_BUFFER_ALIGN 4096

// Convert the max transfer size to number of dTDs
#define UFN_TDS_MASK             0x3FFF
#define UFN_TDS_SHIFT            14
#define UFN_MAX_SIZE_TO_TDS(a)  (((a) >> UFN_TDS_SHIFT) + (((a) & UFN_TDS_MASK) ? 1 : 0))

// allocate memory and check whether the allocation succeeds
#define ALLOC(Ptr, Size) \
  do { \
    Ptr = AllocatePool (Size); \
    if (!(Ptr)) { \
      Status = EFI_OUT_OF_RESOURCES; \
      ERR_CHK ("failed to allocate %d bytes", Size); \
    } \
  } while (0)

// free the memory and assign the pointer to NULL
#define FREE(Ptr) \
  do { \
    FreePool (Ptr); \
    Ptr = NULL; \
  } while (0)

// Converts endpoint index and direction to address.
#define ENDPOINT_ADDR(EndpointIndex, Direction) \
  ( \
    (EndpointIndex) \
    | ( \
        (Direction) == EfiUsbEndpointDirectionHostOut \
                     ? USB_ENDPOINT_DIR_OUT \
                     : USB_ENDPOINT_DIR_IN \
        ) \
  )

/**
 * Endpoint configuration data associated 
 * with each endpoint pipe.
 */
typedef struct _PipePolicyInfo {
  BOOLEAN   ZeroLengthTerminationSupport;
} PipePolicyInfo;


/**
 * Usbfn software device structure.
 */
typedef struct _USBFN_DEV USBFN_DEV;
struct _USBFN_DEV {
  UINTN                                 Signature;
  EFI_USBFN_IO_PROTOCOL                 UsbfnIo;
  struct qhsusb_dcd_dsc_device          SWDevice;
  QCOM_USB_CONFIG_PROTOCOL             *UsbConfigProtocol;
  QCOM_USB_CONFIG_CORE_TYPE             CoreType;
  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE   PlatType;
  EFI_EVENT                             ExitBootServiceEvent;
  EFI_EVENT                     		EnterLPMEvent;   // Triggered during wall charger connection for dead battery by Charger app 
  EFI_UNICODE_STRING_TABLE             *ControllerNameTable;
  PipePolicyInfo                        PipePolicy;
  //
  // This is a temp. workaround of FFU issue.
  //
  VOID                                 *TxBuffer;
};


/**
  Test to see if this driver supports ControllerHandle. Any
  ControllerHandle that has proper protocol installed will
  be supported.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to test.
  @param  RemainingDevicePath  Not used.

  @return EFI_SUCCESS          This driver supports this device.
  @return EFI_UNSUPPORTED      This driver does not support this device.

**/
EFI_STATUS
EFIAPI
UsbFnDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

/**
  Starting the UsbFn Driver.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to test.
  @param  RemainingDevicePath  Not used.

  @return EFI_SUCCESS          supports this device.
  @return EFI_UNSUPPORTED      do not support this device.
  @return EFI_DEVICE_ERROR     cannot be started due to device Error.
  @return EFI_OUT_OF_RESOURCES cannot allocate resources.

**/
EFI_STATUS
EFIAPI
UsbFnDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

/**
  Stop this driver on ControllerHandle. Support stopping any child handles
  created by this driver.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to stop driver on.
  @param  NumberOfChildren     Number of Children in the ChildHandleBuffer.
  @param  ChildHandleBuffer    List of handles for the children we need to stop.

  @return EFI_SUCCESS          Success.
  @return EFI_DEVICE_ERROR     Fail.

**/
EFI_STATUS
EFIAPI
UsbFnDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  );

/**
  This is a driver Exit point function that is called when
  gBS->ExitBootServices() is called.


  @param  Event                   Pointer to this event
  @param  Context                 Event handler private data

**/
VOID
EFIAPI
UsbfnExitBootService (
  EFI_EVENT                      Event,
  VOID                           *Context
  );

/**
  Copy the DeviceInfo to the internal data structure.

  @param  device                  The instance of qhsusb_dcd_dsc_device created based on DeviceInfo
  @param  DeviceInfo              The instance of DeviceInfo from the client

**/
EFI_STATUS
CopyDevice (
  OUT  struct qhsusb_dcd_dsc_device  *device,
  IN   EFI_USB_DEVICE_INFO           *DeviceInfo
  );

/**
  Free the internal qhsusb_dcd_dsc_device device object

  @param  device                  The instance of qhsusb_dcd_dsc_device to be freed.

**/
EFI_STATUS
FreeDevice (
  IN struct qhsusb_dcd_dsc_device *device
  );

/**
 * @brief Driver Enter Low Power mode function triggered by Charger App (Usbfn->EnterLPMEvent)
 *
 * Turns off all USB clocks
 *
 * @param  Event                   Pointer to this event
 * @param  Context                 Event handler private data
 */
VOID
EFIAPI
UsbfnEnterLPM(
    EFI_EVENT                      Event,
    VOID                           *Context
    );

/*
 * Protocol functions
 */

/**
  * @brief Returns the detected port type.
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [out] PortType        USB port type
  *
  * @retval EFI_SUCCESS          Port detected successfully
  * @retval Others               Failed to detect port
  */
EFI_STATUS
EFIAPI
UsbfnDetectPort (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT EFI_USBFN_PORT_TYPE    *PortType
  );

/**
  * @brief Configure the endpoints and activate the port.
  *
  * This function assumes that the hardware has been initialized and uses the
  * supplied DevceInfo to configure the endpoints, activate the port and start
  * receiving USB events.
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [in]  DeviceInfo      Device descriptor information
  *
  * @retval EFI_SUCCESS          The port is ready
  * @retval Others               Endpoint configuration failed
  */
EFI_STATUS
EFIAPI
UsbfnConfigureEnableEndpoints (
  IN EFI_USBFN_IO_PROTOCOL *This,
  IN EFI_USB_DEVICE_INFO   *DeviceInfo
  );

/**
  * @brief Configure the endpoints and activate the port.
  *
  * This function assumes that the hardware has been initialized and uses the
  * supplied DevceInfo to configure the endpoints, activate the port and start
  * receiving USB events. This function accepts DeviceInfo and SSDeviceInfo
  * objects and configures the endpoints with the information from the object
  * that supports the highest speed allowed by the underlying hardware. The
  * high speed and super speed DeviceInfo objects passed in must have the same
  * DeviceClass in the EFI_USB_DEVICE_DESCRIPTOR, otherwise this function will
  * return EFI_UNSUPPORTED.
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [in]  DeviceInfo      Device descriptor information
  * @param [in]  SSDeviceInfo    SuperSpeed device descriptor information
  *
  * @retval EFI_SUCCESS          The port is ready
  * @retval Others               Endpoint configuration failed
  */
EFI_STATUS
EFIAPI
UsbfnConfigureEnableEndpointsEx(
  IN EFI_USBFN_IO_PROTOCOL           *This,
  IN EFI_USB_DEVICE_INFO             *DeviceInfo,
  IN EFI_USB_SUPERSPEED_DEVICE_INFO  *SSDeviceInfo
);

/**
  * @brief Return the max packet size for the specified endpoint type
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [in]  EndpointType    Bulk, interrupt, etc.
  * @param [in]  BusSpeed        USB bus speed
  * @param [out] MaxPacketSize   Maximum packet size, in bytes
  *
  * @retval EFI_SUCCESS          MaxPacketSize is valid
  * @retval Others               Error
  */
EFI_STATUS
EFIAPI
UsbfnGetEndpointMaxPacketSize (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  IN  EFI_USB_ENDPOINT_TYPE  EndpointType,
  IN  EFI_USB_BUS_SPEED      BusSpeed,
  OUT UINT16                 *MaxPacketSize
  );

/**
  * @brief Return a Unicode string describing various device specific info
  *
  * This function does NOT pertain to the EFI_USB_DEVICE_INFO structure but
  * returns information such as the device's serial number.
  *
  * @param [in]     This         UsbfnIo protocol instance
  * @param [in]     Id           ID of requested information
  * @param [in,out] BufferSize   On input, size of buffer. On output, length
  *                              of data returned in buffer.
  * @param [out]    Buffer       Buffer to return requested information in
  *
  * @retval EFI_SUCCESS          Requested info returned in Buffer
  * @retval Others               Error
  */
EFI_STATUS
EFIAPI
UsbfnGetDeviceInfo (
  IN     EFI_USBFN_IO_PROTOCOL     *This,
  IN     EFI_USBFN_DEVICE_INFO_ID  Id,
  IN OUT UINTN                     *BufferSize,
  OUT    VOID                      *Buffer
  );

/**
  * @brief Return the device's Vendor ID (VID) and Product ID (PID)
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [out] Vid             Vendor ID
  * @param [out] Pid             Product ID
  *
  * @retval EFI_SUCCESS          VID and PID are valid
  * @retval Others               Error
  */
EFI_STATUS
EFIAPI
UsbfnGetVendorIdProductId (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT UINT16                 *Vid,
  OUT UINT16                 *Pid
  );

/**
  * @brief Abort a transfer on the specified endpoint
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [in]  EndpointIndex   Endpoint of ongoing transfer
  * @param [in]  Direction       Direction of ongoing transfer
  *
  * @retval EFI_SUCCESS          Transfer aborted successfully
  * @retval Others               No transfer to cancel or error
  */
EFI_STATUS
EFIAPI
UsbfnAbortTransfer (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction
  );

/**
  * @brief Returns whether an endpoint is currently stalled or not
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [in]  EndpointIndex   Endpoint to check
  * @param [in]  Direction       Direction of endpoint
  * @param [out] State           Current stall state (TRUE or FALSE)
  *
  * @retval EFI_SUCCESS          State checked successfully
  * @retval Others               Failed to fetch stall state
  */
EFI_STATUS
EFIAPI
UsbfnGetEndpointStallState (
  IN  EFI_USBFN_IO_PROTOCOL         *This,
  IN  UINT8                         EndpointIndex,
  IN  EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  OUT BOOLEAN                       *State
  );

/**
  * @brief Sets or clears stall on an endpoint
  *
  * In this implementation, it is not acceptable to clear stall on the
  * control endpoint. It is automatically cleared by the hardware.
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [in]  EndpointIndex   Endpoint to change stall state on
  * @param [in]  Direction       Direction of endpoint
  * @param [in]  State           Stall state to set
  *
  * @retval EFI_SUCCESS          Stall state set successfully
  * @retval Others               Failed to set stall state
  */
EFI_STATUS
EFIAPI
UsbfnSetEndpointStallState (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN BOOLEAN                       State
  );

/**
  * @brief Poll for transfer and bus events
  *
  * This function must be repeatedly called by the client to receive updates on
  * the state of the USB bus, setup packet arrivals on control endpoint 0 and
  * status changes for submitted transfers on all endpoints.
  *
  * @param [in]     This         UsbfnIo protocol instance
  * @param [out]    Message      Indicates the type of event
  * @param [in,out] PayloadSize  On input, size of buffer in Payload. On output,
  *                              size of data returned in payload.
  * @param [out]    Payload      The message specific payload
  *
  * @retval EFI_SUCCESS          Successfully polled for events
  * @retval Others               Error
  */
EFI_STATUS
EFIAPI
UsbfnEventHandler (
  IN      EFI_USBFN_IO_PROTOCOL      *This,
  OUT     EFI_USBFN_MESSAGE          *Message,
  IN OUT  UINTN                      *PayloadSize,
  OUT     EFI_USBFN_MESSAGE_PAYLOAD  *Payload
  );

/**
  * @brief Transfer data to or from the host on the specified endpoint
  *
  * See UpdateDeviceInfo for an overall description of request hijacking.
  *
  * @param [in]      This            UsbfnIo protocol instance
  * @param [in]      EndpointIndex   Endpoint to submit transfer on
  * @param [in]      Direction       Direction of endpoint
  * @param [in,out]  BufferSize      Size of buffer
  * @param [in,out]  Buffer          Buffer to read from/write to
  *
  * @retval EFI_SUCCESS          Successfully submitted transfer
  * @retval Others               Failed to submit transfer
  */
EFI_STATUS
EFIAPI
UsbfnTransfer (
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN OUT  UINTN                         *BufferSize,
  IN OUT  VOID                          *Buffer
  );

/**
  * @brief Returns the maximum number of bytes that a single transfer
  *        can accommodate
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [out] MaxTransferSize Maximum supported transfer size
  *
  * @retval EFI_SUCCESS          MaxTransferSize is valid
  * @retval Others               Error
  */
EFI_STATUS
EFIAPI
UsbfnGetMaxTransferSize (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT UINTN                  *MaxTransferSize
  );

/**
  * @brief Allocate a buffer to satisfy any requirements of the controller
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [in]  Size            Number of bytes to allocate
  * @param [out] Buffer          Returned buffer pointer on success
  *
  * @retval EFI_SUCCESS          Buffer was allocated successfully
  * @retval Others               Error
  */
EFI_STATUS
EFIAPI
UsbfnAllocateTransferBuffer (
  IN   EFI_USBFN_IO_PROTOCOL  *This,
  IN   UINTN                  Size,
  OUT  VOID                   **Buffer
  );

/**
  * @brief Free a buffer allocated with UsbfnAllocateTransferBuffer
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [in]  Buffer          Buffer to free
  *
  * @retval EFI_SUCCESS          Buffer freed
  * @retval Others               Invalid buffer pointer
  */
EFI_STATUS
EFIAPI
UsbfnFreeTransferBuffer (
  IN EFI_USBFN_IO_PROTOCOL  *This,
  IN VOID                   *Buffer
  );

/**
  * @brief Initialize the hardware, but must not activate the port
  *
  * @param [in]  This            UsbfnIo protocol instance
  *
  * @retval EFI_SUCCESS          Successfully initialized controller
  * @retval Others               Failed to initialize controller
  */
EFI_STATUS
EFIAPI
UsbfnStartController (
  IN EFI_USBFN_IO_PROTOCOL  *This
  );

/**
  * @brief Disable the hardware
  *
  * @param [in]  This            UsbfnIo protocol instance
  *
  * @retval EFI_SUCCESS          Successfully disabled the controller
  * @retval Others               Failed to disable the controller
  */
EFI_STATUS
EFIAPI
UsbfnStopController (
  IN EFI_USBFN_IO_PROTOCOL  *This
  );

/**
  * @brief Sets policies supported by the USB endpoint
  *
  * This function must be called before StartController or after StopController.
  *
  * @param [in]  This            UsbfnIo protocol instance
  * @param [in]  EndpointIndex   Endpoint to set policy on
  * @param [in]  Direction       Direction of endpoint
  * @param [in]  PolicyType      Policy to be applied
  * @param [in]  BufferSize      Length of policy buffer
  * @param [in]  Buffer          Buffer holding policy value
  *
  * @retval EFI_SUCCESS          Policy successfully applied
  * @retval Others               Failed to apply policy
  */
EFI_STATUS
EFIAPI
UsbfnSetEndpointPolicy(
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         PolicyType,
  IN      UINTN                         BufferSize,
  IN      VOID                          *Buffer
  );

/**
  * @brief Gets policies supported by the endpoint
  *
  * @param [in]     This          UsbfnIo protocol instance
  * @param [in]     EndpointIndex Endpoint to get policy on
  * @param [in]     Direction     Direction of endpoint
  * @param [in]     PolicyType    Policy to return
  * @param [in,out] BufferSize    On input, length of buffer, on output,
  *                               length of data returned in Buffer
  * @param [out]    Buffer        Buffer to hold policy value
  *
  * @retval EFI_SUCCESS           Policy successfully returned
  * @retval Others                Failed to return policy
  */
EFI_STATUS
EFIAPI
UsbfnGetEndpointPolicy(
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         PolicyType,
  IN OUT  UINTN                         *BufferSize,
  OUT     VOID                          *Buffer
  );
#endif  //USBFN_H
