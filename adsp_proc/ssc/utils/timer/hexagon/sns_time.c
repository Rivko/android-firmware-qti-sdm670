/*============================================================================
  @file sns_time.c

  @brief
  Timers and time services for Sensors.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/
#include <stdint.h>
#include "DALSys.h"
#include "qurt.h"
#include "sns_assert.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_time.h"
#include "sns_timer.h"
#include "timer.h"
#include "uTimetick.h"
#include "utimer.h"

/*============================================================================
  Constants
  ============================================================================*/
//#define SNS_TIMER_ENABLE_DEBUG

/*============================================================================
  static data
  ============================================================================*/
static uint32_t global_timer_id SNS_SECTION(".data.sns_time") = 0;
static sns_osa_lock *global_timer_id_mutex SNS_SECTION(".data.sns_time") = 0;
/*============================================================================
  Typedefs
  ============================================================================*/

typedef struct
{
  timer_type timer;
  sns_timer_cb_func callback;
  sns_timer_cb_func_arg args;
  sns_osa_lock *mutex;
  bool is_stopped;
  bool is_periodic;
  sns_osa_mem_type mem_type;
  uint32_t timer_id;
}sns_osa_hexagon_timer;

typedef struct
{
  bool is_periodic;
  bool is_deferrable;
  sns_osa_mem_type mem_type;
}sns_osa_hexagon_timer_attr;

/*============================================================================
  static Data
  ============================================================================*/
timer_group_type  sns_timer_deferrable_group;
timer_group_type  sns_timer_non_deferrable_group;

/*============================================================================
  static Functions
  ============================================================================*/
SNS_SECTION(".text.sns_time") static void sns_timer_generic_cb( void * args)
{
  if( NULL != args )
  {
    sns_osa_hexagon_timer *timer = (sns_osa_hexagon_timer *)args;
    bool is_stopped = true;
    sns_timer_cb_func callback = NULL;
    sns_timer_cb_func_arg args = NULL;
    sns_osa_mem_type mem_type;
    uint32_t timer_id = 0;

    sns_osa_lock_acquire(timer->mutex);
    is_stopped = timer->is_stopped;
    callback = timer->callback;
    args = timer->args;
    timer_id = timer->timer_id;
    mem_type = timer->mem_type;
    sns_osa_lock_release(timer->mutex);
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW,sns_fw_printf, "Timeout for timer with ID: %x, mem_type: %d", timer_id, mem_type);
#endif
    if(!is_stopped)
    {
      callback(args);
    }
    else
    {
      SNS_PRINTF(HIGH,sns_fw_printf,
          "Refusing to call callback of already stopped timer with ID %x",
          timer_id);
    }
  }
  else
  {
    SNS_PRINTF(FATAL,sns_fw_printf, "Timer callback error");
  }
}

static void sns_timer_generic_bimg_cb( void * args)
{
  if( NULL != args )
  {
    sns_osa_hexagon_timer *timer = (sns_osa_hexagon_timer *)args;
    bool is_stopped = true;
    sns_timer_cb_func callback = NULL;
    sns_timer_cb_func_arg args = NULL;
    sns_osa_mem_type mem_type;
    uint32_t timer_id = 0;

    sns_osa_lock_acquire(timer->mutex);
    is_stopped = timer->is_stopped;
    callback = timer->callback;
    args = timer->args;
    timer_id = timer->timer_id;
    mem_type = timer->mem_type;
    sns_osa_lock_release(timer->mutex);
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW,sns_fw_printf, "Timeout for timer with ID: %x, mem_type: %d", timer_id, mem_type);
#endif
    if(!is_stopped)
    {
      callback(args);
    }
    else
    {
      SNS_PRINTF(HIGH,sns_fw_printf,
          "Refusing to call callback of already stopped timer with ID %x",
          timer_id);
    }
  }
  else
  {
    SNS_PRINTF(FATAL,sns_fw_printf, "Timer callback error");
  }
}


/*============================================================================
  Public   Functions
  ============================================================================*/
SNS_SECTION(".text.sns_time") sns_rc sns_timer_attr_init(
  sns_timer_attr*      attrib)
{
  _Static_assert(sizeof(sns_timer_attr) >= sizeof(sns_osa_hexagon_timer_attr),
    "__SIZEOF_ATTR_TIMER is smaller than sizeof(sns_osa_hexagon_timer_attr)");

  if( NULL != attrib )
  {
    sns_osa_hexagon_timer_attr *new_timer_attr = (sns_osa_hexagon_timer_attr *)attrib;
    new_timer_attr->is_periodic = false;
    new_timer_attr->is_deferrable = false;
    new_timer_attr->mem_type = SNS_OSA_MEM_TYPE_NORMAL;
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to initialize timer attributes");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_time") sns_rc sns_timer_attr_set_periodic(
  sns_timer_attr*       attrib,
  bool                      is_periodic)
{
  if(NULL != attrib)
  {
    sns_osa_hexagon_timer_attr *new_timer_attr = (sns_osa_hexagon_timer_attr *) attrib;
    new_timer_attr->is_periodic = is_periodic;
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to set timer periodic attribute");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_time") sns_rc sns_timer_attr_set_deferrable(
  sns_timer_attr*       attrib,
  bool                      is_deferrable)
{
  if(NULL != attrib)
  {
    sns_osa_hexagon_timer_attr *new_timer_attr = (sns_osa_hexagon_timer_attr *) attrib;
    new_timer_attr->is_deferrable = is_deferrable;
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to set timer deferrable attribute");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_time") sns_rc sns_timer_attr_set_memory_partition(
  sns_timer_attr*       attrib,
  sns_osa_mem_type       mem_type)
{
  if(NULL != attrib)
  {
    sns_osa_hexagon_timer_attr *new_timer_attr = (sns_osa_hexagon_timer_attr *) attrib;
    new_timer_attr->mem_type = mem_type;
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to set timer mem_type attribute");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_time") sns_rc sns_timer_create(
  sns_timer_cb_func       timer_cb,
  sns_timer_cb_func_arg   cb_func_arg,
  const sns_timer_attr*   attrib,
  sns_timer**             timer)
{
  if(NULL != attrib && NULL != timer)
  {
    sns_osa_hexagon_timer_attr *new_timer_attr = (sns_osa_hexagon_timer_attr *) attrib;
    uint32_t timer_id = 0;

    sns_osa_lock_acquire(global_timer_id_mutex);
    timer_id = global_timer_id;
    global_timer_id += 1;
    sns_osa_lock_release(global_timer_id_mutex);


    // move to island memory
    sns_osa_lock_attr mutex_attr;
    sns_mem_heap_id timer_mem_source =
      (SNS_OSA_MEM_TYPE_ISLAND == new_timer_attr->mem_type) ? SNS_HEAP_ISLAND:
      SNS_HEAP_MAIN;
    sns_osa_hexagon_timer *new_timer =
      (sns_osa_hexagon_timer *) sns_malloc(timer_mem_source, sizeof(sns_osa_hexagon_timer));
    if(NULL != new_timer)
    {
      sns_osa_lock_attr_init(&mutex_attr);
      sns_osa_lock_attr_set_memory_partition(&mutex_attr, new_timer_attr->mem_type);
      sns_osa_lock_create(&mutex_attr, &new_timer->mutex);
      new_timer->args = cb_func_arg;
      new_timer->callback = timer_cb;
      new_timer->is_stopped = true;
      new_timer->mem_type = new_timer_attr->mem_type;
      new_timer->timer_id = timer_id;
      new_timer->is_periodic = new_timer_attr->is_periodic;

      if( SNS_OSA_MEM_TYPE_ISLAND == new_timer_attr->mem_type )
      {
          // Use the utimer API
          utimer_def_osal(
            (utimer_ptr_type)&new_timer->timer,
            UTIMER_FUNC1_CB_TYPE,
            (timer_t1_cb_type)sns_timer_generic_cb,
            (time_osal_notify_data)new_timer);
#ifdef SNS_TIMER_ENABLE_DEBUG
          SNS_PRINTF(LOW,sns_fw_printf, "Created timer with ID: %x, mem_type: %d",
            new_timer->timer_id, new_timer->mem_type);
#endif
      }
      else
      {
          // Non island timers can be defferable
           timer_group_ptr group_ptr = new_timer_attr->is_deferrable?
             &sns_timer_deferrable_group : &sns_timer_non_deferrable_group;

          timer_def_osal(
            (utimer_ptr_type)&new_timer->timer,
            group_ptr,
            TIMER_FUNC1_CB_TYPE,
            (timer_t1_cb_type)sns_timer_generic_bimg_cb,
            (time_osal_notify_data)new_timer);
#ifdef SNS_TIMER_ENABLE_DEBUG
          SNS_PRINTF(LOW,sns_fw_printf, "Created timer with ID: %x, mem_type: %d",
            new_timer->timer_id, new_timer->mem_type);
#endif
      }
      *timer = (sns_timer *)new_timer;
      return SNS_RC_SUCCESS;
    }
    else
    {
      SNS_PRINTF(ERROR,sns_fw_printf, "Out of memory, can not create timer");
      return SNS_RC_FAILED;
    }
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to create timer");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_time") sns_rc sns_timer_delete(
  sns_timer*              timer)
{
  if(NULL != timer)
  {
    sns_osa_hexagon_timer *this_timer = (sns_osa_hexagon_timer *)timer;
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW,sns_fw_printf, "Deleting timer with ID: %x",
      this_timer->timer_id);
#endif
    sns_osa_lock_acquire(this_timer->mutex);
    this_timer->is_stopped = true;
    if( SNS_OSA_MEM_TYPE_ISLAND == this_timer->mem_type )
    {
      utimer_undef(&this_timer->timer);
    }
    else
    {
      timer_undef(&this_timer->timer);
    }
    sns_osa_lock_release(this_timer->mutex);
    sns_osa_lock_delete(this_timer->mutex);
    sns_free(this_timer);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to delete timer");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_time") sns_rc sns_timer_start_relative(
  sns_timer*              timer,
  sns_time                    duration_ticks)
{
  if(NULL!= timer)
  {
    sns_osa_hexagon_timer *this_timer = (sns_osa_hexagon_timer *)timer;
    time_timetick_type reload_time = this_timer->is_periodic? duration_ticks : 0;
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW, 4,
      "Starting timer with ID: %x, mem_type: %d, duration: %ul, reload time: %ul",
      this_timer->timer_id, this_timer->mem_type, duration_ticks, reload_time);
#endif
    sns_osa_lock_acquire(this_timer->mutex);
    this_timer->is_stopped = false;
    sns_osa_lock_release(this_timer->mutex);

    if( SNS_OSA_MEM_TYPE_ISLAND == this_timer->mem_type )
    {
      utimer_set_64( &this_timer->timer, duration_ticks, reload_time, UT_TICK);
    }
    else
    {
      timer_set_64( &this_timer->timer, duration_ticks, reload_time, T_TICK);
    }
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to start timer");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_time") sns_rc sns_timer_start_absolute(
  sns_timer*              timer,
  sns_time                    expiry)
{
  if(NULL != timer)
  {
    sns_osa_hexagon_timer *this_timer = (sns_osa_hexagon_timer *)timer;
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW,sns_fw_printf,
      "Starting timer with ID: %x, mem_type: %d, expiry: %ul",
      this_timer->timer_id, this_timer->mem_type, expiry);
#endif
    sns_osa_lock_acquire(this_timer->mutex);
    this_timer->is_stopped = false;
    sns_osa_lock_release(this_timer->mutex);

    if( SNS_OSA_MEM_TYPE_ISLAND == this_timer->mem_type )
    {
      utimer_set_absolute( &this_timer->timer, expiry);
    }
    else
    {
      timer_set_absolute( &this_timer->timer, expiry);
    }
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to start timer");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_time") sns_rc sns_timer_stop(
  sns_timer*              timer,
  sns_time*                   time_left)
{
  sns_time remaining = 0;
  if( NULL != timer )
  {
    sns_osa_hexagon_timer *this_timer = (sns_osa_hexagon_timer *)timer;

    sns_osa_lock_acquire(this_timer->mutex);
    this_timer->is_stopped = true;
    sns_osa_lock_release(this_timer->mutex);

    if( SNS_OSA_MEM_TYPE_ISLAND == this_timer->mem_type )
    {
      remaining = (sns_time)utimer_clr_64( &this_timer->timer, UT_TICK);
    }
    else
    {
      remaining = (sns_time)timer_clr_64( &this_timer->timer, T_TICK);
    }

    if((void *)NULL != time_left)
    {
      *time_left = remaining;
    }
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW,sns_fw_printf,
      "Stopped timer with ID: %x, mem_type: %d, time remaining: %ul",
      this_timer->timer_id, this_timer->mem_type, remaining);
#endif
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to stop timer");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_time") sns_rc sns_timer_get_duration(
  sns_timer*              timer,
  sns_time*                   time_left)
{
  if(NULL != timer && NULL != time_left)
  {
    sns_osa_hexagon_timer *this_timer = (sns_osa_hexagon_timer *)timer;
    sns_time remaining;
    if( SNS_OSA_MEM_TYPE_ISLAND == this_timer->mem_type )
    {
      remaining = (sns_time)utimer_get_64( &this_timer->timer, UT_TICK);
    }
    else
    {
      remaining = (sns_time)timer_get_64( &this_timer->timer, T_TICK);
    }
    if((void *)NULL != time_left)
    {
      *time_left = remaining;
    }
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR,sns_fw_printf, "Failed to get timer duration");
  return SNS_RC_NOT_SUPPORTED;
}

void sns_timer_init()
{
  sns_osa_lock_attr mutex_attr;
  sns_osa_lock_attr_init(&mutex_attr);
  sns_osa_lock_attr_set_memory_partition(&mutex_attr, SNS_OSA_MEM_TYPE_ISLAND);
  sns_osa_lock_create(&mutex_attr, &global_timer_id_mutex);

  //timer_init();
  //utimer_init();
  timer_group_enable(&sns_timer_deferrable_group);
  timer_group_enable(&sns_timer_non_deferrable_group);
  timer_group_set_deferrable(&sns_timer_deferrable_group, TRUE);
  timer_group_set_deferrable(&sns_timer_non_deferrable_group, FALSE);
#ifdef SNS_TIMER_ENABLE_DEBUG
  SNS_PRINTF(HIGH,sns_fw_printf, "Timer Init done");
#endif
}

SNS_SECTION(".text.sns_time") sns_time sns_get_system_time(void)
{
#ifdef SNS_USE_LOCAL_CLK_SRC
  return qurt_sysclock_get_hw_ticks();
#else
  return uTimetick_Get();
#endif
}

SNS_SECTION(".text.sns_time") uint64_t sns_get_time_tick_resolution(void)
{
  uint64_t us_for_thousand_ticks;
  us_for_thousand_ticks =  uTimetick_CvtFromTicks(1000, T_USEC);
  // 1000 ticks = x us
  // => 1 tick  = x ns
  return (us_for_thousand_ticks);
}


SNS_SECTION(".text.sns_time") uint64_t sns_get_time_tick_resolution_in_ps(void)
{
  uint64_t us_for_million_ticks;
  us_for_million_ticks =  uTimetick_CvtFromTicks(1000000, T_USEC);
  // 1000000 ticks = x us
  // => 1 tick     = x ps
  return (us_for_million_ticks);
}


SNS_SECTION(".text.sns_time") sns_time sns_convert_ns_to_ticks(uint64_t time_ns)
{
  uint64_t time_us = time_ns/1000;
  return (sns_time)uTimetick_CvtToTicks(time_us, T_USEC);
}

SNS_SECTION(".text.sns_time") sns_rc sns_busy_wait(
  sns_time                  time_ticks)
{
  //TODO: Pull timer_sleep into island mode
  if (SNS_ISLAND_STATE_IN_ISLAND == sns_island_get_island_state())
  {
    //DALSYS_BusyWait(uTimetick_CvtFromTicks(time_ticks, T_USEC));
    SNS_ISLAND_EXIT();
  }

  timer_sleep(time_ticks, T_TICK, TRUE);

  return SNS_RC_SUCCESS;
}
