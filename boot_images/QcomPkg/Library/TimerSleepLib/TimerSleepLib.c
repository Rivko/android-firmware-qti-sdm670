/** @file
  Timer Sleep Library

  Copyright (c) 2014-15, Qualcomm Technologies Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/10/14   abalanag  Initial Revision

=============================================================================*/

#include <Library/ArmArchTimerLib.h>
#include <Library/TimerSleepLib.h>

/**
  Obtains the Match Timetick value that is programmed for next timer interrupt.
  Sleep module needs to call this api to get next match value.

  @param  None.

  @return Next Match value.

**/
UINT64
EFIAPI
TimerSleep_GetNextMatchVal (
  VOID
  )
{
  return ArmArchTimerGetCompareVal ();
}

/**
  Programs the next match value.
  Sleep module needs to call this api when system comes out of LPM. 
  It will need to return the same value that it obtained by using TimerSleep_GetNextMatchVal()

  @param  Match Value that is provided by TimerSleep_GetNextMatchVal.

  @return void.

**/
VOID
EFIAPI
TimerSleep_SetNextMatchVal (
  UINT64 Val
  )
{
  ArmArchTimerSetCompareVal (Val);
  ArmArchTimerEnableTimer ();
  return;
}
