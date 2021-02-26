#ifndef __AUD_SYNC_HAL_V2_H__
#define __AUD_SYNC_HAL_V2_H__

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
#include "aud_sync_hal.h"
#include <stdint.h>    //for uint32_t etc. define

/** 
 * Initialize register to default values and return the version number of the
 * audio HAL. It also sets the base virtual address of the audio hardware
 * registers.
 * 
 * @param[in]  base_ptr Base virtual address of the audio hardware registes.
 * @param[out] version Version of the implementation of Audio HAL.
 */
void hal_aud_sync_init_v2(avtimer_hw_cfg_t *avt_hw_cfg);

/**
 * resets all read/write registers of aud_sync block
 */
void hal_aud_sync_reset_all_v2(void);

/**
 * enables the clocks using LCC_AVT_NS register
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_enable_clocks_v2(hal_aud_sync_intf_t intf);

/**
 * disables the clocks using LCC_AVT_NS register
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_disable_clocks_v2(hal_aud_sync_intf_t intf);

/** 
 * Set the VFR source as specified (external/internal).
 *
 * @param[in] vfr_src VFR source
 */
void hal_aud_sync_set_vfr_source_v2(hal_aud_sync_vfr_source_t vfr_src);

/** 
 * Get the VFR source 
 * 
 * @param[out] vfr_src_ptr Pointer to VFR source
 */
void hal_aud_sync_get_vfr_source_v2(hal_aud_sync_vfr_source_t *vfr_src_ptr);

/**
 * Adjusts the phase between sample counter and reference counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the phase adjustment
 *
 * @param[out] none
 */
void hal_aud_sync_adj_phase_v2(hal_aud_sync_intf_t intf, hal_aud_sync_phase_adj_t val, uint32_t *mask, uint32_t *field_value);

/**
 * enable tracking for the interface & also enables the XO div
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_enable_tracking_v2(hal_aud_sync_intf_t intf);

/**
 * disable tracking for the interface & also disables the XO div
 * TODO: if needed we can decouple tracking and XO Div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_disable_tracking_v2(hal_aud_sync_intf_t intf);

/**
 * resets tracking for the interface & also reset the XO div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_reset_tracking_v2(hal_aud_sync_intf_t intf);

/**
 * unresets tracking for the interface & also reset the XO div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_unreset_tracking_v2(hal_aud_sync_intf_t intf);

/**
 * gets the overflow status of the reference counter & sample counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] ref_ovf_status overflow status of the reference counter
 * @param[out] smp_ovf_status overflow status of the sample counter
 */
void hal_aud_sync_get_ovf_status_v2(hal_aud_sync_intf_t intf, uint16_t *ref_ovf_status, uint16_t *smp_ovf_status);


/**
 * clears the aud_sync status register
 *
 * @param[in] none
 *
 * @param[out] none
 */
void hal_aud_sync_clear_status_v2(void);

/**
 * sets the value by which reference count increments
 *
 * @param[in] intf audio sync interface
 * @param[in] ref_cnt_inc  value of ref count increment
 *
 * @param[out] none
 */
void hal_aud_sync_set_ref_cnt_inc_v2(hal_aud_sync_intf_t intf, uint16_t ref_cnt_inc);

/**
 * sets the value by which crystal oscillator is divided
 *
 * @param[in] intf audio sync interface
 * @param[in] L  cystal is divided by value = L+M/N (where N is 4096). L is the int portion.
 * @param[in] M  cystal is divided by value = L+M/N (where N is 4096). M is the fractional portion.
 *
 * @param[out] none
 */
void hal_aud_sync_set_xo_div_val_v2(hal_aud_sync_intf_t intf,uint32_t sample_rate);


/**
 * gets the value of the sample counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] sample_cnt  value of the sample counter
 *
 */
void hal_aud_sync_get_smp_cnt_v2(hal_aud_sync_intf_t intf, uint32_t *sample_cnt);

/**
 * gets the value of the reference counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] sample_cnt  value of the reference counter
 */
void hal_aud_sync_get_ref_cnt_v2(hal_aud_sync_intf_t intf, uint32_t *reference_cnt);

/**
 * gets the value of the VFR timestamp
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] vfr_cnt  value of the VFR timestamp.
 */
void hal_aud_sync_get_vfr_cnt_v2(hal_aud_sync_intf_t intf, uint32_t *vfr_cnt);

/**
 * sets the sample counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the sample counter
 *
 * @param[out] none
 */
void hal_aud_sync_set_smp_cnt_v2(hal_aud_sync_intf_t intf, uint32_t sample_cnt);

/**
 * sets the reference counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the reference counter
 *
 * @param[out] none
 */
void hal_aud_sync_set_ref_cnt_v2(hal_aud_sync_intf_t intf, uint32_t reference_cnt);

#if defined(__cplusplus)
}
#endif

#endif  // !defined(__AUD_SYNC_HAL_V2_H__)
