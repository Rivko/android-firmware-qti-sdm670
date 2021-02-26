#ifndef _SLEEP_NPA_SCHEDULER_H_
#define _SLEEP_NPA_SCHEDULER_H_
/*==============================================================================
  FILE:         sleep_npa_scheduler.h

  OVERVIEW:     This file contains declarations of functions that are used
                to interface with sleep by the NPA scheduler subsystem.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/inc/sleep_npa_scheduler.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                         EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/** 
 * sleepNPAScheduler_setDeadline
 * 
 * @brief Set the deadline for the NPA scheduler, in absolute ticks.  Sleep
 *        will honor this deadline when choosing low power modes. This function
 *        should NOT be called from the LPR in the sleep context.
 * 
 * @param deadline_abs_ticks:  Next deadline for the NPA scheduler event, in
 *                             absolute ticks.
 *                             NOTE:  The tick here should
 *                             already be adjusted with the
 *                             local delta kept by the timetick
 *                             subsystem.
 */
void sleepNPAScheduler_setDeadline(uint64 deadline_abs_tick);

/**
 * sleepNPAScheduler_getDeadline
 *
 * @brief Returns the NPA scheduler deadline in absolute ticks.
 *
 * @return NPA scheduler deadline in absolute ticks.
 */
uint64 sleepNPAScheduler_getDeadline( void );

/** 
 * sleepNPAScheduler_adjustDeadline 
 *  
 * @brief Adjust the previously set deadline for the NPA scheduler.  This 
 *        function is ONLY meant to be called from the NPA scheduler LPR, when
 *        the deadline needs to be pushed out because of time savings from
 *        the LPR. The new deadline should be expressed here in absolute ticks.
 * 
 * @param deadline_abs_ticks:  Next deadline for the NPA scheduler event, in
 *                             absolute ticks.
 *                             NOTE:  The tick here should
 *                             already be adjusted with the
 *                             local delta kept by the timetick
 *                             subsystem.
 */
void sleepNPAScheduler_adjustDeadline(uint64 deadline_abs_tick);

/** 
 * sleepNPAScheduler_cancelDeadline 
 *  
 * @brief Cancel the deadline for the NPA scheduler, by recording that the 
 *        current deadline is no longer valid.
 */
void sleepNPAScheduler_cancelDeadline(void);

/** 
 * sleepNPAScheduler_init 
 *  
 * @brief Performs any required setup to use the sleep NPA scheduler 
 */
void sleepNPAScheduler_init(void);

#endif /* _SLEEP_NPA_SCHEDULER_H_ */

