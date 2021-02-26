#ifndef FLASH_PROFILE_SFWK_H
#define FLASH_PROFILE_SFWK_H

/**********************************************************************
 * flash_profile_sfwk.h  
 *
 * Flash stats framework header. The Babylon  and the stats framework
 * feature helps to do profiling of flash driver at runtime.
 * The babylon feature uses the  TIMETEST debug
 * port on the SURF to send data from different marking points
 * (indicates start/end of an operation) in driver to the PC through
 * Babylon hardware.This captured data is dumped to a PC for decoding
 * and analysis later.
 * The software profiling framework uses the timetick api's to measure the
 * time spent in an operation and records the min/max and total times along with
 * coarse histogram information .
 *
 * Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who     what, where, why
--------     ---     ----------------------------------------------------------
* 01/03/12     sv      Cleanup and enable flash stats diag support
* 10/27/11     sb      Fixing Compiler warnings
* 05/17/11     bb/sv   Optimization Flash driver
* 09/17/10     sv      Fix compilation issue in SPI NOR driver
* 08/11/10     nr      Initial version
===========================================================================*/
#ifndef FLASH_PROFILING_ENABLED
#define FLASH_PROFILING_ENABLED
#endif

#ifdef FLASH_PROFILING_ENABLED

#include "flash_profile_common.h"

#ifndef PACKED_POST
  /* empty define */
  #define PACKED_POST
  #define PACKED
#endif

#define FLASH_STATS_NUM_HISTOGRAM_BUCKETS 20

enum flash_stats_switch_state{
  FLASH_SFWK_OFF = 0,          /* Turn off sfwk profiling */
  FLASH_SFWK_ON  = 1,          /* Turn on sfwk profiling  */
  FLASH_SFWK_RESET = 2         /* Reset sfwk data */
};

/*============================================================================
 *
 *             F L A S H    B AB Y L O N    C O N F I G S
 *
 ============================================================================*/
PACKED struct PACKED_POST flash_stats_params
{
  uint32 operation_count; /* Counter for total num of ops */
  uint32 data_transfer;   /* records total data transfered */
  uint32 time_elapsed;    /* Records total time elapsed in microsecs */
  uint32 start_time;      /* records start time of the last operation of this type */
  uint32 max_time;        /* records max time (multiplied by 1024) */
  uint32 min_time;        /* records min time(multiplied by a 1024) */
  uint32 npages;          /* records num of pages in last op of this type */
  uint32 histogram[FLASH_STATS_NUM_HISTOGRAM_BUCKETS]; /* Buckets for histogram data */
};

PACKED struct PACKED_POST flash_api_stats
{
   uint32 page_size;    /* stores total page size */
   uint32 main_size;    /* stores main size */
   uint32 spare_size;   /* stores spare size */
   uint32 min_range;    /* stores min range or min of 1st bucket */
   uint32 bucket_size;  /* stores bucket size */
   uint32 third_bucket_min_range; /* stores the min range of the third bucket */
   struct  flash_stats_params api[FLASH_PROFILE_FRAMEWORK_MAX]; /* one per type
                                                              supported */
};

struct flash_stats_synclocks
{
  DALSYS_SYNC_OBJECT(stats_framewrk_sync_object); /* Synchronizaton object */
  DALSYSSyncHandle handle_stats_framewrk_sync;    /* Handle to sync object */
  uint32 sync_object_initialized;
};

/*Initialises the data structures*/
void flash_stats_init(void);

/* Marks the start of the software framework */
void flash_stats_start(uint8 id, uint32 npages);

/* Marks the end of the software framework */
void flash_stats_end(uint8 id);

/* populates and stores the page size and other info */
void flash_stats_set_info (flash_client_ctxt *client_ctxt );

/* Sync locking for multipage support */
void flash_stats_lock(void);

/* Sync unlocking for multipage support */
void flash_stats_unlock(void);

/* Set the flash stats diag packet state */
void flash_stats_diag_pkt_state(uint32 state);

/* Get the flash stats data */
void flash_stats_diag_pkt_get_data(PACKED struct flash_api_stats *rsp_pkt);

#endif /* #ifdef FLASH_PROFILING_ENABLED */
#endif /* FLASH_PROFILE_SFWK_H */
