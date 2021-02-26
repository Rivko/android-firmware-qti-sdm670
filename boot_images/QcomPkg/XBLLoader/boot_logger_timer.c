/*=============================================================================

                             boot logger timer  

GENERAL DESCRIPTION
  This file contains the implementation of boot logger timer functions .
  Time unit in boot logger is microsecond(10^-6 second).
  
Copyright 2010, 2015 - 2017 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
when       who        what, where, why
--------   ---        ---------------------------------------------------------
06/02/17   vk         Add DALPLATFORMINFO_TYPE_UNKNOWN for presil
09/19/16   kpa        update boot_get_time_count to grab count once for pre-sil
05/03/16   digants    Made boot_get_time_count non-static and remove pre_sil
11/17/15   kpa        Cleanup code
10/27/15   elt        Added boot_start_time_marker and boot_copy_time_markers. 
                      Modified existing functions to support time markers
10/05/15   ck         Corrected copyright
09/09/10   kpa        For Presilicon do not wait for stable MPM sleep count value
09/27/10   dh         Add meta info structure to boot logger
08/11/10   dh         Initial port from 7x30 
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_msm.h"
#include "boot_target.h"
#include "boot_logger.h"
#include "boot_util.h"
#include "boot_extern_platforminfo_interface.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_get_time_count

** ==========================================================================
*/
/*!
* 
* @brief
*  Returns the current time count.
*
* @retval
*   a 32 bits counter value as the current value in TIMETICK_CLK register.
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_get_time_count()
{
  volatile uint32 curr_count;
  volatile uint32 last_count;

  DalPlatformInfoPlatformType platform;

  // __FIX__SDM845:  substitute non DAL platforminfo api
  platform = boot_DalPlatformInfo_Platform();

  /*Grab current time count*/
  curr_count = HWIO_IN(TIMETICK_CLK);

  /*Keep grabbing the time until a stable count is given
    for non pre-silicon platforms*/

  do 
  {
    last_count = curr_count;
    curr_count = HWIO_IN(TIMETICK_CLK);
  } while(curr_count != last_count && 
          platform != DALPLATFORMINFO_TYPE_RUMI &&
          platform != DALPLATFORMINFO_TYPE_VIRTIO &&
          platform != DALPLATFORMINFO_TYPE_UNKNOWN);

  return curr_count;
}
/* boot_get_time_count*/


/*===========================================================================

**  Function :  boot_init_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  initialize the timer module's meta data
*  This function will also set the internal reference time to be 
*  the value in the old boot logger meta info structure passed by boot_log_init_data,
*  or 0 if this old meta info structure is not given.
*
* @param[in] init_data the boot logger's initialization data
*
* @retval
*  None
* 
* @par Side Effects
*   None
* 
*/
void boot_init_timer(boot_log_init_data *init_data)
{
  struct boot_log_meta_info *log_meta_info_ptr = boot_log_get_meta_info();
  uint32 index = LOG_BASE_TIME_MARKER + 1;
  uint32 num_entries = init_data->time_marker_buffer_size / 
                       sizeof(boot_time_marker_entry);

  if(log_meta_info_ptr != NULL)
  {
    /* Set time marker buffer ptrs and size */
    log_meta_info_ptr->time_marker_buf_start = 
                                            init_data->time_marker_buffer_start;
    log_meta_info_ptr->time_marker_buf_ptr = 
                                           init_data->time_marker_buffer_start +
                                           ((LOG_BASE_TIME_MARKER + 1) *
                                           sizeof(boot_time_marker_entry));
    log_meta_info_ptr->time_marker_buf_size= init_data->time_marker_buffer_size;

    /* initialize time marker buffer entries */
    for(; index < num_entries; ++index)
    {
      /* initialize entry to zero and set next free ptr */
      boot_time_marker_entry *entry = log_meta_info_ptr->time_marker_buf_start + 
                                      index;
      entry->start_time = 0;
      
      if(index < num_entries - 1)
      {
        /* point to next entry */
        entry->next_free_entry = log_meta_info_ptr->time_marker_buf_start + 
                                 index + 
                                 1;
      } else {
        /*  last free entry */
        entry->next_free_entry = NULL;
      }
    } 
    
    /*Set reference time to the old value if the old meta info exists,otherwise to 0*/
    if(init_data->old_meta_info_start != NULL)
    {
      log_meta_info_ptr->ref_time = ((struct boot_log_meta_info *)
                                    (init_data->old_meta_info_start))->ref_time;
    }
    else
    {
      log_meta_info_ptr->ref_time = 0;
    }

    /* Set time marker buffer to initialized */
    log_meta_info_ptr->time_marker_buf_init = TRUE;
  }
}


/*===========================================================================

**  Function :  boot_get_time

** ==========================================================================
*/
/*!
* 
* @brief
*  Returns a timestamp which is the current absolute time since bootup
*  subtracts the reference time.
*   
* @retval
*    Current time in microseconds. 
*    LOG_TIMESTAMP_OVERFLOW if timestamp overflows
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_get_time()
{
  struct boot_log_meta_info *log_meta_info_ptr = boot_log_get_meta_info();
  uint32 curr_time_count = boot_get_time_count();
  
  /* check if the count overflows*/
  if(curr_time_count < LOG_MAX_COUNT_VAL)
  {
    /*Convert current time count into microseconds*/
    curr_time_count = CALCULATE_TIMESTAMP(curr_time_count);
    if(log_meta_info_ptr != NULL)
    {
       curr_time_count -= log_meta_info_ptr->ref_time;
    }
  }
  else
  {
    curr_time_count = LOG_TIMESTAMP_OVERFLOW;
  }
  
  return curr_time_count;
}
/* boot_get_time*/


/*===========================================================================

**  Function :  boot_start_timer

** ==========================================================================
*/
/*!
* 
* @brief
* This function will start the boot_start_timer/boot_stop_timer sequence.
* It will record the time count at the point of this function's execution.
* If multiple starts are called, the timer will reset, throwing away the
* previous start's timestamp.
*
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_start_timer()
{
  struct boot_log_meta_info *log_meta_info_ptr = boot_log_get_meta_info();
  if(log_meta_info_ptr != NULL && 
     log_meta_info_ptr->time_marker_buf_init == TRUE)
  {
    log_meta_info_ptr->time_marker_buf_start[LOG_BASE_TIME_MARKER].start_time = 
                                                         boot_get_time_count();
    
  }
}
/*boot_start_timer*/

/*===========================================================================

**  Function :  boot_start_time_marker

** ==========================================================================
*/
/*!
* 
* @brief
* This function will start the boot_start_timer/boot_stop_timer for an 
* independent time marker.
*   
* @retval
*   Handle to time marker if one is allocated, LOG_TIME_MARKER_OVERFLOW 
*   otherwise
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_start_time_marker(void)
{
  struct boot_log_meta_info *log_meta_info_ptr = boot_log_get_meta_info();
  boot_time_marker_entry *entry = NULL;
  uint32 handle = LOG_TIME_MARKER_OVERFLOW;
  
  if(log_meta_info_ptr != NULL &&
     log_meta_info_ptr->time_marker_buf_init == TRUE &&
     log_meta_info_ptr->time_marker_buf_ptr != NULL)
  {
    /* get next free entry and update meta info */
    handle = log_meta_info_ptr->time_marker_buf_ptr -
             log_meta_info_ptr->time_marker_buf_start;
    entry = log_meta_info_ptr->time_marker_buf_ptr;
    log_meta_info_ptr->time_marker_buf_ptr = entry->next_free_entry;

    /* update new time marker entry */
    entry->start_time = boot_get_time_count();
    entry->next_free_entry = NULL;
  } 

  return handle;
}
/* boot_start_time_marker */

/*===========================================================================

**  Function :  boot_stop_timer

** ==========================================================================
*/
/*!
* 
* @brief
* This function will return the delta time between the time recorded for the
* given time marker handle.
*
* @param[in] handle  handle to time marker to stop
*
* @retval
*   Delta Time between boot_start_timer and boot_stop_timer in microseconds.
*   If time marker had not been started then it will return 0.
*   LOG_TIMESTAMP_OVERFLOW if delta time overflows.
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_stop_timer(uint32 handle)
{
  uint32 start_time = 0;
  uint32 end_time = 0;
  uint32 delta = 0;
  struct boot_log_meta_info *log_meta_info_ptr = boot_log_get_meta_info();
  boot_time_marker_entry *entry = NULL;
  
  /* check buf initialization and time marker is valid */
  if(log_meta_info_ptr != NULL &&
     log_meta_info_ptr->time_marker_buf_init == TRUE &&
     (handle < log_meta_info_ptr->time_marker_buf_size / 
               sizeof(boot_time_marker_entry)) &&
     log_meta_info_ptr->time_marker_buf_start[handle].start_time != 0)
    {
    /* get start and stop times */
    entry = log_meta_info_ptr->time_marker_buf_start + handle;
    start_time = entry->start_time;
      end_time = boot_get_time_count();
    
    /* free time marker and update free ptr*/
    entry->start_time = 0;
    if(handle != LOG_BASE_TIME_MARKER)
    {
      entry->next_free_entry = log_meta_info_ptr->time_marker_buf_ptr;
      log_meta_info_ptr->time_marker_buf_ptr = 
                              log_meta_info_ptr->time_marker_buf_start + handle;
    }

      /*check if both time points do not overflow and end time is bigger than 
        start time*/
    if(start_time < LOG_MAX_COUNT_VAL &&
        end_time < LOG_MAX_COUNT_VAL && 
      end_time >= start_time)
      {
      delta = CALCULATE_TIMESTAMP(end_time - start_time);
      }
      else
      {
        delta = LOG_TIMESTAMP_OVERFLOW;
      }
    }
  return delta;
}
/*boot_stop_timer*/


/*===========================================================================

**  Function :  boot_get_delta_time

** ==========================================================================
*/
/*!
* 
* @brief
* This function will return the delta time between old_time and now 
* in microseconds.
*
* @param[in] old_time the previous timestamp.
*
* @retval
*   Delta time between old_time and current time.
*   LOG_TIMESTAMP_OVERFLOW if delta time overflows.
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_get_delta_time(uint32 old_time)
{
  uint32 delta = LOG_TIMESTAMP_OVERFLOW;
  uint32 curr_time = 0;
  
  if(old_time != LOG_TIMESTAMP_OVERFLOW)
  {
    curr_time = boot_get_time();
    if(curr_time != LOG_TIMESTAMP_OVERFLOW && curr_time >= old_time)
    {
      delta = curr_time - old_time;
    }
  }
  
  return delta;
}
/*boot_get_delta_time*/


/*===========================================================================

**  Function :  boot_copy_time_markers

** ==========================================================================
*/
/*!
* 
* @brief
* Copy and init time marker info in one memory location to some other 
* memory location.
*
* @param[in] dst_meta_info_addr  destination address to save meta info
*
* @param[in] dst_time_marker_buf_addr  destination address to save time markers
*
* @param[in] dst_time_marker_buf_size  size of dest time marker buffer
*
* @param[in] src_meta_info_addr  source address to read meta info
*
* @param[in] src_time_marker_buf_addr  source address to copy time markers from
*
* @param[in] src_time_marker_buf_size  size of src time marker buffer in bytes
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_copy_time_markers
(
    void *dst_meta_info_addr,
    void *dst_time_marker_buf_addr,
    uint32 dst_time_marker_buf_size,
    void *src_meta_info_addr,
    void *src_time_marker_buf_addr,
    uint32 src_time_marker_buf_size
)
{
  boot_time_marker_entry *entry = NULL;
  uint32 num_entries = dst_time_marker_buf_size / 
                       sizeof(boot_time_marker_entry);
  uint32 prev_entries = src_time_marker_buf_size / 
                        sizeof(boot_time_marker_entry);
  uint32 index = 0; 

  struct boot_log_meta_info *dst_meta_info_ptr = 
           (struct boot_log_meta_info *)dst_meta_info_addr;

  struct boot_log_meta_info *src_meta_info_ptr = 
           (struct boot_log_meta_info *)src_meta_info_addr;
           
  BL_VERIFY(dst_time_marker_buf_addr   != NULL &&
            dst_meta_info_ptr != NULL &&
            src_time_marker_buf_addr   != NULL &&
            src_meta_info_ptr != NULL  &&
            src_time_marker_buf_size <= dst_time_marker_buf_size,
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);

  /* copy the time marker buffer to new location */ 
  qmemcpy(dst_time_marker_buf_addr,
          src_time_marker_buf_addr,
          src_time_marker_buf_size);

  /* initialize new entries and fix old ones */
  for(; index < num_entries; ++index)
  {
    /* get next entry */
    entry = (boot_time_marker_entry *)dst_time_marker_buf_addr + index;

    /* check if old or new entry */
    if(index < prev_entries)
    {
      if(entry->next_free_entry != NULL)
      {
        /* update any old entry pointers */
        entry->next_free_entry = (void *) entry->next_free_entry -
                                 src_time_marker_buf_addr +
                                 dst_time_marker_buf_addr;
      }
    }
    else
    {
      /* zero out start time for new entries */
      entry->start_time = 0;

      /* set next free entry pointer */
      if(index == prev_entries)
      {
        /* set first new entry to head of free list */
        dst_meta_info_ptr->time_marker_buf_ptr = entry;
      }

      if(index < num_entries - 1)
      {
        /* if not last entry, set next entry as free */
        entry->next_free_entry = entry + 1;
      }
      else if(src_meta_info_ptr->time_marker_buf_ptr == NULL)
      {
        /* if last new entry, and no existing free entry */
        entry->next_free_entry = NULL;
      }
      else 
      {
        /* if last new entry, and existing free entry */
        entry->next_free_entry = 
                              (void *)src_meta_info_ptr->time_marker_buf_ptr - 
                              (void *)src_meta_info_ptr->time_marker_buf_start +
                              (dst_time_marker_buf_addr);
      }
    }
  }

  /* update the meta info to reflect the new size and start address */
  dst_meta_info_ptr->time_marker_buf_size = dst_time_marker_buf_size;
  dst_meta_info_ptr->time_marker_buf_start = dst_time_marker_buf_addr;
}
/* boot_copy_time_markers */
