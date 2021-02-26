/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/common/src/AfeTimeStampDriver.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AfeTimeStampDriver.cpp

DESCRIPTION: VFR driver for hw programming

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013-2014,2017 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary. 
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/common/src/AfeTimeStampDriver.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/19/2012  david      created

============================================================================*/

#include "qurt_elite.h"
#include "hal_timestamp.h"
#include "AfeTimeStampDriver.h"
#include "avtimer_drv_api.h"

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

ADSPResult afe_ts_drv_init(vfr_hw_info_t *vfr_hw_info_ptr)
{
   return hal_ts_init(vfr_hw_info_ptr);
}

void afe_ts_drv_release_ts_mux(uint32_t mux_id)
{
   hal_ts_release_ts_mux(mux_id);
}

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
void afe_ts_drv_set_source_type(uint32_t mux_id, uint32 mux_ctl_sel)
{
   hal_ts_set_source_type(mux_id, mux_ctl_sel);
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
void afe_ts_drv_set_timer_source(uint32_t mux_id, uint32 timer_source)
{ 
   hal_ts_set_timer_source(mux_id, timer_source);  
}


/**
  Function for reading the timestamp (us) from latched STC counter

  @param[in]  mux_id: Mux ID for the particular TS mux associated with the TS block
  @param[out] timestamp: latched timestamp (us)

  @result
  None

  @dependencies
  None.
*/
void afe_ts_drv_get_timestamp(uint32_t mux_id, uint64_t *timestamp)
{ 
   uint64_t          tick_count = 0;

   hal_ts_get_tickcount(mux_id, &tick_count);

   *timestamp = avtimer_drv_convert_hw_tick_to_time(tick_count);
}


/**
  Function for configuration the vfr timestamp mux control

  @param[in]  mux_id: Mux ID for the particular TS mux associated with the TS block 
  @param[in]  mux_ctl_sel_in: VFR mux input index

  @result
  None

  @dependencies
  None.
*/
void afe_ts_drv_config_vfr_ts_mux(uint32_t intr_type, 
                                            uint32_t mux_ctl_sel_in,
                                            uint32_t *mux_id)
{
   hal_ts_config_vfr_ts_mux(intr_type, mux_ctl_sel_in, mux_id);
}

