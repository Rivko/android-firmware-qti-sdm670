/**
* @file UsbDevDesc.h
*
* UsbDevDesc Header
*
* Copyright (c) 2014-2016 Qualcomm Technologies Inc. All rights reserved.
*/
/*=============================================================================
EDIT HISTORY

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/25/16   ck      Separate EDK2 undefined USB data structure to EFIUsbEx.h
12/01/15   ck      Add USB SS and USBCV test support
11/23/15   ck      Include EFIUsbDevice.h
08/21/14   ck      Initial revision
=============================================================================*/

#ifndef _USB_DEV_DESC_H_
#define _USB_DEV_DESC_H_

#include <Protocol/EFIUsbfnIo.h>
#include <Protocol/EFIUsbEx.h>
#include <Protocol/EFIUsbDevice.h>

/* The set of USB descriptors and associated data struture **/
typedef struct _USB_DESCRIPTOR_SET {  
  EFI_USB_DEVICE_DESCRIPTOR               DeviceDescriptor;
  EFI_USB_DEVICE_DESCRIPTOR               SSDeviceDescriptor;
  EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR     DeviceQualifierDescriptor;
  VOID                                  **Descriptors;
  VOID                                  **SSDescriptors;
  VOID                                   *BinaryDeviceOjectStore;
  UINT8                                   StrDescCount;
  EFI_USB_STRING_DESCRIPTOR             **StrDescriptors;
} USB_DESCRIPTOR_SET;


/*
* @brief Initialize a local copy of the descriptors from the client
*
* Allocate memory space to cache a local copy of the descriptors from the client.
* The caller needs to call UsbDevDescFreeDescriptors to free the allocated 
* memory space.
*
* @see UsbDevDescFreeDescriptors
*
* @param[in] DeviceDescriptor           : see EFIUsbDevice.h
* @param[in] Descriptors                : see EFIUsbDevice.h
* @param[in] DeviceQualifierDescriptor  : see EFIUsbDevice.h
* @param[in] BinaryDeviceOjectStore     : see EFIUsbDevice.h
*  (optional) Defaults to NULL.
* @param[in] StringDescriptorCount      : see EFIUsbDevice.h
* @param[in] StringDescritors           : see EFIUsbDevice.h
*  (optional) Default to NULL. If not NULL, StringDescriptorCount must be greater
*             than zero.
*
* @param[out] DeviceInfo                : the Device Info to be used with UsbFnIo Protocol
* @param[out] DescriptorSet             : the local copy of descriptors used within the Usb Device protocol
* 
* @retval  EFI_SUCCESS                  : The local copy of the descriptors are cached successfully.
* @retval  Others                       : The operation failed.
*/
EFI_STATUS
UsbDevDescInitDescriptors(
  IN  USB_DEVICE_DESCRIPTOR              *DeviceDescriptor,
  IN  VOID                              **Descriptors,
  IN  USB_DEVICE_QUALIFIER_DESCRIPTOR    *DeviceQualifierDescriptor,
  IN  VOID                               *BinaryDeviceOjectStore, OPTIONAL
  IN  UINT8                               StringDescriptorCount,
  IN  USB_STRING_DESCRIPTOR             **StringDescritors, OPTIONAL
  OUT EFI_USB_DEVICE_INFO                *DeviceInfo,
  OUT USB_DESCRIPTOR_SET                 *DescriptorSet
  );


/*
* @brief Initialize a local copy of the descriptors from the client
*
* Allocate memory space to cache a local copy of the descriptors from the client.
* The caller needs to call UsbDevDescFreeDescriptors to free the allocated
* memory space. The function extends the functionality of UsbDevDescInitDescriptors
* to support SuperSpeed descriptors.
*
* @see UsbDevDescFreeDescriptors
*
* @param[in]  UsbDevDescSet             : The USB descriptor set from the client
* @param[out] DeviceInfo                : The Device Info to be used with UsbFnIo Protocol
* @param[out] SSDeviceInfo              : The SuperSpeed Device Info to be used with UsbFnIo Protocol
* @param[out] DescriptorSet             : The local copy of the descriptors.
*
* @retval  EFI_SUCCESS                  : The local copy of the descriptors are cached successfully.
* @retval  Others                       : The operation failed.
*/
EFI_STATUS
UsbDevDescInitDescriptorsEx(
  IN  USB_DEVICE_DESCRIPTOR_SET          *UsbDevDescSet,
  OUT EFI_USB_DEVICE_INFO                *DeviceInfo,
  OUT EFI_USB_SUPERSPEED_DEVICE_INFO     *SSDeviceInfo,
  OUT USB_DESCRIPTOR_SET                 *DescriptorSet
);


/*
* @brief Free the memory buffers allocated for the cached descriptors
*        in DescriptorSet
*
* @see UsbDevDescInitDescriptors
*
* @param[in] DescriptorSet             : the local copy of descriptors
*
* @retval  EFI_SUCCESS                 : Operation succeeded.
* @retval  Others                      : Operation failed.
*/
EFI_STATUS
UsbDevDescFreeDescriptors(
  IN USB_DESCRIPTOR_SET                       *DescriptorSet
);


/*
* @brief Free the memory buffers allocated for the DeviceInfo
*
* @see DevDescInitDescriptors
*
* @param[in] DeviceInfo             : EFI_USB_DEVICE_INFO to be freed 
*
* @retval     EFI_SUCCESS                Operation succeeded.
* @retval     Others                     Operation failed
*/
EFI_STATUS
UsbDevDescFreeDevInfo(
  IN    EFI_USB_DEVICE_INFO     *DeviceInfo);

#endif //_USB_DEV_DESC_H_
