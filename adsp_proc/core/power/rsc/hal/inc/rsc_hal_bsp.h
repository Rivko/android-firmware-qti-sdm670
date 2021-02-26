#ifndef _RSC_HAL_BSP_H_
#define _RSC_HAL_BSP_H_
/*==============================================================================
  FILE:         rsc_hal_bsp.h

  OVERVIEW:     This file provides typedefs for the target specific data. Usually
                these types emulate underlying rsc HW block.

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/rsc/hal/inc/rsc_hal_bsp.h#1 $
$DateTime: 2017/10/12 22:35:09 $
==============================================================================*/
#include "comdef.h"

/*========================================================================
 *                                 TYPES
 *=======================================================================*/
/* Type to specify time stamp information of each lpm mode */
typedef struct rsc_hal_time_stamp_info_s
{
  uint64 *ts_val;     /* It is an array of "num_ts_events" */
  uint64 time_slept;  /* ts_val[1] - ts_val[2] */
  uint32 count;       /* How many times this mode is executed succefully */
}rsc_hal_time_stamp_info_t;  

/* Type to specify individual RSC modes */
typedef struct rsc_modes_s
{
  uint16  mode_id;        /* Mode id - usually comes from HPG */ 
  uint16  start_addr;     /* Sequence's start address in rsc */
}rsc_modes_t;

#endif /* _RSC_HAL_BSP_H_ */

