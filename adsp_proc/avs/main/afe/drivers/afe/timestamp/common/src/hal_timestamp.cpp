/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/common/src/hal_timestamp.cpp#1 $
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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/timestamp/common/src/hal_timestamp.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/19/2012  RP      created

============================================================================*/

#include "qurt_elite.h"
#include "common.h"
#include "hal_timestamp.h"
#include "hal_timestamp_v1.h"
#include "AfeTimeStampDriver.h"

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

ts_hw_drv_t  ts_hw_drv_global;

ADSPResult hal_ts_init(vfr_hw_info_t *vfr_hw_info_ptr)
{
   memset(&ts_hw_drv_global, 0, sizeof(ts_hw_drv_t));

   switch(vfr_hw_info_ptr->hw_revision)
   {
      case VFR_HW_VERSION_1:
      {
         ts_hw_drv_global.hal_ts_func_ptr.ts_hal_init_fp =  hal_ts_init_v1;
         ts_hw_drv_global.hal_ts_func_ptr.set_source_type_fp = hal_ts_set_source_type_v1; 
         ts_hw_drv_global.hal_ts_func_ptr.set_timer_source_fp = hal_ts_set_timer_source_v1;
         ts_hw_drv_global.hal_ts_func_ptr.get_timestamp_fp = hal_ts_get_timestamp_v1;
         ts_hw_drv_global.hal_ts_func_ptr.config_vfr_ts_mux_fp = hal_ts_config_vfr_ts_mux_v1;
      }
      break;
      default: 
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_ts_init error non-supported hw version 0x%x", vfr_hw_info_ptr->hw_revision);
         return ADSP_EFAILED;
      break; 
   }

   
   ts_hw_drv_global.hal_ts_func_ptr.ts_hal_init_fp(vfr_hw_info_ptr);

   return ADSP_EOK;
}

ADSPResult hal_ts_get_available_mux_id(uint32_t num_ts_mux,
                                       uint32_t *available_mux_bit_field,
                                       uint32_t *mux_id)
{
   uint32_t temp_mux_bit_field;

   temp_mux_bit_field = *available_mux_bit_field;
   *mux_id = 0;

   while(*mux_id < num_ts_mux)
   {
      if(0 == (temp_mux_bit_field & 0x1))
      {
         /* This mux is vacant, mark it as used and return with the corresponding ID */
         *available_mux_bit_field |= (0x1 << (*mux_id));
         return ADSP_EOK;

      }
      temp_mux_bit_field >>= 1;
      (*mux_id)++;
   }

   return ADSP_ENORESOURCE;
}

void hal_ts_release_ts_mux(uint32_t mux_id)
{
   uint32_t *mux_bit_field = NULL;

   switch(mux_id)
   {
      case AFE_TS_VFR0_MUX_ID:
      case AFE_TS_VFR1_MUX_ID:
         mux_bit_field = &ts_hw_drv_global.vfr_ts_mux_bitfield;
         break;
      case AFE_TS_GP0_MUX_ID:
      case AFE_TS_GP1_MUX_ID:
         mux_bit_field = &ts_hw_drv_global.gp_ts_mux_bitfield;
         mux_id -= AFE_TS_GP0_MUX_ID;
         break;
	  default:
         break;	  
   }

   if(NULL != mux_bit_field)
   {
      *mux_bit_field &= ~(0x1UL << mux_id);
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW TS mux release success, mux_id: 0x%lx", mux_id);
   }

   return;
}
