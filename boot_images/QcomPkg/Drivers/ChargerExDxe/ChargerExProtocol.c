/*! @file ChargerExProtocol.c

*  CHARGER EXTENDED MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*  the Charger Extended module.
*
*  Copyright (c) 2016 -2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Inc Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     --------------------------------------------------------------------------
03/03/16   va      Adding offmode, flash APIs to contained for ABL, making open source and local header copies
12/16/16   sm      Added changes to return EFI_UNSUPPORTED Status for CDP and RUMI Platforms
11/09/16   cs      modified charger present API also to detect DCIn. 
09/09/16   sm      Added Status success check after locating protocol. 
05/31/16   sm      New File
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
/**
  PMIC Lib interfaces
 */
#include <Include/Protocol/EFIQcomCharger.h>
#include <Include/Protocol/EFIPmicUsb.h>
#include <Include/Protocol/EFIChargerEx.h>
#include <Include/Protocol/EFIPmicPwrOn.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIPlatformInfo.h>
#include <Library/QcomLib.h>

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
extern EFI_GUID gQcomPmicUsbProtocolGuid;
EFI_CHARGER_EX_PROTOCOL ChargerExProtocolImplementation;

STATIC EFI_QCOM_CHARGER_PROTOCOL    *pQcomChargerProtocol = NULL;
STATIC EFI_QCOM_PMIC_PWRON_PROTOCOL *pPmicPwRonProtocol   = NULL;

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

#define PM_DEVICE_0 0 /* PMIC index */

/*===========================================================================
EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/
/**
EFI_ChargerExInit()

@brief
Initializes charger Extended module
*/
EFI_STATUS ChargerExInitialize( IN EFI_HANDLE         ImageHandle,
                                IN EFI_SYSTEM_TABLE   *SystemTable )
{
  EFI_STATUS Status = EFI_SUCCESS;
  
  Status  = gBS->InstallMultipleProtocolInterfaces(
                 &ImageHandle,      
                 &gChargerExProtocolGuid,
                 &ChargerExProtocolImplementation,
                 NULL
                 );

  return Status;
}

/**
EFI_ChargerExGetChargerPresence()

@brief
 Returns charger presence status
 */
EFI_STATUS
EFIAPI
EFI_ChargerExGetChargerPresence
(
  OUT BOOLEAN  *pChargerPresent
)
{
  EFI_STATUS                           Status = EFI_SUCCESS;
  STATIC EFI_QCOM_PMIC_USB_PROTOCOL   *PmicUsbProtocol   = NULL;
  BOOLEAN                              DcInValid = FALSE;
  EFI_PLATFORMINFO_PLATFORM_TYPE       PlatformType;

  if (!pChargerPresent)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status |= GetPlatformType(&PlatformType);
  if(EFI_SUCCESS != Status)
  {
    DEBUG((EFI_D_WARN, "ChargerExProtocol:: %a Error getting platform type  \r\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  if((EFI_PLATFORMINFO_TYPE_CDP == PlatformType) || (EFI_PLATFORMINFO_TYPE_RUMI == PlatformType))
  {
    DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a CDP/RUMI (%d) Platform detected. No Battery information available. \r\n", __FUNCTION__, PlatformType));
    return EFI_UNSUPPORTED;
  }

  if (!PmicUsbProtocol)
  {
    Status = gBS->LocateProtocol( &gQcomPmicUsbProtocolGuid,
                                  NULL,
                                 (VOID **)&PmicUsbProtocol );

    if((EFI_SUCCESS != Status) || (NULL == PmicUsbProtocol))
    {
      return EFI_DEVICE_ERROR;
    }
  }

  Status = PmicUsbProtocol->GetChargerPresence(pChargerPresent);

  if (EFI_SUCCESS != Status)
  {
    return Status;
  }

  if (!pQcomChargerProtocol)
  {
    Status = gBS->LocateProtocol(&gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol);

    if ((EFI_SUCCESS != Status) || (NULL == pQcomChargerProtocol))
    {
      return EFI_DEVICE_ERROR;
    }
  }

  Status = pQcomChargerProtocol->IsDcInValid(&DcInValid);

  if (EFI_SUCCESS == Status)
  {
    (*pChargerPresent) |= DcInValid;
  } 

  return Status;
}


/**
EFI_ChargerExGetBatteryPresence ()

@brief
Get battery presence status SCHG
*/
EFI_STATUS
EFIAPI
EFI_ChargerExGetBatteryPresence
(
  OUT BOOLEAN  *pBatteryPresent
)
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_PLATFORMINFO_PLATFORM_TYPE  PlatformType;
  
  if (!pBatteryPresent)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status |= GetPlatformType(&PlatformType);
  if(EFI_SUCCESS != Status)
  {
    DEBUG((EFI_D_WARN, "ChargerExProtocol:: %a Error getting platform type  \r\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  if((EFI_PLATFORMINFO_TYPE_CDP == PlatformType) || (EFI_PLATFORMINFO_TYPE_RUMI == PlatformType))
  {
    DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a CDP/RUMI (%d) Platform detected. No Battery information available. \r\n", __FUNCTION__, PlatformType));
    return EFI_UNSUPPORTED;
  }

  if(!pQcomChargerProtocol)
  {
    Status = gBS->LocateProtocol( &gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol );

    if((EFI_SUCCESS != Status) || (NULL == pQcomChargerProtocol))
    {
      return EFI_DEVICE_ERROR;
    }
  }
  
  Status = pQcomChargerProtocol->GetBatteryPresence(pBatteryPresent);
  
  return Status;
}

/**
EFI_ChargerExGetBatteryVoltage ()

@brief
Returns battery voltage
*/
EFI_STATUS
EFIAPI
EFI_ChargerExGetBatteryVoltage
(
  OUT UINT32  *pBatteryVoltage
)
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_PLATFORMINFO_PLATFORM_TYPE  PlatformType;

  if (!pBatteryVoltage)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status |= GetPlatformType(&PlatformType);
  if(EFI_SUCCESS != Status)
  {
    DEBUG((EFI_D_WARN, "ChargerExProtocol:: %a Error getting platform type  \r\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  if((EFI_PLATFORMINFO_TYPE_CDP == PlatformType) || (EFI_PLATFORMINFO_TYPE_RUMI == PlatformType))
  {
    DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a CDP/RUMI (%d) Platform detected. No Battery information available. \r\n", __FUNCTION__, PlatformType));
    return EFI_UNSUPPORTED;
  }

  if(!pQcomChargerProtocol)
  {
    Status = gBS->LocateProtocol( &gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol );

    if(EFI_SUCCESS != Status || NULL == pQcomChargerProtocol)
    {
      return EFI_DEVICE_ERROR;
    }
  }

  Status = pQcomChargerProtocol->GetBatteryVoltage(pBatteryVoltage);

  return Status;
}


/**
EFI_ChargerExIsOffModeCharging ()

@brief
Returns if device needs to go for off mode charging
*/
EFI_STATUS
EFIAPI
EFI_ChargerExIsOffModeCharging
(
  OUT BOOLEAN *bOffModeCharging
)
{
  EFI_STATUS Status         = EFI_SUCCESS;
  BOOLEAN    ChargerPresent = FALSE;
  BOOLEAN    IsWarmReset    = FALSE;
  EFI_PM_PON_REASON_TYPE PonReasonType;
  BOOLEAN    IsColdBoot     = FALSE;
  BOOLEAN    BatteryPresent = FALSE;
  EFI_PLATFORMINFO_PLATFORM_TYPE       PlatformType;

  if(NULL == bOffModeCharging)
    return EFI_INVALID_PARAMETER;

  Status |= GetPlatformType(&PlatformType);
  if(EFI_SUCCESS != Status)
  {
    DEBUG((EFI_D_WARN, "ChargerExProtocol:: %a Error getting platform type  \r\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  if((EFI_PLATFORMINFO_TYPE_CDP == PlatformType) || (EFI_PLATFORMINFO_TYPE_RUMI == PlatformType))
  {
    DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a CDP/RUMI (%d) Platform detected. No Battery information available. \r\n", __FUNCTION__, PlatformType));
    return EFI_UNSUPPORTED;
  }

  if(!pPmicPwRonProtocol)
  {
    Status |= gBS->LocateProtocol( &gQcomPmicPwrOnProtocolGuid, NULL, (VOID **)&pPmicPwRonProtocol);
    if(EFI_SUCCESS != Status || NULL == pPmicPwRonProtocol)
    {
      return EFI_DEVICE_ERROR;
    }
  }

  /* Get Battery Presence, Charger Presence, Voltage */
  Status |=  EFI_ChargerExGetBatteryPresence(&BatteryPresent);
  Status |= pPmicPwRonProtocol->GetPonReason(PM_DEVICE_0, &PonReasonType);
  Status |= pPmicPwRonProtocol->WarmResetStatus(PM_DEVICE_0, &IsWarmReset); /* Need PON API abstracted (no index)*/
  Status |= EFI_ChargerExGetChargerPresence(&ChargerPresent);

  IsColdBoot = !IsWarmReset;

  if(Status == EFI_SUCCESS)
  {
    if((IsColdBoot) 
      && (BatteryPresent) 
      && (ChargerPresent)                   /* If not warm reset and charger,battery present */
      && (!PonReasonType.HARD_RESET) 
      && (!PonReasonType.KPDPWR) /* If not Hard reset and not a Powery key boot */
      && (PonReasonType.PON1 || PonReasonType.USB_CHG))       /* if PON1 and USB triggered PON */
    {
      DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a Off mode charging true \r\n", __FUNCTION__));
      *bOffModeCharging = TRUE;
    }
    else
    {/*
      DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a HARD_RESET = %d .KPDPWR = %d PON1 = %d USB_CHG = %d \r\n", __FUNCTION__, PonReasonType.HARD_RESET, PonReasonType.KPDPWR, 
                          PonReasonType.PON1, PonReasonType.USB_CHG));
      DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a IsColdBoot = %d BatteryPresent = %d ChargerPresent = %d \r\n", __FUNCTION__, IsColdBoot, BatteryPresent, ChargerPresent));
      DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a Off mode charging False \r\n", __FUNCTION__));*/

      *bOffModeCharging = FALSE;
    }
  }

  return Status;
}

/**
EFI_ChargerExIsPowerOk ()

@brief
Returns if battery voltage is good to process with SW flash
*/
EFI_STATUS
EFIAPI
EFI_ChargerExIsPowerOk
(
  IN  EFI_CHARGER_EX_POWER_TYPE   PowerType,
  OUT VOID                       *pPowerTypeInfo
)
{
  EFI_STATUS Status                = EFI_SUCCESS;
  UINT32     SwFlashBattMinVoltage = 0;
  UINT32     BatteryCurrentVoltage = 0;
  BOOLEAN    BatteryPresent        = FALSE;
  BOOLEAN    ChargerPresent        = FALSE;
  EFI_CHARGER_EX_FLASH_INFO *pFlashInfo = NULL;
  EFI_PLATFORMINFO_PLATFORM_TYPE       PlatformType;

  if(!pPowerTypeInfo)
    return EFI_INVALID_PARAMETER;

  Status |= GetPlatformType(&PlatformType);
  if(EFI_SUCCESS != Status)
  {
    DEBUG((EFI_D_WARN, "ChargerExProtocol:: %a Error getting platform type  \r\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }

  if((EFI_PLATFORMINFO_TYPE_CDP == PlatformType) || (EFI_PLATFORMINFO_TYPE_RUMI == PlatformType))
  {
    DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a CDP/RUMI (%d) Platform detected. No Battery information available. \r\n", __FUNCTION__, PlatformType));
    return EFI_UNSUPPORTED;
  }

  switch(PowerType)
  {
    case EFI_CHARGER_EX_POWER_FLASH_BATTERY_VOLTAGE_TYPE:
      pFlashInfo = (EFI_CHARGER_EX_FLASH_INFO *)pPowerTypeInfo;
      /* Get Battery Presence, Charger Presence, Voltage */
      Status =  EFI_ChargerExGetBatteryPresence(&BatteryPresent);
      Status |= EFI_ChargerExGetChargerPresence(&ChargerPresent);
      Status |= EFI_ChargerExGetBatteryVoltage(&BatteryCurrentVoltage);

      if(EFI_SUCCESS == Status)
      {
        pFlashInfo->BattCurrVoltage = BatteryCurrentVoltage;
      }

      if(!pQcomChargerProtocol)
      {
        Status |= gBS->LocateProtocol( &gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol );
        if(EFI_SUCCESS != Status || NULL == pQcomChargerProtocol)
        {
          return EFI_DEVICE_ERROR;
        }
      }

      Status |= pQcomChargerProtocol->GetChargerConfig(EFI_QCOM_CHARGER_CONFIG_KEY_SW_FLASH_VOLTAGE, &SwFlashBattMinVoltage);

      if(Status == EFI_SUCCESS)
      {
        pFlashInfo->BattRequiredVoltage = SwFlashBattMinVoltage;
        /* If battery not present but still device boot up have debug board */
        if(!BatteryPresent || (BatteryPresent  && (BatteryCurrentVoltage > SwFlashBattMinVoltage)))
        {
          pFlashInfo->bCanFlash = TRUE;
        }
        else
        {
          DEBUG(( EFI_D_WARN, "ChargerExProtocol:: %a SwFlashBattMinVoltage = %d mV\r\n", __FUNCTION__,SwFlashBattMinVoltage));
          pFlashInfo->bCanFlash = FALSE;
        }
      }
    break;
    default:
    break;
  }

  return Status;
}


/**
Charger External UEFI Protocol implementation
*/
EFI_CHARGER_EX_PROTOCOL ChargerExProtocolImplementation = 
{
    CHARGER_EX_REVISION,
    EFI_ChargerExGetChargerPresence,
    EFI_ChargerExGetBatteryPresence,
    EFI_ChargerExGetBatteryVoltage,
    EFI_ChargerExIsOffModeCharging,
    EFI_ChargerExIsPowerOk,
};

