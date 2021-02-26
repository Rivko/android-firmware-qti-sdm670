/*========================================================================
Copyright (c) 2010-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/inc/avtimer_hal_devcfg_defs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/15/17   udayb      Created file.
========================================================================== */
/**
@file avtimer_hal_devcfg_defs.h

@brief This file contains avtimer device configuration related definitions.
*/

#ifndef AVTIMER_HAL_DEVCFG_DEFS_H
#define AVTIMER_HAL_DEVCFG_DEFS_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "adsp_error_codes.h"
#include <stdint.h>    //for uint32_t etc. define

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * For avtimer hw instance define
 * Starting from avtimer hw version 0x30050000, there are two hw instances:
 * I.  U_AVTIMER [generate STC]
 * II. U_A_TIME  [source from pull-able PLL for avsync]
 */
typedef enum
{
   AVTIMER_HW_INSTANCE_INDEX_INVALID = -1,
   AVTIMER_HW_INSTANCE_INDEX_0 = 0,     //U_AVTIMER [generate STC]
   AVTIMER_HW_INSTANCE_INDEX_1,         //U_A_TIME1  [source from pull-able PLL for avsync]
   AVTIMER_HW_INSTANCE_INDEX_2,         //U_A_TIME2  [source from pull-able PLL for avsync, 8092 only]
   AVTIMER_HW_INSTANCE_INDEX_MAX,
} avtimer_hw_index;

/**
 * For avtimer hw instance define
 * Starting from avtimer hw version 0x30050000, there are two hw instances:
 * I.  U_AVTIMER [generate STC]
 * II. U_A_TIME  [source from pull-able PLL for avsync]
 */
typedef enum
{
   AVTIMER_HW_MODE_INVALID = -1,
   AVTIMER_HW_MODE_N_ALPAH = 0,    //Legacy mode
   AVTIMER_HW_MODE_L_MN,
   AVTIMER_HW_MODE_QCOUNTER,
   AVTIMER_HW_MODE_MAX = (0x7FFFFFFF)  /* Maximum value to ensure that the enum is 4 bytes long */
} avtimer_hw_mode;

typedef struct
{
  uint32_t          offset_addr; /* register base address */
  avtimer_hw_mode   mode;        /* AVTimer HW mode*/
}avtimer_instance_info_t;




/** @} */ /* end_addtogroup avtimer_hal */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AVTIMER_HAL_DEVCFG_DEFS_H

