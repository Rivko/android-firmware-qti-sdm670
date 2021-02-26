#ifndef __AUD_SYNC_HAL_FUNC_DEFS_H__
#define __AUD_SYNC_HAL_FUNC_DEFS_H__

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

#include "aud_sync_hal.h"


/**
 * resets all read/write registers of aud_sync block
 */
typedef void (*hal_aud_sync_reset_all_func)(void);

/**
 * enables the clocks using LCC_AVT_NS register
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_enable_clocks_func)(hal_aud_sync_intf_t intf);

/**
 * disables the clocks using LCC_AVT_NS register
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_disable_clocks_func)(hal_aud_sync_intf_t intf);

/** 
 * Set the VFR source as specified (external/internal).
 *
 * @param[in] vfr_src VFR source
 */
typedef void (*hal_aud_sync_set_vfr_source_func)(hal_aud_sync_vfr_source_t vfr_src);

/** 
 * Get the VFR source 
 * 
 * @param[out] vfr_src_ptr Pointer to VFR source
 */
typedef void (*hal_aud_sync_get_vfr_source_func)(hal_aud_sync_vfr_source_t *vfr_src_ptr);

/**
 * Adjusts the phase between sample counter and reference counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the phase adjustment
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_adj_phase_func)(hal_aud_sync_intf_t intf, hal_aud_sync_phase_adj_t val, uint32_t *mask, uint32_t *field_value);

/**
 * enable tracking for the interface & also enables the XO div
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_enable_tracking_func)(hal_aud_sync_intf_t intf);

/**
 * disable tracking for the interface & also disables the XO div
 * TODO: if needed we can decouple tracking and XO Div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_disable_tracking_func)(hal_aud_sync_intf_t intf);

/**
 * resets tracking for the interface & also reset the XO div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_reset_tracking_func)(hal_aud_sync_intf_t intf);

/**
 * unresets tracking for the interface & also reset the XO div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_unreset_tracking_func)(hal_aud_sync_intf_t intf);

/**
 * gets the overflow status of the reference counter & sample counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] ref_ovf_status overflow status of the reference counter
 * @param[out] smp_ovf_status overflow status of the sample counter
 */
typedef void (*hal_aud_sync_get_ovf_status_func)(hal_aud_sync_intf_t intf, uint16_t *ref_ovf_status, uint16_t *smp_ovf_status);


/**
 * clears the aud_sync status register
 *
 * @param[in] none
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_clear_status_func)(void);

/**
 * sets the value by which reference count increments
 *
 * @param[in] intf audio sync interface
 * @param[in] ref_cnt_inc  value of ref count increment
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_set_ref_cnt_inc_func)(hal_aud_sync_intf_t intf, uint16_t ref_cnt_inc);

/**
 * sets the value by which crystal oscillator is divided
 *
 * @param[in] intf audio sync interface
 * @param[in] L  cystal is divided by value = L+M/N (where N is 4096). L is the int portion.
 * @param[in] M  cystal is divided by value = L+M/N (where N is 4096). M is the fractional portion.
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_set_xo_div_val_func)(hal_aud_sync_intf_t intf,uint32_t sample_rate);


/**
 * gets the value of the sample counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] sample_cnt  value of the sample counter
 *
 */
typedef void (*hal_aud_sync_get_smp_cnt_func)(hal_aud_sync_intf_t intf, uint32_t *sample_cnt);

/**
 * gets the value of the reference counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] sample_cnt  value of the reference counter
 */
typedef void (*hal_aud_sync_get_ref_cnt_func)(hal_aud_sync_intf_t intf, uint32_t *reference_cnt);

/**
 * gets the value of the VFR timestamp
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] vfr_cnt  value of the VFR timestamp.
 */
typedef void (*hal_aud_sync_get_vfr_cnt_func)(hal_aud_sync_intf_t intf, uint32_t *vfr_cnt);

/**
 * sets the sample counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the sample counter
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_set_smp_cnt_func)(hal_aud_sync_intf_t intf, uint32_t sample_cnt);

/**
 * sets the reference counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the reference counter
 *
 * @param[out] none
 */
typedef void (*hal_aud_sync_set_ref_cnt_func)(hal_aud_sync_intf_t intf, uint32_t reference_cnt);




typedef struct
{
   hal_aud_sync_reset_all_func hal_aud_sync_reset_all_f;
   hal_aud_sync_enable_clocks_func hal_aud_sync_enable_clocks_f;
   hal_aud_sync_disable_clocks_func hal_aud_sync_disable_clocks_f;
   hal_aud_sync_set_vfr_source_func hal_aud_sync_set_vfr_source_f;
   hal_aud_sync_get_vfr_source_func hal_aud_sync_get_vfr_source_f;
   hal_aud_sync_adj_phase_func hal_aud_sync_adj_phase_f;
   hal_aud_sync_enable_tracking_func hal_aud_sync_enable_tracking_f;
   hal_aud_sync_disable_tracking_func hal_aud_sync_disable_tracking_f;
   hal_aud_sync_reset_tracking_func hal_aud_sync_reset_tracking_f;
   hal_aud_sync_unreset_tracking_func hal_aud_sync_unreset_tracking_f;
   hal_aud_sync_get_ovf_status_func hal_aud_sync_get_ovf_status_f;
   hal_aud_sync_clear_status_func hal_aud_sync_clear_status_f;
   hal_aud_sync_set_ref_cnt_inc_func hal_aud_sync_set_ref_cnt_inc_f;
   hal_aud_sync_set_xo_div_val_func hal_aud_sync_set_xo_div_val_f;
   hal_aud_sync_get_smp_cnt_func hal_aud_sync_get_smp_cnt_f;
   hal_aud_sync_get_ref_cnt_func hal_aud_sync_get_ref_cnt_f;
   hal_aud_sync_get_vfr_cnt_func hal_aud_sync_get_vfr_cnt_f;
   hal_aud_sync_set_smp_cnt_func hal_aud_sync_set_smp_cnt_f;
   hal_aud_sync_set_ref_cnt_func hal_aud_sync_set_ref_cnt_f;

} avsync_hal_func_def_t;  

extern avsync_hal_func_def_t* avsync_hal_func_def_ptr_g;

#if defined(__cplusplus)
}
#endif

#endif  // !defined(__AUD_SYNC_HAL_FUNC_DEFS_H__)
