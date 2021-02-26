/**
@file  UsbPwrCtrlLibPmSchg.c
@brief Implementation of UsbPWrCtrlLibrary API for PMIC SCHG module
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
#include <Protocol/EFIPmicUsb.h>
#include <Protocol/EFIPmicSchg.h>



/*===========================================================================*/
/*                  TYPEDEF AND MACRO DEFINITIONS                            */
/*===========================================================================*/
#define USB_PWR_CTRL_LIB_TYPEC_PORT_ROLE_CHANGE_MAX_TIME_IN_MS 250

/*===========================================================================*/
/*                  FUNCTIONS PROTOTYPES                                     */
/*===========================================================================*/

/*===========================================================================*/
/*                  LOCAL VARIABLE DECLARATIONS                              */
/*===========================================================================*/
STATIC EFI_QCOM_PMIC_USB_PROTOCOL           *PmicUsbProtocol   = NULL;
STATIC EFI_QCOM_PMIC_SCHG_PROTOCOL           *PmicSchgProtocol   = NULL;
extern EFI_GUID gQcomPmicUsbProtocolGuid;
extern EFI_GUID gQcomPmicSchgProtocolGuid;

/**
  Initialize the interface to PMIC SCHG Module

  @param none

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSchg_Init()
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Get the PMIC USB charger protocol if not already available.
  if (NULL == PmicUsbProtocol) 
  {
    Status = gBS->LocateProtocol(&gQcomPmicUsbProtocolGuid, NULL, (void**)&PmicUsbProtocol);
    if (EFI_ERROR(Status))
    {
      PmicUsbProtocol = NULL;
    }
  }
  if (NULL == PmicSchgProtocol)     
  {
    Status = gBS->LocateProtocol(&gQcomPmicSchgProtocolGuid, NULL, (void**)&PmicSchgProtocol);
    if (EFI_ERROR(Status))
    {
      PmicSchgProtocol = NULL;
    }
  }
  return Status;
}

/**
  Get Vbus Detect result from PMIC SCHG Module

  @param[IN]    HwInfo          Hardware Information for this request
  @param[OUT]   bVbusDetect     Return the result of Vbus Detect  

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSchg_GetVbusDetect(
  EFI_USB_PWR_CTRL_PORTHW_HW_INFO HwInfo,
  BOOLEAN *bVbusDetect)
{
  EFI_STATUS Status = EFI_SUCCESS;
  //Sanity Check
  if (NULL == PmicUsbProtocol)
  {
    return EFI_UNSUPPORTED;
  }
  if (bVbusDetect == NULL) 
  {
    return EFI_INVALID_PARAMETER;
  }

  Status =  PmicUsbProtocol->UsbinValid(bVbusDetect);

  return Status;
}

/**
  Get Vbus Source OK result from Library

  @param[IN]    HwInfo          Hardware Information for this request
  @param[OUT]   bVbusSourceOK   Return the result of Vbus SourceOK

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSchg_GetVbusSourceOK(
  EFI_USB_PWR_CTRL_PORTHW_HW_INFO HwInfo,
  BOOLEAN *bVbusSourceOK)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PM_USB_OTG_STATUS_TYPE otgSt = EFI_PM_USB_OTG_STATUS_INVALID;
  //Sanity Check
  if (NULL == PmicUsbProtocol)
  {
    return EFI_UNSUPPORTED;
  }
  if (bVbusSourceOK == NULL) 
  {
    return EFI_INVALID_PARAMETER;
  }

  Status =  PmicUsbProtocol->GetOtgStatus(&otgSt);

  if (Status == EFI_SUCCESS && otgSt == EFI_PM_USB_OTG_ENABLED)
  {
    *bVbusSourceOK = TRUE;
  }
  else
  {
    *bVbusSourceOK = FALSE;
  }

  return Status;
}

/**
  Get HSUSB Charger Port Type

  @param[IN]    HwInfo          Hardware Information for this request
  @param[OUT]   ChgPortType     Return Charger Port Tpye

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSchg_GetHSUsbPortType(
  EFI_USB_PWR_CTRL_PORTHW_HW_INFO HwInfo,
  EFI_USB_PWR_CTRL_HSUSB_CHGPORT_TYPE *ChgPortType)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PM_USB_CHGR_PORT_TYPE PmChgPort = EFI_PM_USB_CHG_PORT_INVALID;

  //Sanity Check
  if (NULL == PmicUsbProtocol)
  {
    return EFI_UNSUPPORTED;
  }
  if (ChgPortType == NULL) 
  {
    return EFI_INVALID_PARAMETER;
  }

  Status =  PmicUsbProtocol->ChargerPort(&PmChgPort);

  if (EFI_ERROR(Status)) 
  {
    return Status;
  }

  switch (PmChgPort)
  {
    case EFI_PM_USB_CHG_PORT_SDP:
      *ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_SDP;
      break;
    case EFI_PM_USB_CHG_PORT_OTHER:
      *ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_OCP;
      break;
    case EFI_PM_USB_CHG_PORT_CDP:
      *ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_CDP;
      break;
    case EFI_PM_USB_CHG_PORT_DCP:
      *ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_DCP;
      break;
    case EFI_PM_USB_CHG_PORT_FLOAT_CHARGE:
      *ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_FLOAT_CHARGER;
      break;
    case EFI_PM_USB_CHG_PORT_QC_2P0:
      *ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_QC_2P0;
      break;
    case EFI_PM_USB_CHG_PORT_QC_3P0:
      *ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_QC_3P0;
      break;
    case EFI_PM_USB_CHG_PORT_INVALID:
    default:
      *ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_UNKNOWN;
      break;
  }
  return Status;
}

/**
  Get TypeC Port Status

  @param[IN]    HwInfo          Hardware Information for this request
  @param[OUT]   TypeCPortStatus Return TypeC port Status;

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSchg_GetTypeCPortStatus(
  EFI_USB_PWR_CTRL_PORTHW_HW_INFO HwInfo,
  EFI_USB_PWR_CTRL_TYPEC_PORT_STATUS *TypeCPortStatus)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PM_USB_TYPEC_CONNECT_MODE_STATUS  PmTypeCConnStatus = EFI_PM_USB_TYPEC_CONNECT_MODE_NONE;
  EFI_PM_USB_TYPEC_PORT_STATUS          PmTypeCStatus = {0};
  UINT64 PortRoleStartTime = 0;
  UINT64 currentTime = 0;
  UINT64 PortStatusWaitTime = 0;

  //Sanity Check
  if (NULL == PmicUsbProtocol)
  {
    return EFI_UNSUPPORTED;
  }
  if (TypeCPortStatus == NULL) 
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = PmicUsbProtocol->GetPortRoleSetTime(&PortRoleStartTime);
  if (Status == EFI_SUCCESS && PortRoleStartTime != 0)
  {
    currentTime = GetTimerCountms();
    if (currentTime < PortRoleStartTime + USB_PWR_CTRL_LIB_TYPEC_PORT_ROLE_CHANGE_MAX_TIME_IN_MS)
    {      
      PortStatusWaitTime = PortRoleStartTime + USB_PWR_CTRL_LIB_TYPEC_PORT_ROLE_CHANGE_MAX_TIME_IN_MS - currentTime;
      //MSG: Print Wait Time Here
      gBS->Stall(PortStatusWaitTime * 1000);
    }
  }

  Status = PmicUsbProtocol->UsbCGetConnectState(&PmTypeCConnStatus);
  Status |= PmicUsbProtocol->UsbCGetPortState(&PmTypeCStatus);

  if (EFI_ERROR(Status)) 
  {
    return Status;
  }

  switch(PmTypeCConnStatus)
  {
    case EFI_PM_USB_TYPEC_CONNECT_MODE_INVALID:
    case EFI_PM_USB_TYPEC_CONNECT_MODE_NONE:
      TypeCPortStatus->ConnectSts = FALSE;
      TypeCPortStatus->DataRole = EFI_USB_PWR_CTRL_TYPEC_DATAROLE_UNKNOWN;
      TypeCPortStatus->PowerRole = EFI_USB_PWR_CTRL_TYPEC_POWERROLE_UNKNOWN;
      break;
    case EFI_PM_USB_TYPEC_CONNECT_MODE_DFP:
      TypeCPortStatus->ConnectSts = TRUE;
      TypeCPortStatus->DataRole = EFI_USB_PWR_CTRL_TYPEC_DATAROLE_DFP;
      TypeCPortStatus->PowerRole = EFI_USB_PWR_CTRL_TYPEC_POWERROLE_SOURCE;
      TypeCPortStatus->DetectedSinkType = EFI_USB_PWR_CTRL_TYPEC_PORT_SNK_NONE;
      break;
    case EFI_PM_USB_TYPEC_CONNECT_MODE_UFP:
      TypeCPortStatus->ConnectSts = TRUE;
      TypeCPortStatus->DataRole = EFI_USB_PWR_CTRL_TYPEC_DATAROLE_UFP;
      TypeCPortStatus->PowerRole = EFI_USB_PWR_CTRL_TYPEC_POWERROLE_SINK;
      TypeCPortStatus->DetectedSourceType = EFI_USB_PWR_CTRL_TYPEC_PORT_SRC_NONE;
      break;
  }

  if (EFI_PM_USB_TYPEC_CONNECT_MODE_DFP)
  {
    switch (PmTypeCStatus.ufp_conn_type)
    {
      case EFI_PM_USB_TYPEC_UFP_ATTACHED:
        TypeCPortStatus->DetectedSourceType = EFI_USB_PWR_CTRL_TYPEC_PORT_SRC_RD_OPEN;
        break;
      case EFI_PM_USB_TYPEC_PWR_CABLE_NO_UFP_ATTCHED:
        TypeCPortStatus->DetectedSourceType = EFI_USB_PWR_CTRL_TYPEC_PORT_SRC_RA_OPEN;
        break;
      case EFI_PM_USB_TYPEC_PWR_CABLE_UFP:
        TypeCPortStatus->DetectedSourceType = EFI_USB_PWR_CTRL_TYPEC_PORT_SRC_RD_RA_VCONN;
        break;
      case EFI_PM_USB_TYPEC_AUDIO_ADAP_MODE:
        TypeCPortStatus->DetectedSourceType = EFI_USB_PWR_CTRL_TYPEC_PORT_SRC_RA_RA;
        break;
      case EFI_PM_USB_TYPEC_DEBUG_ACCESSORY_MODE:
        TypeCPortStatus->DetectedSourceType = EFI_USB_PWR_CTRL_TYPEC_PORT_SRC_UNORIENTED_RD_RD;
        break;
      default:
        TypeCPortStatus->DetectedSourceType = EFI_USB_PWR_CTRL_TYPEC_PORT_SRC_NONE;
        break;
    }
  }

  if (EFI_PM_USB_TYPEC_CONNECT_MODE_UFP)
  {
    switch (PmTypeCStatus.dfp_curr_adv)
    {
      case EFI_PM_USB_TYPEC_DFP_CURR_ADV_3A:
        TypeCPortStatus->DetectedSinkType = EFI_USB_PWR_CTRL_TYPEC_PORT_SNK_3P0;
        break;
      case EFI_PM_USB_TYPEC_DFP_CURR_ADV_1P5A:
        TypeCPortStatus->DetectedSinkType = EFI_USB_PWR_CTRL_TYPEC_PORT_SNK_1P5;
        break;
      case EFI_PM_USB_TYPEC_DFP_CURR_ADV_STD:
        TypeCPortStatus->DetectedSinkType = EFI_USB_PWR_CTRL_TYPEC_PORT_SNK_DEFAULT;
        break;
      default:
        TypeCPortStatus->DetectedSinkType = EFI_USB_PWR_CTRL_TYPEC_PORT_SNK_NONE;
        break;
    }
  }

  if (TypeCPortStatus->ConnectSts == FALSE)
  {
    TypeCPortStatus->CCOrientation = EFI_USB_PWR_CTRL_TYPEC_ORIENTATION_OPEN;
  }
  else if (PmTypeCStatus.ccout_out_hiz_sts == FALSE)
  {
    TypeCPortStatus->CCOrientation = EFI_USB_PWR_CTRL_TYPEC_ORIENTATION_UNORIENTED;
  }
  else 
  {
    switch (PmTypeCStatus.cc_out_sts) 
    {
      case EFI_PM_USB_TYPEC_CC_OUT_CC1:
        TypeCPortStatus->CCOrientation = EFI_USB_PWR_CTRL_TYPEC_ORIENTATION_CC1;
        break;
      case EFI_PM_USB_TYPEC_CC_OUT_CC2:
        TypeCPortStatus->CCOrientation = EFI_USB_PWR_CTRL_TYPEC_ORIENTATION_CC2;
        break;
      default:
        //should not reach here
        TypeCPortStatus->CCOrientation = EFI_USB_PWR_CTRL_TYPEC_ORIENTATION_OPEN;
        break;
    }
  }
  return Status;
}

/**
  Set Sink Max Input Current

  @param[IN]    HwInfo          Hardware Information for this request
  @param[IN]    MaxCurrentMA    Value to set Max Input Current

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSchg_SetSnkMaxInputCurrent(
  EFI_USB_PWR_CTRL_PORTHW_HW_INFO HwInfo,
  UINT32 MaxCurrentMA)
{
  EFI_STATUS Status = EFI_SUCCESS;

  //Sanity Check
  if (NULL == PmicSchgProtocol)
  {
    return EFI_UNSUPPORTED;
  }
  Status = PmicSchgProtocol->SetUsbMaxCurrent(HwInfo.ResourceIndex, MaxCurrentMA);
  return Status;
}

/**
  Set Source Max Output Current

  @param[IN]    HwInfo          Hardware Information for this request
  @param[IN]    MaxCurrentMA    Value to set Max Output Current

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSchg_SetSrcMaxOutputCurrent(
  EFI_USB_PWR_CTRL_PORTHW_HW_INFO HwInfo,
  UINT32 MaxCurrentMA)
{
  EFI_STATUS Status = EFI_SUCCESS;

  //Sanity Check
  if (NULL == PmicUsbProtocol)
  {
    return EFI_UNSUPPORTED;
  }

  Status = PmicUsbProtocol->SetOtgILimit(MaxCurrentMA);

  return Status;
}

/**
  Enable/Disable Vbus

  @param[IN]    HwInfo          Specify which port the request is for
  @param[IN]    bVbusEn         Enable/Dsiable Vbus

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSchg_SetVbusEn(
  EFI_USB_PWR_CTRL_PORTHW_HW_INFO HwInfo,
  UINT32 bVbusEn)
{
  EFI_STATUS Status = EFI_SUCCESS;

  //Sanity Check
  if (NULL == PmicUsbProtocol)
  {
    return EFI_UNSUPPORTED;
  }

  Status = PmicUsbProtocol->EnableOtg(bVbusEn);

  return Status;
}

/**
  Enable/Disable Vconn

  @param[IN]    HwInfo          Specify which port the request is for
  @param[IN]    bVconnEn        Enable/Dsiable Vconn

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSchg_SetVconnEn(
  EFI_USB_PWR_CTRL_PORTHW_HW_INFO HwInfo,
  BOOLEAN bVconnEn)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PM_USB_TYPEC_VCONN_CONFIG pm_vconn_cfg = {0};

  //Sanity Check
  if (NULL == PmicUsbProtocol)
  {
    return EFI_UNSUPPORTED;
  }

  pm_vconn_cfg.ss_pd_curr = EFI_PM_USB_TYPEC_VCONN_SS_PD_35MA;
  pm_vconn_cfg.vconn_curr_lim = EFI_PM_USB_TYPEC_VCONN_ILIMIT_500MA;
  pm_vconn_cfg.vconn_oc_en = TRUE;
  pm_vconn_cfg.vconn_en_ctrl = bVconnEn? EFI_PM_USB_TYPEC_VCONN_SW_CTRL_ENABLE : EFI_PM_USB_TYPEC_VCONN_SW_CTRL_DISABLE;

  Status = PmicUsbProtocol->UsbCSetVConn(pm_vconn_cfg);

  return Status;
}


