/**
 * @file UsbfnLbSmImpl.c
 *
 * @brief State function implementation
 *
 * This file implements the state entry and transition functions for use in the
 * loopback transfer state machine.
 *
 * @copyright Copyright (c) 2014 by QUALCOMM Technologies, Inc. All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/02/16   ck      Update the debug message to show the reason of failure
 11/20/14   ml      Migrate to official SuperSpeed support
 08/21/14   ck      move device qualifier descriptor to Usb.h and minor fixes
 04/30/14   ar      Cleanup for NDEBUG build
 02/06/14   ml      Initial implementation
 =============================================================================*/


#include "UsbError.h"
#include "UsbfnLbSm.h"
#include "UsbfnLbSmImpl.h"
#include "UsbfnDesc.h"


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
 * Return string description of USB port type.
 *
 * @param[in]  PortType     Desired USB port type
 *
 * @return Ascii string representation of PortType
 */
STATIC
CHAR8 CONST *
PrintPortType (
  IN  EFI_USBFN_PORT_TYPE   PortType
  )
{
  switch (PortType) {
    case EfiUsbStandardDownstreamPort:
      return "SDP";
    case EfiUsbChargingDownstreamPort:
      return "CDP";
    case EfiUsbDedicatedChargingPort:
      return "DCP";
    case EfiUsbInvalidDedicatedChargingPort:
      return "IDCP";
    default:
      return "Unknown Port";
  }
}


/**
 * Starts the loopback timeout timer.
 *
 * @param[in]  Ulb          Usbfn loopback state machine instance
 */
STATIC
VOID
SetLoopbackTimer(
  IN  ULB_STATE_MACHINE       *Ulb
  )
{
  EFI_STATUS Status;

  Status = gBS->SetTimer(
      Ulb->LbTestData.TimeoutTimer,
      TimerRelative,
      USBLB_TEST_TIMEOUT);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to set loopback test timer: %r", Status);
    return;
  }

  PRINT(APL_VVERB, "Started loopback test timer");
}


/**
 * Stall both bulk eps for loopback error conditions.
 *
 * @param[in] UsbfnIo       UsbfnIo protocol instance
 */
STATIC
VOID
StallBulkEndpoints(
  IN EFI_USBFN_IO_PROTOCOL  *UsbfnIo
  )
{
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  Status = UsbfnIo->SetEndpointStallState(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      TRUE);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "SetEndpointStallState failed on bulk TX: %r", Status);
  }

  Status = UsbfnIo->SetEndpointStallState(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceRx,
      TRUE);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "SetEndpointStallState failed on bulk RX: %r", Status);
  }

  FNC_LEAVE_MSG();
}


/**
 * Validate the transfer result endpoint and buffer pointer.
 *
 * @param[in]  Ulb          Usbfn loopback state machine instance
 * @param[in]  Utr          UsbfnIo transfer result
 * @param[in]  Ep           Expected endpoint
 *
 * @retval EFI_SUCCESS      Transfer result valid
 * @retval EFI_UNSUPPORTED  Transfer result on unexpected direction
 * @retval EFI_DEVICE_ERROR Invalid buffer returned in transfer result
 */
STATIC
EFI_STATUS
ValidateEpBuffer(
  IN  ULB_STATE_MACHINE         *Ulb,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  IN  UINT8                     Ep
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  // Verify expected endpoint in event
  if (Ep != Utr->EndpointIndex) {
    PRINT(APL_VERB, "Unexpected endpoint 0x%02x not handled",
        ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection));
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  switch (Ep) {

    // Verify the control buffer was returned in the event (just print warning, don't halt test)
    case USBLB_CONTROL_EP:
      if (Utr->Buffer != Ulb->CtrlXfer.Buffer) {
        PRINT(APL_WARN, "UsbfnIo Protocol Violation: invalid buffer returned in transfer result\n"
            "    expected: %p\n"
            "    actual:   %p",
            Ulb->CtrlXfer.Buffer,
            Utr->Buffer);
      }
      break;

    // Verify the bulk buffer was returned in the event (just print warning, don't halt test)
    case USBLB_BULK_EP:
      if (Utr->Buffer != Ulb->LbTestData.Xfer.Buffer) {
        PRINT(APL_WARN, "UsbfnIo Protocol Violation: invalid buffer returned in transfer result\n"
            "    expected: %p\n"
            "    actual:   %p",
            Ulb->LbTestData.Xfer.Buffer,
            Utr->Buffer);
      }
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * Handles setup packets containing standard requests.
 *
 * @param[in]  Ulb          Usbfn loopback state machine instance
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
  IN  ULB_STATE_MACHINE       *Ulb,
  IN  EFI_USB_DEVICE_REQUEST  *Req,
  OUT ULB_STATE               *NextState
  )
{
  EFI_STATUS  Status    = EFI_UNSUPPORTED;
  CONST VOID  *TxSrcBuf = NULL;
  UINTN       XferSize  = 0;
  UINT8       ConfigVal = 0;
  FNC_ENTER_MSG();

  // Only handle standard requests
  if (USB_SETUP_RT_TYPE(Req->RequestType) != USB_SETUP_RT_TYPE_STD) {
    PRINT(APL_VERB, "Not handling non-standard request 0x%02x", Req->RequestType);
    goto ON_EXIT;
  }

  *NextState = ULB_STATE_SAME;

  PRINT(APL_VERB,
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

  // Switch on setup request
  switch (Req->Request) {

    case USB_REQ_CLEAR_FEATURE:
    case USB_REQ_SET_FEATURE:

      // only feature we need to process
      if (USB_FEATURE_ENDPOINT_HALT == Req->Value) {
        Status = Ulb->UsbfnIo->SetEndpointStallState (
            Ulb->UsbfnIo,
            USB_INDEX_TO_EP(Req->Index),
            USB_INDEX_TO_EPDIR(Req->Index),
            Req->Request == USB_DEV_SET_FEATURE);
        if (EFI_ERROR(Status)) {
          PRINT(APL_ERROR, "SetEndpointStallState failed: %r", Status);
          goto ON_EXIT;
        } else {
          PRINT(APL_VERB, "%a Feature Endpoint Halt called on ep 0x%x",
              (Req->Request == USB_DEV_SET_FEATURE) ? "Set" : "Clear",
              Req->Index);
        }
      }
      break;

    case USB_REQ_GET_STATUS:

      TxSrcBuf = &Ulb->UsbDevStatus;
      XferSize = sizeof(Ulb->UsbDevStatus);
      PRINT(APL_VERB, "Get Status: %d", Ulb->UsbDevStatus);
      break;

    case USB_REQ_SET_ADDRESS:

      Ulb->BusAddress = Req->Value & 0xff;
      *NextState = ULB_STATE_DEFAULT_ADDRESSED;
      PRINT(APL_VERB, "Set Address: %d", Ulb->BusAddress);
      break;

    case USB_REQ_GET_DESCRIPTOR: {

      UINT8 DescType   =  USB_VALUE_TO_DESC_TYPE(Req->Value);
      UINT8 DescIndex  =  USB_VALUE_TO_DESC_IDX(Req->Value);

      switch (DescType) {

        case USB_DESC_TYPE_DEVICE:
          if (UsbBusSpeedSuper == Ulb->BusSpeed) {
            TxSrcBuf = Ulb->SSDeviceInfo->DeviceDescriptor;
            XferSize = Ulb->SSDeviceInfo->DeviceDescriptor->Length;
          } else {
            TxSrcBuf = Ulb->DeviceInfo->DeviceDescriptor;
            XferSize = Ulb->DeviceInfo->DeviceDescriptor->Length;
          }
          break;

        case USB_DESC_TYPE_DEVICE_QUALIFIER:
          TxSrcBuf = &DeviceQualifier;
          XferSize = DeviceQualifier.Length;
          break;

        case USB_DESC_TYPE_BOS:
          TxSrcBuf = Ulb->SSDeviceInfo->BosDescriptor;
          XferSize = Ulb->SSDeviceInfo->BosDescriptor->TotalLength;
          break;

        case USB_DESC_TYPE_CONFIG:
          switch (Ulb->BusSpeed) {
            case UsbBusSpeedSuper:
              TxSrcBuf = Ulb->SSDeviceInfo->ConfigInfoTable[0]->ConfigDescriptor;
              XferSize = Ulb->SSDeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->TotalLength;
              break;

            // update the non-SS bulk ep size based on connected speed
            case UsbBusSpeedFull:
              Ulb->DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->EndpointDescriptorTable[0]->MaxPacketSize = 64;
              Ulb->DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->EndpointDescriptorTable[1]->MaxPacketSize = 64;
              TxSrcBuf = Ulb->DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor;
              XferSize = Ulb->DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->TotalLength;
              break;

            default:
              Ulb->DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->EndpointDescriptorTable[0]->MaxPacketSize = 512;
              Ulb->DeviceInfo->ConfigInfoTable[0]->InterfaceInfoTable[0]->EndpointDescriptorTable[1]->MaxPacketSize = 512;
              TxSrcBuf = Ulb->DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor;
              XferSize = Ulb->DeviceInfo->ConfigInfoTable[0]->ConfigDescriptor->TotalLength;
              break;
          }
          break;

        case USB_DESC_TYPE_STRING:
          switch (DescIndex) {
            case 0: // String 0
              TxSrcBuf = &Str0Descriptor;
              XferSize = sizeof(Str0Descriptor);
              break;
            case 1: // Manufacturer
              TxSrcBuf = &StrManufacturerDescriptor;
              XferSize = sizeof(StrManufacturerDescriptor);
              break;
            case 2: // Product
              TxSrcBuf = &StrProductDescriptor;
              XferSize = sizeof(StrProductDescriptor);
              break;
            default: // Unsupported string index
              PRINT(APL_VERB, "Unsupported string index 0x%02x", DescIndex);
              goto ON_EXIT;
          }
          break;

        default: // unsupported descriptor type
          PRINT(APL_VERB, "Unsupported descriptor type 0x%02x", DescType);
          goto ON_EXIT;

      }
      PRINT(APL_VERB, "Get Descriptor: %d, %d", DescType, DescIndex);
      break;
    }

    case USB_REQ_GET_CONFIG:

      if (UlbSmGetCurrentState() == ULB_STATE_CONFIGURED ||
          UlbSmGetCurrentState() == ULB_STATE_LOOPBACK) {
        ConfigVal = 1;
      } else {
        ConfigVal = 0;
      }
      TxSrcBuf = &ConfigVal;
      XferSize = sizeof(ConfigVal);
      PRINT(APL_VERB, "Get Configuration: %d", ConfigVal);
      break;

    case USB_REQ_SET_CONFIG:

      if ((Req->Value & 0xff) == 1) {
        *NextState = ULB_STATE_CONFIGURED;
      } else {
        *NextState = ULB_STATE_DEFAULT_ADDRESSED;
      }
      PRINT(APL_VERB, "Set Configuration: %d", Req->Value);
      break;

    case USB_REQ_SET_ISOCH_DELAY:
      // nothing to do, just return status
      break;

    case USB_REQ_SET_SEL:
      // nothing to do, receive handled below and data ignored
      break;

    default:
      PRINT(APL_VERB, "Request not handled");
      goto ON_EXIT;
  }

  USB_ASSERT_GOTO((Ulb->CtrlXfer.XferState == XFER_FREE), ON_EXIT);

  // Just queue a zero length IN transfer for status stage
  if (0 == Req->Length) {

    Ulb->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceTx;
    Ulb->CtrlXfer.BufferOffset = 0;
    Ulb->CtrlXfer.XferSize     = 0;
    PRINT(APL_VERB, "No data stage");

  } else if (Req->RequestType & USB_ENDPOINT_DIR_IN) { // Device needs to send data to host

    Ulb->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceTx;
    Ulb->CtrlXfer.BufferOffset = 0;
    Ulb->CtrlXfer.XferSize     = (Req->Length < XferSize) ? Req->Length : XferSize;
    CopyMem(Ulb->CtrlXfer.Buffer, TxSrcBuf, Ulb->CtrlXfer.XferSize);
    PRINT(APL_VERB, "Sending data to host, length = %d", Ulb->CtrlXfer.XferSize);

  } else { // Device needs to receive data from host

    // Stall if we don't have enough room for data
    if (Req->Length > Ulb->CtrlXfer.BufferSize) {
      Status = Ulb->UsbfnIo->SetEndpointStallState (
          Ulb->UsbfnIo,
          USBLB_CONTROL_EP,
          EfiUsbEndpointDirectionDeviceRx,
          TRUE);
      if (EFI_ERROR(Status)) {
        PRINT(APL_ERROR, "SetEndpointStallState failed: %r", Status);
        goto ON_EXIT;
      }
      PRINT(APL_ERROR, "Not enough room for CTRL RX transfer");
      goto ON_EXIT;
    } else { // receive the data
      Ulb->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceRx;
      Ulb->CtrlXfer.BufferOffset = 0;
      Ulb->CtrlXfer.XferSize     = Req->Length;
      PRINT(APL_VERB, "Receiving data from host, length = %d", Ulb->CtrlXfer.XferSize);
    }
  }

  // Submit the data or status stage
  Status = IssueXfer(Ulb->UsbfnIo, &Ulb->CtrlXfer);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * Handles setup packets containing class requests.
 *
 * @param[in]  Ulb          Usbfn loopback state machine instance
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
  IN  ULB_STATE_MACHINE       *Ulb,
  IN  EFI_USB_DEVICE_REQUEST  *Req,
  OUT ULB_STATE               *NextState
  )
{
  EFI_STATUS  Status    = EFI_UNSUPPORTED;
  FNC_ENTER_MSG();

  // Only handle class requests
  if (USB_SETUP_RT_TYPE(Req->RequestType) != USB_SETUP_RT_TYPE_CLS) {
    PRINT(APL_VERB, "Not handling non-class request 0x%02x", Req->RequestType);
    goto ON_EXIT;
  }

  *NextState = ULB_STATE_SAME;

  PRINT(APL_VERB,
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

  // Switch on setup request
  switch (Req->Request) {

    case USB_CLS_REQ_LOOPBACK:
      PRINT(APL_VERB, "Loopback Test");
      break;

    default:
      PRINT(APL_VERB, "Request not handled");
      goto ON_EXIT;
  }

  USB_ASSERT_GOTO((Ulb->CtrlXfer.XferState == XFER_FREE), ON_EXIT);

  if (0 == Req->Length) {

    // The only supported class requests require a data stage
    PRINT(APL_ERROR, "Loopback class request requires a data stage");
    Status = EFI_PROTOCOL_ERROR;
    goto ON_EXIT;

  } else if (Req->RequestType & USB_ENDPOINT_DIR_IN) {

    // The only supported class requests require an OUT data stage
    PRINT(APL_ERROR, "Loopback class request requires OUT data stage");
    Status = EFI_PROTOCOL_ERROR;
    goto ON_EXIT;

  } else { // Device needs to receive data from host

    Ulb->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceRx;
    Ulb->CtrlXfer.BufferOffset = 0;
    Ulb->CtrlXfer.XferSize     = Req->Length;
    PRINT(APL_VERB, "Receiving data from host, length = %d", Ulb->CtrlXfer.XferSize);
  }

  // Submit the data or status stage
  Status = IssueXfer(Ulb->UsbfnIo, &Ulb->CtrlXfer);
  if (!EFI_ERROR(Status)) {
    Ulb->RcvLbTestConfig = TRUE;
  }

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
 * @param[in]  Ulb          Usbfn loopback state machine instance
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
  IN  ULB_STATE_MACHINE         *Ulb,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  OUT ULB_STATE                 *NextState
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  *NextState = ULB_STATE_SAME;

  PRINT(APL_VERB, "EfiUsbMsgEndpointStatusChangedRx: EP: 0x%02x: BytesTransferred: %d",
      ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection),
      Utr->BytesTransferred);

  Status = ValidateEpBuffer(Ulb, Utr, USBLB_CONTROL_EP);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  Ulb->CtrlXfer.XferState = XFER_FREE;

  // Switch on the transfer status
  switch (Utr->TransferStatus) {

    case UsbTransferStatusComplete:
      if (0 == Utr->BytesTransferred) {

        // Control transfer Status Stage is done
        PRINT(APL_VERB, "Status OUT stage complete");

      } else {

        // Control transfer Data Stage is done
        // Store the loopback test config if neccessary
        if (Ulb->RcvLbTestConfig) {
          Ulb->RcvLbTestConfig = FALSE;
          PRINT(APL_VERB, "Saving loopback test config");

          // Verify data length
          if (Ulb->CtrlXfer.XferSize != sizeof(LB_TEST_CONFIG)) {
            PRINT(APL_ERROR, "Invalid data length received: %d", Ulb->CtrlXfer.XferSize);
            Status = EFI_PROTOCOL_ERROR;
            break;
          }

          // Copy and validate test config
          CopyMem((VOID *)&Ulb->LbTestConfig, Ulb->CtrlXfer.Buffer, Ulb->CtrlXfer.XferSize);
          if (!(Ulb->LbTestConfig.Mode == LB_TEST_MODE_TX_ONLY ||
                Ulb->LbTestConfig.Mode == LB_TEST_MODE_RX_ONLY ||
                Ulb->LbTestConfig.Mode == LB_TEST_MODE_LOOPBACK) ||
              Ulb->LbTestConfig.Iterations == 0 ||
              Ulb->LbTestConfig.BufSize > Ulb->MaxTransferSize) {
            PRINT(APL_ERROR, "Invalid test configuration received: mode %d, itr %d, bufSize %d, maxSize %d",
			Ulb->LbTestConfig.Mode,
			Ulb->LbTestConfig.Iterations,
			Ulb->LbTestConfig.BufSize,
			Ulb->MaxTransferSize);
            Status = EFI_PROTOCOL_ERROR;
            break;
          }

          // Test config valid, move on to loopback
          *NextState = ULB_STATE_LOOPBACK;
        }

        // Queue a zero length Tx Transfer to complete the Status Stage
        PRINT(APL_VERB, "Data OUT stage complete, submitting status IN stage");
        Ulb->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceTx;
        Ulb->CtrlXfer.BufferOffset = 0;
        Ulb->CtrlXfer.XferSize     = 0;

        Status = IssueXfer(Ulb->UsbfnIo, &Ulb->CtrlXfer);
      }
      break;

    case UsbTransferStatusAborted:
      PRINT(APL_VERB, "Control OUT transfer aborted");
      break;

    default: // Other statuses should not occur
      PRINT(APL_VERB, "Unhandled transfer status on CTRL OUT: %d", Utr->TransferStatus);
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
 * @param[in]  Ulb          Usbfn loopback state machine instance
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
  IN  ULB_STATE_MACHINE         *Ulb,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  OUT ULB_STATE                 *NextState
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  *NextState = ULB_STATE_SAME;

  PRINT(APL_VERB, "EfiUsbMsgEndpointStatusChangedTx: EP: 0x%02x: BytesTransferred: %d",
      ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection),
      Utr->BytesTransferred);

  Status = ValidateEpBuffer(Ulb, Utr, USBLB_CONTROL_EP);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  Ulb->CtrlXfer.XferState = XFER_FREE;

  // Switch on the transfer status
  switch (Utr->TransferStatus) {

    case UsbTransferStatusComplete:
      if (0 == Utr->BytesTransferred) {

        // Control transfer Status Stage is done
        PRINT(APL_VERB, "Status IN stage complete");

      } else {

        // Control transfer Data Stage is done
        // Queue a zero length Rx Transfer to complete the Status Stage
        PRINT(APL_VERB, "Data IN stage complete, submitting status OUT stage");
        Ulb->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceRx;
        Ulb->CtrlXfer.BufferOffset = 0;
        Ulb->CtrlXfer.XferSize     = 0;

        Status = IssueXfer(Ulb->UsbfnIo, &Ulb->CtrlXfer);
      }
      break;

    case UsbTransferStatusAborted:
      PRINT(APL_VERB, "Control IN transfer aborted");
      break;

    default: // Other statuses should not occur
      PRINT(APL_VERB, "Unhandled transfer status on CTRL IN: %d", Utr->TransferStatus);
      Status = EFI_DEVICE_ERROR;
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Handles Rx events for bulk loopback data.
 *
 * This function simply needs to process the status for the endpoint and
 * update the loopback transfer. If any failure occurs an error status is
 * returned to the caller which is responsible for stalling the appropriate
 * endpoint.
 *
 * @param[in]  Ulb          Usbfn loopback state machine instance
 * @param[in]  Utr          UsbfnIo transfer result
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Event handled successfully
 * @retval EFI_UNSUPPORTED  Not a supported endpoint
 * @retval Others           Error processing event
 */
STATIC
EFI_STATUS
ProcessBulkEpStatusChangedRx(
  IN  ULB_STATE_MACHINE         *Ulb,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  OUT ULB_STATE                 *NextState
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  *NextState = ULB_STATE_SAME;

  PRINT(APL_VERB, "EfiUsbMsgEndpointStatusChangedRx: EP: 0x%02x: BytesTransferred: %d",
      ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection),
      Utr->BytesTransferred);

  Status = ValidateEpBuffer(Ulb, Utr, USBLB_BULK_EP);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  Ulb->LbTestData.Xfer.XferState = XFER_FREE;

  // restart the timer
  SetLoopbackTimer(Ulb);

  // switch on the transfer status
  switch (Utr->TransferStatus) {

    case UsbTransferStatusComplete:

      // check for expected size complete
      if (Ulb->LbTestData.Xfer.XferSize == Utr->BytesTransferred) {

        // submit ZLT if neccessary
        if ((Ulb->LbTestData.Xfer.XferSize > 0) && Ulb->LbTestData.RxZlt) {
          PRINT(APL_VERB, "Submitting RX ZLT transfer");
          Ulb->LbTestData.Xfer.XferSize = 0;
        } else {

          // data or ZLT transfer is complete
          Ulb->LbTestData.RxIteration++;

          // back to configured state when test is complete
          if (LB_TEST_MODE_RX_ONLY == Ulb->LbTestConfig.Mode) {
            if (Ulb->LbTestData.RxIteration >= Ulb->LbTestConfig.Iterations) {
              PRINT(APL_ALWAYS, "Loopback test complete");
              *NextState = ULB_STATE_CONFIGURED;
              break;
            }
          } else {
            Ulb->LbTestData.Xfer.Direction = EfiUsbEndpointDirectionDeviceTx;
          }
          Ulb->LbTestData.Xfer.XferSize = Ulb->LbTestConfig.BufSize;
        }

        // submit the next transfer
        Status = IssueXfer(Ulb->UsbfnIo, &Ulb->LbTestData.Xfer);
        break;

      } else {
        PRINT(APL_ERROR, "Wrong transfer length completed\n"
            "    expected: %d\n"
            "    actual:   %d",
            Ulb->LbTestData.Xfer.XferSize,
            Utr->BytesTransferred);
        Status = EFI_DEVICE_ERROR;
        break;
      }

    case UsbTransferStatusAborted:
      PRINT(APL_VERB, "Bulk OUT transfer aborted");
      break;

    default: // Other statuses should not occur
      PRINT(APL_VERB, "Unhandled transfer status on bulk OUT: %d", Utr->TransferStatus);
      Status = EFI_DEVICE_ERROR;
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Handles Tx events for bulk loopback data.
 *
 * This function simply needs to process the status for the endpoint and
 * update the loopback transfer. If any failure occurs an error status is
 * returned to the caller which is responsible for stalling the appropriate
 * endpoint.
 *
 * @param[in]  Ulb          Usbfn loopback state machine instance
 * @param[in]  Utr          UsbfnIo transfer result
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Event handled successfully
 * @retval EFI_UNSUPPORTED  Not a supported endpoint
 * @retval Others           Error processing event
 */
STATIC
EFI_STATUS
ProcessBulkEpStatusChangedTx(
  IN  ULB_STATE_MACHINE         *Ulb,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  OUT ULB_STATE                 *NextState
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  *NextState = ULB_STATE_SAME;

  PRINT(APL_VERB, "EfiUsbMsgEndpointStatusChangedTx: EP: 0x%02x: BytesTransferred: %d",
      ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection),
      Utr->BytesTransferred);

  Status = ValidateEpBuffer(Ulb, Utr, USBLB_BULK_EP);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  Ulb->LbTestData.Xfer.XferState = XFER_FREE;

  // restart the timer
  SetLoopbackTimer(Ulb);

  // Switch on the transfer status
  switch (Utr->TransferStatus) {

    case UsbTransferStatusComplete:

      // check for expected size complete
      if (Ulb->LbTestData.Xfer.XferSize == Utr->BytesTransferred) {

        // submit ZLT if neccessary
        if ((Ulb->LbTestData.Xfer.XferSize > 0) && Ulb->LbTestData.TxZlt) {
          PRINT(APL_VERB, "Submitting TX ZLT transfer");
          Ulb->LbTestData.Xfer.XferSize = 0;
        } else {

          // data or ZLT transfer is complete
          Ulb->LbTestData.TxIteration++;

          // back to configured state when test is complete
          if (Ulb->LbTestData.TxIteration >= Ulb->LbTestConfig.Iterations) {
            PRINT(APL_ALWAYS, "Loopback test complete");
            *NextState = ULB_STATE_CONFIGURED;
            break;
          } else {
            Ulb->LbTestData.Xfer.XferSize = Ulb->LbTestConfig.BufSize;
          }

          if (LB_TEST_MODE_LOOPBACK == Ulb->LbTestConfig.Mode) {
            Ulb->LbTestData.Xfer.Direction = EfiUsbEndpointDirectionDeviceRx;
          }
        }

        // submit the next transfer
        Status = IssueXfer(Ulb->UsbfnIo, &Ulb->LbTestData.Xfer);
        break;

      } else {
        PRINT(APL_ERROR, "Wrong transfer length completed\n"
            "    expected: %d\n"
            "    actual:   %d",
            Ulb->LbTestData.Xfer.XferSize,
            Utr->BytesTransferred);
        Status = EFI_DEVICE_ERROR;
        break;
      }

    case UsbTransferStatusAborted:
      PRINT(APL_VERB, "Bulk IN transfer aborted");
      break;

    default: // Other statuses should not occur
      PRINT(APL_VERB, "Unhandled transfer status on bulk IN: %d", Utr->TransferStatus);
      Status = EFI_DEVICE_ERROR;
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*******************************************************************************
 * Functions common to multiple states
 ******************************************************************************/

/**
 * @brief The event does not need to be handled in the current state.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return ULB_STATE_SAME
 */
ULB_STATE Ulb_EventNotHandled (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  PRINT(APL_VERB, "%a not handled in state %a",
      UlbSmGetEventName(Message),
      UlbSmGetStateName(UlbSmGetCurrentState()));
  return ULB_STATE_SAME;
}


/**
 * @brief Logs that the event should not occur in the current state.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return ULB_STATE_SAME
 */
ULB_STATE Ulb_EventError (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  PRINT(APL_ERROR, "%a not allowed in state %a",
      UlbSmGetEventName(Message),
      UlbSmGetStateName(UlbSmGetCurrentState()));
  return ULB_STATE_SAME;
}


/**
 * @brief Always transition to default state on reset.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return ULB_STATE_DEFAULT_ADDRESSED
 */
ULB_STATE Ulb_Common_Reset (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  FNC_ENTER_MSG();
  PRINT(APL_ALWAYS, "Device Reset");

  (void) AbortXfer(Ulb->UsbfnIo, &Ulb->CtrlXfer);
  (void) AbortXfer(Ulb->UsbfnIo, &Ulb->LbTestData.Xfer);
  Ulb->BusAddress = 0;
  Ulb->BusSpeed = UsbBusSpeedUnknown;
  Ulb->RcvLbTestConfig = FALSE;

  FNC_LEAVE_MSG();
  return ULB_STATE_DEFAULT_ADDRESSED;
}


/**
 * @brief Transisiton to suspend state.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return ULB_STATE_SUSPENDED
 */
ULB_STATE Ulb_Common_Suspend (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  return ULB_STATE_SUSPENDED;
}


/**
 * @brief Transition to detached state.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return ULB_STATE_DETACHED;
 */
ULB_STATE Ulb_Common_Detach (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  PRINT(APL_ALWAYS, "Cable Detached");
  return ULB_STATE_DETACHED;
}


/**
 * @brief Process standard USB requests.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
ULB_STATE Ulb_Common_SetupPkt (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  ULB_STATE  NextState = ULB_STATE_SAME;
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  Status = ProcessStdRequest(Ulb, &Payload->udr, &NextState);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to process setup packet: %r", Status);

    // Stall on error or unhandled request
    Status = Ulb->UsbfnIo->SetEndpointStallState(
        Ulb->UsbfnIo,
        USBLB_CONTROL_EP,
        USB_REQUEST_EPDIR(Payload->udr.RequestType),
        TRUE);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "SetEndpointStallState failed: %r", Status);
    }
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Process control transfer Rx events.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
ULB_STATE Ulb_Common_EpStatusChangedRx (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  EFI_STATUS Status;
  ULB_STATE  NextState;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgEndpointStatusChangedRx), ULB_STATE_SAME);

  Status = ProcessCtrlEpStatusChangedRx(Ulb, &Payload->utr, &NextState);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR,"Failed to update control transfer: %r", Status);

    // Stall on error
    Status = Ulb->UsbfnIo->SetEndpointStallState(
        Ulb->UsbfnIo,
        USBLB_CONTROL_EP,
        EfiUsbEndpointDirectionDeviceTx,
        TRUE);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "SetEndpointStallState failed: %r", Status);
    }
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Process control transfer Tx events.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
ULB_STATE Ulb_Common_EpStatusChangedTx (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  EFI_STATUS Status;
  ULB_STATE  NextState;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgEndpointStatusChangedTx), ULB_STATE_SAME);

  Status = ProcessCtrlEpStatusChangedTx(Ulb, &Payload->utr, &NextState);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR,"Failed to update control transfer: %r", Status);

    // Stall on error
    Status = Ulb->UsbfnIo->SetEndpointStallState(
        Ulb->UsbfnIo,
        USBLB_CONTROL_EP,
        EfiUsbEndpointDirectionDeviceRx,
        TRUE);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "SetEndpointStallState failed: %r", Status);
    }
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Save the bus speed.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on speed
 */
ULB_STATE Ulb_Common_Speed (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  ULB_STATE NextState = ULB_STATE_SAME;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgBusEventSpeed), ULB_STATE_SAME);

  Ulb->BusSpeed = Payload->ubs;
  PRINT(APL_ALWAYS, "Bus Speed: %a", PrintBusSpeed(Ulb->BusSpeed));

  // Only advance from ATTACHED to DEFAULT/ADDRESSED on SS connection
  if (Ulb->BusSpeed == UsbBusSpeedSuper) {
    NextState = ULB_STATE_DEFAULT_ADDRESSED;
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
 * ULB_STATE_DETACHED functions
 ******************************************************************************/

/**
 * @brief Initialize/clear internal state.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID Ulb_Detached_Entry (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState)
{
  FNC_ENTER_MSG();

  Ulb->PortType = EfiUsbUnknownPort;
  Ulb->BusSpeed = UsbBusSpeedUnknown;
  Ulb->ResumeState = ULB_INITIAL_STATE;
  Ulb->RcvLbTestConfig = FALSE;

  FNC_LEAVE_MSG();
}


/**
 * @brief Just move to attached state.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return ULB_STATE_ATTACHED
 */
ULB_STATE Ulb_Detached_Attach (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  PRINT(APL_ALWAYS, "Cable Attached");
  return ULB_STATE_ATTACHED;
}


/*******************************************************************************
 * ULB_STATE_ATTACHED functions
 ******************************************************************************/

/**
 * @brief Detect the port type when cable is attached.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID Ulb_Attached_Entry (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState)
{
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  Status = Ulb->UsbfnIo->DetectPort(Ulb->UsbfnIo, &Ulb->PortType);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to detect port type: %r", Status);
  } else {
    PRINT(APL_ALWAYS, "Port Type is %a", PrintPortType(Ulb->PortType));
  }

  FNC_LEAVE_MSG();
}


/*******************************************************************************
 * ULB_STATE_DEFAULT_ADDRESSED functions
 ******************************************************************************/

/*******************************************************************************
 * ULB_STATE_CONFIGURED functions
 ******************************************************************************/

/**
 * @brief Stop the loopback timer
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID Ulb_Configured_Entry (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState)
{
  EFI_STATUS Status;

  Status = gBS->SetTimer(Ulb->LbTestData.TimeoutTimer, TimerCancel, 0);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to cancel loopback timer: %r", Status);
    return;
  }
  PRINT(APL_VVERB, "Cancelled loopback timer");
}


/**
 * @brief Process standard and class USB requests.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
ULB_STATE Ulb_Configured_SetupPkt (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  ULB_STATE  NextState = ULB_STATE_SAME;
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  // Process class requests if not standard
  Status = ProcessStdRequest(Ulb, &Payload->udr, &NextState);
  if (EFI_UNSUPPORTED == Status) {
    Status = ProcessClsRequest(Ulb, &Payload->udr, &NextState);
    if (EFI_UNSUPPORTED == Status) {
      PRINT(APL_ERROR, "Unhandled request 0x%02x", Payload->udr.RequestType);
    }
  }

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to process setup packet: %r", Status);

    // Stall on error or unhandled request
    Status = Ulb->UsbfnIo->SetEndpointStallState(
        Ulb->UsbfnIo,
        USBLB_CONTROL_EP,
        (Payload->udr.RequestType >> 7) & 1
          ? EfiUsbEndpointDirectionHostIn : EfiUsbEndpointDirectionHostOut,
        TRUE);
    if (EFI_ERROR(Status)) {
      PRINT(APL_ERROR, "SetEndpointStallState failed: %r", Status);
    }
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
 * ULB_STATE_LOOPBACK functions
 ******************************************************************************/

/**
 * @brief Initialize the test data and start the loopback transfers.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID Ulb_Loopback_Entry (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState)
{
  EFI_STATUS  Status;
  UINT16      MaxPktSize;
  FNC_ENTER_MSG();

  PRINT(APL_ALWAYS, "Loopback test started\n"
      "    Mode:       %d\n"
      "    Iterations: %d\n"
      "    BufSize:    %d",
      Ulb->LbTestConfig.Mode,
      Ulb->LbTestConfig.Iterations,
      Ulb->LbTestConfig.BufSize);

  // clear ZLT falgs, will be set below if needed
  Ulb->LbTestData.RxZlt = FALSE;
  Ulb->LbTestData.TxZlt = FALSE;

  // get the maximum packet size to determine if ZLT is necessary
  Status = Ulb->UsbfnIo->GetEndpointMaxPacketSize(
      Ulb->UsbfnIo,
      UsbEndpointBulk,
      Ulb->BusSpeed,
      &MaxPktSize);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to get bulk ep max packet size: %r", Status);
  } else if ((Ulb->LbTestConfig.BufSize % MaxPktSize) == 0) {
    if (!Ulb->RxUsbfnZlt) {
      Ulb->LbTestData.RxZlt = TRUE;
    }
    if (!Ulb->TxUsbfnZlt) {
      Ulb->LbTestData.TxZlt = TRUE;
    }
  }

  // setup the first data transfer
  Ulb->LbTestData.RxIteration = 0;
  Ulb->LbTestData.TxIteration = 0;
  Ulb->LbTestData.Xfer.BufferOffset = 0;
  Ulb->LbTestData.Xfer.XferSize = Ulb->LbTestConfig.BufSize;
  switch (Ulb->LbTestConfig.Mode) {

    case LB_TEST_MODE_TX_ONLY:
      SetMem(Ulb->LbTestData.Xfer.Buffer, Ulb->LbTestData.Xfer.BufferSize, 0xA5);
      Ulb->LbTestData.Xfer.Direction = EfiUsbEndpointDirectionDeviceTx;
      break;

    case LB_TEST_MODE_RX_ONLY:
    case LB_TEST_MODE_LOOPBACK:
      Ulb->LbTestData.Xfer.Direction = EfiUsbEndpointDirectionDeviceRx;
      break;
  }

  // submit the transfer
  Status = IssueXfer(Ulb->UsbfnIo, &Ulb->LbTestData.Xfer);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to submit loopback transfer: %r", Status);
    StallBulkEndpoints(Ulb->UsbfnIo);
  } else {
    SetLoopbackTimer(Ulb);
  }

  FNC_LEAVE_MSG();
}


/**
 * @brief Process control and bulk transfer Rx events.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
ULB_STATE Ulb_Loopback_EpStatusChangedRx (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  EFI_STATUS Status = EFI_SUCCESS;
  ULB_STATE  NextState = ULB_STATE_SAME;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgEndpointStatusChangedRx), ULB_STATE_SAME);

  // Process both control and bulk ep events
  switch (Payload->utr.EndpointIndex) {

    case USBLB_CONTROL_EP:
      Status = ProcessCtrlEpStatusChangedRx(Ulb, &Payload->utr, &NextState);

      // Stall opposite control ep on error
      if (EFI_ERROR(Status)) {
        PRINT(APL_ERROR,"Failed to update control transfer: %r", Status);
        Status = Ulb->UsbfnIo->SetEndpointStallState(
            Ulb->UsbfnIo,
            Payload->utr.EndpointIndex,
            EfiUsbEndpointDirectionDeviceTx,
            TRUE);
        if (EFI_ERROR(Status)) {
          PRINT(APL_ERROR, "SetEndpointStallState failed: %r", Status);
        }
      }
      break;

    case USBLB_BULK_EP:
      Status = ProcessBulkEpStatusChangedRx(Ulb, &Payload->utr, &NextState);

      // Stall both bulk eps on error
      if (EFI_ERROR(Status)) {
        PRINT(APL_ERROR,"Failed to update bulk transfer: %r", Status);
        StallBulkEndpoints(Ulb->UsbfnIo);
      }
      break;

    default:
      PRINT(APL_ERROR, "EfiUsbMsgEndpointStatusChangedRx received on invalid ep: 0x%02x",
          ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
      break;
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Process control and bulk transfer Tx events.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
ULB_STATE Ulb_Loopback_EpStatusChangedTx (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  EFI_STATUS Status = EFI_SUCCESS;
  ULB_STATE  NextState = ULB_STATE_SAME;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgEndpointStatusChangedTx), ULB_STATE_SAME);

  // Process both control and bulk ep events
  switch (Payload->utr.EndpointIndex) {

    case USBLB_CONTROL_EP:
      Status = ProcessCtrlEpStatusChangedTx(Ulb, &Payload->utr, &NextState);

      // Stall opposite control ep on error
      if (EFI_ERROR(Status)) {
        PRINT(APL_ERROR,"Failed to update control transfer: %r", Status);
        Status = Ulb->UsbfnIo->SetEndpointStallState(
            Ulb->UsbfnIo,
            Payload->utr.EndpointIndex,
            EfiUsbEndpointDirectionDeviceRx,
            TRUE);
        if (EFI_ERROR(Status)) {
          PRINT(APL_ERROR, "SetEndpointStallState failed: %r", Status);
        }
      }
      break;

    case USBLB_BULK_EP:
      Status = ProcessBulkEpStatusChangedTx(Ulb, &Payload->utr, &NextState);

      // Stall both bulk eps on error
      if (EFI_ERROR(Status)) {
        PRINT(APL_ERROR,"Failed to update bulk transfer: %r", Status);
        StallBulkEndpoints(Ulb->UsbfnIo);
      }
      break;

    default:
      PRINT(APL_ERROR, "EfiUsbMsgEndpointStatusChangedTx received on invalid ep: 0x%02x",
          ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
      break;
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
 * ULB_STATE_SUSPENDED functions
 ******************************************************************************/

/**
 * @brief Save the previous state on suspend.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID Ulb_Suspended_Entry (ULB_STATE_MACHINE *Ulb, ULB_STATE PrevState)
{
  Ulb->ResumeState = PrevState;
  PRINT(APL_ALWAYS, "Bus suspended in state %a", UlbSmGetStateName(PrevState));
}


/**
 * @brief Return to state saved on suspend entry.
 *
 * @param[in] Ulb           Usbfn loopback state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return State saved on suspend
 */
ULB_STATE Ulb_Suspended_Resume (
  IN ULB_STATE_MACHINE          *Ulb,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  PRINT(APL_ALWAYS, "Resuming to state %a", UlbSmGetStateName(Ulb->ResumeState));
  return Ulb->ResumeState;
}

