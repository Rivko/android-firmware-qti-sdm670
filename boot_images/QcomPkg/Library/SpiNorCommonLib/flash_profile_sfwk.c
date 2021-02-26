/*====================================================================
 *
 * FILE: flash_profile_sfwk.c 
 *
 * SERVICES: Software performace Framework
 *
 * DESCRIPTION: This file would contain the function definitions for 
 * the flash perfomance framework
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/03/11     sv      Update flash profile macros
 * 05/18/11     bb/sv   Optimize flash driver
 * 11/04/10     eo      Reference NOR base parameters for flash_stats
 * 08/27/10     nr      Initial Revision
 *==================================================================*/

/*====================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "flash_profile_sfwk.h"
#include <string.h>

//#include "timetick.h"

/* Defines the minimum range of the framework */
#define FLASH_STATS_MIN_RANGE          200
/* Used to define bucket size as pow(2,shiftwidth) */
#define FLASH_STATS_SHIFT_WIDTH        6
/* shift left to increase resolution before division 10 multiplies by 1024 */
#define FLASH_STATS_RESOLUTION_SHIFT   10
/* last bucket */
#define FLASH_STATS_MAX_BUCKET         (FLASH_STATS_NUM_HISTOGRAM_BUCKETS - 1)
/* Bucket size (64 now) */
#define FLASH_STATS_BUCKET_SIZE        (1 << FLASH_STATS_SHIFT_WIDTH)
/* Min division for 1st bucket */
#define FLASH_STATS_MIN_DIV \
  ((FLASH_STATS_MIN_RANGE-1) >> FLASH_STATS_SHIFT_WIDTH)

struct flash_api_stats  flash_stats;
struct flash_stats_synclocks flash_stats_locks;

/*
 * The purpose of this function is to initialize the parameters in the
 * structurend clear the measure fields thus we use memset to clear all
 * params and initialize the min value
 */
void flash_stats_init(void)
{
  int i;
  /* Set all bits to zero */
  memset(&flash_stats, 0x0, sizeof(flash_stats));
  /* populate the min range */
  flash_stats.min_range = FLASH_STATS_MIN_RANGE;
  flash_stats.bucket_size = FLASH_STATS_BUCKET_SIZE;
  flash_stats.third_bucket_min_range =
    (FLASH_STATS_MIN_DIV + 1) << FLASH_STATS_SHIFT_WIDTH;

  for (i = 0; i < FLASH_PROFILE_FRAMEWORK_MAX; i++)
  {
    /* Ensure a correct min value */
    flash_stats.api[i].min_time = 0xffffffff;
  }

  if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
    &(flash_stats_locks.handle_stats_framewrk_sync),
    &(flash_stats_locks.stats_framewrk_sync_object)))
  {
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
      "Flash_STATS_FRAMEWORK:SyncLock Create Failed!");
  }
}

/* The purpose of this function is to mark the start of evaluation  */
void flash_stats_start(uint8 id, uint32 npages)
{
  if (id < FLASH_PROFILE_FRAMEWORK_MAX)
  {
    flash_stats.api[id].operation_count += npages;
    flash_stats.api[id].npages = npages;
    /* start time tick value */
    flash_stats.api[id].start_time = 0; //timetick_get_safe();
  }
}

/*
 * The purpose of this function is to mark the end of the evaluation and then
 * compute the data transfered and add it to the total along with the computaion
 * of the min and max times and recording of histogram data
 */
void flash_stats_end(uint8 id)
{
  uint32 timeticks, time_diff, bucket, time_diff_temp;

  if (id < FLASH_PROFILE_FRAMEWORK_MAX)
  {
    /* end time tick value */
    timeticks = 0; //timetick_get_safe();
    timeticks -= flash_stats.api[id].start_time;
    time_diff = 0; //timetick_sclk_to_prec_us(timeticks);
    
    flash_stats.api[id].time_elapsed += time_diff;
    /* use time_diff for histogram computation */
    time_diff_temp = time_diff;
    /* shift left to ensure no decimals lost on division */
    time_diff_temp <<= FLASH_STATS_RESOLUTION_SHIFT;
    time_diff_temp /= flash_stats.api[id].npages;

    if (time_diff_temp > flash_stats.api[id].max_time)
    {
      flash_stats.api[id].max_time = time_diff_temp;
    }
    else
    {
      if (time_diff_temp < flash_stats.api[id].min_time )
      {
        flash_stats.api[id].min_time = time_diff_temp;
      }
    }

    /* ignore npages for erase in histogram computation */
    if (id != FLASH_PROFILE_ERASE_BLOCKS)
    {
      time_diff /= flash_stats.api[id].npages;
    }

    /* If below min range then count 1st bucket */
    if (time_diff < FLASH_STATS_MIN_RANGE)
    {
      flash_stats.api[id].histogram[0] += flash_stats.api[id].npages;
    }
    else
    {
      /*  Divide by bucket size and adjust to find the location of bucket
       * in the array as 1st bucket may encompass  more than one buket size
       */
      bucket = time_diff >> FLASH_STATS_SHIFT_WIDTH;
      bucket = bucket-FLASH_STATS_MIN_DIV + 1;

      if (bucket < FLASH_STATS_MAX_BUCKET)
      {
        flash_stats.api[id].histogram[bucket] += flash_stats.api[id].npages;
      }
      else
      {
        /* put the rest in last bucket */
        flash_stats.api[id].histogram[FLASH_STATS_MAX_BUCKET] +=
                                                flash_stats.api[id].npages;
      }
    }
  } /* end if (id < FLASH_PROFILE_FRAMEWORK_MAX) */
}

/* Sets the page size and other info required in post processing */
void flash_stats_set_info(flash_client_ctxt *client_ctxt )
{
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  switch (dev_ctxt->dev_info.common.type)
  {
    case FLASH_NOR:
    case FLASH_NOR_SPI:
      /* page_size and main_size are equivalent for NOR devices */
      flash_stats.page_size = dev_ctxt->dev_info.nor.page_size_bytes;
      flash_stats.main_size = dev_ctxt->dev_info.nor.page_size_bytes;
      break;

    default:
      /* Error Fatal */
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_ERROR,
        "Flash_STATS_FRAMEWORK: Unknown flash type!");
  }
  flash_stats.spare_size = flash_stats.page_size - flash_stats.main_size;

}

/* Sync lock  for multipage support */
void flash_stats_lock(void)
{
  DALSYS_SyncEnter(flash_stats_locks.handle_stats_framewrk_sync);
}

/* Sync lock  for multipage support */
void flash_stats_unlock(void)
{
  DALSYS_SyncLeave(flash_stats_locks.handle_stats_framewrk_sync);
}



