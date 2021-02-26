/**
 * @file UsbfnDesc.h
 *
 * Declares the loopback USB descriptor structures.
 *
 * Copyright (c) 2014, QUALCOMM Technologies Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/20/14   ml      Migrate to official SuperSpeed support
 08/21/14   ck      move device qualifier descriptor to Usb.h and minor fixes
 01/28/14   ml      Initial revision

=============================================================================*/

#ifndef _USBFN_DESC_H_
#define _USBFN_DESC_H_


#include "UsbfnLoopback.h"

// USB Loopback device/config/interface/endpoint descriptor set
extern CONST EFI_USB_DEVICE_INFO DeviceInfo;
extern CONST EFI_USB_SUPERSPEED_DEVICE_INFO SSDeviceInfo;
extern CONST EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR DeviceQualifier;


// String descriptors
extern CONST UINT8 Str0Descriptor[4];
extern CONST UINT8 StrManufacturerDescriptor[18];
extern CONST UINT8 StrProductDescriptor[32];


#endif /* _USBFN_DESC_H_ */
