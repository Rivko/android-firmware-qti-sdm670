/** 
  @file  ChargerLibCommon.h
  @brief Charger Lib common API definitions.
*/
/*=============================================================================
  Copyright (c) 2012-2017 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/28/17   pbitra  Enable BMD explicitly
 12/26/17   pbitra  Rerun APSD and reconfigure USB ICL in every periodical check
 07/24/17   pbitra  Added support for charger config parameter cc_float_charging
 07/12/17   cs      add icons for debug board and align charger action enums
 06/21/17   dc      Move Platform Check to QcomChargerDxe and support config override
 03/30/17   ll      Implemented level file-logging
 03/07/17   cs      remove duplicate get errors checking emergency shutdown condition.
 01/23/17   sm      CFG parameter Structure changes
 12/19/16   sm      Added PmicIndex in ChargerHW and BatteryGaugeHW Info data
 11/09/16   cs      add api to check if DCIn is Valid
 10/17/16   sm      Added elements in CHARGERLIB_CHG_WDOG_STS_TYPE enum 
 10/13/16   cs      add protocol API for setting dcin current limit.
 09/06/16   cs      wipower changes to remove 3.5 sec, not suspend DCIn
 08/24/16   sm      Added conserv_chg_fv_delta in struct chargerlib_cfgdata_type 
                    and renamed batt_volt_lim_high_in_mv to batt_volt_lim_high_delta
 08/05/16   va      Adding Charger led indication support
 07/26/16   va      Restarting FG Changes on warm boot 
 07/15/16   cs      Adding Wipower support
 07/07/16   va      DEBUG print to print time stamp in seconds
 06/29/16   sm      Correcting charger port type enum
 06/24/16   sm      Added ChargerLib_IsChargingEnabled() API
 06/29/16   va      Enable charger configs first to enables logs during Initilization of charger app
 06/23/16   va      Adding support for Charger Fg Peripheral dumps
 06/14/16   va      Adding Thermal support 
 05/31/16   va      Update for profile load
 04/21/16   sm      Added APIs to enable/disable and pet charger wdog
 04/12/16   sm      Added enum chargerlib_battid_cfgdata_type
 03/28/16   va      File logging updated 
 03/21/16   va      Exposing Exit API 
 12/01/15   va      Added charger fg peripheral dump
 11/30/15   va      UEFI Thermal Handling support 
 11/18/15   va      UEFI Factory ship mode support  
 10/23/15   va      Time stamp for UART logging 
 08/13/25   al      Enable WDOG if device already in charging
 07/16/15   al      Waiting for temperature to return to charging zone
 06/04/15   al      Adding API to check alarm expiration status
 05/21/15   sm      Added API to pet charger watchdog
 04/30/15   va      IChgMax and VddMax configuration support for all Jeita windows
                    JEITA Low temp, high temp handle
 04/30/15   al      Adding USB Suspend API
 04/09/15   va      JEITA algo Fix - Fcc and Fv Max to be set when temperature reading is available (8994)
 03/31/15   va      Charger Log file size to 7 MB
 01/14/15   al      Adding shutdown and battery error handling to ChargerLib
 12/16/14   al      Adding AFP mode support
 12/04/14   sm      Changed API definition for ChargerInit
                          Added ChargerCheckBatteryStatus  and ChargerGetMaxCurrent APIs
 10/25/14   sv      Updates SW Jeita Feature for 8909
 10/26/14   vk      Remove inline
 09/29/14   va      Added SW Jeita Feature
 08/21/14   va      Enable File Logging 
 08/20/14   sv      Updated the Gauge HW enum.
 08/04/14   sm      Added SMBCHG and FG protocols and removed unused protocols
 01/31/14   ps      Added support for Linear battery charger (LBC) and BMS voltage mode
 03/06/13   dy      Added Charger Extension APIs
 03/05/13   sm      Added EfiBatteryGaugeBq30z55 in EFI_BATTERY_GAUGE_HW_TYPE
 10/09/12   dy      Created

=============================================================================*/
#ifndef __CHARGERLIBCOMMON_H__
#define __CHARGERLIBCOMMON_H__

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
/**
File Logging Dependencies
*/
#include "ULogFront.h"
#include "ULogUEFI.h"

/**
UEFI DEBUG Dependencies
*/
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TimerLib.h>

#include <Protocol/EFIPmicFg.h>
#include <Protocol/EFIPmicSchg.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/
//Charger Log File Name
#define LOGFILE_IN_EFS "ChargerLog"
#define LOGFILE_SIZE   8630272 // 7MB

/* DEBUG ((EFI_D_WARN, "0x%x, ", GetTimerCountms())); \*/

#if !defined(DEBUG_TIME)
  #define DEBUG_TIME(Expression)          \
    do {                           \
         DEBUG ((EFI_D_WARN, "%lld, ", ChargerLib_CalculateTime())); \
        _DEBUG (Expression);       \
      }while (FALSE)
#endif

#define _ULOG(Expression)   ULogPrint Expression

#if !defined(CHARGER_DEBUG)
  #define CHARGER_DEBUG(Expression)        \
    do {                           \
      if (ChargerLib_PrintDebugMsg ()) {  \
         DEBUG ((EFI_D_WARN, "%lld, ", ChargerLib_CalculateTime())); \
        _DEBUG (Expression);       \
      }                            \
      if(ChargerLib_PrintDebugMsgToFile()) {  \
        _ULOG (Expression);         \
      }                            \
    } while (FALSE)
  #define CHARGER_FILE_DEBUG(Expression)        \
    do {                           \
      if(ChargerLib_PrintDebugMsgToFile()) {  \
        _ULOG (Expression);         \
      }                            \
    } while (FALSE)
  /*Use below for File debug a log with Always on UART messeages */
  #define CHARGER_FILE_UART_DEBUG(Expression)      \
    do {               \
      DEBUG_TIME(Expression);    \
      if(ChargerLib_PrintDebugMsgToFile()) {  \
      _ULOG (Expression);     \
      }                \
    } while (FALSE)
  #define CHARGER_UART_DEBUG(Expression)        \
    do {                           \
      if (ChargerLib_PrintDebugMsg ()) {  \
         DEBUG ((EFI_D_WARN, "%lld, ", ChargerLib_CalculateTime())); \
        _DEBUG (Expression);       \
      }                            \
    } while (FALSE)
#else
  //#define CHARGER_DEBUG(Expression)
#endif

/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/
/**
  Charger HW
*/
typedef enum _EFI_BATTERY_CHARGER_HW_TYPE 
{ 
  EfiBatteryChargerNone,
  /**< No charger. */

  EfiBatteryChargerQcomPmicSchg,
  /**< PMIC SCHG Charger. */

  EfiBatteryChargerQcomPmicLbc,
  /**< PMIC Linear Battery Charger. */

  EfiBatteryChargerSMB1360,
  /**< SMB1360 Charger. */

} /** @cond */ EFI_BATTERY_CHARGER_HW_TYPE /** @endcond */;

typedef enum
{
  CHARGERLIB_CHARGING_ERROR_NONE = 0,
  /**< No status. */

  CHARGERLIB_CHARGING_ERROR_BATTERY_NOT_DETECTED,
  /**< Battery is not detected. */

  CHARGERLIB_CHARGING_ERROR_VBATT_OUTSIDE_RANGE,
  /**< Charging logic detected the voltage to be out of the operational range. */

  CHARGERLIB_CHARGING_ERROR_CHARGING_TIMEOUT,
  /**< Charging logic detected that the battery is not being charged within
    a reasonable time. */

  CHARGERLIB_CHARGING_ERROR_BATT_TEMP_OUTSIDE_OPERATIONAL_RANGE,
  /**< Battery is getting too hot to charge. */

  CHARGERLIB_CHARGING_ERROR_BATT_TEMP_OUTSIDE_CHARGING_RANGE,
  /**< Battery is too cold to continue charging. */

  CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL,
  /**< MSM TSENSE is critical and can not continue. */

  CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH,
  /**< MSM TSENSE is HOT to continue charging. */

  CHARGERLIB_CHARGING_ERROR_TSENSE_OK,
  /**< MSM TSENSE is OK to continue charging. */

  CHARGERLIB_CHARGING_ERROR_TSENSE_TIMEOUT,
  /**< MSM TSENSE is HOT and timed out. */

  CHARGERLIB_CHARGING_ERROR_DEBUG_BOARD,
  /**< Battery does not support the charging operation. */

  CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED,
  /**< Phone is not attached to a charging source and cannot continue with
    the charging operation. */

  CHARGERLIB_CHARGING_ERROR_ADC_ERROR,
  /**< ADC error was detected during the charging operation. */

  CHARGERLIB_CHARGING_ERROR_FG_ERROR,
  /**< Fuel gauge error detected. */

  //CHARGERLIB_CHARGING_ERROR_NO_CHARGE_WAIT,
  /**< Battery temperature outside charging range */

  CHARGERLIB_CHARGING_ERROR_LOW_VBATT,
  /**< Battery Voltage below the emergency shutdown limit. */

  CHARGERLIB_CHARGING_ERROR_UNKNOWN_BATTERY,
  /**< Battery Voltage below the emergency shutdown limit. */

  CHARGERLIB_DEVICE_ERROR,
  /**< CHarger lib APIs error . */
} /** @cond */CHARGERLIB_CHARGING_ERROR_TYPES/** @endcond */;

typedef enum
{
  CHARGERLIB_ERROR_ACTION_NONE,
  /**< No error action to be taken */

  CHARGERLIB_ERROR_ACTION_START_CHARGING,
  /**< Action required for enabling charging*/

  CHARGERLIB_ERROR_ACTION_STOP_CHARGING,
  /**< Action required for enabling charging*/

  CHARGERLIB_ERROR_ACTION_GOOD_TO_BOOT,
  /**< Action required for booting to HLOS*/

  CHARGERLIB_ERROR_ACTION_CRITICAL_SHUTDOWN,
  /**< Action required for Critical error */

  CHARGERLIB_ERROR_ACTION_SHUTDOWN,
  /**< Action required for shutdown error */

  CHARGERLIB_ERROR_ACTION_NO_CHARGE_WAIT,
  /**< Action required for no charge but wait */

  CHARGERLIB_ERROR_ACTION_SHUTDOWN_USB_DC_PON_DISABLED = 8,
  /**< Action required for shutdown error */

  CHARGERLIB_ERROR_ACTION_DEBUG_BOARD_GOOD_TO_BOOT,
  /**< Action required for booting to HLOS*/

  CHARGERLIB_ERROR_ACTION_TSENSE_HIGH_WAIT,
  /**< Action required for tesnes high wait*/

  CHARGERLIB_ERROR_ACTION_BATTERY_UNKNOWN,
  /**< Action required for unknown battery */

  CHARGERLIB_ERROR_ACTION_DEBUG_BOARD_WAIT = 13,
  /**< Action required for debug board wait in uefi*/
} CHARGERLIB_ERROR_ACTION_TYPE;

/**
  Charger Info
*/
typedef struct _EFI_BATTERY_CHARGER_INFO_TYPE
{
  EFI_BATTERY_CHARGER_HW_TYPE   ChargerHW;
  /**< Charger HW. */
  UINT32                        Version;
  /**< HW Version. */
  EFI_PM_SCHG_PMIC_INFO         ChargerPmicInfo;
  /**< Charger PMIC Info. */
} /** @cond */ EFI_BATTERY_CHARGER_INFO_TYPE /** @endcond */;

/**
  Battery Gauge HW
*/
typedef enum _EFI_BATTERY_GAUGE_HW_TYPE {
  EfiBatteryGaugeNone,
  /**< No battery gauge. */

  EfiBatteryGaugeQcomPmicFg,
  /**< PMIC FG. */

  EfiBatteryGaugeQcomPmicBmsVm,
  /**< PMIC BMS Voltage Mode  */

  EfiBatteryGaugeSMB1360
} /** @cond */ EFI_BATTERY_GAUGE_HW_TYPE /** @endcond */;

/**
  Battery Gauge Info
*/
typedef struct _EFI_BATTERY_GAUGE_INFO_TYPE
{
  EFI_BATTERY_GAUGE_HW_TYPE     BatteryGaugeHW;
  /**< Battery Gauge HW. */
  UINT32                        Version;
  /**< HW Version. */
  EFI_PM_FG_PMIC_INFO           BatteryGaugePmicInfo;
  /**< Battery Gauge PMIC Info. */
} /** @cond */ EFI_BATTERY_GAUGE_INFO_TYPE /** @endcond */;

typedef enum
{
  CHG_BATT_TYPE_NORMAL,
  /**< Battery with id within normal battery range. */
  CHG_BATT_TYPE_SMART,
  /**< Battery with id within smart battery range. */
  CHG_BATT_TYPE_DEBUG_BOARD,
  /**< DEBUG board */
  CHG_BATT_TYPE_UNKNOWN,
  /**< Battery with id outside any defined range. */
  CHG_BATT_TYPE_INVALID = 0x7FFFFFFF
} ChgBattType;

typedef enum
{
  CHGAPP_RESET_NONE,
  /**< No reset */
  CHGAPP_RESET_SHUTDOWN,
  /**< Normal shutdown */
  CHGAPP_RESET_AFP,
  /**< AFP triggered */
  CHGAPP_RESET_COLD,
  /**< cold reset */
  CHGAPP_RESET_SHIPMODE,
  /**< SHIP MODE */
  CHGAPP_RESET_SHUTDOWN_USB_DC_PON_DISABLED,
  /**< Normal shutdown */
  CHGAPP_RESET_INVALID
} ChgAppSysShutdownType;

typedef enum
{
  /*ChargerLib_Shutdown_AFP*/
  CHARGERLIB_HANDLE_SHUTDOWN_AFP,
  /* ChargerLib_Charger*/
  CHARGERLIB_HANDLE_CHARGE,
  /*ChargerLib_ContinueBoot*/
  CHARGERLIB_HANDLE_CONTINUE_BOOT,
  /*Invalid error*/
  CHARGERLIB_HANDLE_INVALID
}CHARGERLIB_HANDLE_TYPE;

typedef enum
{
  /* Charger Lib Init */
  CHARGERLIB_THERMAL_INIT,
  /* Themal OK to boot */
  CHARGERLIB_THERMAL_OK,
  /* Themal HIGH to wait for thermal to come down TsensLowTemp*/
  CHARGERLIB_THERMAL_HIGH,
  /*Themal CRITICAL shutodwn with AFP */
  CHARGERLIB_THERMAL_CRITICAL,
  /*Themal timeout during HIGH thermal zone, shutodwn with AFP */
  CHARGERLIB_THERMAL_HIGH_TIMEOUT,

  CHARGERLIB_THERMAL_STATUS_MAX = 0x7FFFFFFF,
}ChargerLibThermalStatus;

typedef struct
{
  BOOLEAN  print_charger_app_dbg_msg_to_file;
  /**< Flag to Enable/Disable Charger debug messages to File.  */

  BOOLEAN  print_charger_app_dbg_msg;
  /**< Flag to Enable/Disable Charger debug messages.  */

  BOOLEAN   enable_charger_fg_Dump;
  /**< Flag to Enable/Disable Charger FG periperal debugs*/

  UINT32 file_log_level_mask;
  /**< Debug level mask for logging to file */
  /** refer to DebugLib.h */

  /* add other debug config here*/

}chargerlib_debug_cfgdata_type;

/**
  Thermal Config Data.
*/
typedef struct
{
  INT32 tsens_high_temp;
  /**< High Temperature */

  INT32 tsens_extreme_Temp;
  /**< High Temperature for AFP */

  INT32 tsens_low_temp;
  /**< Low Temperature for thermal wait */

  INT32 tsens_timeout_mins;
  /**< Give up time in thermal wait for AFP */
}chargerlib_thermal_configs;

#define CHARGERLIB_SHIPMODE_STR_LEN 64

typedef struct
{
  BOOLEAN  enable_shipmode;
  /**<  Enable ship mode at boot */
  CHAR8    shipMode_str[CHARGERLIB_SHIPMODE_STR_LEN];
}chargerlib_shipmode_cfg;

typedef enum
{
  CHARGERLIB_PLATFORM_HW_CONFIG_NONE,
  CHARGERLIB_PLATFORM_HW_CONFIG_PMIC,
  CHARGERLIB_PLATFORM_HW_CONFIG_INVALID,
  CHARGERLIB_PLATFORM_HW_CONFIG_MAX = CHARGERLIB_PLATFORM_HW_CONFIG_INVALID,
}CHARGERLIB_PLATFORM_HW_CONFIG_TYPE;

typedef struct
{
  uint32   boot_to_hlos_threshold_in_mv;
  /**<Battery voltage at which device will boot to HLOS */

  uint32   boot_to_hlos_threshold_in_soc;
  /**<Battery SOC at which device will boot to HLOS */

  boolean  soc_based_boot;
  /**< Use Battery SOC or voltage threshold as charging criteria */

  boolean  no_charge_and_wait;
  /**< Disable charging and wait if temp outside charging range.*/

  uint32   batt_volt_lim_high_delta;
  /**< Max Value for battery voltage in mV for WP Platform*/

  uint32   emergency_shutdown_vbatt;
  /**< Lowest Voltage at which device should shutdown gracefully */

  uint32   unknown_battery_behavior;
  /**<Shutdown device upon unknown battery detection */
  /**< 0: Shuts down device, 1: Boot to HLOS if battery more than threshold else shutdown
       2: Conservative Charging 3: Regular charging */

  uint32   debug_board_behavior;
  /**< 0: Show low battery icon, disable PON1/USBIN trigger to prevent reboot and shutdown
       1: Show low battery icon and stay on until device is turned off by user. */

  BOOLEAN  sw_thermal_mitigation_enable;
  /**<  Enable SW thermal mitigation while charging  */

  UINT32   conserv_chg_fv_delta;
  /**< Voltage (in mV) to be reduced from FV_MAX during conservative charging */

  UINT32   charger_led_config;
  /**<  Led Toggle Config for charging status indication
  0 = Disable 1 = solid red during charging 2 = led blink during charging */

  UINT32  enable_charger_wdog;
  /**< TRUE: Enable watchdog when charging is enabled
       FALSE: Keep watchdog disabled when charging is enabled */

  chargerlib_debug_cfgdata_type  dbg_cfg_Data;

  chargerlib_thermal_configs     thermal_configs;

  EFI_PM_SCHG_CFGDATA_TYPE       schg_cfg_data;

  EFI_PM_FG_CFGDATA_TYPE         fg_cfg_data;

  chargerlib_shipmode_cfg        shipmode_cfg;
  /**<  Enable ship mode at boot */

  CHARGERLIB_PLATFORM_HW_CONFIG_TYPE  plat_hw_cfg_override;
  
  BOOLEAN  cc_float_charging;
  /**< Continue charging/ shutdown with CC lines floating */
  
} chargerlib_cfgdata_type;

typedef struct
{
  UINT8 *pFileBuffer;
  UINTN  DataSize;
}chargerlib_batt_profile_data;

typedef struct
{
  /* Enum for profile load/restart state */
  EFI_PM_FG_BATT_PROFILE_STATE eProfileState;
}chargerlib_batt_profile_status;

/**  Charging port types. **/
typedef enum
{
  CHARGERLIB_CHG_PORT_SDP_CHARGER,         /**< Standard Downstream Port */
  CHARGERLIB_CHG_PORT_OCP_CHARGER,         /**< Other Charging Port      */
  CHARGERLIB_CHG_PORT_CDP_CHARGER,         /**< Charging Downstream Port */
  CHARGERLIB_CHG_PORT_DCP_CHARGER,         /**< Dedicated Charging Port  */
  CHARGERLIB_CHG_PORT_FLOAT_CHARGER,
  CHARGERLIB_CHG_PORT_QC_2P0,
  CHARGERLIB_CHG_PORT_QC_3P0,
  CHARGERLIB_CHG_PORT_ICL_OVERRIDE_LATCH,
  CHARGERLIB_CHG_PORT_INVALID = 0xFF,
}CHARGERLIB_CHGR_PORT_TYPE;

typedef enum
{
   CHARGERLIB_ATTACHED_CHGR__NONE,    /**< No charger. */
   CHARGERLIB_ATTACHED_CHGR__BATT,    /**< Battery. */
   CHARGERLIB_ATTACHED_CHGR__USB,     /**< USB path. */
   CHARGERLIB_ATTACHED_CHGR__DCIN,    /**< DCIN path. */
} CHARGERLIB_ATTACHED_CHGR_TYPE;

typedef enum
{
  ChargerLib_Batt_Temp_Normal,
  ChargerLib_Batt_Temp_OutsideOperationalRange,
  ChargerLib_Batt_Temp_OutsideChargingRange,
  ChargerLib_Batt_Temp_Invalid,
}CHARGERLIB_BATT_TEMP_STATUS;

typedef enum
{
  ChargerLib_VBatt_Normal,
  ChargerLib_VBatt_BelowThreshold,
  ChargerLib_VBatt_TooHigh,
  ChargerLib_VBatt_Invalid,
}CHARGERLIB_VBATT_STATUS;

typedef struct
{
  BOOLEAN JeitaHardHot;
  BOOLEAN JeitaSoftHot;
  BOOLEAN JeitaHardCold;
  BOOLEAN JeitaSoftCold;
}CHARGERLIB_HW_JEITA_STATUS;

typedef enum {
  CHARGERLIB_EVENT_DISP_IMAGE_NOBATTERY,
  CHARGERLIB_EVENT_DISP_IMAGE_NOCHARGER,
  CHARGERLIB_EVENT_DISP_IMAGE_ABOVE_THRESHOLD,
  CHARGERLIB_EVENT_DISP_IMAGE_LOWBATTERYCHARGING,
  CHARGERLIB_EVENT_DISP_IMAGE_LOWBATTERY,
  CHARGERLIB_EVENT_DISP_IMAGE_TSENS_THERMAL_SYMBOL,
  CHARGERLIB_EVENT_DISP_IMAGE_TSENS_CRITICAL_SYMBOL,
  CHARGERLIB_EVENT_DISP_IMAGE_DEBUG_BOOT_SYMBOL,
  CHARGERLIB_EVENT_DISP_IMAGE_DEBUG_LOW_SYMBOL,
}CHARGERLIB_EVENT_DISP_IMAGE_TYPE;

typedef struct
{
  UINT32 StateOfCharge;
  INT32  ChargeCurrent;
  INT32  BatteryVoltage;
  INT32  BatteryTemperature;
}chargerlib_batt_status_info;

typedef struct
{
  /*Charger Log File Handled */
  ULogHandle gChargerLogHandle;
  BOOLEAN    print_charger_app_dbg_msg;
  BOOLEAN    print_charger_app_dbg_msg_to_file;
}chargerlib_loginfo;


typedef enum
{
  CHARGERLIB_USB_ICL_MODE_SDP_2_0,        /**< Select USB 2.0 CFG, Set USB51 Mode to 500 mA, set USBIN_MODE to USB 5/1 or USB 9/1.5 Current Level >*/
  CHARGERLIB_USB_ICL_MODE_SDP_3_0,        /**< Select USB 3.0 CFG, Set USB51 Mode to 500 mA, set USBIN_MODE to USB 5/1 or USB 9/1.5 Current Level >*/
  CHARGERLIB_USB_ICL_MODE_WALL_CHARGERS, /**< Select USB 2.0 CFG(Default), Set USB51 Mode to 100 mA, set USBIN_MODE to HC Mode Current Level     >*/
  CHARGERLIB_USB_ICL_MODE_INVALID
}CHARGERLIB_USB_ICL_MODE_TYPE;

typedef enum
{
  CHARGERLIB_CHG_WDOG_STS_BARK,                        /**< Indicates BARK WDOG Timer expired   >*/
  CHARGERLIB_CHG_WDOG_STS_BITE_CHARGING_DISABLED,      /**< Indicates BITE WDOG Timer expired and Charging is Disabled  >*/
  CHARGERLIB_CHG_WDOG_STS_BITE_CHARGING_NOT_DISABLED,  /**< Indicates BITE WDOG Timer expired but Charging is not Disabled  >*/
  CHARGERLIB_CHG_WDOG_STS_INVALID
}CHARGERLIB_CHG_WDOG_STS_TYPE;

typedef enum
{
  CHARGERLIB_CHG_WDOG_DISABLE,
  CHARGERLIB_CHG_WDOG_DISABLE_ON_EXIT,
  CHARGERLIB_CHG_WDOG_LEAVE_ENABLED_ON_EXIT,
  CHARGERLIB_CHG_WDOG_INVALID
}CHARGERLIB_CHG_WDOG_CONFIG_TYPE;

typedef
EFI_STATUS(EFIAPI *EFI_CHARGERLIB_HW_CONFIG_DETECTION)( 
  OUT EFI_BATTERY_CHARGER_INFO_TYPE *pChargerInfoType,
  OUT EFI_BATTERY_GAUGE_INFO_TYPE *pGaugeInfoType
);

typedef struct
{
  EFI_BATTERY_CHARGER_HW_TYPE             charger_hw_type;    /**< Specify Charger HW Type  >*/
  UINT32                                  charger_hw_version; /**< Specify Charger HW Interface Version  >*/
  EFI_BATTERY_GAUGE_HW_TYPE               gauge_hw_type;      /**< Specify Gague HW Type  >*/
  UINT32                                  gauge_hw_version;   /**< Specify Gague HW Interface Version  >*/
}CHARGERLIB_HW_CONFIG_INFO;

typedef struct
{
  EFI_PLATFORMINFO_PLATFORM_TYPE          PlatformType;       /**< Specify Platform Type ID  >*/
  EFI_CHARGERLIB_HW_CONFIG_DETECTION      pHwConfigDetectFunc;/**< Specify hw type detection function for this platform  >*/
  CHARGERLIB_PLATFORM_HW_CONFIG_TYPE      StaticHwConfigIndex;/**< Specify static hw configuration ID>*/  
}CHARGERLIB_PLATFORM_HW_CONFIG_INFO;



/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
EFI_STATUS ChargerLibCommon_Init( chargerlib_cfgdata_type *pChargerLibConfig );

EFI_STATUS ChargerLib_ChargerEnable(BOOLEAN Enable);

EFI_STATUS ChargerLib_SetMaxUsbCurrent(UINT32 MaxCurrent);

EFI_STATUS ChargerLib_GetPortType(CHARGERLIB_CHGR_PORT_TYPE *pPortType);

EFI_STATUS ChargerLib_GetBatteryPresence(BOOLEAN *BatteryPresence);

BOOLEAN ChargerLib_PrintDebugMsg(void);

BOOLEAN ChargerLib_PrintDebugMsgToFile(void);

UINT64  ChargerLib_CalculateTime();

UINT64  ChargerLib_CalculateTimeMSec();

VOID ULogPrint (IN  UINTN ErrorLevel, IN  CONST CHAR8  *Format,  ...);

EFI_STATUS ChargerLib_InitFileLog(BOOLEAN PrintChgAppDbgMsgToFile);

/* Enable Pmic AFP Mode */
EFI_STATUS ChargerLib_EnableAfpMode(void);

/*This API resets the device.*/
EFI_STATUS ChargerLib_ForceSysShutdown(ChgAppSysShutdownType ShutdownType);

/*disable charging and put device in hold off status for specified period*/
EFI_STATUS ChargerLib_HandleNoChargeAndWait(void);

/* API to pet charger watchdog */
EFI_STATUS ChargerLib_PetChgWdog( void );

/*Enables WDOG if charging is already enabled*/
EFI_STATUS ChargerLib_EnWdogIfCharging(void);

/* time stamp 64 bit */
UINT64 ChargerLib_TimeGet64(void);

EFI_STATUS ChargerLib_GetThermalStatus( ChargerLibThermalStatus *ThermalStatus);

/* Dump Charger Pheripheral */
EFI_STATUS ChargerLib_DumpChargerPeripheral(void);

/* Dump FG SRAM Pheripheral */
EFI_STATUS ChargerLib_DumpSram(BOOLEAN bDumpExplicit);

EFI_STATUS ChargerLib_GetErrors(BOOLEAN vbattChecking, CHARGERLIB_CHARGING_ERROR_TYPES  *pChargingError);

EFI_STATUS ChargerLib_GetErrorAction( CHARGERLIB_CHARGING_ERROR_TYPES  ChargingError, CHARGERLIB_ERROR_ACTION_TYPE *pErrorAction);

EFI_STATUS ChargerLib_EnableHWJeita(BOOLEAN Enable);

EFI_STATUS ChargerLib_WasChargerReinserted(BOOLEAN *pChargerReinserted);

EFI_STATUS ChargerLib_ReRunAicl( VOID );

VOID ChargerLibEvent_ExitLPM( VOID );

EFI_STATUS ChargerLib_GetMaxUsbCurrent( UINT32 *pMaxCurrent);

EFI_STATUS ChargerLib_GetBatteryStatus(chargerlib_batt_status_info *BatteryStatus);

EFI_STATUS ChargerLib_InitializeCharging( VOID );

EFI_STATUS ChargerLibEvent_DispBattSymbol(CHARGERLIB_EVENT_DISP_IMAGE_TYPE DispImage, BOOLEAN ClearScreen);

EFI_STATUS ChargerLib_Exit(VOID);

EFI_STATUS ChargerLib_EnableWdog(BOOLEAN Enable);

EFI_STATUS ChargerLib_PetChgWdog( VOID );

EFI_STATUS ChargerLib_LoadProfile( OPTIONAL chargerlib_batt_profile_data *pBattProfileInfo);

EFI_STATUS ChargerLib_GetBatteryProfileStatus(chargerlib_batt_profile_status *ProfileSts);

EFI_STATUS ChargerLib_GetLogInfo(chargerlib_loginfo *gFileHandle );

EFI_STATUS ChargerLib_GetChargingStatus(BOOLEAN *pChargingEnabled);

EFI_STATUS ChargerLib_GetChargingPath(CHARGERLIB_ATTACHED_CHGR_TYPE *ChargingPath);

EFI_STATUS ChargerLib_DcinSuspend(BOOLEAN Suspend);

EFI_STATUS ChargerLib_DcinSetPowerLimit(UINT32 PowerInMicroW);

EFI_STATUS ChargerLib_LedOn( BOOLEAN bLedOn);

EFI_STATUS ChargerLib_ToggleWipowerShutDownLatch();

EFI_STATUS ChargerLib_SetDcinCurrent(UINT32 currentLimit);

EFI_STATUS ChargeLib_IsDcinValid(BOOLEAN* pIsValid);

EFI_STATUS ChargerLib_GetShipMode(BOOLEAN* Enabled);

EFI_STATUS ChargerLib_IsChargingSupported(BOOLEAN *pChargingSupported);

EFI_STATUS ChargerLib_ContinueCharging( VOID );

EFI_STATUS ChargerLib_EnableBMD(VOID);



#endif  /* __CHARGERLIBCOMMON_H__ */

