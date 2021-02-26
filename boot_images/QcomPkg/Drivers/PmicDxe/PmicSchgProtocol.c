/*! @file PmicSchgProtocol.c 

*  PMIC- SCHG MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*  the PMIC SCHG (Switch Mode Battery Charger) module.
*
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Inc Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     --------------------------------------------------------------------------
07/24/17   pbitra  Added EFI_PmicSchgGetChargerConnectorType API
06/13/17   ll      Enable/disable HW AFP based on debug board detection
03/21/17   dch     Initialize ICL for all charger as 500mA and increase it to 1500mA in case charger is not SDP/Float charger
02/15/17   va      Add re run APSD 
02/08/17   ll      Added OTG sequence for OTG device with large cap
02/06/17   cs      add config to enable/disable INOV
01/27/17   ai      Fix use of retail
01/27/17   sm      battery missing is true either latched or RT STATUS is TRUE.
                   Add support for PBS triggered AFP
                   Fixed battery missing not detected after battery missing shutdown and reboot
01/23/17   sm      CFG parameter Structure changes
12/19/16   sm      Added API to get SCHG Charger PmicIndex
12/09/16   sm      Added changes to set OTG current limit when enabling/disabling OTG
11/14/16   sm      Added required changes for preventing accidental reverse boosting in EFI_PmicSchgEnableCharger() and EFI_PmicSchgExit()
11/09/16   sm      Added changes to read latched status of PM_SCHG_MISC_IRQ_WDOG_BARK interrupt
11/04/16   sm      Added APSD done status check in EFI_PmicSchgGetChargerPortType()
10/19/16   sm      Added changes to read latched status of BAT_THERM_OR_ID_MISSING interrupt
                   Removed HVDCP configuration settings
10/17/16   sm      Added SetChargeCmdBit() API
                   Added changes to report charger wdog not supported for PMI8998 versions before v2.0
                   Added changes in EFI_PmicSchgGetChgWdogStatus() to check if disabling charging on 
                   wdog bite functionality is enabled and return status accordingly.
10/13/16   cs      add protocol API for setting dcin current limit.
09/20/16   sv      fixing klocwork issues
09/13/16   sm      Removed host mode setting from init()
09/06/16   cs      wipower changes to remove 3.5 sec, not suspend DCIn
08/24/16   sm      Added function call to set FV_MAX during Init()
08/16/16   sm      Added API to enable/disable HW JEITA and made EFI_PmicSchgEnableJeita() obsolete
07/18/16   sm      Added API to configure USB ICL options
07/15/16   cs      added wipower support
06/24/16   sm      Added API to return if charging is enabled or not. 
06/23/16   sm      Added changes to enable/disable HVDCP
06/23/16   va      Adding support for Charger Fg Peripheral dumps
06/09/16   sm      Added check for hostmode support and then resetting port role 
                   Redefined GetOtgStatus API
05/25/16   sm      Added pm_schg_chgr_set_charge_current() in Init()
                   Added  pm_schg_usb_enable_icl_override_after_apsd() in SetUSBMaxCurrent()
05/10/16   sm      Added back SchgExit for placeholder
04/21/16   sm      Added APIs to enable/disable and pet watchdog
04/07/16   sm      Completing API definitions
03/28/16   sm      Few IRQ corrections
03/28/16   va      Adding HW Jeita support, exit API
02/19/16   sm      Initial draft
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/QcomTargetLib.h>
#include <Library/FuseControlLib.h>

#include "pm_uefi.h"
#include "pm_version.h"
#include "pm_schg_usb.h"
#include "pm_schg_dc.h"
#include "pm_schg_misc.h"
#include "pm_schg_batif.h"
#include "pm_schg_chgr.h"
#include "pm_schg_otg.h"
#include "pm_app_chgr_common.h"

#include <Protocol/EFIPmicSchg.h>
#include <Library/DebugLib.h>
#include <api/dal/DALSys.h>
#include "PmicFileLog.h"
/**
  PMIC Lib interfaces
 */
#include "pm_uefi.h"
#include "pm_core_utils.h"

STATIC EFI_PM_SCHG_CFGDATA_TYPE gSchgCfgData = {0}; 

STATIC UINT32  gSchgPmicIndex = 0,  gSchgSlaveIndex = 0;

/*Charger dump */
#define PMIC_SCHG_PERIPHERAL_START_ADDR      0x1000
#define PMIC_SCHG_MISC_PERIPHERAL_START_ADDR 0x1600
#define PMIC_SCHG_PERIPHERAL_NUM        5
#define PM_DC_ZIN_ICL_PT_VOLT_MV         8000
#define PM_DC_ZIN_ICL_PT_HV_VOLT_MV      9000
#define PM_DC_ZIN_ICL_LV_VOLT_MV         5500
#define PM_DC_ZIN_ICL_MID_LV_VOLT_MV     6500
#define PM_DC_ZIN_ICL_MID_HV_VOLT_MV     8000
#define PM_DC_ZIN_ICL_HV_VOLT_MV         11000

#define PMIC_ALL_LAYER_REV_2 2
#define PMIC_METAL_REV_0 0

#define PMIC_SCHG_MAX_APSD_DONE_CHECK_ITERATION 5
#define PM_SCHG_200_MS_WAIT                     200000 //200 milli Second wait
#define PM_SCHG_2_MS_WAIT                       2000 //2 milli Second wait

#define PM_SCHG_OTG_CURRENT_LIMIT_1500_MA           1500
#define PM_SCHG_OTG_CURRENT_LIMIT_250_MA            250
#define PM_SCHG_OTG_WA_FPFET_SS                     0x3

#define PM_FORCE_USBICL_SDP_FLOAT_DEFAULT_IN_MA  500
#define PM_FORCE_USBICL_OTHER_IN_MA              1500

EFI_STATUS PmicSchgDumpPeripheral( VOID );
EFI_STATUS PmicSchgSetHwAfpThresholds(UINT32 PmicDeviceIndex, INT32 HotThreshold, INT32 ColdThreshold);
EFI_STATUS PmicSchgSetThermRegSrcCfg(UINT32 PmicDeviceIndex, BOOLEAN SkinEnable, BOOLEAN DieEnable, BOOLEAN ComparatorEnable);
EFI_STATUS PmicSchgOTGOverCurrent(UINT32 PmicDeviceIndex);
EFI_STATUS PmicSchgInitialChargerCheckup(UINT32 PmicDeviceIndex);
EFI_STATUS PmicSchgSetChargerDefaultSetting(UINT32 PmicDeviceIndex);

/*===========================================================================
EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/**
EFI_PmicSchgDumpPeripheral()

@brief
 Dump SCHG Peripheral
 */
EFI_STATUS
EFIAPI
EFI_PmicSchgDumpPeripheral( VOID )
{
  EFI_STATUS              Status         = EFI_SUCCESS;

  if(!RETAIL)
  {
    Status = PmicSchgDumpPeripheral();
  }
  else
  {
    return EFI_UNSUPPORTED;
  }
  return Status;
}


/**
EFI_PmicSchgInit ()

@brief
Initializes SCHG
*/
EFI_STATUS
EFIAPI
EFI_PmicSchgInit
(
  IN UINT32 PmicDeviceIndex,
  IN EFI_PM_SCHG_CFGDATA_TYPE SchgCfgData,
  IN BOOLEAN bDbgBoard
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
  
  DALSYS_memscpy(&gSchgCfgData, sizeof(EFI_PM_SCHG_CFGDATA_TYPE), &SchgCfgData, sizeof (EFI_PM_SCHG_CFGDATA_TYPE));

  /* Perform Charger initial Configuration */
  errFlag |= PmicSchgSetChargerDefaultSetting(PmicDeviceIndex);
  errFlag |= PmicSchgInitialChargerCheckup(PmicDeviceIndex);
  
  /* enable interrupt for usb re-insertion  */
  errFlag |= pm_schg_usb_irq_set_trigger(PmicDeviceIndex, PM_SCHG_USB_IRQ_USBIN_PLUGIN, PM_IRQ_TRIGGER_ACTIVE_LOW);
  errFlag |= pm_schg_usb_irq_enable(PmicDeviceIndex, PM_SCHG_USB_IRQ_USBIN_PLUGIN, TRUE);

  /* enable interrupt for batt therm and batt id missing detection  */
  errFlag |= pm_schg_batif_irq_set_trigger(PmicDeviceIndex, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING, PM_IRQ_TRIGGER_ACTIVE_LOW);
  errFlag |= pm_schg_batif_irq_enable(PmicDeviceIndex, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING, TRUE);
  
  /* enable interrupt for charger watchdog bark/bite interrupt */
  errFlag |= pm_schg_misc_irq_set_trigger(PmicDeviceIndex, PM_SCHG_MISC_IRQ_WDOG_BARK, PM_IRQ_TRIGGER_RISING_EDGE);
  errFlag |= pm_schg_misc_irq_enable(PmicDeviceIndex, PM_SCHG_MISC_IRQ_WDOG_BARK, TRUE);
  
  /* enable OTG over current interrupt for OTG device with large capacitance */
  errFlag |= pm_schg_otg_irq_set_trigger(PmicDeviceIndex, PM_SCHG_OTG_IRQ_OTG_OVERCURRENT, PM_IRQ_TRIGGER_RISING_EDGE);
  errFlag |= pm_schg_otg_irq_enable(PmicDeviceIndex, PM_SCHG_OTG_IRQ_OTG_OVERCURRENT, TRUE);

  /*Enable battery missing detection using BATT ID and BATT THERM PIN
    Individual pin enables doen in Fuel Gauge */
  errFlag |= pm_schg_batif_cfg_batt_missing_src(PmicDeviceIndex , PM_SCHG_BAT_MISS_SRC_CFG__PIN_SRC_EN, TRUE);

  errFlag |= pm_schg_chgr_set_float_volt(PmicDeviceIndex, SchgCfgData.ChgFvMax);
  errFlag |= pm_schg_chgr_set_charge_current(PmicDeviceIndex, PM_SCHG_CHGR_FAST_CHARGE_CURRENT, SchgCfgData.ChgFccMax);

  if(gSchgCfgData.SchgDbgCfg.EnableChargerFgDump)
  {
    EFI_PmicSchgDumpPeripheral();
  }

  //set HW AFP thresholds, if using real battery (not debug board)
  if (!bDbgBoard)
  {
	  Status |= PmicSchgSetHwAfpThresholds(PmicDeviceIndex, SchgCfgData.HwAfpHotThreshold, SchgCfgData.HwAfpColdThreshold);
  }

  errFlag |= PmicSchgSetThermRegSrcCfg(PmicDeviceIndex, SchgCfgData.EnableSkinTempINov, SchgCfgData.EnableDieTempINov, SchgCfgData.EnableDieTempCompINov);

  Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}

/*
@brief
De-Initializes SCHG
*/
EFI_STATUS
EFIAPI
EFI_PmicSchgExit
(
  IN UINT32 PmicDeviceIndex
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  /* TBD: placeholder for de init interrupt, cancel timers etc..*/
  errFlag |= pm_schg_usb_irq_enable(PmicDeviceIndex, PM_SCHG_USB_IRQ_USBIN_PLUGIN, FALSE);
  /*clear the interrupt*/
  errFlag |= pm_schg_usb_irq_clear(PmicDeviceIndex, PM_SCHG_USB_IRQ_USBIN_PLUGIN);
  errFlag |= pm_schg_batif_irq_enable(PmicDeviceIndex, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING, FALSE);
  errFlag |= pm_schg_misc_irq_enable(PmicDeviceIndex, PM_SCHG_MISC_IRQ_WDOG_BARK, FALSE);
  errFlag |= pm_schg_otg_irq_enable(PmicDeviceIndex, PM_SCHG_OTG_IRQ_OTG_OVERCURRENT, FALSE);

  /*clear the interrupt*/
  errFlag |= pm_schg_usb_irq_clear(PmicDeviceIndex, PM_SCHG_USB_IRQ_USBIN_PLUGIN);
  errFlag |= pm_schg_batif_irq_clear(PmicDeviceIndex, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING);
  errFlag |= pm_schg_misc_irq_clear(PmicDeviceIndex, PM_SCHG_MISC_IRQ_WDOG_BARK);
  errFlag |= pm_schg_otg_irq_clear(PmicDeviceIndex, PM_SCHG_OTG_IRQ_OTG_OVERCURRENT);

  /* PMI8998 charger prevention of accidental reverse boosting, Enable 1-in-8 mode before exiting UEFI*/
  errFlag |= pm_schg_misc_set_buckboost_refresh_pulse(PmicDeviceIndex , PM_SCHG_MISC_BUCKBOOST_REFRESH_PULSE_1_IN_8);

  /* Perform Charger initial Configuration before exit UEFI */
  errFlag |= PmicSchgSetChargerDefaultSetting(PmicDeviceIndex);
  
  Status = (errFlag == PM_ERR_FLAG__SUCCESS) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}


/**
EFI_PmicSchgEnableCharger ()

@brief
Enable charging
*/
EFI_STATUS
EFIAPI
EFI_PmicSchgEnableCharger
(
  IN UINT32  PmicDeviceIndex,
  IN BOOLEAN ChargerEnable
)
{
  EFI_STATUS           Status  = EFI_SUCCESS;
  pm_err_flag_type     errFlag = PM_ERR_FLAG__SUCCESS;
  pm_schg_chgr_status_type    charger_status;
  BOOLEAN              HWChargingEnabled = FALSE;

  PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a ChargerEnable: %d (0: Disable Charging, 1: Enable Charging)\r\n", __FUNCTION__, ChargerEnable ));

  errFlag |= pm_schg_chgr_get_chgr_status(PmicDeviceIndex, &charger_status);
  HWChargingEnabled = charger_status.status_5.charging_enable;

  if(HWChargingEnabled == ChargerEnable)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a No action required ChargerEnable = %d HWChargingEnable = %d charging_enable = %d \n\r", __FUNCTION__, 
                ChargerEnable, HWChargingEnabled, charger_status.status_5.charging_enable));
  }
  else
  {
    /* If Charging is disabled, and API is called to enable charging
       or if Charging is enabled and API is called to disable charging
       Perform register write */
    errFlag |= pm_schg_chgr_set_chgr_cmd_type(PmicDeviceIndex, PM_SCHG_CHGR_CMD_CHARGING_ENABLE, ChargerEnable);
    
  }

  if(TRUE == ChargerEnable)
  {
    /* PMI8998 charger prevention of accidental reverse boosting, Enable 1-in-4 mode when charging */
    errFlag |= pm_schg_misc_set_buckboost_refresh_pulse(PmicDeviceIndex , PM_SCHG_MISC_BUCKBOOST_REFRESH_PULSE_1_IN_4);
  }

  Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return (Status);
}

EFI_STATUS
EFIAPI
EFI_PmicSchgUsbinValid
(
  IN  UINT32 PmicDeviceIndex,
  OUT BOOLEAN *Valid
)
{
  EFI_STATUS            Status  = EFI_SUCCESS;
  pm_err_flag_type      errFlag = PM_ERR_FLAG__SUCCESS;
  BOOLEAN               UsbinPlugedIn;

  if(NULL == Valid)
  {
    Status = EFI_INVALID_PARAMETER;
  }
  else
  {
    /*Read IRQ real time status*/
    errFlag = pm_schg_usb_irq_status(PmicDeviceIndex, PM_SCHG_USB_IRQ_USBIN_PLUGIN, PM_IRQ_STATUS_RT,(boolean*)&UsbinPlugedIn);

    Status = (PM_ERR_FLAG__SUCCESS == errFlag)? PM_ERR_FLAG__SUCCESS : EFI_DEVICE_ERROR;

    *Valid = (UsbinPlugedIn) ? TRUE : FALSE;
  }

  return Status;
}


EFI_STATUS
EFIAPI
EFI_PmicSchgGetPowerPath
(
  IN  UINT32                  PmicDeviceIndex,
  OUT PM_SCHG_POWER_PATH_TYPE *PowerPath
)
{
  EFI_STATUS                  Status  = EFI_SUCCESS;
  pm_err_flag_type            errFlag = PM_ERR_FLAG__SUCCESS;
  pm_schg_misc_pwr_pth_type   power_path = PM_SCHG_MISC_POWER_PATH__INVALID;

  if(NULL == PowerPath)
  {
    Status = EFI_INVALID_PARAMETER;
  }
  else
  {
    errFlag = pm_schg_misc_get_power_path(PmicDeviceIndex, &power_path);

    *PowerPath =(PM_SCHG_POWER_PATH_TYPE)power_path;

    Status = (PM_ERR_FLAG__SUCCESS == errFlag)? EFI_SUCCESS : EFI_DEVICE_ERROR;
  }

  return Status;
}


/**
EFI_PmicSchgIsBatteryPresent ()

@brief
Gets battery presence status
*/
EFI_STATUS
EFIAPI
EFI_PmicSchgIsBatteryPresent
(
  IN  UINT32 PmicDeviceIndex,
  OUT BOOLEAN *BatteryPresent
)
{
  EFI_STATUS        Status        = EFI_SUCCESS;
  pm_err_flag_type  errFlag       = PM_ERR_FLAG__SUCCESS;
  BOOLEAN           BattMissingLatched = FALSE;
  BOOLEAN           BattMissingRT = FALSE;
  STATIC BOOLEAN    FirstDetectionDone = FALSE;

  if(NULL == BatteryPresent)
  {
    return EFI_INVALID_PARAMETER;
  }
  else
  {
    /* Check BAT_THERM_OR_ID_MISSING Interrupt Latched status. */
    errFlag = pm_schg_batif_irq_status(PmicDeviceIndex, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING, PM_IRQ_STATUS_LATCHED, (boolean *)&BattMissingLatched);
    errFlag |= pm_schg_batif_irq_status(PmicDeviceIndex, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING, PM_IRQ_STATUS_RT, (boolean *)&BattMissingRT);
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Battery Missing: %d, Battery Missing Latched: %d (0: present, 1: missing)\r\n", __FUNCTION__, BattMissingRT, BattMissingLatched));
    if (BattMissingLatched == TRUE)
    {
      /*clear the interrupt*/
      errFlag |= pm_schg_batif_irq_clear(PmicDeviceIndex, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING);
    }

    if (!FirstDetectionDone)
    {
      //first time use RT battery missing due to the latched status may not be set if the battery is missing before device reboot
      //then the edge trigger is already missed, so the latched status will show not missing, but RT status will always refect the 
      //true battery missing status.
      *BatteryPresent = (BattMissingRT) ? FALSE : TRUE;
      FirstDetectionDone = TRUE;
    }
    else
    {
      //if latched is asserted or missing RT is asserted, either is true, battery is missing.
      *BatteryPresent = (BattMissingLatched | BattMissingRT) ? FALSE : TRUE;
    }

    Status = (PM_ERR_FLAG__SUCCESS == errFlag)? EFI_SUCCESS : EFI_DEVICE_ERROR;
  }

  return Status;
}


/**
EFI_PmicChargerPortType ()

@brief
Gets charger port type
*/
EFI_STATUS
EFIAPI
EFI_PmicSchgGetChargerPortType
(
  IN  UINT32 PmicDeviceIndex,
  OUT EFI_PM_SCHG_CHGR_PORT_TYPE *PortType
)
{
  pm_err_flag_type  errFlag       = PM_ERR_FLAG__SUCCESS;
  BOOLEAN           ApsdDoneStatus = FALSE;
  UINT32            ApsdDoneCheckIteration = 0;

  if(NULL == PortType)
  {
    return EFI_INVALID_PARAMETER;
  }

  *PortType = EFI_PM_SCHG_CHG_PORT_INVALID;

  do
  {
    errFlag = pm_schg_usb_get_apsd_status(PmicDeviceIndex, PM_SCHG_USB_APSD_STATUS_APSD_DTC_STATUS_DONE , &ApsdDoneStatus);
    PMIC_UART_DEBUG(( EFI_D_WARN, "PmicDxe:: %a APSD done status: %d \n\r", __FUNCTION__, ApsdDoneStatus));
    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
      break;
    }

    if(TRUE == ApsdDoneStatus)
    {
      errFlag =  pm_schg_usb_get_apsd_result_status(PmicDeviceIndex, (pm_schg_usb_apsd_result_status_type*) PortType);
      if(PM_ERR_FLAG__SUCCESS != errFlag)
      {
        break;
      }
    }
    else
    {
      /*if APSD result is not available yet, read again after 200 milli second.*/
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a APSD not done, stalling for 200mS before re-checking \n\r", __FUNCTION__));
      gBS->Stall(PM_SCHG_200_MS_WAIT);
    }

    ApsdDoneCheckIteration ++;

  }while ((FALSE == ApsdDoneStatus) && (ApsdDoneCheckIteration < PMIC_SCHG_MAX_APSD_DONE_CHECK_ITERATION));

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    *PortType = EFI_PM_SCHG_CHG_PORT_INVALID;
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgConfigOtg
(
   IN  UINT32                    PmicDeviceIndex,
   IN  EFI_PM_SCHG_OTG_CFG_TYPE  OtgCfgType,
   IN  BOOLEAN                   SetValue
)
{
  EFI_STATUS             Status  = EFI_SUCCESS;
  pm_err_flag_type       errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_schg_otg_config_otg((uint32)PmicDeviceIndex, (pm_schg_otg_cfg_type)OtgCfgType, SetValue);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgSetOtgILimit
(
  IN  UINT32   PmicDeviceIndex,
  IN  UINT32   ImAmp
)
{
  EFI_STATUS        Status  = EFI_SUCCESS;
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_schg_otg_set_otg_i_limit((uint32)PmicDeviceIndex,(uint32)ImAmp);
  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}


/**
  Sets Maximum USB current

  @param MaxCurrent  Current to be set

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_INVALID_PARAMETER: A Parameter was incorrect.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_NOT_READY:         The physical device is busy or not ready to
                         process this request.
*/

EFI_STATUS
EFIAPI
EFI_PmicSchgSetUsbMaxCurrent
(
  IN UINT32 PmicDeviceIndex,
  IN UINT32 MaxCurrent
)
{
    EFI_STATUS        Status  = EFI_SUCCESS;
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    /* set desired MAX current */
    errFlag |= pm_schg_usb_set_usbin_current_limit_cfg(PmicDeviceIndex, MaxCurrent);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
      Status = EFI_DEVICE_ERROR;
    }

    return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicSchgEnableAfpMode
(
  IN  UINT32   PmicDeviceIndex
)
{

  if(PM_ERR_FLAG__SUCCESS != pm_schg_misc_en_afp((uint32)PmicDeviceIndex))
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgEnableShipMode
(
  IN  UINT32   PmicDeviceIndex
)
{

  if(PM_ERR_FLAG__SUCCESS != pm_schg_batif_enable_shipmode((uint32)PmicDeviceIndex))
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgSetInputPriority
(
   IN  UINT32                          PmicDeviceIndex,
   IN  EFI_PM_SCHG_INPUT_PRIORITY_TYPE InputPriority
)
{
  if(InputPriority >= EFI_PM_SCHG_INPUT_PRIORITY_INVALID)
  {
    return EFI_INVALID_PARAMETER;
  }

  if(PM_ERR_FLAG__SUCCESS != pm_schg_usb_set_usbin_option1_cfg(PmicDeviceIndex, PM_SCHG_USB_INPUT_PRIORITY, (boolean)InputPriority))
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EFI_PmicSchgEnableChgWdog
(
  IN UINT32  PmicDeviceIndex,
  IN BOOLEAN Enable
)
{
  EFI_STATUS                 Status  = EFI_SUCCESS;
  pm_err_flag_type           errFlag = PM_ERR_FLAG__SUCCESS;
  pm_schg_misc_wdog_cfg_type wd_cfg = {0};
  pm_schg_misc_sbb_wd_cfg    sbb_cfg = {0};
  pm_device_info_type        PmicDeviceInfo;

  errFlag = pm_get_pmic_info(PmicDeviceIndex, &PmicDeviceInfo);
  if((PmicDeviceInfo.ePmicModel == PMIC_IS_PMI8998) && (PMIC_ALL_LAYER_REV_2 > PmicDeviceInfo.nPmicAllLayerRevision) && (TRUE == Enable))
  {
    PMIC_FILE_UART_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Charger watchdog is not supported for PMi8998 versions (%d) before  v2.0 !!! \n\r", __FUNCTION__, PmicDeviceInfo.nPmicAllLayerRevision ));
    return EFI_UNSUPPORTED;
  }

  //Read HW Default watchdog configuration
  errFlag = pm_schg_misc_get_config_wdog(PmicDeviceIndex, &wd_cfg);
  Status = (PM_ERR_FLAG__SUCCESS == errFlag)? PM_ERR_FLAG__SUCCESS : EFI_DEVICE_ERROR;
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
      Status = EFI_DEVICE_ERROR;
      return Status;
  }

  //Enable watchdog and set timeout
  wd_cfg.bite_wdog_int_en = Enable;
  wd_cfg.bark_wdog_int_en = Enable;
  wd_cfg.wdog_timer_en = FALSE; /*Per PMIC system team recommendations, this bit should not be enabled. */
  wd_cfg.wdog_option = Enable;

  errFlag |= pm_schg_misc_get_snarl_bark_bite_wd_cfg(PmicDeviceIndex, &sbb_cfg);

  sbb_cfg.bite_wdog_disable_charging_cfg = Enable;
  
  if(TRUE == Enable)
  {
    sbb_cfg.bark_wdog_timeout = PM_SCHG_MISC_BARK_WDOG_TMOUT_128S;
    sbb_cfg.bite_wdog_timeout = PM_SCHG_MISC_BITE_WDOG_TMOUT_1S;
  }

  errFlag |= pm_schg_misc_set_snarl_bark_bite_wd_cfg(PmicDeviceIndex, sbb_cfg);

  errFlag |= pm_schg_misc_config_wdog(PmicDeviceIndex, wd_cfg);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
      Status = EFI_DEVICE_ERROR;
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Error configuring Watchdog, Status; %d\r\n", __FUNCTION__, errFlag));
      return Status;
  }
  else
  {
    if(Enable)
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Charger Watchdog Enabled\r\n", __FUNCTION__ ));
    }
    else
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a Charger Watchdog Disabled\r\n", __FUNCTION__ ));
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgSetFccMaxCurrent
(
  IN UINT32 PmicDeviceIndex,
  IN UINT32 MaxCurrentInmA
)
{

  if(PM_ERR_FLAG__SUCCESS != pm_schg_chgr_set_charge_current(PmicDeviceIndex, PM_SCHG_CHGR_FAST_CHARGE_CURRENT, MaxCurrentInmA))
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EFI_PmicSchgSetFvMaxVoltage
(
  IN UINT32 PmicDeviceIndex,
  IN UINT32 MaxVoltageInMv
)
{

  if(PM_ERR_FLAG__SUCCESS != pm_schg_chgr_set_float_volt(PmicDeviceIndex, MaxVoltageInMv))
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EFI_PmicSchgEnableOtg
(
  IN  UINT32   PmicDeviceIndex,
  IN  BOOLEAN  Enable
)
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  pm_err_flag_type               errFlag = PM_ERR_FLAG__SUCCESS;
  UINT32                         OtgCurrentLimit = PM_SCHG_OTG_CURRENT_LIMIT_250_MA;

  if (Enable)
  {
      /*---------------------*/
      /* OTG enable sequence */
      /*---------------------*/
      /* 1) Disable 1-in-8 mode(bit ENG_BUCKBOOST_HALT1_8_MODE = 1) before enabling OTG */
      errFlag = pm_schg_otg_halt_1_in_8_mode(PmicDeviceIndex, Enable);

      /* 2) Set 1.5A OTG current limit */
        OtgCurrentLimit = PM_SCHG_OTG_CURRENT_LIMIT_1500_MA;
      errFlag |= pm_schg_otg_set_otg_i_limit(PmicDeviceIndex, OtgCurrentLimit);

      /* 3) 4th FPFET soft start setting */
      errFlag |= pm_schg_dc_set_ivref_otg_ss(PmicDeviceIndex, PM_SCHG_OTG_WA_FPFET_SS);

      }
  errFlag = pm_schg_otg_enable(PmicDeviceIndex, Enable);

  if (!Enable)
      {
      /*----------------------*/
      /* OTG disable sequence */
      /*----------------------*/
      /* 1) Enable 1-in-8 mode(bit ENG_BUCKBOOST_HALT1_8_MODE = 0) */
      errFlag = pm_schg_otg_halt_1_in_8_mode(PmicDeviceIndex, Enable);

      /* 2) Set 250mA OTG current limit */
      errFlag |= pm_schg_otg_set_otg_i_limit(PmicDeviceIndex, OtgCurrentLimit);
    }

  Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}


EFI_STATUS
EFIAPI
EFI_PmicSchgGetOtgStatus
(
   IN  UINT32                       PmicDeviceIndex,
   OUT EFI_PM_SCHG_OTG_STATUS_TYPE  *pOtgStatus
)
{
    EFI_STATUS         Status      = EFI_SUCCESS;
    pm_err_flag_type   err_flag    = PM_ERR_FLAG__SUCCESS;
    BOOLEAN            OverCurrent = FALSE;
    pm_schg_otg_status_type TempOtgStatus = PM_SCHG_OTG_STATUS_INVALID;

    err_flag = pm_schg_otg_get_status(PmicDeviceIndex, &TempOtgStatus);
    err_flag |= pm_schg_otg_irq_status(PmicDeviceIndex, PM_SCHG_OTG_IRQ_OTG_OVERCURRENT, PM_IRQ_STATUS_LATCHED, (boolean *)&OverCurrent);

    if ((PM_ERR_FLAG__SUCCESS == err_flag) && OverCurrent)
    {
        *pOtgStatus = EFI_PM_SCHG_OTG_ERROR;
    
        PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a OTG over current! Executing OTG sequence \n\r", __FUNCTION__));

        /* Clear interrupt if latched status is on */
        err_flag |= pm_schg_otg_irq_clear(PmicDeviceIndex, PM_SCHG_OTG_IRQ_OTG_OVERCURRENT);
    
        /*---------------------------------*/
        /* Run OTG over current sequence */
        /*---------------------------------*/
        Status = PmicSchgOTGOverCurrent(PmicDeviceIndex);
    }
    else
    {
    /*Need to perform explicit matching since the order of enum elements is different*/
        switch (TempOtgStatus)
    {
      case PM_SCHG_OTG_DISABLED: 
        *pOtgStatus = EFI_PM_SCHG_OTG_DISABLED;
        break;
      case PM_SCHG_OTG_EXECUTING_ENABLE: 
        *pOtgStatus = EFI_PM_SCHG_OTG_EXECUTING_ENABLE;
        break;
      case PM_SCHG_OTG_ENABLED: 
        *pOtgStatus = EFI_PM_SCHG_OTG_ENABLED;
        break;
      case PM_SCHG_OTG_EXECUTING_DISABLE: 
        *pOtgStatus = EFI_PM_SCHG_OTG_EXECUTING_DISABLE;
        break;
      case PM_SCHG_OTG_STATUS_INVALID: 
        *pOtgStatus = EFI_PM_SCHG_OTG_DISABLED;
        break;
      case PM_SCHG_OTG_ERROR: 
        *pOtgStatus = EFI_PM_SCHG_OTG_ERROR;
        break;
      default: 
        *pOtgStatus = EFI_PM_SCHG_OTG_ERROR;
    }
    }

    Status = (PM_ERR_FLAG__SUCCESS == err_flag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

    return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgUsbSuspend
(
  IN UINT32  PmicDeviceIndex,
  IN BOOLEAN Enable
  )
{

  if(PM_ERR_FLAG__SUCCESS != pm_schg_usb_set_cmd_il(PmicDeviceIndex, PM_SCHG_USB_CMD_IL_TYPE_USBIN_SUSPEND, Enable))
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EFI_PmicSchgPetChgWdog
(
  IN UINT32 PmicDeviceIndex
)
{
  pm_err_flag_type          errFlag = PM_ERR_FLAG__SUCCESS;
  pm_schg_chgr_status_type  charger_status;

  errFlag |= pm_schg_chgr_get_chgr_status(PmicDeviceIndex, &charger_status);
  if((FALSE == charger_status.status_5.disable_charging) && (PM_ERR_FLAG__SUCCESS == errFlag))
  {//Pet wdog only if charging is enabled
    if (PM_ERR_FLAG__SUCCESS != pm_schg_misc_bark_bite_wdog_pet(PmicDeviceIndex))
    {
      PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a error while petting watchdog : (%d)\r\n", __FUNCTION__, errFlag ));
      return EFI_DEVICE_ERROR;
    }
  }
  else if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG(( EFI_D_WARN, "PmicDxe:: %a error while getting charger status: (%d)\r\n", __FUNCTION__, errFlag ));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgPrintAiclSts
(
IN  UINT32   PmicDeviceIndex,
OUT BOOLEAN  *IsAICLComplete
)
{
	EFI_STATUS        Status = EFI_SUCCESS;
    /*
	pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
	input_current_limit_sts icl_sts = { 0 };

	if (IsAICLComplete == NULL)
	{
        PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Invalid parameter isAICLComplete is NULL \r\n",  __FUNCTION__));
		return EFI_INVALID_PARAMETER;
	}

	err_flag = pm_schg_usb_icl_sts(PmicDeviceIndex, &icl_sts);
	if (PM_ERR_FLAG__SUCCESS != err_flag)
	{
        PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a AICL error while running pm_schg_usb_icl_sts (%d) \r\n",  __FUNCTION__, errFlag));
		return EFI_DEVICE_ERROR;
	}
	*IsAICLComplete = icl_sts.is_aicl_complete;
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a AICL status info AICL completed = %d ,\n\t\t\t \
		ICL mode = %d ,\n\t\t\t \
		usbin suspended = %d ,\n\t\t\t \
		dcin suspended = %d ,\n\t\t\t \
		usbin active pwr src = %d ,\n\t\t\t \
		dcin active pwr src = %d ,\n\t\t\t \
		Input Current Limited to %d \r\n",  __FUNCTION__, 
		icl_sts.is_aicl_complete,
		icl_sts.icl_mode,
		icl_sts.is_usbin_suspended,
		icl_sts.is_dcin_suspended,
		icl_sts.is_usbin_active_pwr_src,
		icl_sts.is_dcin_active_pwr_src,
		icl_sts.input_current_limit));
	
	*/
	return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgChgrSourceReinserted
(
  IN  UINT32   PmicDeviceIndex,
  OUT BOOLEAN  *pReinserted
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;
  BOOLEAN                              LatchedStatus = FALSE;
  BOOLEAN                              RtStatus = FALSE;

  if(NULL == pReinserted)
  {
    Status = EFI_INVALID_PARAMETER;
  }
  else
  {
    *pReinserted = FALSE;
    /*if latched is low*/
    errFlag = pm_schg_usb_irq_status(PmicDeviceIndex, PM_SCHG_USB_IRQ_USBIN_PLUGIN, PM_IRQ_STATUS_LATCHED, (boolean *)&LatchedStatus);
    if(LatchedStatus == TRUE)
    {
      /*Read IRQ real time status*/
      errFlag |= pm_schg_usb_irq_status(PmicDeviceIndex, PM_SCHG_USB_IRQ_USBIN_PLUGIN, PM_IRQ_STATUS_RT, (boolean *)&RtStatus);
      *pReinserted = (RtStatus == TRUE) ? TRUE : FALSE;
      /*clear the interrupt*/
      pm_schg_usb_irq_clear(PmicDeviceIndex, PM_SCHG_USB_IRQ_USBIN_PLUGIN);
    }

    Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? PM_ERR_FLAG__SUCCESS : EFI_DEVICE_ERROR;
  }

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgRerunAicl
(
  IN  UINT32                          PmicDeviceIndex,
  IN  EFI_PM_SCHG_AICL_RERUN_TIME_CFG RerunTime
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  /*
  TBD: Code will be enabled once HW is available and testing can be done
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag |= pm_schg_usb_aicl_options_cfg(PmicDeviceIndex, PM_SCHG_USB_AICL_CFG_RERUN_EN, TRUE);
  errFlag |= pm_schg_misc_set_aicl_rerun_time_cfg(PmicDeviceIndex, (pm_schg_misc_aicl_rerun_time_cfg)RerunTime);
  Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? PM_ERR_FLAG__SUCCESS : EFI_DEVICE_ERROR;
  */

  return Status;
}

EFI_STATUS PmicSchgDumpPeripheral( VOID )
{
  EFI_STATUS Status  = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_schg_chgr_get_schg_pmic_info((UINT8 *)&gSchgPmicIndex, (UINT8 *)&gSchgSlaveIndex);
  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe::%a SCHG_PERIPHERAL get_schg_pmic_info Error (%d) \n\r", __FUNCTION__, errFlag));
    return EFI_UNSUPPORTED;
  }

  errFlag = pm_core_utils_dump_peripheral(gSchgSlaveIndex, PMIC_SCHG_PERIPHERAL_START_ADDR, PMIC_SCHG_PERIPHERAL_NUM);
  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
   PMIC_DEBUG((EFI_D_WARN, "PmicDxe::%a SCHG_PERIPHERAL pm_core_utils_dump_peripheral Error (%d) \r\n",  __FUNCTION__, errFlag));
  }

  /* Dump separately for Misc peripheral */
  errFlag |= pm_core_utils_dump_peripheral(gSchgSlaveIndex, PMIC_SCHG_MISC_PERIPHERAL_START_ADDR, 1);
  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe::%a SCHG_MISC_PERIPHERAL pm_core_utils_dump_peripheral Error (%d) \n\r", __FUNCTION__, errFlag));
  }

  Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}

EFI_STATUS EFI_PmicSchgEnableJeita(UINT32 PmicDeviceIndex, EFI_PM_SCHG_JEITA_TYPE JeitaType)
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgGetChargingStatus
(
  UINT32 PmicDeviceIndex,
  OUT EFI_PM_SCHG_CHARGING_STATUS *pChargingStatus
)
{
  pm_err_flag_type          errFlag = PM_ERR_FLAG__SUCCESS;
  pm_schg_chgr_status_type  charger_status;

  if(NULL == pChargingStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  DALSYS_memset(&charger_status, 0x00, sizeof(charger_status));

  pChargingStatus->bChargingEnabled = FALSE;

  errFlag = pm_schg_chgr_get_chgr_status(PmicDeviceIndex, &charger_status);
  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Error getting charging status = %d \r\n", __FUNCTION__, errFlag));
    return EFI_DEVICE_ERROR;
  }

  pChargingStatus->bChargingEnabled = charger_status.status_5.charging_enable;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgDCInValid
(
IN  UINT32 PmicDeviceIndex,
OUT BOOLEAN *Valid
)
{
  EFI_STATUS            Status = EFI_SUCCESS;
  pm_err_flag_type      errFlag = PM_ERR_FLAG__SUCCESS;
  BOOLEAN               DcinPlugedIn;

  if (NULL == Valid)
  {
    Status = EFI_INVALID_PARAMETER;
  }
  else
  {
    /*get dcin irq status Read IRQ real time status*/
    errFlag = pm_schg_dc_irq_status(PmicDeviceIndex, PM_SCHG_DC_IRQ_DCIN_PLUGIN, PM_IRQ_STATUS_RT, (boolean*)&DcinPlugedIn);

    Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? PM_ERR_FLAG__SUCCESS : EFI_DEVICE_ERROR;

    *Valid = (DcinPlugedIn) ? TRUE : FALSE;
  }

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgDCInSuspend
(
IN  UINT32 PmicDeviceIndex,
IN  BOOLEAN Suspend
)
{
  EFI_STATUS            Status = EFI_SUCCESS;
  pm_err_flag_type      errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_schg_dc_set_cmd_il(PmicDeviceIndex, PM_SCHG_DC_CMD_IL_TYPE_DCIN_SUSPEND, Suspend);

  Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? PM_ERR_FLAG__SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgSetDCInPower
(
IN  UINT32  PmicDeviceIndex,
IN  UINT32  PowerInMicroWalts
)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint32 ilmit_level_ma = 0;

  //Configure PM_SCHG_DC_ZIN_ICL_PT;  Use 8V :  W=VI
  ilmit_level_ma = PowerInMicroWalts / PM_DC_ZIN_ICL_PT_VOLT_MV;
  err_flag |= pm_schg_dc_set_zin_icl(PmicDeviceIndex, PM_SCHG_DC_ZIN_ICL_PT, ilmit_level_ma);

  //Configure PM_SCHG_DC_ZIN_ICL_PT_HV;  9V 
  ilmit_level_ma = PowerInMicroWalts / PM_DC_ZIN_ICL_PT_HV_VOLT_MV;
  err_flag |= pm_schg_dc_set_zin_icl(PmicDeviceIndex, PM_SCHG_DC_ZIN_ICL_PT_HV, ilmit_level_ma);

  //Configure PM_SCHG_DC_ZIN_ICL_LV; Use 5.5V 
  ilmit_level_ma = PowerInMicroWalts / PM_DC_ZIN_ICL_LV_VOLT_MV;
  err_flag |= pm_schg_dc_set_zin_icl(PmicDeviceIndex, PM_SCHG_DC_ZIN_ICL_LV, ilmit_level_ma);

  //Configure PM_SCHG_DC_ZIN_ICL_MID_LV; Use 6.5V 
  ilmit_level_ma = PowerInMicroWalts / PM_DC_ZIN_ICL_MID_LV_VOLT_MV;
  err_flag |= pm_schg_dc_set_zin_icl(PmicDeviceIndex, PM_SCHG_DC_ZIN_ICL_MID_LV, ilmit_level_ma);

  //Configure PM_SCHG_DC_ZIN_ICL_MID_HV; Use 8V 
  ilmit_level_ma = PowerInMicroWalts / PM_DC_ZIN_ICL_MID_HV_VOLT_MV;
  err_flag |= pm_schg_dc_set_zin_icl(PmicDeviceIndex, PM_SCHG_DC_ZIN_ICL_MID_HV, ilmit_level_ma);

  //Configure PM_SCHG_DC_ZIN_ICL_HV; Use 11V 
  ilmit_level_ma = PowerInMicroWalts / PM_DC_ZIN_ICL_HV_VOLT_MV;
  err_flag |= pm_schg_dc_set_zin_icl(PmicDeviceIndex, PM_SCHG_DC_ZIN_ICL_HV, ilmit_level_ma);

  return err_flag;
}


EFI_STATUS
EFIAPI
EFI_PmicSchgSetUsbIclMode
(
  UINT32                            PmicDeviceIndex,
  IN EFI_PM_SCHG_USB_ICL_MODE_TYPE  IclModeType
)
{
  pm_err_flag_type              errFlag = PM_ERR_FLAG__SUCCESS;

  switch(IclModeType)
  {
    case EFI_PM_SCHG_USB_ICL_MODE_SDP_2_0: 
         /**< Select USB 2.0 CFG, Set USB51 Mode to 500 mA, set USBIN_MODE to USB 5/1 or USB 9/1.5 Current Level >*/
         errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_USBIN_MODE_CHG, FALSE);
         errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_USB51_MODE, TRUE);
         errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_CFG_USB3P0_SEL, FALSE);
         break;
    case EFI_PM_SCHG_USB_ICL_MODE_SDP_3_0:
         /**< Select USB 3.0 CFG, Set USB51 Mode to 500 mA, set USBIN_MODE to USB 5/1 or USB 9/1.5 Current Level >*/
         errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_USBIN_MODE_CHG, FALSE);
         errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_USB51_MODE, TRUE);
         errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_CFG_USB3P0_SEL, TRUE);
         break;
    case EFI_PM_SCHG_USB_ICL_MODE_WALL_CHARGERS: 
         /**< Select USB 2.0 CFG(Default), Set USB51 Mode to 100 mA, set USBIN_MODE to HC Mode Current Level     >*/
         errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_USBIN_MODE_CHG, TRUE);
         errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_USB51_MODE, FALSE);
         errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_CFG_USB3P0_SEL, FALSE);
         break;
    default:
         return EFI_INVALID_PARAMETER;
  }

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Error configuring ICL options, status = %d \r\n", __FUNCTION__, errFlag));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EFI_PmicSchgGetChgWdogStatus
(
  IN UINT32                     PmicDeviceIndex,
  OUT EFI_PM_SCHG_WDOG_STS_TYPE *pWdogSts
)
{
  pm_err_flag_type              errFlag = PM_ERR_FLAG__SUCCESS;
  pm_schg_misc_wdog_status      WdogStatus;
  pm_schg_misc_sbb_wd_cfg       WdogCfg;
  BOOLEAN                       WdogBarkBiteSts = FALSE;
  
  if(NULL == pWdogSts)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pWdogSts = EFI_PM_SCHG_WDOG_STS_INVALID;

  errFlag = pm_schg_misc_irq_status(PmicDeviceIndex, PM_SCHG_MISC_IRQ_WDOG_BARK, PM_IRQ_STATUS_LATCHED,  (boolean *)&WdogBarkBiteSts);

  if(WdogBarkBiteSts)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Charger watchdog bark/bite status: %d \r\n", __FUNCTION__, WdogBarkBiteSts));

    /*clear the interrupt*/
    errFlag |= pm_schg_misc_irq_clear(PmicDeviceIndex, PM_SCHG_MISC_IRQ_WDOG_BARK);

    errFlag = pm_schg_misc_get_misc_wdog_status(PmicDeviceIndex, &WdogStatus);
    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
      PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Error getting wdog status, status = %d \r\n", __FUNCTION__, errFlag));
      return EFI_DEVICE_ERROR;
    }
    
    errFlag = pm_schg_misc_get_snarl_bark_bite_wd_cfg( PmicDeviceIndex, &WdogCfg );
    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
      PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Error getting wdog snark/bark/bite status, status = %d \r\n", __FUNCTION__, errFlag));
      return EFI_DEVICE_ERROR;
    }
    
    switch(WdogStatus)
    {
      case PM_SCHG_MISC_BARK:
           *pWdogSts = EFI_PM_SCHG_WDOG_STS_BARK;
           break;
      case PM_SCHG_MISC_BITE:
           if(WdogCfg.bite_wdog_disable_charging_cfg)
           {
             *pWdogSts = EFI_PM_SCHG_WDOG_STS_BITE_CHARGING_DISABLED;
           }
           else
           {
             *pWdogSts = EFI_PM_SCHG_WDOG_STS_BITE_CHARGING_NOT_DISABLED;
           }
           break;
      default: 
           *pWdogSts = EFI_PM_SCHG_WDOG_STS_INVALID;
           break;
    }
  }
 
  return EFI_SUCCESS;
}

EFI_STATUS EFI_PmicSchgEnableHWJeita(UINT32 PmicDeviceIndex, BOOLEAN Enable)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  if(TRUE == Enable)
  {
    /* Set Compensation for Fv and Fcc */
    errFlag |= pm_schg_chgr_set_jeita_fvcomp_cfg(PmicDeviceIndex, gSchgCfgData.JeitaFvCompCfg);
    errFlag |= pm_schg_chgr_set_jeita_ccomp(PmicDeviceIndex, gSchgCfgData.JeitaCcCompCfg);

    errFlag |= pm_schg_chgr_jeita_en_cfg(PmicDeviceIndex, PM_SCHG_CHGR_JEITA_HARD_LIMIT, TRUE);
    errFlag |= pm_schg_chgr_jeita_en_cfg(PmicDeviceIndex, PM_SCHG_CHGR_JEITA_HOT_SL_FVC, TRUE);
    errFlag |= pm_schg_chgr_jeita_en_cfg(PmicDeviceIndex, PM_SCHG_CHGR_JEITA_COLD_SL_FVC, TRUE);
    errFlag |= pm_schg_chgr_jeita_en_cfg(PmicDeviceIndex, PM_SCHG_CHGR_JEITA_HOT_SL_CCC, TRUE);
    errFlag |= pm_schg_chgr_jeita_en_cfg(PmicDeviceIndex, PM_SCHG_CHGR_JEITA_COLD_SL_CCC, TRUE);
  }
  else
  {
    errFlag = pm_schg_chgr_jeita_en_cfg(PmicDeviceIndex, PM_SCHG_CHGR_JEITA_HARD_LIMIT, FALSE);
  }

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Error = %d \r\n", __FUNCTION__, errFlag));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS EFI_PmicSchgToggleWipowerSDLatch(UINT32 PmicDeviceIndex)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_schg_dc_set_cmd_il(PmicDeviceIndex, PM_SCHG_DC_CMD_IL_TYPE_SHDN_N_CLEAR_CMD, TRUE);
  errFlag |= pm_schg_dc_set_cmd_il(PmicDeviceIndex, PM_SCHG_DC_CMD_IL_TYPE_SHDN_N_CLEAR_CMD, FALSE);

  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Error = %d \r\n", __FUNCTION__, errFlag));
    return EFI_DEVICE_ERROR;
  }

  PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a status = %d \r\n", __FUNCTION__, errFlag));
  return EFI_SUCCESS;
}

EFI_STATUS EFI_PmicSchgSetDcInCurrent(UINT32 PmicDeviceIndex, UINT32 dcInCurrentLimit)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_schg_dc_set_usbin_current_limit_cfg(PmicDeviceIndex, dcInCurrentLimit);

  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a Error = %d \r\n", __FUNCTION__, errFlag));
    return EFI_DEVICE_ERROR;
  }

  PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a status = %d \r\n", __FUNCTION__, errFlag));
  return EFI_SUCCESS;
}

EFI_STATUS EFI_PmicSchgSetChargeCmdBit(UINT32 PmicDeviceIndex, BOOLEAN Enable)
{
  pm_err_flag_type     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_schg_chgr_set_chgr_cmd_type(PmicDeviceIndex, PM_SCHG_CHGR_CMD_CHARGING_ENABLE, Enable);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
EFI_PmicSchgGetPmicInfo ()

@brief
Provides SCHG charger PMIC Index.
*/
EFI_STATUS
EFIAPI
EFI_PmicSchgGetPmicInfo
(
  OUT EFI_PM_SCHG_PMIC_INFO *SchgPmicInfo
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  if(!SchgPmicInfo)
  {
    return EFI_INVALID_PARAMETER;
  }

  errFlag = pm_schg_chgr_get_schg_pmic_info((UINT8 *)&gSchgPmicIndex, (UINT8 *)&gSchgSlaveIndex);
  SchgPmicInfo->PmicIndex = gSchgPmicIndex;
  SchgPmicInfo->SlaveIndex = gSchgSlaveIndex;

  return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS PmicSchgSetHwAfpThresholds(UINT32 PmicDeviceIndex, INT32 HotThreshold, INT32 ColdThreshold)
{
#define ZERO_DEG_C_IN_KELVIN   273

  pm_err_flag_type     errFlag = PM_ERR_FLAG__SUCCESS;
  INT32 HotData = 0;
  INT32 ColdData = 0;

  HotData = (HotThreshold + ZERO_DEG_C_IN_KELVIN) / 2;
  ColdData = (ColdThreshold + ZERO_DEG_C_IN_KELVIN) / 2;

  errFlag = pm_app_chgr_enable_temp_monitoring(HotData, ColdData);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS PmicSchgSetThermRegSrcCfg(UINT32 PmicDeviceIndex, BOOLEAN SkinEnable, BOOLEAN DieEnable, BOOLEAN ComparatorEnable)
{
  pm_err_flag_type     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_schg_misc_set_thermreg_src_cfg(PmicDeviceIndex, PM_SCHG_MISC_SKIN_ADC_SRC_EN, SkinEnable);
  errFlag |= pm_schg_misc_set_thermreg_src_cfg(PmicDeviceIndex, PM_SCHG_MISC_DIE_ADC_SRC_EN, DieEnable);
  errFlag |= pm_schg_misc_set_thermreg_src_cfg(PmicDeviceIndex, PM_SCHG_MISC_DIE_CMP_SRC_EN, ComparatorEnable);

  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS PmicSchgOTGOverCurrent(UINT32 PmicDeviceIndex)
{
	EFI_STATUS         Status = EFI_SUCCESS;
	pm_err_flag_type   err_flag = PM_ERR_FLAG__SUCCESS;
	uint8 i = 0;
	BOOLEAN otg_oc_sts = FALSE;
	BOOLEAN bSoftStartDone = FALSE;
	BOOLEAN bOtgOK = FALSE;

	/*---------------------------------*/
	/*  Run OTG over current sequence  */
	/*---------------------------------*/
	for (i = 0; i<3; i++)
	{
		/* [WA] Disable OTG */
		err_flag = pm_schg_otg_enable(PmicDeviceIndex, 0);

		if (PM_ERR_FLAG__SUCCESS == err_flag)
		{
			/* [WA] Check if OTG_OVERCURRENT_LATCHED_STS is 0 */
			err_flag = pm_schg_otg_irq_status(PmicDeviceIndex, PM_SCHG_OTG_IRQ_OTG_OVERCURRENT, PM_IRQ_STATUS_LATCHED, (boolean *)&otg_oc_sts);

			if (PM_ERR_FLAG__SUCCESS == err_flag)
			{
				if (!otg_oc_sts)
				{
					/* [WA] Do not check for USBIN < 1V before reenabling OTG 1153[4] = 1*/
					err_flag = pm_schg_otg_config_otg(PmicDeviceIndex, PM_SCHG_OTG_CFG_QUICKSTART_FASTROLESWAP, TRUE);

					/* [WA] Re-enable OTG */
					err_flag |= pm_schg_otg_enable(PmicDeviceIndex, 1);
				}
				else
				{
					/* Clear interrupt if latched status is on */
					err_flag |= pm_schg_otg_irq_clear(PmicDeviceIndex, PM_SCHG_OTG_IRQ_OTG_OVERCURRENT);
				}
			}
		}
	}

	if (PM_ERR_FLAG__SUCCESS == err_flag)
	{
		/* [WA] Verify boost output is regulating at 5V and BOOST softstart done: 1110[2:0]=000 & 1109[3]=1*/
		err_flag = pm_schg_otg_get_rt_sts_ok(PmicDeviceIndex, &bOtgOK);

		err_flag |= pm_schg_otg_get_boost_softstart_done_status(PmicDeviceIndex, &bSoftStartDone);


		if ((PM_ERR_FLAG__SUCCESS == err_flag) && bOtgOK && bSoftStartDone)
		{
			/* [WA] Re-enable check for USBIN < 1V before reenabling OTG 1153[4] = 0*/
			err_flag = pm_schg_otg_config_otg(PmicDeviceIndex, PM_SCHG_OTG_CFG_QUICKSTART_FASTROLESWAP, FALSE);
		}
	}

	Status = (PM_ERR_FLAG__SUCCESS == err_flag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

	return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicSchgConfigApsd
(
  IN  UINT32                      PmicDeviceIndex,
  IN  EFI_SCHG_APSD_CFGDATA_TYPE *pSchgApsdDataType
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  if(!pSchgApsdDataType)
    return EFI_INVALID_PARAMETER;

  errFlag |= pm_schg_usb_set_cmd_apsd(PmicDeviceIndex, PM_SCHG_USB_APSD_RERUN, pSchgApsdDataType->bApsdRerun);

  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;

}

EFI_STATUS
EFIAPI
EFI_PmicSchgConfigHvdcp
(
  IN UINT32                       PmicDeviceIndex,
  IN EFI_SCHG_HVDCP_CFGDATA_TYPE *pSchgHvdcpDataType
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  if(!pSchgHvdcpDataType)
    return EFI_INVALID_PARAMETER;

  errFlag  = pm_schg_usb_set_usbin_option1_cfg(PmicDeviceIndex, PM_SCHG_USB_HVDCP_AUTH_ALG_EN_CFG, pSchgHvdcpDataType->bEnHvdcp);
  errFlag |= pm_schg_usb_set_usbin_option1_cfg(PmicDeviceIndex, PM_SCHG_USB_HVDCP_AUTONOMOUS_MODE_EN_CFG, pSchgHvdcpDataType->bEnHvdcp);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;

}

/**
EFI_PmicSchgGetChargerConnectorType ()

@brief
Gets charger connector type
*/
EFI_STATUS
EFIAPI
EFI_PmicSchgGetChargerConnectorType
(
  IN  UINT32 PmicDeviceIndex,
  OUT EFI_PM_SCHG_USB_CONNECTOR_TYPE *ConnectorType
)
{
  pm_err_flag_type  errFlag       = PM_ERR_FLAG__SUCCESS;

  pm_schg_usb_connector_type  UsbConnectorType;
  
  if(NULL == ConnectorType)
  {
    return EFI_INVALID_PARAMETER;
  }

  errFlag = pm_schg_usb_get_usb_connector_type(PmicDeviceIndex, &UsbConnectorType );

  *ConnectorType =(EFI_PM_SCHG_USB_CONNECTOR_TYPE)UsbConnectorType;
  
  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    *ConnectorType = EFI_PM_SCHG_USB_CONNECTOR_TYPE_INVALID;
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS PmicSchgSetChargerDefaultSetting(UINT32 PmicDeviceIndex)
{
  pm_err_flag_type    errFlag = PM_ERR_FLAG__SUCCESS;
  
  /* Force ICL=500mA and High Current Mode */
  errFlag = pm_schg_usb_set_usbin_current_limit_cfg(PmicDeviceIndex, PM_FORCE_USBICL_SDP_FLOAT_DEFAULT_IN_MA);
  errFlag |= pm_schg_usb_configure_usbin_icl_options(PmicDeviceIndex, PM_SCHG_USB_ICL_OPTIONS_USBIN_MODE_CHG, TRUE);
  /* Make sure input current will not be modified by APSD */
  errFlag |= pm_schg_usb_enable_icl_override_after_apsd(PmicDeviceIndex, TRUE);
  
  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a fail to apply default setting for charger, Error = %d \r\n", __FUNCTION__, errFlag));
    return EFI_DEVICE_ERROR;    
  }
  
  PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a Setting ICL = 500 mA \n\r", __FUNCTION__));
  return EFI_SUCCESS;
}

EFI_STATUS PmicSchgInitialChargerCheckup(UINT32 PmicDeviceIndex)
{
  pm_err_flag_type    errFlag = PM_ERR_FLAG__SUCCESS;
  BOOLEAN             ApsdDoneStatus = FALSE;
  pm_schg_usb_apsd_result_status_type PortType = PM_SCHG_USB_APSD_RESULT_STATUS_INVALID;
    
  /* Check APSD result if it is available  */
  errFlag = pm_schg_usb_get_apsd_status(PmicDeviceIndex, PM_SCHG_USB_APSD_STATUS_APSD_DTC_STATUS_DONE , &ApsdDoneStatus);
  errFlag |= pm_schg_usb_get_apsd_result_status(PmicDeviceIndex, &PortType);
  
  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a fail to get apsd status or result, Error = %d \r\n", __FUNCTION__, errFlag));
    return EFI_DEVICE_ERROR;  
  }

  PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a APSD done status: %d, Port Type = %d \n\r", __FUNCTION__, ApsdDoneStatus, PortType));
  
  /* Increase ICL to 1500mA when APSD result is not SDP/Float Charger/Invalid */
  if( ApsdDoneStatus &&
      PortType != PM_SCHG_USB_APSD_RESULT_STATUS_SDP_CHARGER &&
      PortType != PM_SCHG_USB_APSD_RESULT_STATUS_FLOAT_CHARGER &&
      PortType != PM_SCHG_USB_APSD_RESULT_STATUS_INVALID)
  {
    errFlag = pm_schg_usb_set_usbin_current_limit_cfg(PmicDeviceIndex, PM_FORCE_USBICL_OTHER_IN_MA);

    if (PM_ERR_FLAG__SUCCESS != errFlag)
    {
      PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a fail to setting ICL, Error = %d \r\n", __FUNCTION__, errFlag));
      return EFI_DEVICE_ERROR;
    }
    
    PMIC_DEBUG((EFI_D_ERROR, "PmicDxe:: %a Setting ICL = 1500 mA (port type = %d)\n\r", __FUNCTION__, PortType));      
  }
  return EFI_SUCCESS;
}

/**
PMIC SCHG UEFI Protocol implementation
*/
EFI_QCOM_PMIC_SCHG_PROTOCOL PmicSchgProtocolImplementation = 
{
    PMIC_SCHG_REVISION,
    EFI_PmicSchgInit,
    EFI_PmicSchgEnableCharger,
    EFI_PmicSchgGetPowerPath, 
    EFI_PmicSchgIsBatteryPresent,
    EFI_PmicSchgGetChargerPortType,
    EFI_PmicSchgConfigOtg,
    EFI_PmicSchgSetOtgILimit,
    EFI_PmicSchgEnableAfpMode,
    EFI_PmicSchgSetInputPriority,
    EFI_PmicSchgSetFccMaxCurrent,
    EFI_PmicSchgSetFvMaxVoltage,
    EFI_PmicSchgEnableOtg,
    EFI_PmicSchgUsbSuspend,
    EFI_PmicSchgEnableJeita,
    EFI_PmicSchgGetOtgStatus,
    EFI_PmicSchgUsbinValid,    
    EFI_PmicSchgSetUsbMaxCurrent,
    EFI_PmicSchgChgrSourceReinserted,
    EFI_PmicSchgRerunAicl,
    //EFI_PmicSchgPrintAiclSts,
    EFI_PmicSchgDumpPeripheral,
    EFI_PmicSchgEnableChgWdog,
    EFI_PmicSchgPetChgWdog,
    EFI_PmicSchgGetChargingStatus,
    EFI_PmicSchgDCInValid,
    EFI_PmicSchgDCInSuspend,
    EFI_PmicSchgSetDCInPower,
    EFI_PmicSchgExit,
    EFI_PmicSchgSetUsbIclMode,
    EFI_PmicSchgGetChgWdogStatus,
    EFI_PmicSchgEnableHWJeita,
    EFI_PmicSchgToggleWipowerSDLatch,
    EFI_PmicSchgSetDcInCurrent,
    EFI_PmicSchgSetChargeCmdBit,
    EFI_PmicSchgGetPmicInfo,
    EFI_PmicSchgConfigApsd,
    EFI_PmicSchgConfigHvdcp,
    EFI_PmicSchgEnableShipMode,
    EFI_PmicSchgGetChargerConnectorType,

};

