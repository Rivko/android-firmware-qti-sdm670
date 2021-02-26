/** @file

  QCOM_USB_PORT_TEST_PROTOCOL implementation.

  Copyright (c) 2011, 2014, 2017 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

**/


/*=============================================================================
                              EDIT HISTORY

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 02/01/17   pm       Port form Boot.xf.1.4
 02/05/14   amitg    Async USB Host Stack
 06/13/13   mliss    Updates to support multiple instances (1 per UsbBus).
 11/02/11   wufan    Initial implementation.
=============================================================================*/

//
// Includes
//
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/EFIUsbPortTestHelper.h>

#include "UsbBus.h"
#include "UsbDesc.h"
#include "UsbPortTestHelper.h"


#define _DBG(EfiD, Expression, ...)	DEBUG((EfiD, Expression, ##__VA_ARGS__))
#define DBG(EfiD, Str, ...) \
  _DBG(EfiD, "%a: " Str "\n", __FUNCTION__, ##__VA_ARGS__)


// Forward declarations
EFI_STATUS
UsbPortTestHelperGetDevInfo (
  IN  QCOM_USB_PORT_TEST_HELPER_PROTOCOL    *This,
  OUT UINT8                                 *DeviceNumber,
  OUT UINT8                                 *ParentPort
  );

EFI_STATUS
UsbPortTestHelperGetDevDesc (
  IN  QCOM_USB_PORT_TEST_HELPER_PROTOCOL        *This
  );


// Protocol template
STATIC QCOM_USB_PORT_TEST_HELPER_PROTOCOL gUsbPortTestHelperTemplate = {
  QCOM_USB_PORT_TEST_HELPER_PROTOCOL_REVISION,
  UsbPortTestHelperGetDevInfo,
  UsbPortTestHelperGetDevDesc,
};


/**
  Find the first USB device in root hub.

  @param Bus            USB BUS
  @param DeviceNumber   the device number
  @param ParentPort     parent port number for the device

  @retval EFI_SUCCESS        success
  @retval EFI_DEVICE_ERROR   no device in root hub

**/
static
EFI_STATUS
FindUsbDevice (
  IN  USB_BUS  *Bus,
  OUT UINT8    *DeviceNumber,
  OUT UINT8    *ParentPort
  )
{
  USB_DEVICE   *dev;
  UINTN        i;

  if (Bus == NULL || DeviceNumber == NULL || ParentPort == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for ( i = 1; i< Bus->MaxDevices; i++ ) {
    dev = Bus->Devices[i];

    if ((NULL != dev) && (0 != dev->Address) && (0 == dev->ParentAddr)) {
      *DeviceNumber = i;
      *ParentPort = dev->ParentPort;
      return EFI_SUCCESS;
    }
  }

  DBG(EFI_D_ERROR, "can't find any device in root hub");
  return EFI_DEVICE_ERROR;
}


EFI_STATUS
UsbPortTestHelperGetDevInfo (
  IN  QCOM_USB_PORT_TEST_HELPER_PROTOCOL  *This,
  OUT UINT8                               *DeviceNumber,
  OUT UINT8                               *ParentPort
  )
{
  QCOM_USB_PORT_TEST_HELPER_PRIVATE   *Priv;

  if (NULL == This || NULL == DeviceNumber || NULL == ParentPort) {
    return EFI_INVALID_PARAMETER;
  }

  Priv = PORT_TEST_HELPER_FROM_THIS(This);

  return FindUsbDevice (Priv->UsbBus, DeviceNumber, ParentPort);
}


EFI_STATUS
UsbPortTestHelperGetDevDesc (
  IN  QCOM_USB_PORT_TEST_HELPER_PROTOCOL        *This
  )
{
  UINT8      DeviceNumber, ParentPort;
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_USB_PORT_TEST_HELPER_PRIVATE  *Priv;

  if (NULL == This) {
    DBG(EFI_D_ERROR, "invalid parameter");
	return EFI_INVALID_PARAMETER;
  }

  Priv = PORT_TEST_HELPER_FROM_THIS(This);

  if (NULL == Priv) {
    DBG(EFI_D_ERROR, "Priv cannot be NULL");
	return EFI_DEVICE_ERROR;
  }

  Status = FindUsbDevice (Priv->UsbBus, &DeviceNumber, &ParentPort);

  if (EFI_ERROR (Status)) {
    DBG (EFI_D_ERROR, "failed to find USB device: %p", Status);
    return Status;
  }

  return UsbGetDevDesc (Priv->UsbBus->Devices[DeviceNumber], FALSE, NULL);
}


EFI_STATUS
UsbPortTestHelperBuildProtocol (
  IN  USB_BUS                             *UsbBus,
  OUT QCOM_USB_PORT_TEST_HELPER_PROTOCOL  **PortTestHelper
  )
{
  QCOM_USB_PORT_TEST_HELPER_PRIVATE   *PortTestPrivate;

  if (NULL == UsbBus || NULL == PortTestHelper) {
    return EFI_INVALID_PARAMETER;
  }

  // allocate private structure containing protocol instance
  PortTestPrivate = AllocateZeroPool (sizeof(QCOM_USB_PORT_TEST_HELPER_PRIVATE));

  if (NULL == PortTestPrivate) {
    DBG (EFI_D_ERROR, "Failed to allocate PortTestHelper private data");
    return EFI_OUT_OF_RESOURCES;
  }

  // initialize private data
  PortTestPrivate->Signature = USB_PORT_TEST_HELPER_SIGNATURE;
  PortTestPrivate->UsbBus = UsbBus;
  CopyMem(&PortTestPrivate->PortTestHelper, &gUsbPortTestHelperTemplate, sizeof(QCOM_USB_PORT_TEST_HELPER_PROTOCOL));

  // return protocol pointer
  *PortTestHelper = &PortTestPrivate->PortTestHelper;

  return EFI_SUCCESS;
}

