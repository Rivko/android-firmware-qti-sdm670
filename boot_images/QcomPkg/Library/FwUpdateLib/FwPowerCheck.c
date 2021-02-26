/** @file PowerCheck.c
   
  Battery power level checks for Firmware Update.

  Microsoft requirement is that system must have at least 25% battery life
  as a pre-installation check to permit firmware update.

  Non-production mode devices will allow firmware update to proceed
  when no battery is present for lab automation testing.

  Copyright (c) 2013,2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 01/27/17       ai          Fix use of retail
 2013/04/30     mic         Added debug message
 2013/02/08     rsb         Enable battery power check on B-Family targets
 2012/10/03     rsb         Set bPowerGood correctly when function is stubbed out
 2012/08/23     al          Removing Charger and ChargerTask related
 2012/08/03     mic         Aligned with UEFI1.0 and added DeviceIndex and 
                            ResourceIndex in PmicChargerProtocol
 2012/07/25     rs          Reduced battery threshold to 25% from 50% for FwUpdate
 2012/07/10     rsb         Added non-production mode support for no battery
 2012/06/05     rs          Replaced AsciiPrint with DEBUG macro
 2012/4/30      rsb         Initial version

=============================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomTargetLib.h>

#include "FwPowerCheck.h"
#include "FwEsrt.h"

/**
  Include Charger DXE Dependencies
  */
#include <Protocol/EFICharger.h>

/**
  Include Charger Extension Dependencies
  */
#include <Protocol/EFIChargerExtn.h>

#define MINIMUM_BATTERY_CHARGE_PERCENT 25

/**
  Define ALLOW_NO_BATTERY_FIRMWARE_UPDATE to allow firmware update to
  proceed for non-production mode devices that have no battery and only
  DC power adapter connected.

  The logic will only permit this behavior for non-production mode
  devices for use in lab automation testing. Devices that are production
  mode will not be permitted to update when no battery is present,
  even if the macro is defined.
  */
#define ALLOW_NO_BATTERY_FIRMWARE_UPDATE

/**
 * Determine if the battery power levels have sufficient power
 * to permit firmware update to proceed based on the pre-condition
 * checks required for UEFI firmware update:
 *    1) System must have at least 25% battery life
 *
 * For Qualcomm reference hardware (if ALLOW_NO_BATTERY_FIRMWARE_UPDATE is defined):
 *    2) or has a DC adaptor plugged in with no battery
 *
 * If the function returns EFI_SUCCESS, then *bPowerGood and *lastAttemptStatus
 * will be modified.  If bPowerGood is FALSE, then lastAttemptStatus
 * should indicate either of FW_LAST_ATMPT_STATUS_POWER_AC_NOT_CONNECTED
 * or FW_LAST_ATMPT_STATUS_POWER_INSUFFICIENT_BATTERY depending on
 * the power condition.
 * 
 * @param bPowerGood        - Return boolean indicating there is sufficient
 *                            power to proceed with firmware update.
 * @param lastAttemptStatus - The error code to populate to the ESRT
 *                            last attempt status field if bPowerGood is
 *                            FALSE.
 * @param QcResultFlag      - The error code to return to QCFirmwareUpdate 
 *                            result
 * @return EFI_STATUS 
 */

EFI_STATUS
FwUpdateCheckPowerLevel(
  OUT BOOLEAN *bPowerGood,
  OUT ESRT_LAST_ATTEMPT_STATUS *lastAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

)
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_BATTERY_CHARGING_PROTOCOL  *ChargerProtocol = NULL;

  /* Parameters to get the current status of battery */
  UINT32 CurrentSOC = 0;
  UINT32 RatedCapacity = 0;
  INT32  ChargeCurrent = 0;

#ifdef ALLOW_NO_BATTERY_FIRMWARE_UPDATE
  EFI_BATTERY_CHARGING_EXTN_PROTOCOL *ChargerExtProtocol = NULL;
  BOOLEAN bBatteryPresent = FALSE;
  EFI_CHARGER_ATTACHED_CHGR_TYPE PowerPath = EFI_CHARGER_ATTACHED_CHGR__NONE;
#endif /* ALLOW_NO_BATTERY_FIRMWARE_UPDATE */

  if ((bPowerGood == NULL) || (lastAttemptStatus == NULL) || (QcResultFlag == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto power_exit;
  }

  /* Locate Charger Protocol */
  Status = gBS->LocateProtocol(
                  &gBatteryChargerProtocolGuid,
                  NULL,
                  (VOID**) &ChargerProtocol );
  
  if ((Status != EFI_SUCCESS) || (ChargerProtocol == NULL)) 
  {
    DEBUG(( EFI_D_ERROR,"FwUpdateCheckPowerLevel: Locate EFI_BATTERY_CHARGING_PROTOCOL returned %r\n", Status));
    goto power_exit;
  }

#ifdef ALLOW_NO_BATTERY_FIRMWARE_UPDATE
  if (!RETAIL)
  {
    /* Locate the Charger Extension Protocol, used on Qualcomm reference devices to 
               check if a battery is connected and if a DC wall adaptor is connected. */
    Status = gBS->LocateProtocol( &gBatteryChargerExtnProtocolGuid,
                                  NULL,
                                  (VOID **) &ChargerExtProtocol );
    
    if ((Status != EFI_SUCCESS) || (ChargerExtProtocol == NULL))
    {
      DEBUG(( EFI_D_ERROR,"FwUpdateCheckPowerLevel: Locate EFI_BATTERY_CHARGING_EXTN_PROTOCOL returned %r\n", Status));

      /* Protocol not found for non-Qualcomm reference devices. Implementation required by OEM. */
      ChargerExtProtocol = NULL;
    }
  }
#endif /* ALLOW_NO_BATTERY_FIRMWARE_UPDATE */

  /* GetBatteryStatus - Get current state of charge */
  Status = ChargerProtocol->GetBatteryStatus( ChargerProtocol,
                                              &CurrentSOC,
                                              &RatedCapacity,
                                              &ChargeCurrent );

  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR,"FwUpdateCheckPowerLevel: GetBatteryStatus returned %r\n", Status));
    goto power_exit;
  }
  
  DEBUG(( EFI_D_WARN,"GetBatteryStatus success, current battery capacity: %x\n", CurrentSOC));
  
  if (CurrentSOC < MINIMUM_BATTERY_CHARGE_PERCENT)
  {
    *bPowerGood = FALSE;

#ifdef ALLOW_NO_BATTERY_FIRMWARE_UPDATE
  
    if (!RETAIL)
    {
      /* If Qualcomm reference hardware, check if there is no battery with DC adaptor cable plugged in,
         and if so then permit the firmware update to proceed. */
      if (ChargerExtProtocol != NULL)
      {
         if (ChargerExtProtocol->GetPowerPath(&PowerPath) != EFI_SUCCESS)
         {
           /* Failed querying if a DC adaptor is present. Return insufficient battery. */
           
           DEBUG(( EFI_D_ERROR,"GetPowerPath returned failure\n"));
           goto power_exit;
         }

         if (ChargerExtProtocol->GetBatteryPresence(&bBatteryPresent) != EFI_SUCCESS)
         {
           /* Failed querying if a battery is present. Return insufficient battery. */
           DEBUG(( EFI_D_ERROR,"GetBatteryPresenceh returned failure\n"));
           goto power_exit;
         }

         DEBUG(( EFI_D_ERROR,"Power Path %d\n", PowerPath));
         DEBUG(( EFI_D_ERROR,"Battery present %d\n", bBatteryPresent));

         /* If the DC adaptor is plugged in and there is no battery, then allow the firmware update to proceed. */
         if ((PowerPath == EFI_CHARGER_ATTACHED_CHGR__DCIN) && (bBatteryPresent == FALSE))
         {
           *bPowerGood = TRUE;
         }
      }
    }
#endif

  }
  else
  {
    *bPowerGood = TRUE;
  }

power_exit:

  if ((Status == EFI_SUCCESS) && (*bPowerGood == FALSE))
  {
    *lastAttemptStatus = FW_LAST_ATMPT_STATUS_POWER_INSUFFICIENT_BATTERY;
    *QcResultFlag = QCFW_UPDATE_ERR_RESOUCE_POWER_INSUFFICEIENT_BATTERY;
  }
  
  return Status;

}
