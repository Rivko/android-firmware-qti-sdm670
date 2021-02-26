/*========================================================================
  This file contains timestamp register programming APIs

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/hal_v1/inc/hal_timestamp_v1.h#1 $
 ====================================================================== */
#ifndef _HAL_TIMESTAMP_V3_H_
#define _HAL_TIMESTAMP_V3_H_

/*==========================================================================
  Include Files
  ========================================================================== */
#include "qurt_elite.h"

/*==========================================================================
  Function prototypes
  ========================================================================== */

/**
  Function for init hal

  @param[in]  pointer to vfr  hw info

  @result
  None

  @dependencies
  None.
*/
void hal_ts_init_v1(vfr_hw_info_t *vfr_hw_info_ptr);



/**
  Function for configuring the mux for interrupt source
  selection

  @param[in]  mux_id: Mux ID for the particular TS mux
  @param[in]  mux_ctl_sel: Mux control input index
  @param[in]  mux_ctl_mask: mask to be use in TS MUX configuration
  @param[in]  ts_phy_base: TS register physical base
  @param[in]  ts_offset: TS register offset

  @result
  None

  @dependencies
  None.
*/
void hal_ts_set_source_type_v1(uint32_t mux_id, uint32 mux_ctl_sel);

/**
  Function for configuring the mux for timer source
  selection

  @param[in]  mux_id: Mux ID for the particular TS mux
  @param[in]  timer_source: Qtimer/AV-timer
  @param[in]  ts_phy_base: TS register physical base
  @param[in]  ts_offset: TS register offset

  @result
  None

  @dependencies
  None.
*/
void hal_ts_set_timer_source_v1(uint32_t mux_id, uint32 timer_source);


/**
  Function for reading the latched STC counter

  @param[in]  mux_id: Mux ID for the particular TS mux associated with the TS block
  @param[in]  ts_phy_base: TS register physical base
  @param[in]  ts_offset: TS register offset
  @param[out] timestamp: latched timestamp

  @result
  None

  @dependencies
  None.
*/
void hal_ts_get_timestamp_v1(uint32_t mux_id, uint64_t *timestamp);


/**
  Function for reading the latched STC counter

  @param[in]  intr_type: Interrupt type to TS mux input 
  @param[in]  mux_ctl_sel_in: VFR mux input index
  @param[out] mux_id: Available mux ID
  @result
  None

  @dependencies
  None.
*/
void hal_ts_config_vfr_ts_mux_v1(uint32_t intr_type, 
                                 uint32_t mux_ctl_sel_in,
                                 uint32_t *mux_id);

#endif /*#if _HAL_TIMESTAMP_V2_H_ */
