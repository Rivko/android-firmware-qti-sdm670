/** @file ChargerPlatform.c

  Implements Qualcomm's battery voltage based threshold Charging solution

  Copyright (c) 2016-2017,  Qualcomm Technologies Inc. All rights reserved.

**/

/*=====================================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     -----------------------------------------------------------------
 12/28/17     pbitra  Enable BMD explicitly
 12/26/17     pbitra  Rerun APSD and reconfigure USB ICL in every periodical check
 07/12/17     cs      remove redundent enum definition for platform charger Action
 06/21/17     dc      Move Platform Check to QcomChargerDxe and support config override
 06/07/17     cs      add configurable delay before enabling BMD
 05/16/17     cs      fixed in no charging wait state, battery status is printing all 0s
 04/24/17     va      Enable Charger App Changes
 03/30/17     ll      Implemented level file-logging
 03/27/17     pbitra  Updated to support section based config for different platforms.
 03/07/17     cs      remove duplicate get errors checking emergency shutdown condition.
 03/09/17     ll      Updated code based on latest smem_mem_type
 02/07/17     sm      add config to enable/disable INOV
 01/31/17     va      Adding Rconn configuration for considering compensation
 01/27/17     sm      Added support for pbs triggered AFP
 01/23/17     sm      CFG parameter Structure changes
 11/10/16     va      Adding Config VBtEmpty threshold, soc update for smem info
 11/09/16     sm      Added changes to disable watchdog before applying the config setting.
 10/19/16     sm      Removed EnableHvdcp cfg param from ChargerPlatform_ReadCfgParams()
 10/17/16     sm      Added SetChargeCmdBit() API call once CHARGERLIB_CHG_WDOG_STS_BITE_CHARGING_DISABLED is detected.
 10/14/16     va      adding aux coffs settings  
 10/06/16     cs      configure batt therm/aux therm bias wait 
 09/29/16     va      Emergency shutdown behavior changes and debug log update
 09/13/16     sm      removed unused parameters from ChargerPlatform_ReadCfgParams()
 09/06/16     cs      wipower changes to remove 3.5 sec, not suspend DCIn
 08/24/16     sm      Added changes for supporting different config parameters settings for 
                      different platforms, MTP, QRD, etc.
 08/12/16     sm      Merged Vbatt and SOC based Platform files.
 08/05/16     va      Adding Charger led indication support
 07/26/16     va      Restarting FG Changes on warm boot 
 07/15/16     cs      Adding Wipower support
 06/28/16     va      Changes to share charger info to smem 
 06/24/16     sm      Added check for charging enabled in GetChargingAction()
 06/23/16     sm      Added EnableHvdcp in ChargerPlatform_ReadCfgParams()
 06/23/16     va      Adding support for Charger Fg Peripheral dumps
 06/14/16     va      Adding Thermal Support 
 06/09/16     sm      Added SupportHostMode in ChargerPlatform_ReadCfgParams()
 05/31/16     va      Adding Battery profile load feature
 05/24/16     sm      Added changes to handle No_Charger_wait case in GetChargingAction()
 05/10/16     sm      Added ChargerPlatform_GetConfigData()
 04/26/16     va      Adding parameters for Aux Coffes, SkinHot and Charger Hot settings
 04/21/16     sm      Added API call for Watchdog
                      Added EnableChargerWdog in ChargerPlatform_ReadCfgParams
 04/12/16     sm      Edited variables in ChargerPlatform_ReadCfgParams()
 04/07/16     sm      Added ChargerPlatform_GetUefiVariableValues()
 03/31/16     sm      Completing API definitions
 03/28/16     va      Add logging CHARGER_DEBUG
 03/21/16     va      Remove dummy configs and add battery missing config
 01/27/16     sm      Initial revision

=============================================================================*/
#include <Uefi.h>

/**
  EFI interfaces
 */
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomUtilsLib.h>
#include <api/pmic/pm/pm_uefi.h>  
#include <Library/FuseControlLib.h>
#include <Library/QcomBaseLib.h>

#include <Protocol/EFIVariableServices.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>

/**
  EFI interfaces
 */
#include <Protocol/GraphicsOutput.h>
#include <Protocol/EFIPlatformInfo.h>

#include "QcomChargerPlatform.h"
#include "QcomChargerPlatform_File.h"

#include <api/pmic/pm/pm_chg.h>

/**
Smem Interface 
*/
#include "smem.h"

/*===========================================================================*/
/*                  FUNCTIONS PROTOTYPES                                     */
/*===========================================================================*/
#define QCOM_CHARGER_IDLE_WAIT_DURATION            3000 /*3 sec in milliseconds*/
#define QCOM_CHARGER_MS_TO_S                       1000

/* UI would be displayed if VBAT read is less than 3700mV is 7% Soc. 
   Having a margin of extra 50mV to consider on the top of error margin */
#define QCOM_CHARGER_VBAT_ERROR_MARGIN 0 /*in Mv as of now keeping default as 0 error margin */
#define QCOM_CHARGER_LOW_THERSHOLD_VBAT 3700 /*in Mv*/
#define QCOM_CHARGER_INVALID_VALUE_MARKER  -1


STATIC UINT32 gThresholdVbatt = 0;
STATIC UINT32 gThresholdSoc   = 0;

QCOM_CHARGER_PLATFORM_CFGDATA_TYPE gChargerPlatformCfgData; 

extern BOOLEAN ExitChargingLoop;
BOOLEAN gChargingInitialized = FALSE;

pm_chg_info_type gChargerSharedInfo;

STATIC EFI_PLATFORMINFO_PLATFORM_TYPE PlatformType;
STATIC BOOLEAN gChargerCfgInitialized = FALSE;

EFI_STATUS ChargerPlatform_GetUefiVariableValues( void );
EFI_STATUS ChargerPlatform_SaveChargeInfoToSmem(pm_chg_info_type *ChargerActionInfo);
EFI_STATUS ChargerPlatform_ProfileLoadingInit(EFI_QCOM_CHARGER_ACTION_TYPE *pActionType, QCOM_CHARGER_PLATFORM_ACTION_INFO *pChargerActionInfo, CHARGERLIB_CHARGING_ERROR_TYPES errType);
EFI_STATUS ChargerPlatform_CheckIfOkToBoot(EFI_QCOM_CHARGER_ACTION_TYPE *pActionType, QCOM_CHARGER_PLATFORM_ACTION_INFO ChargerActionInfo, QCOM_CHARGER_BATT_STATUS_INFO             CurrentBatteryStatus);
EFI_STATUS ChargerPlatform_HandleShipMode(void);

/*===========================================================================*/
/*                  FUNCTIONS PROTOTYPES                                     */
/*===========================================================================*/
/*
ChargerPlatform_Init(): This function locates and initializes ADC Protocal, Charger Protocal and other Protocols that are 
needed for that specific platform. It also loads the cfg file and initialize charger and FG accordingly. 
*/
EFI_STATUS ChargerPlatform_Init( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;

  SetMem(&gChargerPlatformCfgData, sizeof(QCOM_CHARGER_PLATFORM_CFGDATA_TYPE), 0x00);
  SetMem(&gChargerSharedInfo, sizeof(pm_chg_info_type), 0xFF);

  Status |= GetPlatformType(&PlatformType);
  if(EFI_SUCCESS != Status)
  {
    DEBUG((EFI_D_WARN, "QcomChargerPlatform:: %a Error getting platform type  \r\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  //Initialize Charger and Gauge HW Type to be invalid before reading Cfg Data
  gChargerPlatformCfgData.ChargerLibCfgData.plat_hw_cfg_override = CHARGERLIB_PLATFORM_HW_CONFIG_INVALID;

  /* Load CFG file */
  Status |= ChargerPlatformFile_ReadDefaultCfgData();
  if(EFI_SUCCESS == Status)
  {
    CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Successfully read config file\n\r", __FUNCTION__));
    gChargerCfgInitialized = TRUE;
  }

  /* Read debug var from pmic bds menu */
  Status  |= ChargerPlatform_GetUefiVariableValues();

  if ((EFI_SUCCESS == Status) && (TRUE == gChargerPlatformCfgData.ChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg_to_file))
  {
    Status = ChargerPlatformFile_FileLogInit(gChargerPlatformCfgData);
  }

  CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a chargerlib_cfgdata_type = %d QCOM_CHARGER_PLATFORM_CFGDATA_TYPE = %d \r\n", __FUNCTION__, sizeof(chargerlib_cfgdata_type), sizeof(QCOM_CHARGER_PLATFORM_CFGDATA_TYPE) ));

  /* Init Charger lib */
  Status |= ChargerLibCommon_Init((chargerlib_cfgdata_type*)&gChargerPlatformCfgData.ChargerLibCfgData);
  if (EFI_UNSUPPORTED == Status)
  {
      CHARGER_DEBUG(( EFI_D_ERROR, "QcomChargerDxe:: %a Error: Charging is not supported for this platform. \r\n", __FUNCTION__));
      return EFI_SUCCESS;
  }
  else if(EFI_SUCCESS != Status)
  {
    CHARGER_DEBUG(( EFI_D_ERROR, "QcomChargerDxe:: %a Error when initializing ChargerLibCommon = %r \r\n", __FUNCTION__, Status));
    return Status;
  }
  else
  {
    CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Successfully initialized ChargerLibCommon %r \r\n", __FUNCTION__, Status));
  }

  if ((EFI_SUCCESS == Status) && (TRUE == gChargerPlatformCfgData.ChargerLibCfgData.shipmode_cfg.enable_shipmode))
  {
    Status = ChargerPlatform_HandleShipMode();
  }

  gThresholdVbatt = gChargerPlatformCfgData.ChargerLibCfgData.boot_to_hlos_threshold_in_mv;
  gThresholdSoc   = gChargerPlatformCfgData.ChargerLibCfgData.boot_to_hlos_threshold_in_soc;

  /* Enable HW JEITA*/
  Status |= ChargerLib_EnableHWJeita(TRUE);

  /*Disable Charger wdog before applying config based wdog setting*/
  Status = ChargerLib_EnableWdog(FALSE);

  if((QCOM_CHG_WDOG_DISABLE_ON_EXIT == gChargerPlatformCfgData.ChargerLibCfgData.enable_charger_wdog)|| (QCOM_CHG_WDOG_LEAVE_ENABLED_ON_EXIT == gChargerPlatformCfgData.ChargerLibCfgData.enable_charger_wdog))
  {
    /* Enable Charger Watchdog*/
    Status = ChargerLib_EnableWdog(TRUE);
    if(EFI_SUCCESS == Status)
    {
      CHARGER_DEBUG(( EFI_D_ERROR, "QcomChargerDxe:: %a Enabled WDOG.\r\n", __FUNCTION__));
    }
    else if (EFI_UNSUPPORTED == Status)
    {
      /*Do not need to return status as unsupported. Debug statement will be printed from SCG protocol API*/
      Status = EFI_SUCCESS;
    }
  }

  return Status;
}

EFI_STATUS ChargerPlatform_GetChargingAction(EFI_QCOM_CHARGER_ACTION_TYPE *pActionType, QCOM_CHARGER_PLATFORM_ACTION_INFO *pChargerActionInfo, BOOLEAN vbattChecking)
{
  EFI_STATUS Status                = EFI_SUCCESS;
  BOOLEAN    bChargerReinserted    = FALSE;
  CHARGERLIB_CHARGING_ERROR_TYPES           ErrorType  = CHARGERLIB_CHARGING_ERROR_NONE;
  STATIC EFI_QCOM_CHARGER_ACTION_TYPE  PrevChargerAction = EFI_QCOM_CHARGER_ACTION_INVALID;
  BOOLEAN                                   ChargingEnabled    = FALSE;

  if (!pChargerActionInfo || !pActionType)
    return EFI_INVALID_PARAMETER;

  /* Get Error like debug board or battery not detected first */
  Status |= ChargerLib_GetErrors(vbattChecking, &ErrorType);
  if(EFI_SUCCESS != Status)
  {
    CHARGER_DEBUG(( EFI_D_ERROR, "QcomChargerDxe:: %a Error Getting Battery Error = %r.\r\n", __FUNCTION__, Status));
    *pActionType = EFI_QCOM_CHARGER_ACTION_SHUTDOWN;
    PrevChargerAction = *pActionType;
    return Status;
  }

  if((CHARGERLIB_CHARGING_ERROR_BATTERY_NOT_DETECTED == ErrorType ) || (CHARGERLIB_CHARGING_ERROR_DEBUG_BOARD == ErrorType ) ||
    (CHARGERLIB_DEVICE_ERROR == ErrorType ) || (CHARGERLIB_CHARGING_ERROR_UNKNOWN_BATTERY == ErrorType ) ||
    (CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL == ErrorType) || (CHARGERLIB_CHARGING_ERROR_TSENSE_TIMEOUT == ErrorType) || (CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH == ErrorType))
  {
    Status = ChargerLib_GetErrorAction(ErrorType, (((CHARGERLIB_ERROR_ACTION_TYPE*)pActionType)));
    PrevChargerAction = *pActionType;
    CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a pActionType = %d.\r\n", __FUNCTION__, *pActionType));
    /*If there is a battery error, return */
    return Status;
  }

  /* Assign Led config to toggle led */
  pChargerActionInfo->LedConfigType = (QCOM_CHARGER_PLATFORM_CHARGING_LED_CONFIG_TYPE)gChargerPlatformCfgData.ChargerLibCfgData.charger_led_config;

  if (PrevChargerAction == EFI_QCOM_CHARGER_ACTION_INVALID)
  {
    /* Load profile if SocBasedboot is true or profile load is enabled */
    if((gChargerPlatformCfgData.ChargerLibCfgData.soc_based_boot == TRUE ) || (gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.LoadBatteryProfile == TRUE ))
    {
      Status = ChargerPlatform_ProfileLoadingInit(pActionType, pChargerActionInfo, ErrorType);
      PrevChargerAction = *pActionType;

      /* Return since action is decided for this Invalid charging action case */
      return Status;
    }
    else
    {
      CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Battery Profile Loading Not Required \r\n", __FUNCTION__));
    }
  }

  Status |= ChargerLib_GetBatteryStatus((chargerlib_batt_status_info*)&(pChargerActionInfo->BattStsInfo));
  if (EFI_SUCCESS != Status)
  {
    CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Error Getting Battery Status = %r.\r\n", __FUNCTION__, Status));
    *pActionType = EFI_QCOM_CHARGER_ACTION_STOP_CHARGING;
    PrevChargerAction = *pActionType;

    /*If there is an error, return since action is decided */
    return Status;
  }

  if(CHARGERLIB_CHARGING_ERROR_NONE != ErrorType)
  {
    Status = ChargerLib_GetErrorAction(ErrorType, (((CHARGERLIB_ERROR_ACTION_TYPE*)pActionType)));
    PrevChargerAction = *pActionType;

    /*If there is a battery error, return */
    return Status;
  }

  Status = ChargerLib_GetChargingPath(&pChargerActionInfo->ChargerAttached);
  if (EFI_SUCCESS != Status)
  {
    CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Error Getting Power Path = %r.\r\n", __FUNCTION__, Status));
    *pActionType = EFI_QCOM_CHARGER_ACTION_STOP_CHARGING;
    PrevChargerAction = *pActionType;
    return Status;
  }

  if ((QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_USB  == pChargerActionInfo->ChargerAttached) &&
    ((EFI_QCOM_CHARGER_ACTION_INVALID == PrevChargerAction) ||
    (EFI_QCOM_CHARGER_PLATFORM_ACTION_PROFILE_LOAD == PrevChargerAction) ||
    (EFI_QCOM_CHARGER_ACTION_NO_CHARGE_WAIT == PrevChargerAction)))
  {
    *pActionType = EFI_QCOM_CHARGER_ACTION_START_CHARGING;
  }
  else
  {
    Status = ChargerLib_GetChargingStatus(&ChargingEnabled);
    if(EFI_SUCCESS != Status)
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a Error Getting Charging Status = %r \r\n", __FUNCTION__, Status));
      return Status;
    }

    if(FALSE == ChargingEnabled)
    {
      /*Charger register dump in case need to determine why charging is disabled*/
      Status |= ChargerLib_DumpChargerPeripheral();
      if (gChargerPlatformCfgData.WiPowerCfgData.WipowerEnabled && 
         (QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_DCIN == pChargerActionInfo->ChargerAttached))
      {
        *pActionType = EFI_QCOM_CHARGER_ACTION_NO_CHARGE_WAIT;
        }
      else
      {
        *pActionType = EFI_QCOM_CHARGER_ACTION_START_CHARGING;
      }
    }
    else
    {
        /* Charging already started, go to continue. */
      *pActionType = EFI_QCOM_CHARGER_ACTION_CONTINUE;
        /* Assign Led config to toggle led */
        /* pChargerActionInfo->LedConfigType = (QCOM_CHARGER_PLATFORM_CHARGING_LED_CONFIG_TYPE)gChargerPlatformCfgData.ChargerLibCfgData.charger_led_config;*/
      }

    if (QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_USB == pChargerActionInfo->ChargerAttached)
    {
      /* Check if charger was swapped/re-inserted */
      Status = ChargerLib_WasChargerReinserted(&bChargerReinserted);
      if(EFI_SUCCESS != Status )
      {
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a Error = %d Error Checking Charger Re-insertion. \r\n", __FUNCTION__, Status));
        return Status;
      }

      if(bChargerReinserted)
      {
        //Status = ChargerLib_CheckAPSDResults();
        Status = ChargerLib_ReRunAicl();
      }
    }
  }

  Status |= ChargerPlatform_CheckIfOkToBoot(pActionType, *pChargerActionInfo, pChargerActionInfo->BattStsInfo);

  CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Action Returned = %d \r\n", __FUNCTION__,*pActionType));

  PrevChargerAction = *pActionType;

  return Status;
}

EFI_STATUS ChargerPlatform_LoadProfile(CONST QCOM_CHARGER_PLATFORM_ACTION_INFO *pChargerActionInfo)
{
  QCOM_CHARGER_PLATFORM_BATT_PROFILE_DATA BattProfileInfo = {0};
  EFI_STATUS Status = EFI_SUCCESS;

  if(!pChargerActionInfo)
    return EFI_INVALID_PARAMETER;

  if (gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.LoadBatteryProfile == TRUE)
  {
    CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Battery Profile Loading is enabled \r\n", __FUNCTION__ ));
    switch(pChargerActionInfo->ProfState.eProfileState)
    {
      case QCOM_CHARGER_PLATFORM_PROFILE_LOAD:
        Status |= ChargerPlatformFile_ReadBatteryProfile(&BattProfileInfo);
        if(Status == EFI_SUCCESS)
        {
          CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Battery Profile File Read Success DataSize = %d \r\n", __FUNCTION__, BattProfileInfo.DataSize));
          Status |= ChargerLib_LoadProfile((chargerlib_batt_profile_data *)&BattProfileInfo);
        }
        else
        {
          CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Battery Profile File read Failed = %r \r\n", __FUNCTION__, Status));
        }
        /* Free allocated file buffer */
        if(NULL != BattProfileInfo.pFileBuffer)
          gBS->FreePool(BattProfileInfo.pFileBuffer);
      break;
      case QCOM_CHARGER_PLATFORM_PROFILE_RESTART:
        /* Restart FG Condition */
        Status |= ChargerPlatformFile_ReadBatteryProfile(&BattProfileInfo);
        if(Status == EFI_SUCCESS)
        {
          CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Battery Profile File Read Success DataSize = %d \r\n", __FUNCTION__, BattProfileInfo.DataSize));
          Status |= ChargerLib_LoadProfile((chargerlib_batt_profile_data *)&BattProfileInfo);
        }
        else
        {
          CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Battery Profile Restart = %r \r\n", __FUNCTION__, Status));
        }
        /* Free allocated file buffer */
        if(NULL != BattProfileInfo.pFileBuffer)
          gBS->FreePool(BattProfileInfo.pFileBuffer);
      break;

      default:
      break;
    }
  }

  return Status;
}


EFI_STATUS ChargerPlatform_TakeAction(EFI_QCOM_CHARGER_ACTION_TYPE ChargingAction, CONST QCOM_CHARGER_PLATFORM_ACTION_INFO *pChargerActionInfo)
{
  QCOM_CHARGER_BATT_STATUS_INFO CurrentBatteryStatus = {0};
  STATIC BOOLEAN bToggleLed = TRUE;

  if(!pChargerActionInfo)
  {
    CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Invalid parameter \r\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  CurrentBatteryStatus = (QCOM_CHARGER_BATT_STATUS_INFO)pChargerActionInfo->BattStsInfo;

  switch (ChargingAction)
  {
  case EFI_QCOM_CHARGER_ACTION_CRITICAL:
         //print out and flush critical error messages 
         //Perform AFP
         CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerDxe::%a Critical Error occurred. Shutting down \r\n", __FUNCTION__));
         ChargerLib_ForceSysShutdown(CHGAPP_RESET_AFP);
         break;

  case   EFI_QCOM_CHARGER_ACTION_SHUTDOWN:
         //print error message and trigger system shutdown
         //These errors will only be checked and handled when battery voltage is not high enough to boot and uefi charging is needed.
         CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerDxe::%a Waiting for %d s \r\n", __FUNCTION__, QCOM_CHARGER_IDLE_WAIT_DURATION/QCOM_CHARGER_MS_TO_S));
         WaitForTimeout (QCOM_CHARGER_IDLE_WAIT_DURATION, TIMEOUT_WAIT_FOR_KEY, NULL);
         ChargerLib_ForceSysShutdown(CHGAPP_RESET_SHUTDOWN);
         break;

  case EFI_QCOM_CHARGER_ACTION_START_CHARGING:
          /* Assign entry voltage on warm boot and soc after profile is loaded or not loaded as in warm/cold boot */
          if(gChargerSharedInfo.uefi_entry_mV == QCOM_CHARGER_INVALID_VALUE_MARKER)
          {
            gChargerSharedInfo.uefi_entry_mV    = CurrentBatteryStatus.BatteryVoltage;
          }
          if (gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.LoadBatteryProfile == TRUE)
          {
            gChargerSharedInfo.uefi_entry_soc = CurrentBatteryStatus.StateOfCharge;
          }
          else
          {
            gChargerSharedInfo.uefi_entry_soc   = QCOM_CHARGER_INVALID_VALUE_MARKER;
          }
          CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Saving Entry VBatt  = %d SOC = %d \r\n", __FUNCTION__,
                          CurrentBatteryStatus.BatteryVoltage, CurrentBatteryStatus.StateOfCharge));
          if (QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_USB == pChargerActionInfo->ChargerAttached)
          {
            ChargerLib_InitializeCharging();
          }
         break;

  case EFI_QCOM_CHARGER_ACTION_STOP_CHARGING:
         if(TRUE == gChargingInitialized)
         {
           ChargerLib_ChargerEnable(FALSE);
           gChargingInitialized = FALSE;
         }
         if(gChargerPlatformCfgData.ChargerLibCfgData.charger_led_config)
         {
           /* Turn Off Charging */
           bToggleLed = FALSE;
           ChargerLib_LedOn(bToggleLed);
         }
         break;

  case EFI_QCOM_CHARGER_ACTION_NO_CHARGE_WAIT:
  case EFI_QCOM_CHARGER_ACTION_TSENSE_HIGH_WAIT:
         if (gChargerPlatformCfgData.WiPowerCfgData.WipowerEnabled 
            && (QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_DCIN == pChargerActionInfo->ChargerAttached))
         {
           //do nothing if wipower is enabled and dcin is attached
           break;
         }  
         else
         {
           //only need to disable charging or shutdown if not wipower enabled.
         ChargerLib_HandleNoChargeAndWait();
         }

         if(gChargerPlatformCfgData.ChargerLibCfgData.enable_charger_wdog)
         {
           /* Pet the watchdog if feature is enabled */
           ChargerLib_PetChgWdog();
         }
         break;
    
  case EFI_QCOM_CHARGER_ACTION_CONTINUE:
         if(gChargerPlatformCfgData.ChargerLibCfgData.enable_charger_wdog)
         {
           /* Pet the watchdog if feature is enabled */
           ChargerLib_PetChgWdog();
         }
         if(gChargerPlatformCfgData.ChargerLibCfgData.charger_led_config)
         {
            /*  DEBUG ONLY */
           /* CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe::%a SWLedToggleConfig = %d \r\n", __FUNCTION__, gChargerPlatformCfgData.ChargerLibCfgData.charger_led_config)); */
           switch(gChargerPlatformCfgData.ChargerLibCfgData.charger_led_config)
           {
             case QCOM_CHARGER_PLATFORM_CHARGING_LED_ON:
               bToggleLed = TRUE; /* Make sure to turn on flag as control can come back from wait state */
               ChargerLib_LedOn(bToggleLed);
               break;
             case QCOM_CHARGER_PLATFORM_CHARGING_LED_TOGGLE:
               ChargerLib_LedOn(bToggleLed);
               bToggleLed = (bToggleLed == TRUE)? FALSE: TRUE;
              break;
             case QCOM_CHARGER_PLATFORM_CHARGING_LED_OFF:
              /* will not reache here */
             default:
              break;
            }
         }
         else
         {
           /*  DEBUG ONLY */
           /* CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Charging Led Indication is off = %d \r\n", __FUNCTION__, gChargerPlatformCfgData.ChargerLibCfgData.charger_led_config)); */
         }
         //Perform Necessary Configuration to continuously charge the battery
         ChargerLib_ContinueCharging();
         /* Debug FG SRAM */
         ChargerLib_DumpSram(FALSE);
         break;

  case EFI_QCOM_CHARGER_ACTION_GOOD_TO_BOOT:
        /* Assign Exit voltage and soc */
        gChargerSharedInfo.uefi_exit_mV    = CurrentBatteryStatus.BatteryVoltage;
        if (gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.LoadBatteryProfile == TRUE)
        {
          gChargerSharedInfo.uefi_exit_soc = CurrentBatteryStatus.StateOfCharge;
        }
        else
        {
          gChargerSharedInfo.uefi_exit_soc   = QCOM_CHARGER_INVALID_VALUE_MARKER;
        }
        CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Saving Exit VBat = %d Soc = %d \r\n", __FUNCTION__,
                         CurrentBatteryStatus.BatteryVoltage, CurrentBatteryStatus.StateOfCharge));
        /* Save Smem Info ignoring return status as XBL loader changes are not ready yet*/
        ChargerPlatform_SaveChargeInfoToSmem(&gChargerSharedInfo);
        if (gChargerPlatformCfgData.WiPowerCfgData.WipowerEnabled)
        {
          if (gChargerPlatformCfgData.WiPowerCfgData.DCINDisableOnExit)
        {
          //suspend DCIn when wipower flags are set.
          (void)ChargerLib_DcinSuspend(TRUE);
        }
          if (QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_DCIN == pChargerActionInfo->ChargerAttached)
          {
            (void)ChargerLib_ToggleWipowerShutDownLatch();
          }
        }

        /* Turn Off Charging */
        bToggleLed = FALSE;
        ChargerLib_LedOn(bToggleLed);
        break;

  case EFI_QCOM_CHARGER_PLATFORM_ACTION_PROFILE_LOAD:
        /* Enable BMD */
        ChargerLib_EnableBMD();
        /* Assign only entry voltage for smem before profile load, soc will be update after profile is loaded in next action which is start charging action*/
        gChargerSharedInfo.uefi_entry_mV    = CurrentBatteryStatus.BatteryVoltage;
        ChargerPlatform_LoadProfile(pChargerActionInfo);
        break;

  case EFI_QCOM_CHARGER_ACTION_SHUTDOWN_USB_DC_PON_DISABLED:
      //print error message and trigger system shutdown
      //These errors will only be checked and handled when battery voltage is not high enough to boot and uefi charging is needed.
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerDxe::%a Waiting for %d s \r\n", __FUNCTION__, QCOM_CHARGER_IDLE_WAIT_DURATION/QCOM_CHARGER_MS_TO_S));
      WaitForTimeout (QCOM_CHARGER_IDLE_WAIT_DURATION, TIMEOUT_WAIT_FOR_KEY, NULL);
      ChargerLib_ForceSysShutdown(CHGAPP_RESET_SHUTDOWN_USB_DC_PON_DISABLED);
      break;

        break;
    default:
        CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Action Passed = %d \r\n", __FUNCTION__,ChargingAction));
      break;
  }
  
  return EFI_SUCCESS;
}


/**
ChargerPlatform_GetUefiVariableValues()

@brief
Reads UEFI Variables changed via BDS Menu
*/
EFI_STATUS ChargerPlatform_GetUefiVariableValues( VOID)
{
  UINT8          Flag          = 0;
  UINTN          VarSize       = sizeof(Flag);
  STATIC BOOLEAN VariablesRead = FALSE;
  EFI_STATUS     Status = EFI_SUCCESS;

  if (FALSE == VariablesRead)
  {
    VariablesRead = TRUE;
    /*We are supposed to read only once. if the variable doesn't exist, make assumption on the value to treat
      variable not being there as expected condition and cache the expectation instead. We are memsetting the
      structure variable and so by default it is disabled
    */
    Status = gRT->GetVariable(L"PrintChargerAppDbgMsg", &gQcomTokenSpaceGuid, NULL, &VarSize, &Flag);
    if (EFI_SUCCESS == Status )
    {
      gChargerPlatformCfgData.ChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg = (Flag) ? TRUE : FALSE;
    }
    }

  return EFI_SUCCESS;
}

/**
ChargerPlatform_GetConfigData()

@brief
Returns config data
*/
EFI_STATUS ChargerPlatform_GetConfigData
(
  QCOM_CHARGER_PLATFORM_CFGDATA_TYPE *pChargerPlatformCfgData
)
{

  if(!pChargerPlatformCfgData)
    return EFI_INVALID_PARAMETER;

  CopyMemS(pChargerPlatformCfgData, sizeof(QCOM_CHARGER_PLATFORM_CFGDATA_TYPE), &gChargerPlatformCfgData, sizeof(QCOM_CHARGER_PLATFORM_CFGDATA_TYPE));

  return EFI_SUCCESS;
}


/**
ChargerPlatform_ReadCfgParams()

@brief
Battery Parameter Default Configurations file read Call Back
*/
VOID
ChargerPlatform_ReadCfgParams
(
  UINT8* Section,
  UINT8* Key,
  UINT8* Value
)
{

  //CHARGER_DEBUG(( EFI_D_INFO, "Section = \"%s\", Key = \"%s\", Value = \"%s\"\n", Section, Key, Value));

  if (AsciiStriCmp ((CHAR8*)Section, "PlatformCommon") == 0) //Common
  {
    if (AsciiStriCmp ((CHAR8*)Key, "CfgVersion") == 0)
    {
    gChargerPlatformCfgData.CfgVersion = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "SWFlashMinBattVoltageMv") == 0)
    {
      gChargerPlatformCfgData.SWFlashMinBattVoltageMv = ChargerPlatformFile_AsciiToInt((char *)Value);
      return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "BootToHLOSThresholdInMv") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.boot_to_hlos_threshold_in_mv = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "OsStandardBootSocThreshold") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.boot_to_hlos_threshold_in_soc = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "SocBasedBoot") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.soc_based_boot = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "EnableChargerWdog") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.enable_charger_wdog = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }
 
    if (AsciiStriCmp ((CHAR8*)Key, "DispSignOfLifeMaxThresholdMv") == 0)
    {
    gChargerPlatformCfgData.DispSignOfLifeMaxThresholdMv = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "ChargerLedConfig") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.charger_led_config = ChargerPlatformFile_AsciiToInt((CHAR8*)Value);
    return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "WiPowerSupported") == 0)
    {
    gChargerPlatformCfgData.WiPowerCfgData.WipowerEnabled = (AsciiStriCmp((CHAR8*)Value, "TRUE") == 0);
    return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "DCInBootToHLOSThresholdInMv") == 0)
    {
    gChargerPlatformCfgData.WiPowerCfgData.WipowerBootThreshold = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "SuspendDCIn") == 0)
    {
    gChargerPlatformCfgData.WiPowerCfgData.DCINDisableOnExit = (AsciiStriCmp((CHAR8*)Value, "TRUE") == 0);
    return;
    }

    if (AsciiStriCmp((CHAR8 *)Key, "ChargingTermCurrent") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.ChargingTermCurrent = ChargerPlatformFile_AsciiToInt((char *)Value);
    gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.ChargingTermCurrent = gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.ChargingTermCurrent;
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "BattMissingCfg") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.BattMissingCfg= ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "FgCondRestart") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgCondRestart = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "VBattEstDiffThreshold") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.VBattEstDiffThreshold = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "LoadBatteryProfile") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.LoadBatteryProfile = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "SWThermalMitigationEnable") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.sw_thermal_mitigation_enable = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp((CHAR8 *)Key, "BattVoltLimHighDelta") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.batt_volt_lim_high_delta = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "PrintChargerAppDbgMsg") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "PrintChargerAppDbgMsgToFile") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg_to_file= ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "FileLoggingDbgLevelMask") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.dbg_cfg_Data.file_log_level_mask = ChargerPlatformFile_AsciiStrToHex((CHAR8 *)Value, AsciiStrLen((CHAR8 *)Value));
    return;
    }
    
    if (AsciiStriCmp ((CHAR8*)Key, "BatteryIdTolerance") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattIdCfg.BatteryIdTolerance = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "DebugBoardBatteryIdMin") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattIdCfg.DebugBoardBatteryIdMin = ChargerPlatformFile_AsciiToInt((char *)Value);;
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "DebugBoardBatteryIdMax") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattIdCfg.DebugBoardBatteryIdMax = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "RegularBatteryIdMin") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattIdCfg.RegularBatteryIdMin = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }  

    if (AsciiStriCmp ((CHAR8*)Key, "RegularBatteryIdMax") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattIdCfg.RegularBatteryIdMax = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "SmartBatteryIdMin") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattIdCfg.SmartBatteryIdMin = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "SmartBatteryIdMax") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattIdCfg.SmartBatteryIdMax = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "ConservChgFvDelta") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.conserv_chg_fv_delta = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "NoChargeAndWait") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.no_charge_and_wait = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "EmergencyShutdownVbatt") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.emergency_shutdown_vbatt = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "UnknownBatteryBehavior") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.unknown_battery_behavior= ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "DebugBoardBehavior") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.debug_board_behavior= ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "ProgramAuxThermCoeffs") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.AuxThermCoeffs.ProgramAuxThermCoeffs = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "AuxThermC1") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.AuxThermCoeffs.AuxThermC1= ChargerPlatformFile_AsciiStrToHex((CHAR8 *)Value, AsciiStrLen((CHAR8 *)Value));
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "AuxThermC2") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.AuxThermCoeffs.AuxThermC2= ChargerPlatformFile_AsciiStrToHex((CHAR8 *)Value, AsciiStrLen((CHAR8 *)Value));
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "AuxThermC3") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.AuxThermCoeffs.AuxThermC3= ChargerPlatformFile_AsciiStrToHex((CHAR8 *)Value, AsciiStrLen((CHAR8 *)Value));
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "ProgramBattThermCoeffs") == 0)
    {
    gChargerPlatformCfgData.ProgramBattThermCoeffs = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "BattThermC1") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattThermCoffs.BattThermC1= ChargerPlatformFile_AsciiStrToHex((CHAR8 *)Value, AsciiStrLen((CHAR8 *)Value));
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "BattThermC2") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattThermCoffs.BattThermC2= ChargerPlatformFile_AsciiStrToHex((CHAR8 *)Value, AsciiStrLen((CHAR8 *)Value));
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "BattThermC3") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattThermCoffs.BattThermC3= ChargerPlatformFile_AsciiStrToHex((CHAR8 *)Value, AsciiStrLen((CHAR8 *)Value));
    return;
    }
    
    if (AsciiStriCmp ((CHAR8*)Key, "ProgramSkinAndChargerHotThreshold") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.SkinAndChargerHotThreshold.ProgramSkinAndChargerHotThreshold = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "DeviceSkinHotInC") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.SkinAndChargerHotThreshold.DeviceSkinHotInC = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "DeviceSkinTooHotInC") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.SkinAndChargerHotThreshold.DeviceSkinTooHotInC = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "ChargerTooHotInC") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.SkinAndChargerHotThreshold.ChargerTooHotInC= ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "ChargerHotInC") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.SkinAndChargerHotThreshold.ChargerHotInC = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "DumpSram") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgSramDbgCfg.DumpSram = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "DumpSramStartAddr") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgSramDbgCfg.DumpSramStartAddr = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "DumpSramEndAddr") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgSramDbgCfg.DumpSramEndAddr = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "DumpSramDuration") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgSramDbgCfg.DumpSramDuration = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "BattThermBiasWait") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattThermCfg.BiasWait = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }
    if (AsciiStriCmp((CHAR8*)Key, "BattThermGndSel") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattThermCfg.GroundSel = (AsciiStriCmp((CHAR8*)Value, "TRUE") == 0);
    return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "AuxThermBiasWait") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgAuxThermCfg.BiasWait = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }
    if (AsciiStriCmp((CHAR8*)Key, "AuxThermGndSel") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgAuxThermCfg.GroundSel = (AsciiStriCmp((CHAR8*)Value, "TRUE") == 0);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "TsensExtremeTemp") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.thermal_configs.tsens_extreme_Temp = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "TsensHighTemp") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.thermal_configs.tsens_high_temp = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "TsensLowTemp") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.thermal_configs.tsens_low_temp = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "TsensTimeoutMins") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.thermal_configs.tsens_timeout_mins = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "EnableChargerFgDump") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.dbg_cfg_Data.enable_charger_fg_Dump = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "RconnComp") == 0)
    {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.RconnComp = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
    }
    if (AsciiStriCmp ((CHAR8*)Key, "EnShipMode") == 0)
    {
      gChargerPlatformCfgData.ChargerLibCfgData.shipmode_cfg.enable_shipmode = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
      return;
    }

    if (AsciiStriCmp ((CHAR8*)Key, "ShipModeVarStr") == 0)
    {
      AsciiStrCpy (gChargerPlatformCfgData.ChargerLibCfgData.shipmode_cfg.shipMode_str, (CHAR8*)Value);
      return;
    }
    if (AsciiStriCmp((CHAR8*)Key, "SkinTempSrc") == 0)
    {
      gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.EnableSkinTempINov = (AsciiStriCmp((CHAR8*)Value, "TRUE") == 0);
      return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "DieTempSrc") == 0)
    {
      gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.EnableDieTempINov = (AsciiStriCmp((CHAR8*)Value, "TRUE") == 0);
      return;
    }

    if (AsciiStriCmp((CHAR8*)Key, "DieTempCompSrc") == 0)
    {
      gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.EnableDieTempCompINov = (AsciiStriCmp((CHAR8*)Value, "TRUE") == 0);
      return;
    }
    
    if (AsciiStriCmp((CHAR8*)Key, "PlatHWCfgOverrdie") == 0)
    {
      gChargerPlatformCfgData.ChargerLibCfgData.plat_hw_cfg_override = ChargerPlatformFile_AsciiToInt((char *)Value);
      return;
    }    
	
    if (AsciiStriCmp((CHAR8*)Key, "CCFloatCharging") == 0)
    {
      gChargerPlatformCfgData.ChargerLibCfgData.cc_float_charging = ChargerPlatformFile_AsciiToInt((char *)Value);
      return;
    } 
  }
  else if ( ((EFI_PLATFORMINFO_TYPE_MTP == PlatformType) && (AsciiStriCmp ((CHAR8*)Section, "PlatformMTP") == 0)) ||
       (((EFI_PLATFORMINFO_TYPE_QRD == PlatformType) || (EFI_PLATFORMINFO_TYPE_HDK == PlatformType)) && (AsciiStriCmp ((CHAR8*)Section, "PlatformQRD") == 0))  )
  {
  /*Parameters w.r.t. MTP and QRD Platforms*/
  if (AsciiStriCmp ((CHAR8*)Key, "ChgFccMax") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.ChgFccMax = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "ChgFvMax") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.ChgFvMax = ChargerPlatformFile_AsciiToInt((char *)Value);;
    return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaFvCompCfg") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.JeitaFvCompCfg= ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaCcCompCfg") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.JeitaCcCompCfg= ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
  }

  if (AsciiStriCmp((CHAR8*)Key, "HwAfpHotLimit") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.HwAfpHotThreshold = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
  }

  if (AsciiStriCmp((CHAR8*)Key, "HwAfpColdLimit") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.schg_cfg_data.HwAfpColdThreshold = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaCriticalTempLowLimit") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempLowLimit = ChargerPlatformFile_AsciiToInt((char *)Value);
        return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaHardColdLimit") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaHardColdLimit = ChargerPlatformFile_AsciiToInt((char *)Value);
        return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaSoftColdLimit") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaSoftColdLimit = ChargerPlatformFile_AsciiToInt((char *)Value);
        return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaSoftHotLimit") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaSoftHotLimit = ChargerPlatformFile_AsciiToInt((char *)Value);
        return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaHardHotLimit") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaHardHotLimit = ChargerPlatformFile_AsciiToInt((char *)Value);
        return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaCriticalTempHighLimit") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempHighLimit = ChargerPlatformFile_AsciiToInt((char *)Value);
        return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaSoftColdFvCompEnable") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaSoftColdFvCompEnable = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
        return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaSoftHotFvCompEnable") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaSoftHotFvCompEnable = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaSoftColdCcCompEnable") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaSoftColdCcCompEnable = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "JeitaSoftHotCcCompEnable") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaSoftHotCcCompEnable = ( AsciiStriCmp ((CHAR8*)Value, "TRUE") == 0 );
    return;
  }

  if (AsciiStriCmp ((CHAR8*)Key, "VBatEmpty") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.VBatEmpty= ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
  }

  if (AsciiStriCmp((CHAR8*)Key, "BattMissingDelay") == 0)
  {
    gChargerPlatformCfgData.ChargerLibCfgData.fg_cfg_data.FgBattIdCfg.BattMissingDelay = ChargerPlatformFile_AsciiToInt((char *)Value);
    return;
  }
  }
  return;
}


EFI_STATUS ChargerPlatform_SaveChargeInfoToSmem(pm_chg_info_type *ChargerActionInfo)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32     buf_size = sizeof(pm_chg_info_type);
  UINT32     buf_size_ret = 0;
  pm_chg_info_type *pSmemChargerInfo = NULL, *pSmemChargerInfoRet = NULL;

  pSmemChargerInfo = (pm_chg_info_type *) smem_get_addr(SMEM_RESERVED_BATT_INFO, &buf_size_ret);
  if(!pSmemChargerInfo)
  {
    CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Retrieve Error.. XBL Core allocating SMEM now \r\n",__FUNCTION__));
      /* Assume Smem is already initilized in XBL Loader - No need to alloc as XBL loader will alloc for Core keeping this as debug code */
    pSmemChargerInfo = (pm_chg_info_type *)smem_alloc(SMEM_RESERVED_BATT_INFO, buf_size);
    if(!pSmemChargerInfo)
    {
      CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a smem_alloc Error \r\n",__FUNCTION__));
      return EFI_DEVICE_ERROR;
    }
    else
    {
      CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a SMEM Allocated \r\n",__FUNCTION__));
      /* Initialized to -1 since SBL did not allocate smem yet */
      pSmemChargerInfo->version       = 1;
      pSmemChargerInfo->sbl_entry_mV  = QCOM_CHARGER_INVALID_VALUE_MARKER;
      pSmemChargerInfo->sbl_entry_soc = QCOM_CHARGER_INVALID_VALUE_MARKER;
    }
  }

  /* Assign required values to smem memory */
  pSmemChargerInfo->uefi_entry_mV  = ChargerActionInfo->uefi_entry_mV;
  pSmemChargerInfo->uefi_exit_mV   = ChargerActionInfo->uefi_exit_mV;
  pSmemChargerInfo->uefi_entry_soc = ChargerActionInfo->uefi_entry_soc;
  pSmemChargerInfo->uefi_exit_soc  = ChargerActionInfo->uefi_exit_soc;

  buf_size_ret = 0;

  pSmemChargerInfoRet = (pm_chg_info_type *) smem_get_addr(SMEM_RESERVED_BATT_INFO, &buf_size_ret);
  if(!pSmemChargerInfoRet){
    CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Retrieve Error 2 \r\n",__FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: buf_size_ret = %d uefi_entry_mV = %d uefi_exit_mV = %d uefi_entry_soc = %d \r\n", buf_size_ret, pSmemChargerInfoRet->uefi_entry_mV, pSmemChargerInfoRet->uefi_exit_mV, pSmemChargerInfoRet->uefi_entry_soc));
  CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: sbl_entry_mV = %d sbl_entry_soc = %d ueft_exit_soc = %d \r\n", pSmemChargerInfoRet->sbl_entry_mV, pSmemChargerInfoRet->sbl_entry_soc, pSmemChargerInfoRet->uefi_exit_soc));

  pSmemChargerInfoRet = NULL;
  pSmemChargerInfo    = NULL;

  return Status;

}

EFI_STATUS ChargerPlatform_ProfileLoadingInit(EFI_QCOM_CHARGER_ACTION_TYPE *pActionType, QCOM_CHARGER_PLATFORM_ACTION_INFO *pChargerActionInfo, CHARGERLIB_CHARGING_ERROR_TYPES errType)
{
  EFI_STATUS Status = EFI_SUCCESS;
  QCOM_CHARGER_PLATFORM_BATT_PROFILE_STATUS BattProfileStatus    = {0};
  QCOM_CHARGER_BATT_STATUS_INFO             CurrentBatteryStatus = {0};
  //CHARGERLIB_CHARGING_ERROR_TYPES           ErrorType  = CHARGERLIB_CHARGING_ERROR_NONE;

  Status = ChargerLib_GetBatteryProfileStatus((chargerlib_batt_profile_status *)&BattProfileStatus);
  if( EFI_SUCCESS == Status)
  {
    /* Attempt to load profile or restart FG will be called if below errors turns not TRUE */
    if ((errType != CHARGERLIB_CHARGING_ERROR_BATTERY_NOT_DETECTED) &&
       (errType != CHARGERLIB_CHARGING_ERROR_DEBUG_BOARD) &&
       (errType != CHARGERLIB_CHARGING_ERROR_BATT_TEMP_OUTSIDE_OPERATIONAL_RANGE) &&
       (errType != CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL) &&
       (errType != CHARGERLIB_DEVICE_ERROR) &&
       (errType != CHARGERLIB_CHARGING_ERROR_FG_ERROR) &&
       (errType != CHARGERLIB_CHARGING_ERROR_ADC_ERROR))
    {
      /* Assign Profile State info for action */
      pChargerActionInfo->ProfState.eProfileState = (QCOM_CHARGER_PLATFORM_BATT_PROFILE_STATE) BattProfileStatus.eProfileState;
      Status |= ChargerLib_GetBatteryStatus((chargerlib_batt_status_info*)&CurrentBatteryStatus);
      if(EFI_SUCCESS != Status )
      {
        CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Get Battery Status Error = %r.\r\n", __FUNCTION__, Status));
        return Status;
      }
      pChargerActionInfo->BattStsInfo = CurrentBatteryStatus;
      /* Return same action for both profile state as FG would internally handle profile load or restart */
      *pActionType = EFI_QCOM_CHARGER_PLATFORM_ACTION_PROFILE_LOAD;
      switch(BattProfileStatus.eProfileState)
      {
        case PM_FG_BATT_PROFILE_LOAD:
          if(gChargerPlatformCfgData.DispSignOfLifeMaxThresholdMv == 0)
          {
            gChargerPlatformCfgData.DispSignOfLifeMaxThresholdMv  = QCOM_CHARGER_LOW_THERSHOLD_VBAT;
          }
          pChargerActionInfo->DispSignOfLifeMaxThresholdMv = gChargerPlatformCfgData.DispSignOfLifeMaxThresholdMv + QCOM_CHARGER_VBAT_ERROR_MARGIN;
          if (errType == CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED)
            pChargerActionInfo->bChargerSrcPresent = FALSE;
          else
            pChargerActionInfo->bChargerSrcPresent = TRUE;
          /*Disable Charging to get accurate VBAT reading */
          Status |= ChargerLib_ChargerEnable(FALSE);

          /* Not enabling charging since charger app does not know if charging needs to enabled or not at this stage */
          /* Status |= ChargerLib_ChargerEnable(TRUE); */
          break;
          case PM_FG_BATT_PROFILE_RESTART:
            /* Restart FG if needed */
            /* PrevChargerAction = *pActionType = QCOM_CHARGER_PLATFORM_ACTION_PROFILE_LOAD; */
          break;
          default:
          break;
      }
    }
    else
    {
      CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a eProfileState = %d.Charger Lib Error = %d \r\n", __FUNCTION__, BattProfileStatus.eProfileState, errType));
    }
  }
  else
  {
    CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a _GetBatteryProfileStatus/ChargerLib_GetErrors Error = %r.\r\n", __FUNCTION__, Status));
    Status = EFI_SUCCESS; /* Not an error to stop boot */
  }
  
  return Status;
}

EFI_STATUS ChargerPlatform_CheckIfOkToBoot
(
  EFI_QCOM_CHARGER_ACTION_TYPE *pActionType,
  QCOM_CHARGER_PLATFORM_ACTION_INFO  ChargerActionInfo, 
  QCOM_CHARGER_BATT_STATUS_INFO      CurrentBatteryStatus
)
{
  switch(gChargerPlatformCfgData.ChargerLibCfgData.soc_based_boot)
  {
    case FALSE:
      if (gChargerPlatformCfgData.WiPowerCfgData.WipowerEnabled && 
         (QCOM_CHARGER_PLATFORM_CHARGER_ATTACHED_DCIN == ChargerActionInfo.ChargerAttached))
      {
        if (CurrentBatteryStatus.BatteryVoltage >= gChargerPlatformCfgData.WiPowerCfgData.WipowerBootThreshold)
        {
          *pActionType = EFI_QCOM_CHARGER_ACTION_GOOD_TO_BOOT;
          CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Good To Boot To HLOS BatteryVoltage= %d wipower threshold = %d \r\n",
            __FUNCTION__, CurrentBatteryStatus.BatteryVoltage, gChargerPlatformCfgData.WiPowerCfgData.WipowerBootThreshold));
        }
      }
      else if (CurrentBatteryStatus.BatteryVoltage >= gThresholdVbatt)
      {
        *pActionType = EFI_QCOM_CHARGER_ACTION_GOOD_TO_BOOT;
         CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Good To Boot To HLOS BatteryVoltage= %d gThresholdVbatt = %d \r\n",
                        __FUNCTION__, CurrentBatteryStatus.BatteryVoltage, gThresholdVbatt));
      }
      else
      { /* TBD comment out */
        CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a NOT Enough BatteryVoltage To Boot = %d gThresholdVbatt = %d \r\n",
                        __FUNCTION__, CurrentBatteryStatus.BatteryVoltage, gThresholdVbatt));
      }
      break;

    case TRUE:
      if(CurrentBatteryStatus.StateOfCharge >= gThresholdSoc)
      {
        *pActionType = EFI_QCOM_CHARGER_ACTION_GOOD_TO_BOOT;
        CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Good To Boot To HLOS StateOfCharge= %d gThresholdSoc = %d \r\n",
                       __FUNCTION__, CurrentBatteryStatus.StateOfCharge, gThresholdSoc));
      }
      else
      { /* TBD comment out */
        CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Not Enough StateOfCharge= %d To Boot gThresholdSoc = %d \r\n",
                       __FUNCTION__, CurrentBatteryStatus.StateOfCharge, gThresholdSoc));
      }
    break;
    default:
    break;
  }
  
  return EFI_SUCCESS;
}

EFI_STATUS ChargerPlatformFile_GetChargerConfig(EFI_QCOM_CHARGER_CONFIG_KEY ChargerCfgKey, UINT32 *KeyValue)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(FALSE == gChargerCfgInitialized)
  {
    return EFI_DEVICE_ERROR;
  }

  if(NULL == KeyValue)
  {
    return EFI_INVALID_PARAMETER;
  }

  switch(ChargerCfgKey)
  {
    case EFI_QCOM_CHARGER_CONFIG_KEY_SW_FLASH_VOLTAGE:
      *KeyValue = gChargerPlatformCfgData.SWFlashMinBattVoltageMv;
    break;
    default:
      Status = EFI_INVALID_PARAMETER;
    break;
  }

  return Status;
}


EFI_STATUS ChargerPlatform_HandleShipMode(void)
{
  EFI_STATUS Status     = EFI_SUCCESS;
  BOOLEAN    EnShipMode = FALSE;

  /* Read Ship mode var value */
  Status  = ChargerLib_GetShipMode(&EnShipMode);
  if ((EFI_SUCCESS == Status )&& (EnShipMode == TRUE))
  {
    /* Enable ship mode 
    Flash LED or HW flashes LED on ship mode entering
    flush logs on debug environment */
    CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a Performing AFP \r\n",__FUNCTION__));

    Status = ChargerLib_ForceSysShutdown(CHGAPP_RESET_SHIPMODE);
    /* Control does not return */
  }
  return EFI_SUCCESS;
}



