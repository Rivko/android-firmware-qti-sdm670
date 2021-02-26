/*
This file is for LA vbatt based charging in UEFI, it initialize ChargerLib 
to use HW Jeita and implement the function to determine if the conditions to boot to HLOS are met.
*/
/** 
  @file  QcomChargerPlatform.h
  @brief Charger Vbatt based threshold charging UEFI Driver definitions.
         It initializes ChargerLibCommon, initializes HW Jeita 
         and implement the function to determine if the conditions to boot to HLOS are met.
*/
/*=============================================================================
  Copyright (c) 2016 - 2017 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/12/17   cs      remove redundent enum definition for platform charger Action
 03/30/17   ll      Implemented level file-logging
 03/07/17   cs      remove duplicate get errors checking emergency shutdown condition.
 01/23/17   sm      CFG parameter Structure changes
 11/10/16   va      Adding Config VBtEmpty threshold
 10/19/16   sm      Removed EnableHvdcp cfg param from QCOM_CHARGER_PLATFORM_SCHG_CFGDATA_TYPE struct
 10/06/16   cs      configure batt therm/aux therm bias wait 
 09/13/16   sm      Removed unused config parameters from QCOM_CHARGER_PLATFORM_SCHG_CFGDATA_TYPE
                    and QCOM_CHARGER_PLATFORM_CFGDATA_TYPE structs
 09/06/16   cs      wipower changes to remove 3.5 sec, not suspend DCIn
 08/05/16   va      Adding Charger led indication support
 07/26/16   va      Restarting FG Changes on warm boot 
 07/15/16   cs      Adding Wipower support
 06/23/16   va      Adding Action Info for taking Action
 06/23/16   sm      Added EnableHvdcp config parameter in QCOM_CHARGER_PLATFORM_SCHG_CFGDATA_TYPE
 06/23/16   va      Adding support for Charger Fg Peripheral dumps
 06/14/16   va      Adding File log support to Charger App
 06/09/16   sm      Added SupportHostMode in QCOM_CHARGER_PLATFORM_SCHG_CFGDATA_TYPE
 05/31/16   va      Adding Battery profile load feature
 05/10/16   sm      Added ChargerPlatform_GetConfigData()
 04/26/16   va      Adding parameters for Aux Coffes, SkinHot and Charger Hot settings
 04/21/16   sm      Added EnableChargerWdog in QCOM_CHARGER_PLATFORM_CFGDATA_TYPE struct
 04/12/16   sm      Editted elements in BATTID_CFGDATA_TYPE Struct
 03/28/16   va      Adding Jeita Compensation and Consolidating config data into one struct 
 03/21/16   va      Remove dummy configs and add battery missing config
 01/27/16   sm      Initial revision

=============================================================================*/

#ifndef __QCOMCHARGERPLATFORM_H__ 
#define __QCOMCHARGERPLATFORM_H__

#include <api/pmic/charger/ChargerLibCommon.h>
#include <Protocol/EFIPmicSchg.h>
#include <Protocol/EFIPmicFg.h>
#include <Protocol/EFIQcomCharger.h>

/*===========================================================================
MACRO DEFINITIONS
===========================================================================*/

#define QCOM_CHG_WDOG_DISABLE               0
#define QCOM_CHG_WDOG_DISABLE_ON_EXIT       1
#define QCOM_CHG_WDOG_LEAVE_ENABLED_ON_EXIT 2

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
typedef enum
{
  QCOM_CHARGER_PLATFORM_STATUS_GOOD_TO_BOOT = 0,
  /**< Device is good to boot to HLOS */
  QCOM_CHARGER_PLATFORM_STATUS_CHARGING,
  /**< Charging is on-going */
  QCOM_CHARGER_PLATFORM_STATUS_ERROR,
  /**< Error occurred */
  QCOM_CHARGER_PLATFORM_STATUS_PROFILE_LOAD,
  /**< Profile load is needed  */
  QCOM_CHARGER_PLATFORM_STATUS_DEBUG_BOARD_GOOD_TO_BOOT,
  /**< Boot to HLOS */
  QCOM_CHARGER_PLATFORM_STATUS_INVALID,
  /**< Error occurred */
}QCOM_CHARGER_PLATFORM_STATUS_TYPE;


typedef struct
{
    BOOLEAN WipowerEnabled;
    /**< if wipower is enabled or not */

    UINT32  WipowerBootThreshold;
    /**< boot up threshold for wipower charging */

    BOOLEAN DCINDisableOnExit;
    /**< suspend dcin or not */

}QCOM_CHARGER_PLATFORM_WIPOWER_CFGDATA_TYPE;

typedef struct {
  UINT32   CfgVersion;
  /**< Version number for CFG file*/

  BOOLEAN  ProgramBattThermCoeffs;
  /**<  Enables Batt therm coeffs to be programmed via configs instead of profiles */

  BOOLEAN  FullBattChargingEnabled;
  /**<  Enable full battery charging  */

  UINT32   DispSignOfLifeMaxThresholdMv;
  /**< battery Threshold to display */

  UINT32   SWFlashMinBattVoltageMv;
  /**< Min battery voltage for SW image flash required */

  QCOM_CHARGER_PLATFORM_WIPOWER_CFGDATA_TYPE     WiPowerCfgData;

  chargerlib_cfgdata_type    ChargerLibCfgData;

} QCOM_CHARGER_PLATFORM_CFGDATA_TYPE;


typedef struct 
{
  UINT8 *pFileBuffer;
  UINTN  DataSize;
}QCOM_CHARGER_PLATFORM_BATT_PROFILE_DATA;

typedef enum
{
  QCOM_CHARGER_PLATFORM_PROFILE_LOAD,
  QCOM_CHARGER_PLATFORM_PROFILE_RESTART,
  QCOM_CHARGER_PLATFORM_PROFILE_INVALID
}QCOM_CHARGER_PLATFORM_BATT_PROFILE_STATE;

typedef struct 
{
  /* Enum for profile load/restart state */
  QCOM_CHARGER_PLATFORM_BATT_PROFILE_STATE eProfileState;
}QCOM_CHARGER_PLATFORM_BATT_PROFILE_STATUS;

typedef struct
{
  UINT32 StateOfCharge;
  INT32  ChargeCurrent;
  INT32  BatteryVoltage;
  INT32  BatteryTemperature;
}QCOM_CHARGER_BATT_STATUS_INFO;

typedef enum
{
  QCOM_CHARGER_PLATFORM_CHARGING_LED_OFF,
  QCOM_CHARGER_PLATFORM_CHARGING_LED_ON,
  QCOM_CHARGER_PLATFORM_CHARGING_LED_TOGGLE,
  QCOM_CHARGER_PLATFORM_CHARGING_LED_INVALID
}QCOM_CHARGER_PLATFORM_CHARGING_LED_CONFIG_TYPE;

typedef enum 
{
   QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_NONE,    /**< No charger. */
   QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_BATT,    /**< Battery. */
   QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_USB,     /**< USB path. */
   QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_DCIN,    /**< DCIN path. */
}QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_TYPE;

typedef struct 
{
  BOOLEAN bChargerSrcPresent;
  UINT32  DispSignOfLifeMaxThresholdMv;
  QCOM_CHARGER_BATT_STATUS_INFO BattStsInfo;
  CHARGERLIB_ATTACHED_CHGR_TYPE ChargerAttached;
  QCOM_CHARGER_PLATFORM_BATT_PROFILE_STATUS ProfState;
  QCOM_CHARGER_PLATFORM_CHARGING_LED_CONFIG_TYPE LedConfigType;
}QCOM_CHARGER_PLATFORM_ACTION_INFO;

typedef struct
{
  /*Charger Log File Handled */
  VOID      *gChargerLogHandle;
  BOOLEAN    bPrintChargerAppDbgMsg;
  BOOLEAN    bPrintChargerAppDbgMsgToFile;
  UINT32     fileLogLevelMask;
}QCOM_CHARGER_PLATFORM_LOG_INFO;

typedef enum
{
  QCOM_CHARGER_PLATFORM_CHG_WDOG_DISABLE,
  QCOM_CHARGER_PLATFORM_CHG_WDOG_DISABLE_ON_EXIT,
  QCOM_CHARGER_PLATFORM_CHG_WDOG_LEAVE_ENABLED_ON_EXIT,
  QCOM_CHARGER_PLATFORM_CHG_WDOG_INVALID
}QCOM_CHARGER_PLATFORM_CHG_WDOG_CONFIG_TYPE;

/** @} */ /* end_addtogroup efi_qcom_platform_cfg_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
EFI_STATUS ChargerPlatform_Init( VOID );

EFI_STATUS ChargerPlatform_GetChargingAction(EFI_QCOM_CHARGER_ACTION_TYPE *pActionType, QCOM_CHARGER_PLATFORM_ACTION_INFO *pChargerActionInfo, BOOLEAN vbattChecking);

EFI_STATUS ChargerPlatform_TakeAction(EFI_QCOM_CHARGER_ACTION_TYPE ChargingAction, CONST QCOM_CHARGER_PLATFORM_ACTION_INFO *pChargerActionInfo);

VOID ChargerPlatform_ReadCfgParams( UINT8* Section, UINT8* Key, UINT8* Value);

EFI_STATUS ChargerPlatform_GetConfigData(QCOM_CHARGER_PLATFORM_CFGDATA_TYPE *pChargerPlatformCfgData);

EFI_STATUS ChargerPlatformFile_GetLogInfo(QCOM_CHARGER_PLATFORM_LOG_INFO *pFileLogInfo);

EFI_STATUS ChargerPlatformFile_GetChargerConfig(EFI_QCOM_CHARGER_CONFIG_KEY ChargerCfgKey, UINT32 *Value);

#endif  /* __CHARGERPLATFORM_H__ */

