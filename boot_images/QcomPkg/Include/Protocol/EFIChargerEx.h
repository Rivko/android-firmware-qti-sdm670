/** 
@file  EFIChargerEx.h
@brief Charger Extended Protocol for UEFI open source.
*/
/*=============================================================================
Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
All rights reserved.
Qualcomm Technologies Inc Confidential and Proprietary.
    
=============================================================================*/

#ifndef __EFICHARGEREX_H__
#define __EFICHARGEREX_H__

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
03/03/16   va      Adding offmode, flash APIs to contained for ABL, making open source and local header copies same since ABL need those and internally no calls being made
05/04/16   sm      New file.

=============================================================================*/

/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_ChargerEx_constants 
@{ */
/**
Protocol version.
*/

#define CHARGER_EX_REVISION 0x0000000000010003
/** @} */ /* end_addtogroup efi_ChargerEx_constants */

/*  Protocol GUID definition */
/** @ingroup efi_ChargerEx_protocol */
#define EFI_CHARGER_EX_PROTOCOL_GUID \
    { 0x6edc8a6d, 0x2663, 0x43cd, { 0x90, 0xff, 0x46, 0x21, 0xff, 0xd1, 0x0d, 0xf5 } } 

/** @cond */
/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/**
External reference to the Charger Extended Protocol GUID.
*/
extern EFI_GUID gChargerExProtocolGuid;

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
typedef enum {
  EFI_CHARGER_EX_POWER_FLASH_BATTERY_VOLTAGE_TYPE,
  EFI_CHARGER_EX_POWER_TYPE_INVALID = 0x7FFFFFFF
}EFI_CHARGER_EX_POWER_TYPE;


typedef struct {
  BOOLEAN bCanFlash;
  UINT32  BattCurrVoltage;
  UINT32  BattRequiredVoltage;
}EFI_CHARGER_EX_FLASH_INFO;

/**
Protocol declaration.
*/
typedef struct _EFI_CHARGER_EX_PROTOCOL   EFI_CHARGER_EX_PROTOCOL;
/** @endcond */

/** @addtogroup efi_ChargerEx_data_types 
@{ */


/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

/* EFI_CHARGER_EX_GET_CHARGER_PRESENCE */
/**@ingroup efi_charger_ex_get_charger_presence
  @par Summary
  This API returns charger presence status

  @param[out]  pChargerPresent   TRUE:Charger Present, FALSE:Charger not present

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_CHARGER_EX_GET_CHARGER_PRESENCE)
(
  OUT BOOLEAN  *pChargerPresent
);


/* EFI_CHARGER_EX_GET_BATTERY_VOLTAGE */
/**@ingroup efi_charger_ex_get_battery_voltage
  @par Summary
  This API returns battery voltage in mV

  @param[out]  pBatteryVoltagemV   Battery voltage in mV

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_CHARGER_EX_GET_BATTERY_VOLTAGE)
(
  OUT UINT32  *pBatteryVoltagemV
);

/* EFI_CHARGER_EX_GET_BATTERY_PRESENCE */
/**@ingroup efi_charger_ex_get_battery_presence
  @par Summary
  This API returns battery presence status

  @param[out]  pBatteryPresent   TRUE:Battery Present, FALSE:Battery not present

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI * EFI_CHARGER_EX_GET_BATTERY_PRESENCE)
(
  OUT BOOLEAN  *pBatteryPresent
);


/* EFI_CHARGER_EX_IS_OFFMODE_CHARGING */
/**@ingroup efi_charger_ex_is_offmode_charging
  @par Summary
  This API returns if device needs to off mode charging

  @param[out]  bOffModeCharging   TRUE:OffMode Charging is needed, Otherwise FALSE

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS ( EFIAPI * EFI_CHARGER_EX_IS_OFFMODE_CHARGING)
(
  OUT BOOLEAN *bOffModeCharging
);


/* EFI_CHARGER_EX_IS_POWER_OK */
/**@ingroup efi_charger_ex_is_power_ok
  @par Summary
  This API returns if device battery voltage is good for SW image flash

  @param[in]   FlashType     Refer EFI_CHARGER_EX_POWER_TYPE 
  @param[out]  PowerTypeInfo Refer required queried structure

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error.
*/
typedef
EFI_STATUS ( EFIAPI * EFI_CHARGER_EX_IS_POWER_OK)
(
  IN  EFI_CHARGER_EX_POWER_TYPE PowerType,
  OUT VOID                     *PowerTypeInfo
);


/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicCharger_protocol
@par Summary
PMIC Charger Protocol interface.

@par Parameters
@inputprotoparams{pmic_charger_proto_params.tex} 
*/

struct _EFI_CHARGER_EX_PROTOCOL {
  UINT64                                Revision;
  EFI_CHARGER_EX_GET_CHARGER_PRESENCE   GetChargerPresence;    // Returns if charger is present
  EFI_CHARGER_EX_GET_BATTERY_PRESENCE   GetBatteryPresence;    // Returns if battery is present
  EFI_CHARGER_EX_GET_BATTERY_VOLTAGE    GetBatteryVoltage;     // Returns battery voltage
  EFI_CHARGER_EX_IS_OFFMODE_CHARGING    IsOffModeCharging;
  EFI_CHARGER_EX_IS_POWER_OK            IsPowerOk;
};


#endif  /* __EFICHARGEREX_H__ */
