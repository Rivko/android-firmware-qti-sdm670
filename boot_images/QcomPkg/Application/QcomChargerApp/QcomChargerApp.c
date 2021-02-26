/** @file QcomChargerApp.c

  Implements the Qualcomm's Charging application

  Copyright (c) 2016-2018,  Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     -----------------------------------------------------------
 10/13/17     va      Display splash logo if dead battery charging is perfomed
 08/22/17     va      Adding LPM event in extended structure 
 07/21/17     va      Implementing RSC model for LPM interested drivers
 07/12/17     cs      added images for debug board cases.
 06/21/17     dc      Move Platform Check to QcomChargerDxe and support config override
 04/24/17     va      Enable Charger App Changes
 03/24/17     ll      Printed out battery status when in no charge wait state
 03/22/17     ll      Fixed logging sampling time when LED blinking is selected
 03/07/17     cs      remove duplicate get errors checking emergency shutdown condition.
 02/07/17     sm      Added changes to skip ChargerApp for CLS platform
 01/17/17     sm      Code cleanup
 11/10/16     va      Adding crash dump watchdog support
 11/07/16     va      Skip once detected as Debug Board
 10/19/16     sm      Added input parameters to QcomChargerAppEvent_KeyPressHandler() API call
 10/12/16     sv      Fixed Array 'FormatString' KW issue. CR:1063098
 09/29/16     va      update to print function names %a and handle %r format specifier for file logging
 09/26/16     sm      Removed QcomChargerAppEvent_KeyPressControl() API call 
                      and Added keypress check after WaitForTimeout in MonitorCharging API
 09/06/16     cs      wipower changes to remove 3.5 sec, not suspend DCIn
 08/12/16     va      Adding Charger led indication support
 08/10/16     sm      Added changes to skip ChargerApp for RUMI
 07/26/16     va      Restarting FG Changes on warm boot
 07/15/16     cs      added wipower support
 07/07/16     va      DEBUG print to print time stamp in seconds
 07/06/16     sm      Added changes to explicitly disable charging for CDP Platform
 06/28/16     va      Changes to share charger info to smem 
 06/21/16     va      Adding Thermal and File log support
 06/17/16     vk      Delete entry log message
 05/31/16     va      Adding Sign of early life suppot 
 05/23/16     sm      Added changes to handling EFI_QCOM_CHARGER_ACTION_NO_CHARGE_WAIT action
 04/22/16     sm      Added TakeAction() API call for more ChargingAction
 04/13/16     sm      Added support for CDP platform
 03/28/16     va      Add logging CHARGER_DEBUG
 03/21/16     va      Adding Exit API for clean up
 03/15/16     sm      Added changes for bring up to only enter and exit the app
                      without performing any checks. 
 01/27/16     sm      Initial draft for new architecture of Charger app module

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
#include <Library/QcomTargetLib.h>
#include <Library/FuseControlLib.h>
#include <Library/QcomBaseLib.h>

#include <Protocol/EFIVariableServices.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/EFIDisplayUtils.h>

#include <Library/BaseMemoryLib.h>
#include <PrintShLib.h>

/**
  EFI interfaces
 */

#include <Protocol/EFIQcomCharger.h>
#include <Protocol/EFIPlatformInfo.h>

/**
  EFI interfaces
 */
#include <Library/BaseLib.h>

#include "QcomChargerAppFileLogging.h"
#include "QcomChargerAppEventHandler.h"
#include <Lpm.h>

/**
File Logging Dependencies 
*/
#include "ULogFront.h"
#include "ULogUEFI.h"

/*===========================================================================*/
/*                  FUNCTIONS PROTOTYPES                                     */
/*===========================================================================*/
#define QCOM_CHARGER_IDLE_WAIT_DURATION_IN_MS            3000 /*3 sec in milliseconds*/
#define QCOM_CHARGER_DCIN_PENDING_WAIT_DURATION_IN_MS    500  /*500 msec*/
#define QCOM_CHARGER_TOGGLE_LED_WAIT_DURATION_IN_MS      500  /*500 msec*/
#define QCOM_CHARGER_WAIT_TO_LOG_COUNT                   QCOM_CHARGER_IDLE_WAIT_DURATION_IN_MS/QCOM_CHARGER_TOGGLE_LED_WAIT_DURATION_IN_MS  /* 6 times*/

/* Options passed into WaitForTimeout function to break out of the timer loop with a key stroke.*/
#define QCOM_CHARGER_TIMEOUT_WAIT_FOR_KEY    0x01
#define QCOM_CHARGER_POLLING_WAIT_IN_US      3000000 /*3 sec*/
#define QCOM_CHARGER_MS_TO_S                 1000

/*===========================================================================*/
/*                  FUNCTIONS PROTOTYPES                                     */
/*===========================================================================*/
EFI_STATUS QcomChargerApp_PostProcessing( VOID );
EFI_STATUS QcomChargerApp_MonitorCharging( VOID );
EFI_STATUS QcomChargerApp_Initialize( EFI_QCOM_CHARGER_ACTION_TYPE ChargingAction );
EFI_STATUS QcomChargerApp_DeInitialize( VOID );
EFI_STATUS QcomChargerApp_DisplaySignOfLifeOnVBat(EFI_QCOM_CHARGER_ACTION_INFO *ChargerActionInfo);
EFI_STATUS QcomChargerApp_ContinueCharging(EFI_QCOM_CHARGER_ACTION_TYPE ChgAction, EFI_QCOM_CHARGER_ACTION_INFO *pChgActionInfo);

STATIC EFI_QCOM_CHARGER_PROTOCOL    *pQcomChargerProtocol  = NULL;

/*===========================================================================*/
/*                  FUNCTIONS DEFINITIONS                                    */
/*===========================================================================*/
/**
  QcomChargerApp_Entry ()

  @brief
  Initialize Charger App
 */
EFI_STATUS QcomChargerApp_Entry
(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  EFI_QCOM_CHARGER_ACTION_INFO   ChargerActionInfo = {0};
  EFI_QCOM_CHARGER_ACTION_TYPE   ChargingAction;
  BOOLEAN                        bChargingSupported = FALSE;
  
  Status = gBS->LocateProtocol( &gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol );
  if(EFI_SUCCESS != Status || !pQcomChargerProtocol)
  {
    CHARGERAPP_FILE_UART_DEBUG(( EFI_D_ERROR, "QcomChargerApp:: %a Can not locate Charger Protocol = %r \r\n", __FUNCTION__, Status));
    return EFI_DEVICE_ERROR;
  }

  /*Check if Charging is supported in this platform or not*/  
  Status = pQcomChargerProtocol->IsChargingSupported(&bChargingSupported);
  if(EFI_SUCCESS != Status)
  {
    CHARGERAPP_FILE_UART_DEBUG(( EFI_D_ERROR, "QcomChargerApp:: %a Cannot check if this platform support charging = %r \r\n", __FUNCTION__, Status));
    return EFI_DEVICE_ERROR;    
  }
    
  if(FALSE == bChargingSupported)
  {
    CHARGERAPP_FILE_UART_DEBUG(( EFI_D_WARN, "QcomChargerApp:: %a Charging is not supported. Exiting app. \r\n", __FUNCTION__));
    return EFI_SUCCESS;
  } 
  SetMem(&ChargerActionInfo, sizeof(EFI_QCOM_CHARGER_ACTION_INFO), 0x00);
  
  /*Get charging Action to be taken */
  Status = pQcomChargerProtocol->GetChargingAction(&ChargingAction, &ChargerActionInfo, TRUE);

  /* In case of debug board, to save boot time below can be ignored since debug board no Action for charger app */
  if(EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_GOOD_TO_BOOT != ChargingAction)
  {
    Status |= QcomChargerAppFileLog_Init();
    if(EFI_SUCCESS != Status)
    {
      CHARGERAPP_FILE_UART_DEBUG(( EFI_D_WARN, "QcomChargerApp:: %a recieved error from QcomChargerAppFileLog_Init() = %r \r\n", __FUNCTION__, Status));
      /* Override Status */
      Status = EFI_SUCCESS;
    }

    /* Handle if FG battery profile needs to be loaded */
    if(EFI_QCOM_CHARGER_PLATFORM_ACTION_PROFILE_LOAD == ChargingAction)
    {
      if(ChargerActionInfo.ProfState == EFI_QCOM_CHARGER_PROFILE_LOAD)
      {
        Status |= QcomChargerApp_DisplaySignOfLifeOnVBat(&ChargerActionInfo);
      }

      /* Load Battery Profile */
      Status |= pQcomChargerProtocol->TakeAction(ChargingAction, &ChargerActionInfo);

      /*Get charging Action again to be taken to proceed further for charging if needed */
      Status = pQcomChargerProtocol->GetChargingAction(&ChargingAction, &ChargerActionInfo, FALSE);
      if(EFI_SUCCESS != Status )
      {
        CHARGERAPP_DEBUG(( EFI_D_ERROR, "QcomChargerApp:: %a GetChargingAction Status = %r \r\n", __FUNCTION__, Status));
        return EFI_DEVICE_ERROR;
      }
    }

    Status |= pQcomChargerProtocol->TakeAction(ChargingAction, &ChargerActionInfo);

    /* DO charger App Actions */
    switch(ChargingAction)
    {
      case EFI_QCOM_CHARGER_ACTION_START_CHARGING:
      case EFI_QCOM_CHARGER_ACTION_NO_CHARGE_WAIT:
      case EFI_QCOM_CHARGER_ACTION_CONTINUE:
      case EFI_QCOM_CHARGER_ACTION_TSENSE_HIGH_WAIT:
      case EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_WAIT:
           /*Initializes and displays the battery symbol*/
           Status |= QcomChargerApp_Initialize(ChargingAction);
           if(EFI_SUCCESS == Status)
           {
             /* if initialization success then start charging upto target SoC*/
             Status |= QcomChargerApp_MonitorCharging();
             Status |= QcomChargerApp_PostProcessing();
           }
           else
           {
             CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp::%a Initialization error = %r \r\n",__FUNCTION__, Status));
             /* return Status */; /* Allowing deinit */
           }
           break;

      case EFI_QCOM_CHARGER_ACTION_GOOD_TO_BOOT:
      case EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_GOOD_TO_BOOT:
           CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: QcomChargerApp_Start: Battery level is good to boot to HLOS. \r\n"));
           break;

      default:
           CHARGERAPP_DEBUG(( EFI_D_ERROR, "QcomChargerApp:: %a ChargingAction = %d \r\n", __FUNCTION__, ChargingAction));
           break;
    }
  }
  else
  {
    CHARGERAPP_DEBUG(( EFI_D_ERROR, "QcomChargerApp:: %a Debug Board detected boot to hlos ChargingAction = %d \r\n", __FUNCTION__, ChargingAction));
  }
  /* After post processing call to exit module */
  Status |= QcomChargerApp_DeInitialize();

  CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Exiting \r\n", __FUNCTION__));

  return Status;
}

EFI_STATUS QcomChargerApp_Initialize( EFI_QCOM_CHARGER_ACTION_TYPE ChargingAction )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!pQcomChargerProtocol)
  {
    Status = gBS->LocateProtocol( &gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol );
  }

   if (ChargingAction == EFI_QCOM_CHARGER_ACTION_TSENSE_HIGH_WAIT)
  {
    QcomChargerAppEvent_DispBattSymbol(EFI_QCOM_CHARGER_DISP_IMAGE_TSENS_THERMAL_SYMBOL);
  }
  else if (ChargingAction == EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_WAIT)
  {
    QcomChargerAppEvent_DispBattSymbol(EFI_QCOM_CHARGER_DISP_IMAGE_DEBUG_LOW_SYMBOL);
  }
  else
  {
    QcomChargerAppEvent_DispBattSymbol(EFI_QCOM_CHARGER_DISP_IMAGE_LOWBATTERY);
  }
  /*Start required timers */
  Status |= QcomChargerAppEvent_DisplayTimerEvent(TRUE, TRUE);
  
  CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Status = %r \r\n", __FUNCTION__, Status));

  return Status;
}

/*
QcomChargerApp_MonitorCharging(): This is the charging loop that will keep the chargerApp running, 
it will also call GetChargingAction function to get the error status, get charging/gauging status, 
control charging and determine when to exit charging loop.
*/
EFI_STATUS QcomChargerApp_MonitorCharging( VOID )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_QCOM_CHARGER_ACTION_TYPE  ChargingAction;
  EFI_QCOM_CHARGER_ACTION_INFO  ChargerActionInfo = {0};
  BOOLEAN                       ExitChargingLoop = FALSE;
  EFI_INPUT_KEY                 UserKey;

  CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a \r\n", __FUNCTION__));

  if(!pQcomChargerProtocol)
  {
    Status = gBS->LocateProtocol( &gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol );
    if(EFI_SUCCESS != Status)
      return Status;
  }

  /* Disable crash dump watchdog only when charger app starts charging */
  gBS->SetWatchdogTimer(0, 0, 0, NULL);

  CHARGERAPP_FILE_DEBUG((EFI_D_WARN, "RebootCount,TimeStamp,StateOfCharge,Voltage,ChargeCurrent,Temp \r\n"));
  DEBUG((EFI_D_WARN, "QcomChargerApp::%a TimeStamp,StateOfCharge,Voltage,ChargeCurrent,Temp \r\n",__FUNCTION__));

  Status = QcomChargerAppEvent_ReportStatusCode(gEfiPciIoProtocolGuid, LPM_ENTRY);

  do
  {
    /* Get charging action to be taken */
    Status = pQcomChargerProtocol->GetChargingAction(&ChargingAction, &ChargerActionInfo, TRUE);
    CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a ChargingAction = %d Status = %r LedConfigType = %d \r\n", __FUNCTION__, ChargingAction, Status, ChargerActionInfo.LedConfigType));
    if(EFI_SUCCESS != Status)
    {
      return Status;
    }

    switch(ChargingAction)
    {
      case EFI_QCOM_CHARGER_ACTION_GOOD_TO_BOOT:
      case EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_GOOD_TO_BOOT:
           Status = pQcomChargerProtocol->TakeAction(ChargingAction, &ChargerActionInfo);

           QcomChargerAppEvent_ExitLPM();/* Exit LPM and */

           QcomChargerAppEvent_DisplayTimerEvent(FALSE, FALSE);  /*Display image */

           ExitChargingLoop = TRUE;
           break;

      case EFI_QCOM_CHARGER_ACTION_SHUTDOWN:
           QcomChargerAppEvent_DisplayTimerEvent(FALSE, FALSE); /*Display image before shutting down*/

           QcomChargerAppEvent_ExitLPM(); /* Exit LPM */

           Status = pQcomChargerProtocol->TakeAction(ChargingAction, &ChargerActionInfo);
           QcomChargerAppFileLog_LogParam(&ChargerActionInfo);
           break;

      case EFI_QCOM_CHARGER_ACTION_NO_CHARGE_WAIT:
      case EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_WAIT:
           /* Take Action to make sure charging is disabled */
           Status = pQcomChargerProtocol->TakeAction(EFI_QCOM_CHARGER_ACTION_NO_CHARGE_WAIT, &ChargerActionInfo);
           
           QcomChargerAppFileLog_LogParam(&ChargerActionInfo);
           CHARGERAPP_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a No Charge Wait ChrgAct = %d wait %d ms \r\n", __FUNCTION__, ChargingAction, QCOM_CHARGER_IDLE_WAIT_DURATION_IN_MS));
           WaitForTimeout (QCOM_CHARGER_IDLE_WAIT_DURATION_IN_MS, QCOM_CHARGER_TIMEOUT_WAIT_FOR_KEY, &UserKey);

           /*Check if WaitForTimeout returned due to key press */
           if((UserKey.UnicodeChar != 0x00) || (UserKey.ScanCode != 0x00))
           {
             DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Keypress detected \r\n", __FUNCTION__));
             if (ChargingAction == EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_WAIT)
             {
               QcomChargerAppEvent_KeyPressHandler(QCOMCHARGERAPP_EVENT_ANIMATION__DEBUG_BOARD_NO_ANIM);
             }
             else
             {
               QcomChargerAppEvent_KeyPressHandler(QCOMCHARGERAPP_EVENT_ANIMATION__LOW_BATTERY_CHARGER_NO_ANIM);
             }            
           }

           break;

      case EFI_QCOM_CHARGER_ACTION_TSENSE_HIGH_WAIT:
        {
          /* Take Action to make sure charging is disabled */
          Status = pQcomChargerProtocol->TakeAction(ChargingAction, &ChargerActionInfo);

		  QcomChargerAppFileLog_LogParam(&ChargerActionInfo);
          CHARGERAPP_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a No Charge Wait ChrgAct = %d wait %d ms \r\n", __FUNCTION__, ChargingAction, QCOM_CHARGER_IDLE_WAIT_DURATION_IN_MS));
          WaitForTimeout(QCOM_CHARGER_IDLE_WAIT_DURATION_IN_MS, QCOM_CHARGER_TIMEOUT_WAIT_FOR_KEY, &UserKey);
          /*Check if WaitForTimeout returned due to key press */
          if ((UserKey.UnicodeChar != 0x00) || (UserKey.ScanCode != 0x00))
          {
            DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Keypress detected \r\n", __FUNCTION__));
            QcomChargerAppEvent_KeyPressHandler(QCOMCHARGERAPP_EVENT_ANIMATION__THERMAL_NO_ANIM);
          }
        }
      break;
      
      case EFI_QCOM_CHARGER_ACTION_CRITICAL:
           /*Exit LPM and Display image before shutting down*/
           QcomChargerAppEvent_ExitLPM();

           Status = pQcomChargerProtocol->TakeAction(ChargingAction, &ChargerActionInfo);
           break;

      case EFI_QCOM_CHARGER_ACTION_SHUTDOWN_USB_DC_PON_DISABLED:
           /*Exit LPM and Display image before shutting down*/
           QcomChargerAppEvent_ExitLPM();

           CHARGERAPP_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Shutdown with PON disabled \r\n", __FUNCTION__));

           Status = pQcomChargerProtocol->TakeAction(ChargingAction, &ChargerActionInfo);
           QcomChargerAppFileLog_LogParam(&ChargerActionInfo);
           break;
      case EFI_QCOM_CHARGER_ACTION_CONTINUE:
      default:
           Status = QcomChargerApp_ContinueCharging(ChargingAction, &ChargerActionInfo);
           break;
    }

  }while (FALSE == ExitChargingLoop);

  /* Start 10min watchdog timer */
  gBS->SetWatchdogTimer(10 * 60, 0, 0, NULL);

  if (ChargingAction == EFI_QCOM_CHARGER_ACTION_DEBUG_BOARD_GOOD_TO_BOOT)
  {
    Status |= pQcomChargerProtocol->DisplayImage(EFI_QCOM_CHARGER_DISP_IMAGE_DEBUG_BOOT_SYMBOL, TRUE);
  }
  else
  {
    Status |= pQcomChargerProtocol->DisplayImage(EFI_QCOM_CHARGER_DISP_IMAGE_ABOVE_THRESHOLD, TRUE);
  }

  CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Exiting ChargingAction = %d \r\n", __FUNCTION__, ChargingAction));

  return Status;
}

/*After Charging is completed, handle cases for threshold charging and full battery charging*/
EFI_STATUS QcomChargerApp_PostProcessing( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_QCOM_DISPLAY_UTILS_PROTOCOL *pDisplayUtilsProtocol = NULL;
  EFI_DISPLAY_UTILS_RENDERLOGO_CONFIG RenderLogoCfg = { 0 };

  if(!pQcomChargerProtocol)
  {
    Status = gBS->LocateProtocol( &gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol );
  }

  QcomChargerAppEvent_ExitLPM();

  Status = QcomChargerAppEvent_ReportStatusCode(gEfiPciIoProtocolGuid, LPM_EXIT);

  /* Close timer events */
  QcomChargerAppEvent_DisplayTimerEvent(FALSE, FALSE); 

  /* Ensure charging is enabled when booting from UEFI to HLOS */
  Status |= pQcomChargerProtocol->EnableCharging(TRUE);

  if(NULL == pDisplayUtilsProtocol)
  {
    Status |= gBS->LocateProtocol( &gQcomDisplayUtilsProtocolGuid, NULL, (VOID **)&pDisplayUtilsProtocol);
  }

  if((EFI_SUCCESS == Status) && (pDisplayUtilsProtocol != NULL))
  {
    RenderLogoCfg.uDisplayId = 0;
    RenderLogoCfg.uFlags     = DISPLAY_UTILS_RENDERLOGO_CLEARSCREEN;
    RenderLogoCfg.xPosition  = 0;
    RenderLogoCfg.yPosition  = 0;
    Status |= pDisplayUtilsProtocol->DisplayUtilsRenderLogo(&RenderLogoCfg);
  }

  return Status;
}

EFI_STATUS QcomChargerApp_DeInitialize( VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!pQcomChargerProtocol)
  {
    Status = gBS->LocateProtocol( &gQcomChargerProtocolGuid, NULL, (VOID **)&pQcomChargerProtocol );
  }

  if((EFI_SUCCESS == Status) && (pQcomChargerProtocol))
  {
    Status = pQcomChargerProtocol->ChargerDeInit(EFI_QCOM_CHARGER_ACTION_EXIT);
  }

  return Status;
}


EFI_STATUS QcomChargerApp_DisplaySignOfLifeOnVBat(EFI_QCOM_CHARGER_ACTION_INFO *ChargerActionInfo)
{
  EFI_STATUS Status   = EFI_SUCCESS;

  if(!ChargerActionInfo)
    return EFI_INVALID_PARAMETER;

  CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp::%a CurrSoc = %d, BatteryVolt = %d mV, VbatThresholdForDisp = %d mV, Status = %r \r\n",__FUNCTION__,
                ChargerActionInfo->BatteryStatusInfo.StateOfCharge , ChargerActionInfo->BatteryStatusInfo.BatteryVoltage, ChargerActionInfo->DispSignOfLifeMaxThresholdMv, Status));

  if(ChargerActionInfo->BatteryStatusInfo.BatteryVoltage <= (ChargerActionInfo->DispSignOfLifeMaxThresholdMv ))
  {
    if(TRUE == ChargerActionInfo->bChargerSrcPresent)
    {
      Status |= QcomChargerAppEvent_DispBattSymbol(EFI_QCOM_CHARGER_DISP_IMAGE_LOWBATTERYCHARGING);
    }
    else
    {
      Status |= QcomChargerAppEvent_DispBattSymbol(EFI_QCOM_CHARGER_DISP_IMAGE_LOWBATTERY);
    }
  }
  else
  {
    CHARGERAPP_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Skipping Init Display On CurrSoc = %d, BatteryVolt = %d mV, ", __FUNCTION__, 
                      ChargerActionInfo->BatteryStatusInfo.StateOfCharge , ChargerActionInfo->BatteryStatusInfo.BatteryVoltage));
	CHARGERAPP_DEBUG((EFI_D_WARN, "DispSignOfLifeMaxThresholdMv = %d mV Status = %r \r\n",
                      ChargerActionInfo->DispSignOfLifeMaxThresholdMv, Status));
  }

  return Status;
}

/**
  QcomChargerApp_ContinueCharging ()

  @brief
  Take action to continue charging
 */
EFI_STATUS QcomChargerApp_ContinueCharging(EFI_QCOM_CHARGER_ACTION_TYPE ChgAction, EFI_QCOM_CHARGER_ACTION_INFO *pChgActionInfo)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8 ledToggleCnt;
	EFI_INPUT_KEY UserKey;

	/* Take Action */
	if (pChgActionInfo->LedConfigType == EFI_QCOM_CHARGER_CHARGING_LED_TOGGLE)
	{
		ledToggleCnt = 0;
		do
		{
			Status |= pQcomChargerProtocol->TakeAction(ChgAction, pChgActionInfo);
			WaitForTimeout(QCOM_CHARGER_TOGGLE_LED_WAIT_DURATION_IN_MS, QCOM_CHARGER_TIMEOUT_WAIT_FOR_KEY, &UserKey);
			/*Check if WaitForTimeout returned due to key press */
			if ((UserKey.UnicodeChar != 0x00) || (UserKey.ScanCode != 0x00))
			{
				DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Keypress detected \r\n", __FUNCTION__));
				QcomChargerAppEvent_KeyPressHandler(QCOMCHARGERAPP_EVENT_ANIMATION__CHARGING_ANIM);
			}
			ledToggleCnt++;
	    // Wait 3sec (QCOM_CHARGER_IDLE_WAIT_DURATION_IN_MS) to go to next charger status check loop
		} while (ledToggleCnt < QCOM_CHARGER_WAIT_TO_LOG_COUNT); 

	}
	else
	{
		Status |= pQcomChargerProtocol->TakeAction(ChgAction, pChgActionInfo);
		WaitForTimeout(QCOM_CHARGER_IDLE_WAIT_DURATION_IN_MS, QCOM_CHARGER_TIMEOUT_WAIT_FOR_KEY, &UserKey);
		/*Check if WaitForTimeout returned due to key press */
		if ((UserKey.UnicodeChar != 0x00) || (UserKey.ScanCode != 0x00))
		{
			DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Keypress detected \r\n", __FUNCTION__));
			QcomChargerAppEvent_KeyPressHandler(QCOMCHARGERAPP_EVENT_ANIMATION__CHARGING_ANIM);
		}
	}
	QcomChargerAppFileLog_LogParam(pChgActionInfo);
	CHARGERAPP_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerApp:: %a Waiting for %d ms \r\n", __FUNCTION__, QCOM_CHARGER_IDLE_WAIT_DURATION_IN_MS));

	return Status;
}
