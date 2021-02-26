/** 
  @file  QcomChargerAppEventHandler.h
  @brief QCOM Charger App event handler API definitions.
*/
/*=============================================================================
  Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/12/17   cs      added images for debug board cases.
 03/09/17   cs      add support to display thermal image
 10/19/16   sm      Added enum QCOMCHARGERAPP_EVENT_ANIMATION_TYPE and input parameter 
                    to QcomChargerAppEvent_KeyPressHandler() API
 09/27/16   sm      Renamed QcomChargerAppEvent_KeyPressEventHandler to QcomChargerAppEvent_KeyPressHandler
                    and removed QcomChargerAppEvent_KeyPressControl()
 05/31/16   va      Adding Sign of early life suppot 
 03/14/16   sm      Initial draft

=============================================================================*/
#ifndef __QCOMCHARGERAPPEVENTHANDLER_H__
#define __QCOMCHARGERAPPEVENTHANDLER_H__

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/**
UEFI DEBUG Dependencies 
*/
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TimerLib.h>
#include <Lpm.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/
/**  Attached charger type. **/
typedef enum 
{
   QCOMCHARGERAPP_EVENT_ANIMATION__LOW_BATTERY_NO_ANIM,    /**< No charger. */
   QCOMCHARGERAPP_EVENT_ANIMATION__CHARGING_ANIM,    /**< Battery.    */
   QCOMCHARGERAPP_EVENT_ANIMATION__THERMAL_NO_ANIM,    /**< Tsense High Wait */
   QCOMCHARGERAPP_EVENT_ANIMATION__LOW_BATTERY_CHARGER_NO_ANIM,    /**< for no charge wait. */
   QCOMCHARGERAPP_EVENT_ANIMATION__DEBUG_BOARD_NO_ANIM,    /**< debug board */
   QCOMCHARGERAPP_EVENT_ANIMATION__INVALID,    /**< DCIN path.  */
} QCOMCHARGERAPP_EVENT_ANIMATION_TYPE;

/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
EFI_STATUS QcomChargerAppEvent_KeyPressHandler( QCOMCHARGERAPP_EVENT_ANIMATION_TYPE AnimationType );

VOID QcomChargerAppEvent_ExitLPM( VOID );

EFI_STATUS QcomChargerAppEvent_DisplayTimerEvent(BOOLEAN bDispOffTimer, BOOLEAN AnimTimer);

EFI_STATUS QcomChargerAppEvent_DispBattSymbol(EFI_QCOM_CHARGER_DISP_IMAGE_TYPE DispImageType);

EFI_STATUS QcomChargerAppEvent_ReportStatusCode(EFI_GUID gCurrentGUID, LPM_STATE LpmState);

#endif  /* __QCOMCHARGERAPPEVENTHANDLER_H__ */

