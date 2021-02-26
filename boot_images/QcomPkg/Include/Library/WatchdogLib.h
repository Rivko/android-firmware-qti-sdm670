/** @file WatchdogLib.h
   
  Library implementation to support UEFI Watchdog reset.
  
  Copyright (c) 2017 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/24/12   rks      Created
=============================================================================*/
#ifndef __WATCHDOGLIB_H__
#define __WATCHDOGLIB_H__

#define SEC_TO_MSEC(x) (x*1000)

/*==============================================================================
  FUNCTION      WatchDogEnable

  DESCRIPTION   Set Bit 0 in WDOG_CTL to Enable WatchDogReset 
==============================================================================*/
VOID WatchDogEnable ( VOID );

/*==============================================================================
  FUNCTION      WatchDogDisable

  DESCRIPTION   Reset Bit 0 in WDOG_CTL to Disable WatchDogReset 
==============================================================================*/
VOID WatchDogDisable ( VOID );

/*==============================================================================
  FUNCTION      WatchDogSetBiteTimer

  DESCRIPTION   Set the WDOG_BITE_TIME with the input BiteTime
  
  BITETIME      IN PARAM - time in seconds
  
  RETVAL        EFI_SUCCESS upon success
                EFI_INVALID_PARAMTER if BITETIME > max time 
  
  Note: If the watchdog Bite timer value is changed while wdog is enabled,
        the new Bite timer value will take effect immediately

==============================================================================*/
EFI_STATUS WatchDogSetBiteTimer ( UINT32 );

/*==============================================================================
  FUNCTION      ResetWatchDogCounter

  DESCRIPTION   Set the WDOG_RESET register to reset the count in 
                WDOG_STATUS register.
                

==============================================================================*/
VOID ResetWatchDogCounter( VOID );

/*==============================================================================
  FUNCTION      DoWatchDogReset

  DESCRIPTION   Enable WatchDog and force immediate reset
==============================================================================*/

EFI_STATUS DoWatchDogReset ( VOID );

#endif /* __WATCHDOGLIB_H__ */
