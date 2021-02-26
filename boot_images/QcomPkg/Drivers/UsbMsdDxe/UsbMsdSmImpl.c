/**
 * @file UsbDevSmImpl.c
 *
 * @brief State function implementation
 *
 * This file implements the state entry and transition functions for use in the
 * transfer state machine.
 *
 * @copyright Copyright (c) 2014,2017 by Qualcomm Technologies, Inc. All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/01/17   pm      Port from Boot.xf.1.4, Fix control transfers issue in middle of bulk data transfers
 01/19/17   pm      Fix KW issue
 12/08/14   ck      Fix incorrect LunNum after unmount
 10/14/14   ck      Initial Version. Introduce state machine and remove global vars
 =============================================================================*/

#include <Library/UefiBootServicesTableLib.h> // gBS->*
#include <Library/MemoryAllocationLib.h>      // memory allocation functions. 
#include "UsbError.h"                         // USB_ASSERT_*
#include "UsbMsd.h"                           // USBMSD_DEV
#include "UsbMsdSmImpl.h"                     // header
#include "UsbMsdDesc.h"                       // Endpoint Number macro
#include "UsbMsdUtility.h"                    // FNC_*_MSG
#include "UsbMsdScsi.h"                       // CBW, CSW, and related constant variables

/* USB MSD specific class requests */
#define USB_SETUP_REQ_MS_RESET          0xff
#define USB_SETUP_REQ_MS_GET_MAX_LUN    0xfe

/* Transfer Buffer Idx to distinguish buffers transferred on the same endpoint*/
typedef enum _MSD_XFER_BUFF_IDX {
  CTRL_XFER_IDX,
  BULK_TX_XFER_IDX,
  BULK_RX_XFER_IDX,
  CBW_XFER_IDX,
  CSW_XFER_IDX
} MSD_XFER_BUFF_IDX;


/*******************************************************************************
 * Helper functions
 ******************************************************************************/


/**
 * Return string description of bus speed.
 *
 * @param[in]  BusSpeed     Desired USB bus speed
 *
 * @return Ascii string representation of BusSpeed
 */
STATIC
CHAR8 CONST *
PrintBusSpeed (
  IN  EFI_USB_BUS_SPEED     BusSpeed
  )
{
  switch (BusSpeed) {
    case UsbBusSpeedLow:
      return "Low";
    case UsbBusSpeedFull:
      return "Full";
    case UsbBusSpeedHigh:
      return "High";
    case UsbBusSpeedSuper:
      return "Super";
    default:
      return "Unknown";
  }
}

/**
 * Validate the transfer result endpoint and buffer pointer.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Utr          UsbfnIo transfer result
 * @param[in]  Ep           Expected endpoint
 * @param[in]  Idx          Transfer Buffer Idx to distinguish buffers 
                            transferred on the same endpoint
 *
 * @retval EFI_SUCCESS      Transfer result valid
 * @retval EFI_UNSUPPORTED  Transfer result on unexpected direction
 * @retval EFI_DEVICE_ERROR Invalid buffer returned in transfer result
 */
STATIC
EFI_STATUS
ValidateEpBuffer(
  IN  DEV_STATE_MACHINE            *DevSm,
  IN  EFI_USBFN_TRANSFER_RESULT    *Utr,
  IN  UINT8                         Ep,
  IN  MSD_XFER_BUFF_IDX             Idx
)
{
  EFI_STATUS Status  = EFI_SUCCESS;

  FNC_ENTER_MSG();

  // Verify expected endpoint in event
  if (Ep != Utr->EndpointIndex) {
    DBG(EFI_D_VERBOSE, "Unexpected endpoint 0x%02x not handled",
      ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection));
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  switch (Idx) {
    // Verify the control buffer was returned in the event (just print warning, don't halt test)
  case CTRL_XFER_IDX:
    if (Utr->Buffer != DevSm->CtrlXfer.Buffer) {
      DBG(EFI_D_WARN, "UsbfnIo Protocol Violation: invalid buffer returned in transfer result\n"
        "    expected: %p\n"
        "    actual:   %p",
        DevSm->CtrlXfer.Buffer,
        Utr->Buffer);
    }
    break;
  case BULK_RX_XFER_IDX:
    if (Utr->Buffer != DevSm->BulkRxXfer.Buffer + DevSm->BulkRxXfer.BufferOffset) {
      DBG(EFI_D_WARN, "UsbfnIo Protocol Violation: Ep %d, EpDir %d invalid buffer returned in transfer result\n"
        "    expected: %p\n"
        "    actual:   %p",
        Utr->EndpointIndex,
        Utr->EndpointDirection,
        DevSm->BulkRxXfer.Buffer,
        Utr->Buffer);
    }
    break;
  case BULK_TX_XFER_IDX:
    if (Utr->Buffer != DevSm->BulkTxXfer.Buffer + DevSm->BulkTxXfer.BufferOffset) {
      DBG(EFI_D_WARN, "UsbfnIo Protocol Violation: Ep %d, EpDir %d invalid buffer returned in transfer result\n"
        "    expected: %p\n"
        "    actual:   %p",
        Utr->EndpointIndex,
        Utr->EndpointDirection,
        DevSm->BulkRxXfer.Buffer,
        Utr->Buffer);
    }
    break;
  case CBW_XFER_IDX:
    if (Utr->Buffer != DevSm->CBW.Buffer) {
      DBG(EFI_D_WARN, "UsbfnIo Protocol Violation: invalid CBW buffer returned in transfer result\n"
        "    expected: %p\n"
        "    actual:   %p",        
        DevSm->CBW.Buffer,
        Utr->Buffer);
    }
    break;
  case CSW_XFER_IDX:
    if (Utr->Buffer != DevSm->CSW.Buffer) {
      DBG(EFI_D_WARN, "UsbfnIo Protocol Violation: invalid CSW buffer returned in transfer result\n"
        "    expected: %p\n"
        "    actual:   %p",
        DevSm->CSW.Buffer,
        Utr->Buffer);
    }
    break;
  default:
    DBG(EFI_D_WARN, "invalid MSD_XFER_BUFF_IDX value %d", Idx);
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * Handles setup packets containing standard requests.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Req          Setup packet data
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Next transfer started
 * @retval EFI_UNSUPPORTED  Request not handled
 * @retval Others           Error processing request
 */
STATIC
EFI_STATUS
ProcessStdRequest(
  IN  DEV_STATE_MACHINE       *DevSm,
  IN  EFI_USB_DEVICE_REQUEST  *Req,
  OUT DEV_STATE               *NextState
  )
{
  EFI_STATUS                  Status    = EFI_UNSUPPORTED;
  VOID                       *Data      = NULL;
  UINTN                       DataSize  = 0;
  UINT8                       ConfigVal = 0;
  EFI_USB_STRING_DESCRIPTOR  *StrDesc   = NULL;

  FNC_ENTER_MSG();

  *NextState = DEV_STATE_SAME;

  // Switch on setup request
  switch (Req->Request) {

    case USB_DEV_CLEAR_FEATURE:
    case USB_DEV_SET_FEATURE:

      if (USB_FEATURE_ENDPOINT_HALT == Req->Value) {
        Status = DevSm->UsbfnIo->SetEndpointStallState (
            DevSm->UsbfnIo,
            USB_INDEX_TO_EP(Req->Index),
            USB_INDEX_TO_EPDIR(Req->Index),
            Req->Request == USB_DEV_SET_FEATURE);
        if (EFI_ERROR(Status)) {
          DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
          goto ON_EXIT;
        } else {
          DBG(EFI_D_VERBOSE, "%a Feature Endpoint Halt called on ep 0x%x",
              (Req->Request == USB_DEV_SET_FEATURE) ? "Set" : "Clear",
              Req->Index);
        }
      }
      break;

    case USB_DEV_GET_STATUS:

      Data = &DevSm->UsbDevStatus;
      DataSize = sizeof(DevSm->UsbDevStatus);
      DBG(EFI_D_VERBOSE, "Get Status: %d", DevSm->UsbDevStatus);
      break;

    case USB_DEV_SET_ADDRESS:

      DevSm->BusAddress = Req->Value & 0xff;
      *NextState = DEV_STATE_DEFAULT_ADDRESSED;
      DBG(EFI_D_VERBOSE, "Set Address: %d", DevSm->BusAddress);
      break;

    case USB_DEV_GET_DESCRIPTOR: {

      UINT8 DescType   =  USB_VALUE_TO_DESC_TYPE(Req->Value);
      UINT8 DescIndex  =  USB_VALUE_TO_DESC_IDX(Req->Value);

      switch (DescType) {

        case ((UINT8) USB_DESC_TYPE_DEVICE):
          Data = (VOID *) &DeviceDescriptor;
          DataSize = DeviceDescriptor.Length;
          break;

        case ((UINT8) USB_DESC_TYPE_DEVICE_QUALIFIER):
          Data = (VOID *) &DeviceQualifier;
          DataSize = DeviceQualifier.Length;
          break;

        case ((UINT8) USB_DESC_TYPE_CONFIG):
          Data = &TotalConfigDescriptor.ConfigDescriptor;
          DataSize = TotalConfigDescriptor.ConfigDescriptor.TotalLength;
          break;

        case ((UINT8) USB_DESC_TYPE_STRING):

          switch (DescIndex) {
          case 0: // String 0
            Data = (VOID *) Str0Descriptor;
            DataSize = sizeof(Str0Descriptor);
            break;
          case 1: // Manufacturer
            Data = (VOID *) StrManufacturerDescriptor;
            DataSize = sizeof(StrManufacturerDescriptor);
            break;
          case 2: // Product
            Data = (VOID *) StrProductDescriptor;
            DataSize = sizeof(StrProductDescriptor);
            break;
          case 3: // Serial
            StrDesc = AllocateZeroPool(sizeof(UINT8)+sizeof(UINT8)+DevSm->SerialNumberLen);
            if (StrDesc) {
              StrDesc->DescriptorType = USB_DESC_TYPE_STRING;
              StrDesc->Length = sizeof(StrDesc->DescriptorType) + sizeof(StrDesc->Length) + DevSm->SerialNumberLen;
              gBS->CopyMem(StrDesc->String, DevSm->SerialNumber, DevSm->SerialNumberLen);
              Data = (VOID *)StrDesc;
              DataSize = StrDesc->Length;
            } else {
              Data = (VOID *)StrSerialDescriptor;
              DataSize = sizeof (StrSerialDescriptor);
            }            
            break;
          default: // Unsupported string index
            DBG(EFI_D_ERROR, "Unsupported string index 0x%02x", DescIndex);
            goto ON_EXIT;
          }
          break;
          
        default: // unsupported descriptor type
          DBG(EFI_D_VERBOSE, "Unsupported descriptor type 0x%02x", DescType);
          goto ON_EXIT;

      }
      DBG(EFI_D_VERBOSE, "Get Descriptor: %d, %d", DescType, DescIndex);
      break;
    }

    case USB_DEV_GET_CONFIGURATION:

      if (DevSmGetCurrentState(DevSm) == DEV_STATE_CONFIGURED) {
        ConfigVal = 1;
      } else {
        ConfigVal = 0;
      }
      Data = &ConfigVal;
      DataSize = sizeof(ConfigVal);
      DBG(EFI_D_VERBOSE, "Get Configuration: %d", ConfigVal);
      break;

    case USB_DEV_SET_CONFIGURATION:

      if ((Req->Value & 0xff) == 1) {
        *NextState = DEV_STATE_CONFIGURED;
      } else {
        *NextState = DEV_STATE_DEFAULT_ADDRESSED;
      }
      DBG(EFI_D_VERBOSE, "Set Configuration: %d", Req->Value);
      break;

    default:
      DBG(EFI_D_VERBOSE, "Request not handled");
      goto ON_EXIT;
  }

  USB_ASSERT_GOTO((DevSm->CtrlXfer.XferState == XFER_FREE), ON_EXIT);

  // Just queue a zero length IN transfer for status stage
  if (0 == Req->Length) {

    DevSm->CtrlXfer.EndpointDirection    = EfiUsbEndpointDirectionDeviceTx;
    DevSm->CtrlXfer.BufferOffset = 0;
    DevSm->CtrlXfer.XferSize     = 0;
    DBG(EFI_D_VERBOSE, "No data stage");

  } else if (Data && (Req->RequestType & USB_ENDPOINT_DIR_IN)) { // the device needs to send data to host

    DevSm->CtrlXfer.EndpointDirection    = EfiUsbEndpointDirectionDeviceTx;
    DevSm->CtrlXfer.BufferOffset = 0;
    // Regardless how much the host wants, we can send at most of data size 
    // equal to XferSize since XferSize represents the buffer size
    DevSm->CtrlXfer.XferSize     = (Req->Length < DataSize) ? Req->Length : DataSize;
    gBS->CopyMem(DevSm->CtrlXfer.Buffer, Data, DevSm->CtrlXfer.XferSize);
    DBG(EFI_D_VERBOSE, "Sending data to host, length = %d", DevSm->CtrlXfer.XferSize);

  } else { // Device needs to receive data from host

    // Stall if we don't have enough room for data
    if (Req->Length > DevSm->CtrlXfer.BufferSize) {
      Status = DevSm->UsbfnIo->SetEndpointStallState (
          DevSm->UsbfnIo,
          USBMSD_CONTROL_EP,
          EfiUsbEndpointDirectionDeviceRx,
          TRUE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
        goto ON_EXIT;
      }
      DBG(EFI_D_ERROR, "Not enough room for CTRL RX transfer");
      goto ON_EXIT;
    } else { // receive the data
      DevSm->CtrlXfer.EndpointDirection    = EfiUsbEndpointDirectionDeviceRx;
      DevSm->CtrlXfer.BufferOffset = 0;
      DevSm->CtrlXfer.XferSize     = Req->Length;
      DBG(EFI_D_VERBOSE, "Receiving data from host, length = %d", DevSm->CtrlXfer.XferSize);
    }
  }

  // Submit the data or status stage
  Status = IssueXfer(DevSm->UsbfnIo, &DevSm->CtrlXfer);

ON_EXIT:

  if (StrDesc) {
    FreePool(StrDesc);
  }

  FNC_LEAVE_MSG();
  return Status;
}


/**
 * Handles setup packets containing class requests.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Req          Setup packet data
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Next transfer started
 * @retval EFI_UNSUPPORTED  Request not handled
 * @retval Others           Error processing request
 */
STATIC
EFI_STATUS
ProcessClsRequest(
  IN  DEV_STATE_MACHINE       *DevSm,
  IN  EFI_USB_DEVICE_REQUEST  *Req,
  OUT DEV_STATE               *NextState
  )
{
  EFI_STATUS             Status      = EFI_SUCCESS;
  VOID*                  Data        = NULL;
  UINTN                  DataSize    = 0;
  USBMSD_DEV            *UsbMsd      = NULL;
  UINT8                  LunNum      = 0;
  EFI_USBFN_IO_PROTOCOL *UsbfnIo     = NULL;
  XFER_TYPE             *CtrlXfer    = NULL;
  XFER_TYPE             *MsdBulkIn   = NULL;
  XFER_TYPE             *MsdBulkOut  = NULL;
  
  FNC_ENTER_MSG();

  *NextState = DEV_STATE_SAME; 

  USB_ASSERT_GOTO(Req, ON_EXIT);

  UsbMsd = USBMSD_FROM_SM(DevSm);

  USB_ASSERT_GOTO(UsbMsd, ON_EXIT);

  LunNum     =  UsbMsd->UsbMsdMedia.NumLuns - 1;
  UsbfnIo    =  UsbMsd->StateMachine.UsbfnIo;
  CtrlXfer   = &UsbMsd->StateMachine.CtrlXfer;
  MsdBulkIn  = &UsbMsd->StateMachine.BulkTxXfer;
  MsdBulkOut = &UsbMsd->StateMachine.BulkRxXfer;

  switch (Req->Request) {
  case USB_SETUP_REQ_MS_RESET:
    DBG(EFI_D_INFO, "Recv USB_SETUP_REQ_MS_RESET");
    if (Req->Length == 0 && Req->Value == 0)
    {
      /* clear MS dQH and dTDs */
      /* cancel active MS transfers */
      DBG(EFI_D_INFO, "USB_SETUP_REQ_MS_RESET: Abort Transfer On Ep1 and Ep2");
      AbortXfer(UsbfnIo, MsdBulkIn);
      AbortXfer(UsbfnIo, MsdBulkOut);
      UsbMsd->StateMachine.RcvcMsdClsReq = TRUE;
    }
    break;

  case USB_SETUP_REQ_MS_GET_MAX_LUN:
    DBG(EFI_D_INFO, "Recv USB_SETUP_REQ_MS_GET_MAX_LUN");
    if (Req->Length == 1 && Req->Value == 0)
    {
      Data     = &LunNum;
      DataSize = 1;
      UsbMsd->StateMachine.RcvcMsdClsReq = TRUE;
    }
    break;
  default:  /* not supported */
    DBG(EFI_D_ERROR, "Unknown Class Req %d", Req->Request);
    break;
  }

  // no data to send or receive
  // just queue a zero length IN transfer for status stage
  if (0 == Req->Length) {
    DBG(EFI_D_INFO, "No Data Stage");
    CtrlXfer->XferState         = XFER_PENDING_ENQUEUE;
    CtrlXfer->BufferOffset      = 0;
    CtrlXfer->XferSize          = 0;
    CtrlXfer->EndpointDirection = EfiUsbEndpointDirectionHostIn;
  } else if (Data && (Req->RequestType & USB_ENDPOINT_DIR_IN)) { // the device needs to send data to the host
    CtrlXfer->EndpointDirection = EfiUsbEndpointDirectionHostIn;
    CtrlXfer->XferState         = XFER_PENDING_ENQUEUE;
    CtrlXfer->BufferOffset      = 0;
    // Regardless how much the host wants, we can send at most of data size 
    // equal to XferSize since XferSize represents the buffer size
    CtrlXfer->XferSize          = Req->Length < DataSize ? Req->Length : DataSize;
    gBS->CopyMem(CtrlXfer->Buffer, Data, CtrlXfer->XferSize);
    DBG(EFI_D_INFO, "Sending Data From Device To Host. Data size = %d", CtrlXfer->XferSize);
  } else { // the device needs to receive data from the host
    DBG(EFI_D_INFO, "Receiving Data From Host To Device");
    CtrlXfer->EndpointDirection = EfiUsbEndpointDirectionHostOut;
    CtrlXfer->XferState         = XFER_PENDING_ENQUEUE;
  }
 
  // Submit the data or status stage
  Status = IssueXfer(UsbfnIo, CtrlXfer);
 
ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Handles Rx events for standard control requests.
 *
 * This function simply needs to process the status for the endpoint and
 * update the control transfer. If any failure occurs an error status is
 * returned to the caller which is responsible for stalling the appropriate
 * endpoint.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Utr          UsbfnIo transfer result
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Event handled successfully
 * @retval EFI_UNSUPPORTED  Not a supported endpoint
 * @retval Others           Error processing event
 */
STATIC
EFI_STATUS
ProcessCtrlEpStatusChangedRx(
  IN  DEV_STATE_MACHINE         *DevSm,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  OUT DEV_STATE                 *NextState
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_SETSTS_STR(
    (DevSm && Utr && NextState),
    ON_EXIT,
    EFI_INVALID_PARAMETER,
    Status,
    "Invalid Parameters");

  *NextState = DEV_STATE_SAME;

  DBG(EFI_D_VERBOSE, "EfiUsbMsgEndpointStatusChangedRx: EP: 0x%02x: BytesTransferred: %d",
      ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection),
      Utr->BytesTransferred);

  ValidateEpBuffer(DevSm, Utr, USBMSD_CONTROL_EP, CTRL_XFER_IDX);  

  DevSm->CtrlXfer.XferState = XFER_FREE;

  // Switch on the transfer status
  switch (Utr->TransferStatus) {

    case UsbTransferStatusComplete:
      if (0 == Utr->BytesTransferred) {

        // Control transfer Status Stage is done
        DBG(EFI_D_VERBOSE, "Status OUT stage complete");

      } else {
        DBG(EFI_D_INFO, "Just recieved %d bytes", DevSm->CtrlXfer.XferSize); 
          
        // Queue a zero length Tx Transfer to complete the Status Stage
        DBG(EFI_D_VERBOSE, "Data OUT stage complete, submitting status IN stage");
        DevSm->CtrlXfer.EndpointDirection    = EfiUsbEndpointDirectionDeviceTx;
        DevSm->CtrlXfer.BufferOffset = 0;
        DevSm->CtrlXfer.XferSize     = 0;

        Status = IssueXfer(DevSm->UsbfnIo, &DevSm->CtrlXfer);   
        
      }
      break;

    case UsbTransferStatusAborted:
      DBG(EFI_D_VERBOSE, "Control OUT transfer aborted");
      break;

    default: // Other statuses should not occur
      DBG(EFI_D_VERBOSE, "Unhandled transfer status on CTRL OUT: %d", Utr->TransferStatus);
      Status = EFI_DEVICE_ERROR;
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Handles Tx events for standard control requests.
 *
 * This function simply needs to process the status for the endpoint and
 * update the control transfer. If any failure occurs an error status is
 * returned to the caller which is responsible for stalling the appropriate
 * endpoint.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Utr          UsbfnIo transfer result
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Event handled successfully
 * @retval EFI_UNSUPPORTED  Not a supported endpoint
 * @retval Others           Error processing event
 */
STATIC
EFI_STATUS
ProcessCtrlEpStatusChangedTx(
  IN  DEV_STATE_MACHINE         *DevSm,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  OUT DEV_STATE                 *NextState
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  USB_ASSERT_GOTO_SETSTS_STR(
    (DevSm && Utr && NextState),
    ON_EXIT,
    EFI_INVALID_PARAMETER,
    Status,
    "Invalid Parameters");

  *NextState = DEV_STATE_SAME;

  DBG(EFI_D_VERBOSE, "EfiUsbMsgEndpointStatusChangedTx: EP: 0x%02x: BytesTransferred: %d",
      ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection),
      Utr->BytesTransferred);

  ValidateEpBuffer(DevSm, Utr, USBMSD_CONTROL_EP, CTRL_XFER_IDX);  

  DevSm->CtrlXfer.XferState = XFER_FREE;

  // Switch on the transfer status
  switch (Utr->TransferStatus) {

    case UsbTransferStatusComplete:
      if (0 == Utr->BytesTransferred) {
        // Control transfer Status Stage is done
        DBG(EFI_D_VERBOSE, "Status IN stage complete");
      } else {
        // Control transfer Data Stage is done
        // Queue a zero length Rx Transfer to complete the Status Stage
        DBG(EFI_D_VERBOSE, "Data IN stage complete, submitting status OUT stage");
        DevSm->CtrlXfer.EndpointDirection    = EfiUsbEndpointDirectionDeviceRx;
        DevSm->CtrlXfer.BufferOffset = 0;
        DevSm->CtrlXfer.XferSize     = 0;

        Status = IssueXfer(DevSm->UsbfnIo, &DevSm->CtrlXfer);

        if (DevSm->RcvcMsdClsReq == TRUE) {
          DevSm->RcvcMsdClsReq = FALSE;
          *NextState           = DEV_STATE_CBW;
        }
      }
      break;

    case UsbTransferStatusAborted:
      DBG(EFI_D_VERBOSE, "Control IN transfer aborted");
      break;

    default: // Other statuses should not occur
      DBG(EFI_D_VERBOSE, "Unhandled transfer status on CTRL IN: %d", Utr->TransferStatus);
      Status = EFI_DEVICE_ERROR;
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


STATIC
EFI_STATUS
RecvNewCBW(
  IN DEV_STATE_MACHINE   *DevSm
)
{
  EFI_STATUS             Status         = EFI_SUCCESS;  

  ARG_CHK(DevSm);

  DevSm->CBW.BufferOffset = 0;

  Status = IssueXfer(DevSm->UsbfnIo, &DevSm->CBW);  
  ERR_CHK("failed to queue CBW");

ON_EXIT:
  return Status;
}


/*******************************************************************************
 * Functions common to multiple states
 ******************************************************************************/

/**
 * @brief The event does not need to be handled in the current state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_SAME
 */
DEV_STATE Dev_EventNotHandled (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_VERBOSE, "%a not handled in state %a",
      DevSmGetEventName(Message),
      DevSmGetStateName(DevSm, DevSmGetCurrentState(DevSm)));
  return DEV_STATE_SAME;
}


/**
 * @brief Logs that the event should not occur in the current state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_SAME
 */
DEV_STATE Dev_EventError (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_ERROR, "%a not allowed in state %a",
      DevSmGetEventName(Message),
      DevSmGetStateName(DevSm, DevSmGetCurrentState(DevSm)));
  return DEV_STATE_SAME;
}


/**
 * @brief Always transition to default state on reset.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_DEFAULT_ADDRESSED
 */
DEV_STATE Dev_Common_Reset (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  FNC_ENTER_MSG();
  DBG(EFI_D_INFO, "Device Reset");

  (void) AbortXfer(DevSm->UsbfnIo, &DevSm->CtrlXfer);
  DevSm->BusAddress = 0;
  DevSm->BusSpeed = UsbBusSpeedUnknown;

  FNC_LEAVE_MSG();
  return DEV_STATE_DEFAULT_ADDRESSED;
}


/**
 * @brief Transisiton to suspend state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_SUSPENDED
 */
DEV_STATE Dev_Common_Suspend (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  return DEV_STATE_SUSPENDED;
}


/**
 * @brief Transition to detached state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_DETACHED;
 */
DEV_STATE Dev_Common_Detach (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_INFO, "Cable Detached");
  return DEV_STATE_DETACHED;
}


/**
 * @brief Process standard USB requests.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
DEV_STATE Dev_Common_SetupPkt (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DEV_STATE  NextState = DEV_STATE_SAME;
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  Status = ProcessStdRequest(DevSm, &Payload->udr, &NextState);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to process setup packet: %r", Status);

    // Stall on error or unhandled request
    Status = DevSm->UsbfnIo->SetEndpointStallState(
        DevSm->UsbfnIo,
        USBMSD_CONTROL_EP,
        USB_REQUEST_EPDIR(Payload->udr.RequestType),
        TRUE);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
    }
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Process control transfer Rx events.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
DEV_STATE Dev_Common_EpStatusChangedRx (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  EFI_STATUS Status    = EFI_SUCCESS;
  DEV_STATE  NextState = DEV_STATE_SAME;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgEndpointStatusChangedRx), DEV_STATE_SAME);

  // Process control ep events
  switch (Payload->utr.EndpointIndex) {

  case USBMSD_CONTROL_EP:
    // Only handle standard request on control endpoint
    Status = ProcessCtrlEpStatusChangedRx(DevSm, &Payload->utr, &NextState);

    // Stall Control Endpoint
    // For CI, both dir needs to be stalled
    // For SNPS, always stall physical endpoint zero
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to update control transfer: %r", Status);
      Status = DevSm->UsbfnIo->SetEndpointStallState(
        DevSm->UsbfnIo,
        USBMSD_CONTROL_EP,
        EfiUsbEndpointDirectionDeviceRx,
        TRUE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
      }
    }
    break;

  default:
    DBG(EFI_D_INFO, "EfiUsbMsgEndpointStatusChangedRx received on ep: 0x%02x",
      ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
    break;
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Process control transfer Tx events.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
DEV_STATE Dev_Common_EpStatusChangedTx (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  EFI_STATUS Status    = EFI_SUCCESS;
  DEV_STATE  NextState = DEV_STATE_SAME;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgEndpointStatusChangedTx), DEV_STATE_SAME);

  // Process control ep events
  switch (Payload->utr.EndpointIndex) {

  case USBMSD_CONTROL_EP:
    // Only handle standard request on control endpoint
    Status = ProcessCtrlEpStatusChangedTx(DevSm, &Payload->utr, &NextState);

    // Stall Control Endpoint
    // For CI, both dir needs to be stalled
    // For SNPS, always stall physical endpoint zero
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to update control transfer: %r", Status);
      Status = DevSm->UsbfnIo->SetEndpointStallState(
        DevSm->UsbfnIo,
        USBMSD_CONTROL_EP,
        EfiUsbEndpointDirectionDeviceRx,
        TRUE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
      }
    }
    break;

  default:
    DBG(EFI_D_INFO, "EfiUsbMsgEndpointStatusChangedTx received on ep: 0x%02x",
      ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
    break;
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Save the bus speed.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on speed
 */
DEV_STATE Dev_Common_Speed (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DEV_STATE  NextState     = DEV_STATE_SAME;
  UINT16     MaxPacketSize = 0;
  EFI_STATUS Status        = EFI_SUCCESS;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgBusEventSpeed), DEV_STATE_SAME);

  DevSm->BusSpeed = Payload->ubs;
  DBG(EFI_D_INFO, "Bus Speed: %a", PrintBusSpeed(DevSm->BusSpeed));
 
  // update the bulk ep size based on connected speed
  Status = DevSm->UsbfnIo->GetEndpointMaxPacketSize(DevSm->UsbfnIo, UsbEndpointBulk, DevSm->BusSpeed, &MaxPacketSize);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "GetEndpointMaxPacketSize failed: %r", Status);
  } else {   
    TotalConfigDescriptor.EndpointDescriptor0.MaxPacketSize = MaxPacketSize;
    TotalConfigDescriptor.EndpointDescriptor1.MaxPacketSize = MaxPacketSize;
  }  

  // Only advance from ATTACHED to DEFAULT/ADDRESSED on SS connection
  if (DevSm->BusSpeed == UsbBusSpeedSuper) {
    NextState = DEV_STATE_DEFAULT_ADDRESSED;
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
 * DEV_STATE_DETACHED functions
 ******************************************************************************/

/**
 * @brief Initialize/clear internal state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID 
Dev_Detached_Entry (
  IN DEV_STATE_MACHINE *DevSm, 
  IN DEV_STATE PrevState)
{
  FNC_ENTER_MSG();

  DevSm->PortType = EfiUsbUnknownPort;
  DevSm->BusSpeed = UsbBusSpeedUnknown;
  DevSm->ResumeState = DEV_INITIAL_STATE;

  FNC_LEAVE_MSG();
}


/**
 * @brief Just move to attached state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_ATTACHED
 */
DEV_STATE Dev_Detached_Attach (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_INFO, "Cable Attached");
  return DEV_STATE_ATTACHED;
}


/*******************************************************************************
 * DEV_STATE_ATTACHED functions
 ******************************************************************************/

/**
 * @brief Detect the port type when cable is attached.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID 
Dev_Attached_Entry (
  IN DEV_STATE_MACHINE *DevSm, 
  IN DEV_STATE          PrevState)
{
  FNC_ENTER_MSG();
  FNC_LEAVE_MSG();
}


/**
 * @brief Process EpStatusChangeRx event in attached state.
 *
 * If errors happen during mass storage operation, the state
 * will be set to Attached state. 
 * This allows users to detach the USB cable and attach the 
 * USB cable again, and try to recover the system.
 * However, when cable is unplugged, CBW transfer will be canceled first
 * before the cable detach event is received. Because CBW transfer is
 * canceled, we treat it as an error and reach the Attached state. 
 * The function is provided so that when the subsequent transfers 
 * are canceled due to cable detach, we do not print false error messages.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
DEV_STATE Dev_Attached_EpStatusChangedRx (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  CHAR8     *XferName  = NULL;

  FNC_ENTER_MSG();

  USB_ASSERT_RETURN(((Message == EfiUsbMsgEndpointStatusChangedRx) && DevSm && Payload), DEV_STATE_SAME);

  if (Payload->utr.Buffer == DevSm->CtrlXfer.Buffer) {
    XferName = "CTRL";
  }
  else if (Payload->utr.Buffer == DevSm->BulkRxXfer.Buffer) {
    XferName = "RX";
  }
  else if (Payload->utr.Buffer == DevSm->BulkTxXfer.Buffer) {
    XferName = "TX";
  }
  else if (Payload->utr.Buffer == DevSm->CBW.Buffer) {
    XferName = "CBW";
  }
  else if (Payload->utr.Buffer ==  DevSm->CSW.Buffer) {
   XferName = "CSW";
  }
  else {
    XferName ="Invalid";
  }

  DBG(EFI_D_INFO, "Ep %d, Dir %d, Buffer %a",
    Payload->utr.EndpointIndex,
    Payload->utr.EndpointDirection,
    XferName
    );

  FNC_LEAVE_MSG();
  return DEV_STATE_SAME;
}


/*******************************************************************************
 * DEV_STATE_DEFAULT_ADDRESSED functions
 ******************************************************************************/

/*******************************************************************************
 * DEV_STATE_CONFIGURED functions
 ******************************************************************************/
/**
 * @brief Process standard and class USB requests.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
DEV_STATE Dev_Configured_SetupPkt (
  IN DEV_STATE_MACHINE         *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD *Payload )
{
  EFI_STATUS               Status    = EFI_SUCCESS;
  DEV_STATE                NextState = DEV_STATE_SAME;
  EFI_USB_DEVICE_REQUEST  *Req       = NULL;
  FNC_ENTER_MSG();

  Req =  &Payload->udr;

  DBG(EFI_D_VERBOSE,
      "EfiUsbMsgSetupPacket:\n"
      "  bmRequestType: 0x%02x\n"
      "  bRequest:      0x%02x\n"
      "  wValue:      0x%04x\n"
      "  wIndex:      0x%04x\n"
      "  wLength:     0x%04x",
      Req->RequestType,
      Req->Request,
      Req->Value,
      Req->Index,
      Req->Length);

  if (USB_SETUP_RT_TYPE(Req->RequestType) == USB_SETUP_RT_TYPE_STD) {
    // Process standard requests
    Status = ProcessStdRequest(DevSm, Req, &NextState);
  } else if (USB_SETUP_RT_TYPE(Req->RequestType) == USB_SETUP_RT_TYPE_CLS) {
    // Process class request
    Status = ProcessClsRequest(DevSm, Req, &NextState);
  } else {
    Status = EFI_UNSUPPORTED;
  }

  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to process setup packet: %r", Status);

    // Stall on error or unhandled request
    Status = DevSm->UsbfnIo->SetEndpointStallState(
        DevSm->UsbfnIo,
        USBMSD_CONTROL_EP,
        (Payload->udr.RequestType >> 7) & 1
          ? EfiUsbEndpointDirectionHostIn : EfiUsbEndpointDirectionHostOut,
        TRUE);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
    }
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
 * DEV_STATE_SUSPENDED functions
 ******************************************************************************/
/**
 * @brief Save the previous state on suspend.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID 
Dev_Suspended_Entry (
  IN DEV_STATE_MACHINE *DevSm, 
  IN DEV_STATE          PrevState)
{
  DevSm->ResumeState = PrevState;
  DBG(EFI_D_INFO, "Bus suspended in state %a", DevSmGetStateName(DevSm, PrevState));
}


/**
 * @brief Return to state saved on suspend entry.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return State saved on suspend
 */
DEV_STATE Dev_Suspended_Resume (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_INFO, "Resuming to state %a", DevSmGetStateName(DevSm, DevSm->ResumeState));
  return DevSm->ResumeState;
}


/**
* @brief Handle EpStatusChangedRx in suspended state.
*
* It is possible the xfer is aborted when receiving the suspend.
*
* @param[in] DevSm         Usbfn state machine instance
* @param[in] Message       UsbfnIo message
* @param[in] PayloadSize   Size of data in Payload
* @param[in] Payload       UsbfnIo message payload
*
* @return State saved on suspend
*/
DEV_STATE Dev_Suspended_EpStatusChangedRx(
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload)
{
  // if the EpStatusChangedRx, it might be an  error
  if (UsbTransferStatusAborted != Payload->utr.TransferStatus) {
    DBG(EFI_D_WARN, "Ep %d Dir %d, Buff %p, Bytes %d, Status %d",
      Payload->utr.EndpointIndex,
      Payload->utr.EndpointDirection,
      Payload->utr.Buffer,
      Payload->utr.BytesTransferred,
      Payload->utr.TransferStatus);
  }
  return DEV_STATE_SAME;
}


/**
* @brief Handle EpStatusChangedTx in suspended state.
*
* It is possible the xfer is aborted when receiving the suspend.
*
* @param[in] DevSm         Usbfn state machine instance
* @param[in] Message       UsbfnIo message
* @param[in] PayloadSize   Size of data in Payload
* @param[in] Payload       UsbfnIo message payload
*
* @return State saved on suspend
*/
DEV_STATE Dev_Suspended_EpStatusChangedTx(
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload)
{
  // if the EpStatusChangedTx, it might be an  error
  if (UsbTransferStatusAborted != Payload->utr.TransferStatus) {
    DBG(EFI_D_WARN, "Ep %d Dir %d, Buff %p, Bytes %d, Status %d",
      Payload->utr.EndpointIndex,
      Payload->utr.EndpointDirection,
      Payload->utr.Buffer,
      Payload->utr.BytesTransferred,
      Payload->utr.TransferStatus);
  }
  return DEV_STATE_SAME;
}

/*******************************************************************************
* DEV_STATE_CBW functions
******************************************************************************/
/**
* @brief Prepare to receive CBW
*
* @param[in] DevSm         Usbfn state machine instance
* @param[in] PrevState     State we are coming from
*/
VOID Dev_CBW_Entry(IN DEV_STATE_MACHINE *DevSm, IN DEV_STATE PrevState) 
{    
  DBG(EFI_D_INFO, "Prepare for CBW");
  RecvNewCBW(DevSm);
}


/**
* @brief Handle CBW sent by host
*
* @param[in] DevSm         Usbfn state machine instance
* @param[in] Message       UsbfnIo message
* @param[in] PayloadSize   Size of data in Payload
* @param[in] Payload       UsbfnIo message payload
*
* @return next state
*/
DEV_STATE Dev_CBW_EpStatusChangedTx(
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload)
{
  return DEV_STATE_SAME;
}

/**
* @brief Handle CBW sent by host
*
* @param[in] DevSm         Usbfn state machine instance
* @param[in] Message       UsbfnIo message
* @param[in] PayloadSize   Size of data in Payload
* @param[in] Payload       UsbfnIo message payload
*
* @return next state
*/
DEV_STATE Dev_CBW_EpStatusChangedRx(
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload)
{
  EFI_STATUS                 Status    = EFI_SUCCESS;
  USBMSD_DEV                *UsbMsd    = NULL;
  DEV_STATE                  NextState = DEV_STATE_SAME;

  FNC_ENTER_MSG();

  ARG_CHK(Payload && DevSm);

  USB_ASSERT_GOTO((Message == EfiUsbMsgEndpointStatusChangedRx), ON_EXIT);

  // Process both control and bulk ep events
  switch (Payload->utr.EndpointIndex) {

  case USBMSD_CONTROL_EP:
    Status = ProcessCtrlEpStatusChangedRx(DevSm, &Payload->utr, &NextState);

    // Stall opposite control ep on error
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to update control transfer: %r", Status);
      Status = DevSm->UsbfnIo->SetEndpointStallState(
        DevSm->UsbfnIo,
        Payload->utr.EndpointIndex,
        EfiUsbEndpointDirectionDeviceTx,
        TRUE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
      }
    }
    break;

  case USBMSD_BULK_EP:
    DBG(EFI_D_INFO, "Handle New CBW");

    ValidateEpBuffer(DevSm, &Payload->utr, USBMSD_BULK_EP, CBW_XFER_IDX);

    // check xfer status
    if (UsbTransferStatusComplete != Payload->utr.TransferStatus) {
      // something is not correct, move to attach and wait for reset
      DBG(EFI_D_ERROR, "CBW xfer failed.");
      NextState = DEV_STATE_ATTACHED;
    }
    else {
      DevSm->CBW.BufferOffset += Payload->utr.BytesTransferred;
      UsbMsd           = USBMSD_FROM_SM(DevSm);
      NextState        = handle_new_cbw(UsbMsd);
    }

    // reset offset to zero, after CBW is handled or error happens
    DevSm->CBW.XferState     = XFER_FREE;
    DevSm->CBW.BufferOffset = 0;
    break;

  default:
    DBG(EFI_D_ERROR, "EfiUsbMsgEndpointStatusChangedRx received on invalid ep: 0x%02x",
      ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
    break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
* DEV_STATE_SEND_CSW functions
******************************************************************************/
DEV_STATE Dev_CSW_EpStatusChangedTx(
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload)
{
  DEV_STATE   NextState = DEV_STATE_SAME;
  EFI_STATUS  Status    = EFI_SUCCESS;
  
  FNC_ENTER_MSG();
  ARG_CHK(DevSm && Payload);

  switch (Payload->utr.EndpointIndex) {
  case USBMSD_CONTROL_EP:
    Status = ProcessCtrlEpStatusChangedTx(DevSm, &Payload->utr, &NextState);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to update control transfer: %r", Status);
      Status = DevSm->UsbfnIo->SetEndpointStallState(
        DevSm->UsbfnIo,
        USBMSD_CONTROL_EP,
        EfiUsbEndpointDirectionDeviceRx,
        TRUE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
      }
    }
    break;
  case USBMSD_BULK_EP:
  ValidateEpBuffer(DevSm, &Payload->utr, USBMSD_BULK_EP, CSW_XFER_IDX);
 
  // check xfer status
  if (UsbTransferStatusComplete != Payload->utr.TransferStatus) {
    // something is not correct, move to attach and wait for reset
    DBG(EFI_D_ERROR, "CSW xfer failed.");
    NextState = DEV_STATE_ATTACHED;
  }
  else {
    NextState = DEV_STATE_CBW;
  }

  // Reset CSW XFER status to free
  DevSm->CSW.XferState     = XFER_FREE;
  DevSm->CSW.BufferOffset  = 0;
	break;
  default:
    DBG(EFI_D_ERROR, "EfiUsbMsgEndpointStatusChangedTx received on ep: 0x%02x",
      ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
    break;
 }
ON_EXIT:
  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
* DEV_STATE_SEND_TO_HOST functions
******************************************************************************/

DEV_STATE Dev_EpStatusChangedTx(
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload)
{  
  EFI_STATUS                 Status         = EFI_SUCCESS;  
  DEV_STATE                  NextState      = DEV_STATE_SAME;
  UINT32                     CSWDataResidue = 0;
  USBMSD_DEV                *UsbMsd         = NULL;

  FNC_ENTER_MSG();

  ARG_CHK(Payload && DevSm);

  DBG(EFI_D_VERBOSE, "Payload %p, Buf %p, Bytes Xferred %d, Ep %d, EpDir %d, Status %d",
    Payload,
    Payload->utr.Buffer,
    Payload->utr.BytesTransferred,
    Payload->utr.EndpointIndex,
    Payload->utr.EndpointDirection,
    Payload->utr.TransferStatus);

  switch (Payload->utr.EndpointIndex) {
  case USBMSD_CONTROL_EP:
    Status = ProcessCtrlEpStatusChangedTx(DevSm, &Payload->utr, &NextState);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to update control transfer: %r", Status);
      Status = DevSm->UsbfnIo->SetEndpointStallState(
        DevSm->UsbfnIo,
        USBMSD_CONTROL_EP,
        EfiUsbEndpointDirectionDeviceRx,
        TRUE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
      }
    }
    break;
  case USBMSD_BULK_EP:
  // check xfer status
  if (UsbTransferStatusComplete != Payload->utr.TransferStatus) {
    // something is not correct, move to attach and wait for reset
    DBG(EFI_D_ERROR, "Tx Data xfer failed.");
    NextState = DEV_STATE_ATTACHED;
  }
  else {
    UsbMsd = USBMSD_FROM_SM(DevSm);

    // ValidateEpBuffer needs to be called BEFORE updating offset.
    ValidateEpBuffer(DevSm, &Payload->utr, USBMSD_BULK_EP, BULK_TX_XFER_IDX);

    DevSm->BulkTxXfer.XferState     = XFER_FREE;
    DevSm->BulkTxXfer.BufferOffset += Payload->utr.BytesTransferred;
    CSWDataResidue                  = DevSm->BulkTxXfer.XferSize - DevSm->BulkTxXfer.BufferOffset;

    if (0 == CSWDataResidue) {
      DBG(EFI_D_INFO, "All Data Sent to Host. Send CSW");      
      DevSm->BulkTxXfer.BufferOffset = 0;
      NextState = sending_csw(UsbMsd, CSW_GOOD_STATUS, CSWDataResidue);
    }
    else {
      DBG(EFI_D_INFO, "Send Remaining Data: %d bytes", CSWDataResidue);
      Status = IssueXfer(DevSm->UsbfnIo, &DevSm->BulkTxXfer);
      WRN_CHK("IssueXfer Failed %r", Status);
      NextState = sending_csw(UsbMsd, CSW_ERROR_STATUS, CSWDataResidue);
    }
  }
  break;
  default:
    DBG(EFI_D_ERROR, "EfiUsbMsgEndpointStatusChangedTx received on ep: 0x%02x",
      ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
    break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
* DEV_STATE_PREPARE_FOR_CBW functions
******************************************************************************/
DEV_STATE Dev_EpStatusChangedRx(
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload)
{    
  EFI_STATUS                  Status            = EFI_SUCCESS;  
  DEV_STATE                   NextState         = DEV_STATE_SAME;  
  UINT32                      CSWDataResidue    = 0;
  USBMSD_DEV                 *UsbMsd            = NULL;
  
  ARG_CHK(Payload && DevSm);

  DBG(EFI_D_VERBOSE, "Payload %p, Buf %p, Bytes Xferred %d, Ep %d, EpDir %d, Status %d",
    Payload,
    Payload->utr.Buffer,
    Payload->utr.BytesTransferred,
    Payload->utr.EndpointIndex,
    Payload->utr.EndpointDirection,
    Payload->utr.TransferStatus);

  switch (Payload->utr.EndpointIndex) {
  case USBMSD_CONTROL_EP:
    Status = ProcessCtrlEpStatusChangedRx(DevSm, &Payload->utr, &NextState);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "Failed to update control transfer: %r", Status);
      Status = DevSm->UsbfnIo->SetEndpointStallState(
        DevSm->UsbfnIo,
        Payload->utr.EndpointIndex,
        EfiUsbEndpointDirectionDeviceTx,
        TRUE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
      }
    }
    break;
  case USBMSD_BULK_EP:
  // check xfer status
  if (UsbTransferStatusComplete != Payload->utr.TransferStatus) {
    // something is not correct, move to attach and wait for reset
    DBG(EFI_D_ERROR, "Rx Data xfer failed.");
    NextState = DEV_STATE_ATTACHED;
  }
  else {
    UsbMsd = USBMSD_FROM_SM(DevSm);

    // ValidateEpBuffer needs to be called BEFORE updating offset. 
    ValidateEpBuffer(DevSm, &Payload->utr, USBMSD_BULK_EP, BULK_RX_XFER_IDX);

    DevSm->BulkRxXfer.XferState     = XFER_FREE;
    DevSm->BulkRxXfer.BufferOffset += Payload->utr.BytesTransferred;
    CSWDataResidue                  = DevSm->BulkRxXfer.XferSize - DevSm->BulkRxXfer.BufferOffset;

    if (CSWDataResidue == 0) {
      DBG(EFI_D_INFO, "Recv All Data from Host. Write to EMMC Card");
      NextState = handle_rx_data(UsbMsd);
      DevSm->BulkRxXfer.BufferOffset = 0;
    }
    else {
      DBG(EFI_D_INFO, "Recv Remaining Data %d", CSWDataResidue);
      Status = IssueXfer(DevSm->UsbfnIo, &DevSm->BulkRxXfer);
      if (EFI_ERROR(Status)){
        NextState = sending_csw(UsbMsd, CSW_ERROR_STATUS, CSWDataResidue);
      }
    }
  }
  break;
  default:
    DBG(EFI_D_ERROR, "EfiUsbMsgEndpointStatusChangedRx received on invalid ep: 0x%02x",
      ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
    break;

}
  
ON_EXIT:
  return NextState;
}
