/**
 * @file UsbfnDesc.c
 *
 * Defines the loopback USB descriptor structures.
 *
 * Copyright (c) 2014, QUALCOMM Technologies Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/02/16   ck      Remove the CONST modifier of structures which are modified
 11/20/14   ml      Migrate to official SuperSpeed support
 06/09/14   ck      Change bMaxPower for SS enumeration
 01/28/14   ml      Initial revision
=============================================================================*/


#include "UsbfnDesc.h"


#pragma pack(push)
#pragma pack(1)

STATIC
CONST
EFI_USB_DEVICE_DESCRIPTOR
DeviceDescriptor = {
  sizeof(EFI_USB_DEVICE_DESCRIPTOR),  // uint8  bLength;
  USB_DESC_TYPE_DEVICE,               // uint8  bDescriptorType;
  0x0200,                             // uint16 bcdUSB;
  0xff,                               // uint8  bDeviceClass;
  0xff,                               // uint8  bDeviceSubClass;
  0xff,                               // uint8  bDeviceProtocol;
  64,                                 // uint8  bMaxPacketSize0;
  USBLB_VENDOR_ID,                    // uint16 idVendor;
  USBLB_PRODUCT_ID,                   // uint16 idProduct;
  0x0,                                // uint16 bcdDevice;
  1,                                  // uint8  iManufacturer;
  2,                                  // uint8  iProduct;
  0,                                  // uint8  iSerialNumber;
  1                                   // uint8  bNumConfigurations;
};

STATIC
CONST
EFI_USB_DEVICE_DESCRIPTOR
SSDeviceDescriptor = {
  sizeof(EFI_USB_DEVICE_DESCRIPTOR),  // uint8  bLength;
  USB_DESC_TYPE_DEVICE,               // uint8  bDescriptorType;
  0x0300,                             // uint16 bcdUSB;
  0xff,                               // uint8  bDeviceClass;
  0xff,                               // uint8  bDeviceSubClass;
  0xff,                               // uint8  bDeviceProtocol;
  9,                                  // uint8  bMaxPacketSize0;
  USBLB_VENDOR_ID,                    // uint16 idVendor;
  USBLB_PRODUCT_ID,                   // uint16 idProduct;
  0x0,                                // uint16 bcdDevice;
  1,                                  // uint8  iManufacturer;
  2,                                  // uint8  iProduct;
  0,                                  // uint8  iSerialNumber;
  1                                   // uint8  bNumConfigurations;
};

CONST
EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR
DeviceQualifier = {
  sizeof(EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR),  // uint8  bLength;
  USB_DESC_TYPE_DEVICE_QUALIFIER,               // uint8  bDescriptorType;
  0x0200,                                       // uint16 bcdUSB;
  0xff,                                         // uint8  bDeviceClass;
  0xff,                                         // uint8  bDeviceSubClass;
  0xff,                                         // uint8  bDeviceProtocol;
  64,                                           // uint8  bMaxPacketSize0;
  1,                                            // uint8  bNumConfigurations;
  0                                             // uint8  bReserved;
};

STATIC
CONST
struct {
  EFI_USB_BOS_DESCRIPTOR                BosDescriptor;
  EFI_USB_USB_20_EXTENSION_DESCRIPTOR   Usb2ExtDescriptor;
  EFI_USB_SUPERSPEED_USB_DESCRIPTOR     SsUsbDescriptor;
} BinaryObjectStore = {
  // BOS Descriiptor
  {
    sizeof(EFI_USB_BOS_DESCRIPTOR),               // Descriptor Size
    USB_DESC_TYPE_BOS,                            // Descriptor Type
    sizeof(BinaryObjectStore),                    // Total Length
    2                                             // Number of device capabilities
  },
  // USB2 Extension Desc
  {
    sizeof(EFI_USB_USB_20_EXTENSION_DESCRIPTOR),  // Descriptor Size
    USB_DESC_TYPE_DEVICE_CAPABILITY,              // Device Capability Type descriptor
    USB_DEV_CAP_TYPE_USB_20_EXTENSION,            // USB 2.0 Extension Capability Type
    0x2                                           // Supported device level features
  },
  // Super Speed Device Capability Desc
  {
    sizeof(EFI_USB_SUPERSPEED_USB_DESCRIPTOR),    // Descriptor Size
    USB_DESC_TYPE_DEVICE_CAPABILITY,              // Device Capability Type descriptor
    USB_DEV_CAP_TYPE_SUPERSPEED_USB,              // SuperSpeed Device Capability Type
    0x00,                                         // Supported device level features
    0x0E,                                         // Speeds Supported by the device: SS, HS and FS
    0x01,                                         // Functionality support
    0x07,                                         // U1 Device Exit Latency
    0x65                                          // U2 Device Exit Latency
  }
};

STATIC
struct {
  EFI_USB_CONFIG_DESCRIPTOR    ConfigDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR  EndpointDescriptor0;
  EFI_USB_ENDPOINT_DESCRIPTOR  EndpointDescriptor1;
} TotalConfigDescriptor = {
  {
    sizeof(EFI_USB_CONFIG_DESCRIPTOR),    // uint8  bLength;
    USB_DESC_TYPE_CONFIG,                 // uint8  bDescriptorType;
    sizeof(TotalConfigDescriptor),        // uint16 wTotalLength;
    1,                                    // uint8  bNumInterfaces;
    1,                                    // uint8  bConfigurationValue;
    0,                                    // uint8  iConfiguration;
    0x80,                                 // uint8  bmAttributes;
    10                                    // uint8  bMaxPower;
  },
  {
    sizeof(EFI_USB_INTERFACE_DESCRIPTOR), // uint8  bLength;
    USB_DESC_TYPE_INTERFACE,              // uint8  bDescriptorType;
    0,                                    // uint8  bInterfaceNumber;
    0,                                    // uint8  bAlternateSetting;
    2,                                    // uint8  bNumEndpoints;
    0xff,                                 // uint8  bInterfaceClass;
    0xff,                                 // uint8  bInterfaceSubClass;
    0xff,                                 // uint8  bInterfaceProtocol;
    0                                     // uint8  iInterface;
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR),  // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,               // uint8  bDescriptorType;
    ENDPOINT_ADDR(USBLB_BULK_EP, TRUE),   // uint8  bEndpointAddress;
    USB_ENDPOINT_BULK,                    // uint8  bmAttributes;
    512,                                  // uint16 wMaxPacketSize; HS=512 , FS=64
    0                                     // uint8  bInterval;
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR),  // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,               // uint8  bDescriptorType;
    ENDPOINT_ADDR(USBLB_BULK_EP, FALSE),  // uint8  bEndpointAddress;
    USB_ENDPOINT_BULK,                    // uint8  bmAttributes;
    512,                                  // uint16 wMaxPacketSize; HS=512 , FS=64
    0                                     // uint8  bInterval;
  },
};

STATIC
struct {
  EFI_USB_CONFIG_DESCRIPTOR                 ConfigDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR              InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR               EndpointDescriptor0;
  EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR  EndpointCompanionDescriptor0;
  EFI_USB_ENDPOINT_DESCRIPTOR               EndpointDescriptor1;
  EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR  EndpointCompanionDescriptor1;
} TotalSSConfigDescriptor = {
  {
    sizeof(EFI_USB_CONFIG_DESCRIPTOR),    // uint8  bLength;
    USB_DESC_TYPE_CONFIG,                 // uint8  bDescriptorType;
    sizeof(TotalSSConfigDescriptor),      // uint16 wTotalLength;
    1,                                    // uint8  bNumInterfaces;
    1,                                    // uint8  bConfigurationValue;
    0,                                    // uint8  iConfiguration;
    0x80,                                 // uint8  bmAttributes;
    10                                    // uint8  bMaxPower;
  },
  {
    sizeof(EFI_USB_INTERFACE_DESCRIPTOR), // uint8  bLength;
    USB_DESC_TYPE_INTERFACE,              // uint8  bDescriptorType;
    0,                                    // uint8  bInterfaceNumber;
    0,                                    // uint8  bAlternateSetting;
    2,                                    // uint8  bNumEndpoints;
    0xff,                                 // uint8  bInterfaceClass;
    0xff,                                 // uint8  bInterfaceSubClass;
    0xff,                                 // uint8  bInterfaceProtocol;
    0                                     // uint8  iInterface;
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR),  // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,               // uint8  bDescriptorType;
    ENDPOINT_ADDR(USBLB_BULK_EP, TRUE),   // uint8  bEndpointAddress;
    USB_ENDPOINT_BULK,                    // uint8  bmAttributes;
    1024,                                 // uint16 wMaxPacketSize;
    0                                     // uint8  bInterval;
  },
  {
    sizeof(EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR), // uint8  bLength;
    USB_DESC_TYPE_SS_ENDPOINT_COMPANION,              // uint8  bDescriptorType;
    4,                                                // uint8  bMaxBurst;
    0,                                                // uint8  bmAttributes;
    0                                                 // uint16 wBytesPerInterval;
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR),  // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,               // uint8  bDescriptorType;
    ENDPOINT_ADDR(USBLB_BULK_EP, FALSE),  // uint8  bEndpointAddress;
    USB_ENDPOINT_BULK,                    // uint8  bmAttributes;
    1024,                                 // uint16 wMaxPacketSize;
    0                                     // uint8  bInterval;
  },
  {
    sizeof(EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR), // uint8  bLength;
    USB_DESC_TYPE_SS_ENDPOINT_COMPANION,              // uint8  bDescriptorType;
    4,                                                // uint8  bMaxBurst;
    0,                                                // uint8  bmAttributes;
    0                                                 // uint16 wBytesPerInterval;
  },
};

#pragma pack(pop)


// UsbfnIo Device Info Structures
STATIC
CONST
EFI_USB_ENDPOINT_DESCRIPTOR
*EndpointDescriptorTable[] = {
  &TotalConfigDescriptor.EndpointDescriptor0,
  &TotalConfigDescriptor.EndpointDescriptor1
};

STATIC
CONST
EFI_USB_INTERFACE_INFO
InterfaceInfo = {
  (EFI_USB_INTERFACE_DESCRIPTOR *)&TotalConfigDescriptor.InterfaceDescriptor,
  (EFI_USB_ENDPOINT_DESCRIPTOR **)EndpointDescriptorTable
};

STATIC
CONST
EFI_USB_INTERFACE_INFO
*InterfaceInfoTable[] = {
  &InterfaceInfo
};

STATIC
CONST
EFI_USB_CONFIG_INFO
ConfigInfo = {
  (EFI_USB_CONFIG_DESCRIPTOR *)&TotalConfigDescriptor.ConfigDescriptor,
  (EFI_USB_INTERFACE_INFO **)InterfaceInfoTable
};

STATIC
CONST
EFI_USB_CONFIG_INFO
*ConfigInfoTable[] = {
  &ConfigInfo
};

CONST
EFI_USB_DEVICE_INFO
DeviceInfo = {
  (EFI_USB_DEVICE_DESCRIPTOR *)&DeviceDescriptor,
  (EFI_USB_CONFIG_INFO **)ConfigInfoTable
};

// UsbfnIo SuperSpeed Device Info Structures
STATIC
CONST
EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR
SSEndpointDescriptor0 = {
  (EFI_USB_ENDPOINT_DESCRIPTOR *)&TotalSSConfigDescriptor.EndpointDescriptor0,
  (EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR *)&TotalSSConfigDescriptor.EndpointCompanionDescriptor0
};

STATIC
CONST
EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR
SSEndpointDescriptor1 = {
  (EFI_USB_ENDPOINT_DESCRIPTOR *)&TotalSSConfigDescriptor.EndpointDescriptor1,
  (EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR *)&TotalSSConfigDescriptor.EndpointCompanionDescriptor1
};

STATIC
CONST
EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR
*SSEndpointDescriptorTable[] = {
  &SSEndpointDescriptor0,
  &SSEndpointDescriptor1
};

STATIC
CONST
EFI_USB_SUPERSPEED_INTERFACE_INFO
SSInterfaceInfo = {
  (EFI_USB_INTERFACE_DESCRIPTOR *)&TotalSSConfigDescriptor.InterfaceDescriptor,
  (EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR **)SSEndpointDescriptorTable
};

STATIC
CONST
EFI_USB_SUPERSPEED_INTERFACE_INFO
*SSInterfaceInfoTable[] = {
  &SSInterfaceInfo
};

STATIC
CONST
EFI_USB_SUPERSPEED_CONFIG_INFO
SSConfigInfo = {
  (EFI_USB_CONFIG_DESCRIPTOR *)&TotalSSConfigDescriptor.ConfigDescriptor,
  (EFI_USB_SUPERSPEED_INTERFACE_INFO **)SSInterfaceInfoTable
};

STATIC
CONST
EFI_USB_SUPERSPEED_CONFIG_INFO
*SSConfigInfoTable[] = {
  &SSConfigInfo,
};

CONST
EFI_USB_SUPERSPEED_DEVICE_INFO
SSDeviceInfo = {
  (EFI_USB_DEVICE_DESCRIPTOR *)&SSDeviceDescriptor,
  (EFI_USB_SUPERSPEED_CONFIG_INFO **)SSConfigInfoTable,
  (EFI_USB_BOS_DESCRIPTOR *)&BinaryObjectStore
};

// String Descriptors
CONST
UINT8
Str0Descriptor[4] = {
  sizeof(Str0Descriptor),USB_DESC_TYPE_STRING,
  0x09,0x04 // Langid : US_EN.
};

CONST
UINT8
StrManufacturerDescriptor[18] = {
  sizeof(StrManufacturerDescriptor),USB_DESC_TYPE_STRING,
  'Q',0,
  'U',0,
  'A',0,
  'L',0,
  'C',0,
  'O',0,
  'M',0,
  'M',0,
};

CONST
UINT8
StrProductDescriptor[32] = {
  sizeof(StrProductDescriptor),USB_DESC_TYPE_STRING,
  'U',0,
  'S',0,
  'B',0,
  ' ',0,
  'F',0,
  'N',0,
  ' ',0,
  'L',0,
  'o',0,
  'o',0,
  'p',0,
  'b',0,
  'a',0,
  'c',0,
  'k',0,
};

