/** 
  @file  EFIPmicBmsVm.h
  @brief PMIC BMS voltage mode UEFI Protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------

 12/05/13   ps      Created.

=============================================================================*/
#ifndef __EFIPMICBMSVM_H__
#define __EFIPMICBMSVM_H__

/** @ingroup efi_pmicVmBms_constants
  Protocol version.
*/
#define PMIC_BMS_VM_REVISION 0x00010000

/* PMIC UEFI typedefs */
/** @cond */
typedef struct _EFI_QCOM_PMIC_BMS_VM_PROTOCOL  EFI_QCOM_PMIC_BMS_VM_PROTOCOL;
/** @endcond */

/*  PMIC VM BMS UEFI Protocol GUID */
/** @ingroup efi_pmicVmBms_protocol */
extern EFI_GUID gQcomPmicBmsVmProtocolGuid;

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
  /**< Estimated OCV value to be used in case of faulty OCV reading from BMS_VM HW */

  UINT32   BatterySettledTime;
  /**< Time within which the battery is expected to settle*/

} EFI_PM_BMS_VM_CFGDATA_TYPE;
/** @} */ /* end_addtogroup efi_pmicVmBms_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PMIC_BMS_VM_INIT */
/** @ingroup efi_pmicVmBmsInit
  @par Summary
  Initializes BMS_VM Module
 
  @param[in] None

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_VM_INIT)( VOID );

/* EFI_PMIC_BMS_VM_OCV_UPDATE_ENABLE */
/** @ingroup efi_pmicVmBmsOcvUpdateEnable
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
EFI_STATUS (EFIAPI *EFI_PMIC_BMS_VM_ENABLE_OCV_UPDATE)(
   IN UINT32 PmicDeviceIndex,
   IN BOOLEAN Enable
);

/* EFI_PMIC_BMS_VM_GET_CHARGE_CURRENT */
/** @ingroup efi_pmicVmBmsGetChargeCurrent
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
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_VM_GET_CHARGE_CURRENT)(
  IN UINT32 PmicDeviceIndex,
  OUT INT32 *ChargeCurrent
);

/* EFI_PMIC_BMS_VM_GET_SOC */
/** @ingroup efi_pmicVmBmsGetSoc
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
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_VM_GET_SOC)(
  IN UINT32 PmicDeviceIndex,
  OUT UINT32 *StateOfCharge
);

/* EFI_PMIC_BMS_VM_GET_RATED_CAPACITY */
/** @ingroup efi_pmicVmBmsGetRatedCapacity
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
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_VM_GET_RATED_CAPACITY)(
  IN UINT32 PmicDeviceIndex,
  OUT UINT32 *RatedCapacity
);

/* EFI_PMIC_BMS_VM_GET_RATED_CAPACITY */
/** @ingroup efi_pmicVmBmsGetRatedCapacity
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
EFI_STATUS(EFIAPI *EFI_PMIC_BMS_VM_GET_BATTERY_CONFIG_DATA)(
  OUT EFI_PM_BMS_VM_CFGDATA_TYPE *BatteryCfgData
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicVmBms_protocol
  @par Summary
  Qualcomm PMIC Battery Monitoring System (BMS_VM) Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_bms_proto_params.tex} 
*/
struct _EFI_QCOM_PMIC_BMS_VM_PROTOCOL
{
  UINT32                                   Revision;
  EFI_PMIC_BMS_VM_INIT                     BmsInit;
  EFI_PMIC_BMS_VM_ENABLE_OCV_UPDATE        EnableOcvUpdate;
  EFI_PMIC_BMS_VM_GET_CHARGE_CURRENT       GetChargeCurrent;
  EFI_PMIC_BMS_VM_GET_SOC                  GetSoc;
  EFI_PMIC_BMS_VM_GET_RATED_CAPACITY       GetRatedCapacity;
  EFI_PMIC_BMS_VM_GET_BATTERY_CONFIG_DATA  GetBatteryConfigData;
};

#endif  /* __EFIPMICVMBMS_H__*/
