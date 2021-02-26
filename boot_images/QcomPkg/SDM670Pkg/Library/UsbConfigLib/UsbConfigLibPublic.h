/** @file \UsbConfigPublic.h
 *
 * The Public Header for UsbConfigLib which defines the command data structure 
 * and lib shared between USB Core and USB Port modules.
 * Copyright (c) 2016-2017 QUALCOMM Technologies Inc. All rights reserved.
 */

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 03/17/17   pm       Add secondary core
 02/06/17   pm       Add Clamshell Support
 02/23/16   ck       Init Check-In
=============================================================================*/

#ifndef _USB_CONFIG_PUB_H_
#define _USB_CONFIG_PUB_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/EFIPmicUsb.h>
#include <Protocol/EFIUsbConfig.h>
#include "UsbError.h"
#include <Library/UsbConfigLib.h>
#include <Library/PcdLib.h>
#include <Protocol/EFIPlatformInfo.h>


extern EFI_QCOM_PMIC_USB_PROTOCOL            *gPmicUsbProtocol;
extern EFI_GUID                               gQcomTokenSpaceGuid; // Used for getting the BCD variable "UsbUseSynopsys"
/**
USB core enumeration
*/
typedef enum {
  USB_CORE_0_SDM_670 = 0,      /**< Core0. */
  USB_CORE_MAX_NUM_SDM_670,    /**< Invalid Core. */
} QCOM_USB_CORE_NUM_SDM_670;

//! Macro to Indentify Clamshell Platform
#define IS_CLS_PLAT(PLAT) (PLAT == EFI_PLATFORMINFO_TYPE_CLS)

#endif /* _USB_CONFIG_PUB_H_ */

