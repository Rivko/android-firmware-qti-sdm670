/**
 * @file UsbfnChgDesc.c
 *
 * Defines the USB descriptor structures used for the charger app.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/23/16   ck      Init version
=============================================================================*/


#include "UsbfnChgDesc.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/EFICardInfo.h>
#include <Library/PrintLib.h>

#define USB_VENDOR_ID      0x05C6
#define USB_PRODUCT_ID     0x0001
#define USB_INTR_EP_IN       0x81

/* USB Device Descriptor */
STATIC
EFI_USB_DEVICE_DESCRIPTOR
DeviceDescriptor = {
  sizeof(EFI_USB_DEVICE_DESCRIPTOR),  // uint8  bLength;
  USB_DESC_TYPE_DEVICE,               // uint8  bDescriptorType;
  0x0210,                             // uint16 bcdUSB;
  0x00,                               // uint8  bDeviceClass;
  0x00,                               // uint8  bDeviceSubClass;
  0x00,                               // uint8  bDeviceProtocol;
  64,                                 // uint8  bMaxPacketSize0;
  USB_VENDOR_ID,                      // uint16 idVendor;
  USB_PRODUCT_ID,                     // uint16 idProduct;
  0x0100,                              // uint16 bcdDevice;
  0,                                  // uint8  iManufacturer;
  0,                                  // uint8  iProduct;
  0,                                  // uint8  iSerialNumber;
  1                                   // uint8  bNumConfigurations;
};


STATIC
EFI_USB_DEVICE_DESCRIPTOR
SSDeviceDescriptor = {
  sizeof(EFI_USB_DEVICE_DESCRIPTOR),  // uint8  bLength;
  USB_DESC_TYPE_DEVICE,               // uint8  bDescriptorType;
  0x0300,                             // uint16 bcdUSB;
  0x00,                               // uint8  bDeviceClass;
  0x00,                               // uint8  bDeviceSubClass;
  0x00,                               // uint8  bDeviceProtocol;
  9,                                  // uint8  bMaxPacketSize0;
  USB_VENDOR_ID,                      // uint16 idVendor;
  USB_PRODUCT_ID,                     // uint16 idProduct;
  0x0100,                             // uint16 bcdDevice;
  0,                                  // uint8  iManufacturer;
  0,                                  // uint8  iProduct;
  0,                                  // uint8  iSerialNumber;
  1                                   // uint8  bNumConfigurations;
};


/* USB Device Qualifier Descriptor */
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


/* USB HID Report descriptor. HID spec 1.11 sec 5.2 */
UINT8
ReportDescriptor[USBFN_CHG_DEV_REPORT_DESC_SIZE] ={
  0x05, 0x01,                 // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,                 // USAGE (Mouse)
  0xA1, 0x01,                 // COLLECTION (Application)
  0x09, 0x01,                 //   USAGE (Pointer)
  0xA1, 0x00,                 //   COLLECTION (Physical)
  0x05, 0x09,                 //     USAGE_PAGE (Button)
  0x19, 0x01,                 //     USAGE_MINIMUM (Button 1)
  0x29, 0x03,                 //     USAGE_MAXIMUM (Button 3)
  0x15, 0x00,                 //     LOGICAL_MINIMUM (0)
  0x25, 0x01,                 //     LOGICAL_MAXIMUM (1)
  0x75, 0x01,                 //     REPORT_SIZE (1)
  0x95, 0x03,                 //     REPORT_COUNT (3)
  0x81, 0x02,                 //     INPUT (Data,Var,Abs)
  0x75, 0x05,                 //     REPORT_SIZE (5)
  0x95, 0x01,                 //     REPORT_COUNT (1)
  0x81, 0x01,                 //     INPUT (Cnst,Var,Abs)
  0x05, 0x01,                 //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,                 //     USAGE (X)
  0x09, 0x31,                 //     USAGE (Y)
  0x09, 0x38,                 //     USAGE (Wheel)
  0x15, 0x81,                 //     LOGICAL_MINIMUM (-127)
  0x25, 0x7F,                 //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,                 //     REPORT_SIZE (8) 
  0x95, 0x03,                 //     REPORT_COUNT (3)
  0x81, 0x06,                 //     INPUT (Data,Var,Rel)
  0xC0,                       //   END_COLLECTION
  0xC0                        // END_COLLECTION
};


/* USB Configuration as HID */
STATIC
struct _CfgDescTree {
  EFI_USB_CONFIG_DESCRIPTOR    ConfigDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
  EFI_USB_HID_DESCRIPTOR       HidDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR  EndpointDescriptor0;
} TotalConfigDescriptor = {
  {
    sizeof(EFI_USB_CONFIG_DESCRIPTOR),      // uint8  bLength;
    USB_DESC_TYPE_CONFIG,                   // uint8  bDescriptorType;
      sizeof(EFI_USB_CONFIG_DESCRIPTOR)
    + sizeof (EFI_USB_INTERFACE_DESCRIPTOR)
    + sizeof (EFI_USB_HID_DESCRIPTOR)
    + sizeof (EFI_USB_ENDPOINT_DESCRIPTOR), // uint16 wTotalLength;
    1,                                      // uint8  bNumInterfaces;
    1,                                      // uint8  bConfigurationValue;
    0,                                      // uint8  iConfiguration;
    0xC0,                                   // uint8  bmAttributes; self powered
    0xFA                                    // uint8  bMaxPower; 500 ma
  },
  {
    sizeof(EFI_USB_INTERFACE_DESCRIPTOR),    // uint8  bLength;
    USB_DESC_TYPE_INTERFACE,                 // uint8  bDescriptorType;
    0,                                       // uint8  bInterfaceNumber;
    0,                                       // uint8  bAlternateSetting;
    1,                                       // uint8  bNumEndpoints;
    0x03,                                    // uint8  bInterfaceClass;
    0x01,                                    // uint8  bInterfaceSubClass;
    0x02,                                    // uint8  bInterfaceProtocol;
    0                                        // uint8  iInterface;
  },
  {
    sizeof(EFI_USB_HID_DESCRIPTOR),          // bLength
    USB_DESC_TYPE_HID,                       // bDescriptorType
    0x111,                                   // bcdHID
    0x00,                                    // bCountryCode
    0x01,                                    // bNumDescriptors
    { {0x22, USBFN_CHG_DEV_REPORT_DESC_SIZE} } // bDescriptorType, wDescriptorLength
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR), // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,              // uint8  bDescriptorType;
    USB_INTR_EP_IN,                      // uint8  bEndpointAddress;
    USB_ENDPOINT_INTERRUPT,              // uint8  bmAttributes;
    0x04,                                // uint16 wMaxPacketSize;
    0x0A                                 // uint8  bInterval;
  },
};


STATIC
struct _SSCfgDescTree {
  EFI_USB_CONFIG_DESCRIPTOR    ConfigDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
  EFI_USB_HID_DESCRIPTOR       HidDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR  EndpointDescriptor0;
  EFI_USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR EndpointCompanionDescriptor0;
} TotalSSConfigDescriptor = {
  {
    sizeof(EFI_USB_CONFIG_DESCRIPTOR),      // uint8  bLength;               0x09
    USB_DESC_TYPE_CONFIG,                   // uint8  bDescriptorType;       0x02
    sizeof(TotalSSConfigDescriptor),        // uint16 wTotalLength;
    1,                                      // uint8  bNumInterfaces;
    1,                                      // uint8  bConfigurationValue;
    0,                                      // uint8  iConfiguration;
    0xC0,                                   // uint8  bmAttributes;
    0x10                                    // uint8  bMaxPower;
  },
  {
    sizeof(EFI_USB_INTERFACE_DESCRIPTOR),    // uint8  bLength;
    USB_DESC_TYPE_INTERFACE,                 // uint8  bDescriptorType;
    0,                                       // uint8  bInterfaceNumber;
    0,                                       // uint8  bAlternateSetting;
    1,                                       // uint8  bNumEndpoints;
    0x03,                                    // uint8  bInterfaceClass;
    0x01,                                    // uint8  bInterfaceSubClass;
    0x02,                                    // uint8  bInterfaceProtocol;
    0                                        // uint8  iInterface;
  },
  {
    sizeof(EFI_USB_HID_DESCRIPTOR),          // bLength
    USB_DESC_TYPE_HID,                       // bDescriptorType
    0x111,                                   // bcdHID
    0x00,                                    // bCountryCode
    0x01,                                    // bNumDescriptors
    { {0x22, USBFN_CHG_DEV_REPORT_DESC_SIZE} } // bDescriptorType, wDescriptorLength
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR), // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,              // uint8  bDescriptorType;
    USB_INTR_EP_IN,                      // uint8  bEndpointAddress;
    USB_ENDPOINT_INTERRUPT,              // uint8  bmAttributes;
    0x04,                                // uint16 wMaxPacketSize;
    0x0A                                 // uint8  bInterval;
  },
  {
    sizeof(EFI_USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR), // uint8 bLength
    USB_DESC_TYPE_SS_ENDPOINT_COMPANION,                      // uint8 bDescriptorType
    0,                                                        // uint8 bMaxBurst,    0 => max burst 1
    0,                                                        // uint8 bmAttributes, 0 => Reserved
    0x0A,                                                     // uint8 wBytesPerInterval.
  }
};


/* Lang String Descriptor */
CONST
UINT8
Str0Descriptor[4] = {
  sizeof(Str0Descriptor),USB_DESC_TYPE_STRING,
  0x09,0x04 // Langid : US_EN.
};


/* Manufacture String Descriptor */
CONST
UINT8
StrManufacturerDescriptor[18] = {
  sizeof(StrManufacturerDescriptor),USB_DESC_TYPE_STRING,
  'Q',0,
  'u',0,
  'a',0,
  'l',0,
  'c',0,
  'o',0,
  'm',0,
  'm',0,
};


/* Product String Descriptor */
CONST
UINT8
StrProductDescriptor[18] = {
  sizeof(StrProductDescriptor),USB_DESC_TYPE_STRING,
  'M',0,
  'S',0,
  'M',0,
  'P',0,
  'H',0,
  'O',0,
  'N',0,
  'E',0,
};


/* String Descriptor Array consumed by USB Device Protocol */
EFI_USB_STRING_DESCRIPTOR *StrDescriptors[USBFN_CHG_DEV_NUM_STR_DESC]=
{
   (EFI_USB_STRING_DESCRIPTOR*) Str0Descriptor,
   (EFI_USB_STRING_DESCRIPTOR*) StrManufacturerDescriptor,
   (EFI_USB_STRING_DESCRIPTOR*) StrProductDescriptor
};


/* see header for documentation */
EFI_STATUS
BuildDefaultDescriptors(
  OUT USB_DEVICE_DESCRIPTOR  **DevDesc,
  OUT VOID                  ***Descriptors,
  OUT USB_DEVICE_DESCRIPTOR  **SSDevDesc,
  OUT VOID                  ***SSDescriptors)
{
  EFI_STATUS                   Status  = EFI_SUCCESS ;  
  struct _CfgDescTree        **Array   = NULL;
  struct _SSCfgDescTree      **SSArray = NULL;
  UINT8                        NumCfg  = 0; 

  if (!DevDesc || !Descriptors) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  *DevDesc = &DeviceDescriptor;
  NumCfg   = DeviceDescriptor.NumConfigurations;
  Array = AllocateZeroPool(NumCfg * sizeof (struct _CfgDescTree *));
  if (!Array) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  Array[0] = &TotalConfigDescriptor;
  *Descriptors = (VOID **) Array;
  
  *SSDevDesc = &SSDeviceDescriptor;
  NumCfg = SSDeviceDescriptor.NumConfigurations;
  SSArray = AllocateZeroPool(NumCfg * sizeof (struct _SSCfgDescTree *));
  if (!SSArray) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  SSArray[0] = &TotalSSConfigDescriptor;
  *SSDescriptors = (VOID **) SSArray;
  

ON_EXIT:
  return Status;
}


/* see header for documentation */
VOID
FreeDefaultDescriptors(
  IN USB_DEVICE_DESCRIPTOR  *DevDesc,
  IN VOID                  **Descriptors,
  IN VOID                  **SSDescriptors)
{
  if (!DevDesc || !Descriptors || SSDescriptors) {
    goto ON_EXIT;
  }

  FreePool (Descriptors);
  FreePool (SSDescriptors);
  
ON_EXIT:
  return;
}
