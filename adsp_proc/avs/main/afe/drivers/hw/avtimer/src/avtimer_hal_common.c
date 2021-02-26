/*========================================================================
avtimer.c

This file contains avtimer hardware abstraction layer.

Copyright (c) 2010-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/src/avtimer_hal_common.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "avtimer_hal.h"
#include "avtimer_hal_v2.h"

#include "qurt_elite.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

static uint32   avtimer_hw_version = 0;

avtimer_hal_func_def_t* avtimer_hal_func_def_ptr_g = NULL;


/**
Initialize avtimer hw. Register q6ss_sirc0 interrupt(int 23).
 * @param[out] version Version of the implementation of Audio HAL.
 * @param[in]  avtimer_base_addr Base virtual address of the avtimer registes.
 * @param[in]  l2vic_base_addr Base virtual address of the L2VIC registes.
 * @param[in]  lcc_base_addr Base virtual address of the LCC registes.


@return
 ADSPResult error code.
*/
ADSPResult hal_avtimer_hw_init(avtimer_hw_cfg_t *avt_hw_cfg)
{
   avtimer_hw_version = avt_hw_cfg->hw_revision;
   avtimer_hal_func_def_ptr_g = (avtimer_hal_func_def_t*)avt_hw_cfg->avtimer_hal_func_def_ptr;

   switch(avtimer_hw_version)
   {
      case AVTIMER_HW_VERSION_2:
      default:
      {
         hal_avtimer_hw_init_v2(avt_hw_cfg);

         avtimer_hal_func_def_ptr_g->hal_avtimer_hw_enable_f = hal_avtimer_hw_enable_v2;
         avtimer_hal_func_def_ptr_g->hal_avtimer_hw_disable_f = hal_avtimer_hw_disable_v2;
         avtimer_hal_func_def_ptr_g->hal_avtimer_prog_hw_timer_int0_f = hal_avtimer_prog_hw_timer_int0_v2;
         avtimer_hal_func_def_ptr_g->hal_avtimer_prog_hw_timer_int1_f = hal_avtimer_prog_hw_timer_int1_v2;
         avtimer_hal_func_def_ptr_g->hal_avtimer_prog_hw_timer_resmod_f = hal_avtimer_prog_hw_timer_resmod_v2;
         avtimer_hal_func_def_ptr_g->hal_avtimer_get_current_hw_timer_tick_f = hal_avtimer_get_current_hw_timer_tick_v2;
         avtimer_hal_func_def_ptr_g->hal_avtimer_latch_av_qtimer_tick_f = hal_avtimer_latch_av_qtimer_tick_v2;
         avtimer_hal_func_def_ptr_g->hal_avtimer_convert_hw_tick_to_time_func_f = hal_avtimer_convert_hw_tick_to_time_v2;
      }
      break;
   }

   return ADSP_EOK;
}

