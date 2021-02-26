/**
@file  UsbPwrCtrl.c
@brief USB Power Control Module related Declaration
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

/*===========================================================================*/
/*                  INCLUDE FILES                                            */
/*===========================================================================*/
#include <Uefi.h>

/**
  Library Dependencies
*/
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UsbPwrCtrlLib.h>


/**
  Protocol Dependencies
*/
#include <Protocol/EFIUsbPwrCtrl.h>

/*===========================================================================*/
/*                  LOCAL VARIABLE DECLARATIONS                              */
/*===========================================================================*/
EFI_USB_PWR_CTRL_PROTOCOL UsbPwrCtrlProtocolImplementation;

/*===========================================================================*/
/*                  FUNCTION DEFINITIONS                                     */
/*===========================================================================*/
/**
  Initialize UsbPwrCtrl Module

  @param none

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS UsbPwrCtrlInit(
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status = EFI_SUCCESS;
  Status = UsbPwrCtrlLib_Init();

  if(EFI_SUCCESS == Status)
  {
    Status = gBS->InstallMultipleProtocolInterfaces( &ImageHandle,
                                                     &gQcomUsbPwrCtrlProtocolGuid,
                                                     &UsbPwrCtrlProtocolImplementation,
                                                     NULL );
  }
  return Status;
}

/**
EFI_UsbPwrCtrl_GetHwInfo()

@brief
Get Hardware Information
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_GetHwInfo
(
  OUT EFI_USB_PWR_CTRL_HW_INFO *pHwInfo
)
{
  return EFI_SUCCESS;
}

/**
EFI_UsbPwrCtrl_GetVbusDetectStatus()

@brief
Get Vbus Detect Status
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_GetVbusDetectStatus
(
  IN  UINT8   PortIndex,
  OUT BOOLEAN *bVbusDetect
)
{
  return UsbPwrCtrlLib_GetVbusDetect(PortIndex, bVbusDetect);
}

/**
EFI_UsbPwrCtrl_GetVbusSrcOkStatus()

@brief
Get Vbus Source-OK Status
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_GetVbusSrcOkStatus
(
  IN  UINT8   PortIndex,
  OUT BOOLEAN *bVbusSrcOk
)
{
  return UsbPwrCtrlLib_GetVbusSourceOK(PortIndex, bVbusSrcOk);
}

/**
EFI_UsbPwrCtrl_GetHsUsbChgPortType()

@brief
Get BC1.2 Detection Result
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_GetHsUsbChgPortType
(
  IN  UINT8   PortIndex,
  OUT EFI_USB_PWR_CTRL_HSUSB_CHGPORT_TYPE *ChgPortType
)
{
  return UsbPwrCtrlLib_GetHSUsbChgPortType(PortIndex, ChgPortType);
}

/**
EFI_UsbPwrCtrl_GetUsbIDStatus()

@brief
Get USB ID Status
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_GetUsbIDStatus
(
  IN  UINT8   PortIndex,
  OUT BOOLEAN *bUsbId
)
{
  return UsbPwrCtrlLib_GetUsbIDStatus(PortIndex, bUsbId);
}

/**
EFI_UsbPwrCtrl_GetTypeCPortStatus()

@brief
Get Type C Port Status
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_GetTypeCPortStatus
(
  IN  UINT8   PortIndex,
  OUT EFI_USB_PWR_CTRL_TYPEC_PORT_STATUS *TypeCPortStatus
)
{
  return UsbPwrCtrlLib_GetTypeCPortStatus(PortIndex, TypeCPortStatus);
}

/**
EFI_UsbPwrCtrl_GetTypeCPortPDStatus()

@brief
Get Type C Port Power Delivery Status
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_GetTypeCPortPDStatus
(
  IN  UINT8   PortIndex,
  OUT EFI_USB_PWR_CTRL_PD_STATUS  *PDStatus
)
{
  return UsbPwrCtrlLib_GetPDStatus(PortIndex, PDStatus);
}

/**
EFI_UsbPwrCtrl_SetInputPower()

@brief
Configure Input Power
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_SetInputPower
(
  IN  UINT8   PortIndex,
  IN  EFI_USB_PWR_CTRL_INOUT_POWER_CONFIG  InputPowerCfg
)
{
  return UsbPwrCtrlLib_SetSnkMaxInputCurrent(PortIndex, InputPowerCfg.MaxCurrentMA);
}

/**
EFI_UsbPwrCtrl_SetOutputPower()

@brief
Configure Output Power
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_SetOutputPower
(
  IN  UINT8   PortIndex,
  IN  EFI_USB_PWR_CTRL_INOUT_POWER_CONFIG  OutputPowerCfg
)
{
  return UsbPwrCtrlLib_SetSrcMaxOutputCurrent(PortIndex, OutputPowerCfg.MaxCurrentMA);
}

/**
EFI_UsbPwrCtrl_SetVbusSourceEn()

@brief
Enable/Disable Vbus Source
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_SetVbusSourceEn
(
  IN  UINT8   PortIndex,
  IN  BOOLEAN  bVbusSrcEn
)
{
  return UsbPwrCtrlLib_SetVbusEn(PortIndex, bVbusSrcEn);
}

/**
EFI_UsbPwrCtrl_SetVconnEn()

@brief
Enable/Disable Vconn Source
*/
EFI_STATUS
EFIAPI 
EFI_UsbPwrCtrl_SetVconnEn
(
  IN  UINT8   PortIndex,
  IN  BOOLEAN  bVconnEn
)
{
  return UsbPwrCtrlLib_SetVconnEn(PortIndex, bVconnEn);
}

/**
  USB Power Control Protocol implementation
 */
EFI_USB_PWR_CTRL_PROTOCOL UsbPwrCtrlProtocolImplementation = 
{
  USB_PWR_CTRL_REVISION,
  EFI_UsbPwrCtrl_GetHwInfo,
  EFI_UsbPwrCtrl_GetVbusDetectStatus,
  EFI_UsbPwrCtrl_GetVbusSrcOkStatus,
  EFI_UsbPwrCtrl_GetHsUsbChgPortType,
  EFI_UsbPwrCtrl_GetUsbIDStatus,
  EFI_UsbPwrCtrl_GetTypeCPortStatus,
  EFI_UsbPwrCtrl_GetTypeCPortPDStatus,
  EFI_UsbPwrCtrl_SetInputPower,
  EFI_UsbPwrCtrl_SetOutputPower,
  EFI_UsbPwrCtrl_SetVbusSourceEn,
  EFI_UsbPwrCtrl_SetVconnEn,
};


