/**
 * @file UsbfnDescApp.c
 *
 * Defines the fastboot USB descriptor structures.
 *
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/13/16   jt      Change SdccCardInfo to MemCardInfo 
 01/25/16   ck      Remove dependency of Usbfn protocol
 02/03/15   bh      Fixed KW errors
 01/14/15   bh      Fixed LLVM warning
 09/12/14   bh      Made Fastboot standalone app 
 08/15/14   ml      Clean up
 08/14/14   ml      Initial revision
=============================================================================*/


#include "UsbDescriptors.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/EFICardInfo.h>
#include <Library/PrintLib.h>

#define FAST_BOOT_VENDOR      0x18d1
#define FAST_BOOT_IDPRODUCT   0xD00D
//TODO: Full UUID descriptor should be of length 74, investigate 
//      why it only works up to 62
#define MAX_DESC_LEN          62
#define UUID_STR_LEN          36

STATIC MEM_CARD_INFO          CardInfoData;

STATIC
EFI_USB_DEVICE_DESCRIPTOR
DeviceDescriptor = {
  sizeof(EFI_USB_DEVICE_DESCRIPTOR),  // uint8  bLength;
  USB_DESC_TYPE_DEVICE,               // uint8  bDescriptorType;
  0x0200,                             // uint16 bcdUSB;
  0x00,                               // uint8  bDeviceClass;
  0x00,                               // uint8  bDeviceSubClass;
  0x00,                               // uint8  bDeviceProtocol;
  64,                                 // uint8  bMaxPacketSize0;
  FAST_BOOT_VENDOR,                   // uint16 idVendor;
  FAST_BOOT_IDPRODUCT,                // uint16 idProduct;
  0x100,                              // uint16 bcdDevice;
  1,                                  // uint8  iManufacturer;
  2,                                  // uint8  iProduct;
  3,                                  // uint8  iSerialNumber;
  1                                   // uint8  bNumConfigurations;
};


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
struct _CfgDescTree {
  EFI_USB_CONFIG_DESCRIPTOR    ConfigDescriptor;
  EFI_USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
  EFI_USB_ENDPOINT_DESCRIPTOR  EndpointDescriptor0;
  EFI_USB_ENDPOINT_DESCRIPTOR  EndpointDescriptor1;
} TotalConfigDescriptor = {
  {
    sizeof(EFI_USB_CONFIG_DESCRIPTOR),      // uint8  bLength;
    USB_DESC_TYPE_CONFIG,                   // uint8  bDescriptorType;
      sizeof(EFI_USB_CONFIG_DESCRIPTOR)
    + sizeof (EFI_USB_INTERFACE_DESCRIPTOR)
    + sizeof (EFI_USB_ENDPOINT_DESCRIPTOR)
    + sizeof (EFI_USB_ENDPOINT_DESCRIPTOR), // uint16 wTotalLength;
    1,                                      // uint8  bNumInterfaces;
    1,                                      // uint8  bConfigurationValue;
    0,                                      // uint8  iConfiguration;
    0x80,                                   // uint8  bmAttributes;
    0x50                                      // uint8  bMaxPower;
  },
  {
    sizeof(EFI_USB_INTERFACE_DESCRIPTOR), // uint8  bLength;
    USB_DESC_TYPE_INTERFACE,              // uint8  bDescriptorType;
    0,                                    // uint8  bInterfaceNumber;
    0,                                    // uint8  bAlternateSetting;
    2,                                    // uint8  bNumEndpoints;
    0xff,                                 // uint8  bInterfaceClass;
    0x42,                                 // uint8  bInterfaceSubClass;
    0x03,                                 // uint8  bInterfaceProtocol;
    4 
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR), // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,              // uint8  bDescriptorType;
    ENDPOINT_ADDR(USBLB_BULK_EP, TRUE),  // uint8  bEndpointAddress;
    USB_ENDPOINT_BULK,                   // uint8  bmAttributes;
    512,                                 // uint16 wMaxPacketSize; HS=512 , FS=64
    0                                    // uint8  bInterval;
  },
  {
    sizeof(EFI_USB_ENDPOINT_DESCRIPTOR), // uint8  bLength;
    USB_DESC_TYPE_ENDPOINT,              // uint8  bDescriptorType;
    ENDPOINT_ADDR(USBLB_BULK_EP, FALSE), // uint8  bEndpointAddress;
    USB_ENDPOINT_BULK,                   // uint8  bmAttributes;
    512,                                 // uint16 wMaxPacketSize; HS=512 , FS=64
    1                                    // uint8  bInterval;
  },
};

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
  'u',0,
  'a',0,
  'l',0,
  'c',0,
  'o',0,
  'm',0,
  'm',0,
};

UINT8
StrSerialDescriptor[MAX_DESC_LEN] ;/*= {
  sizeof(StrSerialDescriptor),USB_DESC_TYPE_STRING,
  '9',0,
  'a',0,
  '1',0,
  '8',0,
  '9',0,// c
  '9',0,// 4
  '1',0,//system table
};
*/

CONST
UINT8
StrInterfaceDescriptor[18] = {
  sizeof(StrInterfaceDescriptor),USB_DESC_TYPE_STRING,
  'f',0,
  'a',0,
  's',0,
  't',0,
  'b',0,
  'o',0,
  'o',0,
  't',0,
};


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

EFI_USB_STRING_DESCRIPTOR *StrDescriptors[5]= 
{
   (EFI_USB_STRING_DESCRIPTOR*) Str0Descriptor,
   (EFI_USB_STRING_DESCRIPTOR*) StrManufacturerDescriptor,
   (EFI_USB_STRING_DESCRIPTOR*) StrProductDescriptor,
   (EFI_USB_STRING_DESCRIPTOR*) StrSerialDescriptor,
   (EFI_USB_STRING_DESCRIPTOR*) StrInterfaceDescriptor
};

VOID
BuildDefaultDescriptors(
  OUT USB_DEVICE_DESCRIPTOR  **DevDesc,
  OUT VOID                  ***Descriptors)
{
  EFI_STATUS                   Status = EFI_INVALID_PARAMETER ;
  EFI_BLOCK_IO_PROTOCOL       *BlkIo;
  EFI_HANDLE                  *SdccHandles;
  UINTN                        NumberOfSdccHandles;
  UINTN                        i;
  EFI_MEM_CARDINFO_PROTOCOL  *CardInfo;
  UINT8                        Index = 0;
  UINT8                        NumCfg = 0; 
  UINT8                        Raw_UUID[16];
  CHAR8                        Str_UUID[UUID_STR_LEN];
  CHAR8                        Final_UUID[UUID_STR_LEN*2+2];
  struct _CfgDescTree        **Array = NULL;

  Status = gBS->LocateHandleBuffer (ByProtocol,
                                    &gEfiBlockIoProtocolGuid,
                                    NULL,
                                    &NumberOfSdccHandles,
                                    &SdccHandles);
  if(Status != EFI_SUCCESS) 
  {
    DEBUG((EFI_D_ERROR,"Failed to get BlkIo handles\n"));
    return;
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
      CopyMem(&Raw_UUID[0], &CardInfoData.mfr_id, 16);
      break;
    }
  }
  
  AsciiSPrint(&Str_UUID[0], sizeof(Str_UUID), "%g", &Raw_UUID[0]);
  Final_UUID[0] = MAX_DESC_LEN;
  Final_UUID[1] = USB_DESC_TYPE_STRING;
  for (i = 0; i < sizeof(Str_UUID); i++) {
     Final_UUID[i*2+2] = Str_UUID[i];
     Final_UUID[i*2+3] = 0;
  }

  CopyMem((VOID *) &StrSerialDescriptor, &Final_UUID, sizeof(StrSerialDescriptor));

  *DevDesc = &DeviceDescriptor;
  NumCfg   = DeviceDescriptor.NumConfigurations;

  Array = AllocatePool(NumCfg * sizeof (struct _CfgDescTree *));
  ASSERT(Array != NULL);
  if (Array == NULL)
    return;
  ASSERT(NumCfg == 1); // Testing Support Only One Cfg at this time.

  for (Index = 0; Index < NumCfg; Index++)
  {
    Array[Index] = &TotalConfigDescriptor;
  }

  *Descriptors = (VOID **) Array;
}

