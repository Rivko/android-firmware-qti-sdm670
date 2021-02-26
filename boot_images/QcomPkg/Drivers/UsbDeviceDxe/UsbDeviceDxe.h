/**
 * @file UsbDeviceDxe.h
 *
 * UsbDeviceDxe header file
 *
 * Defines various USB related constants and data transfer structures/functions.
 *
 * Copyright (c) 2014-2016 QUALCOMM Technologies Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/25/16   ck      Separate EDK2 undefined USB data structure to EFIUsbEx.h
 12/01/15   ck      Add USB SS and USBCV test support
 08/21/14   ck      Rename and remove unused constants and data structures
 08/18/14   ck      Add comments and remove unused code
 08/02/14   ck      Initial port from loopback app
=============================================================================*/

#ifndef _USB_DEVICE_DXE_H_
#define _USB_DEVICE_DXE_H_

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/EFIUsbfnIo.h>
#include <Protocol/EFIUsbEx.h>
// driver constants
#define USBDEV_CONTROL_EP              0
#define USBDEV_CONTROL_BUF_SIZE        (16 * 1024)

// Get High Bytes
#define USB_GET_HIGH_BYTE(WORD16) (((WORD16) & 0xFF00) >> 8)

// Get Low Bytes
#define USB_GET_LOW_BYTE(WORD16)  (((WORD16) & 0x00FF))

// Converts endpoint index and direction to address.
#define ENDPOINT_ADDR(EndpointIndex, Tx) \
  ((EndpointIndex) | ((Tx) ? 0x80 : 0x00))

// Get ep direction from USB request (see USB 3.0 ch 9.3)
#define USB_REQUEST_EPDIR(index)      (((index) >> 7 & 0x1) ? EfiUsbEndpointDirectionHostIn : \
                                                               EfiUsbEndpointDirectionHostOut)

// Get ep and dir from wIndex field of USB request (see USB 3.0 ch 9.3.4)
#define USB_INDEX_TO_EP(index)        ((index) & 0xf)
#define USB_INDEX_TO_EPDIR(index)     (((index) >> 7 & 0x1) ? EfiUsbEndpointDirectionHostIn : \
                                                               EfiUsbEndpointDirectionHostOut)

// Get descriptor type and index from wValue field of USB request (see USB 3.0 ch 9.4.3)
#define USB_VALUE_TO_DESC_TYPE(value) ((value) >> 8)
#define USB_VALUE_TO_DESC_IDX(value)  ((value) & 0xff)

// Other USB related constants/macros
#define USB_MAX_ENDPOINT_NUMBER       15

#define USB_SETUP_RT_TYPE_SHFT        5
#define USB_SETUP_RT_TYPE_BMSK        (0x3 << USB_SETUP_RT_TYPE_SHFT)
#define USB_SETUP_RT_TYPE(bmRT)       (((bmRT) & USB_SETUP_RT_TYPE_BMSK) >> USB_SETUP_RT_TYPE_SHFT)
#define USB_SETUP_RT_TYPE_STD         0
#define USB_SETUP_RT_TYPE_CLS         1
#define USB_SETUP_RT_TYPE_VND         2

#define USB_SETUP_RT_RCP_SHFT         0
#define USB_SETUP_RT_RCP_BMSK         (0x1f << USB_SETUP_RT_RCP_SHFT)
#define USB_SETUP_RT_RCP(bmRT)        (((bmRT) & USB_SETUP_RT_RCP_BMSK) >> USB_SETUP_RT_RCP_SHFT)
#define USB_SETUP_RT_RCP_DEV          0
#define USB_SETUP_RT_RCP_IFC          1
#define USB_SETUP_RT_RCP_EP           2
#define USB_SETUP_RT_RCP_OTH          3

// Transfer state
typedef enum XFER_STATE_TYPE {
  XFER_FREE,             // unused, but cannot be queued
  XFER_PENDING_ENQUEUE,  // the transfer is waiting to be queued
  XFER_PENDING_COMPLETE, // waiting for queued transfer to complete
} XFER_STATE_TYPE;


// Transfer type
typedef struct _XFER_TYPE {
  UINT8                         EndpointIndex; // endpoint of transfer
  EFI_USBFN_ENDPOINT_DIRECTION  Direction;     // direction of transfer
  XFER_STATE_TYPE               XferState;     // state of transfer
  UINT8                         *Buffer;       // application supplied data buffer
  UINTN                         BufferSize;    // size of data buffer
  UINTN                         BufferOffset;  // offset into buffer of data start
  UINTN                         XferSize;      // size of data in buffer to transfer
} XFER_TYPE;


/**
 * Queues a transfer to the Usbfn driver.
 *
 * @param[in]     UsbfnIo   UsbfnIo protocol instance
 * @param[in,out] Xfer      The transfer to be queued
 *
 * @retval Status from Usbfn
 */
EFI_STATUS
IssueXfer (
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
  );


/**
 * Aborts a transfer that has been submitted to Usbfn.
 *
 * @param[in]     UsbfnIo   UsbfnIo protocol instance
 * @param[in,out] Xfer      The transfer to be aborted
 *
 * @retval Status from Usbfn
 */
EFI_STATUS
AbortXfer (
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
  );

#endif //_USB_DEVICE_DXE_H_

