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

static UINT32 isTimerInitialized = 0;

/**
  Stalls the CPU for at least the given number of microseconds.

  Stalls the CPU for the number of microseconds specified by MicroSeconds.

  @param  MicroSeconds  The minimum number of microseconds to delay.

  @return The value of MicroSeconds inputted.

**/
UINTN
EFIAPI
MicroSecondDelay (
  IN  UINTN MicroSeconds
  )
{
  UINT64  NanoSeconds;
  
  NanoSeconds = MultU64x32(MicroSeconds, 1000);

  while (NanoSeconds > (UINTN)-1) { 
    NanoSecondDelay((UINTN)-1);
    NanoSeconds -= (UINTN)-1;
  }

  NanoSecondDelay(NanoSeconds);

  return MicroSeconds;
}

/**
  Stalls the CPU for at least the given number of nanoseconds.

  Stalls the CPU for the number of nanoseconds specified by NanoSeconds.

  @param  NanoSeconds The minimum number of nanoseconds to delay.

  @return The value of NanoSeconds inputted.

**/
UINTN
EFIAPI
NanoSecondDelay (
  IN  UINTN NanoSeconds
  )
{
  UINT64 StartTime;
  UINT64 CurrentTime;
  UINT64 ElapsedTime;
  UINT64 Delay64;
  UINT32 TimetickFreq;
  UINTN InterruptTimerIndex = PcdGet32(PcdInterruptTimerIndex);

  if (NanoSeconds < 25) {
      return 0;
  }

  if(isTimerInitialized == 0) {
    isTimerInitialized++;
    TimerInit(InterruptTimerIndex);
  }

  (void)Timetick_GetFreq((Timetick_timer_Type)InterruptTimerIndex, &TimetickFreq);
  
  Delay64 = MultU64x32(NanoSeconds, TimetickFreq);
  Delay64 = DivU64x32(Delay64, CONVERSION_NS_TO_SEC);
  
  // Take initial reading
  Timetick_GetCount((Timetick_timer_Type)InterruptTimerIndex, &StartTime);
  
  do 
  {
    Timetick_GetCount((Timetick_timer_Type)InterruptTimerIndex, &CurrentTime);
    ElapsedTime = CurrentTime - StartTime;
  } while (ElapsedTime < Delay64);

  Delay64 = MultU64x32((UINT64)ElapsedTime, CONVERSION_NS_TO_SEC);
  NanoSeconds = DivU64x32(Delay64, TimetickFreq);

  return NanoSeconds;
}

/**
  Retrieves the current value of a 64-bit free running performance counter.

  The counter can either count up by 1 or count down by 1. If the physical
  performance counter counts by a larger increment, then the counter values
  must be translated. The properties of the counter can be retrieved from
  GetPerformanceCounterProperties().

  @return The current value of the free running performance counter.

**/
UINT64
EFIAPI
GetPerformanceCounter (
  VOID
  )
{ 
  UINT64 CurrentCount;
  Timetick_Init();
  Timetick_GetCount((Timetick_timer_Type)PcdGet32(PcdPerformanceCounterIndex), &CurrentCount);
  return CurrentCount;
}

/**
  Retrieves the 64-bit frequency in Hz and the range of performance counter
  values.

  If StartValue is not NULL, then the value that the performance counter starts
  with immediately after is it rolls over is returned in StartValue. If
  EndValue is not NULL, then the value that the performance counter end with
  immediately before it rolls over is returned in EndValue. The 64-bit
  frequency of the performance counter in Hz is always returned. If StartValue
  is less than EndValue, then the performance counter counts up. If StartValue
  is greater than EndValue, then the performance counter counts down. For
  example, a 64-bit free running counter that counts up would have a StartValue
  of 0 and an EndValue of 0xFFFFFFFFFFFFFFFF. A 24-bit free running counter
  that counts down would have a StartValue of 0xFFFFFF and an EndValue of 0.

  @param  StartValue  The value the performance counter starts with when it
                      rolls over.
  @param  EndValue    The value that the performance counter ends with before
                      it rolls over.

  @return The frequency in Hz.

**/
UINT64
EFIAPI
GetPerformanceCounterProperties (
  OUT UINT64  *StartValue,  OPTIONAL
  OUT UINT64  *EndValue     OPTIONAL
  )
{
  UINT32 TimetickFreq;
  Timetick_Init();
  if (StartValue != NULL) {
    *StartValue = PcdGet64(PcdPerfomanceCounterStart);
  }
  
  if (EndValue != NULL) {
    *EndValue = PcdGet64(PcdPerfomanceCounterEnd);
  }
  Timetick_GetFreq((Timetick_timer_Type)PcdGet32(PcdPerformanceCounterIndex), &TimetickFreq);
  return (UINT64) TimetickFreq;
}


