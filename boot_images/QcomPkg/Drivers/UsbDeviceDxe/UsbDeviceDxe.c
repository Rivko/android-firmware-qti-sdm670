/**
 * @file UsbDeviceDxe.c
 *
 * Implement the USB Device Protocol.
 *
 * Copyright (c) 2014-2017 Qualcomm Technologies Inc. All rights reserved. 
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/01/17   pm      Port from BOOT.XF.1.4, Report aborted status for the client
 03/31/16   ck      Allow AllocateBuffer to be used before Start or StartEx
 01/22/16   ck      Support on demand USBFn configuration
 12/01/15   ck      Add USB SS and USBCV test support
 01/15/15   ck      Added support of USB enumeration for charger app
 10/26/14   vk      Remove inline
 09/17/14   ck      Update for the new USB Device header
 08/21/14   ck      Allow client to set more descriptors
 08/18/14   ck      Add comments and remove unused code
 08/15/14   ml      Clean up
 08/02/14   ck      Initial revision
=============================================================================*/
#include <Uefi.h>
#include <Protocol/EFIUsbfnIo.h>
#include <Library/MemoryAllocationLib.h>

#include "UsbDeviceDxe.h"
#include "UsbDevSm.h"
#include "UsbDevUtil.h"
#include "UsbDevDesc.h"

extern EFI_GUID gEfiUsbDeviceOemDataChargerTypeGuid;
extern EFI_GUID gEfiUsbDeviceOemDataSetupPacketGuid;
extern EFI_GUID gEfiUsbDeviceOemDataMaxPktSizeGuid;

// USBfnIo Protocol Handle
STATIC EFI_USBFN_IO_PROTOCOL            *gUsbfnIo      = NULL;
STATIC EFI_USB_DEVICE_INFO              gDeviceInfo   = {0};
STATIC EFI_USB_SUPERSPEED_DEVICE_INFO   gSSDeviceInfo = {0};
//
// EFIUsbDevice Implementation
//

/*
* @brief Implement USB_DEVICE_START in EFIUsbDevice.h
*
* Start the USBFn controller.
*
* @see EFIUsbDevice.h
*
* @param[in] DeviceDescriptor           : see EFIUsbDevice.h
* @param[in] Descriptors                : see EFIUsbDevice.h
* @param[in] DeviceQualifierDescriptor  : see EFIUsbDevice.h
* @param[in] BinaryDeviceOjectStore     : see EFIUsbDevice.h
*  (optional) Defaults to NULL. 
* @param[in] StringDescriptorCount      : see EFIUsbDevice.h
* @param[in] StringDescritors           : see EFIUsbDevice.h
*  (optional) Default to NULL. If not NULL, StringDescriptorCount must be greater
*             than zero.
* @retval                               : see EFIUsbDevice.h
*/
EFI_STATUS
UsbDeviceStart(
  IN USB_DEVICE_DESCRIPTOR                 *DeviceDescriptor,
  IN VOID                                 **Descriptors,
  IN USB_DEVICE_QUALIFIER_DESCRIPTOR       *DeviceQualifierDescriptor,
  IN VOID                                  *BinaryDeviceOjectStore, OPTIONAL
  IN UINT8                                  StringDescriptorCount,
  IN EFI_USB_STRING_DESCRIPTOR            **StringDescritors        OPTIONAL);


/*
* @brief Implement USB_DEVICE_START_EX in EFIUsbDevice.h
*
* Start the USBFn controller.
*
* @see EFIUsbDevice.h
*
* @param[in] UsbDevDescSet           : see EFIUsbDevice.h
*
* @retval                            : see EFIUsbDevice.h
*/
EFI_STATUS
UsbDeviceStartEx(
  IN USB_DEVICE_DESCRIPTOR_SET         *UsbDevDescSet);


/*
* @brief Implement USB_DEVICE_STOP in EFIUsbDevice.h
*
* Stop the USB controller and de-allocate resources.
*
* @see EFIUsbDevice.h
*
* @retval                               : see EFIUsbDevice.h
*
*/
EFI_STATUS
UsbDeviceStop(VOID);

/*
* @brief Implement USB_DEVICE_FREE_TRANSFER_BUFFER in EFIUsbDevice.h
*
* Free buffer allocated by USB_DEVICE_ALLOCATE_TRANSFER_BUFFER
*
* @see EFIUsbDevice.h
*
* @param[in] Buffer    : see EFIUsbDevice.h
* @retval              : see EFIUsbDevice.h
*/
EFI_STATUS
UsbDeviceFreeTransferBuffer(
  IN VOID *Buffer);


/*
* @brief Implement USB_DEVICE_HANDLE_EVENT in EFIUsbDevice.h
*
* The client calls the function periodically to obtain the events
* generated by the USB function controller.
*
* @see EFIUsbDevice.h
*
* @param[out]     Event           : see EFIUsbDevice.h
* @param[in,out]  EventDataSize   : see EFIUsbDevice.h
* @param[out]     EventData       : see EFIUsbDevice.h
* @retval                     : see EFIUsbDevice.h
*/
EFI_STATUS
UsbDeviceHandleEvent(
  OUT       USB_DEVICE_EVENT            *Event,
  OUT       UINTN                       *EventDataSize,
  OUT       USB_DEVICE_EVENT_DATA       *EventData);


/*
* @brief Implement USB_DEVICE_ALLOCATE_TRANSFER_BUFFER in EFIUsbDevice.h
*
* Allocate data transfer buffer. It is the caller's responsibility to de-allocate the buffer
* when it is no longer needed. The buffer must be de-allocated use USB_DEVICE_FREE_TRANSFER_BUFFER
*
* @see EFIUsbDevice.h
*
* @param[in]  Size    : see EFIUsbDevice.h
* @param[out] Buffer  : see EFIUsbDevice.h
* @retval             : see EFIUsbDevice.h
*/
EFI_STATUS
UsbDeviceAllocateBuffer(
  IN   UINTN      Size,
  OUT  VOID       **Buffer);


/*
* @brief Implement USB_DEVICE_SEND in EFIUsbDevice.h
*
* Queue the transfer of the client to the hardware. The client needs to call
* USB_DEVICE_HANDLE_EVENT periodically to obtain the completion notice.
*
* @see EFIUsbDevice.h
*
* @param[in]  EndpointIndex    : see EFIUsbDevice.h
* @param[in]  Size             : see EFIUsbDevice.h
* @param[in]  Buffer           : see EFIUsbDevice.h
*
* @retval                      : see EFIUsbDevice.h
*/
EFI_STATUS
UsbDeviceSend(
  IN       UINT8    EndpointIndex,
  IN       UINTN    Size,
  IN       VOID    *Buffer);


/*
* @brief Implement USB_DEVICE_ABORT_XFER in EFIUsbDevice.h
*
* Abort the transfer on EndpointIndex
*
* @see EFIUsbDevice.h
*
* @param[in]  EndpointIndex    : see EFIUsbDevice.h
*
* @retval                      : see EFIUsbDevice.h
*/
EFI_STATUS
UsbDeviceAbortXfer(
  IN       UINT8    EndpointIndex);


/*
* @brief Implement USB_DEVICE_SET_ENDPOINT_STALL_STATE in EFIUsbDevice.h
*
* Set the stall state for EndpointIndex 
*
* @see EFIUsbDevice.h
*
* @param[in]  EndpointIndex    : see EFIUsbDevice.h
*
* @retval                      : see EFIUsbDevice.h
*/
EFI_STATUS
UsbDeviceSetEndpointStallState(
  IN       UINT8    EndpointIndex,
  IN       BOOLEAN  State);


/* Instance of the USBDevice Protocol */
EFI_USB_DEVICE_PROTOCOL UsbDeviceProtocol =
{
  EFI_USB_DEVICE_PROTOCOL_REVISION,
  UsbDeviceStart,
  UsbDeviceSend,
  UsbDeviceHandleEvent,
  UsbDeviceAllocateBuffer,
  UsbDeviceFreeTransferBuffer,
  UsbDeviceStop,
  UsbDeviceAbortXfer,
  UsbDeviceSetEndpointStallState,
  UsbDeviceStartEx
};


/*
* See header for documentation.
*/
EFI_STATUS
IssueXfer (
  IN     EFI_USBFN_IO_PROTOCOL    *UsbfnIo,
  IN OUT XFER_TYPE                *Xfer
)
{
  EFI_STATUS        Status     = EFI_SUCCESS;
  UINTN             BufferSize = 0;
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
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction));
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  Buffer = Xfer->Buffer;
  Buffer += Xfer->BufferOffset;
  BufferSize = Xfer->XferSize - Xfer->BufferOffset;
  Status = UsbfnIo->Transfer(
      UsbfnIo,
      Xfer->EndpointIndex,
      Xfer->Direction,
      &BufferSize,
      Buffer);
  if (EFI_ERROR(Status)) 
  {
    DBG(EFI_D_ERROR, "Failed to submit transfer on EP 0x%02x: %r",
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction),
        Status);
    goto ON_EXIT;
  }

  DBG(EFI_D_VERBOSE, "Transfer submitted on EP 0x%02x, pending completion",
       ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction));
  Xfer->XferState = XFER_PENDING_COMPLETE;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for documentation.
 */
EFI_STATUS
AbortXfer (
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
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction));
    goto ON_EXIT;
  }

  Status = UsbfnIo->AbortTransfer(
      UsbfnIo,
      Xfer->EndpointIndex,
      Xfer->Direction);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to abort transfer on EP 0x%02x: %r",
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction),
        Status);
  }

  DBG(EFI_D_VERBOSE, "Transfer on EP 0x%02x aborted, free",
        ENDPOINT_ADDR(Xfer->EndpointIndex, Xfer->Direction));
  Xfer->XferState = XFER_FREE;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* See forward declaration above */
EFI_STATUS
UsbDeviceStop(VOID)
{
  EFI_STATUS Status = EFI_DEVICE_ERROR;
  FNC_ENTER_MSG();

  /* Uninit State Machine */
  DevSmCleanup();

  /* Stop Usb controller. If controller cannot be stopped, don't free the 
     resources, as it may crash the system */
  if (gUsbfnIo)
    Status = gUsbfnIo->StopController(gUsbfnIo);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "StopController failed %r",Status);
    goto ON_EXIT;
  }
  
  /* Free descriptors */
  Status = UsbDevDescFreeDescriptors(&UsbfnDevSm.DescriptorSet);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "UsbDevDescFreeDescriptors failed %r",Status);
  }

  /* Free the DeviceInfo
   * UsbFn implementation may or may not cache the DeviceInfo.
   * Thus, the structure can only be freed after controller stop
   */
  if (gDeviceInfo.DeviceDescriptor) {
    Status = UsbDevDescFreeDevInfo(&gDeviceInfo);
  }
  else if (gSSDeviceInfo.DeviceDescriptor) {
    Status = UsbDevDescFreeDevInfo((EFI_USB_DEVICE_INFO *)&gSSDeviceInfo);
  }
  else {
    Status = EFI_UNSUPPORTED;
  }
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "UsbDevDescFreeDevInfo failed %r",Status);
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* See forward declaration above */
EFI_STATUS
UsbDeviceSend(
  IN       UINT8    EndpointIndex,
  IN       UINTN    Size,
  IN       VOID    *Buffer
)
{
  EFI_STATUS                   Status;
  UINT8                        Ep;
  EFI_USBFN_ENDPOINT_DIRECTION Dir;
  FNC_ENTER_MSG();
  
  Ep  = USB_INDEX_TO_EP(EndpointIndex);
  Dir = USB_INDEX_TO_EPDIR(EndpointIndex);
  
  Status = gUsbfnIo->Transfer(gUsbfnIo, Ep, Dir, &Size, Buffer);

  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to submit data transfer: %r", Status);
    goto ON_EXIT;
  }

  if (Ep == USBDEV_CONTROL_EP) {
    UsbfnDevSm.ClsVndReq = TRUE;
  }
  
ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* See forward declaration above */
EFI_STATUS
UsbDeviceAbortXfer(
  IN       UINT8    EndpointIndex
)
{
  EFI_STATUS                   Status;
  UINT8                        Ep;
  EFI_USBFN_ENDPOINT_DIRECTION Dir;
  FNC_ENTER_MSG();

  Ep = USB_INDEX_TO_EP(EndpointIndex);
  Dir = USB_INDEX_TO_EPDIR(EndpointIndex);


  Status = gUsbfnIo->AbortTransfer(gUsbfnIo, Ep, Dir);

  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to submit data transfer: %r", Status);
  }

  FNC_LEAVE_MSG();
  return Status;
}


/*
* @brief Convert the EFI_USBFN_TRANSFER_STATUS to USB_DEVICE_TRANSFER_STATUS
*
* @see EFIUsbDevice.h
* @see EFIUsbfnIo.h
*
* @param[in]  XferStatus    : see EFIUsbfnIo.h
*
* @retval USB_DEVICE_TRANSFER_STATUS
*/
USB_DEVICE_TRANSFER_STATUS
ToDeviceTransferStatus(
  IN  EFI_USBFN_TRANSFER_STATUS XferStatus
)
{
  USB_DEVICE_TRANSFER_STATUS DevXferStatus;
  FNC_ENTER_MSG();

  switch (XferStatus) {
  case UsbTransferStatusAborted:
    DevXferStatus = UsbDeviceTransferStatusCancelled;
    break;
  case UsbTransferStatusUnknown:
  case UsbTransferStatusNone:
    DevXferStatus = UsbDeviceTransferStatusCompleteError;
    break;
  case UsbTransferStatusComplete:
    DevXferStatus = UsbDeviceTransferStatusCompleteOK;
    break;
  case UsbTransferStatusActive:
    DevXferStatus = UsbDeviceTransferStatusActive;
    break;
  default:
    DevXferStatus = UsbDeviceTransferStatusCompleteError;
  };

  FNC_LEAVE_MSG(); 
  return DevXferStatus;
}

/* See forward declaration above */
EFI_STATUS
UsbDeviceHandleEvent(
  OUT       USB_DEVICE_EVENT         *Event,
  OUT       UINTN                    *EventDataSize,
  OUT       USB_DEVICE_EVENT_DATA    *EventData)
{
  EFI_STATUS                      Status;
  EFI_USBFN_MESSAGE               UsbFnMsg;
  EFI_USBFN_MESSAGE_PAYLOAD       UsbFnPayload;
  UINTN                           UsbFnPayloadSize;

  if (!Event || !EventDataSize || !EventData) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "Invalid Parameter");
    goto ON_EXIT;
  }

  // Usb Device Event for client
  *Event          = UsbDeviceEventNoEvent;

  /* Always poll for events */
  UsbFnPayloadSize = sizeof(EFI_USBFN_MESSAGE_PAYLOAD);
  Status = gUsbfnIo->EventHandler(gUsbfnIo, &UsbFnMsg, &UsbFnPayloadSize, &UsbFnPayload);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "UsbfnIo EventHandler failed: %r", Status);
    goto ON_EXIT;
  }

  /* Pass meaningful messages to state machine */
  if (UsbFnMsg != EfiUsbMsgNone) {
    Status = DevSmProcessEvent(UsbFnMsg, UsbFnPayloadSize, &UsbFnPayload);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "State machine failed to process event: %r", Status);
      goto ON_EXIT;
    }

    // Notify the client of USB Device based on the USBFnIO messages
    // Transfer Finished
    if ((UsbFnMsg == EfiUsbMsgEndpointStatusChangedRx) || (UsbFnMsg == EfiUsbMsgEndpointStatusChangedTx)) {
      // filter out the EfiUsbMsgEndpointStatusChangedRx and EfiUsbMsgEndpointStatusChangedTx events on EP 0. 
      if (UsbFnPayload.utr.EndpointIndex == USBDEV_CONTROL_EP && UsbfnDevSm.ClsVndReq == FALSE) {
        *Event          = UsbDeviceEventNoEvent; 
      }      
      // non-std control transfer is always passed to the client
      else if ( (USBDEV_CONTROL_EP != UsbFnPayload.utr.EndpointIndex) || 
                (TRUE == UsbfnDevSm.ClsVndReq) ) {
       *Event                                     = UsbDeviceEventTransferNotification;
       *EventDataSize                             = sizeof (EventData->TransferOutcome);
        EventData->TransferOutcome.DataBuffer     = UsbFnPayload.utr.Buffer;
        EventData->TransferOutcome.EndpointIndex  = ENDPOINT_ADDR(UsbFnPayload.utr.EndpointIndex, UsbFnPayload.utr.EndpointDirection);
        EventData->TransferOutcome.BytesCompleted = UsbFnPayload.utr.BytesTransferred;
        EventData->TransferOutcome.Status         = ToDeviceTransferStatus(UsbFnPayload.utr.TransferStatus);
        DBG(EFI_D_INFO, "Pass Endpoint Status Change on Ep %d Dir %d to Client",
            UsbFnPayload.utr.EndpointIndex,
            UsbFnPayload.utr.EndpointDirection);
      }
      UsbfnDevSm.ClsVndReq = FALSE;
    }

    // Notify the client that it can now send non-control USB transfers.
    if ((UsbfnDevSm.PreviousState != DEV_STATE_CONFIGURED) && (UsbfnDevSm.CurrentState == DEV_STATE_CONFIGURED)) {
      *Event                 = UsbDeviceEventDeviceStateChange;
      *EventDataSize         = sizeof(EventData->DeviceState);
      EventData->DeviceState = UsbDeviceStateConnected;
      DBG(EFI_D_INFO, "Notify Connection");
    }

    // Notify the client that USB cable is disconnected
    if ((UsbfnDevSm.PreviousState != UsbfnDevSm.CurrentState) && (UsbfnDevSm.CurrentState == DEV_STATE_DETACHED)) {
      *Event                 = UsbDeviceEventDeviceStateChange;
      *EventDataSize         = sizeof(EventData->DeviceState);
      EventData->DeviceState = UsbDeviceStateDisconnected;
      DBG(EFI_D_INFO, "Notify Disconnect");
    }

    // Notify the client about the class request. If the client does not 
    // need it, it can ignore it.
    if ((UsbFnMsg == EfiUsbMsgSetupPacket) && (UsbfnDevSm.ClsVndReq == TRUE)) {
      *Event                                                  = UsbDeviceEventOemEvent;
      *EventDataSize                                          = sizeof(EventData->OemData); 
      EventData->OemData.DataGuid                             = gEfiUsbDeviceOemDataSetupPacketGuid;
      EventData->OemData.VersionNumber                        = 0;
      EventData->OemData.Context                              = AllocateZeroPool(sizeof (UsbFnPayload.udr));
      *((EFI_USB_DEVICE_REQUEST *)EventData->OemData.Context) = UsbFnPayload.udr;
      EventData->OemData.Size                                 = sizeof (UsbFnPayload.udr);
      DBG(EFI_D_INFO, "Notify Class Request");
    }

    // Notify the client about the MaxPacketSize0. This is used for client who needs to handle ZLT
    if (UsbFnMsg == EfiUsbMsgBusEventSpeed) {
      *Event                            = UsbDeviceEventOemEvent;
      *EventDataSize                    = sizeof(EventData->OemData);
      EventData->OemData.DataGuid       = gEfiUsbDeviceOemDataMaxPktSizeGuid;
      EventData->OemData.VersionNumber  = UsbEndpointBulk; // use the field for endpoint type
      EventData->OemData.Context        = AllocateZeroPool(sizeof (UINTN));
      Status                            = UsbfnDevSm.UsbfnIo->GetEndpointMaxPacketSize(
                                                    UsbfnDevSm.UsbfnIo, 
                                                    UsbEndpointBulk, 
                                                    UsbfnDevSm.BusSpeed,
                                                    (UINT16 *) EventData->OemData.Context);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "GetEndpointMaxPacketSize failed: %r", Status);
      }
      EventData->OemData.Size           = sizeof(UINTN);
      DBG(EFI_D_INFO, "Notify MaxPktSize on Bulk");
    }

    //Notify the client about the USB charger type.
    if ((UsbFnMsg == EfiUsbMsgBusEventAttach)) {
      *Event                                                  = UsbDeviceEventOemEvent;
      *EventDataSize                                          = sizeof(EventData->OemData);
      EventData->OemData.DataGuid                             = gEfiUsbDeviceOemDataChargerTypeGuid;
      EventData->OemData.VersionNumber                        = 0;
      EventData->OemData.Context                              = AllocateZeroPool(sizeof (EFI_USBFN_PORT_TYPE));
      *((EFI_USBFN_PORT_TYPE *)EventData->OemData.Context)    = EfiUsbUnknownPort;
      UsbfnDevSm.UsbfnIo->DetectPort(UsbfnDevSm.UsbfnIo, (EFI_USBFN_PORT_TYPE*) EventData->OemData.Context);
      EventData->OemData.Size                                 = sizeof(EFI_USBFN_PORT_TYPE);
    }
  }
  
ON_EXIT:
  return Status;
}


/* Open protocols needed for UsbDeviceDxe */
static
EFI_STATUS
UsbDeviceOpenProtocols (
  VOID
) 
{
  EFI_STATUS            Status = EFI_SUCCESS;

  FNC_ENTER_MSG();
  // Locate the USB FN IO protocol.
  if (NULL == gUsbfnIo) {
    Status = gBS->LocateProtocol(
    &gEfiUsbfnIoProtocolGuid,
    NULL,
    (VOID **)&gUsbfnIo);

    if (EFI_ERROR(Status)) {
      gUsbfnIo = NULL;
      DBG(EFI_D_ERROR, "Failed to locate USB FN IO protocol: %r", Status);
      goto ON_EXIT;
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* See forward declaration above */
EFI_STATUS
UsbDeviceStartEx(
  IN USB_DEVICE_DESCRIPTOR_SET         *UsbDevDescSet
)
{
  EFI_STATUS            Status = EFI_SUCCESS;

  FNC_ENTER_MSG();

  if (!UsbDevDescSet) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Status = UsbDeviceOpenProtocols();
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  Status = gUsbfnIo->StopController(gUsbfnIo);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  // dummy implementation in the USBFn Io
  // just call it.
  Status = gUsbfnIo->StartController(gUsbfnIo);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to Start USB Controller: %r", Status);
    goto ON_EXIT;
  }

  // Create a DeviceInfo for UsbFnIO
  // Initialized the DescriptorSet

  Status = UsbDevDescInitDescriptorsEx(UsbDevDescSet, &gDeviceInfo, &gSSDeviceInfo, &UsbfnDevSm.DescriptorSet);

  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "UsbDevDescInitDescriptors failed: %r", Status);
    goto ON_EXIT;
  }

  // Initialize device state machine
  Status = DevSmInitialize(gUsbfnIo, FALSE);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to initialize state machine: %r", Status);
    goto ON_EXIT;
  }

  // Copy configuration and set RS
  Status = gUsbfnIo->ConfigureEnableEndpointsEx(gUsbfnIo, &gDeviceInfo, &gSSDeviceInfo);

  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to configure and enable endpoints: %r", Status);
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* See forward declaration above */
EFI_STATUS
UsbDeviceStart (
  IN USB_DEVICE_DESCRIPTOR                 *DeviceDescriptor,
  IN VOID                                 **Descriptors,
  IN USB_DEVICE_QUALIFIER_DESCRIPTOR       *DeviceQualifierDescriptor,
  IN VOID                                  *BinaryDeviceOjectStore, OPTIONAL
  IN UINT8                                  StringDescriptorCount,
  IN USB_STRING_DESCRIPTOR                **StringDescritors OPTIONAL
)
{
  EFI_STATUS            Status     = EFI_SUCCESS;
  FNC_ENTER_MSG();

  if (!DeviceDescriptor || !Descriptors || !DeviceQualifierDescriptor || (StringDescriptorCount > 0 && !StringDescritors)) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Status = UsbDeviceOpenProtocols();
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  Status = gUsbfnIo->StopController(gUsbfnIo);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  } 

  // dummy implementation in the USBFn Io
  // just call it.
  Status = gUsbfnIo->StartController(gUsbfnIo);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to Start USB Controller: %r", Status);
    goto ON_EXIT;
  }

  // Create a DeviceInfo for UsbFnIO
  // Initialized the DescriptorSet
  Status = UsbDevDescInitDescriptors(
    DeviceDescriptor,
    Descriptors,
    DeviceQualifierDescriptor,
    BinaryDeviceOjectStore,
    StringDescriptorCount,
    StringDescritors,
    &gDeviceInfo,
    &UsbfnDevSm.DescriptorSet);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "UsbDevDescInitDescriptors failed: %r", Status);
    goto ON_EXIT;
  }

  // Initialize device state machine
  Status = DevSmInitialize(gUsbfnIo, FALSE);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to initialize state machine: %r", Status);
    goto ON_EXIT;
  }  

  // Copy configuration and set RS
  Status = gUsbfnIo->ConfigureEnableEndpoints(gUsbfnIo, &gDeviceInfo);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to configure and enable endpoints: %r", Status);
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* See forward declaration above */
EFI_STATUS
UsbDeviceAllocateBuffer(
  IN   UINTN      Size,
  OUT  VOID       **Buffer
)
{
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  if (gUsbfnIo == NULL) {
    Status = UsbDeviceOpenProtocols ();
    if(EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "AllocateTransferBuffer failed: %r", Status);
      goto ON_EXIT;
    }    
  }

  Status = gUsbfnIo->AllocateTransferBuffer(gUsbfnIo, Size, Buffer);

  if(EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "AllocateTransferBuffer failed: %r", Status);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/* See forward declaration above */
EFI_STATUS
UsbDeviceFreeTransferBuffer(
  IN VOID          *Buffer
)
{
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  Status = gUsbfnIo->FreeTransferBuffer(gUsbfnIo, Buffer);

  if(EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "FreeTransferBuffer failed: %r", Status);
  } 

  FNC_LEAVE_MSG();
  return Status;
}


/* See forward declaration above */
EFI_STATUS
UsbDeviceSetEndpointStallState(
  IN       UINT8    EndpointIndex,
  IN       BOOLEAN  State)
{
  EFI_STATUS                   Status;
  UINT8                        Ep;
  EFI_USBFN_ENDPOINT_DIRECTION Dir;
  FNC_ENTER_MSG();

  Ep  = USB_INDEX_TO_EP(EndpointIndex);
  Dir = USB_INDEX_TO_EPDIR(EndpointIndex);

  Status = gUsbfnIo->SetEndpointStallState(gUsbfnIo, Ep, Dir, State);
  if(EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
  }

  FNC_LEAVE_MSG();
  return Status;
}


/**
 * The user Entry Point for Driver.
 *
 * This function performs the following:
 *
 * @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 * @param[in] SystemTable    A pointer to the EFI System Table.
 *
 * @retval EFI_SUCCESS       All UsbfnIo tests passed.
 * @retval other             Error occurred while testing UsbfnIo.
 */
EFI_STATUS
EFIAPI
UsbDeviceDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                      Status        = EFI_SUCCESS;
  FNC_ENTER_MSG();

  Status = gBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
    &gEfiUsbDeviceProtocolGuid,
    &UsbDeviceProtocol,
    NULL);

  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to install UsbDevice protocol: %r", Status);
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}
