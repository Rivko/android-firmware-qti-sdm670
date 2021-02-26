/** 
  @file  EFIPmicFg.h
  @brief PMIC Fuel Gauge (FG) UEFI Protocol definitions.
           Estimates Battery State of Charge (SOC) and holds other required Battery parameters Information
           For E.g. Battery Charge Current, Voltage
*/
/*=============================================================================
  Copyright (c) 2014 - 2017 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
12/28/17   pbitra   API to enable BMD explicitly
06/07/17    cs      add configurable delay before enabling BMD
03/30/17    ll      Implemented level file-logging
01/31/17    va      Adding Rconn configuration for considering compensation
01/23/17    sm      CFG parameter Structure changes
12/19/16    sm      Added API to get FG PmicIndex
11/10/16    va      Adding Config VBtEmpty threshold
11/07/16    cs      Added API to disable ESR Pulse
10/14/16    va      adding aux coffs settings 
10/06/16    cs      configure batt therm/aux therm bias wait 
08/24/16    sm      Removed SW JEITA related code
07/25/16    va      Adding support for Restart FG condition
06/23/16    va      Adding support for Charger Fg Peripheral dumps
05/20/16    va      Update for Profile load and struct mis match 
04/26/16    va      Adding parameters for Aux Coffes, SkinHot and Charger Hot settings
03/28/16    va      Consolidating config data into one struct 
03/22/16    va      Updating FG Protocol
01/27/16    sm      Split Init and Exit APIs for LA v/s WP cases
12/01/15    va      Added charger fg peripheral dump
11/30/15    va      UEFI Thermal Handling support 
07/16/15    al      Adding variable to store out of charging temp range behaviour
06/11/15    sm      Added EmergencyShutdownVbatt config parameter
05/27/15    sm      Added OsNonStandardBootSocThreshold and OsStandardBootSocThreshold 
                    config parameters
04/30/15    va      IChgMax and VddMax configuration support for all Jeita windows
                    JEITA Low temp, high temp handle 
04/16/15    al      Added FG restart time
04/30/15    al      Make FCC calibration execution configurable
04/09/15    va      API to read Battery temperature validity
02/25/15    sm      Added API to read SRAM shadow register
02/20/15    al      Create separate struct for battery id related
01/19/15    al      Creating separate struct for JEITA
01/05/15    va      Added Multiple Battery Profile and Profile Parser Status API
09/29/14    va      Jeita Feature
09/25/14    va      Update for New Battery Profile Format
09/23/14    al      Adding charging termination current variable 
08/21/14    va      Enable File Logging 
08/13/14    sm      Added APIs to return Rated capacity and battery temperature
06/20/14    va      Initial Draft

=============================================================================*/

#ifndef __EFIPMICFG_H__
#define __EFIPMICFG_H__

/*===========================================================================
INCLUDE FILES
===========================================================================*/

/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/**
  Protocol version.
*/
#define PMIC_FG_REVISION 0x0000000000010016
/** @} */ /* end_addtogroup efi_pmicfg_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicSmbchg_protocol */
#define EFI_PMIC_FG_PROTOCOL_GUID \
  { 0x9aed0312, 0x724a, 0x4209, { 0x86, 0x20, 0xf9, 0x3d, 0xe0, 0xbb, 0xd8, 0xfb } }

/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/**
External reference to the PMIC Fg UEFI Protocol GUID.
*/

extern EFI_GUID gQcomPmicFgProtocolGuid;


/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_FG_BASIC_PROTOCOL  EFI_QCOM_PMIC_FG_BASIC_PROTOCOL;

typedef struct _EFI_QCOM_PMIC_FG_EXT_PROTOCOL    EFI_QCOM_PMIC_FG_EXT_PROTOCOL;

typedef struct
{
  UINT64 BattId;
}EFI_PM_BATTERY_ID;

typedef struct
{
  UINT32  BatteryIdTolerance;
  /**< Battery ID Tolerance Percentage */

  UINT32  DebugBoardBatteryIdMin;
  /**<  Debug board Battery ID Range Min */

  UINT32  DebugBoardBatteryIdMax;
  /**<  Debug board Battery ID Range Max */

  UINT32 RegularBatteryIdMin;
  /**< Regular Battery ID Range Min */

  UINT32 RegularBatteryIdMax;
  /**<  Regular Battery ID Range Max */

  UINT32 SmartBatteryIdMin;
  /**< Smart Battery ID Range Min */

  UINT32 SmartBatteryIdMax;
  /**<  Smart Battery ID Range Max */

  UINT32 BattMissingDelay;
  /**<  delay in msec before enable batt missing */

}EFI_PM_BATTID_CFGDATA_TYPE;

typedef struct 
{
  BOOLEAN  ProgramAuxThermCoeffs;
  /**<  Enables Aux therm coeffs to be programmed via configs instead of profiles */

  INT32   AuxThermC1;
  /**<  Battery Aux therm coeffs*/

  INT32   AuxThermC2;
  /**<  Battery Aux therm coeffs*/

  INT32   AuxThermC3;
  /**<  Battery Aux therm coeffs*/

}EFI_PM_AUX_THERM_COEFFS;

typedef struct 
{
  INT32   BattThermC1;
  /**<  Battery Batt therm coeffs*/

  INT32   BattThermC2;
  /**<  Battery Batt therm coeffs*/

  INT32   BattThermC3;
  /**<  Battery Batt therm coeffs*/

}EFI_PM_BATT_THERM_COEFFS;

typedef struct
{
  BOOLEAN  ProgramSkinAndChargerHotThreshold;
  /**<  Enables Pmic Skin and Charger Hot Threshols to be programmed via configs instead of profiles */
  INT32   DeviceSkinHotInC;
  /**<  Device Skin hot */
  INT32   DeviceSkinTooHotInC;
  /**<  Device Skin too hot */
  INT32   ChargerHotInC;
  /**<  Charger Hot Threshold */
  INT32   ChargerTooHotInC;
  /**<  Charger too Hot Threshold */

}EFI_PM_SKIN_CHARGERHOT_THRESHOLD;

typedef struct
{
  INT32    JeitaCriticalTempLowLimit;
  /**< Lower limit of Batery temperature value to perform AFP*/

  INT32    JeitaHardColdLimit; // put all variable for error in one struct..phase 2
  /**< Lower limit of Batery temperature value to stop charging*/

  INT32    JeitaSoftColdLimit;
  /**< Lower limit of Batery temperature value to lower charging parameters*/

  INT32    JeitaSoftHotLimit;
  /**< Upper limit of Batery temperature value to lower charging parameters*/

  INT32    JeitaHardHotLimit;
  /**< Upper limit of Batery temperature value to stop charging*/

  INT32    JeitaCriticalTempHighLimit;
  /**< Upper limit of Batery temperature value to perform AFP*/

  BOOLEAN JeitaSoftColdFvCompEnable;
  /**< Enable/Disable Float Voltage compensation during soft cold*/

  BOOLEAN JeitaSoftHotFvCompEnable;
  /**< Enable/Disable Float Voltage compensation during soft hot*/

  BOOLEAN JeitaSoftColdCcCompEnable;
  /**< Enable/Disable Charge Current compensation during soft cold*/

  BOOLEAN JeitaSoftHotCcCompEnable;
  /**< Enable/Disable Charge Current compensation during soft cold*/
}EFI_PM_HW_JEITA_THRESHOLD;


typedef struct
{
  BOOLEAN  DumpSram;
  /**< Dump SRAM contents */
  UINT32   DumpSramStartAddr;
  /**< #SRAM Block    SRAM Address
    #dump SRAM Start and End Address in Hex Format*/
  UINT32   DumpSramEndAddr;
  /**<  End Address */
  UINT32   DumpSramDuration;
  /**<  dump SRAM contents timer Duration in s */
}EFI_PM_FG_SRAM_DEBUG_CFGS;


typedef struct
{
  UINT32 BiasWait;
  BOOLEAN GroundSel;
}EFI_PM_FG_THERM_CONFIG;


typedef struct {

  BOOLEAN  PrintChargerAppDbgMsgToFile;
  /**< Flag to Enable/Disable Charger debug messages to File.  */

  BOOLEAN  PrintChargerAppDbgMsg;
  /**< Flag to Enable/Disable Charger debug messages.  */

  BOOLEAN  EnableChargerFgDump;
  /**< Flag to Enable/Disable Charger FG periperal debugs*/

  UINT32 FileLoggingLevelMask;
  /**< Debug level mask for logging to file */
  /** refer to DebugLib.h */

  /* add other debug config here*/

}EFI_PM_FG_DEBUG_CFGDATA_TYPE;

typedef struct
{
  UINT32   ChargingTermCurrent;
  /**<  Charging termination current*/

  UINT32   BattMissingCfg;
  /**<  Battery Missing Config Param*/

  BOOLEAN  FgCondRestart;
  /**<  FG Conditional Restart on reset */

  UINT32   VBattEstDiffThreshold;
  /**<  FG Fuel Gauge Voltage Battery Threshold difference to decide FG Restart condition at FG Algo start */

  BOOLEAN  LoadBatteryProfile;
  /**<  Load Fuel Gauge Battery Profile profile for SOC estimation and accuracy */

  UINT32   VBatEmpty;
  /**<  VBat Empty threshold for empty interrupt */

  INT32    RconnComp;
  /**< Rconn compensation */

  EFI_PM_AUX_THERM_COEFFS           AuxThermCoeffs;
  /**<  Battery Aux Therm Coffes*/

  EFI_PM_HW_JEITA_THRESHOLD         HwJeitaThreshold;
  /**<  HW Jeita Thresholds used when HW Jeita is enabled */

  EFI_PM_SKIN_CHARGERHOT_THRESHOLD  SkinAndChargerHotThreshold;
  /**<  Battery Pmic Skin and Charger Hot Threshols */

  EFI_PM_FG_SRAM_DEBUG_CFGS         FgSramDbgCfg;
  /**<  SRAM Debug configs */

  EFI_PM_FG_THERM_CONFIG            FgBattThermCfg;
  /**<  batt therm configs */

  EFI_PM_FG_THERM_CONFIG            FgAuxThermCfg;
  /**<  aux therm configs */

  EFI_PM_BATT_THERM_COEFFS          FgBattThermCoffs;
  /**<  batt therm configs */
  
  EFI_PM_BATTID_CFGDATA_TYPE        FgBattIdCfg;

  EFI_PM_FG_DEBUG_CFGDATA_TYPE      FgDbgCfg;

}EFI_PM_FG_CFGDATA_TYPE;

typedef struct
{
  BOOLEAN jeitaHardHot;
  BOOLEAN jeitaSoftHot;
  BOOLEAN jeitaHardCold;
  BOOLEAN jeitaSoftCold;
}EFI_PM_FG_BATT_TEMP_STATUS;

typedef struct _EFI_PM_FG_BATT_STATUS_INFO
{
  UINT32 StateOfCharge;
  INT32  ChargeCurrent;
  INT32  BatteryVoltage;
  INT32  BatteryTemperature;
}EFI_PM_FG_BATT_STATUS_INFO;

typedef enum
{
  PM_FG_BATT_PROFILE_LOAD,
  PM_FG_BATT_PROFILE_RESTART,
  PM_FG_BATT_PROFILE_INVALID
}EFI_PM_FG_BATT_PROFILE_STATE;

typedef struct
{
  /* Enum for profile load/restart state */
  EFI_PM_FG_BATT_PROFILE_STATE eProfileState;
}EFI_PM_FG_BATT_PROFILE_STATUS;

typedef enum _PM_FG_BATT_INFO_BM_THERM_TH{
  PM_FG_BATT_INFO_RES_TH_0P75_MOHM,
  PM_FG_BATT_INFO_RES_TH_1P00_MOHM,
  PM_FG_BATT_INFO_RES_TH_1P50_MOHM,
  PM_FG_BATT_INFO_RES_TH_3P00_MOHM,
  PM_FG_BATT_INFO_BATT_BH_THERM_TH_INVALID,
}PM_FG_BATT_INFO_BM_THERM_TH;

typedef struct
{
  PM_FG_BATT_INFO_BM_THERM_TH BmThermTh;
  BOOLEAN                     BmFromTherm;
  BOOLEAN                     BmFromBattId;
}PM_FG_BATT_INFO_BATT_MISSING_CFG;

typedef struct
{
  UINT8 *pFileBuffer;
  UINTN  DataSize;
}EFI_PM_FG_BATT_PROFILE_DATA;

typedef struct
{
  UINT32 PmicIndex;
  UINT32 SlaveIndex;
} EFI_PM_FG_PMIC_INFO;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PMIC_FG_BASIC_INIT */
/** @ingroup efi_pmicFgInit
  @par Summary
  Initializes FG Module

  @param[in] PmicDeviceIndex: Pmic Device Index (0 for primary)
  @param[in] FgCfgData: Fg Cfg data (EFI_PM_FG_CFGDATA_TYPE)
  @param[in] DbgCfgData: Debug config data (EFI_CHARGER_DEBUG_CFGDATA_TYPE )

 @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_BASIC_INIT)(
  IN UINT32                      PmicDeviceIndex,
  IN EFI_PM_FG_CFGDATA_TYPE     *pFgCfgData
);

/* EFI_PMIC_FG_EXTENDED_INIT */
/** @ingroup efi_pmicFgExtendedInit
  @par Summary
  Initializes FG Module

  @param[in] PmicDeviceIndex: Pmic Device Index (0 for primary)

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_EXTENDED_INIT)(
  IN UINT32                      PmicDeviceIndex,
  OPTIONAL EFI_PM_FG_BATT_PROFILE_DATA *pBattProfileData
);
/* EFI_PMIC_FG_EXIT */
/** @ingroup efi_pmicFgExit
  @par Summary
  Exit FG Module

  @param[in] PmicDeviceIndex: Pmic Device Index (0 for primary)

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_BASIC_EXIT)( 
  IN UINT32 PmicDeviceIndex
);


/* EFI_PMIC_FG_EXTENDED_EXIT */
/** @ingroup efi_pmicFgExit
  @par Summary
  Exit FG Module

  @param[in] PmicDeviceIndex: Pmic Device Index (0 for primary)

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_EXTENDED_EXIT)(
  IN UINT32 PmicDeviceIndex
);

/* EFI_PMIC_FG_GET_BATTERY_STS_INFO */
/** @ingroup efi_pmicFgGetBatteryStsInfo
  @par Summary
  Get Battery Status Information

  @param[in]  PmicDeviceIndex    : Pmic Device Index (0 for primary)
  @param[out] pBattStsInfo      :  Battery status info

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_GET_BATTERY_STS_INFO)(
  IN  UINT32          PmicDeviceIndex,
  OUT EFI_PM_FG_BATT_STATUS_INFO *pBattStsInfo
);


/* EFI_PMIC_FG_GET_RATED_CAPACITY */
/** @ingroup efi_pmicFgGetRatedCapacity
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
EFI_STATUS(EFIAPI *EFI_PMIC_FG_GET_RATED_CAPACITY)(
  IN UINT32 PmicDeviceIndex,
  OUT UINT32 *RatedCapacity
);

/* EFI_PMIC_FG_SET_BATTERY_CONFIG_DATA */
/** @ingroup efi_pmicFgSetRatedCapacity
  @par Summary
  Set config data for battery

  @param[in] BatteryCfgData  : Battery config Data

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_SET_BATTERY_CONFIG_DATA)(
  OUT EFI_PM_FG_CFGDATA_TYPE *BatteryCfgData
);

/* EFI_PMIC_FG_GET_BATT_ID */
/** @ingroup efi_pmicFgGetBatteryId
  @par Summary
  Get battery id

  @param[out] BattId           : Battery id

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_GET_BATTERY_ID)(
  OUT EFI_PM_BATTERY_ID *BatteryId
);

/* EFI_PMIC_FG_DUMP_PERIPHERAL */
/** @ingroup efi_pmicFgDumpPeripheral
  @par Summary
    Dumps FG Pheripherals

  @param: Void

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_DUMP_PERIPHERAL )(
  VOID
);

/* EFI_PMIC_FG_CONFIGURE_HW_JEITA */
/** @ingroup efi_pmicFgConfigureHwJeita
  @par Summary
   Configures HW JEITA parameters

  @param[in]  PmicDeviceIndex   : Pmic Device Index (0 for primary)

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_CONFIGURE_HW_JEITA )(
  IN UINT32 PmicDeviceIndex
);

/* EFI_PMIC_FG_GET_HW_JEITA_STATUS */
/** @ingroup efi_pmicFgGetHwJeitaStatus
  @par Summary
   Returns HW JEITA Status

  @param[in]  PmicDeviceIndex  : Pmic Device Index (0 for primary)
  @param[out] pHwJeitaStatus   : HW JEITA Status
                                 see PM_FG_JEITA_STATUS for more details.

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_FG_GET_HW_JEITA_STATUS)(
  IN  UINT32                       PmicDeviceIndex,
  OUT EFI_PM_FG_BATT_TEMP_STATUS  *pHwJeitaStatus
);

/* EFI_PMIC_FG_GET_BATT_MISSING_CFG */
/** @ingroup efi_pmicFgGetBattMissingCfg
  @par Summary
   Returns Battery Missing Cfg Status

  @param[in]  PmicDeviceIndex  : Pmic Device Index (0 for primary)
  @param[out] pBattMissingCfg  : Battery Missing Cfg Status
                                 see PM_FG_BATT_INFO_BATT_MISSING_CFG for more details.

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_FG_GET_BATT_MISSING_CFG)(
  IN  UINT32                             PmicDeviceIndex,
  OUT PM_FG_BATT_INFO_BATT_MISSING_CFG  *pBattMissingCfg
);

/* EFI_PMIC_FG_SET_BATT_MISSING_CFG */
/** @ingroup efi_pmicFgSetBattMissingCfg
  @par Summary
   Returns Battery Missing Cfg Status

  @param[in]  PmicDeviceIndex  : Pmic Device Index (0 for primary)
  @param[out] BattMissingCfg   : Set Battery Missing Cfg Status
                                 see PM_FG_BATT_INFO_BATT_MISSING_CFG for more details.

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_FG_SET_BATT_MISSING_CFG)(
  IN UINT32                            PmicDeviceIndex,
  IN PM_FG_BATT_INFO_BATT_MISSING_CFG  BattMissingCfg
);


/* EFI_PMIC_FG_GET_BATT_PROFILE_STATUS */
/** @ingroup efi_pmicFgGetBatteryProfileStatus
  @par Summary
  Get Battery Profile Status Information

  @param[in]  PmicDeviceIndex    : Pmic Device Index (0 for primary)
  @param[out] pBattProfileStatus :  Battery Profile status (profile flashed status)

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_GET_BATT_PROFILE_STATUS)(
  IN  UINT32                         PmicDeviceIndex,
  OUT EFI_PM_FG_BATT_PROFILE_STATUS *pBattProfileSts
);

/* EFI_PMIC_FG_DUMP_SRAM */
/** @ingroup efi_pmicFgDumpPeripheral
  @par Summary
    Dumps FG SRAM
  @param: Void

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_DUMP_SRAM)(
  IN UINT32 PmicDeviceIndex
);

/* EFI_PMIC_FG_ENABLE_ESR_PULSE */
/** @ingroup efi_pmicFgEnableESRPulse
@par Summary
enable/disable ESR pulses.

 @param[in]:PmicDeviceIndex: Pmic Device Index (0 for primary, 1 for PMI)
 @param[in]: enable: TRUE to enable ESR pulse (default already enabled. 
                FALSE to disable ESR pulse. If not using FG for battery percentage reading or aging.

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_INVALID_PARAMETER -- Parameter is invalid. \n
EFI_DEVICE_ERROR      -- Physical device reported an error. \n
EFI_NOT_READY         -- Physical device is busy or not ready to
process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_ENABLE_ESR_PULSE)(
  IN UINT32 PmicDeviceIndex,
  IN BOOLEAN enable
);

/* EFI_PMIC_FG_BASIC_GET_PMIC_INFO */
/** @ingroup efi_pmicFgInit
  @par Summary
  Provides PMIC Index for FG Module

  @param[out] FgPmicInfo   Pmic Device Index (0 for primary)

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_BASIC_GET_PMIC_INFO)(
  OUT EFI_PM_FG_PMIC_INFO *FgPmicInfo
);

/* EFI_PMIC_FG_ENABLE_BMD */
/** @ingroup efi_pmicFgEnableBMD
  @par Summary
  Enable BMD
  @param: Void

 @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. \n
  EFI_NOT_READY         -- Physical device is busy or not ready to
                           process this request.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PMIC_FG_ENABLE_BMD)(
  IN UINT32 PmicDeviceIndex
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicFg_protocol
  @par Summary
  Qualcomm PMIC Battery Monitoring System (FG) Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_fg_proto_params.tex} 
*/
struct _EFI_QCOM_PMIC_FG_BASIC_PROTOCOL
{
  UINT64                                 Revision;
  EFI_PMIC_FG_BASIC_INIT                 FgBasicInit;
  EFI_PMIC_FG_EXTENDED_INIT              FgExtendedInit;
  EFI_PMIC_FG_GET_BATTERY_STS_INFO       GetBatteryStatus;
  EFI_PMIC_FG_GET_BATT_PROFILE_STATUS    GetBattProfileStatus;
  EFI_PMIC_FG_GET_BATTERY_ID             GetBatteryId;
  EFI_PMIC_FG_CONFIGURE_HW_JEITA         ConfigureHwJeita;
  EFI_PMIC_FG_GET_HW_JEITA_STATUS        GetHwJeitaStatus;
  EFI_PMIC_FG_GET_BATT_MISSING_CFG       GetBattMissingCfg;
  EFI_PMIC_FG_SET_BATT_MISSING_CFG       SetBattMissingCfg;
  EFI_PMIC_FG_BASIC_EXIT                 FgBasicExit;
  EFI_PMIC_FG_EXTENDED_EXIT              FgExtendedExit;
  EFI_PMIC_FG_DUMP_PERIPHERAL            DumpPeripheral;
  EFI_PMIC_FG_DUMP_SRAM                  DumpFgSram;
  EFI_PMIC_FG_ENABLE_ESR_PULSE           FgEnableESRPulse;
  EFI_PMIC_FG_BASIC_GET_PMIC_INFO        FgBasicGetPmicInfo;
  EFI_PMIC_FG_ENABLE_BMD                 FgEnableBMD;
};

/** @ingroup efi_pmicFg_protocol
  @par Summary
  Qualcomm PMIC Battery Monitoring System (FG) Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_fg_proto_params.tex} 
*/
struct _EFI_QCOM_PMIC_FG_EXT_PROTOCOL
{
  UINT64                                 Revision;
  EFI_PMIC_FG_BASIC_INIT                 FgBasicInit;
  EFI_PMIC_FG_EXTENDED_INIT              FgExtendedInit;
  EFI_PMIC_FG_GET_BATTERY_STS_INFO       GetBatteryStatus;
  EFI_PMIC_FG_GET_BATT_PROFILE_STATUS    GetBattProfileStatus;
  EFI_PMIC_FG_GET_BATTERY_ID             GetBatteryId;
  EFI_PMIC_FG_CONFIGURE_HW_JEITA         ConfigureHwJeita;
  EFI_PMIC_FG_GET_HW_JEITA_STATUS        GetHwJeitaStatus;
  EFI_PMIC_FG_GET_BATT_MISSING_CFG       GetBattMissingCfg;
  EFI_PMIC_FG_SET_BATT_MISSING_CFG       SetBattMissingCfg;
  EFI_PMIC_FG_BASIC_EXIT                 FgBasicExit;
  EFI_PMIC_FG_EXTENDED_EXIT              FgExtendedExit;
  EFI_PMIC_FG_DUMP_PERIPHERAL            DumpPeripheral;
  EFI_PMIC_FG_DUMP_SRAM                  DumpFgSram;
  EFI_PMIC_FG_ENABLE_ESR_PULSE           FgEnableESRPulse;
  EFI_PMIC_FG_BASIC_GET_PMIC_INFO        FgBasicGetPmicInfo;
  EFI_PMIC_FG_ENABLE_BMD                 FgEnableBMD;
};

#endif  /* __EFIPMICFG_H__*/
