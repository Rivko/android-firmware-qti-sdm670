/** ======================================================================================
  @file sns_pram.c

  @brief PRAM data buffer manager

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/inc/internal/sns_pram.h#1 $
  $DateTime: 2018/02/01 03:15:18 $
  $Change: 15367748 $
====================================================================================== **/


/**
*****************************************************************************************
                               Includes
*****************************************************************************************
*/
#include <stdbool.h>
#include <stdint.h>
#include "sns_rc.h"
#include "sns_time.h"

/**
*****************************************************************************************
                                  Constants & Macros
*****************************************************************************************
*/


/**
*****************************************************************************************
                                  Structures
*****************************************************************************************
*/
// Opaque memory block structure
typedef struct sns_pram_block_s sns_pram_block_s;

/**
   PRAM regions are handles assigned by the data buffer manager. Each call to
   sns_pram_data_init will initialize a new region */
typedef enum
{
  PRAM_REGION_A,
  PRAM_REGION_MAX
} sns_pram_region_e;

typedef void *sns_pram_cache;

/**
*****************************************************************************************
                                  Public Functions
*****************************************************************************************
*/

/*=============================================================================

FUNCTION sns_pram_init

DESCRIPTION
  Called once to init the circular PRAM buffer for sensor data.

PARAMETERS
  buffer_start_address[i] Address of the start of the PRAM region dedicated to
                          sensor data transfer
  size                [i] Size of the PRAM region for sensor data

DEPENDENCIES

RETURN VALUE 
  the ID assigned to the memory region starting at buffer_start_addr

SIDE EFFECTS
=============================================================================*/
sns_pram_region_e sns_pram_data_init(void * buffer_start_addr, uint32_t size);

/*=============================================================================
FUNCTION sns_pram_cache_region

DESCRIPTION
  Copies PRAM data into local memory for processing. The region may be re-used
  by the SDC after this function returns.

  Call sns_pram_cache_release() when local processing is complete.

PARAMETERS
  region       [i] Memory region on which to operate
  cache_handle [o] Handle to cached data
  start_offset [i] Start offset of data to cache
  end_offset   [i] End offset of data to cache.

RETURN VALUE

SIDE EFFECTS
  Allocates local memory to store the PRAM data.
=============================================================================*/
sns_rc sns_pram_cache_region(sns_pram_region_e region,
                             sns_pram_cache   *cache_handle,
                             int16_t           start_offset,
                             int16_t           end_offset );

/*=============================================================================
FUNCTION sns_pram_cache_release

DESCRIPTION
  Release copy of PRAM data in local memory after processing.

PARAMETERS
  cache_handle [i] Cached region on which to operate

RETURN VALUE

SIDE EFFECTS
=============================================================================*/
sns_rc sns_pram_cache_release(sns_pram_cache cache_handle);

/*=============================================================================
FUNCTION sns_pram_get_next_block

DESCRIPTION
  Returns a pointer to a PRAM block for sending to a physical sensor instance.
  Data may be freed and no longer accessable after the next call to
  sns_pram_get_next_block(), so all returned data should be processed before
  calling this function again.

DEPENDENCIES
  Region must be cached first.

PARAMETERS
  cache_handle  [i] Cached region on which to operate
  sensor_handle [o] index of the sensor which created the data
  timestamp     [o] timestamp assigned to this region.
  data_size     [o] size of data block
  sensor_data   [o] pointer to sensor data

RETURN VALUE
  SNS_RC_FAILED -- when there is no more data in the cache

SIDE EFFECTS
  May allocate additional memory to store the block.
=============================================================================*/
sns_rc sns_pram_get_next_block(sns_pram_cache     cache_handle,
                               uint8_t           *sensor_handle,
                               sns_time          *timestamp,
                               uint16_t          *data_size,
                               uint8_t          **sensor_data);

