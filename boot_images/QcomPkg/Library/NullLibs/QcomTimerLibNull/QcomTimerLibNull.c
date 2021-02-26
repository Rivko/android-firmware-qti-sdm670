/** @file
  Timer Library Null - stubs

  Copyright (c) 2013, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/12/13   vk      Initial revision.

=============================================================================*/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/QcomTimerLib.h>
#include <Library/TimerLib.h>


EFI_STATUS
SetTimerPeriod (
  IN EFI_TIMER_ARCH_PROTOCOL  *This,
  IN UINTN                    TimerIndex,
  IN UINT64                   TimerPeriod
  )
{
  return EFI_SUCCESS;
}

UINTN
InterruptVectorForTimer (
  IN  UINTN TimerIndex
  )
{
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
  return;
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
  return EFI_SUCCESS;
}