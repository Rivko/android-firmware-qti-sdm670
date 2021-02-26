/**
 * @file FastBootMain.c
 *
 * FastBoot Main File
 *  
 * Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
 */

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/31/2016 ck      Add delay USB init support for fastboot client
 09/24/14   ck      Update to adjust EfiUsbDevice header change
 09/12/14   bs      Made Fastboot standalone app 
 08/15/14   ml      Clean up
 08/15/14   ml      Initial revision

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIUsbDevice.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>

#include "UsbDescriptors.h"
#include "Usbfastboot.h"
#include "FastbootUtils.h"

#define USB_BUFF_SIZE 1024*1024*1

extern EFI_GUID   gEfiUsbDeviceProtocolGuid;
extern BOOLEAN    Finished;

EFI_USB_DEVICE_PROTOCOL         *UsbDeviceProtocol;
VOID                            *gRxBuffer;
VOID                            *gTxBuffer;

extern EFI_GUID   gInitUsbControllerGuid;

/**
 * Dummy function needed for event notification callback 
 */
VOID
DummyNotify (
	IN EFI_EVENT Event,
	IN VOID *Context
	) 
{ 
  ; 
}

EFI_HANDLE fabImageHandle;
EFI_EVENT  UsbConfigEvt;

// USB Endpoint Direction
#define USB_ENDPOINT_DIRECTION_OUT        0
#define USB_ENDPOINT_DIRECTION_IN         1

// Get ep and dir from EndpointIndex
#define USB_INDEX_TO_EP(index)        ((index) & 0xf)
#define USB_INDEX_TO_EPDIR(index)     (((index) >> 7 & 0x1) ? USB_ENDPOINT_DIRECTION_IN : USB_ENDPOINT_DIRECTION_OUT)

EFI_STATUS
FastbootUsbDeviceStart(VOID)
{
  EFI_STATUS                      Status;
  USB_DEVICE_DESCRIPTOR          *DevDesc;
  VOID                          **Descriptors;
  
  /* Locate the USBFastboot  Protocol from DXE */
  Status = gBS->LocateProtocol( &gEfiUsbDeviceProtocolGuid, 
                                NULL,
                                (VOID **) &UsbDeviceProtocol);
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("couldnt find Fastboot USB protocol, exiting now");
    return Status;
  }

  BuildDefaultDescriptors(&DevDesc, &Descriptors); //see if this is necesary
 
  
  /* no callbacks */
  Status = UsbDeviceProtocol->Start(DevDesc, Descriptors, &DeviceQualifier, NULL, 5, StrDescriptors);

  Status = UsbDeviceProtocol->AllocateTransferBuffer(USB_BUFF_SIZE, &gRxBuffer);
  if (EFI_ERROR(Status))
  {
    AsciiPrint("could not allocate rx buffer");
    return Status;
  }

  Status = UsbDeviceProtocol->AllocateTransferBuffer(USB_BUFF_SIZE, &gTxBuffer);
  if (EFI_ERROR(Status))
  {
    AsciiPrint("could not allocate tx buffer");
    return Status;
  }

  return Status;
}

EFI_STATUS
FastbootUsbDeviceStop( VOID )
{
  EFI_STATUS     Status;

  Status = UsbDeviceProtocol->FreeTransferBuffer(gTxBuffer);
  if (EFI_ERROR(Status))
  {
    AsciiPrint("could not free tx buffer");
    return Status;
  }
  Status = UsbDeviceProtocol->FreeTransferBuffer(gRxBuffer);
  if (EFI_ERROR(Status))
  {
    AsciiPrint("could not free rx buffer");
    return Status;
  }
  return Status;
}

EFI_STATUS
ProcessBulkXfrCompleteRx(
  IN  USB_DEVICE_TRANSFER_OUTCOME *Uto
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  
  // switch on the transfer status
  switch (Uto->Status) 
  {
    case UsbDeviceTransferStatusCompleteOK:
      DataReady(Uto->BytesCompleted, gRxBuffer);
      break;

    case UsbDeviceTransferStatusCancelled:
      //if usb connected, retry, otherwise wait to get connected, then retry
      DEBUG((EFI_D_ERROR, "Bulk in XFR aborted\n"));
      Status = EFI_ABORTED;
      break;

    default: // Other statuses should not occur
      Status = EFI_DEVICE_ERROR;
      break;
  }
  return Status;
}

EFI_STATUS
ProcessBulkXfrCompleteTx(
  IN  USB_DEVICE_TRANSFER_OUTCOME *Uto
)
{
  EFI_STATUS  Status = EFI_SUCCESS;

  // Switch on the transfer status
  switch (Uto->Status) {
    case UsbDeviceTransferStatusCompleteOK:
      /* Just Queue the next recieve, must be a Command */
      Status = UsbDeviceProtocol->Send(0x1, GetXfrSize() , gRxBuffer);
      break;

    case UsbDeviceTransferStatusCancelled:
      AsciiPrint("Bulk in xfr aborted");
      Status = EFI_DEVICE_ERROR;
      break;

    default: // Other statuses should not occur
      AsciiPrint("unhandled trasnfer status");
      Status = EFI_DEVICE_ERROR;
      break;
  }
  return Status;
}

STATIC
VOID
SignalToEnumerate (VOID)
{
  EFI_STATUS Status;	
  Status = gBS->CreateEventEx(
      EVT_NOTIFY_SIGNAL,
      TPL_CALLBACK,
      DummyNotify,
      NULL,
      &gInitUsbControllerGuid,
      &UsbConfigEvt
      );

  if (EFI_ERROR(Status)) {
	DEBUG ((EFI_D_ERROR, "Event not signalled: %r", Status));
  }
  else {
    gBS->SignalEvent(UsbConfigEvt);
    gBS->CloseEvent(UsbConfigEvt);
  }
}

EFI_STATUS
EFIAPI
FastbootAppEntryPoint(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
 )
{
  EFI_STATUS                      Status     = EFI_SUCCESS;
  EFI_INPUT_KEY                   Key;
  USB_DEVICE_EVENT                Msg;
  USB_DEVICE_EVENT_DATA           Payload;
  UINTN                           PayloadSize;
  fabImageHandle = ImageHandle;

  if(sizeof (UINTN) == 0x8)
    AsciiPrint ("UEFI Fastboot Build Info : 64b %a %a\n", __DATE__, __TIME__);
  else
    AsciiPrint ("UEFI Fastboot Build Info : 32b %a %a\n", __DATE__, __TIME__);

  SignalToEnumerate();

  Status = FastbootAppInit();
  if (Status  != EFI_SUCCESS)
  {
    AsciiPrint("couldnt init fastboot , exiting");
    return Status;
  }
  Status = FastbootUsbDeviceStart();
  if (Status  != EFI_SUCCESS)
  {
    AsciiPrint("couldnt Start fastboot usb device, exiting");
    return Status;
  }

  while (ReadAnyKey(&Key, READ_KEY_ATTRIB_NO_BLOCKING) != EFI_SUCCESS) 
  {
    UsbDeviceProtocol->HandleEvent(&Msg, &PayloadSize, &Payload);
    if (UsbDeviceEventDeviceStateChange == Msg) 
    {
      if (UsbDeviceStateConnected == Payload.DeviceState) {
        Status = UsbDeviceProtocol->Send(0x1, 511, gRxBuffer); // Max command length is 64 anyway
      }
    } 
    else if (UsbDeviceEventTransferNotification == Msg) {
      if (1 == USB_INDEX_TO_EP(Payload.TransferOutcome.EndpointIndex)) {
        if (USB_ENDPOINT_DIRECTION_OUT == USB_INDEX_TO_EPDIR(Payload.TransferOutcome.EndpointIndex)) {
          Status = ProcessBulkXfrCompleteRx(&Payload.TransferOutcome);
          if (EFI_ERROR(Status))
          {
            AsciiPrint("Error, should not happen! Fastboot USB fail");
            Finished = TRUE;
          }
        } 
        else {
          Status = ProcessBulkXfrCompleteTx(&Payload.TransferOutcome);
          if (EFI_ERROR(Status))
          {
            AsciiPrint("Error, should not happen! Fastboot USB fail");
            Finished = TRUE;
          }
        }
      }
    }
    if (Finished) 
    {
      AsciiPrint("Continue detected, Exiting App...\n");
      break;
    }
  }


  //AsciiPrint("ReadAnyKey registered input, exiting\n");
  Status = FastbootAppUnInit();
  if (Status  != EFI_SUCCESS)
  {
    AsciiPrint("couldnt uninit fastboot\n");
    return Status;
  }

  Status = FastbootUsbDeviceStop();
  return Status;
}
