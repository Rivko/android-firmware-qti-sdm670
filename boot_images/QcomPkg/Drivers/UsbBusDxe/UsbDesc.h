/** @file

    Manage Usb Descriptor List

Copyright (c) 2010 - 2017 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
Portions Copyright (c) 2007 - 2014, Intel Corporation. All rights reserved.<BR>
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
 --------   ---      ----------------------------------------------------------
 02/01/17   pm       Port changes from BOOT.XF.1.4
 02/05/14   amitg    Async USB Host Stack
 03/30/11   sniedzie Port fix for enumeration failure.
 03/30/11   sniedzie Integrated from MdeModulePkg for QCOM changes.

=============================================================================*/

#ifndef _USB_DESCRIPTOR_H_
#define _USB_DESCRIPTOR_H_

#define USB_MAX_INTERFACE_SETTING  256

#define CONTROL_IN_ASYNC_POLL_TIME 3 // Timeout in ms

//
// The RequestType in EFI_USB_DEVICE_REQUEST is composed of
// three fields: One bit direction, 2 bit type, and 5 bit
// target.
//
#define USB_REQUEST_TYPE(Dir, Type, Target) \
          ((UINT8)((((Dir) == EfiUsbDataIn ? 0x01 : 0) << 7) | (Type) | (Target)))

//
// A common header for usb standard descriptor.
// Each stand descriptor has a length and type.
//
#pragma pack(1)
typedef struct {
  UINT8                   Len;
  UINT8                   Type;
} USB_DESC_HEAD;
#pragma pack()


//
// Each USB device has a device descriptor. Each device may
// have several configures. Each configure contains several
// interfaces. Each interface may have several settings. Each
// setting has several endpoints.
//
// EFI_USB_..._DESCRIPTOR must be the first member of the
// structure.
//
typedef struct {
  EFI_USB_ENDPOINT_DESCRIPTOR   Desc;
  UINT8                         Toggle;
} USB_ENDPOINT_DESC;

typedef struct {
  EFI_USB_INTERFACE_DESCRIPTOR  Desc;
  USB_ENDPOINT_DESC             **Endpoints;
} USB_INTERFACE_SETTING;

//
// An interface may have several settings. Use a
// fixed max number of settings to simplify code.
// It should sufice in most environments.
//
typedef struct {
  USB_INTERFACE_SETTING*        Settings[USB_MAX_INTERFACE_SETTING];
  UINTN                         NumOfSetting;
  UINTN                         ActiveIndex;  // Index of active setting
} USB_INTERFACE_DESC;

typedef struct {
  EFI_USB_CONFIG_DESCRIPTOR     Desc;
  USB_INTERFACE_DESC            **Interfaces;
} USB_CONFIG_DESC;

typedef struct {
  EFI_USB_DEVICE_DESCRIPTOR     Desc;
  USB_CONFIG_DESC               **Configs;
} USB_DEVICE_DESC;

/**
  USB standard control transfer support routine. This
  function is used by USB device. It is possible that
  the device's interfaces are still waiting to be
  enumerated.

  @param  UsbDev                The usb device.
  @param  Direction             The direction of data transfer.
  @param  Type                  Standard / class specific / vendor specific.
  @param  Target                The receiving target.
  @param  Request               Which request.
  @param  Value                 The wValue parameter of the request.
  @param  Index                 The wIndex parameter of the request.
  @param  Buf                   The buffer to receive data into / transmit from.
  @param  Length                The length of the buffer.
  @param  AsyncFlag             Flag to define async control transfer
  @param  AsyncConext           Async control transfer context

  @retval EFI_SUCCESS           The control request is executed.
  @retval EFI_DEVICE_ERROR      Failed to execute the control transfer.

**/
EFI_STATUS
UsbCtrlRequest (
  IN USB_DEVICE             *UsbDev,
  IN EFI_USB_DATA_DIRECTION Direction,
  IN UINTN                  Type,
  IN UINTN                  Target,
  IN UINTN                  Request,
  IN UINT16                 Value,
  IN UINT16                 Index,
  IN OUT VOID               *Buf,
  IN UINTN                  Length,
  IN BOOLEAN                AsyncFlag,
  IN VOID                   *AsyncConext
  );

/**
  Return the max packet size for endpoint zero. This function
  is the first function called to get descriptors during bus
  enumeration.

  @param  UsbDev                The usb device.
  @param  AsyncFlag             Flag to define async control transfer
  @param  AsyncConext           Async control transfer context

  @retval EFI_SUCCESS           The max packet size of endpoint zero is retrieved.
  @retval EFI_DEVICE_ERROR      Failed to retrieve it.

**/
EFI_STATUS
UsbGetMaxPacketSize0 (
  IN USB_DEVICE           *UsbDev,
  IN BOOLEAN               AsyncFlag,
  IN  VOID                *AsyncConext
  );

/**
  Free a device descriptor with its configurations.

  @param  DevDesc               The device descriptor.

  @return None.

**/
VOID
UsbFreeDevDesc (
  IN USB_DEVICE_DESC      *DevDesc
  );

/**
  Retrieve the indexed string for the language. It requires two
  steps to get a string, first to get the string's length. Then
  the string itself.

  @param  UsbDev                The usb device.
  @param  StringIndex           The index of the string to retrieve.
  @param  LangId                Language ID.
  @param  AsyncFlag             Flag to define async control transfer
  @param  AsyncConext           Async control transfer context

  @return The created string descriptor or NULL.

**/
EFI_USB_STRING_DESCRIPTOR*
UsbGetOneString (
  IN     USB_DEVICE       *UsbDev,
  IN     UINT8            StringIndex,
  IN     UINT16           LangId,
  IN     BOOLEAN          AsyncFlag,
  IN     VOID             *AsyncConext
  );

/**
  Build the whole array of descriptors. This function must
  be called after UsbGetMaxPacketSize0 returns the max packet
  size correctly for endpoint 0.

  @param  UsbDev                The Usb device.
  @param  AsyncFlag             Flag to define async control transfer
  @param  AsyncConext           Async control transfer context

  @retval EFI_SUCCESS           The descriptor table is build.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate resource for the descriptor.

**/
EFI_STATUS
UsbBuildDescTable (
  IN USB_DEVICE           *UsbDev,
  IN  BOOLEAN             AsyncFlag,
  IN  VOID                *AsyncConext
  );

/**
  Set the device's address.

  @param  UsbDev                The device to set address to.
  @param  Address               The address to set.
  @param  AsyncFlag             Flag to define async control transfer
  @param  AsyncConext           Async control transfer context

  @retval EFI_SUCCESS           The device is set to the address.
  @retval Others                Failed to set the device address.

**/
EFI_STATUS
UsbSetAddress (
  IN USB_DEVICE           *UsbDev,
  IN UINT8                Address,
  IN BOOLEAN               AsyncFlag,
  IN  VOID                *AsyncConext
  );

/**
  Set the device's configuration. This function changes
  the device's internal state. UsbSelectConfig changes
  the Usb bus's internal state.

  @param  UsbDev                The USB device to set configure to.
  @param  ConfigIndex           The configure index to set.
  @param  AsyncFlag             Flag to define async control transfer
  @param  AsyncConext           Async control transfer context

  @retval EFI_SUCCESS           The device is configured now.
  @retval Others                Failed to set the device configure.

**/
EFI_STATUS
UsbSetConfig (
  IN USB_DEVICE           *UsbDev,
  IN UINT8                ConfigIndex,
  IN BOOLEAN              AsyncFlag,
  IN  VOID                *AsyncConext
  );

/**
  Usb UsbIo interface to clear the feature. This is should
  only be used by HUB which is considered a device driver
  on top of the UsbIo interface.

  @param  UsbIo                 The UsbIo interface.
  @param  Target                The target of the transfer: endpoint/device.
  @param  Feature               The feature to clear.
  @param  Index                 The wIndex parameter.

  @retval EFI_SUCCESS           The device feature is cleared.
  @retval Others                Failed to clear the feature.

**/
EFI_STATUS
UsbIoClearFeature (
  IN  EFI_USB_IO_PROTOCOL *UsbIo,
  IN  UINTN               Target,
  IN  UINT16              Feature,
  IN  UINT16              Index
  );

/**
  Get the device descriptor for the device.

  @param  UsbDev                The Usb device to retrieve descriptor from.
  @param  AsyncFlag             Flag to define async control transfer
  @param  AsyncConext           Async control transfer context

  @retval EFI_SUCCESS           The device descriptor is returned.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory.

**/
EFI_STATUS
UsbGetDevDesc (
  IN USB_DEVICE           *UsbDev,
  IN  BOOLEAN             AsyncFlag,
  IN  VOID                *AsyncConext
  );
#endif