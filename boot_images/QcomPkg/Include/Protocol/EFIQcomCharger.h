/** 
  @file  EFIQcomCharger.h
  @brief Qualcomm Charger UEFI Protocol definitions.
           Returns battery charging status and take action such as shutdown, etc
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
 07/12/17   cs      Add new icons for debug board
 05/22/17   dc      Move Platform Check From QcomChargerApp to QcomChargerDxe
 03/30/17   ll      Implemented level file-logging
 03/07/17   cs      remove duplicate get errors checking emergency shutdown condition.
 11/09/16   cs      modified charger present API also to detect DCIn. 
 11/07/16   va      Added Action type for debug board
 09/29/16   va      Added Action type for emergency shutdown
 09/06/16   cs      wipower changes to remove 3.5 sec, not suspend DCIn
 07/25/16   va      Adding support for Restart FG condition
 07/15/16   cs      Adding Wipower support
 06/23/16   va      Adding Action Info for taking Action
 06/23/16   va      Adding support for Charger Fg Peripheral dumps
 06/14/16   va      Adding File log support for Charger App
 06/03/16   va      Update for Profile load and sign of early life 
 06/01/16   sm      Added wrapper APIs for battery presence and battery voltage
 03/21/16   va      Adding Exit API for clean up
 02/10/16   sm      Initial Draft

=============================================================================*/

#ifndef __EFIQCOMCHARGER_H__
#define __EFIQCOMCHARGER_H__

/*===========================================================================
INCLUDE FILES
===========================================================================*/
#include "EFIULog.h"
/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/**
  Protocol version.
*/
#define QCOM_CHARGER_REVISION 0x0000000000010007
/** @} */ /* end_addtogroup efi_pmicfg_constants */

/*  Protocol GUID definition */
/** @ingroup efi_qcomcharger_protocol */
#define EFI_QCOM_CHARGER_PROTOCOL_GUID \
  { 0x21bfb78b, 0x02ea, 0x4ab7, { 0x99, 0x40, 0xb5, 0x8a, 0x2a, 0xd6, 0x58, 0x29 } }

/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/**
External reference to the PMIC Fg UEFI Protocol GUID.
*/

extern EFI_GUID gQcomChargerProtocolGuid;


/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_CHARGER_PROTOCOL  EFI_QCOM_CHARGER_PROTOCOL;

/*===========================================================================*/
typedef enum
{
  EFI_QCOM_CHARGER_STATUS_GOOD_TO_BOOT = 0,
  /**< Device is good to boot to HLOS */
  EFI_QCOM_CHARGER_STATUS_CHARGING,
  /**< Charging is on-going */
  EFI_QCOM_CHARGER_STATUS_ERROR,
  /**< Error occurred */
  EFI_QCOM_CHARGER_PLATFORM_STATUS_PROFILE_LOAD,
  /**< Profile load is needed  */
  EFI_QCOM_CHARGER_STATUS_DEBUG_BOARD_GOOD_TO_BOOT,
  /**< Detected debug board good to boot */
  EFI_QCOM_CHARGER_STATUS_INVALID,
  /**< Error occurred */
}EFI_QCOM_CHARGER_STATUS_TYPE;

typedef enum
{
  EFI_QCOM_CHARGER_ACTION_CONTINUE,
  /**< Continue charging */
  EFI_QCOM_CHARGER_ACTION_START_CHARGING,
  /**< Start Charging */
  EFI_QCOM_CHARGER_ACTION_STOP_CHARGING,
  /**<  Stop Charging */
  EFI_QCOM_CHARGER_ACTION_GOOD_TO_BOOT,
  /**< Boot to HLOS */
  EFI_QCOM_CHARGER_ACTION_CRITICAL,
  /**< Action for critical error */
  EFI_QCOM_CHARGER_ACTION_SHUTDOWN,
  /**< Shutdown device */
  EFI_QCOM_CHARGER_ACTION_NO_CHARGE_WAIT,
  /**< Stop charging and wait */
  EFI_QCOM_CHARGER_PLATFORM_ACTION_PROFILE_LOAD,
  /**< Profile load Action */
  EFI_QCOM_CHARGER_ACTION_SHUTDOWN_USB_DC_PON_DISABLED,
  /**< Action required for shutdown disable PON1 */
  EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_GOOD_TO_BOOT,
  /**< debug board Boot to HLOS */
  EFI_QCOM_CHARGER_ACTION_TSENSE_HIGH_WAIT,
  /**< action for tsense high*/
  EFI_QCOM_CHARGER_ACTION_BATTERY_UNKNOWN,
  /**< Stop charging and wait */
  EFI_QCOM_CHARGER_ACTION_EXIT,
  /**< Clean up Charger */
  EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_WAIT,
  /**< debug board stay in uefi */
  EFI_QCOM_CHARGER_ACTION_INVALID,
  /**< invalid action */
}EFI_QCOM_CHARGER_ACTION_TYPE;


typedef enum { 
  EFI_QCOM_CHARGER_ERROR_NONE = 0,
  /**< No status. */

  EFI_QCOM_CHARGER_ERROR_BATTERY_NOT_DETECTED,
  /**< Battery is not detected. */

  EFI_QCOM_CHARGER_ERROR_VBATT_OUTSIDE_RANGE,
  /**< Charging logic detected the voltage to be out of the operational range. */

  EFI_QCOM_CHARGER_ERROR_CHARGING_TIMEOUT,
  /**< Charging logic detected that the battery is not being charged within
    a reasonable time. */

  EFI_QCOM_CHARGER_ERROR_BATT_TEMP_OUTSIDE_OPERATIONAL_RANGE,
  /**< Battery temperature is too hot to charge. */

  EFI_QCOM_CHARGER_ERROR_BATT_TEMP_OUTSIDE_CHARGING_RANGE,
  /**< Battery temperature is too hot to continue charging. */

  EFI_QCOM_CHARGER_ERROR_TSENSE_CRITICAL,
  /**< Battery is getting too hot to charge. */

  EFI_QCOM_CHARGER_ERROR_TSENSE_HOT,
  /**< Battery is too cold to continue charging. */

  EFI_QCOM_CHARGER_ERROR_BATTERY_CHARGING_NOT_SUPPORTED,
  /**< Battery does not support the charging operation. */

  EFI_QCOM_CHARGER_ERROR_CHARGING_SOURCE_NOT_DETECTED,
  /**< Phone is not attached to a charging source and cannot continue with 
    the charging operation. */

  EFI_QCOM_CHARGER_ERROR_ADC_ERROR,
  /**< ADC error was detected during the charging operation. */

  EFI_QCOM_CHARGER_ERROR_FG_ERROR,
  /**< Fuel gauge error detected. */

  //EFI_QCOM_CHARGER_ERROR_NO_CHARGE_WAIT,
  /**< Battery temperature outside charging range */

  EFI_QCOM_CHARGER_ERROR_LOW_VBATT,
  /**< Battery Voltage below the emergency shutdown limit. */

  EFI_QCOM_CHARGER_ERROR_UNKNOWN_BATTERY,
  /**< Battery Voltage below the emergency shutdown limit. */

  EFI_QCOM_CHARGER_ERROR_DEBUG_BOARD_GOOD_TO_BOOT
  /**< Detected debug board */
} /** @cond */EFI_QCOM_CHARGER_ERROR_TYPES/** @endcond */;

typedef enum {
  EFI_QCOM_CHARGER_DISP_IMAGE_NOBATTERY,
  EFI_QCOM_CHARGER_DISP_IMAGE_NOCHARGER,
  EFI_QCOM_CHARGER_DISP_IMAGE_ABOVE_THRESHOLD,
  EFI_QCOM_CHARGER_DISP_IMAGE_LOWBATTERYCHARGING,
  EFI_QCOM_CHARGER_DISP_IMAGE_LOWBATTERY,
  EFI_QCOM_CHARGER_DISP_IMAGE_TSENS_THERMAL_SYMBOL,
  EFI_QCOM_CHARGER_DISP_IMAGE_TSENS_CRITICAL_SYMBOL,
  EFI_QCOM_CHARGER_DISP_IMAGE_DEBUG_BOOT_SYMBOL,
  EFI_QCOM_CHARGER_DISP_IMAGE_DEBUG_LOW_SYMBOL,
}EFI_QCOM_CHARGER_DISP_IMAGE_TYPE;


typedef struct
{
  UINT32 StateOfCharge;
  INT32  ChargeCurrent;
  INT32  BatteryVoltage;
  INT32  BatteryTemperature;
}EFI_QCOM_CHARGER_BATT_STATUS_INFO;

typedef enum
{
  EFI_QCOM_CHARGER_PROFILE_LOAD,
  EFI_QCOM_CHARGER_PROFILE_RESTART,
  EFI_QCOM_CHARGER_PROFILE_INVALID
}EFI_QCOM_CHARGER_BATT_PROFILE_STATE;

typedef enum
{
  EFI_QCOM_CHARGER_CHARGING_LED_OFF,
  EFI_QCOM_CHARGER_CHARGING_LED_ON,
  EFI_QCOM_CHARGER_CHARGING_LED_TOGGLE,
  EFI_QCOM_CHARGER_CHARGING_LED_INVALID
}EFI_QCOM_CHARGER_CHARGING_LED_CONFIG_TYPE;

typedef enum 
{
   EFI_QCOM_CHARGER_ATTACHED_NONE,    /**< No charger. */
   EFI_QCOM_CHARGER_ATTACHED_BATT,    /**< Battery. */
   EFI_QCOM_CHARGER_ATTACHED_USB,     /**< USB path. */
   EFI_QCOM_CHARGER_ATTACHED_DCIN,    /**< DCIN path. */
} EFI_QCOM_CHARGER_ATTACHED_TYPE;

typedef struct 
{
  BOOLEAN bChargerSrcPresent;
  UINT32  DispSignOfLifeMaxThresholdMv;
  EFI_QCOM_CHARGER_BATT_STATUS_INFO         BatteryStatusInfo;
  EFI_QCOM_CHARGER_ATTACHED_TYPE            ChargerAttached;
  EFI_QCOM_CHARGER_BATT_PROFILE_STATE       ProfState;
  EFI_QCOM_CHARGER_CHARGING_LED_CONFIG_TYPE LedConfigType;
}EFI_QCOM_CHARGER_ACTION_INFO;


typedef struct 
{
  /*Charger Log File Handled */
  ULogHandle gChargerLogHandle;
  BOOLEAN    bPrintChargerAppDbgMsg;
  BOOLEAN    bPrintChargerAppDbgMsgToFile;
  UINT32     fileLogLevelMask;
}EFI_QCOM_CHARGER_LOG_INFO;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_QCOM_CHARGER_ENABLE_CHARGING */
/** @ingroup efi_qcomchargerEnableCharging
* @par Summary
* Enables battery charging
*
* @param[in] Enable: TRUE: Enable Charging
*                    FALSE: Disable Charging
*
*@return 
* EFI_SUCCESS           -- Function completed successfully. \n
* EFI_INVALID_PARAMETER -- Parameter is invalid. \n
* EFI_DEVICE_ERROR      -- Physical device reported an error. \n
* EFI_NOT_READY         -- Physical device is busy or not ready to
*                          process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_ENABLE_CHARGING)( 
  IN BOOLEAN Enable
);

/* EFI_QCOM_CHARGER_GET_MAX_USB_CURRENT */
/** @ingroup efi_qcomchargerGetMaxUsbCurrent
* @par Summary
* Gets Max USB current from CFG file
*
* @param[out] pMaxCurrent: value of current in mA
*
*@return 
* EFI_SUCCESS           -- Function completed successfully. \n
* EFI_INVALID_PARAMETER -- Parameter is invalid. \n
* EFI_DEVICE_ERROR      -- Physical device reported an error. \n
* EFI_NOT_READY         -- Physical device is busy or not ready to
*                          process this request.
*/
typedef 
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_GET_MAX_USB_CURRENT)(
  OUT UINT32 *pMaxCurrent
);

/* EFI_QCOM_CHARGER_SET_MAX_USB_CURRENT */
/** @ingroup efi_qcomchargerSetMaxUsbCurrent
* @par Summary
* Enables battery charging
*
* @param[in] MaxCurrent: value of current in mA
*
*@return 
* EFI_SUCCESS           -- Function completed successfully. \n
* EFI_INVALID_PARAMETER -- Parameter is invalid. \n
* EFI_DEVICE_ERROR      -- Physical device reported an error. \n
* EFI_NOT_READY         -- Physical device is busy or not ready to
*                          process this request.
*/
typedef 
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_SET_MAX_USB_CURRENT)(
  IN UINT32 MaxCurrent
);


/* EFI_QCOMCHARGER_GET_CHARGING_STATUS */
/** @ingroup efi_qcomchargerGetChargingAction
  @par Summary
  Returns required charging action
 
  @param[out] pChargingAction: Required action type
                               refer to EFI_QCOM_CHARGER_ACTION_TYPE for details
  @param[out] pChargerActionInfo: information such as battery voltage, soc and charger present
                               refer to EFI_QCOM_CHARGER_ACTION_INFO for details
  @param[in] vbattChecking: if low vbatt checking for emergency shutdown will be done.

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_GET_CHARGING_ACTION)( 
  OUT EFI_QCOM_CHARGER_ACTION_TYPE  *pChargingAction,
  OUT EFI_QCOM_CHARGER_ACTION_INFO  *pChargerActionInfo,
  IN  BOOLEAN                       vbattChecking
);

/* EFI_QCOMCHARGER_TAKE_ACTION */
/** @ingroup efi_qcomchargerTakeAction
  @par Summary
  Get Charge Current
 
  @param[in] ChargingAction: Required action type
                              refer to EFI_QCOM_CHARGER_CHARGING_ACTION_TYPE for details
 @param[in] pChargerActionInfo: Required action info 
                             refer to EFI_QCOM_CHARGER_ACTION_INFO for details

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_TAKE_ACTION)( 
  IN EFI_QCOM_CHARGER_ACTION_TYPE ChargingAction,
  IN CONST EFI_QCOM_CHARGER_ACTION_INFO *pChargerActionInfo
);

/* EFI_QCOMCHARGER_TAKE_ACTION */
/** @ingroup efi_qcomchargerTakeAction
  @par Summary
  Get Charge Current
 
  @param[in] ChargingAction: Required action type
                              refer to EFI_QCOM_CHARGER_CHARGING_ACTION_TYPE for details

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_DISPLAY_IMAGE)( 
  IN EFI_QCOM_CHARGER_DISP_IMAGE_TYPE ImageType,
  IN BOOLEAN ClearScreen
);


/* EFI_QCOM_CHARGER_EXIT */
/** @ingroup efi_qcomchargerExit
@par Summary
Clean up Qcom charger dxe on charging exit or critical error
 
 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_DEINIT)(
  IN EFI_QCOM_CHARGER_ACTION_TYPE ChargingAction
);


/* EFI_QCOM_CHARGER_GET_BATTERY_PRESENCE */
/** @ingroup efi_qcomchargerGetBatteryPresence
  @par Summary
  Returns battery presence status
 
  @param[out] *pBatteryPresence: TRUE: Battery is Present
                                 FALSE: Battery is not present

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_GET_BATTERY_PRESENCE)(
  OUT BOOLEAN *pBatteryPresence
);

/* EFI_QCOM_CHARGER_GET_BATTERY_VOLTAGE */
/** @ingroup efi_qcomchargerGetBatteryVoltage
  @par Summary
  Returns battery voltage
 
  @param[out] *pBatteryVoltage: TRUE: Battery is Present
                                 FALSE: Battery is not present

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_GET_BATTERY_VOLTAGE)( 
  OUT UINT32 *pBatteryVoltage
);

/* EFI_QCOM_CHARGER_GET_FILE_LOG_INFO */
/** @ingroup efi_getFileLogInfo
  @par Summary
  Returns File Log Information as File log handle
 
  @param[out] *pFileLogInfo: File Log informtion as in File log handle

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_GET_LOG_INFO)( 
  OUT EFI_QCOM_CHARGER_LOG_INFO *pFileLogInfo
);

/* EFI_QCOM_CHARGER_GET_FILE_LOG_INFO */
/** @ingroup efi_getFileLogInfo
  @par Summary
  Returns File Log Information as File log handle
 
  @param[out] *pFileLogInfo: File Log informtion as in File log handle

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_DUMP_PERIPHERAL)( 
);

/* EFI_QCOM_CHARGER_IS_DCIN_VALID */
/** @ingroup efi_isDcInValid
@par Summary
Returns if dcin is valid or not

@param[out] *pIsValid: if dcin is valid or not

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_INVALID_PARAMETER -- Parameter is invalid. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
EFI_NOT_READY         -- Physical device is busy or not ready to
process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_IS_DCIN_VALID)(
OUT BOOLEAN* pIsValid
);

typedef enum
{
  /* Key to retrive battery minimum voltage for allowing SW image flash */
  EFI_QCOM_CHARGER_CONFIG_KEY_SW_FLASH_VOLTAGE,
  EFI_QCOM_CHARGER_CONFIG_KEY_INVALID = 0x7FFFFFFF,
}EFI_QCOM_CHARGER_CONFIG_KEY;
/* EFI_QCOM_CHARGER_GET_CHARGER_CONFIG */
/** @ingroup efi_qcom_charger_get_charger_config
@par Summary
Returns if dcin is valid or not

@param[out] *pIsValid: if dcin is valid or not

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_INVALID_PARAMETER -- Parameter is invalid. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
EFI_NOT_READY         -- Physical device is busy or not ready to
process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_GET_CHARGER_CONFIG)(
IN EFI_QCOM_CHARGER_CONFIG_KEY  ChargerCfgKey,
OUT  UINT32 *KeyValue
);

/* EFI_QCOM_CHARGER_IS_CHARGING_SUPPORTED */
/** @ingroup efi_isChargingSupported
@par Summary
Returns if charging is supported or not

@param[out] *pIsChargingSupported: if charging is supported or not

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_INVALID_PARAMETER -- Parameter is invalid. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
EFI_NOT_READY         -- Physical device is busy or not ready to
process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_QCOM_CHARGER_IS_CHARGING_SUPPORTED)(
OUT BOOLEAN* pIsChargingSupported
);
/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_qcomCharger_protocol
  @par Summary
  Qualcomm Charger Protocol interface.

  @par Parameters
  @inputprotoparams{qcom_charger_proto_params.tex} 
*/
struct _EFI_QCOM_CHARGER_PROTOCOL
{
  UINT64                                 Revision;
  EFI_QCOM_CHARGER_ENABLE_CHARGING       EnableCharging;
  EFI_QCOM_CHARGER_GET_MAX_USB_CURRENT   GetMaxUsbCurrent;
  EFI_QCOM_CHARGER_SET_MAX_USB_CURRENT   SetMaxUsbCurrent;
  EFI_QCOM_CHARGER_GET_CHARGING_ACTION   GetChargingAction;
  EFI_QCOM_CHARGER_TAKE_ACTION           TakeAction;
  EFI_QCOM_CHARGER_DISPLAY_IMAGE         DisplayImage;
  EFI_QCOM_CHARGER_GET_BATTERY_PRESENCE  GetBatteryPresence;
  EFI_QCOM_CHARGER_GET_BATTERY_VOLTAGE   GetBatteryVoltage;
  EFI_QCOM_CHARGER_DEINIT                ChargerDeInit;
  EFI_QCOM_CHARGER_GET_LOG_INFO          GetLogInfo;
  EFI_QCOM_CHARGER_DUMP_PERIPHERAL       DumpPeripheral;
  EFI_QCOM_CHARGER_IS_DCIN_VALID         IsDcInValid;
  EFI_QCOM_CHARGER_GET_CHARGER_CONFIG    GetChargerConfig;
  EFI_QCOM_CHARGER_IS_CHARGING_SUPPORTED IsChargingSupported;
};

#endif  /* __EFIQCCHARGER_H__*/
