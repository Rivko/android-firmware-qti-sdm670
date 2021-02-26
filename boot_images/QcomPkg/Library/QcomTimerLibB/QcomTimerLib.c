/** @file
  Timer Library

  Copyright (c) 2010-2012, Qualcomm Technologies Inc. All rights reserved.
  Portions Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.

  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/03/12   cpaulo  Changes for B-Family targets
 01/23/12   cpaulo  Using newly API TimetickSetPeriod() to avoid min-rearm in SetPeriod()
 05/18/11   EBR     Fixed GetPerformanceCounter not returning correct time.
 02/04/11   niting  Moved initialization of free running timer to Sec
 01/26/11   niting  Initial revision.

=============================================================================*/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include "TimetickUEFI.h"

#include <Library/QcomTimerLib.h>
#include <Library/TimerLib.h>

#define CONVERSION_100NS_TO_SEC 10000000

#define CONVERSION_NS_TO_SEC 1000000000

#define TIMER_MAX_VAL 0xFFFFFFFFFFFFFFuLL

typedef struct
{
  /* This should only be written to by timer interrupt */
  UINT64 uTimerCountVal;

  /* This should only be written to by timer interrupt */
  UINT64 uTimerMatchVal;

  /* Timer period in 100ns units */
  UINT64 uTimerPeriod;

  /* Timer period in ticks interpretted by timetick driver */
  UINT64 uTimerPeriodTicks;

} TimerInternalStruct;

static TimerInternalStruct TimerInternal =
{
  0,
  0,
  10000000,  /* 10 Seconds */
  0xB71B000uLL /* 10 Seconds in ticks */
};

EFI_STATUS
SetTimerPeriod (
  IN EFI_TIMER_ARCH_PROTOCOL  *This,
  IN UINTN                    TimerIndex,
  IN UINT64                   TimerPeriod
  )
{
    UINT64 TimetickPeriod;
    UINT64 CurrentTimetick;
    UINT32 TimetickFreq;
    UINT64 TimetickMatch;
    UINT64 TimetickMatchSet;
    EFI_STATUS Status;
    Timetick_timer_Type InterruptTimer = (Timetick_timer_Type)TimerIndex;
    
    if (TimerIndex != TIMETICK_QTIMER){
      return EFI_INVALID_PARAMETER;
    }

    if(TimerPeriod == 0) {
      TimerInternal.uTimerPeriodTicks = TIMER_MAX_VAL;
      TimerInternal.uTimerPeriod = TimerPeriod;
      Status = Timetick_GetCount(InterruptTimer, &TimerInternal.uTimerCountVal);
      ASSERT_EFI_ERROR(Status);
      Status = Timetick_SetMatchValue(InterruptTimer, TimerInternal.uTimerPeriodTicks, &TimerInternal.uTimerMatchVal);
      ASSERT_EFI_ERROR(Status);
      return Status;
    }
  
    Status = Timetick_GetFreq(InterruptTimer, &TimetickFreq);
    ASSERT_EFI_ERROR(Status);
    TimetickPeriod = MultU64x32(TimerPeriod, TimetickFreq);
    TimetickPeriod = DivU64x32(TimetickPeriod, CONVERSION_100NS_TO_SEC);

    Status = Timetick_GetCount(InterruptTimer, &CurrentTimetick);
    ASSERT_EFI_ERROR(Status);

    TimetickMatch = CurrentTimetick + TimetickPeriod;

    Status = Timetick_SetMatchValue(InterruptTimer, TimetickMatch, 
                                     &TimetickMatchSet);
    ASSERT_EFI_ERROR(Status);
 
    /* Update Internal Data */   
    TimerInternal.uTimerPeriod = TimerPeriod;
    TimerInternal.uTimerPeriodTicks = TimetickPeriod;
    TimerInternal.uTimerCountVal = CurrentTimetick;
    TimerInternal.uTimerMatchVal = TimetickMatchSet;
 
    ASSERT_EFI_ERROR(Status);
    return Status;
}

UINTN
InterruptVectorForTimer (
  IN  UINTN TimerIndex
  )
{
  if (TimerIndex >= TIMETICK_NUM_TIMERS) {
    ASSERT(FALSE);
    return 0xFFFFFFFF;
  }

  switch(TimerIndex) {
  /* Case 0-2 are legacy indices, they are not supported anymore */
  case 0:
  case 1:
  case 2: 
    break;
  case 3: 
    return PcdGet32(PcdTimer1IRQ);
  }

  return 0;
}


/**
  Initializes and starts timer based on index.

  @param  TimerIndex  Index of timer to initialize.

**/
VOID
TimerInit (
  IN  UINTN TimerIndex
  )
{
  EFI_STATUS Status;

  Status = Timetick_Init();
  ASSERT_EFI_ERROR(Status);

  switch (TimerIndex) {
  case  0: //GPT0 Timer (Timers) -- Obsolete
    break;
  case  2: //DGT Performance Timer (Performance/Delays) -- Obsolete
    break;
  case  3: //QTimer Frame 0
    Timetick_Enable((Timetick_timer_Type)PcdGet32(PcdInterruptTimerIndex),TRUE);
    break;
  default:
    break;
  }
}


/**
  Calls into library from TimerInterruptHandler to update 
  library accordingly. 

  @param  TimerPeriod       Number of 100ns units since last 
                            interrupt.
  @return EFI_DEVICE_ERROR  Returned if library is not 
                            initialized.
  @return EFI_SUCCESS       Returned library successfully 
                            acknowledges the interrupt.
   
**/
EFI_STATUS
TimerLibUpdateInterrupt (
  IN UINT64                   TimerPeriod
  )
{
  /* If the timer hasn't been initialized properly,
     then we should fail this call and not set match val */
  if ( TimerPeriod != TimerInternal.uTimerPeriod ) {
    return EFI_DEVICE_ERROR;
  }

  TimerInternal.uTimerCountVal += TimerInternal.uTimerPeriodTicks;

  Timetick_SetMatchValue(TIMETICK_QTIMER,
                         TimerInternal.uTimerCountVal + TimerInternal.uTimerPeriodTicks, 
                         &TimerInternal.uTimerMatchVal);

  return EFI_SUCCESS;
}
