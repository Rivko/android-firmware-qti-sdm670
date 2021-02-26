#ifndef QSEE_SYNC_H
#define QSEE_SYNC_H

/**
@file qsee_sync.h
@brief Provide API wrappers for synchronization functions.
*/

/*============================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_sync.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $


when         who       what, where, why
-----------  --------  --------------------------------------------------------
2015-Aug-18  cmihalik  Initial revision.

=========================================================================== */

#include <stdint.h>

/**
 * @brief Perform a busy wait (spin) for the input number of microseconds.
 *
 * @param[in] timeout_us - number of microseconds to wait.
 *
 * @return O on success, nonzero otherwise.
 */

int qsee_spin( uint32_t timeout_us );

#endif

