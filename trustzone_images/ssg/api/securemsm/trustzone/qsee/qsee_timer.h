#ifndef QSEE_TIMER_H
#define QSEE_TIMER_H

/**
@file qsee_timer.h
@brief Provide API wrappers for timer functions
*/

/*============================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_timer.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     --------------------------------------------------------
08/04/11    rv     Initial Revision

=========================================================================== */

/**
 * @brief Get up time from bootup in ms.
 *
 * @param[in] None
 *
 * @return the uptime in ms from system bootup.
 *
 */
unsigned long long qsee_get_uptime(void);

#endif /*QSEE_TIMER_H*/

