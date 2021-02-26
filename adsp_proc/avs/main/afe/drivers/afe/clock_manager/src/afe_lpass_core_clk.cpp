/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/afe_lpass_core_clk.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     clock_manager.cpp

DESCRIPTION: Main driver interface to the clock control

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI)
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/afe_lpass_core_clk.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/15/14    yongseok      Created file

============================================================================*/


/*=====================================================================
 Includes
 ======================================================================*/
#include "Elite.h"
#include "adsp_afe_service_commands.h"
#include "hwd_mmpm.h"
#include "afe_lpass_core_clk.h"
#include "clock_manager_type_i.h"
extern generic_clk_cfg_global_info_t *afe_generic_clk_cfg_global_ctx_ptr;

#define AFE_LPASS_CORE_CLK_DISABLE 0
#define AFE_LPASS_CORE_CLK_ENABLE  1

/**
* sif_clk_cfg_adspm_info_t
*/
typedef struct _lpass_core_clk_cfg_adspm_info
{
   uint32_t client_id;
   int32_t  power_vote_cnt;
   uint32_t lpass_core_clk_status;
}  lpass_core_clk_cfg_adspm_info_t;

static lpass_core_clk_cfg_adspm_info_t lpass_core_adsppm_info;

//this is for the vote related to AHB bus shared clocks
//This is supposed to be okay for all chip family because it will be always on if it is not required.
ADSPResult afe_lpass_core_clk_init(uint32_t hw_version)
{
   ADSPResult result = ADSP_EOK;

   memset((void*)&lpass_core_adsppm_info, 0, sizeof(lpass_core_clk_cfg_adspm_info_t));

   //1. Register for ADSPPM
   if(0 != (lpass_core_adsppm_info.client_id = afe_mmpm_register_lpass_core()))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Lpass Core Clk: HwdMmpm_Register Pass");
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Lpass Core Clk: HwdMmpm_Register fail:0x%x", result);
      result = ADSP_EFAILED;
   }
   return result;
}

//this is for the vote related to AHB bus shared clocks
ADSPResult afe_lpass_core_clk_deinit(void)
{
   ADSPResult result = ADSP_EFAILED;

   //Deregister ADSPPM
   if(ADSP_EOK != (result = HwdMmpm_Deregister(lpass_core_adsppm_info.client_id)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Lpass Core Clk: HwdMmpm_Deregister fail:0x%x", result);
   }

   return result;
}



/**
  @brief Set clock

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] cfg_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_lpass_core_clk_set(int8_t *cfg_ptr, uint32_t payload_size)
{
   ADSPResult  		 result = ADSP_EOK;
	afe_param_id_lpass_core_shared_clk_cfg_t *lpass_core_clk_cfg_ptr =  (afe_param_id_lpass_core_shared_clk_cfg_t *)cfg_ptr;

	if(payload_size < sizeof(afe_param_id_lpass_core_shared_clk_cfg_t))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Size of Lpass Core Clock config payload too small");
		return ADSP_EFAILED;
	}

	if(lpass_core_clk_cfg_ptr->lpass_core_shared_clk_cfg_minor_version > AFE_API_VERSION_LPASS_CORE_SHARED_CLK_CONFIG)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported minor version");
		return ADSP_EUNSUPPORTED;
	}
   
   if(lpass_core_clk_cfg_ptr->enable)
   {
      lpass_core_adsppm_info.power_vote_cnt++;
      
      if(AFE_LPASS_CORE_CLK_DISABLE== lpass_core_adsppm_info.lpass_core_clk_status)
      {
         if(ADSP_EOK != (result = HwdMmpm_RequestPwr(lpass_core_adsppm_info.client_id)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Lpass Core Clk: HwdMmpm_RequestPwr fail: 0x%x", result);
         }
         else
         {
            lpass_core_adsppm_info.lpass_core_clk_status = AFE_LPASS_CORE_CLK_ENABLE;
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE Lpass Core Clk has been enabled: %lu", lpass_core_adsppm_info.lpass_core_clk_status);
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AFE Lpass Core Clk has already been enabled: vote cnt %ld", lpass_core_adsppm_info.power_vote_cnt);
      }
   }            
   else
   {
      lpass_core_adsppm_info.power_vote_cnt--;
         
      if(0 == lpass_core_adsppm_info.power_vote_cnt)
      {
         if(ADSP_EOK != (result = HwdMmpm_ReleasePwr(lpass_core_adsppm_info.client_id)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Lpass Core Clk HwdMmpm_ReleasePwr fail: 0x%x", result);
         }
         else
         {
            lpass_core_adsppm_info.lpass_core_clk_status = AFE_LPASS_CORE_CLK_DISABLE;
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE Lpass Core Clk has been disabled: %ld", lpass_core_adsppm_info.lpass_core_clk_status);
         }
      }
      else if(0 > lpass_core_adsppm_info.power_vote_cnt)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AFE Lpass Core Clk has already been disabled: vote cnt %ld", lpass_core_adsppm_info.power_vote_cnt);
      }
   }
   
   return result;
   
}


uint32_t afe_mmpm_register_lpass_core()
{
  return HwdMmpm_Register((MmpmCoreIdType)((afe_generic_clk_cfg_global_ctx_ptr->devcfg).clk_mgr_coreid), MMPM_CORE_INSTANCE_0);
}

