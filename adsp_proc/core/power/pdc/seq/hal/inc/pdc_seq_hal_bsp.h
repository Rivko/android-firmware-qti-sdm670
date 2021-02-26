#ifndef __PDC_SEQ_HAL_BSP_H__
#define __PDC_SEQ_HAL_BSP_H__
/*==============================================================================
  FILE:         pdc_seq_hal_bsp.h

  OVERVIEW:     This file defines various types for target specific data. Usually they
                are based on underlying HW block.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/seq/hal/inc/pdc_seq_hal_bsp.h#2 $
$DateTime: 2017/10/12 22:35:09 $
==============================================================================*/
#include "comdef.h"
#include "pdc_seq.h"

/*==============================================================================
                               TYPES DEFINITIONS
 =============================================================================*/
/* Structure containing one time configuration of PDC sequencer */
typedef struct pdc_seq_hal_bsp_cfg_s
{
  uint8   *br_addr;          /* Branch addresses for sequences */
  uint32  br_count;          /* Number of branch addresses */
  uint32  *delay;            /* Delays for sequences */
  uint32  delay_count;       /* Number of delays */
} pdc_seq_hal_bsp_cfg_t;

/* Type containing information on a low power mode from PDC sequencer
 * point of view. */
typedef struct pdc_seq_hal_bsp_mode_s
{
  pdc_low_power_mode_type     mode_id;
  int16                       start_addr;          
} pdc_seq_hal_bsp_mode_t;

#endif /* __PDC_SEQ_HAL_BSP_H__ */

