/**
* @file UsbMsdDesc.c
*
* Defines the USB descriptors used for the mass storage device enumeration.
*
* Copyright (c) 2014, QUALCOMM Technologies Inc. All rights reserved.
*/
/*=============================================================================
EDIT HISTORY

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/14/14   ck      Init version
=============================================================================*/
#include "UsbMsdDesc.h"          // header
#include "UsbMsdUtility.h"       // ENDPOINT_ADDR macro 


CONST
EFI_USB_DEVICE_DESCRIPTOR
DeviceDescriptor ={
  sizeof(EFI_USB_DEVICE_DESCRIPTOR),  // uint8  bLength;
  USB_DESC_TYPE_DEVICE,               // uint8  bDescriptorType;
  0x0200,                             // uint16 bcdUSB;
  0x00,                               // uint8  bDeviceClass;
  0x00,                               // uint8  bDeviceSubClass;
  0x00,                               // uint8  bDeviceProtocol;
  64,                                 // uint8  bMaxPacketSize0;
  0x05C6,                             // uint16 idVendor;
  0xF000,                             // uint16 idProduct;
  0x0,                                // uint16 bcdDevice;
  1,                                  // uint8  iManufacturer;
  2,                                  // uint8  iProduct;
  3,                                  // uint8  iSerialNumber;
  0x1                                 // uint8  bNumConfigurations;
};

CONST
EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR
DeviceQualifier ={
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


USB_CONFIGURATION_TREE TotalConfigDescriptor ={
  {
    sizeof(EFI_USB_CONFIG_DESCRIPTOR),      // uint8  bLength;
    USB_DESC_TYPE_CONFIG,                   // uint8  bDescriptorType;
    sizeof(EFI_USB_CONFIG_DESCRIPTOR)
    +sizeof (EFI_USB_INTERFACE_DESCRIPTOR)
    +sizeof (EFI_USB_ENDPOINT_DESCRIPTOR)
    +sizeof (EFI_USB_ENDPOINT_DESCRIPTOR), // uint16 wTotalLength;
    1,                                      // uint8  bNumInterfaces;
    1,                                      // uint8  bConfigurationValue;
    0,                                      // uint8  iConfiguration;
    0x80,                                   // uint8  bmAttributes;
    1                                       // uint8  bMaxPower;
  },
  {
    sizeof(EFI_USB_INTERFACE_DESCRIPTOR),   // uint8  bLength;
    USB_DESC_TYPE_INTERFACE,                // uint8  bDescriptorType;
    0,                                      // uint8  bInterfaceNumber;
    0,                                      // uint8  bAlternateSetting;
    2,                                      // uint8  bNumEndpoints;
    0x08,                                   // uint8  bInterfaceClass;
    0x06,                                   // uint8  bInterfaceSubClass;
    0x50,                                   // uint8  bInterfaceProtocol;
    0                                       // uint8  iInterface;
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR),                           // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,                                        // uint8  bDescriptorType;
    ENDPOINT_ADDR(USBMSD_BULK_EP, EfiUsbEndpointDirectionHostIn),  // uint8  bEndpointAddress;
    USB_ENDPOINT_BULK,                                             // uint8  bmAttributes;
    512,                                                           // uint16 wMaxPacketSize;
    0                                                              // uint8  bInterval;
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR),                           // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,                                        // uint8  bDescriptorType;
    ENDPOINT_ADDR(USBMSD_BULK_EP, EfiUsbEndpointDirectionHostOut),    // uint8  bEndpointAddress;
    USB_ENDPOINT_BULK,                                             // uint8  bmAttributes;
    512,                                                           // uint16 wMaxPacketSize;
    0                                                              // uint8  bInterval;
  },
};

CONST
EFI_USB_ENDPOINT_DESCRIPTOR
*EndpointDescriptorTable[] ={
  &TotalConfigDescriptor.EndpointDescriptor0,
  &TotalConfigDescriptor.EndpointDescriptor1,
};

CONST
EFI_USB_INTERFACE_INFO
InterfaceInfo ={
  (EFI_USB_INTERFACE_DESCRIPTOR *)&TotalConfigDescriptor.InterfaceDescriptor,
  (EFI_USB_ENDPOINT_DESCRIPTOR **)EndpointDescriptorTable
};

CONST
EFI_USB_INTERFACE_INFO
*InterfaceInfoTable[] ={
  &InterfaceInfo,
};

CONST
EFI_USB_CONFIG_INFO
ConfigInfo ={
  (EFI_USB_CONFIG_DESCRIPTOR *)&TotalConfigDescriptor.ConfigDescriptor,
  (EFI_USB_INTERFACE_INFO **)InterfaceInfoTable
};

CONST
EFI_USB_CONFIG_INFO
*ConfigInfoTable[] ={
  &ConfigInfo,
};

CONST
EFI_USB_DEVICE_INFO
UsbMsdDeviceInfo ={
  (EFI_USB_DEVICE_DESCRIPTOR *)&DeviceDescriptor,
  (EFI_USB_CONFIG_INFO **)ConfigInfoTable
};

CONST
UINT8
Str0Descriptor[4] ={
  sizeof(Str0Descriptor),
  USB_DESC_TYPE_STRING,
  0x09, 0x04 // Langid : US_EN.
};

CONST
UINT8
StrManufacturerDescriptor[18] ={
  sizeof(StrManufacturerDescriptor),
  USB_DESC_TYPE_STRING,
  'Q', 0,
  'U', 0,
  'A', 0,
  'L', 0,
  'C', 0,
  'O', 0,
  'M', 0,
  'M', 0,
};

CONST
UINT8
StrProductDescriptor[34] ={
  sizeof(StrProductDescriptor),
  USB_DESC_TYPE_STRING,
  'U', 0,
  'S', 0,
  'B', 0,
  ' ', 0,
  'M', 0,
  'A', 0,
  'S', 0,
  'S', 0,
  ' ', 0,
  'S', 0,
  'T', 0,
  'O', 0,
  'R', 0,
  'A', 0,
  'G', 0,
  'E', 0,
};


/*
* If USBFn is not able to retrieve the serial number from the system, 
* use the hardcoded one. 
*/
CONST
UINT8
StrSerialDescriptor[42] ={
  sizeof(StrProductDescriptor),
  USB_DESC_TYPE_STRING,
  '0', 0,
  '5', 0,
  'C', 0,
  '6', 0,
  'F', 0,
  '0', 0,
  '0', 0,
  '0', 0,
  'D', 0,
  '3', 0,
  '5', 0,
  '3', 0,
  '9', 0,
  '5', 0,
  '4', 0,
  '7', 0,
  '6', 0,
  '3', 0,
  'A', 0,
  'F', 0,
};
