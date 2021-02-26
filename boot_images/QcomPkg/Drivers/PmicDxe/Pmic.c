/* @file Pmic.c

  Implements the Pmic protocol
  
  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential.
*/


/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     -----------------------------------------------------------
 08/18/17     ks      Added PmicLcdbProtocol and PmicOledbProtocol support
 04/18/17     va      Moving target specific call to target file
 04/18/17     va      Added API call to set USB typeC port role in PmicInitialize based on HostModeSupported PCD
 08/19/16     pb      Enabled Test protocol (added QcomTargetLib.h for PRODMODE)
 03/24/16     al      Add keypress support
 03/13/16     ck      Enable PMIC NPA client support
 03/09/16     va      Moved common protocol install to pmic.c
 03/08/16     al      Checking for NpaDxe before initializing PAM   
 07/23/15     al      Refactoring code
 11/12/14     al      Adding core specific DAL initialization
 10/07/13     al      Moving platform config to target
 05/06/13     al      Storing pmic info and FabId
 04/01/13     sm      Added check for BattChgApp UEFI Variable
 03/07/13     dy      Add shutdown path for Liquid if pon trigger is charger
 11/06/12     dy      Add SMB350 Workaround
 10/25/12     al      Moved protocol installation to targets
=============================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>
#include <Library/SerialPortShLib.h>
#include "DALDeviceId.h"
#include "DALFramework.h"


#include "pm_uefi.h"
#include "pm_pon.h"
#include "pm_appsbl_proc_npa.h"
#include "pm_app_pon.h"

/**
  PMIC PROTOCOL interface
 */
#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomTargetLib.h>
#include "Protocol/EFIPmicClkBuff.h"
#include "Protocol/EFIPmicVib.h"
#include "Protocol/EFIPmicWled.h"
#include "Protocol/EFIPmicGpio.h"
#include "Protocol/EFIPmicMpp.h"
#include "Protocol/EFIPmicVreg.h"
#include "Protocol/EFIPmicLpg.h"
#include "Protocol/EFIPmicRTC.h"
#include "Protocol/EFIPmicPwrOn.h"
#include "Protocol/EFIPmicMipiBif.h"
#include "Protocol/EFIPmicRgbLed.h"
#include "Protocol/EFIPmicVersion.h"
#include "Protocol/EFIPmicHaptics.h"
#include "Protocol/EFIPmicIbb.h"
#include "Protocol/EFIPmicLab.h"
#include "Protocol/EFIPmicUsb.h"
#include "Protocol/EFIPmicLcdb.h"
#include "Protocol/EFIPmicOledb.h"


/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/
extern EFI_QCOM_PMIC_CLKBUFF_PROTOCOL   PmicClkBuffProtocolImplementation;
extern EFI_QCOM_PMIC_HAPTICS_PROTOCOL   PmicHapticsProtocolImplementation;
extern EFI_QCOM_PMIC_WLED_PROTOCOL      PmicWledProtocolImplementation;
extern EFI_QCOM_PMIC_GPIO_PROTOCOL      PmicGpioProtocolImplementation;
extern EFI_QCOM_PMIC_VREG_PROTOCOL      PmicVregProtocolImplementation;
extern EFI_QCOM_PMIC_LPG_PROTOCOL       PmicLpgProtocolImplementation;
extern EFI_QCOM_PMIC_RTC_PROTOCOL       PmicRtcProtocolImplementation;
extern EFI_QCOM_PMIC_PWRON_PROTOCOL     PmicPwronProtocolImplementation;
extern EFI_QCOM_PMIC_MIPIBIF_PROTOCOL   PmicMipiBifProtocolImplementation;
extern EFI_QCOM_PMIC_RGB_LED_PROTOCOL   PmicRgbLedProtocolImplementation;
extern EFI_QCOM_PMIC_VERSION_PROTOCOL   PmicVersionProtocolImplementation;
extern EFI_QCOM_PMIC_IBB_PROTOCOL       PmicIbbProtocolImplementation;
extern EFI_QCOM_PMIC_LAB_PROTOCOL       PmicLabProtocolImplementation;
extern EFI_QCOM_PMIC_USB_PROTOCOL       PmicUsbProtocolImplementation;

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

#define ARRAY_LENGTH(x)  (sizeof(x) / sizeof(x[0]))

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
extern DALREG_DriverInfo DALSystem_DriverInfo;

static StringDevice gaDALModDriverList[] =
{
   {"/pmic/target", 0, 0, &DALSystem_DriverInfo, 0, NULL},
   {"/pmic/common", 0, 0, &DALSystem_DriverInfo, 0, NULL},
};


static DALSYSConfig gPmicDALSYSModConfig =
{
   // string device info
   {ARRAY_LENGTH(gaDALModDriverList), gaDALModDriverList},
   //numeric device info
   {0,NULL}
};

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
/**
  PmicInitialize ()

  @brief
  Init implementation of EFI_QCOM_PMIC_CLIENT_DEVICE_PROTOCOL
 */
EFI_STATUS
PmicInitialize(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS Status = EFI_SUCCESS;

  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  EFI_PLATFORMINFO_PLATFORM_TYPE PlatformType;

  /* initialize DAL */
  DALSYS_InitMod(&gPmicDALSYSModConfig);

  /* Detect platform type */
  GetPlatformType(&PlatformType);

  /* Initialize the PMIC Library */
  err_flag = pm_init();

  /*support for platform that don't have PMIC*/
  if (err_flag == PM_ERR_FLAG_INVALID_PARAMETER)
  {
    Status = (PlatformType == EFI_PLATFORMINFO_TYPE_RUMI)? EFI_SUCCESS : EFI_DEVICE_ERROR;
    DEBUG((EFI_D_ERROR, "PmicDxe: PMIC was not detected \n\r"));
  }
  
  /*install common protocols*/
  Status |= pm_install_protocol(ImageHandle,SystemTable);
  /*install platform specific protocols*/
  Status |= pm_post_pmic_initialization(ImageHandle, SystemTable);

  /*check for valid PON keypress in PMIC is present*/
  if(err_flag == PM_ERR_FLAG_SUCCESS)
  {
    err_flag = pm_app_pwrkey_long_press_check(PM_PON_PWRKEY_DBNC_CHK_AT_CORE);
  }

  if ((EFI_PLATFORMINFO_TYPE_CLS != PlatformType) && (TRUE == FeaturePcdGet(HostModeSupported)))
  {
    /* Host mode feature is TRUE */
     err_flag = pm_post_pmic_usb_initialization();
  }

  return Status;
}


pm_err_flag_type pm_install_protocol(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS  Status = EFI_SUCCESS;

  Status  = gBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
    &gQcomPmicGpioProtocolGuid,
    &PmicGpioProtocolImplementation,
    &gQcomPmicVregProtocolGuid,
    &PmicVregProtocolImplementation,
    &gQcomPmicLpgProtocolGuid,
    &PmicLpgProtocolImplementation,
    &gQcomPmicClkBuffProtocolGuid,
    &PmicClkBuffProtocolImplementation,
    &gQcomPmicRtcProtocolGuid,
    &PmicRtcProtocolImplementation,
    &gQcomPmicPwrOnProtocolGuid,
    &PmicPwronProtocolImplementation,
    &gQcomPmicVersionProtocolGuid,
    &PmicVersionProtocolImplementation,
    &gQcomPmicMipiBifProtocolGuid,
    &PmicMipiBifProtocolImplementation,
    &gQcomPmicWledProtocolGuid,
    &PmicWledProtocolImplementation,
    &gQcomPmicIbbProtocolGuid,
    &PmicIbbProtocolImplementation,
    &gQcomPmicLabProtocolGuid,
    &PmicLabProtocolImplementation,
    &gQcomPmicRgbLedProtocolGuid,
    &PmicRgbLedProtocolImplementation,
    &gQcomPmicUsbProtocolGuid,
    &PmicUsbProtocolImplementation,
    &gQcomPmicHapticsProtocolGuid,
    &PmicHapticsProtocolImplementation,
    &gQcomPmicLcdbProtocolGuid,
    &PmicLcdbProtocolImplementation,
    &gQcomPmicOledbProtocolGuid,
    &PmicOledbProtocolImplementation,
    NULL
    );

  return (pm_err_flag_type)(Status);
}



