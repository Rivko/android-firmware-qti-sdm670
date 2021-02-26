/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/common/stub_src/AfeTimeStampDriver_stub.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hal_vfr_driver.cpp

DESCRIPTION: VFR register programming

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/common/stub_src/AfeTimeStampDriver_stub.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/19/2012  david      created

============================================================================*/

#include "qurt_elite.h"
#include "AfeTimeStampDriver.h"


/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

ADSPResult afe_ts_drv_init(vfr_hw_info_t *vfr_hw_info_ptr)
{
   return ADSP_EUNSUPPORTED;
}

void afe_ts_drv_release_ts_mux(uint32_t mux_id)
{
   return;
}

void afe_ts_drv_set_source_type(uint32_t mux_id, uint32 mux_ctl_sel)
{
   return;

}


void afe_ts_drv_set_timer_source(uint32_t mux_id, uint32 timer_source)
{ 
   return;
}

void afe_ts_drv_get_timestamp(uint32_t mux_id, uint64_t *timestamp)
{ 
   return;
}


void afe_ts_drv_config_vfr_ts_mux(uint32_t intr_type, 
                                            uint32_t mux_ctl_sel_in,
                                            uint32_t *mux_id)
{
   return;
}

