/** @file UsbMsdXfer.h

The header file for UsbMsdXfer.c

Copyright (c) 2014,2016 QUALCOMM Technologies Inc. All rights reserved.

**/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/25/16   ck      Separate EDK2 undefined USB data structure to EFIUsbEx.h
10/06/14   ck      Initial version
=============================================================================*/
#ifndef __USB_MSD_XFER_H__
#define __USB_MSD_XFER_H__

#include <Protocol/EFIUsbfnIo.h>  // EFI_USBFN_IO_PROTOCOL
#include <Protocol/EFIUsbEx.h>    // USB data structure

// state of Xfer
typedef enum _XFER_STATE_TYPE {
  XFER_FREE,              // Unused, but cannot be queued.
  XFER_PENDING_ENQUEUE,   // The transfer is waiting to be queued in hw.
  XFER_PENDING_COMPLETE   // The transfer has been process by hw. waiting for further application processed, if any
} XFER_STATE_TYPE;

// Transfer Type
typedef struct _XFER_TYPE {
  UINT8                          EndpointIndex;         // endpoint of transfer
  UINT8                          EndpointDirection;     // direction of transfer
  XFER_STATE_TYPE                XferState;             // state of transfer
  UINT8                         *Buffer;                // application supplied data buffer
  UINTN                          BufferSize;            // size of data buffer
  UINTN                          BufferOffset;          // offset into buffer of data start
  UINTN                          XferSize;              // size of data in buffer to transfer
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
IssueXfer(
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
AbortXfer(
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
);

#endif // __USB_MSD_XFER_H__
