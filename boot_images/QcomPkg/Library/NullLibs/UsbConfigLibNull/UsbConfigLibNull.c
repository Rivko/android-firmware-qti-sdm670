



/** @file

  QCOM_USB_CONFIG_PROTOCOL 8996 implementation.

  Copyright (c) 2011 - 2015, QUALCOMM Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/NullLibs/UsbConfigLibNull/UsbConfigLibNull.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 05/08/15   plc      Initial Revision
=============================================================================*/

//
// Includes
//
#include "UsbConfigPrivate.h"

// 
// Functions
//
/**
  This routine toggles the USB mode configuration on the core specified by CoreNum  

  @param  CoreNum             USB core number  

  @retval EFI_SUCCESS         Operation succeeded. 
  @retval Others              Operation failed.

**/
EFI_STATUS
EFIAPI UsbToggleControllerMode(
  IN   QCOM_USB_CORE_NUM          CoreNum
  )
{
  return EFI_SUCCESS;
}

/**
  Placeholder function

  @param  ConfigType             Configuration type.

  @retval EFI_SUCCESS            Success
  @retval EFI_UNSUPPORTED        Unsupported ConfigType.

**/
EFI_STATUS
EFIAPI
ExitUsbLibServices (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  UsbConfigInit does HW/driver initialization that's not dependent on other driver requests.

  @retval EFI_SUCCESS            Success.
  @retval EFI_DEVICE_ERROR       Failure

**/
EFI_STATUS
EFIAPI
UsbConfigInit (
  VOID
  )
{
  return EFI_SUCCESS;
}


/**
  Returns maximum number of cores supported in the mode

  @param  Mode                   USB mode
  @param  MaxCoreNum             Number of cores supported

  @retval EFI_SUCCESS            Success
  @retval EFI_UNSUPPORTED        Unsupported ConfigType.

**/
EFI_STATUS
EFIAPI
GetUsbMaxHostCoreNum(
  IN  QCOM_USB_MODE_TYPE  Mode,
  OUT UINT8               *MaxCoreNum
  ) 
{
  return EFI_UNSUPPORTED;
}

