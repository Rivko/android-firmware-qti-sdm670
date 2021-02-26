/**
 * @file UsbfnDescApp.h
 *
 * Declares the Fastboot USB descriptor structures.
 *
 * Copyright (c) 2014,2016 Qualcomm Technologies, Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/25/16   ck      Remove dependency of Usbfn protocol
 09/12/14   bs      Made Fastboot standalone app 
 08/15/14   ml      Clean up
 08/15/14   ml      Initial revision

=============================================================================*/

#ifndef _USBFN_DESCAPP_H_
#define _USBFN_DESCAPP_H_

#include "Usbfastboot.h"

/* Fastboot device/config/interface/endpoint descriptor set */
extern EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR       DeviceQualifier;

/* Fastboot String descriptors */
extern EFI_USB_STRING_DESCRIPTOR *StrDescriptors[5];

VOID
BuildDefaultDescriptors(OUT USB_DEVICE_DESCRIPTOR **DevDesc, OUT VOID ***Descriptors);
#endif /* _USBFN_DESCAPP_H_ */
