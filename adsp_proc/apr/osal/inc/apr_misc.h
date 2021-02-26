#ifndef __APR_MISC_H__
#define __APR_MISC_H__

/*
  Copyright (C) 2009 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/inc/apr_misc.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

/*******************************************************************************
 * Core Routines                                                               *
 ******************************************************************************/
/**
 * Go to sleep for the given time.
 *
 * \param[in] time_ns Value in nano seconds.
 *
 * \return
 * APR_EOK -- Success.
 *
 * \dependencies
 * None.
 */
APR_INTERNAL int32_t apr_misc_sleep ( uint64_t time_ns );

/**
 * Get system level clock ticks which are in sync across all cores.
 *
 * \return
 * Tick value in 64-bit format.
 *
 * \dependencies
 * None.
 */
APR_INTERNAL uint64_t apr_get_clock_ticks ( void );

/**
 * Get system level clock info in micro seconds which are in sync across all
 * cores.
 *
 * \return
 * Micro seconds in 64-bit format.
 *
 * \dependencies
 * None.
 */
APR_INTERNAL uint64_t apr_get_clock_in_us ( void );

/** @} */ /* end_addtogroup apr_osal */
#endif /* __APR_MISC_H__ */

