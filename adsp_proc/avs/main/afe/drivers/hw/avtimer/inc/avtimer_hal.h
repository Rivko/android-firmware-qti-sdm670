/*========================================================================
Copyright (c) 2010-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/inc/avtimer_hal.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.
========================================================================== */
/**
@file avtimer_hal.h

@brief This file contains avtimer hardware abstraction layer.
*/

#ifndef AVTIMER_HAL_H
#define AVTIMER_HAL_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "adsp_error_codes.h"
#include "common.h"
#include <stdint.h>    //for uint32_t etc. define
#include "avtimer_hal_devcfg_defs.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define AVTIMER_HW_VERSION_2        0x30040000    //8x62 (avtimer only), 8084 (avtimer + atime), 9x35 (avtimer only),
#define AVTIMER_HW_VERSION_2_1      0x30040001    //8902 (avtimer + atime1 + atime2)

typedef struct
{
   uint32 avtimer_base_phys_addr;
   uint32 avtimer_base_virt_addr;
   /* AVtimer HW instance information*/
   avtimer_instance_info_t avtimer_instance_info[AVTIMER_HW_INSTANCE_INDEX_MAX];
   uint32 audsync_offset;   // aud sync reg offset LPASS_AUD_SYNC_CTL

#ifdef DEBUG_AVTIMER_HAL
   uint32 l2vic_base_virt_addr;
   uint32 lcc_base_virt_addr;
#endif
   uint32 avtimer_l2_int_0_num;
#ifdef DEBUG_AVTIMER_HAL
   uint32 avtimer_l2_int_0_num_in_slice;
#endif
   uint32 hw_revision;

   void* avtimer_hal_func_def_ptr;
   void* avsync_hal_func_def_ptr;
} avtimer_hw_cfg_t;

#include "avtimer_hal_func_defs.h"

/**
Initialize avtimer hw. Register q6ss_sirc0 interrupt(int 23).
 * @param[out] version Version of the implementation of Audio HAL.
 * @param[in]  avtimer_base_addr Base virtual address of the avtimer registes.
 * @param[in]  l2vic_base_addr Base virtual address of the L2VIC registes.
 * @param[in]  lcc_base_addr Base virtual address of the LCC registes.


@return
 ADSPResult error code.
*/
ADSPResult hal_avtimer_hw_init(avtimer_hw_cfg_t *avt_hw_cfg);

/*
* Gets the AVtimer HW mode, based on HW instance index
* @param[in]  hw_index AV timer HW instance index
* @return
*  avtimer_hw_mode  AVtimer HW mode.
*/
avtimer_hw_mode  get_avtimer_hw_instance_mode(avtimer_hw_index hw_index);

static inline ADSPResult hal_avtimer_hw_enable(avtimer_hw_index idx, avtimer_hw_mode hw_mode)
{
   return avtimer_hal_func_def_ptr_g->hal_avtimer_hw_enable_f(idx, hw_mode);
}

/**
Deinitialize avtimer hw.

@return
 ADSPResult error code.
*/
static inline ADSPResult hal_avtimer_hw_disable(avtimer_hw_index idx, avtimer_hw_mode hw_mode)
{
   return avtimer_hal_func_def_ptr_g->hal_avtimer_hw_disable_f(idx, hw_mode);
}

/**
convert the tick count to time in us

@return
 uint64_t time in us
*/
static inline uint64_t hal_avtimer_convert_hw_tick_to_time(avtimer_hw_mode hw_mode, uint64_t  tick_count)
{
return avtimer_hal_func_def_ptr_g->hal_avtimer_convert_hw_tick_to_time_func_f(hw_mode, tick_count);
}

/**
program int0(avtimer_int0) with the match value. Interrupt(Int0)
will be generated when the timer count matches the
int0_match_value

@param[in] int0_match_value match value @ which int0 will be generated
           It is always based on 1 MHz ticks

@return
 none.
*/
static inline void hal_avtimer_prog_hw_timer_int0(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t int0_match_value)
{
   return avtimer_hal_func_def_ptr_g->hal_avtimer_prog_hw_timer_int0_f(idx, hw_mode, int0_match_value);
}

/**
program int1(avtimer_int1) with the match value. Interrupt(Int1)
will be generated when the timer count matches the
int1_match_value

@param[in] int1_match_value match value @ which int1 will be generated
           It is always based on 1 MHz ticks

@return
 none.
*/
static inline void hal_avtimer_prog_hw_timer_int1(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t int1_match_value)
{
   return avtimer_hal_func_def_ptr_g->hal_avtimer_prog_hw_timer_int1_f(idx, hw_mode, int1_match_value);
}

/**
program resmod(avtimer_resmod) with the resmod_value. Whenever
the resolution counter go past the resmod_value, timer count is
incremented by 1.

@param[in] resmod_value value for which timer count is
      incremented by 1.

@return
 none.
*/
static inline void hal_avtimer_prog_hw_timer_resmod(avtimer_hw_index idx, uint32_t resmod_value)
{
   return avtimer_hal_func_def_ptr_g->hal_avtimer_prog_hw_timer_resmod_f(idx, resmod_value);
}


/**
get hw timer tick current value


@return
 returns current timer count value.
*/
static inline uint64_t hal_avtimer_get_current_hw_timer_tick(avtimer_hw_index idx, avtimer_hw_mode hw_mode)
{
   return avtimer_hal_func_def_ptr_g->hal_avtimer_get_current_hw_timer_tick_f(idx, hw_mode);
}

/**
latch avtimer and qtimer tick


@return
 returns current timer count value.
*/
static inline ADSPResult hal_avtimer_latch_av_q_timer_tick(avtimer_hw_index idx, avtimer_hw_mode hw_mode,
                                                         uint64_t *avt_ts, uint64_t *qt_ts)
{
   return avtimer_hal_func_def_ptr_g->hal_avtimer_latch_av_qtimer_tick_f(idx, hw_mode, avt_ts, qt_ts);
}


/** @} */ /* end_addtogroup avtimer_hal */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AVTIMER_HAL_H

