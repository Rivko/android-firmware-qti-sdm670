/*========================================================================
  This file contains timestamp register programming APIs

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/common/src/hal_timestamp.h#1 $
 ====================================================================== */
#ifndef _HAL_TIMESTAMP_H_
#define _HAL_TIMESTAMP_H_

/*==========================================================================
  Include Files
  ========================================================================== */
#include "qurt_elite.h"
#include "hal_ts_func_defs.h"
#include "VfrDriverInternal.h"

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

/*==========================================================================
  Stucture definitions
  ========================================================================== */
typedef struct ts_hw_drv
{
   uint32_t             vfr_ts_mux_bitfield;
   uint32_t             gp_ts_mux_bitfield;
   hal_ts_func_def_t    hal_ts_func_ptr;
}ts_hw_drv_t;


extern ts_hw_drv_t  ts_hw_drv_global;

/*==========================================================================
  Function prototypes
  ========================================================================== */

ADSPResult hal_ts_init(vfr_hw_info_t *vfr_hw_info_ptr);

/**
  Function for configuring the mux for interrupt source
  selection

  @param[in]  mux_id: Mux ID for the particular TS mux
  @param[in]  mux_ctl_sel: Mux control input index

  @result
  None

  @dependencies
  None.
*/
ADSPResult hal_ts_get_available_mux_id(uint32_t num_ts_mux,
                                       uint32_t *available_mux_bit_field,
                                       uint32_t *mux_id);


/**
  Function for configuring the mux for interrupt source
  selection

  @param[in]  mux_id: Mux ID for the particular TS mux
  @param[in]  mux_ctl_sel: Mux control input index

  @result
  None

  @dependencies
  None.
*/
void hal_ts_release_ts_mux(uint32_t mux_id);


/**
  Function for configuring the mux for interrupt source
  selection

  @param[in]  mux_id: Mux ID for the particular TS mux
  @param[in]  mux_ctl_sel: Mux control input index

  @result
  None

  @dependencies
  None.
*/
static inline void hal_ts_set_source_type(uint32_t mux_id, uint32 mux_ctl_sel)
{
   ts_hw_drv_global.hal_ts_func_ptr.set_source_type_fp(mux_id, mux_ctl_sel);
}

/**
  Function for configuring the mux for timer source
  selection

  @param[in]  mux_id: Mux ID for the particular TS mux
  @param[in]  timer_source: Qtimer/AV-timer

  @result
  None

  @dependencies
  None.
*/
static inline void hal_ts_set_timer_source(uint32_t mux_id, uint32 timer_source)
{ 
   ts_hw_drv_global.hal_ts_func_ptr.set_timer_source_fp(mux_id, timer_source);  
}


/**
  Function for reading the latched STC counter

  @param[in]  mux_id: Mux ID for the particular TS mux associated with the TS block
  @param[out] timestamp: latched timestamp

  @result
  None

  @dependencies
  None.
*/
static inline void hal_ts_get_tickcount(uint32_t mux_id, uint64_t *tickcount)
{ 
   ts_hw_drv_global.hal_ts_func_ptr.get_timestamp_fp(mux_id, tickcount);
}


/**
  Function for reading the latched STC counter

  @param[in]  mux_id: Mux ID for the particular TS mux associated with the TS block 
  @param[in]  mux_ctl_sel_in: VFR mux input index
  @result
  None

  @dependencies
  None.
*/
static inline void hal_ts_config_vfr_ts_mux(uint32_t intr_type, 
                                            uint32_t mux_ctl_sel_in,
                                            uint32_t *mux_id)
{
   ts_hw_drv_global.hal_ts_func_ptr.config_vfr_ts_mux_fp(intr_type, mux_ctl_sel_in, mux_id);
}

#endif /*#if _HAL_TIMESTAMP_H_*/
