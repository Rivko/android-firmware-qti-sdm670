/*==============================================================================

FILE:      TimetickUEFI.c

DESCRIPTION: This file implements a wrapper around hal timer 
             functions for UEFI.

===========================================================================
             Copyright (c) 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/18/12   pbi      Added a new API, Timetick_SetTimerPeriod
 03/15/11   pbi      Changed Timetick_GetFreq to take clk divider for ADGT
                     into account.
                     Added a new API, Timetick_Init to separate the initialization
                     from Enable.
 02/08/11   pbi      Created.

=============================================================================*/
#include <Uefi.h>
#include "HALTimer.h"
#include "TimetickUEFI.h"
#include <Library/PcdLib.h>

/*========================================================================

                           DATA DEFINITIONS

========================================================================*/
/*------------------------------------------------------------------------
  Constants used by the timetick driver
------------------------------------------------------------------------*/
/* Reading the current time, and programming the timetick match value 
   takes a finite period of time.  Ensure the match value is at least 
   this many slow clocks after the value that was just read.  Note that 
   this value may vary based upon hardware, but for simplicity we just 
   use the maximum necessary value. */
#define  MIN_REARM_SCLK           9

static EFI_STATUS Timetick_MapTimerToHAL(Timetick_timer_Type nTimer, HAL_timer_Type *pHalTimer)
{
  switch (nTimer) 
  {
    case TIMETICK_TIMER_GPT0:
      *pHalTimer =  HAL_TIMER_GPT0;
      break;

    case TIMETICK_TIMER_GPT1:
      *pHalTimer =  HAL_TIMER_GPT1;
      break;

    case TIMETICK_TIMER_ADGT:
      *pHalTimer =  HAL_TIMER_ADGT;
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}


/*=============================================================================

FUNCTION Timetick_Init
 
=============================================================================*/
EFI_STATUS Timetick_Init( void)
{
  static nIsHALTimerInitialized=0;

  /* Initialize the underlying Timer HAL*/
  if ( !nIsHALTimerInitialized )
  {
    nIsHALTimerInitialized = 1;
    HAL_timer_Init();
  }
  return EFI_SUCCESS;
}/* END Timetick_Init */


/*=============================================================================

FUNCTION Timetick_Enable
 
=============================================================================*/
EFI_STATUS Timetick_Enable( Timetick_timer_Type nTimer, boolean enable)
{
  EFI_STATUS status;
  HAL_timer_Type nHalTimer;

  status = Timetick_MapTimerToHAL(nTimer, &nHalTimer);
  if ( status == EFI_SUCCESS )
  {
    HAL_timer_Enable (nHalTimer, enable);
  }

  return status;
}/* END Timetick_Enable */


/*=============================================================================

FUNCTION Timetick_ClearOnMatchEnable

=============================================================================*/
EFI_STATUS Timetick_ClearOnMatchEnable(Timetick_timer_Type nTimer, boolean enable)
{  
  EFI_STATUS status;
  HAL_timer_Type nHalTimer;

  status = Timetick_MapTimerToHAL(nTimer, &nHalTimer);
  if ( status == EFI_SUCCESS )
  {
    HAL_timer_ClearOnMatchEnable (nHalTimer, enable);
  }
  return status;
}/* END Timetick_ClearOnMatchEnable */


/*==========================================================================

  FUNCTION      Timetick_Get

==========================================================================*/
EFI_STATUS Timetick_GetCount(Timetick_timer_Type nTimer, uint32 *pTick)
{
  EFI_STATUS status;
  HAL_timer_Type nHalTimer;

  status = Timetick_MapTimerToHAL(nTimer, &nHalTimer);
  if ( status == EFI_SUCCESS )
  {
    HAL_timer_ReadCount (nHalTimer, pTick);
  }
  return status;
} /* END Timetick_Get */


/*==========================================================================

  FUNCTION      Timetick_SetMatchValue

==========================================================================*/
EFI_STATUS Timetick_SetMatchValue
(
  Timetick_timer_Type         nTimer, 
  uint32                      matchCount,
  uint32                      *matchVal
)
{
  EFI_STATUS status;
  HAL_timer_Type nHalTimer;
  uint32 ticks_now;

  status = Timetick_MapTimerToHAL(nTimer, &nHalTimer);
  if ( status == EFI_SUCCESS )
  {
      HAL_timer_ReadCount (nHalTimer, &ticks_now);
      HAL_timer_SetMatchValue(nHalTimer, matchCount,ticks_now, MIN_REARM_SCLK, matchVal);
  }
  return status;

} /* END Timetick_SetMatchValue */


/*==========================================================================

  FUNCTION      Timetick_GetMatchValue

==========================================================================*/
EFI_STATUS Timetick_GetMatchValue(Timetick_timer_Type nTimer, uint32 *pTick)
{
  EFI_STATUS status;
  HAL_timer_Type nHalTimer;

  status = Timetick_MapTimerToHAL(nTimer, &nHalTimer);
  if ( status == EFI_SUCCESS )
  {
    HAL_timer_ReadMatchValue (nHalTimer, pTick);
  }
  return status;

} /* END Timetick_GetMatchValue */



/*=============================================================================

FUNCTION Timetick_GetFreq

=============================================================================*/
EFI_STATUS Timetick_GetFreq(Timetick_timer_Type nTimer, uint32 *nFreq) 
{
  EFI_STATUS status = EFI_SUCCESS;
  HAL_timer_Type nHalTimer;
  uint32 nDivider;

  status = Timetick_MapTimerToHAL(nTimer, &nHalTimer);
  if ( status == EFI_SUCCESS )
  {
    HAL_timer_GetDivider(nHalTimer, &nDivider);
  
    switch (nTimer) 
    {
      case TIMETICK_TIMER_GPT0:
        *nFreq = PcdGet32(PcdTimerGPT0Freq)/nDivider;
        break;
    
      case TIMETICK_TIMER_GPT1:
         *nFreq = PcdGet32(PcdTimerGPT1Freq)/nDivider;
        break;
    
      case TIMETICK_TIMER_ADGT:
         *nFreq = PcdGet32(PcdTimerADGTFreq)/nDivider;
         break;

      default:
        return EFI_INVALID_PARAMETER;
    }
  }
  return status;

}/* END Timetick_GetFreq */



/*=============================================================================

FUNCTION Timetick_ClearTimer

=============================================================================*/
EFI_STATUS Timetick_ClearTimer(Timetick_timer_Type nTimer)
{
  EFI_STATUS status;
  HAL_timer_Type nHalTimer;

  status = Timetick_MapTimerToHAL(nTimer, &nHalTimer);
  if ( status == EFI_SUCCESS )
  {
    HAL_timer_Clear(nHalTimer);
  }
  return status;

}/* END Timetick_ClearTimer */


/*=============================================================================

FUNCTION Timetick_SetTimerPeriod

=============================================================================*/
EFI_STATUS Timetick_SetTimerPeriod
(
  Timetick_timer_Type         nTimer, 
  uint32                      matchCount,
  uint32                      *matchVal
)
{
  EFI_STATUS status;
  HAL_timer_Type nHalTimer;

  status = Timetick_MapTimerToHAL(nTimer, &nHalTimer);
  if ( status == EFI_SUCCESS )
  {
      HAL_timer_Enable (nHalTimer, 1);
      HAL_timer_ClearOnMatchEnable (nHalTimer, 1);
      HAL_timer_WriteMatchValue(nHalTimer, matchCount);
      HAL_timer_Clear(nHalTimer);
      HAL_timer_ReadMatchValue(nHalTimer, matchVal);
  }
  return status;

} /* END Timetick_SetTimerPeriod */

