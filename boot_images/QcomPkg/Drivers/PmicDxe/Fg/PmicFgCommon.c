/*! @file PmicFgCommon.c
 *
 *  PMIC-FG MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support
 *  the PMIC FG commone file.
 *
 *  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
12/28/17   pbitra API to enable BMD explicitly
07/12/17   va     Disable battery id sync mode in dxe and enable BMD when chargerapp comes
07/03/16   va     Do One time configuration on all boot, dvdd reset only on 20th addr data non zero
07/07/17   cs     set batt missing cfg to 2, batt missing is not getting detected.
06/09/17   va     Detect the bad conditions of FG SRAM (trim registers) and issue VDD hard reset on PM/PMi to recover
06/07/17   cs     add configurable delay before enabling BMD
03/30/17   ll     Implemented level file-logging
03/15/17   cs     fix SBL 160 ms extra delay due to getting fresh batt id issue
01/27/17   ai     Fix use of retail
01/23/17   sm      CFG parameter Structure changes
12/19/16   sm     Added API to get FG PmicIndex
11/10/16   va     Removing SOC EMPTY IRQ
11/07/16   cs     Added API to disable ESR Pulse
10/14/16   va     update Aux setting 
10/11/16   va     Read profile on every boot, compare profile data vs sram data
10/06/16   cs     configuring batt therm/aux therm
09/20/16   sv     fixing klocwork issues
09/20/16   va     Enable Restarting FG 
07/26/16   va     Restarting FG Changes on warm boot 
06/23/16   va     Adding support for Charger Fg Peripheral dumps
06/14/16   va     Updating calculations for jeita thresholds, Reading IBat, Vbat from batt info
05/26/16   va     Update for Profile Load
05/24/16   sm     Added API calls to Enable FG Algorithm, BCL and GOC 
04/26/16   va     Adding parameters for Aux Coffes, SkinHot and Charger Hot settings
03/28/16   va     Consolidating config data into one struct 
03/21/16   va     New File.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/**
  EFI interfaces
 */
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIPmicFg.h>
#include <Protocol/EFIPmicPwrOn.h>
#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/EFIChipInfo.h>
#include <Library/QcomTargetLib.h>
#include <Library/FuseControlLib.h>

#include "DDIPlatformInfo.h"

/**
SPMI depedency
*/
#include "SpmiBus.h"

/**
  PMIC Lib interfaces
 */
#include "pm_uefi.h"
#include "PmicFileLog.h"
#include "PmicFgCommon.h"

#include "pm_fg_batt_soc.h"
#include "pm_fg_batt_info.h"
#include "pm_fg_batt_soc.h"
#include "pm_fg_bcl.h"
#include "pm_version.h"
#include "pm_schg_batif.h"
#include "pm_schg_chgr.h"
#include "pm_schg_misc.h"
#include "pm_fg_mem_if.h"
#include <../../Library/PmicLib/utils/inc/pm_utils.h>
#include "pm_err_flags.h"
#include "pm_core_utils.h"
#include "pm_fg_adc_rr.h"


/**
  PMIC Fg interface
 */
#include <PmicFgBattProfile.h>
#include <PmicFgSram.h>

/**
QCom Lib dependency
*/
//#include <Library/QcomLib.h>
//#include <Library/MemoryAllocationLib.h>
#include "string.h"
#include <Library/BaseMemoryLib.h>

/**
  ADC Dependencies
 */
#include <Protocol/EFIAdc.h>

#include "pm_fg_adc_rr.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
/* FG Peripheral dump */
#define PMIC_FG_PERIPHERAL_START_ADDR   0x4000
#define PMIC_FG_PERIPHERAL_NUM          6

#define INCREMENT_ONE                   1
#define DECREMENT_ONE                  -1

#define PMIC_FG_TEMP_CONV_BASE          30
#define PMIC_FG_TEMP_CONV_1_LSB         2  /* (-30 + (THRESHOLD * 0.5) C;)*/

#define PMIC_FG_BATT_ID_CONV_WAIT   10000 /* 10 msec */
/*max wait for 200 msec, this only will happen for high batt id with large capacitor. normally 10 msec is enough for hig batt id*/
#define PMIC_FG_MAX_BATT_ID_CONV_COUNT   20 

#define PMIC_FG_MAX_BMD_ENABLE_DELAY     500


/*===========================================================================

                        GLOBAL DATA DEFINITIONS

===========================================================================*/
EFI_PM_FG_CFGDATA_TYPE      gFgConfigData = {0};
STATIC UINT32               gFgPmicIndex  = 0, gFgSlaveIndex = 0;
extern EFI_GUID gQcomPmicPwrOnProtocolGuid;

/*===========================================================================

                        LOCAL FUNCTION DECLARATIONS

===========================================================================*/
VOID PmicFgPostExit(VOID);
EFI_STATUS PmicFgCommon_Init(UINT32 PmicDeviceIndex,  IN EFI_PM_FG_CFGDATA_TYPE FgCfgData);
EFI_STATUS PmicFgCommon_WaitForBMDEnable(UINT32 PmicDeviceIndex);
pm_err_flag_type PmicFgCommon_SetBattMissingCfg(UINT32 PmicDeviceIndex, UINT32 BattMCfg);
EFI_STATUS PmicFgCommon_DisableBATTIDConversion(UINT32 PmicDeviceIndex);
/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/**
EFI_PmicFgExit()

@brief
Exit Fule Gauge Module
*/
EFI_STATUS
EFIAPI
EFI_PmicFgBasicExit
(
  UINT32 PmicDeviceIndex
)
{
  /* De-init interrupt, cancel timers etc..*/
  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a \r\n", __FUNCTION__));

  PmicFgPostExit();

  return EFI_SUCCESS;
}

EFI_STATUS PmicFgCommon_DumpPeripheral( VOID )
{
  EFI_STATUS       Status  = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;

  errFlag = pm_fg_mem_if_get_fg_pmic_info((UINT8 *)&gFgPmicIndex, (UINT8 *)&gFgSlaveIndex);
  if(PM_ERR_FLAG_SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a get_fg_pmic_info  Error (%d) \n\r", __FUNCTION__, errFlag));
    return EFI_UNSUPPORTED;
  }

  errFlag =  pm_core_utils_dump_peripheral(gFgSlaveIndex, PMIC_FG_PERIPHERAL_START_ADDR, PMIC_FG_PERIPHERAL_NUM);
  if(PM_ERR_FLAG_SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a FG pm_core_utils_dump_peripheral Error (%d) \r\n", __FUNCTION__, errFlag));
  }

  return (Status | errFlag);
}

VOID PmicFgPostExit(VOID)
{
  EFI_CHIPINFO_PROTOCOL     *ChipInfoProtocol = NULL;
  EFIChipInfoIdType         eId = (EFIChipInfoIdType)0;
  gBS->LocateProtocol (&gEfiChipInfoProtocolGuid, NULL, (VOID **) &ChipInfoProtocol);
  if (NULL != ChipInfoProtocol)
  {
    ChipInfoProtocol->GetChipId(ChipInfoProtocol, &eId);
  }

  if ((eId == EFICHIPINFO_ID_MSM8994) && (PMIC_IS_PM8004 == pm_get_pmic_model(PM_DEVICE_2)))
  {
    PMIC_FILE_UART_DEBUG(( EFI_D_WARN, "******** UEFI IS NOT SUPPORTED ON DEVICE WITH PM8004 ******* \r\n"));
  }
}

/**
EFI_PmicFgExtendedExit()

@brief
Extended Exit for Fule Gauge Module
*/
EFI_STATUS
EFIAPI
EFI_PmicFgExtendedExit
(
  UINT32 PmicDeviceIndex
)
{
  EFI_PM_FG_BATT_STATUS_INFO BattStsInfo      = {0};
  BOOLEAN                    bStatus          = FALSE;
  BOOLEAN                    bExceptionStatus = FALSE;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a \r\n", __FUNCTION__));

  /* Make sure to release SRAM memory access since this is UEFI exit process */
  PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionStatus);

  EFI_GetBatteryStatus(PmicDeviceIndex, &BattStsInfo);

  PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a StateOfCharge:= %d \r\n", __FUNCTION__, BattStsInfo.StateOfCharge));

  /* Profile_integrity check to confirm battery profile valid or not*/
  pm_fg_batt_info_get_sys_batt_sts(PmicDeviceIndex, PM_FG_BATT_INFO_BATT_REMOVED_LATCHED_STS, &bStatus);
  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Batt Removed Latched status = %d \r\n", __FUNCTION__, bStatus));

  PmicFgPostExit();

  return EFI_SUCCESS;
}

/**
EFI_PmicFgInit()

@brief
Initializes Fule Gauge Module
*/
EFI_STATUS
EFIAPI
EFI_PmicFgBasicInit
(
  UINT32                      PmicDeviceIndex,
  EFI_PM_FG_CFGDATA_TYPE     *pFgCfgData
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(NULL == pFgCfgData )
  {
    return EFI_INVALID_PARAMETER;
  }

  /* Zero out global data */
  SetMem (&gFgConfigData, sizeof(EFI_PM_FG_CFGDATA_TYPE), 0x00);

  /* Assign global Config Data */
  CopyMemS(&gFgConfigData, sizeof(EFI_PM_FG_CFGDATA_TYPE), pFgCfgData, sizeof(EFI_PM_FG_CFGDATA_TYPE));

  Status |= PmicFgCommon_Init(PmicDeviceIndex, gFgConfigData);

  /* Enable after verification */
  EFI_PmicFgDumpPeripheral();

  return Status;
}

/**
EFI_PmicFgEnableBMD()

@brief
Enables BMD 
*/
EFI_STATUS
EFIAPI
EFI_PmicFgEnableBMD
(
    IN UINT32 PmicDeviceIndex
)
{

  EFI_STATUS  Status = EFI_SUCCESS;

  Status = PmicFgCommon_WaitForBMDEnable(PmicDeviceIndex);

  /* Enable BMD */
  Status |= PmicFgCommon_SetBattMissingCfg(PmicDeviceIndex, gFgConfigData.BattMissingCfg);

  return Status;

}

/**
EFI_PmicFgExtendedInit()

@brief
Initializes Fule Gauge Module with battery profile if profile loading is enabled
*/
EFI_STATUS
EFIAPI
EFI_PmicFgExtendedInit
(
  IN UINT32                        PmicDeviceIndex,
  OPTIONAL EFI_PM_FG_BATT_PROFILE_DATA  *pBattProfileData
)
{
  EFI_STATUS       Status  = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  EFI_PM_FG_PROFILE_PARSER_STATUS parseStatus = PM_PARSER_STATUS_GOOD_MATCH;
  EFI_PM_FG_BATT_PROFILE_STATE    eProfileLoad = PM_FG_BATT_PROFILE_INVALID;
  BOOLEAN      bProfileMatch = TRUE;
  AdcRr_BattId Battery_Id    = {0};

  if( FALSE == gFgConfigData.LoadBatteryProfile)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Invalid Param/Call in build parameter selection \r\n", __FUNCTION__));
    return EFI_SUCCESS;
  }

  /* Keep for FG Extended init */
  PmicFgBattProfileInit(&gFgConfigData);

  Status = PmicFgSram_Init(PmicDeviceIndex, FG_SRAM_STATUS_INIT, &gFgConfigData);

  /* Parse profile on every boot to locate each profile data */
  parseStatus |= PmicBattProfileParseDataBuffer(pBattProfileData->pFileBuffer, pBattProfileData->DataSize);

  Status |= PmicFgCommon_ProfileLoadNeeded(PmicDeviceIndex, &eProfileLoad);

  /* Set and Pass Tolerance level */
  Battery_Id.BatteryIdTolerance = gFgConfigData.FgBattIdCfg.BatteryIdTolerance;

  Status |= PmicFgSram_VerifyProfileMatch(PmicDeviceIndex, Battery_Id, &bProfileMatch);
  if((errFlag != PM_ERR_FLAG_SUCCESS) || (Status != EFI_SUCCESS))
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Error Reading Profile Match = %d, Status = %r \r\n", __FUNCTION__, bProfileMatch, Status));
    return EFI_DEVICE_ERROR;
  }
  else
  {
    /* debug success case for test */
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Success Reading Profile Match = %d, Status = %r \r\n", __FUNCTION__, bProfileMatch, Status));
  }
  if((bProfileMatch == FALSE) || ( PM_FG_BATT_PROFILE_LOAD == eProfileLoad))
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a  eProfileLoad = %d \r\n",__FUNCTION__, eProfileLoad));
    /* TBD Clear Battery Removed latch status Discuss with HLOS that if HLOS will clear it for first time or UEFI LA,WP will clear the status */
    errFlag |= pm_fg_batt_info_clr_batt_removed_latched_sts(PmicDeviceIndex);

    /*  Enable after DMA or IMA access is implemented and profile load sequence is ready */
    Status |= PmicFgBattHandleProfile(PmicDeviceIndex);
  }
  else if (PM_FG_BATT_PROFILE_RESTART == eProfileLoad)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a  Battery profile loading skipped. Status = %r eProfileLoad = %d \r\n", __FUNCTION__, Status, eProfileLoad));
    /* As we do not need to load profile Do Conditional Restart here if condition for restart matches */
    if((EFI_SUCCESS == Status ) && (TRUE == gFgConfigData.FgCondRestart))
    {
      /* Restart FG if needed conditions matched */
      Status  |= PmicFgSram_CondRestart(PmicDeviceIndex, gFgConfigData);
      PMIC_DEBUG(( EFI_D_INFO, "PmicDxe:: %a  FG Cond Restart Sequence if VBattEstDiffThreshold = %d mv \r\n", __FUNCTION__, gFgConfigData.VBattEstDiffThreshold));
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Skipping FG Cond Restart Sequence flag = %d \r\n", __FUNCTION__, Status, gFgConfigData.FgCondRestart));
    }
  }else
  {
    /* Should not hit here */
  }

  return (Status | errFlag);

}

EFI_STATUS PmicFgCommon_ProfileLoadNeeded(UINT32 PmicDeviceIndex, EFI_PM_FG_BATT_PROFILE_STATE *eProfileState)
{
  EFI_STATUS       Status  = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  BOOLEAN          bBattRemovedLatcheStatus = FALSE;
  BOOLEAN          bIntergrityBit = FALSE;

  if(!eProfileState)
    return EFI_INVALID_PARAMETER;

  *eProfileState = PM_FG_BATT_PROFILE_INVALID;

  /* Profile_integrity check to confirm battery profile valid or not , latched status will be cleared when profile is flashed in extended init API */
  errFlag = pm_fg_batt_info_get_sys_batt_sts(PmicDeviceIndex, PM_FG_BATT_INFO_BATT_REMOVED_LATCHED_STS, &bBattRemovedLatcheStatus);

  Status |= PmicFgSram_ReadIntegrityBit(PmicDeviceIndex, &bIntergrityBit);
  if((errFlag != PM_ERR_FLAG_SUCCESS) || (Status != EFI_SUCCESS))
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Error Reading Batt Removed latch sts = %d, Integrity Bit = %d, Status = %r \r\n", __FUNCTION__, bBattRemovedLatcheStatus, bIntergrityBit, errFlag));
    return EFI_DEVICE_ERROR;
  }
  else
  {
    /* debug success case for test */
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Batt Removed latched sts  = %d, bIntergrityBit = %d \r\n", __FUNCTION__, bBattRemovedLatcheStatus, bIntergrityBit));
  }

  if ((TRUE == bBattRemovedLatcheStatus) || (FALSE == bIntergrityBit))
  {
    *eProfileState = PM_FG_BATT_PROFILE_LOAD;
  }
  else
  {
    *eProfileState = PM_FG_BATT_PROFILE_RESTART;
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Status = %r, bIntergrityBit = %d \r\n",__FUNCTION__, Status, bIntergrityBit));
  }
  return (Status | errFlag);

}


pm_err_flag_type PmicFgCommon_SetBattMissingCfg(UINT32 PmicDeviceIndex, UINT32 BattMCfg)
{
  PM_FG_BATT_INFO_BATT_MISSING_CFG BattMissingCfg;
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  switch(BattMCfg)
  {
    case 0:
      BattMissingCfg.BmFromTherm   = FALSE;
      BattMissingCfg.BmFromBattId  = TRUE;
    break;
    case 1:
      BattMissingCfg.BmFromTherm   = TRUE;
      BattMissingCfg.BmFromBattId = FALSE;
    break;
    case 3:
      BattMissingCfg.BmFromTherm   = TRUE;
      BattMissingCfg.BmFromBattId = TRUE;
    break;
    default:
      BattMissingCfg.BmFromTherm = FALSE;
      BattMissingCfg.BmFromBattId = TRUE;
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Invalid batt missing cfg = %d \r\n", __FUNCTION__, BattMCfg));
    break;
  }
  BattMissingCfg.BmThermTh = PM_FG_BATT_INFO_RES_TH_0P75_MOHM;

  err_flag = pm_fg_batt_info_set_batt_missing_cfg(PmicDeviceIndex, *(pm_fg_batt_info_batt_missing_cfg*) &BattMissingCfg);

  return err_flag;
}

EFI_STATUS PmicFgCommon_SetThermCoeffs
(
  IN UINT32                     PmicDeviceIndex,
  IN EFI_PM_AUX_THERM_COEFFS   *pAuxThermCoeffs,
  IN EFI_PM_BATT_THERM_COEFFS  *pBattThermCoeffs
)
{
  EFI_STATUS       Status   = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  if(NULL == pAuxThermCoeffs || NULL == pBattThermCoeffs)
  {
    return EFI_INVALID_PARAMETER;
  }
  /* Set Batt Therm Coffs */
  err_flag  = pm_fg_batt_info_set_therm_coeff_value(PmicDeviceIndex, PM_FG_BATT_INFO_THERM_COEFF_C1, pBattThermCoeffs->BattThermC1);
  err_flag |= pm_fg_batt_info_set_therm_coeff_value(PmicDeviceIndex, PM_FG_BATT_INFO_THERM_COEFF_C2, pBattThermCoeffs->BattThermC2);
  err_flag |= pm_fg_batt_info_set_therm_coeff_value(PmicDeviceIndex, PM_FG_BATT_INFO_THERM_COEFF_C3, pBattThermCoeffs->BattThermC3);

  /* Set Skin Aux Therm Coffs */
  err_flag |= pm_fg_adc_rr_set_aux_therm_coefficient(PmicDeviceIndex, PM_FG_ADC_RR_AUX_THERM_COEF_C1, pAuxThermCoeffs->AuxThermC1);
  err_flag |= pm_fg_adc_rr_set_aux_therm_coefficient(PmicDeviceIndex, PM_FG_ADC_RR_AUX_THERM_COEF_C2, pAuxThermCoeffs->AuxThermC2);
  err_flag |= pm_fg_adc_rr_set_aux_therm_coefficient(PmicDeviceIndex, PM_FG_ADC_RR_AUX_THERM_COEF_C3, pAuxThermCoeffs->AuxThermC3);

  return (Status |err_flag);

}


EFI_STATUS PmicFgCommon_SetSkinAndChargerHotThreshold(EFI_PM_SKIN_CHARGERHOT_THRESHOLD *pSkinAndChargerHotThreshold)
{
  EFI_STATUS       Status = EFI_SUCCESS;
  EFI_ADC_PROTOCOL *AdcProtocol = NULL;

  if(NULL == pSkinAndChargerHotThreshold)
  {
    return EFI_INVALID_PARAMETER;
  }

  if(!AdcProtocol)
  {
    Status = gBS->LocateProtocol( &gEfiAdcProtocolGuid,
                                  NULL,
                                  (VOID**) &AdcProtocol);
  }

  if ((EFI_ERROR(Status)) || (NULL == AdcProtocol))
  {
    PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a: Failed to load ADC Protocol!\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  Status |= AdcProtocol->AdcSetFuelGaugeThreshold(EFI_ADC_THRESH_CHARGER_HOT, gFgConfigData.SkinAndChargerHotThreshold.ChargerHotInC);

  Status |= AdcProtocol->AdcSetFuelGaugeThreshold(EFI_ADC_THRESH_CHARGER_TOO_HOT, gFgConfigData.SkinAndChargerHotThreshold.ChargerTooHotInC);

  Status |= AdcProtocol->AdcSetFuelGaugeThreshold(EFI_ADC_THRESH_SKIN_HOT, gFgConfigData.SkinAndChargerHotThreshold.DeviceSkinHotInC);

  Status |= AdcProtocol->AdcSetFuelGaugeThreshold(EFI_ADC_THRESH_SKIN_TOO_HOT, gFgConfigData.SkinAndChargerHotThreshold.DeviceSkinTooHotInC);

  return Status;
}

EFI_STATUS PmicFgCommon_SetThermBiasWait(UINT32 PmicDeviceIndex, EFI_PM_FG_THERM_CONFIG* pBattThermCfg, EFI_PM_FG_THERM_CONFIG* pAuxThermCfg)
{
  EFI_STATUS       Status = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  pm_fg_adc_rr_aux_therm_cfg_type auxThermConfig;

  if ((NULL == pBattThermCfg) || (NULL == pAuxThermCfg))
  {
    return EFI_INVALID_PARAMETER;
  }
  errFlag = pm_fg_set_adc_rr_batt_therm_cfg(PmicDeviceIndex, (pm_fg_adc_rr_bias_wait_type)(pBattThermCfg->BiasWait), pBattThermCfg->GroundSel);


  auxThermConfig.aux_therm_cfg_bias_wait = (pm_fg_adc_rr_bias_wait_type)(pAuxThermCfg->BiasWait);
  auxThermConfig.tmp_hyst = PM_FG_ADC_RR_AUX_THERM_CFG_NO_HYST;
  auxThermConfig.batt_therm_sync = FALSE;
  auxThermConfig.aux_therm_mode = FALSE;
  auxThermConfig.gnd_sel = (boolean)(pAuxThermCfg->GroundSel);
  errFlag |= pm_fg_adc_rr_aux_therm_cfg(PmicDeviceIndex, &auxThermConfig);

  return (Status | errFlag);
}

/**
PmicFgCommon_Init()

@brief
Initializes Fule Gauge Module
*/
EFI_STATUS
PmicFgCommon_Init
(
  IN UINT32                     PmicDeviceIndex,
  IN EFI_PM_FG_CFGDATA_TYPE     FgCfgData
)
{
  EFI_STATUS       Status  = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  pm_fg_batt_info_batt_temp_hyst_cfg hyst_cfg = PM_FG_BATT_INFO_NO_HYSTERESIS;

  Status = PmicFgCommon_DisableBATTIDConversion(PmicDeviceIndex);
  PMIC_DEBUG((EFI_D_ERROR, "PmicDxe: FG sync mode disabled Status = %r \n\r", Status));

  /* Check SRAM health and issue reset if needed this call will not return if SRAM contents are not healthy */
  Status = PmicFgSram_SramHealthy(PmicDeviceIndex);

  /* Enable Fg Algo */
  pm_fg_batt_soc_enable_fg_algorithm(PmicDeviceIndex, TRUE);

  /* Enable Fg BCL periperial */
  pm_fg_bcl_enable_ctl(PmicDeviceIndex, TRUE);

  /* Enable Fg BCL Gain Offset corruption for VBAT accuracy  */
  pm_fg_set_bcl_cfg(PmicDeviceIndex, PM_FG_BCL_BGOCEN, TRUE);

  /* Init File log and UART log TBD enable after File log implementation */
  PmicFileLog_InitFileLog(FgCfgData.FgDbgCfg.PrintChargerAppDbgMsg, 
                          FgCfgData.FgDbgCfg.PrintChargerAppDbgMsgToFile,
                          FgCfgData.FgDbgCfg.FileLoggingLevelMask);

  /* Update THERM C1 C2 C3 Coffs from battery config */
  if (TRUE == gFgConfigData.AuxThermCoeffs.ProgramAuxThermCoeffs)
  {
    Status = PmicFgCommon_SetThermCoeffs(PmicDeviceIndex, &gFgConfigData.AuxThermCoeffs, &gFgConfigData.FgBattThermCoffs);
  }
  /* Update Skin Hot and Charger Hot Thersholds from battery config */
  if (TRUE == gFgConfigData.SkinAndChargerHotThreshold.ProgramSkinAndChargerHotThreshold)
  {
    Status |= PmicFgCommon_SetSkinAndChargerHotThreshold(&gFgConfigData.SkinAndChargerHotThreshold);
  }

  Status |= PmicFgCommon_SetThermBiasWait(PmicDeviceIndex, &(gFgConfigData.FgBattThermCfg), &(gFgConfigData.FgAuxThermCfg));

  /* set default Hyst level */
  errFlag =  pm_fg_batt_info_get_batt_temp_hyst_cfg(PmicDeviceIndex, &hyst_cfg);

  if((errFlag == PM_ERR_FLAG_SUCCESS) && hyst_cfg == PM_FG_BATT_INFO_NO_HYSTERESIS)
  {
    errFlag |= pm_fg_batt_info_set_batt_temp_hyst_cfg(PmicDeviceIndex, PM_FG_BATT_INFO_HYST_2_C);
  }

  /* Always Set Initial SRAM configuration */
  Status |= PmicFgSram_SetOneTimeSramCfgs(PmicDeviceIndex);


  return (Status | errFlag);
}


/**
EFI_PmicFgBatteryStatus()

@brief
Returns Battery Status parameters such as State of Charge (SOC)
*/
EFI_STATUS
EFIAPI
EFI_GetBatteryStatus
(
   IN  UINT32         PmicDeviceIndex,
   OUT EFI_PM_FG_BATT_STATUS_INFO *BattStsInfo
)
{
  EFI_STATUS Status   = EFI_SUCCESS;
  EFI_PM_FG_BATT_STATUS_INFO stsInfo = {0};

  if(NULL == BattStsInfo )
  {
    return EFI_INVALID_PARAMETER;
  }
  Status = PmicFgCommon_GetBatteryStatus(PmicDeviceIndex, &stsInfo);

  *BattStsInfo = stsInfo;

  return Status;
}


/**
EFI_GetBattProfileStatus()

@brief
Returns Battery profile Status parameters such as profile load status
*/
EFI_STATUS
EFIAPI
EFI_GetBattProfileStatus
(
   IN  UINT32         PmicDeviceIndex,
   OUT EFI_PM_FG_BATT_PROFILE_STATUS *pBattProfileStatus
)
{
  EFI_STATUS                   Status           = EFI_SUCCESS;
  EFI_PM_FG_BATT_PROFILE_STATE BattProfileState = {0};

  if(NULL == pBattProfileStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status |= PmicFgCommon_ProfileLoadNeeded(PmicDeviceIndex, &BattProfileState);
  if(EFI_SUCCESS == Status)
  {
    pBattProfileStatus->eProfileState = BattProfileState;
  }
  else
  {
    PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a: Failed to Profile Status = %r\r\n", __FUNCTION__, Status));
  }

  return Status;
}


/**
PmicFgCommon_GetBatteryStatus

@brief
Returns Battery Status parameters such as State of Charge (SOC)
*/
EFI_STATUS
PmicFgCommon_GetBatteryStatus
(
   IN  UINT32         PmicDeviceIndex,
   OUT EFI_PM_FG_BATT_STATUS_INFO *BattStsInfo
)
{
  EFI_STATUS       Status   = EFI_SUCCESS;
  pm_err_flag_type errFlag  = PM_ERR_FLAG_SUCCESS;
  UINT32           Soc  = 0;
  UINT32           VBat = 0;
  INT32            ibat = 0;
  INT32            BattTemp = 0;

  if(NULL == BattStsInfo )
  {
    return EFI_INVALID_PARAMETER;
  }
  /* Get Battery SOC */
  errFlag =  pm_fg_batt_soc_get_monotonic_soc(PmicDeviceIndex, &Soc);
  Status = (PM_ERR_FLAG_SUCCESS == errFlag)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  BattStsInfo->StateOfCharge = Soc;

  /* Get Battery voltage */
  errFlag |= pm_fg_batt_info_get_vbatt(PmicDeviceIndex, &VBat);
  Status = (PM_ERR_FLAG_SUCCESS == errFlag)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  BattStsInfo->BatteryVoltage = VBat;

  /* Get Battery charge current */
  errFlag |= pm_fg_batt_info_get_ibatt(PmicDeviceIndex, &ibat);

  BattStsInfo->ChargeCurrent = ibat;

  errFlag  |= pm_fg_batt_info_get_batt_temp(PmicDeviceIndex, &BattTemp);

  BattStsInfo->BatteryTemperature = BattTemp;

  /* DEBUG ONLY */
  /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a StateOfCharge = %d, VBat = %d mV Current = %d mA Temperature = %d C\r\n",
                           __FUNCTION__, Soc, VBat, ibat, BattTemp)); */

  return (Status | errFlag);
}


EFI_STATUS
EFIAPI
EFI_PmicFgGetBatteryId
(
  OUT EFI_PM_BATTERY_ID* BatteryId
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(NULL == BatteryId)
  {
    Status = EFI_INVALID_PARAMETER;
  }
  else
  {
    Status = PmicFgCommon_GetBatteryId(BatteryId);
  }

  return Status;
}

EFI_STATUS PmicFgCommon_GetBatteryId(EFI_PM_BATTERY_ID* BatteryId)
{
  EFI_STATUS Status = EFI_SUCCESS;
  AdcRr_BattId Batt_Id = {0};
  if(NULL == BatteryId)
  {
    Status = EFI_INVALID_PARAMETER;
  }
  else
  {
    //Reaad for ADC RR when available
    Status = PmicFgSram_ReadBattID(&Batt_Id);
    if(EFI_SUCCESS == Status )
      BatteryId->BattId = Batt_Id.Bid;
    else
      BatteryId->BattId = (UINT64) 7500; //debug board  ID
  }

  return Status;
}


/**
EFI_PmicFgDumpPeripheral()

@brief
 Dump FG Pheripheral
 */
EFI_STATUS
EFIAPI
EFI_PmicFgDumpPeripheral( VOID )
{
  EFI_STATUS              Status         = EFI_SUCCESS;
  //EFI_PM_FG_CFGDATA_TYPE BatteryCfgData = {0};

  if(!RETAIL)
  {
    if((EFI_SUCCESS == Status )&& (TRUE == gFgConfigData.FgDbgCfg.EnableChargerFgDump))
    {
     Status = PmicFgCommon_DumpPeripheral();
    }
    else
    {
      /* PMIC_DEBUG(( EFI_D_WARN, "PmicDxe: EFI_PmicFgDumpPeripheral Disabled \r\n")); */
    }
  }
  else
  {
    return EFI_UNSUPPORTED;
  }

  return Status;
}

/**
EFI_PmicFgDumpFgSram()

@brief
 Dump FG SRAM
*/
EFI_STATUS
EFIAPI
EFI_PmicFgDumpFgSram( IN UINT32 PmicDeviceIndex)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8 NoOfBytesToRead = 0;
  if(!RETAIL)
  {
    if(gFgConfigData.FgSramDbgCfg.DumpSram == TRUE )
    {
      NoOfBytesToRead = gFgConfigData.FgSramDbgCfg.DumpSramEndAddr - gFgConfigData.FgSramDbgCfg.DumpSramStartAddr;
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a NoOfBytesToRead = %d \r\n",__FUNCTION__, NoOfBytesToRead));
      Status |= PmicFgSram_Dump(PmicDeviceIndex, gFgConfigData.FgSramDbgCfg.DumpSramStartAddr, NoOfBytesToRead);
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Dump Completed = %r \r\n", __FUNCTION__, Status));
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Disabled \r\n", __FUNCTION__));
    }
  }

  return Status;
}




EFI_STATUS
EFIAPI
EFI_PmicFgConfigureHwJeita( UINT32 PmicDeviceIndex )
{
  EFI_STATUS Status  = EFI_SUCCESS;

  Status = PmicFgCommon_ConfigHwJeita(PmicDeviceIndex);

  return Status;
}

EFI_STATUS PmicFgCommon_ConfigHwJeita( UINT32 PmicDeviceIndex )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  pm_err_flag_type errFlg = PM_ERR_FLAG_SUCCESS;
  UINT32 JeitaSoftColdThr = 0, JeitaSoftHotThr = 0, JeitaHardColdThr = 0, JeitaHardHotThr = 0; 


  /* PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Config Params JeitaSoftColdThr = 0x%x, JeitaSoftHotThr = 0x%x, JeitaHardColdThr = 0x%x, JeitaHardHotThr =0x%x  \r\n", __FUNCTION__,
                                        gFgConfigData.HwJeitaThreshold.JeitaSoftColdLimit, gFgConfigData.HwJeitaThreshold.JeitaSoftHotLimit,
                                        gFgConfigData.HwJeitaThreshold.JeitaHardColdLimit, gFgConfigData.HwJeitaThreshold.JeitaHardHotLimit)); */

  JeitaSoftColdThr = (gFgConfigData.HwJeitaThreshold.JeitaSoftColdLimit + PMIC_FG_TEMP_CONV_BASE ) * PMIC_FG_TEMP_CONV_1_LSB;
  JeitaSoftHotThr =  (gFgConfigData.HwJeitaThreshold.JeitaSoftHotLimit  + PMIC_FG_TEMP_CONV_BASE ) * PMIC_FG_TEMP_CONV_1_LSB;
  JeitaHardColdThr = (gFgConfigData.HwJeitaThreshold.JeitaHardColdLimit + PMIC_FG_TEMP_CONV_BASE ) * PMIC_FG_TEMP_CONV_1_LSB;
  JeitaHardHotThr =  (gFgConfigData.HwJeitaThreshold.JeitaHardHotLimit  + PMIC_FG_TEMP_CONV_BASE ) * PMIC_FG_TEMP_CONV_1_LSB;

  PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Converted Config Params JeitaSoftColdThr = 0x%x, JeitaSoftHotThr = 0x%x, ", __FUNCTION__,
                                        JeitaSoftColdThr, JeitaSoftHotThr ));

  PMIC_DEBUG((EFI_D_WARN, " JeitaHardColdThr = 0x%x, JeitaHardHotThr =0x%x \r\n",
                                        JeitaHardColdThr,JeitaHardHotThr ));

  errFlg  = pm_fg_batt_info_set_jeita_threshold_value(PmicDeviceIndex, PM_FG_BATT_INFO_JEITA_THRESHOLD_TOO_COLD, JeitaHardColdThr);
  errFlg |= pm_fg_batt_info_set_jeita_threshold_value(PmicDeviceIndex, PM_FG_BATT_INFO_JEITA_THRESHOLD_COLD, JeitaSoftColdThr);
  errFlg |= pm_fg_batt_info_set_jeita_threshold_value(PmicDeviceIndex, PM_FG_BATT_INFO_JEITA_THRESHOLD_HOT, JeitaSoftHotThr);
  errFlg |= pm_fg_batt_info_set_jeita_threshold_value(PmicDeviceIndex, PM_FG_BATT_INFO_JEITA_THRESHOLD_TOO_HOT, JeitaHardHotThr);

  return (Status | errFlg);

}

EFI_STATUS
EFIAPI
EFI_PmicFgGetHWJeitaStatus( UINT32 PmicDeviceIndex, EFI_PM_FG_BATT_TEMP_STATUS *pHwJeitaStatus)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!pHwJeitaStatus)
    return EFI_INVALID_PARAMETER;

  Status = PmicFgCommon_GetHWJeitaStatus(PmicDeviceIndex, pHwJeitaStatus);

  return Status;
}

EFI_STATUS PmicFgCommon_GetHWJeitaStatus( UINT32 PmicDeviceIndex, EFI_PM_FG_BATT_TEMP_STATUS *pHwJeitaStatus)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  EFI_STATUS       Status  = EFI_SUCCESS;

  if(!pHwJeitaStatus)
    return EFI_INVALID_PARAMETER;

  errFlag = pm_fg_batt_info_get_batt_temp_sts(PmicDeviceIndex, PM_FG_BATT_INFO_JEITA_TOO_COLD_STS, &pHwJeitaStatus->jeitaHardCold);
  errFlag |= pm_fg_batt_info_get_batt_temp_sts(PmicDeviceIndex, PM_FG_BATT_INFO_JEITA_TOO_HOT_STS, &pHwJeitaStatus->jeitaHardHot);
  errFlag |= pm_fg_batt_info_get_batt_temp_sts(PmicDeviceIndex, PM_FG_BATT_INFO_JEITA_COLD_STS,    &pHwJeitaStatus->jeitaSoftCold);
  errFlag |= pm_fg_batt_info_get_batt_temp_sts(PmicDeviceIndex, PM_FG_BATT_INFO_JEITA_HOT_STS,     &pHwJeitaStatus->jeitaSoftHot);

  if(PM_ERR_FLAG_SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Error in getting batt temp sts = %d  \r\n", __FUNCTION__, errFlag));
    return EFI_DEVICE_ERROR;
  }

  return (Status | errFlag);
}


EFI_STATUS
EFIAPI
EFI_PmicFgSetBattMissingCfg(UINT32 PmicDeviceIndex, PM_FG_BATT_INFO_BATT_MISSING_CFG  BattMissingCfg)
{
  EFI_STATUS       Status = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;

  errFlag = pm_fg_batt_info_set_batt_missing_cfg(PmicDeviceIndex,* (pm_fg_batt_info_batt_missing_cfg*) &BattMissingCfg);

  return ( Status | errFlag );
}


EFI_STATUS
EFIAPI
EFI_PmicFgGetBattMissingCfg(UINT32 PmicDeviceIndex, PM_FG_BATT_INFO_BATT_MISSING_CFG *pBattMissingCfg)
{
  EFI_STATUS       Status = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;

  if(NULL == pBattMissingCfg)
    return EFI_INVALID_PARAMETER;

  errFlag = pm_fg_batt_info_get_batt_missing_cfg(PmicDeviceIndex, (pm_fg_batt_info_batt_missing_cfg *) pBattMissingCfg);

  return ( Status | errFlag );
}

EFI_STATUS
EFIAPI
EFI_PmicFgEnableESRPulse(UINT32 PmicDeviceIndex, BOOLEAN enable)
{
  EFI_STATUS       Status = EFI_SUCCESS;

  Status = PmicFgSram_EnableESRPulse(PmicDeviceIndex, enable);

  return Status;
}

/**
EFI_PmicFgBasicGetPmicInfo()

@brief
Provides PMIC Index on which FG resides
*/
EFI_STATUS
EFIAPI
EFI_PmicFgBasicGetPmicInfo
(
    OUT EFI_PM_FG_PMIC_INFO *FgPmicInfo
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;

  if(!FgPmicInfo)
  {
    return EFI_INVALID_PARAMETER;
  }

  errFlag = pm_fg_mem_if_get_fg_pmic_info((UINT8 *)&gFgPmicIndex, (UINT8 *)&gFgSlaveIndex);
  FgPmicInfo->PmicIndex = gFgPmicIndex;
  FgPmicInfo->SlaveIndex = gFgSlaveIndex;

  return (PM_ERR_FLAG_SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

}

EFI_STATUS PmicFgCommon_WaitForBMDEnable(UINT32 PmicDeviceIndex)
{
  EFI_STATUS       Status  = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  UINT32  count            = 0;
  boolean batt_id_done     = FALSE;

  /* Poll for Battery ID IRQ */
  while (count < PMIC_FG_MAX_BATT_ID_CONV_COUNT)
  {

    /* Check battery ID availabity status until it is 1. */
    errFlag |= pm_fg_adc_rr_irq_status(PmicDeviceIndex, PM_FG_ADC_RR_IRQ_BT_ID, PM_IRQ_STATUS_RT, &batt_id_done);
    if (batt_id_done)
      break;

    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a: Wait BT_ID = %d \r\n", __FUNCTION__, PMIC_FG_BATT_ID_CONV_WAIT/1000));

    /* wait for 10 ms */
    gBS->Stall(PMIC_FG_BATT_ID_CONV_WAIT);
    count++;
  }

  /* For battery ID with no capacitor, delay = 0, for batt id with 0.1 uF capacity, need to use 100 ms ddelay */
  if (gFgConfigData.FgBattIdCfg.BattMissingDelay > PMIC_FG_MAX_BMD_ENABLE_DELAY)
  {
    gFgConfigData.FgBattIdCfg.BattMissingDelay = PMIC_FG_MAX_BMD_ENABLE_DELAY;
  }

  if (gFgConfigData.FgBattIdCfg.BattMissingDelay != 0)
  {
    gBS->Stall(gFgConfigData.FgBattIdCfg.BattMissingDelay * 1000);
  }

  return (Status | errFlag);
}

EFI_STATUS PmicFgCommon_DisableBATTIDConversion(UINT32 PmicDeviceIndex)
{
  EFI_STATUS       Status = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  EFI_ADC_PROTOCOL       *pAdcProtocol = NULL;

  /* disable continous mode */
  if (!pAdcProtocol)
  {
    Status = gBS->LocateProtocol(&gEfiAdcProtocolGuid,
             NULL,
             (VOID**)&pAdcProtocol);
  }

  if ((EFI_ERROR(Status)) || (NULL == pAdcProtocol))
  {
    PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a: Failed to load ADC Protocol! Status = %r \r\n", __FUNCTION__, Status));
    return EFI_DEVICE_ERROR;
  }
  pAdcProtocol->AdcSetFuelGaugeBattIdForced(FALSE);
  /* disable battery ID conversion*/
  errFlag = pm_fg_adc_rr_batt_id_ctrl(PmicDeviceIndex, PM_FG_ADC_RR_BATT_ID_CTRL_CHANNEL_CONV, FALSE);
  
  return (Status | errFlag);
}
