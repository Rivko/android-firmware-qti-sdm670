/**
@file  UsbPwrCtrlLibConfig.h
@brief Provide UsbPwrCtrlLibConfig Related definition
*/
/*=============================================================================
Copyright (c) 2017 Qualcomm Technologies, Incorporated.
All rights reserved.
Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/13/17   dc      Initial Release
=============================================================================*/
#ifndef __USBPWRCTRLLIB_CONFIG_H__
#define __USBPWRCTRLLIB_CONFIG_H__

#include <Protocol/EFIPlatformInfo.h>

/**
  Initialize UsbPwrCtrl Hardware based input Platform Type

  @param[In] PlatformType       Platfrom Information to initialize the library. 

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibConfig_Init(EFI_PLATFORMINFO_PLATFORM_TYPE PlatformType);


/**
  Return UsbPwrCtrl Active Hardware Information

  @param[In] PlatformType       Hardware Information to initialize the library. 
  @param[Out] pHwInfo           Pointer where HwInfo is saved

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function 
  EFI_NOT_READY:                The physical device is not ready for this request 
*/

EFI_STATUS
UsbPwrCtrlLibConfig_GetHWInfo(
//  EFI_USB_PWR_CTRL_HW_INFO *pHwInfo);
  EFI_USB_PWR_CTRL_HW_INFO **pHwInfo);


#endif
