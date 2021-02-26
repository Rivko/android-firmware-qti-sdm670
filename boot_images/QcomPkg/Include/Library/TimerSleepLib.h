#ifndef TIMERSLEEP_H
#define TIMERSLEEP_H

/*==============================================================================

FILE:      TimerSleep.h

DESCRIPTION: Public API interface for the timersleep module.

===========================================================================
             Copyright (c) 2014 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/10/14   ab      Created.

=============================================================================*/
#include <PiDxe.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>

/* ============================================================================
**  Function : TimerSleep_GetNextMatchVal
** ============================================================================
*/
/*!
    @brief
    Obtains the Match Timetick value that is programmed for next timer interrupt.
    Sleep module needs to call this api to get next match value.

    @details
    None

    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval MatchValue.     
    @sa None
*/
UINT64 TimerSleep_GetNextMatchVal(void);


/* ============================================================================
**  Function : TimerSleep_SetNextMatchVal
** ============================================================================
*/
/*!
    @brief
    Sets the next match value with the given timetick

    @details
    Programs the next match value to the timer hw.
    Sleep module needs to call this api when system comes out of LPM. 
    It will need to return the same value that it obtained by using TimerSleep_GetNextMatchVal()

    @param[in] value   timetick match value

    @par Dependencies
    None 
    
    @par Side Effects
    Programs the timetick hw at which interrupt will be generated
    
    @retval None.     
    @sa None
   
*/
void TimerSleep_SetNextMatchVal(UINT64);

#endif /* TIMERSLEEP_H */
