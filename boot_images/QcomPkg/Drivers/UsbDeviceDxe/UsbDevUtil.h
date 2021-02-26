/**
* @file UsbfnDevUtil.h
*
* @brief Utility macros and functions.
*
* @copyright Copyright (C) 2014-2015, 2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
*/
/*==============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/01/17   pm      Port form Boot.xf.1.4
11/06/15   ck      Add USB SS and USBCV test support
08/21/14   ck      Initial verion
=============================================================================*/
#ifndef _USB_DEV_UTIL_H_
#define _USB_DEV_UTIL_H_

#include <Library/DebugLib.h>

#define _DBG(EfiD, Expression, ...)	DEBUG((EfiD, Expression, ##__VA_ARGS__))
#define DBG(EfiD, Str, ...) \
  _DBG(EfiD, "%a: " Str "\n", __FUNCTION__, ##__VA_ARGS__)

// Major version of USBBcd
#define USB_BCD_MAJOR_VER(BCD_VER) ((BCD_VER)>>8)

// For debug stack info.
#define FNC_ENTER_MSG() DBG(EFI_D_VERBOSE, "++")
#define FNC_LEAVE_MSG() DBG(EFI_D_VERBOSE, "--")


#endif  //_USB_DEV_UTIL_H_
