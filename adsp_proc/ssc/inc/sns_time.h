#pragma once

/*======================================================================================
  @file sns_time.h

  @brief
  Definitions for System time

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/inc/sns_time.h#1 $
  $DateTime: 2018/02/01 03:15:18 $
  $Change: 15367748 $
  ======================================================================================*/

/*
*****************************************************************************************
                               Includes
*****************************************************************************************
*/
#include <stdint.h>
#include "sns_rc.h"

/*
*****************************************************************************************
                               Typedefs
*****************************************************************************************
*/
typedef uint64_t            sns_time;       /**< time tick */

/*
*****************************************************************************************
                               Functions
*****************************************************************************************
*/
/*!
  ---------------------------------------------------------------------------------------
  sns_get_system_time
  ---------------------------------------------------------------------------------------

  @brief Gets the current system time tick

  @param none

  @return
  current system time tick
  ---------------------------------------------------------------------------------------
*/
sns_time sns_get_system_time(void);


/*!
  ---------------------------------------------------------------------------------------
  sns_get_time_tick_resolution
  ---------------------------------------------------------------------------------------

  @brief Gets number of nanoseconds in one time tick

  @param none

  @return
  number of nanoseconds
  ---------------------------------------------------------------------------------------
*/
uint64_t sns_get_time_tick_resolution(void);

/*!
  ---------------------------------------------------------------------------------------
  sns_get_time_tick_resolution_in_ps
  ---------------------------------------------------------------------------------------

  @brief Gets number of picoseconds in one time tick

  @param none

  @return
  number of picoseconds
  ---------------------------------------------------------------------------------------
*/
uint64_t sns_get_time_tick_resolution_in_ps(void);

/*!
  ---------------------------------------------------------------------------------------
  sns_convert_ns_to_ticks
  ---------------------------------------------------------------------------------------

  @brief Returns the number of ticks equivalent to the give number of nanoseconds

  @param [i] time_ns        time in nanoseconds

  @return
  number of ticks
  ---------------------------------------------------------------------------------------
*/
sns_time sns_convert_ns_to_ticks(
  uint64_t                  time_ns);


/*!
  ---------------------------------------------------------------------------------------
  sns_busy_wait
  ---------------------------------------------------------------------------------------

  @brief Waits for the given number of time ticks

  @param [i] time_ns    wait duration

  @return
  SNS_RC_SUCCESS        the wait duration has passed
  SNS_RC_NOT_SUPPORTED  busy wait is not supported in system
  SNS_RC_OUTSIDE_RANGE  the given wait duration excceds threshold
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_busy_wait(
  sns_time                  time_ticks);
