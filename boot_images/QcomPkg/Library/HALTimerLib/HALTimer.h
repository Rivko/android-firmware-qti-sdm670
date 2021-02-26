#ifndef HAL_TIMER_H
#define HAL_TIMER_H
/*
===========================================================================

FILE:         HALTimer.h

DESCRIPTION:  
  This is the hardware abstraction layer interface for the timetick module.
  This file is supposed to be used by the timetick driver alone.

===========================================================================
             Copyright (c) 2011-12 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/18/12   pbi      Added a new function, HAL_timer_WriteMatchValue
 03/15/11   pbi      Added a new function, HAL_timer_GetDivider
 02/02/11   pbi      Added support for GPT1, Added a new function, 
                     HAL_timer_ClearOnMatchEnable

=============================================================================*/


/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */

#include <HALcomdef.h>


/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/* Number of timers handled by the timer hal */
   

/*
 * HAL_timer_Type
 *
 * List of all timers.
 */
typedef enum
{
  HAL_TIMER_GPT0                 = 0,
  HAL_TIMER_ADGT                 = 1,
  HAL_TIMER_SLEEP_XTAL_TIMETICK  = 2,
  HAL_TIMER_SLEEP_XTAL_TIMER_0   = 3,
  HAL_TIMER_CCU_TMR1             = 4,
  HAL_TIMER_GPT1                 = 5,
  HAL_NUM_TIMERS,
  HAL_ENUM_32BITS(TIMER_TYPE)

} HAL_timer_Type;

/*
 * HAL_timer_InfoType
 *
 * Structure holding timer specific information.
 */
typedef struct
{
  uint32                        nEnAddress;
  uint32                        nEnShift;
  uint32                        nEnClearOnMatchShift;
  uint32                        nCountValAddress;
  uint32                        nMatchValAddress;
  uint32                        nClearAddress;
  uint32                        nClkCntlAddress;
  uint32                        nClkCntlDivMask;
  uint32                        nClkCntlDivShift;
  void (*setMatchValueFunc)(uint32, uint32, uint32, uint32*);
} HAL_timer_InfoType;

/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

void HAL_timer_Init(void);

void HAL_timer_Enable (HAL_timer_Type, boolean);

void HAL_timer_ClearOnMatchEnable (HAL_timer_Type, boolean);

void HAL_timer_ReadCount (HAL_timer_Type, uint32*);

void HAL_timer_SetMatchValue (HAL_timer_Type, uint32 , uint32 , uint32 , uint32*);

void HAL_timer_ReadMatchValue (HAL_timer_Type, uint32*);

void HAL_timer_Clear (HAL_timer_Type);

void HAL_timer_GetDivider (HAL_timer_Type, uint32*);

void HAL_timer_InitPlatform(HAL_timer_InfoType ** );

/*
===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================

HAL_timer_Init( )

Description:
   This function initializes all the timers in the platform.

Parameter:
    None.
Return:
   None.

===============================================================================
HAL_timer_Enable( )

Description:
   This function is used to enable/disable a particular timer.

Parameter:
    nTimer - The timer to be enabled.
    bEn    - Enable/Disable bit for the timer
Return:
   None.

=============================================================================== 
HAL_timer_ClearOnMatchEnable( )

Description:
   This function is used to enable/disable the clear on match feature of the timer.

Parameter:
    nTimer - The timer to be enabled.
    bEn    - Enable/Disable bit for clear on match.
Return:
   None.
 
=============================================================================== 
HAL_timer_ReadCount( )

Description:
   This function is used to read the specified timer's count value.

Parameter:
    nTimer - The timer to be read.
   *nCount - pointer to counter value.

===============================================================================
HAL_timer_SetMatchValue( )

Description:
   This function is used to set the timer match value register for the next
   timer interrupt.

Parameters:
   nTimer               - The timer to be set.
   nMatchCount          - Slow clock count at which next interrupt will occur.
   nTicksNow            - Current slow clock count.
   nMinRearmSclk        - Minimum time till next scheduled timer interrupt.
   *pnMatchValWritten   - Pointer to the Match Value written
Return:
   match value set.

===============================================================================
HAL_timer_ReadMatchValue( )

Description:
   This function is used to read the timer match value register.

Parameters:
   nTimer          - The timer to be read.
   *pnMatchValue   - Pointer to the Match Value read
Return:
   match value read.

===============================================================================
HAL_timer_Clear( )

Description:
   This function is used to clear the timer count register.

Parameters:
   nTimer - The timer to be cleared.

Return:
   None.

===============================================================================
HAL_timer_GetDivider( )

Description:
   This function is used to read the clk divider for the timer.

Parameters:
   nTimer - The timer to be read.
   nDivider - Pointer to the divider value returned.

Return:
   None.

=============================================================================== 
 
HAL_timer_WriteMatchValue( )

Description:
   This function is used to write the match value register.

Parameters:
   nTimer - The timer to be read.
   pnMatchValue - Match value to be written.

Return:
   None.

=============================================================================== 

HAL_timer_InitPlatform( )

Description:
   This function is used to initialize the timer platform.
   This is specific to the processor.

Parameter:
    **pTimerInfo : Array of pointers to Timer info Structures
Return:
   None.

===============================================================================
*/
#endif /* HAL_TIMER_H */
