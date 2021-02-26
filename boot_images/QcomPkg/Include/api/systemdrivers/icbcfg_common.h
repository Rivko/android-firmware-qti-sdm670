#ifndef __ICBCFG_COMMON_H__
#define __ICBCFG_COMMON_H__
/*============================================================================

FILE:      icbcfg_common.h

DESCRIPTION: Common type definitions and includes for icbcfg.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdrivers/icbcfg_common.h#1 $
$DateTime: 2017/09/18 09:26:13 $
$Author: pwbldsvc $
$Change: 14402555 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2016/10/11  sds    Created

         Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                       QUALCOMM Proprietary/GTDR
============================================================================*/
#include "comdef.h"
#include <stdbool.h>

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/

/*============================================================================
                          TYPE DEFINITION
============================================================================*/
/**
 * ICBCFG Error types.
 */ 
typedef enum
{
  ICBCFG_SUCCESS             = 0,          /**< Success */
  ICBCFG_ERROR_INVALID_PARAM,              /**< Invalid Parameters */
  ICBCFG_ERROR_INIT_FAILURE,               /**< Initialization Failure */
  ICBCFG_ERROR_EXHAUSTED_SEGMENTS,         /**< Out of slave segments. */
  ICBCFG_ERROR_UNSUPPORTED,                /**< This operation unsupported on this target. */
  ICBCFG_ERROR_INVALID_ADDRESS,            /**< Unable to find an address translation. */
  /* Add new errors here */         

  ICBCFG_ERROR_SIZE = 0x7FFFFFFF, /* force 32-bit enum */
  ICBCFG_ERROR = -1
}icbcfg_error_type;

/**
 * ICBCFG memory region types
*/
typedef struct
{
  uint64 base_addr;
  uint64 size;
  bool   interleaved;
} region_type;

/* DDR channel definitions */
#define MAX_REGIONS 6
typedef struct
{
  region_type regions[MAX_REGIONS];
} regions_type;

/* DDR slave region configuration */
#define MAX_CHANNELS 4
typedef struct
{
  regions_type channels[MAX_CHANNELS];
} icb_mem_map_type;

typedef struct
{
  uint64 start_addr;
  uint64 end_addr;
} icbcfg_mem_region_type;

#endif /* __ICBCFG_COMMON_H__ */
