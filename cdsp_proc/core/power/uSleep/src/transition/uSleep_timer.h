#ifndef _USLEEP_TIMER_H_
#define _USLEEP_TIMER_H_
/*==============================================================================
  FILE:         uSleep_timer.h

  OVERVIEW:     Internal types and definitions for the handling of the normal
                operational mode timer

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/transition/uSleep_timer.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                            DEFINITIONS & TYPES
 =============================================================================*/
#define DDR_TIMER_HANDLER_STACK_SIZE  768   /* Stack size for the timer handler task */
#define DDR_TIMER_HANDLER_PRIORITY    9     /* Pritory for the task */

/* Signal types for the timer task */
typedef enum uSleep_timer_signals
{
  USLEEP_TIMER_EXIT_EXPIRED     = 0x01, /* Adjusted DDR timer expired */
}uSleep_timer_signals;

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * uSleepTimer_init
 * 
 * @brief Initializes the uSleep timer handler task that will allow uSleep
 *        to exit early enough to service the first non-deferrable DDR timer.
 */
void uSleepTimer_init(void);

/**
 * uSleepTimer_setValue
 * 
 * @brief Adds a non-deferrable uTimer in order to exit uSleep mode in time to 
 *        service the DDR timer.
 *  
 * @param deadline: Absolute timer value in ticks
 */
void uSleepTimer_setValue(uint64 deadline);

/**
 * uSleepTimer_clearTimer 
 *  
 * @brief Clears the uTimer that was created from the normal mode timer.  This 
 *        function should be called on the exit transition. 
 */
void uSleepTimer_clearTimer(void);

/**
 * uSleepTimer_timerSignalTask 
 *  
 * @brief Main uSleep timer task that handles exiting island mode when a DDR 
 *        timer triggers the exit.   
 */
void uSleepTimer_timerSignalTask(void *ignored);

#endif /* _USLEEP_TIMER_H_ */

