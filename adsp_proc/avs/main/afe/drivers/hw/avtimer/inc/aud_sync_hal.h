#ifndef __AUD_SYNC_HAL_H__
#define __AUD_SYNC_HAL_H__

/** @file aud_sync_hal.h
    This module contains Audio Hardware interface Abstraction layer function 
    declarations. This file publicizes enums defined by the HAL. 

    Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History



when       who     what, where, why
------     ---    -------------------------------------------------------
05/23/11    rb     file creation

========================================================================== */

#if defined(__cplusplus)
extern "C" {
#endif

#include "adsp_error_codes.h"
#include "common.h"
#include "avtimer_hal.h"
#include <stdint.h>    //for uint32_t etc. define

#if defined DEBUG || defined SIM
   #include <assert.h>
   #define AUD_SYNC_ASSERT(x) assert(x)
#else
   #define AUD_SYNC_ASSERT(x)
#endif

/**
 * VFR source can be either internal (MODEM subsystem ) or an external pin
 */
typedef enum hal_aud_sync_vfr_source
{
   INTERNAL = 0,
   EXTERNAL
} hal_aud_sync_vfr_source_t ;

/**
 * audio sync interfaces: BT TX (spkr path: with tracking done wrt to reference and wrt vfr) and
 * FM RX or TX
 */
typedef enum
{
   HAL_AUD_SYNC_INTF_INVALID = -1,
   HAL_AUD_SYNC_INTF_BT_1 = 0, //DPLL1
   HAL_AUD_SYNC_INTF_BT_2,     //DPLL2
   HAL_AUD_SYNC_INTF_FM,
   HAL_AUD_SYNC_INTF_MAX
} hal_aud_sync_intf_t ;

/**
 * value by which phase adjustment is allowed by hardware. value and enum defines should match.
 */
typedef enum
{
   HAL_AUD_SYNC_PHASE_ADJ_VAL_MINUS_ONE = -1,
   HAL_AUD_SYNC_PHASE_ADJ_VAL_ZERO,
   HAL_AUD_SYNC_PHASE_ADJ_VAL_PLUS_ONE,
}hal_aud_sync_phase_adj_t;

/**
 * sampling late index for the L M table value.
 */
typedef enum
{
   INVALID = -1,
   EIGHT_K = 0,
   SIXTEEN_K,
   FOURTY_EIGHT_K
} sampling_rate_t;

#include "aud_sync_hal_func_defs.h"


sampling_rate_t sample_rate_2_index(uint32_t sample_rate);


/** 
 * Initialize register to default values and return the version number of the
 * audio HAL. It also sets the base virtual address of the audio hardware
 * registers.
 * 
 * @param[in]  base_ptr Base virtual address of the audio hardware registes.
 * @param[out] version Version of the implementation of Audio HAL.
 */
void hal_aud_sync_init(avtimer_hw_cfg_t *avt_hw_cfg);

/**
 * resets all read/write registers of aud_sync block
 */
static inline void hal_aud_sync_reset_all(void)
{
   avsync_hal_func_def_ptr_g->hal_aud_sync_reset_all_f();
}

/**
 * enables the clocks using LCC_AVT_NS register
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
static inline void hal_aud_sync_enable_clocks(hal_aud_sync_intf_t intf)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_enable_clocks_f(intf);
}

/**
 * disables the clocks using LCC_AVT_NS register
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
static inline void hal_aud_sync_disable_clocks(hal_aud_sync_intf_t intf)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_disable_clocks_f(intf);
}

/** 
 * Set the VFR source as specified (external/internal).
 *
 * @param[in] vfr_src VFR source
 */
static inline void hal_aud_sync_set_vfr_source(hal_aud_sync_vfr_source_t vfr_src)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_set_vfr_source_f(vfr_src);
}

/** 
 * Get the VFR source 
 * 
 * @param[out] vfr_src_ptr Pointer to VFR source
 */
static inline void hal_aud_sync_get_vfr_source(hal_aud_sync_vfr_source_t *vfr_src_ptr)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_get_vfr_source_f(vfr_src_ptr);
}

/**
 * Adjusts the phase between sample counter and reference counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the phase adjustment
 *
 * @param[out] none
 */
static inline void hal_aud_sync_adj_phase(hal_aud_sync_intf_t intf, hal_aud_sync_phase_adj_t val, uint32_t *mask, uint32_t *field_value)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_adj_phase_f(intf, val, mask, field_value);
}

/**
 * enable tracking for the interface & also enables the XO div
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
static inline void hal_aud_sync_enable_tracking(hal_aud_sync_intf_t intf)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_enable_tracking_f(intf);
}

/**
 * disable tracking for the interface & also disables the XO div
 * TODO: if needed we can decouple tracking and XO Div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
static inline void hal_aud_sync_disable_tracking(hal_aud_sync_intf_t intf)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_disable_tracking_f(intf);
}

/**
 * resets tracking for the interface & also reset the XO div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
static inline void hal_aud_sync_reset_tracking(hal_aud_sync_intf_t intf)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_reset_tracking_f(intf);
}

/**
 * unresets tracking for the interface & also reset the XO div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
static inline void hal_aud_sync_unreset_tracking(hal_aud_sync_intf_t intf)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_unreset_tracking_f(intf);
}

/**
 * gets the overflow status of the reference counter & sample counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] ref_ovf_status overflow status of the reference counter
 * @param[out] smp_ovf_status overflow status of the sample counter
 */
static inline void hal_aud_sync_get_ovf_status(hal_aud_sync_intf_t intf, uint16_t *ref_ovf_status, uint16_t *smp_ovf_status)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_get_ovf_status_f(intf, ref_ovf_status, smp_ovf_status);
}

/**
 * clears the aud_sync status register
 *
 * @param[in] none
 *
 * @param[out] none
 */
static inline void hal_aud_sync_clear_status(void)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_clear_status_f();
}

/**
 * sets the value by which reference count increments
 *
 * @param[in] intf audio sync interface
 * @param[in] ref_cnt_inc  value of ref count increment
 *
 * @param[out] none
 */
static inline void hal_aud_sync_set_ref_cnt_inc(hal_aud_sync_intf_t intf, uint16_t ref_cnt_inc)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_set_ref_cnt_inc_f(intf, ref_cnt_inc);
}

/**
 * sets the value by which crystal oscillator is divided
 *
 * @param[in] intf audio sync interface
 * @param[in] L  cystal is divided by value = L+M/N (where N is 4096). L is the int portion.
 * @param[in] M  cystal is divided by value = L+M/N (where N is 4096). M is the fractional portion.
 *
 * @param[out] none
 */
static inline void hal_aud_sync_set_xo_div_val(hal_aud_sync_intf_t intf,uint32_t sample_rate)
{
   avsync_hal_func_def_ptr_g->hal_aud_sync_set_xo_div_val_f(intf, sample_rate);
}


/**
 * gets the value of the sample counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] sample_cnt  value of the sample counter
 *
 */
static inline void hal_aud_sync_get_smp_cnt(hal_aud_sync_intf_t intf, uint32_t *sample_cnt)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_get_smp_cnt_f(intf, sample_cnt);
}

/**
 * gets the value of the reference counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] sample_cnt  value of the reference counter
 */
static inline void hal_aud_sync_get_ref_cnt(hal_aud_sync_intf_t intf, uint32_t *reference_cnt)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_get_ref_cnt_f(intf, reference_cnt);
}

/**
 * gets the value of the VFR timestamp
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] vfr_cnt  value of the VFR timestamp.
 */
static inline void hal_aud_sync_get_vfr_cnt(hal_aud_sync_intf_t intf, uint32_t *vfr_cnt)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_get_vfr_cnt_f(intf, vfr_cnt);
}

/**
 * sets the sample counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the sample counter
 *
 * @param[out] none
 */
static inline void hal_aud_sync_set_smp_cnt(hal_aud_sync_intf_t intf, uint32_t sample_cnt)
{
    avsync_hal_func_def_ptr_g->hal_aud_sync_set_smp_cnt_f(intf, sample_cnt);
}

/**
 * sets the reference counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the reference counter
 *
 * @param[out] none
 */
static inline void hal_aud_sync_set_ref_cnt(hal_aud_sync_intf_t intf, uint32_t reference_cnt)
{
   avsync_hal_func_def_ptr_g->hal_aud_sync_set_ref_cnt_f(intf, reference_cnt);
}

#if defined(__cplusplus)
}
#endif

#endif  // !defined(__AUD_SYNC_HAL_H__)
