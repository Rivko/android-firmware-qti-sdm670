/**
  @file UsbIo.h
  @brief EFI Usb I/O Protocol as defined in UEFI specification.

  This protocol is used by code, typically drivers, running in the EFI 
  boot services environment to access USB devices like USB keyboards, 
  mice and mass storage devices. In particular, functions for managing devices 
  on USB buses are defined here.
  
  Copyright (c) 2012-2015 Qualcomm Technologies, Inc. All rights reserved.
  Portions Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*/
/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 11/23/15   ck       Move Qualcomm added type definitions from EFIUsbIo.h to EFIUsbfnIo.h
 08/21/14   ck       Add device qualifier descriptor typedef
 04/30/14   ar       Cleanup for NDEBUG build  
 02/05/14   amitg    Async USB Host Stack
 02/03/14   llg      (Tech Pubs) Edited/added Doxygen comments and markup.
 08/27/13   llg      (Tech Pubs) Edited/added Doxygen comments and markup.
 08/23/13   ml       Added USB 3.0 descriptors
 09/27/12   llg      (Tech Pubs) Edited/added Doxygen comments and markup.

=============================================================================*/

#ifndef __USB_IO_H__
#define __USB_IO_H__

#include <IndustryStandard/Usb.h>


/* Global ID for the USB I/O Protocol */
/** @ingroup efi_usbIo_protocol */
#define EFI_USB_IO_PROTOCOL_GUID \
  { \
    0x2B2F68D6, 0x0CD2, 0x44cf, {0x8E, 0x8B, 0xBB, 0xA2, 0x0B, 0x1B, 0x5B, 0x75 } \
  }

/** @cond */
typedef struct _EFI_USB_IO_PROTOCOL   EFI_USB_IO_PROTOCOL;
/** @endcond */


/* Related definition for EFI USB I/O protocol */

//
// USB HS Test Mode for compliance testing
//
typedef enum {
  USB_TEST_DISABLE,
  USB_TEST_J_STATE,
  USB_TEST_K_STATE,
  USB_TEST_SE0_NAK,
  USB_TEST_PACKET,
  USB_TEST_FORCE_ENABLE,
  USB_TEST_GET_DISCONNECT_STATUS,
  USB_TEST_DISABLE_PERIODIC,
  USB_TEST_ENABLE_PERIODIC,
  USB_TEST_MODE_MAX
} USB_TEST_MODE;


/* USB standard descriptors and request */
/** @addtogroup efi_usb_io_data_types 
@{ */
/** USB device request; used by #EFI_USB_IO_CONTROL_TRANSFER(). */
typedef USB_DEVICE_REQUEST                    EFI_USB_DEVICE_REQUEST;
/** USB Device descriptor; used by #EFI_USB_IO_GET_DEVICE_DESCRIPTOR(). */
typedef USB_DEVICE_DESCRIPTOR                 EFI_USB_DEVICE_DESCRIPTOR;
/** USB Configuration descriptor; used by #EFI_USB_IO_GET_CONFIG_DESCRIPTOR(). */
typedef USB_CONFIG_DESCRIPTOR                 EFI_USB_CONFIG_DESCRIPTOR;
/** USB Interface descriptor; used by #EFI_USB_IO_GET_INTERFACE_DESCRIPTOR(). */
typedef USB_INTERFACE_DESCRIPTOR              EFI_USB_INTERFACE_DESCRIPTOR;
/** USB Endpoint descriptor; used by #EFI_USB_IO_GET_ENDPOINT_DESCRIPTOR(). */
typedef USB_ENDPOINT_DESCRIPTOR               EFI_USB_ENDPOINT_DESCRIPTOR;


/**  USB data transfer direction. */
typedef enum {
  EfiUsbDataIn,   /**< In. */
  EfiUsbDataOut,  /**< Out. */
  EfiUsbNoData    /**< No data. */
} EFI_USB_DATA_DIRECTION;
/** @} */ /* end_addtogroup efi_usb_io_data_types */

/*  USB Transfer Results */
/** @addtogroup efi_usb_io_constants 
@{ */
/** USB transfer result: no error; operation was successful. */
#define EFI_USB_NOERROR             0x00
/** USB transfer result: operation did not execute. */
#define EFI_USB_ERR_NOTEXECUTE      0x01
/** USB transfer result: operation stalled. */
#define EFI_USB_ERR_STALL           0x02
/** USB transfer result: buffer error. */
#define EFI_USB_ERR_BUFFER          0x04
/** USB transfer result: babble error. */
#define EFI_USB_ERR_BABBLE          0x08
/** USB transfer result: no acknowledgement. */
#define EFI_USB_ERR_NAK             0x10
/** USB transfer result: CRC error. */
#define EFI_USB_ERR_CRC             0x20
/** USB transfer result: timeout. */
#define EFI_USB_ERR_TIMEOUT         0x40
/** USB transfer result: bitstuff error. */
#define EFI_USB_ERR_BITSTUFF        0x80
 /** USB transfer result: system error. */
#define EFI_USB_ERR_SYSTEM          0x100
/** USB transfer result: split transfer error. */
#define EFI_USB_ERR_SPLIT_TRANSFER  0x200
/** USB transfer result: memory alignment error */
#define EFI_USB_MEM_ALIGN           0x400
/** @} */ /* end_addtogroup efi_usb_io_constants */

/*===========================================================================
  CALLBACK FUNCTIONS 
===========================================================================*/
/* EFI_ASYNC_USB_TRANSFER_CALLBACK */
/** @ingroup efi_usb_io_callback_functions
  @par Summary
  Asynchronous USB transfer callback routine for the client.

  @param[in]  Data        Data received or sent via the USB asynchronous 
                          transfer if the transfer completed successfully.
  @param[in]  DataLength  Length of Data received or sent via the asynchronous
                          transfer if the transfer completed successfully.
  @param[in]  Context     Data passed from the 
                          EFI_USB_IO_ASYNC_ISOCHRONOUS_TRANSFER() request.
  @param[in]  Status      Indicates the result of the asynchronous transfer.

  @return
  EFI_SUCCESS      -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Physical device reported an error. @newpage
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ASYNC_USB_TRANSFER_CALLBACK)(
  IN VOID         *Data,
  IN UINTN        DataLength,
  IN VOID         *Context,
  IN UINT32       Status
  );

/* EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK */
/** @ingroup efi_usb_io_callback_functions
  @par Summary
  Asynchronous USB transfer callback routine for interrupt and bulk transfers.

  @param[in]  Data        Data received or sent via the USB asynchronous 
                          transfer if the transfer completed successfully.
  @param[in]  DataLength  Length of Data received or sent via the asynchronous
                          transfer if the transfer completed successfully.
  @param[in]  Context     Data passed from the 
                          EFI_USB_IO_ASYNC_INTERRUPT_TRANSFER() or 
                          EFI_USB_IO_ASYNC_BULK_TRANSFER() request.
  @param[in]  Status      Indicates the result of the asynchronous transfer.
  @param[in]  Toggle      Data toggle value after the data transferred.

  @return
  EFI_SUCCESS      -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Physical device reported an error.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK)(
  IN VOID         *Data,
  IN UINTN        DataLength,
  IN VOID         *Context,
  IN UINT32       Status,
  IN UINT8        Toggle
  );


/* Prototype for EFI USB I/O protocol */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_USB_IO_CONTROL_TRANSFER */
/** @ingroup efi_usb_io_control_transfer
  @par Summary
  Manages a USB device with a control transfer pipe. A control transfer is
  typically used to perform device initialization and configuration.

  @param[in]  This        Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]  Request     Pointer to #EFI_USB_DEVICE_REQUEST that is sent to 
                          the USB device.
  @param[in]  Direction   Data direction; see #EFI_USB_DATA_DIRECTION for 
                          details.
  @param[in]  Timeout     Transfer timeout in milliseconds.
  @param[in]  Data        Pointer to the data buffer that is transmitted to 
                          or received from the USB device.
  @param[in]  DataLength  Size (in bytes) of the data buffer specified by Data.
  @param[out] Status      USB transfer status; see 
                          Section @xref{dox:usbIoConstants} for descriptions.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Transfer failed; the transfer status is returned in 
                           Status. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of
                           resources. \n
  EFI_TIMEOUT           -- Control transfer failed due to timeout.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_CONTROL_TRANSFER)(
  IN EFI_USB_IO_PROTOCOL                        *This,
  IN EFI_USB_DEVICE_REQUEST                     *Request,
  IN EFI_USB_DATA_DIRECTION                     Direction,
  IN UINT32                                     Timeout,
  IN OUT VOID                                   *Data OPTIONAL,
  IN UINTN                                      DataLength  OPTIONAL,
  OUT UINT32                                    *Status
  );

/* EFI_USB_IO_ASYNC_CONTROL_TRANSFER */
/** @ingroup efi_usb_io_async_bulk_transfer
  @par Summary
  Manages a USB device with an asynchronous bulk transfer pipe. Bulk transfers 
  are typically used to transfer large amounts of data to/from USB devices.


  @param[in]       This             Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]       Request          Pointer to #EFI_USB_DEVICE_REQUEST that is sent to 
                                        the USB device.
  @param[in]                        PollingInterval  Periodic rate (in milliseconds) that the transfer 
                                        is to be executed. The value must be between 
                                        1 to 255, otherwise EFI_INVALID_PARAMETER is 
                                        returned. 
  @param[in]       Direction        Data direction; see #EFI_USB_DATA_DIRECTION for 
                                       details.
  @param[in, out]  Data             Pointer to the data buffer that is transmitted 
                                         to or received from the USB device.
  @param[in, out]  DataLength       Data buffer size or the number of transferred 
                                         bytes. 
                                      - On input, the size (in bytes) of the data 
                                        buffer specified by Data
                                      - On output, the number of bytes that were 
                                         actually transferred @tablebulletend
  @param[in]       BulkCallBack     Callback function 
                                        #EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK. 
                                        This function is called if the asynchronous
                                         bulk transfer is completed. 
  @param[in]       Context          Data passed to the bulk callback function 
                                         #EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK. This 
                                         is an optional parameter and may be NULL.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Transfer failed; the transfer status is returned in 
                           Status. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. \n
  EFI_TIMEOUT           -- Control transfer failed due to timeout.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_ASYNC_CONTROL_TRANSFER)(
  IN EFI_USB_IO_PROTOCOL            *This,
  IN  EFI_USB_DEVICE_REQUEST        *Request,
  IN UINTN                          PollingInterval    OPTIONAL,
  IN EFI_USB_DATA_DIRECTION         Direction,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      *DataLength,
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK BulkCallBack  OPTIONAL,
  IN VOID                           *Context OPTIONAL
  );




/* EFI_USB_IO_BULK_TRANSFER */
/** @ingroup efi_usb_io_bulk_transfer
  @par Summary
  Manages a USB device with a bulk transfer pipe. Bulk transfers are
  typically used to transfer large amounts of data to/from USB devices.

  @param[in]  This            Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]  DeviceEndpoint  Destination USB device endpoint to which the 
                              device request is being sent. DeviceEndpoint must 
                              be between 0x01 and 0x0F or between 0x81 and 0x8F, 
                              otherwise EFI_INVALID_PARAMETER is returned. If 
                              the endpoint is not a bulk endpoint, 
                              EFI_INVALID_PARAMETER is returned. The MSB of 
                              this parameter indicates the endpoint direction; 
                              1 stands for an IN endpoint, and 0 stands for 
                              an OUT endpoint.
  @param[in]  Data            Pointer to the data buffer that is transmitted 
                              to or received from the USB device.
  @param[in]  DataLength      Data buffer size or the number of transferred 
                              bytes. 
                              - On input, the size (in bytes) of the data 
                                buffer specified by Data
                              - On output, the number of bytes that were 
                                actually transferred @tablebulletend
  @param[in]  Timeout         Transfer timeout (in milliseconds). If Timeout 
                              is 0, the caller must wait for the function to be 
                              completed, i.e., until EFI_SUCCESS or 
                              EFI_DEVICE_ERROR is returned.
  @param[out] Status          USB transfer status; see 
                              Section @xref{dox:usbIoConstants} for 
                              descriptions.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Transfer failed; the transfer status is returned in 
                           Status. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. \n
  EFI_TIMEOUT           -- Control transfer failed due to timeout.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_BULK_TRANSFER)(
  IN EFI_USB_IO_PROTOCOL            *This,
  IN UINT8                          DeviceEndpoint,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      *DataLength,
  IN UINTN                          Timeout,
  OUT UINT32                        *Status
  );

/* EFI_USB_IO_ASYNC_BULK_TRANSFER */
/** @ingroup efi_usb_io_async_bulk_transfer
  @par Summary
  Manages a USB device with an asynchronous bulk transfer pipe. Bulk transfers 
  are typically used to transfer large amounts of data to/from USB devices.


  @param[in]  This             Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]  DeviceEndpoint   Destination USB device endpoint to which the 
                               device request is being sent. DeviceEndpoint must 
                               be between 0x01 and 0x0F or between 0x81 and 0x8F, 
                               otherwise EFI_INVALID_PARAMETER is returned. If 
                               the endpoint is not a bulk endpoint, 
                               EFI_INVALID_PARAMETER is returned. The MSB of 
                               this parameter indicates the endpoint direction; 
                               1 stands for an IN endpoint, and 0 stands for 
                               an OUT endpoint.
  @param[in]  PollingInterval  Periodic rate (in milliseconds) that the transfer 
                               is to be executed. The value must be between 
                               1 to 255, otherwise EFI_INVALID_PARAMETER is 
                               returned. 
  @param[in]  Data             Pointer to the data buffer that is transmitted 
                               to or received from the USB device.
  @param[in]  DataLength       Data buffer size or the number of transferred 
                               bytes. 
                               - On input, the size (in bytes) of the data 
                                 buffer specified by Data
                               - On output, the number of bytes that were 
                                 actually transferred @tablebulletend
  @param[in]  BulkCallBack     Callback function 
                               EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK(). 
                               This function is called if the asynchronous
                               bulk transfer is completed. 
  @param[in]  Context          Data passed to the bulk callback function 
                               EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK(). This 
                               is an optional parameter and may be NULL.

  @newpage
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_DEVICE_ERROR      -- Transfer failed. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_ASYNC_BULK_TRANSFER)(
  IN EFI_USB_IO_PROTOCOL            *This,
  IN UINT8                          DeviceEndpoint,
  IN UINTN                          PollingInterval    OPTIONAL,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      *DataLength,
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK BulkCallBack  OPTIONAL,
  IN VOID                           *Context OPTIONAL
  );
  
/* EFI_USB_IO_ASYNC_INTERRUPT_TRANSFER */
/** @ingroup efi_usb_io_async_interrupt_transfer
  @par Summary
  Manages a USB device with an asynchronous interrupt transfer pipe. An 
  asynchronous interrupt transfer is typically used to query a device's status 
  at a fixed rate. For example, keyboard, mouse, and hub devices use this type 
  of transfer to query their interrupt endpoints at a fixed rate.

  @param[in]  This             Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]  DeviceEndpoint   Destination USB device endpoint to which the 
                               device request is being sent. DeviceEndpoint must 
                               be between 0x01 and 0x0F or between 0x81 and 0x8F, 
                               otherwise EFI_INVALID_PARAMETER is returned. If 
                               the endpoint is not a bulk endpoint, 
                               EFI_INVALID_PARAMETER is returned. The MSB of 
                               this parameter indicates the endpoint direction; 
                               1 stands for an IN endpoint, and 0 stands for 
                               an OUT endpoint.
  @param[in]  IsNewTransfer    If TRUE, a new transfer is submitted to the USB 
                               controller. If FALSE, the interrupt transfer is 
                               deleted from the device's interrupt transfer 
                               queue.
  @param[in]  PollingInterval  Periodic rate (in milliseconds) that the transfer 
                               is to be executed. This parameter is required 
                               when IsNewTransfer is TRUE. The value must be 
                               between 1 to 255, otherwise EFI_INVALID_PARAMETER 
                               is returned. 
  @param[in]  DataLength         Specifies the length, in bytes, of the data to 
                                 be received from the USB device. This parameter 
                                 is only required when IsNewTransfer is TRUE.
  @param[in]  InterruptCallback  Callback function 
                                 EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK(). 
                                 This function is called if the asynchronous
                                 interrupt transfer is completed. This parameter 
                                 is required when IsNewTransfer is TRUE.
  @param[in]  Context            Data passed to the interrupt callback function 
                                 EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK(). 
                                 This is an optional parameter and may be NULL.

  @return
  EFI_SUCCESS      -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Transfer failed. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_ASYNC_INTERRUPT_TRANSFER)(
  IN EFI_USB_IO_PROTOCOL                                 *This,
  IN UINT8                                               DeviceEndpoint,
  IN BOOLEAN                                             IsNewTransfer,
  IN UINTN                                               PollingInterval    OPTIONAL,
  IN UINTN                                               DataLength         OPTIONAL,
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK                     InterruptCallBack  OPTIONAL,
  IN VOID                                                *Context OPTIONAL
  );

/* EFI_USB_IO_SYNC_INTERRUPT_TRANSFER */
/** @ingroup efi_usb_io_sync_interrupt_transfer
  @par Summary
  Manages a USB device with a synchronous interrupt transfer pipe.

  @param[in]  This            Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]  DeviceEndpoint  Destination USB device endpoint to which the 
                              device request is being sent. DeviceEndpoint must 
                              be between 0x01 and 0x0F or between 0x81 and 0x8F, 
                              otherwise EFI_INVALID_PARAMETER is returned. If 
                              the endpoint is not a bulk endpoint, 
                              EFI_INVALID_PARAMETER is returned. The MSB of 
                              this parameter indicates the endpoint direction; 
                              1 stands for an IN endpoint, and 0 stands for 
                              an OUT endpoint.
  @param[in]  Data            Pointer to the data buffer that is transmitted 
                              to or received from the USB device.
  @param[in]  DataLength      Data buffer size or the number of transferred 
                              bytes.
                              - On input, the size (in bytes) of the data 
                                buffer specified by Data
                              - On output, the number of bytes that were 
                                actually transferred @tablebulletend
  @param[in]  Timeout         Transfer timeout (in milliseconds). If Timeout 
                              is 0, the caller must wait for the function to be 
                              completed, i.e., until EFI_SUCCESS or 
                              EFI_DEVICE_ERROR is returned.
  @param[out] Status          USB transfer status; see 
                              Section @xref{dox:usbIoConstants} for 
                              descriptions.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. \n
  EFI_DEVICE_ERROR      -- Transfer failed; the transfer status is returned in 
                           Status. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. \n
  EFI_TIMEOUT           -- Transfer failed due to timeout.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_SYNC_INTERRUPT_TRANSFER)(
  IN EFI_USB_IO_PROTOCOL            *This,
  IN     UINT8                      DeviceEndpoint,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      *DataLength,
  IN     UINTN                      Timeout,
  OUT    UINT32                     *Status
  );

/* EFI_USB_IO_ISOCHRONOUS_TRANSFER */
/** @ingroup efi_usb_io_isochronous_transfer
  @par Summary
  Manages a USB device with an isochronous transfer pipe. An isochronous 
  transfer is typically used to transfer streaming data.

  @param[in]  This            Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]  DeviceEndpoint  Destination USB device endpoint to which the 
                              device request is being sent. DeviceEndpoint must 
                              be between 0x01 and 0x0F or between 0x81 and 0x8F, 
                              otherwise EFI_INVALID_PARAMETER is returned. If 
                              the endpoint is not a bulk endpoint, 
                              EFI_INVALID_PARAMETER is returned. The MSB of 
                              this parameter indicates the endpoint direction; 
                              1 stands for an IN endpoint, and 0 stands for 
                              an OUT endpoint.
  @param[in]  Data            Pointer to the data buffer that is transmitted 
                              to or received from the USB device.
  @param[in]  DataLength      Size (in bytes) of the data buffer specified by 
                              Data.
  @param[out] Status          USB transfer status; see 
                              Section @xref{dox:usbIoConstants} for descriptions.

  @return
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- DeviceEndpoint parameter is not valid. 
  @par
  EFI_DEVICE_ERROR      -- Transfer failed due to a reason other than timeout; 
                           the transfer status is returned in Status. 
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. 
  @par
  EFI_TIMEOUT           -- Transfer failed due to timeout.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_ISOCHRONOUS_TRANSFER)(
  IN EFI_USB_IO_PROTOCOL            *This,
  IN     UINT8                      DeviceEndpoint,
  IN OUT VOID                       *Data,
  IN     UINTN                      DataLength,
  OUT    UINT32                     *Status
  );

/* EFI_USB_IO_ASYNC_ISOCHRONOUS_TRANSFER */
/** @ingroup efi_usb_io_async_isochronous_transfer
  @par Summary
  Manages a USB device with a nonblocking isochronous transfer pipe. An 
  isochronous transfer is typically used to transfer streaming data.

  @param[in]  This            Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]  DeviceEndpoint  Destination USB device endpoint to which the 
                              device request is being sent. DeviceEndpoint must 
                              be between 0x01 and 0x0F or between 0x81 and 0x8F, 
                              otherwise EFI_INVALID_PARAMETER is returned. If 
                              the endpoint is not a bulk endpoint, 
                              EFI_INVALID_PARAMETER is returned. The MSB of 
                              this parameter indicates the endpoint direction; 
                              1 stands for an IN endpoint, and 0 stands for 
                              an OUT endpoint.
  @param[in]  Data            Pointer to the data buffer that is transmitted 
                              to or received from the USB device.
  @param[in]  DataLength      Size (in bytes) of the data buffer specified by 
                              Data.
  @param[in]  IsochronousCallback  Callback function 
                                   #EFI_ASYNC_USB_TRANSFER_CALLBACK. This 
                                   function is called if the requested 
                                   isochronous transfer is completed.
  @param[in]  Context              Data passed to the isochronous callback 
                                   function EFI_ASYNC_USB_TRANSFER_CALLBACK(). 
                                   This is an optional parameter and may be 
                                   NULL.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- DeviceEndpoint parameter is not valid. \n
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_ASYNC_ISOCHRONOUS_TRANSFER)(
  IN EFI_USB_IO_PROTOCOL              *This,
  IN UINT8                            DeviceEndpoint,
  IN OUT VOID                         *Data,
  IN     UINTN                        DataLength,
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK  IsochronousCallBack,
  IN VOID                             *Context OPTIONAL
  );

/* EFI_USB_IO_PORT_RESET */
/** @ingroup efi_usb_io_port_reset
  @par Summary
  Resets and reconfigures the USB controller. This function works for all USB 
  devices except USB hub controllers.

  @param[in]  This  Pointer to the EFI_USB_IO_PROTOCOL instance.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Controller specified by the <i>This</i> parameter is
                           a USB hub. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_PORT_RESET)(
  IN EFI_USB_IO_PROTOCOL    *This
  );

/* EFI_USB_IO_GET_DEVICE_DESCRIPTOR */
/** @ingroup efi_usb_io_get_device_descriptor
  @par Summary
  Retrieves the USB device descriptor.

  @param[in]  This              Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[out] DeviceDescriptor  Pointer to the caller-allocated 
                                #EFI_USB_DEVICE_DESCRIPTOR.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- DeviceDescriptor parameter is NULL. \n
  EFI_NOT_FOUND         -- Device descriptor was not found; the device may not be 
                           configured correctly.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_DEVICE_DESCRIPTOR)(
  IN EFI_USB_IO_PROTOCOL            *This,
  OUT EFI_USB_DEVICE_DESCRIPTOR     *DeviceDescriptor
  );

/* EFI_USB_IO_GET_CONFIG_DESCRIPTOR */
/** @ingroup efi_usb_io_get_config_descriptor
  @par Summary
  Retrieves the USB configuration descriptor.

  @param[in]  This                     Pointer to the EFI_USB_IO_PROTOCOL 
                                       instance.
  @param[out] ConfigurationDescriptor  Pointer to the caller-allocated 
                                       #EFI_USB_CONFIG_DESCRIPTOR.

  @return
  EFI_SUCCESS           -- Function completed successfully.
  @par
  EFI_INVALID_PARAMETER -- ConfigurationDescriptor parameter is NULL.
  @par
  EFI_NOT_FOUND         -- Active configuration descriptor was not found; the 
                           device may not be configured correctly.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_CONFIG_DESCRIPTOR)(
  IN EFI_USB_IO_PROTOCOL            *This,
  OUT EFI_USB_CONFIG_DESCRIPTOR     *ConfigurationDescriptor
  );

/* EFI_USB_IO_GET_INTERFACE_DESCRIPTOR */
/** @ingroup efi_usb_io_get_interface_descriptor
  @par Summary
  Retrieves the interface descriptor for a USB device controller. An interface 
  within a USB device is equivalent to a USB controller within the current 
  configuration.

  @param[in]  This                 Pointer to the EFI_USB_IO_PROTOCOL 
                                   instance.
  @param[out] InterfaceDescriptor  Pointer to the caller-allocated 
                                   #EFI_USB_INTERFACE_DESCRIPTOR within the 
                                   configuration setting.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- InterfaceDescriptor parameter is NULL. \n
  EFI_NOT_FOUND         -- Interface descriptor was not found; the device may not 
                           be configured correctly.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_INTERFACE_DESCRIPTOR)(
  IN EFI_USB_IO_PROTOCOL            *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR  *InterfaceDescriptor
  );

/* EFI_USB_IO_GET_ENDPOINT_DESCRIPTOR */
/** @ingroup efi_usb_io_get_endpoint_descriptor
  @par Summary
  Retrieves an endpoint descriptor within a USB controller.

  @param[in]  This                Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]  EndpointIndex       Indicates which endpoint descriptor to 
                                  retrieve.
  @param[out] EndpointDescriptor  Pointer to the caller-allocated 
                                  #EFI_USB_ENDPOINT_DESCRIPTOR of a USB 
                                  controller.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- One or more parameters are invalid. \n
  EFI_NOT_FOUND         -- Endpoint descriptor was not found; the device may not 
                           be configured correctly.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_ENDPOINT_DESCRIPTOR)(
  IN EFI_USB_IO_PROTOCOL            *This,
  IN  UINT8                         EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR   *EndpointDescriptor
  );

/* EFI_USB_IO_GET_STRING_DESCRIPTOR */
/** @ingroup efi_usb_io_get_string_descriptor
  @par Summary
  Retrieves a string stored in a USB device.

  @param[in]  This      Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[in]  LangID    Language ID for the string being retrieved.
  @param[in]  StringID  ID of the string being retrieved.
  @param[out] String    Pointer to a buffer allocated by this function with
                        AllocatePool() to store the string. If this function 
                        returns EFI_SUCCESS, it stores the string the caller 
                        wants to obtain. The caller must release the string 
                        buffer with FreePool() after the string is not used 
                        any more.

  @return
  EFI_SUCCESS          -- Function completed successfully. \n
  EFI_NOT_FOUND        -- String specified by the LangID and StringID 
                          parameters was not found. \n
  EFI_OUT_OF_RESOURCES -- Not enough resources to allocate the return buffer 
                          string.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_STRING_DESCRIPTOR)(
  IN EFI_USB_IO_PROTOCOL            *This,
  IN  UINT16                        LangID,
  IN  UINT8                         StringID,
  OUT CHAR16                        **String
  );

/* EFI_USB_IO_GET_SUPPORTED_LANGUAGE */
/** @ingroup efi_usb_io_get_supported_language
  @par Summary
  Retrieves all the language ID codes that the USB device supports.

  @param[in]  This         Pointer to the EFI_USB_IO_PROTOCOL instance.
  @param[out] LangIDTable  Language ID for the string the caller wants to 
                           obtain. This is a 16-bit ID defined by Microsoft@regns. 
                           This buffer pointer is allocated and maintained by 
                           the USB bus driver; the caller must not modify its 
                           contents.
  @param[out] TableSize    Size (in bytes) of the table LangIDTable.

  @return
  EFI_SUCCESS      -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Physical device reported an error.

*/
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_SUPPORTED_LANGUAGE)(
  IN EFI_USB_IO_PROTOCOL            *This,
  OUT UINT16                        **LangIDTable,
  OUT UINT16                        *TableSize
  );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_usbIo_protocol
  @par Summary
  This protocol provides four basic transfers types described in the 
  @latexonly\textit{Universal Serial Bus Specification~2.0}@endlatexonly 
  @xhyperref{R3,[R3]}. These include control transfer, bulk transfer, 
  interrupt transfer, and isochronous transfer. The EFI_USB_IO_PROTOCOL 
  also provides basic USB device/controller management and configuration 
  interfaces. A USB device driver uses the services of this protocol to 
  manage USB devices.  

  @par Parameters
  @inputprotoparams{usb_io_proto_params.tex} 
*/
struct _EFI_USB_IO_PROTOCOL {
  //
  // IO transfer
  //
  EFI_USB_IO_CONTROL_TRANSFER           UsbControlTransfer;
  EFI_USB_IO_BULK_TRANSFER              UsbBulkTransfer;
  EFI_USB_IO_ASYNC_INTERRUPT_TRANSFER   UsbAsyncInterruptTransfer;
  EFI_USB_IO_SYNC_INTERRUPT_TRANSFER    UsbSyncInterruptTransfer;
  EFI_USB_IO_ISOCHRONOUS_TRANSFER       UsbIsochronousTransfer;
  EFI_USB_IO_ASYNC_ISOCHRONOUS_TRANSFER UsbAsyncIsochronousTransfer;

  //
  // Common device request
  //
  EFI_USB_IO_GET_DEVICE_DESCRIPTOR      UsbGetDeviceDescriptor;
  EFI_USB_IO_GET_CONFIG_DESCRIPTOR      UsbGetConfigDescriptor;
  EFI_USB_IO_GET_INTERFACE_DESCRIPTOR   UsbGetInterfaceDescriptor;
  EFI_USB_IO_GET_ENDPOINT_DESCRIPTOR    UsbGetEndpointDescriptor;
  EFI_USB_IO_GET_STRING_DESCRIPTOR      UsbGetStringDescriptor;
  EFI_USB_IO_GET_SUPPORTED_LANGUAGE     UsbGetSupportedLanguages;

  //
  // Reset controller's parent port
  //
  EFI_USB_IO_PORT_RESET                 UsbPortReset;

  EFI_USB_IO_ASYNC_BULK_TRANSFER        UsbAsyncBulkTransfer;
  EFI_USB_IO_ASYNC_CONTROL_TRANSFER     UsbAsyncControlTransfer;
};

extern EFI_GUID gEfiUsbIoProtocolGuid;

#endif
