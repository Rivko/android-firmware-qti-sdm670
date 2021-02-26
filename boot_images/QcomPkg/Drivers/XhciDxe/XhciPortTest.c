/** @file XhciPortTest.c

  QCOM_USB_PORT_TEST_PROTOCOL driver.

  Copyright (c) 2011 - 2017, QUALCOMM Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 09/04/17   gk      UEFI USB compliance test changes
 04/30/14   amitg   Updates Assert with USB Assert Macro
 06/06/13   mliss    Initial draft

=============================================================================*/

#include <Base.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/HAL_hsusb.h>
#include <Library/HAL_ssusb.h>
#include <Library/UsbConfigLib.h>
#include <Protocol/EFIUsbConfig.h>
#include "Xhci.h"


// get the Xhci instance from PortTestProtocol this pointer
#define XHC_FROM_PORTTEST_THIS(a)               CR(a, USB_XHCI_INSTANCE, UsbPortTest, XHCI_INSTANCE_SIG)


/**
  Run usb port test. Host mode procedures defined in EHCI spec 4.14.
   
  @param  TestMode   Test mode 
  @param  Data       [IN] *Data =1 means device mode. [OUT] Test results for some tests.

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Unsupported test mode or data is null 
                                  when output data is expected.
  @retval EFI_NOT_FOUND           Failed to locate USB config protocol

**/
EFI_STATUS
EFIAPI
XhcRunUsbPortTest (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,
  IN  QCOM_USB_PORT_TEST_MODE         TestMode,
  IN OUT UINTN                        *Data
  );

/**
  Set parameter used in Port Test.
   
  @param  ParamType   Parameter type 
  @param  ParamValue  Parameter value

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Parameter type not valid.

**/
EFI_STATUS
XhcPortTestSetParam (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,            
  IN  QCOM_USB_PORT_TEST_PARAM        ParamType,
  IN  INTN                            ParamValue
  );

/**
  Get parameter used in Port Test.
   
  @param  ParamType   Parameter type 
  @param  ParamValue  Parameter value

  @retval EFI_SUCCESS             Success.
  @retval EFI_INVALID_PARAMETER   Parameter type or pointer not valid.

**/
EFI_STATUS
XhcPortTestGetParam (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,            
  IN  QCOM_USB_PORT_TEST_PARAM        ParamType,
  IN  INTN                            *ParamValue
  );

/**
  Uniquely identify an instance of this protocol.

  @param This       Protocol instance pointer

  @return Identification string
**/
CHAR16 *
XhcPortTestIdentify (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This
  );


//
// The global UsbPortTest protocol instance for Xhci driver.
//
QCOM_USB_PORT_TEST_PROTOCOL gXhciUsbPortTestTemplate = {
  QCOM_USB_PORT_TEST_PROTOCOL_REVISION,
  XhcRunUsbPortTest,
  XhcPortTestGetParam,
  XhcPortTestSetParam,
  XhcPortTestIdentify,
  -1
};


//
// Protocol implementation
//


/**
  Helper function to disable all device slots and ports.

  @param [in] Xhc       The XHCI instance pointer

  @return EFI_SUCCESS   All device slots and ports were disabled
  @return others        Failed to disable at least 1 port
**/
EFI_STATUS
XhcDisableAllDeviceSlotsAndPorts (
  IN USB_XHCI_INSTANCE      *Xhc
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN       Index;

  if (NULL == Xhc) {
    return EFI_INVALID_PARAMETER;
  }

  // Disable all device slots, entry 0 is reserved.
  for (Index = 1; Index < 256; Index++) {
    if (!Xhc->UsbDevContext[Index].Enabled ||
        (Xhc->UsbDevContext[Index].SlotId == 0)) {
      continue;
    }
    if (Xhc->HcCParams.Data.Csz == 0) {
      XhcDisableSlotCmd (Xhc, Xhc->UsbDevContext[Index].SlotId);
    } else {
      XhcDisableSlotCmd64 (Xhc, Xhc->UsbDevContext[Index].SlotId);
    }
  }

  // Disable all ports
  for (Index = 0; Index < Xhc->HcSParams1.Data.MaxPorts; Index++) {
    EFI_STATUS WaitStatus;

    // Clear PP
    XhcClearOpRegBit(Xhc, XHC_PORTSC_OFFSET + (0x10 * Index), XHC_PORTSC_PP);

    // Wait for PP to clear, save error status but continue to other ports on timeout
    WaitStatus = XhcWaitOpRegBit(Xhc, XHC_PORTSC_OFFSET + (0x10 * Index), XHC_PORTSC_PP, FALSE, XHC_GENERIC_TIMEOUT);
    if (EFI_ERROR(WaitStatus)) {
      Status = WaitStatus;
      DBG(EFI_D_ERROR, "Failed to disable port %d", Index);
    }
  }

  return Status;
}


/**
  Stop endpoint traffic.

  @param  Xhc           The XHCI Instance.
  @param  SlotId        The slot id to be evaluated.
  @param  Dci           The device context index of the endpoint to stop.

  @retval EFI_SUCCESS   Successfully evaluate the device endpoint 0.

**/
STATIC
EFI_STATUS
XhcStopEndpointCmd (
  IN USB_XHCI_INSTANCE        *Xhc,
  IN UINT8                    SlotId,
  IN UINT8                    Dci
  )
{
  EFI_STATUS                  Status;
  CMD_TRB_STOP_ENDPOINT       CmdTrbStopEp;
  EVT_TRB_COMMAND_COMPLETION  *EvtTrb;

  USB_ASSERT_RETURN((Xhc->UsbDevContext[SlotId].SlotId != 0), EFI_INVALID_PARAMETER);
  ZeroMem (&CmdTrbStopEp, sizeof (CmdTrbStopEp));
  CmdTrbStopEp.CycleBit = 1;
  CmdTrbStopEp.Type     = TRB_TYPE_STOP_ENDPOINT;
  CmdTrbStopEp.EDID     = Dci;
  CmdTrbStopEp.SlotId   = Xhc->UsbDevContext[SlotId].SlotId;
  DBG (EFI_D_INFO, "Stop Endpoint");
  Status = XhcCmdTransfer (
             Xhc,
             (TRB_TEMPLATE *) (UINTN) &CmdTrbStopEp,
             XHC_GENERIC_TIMEOUT,
             (TRB_TEMPLATE **) (UINTN) &EvtTrb
             );
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Stop Endpoint command failed");
  }

  return Status;
}


/**
  Stop polling async transfers.

  @param  Xhc           The XHCI Instance.
**/
STATIC
VOID
XhcStopAsyncTransfers (
  IN USB_XHCI_INSTANCE        *Xhc
  )
{
  LIST_ENTRY              *Entry;
  LIST_ENTRY              *Next;
  URB                     *Urb = NULL;

  EFI_LIST_FOR_EACH_SAFE (Entry, Next, &Xhc->AsyncIntTransfers) {
    Urb = EFI_LIST_CONTAINER (Entry, URB, UrbList);
    RemoveEntryList (&Urb->UrbList);
    InsertTailList (&Xhc->SavedAsyncTransfers, &Urb->UrbList);
  }
}


/**
  Resume polling async transfers.

  @param  Xhc           The XHCI Instance.
**/
STATIC
VOID
XhcRestoreAsyncTransfers (
  IN USB_XHCI_INSTANCE        *Xhc
  )
{
  LIST_ENTRY              *Entry;
  LIST_ENTRY              *Next;
  URB                     *Urb = NULL;

  EFI_LIST_FOR_EACH_SAFE (Entry, Next, &Xhc->SavedAsyncTransfers) {
    Urb = EFI_LIST_CONTAINER (Entry, URB, UrbList);
    RemoveEntryList (&Urb->UrbList);
    InsertTailList (&Xhc->AsyncIntTransfers, &Urb->UrbList);
  }
}


EFI_STATUS
EFIAPI
XhcRunUsbPortTest (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,
  IN  QCOM_USB_PORT_TEST_MODE         TestMode,
  IN OUT UINTN                        *Data
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  USB_XHCI_INSTANCE         *Xhc;
  UINT32                    RegVal;
  BOOLEAN                   Use64;
  DEVICE_CONTEXT            *DevContext;
  DEVICE_CONTEXT_64         *DevContext64;
  UINT16                    SlotId;
  UINT8                     Dci;


  if (NULL == This) {
    Status = EFI_INVALID_PARAMETER;
    DBG (EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  // Get the Xhci instance pointer from the protocol
  Xhc = XHC_FROM_PORTTEST_THIS (This);

  Use64 = Xhc->HcCParams.Data.Csz;

  DBG (EFI_D_INFO, "USB port test mode 0x%x", TestMode);

  switch (TestMode) {
    case USB_PORT_TEST_GET_DISCONNECT_STATUS:
      // Retrieve the connection status
      if ( Data == NULL ) {
          return EFI_INVALID_PARAMETER;
      }
      *Data = XhcReadOpReg(Xhc, XHC_PORTSC_OFFSET) & XHC_PORTSC_CCS; // port 0 is HS port
      break;
    
    case USB_PORT_TEST_DISABLE_PERIODIC:

      // remove transfers from the async list to prevent them being restarted
      // after we issue a stop command
      XhcStopAsyncTransfers(Xhc);

      // For each enabled device slot
      //   For each endpoint context (up to max defined in slot context)
      //     execute stop endpoint command if endpoint is running

      // entry 0 in the UsbDeviceContext is reserved
      for (SlotId = 1; SlotId < 256; ++SlotId) {

        // do nothing if slot isn't enabled
        if (Xhc->UsbDevContext[SlotId].Enabled == 0) {
          continue;
        }

        // use 64 bit structures
        if (Use64) {
          DevContext64 = Xhc->UsbDevContext[SlotId].OutputContext;

          // ignore control endpoint on Dci = 1
          for (Dci = 2; Dci <= DevContext64->Slot.ContextEntries; ++Dci) {

            // software uses 0 based indexing in the EP array, which
            // corresponds to the (Device Context Index - 1)
            if (DevContext64->EP[Dci-1].EPState == 1) {
              Status = XhcStopEndpointCmd(Xhc, (UINT8)SlotId, Dci);
              if (EFI_ERROR(Status)) {
                DBG(EFI_D_ERROR, "Failed to stop traffic on DCI %d of slot %d", Dci, SlotId);
                goto ON_EXIT;
              }
            }
          } // end for Dci

        // use 32 bit structures
        } else {
          DevContext = Xhc->UsbDevContext[SlotId].OutputContext;

          // ignore control endpoint on Dci = 1
          for (Dci = 2; Dci <= DevContext->Slot.ContextEntries; ++Dci) {

            // software uses 0 based indexing in the EP array, which
            // corresponds to the (Device Context Index - 1)
            if (DevContext->EP[Dci-1].EPState == 1) {
              Status = XhcStopEndpointCmd(Xhc, (UINT8)SlotId, Dci);
              if (EFI_ERROR(Status)) {
                DBG(EFI_D_ERROR, "Failed to stop traffic on DCI %d of slot %d", Dci, SlotId);
                goto ON_EXIT;
              }
            }
          } // end for Dci
        }
      } // end for SlotId

      break;

    case USB_PORT_TEST_ENABLE_PERIODIC:

      // For each enabled device slot
      //   For each endpoint context (up to max defined in slot context)
      //     ring endpoint doorbell if stopped

      // entry 0 in the UsbDeviceContext is reserved
      for (SlotId = 1; SlotId < 256; ++SlotId) {

        // do nothing if slot isn't enabled
        if (Xhc->UsbDevContext[SlotId].Enabled == 0) {
          continue;
        }

        // use 64 bit structures
        if (Use64) {
          DevContext64 = Xhc->UsbDevContext[SlotId].OutputContext;

          // ignore control endpoint on Dci = 1
          for (Dci = 2; Dci <= DevContext64->Slot.ContextEntries; ++Dci) {

            // software uses 0 based indexing in the EP array, which
            // corresponds to the (Device Context Index - 1)
            if (DevContext64->EP[Dci-1].EPState == 3) {
              Status = XhcRingDoorBell(Xhc, (UINT8)SlotId, Dci);
              if (EFI_ERROR(Status)) {
                DBG(EFI_D_ERROR, "Failed to ring doorbell on DCI %d of slot %d", Dci, SlotId);
                goto ON_EXIT;
              }
            }
          } // end for Dci

        // use 32 bit structures
        } else {
          DevContext = Xhc->UsbDevContext[SlotId].OutputContext;

          // ignore control endpoint on Dci = 1
          for (Dci = 2; Dci <= DevContext->Slot.ContextEntries; ++Dci) {

            // software uses 0 based indexing in the EP array, which
            // corresponds to the (Device Context Index - 1)
            if (DevContext->EP[Dci-1].EPState == 3) {
              Status = XhcRingDoorBell(Xhc, (UINT8)SlotId, Dci);
              if (EFI_ERROR(Status)) {
                DBG(EFI_D_ERROR, "Failed to ring doorbell on DCI %d of slot %d", Dci, SlotId);
                goto ON_EXIT;
              }
            }
          } // end for Dci
        }
      } // end for SlotId

      // restore saved async transfers to the pending list to resume polling them
      XhcRestoreAsyncTransfers(Xhc);

      break;
    
    case USB_PORT_TEST_DISABLE:
      // Clear the port test bits (port 0 is HS)
      RegVal = XhcReadOpReg(Xhc, XHC_PORTPMSC_OFFSET);
      RegVal &= ~XHC_PORTPMSC2_PTC;
      XhcWriteOpReg(Xhc, XHC_PORTPMSC_OFFSET, RegVal);

      // Halt (if needed) and reset the controller to end test mode
      Status = XhcResetHC(Xhc, XHC_GENERIC_TIMEOUT);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Faile to reset host controller");
      }
      break;

    case USB_PORT_TEST_J_STATE:
    case USB_PORT_TEST_K_STATE:
    case USB_PORT_TEST_SE0_NAK:
    case USB_PORT_TEST_PACKET:
    case USB_PORT_TEST_FORCE_ENABLE:

      // Disable all device slots and ports
      Status = XhcDisableAllDeviceSlotsAndPorts(Xhc);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Failed to disable device slots and ports, not enabling test mode");
        goto ON_EXIT;
      }

      // Stop the controller and wait for it to halt
      Status = XhcHaltHC(Xhc, XHC_GENERIC_TIMEOUT);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Failed to halt the host controller");
        goto ON_EXIT;
      }

      // Set Test Mode (port 0 is HS)
      RegVal = XhcReadOpReg(Xhc, XHC_PORTPMSC_OFFSET);
      RegVal &= ~XHC_PORTPMSC2_PTC;
      XhcWriteOpReg(Xhc, XHC_PORTPMSC_OFFSET, RegVal);
      RegVal |= TestMode << XHC_PORTPMSC2_PTC_SHFT;
      XhcWriteOpReg(Xhc, XHC_PORTPMSC_OFFSET, RegVal);

      // Start the controller for the FORCE_ENABLE test
      if (TestMode == USB_PORT_TEST_FORCE_ENABLE) {
        Status = XhcRunHC(Xhc, XHC_GENERIC_TIMEOUT);
        if (EFI_ERROR(Status)) {
          DBG(EFI_D_ERROR, "Failed to start host controller");
        }
      }
      break;
	  
	case USB_PORT_TEST_ENABLE_SS_COMPLIANCE:
      // Set SS compliance mode (port 1 is SS)
      XhcWriteOpReg(Xhc, XHC_PORTSC_OFFSET + (0x10 * 1), XHC_PORTSC_LWS|XHC_PORTSC_PP|XHC_PORTSC_PLS_COMPLIANCE_MASK);
    break;
	
    default:
      DBG (EFI_D_WARN, "invalid USB port test mode");
      Status = EFI_INVALID_PARAMETER;
      break;
  }

ON_EXIT: 
  return Status;
}


EFI_STATUS
XhcPortTestSetParam (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,            
  IN  QCOM_USB_PORT_TEST_PARAM        ParamType,
  IN  INTN                            ParamValue
  )
{
  if (NULL == This) {
    return EFI_INVALID_PARAMETER;
  }

  switch (ParamType)
  {
    case USB_PORT_TEST_PARAM_QTD_INDEX: 
      This->QtdIndex = ParamValue;
      break;
    default:
      return EFI_INVALID_PARAMETER;     
  }
  return EFI_SUCCESS;
}


EFI_STATUS
XhcPortTestGetParam (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This,            
  IN  QCOM_USB_PORT_TEST_PARAM        ParamType,
  IN  INTN                            *ParamValue
  )
{
  if (NULL == This || NULL == ParamValue) {
    return EFI_INVALID_PARAMETER;
  }

  switch (ParamType)
  {
    case USB_PORT_TEST_PARAM_QTD_INDEX: 
      *ParamValue = This->QtdIndex;
      break;
    default:
      return EFI_INVALID_PARAMETER;     
  }
  return EFI_SUCCESS;
}


CHAR16 *
XhcPortTestIdentify (
  IN  QCOM_USB_PORT_TEST_PROTOCOL     *This
  )
{
  USB_XHCI_INSTANCE         *Xhc;

  if (NULL == This) {
    DBG (EFI_D_ERROR, "invalid parameter");
    return L"XHCI PortTest, unknown instance";
  }

  Xhc = XHC_FROM_PORTTEST_THIS (This);

  return Xhc->UsbPortTestIdentity;
}


