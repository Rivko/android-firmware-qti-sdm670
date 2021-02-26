/** @file Usbfn.c

  USB Function Driver which supports the USB Function Protocol

  Copyright (c) 2010-2016, Qualcomm Technologies Inc. All rights reserved.
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


 when       who      what, where, why
 --------   ---      -----------------------------------------------------------
 09/13/16   jt       Change SdccCardInfo to MemCardInfo 
 07/13/15   al       Changing TPL_NOTIFY to TPL_CALLBACK
 01/20/15   ck       Update and clean up EFIUsbfnIo Protocol
 12/12/14   arung    Porting UEFI LPM support
 10/04/14   ck       Fix mass storage app issue in 64 bit UEFI
 08/13/14   amitg    Clean Up Code
 06/09/14   ck       Update EFI_USBFN_IO_PROTOCOL_REVISION
 06/05/14   amitg    Fix the Revision Number
 04/30/14   ar       Cleanup for NDEBUG build  
 10/13/11   cching   port the change to return usb msg in order
 06/24/11   cching   port the Usbfn to UEFI
 08/11/11   sbryksin Changes for 8960v2 w/ HSIC
 =============================================================================*/
//
// Includes
//
#include "Usbfn.h"
#include <Library/UsbfnLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/UsbConfigLib.h>
#include <Library/HAL_hsusb.h>
#include <Library/IoLib.h>
#include <Protocol/LoadedImage.h>
#include <Guid/EventGroup.h>
#include <Protocol/EFIUsbConfig.h>
#include "UsbDci.h"
#include <Library/UncachedMemoryAllocationLib.h>
#include <Protocol/EFICardInfo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/EFIUsbPortTest.h>
#define USB_FEATURE_SELECTOR_TEST_MODE             2

#define MAX_USBFN_MESSAGE_SIZE 5

// external core base address
extern UINTN dci_core_base_address[DCI_MAX_CORES];
extern EFI_GUID gEfiEventEnterLPMGuid;
  
EFI_DRIVER_BINDING_PROTOCOL
gUsbFnDriverBinding = {
  UsbFnDriverBindingSupported,
  UsbFnDriverBindingStart,
  UsbFnDriverBindingStop,
  0x30,
  NULL,
  NULL
};

//
// Statics
//
STATIC
USBFN_DEV
*Usbfn =  NULL;

STATIC
CONST
uint8
str_manuf[] = {
  'Q',0,
  'u',0,
  'a',0,
  'l',0,
  'c',0,
  'o',0,
  'm',0,
  'm',0,
  ' ',0,
  'C',0,
  'D',0,
  'M',0,
  'A',0,
  ' ',0,
  'T',0,
  'e',0,
  'c',0,
  'h',0,
  'n',0,
  'o',0,
  'l',0,
  'o',0,
  'g',0,
  'i',0,
  'e',0,
  's',0,
  ' ',0,
  'M',0,
  'S',0,
  'M',0,
};

STATIC
CONST
uint8
str_product[] = {
  'Q',0,
  'U',0,
  'A',0,
  'L',0,
  'C',0,
  'O',0,
  'M',0,
  'M',0,
  ' ',0,
  'M',0,
  'S',0,
  'M',0,
  ' ',0,
  'D',0,
  'E',0,
  'V',0,
  'I',0,
  'C',0,
  'E',0,
};

#ifndef MDEPKG_NDEBUG
STATIC
CONST
CHAR8
*MsgToStr[] =
{
  "EfiUsbMsgNone",
  "EfiUsbMsgSetupPacket",
  "EfiUsbMsgEndpointStatusChangedRx",
  "EfiUsbMsgEndpointStatusChangedTx",
  "EfiUsbMsgBusEventDetach",
  "EfiUsbMsgBusEventAttach",
  "EfiUsbMsgBusEventReset",
  "EfiUsbMsgBusEventSuspend",
  "EfiUsbMsgBusEventResume",
  "EfiUsbMsgBusEventSpeed",
};
#endif // MDEPKG_NDEBUG


//
// Implementation: Static Miscellaneous
//

STATIC
EFI_STATUS
HandleSetZLTSupport (
  IN EFI_USBFN_IO_PROTOCOL*        This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN BOOLEAN                       Enable
  );

// Returns URB associated with endpoint, reusable URB, or NULL.
STATIC
struct qhsusb_urb *
FindUrb (
  IN struct qhsusb_dcd_dsc_device  *dev,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction
  )
{
  UINT8 endpoint_address = ENDPOINT_ADDR(EndpointIndex, Direction);
  UINTN i = 0;

  USB_ASSERT_RETURN (dev, NULL);

  // Find existing endpoint URB.
  for (i = 0; i < MAX_URBS; ++i) {
    if (dev->Urb[i].endpoint_address == endpoint_address) {
      return &dev->Urb[i];
    }
  }

  // URB not found. Return reusable URB.
  for (i = 0; i < MAX_URBS; ++i) {
    if (dev->Urb[i].transfer_status == QHSUSB_URB_STATUS_REUSABLE) {
      return &dev->Urb[i];
    }
  }

  return NULL;
}

STATIC
EFI_STATUS
FindEndpoint (
  IN   USBFN_DEV                       *Usbfn,
  IN   UINT8                           EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION      Direction,
  OUT  struct qhsusb_dcd_dsc_endpoint  **endpoint
  )
{

  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  ARG_CHK (Usbfn && endpoint);

  if (!Usbfn->SWDevice.config_value) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  *endpoint = find_ep(&Usbfn->SWDevice,
      ENDPOINT_ADDR (EndpointIndex, Direction));

  // Device is enumerated but endpoint is not found: invalid endpoint address
  // for active configuration.
  ARG_CHK (*endpoint);

  ON_EXIT: FNC_LEAVE_MSG ();
  return Status;
}

/**
 * @brief Driver Enter Low Power mode function triggered by Charger App (Usbfn->EnterLPMEvent)
 *
 * Turns off all USB clocks
 *
 * @param  Event                   Pointer to this event
 * @param  Context                 Event hanlder private data
 */
VOID
EFIAPI
UsbfnEnterLPM (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  USBFN_DEV   *Usbfn = NULL;
  EFI_STATUS Status = EFI_SUCCESS;
  
  FNC_ENTER_MSG ();

  if (NULL == Context) {
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  // Get USBFN Context
  Usbfn = (USBFN_DEV *) Context;

  // Enter Low Power Mode
  Status = Usbfn->UsbConfigProtocol->EnterLPM(Usbfn->UsbConfigProtocol,
    Usbfn->UsbConfigProtocol->modeType, Usbfn->UsbConfigProtocol->coreNum);

  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR,"UsbfnEnterLPM: Unable to enter low power mode\r\n");
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
}


EFI_STATUS
usbfn_transfer (
  IN      USBFN_DEV                     *Usbfn,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN OUT  UINTN                         *BufferSize,
  IN OUT  VOID                          *Buffer
  )
{
  EFI_STATUS                      Status       = EFI_SUCCESS;
  struct qhsusb_dcd_dsc_endpoint *endpoint     = NULL;
  struct qhsusb_urb              *Urb          = NULL;
  VOID                           *DataBuffer   = NULL;
  static BOOLEAN                  PrintErrMsg  = TRUE;

  DataBuffer = Buffer;

  FNC_ENTER_MSG ();

  if(!Usbfn || !BufferSize || !Buffer){
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // Per spec, we must validate the endpoint.
  if (EndpointIndex) {
    Status = FindEndpoint(Usbfn, EndpointIndex, Direction, &endpoint);
    ERR_CHK (
        "unable to find endpoint index %01x, direction %01x",
        EndpointIndex,
        Direction
    );
  }

  // Get the next available completed URB or free URB, if any.
  Urb = FindUrb(&Usbfn->SWDevice, EndpointIndex, Direction);

  if (!Urb || Urb->transfer_status == QHSUSB_URB_STATUS_ACTIVE) {
    // Waiting for URB to land.
    Status = EFI_NOT_READY;
    goto ON_EXIT;
  }

  Urb->transfer_buffer_ptr_tx = NULL;

  if( !IS_ALIGNED(Buffer, USBFN_DEFAULT_BUFFER_ALIGN) ) {
    if(Direction == EfiUsbEndpointDirectionDeviceRx){
      DBG(EFI_D_ERROR,"Dir RX. Buffer is not 4k aligned");
      Status = EFI_INVALID_PARAMETER;
                  goto ON_EXIT;
    }
    else {
      // As a temp. workaround for FFU download. Copy the data a
      // 4k aligned buffer we created.
      if (PrintErrMsg) {
        DBG(EFI_D_WARN,"Dir TX. Buffer is not 4k aligned");
        PrintErrMsg = FALSE;
      }
      if(IS_NOT_32_BIT_ADDR(Buffer)){
        // CI core does not support 64 bit address
        USB_ASSERT_RETURN(0, EFI_INVALID_PARAMETER);
      }
      CopyMem(Usbfn->TxBuffer, Buffer, *BufferSize);
    }
    DataBuffer = Usbfn->TxBuffer;
    Urb->transfer_buffer_ptr_tx = Buffer;
  }

  // Queue next transfer.

  // TODO: if we can guarantee one URB per endpoint
  // (MAX_URBS >= ifc0->bNumEndpoints + ...), we may assign each endpoint one at
  // ConfigureEnableEndpoints() time. As it stands, we have an URB pool  that
  // may be exhausted by high endpoint configurations. This may make EP
  // validation cheaper.
  Urb->endpoint_address = ENDPOINT_ADDR (EndpointIndex, Direction);

  // Update URB with amount of data that can be sent or received.
  Urb->transfer_length = MIN( ((UINT32)*BufferSize), Usbfn->SWDevice.max_transfer_size[EndpointIndex][Direction]);

  // Update URB with buffer to be used.
  Urb->transfer_buffer_ptr = (UINT32) (UINTN) DataBuffer;

  // Queue.
  if (qhsusb_submit_urb(Urb)) {
    Status = EFI_DEVICE_ERROR;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
 Run usb test mode.

 @param  core_type        Usbfn core type
 @param  mode             Usb test mode

 @retval EFI_SUCCESS            Success
         EFI_INVALID_PARAMETER  Test mode not supported.

 **/
STATIC
EFI_STATUS
usb_test_mode (
  USBFN_DEV                   *usbfn,
  QCOM_USB_CONFIG_CORE_TYPE   core_type,
  UINTN                       mode
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  UINTN                           BufferSize = 0;
  UINTN                           BaseAddr;

  if (NULL == usbfn) {
    DBG (EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  BaseAddr = dci_core_base_address[usbfn->SWDevice.core_id];

  switch ( mode ) {
    case USB_PORT_TEST_J_STATE:
    case USB_PORT_TEST_K_STATE:
    case USB_PORT_TEST_SE0_NAK:
    case USB_PORT_TEST_PACKET:
    case USB_PORT_TEST_FORCE_ENABLE:
      // send a ZLP in status stage
      usbfn_transfer ( usbfn, 0, EfiUsbEndpointDirectionHostIn, &BufferSize, 0);

      // set the test mode bits
      HAL_HSUSB_AND (BaseAddr, HAL_HSUSB_PORTSC_ADDR(0), ~HAL_HSUSB_PORTSC_PTC_BMSK);
      HAL_HSUSB_OR  (BaseAddr, HAL_HSUSB_PORTSC_ADDR(0), mode << HAL_HSUSB_PORTSC_PTC_SHFT);

      DBG ( EFI_D_INFO, "USB in test mode: %d\n", mode);
      break;

    default:
       DBG ( EFI_D_ERROR, "Test mode not supported: 0x%x\n", mode );
       Status = EFI_INVALID_PARAMETER;
       break;
  }


ON_EXIT:
    return Status;

}


/**
 A callback function invoked when a setup pkt whose recipient is usb device is
 received.

 @param  dev              A USB device
 @param  pkt              A setup pkt received.

 @retval 0                Legacy return value.

 **/
STATIC
int
setup_dev_cb (
  struct qhsusb_dcd_dsc_device  *dev,
  struct usb_setup_packet       *pkt
  )
{
  EFI_USBFN_MESSAGE_NODE* NewMsg = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  if(!dev || !pkt) {
    DBG ( EFI_D_ERROR, "None of dev or pkt should be NULL. dev = 0x%p, pkt = 0x%p", dev, pkt);
    goto ON_EXIT;
  }

  //
  // USB test mode is handled locally in usbfn.
  //
  if ((pkt->bRequest == USB_SETUP_REQ_SET_FEATURE) &&
      (pkt->wValue == USB_FEATURE_SELECTOR_TEST_MODE) &&
      ((pkt->wIndex & 0xff) == 0)) {
    usb_test_mode ( dev->usbfn, dev->usbfn->CoreType, pkt->wIndex >> 8);
    goto ON_EXIT;
  }

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&dev->UsbfnMessagesMemPool);
  USB_ASSERT_RETURN(ListEntry, -1);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = EfiUsbMsgSetupPacket;
  CopyMem (&(NewMsg->Data.SetupCbRequest), pkt, sizeof (NewMsg->Data.SetupCbRequest));
  InsertTailList(&dev->UsbfnMessages, &NewMsg->Link);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return 0;
}

/**
 A callback function invoked when a setup pkt whose recipient is usb interface is
 received.

 @param  ifc              A USB interface
 @param  pkt              A setup pkt received.

 @retval 0                Legacy return value.

 **/
STATIC
int
setup_ifc_cb (
  struct qhsusb_dcd_dsc_interface  *ifc,
  struct usb_setup_packet          *pkt
  )
{
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  if (!ifc || !pkt) {
    DBG(EFI_D_ERROR, "None of ifc or pkt should be NULL. ifc = 0x%p, pkt = 0x%p", ifc, pkt);
    goto ON_EXIT;
  }

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&ifc->configuration->device->UsbfnMessagesMemPool);
  ASSERT(ListEntry);
  if(!ListEntry){
    DBG(EFI_D_ERROR,"setup_ifc_cb: UsbfnMessagesMemPool is empty, drop setup message");
    goto ON_EXIT;
  }

  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = EfiUsbMsgSetupPacket;
  CopyMem (&(NewMsg->Data.SetupCbRequest), pkt, sizeof (NewMsg->Data.SetupCbRequest));
  InsertTailList(& ifc->configuration->device->UsbfnMessages, &NewMsg->Link);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return 0;
}

/**
 A callback function invoked when a setup pkt whose recipient is usb endpoint is
 received.

 @param  ep               A USB endpoint
 @param  pkt              A setup pkt received.

 @retval 0                Legacy return value.

 **/
STATIC
int
setup_ep_cb (
  struct qhsusb_dcd_dsc_endpoint   *ep,
  struct usb_setup_packet          *pkt
  )
{
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  if (!ep || !pkt) {
    DBG(EFI_D_ERROR, "None of ep or pkt should be NULL. ifc = 0x%p, pkt = 0x%p", ep, pkt);
    goto ON_EXIT;
  }

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&ep->ifc->configuration->device->UsbfnMessagesMemPool);

  ASSERT(ListEntry);
  if(!ListEntry){
    DBG(EFI_D_ERROR,"setup_ep_cb: UsbfnMessagesMemPool is empty, drop setup message");
    goto ON_EXIT;
  }

  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = EfiUsbMsgSetupPacket;
  CopyMem (&(NewMsg->Data.SetupCbRequest), pkt, sizeof (NewMsg->Data.SetupCbRequest));
  InsertTailList(&ep->ifc->configuration->device->UsbfnMessages, &NewMsg->Link);

  ON_EXIT:
  FNC_LEAVE_MSG ();
  return 0;
}


/**
 A callback function invoked when a USB transfer has completed

 @param  urb              A USB URB whose transfer has been completed

 **/
STATIC
void
xfer_cb (
  struct qhsusb_urb *urb
  )
{
  struct qhsusb_dcd_dsc_device *dev = NULL;
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT (urb);

  DBG (EFI_D_INFO, "ep x%02x", urb->endpoint_address);

  dev = urb->usb_device;

  // TODO: we can eliminate having to store this info and derive directly at
  // time of EventHandler(). Maybe just a BOOLEAN that an URB was completed.

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&dev->UsbfnMessagesMemPool);
  USB_ASSERT(ListEntry);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = urb->endpoint_address & USB_ENDPOINT_DIR_IN
                                         ?  EfiUsbMsgEndpointStatusChangedTx
                                         :  EfiUsbMsgEndpointStatusChangedRx;
  NewMsg->Data.XferCbResult.BytesTransferred = urb->actual_length;
  NewMsg->Data.XferCbResult.TransferStatus = (EFI_USBFN_TRANSFER_STATUS) urb->transfer_status;
  NewMsg->Data.XferCbResult.EndpointIndex = urb->endpoint_address & ~USB_ENDPOINT_DIR_BMSK;
  NewMsg->Data.XferCbResult.EndpointDirection = urb->endpoint_address & USB_ENDPOINT_DIR_IN
                                         ?  EfiUsbEndpointDirectionHostIn
                                         :  EfiUsbEndpointDirectionHostOut;

  //
  // Work around the current issue in FFU. Every TX transfer from FFU module is not
  // create by UsbfnAllocateTransferBuffer. Thus, in UsbfnTransfer method, if the buffer
  // passed in is not 4k aligned, the content in the client supplied buffer is copied
  // to the buffer created locally. And that buffer is queued to
  // the hardware. However, we need to point Data.XferCbResult.Buffer to the original
  // buffer (pointed by transfer_buffer_ptr_tx); otherwise, a warning message will
  // be printed. Any message printed through UART will greatly impact the USB TP.
  //
  if(EfiUsbEndpointDirectionHostIn == NewMsg->Data.XferCbResult.EndpointDirection &&
     NULL != urb->transfer_buffer_ptr_tx) {
    NewMsg->Data.XferCbResult.Buffer = urb->transfer_buffer_ptr_tx;
  } else {
    NewMsg->Data.XferCbResult.Buffer = (VOID *) (UINTN) urb->transfer_buffer_ptr;
  }


  InsertTailList(&dev->UsbfnMessages, &NewMsg->Link);

  // The client has the data now.
  urb->transfer_status = QHSUSB_URB_STATUS_REUSABLE;

  FNC_LEAVE_MSG ();
}


/**
 A callback function invoked when a USB is attached/detached

 @param  dev                A USB device
 @param  is_cable_attached  Whether the USB cable is attached/detached

 **/
STATIC
void
xtach_cb (
  struct qhsusb_dcd_dsc_device  *dev,
  boolean                       is_cable_attached
  )
{
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT (dev);


  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&dev->UsbfnMessagesMemPool);
  USB_ASSERT(ListEntry);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = is_cable_attached
                  ? EfiUsbMsgBusEventAttach
                  : EfiUsbMsgBusEventDetach;
  InsertTailList(&dev->UsbfnMessages, &NewMsg->Link);
  FNC_LEAVE_MSG ();
}


/**
 A callback function invoked when a USB reset is detected

 @param  dev                A USB device

 **/
STATIC
void
reset_cb (
  struct qhsusb_dcd_dsc_device *dev
  )
{
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT (dev);


  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&dev->UsbfnMessagesMemPool);
  USB_ASSERT(ListEntry);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = EfiUsbMsgBusEventReset;
  InsertTailList(&dev->UsbfnMessages, &NewMsg->Link);

  FNC_LEAVE_MSG ();
}


/**
 A callback function invoked when a USB suspend is detected

 @param  dev                A USB device

 **/
STATIC
void
suspend_cb (
  struct qhsusb_dcd_dsc_device *dev
  )
{
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT (dev);

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&dev->UsbfnMessagesMemPool);
  USB_ASSERT(ListEntry);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = EfiUsbMsgBusEventSuspend;
  InsertTailList(&dev->UsbfnMessages, &NewMsg->Link);

  FNC_LEAVE_MSG ();
}


/**
 A callback function invoked when a USB resume is detected

 @param  dev                A USB device

 **/
STATIC
void
resume_cb (
  struct qhsusb_dcd_dsc_device *dev
  )
{
  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT (dev);

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&dev->UsbfnMessagesMemPool);
  USB_ASSERT(ListEntry);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);

  NewMsg->Message = EfiUsbMsgBusEventResume;
  InsertTailList(&dev->UsbfnMessages, &NewMsg->Link);

  FNC_LEAVE_MSG ();
}


/**
 A callback function invoked when a USB operation speed is detected

 @param  dev                A USB device
 @param  speed              USB operation speed

 **/
void
speed_cb (
  struct qhsusb_dcd_dsc_device *dev,
  int speed
  )
{

  EFI_USBFN_MESSAGE_NODE* NewMsg    = NULL;
  LIST_ENTRY*             ListEntry = NULL;

  FNC_ENTER_MSG ();

  USB_ASSERT (dev);

  //
  // Get a free Message from the memory pool
  //
  ListEntry = GetFirstNode(&dev->UsbfnMessagesMemPool);
  USB_ASSERT(ListEntry);
  RemoveEntryList(ListEntry);
  NewMsg = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);


  NewMsg->Message = EfiUsbMsgBusEventSpeed;
  NewMsg->Data.SpeedCbSpeed = (EFI_USB_BUS_SPEED) speed;
  InsertTailList(&dev->UsbfnMessages, &NewMsg->Link);

  FNC_LEAVE_MSG ();
}

//
// Implementation: More Miscellaneous
//
STATIC
USBFN_DEV
*CreateUsbfnInstance (
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV                           *Usbfn                  = NULL;
  UINTN                                i                      = 0;
  EFI_PLATFORMINFO_PROTOCOL           *PlatformInfoProtocol   = NULL;
  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE  PlatformInfo;
  CONST UINTN                          TX_BUFF_SIZE           = 65536;

  FNC_ENTER_MSG ();

  Usbfn = (USBFN_DEV *) AllocateZeroPool(sizeof(USBFN_DEV));
  if (!Usbfn) {
    DBG (EFI_D_ERROR, "memory allocation failed for USB FN");
    goto ON_EXIT;
  }

  Usbfn->Signature = USBFN_DEV_SIGNATURE;

  // USBFN IO protocol.
  Usbfn->UsbfnIo.Revision                   = EFI_USBFN_IO_PROTOCOL_REVISION;
  Usbfn->UsbfnIo.DetectPort                 = UsbfnDetectPort;
  Usbfn->UsbfnIo.ConfigureEnableEndpoints   = UsbfnConfigureEnableEndpoints;
  Usbfn->UsbfnIo.ConfigureEnableEndpointsEx = UsbfnConfigureEnableEndpointsEx;
  Usbfn->UsbfnIo.GetEndpointMaxPacketSize   = UsbfnGetEndpointMaxPacketSize;
  Usbfn->UsbfnIo.GetDeviceInfo              = UsbfnGetDeviceInfo;
  Usbfn->UsbfnIo.GetVendorIdProductId       = UsbfnGetVendorIdProductId;
  Usbfn->UsbfnIo.AbortTransfer              = UsbfnAbortTransfer;
  Usbfn->UsbfnIo.GetEndpointStallState      = UsbfnGetEndpointStallState;
  Usbfn->UsbfnIo.SetEndpointStallState      = UsbfnSetEndpointStallState;
  Usbfn->UsbfnIo.EventHandler               = UsbfnEventHandler;
  Usbfn->UsbfnIo.Transfer                   = UsbfnTransfer;
  Usbfn->UsbfnIo.GetMaxTransferSize         = UsbfnGetMaxTransferSize;
  Usbfn->UsbfnIo.AllocateTransferBuffer     = UsbfnAllocateTransferBuffer;
  Usbfn->UsbfnIo.FreeTransferBuffer         = UsbfnFreeTransferBuffer;
  Usbfn->UsbfnIo.StartController            = UsbfnStartController;
  Usbfn->UsbfnIo.StopController             = UsbfnStopController;
  Usbfn->UsbfnIo.SetEndpointPolicy          = UsbfnSetEndpointPolicy;
  Usbfn->UsbfnIo.GetEndpointPolicy          = UsbfnGetEndpointPolicy;

  //
  // Set Default Policy
  //
  Usbfn->PipePolicy.ZeroLengthTerminationSupport = TRUE;

  // If the max transfer size is greater than 16 KB, ZLT must be disabled
  for (i = 0; i < DCD_MAX_EP; ++i){
    Usbfn->SWDevice.zlt[i][EfiUsbEndpointDirectionHostIn]  = FALSE;
    Usbfn->SWDevice.zlt[i][EfiUsbEndpointDirectionHostOut] = FALSE;
    Usbfn->SWDevice.max_transfer_size[i][EfiUsbEndpointDirectionHostIn]  = DEFAULT_URB_MAX_BUFFER_SIZE;
    Usbfn->SWDevice.max_transfer_size[i][EfiUsbEndpointDirectionHostOut] = DEFAULT_URB_MAX_BUFFER_SIZE;
  }

  Usbfn->SWDevice.descriptor = NULL;
  Usbfn->SWDevice.configurations = NULL;

  Usbfn->SWDevice.init = FALSE;
  Usbfn->SWDevice.core_id = 0;
  Usbfn->SWDevice.notify = NULL;
  Usbfn->SWDevice.setup = setup_dev_cb;
  Usbfn->SWDevice.setup_ifc = setup_ifc_cb;
  Usbfn->SWDevice.setup_ep = setup_ep_cb;
  Usbfn->SWDevice.is_attached = 0;
  Usbfn->SWDevice.speed = QHSUSB_DCD_SPEED_UNKNOWN;
  Usbfn->SWDevice.max_current = 0;
  Usbfn->SWDevice.is_suspended = FALSE;
  Usbfn->SWDevice.address = 1; // nonzero
  Usbfn->SWDevice.config_value = 0;
  Usbfn->SWDevice.active_configuration = NULL;

  // URBs.
  for (i = 0; i < MAX_URBS; ++i) {
    // Some of these fields never change.
    Usbfn->SWDevice.Urb[i].signature = QHSUSB_URB_SIGNATURE;
    Usbfn->SWDevice.Urb[i].transfer_status = QHSUSB_URB_STATUS_REUSABLE;
    Usbfn->SWDevice.Urb[i].usb_device = &Usbfn->SWDevice;
    Usbfn->SWDevice.Urb[i].core_id = 0;
    Usbfn->SWDevice.Urb[i].endpoint_address = 0x00;
    Usbfn->SWDevice.Urb[i].transfer_buffer_ptr = 0;
    Usbfn->SWDevice.Urb[i].transfer_length = 0;
    Usbfn->SWDevice.Urb[i].actual_length = 0;
    Usbfn->SWDevice.Urb[i].complete_callback = xfer_cb;
  }

  InitializeListHead(&Usbfn->SWDevice.UsbfnMessages);

  InitializeListHead(&Usbfn->SWDevice.UsbfnMessagesMemPool);
  Usbfn->SWDevice.UsbfnMessageMemPoolBuffer = (EFI_USBFN_MESSAGE_NODE *) AllocateZeroPool(sizeof(EFI_USBFN_MESSAGE_NODE) * MAX_USBFN_MESSAGE_SIZE);
  for (i = 0; i < MAX_USBFN_MESSAGE_SIZE; ++i){
    EFI_USBFN_MESSAGE_NODE* NewMsg = &(Usbfn->SWDevice.UsbfnMessageMemPoolBuffer)[i];
    InsertTailList(&Usbfn->SWDevice.UsbfnMessagesMemPool, &NewMsg->Link);
  }


  Usbfn->SWDevice.reset_cb = reset_cb;
  Usbfn->SWDevice.xtach_cb = xtach_cb;
  Usbfn->SWDevice.suspend_cb = suspend_cb;
  Usbfn->SWDevice.resume_cb = resume_cb;
  Usbfn->SWDevice.speed_cb = speed_cb;
  Usbfn->SWDevice.usbfn = Usbfn;

  Status = gBS->LocateProtocol(&gEfiPlatformInfoProtocolGuid, NULL,
      (VOID **) &PlatformInfoProtocol);
  ERR_CHK ("Failed to get gEfiPlatformInfoProtocolGuid");

  Status = PlatformInfoProtocol->GetPlatformInfo(PlatformInfoProtocol,
      &PlatformInfo);
  ERR_CHK ("Failed to get PlatformInfo");

  Usbfn->UsbConfigProtocol = NULL;
  Usbfn->PlatType = PlatformInfo;

  Usbfn->TxBuffer = UncachedAllocateAlignedPool(TX_BUFF_SIZE,USBFN_DEFAULT_BUFFER_ALIGN);
  if(NULL == Usbfn->TxBuffer ||
     IS_NOT_32_BIT_ADDR(Usbfn->TxBuffer) ||
     !IS_ALIGNED(Usbfn->TxBuffer, USBFN_DEFAULT_BUFFER_ALIGN) ) {
    Status = EFI_DEVICE_ERROR;
    USB_ASSERT_GOTO(FALSE, ON_EXIT);
  }

  DBG(EFI_D_POOL, "TX BUFFER FOR FFU WORKAROUND = 0x%x", Usbfn->TxBuffer);

  if(NULL == Usbfn->TxBuffer){
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  Status = EFI_SUCCESS;

ON_EXIT:
  if (EFI_ERROR (Status)) {
    Usbfn = NULL;
  }

  FNC_LEAVE_MSG ();
  return Usbfn;
}

//
// Implementation: Protocol
//

// TODO: test charging on each platform.
EFI_STATUS
EFIAPI
UsbfnDetectPort (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT EFI_USBFN_PORT_TYPE    *PortType
  )
{
  EFI_STATUS Status              = EFI_SUCCESS;
  USBFN_DEV *Usbfn               = NULL;
  qhsusb_chg_port_type QPortType = QHSUSB_CHG_PORT_UNKNOWN;

  FNC_ENTER_MSG ();

  ARG_CHK (This && PortType);

  Usbfn = USBFN_FROM_THIS (This);

  // Get port type.
  QPortType = qhsusb_dci_get_charger_type(Usbfn->SWDevice.core_id);

  // Convert from AMSS QHSUSB DL to USBFN.
  switch (QPortType) {
  case QHSUSB_CHG_PORT_SDP:
    *PortType = EfiUsbStandardDownstreamPort;
    break;
  case QHSUSB_CHG_PORT_CDP:
    *PortType = EfiUsbChargingDownstreamPort;
    break;
  case QHSUSB_CHG_PORT_DCP:
    *PortType = EfiUsbDedicatedChargingPort;
    break;
  case QHSUSB_CHG_PORT_ACA_DOCK:
  case QHSUSB_CHG_PORT_ACA_A:
  case QHSUSB_CHG_PORT_ACA_B:
  case QHSUSB_CHG_PORT_ACA_C:
  case QHSUSB_CHG_PORT_INVALID:
    *PortType = EfiUsbInvalidDedicatedChargingPort;
    break;
  case QHSUSB_CHG_PORT_UNKNOWN:
  default:    
    *PortType = EfiUsbUnknownPort;
    break;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

STATIC
EFI_STATUS
HandleSetZLTSupport (
  IN EFI_USBFN_IO_PROTOCOL*        This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN BOOLEAN                       Enable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();

  ARG_CHK (This);

  Usbfn = USBFN_FROM_THIS (This);

  if(EndpointIndex >= DCD_MAX_EP || (UINT8) Direction >= DCD_MAX_DIR){
    return EFI_INVALID_PARAMETER;
  }

  // Turn on ZLT on RX endpoint is not supported
  if(EfiUsbEndpointDirectionDeviceRx == Direction){
    DBG(EFI_D_ERROR, "Enabling ZLT on a RX endpoint is not supported");
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  Usbfn->SWDevice.zlt[EndpointIndex][Direction] = Enable;

  ON_EXIT:
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnConfigureEnableEndpoints (
  IN EFI_USBFN_IO_PROTOCOL  *This,
  IN EFI_USB_DEVICE_INFO    *DeviceInfo
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;
  UINT32 MaxTD = 0;
  UINT32 i,j;

  FNC_ENTER_MSG ();

  ARG_CHK (This && DeviceInfo);

  Usbfn = USBFN_FROM_THIS (This);

  if (Usbfn->SWDevice.descriptor) {
    // We've already been configured.
    Status = FreeDevice(&Usbfn->SWDevice);
    WRN_CHK ("failed to free device");

    Usbfn->SWDevice.descriptor = NULL;
    Usbfn->SWDevice.configurations = NULL;
  }

  // Store all descriptor information in AMSS QHSUSB DL format.
  Status = CopyDevice(&Usbfn->SWDevice, DeviceInfo);
  ERR_CHK ("failed to copy device info");

  // Set transfer_length_dTD in each URB.
  for(i = 0; i < DCD_MAX_EP; ++i){
    for(j = 0; j < DCD_MAX_DIR; ++j){
      UINT32 Tds = UFN_MAX_SIZE_TO_TDS(Usbfn->SWDevice.max_transfer_size[i][j]);
      if(MaxTD < Tds){
        MaxTD = Tds;
      }
    }
  }

  for(i = 0; i < MAX_URBS; ++i){
    Usbfn->SWDevice.Urb[i].transfer_length_dTD = (uint32 *) AllocateZeroPool( (MaxTD+1) * sizeof(uint32) );
    if(NULL == Usbfn->SWDevice.Urb[i].transfer_length_dTD){
      Status = EFI_OUT_OF_RESOURCES;
      ERR_CHK ("transfer_length_dTD allocation fails");
    }
  }

  // Set enum flag for RS bit to be turned OFF in qhsusb_dci_init();
  qhsusb_dci_set_enum_flag(FALSE);
  qhsusb_dcd_init(&Usbfn->SWDevice);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnConfigureEnableEndpointsEx(
    IN EFI_USBFN_IO_PROTOCOL           *This,
    IN EFI_USB_DEVICE_INFO             *DeviceInfo,
    IN EFI_USB_SUPERSPEED_DEVICE_INFO  *SSDeviceInfo
)
{
    return UsbfnConfigureEnableEndpoints(This, DeviceInfo);
}

EFI_STATUS
EFIAPI
UsbfnGetEndpointMaxPacketSize (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  IN  EFI_USB_ENDPOINT_TYPE  EndpointType,
  IN  EFI_USB_BUS_SPEED      BusSpeed,
  OUT UINT16                 *MaxPacketSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if(!This || !MaxPacketSize){
    DBG (EFI_D_ERROR, "Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  // TODO: use some max packet macros.
  // Default to maximum speed.
  switch (EndpointType) {
  case UsbEndpointControl:
    *MaxPacketSize = BusSpeed == UsbBusSpeedLow
                   ? 8
                   : 64;
    break;
  case UsbEndpointIsochronous:
    *MaxPacketSize = BusSpeed == UsbBusSpeedFull
                   ? 1023
                   : 1024;
    break;
  case UsbEndpointBulk:
    *MaxPacketSize = BusSpeed == UsbBusSpeedFull
                   ? 64
                   : 512;
    break;
  case UsbEndpointInterrupt:
    *MaxPacketSize = BusSpeed == UsbBusSpeedLow
                   ? 8
                   : BusSpeed == UsbBusSpeedFull
                   ? 64
                   : 1024;
    break;
  default:
    ARG_CHK (FALSE);
    break;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


#define EMMC_UUID_CANONICAL_FORM_SIZE_IN_BYTES 72 //  32 alphanumeric characters and four hyphens in WCHAR
#define EMMC_UUID_OCTET_SIZE                   16 //  UUID is a 16-octet number
/**
A UUID is a 16-octet (128-bit) number.
In its canonical form, a UUID is represented by 32 hexadecimal digits,
displayed in five groups separated by hyphens, in the form 8-4-4-4-12 for a
total of 36 characters (32 alphanumeric characters and four hyphens).
For example:
550e8400-e29b-41d4-a716-446655440000
**/
STATIC
VOID
GetUUIDCanonicalForm(
  CHAR16  *Dest,
  UINT8   *Src)
{
  CHAR16 hexArray[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  CHAR16 UUIDHexString[32] = {0};
  CHAR16 *pUUIDHexString = &UUIDHexString[0];
  UINT8  numDigPerGroup[5] = {8,4,4,4,12};
  UINT8  i;
  UINT8  v;
  UINT8  numGroup = 5;

  for ( i = 0; i < EMMC_UUID_OCTET_SIZE; i++ ) {
    v = Src[i] & 0xFF;
    UUIDHexString[i * 2]     = hexArray[v >> 4];
    UUIDHexString[i * 2 + 1] = hexArray[v & 0x0F];
  }

  for( i = 0; i < numGroup; i++){
    CopyMem(Dest, pUUIDHexString, numDigPerGroup[i] * sizeof(CHAR16));
    Dest += numDigPerGroup[i];
    if (i < numGroup - 1) {
      *Dest = '-';
      Dest++;
    }
    pUUIDHexString += numDigPerGroup[i];
  }
}


/**
  Get the USB serial number.

  The serial number is based on the unique serial number from the non-removable emmc.

  @param  BufferSize             On input, the size of the user buffer.
                                 On output, the size of the serial number in bytes
  @param  Buffer                 The buffer provided by the user

  @retval EFI_SUCCESS            USB serial number is successfully obtained.
  @retval other                  Operation failed

**/
STATIC
EFI_STATUS
GetUsbSerialNumber (
  IN OUT UINTN                     *BufferSize,
  OUT    VOID                      *Buffer)
{

  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_BLOCK_IO_PROTOCOL       *BlkIo;
  EFI_HANDLE                  *SdccHandles;
  UINTN                       NumberOfSdccHandles;
  UINTN                       i, j;
  EFI_MEM_CARDINFO_PROTOCOL  *CardInfo;
  MEM_CARD_INFO              CardInfoData;
  UINT8                       UUID[EMMC_UUID_OCTET_SIZE];

  if (!BufferSize || !Buffer) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  if(*BufferSize < EMMC_UUID_CANONICAL_FORM_SIZE_IN_BYTES){
    Status = EFI_BUFFER_TOO_SMALL;
    *BufferSize = EMMC_UUID_CANONICAL_FORM_SIZE_IN_BYTES;
    DEBUG((EFI_D_ERROR,"Buffer size is too small"));
    goto ON_EXIT;
  }

  Status = gBS->LocateHandleBuffer (ByProtocol,
                                    &gEfiBlockIoProtocolGuid,
                                    NULL,
                                    &NumberOfSdccHandles,
                                    &SdccHandles);
  if(Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR,"Failed to get BlkIo handles"));
    return Status;
  }

  /* Loop through to search for the ones we are interested in,
   * Non removable media. */
  for (i = 0; i < NumberOfSdccHandles; i++){

    Status = gBS->HandleProtocol (SdccHandles[i],
                                  &gEfiBlockIoProtocolGuid,
                                  (VOID **) &BlkIo);
    if (EFI_ERROR (Status))
      continue;

    if (BlkIo->Media->RemovableMedia)
      continue;

    Status = gBS->HandleProtocol (SdccHandles[i],
                                  &gEfiMemCardInfoProtocolGuid,
                                  (VOID**)&CardInfo);

    if(Status != EFI_SUCCESS)
      continue;

    if (CardInfo->GetCardInfo (CardInfo, &CardInfoData) == EFI_SUCCESS){
      // copy the data to UUID
      CopyMem(UUID, &CardInfoData, EMMC_UUID_OCTET_SIZE);
      *BufferSize = EMMC_UUID_CANONICAL_FORM_SIZE_IN_BYTES;
      GetUUIDCanonicalForm(Buffer, UUID);
      for(j = 0; j < EMMC_UUID_CANONICAL_FORM_SIZE_IN_BYTES / 2; j++){
        DBG(EFI_D_INFO,"Buffer[%d] = 0x%x",j, ((CHAR16*)Buffer)[j]);
      }
      break;
    }
  }


  ON_EXIT:
  return Status;

}


/**
  Get the USB Manufacturer Name.

  @param  BufferSize             On input, the size of the user buffer.
                                 On output, the size of the manufacturer name in bytes
  @param  Buffer                 The buffer provided by the user

  @retval EFI_SUCCESS            USB Manufacturer Name is successfully obtained.
  @retval other                  Operation failed

**/
STATIC
EFI_STATUS
GetUsbManufacturerName (
  IN OUT UINTN                     *BufferSize,
  OUT    VOID                      *Buffer)
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if (!BufferSize || !Buffer) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  if(*BufferSize < sizeof(str_manuf)){
    Status = EFI_BUFFER_TOO_SMALL;
    *BufferSize = sizeof(str_manuf);
    DEBUG((EFI_D_ERROR,"Buffer size is too small"));
    goto ON_EXIT;
  }

  // Copy the string.
  CopyMem(Buffer, str_manuf, sizeof(str_manuf));

  ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Get the USB Product String

  @param  BufferSize             On input, the size of the user buffer.
                                 On output, the size of the product string in bytes
  @param  Buffer                 The buffer provided by the user

  @retval EFI_SUCCESS            USB Product String is successfully obtained.
  @retval other                  Operation failed

**/
STATIC
EFI_STATUS
GetUsbProductString (
  IN OUT UINTN                     *BufferSize,
  OUT    VOID                      *Buffer)
{

  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if (!BufferSize || !Buffer) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  if(*BufferSize < sizeof(str_product)){
    Status = EFI_BUFFER_TOO_SMALL;
    *BufferSize = sizeof(str_product);
    DEBUG((EFI_D_ERROR,"Buffer size is too small"));
    goto ON_EXIT;
  }

  // Copy the string.
  CopyMem(Buffer, str_product, sizeof(str_product));

  ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}
EFI_STATUS
EFIAPI
UsbfnGetDeviceInfo (
  IN     EFI_USBFN_IO_PROTOCOL     *This,
  IN     EFI_USBFN_DEVICE_INFO_ID  Id,
  IN OUT UINTN                     *BufferSize,
  OUT    VOID                      *Buffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if (!This || !BufferSize || !Buffer) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // Decode request into source string and source string size.
  switch (Id) {
  case EfiUsbDeviceInfoSerialNumber:
    Status = GetUsbSerialNumber(BufferSize, Buffer);
    break;
  case EfiUsbDeviceInfoManufacturerName:
    Status = GetUsbManufacturerName(BufferSize, Buffer);
    break;
  case EfiUsbDeviceInfoProductName:
    Status = GetUsbProductString(BufferSize, Buffer);
    break;
  case EfiUsbDeviceInfoUnknown:
  default:
    ARG_CHK (FALSE);
    break;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnGetVendorIdProductId (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT UINT16                 *Vid,
  OUT UINT16                 *Pid
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();

  if (!This || !Vid || !Pid) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  if(!Usbfn) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  if (!Usbfn->SWDevice.descriptor) {
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  *Vid = Usbfn->SWDevice.descriptor->idVendor;
  *Pid = Usbfn->SWDevice.descriptor->idProduct;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnAbortTransfer (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
    USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();

  // TODO: the protocol spec says we have to return invalid
  // parameter if the endpoint isn't in the configuration. But what if we want
  // to abort a transfer when the device isn't attached or is otherwise
  // unconfigured?

  if (!This ||
      ((Direction != EfiUsbEndpointDirectionDeviceTx) && (Direction != EfiUsbEndpointDirectionDeviceRx)) ||
      EndpointIndex > QHUSB_MAX_ENDPOINTS) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  if(!Usbfn){
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  // Abort transfer.
  (VOID) qhsusb_dci_cancel_transfer(Usbfn->SWDevice.core_id, EndpointIndex, Direction);

ON_EXIT:
    FNC_LEAVE_MSG ();
    return Status;
}

EFI_STATUS
EFIAPI
UsbfnGetEndpointStallState (
  IN  EFI_USBFN_IO_PROTOCOL         *This,
  IN  UINT8                         EndpointIndex,
  IN  EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  OUT BOOLEAN                       *State
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;
  struct qhsusb_dcd_dsc_endpoint *endpoint = NULL;

  FNC_ENTER_MSG ();

  if (!This || !State) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  // Validate endpoint.
  if (EndpointIndex) {
    Status = FindEndpoint(Usbfn, EndpointIndex, Direction, &endpoint);
    ERR_CHK (
        "unable to find endpoint index %01x, direction %01x",
        EndpointIndex,
        Direction
    );
  }

  *State = qhsusb_dci_get_endpoint_stall_state(
      Usbfn->SWDevice.core_id,
      EndpointIndex,
      Direction
      );

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnSetEndpointStallState (
  IN EFI_USBFN_IO_PROTOCOL         *This,
  IN UINT8                         EndpointIndex,
  IN EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN BOOLEAN                       State
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;
  struct qhsusb_dcd_dsc_endpoint *endpoint = NULL;

  FNC_ENTER_MSG ();

  ARG_CHK (This);

  Usbfn = USBFN_FROM_THIS (This);

  // Validate endpoint.
  if (EndpointIndex) {
    Status = FindEndpoint(Usbfn, EndpointIndex, Direction, &endpoint);
    ERR_CHK (
        "unable to find endpoint index %01x, direction %01x",
        EndpointIndex,
        Direction
    );
  }

  if (State) {
    (VOID) qhsusb_dci_stall_endpoint(
             Usbfn->SWDevice.core_id,
             EndpointIndex,
             Direction
             );
  } else {
    (VOID) qhsusb_dci_unstall_endpoint(
             Usbfn->SWDevice.core_id,
             EndpointIndex,
             Direction
             );
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnEventHandler (
  IN      EFI_USBFN_IO_PROTOCOL      *This,
  OUT     EFI_USBFN_MESSAGE          *Message,
  IN OUT  UINTN                      *PayloadSize,
  OUT     EFI_USBFN_MESSAGE_PAYLOAD  *Payload
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;
  VOID *Src = NULL;
  UINTN SrcSize = 0;
  EFI_USBFN_MESSAGE NoneMsg = EfiUsbMsgNone;
  EFI_USBFN_MESSAGE *Msg = NULL;
  BOOLEAN            HasNoMsg   =  TRUE;
  LIST_ENTRY         *ListEntry =  NULL;
  EFI_USBFN_MESSAGE_NODE *CurrentMsgNode = NULL;

  if (!This || !Message || !PayloadSize || !Payload) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  HasNoMsg = IsListEmpty(&Usbfn->SWDevice.UsbfnMessages);

  if (HasNoMsg) {
    Msg      =  &NoneMsg;
  } else {
    ListEntry = GetFirstNode(&Usbfn->SWDevice.UsbfnMessages);
    USB_ASSERT_RETURN(ListEntry, EFI_DEVICE_ERROR);
    CurrentMsgNode = BASE_CR(ListEntry, EFI_USBFN_MESSAGE_NODE, Link);
    Msg = &CurrentMsgNode->Message;

    // Check for a message payload, if any.
    switch (CurrentMsgNode->Message) {

    case EfiUsbMsgEndpointStatusChangedRx:
    case EfiUsbMsgEndpointStatusChangedTx:
      Src      =  &(CurrentMsgNode->Data.XferCbResult);
      SrcSize  =  sizeof (CurrentMsgNode->Data.XferCbResult);
      break;

    case EfiUsbMsgSetupPacket:
      Src      =  &(CurrentMsgNode->Data.SetupCbRequest);
      SrcSize  =  sizeof (CurrentMsgNode->Data.SetupCbRequest);
      break;

    case EfiUsbMsgBusEventSpeed:
      Src      =  &(CurrentMsgNode->Data.SpeedCbSpeed);
      SrcSize  =  sizeof (CurrentMsgNode->Data.SpeedCbSpeed);

    default :
      ;
    }
  }

  if (!Msg || (int)(*Msg) < EfiUsbMsgNone || (int)(*Msg) > EfiUsbMsgBusEventSpeed) {
    if (Msg) {
      DBG(EFI_D_ERROR, "ERROR: *Msg = %d", *Msg);
    } else {
      DBG(EFI_D_ERROR, "ERROR: Msg should not be NULL");
    }
    Msg = &NoneMsg;
    Status = EFI_DEVICE_ERROR;
  }

  if (*Msg > EfiUsbMsgNone && *Msg <= EfiUsbMsgBusEventSpeed) {
    DBG (EFI_D_INFO, "%a", MsgToStr[*Msg]);
  }

  if (*Msg == EfiUsbMsgNone) {
    // No messages in queue. It's safe to poll again.
    qhsusb_dcd_poll(&Usbfn->SWDevice);
  } else {
    // if there is payload, copy it to the client buffer
    if (Src && SrcSize > 0) {
      if (*PayloadSize < SrcSize) {
        Status = EFI_BUFFER_TOO_SMALL;
      } else {
        CopyMem(Payload, Src, SrcSize);
      }
    }
  }

  // Always update client.
  *Message = *Msg;
  if (SrcSize) {
    *PayloadSize = sizeof(*Payload);
  } else {
    *PayloadSize = 0;
  }

  // Update internal state.
  *Msg = EfiUsbMsgNone;

ON_EXIT:
  if(ListEntry) {
    //
    // Remove the message from the active
    // message list and put it back to the
    // memory pool
    //
    RemoveEntryList(ListEntry);
    InsertTailList(&Usbfn->SWDevice.UsbfnMessagesMemPool, ListEntry);
  }
  return Status;
}


EFI_STATUS
EFIAPI
UsbfnTransfer (
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN OUT  UINTN                         *BufferSize,
  IN OUT  VOID                          *Buffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USBFN_DEV *Usbfn = NULL;

  FNC_ENTER_MSG ();

  if (!This || !BufferSize || !Buffer) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Usbfn = USBFN_FROM_THIS (This);

  if(!Usbfn) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  Status = usbfn_transfer ( Usbfn, EndpointIndex, Direction, BufferSize, Buffer);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnGetMaxTransferSize (
  IN  EFI_USBFN_IO_PROTOCOL  *This,
  OUT UINTN                  *MaxTransferSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG ();

  if (!This || !MaxTransferSize) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  *MaxTransferSize = DTD_MAX_TRANSFER_16KB;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


EFI_STATUS
EFIAPI
UsbfnAllocateTransferBuffer (
  IN   EFI_USBFN_IO_PROTOCOL  *This,
  IN   UINTN                  Size,
  OUT  VOID                   **Buffer
  )
{

  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG ();

  if(!Buffer || !This || Size > DEFAULT_URB_MAX_BUFFER_SIZE || 0 == Size){
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "Invalid Parameters");
    goto ON_EXIT;
  }

  *Buffer = UncachedAllocateAlignedPool(Size,USBFN_DEFAULT_BUFFER_ALIGN);

  if(NULL == *Buffer){
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  if(IS_NOT_32_BIT_ADDR(*Buffer) || 
     !IS_ALIGNED(*Buffer, USBFN_DEFAULT_BUFFER_ALIGN)){
    USB_ASSERT_RETURN(FALSE, EFI_DEVICE_ERROR);
  }

  DBG(EFI_D_POOL, "Transfer BUFFER IS 0x%x", *Buffer);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnFreeTransferBuffer (
  IN EFI_USBFN_IO_PROTOCOL  *This,
  IN VOID                   *Buffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG ();

  ARG_CHK (Buffer);

  //
  // TODO: Due to the memory allocator issue, don't free it until the fix is checked in
  //
  UncachedFreeAlignedPool(Buffer);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnStartController (
  IN EFI_USBFN_IO_PROTOCOL  *This
  )
{
  FNC_ENTER_MSG ();
  FNC_LEAVE_MSG ();
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UsbfnStopController (
  IN EFI_USBFN_IO_PROTOCOL  *This
    )
{
  EFI_STATUS    Status      = EFI_SUCCESS;
  USBFN_DEV    *Usbfn       = NULL;
  LIST_ENTRY   *ListEntry   = NULL;
  FNC_ENTER_MSG ();

  ARG_CHK (This);

  Usbfn = USBFN_FROM_THIS (This);


  if(!Usbfn->SWDevice.init){
    DBG(EFI_D_INFO, "The controller is already stopped");
    return EFI_SUCCESS;
  }

  // clean up the dci layer
  Status = qhsusb_dci_shutdown (Usbfn->SWDevice.core_id);
  if(EFI_ERROR(Status)) {
    DBG (EFI_D_ERROR, "qhsusb_dci_shutdown failed: %r");
  }

  Usbfn->SWDevice.init         = FALSE;
  Usbfn->SWDevice.config_value = 0;
  Usbfn->SWDevice.is_attached  = 0;
  Usbfn->SWDevice.speed        = QHSUSB_DCD_SPEED_UNKNOWN;
  Usbfn->SWDevice.address      = 0;

  // disconnect the device from the bus
  qhsusb_dci_disconnect(Usbfn->SWDevice.core_id);

  if (Usbfn->SWDevice.descriptor) {
    FreeDevice(&Usbfn->SWDevice);
  }

  // remove every message from UsbfnMessages
  // and put it back to UsbfnMessagesMemPool 
  while (!IsListEmpty(&Usbfn->SWDevice.UsbfnMessages)) {
     ListEntry = GetFirstNode(&Usbfn->SWDevice.UsbfnMessages);
     RemoveEntryList(ListEntry);
     InsertTailList(&Usbfn->SWDevice.UsbfnMessagesMemPool, ListEntry);
  }
 
ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}



EFI_STATUS
UsbfnUnload (
  IN EFI_HANDLE ImageHandle
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG ();

  UsbfnExitBootService(0, Usbfn);

  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Entry point for EFI drivers.

  @param  ImageHandle       EFI_HANDLE.
  @param  SystemTable       EFI_SYSTEM_TABLE.

  @return EFI_SUCCESS       Success.
          EFI_DEVICE_ERROR  Fail.

**/
EFI_STATUS
EFIAPI
UsbfnDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gUsbFnDriverBinding,
           ImageHandle,
           NULL,
           NULL
           );
}



/**
  Test to see if this driver supports ControllerHandle. Any
  ControllerHandle that has Usb2HcProtocol installed will
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
  )
{

  QCOM_USB_CONFIG_PROTOCOL *pUsbConfigProtocol;
  EFI_STATUS              Status;

  // Check if UsbConfig protocol is installed
  Status = gBS->OpenProtocol (
                  Controller,
                  &gQcomUsbConfigProtocolGuid,
                  (VOID **) &pUsbConfigProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) 
  {
    return Status;
  }

  //check if controller is for a valid core that supports device USB2.0 mode
  if ( (pUsbConfigProtocol->coreNum < USB_CORE_MAX_NUM) && (pUsbConfigProtocol->modeType == USB_DEVICE_MODE_HS) )
  {
    DEBUG(( EFI_D_INFO, "UsbFnDriverBindingSupported: This is core#%d, CoreType = (0x%x)\r\n", 
                pUsbConfigProtocol->coreNum, pUsbConfigProtocol->modeType ));
    Status = EFI_SUCCESS;
  }
  else
  {
    Status = EFI_UNSUPPORTED;
  }
   
  // Close the USB_CONFIG used to perform the supported test
  gBS->CloseProtocol (
         Controller,
         &gQcomUsbConfigProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}



EFI_STATUS
EFIAPI
UsbFnDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImageProtocol = NULL;

  FNC_ENTER_MSG ();

  Usbfn = CreateUsbfnInstance();
  if (!Usbfn) {
    DBG (EFI_D_ERROR, "unable to create USB FN");
    goto ON_EXIT;
  }
  
  //Open the Plat Config protocol.
  Status = gBS->OpenProtocol (
                  Controller,
                  &gQcomUsbConfigProtocolGuid,
                  (VOID **) &Usbfn->UsbConfigProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (Status == EFI_ALREADY_STARTED)
  {
   DEBUG(( EFI_D_ERROR,
            "UsbFnDriverBindingStart:  USB Config Protocol already started, Status =  (0x%x)\r\n", Status));
    return Status;
  }
  else if (EFI_ERROR (Status)) {
     DEBUG(( EFI_D_ERROR,
            "UsbFnDriverBindingStart: Unable to open USB Config Protocol, Status =  (0x%x)\r\n", Status));
    return Status;
  }

  DEBUG(( EFI_D_INFO, "UsbFnDriverBindingStart: This is core#%d, CoreType = (0x%x)\r\n", 
            Usbfn->UsbConfigProtocol->coreNum, Usbfn->UsbConfigProtocol->modeType ));

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&LoadedImageProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  WRN_CHK ("failed to open protocol:loaded image protocol");

  if (LoadedImageProtocol) {
    LoadedImageProtocol->Unload = UsbfnUnload;
  }
  
  //save core type
  Status = Usbfn->UsbConfigProtocol->GetUsbFnConfig(Usbfn->UsbConfigProtocol,
      &Usbfn->CoreType);
  ERR_CHK ("failed to get USB Fn configuration");

  //initialize USB core
  Status = Usbfn->UsbConfigProtocol->ConfigUsb(Usbfn->UsbConfigProtocol, USB_DEVICE_MODE_HS, Usbfn->UsbConfigProtocol->coreNum);
  ERR_CHK ("failed to perform platform configuration");

  // Install UsbfnIo protocol.
  Status = gBS->InstallMultipleProtocolInterfaces(&Controller,
      &gEfiUsbfnIoProtocolGuid, &Usbfn->UsbfnIo, NULL);
  ERR_CHK ("unable to install USBFN protocols");

  // Register for boot services exit event.
  Status = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
      UsbfnExitBootService, Usbfn, &gEfiEventExitBootServicesGuid,
      &Usbfn->ExitBootServiceEvent);

  if (EFI_ERROR (Status)) {
    DBG(EFI_D_ERROR,"UsbFnDriverBindingStart: ExitBootServiceEvent creation failed %r", Status);
    Usbfn->ExitBootServiceEvent = NULL;
  }

  // Register for LPM Start event
  Status = gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
      UsbfnEnterLPM, Usbfn, &gEfiEventEnterLPMGuid,
      &Usbfn->EnterLPMEvent);
 
  if (EFI_ERROR (Status)) {
    DBG(EFI_D_ERROR,"UsbFnDriverBindingStart: EnterLPMEvent creation failed %r", Status);
    Usbfn->EnterLPMEvent = NULL;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}



EFI_STATUS
EFIAPI
UsbFnDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_USBFN_IO_PROTOCOL *UsbfnIo;
  
  FNC_ENTER_MSG ();

  DEBUG(( EFI_D_ERROR, "UsbFnDriverBindingStop++\r\n"));
  
  //
  // Test whether the Controller handler passed in is a valid
  // Usb controller handle that should be supported, if not,
  // return the error status directly
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbfnIoProtocolGuid,
                  (VOID **) &UsbfnIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiUsbfnIoProtocolGuid,
                  UsbfnIo
                  );

  if (EFI_ERROR (Status)) {
    DEBUG(( EFI_D_ERROR,
            "UsbFnDriverBindingStop: failed to uninstall gEfiUsbfnIoProtocolGuid. Status =  (0x%x)\r\n", Status));
    return Status;
  }

  UsbfnExitBootService(0, Usbfn);
  
  //close UsbConfig handle on controller DriverBinding handle
  Status = gBS->CloseProtocol (
         Controller,
         &gQcomUsbConfigProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
         
  if (EFI_ERROR (Status)) {
      DEBUG(( EFI_D_ERROR,
          "UsbFnDriverBindingStop: failed to close gQcomUsbConfigProtocolGuid. Status =  (0x%x)\r\n", Status));
  }
  
 Status = gBS->CloseProtocol (
                  Controller,
                  &gEfiLoadedImageProtocolGuid,
                  This->DriverBindingHandle,
                  Controller
                  );
  WRN_CHK ("failed to close loaded image protocol");

  Usbfn = NULL;

  FNC_LEAVE_MSG ();
  return Status;
}


/**
 This is a driver exit point function that is called when
 gBS->ExitBootServices() is called.

 @param  Event                   Pointer to this event
 @param  Context                 Event hanlder private data

 **/
VOID
EFIAPI
UsbfnExitBootService (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  USBFN_DEV  *Usbfn  = NULL;

  FNC_ENTER_MSG ();

  ARG_CHK (Context);

  Usbfn = (USBFN_DEV *) Context;

  UsbfnStopController(&Usbfn->UsbfnIo);

 ON_EXIT:
  if (Usbfn) { 
    gBS->CloseEvent(Usbfn->ExitBootServiceEvent);

    //
    // free Message from the memory pool
    //
    FreePool(Usbfn->SWDevice.UsbfnMessageMemPoolBuffer);

    UncachedFreeAlignedPool(Usbfn->TxBuffer);
    FreePool(Usbfn);
  }
  FNC_LEAVE_MSG ();
}

EFI_STATUS
EFIAPI
UsbfnSetEndpointPolicy(
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         PolicyType,
  IN      UINTN                         BufferSize,
  IN      VOID                          *Buffer
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;  

  FNC_ENTER_MSG();

  if(!This || !Buffer ||
     EndpointIndex >= USB_ENDPOINT_NUMBER_MAX || 
     Direction > EfiUsbEndpointDirectionHostIn){
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters");    
  }

  DBG(EFI_D_INFO, "EP %d, Dir %d, PolicyType %d, BufferSize %d, Buffer %p",
    EndpointIndex,
    Direction,
    PolicyType,
    BufferSize,
    Buffer
    );

  switch (PolicyType){  
  case EfiUsbPolicyZeroLengthTermination:
    if (BufferSize > sizeof(BOOLEAN)){
      Status = EFI_INVALID_PARAMETER;
      ERR_CHK("Value is too large");
    }

    Status = HandleSetZLTSupport(This, EndpointIndex, Direction, *((BOOLEAN *)Buffer));
    ERR_CHK("EfiUsbZeroLengthTermination cannot be set as requested");
    break;
  default:
    Status = EFI_UNSUPPORTED;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

EFI_STATUS
EFIAPI
UsbfnGetEndpointPolicy(
  IN      EFI_USBFN_IO_PROTOCOL         *This,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         PolicyType,
  IN OUT  UINTN                         *BufferSize,
  OUT     VOID                          *Buffer
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  USBFN_DEV   *Usbfn  = NULL;

  FNC_ENTER_MSG ();

  if (!This || !BufferSize || !Buffer ||
      EndpointIndex >= USB_ENDPOINT_NUMBER_MAX ||
      Direction > EfiUsbEndpointDirectionHostIn) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("Invalid Parameters");
  }

  DBG(EFI_D_INFO, "EP %d, Dir %d, PolicyType %d, BufferSize %d, Buffer %p",
    EndpointIndex,
    Direction,
    PolicyType,
    *BufferSize,
    Buffer
    );

  Usbfn = USBFN_FROM_THIS(This);
  USB_ASSERT_RETURN(Usbfn, EFI_INVALID_PARAMETER);

  switch (PolicyType){  
  case EfiUsbPolicyMaxTransactionSize:
    if (*BufferSize < sizeof(UINT32)){
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = sizeof(UINT32);
      ERR_CHK("buffer too small");
    }

    *BufferSize = sizeof(UINTN);
    *((UINTN *) Buffer) = (Usbfn->SWDevice.max_transfer_size)[EndpointIndex][Direction];
    break;
  case EfiUsbPolicyZeroLengthTerminationSupport:
    // check for buffer size
    if(*BufferSize < sizeof(BOOLEAN)){
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = sizeof(BOOLEAN);
      ERR_CHK("buffer too small");
    }
    // CI core supports ZLT on TX endpoints
    if (Direction == EfiUsbEndpointDirectionDeviceTx) {
      *((BOOLEAN *)Buffer) = TRUE;
    } else {
      *((BOOLEAN *)Buffer) = FALSE;
    }
    *BufferSize = sizeof(BOOLEAN);
    break;
  case EfiUsbPolicyZeroLengthTermination:
    if (*BufferSize < sizeof(BOOLEAN)){
      Status = EFI_BUFFER_TOO_SMALL;
      *BufferSize = sizeof(BOOLEAN);
      ERR_CHK("buffer too small");
    }
    *BufferSize = sizeof(BOOLEAN);
    *((BOOLEAN *) Buffer) = Usbfn->SWDevice.zlt[EndpointIndex][Direction];
    break;
  default:
    Status = EFI_UNSUPPORTED;
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}
