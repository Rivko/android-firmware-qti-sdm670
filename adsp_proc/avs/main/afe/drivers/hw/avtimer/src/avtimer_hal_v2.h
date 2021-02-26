/*========================================================================
Copyright (c) 2010-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/src/avtimer_hal_v2.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.
========================================================================== */
/**
@file avtimer_hal.h

@brief This file contains avtimer hardware abstraction layer.
*/

#ifndef AVTIMER_HAL_V2_H__
#define AVTIMER_HAL_V2_H__

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "adsp_error_codes.h"
#include "common.h"
#include "avtimer_hal.h"
#include <stdint.h>    //for uint32_t etc. define

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/**
Initialize avtimer hw. Register q6ss_sirc0 interrupt(int 23).
 * @param[out] version Version of the implementation of Audio HAL.
 * @param[in]  avtimer_base_addr Base virtual address of the avtimer registes.
 * @param[in]  l2vic_base_addr Base virtual address of the L2VIC registes.
 * @param[in]  lcc_base_addr Base virtual address of the LCC registes.


@return
 ADSPResult error code.
*/
ADSPResult hal_avtimer_hw_init_v2(avtimer_hw_cfg_t *avt_hw_cfg);

ADSPResult hal_avtimer_hw_enable_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode);

/**
Deinitialize avtimer hw. 

@return
 ADSPResult error code.
*/
ADSPResult hal_avtimer_hw_disable_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode);


/**
program int0(avtimer_int0) with the match value. Interrupt(Int0)
will be generated when the timer count matches the 
int0_match_value 

@param[in] int0_match_value match value @ which int0 will be 
 	  generated

@return
 none.
*/
void hal_avtimer_prog_hw_timer_int0_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t int0_match_value);

/**
program int1(avtimer_int1) with the match value. Interrupt(Int1)
will be generated when the timer count matches the 
int1_match_value 

@param[in] int1_match_value match value @ which int1 will be 
 	  generated

@return
 none.
*/
void hal_avtimer_prog_hw_timer_int1_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t int1_match_value);


/**
program resmod(avtimer_resmod) with the resmod_value. Whenever 
the resolution counter go past the resmod_value, timer count is 
incremented by 1. 
 
@param[in] resmod_value value for which timer count is 
      incremented by 1.

@return
 none.
*/
void hal_avtimer_prog_hw_timer_resmod_v2(avtimer_hw_index idx, uint32_t resmod_value);



/**
program hw timer tick value. 

@param[in] tick_value tick value to which hw timer is readjusted

@return
 none.
*/
void hal_avtimer_prog_hw_timer_tick_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t tick_value);



/**
get hw timer tick current value 
 
 
@return
 returns current timer count value.
*/
uint64_t hal_avtimer_get_current_hw_timer_tick_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode);

/**
Converts hw tick count to time

@return
 returns time.
*/
uint64_t hal_avtimer_convert_hw_tick_to_time_v2(avtimer_hw_mode hw_mode, uint64_t tick_count);

/**
latches avtimer and qtimer timestamps together

@return
 returns ADSPResult
*/
ADSPResult hal_avtimer_latch_av_qtimer_tick_v2(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t *avtimer_ts,
                                               uint64_t *qtimer_ts);

/** @} */ /* end_addtogroup avtimer_hal */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AVTIMER_HAL_V2_H__

