/** @file QcomTimerLib.h
   
  Timer Library interface header file

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/12/12   cpaulo  Changed name to QcomTimerLib.h
 02/16/11   EBR     Merged in functions from MdePkg\Include\Library\TimerLib.h
 02/07/11   niting  Added TimerInit to be used in SEC and DXE
 01/26/11   niting  Initial revision.

=============================================================================*/

#ifndef __QCOM_TIMER_LIB_H__
#define __QCOM_TIMER_LIB_H__

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Timer.h>

/**
  Sets the period that the periodic timer should fire at.
  
  @param[in] This Pointer to the timer protocol.
  @param[in] TimerIndex  Index of the Timer to use to time the 
                         firings.
  @param[in] TimerPeriod Period of the timer, in 100ns units.
  
  @return
  EFI_STATUS indiciating if the timer or not.
**/
EFI_STATUS
SetTimerPeriod (
  IN EFI_TIMER_ARCH_PROTOCOL  *This,
  IN UINTN                    TimerIndex,
  IN UINT64                   TimerPeriod
  );

/**
  This function returns the vector address for a given timer.

  @param  TimerIndex  QGIC Index for timer selection. 
   
  @retval Timer vector address.
**/
UINTN
InterruptVectorForTimer (
  IN  UINTN TimerIndex
  );

/**
  Initializes and starts timer based on index.

  @param  TimerIndex  Index of timer to initialize.

**/
VOID
TimerInit (
  IN  UINTN TimerIndex
  );

/**
  Calls into library from TimerInterruptHandler to update 
  library accordingly. 

  @param  TimerPeriod       Number of 100ns units since last 
                            interrupt.
  @return EFI_SUCCESS       Returned library successfully 
                            acknowledges the interrupt.
   
**/
EFI_STATUS
TimerLibUpdateInterrupt (
  IN UINT64                   TimerPeriod
  );


#endif /* __QCOM_TIMER_LIB_H__ */
