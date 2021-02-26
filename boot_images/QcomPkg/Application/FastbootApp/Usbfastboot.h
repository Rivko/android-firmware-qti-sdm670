/**
 * @file Usbfastboot.h
 *
 * USB descriptor helper defines
 *
 * Defines various USB related constants, data transfer structures/functions,
 * and an application level printing abstraction.
 *
 * Copyright (c) 2014,2016 Qualcomm Technologies, Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/25/16   ck      Remove dependency of Usbfn protocol
 08/15/14   ml      Clean up
 08/15/14   ml      Initial revision

=============================================================================*/

#ifndef _USBFASTBOOT_H_
#define _USBFASTBOOT_H_


#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <IndustryStandard/Usb.h>
#include <Protocol/UsbIo.h>
#include <Protocol/EFIUsbEx.h>

#define _DBG(EfiD, Expression, ...)	DEBUG((EfiD, Expression, ##__VA_ARGS__))
#define DBG(EfiD, Str, ...) \
  _DBG(EfiD, "%a: " Str "\n", __FUNCTION__, ##__VA_ARGS__)

/* USB endpoint number for bulk data transfers (both IN/OUT used) */ 
#define USBLB_BULK_EP                 1

/* Converts endpoint index and direction to address. */
#define ENDPOINT_ADDR(EndpointIndex, Tx) \
  ((EndpointIndex) | ((Tx) ? 0x80 : 0x00))

/* Other USB related constants/macros */
#define USB_MAX_ENDPOINT_NUMBER       15

#define USB_SETUP_RT_TYPE_SHFT        5
#define USB_SETUP_RT_TYPE_BMSK        (0x3 << USB_SETUP_RT_TYPE_SHFT)
#define USB_SETUP_RT_TYPE(bmRT)       (((bmRT) & USB_SETUP_RT_TYPE_BMSK) >> USB_SETUP_RT_TYPE_SHFT)

#define USB_SETUP_RT_TYPE_CLS         1
#define USB_SETUP_RT_TYPE_VND         2 
#define USB_SETUP_RT_TYPE_STD         0 

#define USB_SETUP_RT_RCP_SHFT         0
#define USB_SETUP_RT_RCP_BMSK         (0x1f << USB_SETUP_RT_RCP_SHFT)
#define USB_SETUP_RT_RCP(bmRT)        (((bmRT) & USB_SETUP_RT_RCP_BMSK) >> USB_SETUP_RT_RCP_SHFT)
#define USB_SETUP_RT_RCP_DEV          0
#define USB_SETUP_RT_RCP_IFC          1
#define USB_SETUP_RT_RCP_EP           2
#define USB_SETUP_RT_RCP_OTH          3

extern VOID *gTxBuffer;
extern VOID *gRxBuffer;

#endif /* USBfastboot.h */
