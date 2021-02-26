/** @file UsbMsdXfer.c

Implement the wrapper functions for USB transfer and abort.

Copyright (c) 2014, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

**/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/06/14   ck      Initial Version
=============================================================================*/

#include "UsbMsdXfer.h"    // header
#include "UsbMsdUtility.h" // FNC_*_MSG 

/*
* See header for documentation.
*/
EFI_STATUS
IssueXfer(
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
)
{
  EFI_STATUS        Status     = EFI_SUCCESS;
  UINT8            *Buffer     = NULL;

  FNC_ENTER_MSG();

  if (NULL == UsbfnIo || NULL == Xfer)
  {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // verify Xfer is not active
  if (XFER_PENDING_COMPLETE == Xfer->XferState)
  {
    DBG(EFI_D_ERROR, "Transfer on EP 0x%02x active, not submitting",
      ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->EndpointDirection));
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  Buffer   = Xfer->Buffer;
  Buffer  += Xfer->BufferOffset;  
  Status = UsbfnIo->Transfer(
    UsbfnIo,
    Xfer->EndpointIndex,
    (EFI_USBFN_ENDPOINT_DIRECTION)Xfer->EndpointDirection,
    &Xfer->XferSize,
    Buffer);
  if (EFI_ERROR(Status))
  {
    DBG(EFI_D_ERROR, "Failed to submit transfer on EP 0x%02x: %r",
      ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->EndpointDirection),
      Status);
    goto ON_EXIT;
  }

  DBG(EFI_D_VERBOSE, "Transfer submitted on EP 0x%02x, pending completion. UsbfnIo %p, Buffer %p, OffSet %d, XferSize %d",
    ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->EndpointDirection),
    UsbfnIo,
    Buffer,
    Xfer->BufferOffset, 
    Xfer->XferSize
    );

  Xfer->XferState = XFER_PENDING_COMPLETE;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
* See header for documentation.
*/
EFI_STATUS
AbortXfer(
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  if (NULL == UsbfnIo || NULL == Xfer) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // Do nothing if the transfer hasn't been submitted
  if (Xfer->XferState != XFER_PENDING_COMPLETE) {
    DBG(EFI_D_VERBOSE, "Transfer on EP 0x%02x not active, nothing to do",
      ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->EndpointDirection));
    goto ON_EXIT;
  }

  Status = UsbfnIo->AbortTransfer(
    UsbfnIo,
    Xfer->EndpointIndex,
    (EFI_USBFN_ENDPOINT_DIRECTION)Xfer->EndpointDirection);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to abort transfer on EP 0x%02x: %r",
      ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->EndpointDirection),
      Status);
  }

  DBG(EFI_D_VERBOSE, "Transfer on EP 0x%02x aborted, free",
    ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->EndpointDirection));
  Xfer->XferState = XFER_FREE;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}