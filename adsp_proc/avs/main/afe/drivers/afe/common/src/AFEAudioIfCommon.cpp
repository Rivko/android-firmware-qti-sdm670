/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEAudioIfCommon.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hdmi_input_drv.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013 - 2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEAudioIfCommon.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
7/30/2012   RP   Created


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "qurt_elite.h"
#include "qurt_error.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALSys.h"
#include "hwd_devcfg.h"
#include "AFEAudioIfCommon.h"
#include "AFEHalAudioIfCommon.h"
#include "AFEHalDump.h"


/*=====================================================================
 Globals
 ======================================================================*/


typedef enum
{
   AFE_AUDIOIF_COMMON_DEINIT = 0,
   AFE_AUDIOIF_COMMON_INIT
} afe_audioif_common_state_t;

typedef struct afe_audioif_mux_dev_cfg
{
   uint32_t mux_reg_base;        // lpaif reg base addr
   uint32_t mux_reg_size;        // lpaif reg page size
   uint32_t mux_reg_offset;      // mux reg addr
   uint32_t mux_inf_type_cnt;    // # of interfaces that  is shared the output with mux
   uint32_t shared_inf_type_list_map[MAX_LPAIF_MUX_NUM];   // 1:1 mapping for i2s  interface
   uint32_t hw_revision;   //  pcm hw revision
} afe_audioif_mux_dev_cfg_t;


typedef struct audioif_mux_state
{
   audioif_hw_t audioif_hwtype;
   uint32_t mux_usage_cnt[MAX_LPAIF_MUX_NUM];
} audioif_mux_state_t;


typedef struct audioif_common_resource_state
{
   afe_audioif_common_state_t state;
   qurt_elite_mutex_t mux_lock;
   audioif_mux_state_t mux_state[AUDIOIF_COMMON_MAX];
   afe_audioif_mux_dev_cfg_t mux_dev_cfg;
   uint32_t                  mux_virt_addr;     /* mux main register virtual address mapping */
   qurt_mem_region_t         mux_qmem_region;   /* mux register mapping region */
}audioif_common_resource_state_t;


static audioif_common_resource_state_t audioif_resource_state;

static ADSPResult afe_audioif_mux_device_cfg_init(afe_audioif_mux_dev_cfg_t *mux_hw_info_ptr);


ADSPResult afe_audioif_mux_init(void)
{
	ADSPResult   result = ADSP_EOK;

	memset(&audioif_resource_state, 0, sizeof(audioif_common_resource_state_t));
 	afe_audioif_mux_dev_cfg_t *mux_hw_info_ptr = &audioif_resource_state.mux_dev_cfg;

   // 1. read mux hwd dev config and cache it to the mux global state
   if(ADSP_FAILED(result = afe_audioif_mux_device_cfg_init(mux_hw_info_ptr)))
	{
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error afe_audioif_mux_device_cfg_init");
		return result;
	}

   // 2. create memory region for lpass mux register interface
   //   This information is need to be saved to  global state

   // mux register interface
   if(ADSP_FAILED(result = HwdDevCfg_GetVirtAddress(&audioif_resource_state.mux_qmem_region,
            mux_hw_info_ptr->mux_reg_base, mux_hw_info_ptr->mux_reg_size, &audioif_resource_state.mux_virt_addr)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for mux register base");
      return result;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "mux virtual addr 0x%x phy addr 0x%x",
      (unsigned int)audioif_resource_state.mux_virt_addr, (unsigned int)mux_hw_info_ptr->mux_reg_base);
   }

   // 3. do the hal init which save the physical address and virtual address offset.
   hal_audioif_common_init(mux_hw_info_ptr->hw_revision, mux_hw_info_ptr->mux_reg_base, audioif_resource_state.mux_virt_addr, mux_hw_info_ptr->mux_reg_offset);

   //4. initialize mutex
   qurt_elite_mutex_init(&audioif_resource_state.mux_lock);

   audioif_resource_state.state = AFE_AUDIOIF_COMMON_INIT;

	return ADSP_EOK;


}

ADSPResult afe_audioif_mux_deinit(void)
{
   /* release all resource allocated during init*/
   HwdDevCfg_DestoryVirtAddress(audioif_resource_state.mux_qmem_region);

   //4. initialize mutex
   qurt_elite_mutex_destroy(&audioif_resource_state.mux_lock);

#ifdef AUDIOIF_REG_DUMP
   hal_afe_dump_deinit();
#endif

   audioif_resource_state.state = AFE_AUDIOIF_COMMON_DEINIT;

	return ADSP_EOK;
}


bool_t afe_audioif_mux_shared(audioif_hw_t intf_type, uint32_t intf_index)
{
   uint32_t i = 0;
   uint32_t max_num_shared_inf = audioif_resource_state.mux_dev_cfg.mux_inf_type_cnt;
   bool_t result = FALSE;

   for(i = 0; i < max_num_shared_inf ; i ++)
   {
      if(intf_index == audioif_resource_state.mux_dev_cfg.shared_inf_type_list_map[i])
      {
         result = TRUE;
      }
   }

   return result;
}

// config mux only for PRI, SEC, TER, QUA MI2S (Not the SPKR I2S) it is related to clocks.
// what if there is no mux required for pri, sec, ter, qua....

ADSPResult afe_audioif_mux_alloc(audioif_hw_t intf_type, uint32_t intf_index)
{
	ADSPResult result = ADSP_EOK;


   if(AFE_AUDIOIF_COMMON_INIT != audioif_resource_state.state)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "audioif mux is not required intf_type %d intf_index %d",intf_type, intf_index);
      return result;
   }

   if(audioif_resource_state.mux_dev_cfg.mux_inf_type_cnt <= intf_index)
   {
     /* For warlock (mux_inf_type_cnt = 5), since Spkr_i2s interface is
      * updated as bi-directional and used as quinary i2s, need to allow
      * spkr_i2s index (4) and quinary PCM/TDM index (4) to allocate mux.
      *
      * For other targets, do not allow mux allocation for spkr_i2s index */
     return result;
   }

   // if the interface shared the output lines with other interface...
   if(afe_audioif_mux_shared(intf_type, intf_index))
   {
      uint16_t i_curr_inf_index;

      qurt_elite_mutex_lock(&audioif_resource_state.mux_lock);

      // Iterate thru all inf(s) to check the MUX is available to use or not
      // Skip the intf is going to set..
      for(i_curr_inf_index = AUDIOIF_I2S; i_curr_inf_index < AUDIOIF_COMMON_MAX; i_curr_inf_index++)
      {
         if(i_curr_inf_index == intf_type)
         {
            continue;
          }

          if(0 != audioif_resource_state.mux_state[i_curr_inf_index].mux_usage_cnt[intf_index])
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audioif mux aleady allocated to intf %d and inf_index %d",
              i_curr_inf_index, intf_index);
             result = ADSP_ENORESOURCE;
          }
      }

      // If the MUX is ok to use, check it is already use and increase count
      if(ADSP_EOK == result)
      {
          // Do the hardware mux set if not set yet, otherwise just increase count
          if(0 == audioif_resource_state.mux_state[intf_type].mux_usage_cnt[intf_index])
          {
              hal_audioif_common_config_mux(intf_type, intf_index);
          }
          audioif_resource_state.mux_state[intf_type].mux_usage_cnt[intf_index]++;
          MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== audioif mux config to [inf_type:%d] [mux index:%d] [count: %d] ====",
            intf_type, intf_index, audioif_resource_state.mux_state[intf_type].mux_usage_cnt[intf_index]);
      }

      qurt_elite_mutex_unlock(&audioif_resource_state.mux_lock);
   }

   return result;
}

ADSPResult afe_audioif_mux_free(audioif_hw_t intf_type, uint32_t intf_index)
{
	ADSPResult result = ADSP_EOK;

   if(AFE_AUDIOIF_COMMON_INIT != audioif_resource_state.state)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "audioif mux is not required intf_type %d intf_index %d",intf_type, intf_index);
      return result;
   }

   if(audioif_resource_state.mux_dev_cfg.mux_inf_type_cnt <= intf_index)
   {
     /* For warlock (mux_inf_type_cnt = 5), since Spkr_i2s interface is
      * updated as bi-directional and used as quinary i2s, need to allow
      * spkr_i2s index (4) and quinary PCM/TDM index (4) to de-allocate mux.
      *
      * For other targets, do not allow mux allocation/de-allocation for
      * spkr_i2s index*/
     return result;
   }

   if(afe_audioif_mux_shared(intf_type, intf_index))
   {
      qurt_elite_mutex_lock(&audioif_resource_state.mux_lock);

      if(0 < audioif_resource_state.mux_state[intf_type].mux_usage_cnt[intf_index])
      {
         audioif_resource_state.mux_state[intf_type].mux_usage_cnt[intf_index]--;
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audioif mux aleady is freed inttype %d intf %d",intf_type, intf_index);

         result = ADSP_EUNEXPECTED;
      }

      qurt_elite_mutex_unlock(&audioif_resource_state.mux_lock);
   }

   return result;

}


/**
  @brief Obtain the chip specific information for initializing the driver

  @param[in] mux_hw_info_ptr pointer for mux config structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_audioif_mux_device_cfg_init(afe_audioif_mux_dev_cfg_t *mux_hw_info_ptr)
{
   DALSYSPropertyVar prop_var;
   lpasshwio_prop_lpaif_struct_t  *lpaif_prop_dev_cfg_ptr = NULL;
   const char *dev_id="LPAIF";
   uint32 i;
   ADSPResult result = ADSP_EOK;

   DALSYS_PROPERTY_HANDLE_DECLARE(dal_hdl);

   /* Get device handle */
   if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, dal_hdl) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting the DAL device handle for I2S");
      result = ADSP_EFAILED;
   }
	else
   {
      /* Read property ptr */
      if( DAL_SUCCESS != DALSYS_GetPropertyValue(dal_hdl, "LpaifPropStructPtr", 0, &prop_var) )
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting HDMI input Property Struct Ptr");
         result = ADSP_EFAILED;
      }
      else
      {
         lpaif_prop_dev_cfg_ptr = (lpasshwio_prop_lpaif_struct_t *)prop_var.Val.pStruct;

         if (NOT_SUPPORTED == lpaif_prop_dev_cfg_ptr->lpaif_mux_prop.is_supported)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "lpaif mux is not supported");
            result = ADSP_EUNSUPPORTED;
         }

         /* hw revision */
         mux_hw_info_ptr->hw_revision = lpaif_prop_dev_cfg_ptr->hw_revision;

         /* MUX register base address */
         mux_hw_info_ptr->mux_reg_base = lpaif_prop_dev_cfg_ptr->lpaif_reg_base;
         /* MUX register page size */
         mux_hw_info_ptr->mux_reg_size = lpaif_prop_dev_cfg_ptr->lpaif_reg_size;
         /* MUX register offset */
         mux_hw_info_ptr->mux_reg_offset = lpaif_prop_dev_cfg_ptr->lpaif_mux_prop.mux_reg_offset_addr;
         /* number of i2s interfaces */
         mux_hw_info_ptr->mux_inf_type_cnt = lpaif_prop_dev_cfg_ptr->lpaif_mux_prop.mux_inf_type_cnt;
         for(i=0; i < mux_hw_info_ptr->mux_inf_type_cnt; i++)
         {
            mux_hw_info_ptr->shared_inf_type_list_map[i] = lpaif_prop_dev_cfg_ptr->lpaif_mux_prop.shared_inf_type_list_map[i];
         }
#ifdef AUDIOIF_REG_DUMP
         // need to revist
         hal_afe_dump_init(lpaif_prop_dev_cfg_ptr->hw_revision);
#endif
      }
   }

	return result;
}


