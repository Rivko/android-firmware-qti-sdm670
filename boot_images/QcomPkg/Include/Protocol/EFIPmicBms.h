/** 
  @file  EFIPmicBms.h
  @brief PMIC BMS UEFI Protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2012 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/06/13   sm      Added EstimatedOcv, BatterySettledTime in Struct EFI_PM_BMS_CFGDATA_TYPE 
 03/05/13   sm      Added separated BattVoltLimHighInmV and BattCurrLimHighInmA variables 
                    for WP and WA in Struct EFI_PM_BMS_CFGDATA_TYPE 
 02/19/13   sm      Added API to get battery config data
 01/30/13   sm      Added API to initialize BMS parameters  
 10/17/12   dy      Add API to Get SOC, Capacity and Charge Current
 09/05/12   al      Removed resource index since there is single BMS
 07/27/12   al      Renamed all Enums,added device index and resource index
 07/24/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 05/17/12   al      Added API to set the enable/disable OCV
 05/17/12   al      Created.

=============================================================================*/
#ifndef __EFIPMICBMS_H__
#define __EFIPMICBMS_H__

/** @ingroup efi_pmicBms_constants
  Protocol version.
*/
#define PMIC_BMS_REVISION 0x0000000000010003

/* PMIC UEFI typedefs */
/** @cond */
typedef struct _EFI_QCOM_PMIC_BMS_PROTOCOL  EFI_QCOM_PMIC_BMS_PROTOCOL;
/** @endcond */

/*  PMIC Bms UEFI Protocol GUID */
/** @ingroup efi_pmicBms_protocol */
extern EFI_GUID gQcomPmicBmsProtocolGuid;

/**
  Battery Config Data.
*/
typedef struct 
{
  UINT32   CfgVersion;
  /**< Version number for CFG file*/

  INT32    BattTempLimLow;
  /**< Lower limit of Batery temperature value */
  INT32    BattTempLimHigh;
  /**< Upper limit of Batery temperature value */

  UINT32   BattIDLimLow;
  /**< Lower limit of Batery ID value */
  UINT32   BattIDLimHigh;
  /**< Upper limit of Batery ID value */

  UINT32   WPBattVoltLimHighInmV;
  /**< Max Value for battery voltage in mV for WP Platform*/  
  UINT32   WPBattCurrLimHighInmA;
  /**< Max value for battery current in mA for WP Platform*/

  UINT32   WABattVoltLimHighInmV;
  /**< Max Value for battery voltage in mV for WA Platform*/
  UINT32   WABattCurrLimHighInmA;
  /**< Max value for battery current in mA for WA Platform*/

  UINT32   BattRbattNomWP;
  /**< Value of Rbatt Nominal for WP platform */

  UINT32   BattRbattNomWA;
  /**< Value of Rbatt Nominal for WA platform */

  UINT32   BmsITestInmA;
  /**< Value of ITest in mA */
  UINT32   BmsVFailInuV;
  /**< Value of VFailure in uV */

  UINT32   ChgVddSafeInmV;
  /**< Value of VDD_SAFE in mV */
  UINT32   ChgIbatSafeInmA;
  /**< Value of IBAT_SAFE in mA */

  UINT32   ChgVddMaxInmV;
  /**< Value of VDD_MAX in mV */
  UINT32   ChgIbatMaxInmA;
  /**< Value of IBAT_MAX in mA */

  BOOLEAN  BadBattShutdown;
  /**< Shutdown the device if bad battery condition is TRUE */

  BOOLEAN  ReadBattProfileFromPartition;
  /**< Whether to Read Battery Profile form Partion */

  UINT32   BatteryGauge;
  /**< What type of Battery Gauge to use.
       See EFI_BATTERY_GAUGE_HW_TYPE for details. */

  INT32    ExternalRsense;
  /**< Value of External Resistance */

  BOOLEAN  PrintDebugMsg;
  /**< Flag to Enable/Disable printing debug messages.  */

  UINT32   RPCB;
  /**< Resistance of PCB Layout */

  UINT32   EstimatedOcv;
  /**< Estimated OCV value to be used in case of faulty OCV reading from BMS HW */

  UINT32   BatterySettledTime;
  /**< Time within which the battery is expected to settle*/

} EFI_PM_BMS_CFGDATA_TYPE;
/** @} */ /* end_addtogroup efi_pmicBms_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PMIC_BMS_INIT */
/** @ingroup efi_pmicBmsInit
  @par Summary
  Initializes BMS Module
 
  @param[in] None

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_INIT)( VOID );

/* EFI_PMIC_BMS_OCV_UPDATE_ENABLE */
/** @ingroup efi_pmicBmsOcvUpdateEnable
  @par Summary
  Enables/disables an OCV update. 
 
  @param[in]  PmicDeviceIndex  Pmic Device Index (0 for primary).
  @param[in]  Enable           If TRUE, enable an OCV update; if FALSE, disable an OCV 
                               update.  

  @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_BMS_ENABLE_OCV_UPDATE)(
   IN UINT32 PmicDeviceIndex,
   IN BOOLEAN Enable
);

/* EFI_PMIC_BMS_GET_CHARGE_CURRENT */
/** @ingroup efi_pmicBmsGetChargeCurrent
  @par Summary
  Get Charge Current
 
  @param[in]  PmicDeviceIndex    : Pmic Device Index (0 for primary)
  @param[out] ChargeCurrent      : Charge Current in mA

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_GET_CHARGE_CURRENT)(
  IN UINT32 PmicDeviceIndex,
  OUT INT32 *ChargeCurrent
);

/* EFI_PMIC_BMS_GET_SOC */
/** @ingroup efi_pmicBmsGetSoc
  @par Summary
  Get State of Charge of the battery
 
  @param[in]  PmicDeviceIndex    : Pmic Device Index (0 for primary)
  @param[out] StateOfCharge      : State of Charge of battery (%)

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_GET_SOC)(
  IN UINT32 PmicDeviceIndex,
  OUT UINT32 *StateOfCharge
);

/* EFI_PMIC_BMS_GET_RATED_CAPACITY */
/** @ingroup efi_pmicBmsGetRatedCapacity
  @par Summary
  Get full charge rated capacity of the battery
 
  @param[in]  PmicDeviceIndex    : Pmic Device Index (0 for primary)
  @param[out] RatedCapacity      : Full charge capacity of the battery (mAh)

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_GET_RATED_CAPACITY)(
  IN UINT32 PmicDeviceIndex,
  OUT UINT32 *RatedCapacity
);

/* EFI_PMIC_BMS_GET_RATED_CAPACITY */
/** @ingroup efi_pmicBmsGetRatedCapacity
  @par Summary
  Get full charge rated capacity of the battery
 
  @param[in]  PmicDeviceIndex    : Pmic Device Index (0 for primary)
  @param[out] RatedCapacity      : Full charge capacity of the battery (mAh)

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_GET_BATTERY_CONFIG_DATA)(
  OUT EFI_PM_BMS_CFGDATA_TYPE *BatteryCfgData
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicBms_protocol
  @par Summary
  Qualcomm PMIC Battery Monitoring System (BMS) Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_bms_proto_params.tex} 
*/
struct _EFI_QCOM_PMIC_BMS_PROTOCOL
{
  UINT64                                Revision;
  EFI_PMIC_BMS_INIT                     BmsInit;
  EFI_PMIC_BMS_ENABLE_OCV_UPDATE        EnableOcvUpdate;
  EFI_PMIC_BMS_GET_CHARGE_CURRENT       GetChargeCurrent;
  EFI_PMIC_BMS_GET_SOC                  GetSoc;
  EFI_PMIC_BMS_GET_RATED_CAPACITY       GetRatedCapacity;
  EFI_PMIC_BMS_GET_BATTERY_CONFIG_DATA  GetBatteryConfigData;
};

#endif  /* __EFIPMICBMS_H__*/
