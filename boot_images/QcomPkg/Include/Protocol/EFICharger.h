/** 
  @file  EFICharger.h
  @brief Charger UEFI Protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2011-2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies Inc, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/30/15   va      UEFI Thermal Handling support 
 07/16/15   al      Change cool off error to more generic to support warmup as well
 06/11/15   sm      Added EfiBatteryChargingLowVbattError error condition in 
                    _EFI_BATTERY_CHARGING_STATUS enum
 04/16/15   al      Added temp below error 
 01/19/15   al      Adding error type for handling cooling off 
 11/21/13   sv      Added BmsError to charging status
 11/25/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 09/13/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 07/17/13   al      Added EfiBatteryChargingAdcStatusError for ADC API calls fail
 05/06/13   sm      Added EFI_BATTERY_CHARGING_PROTOCOL_REVISION to 
                    EFI_BATTERY_CHARGING_PROTOCOL
                    Changed EFI_BATTERY_CHARGE_CURRENT_NOT_SUPPORTED to 0x80000000
 04/29/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.					
 06/28/12   dy      Add new charger error codes
 01/26/12   vishalo Merge in Techpubs Doxygen changes
 10/07/11    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 10/03/11   dy      Remove Revision from Protocol struct 
 06/11/11   dy      created

=============================================================================*/
#ifndef __EFICHARGER_H__
#define __EFICHARGER_H__

/** @addtogroup efi_charger_constants 
@{ */
/**
  Protocol version.
*/
#define EFI_BATTERY_CHARGING_PROTOCOL_REVISION          0x00010001

/**
  Battery charge current is not supported.
*/
#define EFI_BATTERY_CHARGE_CURRENT_NOT_SUPPORTED 0x80000000

/** @} */ /* end_addtogroup efi_charger_constants */

/** @cond */
/**
  Charger UEFI typedefs
*/
typedef struct _EFI_QCOM_CHARGER_PROTOCOL  EFI_QCOM_CHARGER_PROTOCOL;
/** @endcond */

/* Charger UEFI GUID */
/** @ingroup efi_charger_protocol */
extern EFI_GUID gBatteryChargerProtocolGuid;

/** @cond */
typedef struct _EFI_BATTERY_CHARGING_PROTOCOL EFI_BATTERY_CHARGING_PROTOCOL;
/** @endcond */

/** @addtogroup efi_charger_data_types 
@{ */
/**
  Charging status. This enumeration is used by the 
  #_EFI_BATTERY_CHARGING_COMPLETION_TOKEN data structure.
*/
typedef enum _EFI_BATTERY_CHARGING_STATUS { 
  EfiBatteryChargingStatusNone = 0,
  /**< No status. */
  
  EfiBatteryChargingStatusSuccess,
  /**< Operation completed successfully. */
  
  EfiBatteryChargingStatusOverheat,
  /**< Battery is getting too hot to charge. */
  
  EfiBatteryChargingStatusVoltageOutOfRange,
  /**< Charging logic detected the voltage to be out of the operational range. */
  
  EfiBatteryChargingStatusCurrentOutOfRange,
  /**< Charging logic detected the current to be out of the operational range. */
  
  EfiBatteryChargingStatusTimeout,
  /**< Charging logic detected that the battery is not being charged within
    a reasonable time. */
  
  EfiBatteryChargingStatusAborted,
  /**< Operation was aborted. */

  EfiBatteryChargingStatusDeviceError,
  /**< Physical device reported an error. */
  
  EfiBatteryChargingStatusExtremeCold,
  /**< Battery is too cold to continue charging. */
  
  EfiBatteryChargingStatusBatteryChargingNotSupported,
  /**< Battery does not support the charging operation. */
  
  EfiBatteryChargingStatusBatteryNotDetected,
  /**< Battery is not detected. */
  
  EfiBatteryChargingSourceNotDetected,
  /**< Phone is not attached to a charging source and cannot continue with 
    the charging operation. */
  
  EfiBatteryChargingSourceVoltageInvalid,
  /**< Charging source supplied an invalid voltage. */
  
  EfiBatteryChargingSourceCurrentInvalid,
  /**< Charging source supplied an invalid current. */
  
  EfiBatteryChargingErrorRequestShutdown,
  /**< Driver requested a system shutdown. */
  
  EfiBatteryChargingErrorRequestReboot,
  /**< Driver requested a system reboot. */

  EfiBatteryChargingAdcStatusError,
  /**< ADC error was detected during the charging operation. */

  EfiBatteryChargingFgError,
  /**< Fuel gauge error detected. */
  
  EfiBatteryChargingBmsError,
  /**< Fuel gauge error detected. */

  EfiBatteryChargingNoChargeAndWait,
  /**< Battery temperature outside charging range */

  EfiBatteryChargingLowVbattError,
  /**< Battery Voltage below the emergency shutdown limit. */

  EfiDeviceThemalCriticalError,
  /**< MSM thermal critical error for shutdown */

} /** @cond */EFI_BATTERY_CHARGING_STATUS/** @endcond */;

/* Token to be returned on with charge completion event */
/**
  Charge completion token. 
*/
typedef struct _EFI_BATTERY_CHARGING_COMPLETION_TOKEN {
  EFI_EVENT                     Event;
  /**< Charge completion. */
  EFI_BATTERY_CHARGING_STATUS   Status;
  /**< Charging status; see #_EFI_BATTERY_CHARGING_STATUS for details. */
} /** @cond */EFI_BATTERY_CHARGING_COMPLETION_TOKEN/** @endcond */;
/** @} */ /* end_addtogroup efi_charger_data_types */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_BATTERY_CHARGING_GET_BATTERY_STATUS */ 
/** @ingroup efi_charger_get_battery_status
  @par Summary
  Returns information about the current state of the main battery.

  @param[in]  This               Pointer to the 
                                 EFI_BATTERY_CHARGING_PROTOCOL instance.
  @param[out] StateOfCharge      Current State of Charge (SoC) of the main 
                                 battery. SoC is represented in percentage; 
                                 100% indicates a full charge.
  @param[out] RatedCapacity      Rated capacity of the battery in mAh.                     
  @param[out] ChargeCurrent      Charge current as follows:
                                 - If the battery is charging, returns 
                                   a positive number indicating the current 
                                   delivered to the battery in mA 
                                 - If the battery is discharging, returns 
                                   a negative number indicating the current 
                                   being drawn from the battery in mA 
                                 - If the battery is neither charging nor 
                                   discharging, zero is returned 
                                 - If the hardware is unable to provide 
                                   the information, 
                                   EFI_BATTERY_CHARGE_CURRENT_NOT_SUPPORTED 
                                   is returned
                                 @tablebulletend

  @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request, or the device is not
                           attached to the host.
*/
typedef
EFI_STATUS (EFIAPI * EFI_BATTERY_CHARGING_GET_BATTERY_STATUS)(
  IN EFI_BATTERY_CHARGING_PROTOCOL *This,
  OUT UINT32 *StateOfCharge,
  OUT UINT32 *RatedCapacity,
  OUT INT32 *ChargeCurrent
);


/* EFI_BATTERY_CHARGING_CHARGE_BATTERY */ 
/** @ingroup efi_charger_charge_battery
  @par Summary
  Charges the main battery to the specified target level with a maximum
  current charge.
 
  @param[in] This                      Pointer to the 
                                       EFI_BATTERY_CHARGING_PROTOCOL instance.
  @param[in] MaximumCurrent            Optional parameter that allows 
                                       the caller to specify the maximum current 
                                       in mA that can be used to charge the 
                                       main battery. A NULL value prompts the 
                                       driver implementing this protocol to 
                                       handle such details on its own.
  @param[in] TargetStateOfCharge       Target SoC of the main
                                       battery after which the function will
                                       return if CompletionToken is NULL. SoC is
                                       represented in percentage; 100% indicates
                                       a full charge.                 
  @param[in] CompletionToken           Pointer to a token associated with the 
                                       requested charge operation; see 
                                       #_EFI_BATTERY_CHARGING_COMPLETION_TOKEN 
                                       for details. 

  @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request, or the device is not
                           attached to the host.
*/
typedef
EFI_STATUS (EFIAPI * EFI_BATTERY_CHARGING_CHARGE_BATTERY)(
  IN EFI_BATTERY_CHARGING_PROTOCOL *This,
  IN UINT32 MaximumCurrent OPTIONAL,
  IN UINT32 TargetStateOfCharge,
  IN EFI_BATTERY_CHARGING_COMPLETION_TOKEN *CompletionToken
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_charger_protocol
  @par Summary
  Battery Charger Protocol interface.

  @par Parameters
  @inputprotoparams{charger_proto_params.tex} 
*/
struct _EFI_BATTERY_CHARGING_PROTOCOL {
  EFI_BATTERY_CHARGING_GET_BATTERY_STATUS   GetBatteryStatus;
  EFI_BATTERY_CHARGING_CHARGE_BATTERY       ChargeBattery;
  UINT32                                    Revision;
};

#endif  /* __EFICHARGER_H__ */
