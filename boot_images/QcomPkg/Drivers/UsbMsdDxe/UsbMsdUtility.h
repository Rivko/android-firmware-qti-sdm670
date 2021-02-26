/**
* @file UsbMsdUtility.h
*
* @brief Implementation of helper macros
*
* This file defines helper macros used through the module 
*
* Copyright (c) 2014, 2017 by Qualcomm Technologies, Inc. All Rights Reserved.
*/
/*==============================================================================
EDIT HISTORY

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/01/17   pm      Port form Boot.xf.1.4
10/14/14   ck      Add history. Introduce state machine and remove global vars
=============================================================================*/

#ifndef __USB_MSD_UTILITY_H__
#define __USB_MSD_UTILITY_H__

#include <Library/DebugLib.h> // DEBUG macro

//
// Miscellaneous Definitions and helper MACROs
//

// Terminate output with a newline, and eliminate the need for double parens.
#define _DBG(EfiD, Expression, ...)	DEBUG((EfiD, Expression, ##__VA_ARGS__))
#define DBG(EfiD, Str, ...) \
  _DBG (EfiD, "%a: " Str "\n", __FUNCTION__, ##__VA_ARGS__);

// For debug stack info.
#define FNC_ENTER_MSG() DBG (EFI_D_INFO, "++")
#define FNC_LEAVE_MSG() DBG (EFI_D_INFO, "--")

// Converts endpoint index and direction to address.
#define ENDPOINT_ADDR(EndpointIndex, Tx) \
  ((EndpointIndex) | ((Tx) ? 0x80 : 0x00))

// Get ep direction from USB request (see USB 3.0 ch 9.3)
#define USB_REQUEST_EPDIR(index)      (((index) >> 7 & 0x1) ? EfiUsbEndpointDirectionHostIn : \
  EfiUsbEndpointDirectionHostOut)

// Get ep and dir from wIndex field of USB request (see USB 3.0 ch 9.3.4)
#define USB_INDEX_TO_EP(index)        ((index) & 0xf)
#define USB_INDEX_TO_EPDIR(index)     (((index) >> 7 & 0x1) ? EfiUsbEndpointDirectionHostIn : \
  EfiUsbEndpointDirectionHostOut)

// Get descriptor type and index from wValue field of USB request (see USB 3.0 ch 9.4.3)
#define USB_VALUE_TO_DESC_TYPE(value) ((value) >> 8)
#define USB_VALUE_TO_DESC_IDX(value)  ((value) & 0xff)

// Other USB related constants/macros
#define USB_MAX_ENDPOINT_NUMBER       15

#define USB_SETUP_RT_TYPE_SHFT        5
#define USB_SETUP_RT_TYPE_BMSK        (0x3 << USB_SETUP_RT_TYPE_SHFT)
#define USB_SETUP_RT_TYPE(bmRT)       (((bmRT) & USB_SETUP_RT_TYPE_BMSK) >> USB_SETUP_RT_TYPE_SHFT)
#define USB_SETUP_RT_TYPE_STD         0
#define USB_SETUP_RT_TYPE_CLS         1
#define USB_SETUP_RT_TYPE_VND         2

#define USB_SETUP_RT_RCP_SHFT         0
#define USB_SETUP_RT_RCP_BMSK         (0x1f << USB_SETUP_RT_RCP_SHFT)
#define USB_SETUP_RT_RCP(bmRT)        (((bmRT) & USB_SETUP_RT_RCP_BMSK) >> USB_SETUP_RT_RCP_SHFT)
#define USB_SETUP_RT_RCP_DEV          0
#define USB_SETUP_RT_RCP_IFC          1
#define USB_SETUP_RT_RCP_EP           2
#define USB_SETUP_RT_RCP_OTH          3

#define ERR_CHK(ErrStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_ERROR, ErrStr, ##__VA_ARGS__); \
      goto ON_EXIT; \
    } \
  } while (0)

// Highly undesirable, but continue anyway.
#define WRN_CHK(WrnStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_WARN, WrnStr, ##__VA_ARGS__); \
    } \
  } while (0)

// Check parameters.
#define ARG_CHK(Expr) \
  do { \
    if (!(Expr)) { \
      Status = EFI_INVALID_PARAMETER; \
      ERR_CHK ("invalid parameter"); \
    } \
  } while (0)

// Check Memory Allocation
#define MCR_CHK(Expr) \
  do { \
    if (!(Expr)) { \
    Status = EFI_OUT_OF_RESOURCES; \
    ERR_CHK("Mem allocation for " #Expr " failed"); \
    } \
  } while (0)

#endif /* __USB_MSD_UTILITY_H__ */
