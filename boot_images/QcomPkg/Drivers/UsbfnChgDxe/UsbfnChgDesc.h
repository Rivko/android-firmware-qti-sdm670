/**
 * @file UsbfnChgDesc.h
 *
 * Header of UsbfnChgDesc.c
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/23/16   ck      Init version
=============================================================================*/

#ifndef _USBFN_CHG_DESC_H_
#define _USBFN_CHG_DESC_H_

#include "UsbfnChgDxe.h"
#include <Protocol/EFIUsbfnIo.h>

#define USBFN_CHG_DEV_NUM_STR_DESC     3
#define USBFN_CHG_DEV_REPORT_DESC_SIZE 52

extern EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR       DeviceQualifier;
extern UINT8                                     ReportDescriptor[USBFN_CHG_DEV_REPORT_DESC_SIZE];
extern EFI_USB_STRING_DESCRIPTOR                *StrDescriptors[USBFN_CHG_DEV_NUM_STR_DESC];


/**
* @brief Build descriptors in the format which is consumed by USB Device Protocol 
*
* Based on the descriptors defined in UsbfnChgDesc.c, construct the descriptor struct
* which is used by the USB Device Protocol. 
* Caller is responsible to call FreeDefaultDescriptors to free any resources allocated
* in the function. 
* 
*
* @param  DevDesc                 A pointer to a point of USB_DEVICE_DESCRIPTOR
* @param  Descriptors             A pointer to a array of USB configuration array
* @
*/
EFI_STATUS
BuildDefaultDescriptors(
  OUT USB_DEVICE_DESCRIPTOR  **DevDesc,
  OUT VOID                  ***Descriptors,
  OUT USB_DEVICE_DESCRIPTOR  **SSDevDesc,
  OUT VOID                  ***SSDescriptors);

/**
* @brief Free the resources allocated in BuildDefaultDescriptors 
*
* @param  DevDesc                 A pointer to the USB_DEVICE_DESCRIPTOR passed in BuildDefaultDescriptors
* @param  Descriptors             The array of USB configuration array passed in BuildDefaultDescriptors
*/
VOID
FreeDefaultDescriptors(
  IN USB_DEVICE_DESCRIPTOR  *DevDesc,
  IN VOID                  **Descriptors,
  IN VOID                  **SSDescriptors);

#endif /* _USBFN_CHG_DESC_H_ */
