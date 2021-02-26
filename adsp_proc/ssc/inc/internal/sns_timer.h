#pragma once

/*======================================================================================
  @file sns_timer.h

  @brief
  Timer for Sensors.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/inc/internal/sns_timer.h#1 $
  $DateTime: 2018/02/01 03:15:18 $
  $Change: 15367748 $
  ======================================================================================*/

/*
*****************************************************************************************
                               Includes
*****************************************************************************************
*/
#include <stdbool.h>
#include <stdint.h>
#include "sns_osa_attr.h"
#include "sns_rc.h"
#include "sns_time.h"

/*
*****************************************************************************************
                               Typedefs
*****************************************************************************************
*/
typedef struct sns_timer      sns_timer;      /**< OS dependent timer */

typedef union
{
  char                      __size[__SIZEOF_ATTR_TIMER]; /**< OS dependent */
  long int                  __alignment;
} sns_timer_attr;

typedef void*  sns_timer_cb_func_arg;
typedef void (*sns_timer_cb_func)(sns_timer_cb_func_arg);

/*
*****************************************************************************************
                               Functions
*****************************************************************************************
*/
/*!
  ---------------------------------------------------------------------------------------
  sns_timer_attr_init
  ---------------------------------------------------------------------------------------

  @brief Initializes the given timer attribute structure with defaults

  @param [io] attrib        the attribute structure

  @return
  SNS_RC_SUCCESS            attribute structure initialized
  SNS_RC_OUTSIDE_RANGE      input parameter is invalid
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_attr_init(
  sns_timer_attr*      attrib);


/*!
  ---------------------------------------------------------------------------------------
  sns_timer_attr_set_periodic
  ---------------------------------------------------------------------------------------

  @brief Specifies whether the timer should be periodic or one-shot

  @param [io] attrib        the attribute structure
  @param [i]  is_periodic   whether the timer should be periodic or one-shot

  @return
  SNS_RC_SUCCESS            attribute set
  SNS_RC_OUTSIDE_RANGE      one or more input parameters are invalid
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_attr_set_periodic(
  sns_timer_attr*       attrib,
  bool                      is_periodic);


/*!
  ---------------------------------------------------------------------------------------
  sns_timer_attr_set_deferrable
  ---------------------------------------------------------------------------------------

  @brief Specifies whether the timer should be deferrable or not

  @param [io] attrib        the attribute structure
  @param [i]  is_deferrable whether the timer should be deferrable or not

  @return
  SNS_RC_SUCCESS            attribute set
  SNS_RC_OUTSIDE_RANGE      one or more input parameters are invalid
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_attr_set_deferrable(
  sns_timer_attr*       attrib,
  bool                      is_deferrable);


/*!
  ---------------------------------------------------------------------------------------
  sns_timer_attr_set_memory_partition
  ---------------------------------------------------------------------------------------

  @brief Specifies the memory type where the timer will be located

  @param [io] attrib        the attribute structure
  @param [i]  mem_type      the memory type

  @return
  SNS_RC_SUCCESS            attribute set
  SNS_RC_OUTSIDE_RANGE      one or more input parameters are invalid
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_attr_set_memory_partition(
  sns_timer_attr*       attrib,
  sns_osa_mem_type          mem_type);


/*!
  ---------------------------------------------------------------------------------------
  sns_timer_create
  ---------------------------------------------------------------------------------------

  @brief Creates a new timer

  @param [i] timer_cb         function called when timer expires
  @param [i] cb_func_arg      input parameter to timer_cb
  @param [i] attrib           the initialized timer attribute structure
  @param [o] timer            destination for the newly created timer

  @return
  SNS_RC_SUCCESS              new timer successfully created
  SNS_RC_RESOURCE_UNAVAIL     no memory
  SNS_RC_OUTSIDE_RANGE        one or more input parameters are invalid
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_create(
  sns_timer_cb_func       timer_cb,
  sns_timer_cb_func_arg   cb_func_arg,
  const sns_timer_attr*   attrib,
  sns_timer**             timer);


/*!
  ---------------------------------------------------------------------------------------
  sns_timer_delete
  ---------------------------------------------------------------------------------------

  @brief Deletes the given timer

  @param [i] timer            the timer

  @return
  SNS_RC_SUCCESS              timer deleted
  SNS_RC_OUTSIDE_RANGE        given timer not found

  @note
  Depending on implementation the timer callback function may or may not be called
  after the timer is deleted.
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_delete(
  sns_timer*              timer);


/*!
  ---------------------------------------------------------------------------------------
  sns_timer_start_relative
  ---------------------------------------------------------------------------------------

  @brief Starts or restart the given timer to expire after the given duration

  @param [i] timer            the timer
  @param [i] duration         the number of time ticks before timer should expire

  @return
  SNS_RC_SUCCESS              timer started
  SNS_RC_OUTSIDE_RANGE        one or more input parameters are invalid
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_start_relative(
  sns_timer*              timer,
  sns_time                    duration_ticks);


/*!
  ---------------------------------------------------------------------------------------
  sns_timer_start_absolute
  ---------------------------------------------------------------------------------------

  @brief Starts or restart the given timer to expire at the given time tick

  @param [i] timer            the timer
  @param [i] expiry           the time tick when timer should expire

  @return
  SNS_RC_SUCCESS              timer started
  SNS_RC_OUTSIDE_RANGE        one or more input parameters are invalid
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_start_absolute(
  sns_timer*              timer,
  sns_time                    expiry);


/*!
  ---------------------------------------------------------------------------------------
  sns_timer_stop
  ---------------------------------------------------------------------------------------

  @brief Stops the given timer and return remaining time if necessary

  @param [i] timer            the timer
  @param [o] time_left        if not NULL, destination for remaining time ticks

  @return
  SNS_RC_SUCCESS              timer stopped
  SNS_RC_OUTSIDE_RANGE        one or more input parameters are invalid
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_stop(
  sns_timer*              timer,
  sns_time*                   time_left);


/*!
  ---------------------------------------------------------------------------------------
  sns_timer_get_duration
  ---------------------------------------------------------------------------------------

  @brief Gets the amount of remaining time before the given timer expires

  @param [i] timer            the timer
  @param [o] time_left        destination for remaining time ticks

  @return
  SNS_RC_SUCCESS              remaining time retrieved
  SNS_RC_OUTSIDE_RANGE        one or more input parameters are invalid
  ---------------------------------------------------------------------------------------
*/
sns_rc sns_timer_get_duration(
  sns_timer*              timer,
  sns_time*                   time_left);

