/*========================================================================
Copyright (c) 2010-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/inc/avtimer_hal_func_defs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.
========================================================================== */
/**
@file avtimer_hal.h

@brief This file contains avtimer hardware abstraction layer.
*/

#ifndef AVTIMER_HAL_FUNC_DEFS_H__
#define AVTIMER_HAL_FUNC_DEFS_H__

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "avtimer_hal.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
Enable avtimer hw. 

@return
 ADSPResult error code.
*/
typedef ADSPResult (*hal_avtimer_hw_enable_func)(avtimer_hw_index idx, avtimer_hw_mode hw_mode);

/**
Disable avtimer hw. 

@return
 ADSPResult error code.
*/
typedef ADSPResult (*hal_avtimer_hw_disable_func)(avtimer_hw_index idx, avtimer_hw_mode hw_mode);


/**
program int0(avtimer_int0) with the match value. Interrupt(Int0)
will be generated when the timer count matches the 
int0_match_value 

@param[in] int0_match_value match value @ which int0 will be 
 	  generated

@return
 none.
*/
typedef void (*hal_avtimer_prog_hw_timer_int0_func)(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t int0_match_value);

/**
program int1(avtimer_int1) with the match value. Interrupt(Int1)
will be generated when the timer count matches the 
int1_match_value 

@param[in] int1_match_value match value @ which int1 will be 
 	  generated

@return
 none.
*/
typedef void (*hal_avtimer_prog_hw_timer_int1_func)(avtimer_hw_index idx, avtimer_hw_mode hw_mode, uint64_t int1_match_value);


/**
program resmod(avtimer_resmod) with the resmod_value. Whenever 
the resolution counter go past the resmod_value, timer count is 
incremented by 1. 
 
@param[in] resmod_value value for which timer count is 
      incremented by 1.

@return
 none.
*/
typedef void (*hal_avtimer_prog_hw_timer_resmod_func)(avtimer_hw_index idx, uint32_t resmod_value);




/**
get hw timer tick current value 
 
 
@return
 returns current timer count value.
*/
typedef uint64_t (*hal_avtimer_get_current_hw_timer_tick_func)(avtimer_hw_index idx, avtimer_hw_mode hw_mode);

/**
latch av timer and qtimer tick current value together


@return
 returns ADSPResult.
*/
typedef ADSPResult (*hal_avtimer_latch_av_qtimer_tick_func)(avtimer_hw_index idx, avtimer_hw_mode hw_mode,
    uint64_t *avt_ts, uint64_t *qt_ts);

/**
Converts HW timer tick to time

@return
 returns time.
*/
typedef uint64_t (*hal_avtimer_convert_hw_tick_to_time_func)(avtimer_hw_mode hw_mode,uint64_t hw_tick);

typedef struct
{
   hal_avtimer_hw_enable_func hal_avtimer_hw_enable_f;
   hal_avtimer_hw_disable_func hal_avtimer_hw_disable_f;
   hal_avtimer_prog_hw_timer_int0_func hal_avtimer_prog_hw_timer_int0_f;
   hal_avtimer_prog_hw_timer_int1_func hal_avtimer_prog_hw_timer_int1_f;
   hal_avtimer_prog_hw_timer_resmod_func hal_avtimer_prog_hw_timer_resmod_f;
   hal_avtimer_get_current_hw_timer_tick_func hal_avtimer_get_current_hw_timer_tick_f;
   hal_avtimer_latch_av_qtimer_tick_func hal_avtimer_latch_av_qtimer_tick_f;
   hal_avtimer_convert_hw_tick_to_time_func hal_avtimer_convert_hw_tick_to_time_func_f;
} avtimer_hal_func_def_t; 

extern avtimer_hal_func_def_t* avtimer_hal_func_def_ptr_g;

/** @} */ /* end_addtogroup avtimer_hal_func_defs.h */


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AVTIMER_HAL_FUNC_DEFS_H__

