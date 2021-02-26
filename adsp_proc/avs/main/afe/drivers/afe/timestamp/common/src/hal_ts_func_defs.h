/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/common/src/hal_ts_func_defs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _HAL_TS_FUNC_DEFS_H__
#define _HAL_TS_FUNC_DEFS_H__

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "VfrDriverInternal.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/**
  Function for enabling the VFR source driver

  @param[in]  vfr_hw_info_ptr:  pointer to vfr hal info

  @result
  ADSP_EOK, on success, error code on failure

  @dependencies
  None.
*/
typedef void (*ts_hal_init_func_t)(vfr_hw_info_t *vfr_hw_info_ptr);


/**
  Function for enabling the VFR source driver

  @param[in]  afe_port_ptr:  pointer to AFE port
  @param[in]  vfr_state_ptr:   pointer to VFR service source 
        object

  @result
  ADSP_EOK, on success, error code on failure

  @dependencies
  None.
*/
typedef void (*ts_mux_set_source_type)(uint32_t mux_id, uint32 mux_ctl_sel);

/**
  Function for enabling the VFR source driver

  @param[in]  p_vfr_drv:   pointer to VFR service driver object 
  @param[in]  vfr_state_ptr:   pointer to VFR service source 
        object

  @result
  ADSP_EOK, on success, error code on failure

  @dependencies
  None.
*/
typedef void (*ts_mux_set_timer_source)(uint32_t mux_id, uint32 timer_source);

/**
  Function for disabling the VFR source driver

  @param[in]  p_vfr_drv:   pointer to VFR service driver object
  @param[in]  vfr_state_ptr:   pointer to VFR service source 
        object

  @result
  ADSP_EOK, on success, error code on failure

  @dependencies
  None.
*/
typedef void (*ts_mux_get_timestamp)(uint32_t mux_id, uint64_t *timestamp);


typedef void (*ts_mux_config_vfr_ts_mux)(uint32_t intr_type, uint32_t mux_ctl_sel_in, uint32_t *mux_id);

typedef struct hal_ts_func_def
{
   ts_hal_init_func_t             ts_hal_init_fp;
   ts_mux_set_source_type         set_source_type_fp;
   ts_mux_set_timer_source        set_timer_source_fp;
   ts_mux_get_timestamp           get_timestamp_fp;
   ts_mux_config_vfr_ts_mux		 config_vfr_ts_mux_fp;	
} hal_ts_func_def_t;


extern hal_ts_func_def_t   *hal_ts_func_def_fp;


/** @} */ /* end_addtogroup avtimer_hal_func_defs.h */


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef _HAL_TS_FUNC_DEFS_H__

