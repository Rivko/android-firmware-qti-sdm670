/*========================================================================

*//** @file Vptx_Modules.cpp

Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code that acts as a pipe between Voice Proc Tx(VPTX)
Dynamic service and library wrappers.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/src/Vptx_Modules.cpp#3 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/20/11   dc     Created file.

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Vptx_Modules.h"
#include "Vptx_Svc.h"
#include "adsp_amdb_static.h"
#include "capi_v2_library_factory.h"
#include "Elite_intf_extns_change_media_fmt.h"

// Thread Stack size -  defined in config
extern const unsigned int VPTX_STACK_SIZE;

// Callback function for CAPI modules to raise events
static capi_v2_err_t vptx_capi_v2_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr);

static void vptx_get_near_far_sample_drift(vptx_t* pVptx, int32_t drift[2]);
static ADSPResult vptx_get_additonal_module_static_property(vptx_t* pVptx,voice_capi_module_t* module_info);
static void voice_log_ec_noise_ref_signal(vptx_t* pVptx);
static ADSPResult vptx_reconfig_stream_io_buf(voice_capi_module_t *module_ptr);
int32_t vptx_send_hpcm_buf_available_evt(vptx_t* pVptx, void* data_ptr);

static ADSPResult vptx_allocate_process_buffers(vptx_t* pVptx);
static ADSPResult vptx_set_topo_framesize(vptx_t *pVptx);
static uint32_t vptx_get_port_info_size(vptx_t *pVptx);
static void vptx_set_port_info_mem(vptx_t *pVptx, int8_t **mem);
static bool_t vptx_can_module_process(voice_capi_module_t *curr_module_ptr, capi_v2_stream_data_t *In[], capi_v2_stream_data_t *Out[],
                                     bool_t is_rate_matching_supported, int32_t *nSlipStuffSamples);

static ADSPResult vptx_process_port_threshold_module(voice_capi_module_t *curr_module_ptr, capi_v2_stream_data_t *In[], capi_v2_stream_data_t *Out[],
                                     bool_t is_rate_matching_supported, int32_t *nSlipStuffSamples, int8_t* pModule_scratch_mem);

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

static bool_t vptx_is_mic_path_channel_mapping(uint8_t *channel_type_ptr)
{
   switch (*channel_type_ptr)
   {
      case PCM_CHANNEL_PRI_MIC:
      case PCM_CHANNEL_SEC_MIC_1:
      case PCM_CHANNEL_SEC_MIC_2:
      case PCM_CHANNEL_SEC_MIC_3:
      case PCM_CHANNEL_FAR_REF:
      {
          return TRUE;
      }
      default:
      {
          return FALSE;
      }
   }

}

static void vptx_update_ec_channel_map(vptx_t *pVptx, voice_capi_data_format_struct_t *media_fmt_ptr, uint16_t is_pre_post_update)
{   
   uint32_t ch;

   uint8_t mic_channel_map[QUAD] = { PCM_CHANNEL_PRI_MIC, PCM_CHANNEL_SEC_MIC_1,
                                     PCM_CHANNEL_SEC_MIC_2, PCM_CHANNEL_SEC_MIC_3 };

   if(0 == is_pre_post_update)
   {
      if(FALSE == vptx_is_mic_path_channel_mapping((uint8_t *)&media_fmt_ptr->data_format.channel_type[0]))
      {
         //If speaker path channel map is received, overwrite it with mic path channel map.
         for(ch = 0; ch < media_fmt_ptr->data_format.num_channels ; ch++)
         {
            media_fmt_ptr->data_format.channel_type[ch] = mic_channel_map[ch];
         }
      }
   }
   else
   {
      if(FALSE == vptx_is_mic_path_channel_mapping((uint8_t *)&media_fmt_ptr->data_format.channel_type[0]))
      {
         //rebase to original values.
         for(ch = 0; ch < media_fmt_ptr->data_format.num_channels ; ch++)
         {
            media_fmt_ptr->data_format.channel_type[ch] = pVptx->io.near_in.mediatype.channel_mapping[ch];
         }
      }
   }
}

void vptx_modules_init(vptx_t* pVptx)
{

   pVptx->mute = VOICE_UNMUTE; // don't cache this

}


/*
 * This function initializes all the modules with fresh calibration at the start of every call
 * Modules internaly need to check if reinit is required to avoid convergence issues after handover.
 */
ADSPResult vptx_modules_full_init(vptx_t* pVptx)
{
   capi_v2_err_t capi_result=CAPI_V2_EOK;
   capi_v2_prop_t reset_prop[] = {
      { CAPI_V2_ALGORITHMIC_RESET, { NULL, 0, 0 }, {FALSE, FALSE, 0} },
   };
   capi_v2_proplist_t reset_prop_list = {sizeof(reset_prop)/sizeof(capi_v2_prop_t), reset_prop};

   for(uint32_t i=0; i<pVptx->modules.num_modules;i++)
   {
      if((pVptx->modules.modules_array[i].is_capi)&&(!pVptx->modules.modules_array[i].is_virtual_stub))
      {
         //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Issuing ALGO_RESET for module ID(%#lx), session (%#lx)",pVptx->modules.modules_array[i].module_id, pVptx->session.session_num);
         if(pVptx->modules.modules_array[i].module_ptr)
         {
            capi_result = pVptx->modules.modules_array[i].module_ptr->vtbl_ptr->set_properties(pVptx->modules.modules_array[i].module_ptr, &reset_prop_list);
         }
         else
         {
            capi_result = CAPI_V2_EUNSUPPORTED;
         }
         if(CAPI_V2_FAILED(capi_result))
         {
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: ALGO_RESET failed for moduleId(%lu),module_ptr(%p),result(0x%lx) session (%lx)",
                  pVptx->modules.modules_array[i].module_id,pVptx->modules.modules_array[i].module_ptr,capi_result,pVptx->session.session_num);
         }
      }
   }

   /*
    * ALGO_RESET does not clear Sampleslip state/drift related information.
    * Need to pass SS_PARAM_RESET propId to clear the same.
    */
   {
      capi_v2_buf_t param_data_buf;
      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;

      param_data_buf.data_ptr = NULL;
      param_data_buf.actual_data_len = param_data_buf.max_data_len = 0;
      if(pVptx->modules.ss_capi.module_ptr)
      {
         capi_result = pVptx->modules.ss_capi.module_ptr->vtbl_ptr->set_param(pVptx->modules.ss_capi.module_ptr,
               SS_PARAM_RESET,&port_info,&param_data_buf);
         if(CAPI_V2_FAILED(capi_result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: SS_PARAM_RESET failed for SS CAPI, session (%#lx)",pVptx->session.session_num);
         }
      }
      else if(pVptx->modules.special_capis[ECNS_CAPI])
      {
         capi_result = pVptx->modules.special_capis[ECNS_CAPI]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[ECNS_CAPI]->module_ptr,
               SS_PARAM_RESET,&port_info,&param_data_buf);
         if(CAPI_V2_FAILED(capi_result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: SS_PARAM_RESET failed for ECNS CAPI, session (%#lx)",pVptx->session.session_num);
         }
      }
   }

   return capi_result;
}

ADSPResult vptx_aggregate_modules_kpps(vptx_t* pVptx, uint32_t* kpps_changed)
{
   uint32_t kpps = 0;
   uint32_t modules_kpps = 0;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_kpps(): Begin KPPS query, session (%lx)", pVptx->session.session_num);

   voice_capi_module_t* curr_module_ptr = pVptx->modules.modules_array;

   for(uint32_t i=0; i<pVptx->modules.num_modules;i++)
   {
      if(curr_module_ptr->is_capi)
      {
         //account for kpps only if module is enabled.
         kpps = 0;
         if(TRUE == curr_module_ptr->is_enabled)
         {
            kpps = curr_module_ptr->kpps;
         }
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_kpps(): ModId(0x%lx) Instance(%d) reported KPPS(%ld), is enabled(%d)",
               curr_module_ptr->module_id, curr_module_ptr->instance_id, kpps, (int)curr_module_ptr->is_enabled);
         modules_kpps += kpps;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: get_kpps(): Unknown non-capi module ModId(0x%lx)",curr_module_ptr->module_id);
      }
      curr_module_ptr++;
   }

   if (pVptx->modules.ss_capi.module_ptr) // if SS is created by service
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_kpps(): Sample slip kpps (%d)", (int)pVptx->modules.ss_capi.kpps);
      modules_kpps += pVptx->modules.ss_capi.kpps;
   }

   modules_kpps += 1000; // adding extra 1000 kpps to ceil the number to mpps

   // Retaining behavior as it is as of today. That is to report an increase in KPPS only.
   // Once the VOICE_CMD_SET_TIMING_PARAMS commands are removed,
   // this can be modified to (pVptx->timing.modules_kpps != modules_kpps)
   if (pVptx->timing.modules_kpps >= modules_kpps)
   {
      *kpps_changed = FALSE;
   }
   else
   {
      *kpps_changed = TRUE;
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_kpps(): Vptx KPPS changed from (%lu) to (%lu), session(%x)", pVptx->timing.modules_kpps, modules_kpps, (int)pVptx->session.session_num);
   }

   // Update state
   pVptx->timing.modules_kpps = modules_kpps;

   return ADSP_EOK;
}

ADSPResult vptx_aggregate_modules_delay(vptx_t* pVptx)
{
   uint32_t delay = 0;
   uint32_t modules_delay = 0;

   voice_capi_module_t* curr_module_ptr = pVptx->modules.modules_array;

   for(uint32_t i=0; i<pVptx->modules.num_modules;i++)
   {
      if(curr_module_ptr->is_capi)
      {
         //account for delay only if module is enabled.
         delay = 0;
         if(TRUE == curr_module_ptr->is_enabled)
         {
            delay = curr_module_ptr->delay;
         }
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_delay(): ModId(0x%lx) Instance(%d) reported delay(%d), is enabled(%d)",
               curr_module_ptr->module_id ,curr_module_ptr->instance_id, (int)delay, (int)curr_module_ptr->is_enabled);
         modules_delay += delay;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: get_delay(): Unknown non-capi module ModId(0x%lx)",curr_module_ptr->module_id);
      }
      curr_module_ptr++;
   }

   if (pVptx->modules.ss_capi.module_ptr) // if SS is created by service
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_delay(): Sample slip delay (%d)", (int)pVptx->modules.ss_capi.delay);
      modules_delay += pVptx->modules.ss_capi.delay;
   }


   // prebuffering , ideally it is not part of modules delay but helpful to account here to avoid extra operation
   modules_delay += (PRE_BUF * 1000);

   // Update state
   pVptx->timing.modules_delay = modules_delay;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_delay(): total vptx algo delay (%d)", (int)modules_delay);

   return ADSP_EOK;
}

void vptx_modules_end(vptx_t* pVptx)
{
   uint32_t temp_modid,temp_mod_index;
   uint16_t temp_instance_id;
   bool_t temp_stub_info;
   void *temp_handle;

   // Check guard band for memory corruption.
   voice_check_guard_band((int8_t**)pVptx->memory.guard_band_ptrs, (int16_t)pVptx->modules.num_modules, TX, pVptx->session.session_num);

   voice_capi_module_t* curr_module_ptr = pVptx->modules.modules_array;
   for(uint32_t i=0; i < pVptx->modules.num_modules ; i++)
   {

      if(curr_module_ptr->is_capi)
      {
         if(curr_module_ptr->fwk_extn_ptr)
         {
            qurt_elite_memory_free(curr_module_ptr->fwk_extn_ptr);
            curr_module_ptr->fwk_extn_ptr = NULL;
         }
         if(curr_module_ptr->module_ptr)
         {
            curr_module_ptr->module_ptr->vtbl_ptr->end(curr_module_ptr->module_ptr);
         }
         curr_module_ptr->module_ptr = NULL;

         if(TRUE == pVptx->reconfig_state.retain_amdb_handle)
         {
            temp_modid = curr_module_ptr->module_id;
            temp_instance_id = curr_module_ptr->instance_id;
            temp_handle = curr_module_ptr->amdb_handle_ptr;
            temp_mod_index = curr_module_ptr->module_index;
            temp_stub_info = curr_module_ptr->is_virtual_stub;
            memset(curr_module_ptr,0,sizeof(voice_capi_module_t));
            curr_module_ptr->module_id = temp_modid;
            curr_module_ptr->instance_id = temp_instance_id;
            curr_module_ptr->amdb_handle_ptr = temp_handle ;
            curr_module_ptr->module_index = temp_mod_index ;
            curr_module_ptr->is_virtual_stub = temp_stub_info;
         }
         else
         {
            curr_module_ptr->amdb_handle_ptr = NULL;
         }
      }
      else
      {
         switch(curr_module_ptr->module_id)
         {
            default:
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: module end(): Unknown non-capi module ModId(0x%lx)",curr_module_ptr->module_id);
               break;
            }
         }
      }
      curr_module_ptr++;
   }

   if ((NULL != pVptx->modules.handle_info_ptr) && (FALSE == pVptx->reconfig_state.retain_amdb_handle))
   {
      adsp_amdb_release_handles(pVptx->modules.handle_info_ptr, pVptx->modules.num_modules);
      qurt_elite_memory_free(pVptx->modules.handle_info_ptr);
      pVptx->modules.handle_info_ptr = NULL;
   }

   //clear special capi pointers and resampler state
   memset(&pVptx->modules.special_capis[0],0,sizeof(pVptx->modules.special_capis));
   if (pVptx->modules.ss_capi.module_ptr)
   {
      pVptx->modules.ss_capi.module_ptr->vtbl_ptr->end(pVptx->modules.ss_capi.module_ptr);
      pVptx->modules.ss_capi.module_ptr = NULL;
   }
   if (NULL != pVptx->modules.ss_handle_info.h.capi_v2_handle)
   {
      adsp_amdb_release_handles(&pVptx->modules.ss_handle_info, 1);
   }
   pVptx->modules.ss_handle_info.h.capi_v2_handle = NULL;
}


static void vptx_check_for_capi_module(voice_capi_module_t *module_info)
{
   switch(module_info->module_id)
   {
      default:
      {
         //default is capi.
         module_info->is_capi = TRUE;
         break;
      }
   }
}

static void vptx_set_module_array(vptx_t* pVptx)
{
   uint32_t num_modules = pVptx->modules.num_modules;
   uint32_t soft_mute_idx, resamp_idx;
   
   if((VPTX_AUDIO == pVptx->session.audio_instance_flag) && (pVptx->io.near_in.mediatype.sample_rate != pVptx->io.output.mediatype.sample_rate))
   {
      soft_mute_idx = pVptx->modules.num_modules-2;
      resamp_idx = pVptx->modules.num_modules -1;
   }
   else
   {
      soft_mute_idx = pVptx->modules.num_modules-1;
      resamp_idx = INVALID; 
   }
   
   pVptx->modules.modules_array[soft_mute_idx].module_id = VOICE_MODULE_SOFT_MUTE;
   pVptx->modules.modules_array[soft_mute_idx].instance_id = 0;
   num_modules = pVptx->modules.num_modules-1;
  
   if(resamp_idx != INVALID)
   {
      pVptx->modules.modules_array[resamp_idx].module_id = AUDPROC_MODULE_ID_RESAMPLER;
      pVptx->modules.modules_array[resamp_idx].instance_id = 0;
      num_modules = pVptx->modules.num_modules-2;

      pVptx->modules.special_capis[DYN_RESAMP] = &pVptx->modules.modules_array[resamp_idx];
   }
   
   for (uint32_t i = 0 ; i < num_modules; i++)
   {
      pVptx->modules.modules_array[i].module_id = pVptx->modules.modules_list[i].module_id;
      pVptx->modules.modules_array[i].instance_id = pVptx->modules.modules_list[i].instance_id;     
      pVptx->modules.modules_array[i].module_index = i; //for backtracing array if needed
   }

   return;
}

static void vptx_set_dyn_resampler_media_fmt(vptx_t* pVptx, voice_capi_module_t* resamp_module_ptr)
{
   capi_v2_buf_t param_data_buf;
   capi_v2_port_info_t out_port_info         = {TRUE,FALSE,0};
   intf_extn_param_id_chg_mf_propose_op_mf_t dynamic_resampler_op_mf;
   dynamic_resampler_op_mf.default_sampling_rate = FALSE;
   dynamic_resampler_op_mf.sampling_rate = pVptx->io.output.mediatype.sample_rate;
   dynamic_resampler_op_mf.default_channel_info = TRUE;
   dynamic_resampler_op_mf.default_bits_per_sample = TRUE;

   param_data_buf.data_ptr =(int8_t*) &dynamic_resampler_op_mf;
   param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(dynamic_resampler_op_mf);

   resamp_module_ptr->module_ptr->vtbl_ptr->set_param(resamp_module_ptr->module_ptr,INTF_EXTN_PARAM_ID_CHG_MF_PROPOSE_OP_MF,&out_port_info,&param_data_buf);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: setting dynamic resampler output media format (%lu), session(%lx)",dynamic_resampler_op_mf.sampling_rate,pVptx->session.session_num);
   
   return;
}

static ADSPResult vptx_allocate_process_buffers(vptx_t* pVptx)
{
   ADSPResult nResult = ADSP_EOK;
   uint32_t nTotalSize;
   uint32_t process_buf_near_ch_samples ,
   process_buf_near_ch0123_size , process_buf_far_ch_samples, process_buf_far_size;
  uint32_t max_vptx_channels = 0, modules_scratch_mem_size = 0;

   // memory for processing buffers
   process_buf_near_ch_samples = VOICE_ROUNDTO8(pVptx->io.near_in.frame_samples + pVptx->io.prebuffer_samples); // samples per near channels
   process_buf_near_ch0123_size = (process_buf_near_ch_samples << 1)* pVptx->io.near_in.mediatype.num_channels; // size for num_channels

   //Additional memory to use as ping-pong buffers for non-inplace modules
   process_buf_near_ch0123_size = process_buf_near_ch0123_size<<1;

   // far buffers
   process_buf_far_ch_samples = VOICE_ROUNDTO8(pVptx->io.far_in.frame_samples + pVptx->io.prebuffer_samples); // far channels
   process_buf_far_size = (process_buf_far_ch_samples << 1) * pVptx->io.far_in.mediatype.num_channels;

   nTotalSize = process_buf_near_ch0123_size    \
         + process_buf_far_size;


  if(pVptx->modules.is_data_threshold_raised)
  {
      max_vptx_channels = (pVptx->io.near_in.mediatype.num_channels > pVptx->io.far_in.mediatype.num_channels)?pVptx->io.near_in.mediatype.num_channels:pVptx->io.far_in.mediatype.num_channels;
      modules_scratch_mem_size = (VPTX_MAX_INPUT_PORT + ECNS_MAX_OUTPUT_PORT) * VOICE_ROUNDTO8(sizeof(capi_v2_buf_t)) * max_vptx_channels;
      nTotalSize += modules_scratch_mem_size;
  }

  pVptx->memory.pBufStartAddr = (int8_t *)qurt_elite_memory_malloc(nTotalSize, QURT_ELITE_HEAP_DEFAULT);
  if (NULL == pVptx->memory.pBufStartAddr)
  {
     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to allocate memory for VPTX modules!! session(%x)",(int)pVptx->session.session_num);
     return ADSP_ENOMEMORY;
  }
  memset( pVptx->memory.pBufStartAddr, 0, nTotalSize); // clear the memory before use

  // UsageAddr is a byte pointer
  pVptx->memory.nSizeBuffs = nTotalSize; // size allocated
  pVptx->memory.pUsageAddr = pVptx->memory.pBufStartAddr; // Usage level pointer


  for (int32_t j = 0; j < pVptx->io.near_in.mediatype.num_channels; j++)
  {
     pVptx->io.near_in.process_buf_ptr[j] = pVptx->memory.pUsageAddr;
     pVptx->memory.pUsageAddr += process_buf_near_ch_samples << 1;
     pVptx->memory.pUsageAddr = (int8_t*)VOICE_ROUNDTO8(pVptx->memory.pUsageAddr);
  }

  //these buffers are allocated to support capi modules which are not in-place
  //Running the second loop deliberately to keep the adjacent buffers in contiguous memory location.
  for (int32_t j = 0; j < pVptx->io.near_in.mediatype.num_channels; j++)
  {
     pVptx->io.near_in.process_buf_ptr_sec[j] = pVptx->memory.pUsageAddr;
     pVptx->memory.pUsageAddr += process_buf_near_ch_samples << 1;
     pVptx->memory.pUsageAddr = (int8_t*)VOICE_ROUNDTO8(pVptx->memory.pUsageAddr);
  }

  for (int32_t j = 0; j < pVptx->io.far_in.mediatype.num_channels; j++)
  {
     pVptx->io.far_in.process_buf_ptr[j] = pVptx->memory.pUsageAddr;
     pVptx->memory.pUsageAddr += process_buf_far_ch_samples << 1;
     pVptx->memory.pUsageAddr = (int8_t*)VOICE_ROUNDTO8(pVptx->memory.pUsageAddr);
  }

  if(pVptx->modules.is_data_threshold_raised)
  {
     pVptx->modules.pScrath_mem = pVptx->memory.pUsageAddr;
     pVptx->memory.pUsageAddr += modules_scratch_mem_size;
  }
  // Sanity check to see the allocated memory was enough
  if ((uint32_t)(pVptx->memory.pUsageAddr - pVptx->memory.pBufStartAddr) <= pVptx->memory.nSizeBuffs)
  {
     nResult = ADSP_EOK;
  }
  else
  {
     MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Error allocating memory result(%d) session(%x), usage: %d, allocated: %d",nResult,(int)pVptx->session.session_num,(int) (pVptx->memory.pUsageAddr - pVptx->memory.pStartAddr),(int) pVptx->memory.nSize);
     nResult = ADSP_ENOMEMORY;
     return nResult;
  }

  return nResult;
}
void vptx_free_circ_bufs(vptx_t* pVptx)
{
   if(NULL != pVptx->memory.pCircBufAddr)
   {
      qurt_elite_memory_free(pVptx->memory.pCircBufAddr);
   }
   pVptx->memory.pCircBufAddr = NULL;
   pVptx->memory.circ_buf_vfr_cycle = 0;
   return;
}

ADSPResult vptx_allocate_circ_bufs(vptx_t* pVptx)
{
   uint32_t nNearCircBufSamples, nFarCircBufSamples, nOutCircBufSamples, nLoopCircBufSamples ;
   uint32_t nCircBufNearSize, nCircBufFarSize, nCircBufOutSize, nCircBufLoopSize, nTotalSize, scale_factor;

   // check if already allocated
   if((NULL != pVptx->memory.pCircBufAddr) && (pVptx->memory.circ_buf_vfr_cycle == pVptx->session.vfr_cycle))
   {
      return ADSP_EOK;
   }

   //Free if already allocated (takes care of null check)
   vptx_free_circ_bufs(pVptx);

   // Default to 20 ms if no timing command provided (e.g. audio path)
   if(0 == pVptx->session.vfr_cycle)
   {
      pVptx->session.vfr_cycle = 1;
   }

   // Circ buf should always be allocated for max amount of data in cdrx cycle, even for 10 ms topos
   // with volte optimizations, processing won't start at 10 ms buffered point, but may even start
   // at almost full cdrx cycle buffered if proc time is short enough
   if(VPTX_AUDIO == pVptx->session.audio_instance_flag)
   {
       scale_factor = 1;
   }
   else
   {
       scale_factor = pVptx->session.vfr_cycle * ((10 == pVptx->io.num_1ms_frame_block_process) ? 2 : 1 );
   }

   // samples(in words) per channel(10ms) + pre buffer(1ms) + jitter buffer(1ms)
   nNearCircBufSamples = VOICE_ROUNDTO8(scale_factor * pVptx->io.near_in.frame_samples + (pVptx->io.prebuffer_samples) + (pVptx->io.jitter_buffer_samples));
   // samples(in words) per channel(10ms) + pre buffer(1ms) + jitter buffer(1ms)
   nFarCircBufSamples  = VOICE_ROUNDTO8(scale_factor * pVptx->io.far_in.frame_samples + (pVptx->io.prebuffer_samples) + (pVptx->io.jitter_buffer_samples));
   nOutCircBufSamples  = VOICE_ROUNDTO8(pVptx->session.vfr_cycle * pVptx->io.output.buf_samples);
   nLoopCircBufSamples = VOICE_ROUNDTO8(pVptx->io.output.buf_samples);
   nCircBufNearSize    = (pVptx->io.near_in.mediatype.num_channels * nNearCircBufSamples) << 1; // size(in bytes) including all channels for malloc
   nCircBufFarSize     = (pVptx->io.far_in.mediatype.num_channels * nFarCircBufSamples) << 1;
   nCircBufOutSize     = (pVptx->io.output.mediatype.num_channels * nOutCircBufSamples) << 1;
   nCircBufLoopSize    = (pVptx->io.loopback_output.mediatype.num_channels * nLoopCircBufSamples) << 1;

   nTotalSize = nCircBufNearSize          \
         + nCircBufFarSize                \
         + nCircBufOutSize                \
         + nCircBufLoopSize;


   if (pVptx->io.num_1ms_frame_block_process == 20)
   {
      // allocating memory for scratch buffer to store initial data
      pVptx->io.scratch.data_size_bytes = 2*nNearCircBufSamples << 1;  // in bytes
      nTotalSize += pVptx->io.scratch.data_size_bytes;
   }

   pVptx->memory.pCircBufAddr = (int8_t *)qurt_elite_memory_malloc(nTotalSize, QURT_ELITE_HEAP_DEFAULT);
   if(NULL == pVptx->memory.pCircBufAddr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for VPTX circbuf!! session(%x)",(int)pVptx->session.session_num);
      return ADSP_ENOMEMORY;
   }
   pVptx->memory.circ_buf_vfr_cycle = pVptx->session.vfr_cycle; //store vfr cycle for which memory is allocated

   memset( pVptx->memory.pCircBufAddr, 0, nTotalSize); // clear the memory before use
   //assign pointers
   int8_t          *pUsageAddr = pVptx->memory.pCircBufAddr;
   for (int32_t j = 0; j < pVptx->io.near_in.mediatype.num_channels; j++)
   {
      pVptx->io.near_in.circ_buf.buf_ptr[j] = pUsageAddr;
      pUsageAddr += nNearCircBufSamples << 1;
      pUsageAddr = (int8_t*)VOICE_ROUNDTO8(pUsageAddr);
   }

   for (int32_t j = 0; j < pVptx->io.far_in.mediatype.num_channels; j++)
   {
      pVptx->io.far_in.circ_buf.buf_ptr[j] = pUsageAddr;
      pUsageAddr += nFarCircBufSamples << 1;
      pUsageAddr = (int8_t*)VOICE_ROUNDTO8(pUsageAddr);
   }

   for (int32_t j = 0; j < pVptx->io.output.mediatype.num_channels; j++)
   {
      pVptx->io.output.circ_buf.buf_ptr[j] = pUsageAddr;
      pUsageAddr += nOutCircBufSamples << 1;
      pUsageAddr = (int8_t*)VOICE_ROUNDTO8(pUsageAddr);
   }
   for (int32_t j = 0; j < pVptx->io.loopback_output.mediatype.num_channels; j++)
   {
      pVptx->io.loopback_output.circ_buf.buf_ptr[j] = pUsageAddr;
      pUsageAddr += nLoopCircBufSamples << 1;
      pUsageAddr = (int8_t*)VOICE_ROUNDTO8(pUsageAddr);
   }

   //initialize buffers
   vptx_circ_buf_init(pVptx, nNearCircBufSamples, nFarCircBufSamples, nOutCircBufSamples, nLoopCircBufSamples);

  if (pVptx->io.num_1ms_frame_block_process == 20)
  {
      // This scratch buffer should not be used before sync_tick_handler. It can be re-used in vptx_process.
      // It is added for saving/restoring data or padding zeros during sync tick as part of KPI improvements
      pVptx->io.scratch.data_ptr = pUsageAddr;
      pVptx->memory.pUsageAddr += pVptx->io.scratch.data_size_bytes;
      pUsageAddr = (int8_t*)VOICE_ROUNDTO8(pUsageAddr);
  }

   return ADSP_EOK;

}

static uint32_t vptx_get_port_info_size(vptx_t *pVptx)
{
   uint32_t port_info_size = 0;
   uint32_t i = 0;
   voice_capi_module_t *curr_module_ptr = &pVptx->modules.modules_array[0]; // set it to the first module
   uint32_t total_size = 0;

   if(pVptx->modules.ss_capi.mem_size_allocated > 0)
   {
       port_info_size = sizeof(voice_module_port_t) * pVptx->modules.ss_capi.port_info.port_num_info.num_input_ports;
       total_size += VOICE_ROUNDTO8(port_info_size);
       port_info_size = sizeof(voice_module_port_t) * pVptx->modules.ss_capi.port_info.port_num_info.num_output_ports;
       total_size += VOICE_ROUNDTO8(port_info_size);
   }

   for(i=0; i < pVptx->modules.num_modules; i++)
   {
       curr_module_ptr = &pVptx->modules.modules_array[i]; // set it to the current module
       port_info_size = sizeof(voice_module_port_t) * curr_module_ptr->port_info.port_num_info.num_input_ports;
       total_size += VOICE_ROUNDTO8(port_info_size);
       port_info_size = sizeof(voice_module_port_t) * curr_module_ptr->port_info.port_num_info.num_output_ports;
       total_size += VOICE_ROUNDTO8(port_info_size);
   }

   return total_size;
}

static void vptx_set_port_info_mem(vptx_t *pVptx, int8_t **mem)
{
   uint32_t i = 0;
   voice_capi_module_t *curr_module_ptr = &pVptx->modules.modules_array[0]; // set it to the first module
   int8_t *pMem = *mem;

   if(pVptx->modules.ss_capi.mem_size_allocated > 0)
   {
      pVptx->modules.ss_capi.port_info.input_port = (voice_module_port_t *)pMem;
      pMem += (VOICE_ROUNDTO8(sizeof(voice_module_port_t) * pVptx->modules.ss_capi.port_info.port_num_info.num_input_ports));

      pVptx->modules.ss_capi.port_info.output_port = (voice_module_port_t *)pMem;
      pMem += (VOICE_ROUNDTO8(sizeof(voice_module_port_t) * pVptx->modules.ss_capi.port_info.port_num_info.num_output_ports));
   }

   for(i=0; i < pVptx->modules.num_modules; i++)
   {
       curr_module_ptr = &pVptx->modules.modules_array[i];
       curr_module_ptr->port_info.input_port = (voice_module_port_t *)pMem;
       pMem += (VOICE_ROUNDTO8(sizeof(voice_module_port_t) * curr_module_ptr->port_info.port_num_info.num_input_ports));

       curr_module_ptr->port_info.output_port = (voice_module_port_t *)pMem;
       pMem += (VOICE_ROUNDTO8(sizeof(voice_module_port_t) * curr_module_ptr->port_info.port_num_info.num_output_ports));
   }

   *mem = pMem;
   return;
}

ADSPResult vptx_allocate_mem(vptx_t* pVptx)
{
   ADSPResult nResult = ADSP_EOK;
   int8_t i;
   uint32_t nTotalSize = 0, max_module_stack_needed = 0, stack_size = 0;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   uint32_t resampler_out_buf_size = 0;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_allocate_mem begin session(%x)",(int)pVptx->session.session_num);

   // For all other topologies including none topology
   // Iterate over array and do these things:
   // 1. Update internal stuff, like module id etc.
   // 2. Update if it's not a CAPI for special handling in later stages
   if (FALSE == pVptx->reconfig_state.retain_amdb_handle)
   {
      pVptx->modules.handle_info_ptr = (adsp_amdb_module_handle_info_t*)qurt_elite_memory_malloc(sizeof(adsp_amdb_module_handle_info_t)*pVptx->modules.num_modules, QURT_ELITE_HEAP_DEFAULT);
      if (NULL == pVptx->modules.handle_info_ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Failed to allocate memory for the module info structure for AMDB.");
         return ADSP_ENOMEMORY;
      }
      
      vptx_set_module_array(pVptx);
            
      for (i = 0 ; i < (int32_t)pVptx->modules.num_modules; i++)
      {
         // Fill the structure to query the handles from AMDB
         pVptx->modules.handle_info_ptr[i].interface_type = CAPI_V2;
         pVptx->modules.handle_info_ptr[i].type = AMDB_MODULE_TYPE_GENERIC;
         pVptx->modules.handle_info_ptr[i].id1 = pVptx->modules.modules_array[i].module_id;
         pVptx->modules.handle_info_ptr[i].id2 = 0;
         pVptx->modules.handle_info_ptr[i].h.capi_v2_handle = NULL;
         pVptx->modules.handle_info_ptr[i].result = ADSP_EFAILED;
      }

      /*
       * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
       * happens using a thread pool using threads of very low priority. This can cause the current thread
       * to be blocked because of a low priority thread. If this is not desired, a callback function
       * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
       * thread can then handle other tasks in parallel.
       */
      adsp_amdb_get_modules_request(pVptx->modules.handle_info_ptr, pVptx->modules.num_modules, NULL, NULL);
   }

   voice_capi_module_t* curr_module_ptr = pVptx->modules.modules_array;

   for(i = 0; i < (int32_t)pVptx->modules.num_modules; i++)
   {
      curr_module_ptr->svc_ptr = pVptx;
      vptx_check_for_capi_module(curr_module_ptr);

      if(TRUE == curr_module_ptr->is_capi)
      {
         // populate function pointers from AMDB
         if(FALSE == pVptx->reconfig_state.retain_amdb_handle)
         {
            nResult = pVptx->modules.handle_info_ptr[i].result;
            if(ADSP_FAILED(nResult))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: failed to get AMDB handle for moduleId(0x%lx), session(0x%lx)",curr_module_ptr->module_id,pVptx->session.session_num);
               return nResult;
            }

            if ((CAPI_V2 != pVptx->modules.handle_info_ptr[i].interface_type)&&(STUB != pVptx->modules.handle_info_ptr[i].interface_type))               {
               MSG_4(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: AMDB handle for moduleId(0x%lx), session(0x%lx) has type %d, when expected %d.",curr_module_ptr->module_id,pVptx->session.session_num, (int)pVptx->modules.handle_info_ptr[i].interface_type, (int)CAPI_V2);
               return ADSP_EUNSUPPORTED;
            }

            //dbg: MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: AMDB handle for moduleId(0x%lx) = (0x%p)",curr_module_ptr->module_id,pVptx->modules.handle_info_ptr[i].h.capi_v2_handle);
            if (STUB != pVptx->modules.handle_info_ptr[i].interface_type)
            {
               curr_module_ptr->amdb_handle_ptr = (void*)pVptx->modules.handle_info_ptr[i].h.capi_v2_handle;
            }
            else
            {
               curr_module_ptr->is_virtual_stub = TRUE;
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VPTX moduleId(0x%lx) is virtually stubbed  session(0x%lx)",curr_module_ptr->module_id,pVptx->session.session_num);

            }

         }

         if(!curr_module_ptr->is_virtual_stub)
         {
            // get static props and the module memory
            pVptx->memory.module_size[i] = voice_capi_get_static_prop(curr_module_ptr);
         }


         if(curr_module_ptr->fwk_extn_ptr)
         {
            vptx_get_additonal_module_static_property(pVptx,curr_module_ptr);
         }
         //check max stack size
         if(curr_module_ptr->stack_size > max_module_stack_needed)
         {
            max_module_stack_needed = curr_module_ptr->stack_size;
         }
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: get size for unknown non-capi moduleid(0x%lx) is called, session(0x%lx)",curr_module_ptr->module_id, pVptx->session.session_num);
      }

      curr_module_ptr->port_info.port_num_info.num_input_ports = 1;  // setting input port for all modules as 1 by default, this may change based on FWK EXTNS
      curr_module_ptr->port_info.port_num_info.num_output_ports = 1; // setting output port for all modules as 1 by default, this may change based on FWK EXTNS

      // assign correct media format to use
      if(pVptx->modules.special_capis[ECNS_CAPI])
      {
         //module is either post EC or EC itself or AVC
         if(curr_module_ptr == pVptx->modules.special_capis[ECNS_CAPI])
         {
            curr_module_ptr->media_format = VOICE_CAPI_MEDIA_FMT_EC;
            curr_module_ptr->port_info.port_num_info.num_input_ports = pVptx->ec_capi_info.num_port.num_input_ports;
            curr_module_ptr->port_info.port_num_info.num_output_ports = pVptx->ec_capi_info.num_port.num_output_ports;
         }
         else if(curr_module_ptr == pVptx->modules.special_capis[AVCRVE_CAPI])
         {
            curr_module_ptr->media_format = VOICE_CAPI_MEDIA_FMT_AVC_RVE;
            curr_module_ptr->port_info.port_num_info.num_input_ports = pVptx->ec_capi_info.num_port.num_output_ports;
         }
         else
         {
            curr_module_ptr->media_format = VOICE_CAPI_MEDIA_FMT_POST_EC;
         }
      }
      else
      {
         //can only be pre EC in this case
         curr_module_ptr->media_format = VOICE_CAPI_MEDIA_FMT_PRE_EC;
      }

      curr_module_ptr++;
   }

   //By this time pVptx->modules.special_capis[ECNS_CAPI] will be valid is EC is not virtually stubbed
   // check is EC module is virtually stubbed
   if((!pVptx->modules.special_capis[ECNS_CAPI]) && (pVptx->io.near_in.mediatype.num_channels != 1))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! Topo creation failure as topo is created with multichannel (%d) but EC is virtually stubbed", pVptx->io.near_in.mediatype.num_channels);
      return ADSP_EFAILED;
   }

   // Create SS capi in following case.
   // 1. EC is not present. This is aplicable for DL v1 topologies as well
   // 2. EC does no support rate matching
   // determine whether to initialize fwk level ss_capi
   if( ( (pVptx->modules.special_capis[ECNS_CAPI]) && (FALSE == pVptx->ec_capi_info.ecns_rate_matching_info.is_rate_matching_supported) ) || (!pVptx->modules.special_capis[ECNS_CAPI]) )
   {
      pVptx->modules.ss_capi.module_id = VOICE_MODULE_SAMPLE_SLIP;
      pVptx->modules.ss_capi.module_index = CAPI_V2_DATA_FORMAT_INVALID_VAL;  //it is called at the start if the topology is created by framework
      pVptx->modules.ss_capi.svc_ptr = pVptx;
      pVptx->modules.ss_capi.is_capi = TRUE;

      // query function pointers from AMDB
      pVptx->modules.ss_handle_info.interface_type = CAPI_V2;
      pVptx->modules.ss_handle_info.type = AMDB_MODULE_TYPE_GENERIC;
      pVptx->modules.ss_handle_info.id1 = (int)pVptx->modules.ss_capi.module_id;
      pVptx->modules.ss_handle_info.id2 = 0;
      pVptx->modules.ss_handle_info.h.capi_v2_handle = NULL;
      pVptx->modules.ss_handle_info.result = ADSP_EFAILED;

      /*
       * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
       * happens using a thread pool using threads of very low priority. This can cause the current thread
       * to be blocked because of a low priority thread. If this is not desired, a callback function
       * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
       * thread can then handle other tasks in parallel.
       */
      adsp_amdb_get_modules_request(&pVptx->modules.ss_handle_info, 1, NULL, NULL);

      if(ADSP_FAILED(pVptx->modules.ss_handle_info.result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: failed to get AMDB handle for SS. session(0x%lx)",pVptx->session.session_num);
         return pVptx->modules.ss_handle_info.result;
      }


      if (CAPI_V2 != pVptx->modules.ss_handle_info.interface_type)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: AMDB handle for SS, session(0x%lx) has type %d, when expected %d.",pVptx->session.session_num, (int)pVptx->modules.ss_handle_info.interface_type, (int)CAPI_V2);
         return ADSP_EUNSUPPORTED;
      }
      if (STUB==pVptx->modules.ss_handle_info.interface_type)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Unexpected Virtual stubbing of Sample Slip, session(0x%lx) type %d ",pVptx->session.session_num, (int)CAPI_V2);
         return ADSP_EFAILED;
      }

      pVptx->modules.ss_capi.amdb_handle_ptr = (void*)pVptx->modules.ss_handle_info.h.capi_v2_handle;

      voice_capi_get_static_prop(&pVptx->modules.ss_capi);

      //Update stack size again
      if(pVptx->modules.ss_capi.stack_size > max_module_stack_needed)
      {
         max_module_stack_needed = pVptx->modules.ss_capi.stack_size;
      }
      pVptx->modules.ss_capi.media_format = VOICE_CAPI_MEDIA_FMT_EC;

      // input and output ports for sample slip will be same
      if(pVptx->modules.special_capis[ECNS_CAPI])
      {
         pVptx->modules.ss_capi.port_info.port_num_info.num_input_ports = pVptx->ec_capi_info.num_port.num_input_ports;
         pVptx->modules.ss_capi.port_info.port_num_info.num_output_ports = pVptx->ec_capi_info.num_port.num_input_ports;
      }
      else
      {
          pVptx->modules.ss_capi.port_info.port_num_info.num_input_ports = 1;
          pVptx->modules.ss_capi.port_info.port_num_info.num_output_ports = 1;

      }
   }
   else
   {
      pVptx->modules.ss_capi.mem_size_allocated = 0;
   }

   // Allocate output buffer for resampler
   if(pVptx->modules.special_capis[DYN_RESAMP])
   {
      resampler_out_buf_size = VOICE_ROUNDTO8(pVptx->io.output.buf_samples << 1);
   }

   //add framework stack
   max_module_stack_needed += VPTX_SELF_STACK_IN_BYTES;
   // Update stack size if zero (indicating dynamic topologies did not update it)
   if(0 == stack_size)
   {
      stack_size = max_module_stack_needed>VPTX_MIN_STACK_IN_BYTES? max_module_stack_needed:VPTX_MIN_STACK_IN_BYTES;
   }
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vptx max module stack(%lu), stack needed(%lu), current stack(%lu)", max_module_stack_needed, stack_size, pVptx->session.stack_size);

   if( stack_size != pVptx->session.stack_size )
   {
      pVptx->session.stack_changed = TRUE;
      pVptx->session.stack_size = stack_size;
   }

   nTotalSize += vptx_get_port_info_size(pVptx);

   for(uint32_t j = 0 ; j < pVptx->modules.num_modules ; j++)
   {
      //madule_size[i] should be aligned to 8 by now
      nTotalSize             +=   voice_add_guard_band(pVptx->memory.module_size[j]);
   }

   //add memory for SS and resamplers
   nTotalSize += pVptx->modules.ss_capi.mem_size_allocated;
   nTotalSize += resampler_out_buf_size;


   pVptx->memory.pStartAddr = (int8_t *)qurt_elite_memory_malloc(nTotalSize, QURT_ELITE_HEAP_DEFAULT);
   if (NULL == pVptx->memory.pStartAddr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to allocate memory for VPTX modules!! session(%x)",(int)pVptx->session.session_num);
      return ADSP_ENOMEMORY;
   }
   memset( pVptx->memory.pStartAddr, 0, nTotalSize); // clear the memory before use

   // UsageAddr is a byte pointer
   pVptx->memory.nSize = nTotalSize; // size allocated
   pVptx->memory.pUsageAddr = pVptx->memory.pStartAddr; // Usage level pointer


   vptx_set_port_info_mem(pVptx, &pVptx->memory.pUsageAddr);

   // 1. Populate Guard band memory locations allocated after each module and update in guard band pointers array.
   // 2. Initialize guard band memory with known value.
   nResult = voice_init_guard_band( (int8_t**)pVptx->memory.guard_band_ptrs, pVptx->memory.module_size, pVptx->modules.num_modules, pVptx->memory.pUsageAddr);
   if ( ADSP_EOK != nResult )
   {
      return nResult; // Returning error because voice_init_guard_band() is called with NULL pointer in guard_band_ptr[] array.
   }

   // populate input and output media formats
   {
      voice_capi_populate_data_format_struct(&pVptx->ec_capi_info.input_port[0], &pVptx->io.near_in.mediatype);
      voice_capi_populate_data_format_struct(&pVptx->ec_capi_info.input_port[1], &pVptx->io.far_in.mediatype);

      if(pVptx->modules.special_capis[ECNS_CAPI])
      {
          if ((pVptx->ec_capi_info.force_mono_reference) || (1 == pVptx->ec_capi_info.input_port[1].data_format.num_channels))
          {
              // forcing far end number of channels to 1
              // VPTX will still receive data base on actual far number of channels i.e pVptx->io.far_in.mediatype.num_channels
              // but it set the number of far channels to EC capi as 1. Then same will go to sampleslip which is fine as we need not coorect other far channels

              pVptx->ec_capi_info.input_port[1].data_format.num_channels = 1;
              pVptx->ec_capi_info.input_port[1].data_format.channel_type[0] = PCM_CHANNEL_FAR_REF; // for backward compatibility
              MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Forcing reference number of channels to 1 in mediatype info since ECNS does not support multichannel reference");

          }

      }

      //for output media format,ECNS should update output media format using even_cb mechanism.
      //However when ECNS is not present in the topology, take care here.
      //ECNS will not be present in NONE topology and the DL V1 topologies. need to take care of these differently.

      if(!pVptx->modules.special_capis[ECNS_CAPI])
      {
         //If ECNS is not present in the topology update output media format with near input media format
         pVptx->ec_capi_info.output_port[0] = pVptx->ec_capi_info.input_port[0];
         pVptx->ec_capi_info.output_port[0].data_format.num_channels = 1;

         pVptx->ec_capi_info.num_port.num_input_ports = 1; //for none topology
         pVptx->ec_capi_info.num_port.num_output_ports = 1;


         pVptx->ec_capi_info.ecns_lec_output_port_idx = CAPI_V2_DATA_FORMAT_INVALID_VAL;
         pVptx->ec_capi_info.ecns_nr_output_port_idx = CAPI_V2_DATA_FORMAT_INVALID_VAL;
         pVptx->ec_capi_info.ecns_prim_output_port_idx = 0;
      }
   }

   capi_v2_heap_id_t heap_id             = { (uint32_t)QURT_ELITE_HEAP_DEFAULT };
   capi_v2_port_info_t port_info         = {TRUE,TRUE,0};
   capi_v2_event_callback_info_t cb_info = { vptx_capi_v2_cb_func, NULL };

   capi_v2_session_identifier_t sess_identifier;
   sess_identifier.service_id = (uint16_t)((ELITE_VOICEPROCTX_SVCID & 0xFFFF0000ul) >> 16);
   sess_identifier.session_id = pVptx->session.session_num;
   //MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Vptx session identifier svc %d, session %d", sess_identifier.service_id, sess_identifier.session_id);

   //Set up default init prop list. The order of the prop should not be changed
   capi_v2_prop_t init_props[]= {
         { CAPI_V2_HEAP_ID,             { reinterpret_cast<int8_t *>(&heap_id), sizeof(heap_id),  sizeof(heap_id) }, port_info },
         { CAPI_V2_EVENT_CALLBACK_INFO, { reinterpret_cast<int8_t *>(&cb_info), sizeof(cb_info),  sizeof(cb_info) }, port_info },
         { CAPI_V2_PORT_NUM_INFO,       { NULL, 0, 0 }, port_info },
         { CAPI_V2_INPUT_MEDIA_FORMAT,  { NULL, 0, 0 }, port_info },
         { CAPI_V2_SESSION_IDENTIFIER,  { reinterpret_cast<int8_t *>(&sess_identifier), sizeof(sess_identifier), sizeof(sess_identifier) }, port_info }
   };
   capi_v2_proplist_t init_props_list = {VOICE_SIZE_OF_ARRAY(init_props),init_props};

   uint32_t port_info_idx = 2, input_media_format_idx = 3;

   /*
    * Following is the input media format for modules
    * 1. MODULE_BEFORE_EC  --> Media format is ECNS port 0 i.e Near port
    * 2. MODULE_EC         --> ECNS media format
    * 3. MODULE_AVC        --> ECNs output media format. All ports.
    * 4. MODULE_AFTER_EC   --> ECNS output port[0]  media format.
    */

   capi_v2_port_num_info_t pre_ec_port_num = {1,1}; // populate these accordingly
   capi_v2_port_num_info_t post_ec_port_num = {1,1};
   capi_v2_port_num_info_t ec_port_num = pVptx->ec_capi_info.num_port;
   capi_v2_port_num_info_t avc_rve_port_num = {pVptx->ec_capi_info.num_port.num_output_ports,1};

   // List of INPUT_MEDIA_FORMATs sorted in order of enums in voice_capi_media_format_t. That way you can just index into it using the enum
   //TODO: move this to a separate function
   capi_v2_buf_t port_num_buf[] = {
         { reinterpret_cast<int8_t *>(&pre_ec_port_num) , sizeof(capi_v2_port_num_info_t), sizeof(capi_v2_port_num_info_t) },
         { reinterpret_cast<int8_t *>(&ec_port_num),      sizeof(capi_v2_port_num_info_t), sizeof(capi_v2_port_num_info_t) },
         { reinterpret_cast<int8_t *>(&post_ec_port_num), sizeof(capi_v2_port_num_info_t), sizeof(capi_v2_port_num_info_t) },
         { reinterpret_cast<int8_t *>(&avc_rve_port_num), sizeof(capi_v2_port_num_info_t), sizeof(capi_v2_port_num_info_t) },
   };

   //If ECNS is present in the topology, pVptx->ec_capi_info.output_port wont be updated by this time. once ECNS init is called, it will be updated via cb function
   capi_v2_buf_t media_format_buf[] = {
         { reinterpret_cast<int8_t *>(&pVptx->ec_capi_info.input_port[0]), sizeof(voice_capi_data_format_struct_t)*pre_ec_port_num.num_input_ports, sizeof(voice_capi_data_format_struct_t)*pre_ec_port_num.num_input_ports  },
         { reinterpret_cast<int8_t *>(&pVptx->ec_capi_info.input_port[0]),  sizeof(voice_capi_data_format_struct_t)*ec_port_num.num_input_ports, sizeof(voice_capi_data_format_struct_t)*ec_port_num.num_input_ports  },
         { reinterpret_cast<int8_t *>(&pVptx->ec_capi_info.output_port[0]), sizeof(voice_capi_data_format_struct_t)*post_ec_port_num.num_input_ports, sizeof(voice_capi_data_format_struct_t)*post_ec_port_num.num_input_ports  },
         { reinterpret_cast<int8_t *>(&pVptx->ec_capi_info.output_port[0]), sizeof(voice_capi_data_format_struct_t)*avc_rve_port_num.num_input_ports, sizeof(voice_capi_data_format_struct_t)*avc_rve_port_num.num_input_ports  },
   };

   voice_capi_data_format_struct_t *media_fmt;

   curr_module_ptr = pVptx->modules.modules_array;

   for(i = 0; i < (int32_t)pVptx->modules.num_modules; i++)
   {
      //check media format to use and update internal init prop accordingly
      if(TRUE == curr_module_ptr->is_capi)
      {
         // For backward compatibility, provide mic path channel map to EC
         if(pVptx->modules.special_capis[ECNS_CAPI] == curr_module_ptr)
         {
            vptx_update_ec_channel_map(pVptx, (voice_capi_data_format_struct_t *)media_format_buf[curr_module_ptr->media_format].data_ptr,0);
         }

         for(uint32_t port_id = 0; port_id < curr_module_ptr->port_info.port_num_info.num_input_ports; port_id++)
         {
             media_fmt = (voice_capi_data_format_struct_t *)media_format_buf[curr_module_ptr->media_format].data_ptr;
             memscpy(&curr_module_ptr->port_info.input_port[port_id].media_fmt, sizeof(voice_capi_data_format_struct_t), &media_fmt[port_id], sizeof(voice_capi_data_format_struct_t));
         }

         if((pVptx->modules.special_capis[ECNS_CAPI] != curr_module_ptr)
	  && (pVptx->modules.special_capis[DYN_RESAMP] != curr_module_ptr))			 // ECNS and dyn resamp output media format will be updated when module returns output media format
         {
            for(uint32_t port_id = 0; port_id < curr_module_ptr->port_info.port_num_info.num_output_ports; port_id++)
            {
                media_fmt = (voice_capi_data_format_struct_t *)media_format_buf[curr_module_ptr->media_format].data_ptr;
                memscpy(&curr_module_ptr->port_info.output_port[port_id].media_fmt, sizeof(voice_capi_data_format_struct_t), &media_fmt[port_id], sizeof(voice_capi_data_format_struct_t));
            }
         }

         memscpy(&init_props[input_media_format_idx].payload, sizeof(capi_v2_buf_t), &media_format_buf[curr_module_ptr->media_format], sizeof(capi_v2_buf_t));
         memscpy(&init_props[port_info_idx].payload, sizeof(capi_v2_buf_t), &port_num_buf[curr_module_ptr->media_format], sizeof(capi_v2_buf_t));

         cb_info.event_context = curr_module_ptr;
         curr_module_ptr->module_ptr = (capi_v2_t*)pVptx->memory.pUsageAddr;

         if (!curr_module_ptr->is_virtual_stub)
         {
            // CAPI assumes modules are enabled by default, we can keep the same behavior here
            // If a module does not support the media fmt internally or disabled by default, it need to raise process state.
            curr_module_ptr->is_enabled = TRUE;

            capi_result = adsp_amdb_capi_v2_init_f((adsp_amdb_capi_v2_t*)curr_module_ptr->amdb_handle_ptr, curr_module_ptr->module_ptr, &init_props_list);
            if(CAPI_V2_EOK != capi_result)
            {
               //check if the init failed for ECNS module. If it is ECNS module then fail topology creation.
               if(curr_module_ptr == pVptx->modules.special_capis[ECNS_CAPI])
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! ECNS Module id(0x%lx) failed to init with error %ld, failing topo creation!!", curr_module_ptr->module_id, capi_result);
                  curr_module_ptr->module_ptr = NULL; //assign module pointer to NULL so that later it is not used
                  return capi_v2_err_to_adsp_result(capi_result);
               }
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! Module id(0x%lx) failed to init with error %ld, Proceeding!", curr_module_ptr->module_id, capi_result);
               curr_module_ptr->module_ptr = NULL;
               curr_module_ptr->is_enabled = FALSE;
            }
            else
            {
               if(curr_module_ptr->fwk_extn_ptr)
               {
                  nResult = vptx_handle_fwk_extension(pVptx,curr_module_ptr);
                  if(ADSP_FAILED(nResult))
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: failed to handle fwk extn, session(%#lx)",pVptx->session.session_num);
                     return nResult;
                  }
               }
            }
         }
         else
         {
            curr_module_ptr->is_enabled = FALSE;         // disabling enable flag for stubbed module
            curr_module_ptr->module_ptr = NULL;
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: bypassing init for vitual stubbed vptx module (0x%lx), session(%#lx)",curr_module_ptr->module_id,pVptx->session.session_num);
         }

         if(pVptx->modules.special_capis[ECNS_CAPI] == curr_module_ptr)
         {
            vptx_update_ec_channel_map(pVptx, (voice_capi_data_format_struct_t *)media_format_buf[curr_module_ptr->media_format].data_ptr,1);
         }

         voice_print_media_fmt(curr_module_ptr);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: init for unknown non-capi moduleid(0x%lx) is called, session(0x%lx)",curr_module_ptr->module_id, pVptx->session.session_num);
      }

      // take care of pUsageAddr
      pVptx->memory.pUsageAddr += voice_add_guard_band(curr_module_ptr->mem_size_allocated);
      curr_module_ptr++;
   }

   //create ss_capi here
   if(VOICE_MODULE_SIZE_ZERO != pVptx->modules.ss_capi.mem_size_allocated)
   {
      curr_module_ptr = &pVptx->modules.ss_capi;
      for(uint32_t port_id = 0; port_id < curr_module_ptr->port_info.port_num_info.num_input_ports; port_id++)
      {
          media_fmt = (voice_capi_data_format_struct_t *)media_format_buf[curr_module_ptr->media_format].data_ptr;
          memscpy(&curr_module_ptr->port_info.input_port[port_id].media_fmt, sizeof(voice_capi_data_format_struct_t), &media_fmt[port_id], sizeof(voice_capi_data_format_struct_t));
      }
      for(uint32_t port_id = 0; port_id < curr_module_ptr->port_info.port_num_info.num_output_ports; port_id++)
      {
          media_fmt = (voice_capi_data_format_struct_t *)media_format_buf[curr_module_ptr->media_format].data_ptr;
          memscpy(&curr_module_ptr->port_info.output_port[port_id].media_fmt, sizeof(voice_capi_data_format_struct_t), &media_fmt[port_id], sizeof(voice_capi_data_format_struct_t));
      }
      memscpy(&init_props[input_media_format_idx].payload, sizeof(capi_v2_buf_t), &media_format_buf[pVptx->modules.ss_capi.media_format], sizeof(capi_v2_buf_t));
      memscpy(&init_props[port_info_idx].payload, sizeof(capi_v2_buf_t), &port_num_buf[pVptx->modules.ss_capi.media_format], sizeof(capi_v2_buf_t));

      pVptx->modules.ss_capi.module_ptr = (capi_v2_t*)pVptx->memory.pUsageAddr;
      cb_info.event_context = &pVptx->modules.ss_capi;

      capi_result = adsp_amdb_capi_v2_init_f((adsp_amdb_capi_v2_t*)pVptx->modules.ss_capi.amdb_handle_ptr, pVptx->modules.ss_capi.module_ptr, &init_props_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! SS capi(0x%lx) failed to init with error %ld, failing topo creation!", pVptx->modules.ss_capi.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }

      pVptx->modules.ss_capi.is_enabled = TRUE;

      capi_v2_buf_t param_data_buf;
      set_ss_operating_mode_t ss_operating_mode;

      if ((pVptx->modules.special_capis[ECNS_CAPI]))
      {
         ss_operating_mode.operating_mode= SS_MODE_1;  //EC does not support rate matching
      }
      else
      {
         ss_operating_mode.operating_mode= SS_MODE_4;   //Topo does not contain EC
      }

      param_data_buf.data_ptr =(int8_t*) &ss_operating_mode;
      param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(set_ss_operating_mode_t);

      pVptx->modules.ss_capi.module_ptr->vtbl_ptr->set_param(pVptx->modules.ss_capi.module_ptr,SS_PARAM_MODE_OPER,&port_info,&param_data_buf);

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: creating SS capi from VPTX service operating mode(%d), session(%lx)",ss_operating_mode.operating_mode,pVptx->session.session_num);
   }
   pVptx->memory.pUsageAddr += pVptx->modules.ss_capi.mem_size_allocated;

   // Resampler memory allocation      
   pVptx->modules.resampler_out_buf = pVptx->memory.pUsageAddr;
   pVptx->memory.pUsageAddr +=resampler_out_buf_size;


   nResult = voice_check_guard_band((int8_t**)pVptx->memory.guard_band_ptrs, pVptx->modules.num_modules, TX, pVptx->session.session_num);
   if ( ADSP_EOK != nResult )
   {
      return nResult; // returning error as corruption is detected in check_guard_band()
   }

   // Sanity check to see the allocated memory was enough
   if ((uint32_t)(pVptx->memory.pUsageAddr - pVptx->memory.pStartAddr) <= pVptx->memory.nSize)
   {
      nResult = ADSP_EOK;
   }
   else
   {
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Error allocating memory result(%d) session(%x), usage: %d, allocated: %d",nResult,(int)pVptx->session.session_num,(int) (pVptx->memory.pUsageAddr - pVptx->memory.pStartAddr),(int) pVptx->memory.nSize);
      nResult = ADSP_ENOMEMORY;
      return nResult;
   }

   if(pVptx->modules.special_capis[DYN_RESAMP])
   {
      vptx_set_dyn_resampler_media_fmt(pVptx, pVptx->modules.special_capis[DYN_RESAMP]);
   }
   nResult = vptx_set_topo_framesize(pVptx);
   if(ADSP_FAILED(nResult))
   {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: VPTX session(%lx) set topology framesize failed with error (%lu), bailing out topo creation", pVptx->session.session_num, nResult);
       return nResult;
   }
   vptx_allocate_process_buffers(pVptx);

   if(VPTX_AUDIO == pVptx->session.audio_instance_flag)
   {
      vptx_allocate_circ_bufs(pVptx);
   }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_allocate_mem end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

ADSPResult vptx_allocate_mem_free(vptx_t* pVptx)
{
   if (pVptx->memory.pStartAddr)
   {
      qurt_elite_memory_free(pVptx->memory.pStartAddr);
      pVptx->memory.pStartAddr = NULL;
   }

   if(pVptx->memory.pBufStartAddr)
   {
     qurt_elite_memory_free(pVptx->memory.pBufStartAddr);
     pVptx->memory.pBufStartAddr = NULL;
   }

   pVptx->modules.pScrath_mem = NULL;
   return ADSP_EOK;
}

static void vptx_get_near_far_sample_drift(vptx_t* pVptx, int32_t drift[2])
{
   instantaneous_drift_t inst_drift;
   inst_drift.inst_drift_in_us = drift;
   drift_samples_t drift_samples;
   capi_v2_buf_t param_data_buf;
   capi_v2_port_info_t port_info;
   port_info.is_valid = FALSE;
   int16_t slip_stuff_samples_near, slip_stuff_samples_far;
   capi_v2_err_t result = CAPI_V2_EOK;

   param_data_buf.data_ptr = (int8_t*)&inst_drift;
   param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(instantaneous_drift_t);
   // calculate number of slip/stuff samples
   if (pVptx->modules.ss_capi.module_ptr)  // SS created by service
   {
      pVptx->modules.ss_capi.module_ptr->vtbl_ptr->set_param(pVptx->modules.ss_capi.module_ptr,
            SS_PARAM_INST_DRIFT,&port_info,&param_data_buf);

   }
   else if(pVptx->modules.special_capis[ECNS_CAPI])
   {
      pVptx->modules.special_capis[ECNS_CAPI]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[ECNS_CAPI]->module_ptr,
            SS_PARAM_INST_DRIFT,&port_info,&param_data_buf);
   }

   // get the actual drift to be corrected in current frame=
   memset(drift,0,sizeof(uint32_t)*2);

   drift_samples.drift_in_samples = drift;  // reuse the same buffer drift as was used for setting instant drift
   param_data_buf.data_ptr = (int8_t*)&drift_samples;
   param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(drift_samples_t);

   if (pVptx->modules.ss_capi.module_ptr)  // SS created by service
   {
      pVptx->modules.ss_capi.module_ptr->vtbl_ptr->get_param(pVptx->modules.ss_capi.module_ptr,
            SS_PARAM_DRIFT_SAMPLES,&port_info,&param_data_buf);
   }
   else if(pVptx->modules.special_capis[ECNS_CAPI])
   {
      result = pVptx->modules.special_capis[ECNS_CAPI]->module_ptr->vtbl_ptr->get_param(pVptx->modules.special_capis[ECNS_CAPI]->module_ptr,
            SS_PARAM_DRIFT_SAMPLES,&port_info,&param_data_buf);
   }


   if(CAPI_V2_EOK == result)
   {
      slip_stuff_samples_near = (int16_t)drift[0];
      slip_stuff_samples_far = (int16_t)drift[1];
   }
   else
   {
      // For whatever reason, if EC module does not succeed for get, the values might be garbage
      // Just use zero instead
      slip_stuff_samples_near = 0;
      slip_stuff_samples_far = 0;
   }

   if ((slip_stuff_samples_near !=0) || (slip_stuff_samples_far !=0))
   {
      //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Drift in VPTX to be corrected NEAR (%d) FAR (%d)",slip_stuff_samples_near,slip_stuff_samples_far);

      // NEar - adjust buffer samples left in sample slip buffer
      vptx_cal_sample_slip_stuff(slip_stuff_samples_near,pVptx->io.prebuffer_samples,&(pVptx->ratematch.ss_buf_samples_near),pVptx->session.session_num,NEAR);

      // Far - adjust buffer samples left in sample slip buffer
      vptx_cal_sample_slip_stuff(slip_stuff_samples_far,pVptx->io.prebuffer_samples,&(pVptx->ratematch.ss_buf_samples_far),pVptx->session.session_num,FAR);

   }
}


static bool_t vptx_can_module_process(voice_capi_module_t *curr_module_ptr, capi_v2_stream_data_t *In[], capi_v2_stream_data_t *Out[],
                                     bool_t is_rate_matching_supported, int32_t *nSlipStuffSamples)
{
   uint32_t data_threshold = 0;
   uint32_t bytes_per_sample = 0;

   // check if input has enough samples
   for(uint32_t i = 0; i < curr_module_ptr->port_info.port_num_info.num_input_ports; i++)
   {
       voice_module_port_t *pPort_info = &curr_module_ptr->port_info.input_port[i];
       bytes_per_sample = pPort_info->media_fmt.data_format.bits_per_sample >> 3;
       data_threshold = (is_rate_matching_supported)?(pPort_info->data_threshold - nSlipStuffSamples[i]*bytes_per_sample):pPort_info->data_threshold;
       for(uint32_t ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
       {
          if(data_threshold > In[i]->buf_ptr[ch].actual_data_len)
          {
             return FALSE;
          }
       }
   }

   // check if output has enough memory
   for(uint32_t i = 0; i < curr_module_ptr->port_info.port_num_info.num_output_ports; i++)
   {
       voice_module_port_t *pPort_info = &curr_module_ptr->port_info.output_port[i];
       data_threshold = pPort_info->data_threshold;
       for(uint32_t ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
       {
          if(data_threshold > Out[i]->buf_ptr[ch].max_data_len)
          {
             return FALSE;
          }
       }
   }

   return TRUE;
}

static ADSPResult vptx_process_port_threshold_module(voice_capi_module_t *curr_module_ptr, capi_v2_stream_data_t *In[], capi_v2_stream_data_t *Out[],
                                     bool_t is_rate_matching_supported, int32_t *nSlipStuffSamples, int8_t* pModule_scratch_mem)
{
   uint32_t input_data, output_data, data_threshold, i, ch;
   voice_module_port_t *pPort_info;
   capi_v2_buf_t*   temp_input_buf[VPTX_MAX_INPUT_PORT];
   capi_v2_buf_t*   temp_output_buf[ECNS_MAX_OUTPUT_PORT];
   uint32_t        unread_data[VPTX_MAX_INPUT_PORT] = {0};
   uint32_t bytes_per_sample;

   if(NULL == pModule_scratch_mem)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ERROR!! vptx_process_port_threshold_module No scratch memory allocated");
      curr_module_ptr->is_enabled = FALSE;

      return ADSP_ENOMEMORY;
   }

   for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_input_ports; i++)
   {
       pPort_info = &curr_module_ptr->port_info.input_port[i];
       temp_input_buf[i] = (capi_v2_buf_t *)pModule_scratch_mem;
       pModule_scratch_mem += VOICE_ROUNDTO8(sizeof(capi_v2_buf_t))*pPort_info->media_fmt.data_format.num_channels;
   }

   // store the initial stream pointers
   for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_input_ports; i++)
   {
       pPort_info = &curr_module_ptr->port_info.input_port[i];
       for(ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
       {
           memscpy(&temp_input_buf[i][ch], sizeof(capi_v2_buf_t), &In[i]->buf_ptr[ch], sizeof(capi_v2_buf_t));
       }
   }

   if(curr_module_ptr->output_buf_index != curr_module_ptr->input_buf_index)  // if module is not inplace only then we need output stream scratch allocation
   {
      for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_output_ports; i++)
      {
          pPort_info = &curr_module_ptr->port_info.output_port[i];
          temp_output_buf[i] = (capi_v2_buf_t *)pModule_scratch_mem;
          pModule_scratch_mem += VOICE_ROUNDTO8(sizeof(capi_v2_buf_t))*pPort_info->media_fmt.data_format.num_channels;
      }

      for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_output_ports; i++)
      {
          pPort_info = &curr_module_ptr->port_info.output_port[i];
          for(ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
          {
              memscpy(&temp_output_buf[i][ch], sizeof(capi_v2_buf_t), &Out[i]->buf_ptr[ch], sizeof(capi_v2_buf_t));
          }
      }
   }

   while(vptx_can_module_process(curr_module_ptr, In, Out, is_rate_matching_supported, nSlipStuffSamples))
   {
      for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_input_ports; i++)
      {
          pPort_info = &curr_module_ptr->port_info.input_port[i];
          bytes_per_sample = pPort_info->media_fmt.data_format.bits_per_sample >> 3;
          data_threshold = pPort_info->data_threshold;
          for(ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
          {
              unread_data[i] = In[i]->buf_ptr[ch].actual_data_len;
              input_data = (is_rate_matching_supported)?((unread_data[i] > (data_threshold - nSlipStuffSamples[i]*bytes_per_sample))?data_threshold:unread_data[i]):data_threshold;
              In[i]->buf_ptr[ch].actual_data_len = input_data;
          }
      }

      curr_module_ptr->module_ptr->vtbl_ptr->process(curr_module_ptr->module_ptr,In, Out);

      if(curr_module_ptr->output_buf_index != curr_module_ptr->input_buf_index)
      {
         for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_output_ports; i++)
         {
             pPort_info = &curr_module_ptr->port_info.output_port[i];
             for(ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
             {
                 output_data = Out[i]->buf_ptr[ch].actual_data_len;
                 Out[i]->buf_ptr[ch].data_ptr += output_data;
                 Out[i]->buf_ptr[ch].max_data_len -= output_data;
             }
         }
      }

      for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_input_ports; i++)
      {
          pPort_info = &curr_module_ptr->port_info.input_port[i];
          for(ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
          {
              input_data = In[i]->buf_ptr[ch].actual_data_len;
              In[i]->buf_ptr[ch].data_ptr += input_data;
              In[i]->buf_ptr[ch].max_data_len -= input_data;
              In[i]->buf_ptr[ch].actual_data_len = unread_data[i] - input_data;
          }
      }

   }

   if(curr_module_ptr->output_buf_index != curr_module_ptr->input_buf_index)
   {
      for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_output_ports; i++)
      {
          pPort_info = &curr_module_ptr->port_info.output_port[i];
          for(ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
          {
              output_data = (uint32_t)(Out[i]->buf_ptr[ch].data_ptr - temp_output_buf[i][ch].data_ptr);
              memscpy(&Out[i]->buf_ptr[ch], sizeof(capi_v2_buf_t), &temp_output_buf[i][ch], sizeof(capi_v2_buf_t));
              Out[i]->buf_ptr[ch].actual_data_len = output_data;
          }
      }

      for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_input_ports; i++)
      {
          pPort_info = &curr_module_ptr->port_info.input_port[i];
          for(ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
          {
              memscpy(&In[i]->buf_ptr[ch], sizeof(capi_v2_buf_t), &temp_input_buf[i][ch], sizeof(capi_v2_buf_t));
          }

      }
   }
   else
   {
      for(i = 0; i < curr_module_ptr->port_info.port_num_info.num_input_ports; i++)
      {
          pPort_info = &curr_module_ptr->port_info.input_port[i];
          for(ch = 0; ch < pPort_info->media_fmt.data_format.num_channels; ch++)
          {
              output_data = (uint32_t)(In[i]->buf_ptr[ch].data_ptr - temp_input_buf[i][ch].data_ptr);
              memscpy(&In[i]->buf_ptr[ch], sizeof(capi_v2_buf_t), &temp_input_buf[i][ch], sizeof(capi_v2_buf_t));
              In[i]->buf_ptr[ch].actual_data_len = output_data;   // In case when module raising data threshold is inplace and supports rate matching as well, input data may not be same output data len, hence need to calculate output data written to get actual data len
          }

      }
   }

   return ADSP_EOK;
}

ADSPResult vptx_process(vptx_t* pVptx)
{
   uint32_t process_frame_samples = pVptx->io.near_in.frame_samples; // always 10ms, doesnt vary with sample slipping-stuffing
   int32_t nSlipStuffSamples[VPTX_MAX_INPUT_PORT] = {0};
   uint32_t i = 0;
   int32_t drift[2] = {0,0};   // for two ports
   //pointer to a 2D array of stream_data pointers
   capi_v2_stream_data_t* (*stream_data_ptrs)[NUM_STRM_BUF_IDX][ECNS_MAX_OUTPUT_PORT] = &pVptx->stream_data_ptrs;
   uint32_t bytes_per_sample = pVptx->io.near_in.mediatype.bits_per_sample >> 3;  // bytes per sample for vptx topology


   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_process begin EC enable(%x) session(%x)",pVptx->modules.ecns_struct.enable,(int)pVptx->session.session_num);
   vptx_cal_drift(pVptx,drift);
   // Get samples to correct in this frame
   vptx_get_near_far_sample_drift(pVptx, drift );

   nSlipStuffSamples[0] = drift[0];
   nSlipStuffSamples[1]  = drift[1];

   voice_capi_module_t *curr_module_ptr = pVptx->modules.modules_array;

   // common for all topologies -- read near input samples
   voice_multi_ch_circ_buf_read(&(pVptx->io.near_in.circ_buf), &(pVptx->io.near_in.process_buf_ptr), (int32_t) (pVptx->io.near_in.frame_samples - nSlipStuffSamples[0])); // no sample slipping yet
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Read Near Data to circ Buf %d session(%x)",(int)pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples,(int)pVptx->session.session_num);

   // if not in a voice call and if recording is ON from audio path then zero out the far buffer samples
   if(TRUE  == pVptx->io.force_far_zeros)
   {
      //No drift is calculated in this case and nSlipStuffSamples_far will be 0. corection type should be async so memset framesamples to 0
      //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx memset far to zero, %d samples, session(%x)", (int)(pVptx->io.far_in.frame_samples - nSlipStuffSamples[1]), (int)pVptx->session.session_num);
       for (i=0; i<pVptx->io.far_in.mediatype.num_channels ; i++)
       {
           memset(pVptx->io.far_in.process_buf_ptr[i], 0, (pVptx->io.far_in.frame_samples * bytes_per_sample));
       }
   }
   else
   {
      voice_multi_ch_circ_buf_read(&(pVptx->io.far_in.circ_buf), &(pVptx->io.far_in.process_buf_ptr), (int32_t) (pVptx->io.far_in.frame_samples - nSlipStuffSamples[1]));
      //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Read Far Data to circ Buf %d session(%x)",(int)pVptx->io.far_in.circ_buf.circ_buf[0].unread_samples,(int)pVptx->session.session_num);
   }

   //update actual size based on sample slip drift information
   //for output port, the capi need to update if it is changing the number of samples. for instance ECNS and SS capis.
   for(i = 0; i < pVptx->io.near_in.mediatype.num_channels; i++)
   {
     pVptx->near_input_buf[0][i].actual_data_len = (uint32_t)((pVptx->io.near_in.frame_samples - nSlipStuffSamples[0]) * bytes_per_sample);
     pVptx->near_input_buf[1][i].actual_data_len = (uint32_t)((pVptx->io.near_in.frame_samples - nSlipStuffSamples[0]) * bytes_per_sample);
   }
   for(i = 0; i < pVptx->io.far_in.mediatype.num_channels; i++)
   {
       pVptx->far_input[i].actual_data_len = (uint32_t)((pVptx->io.far_in.frame_samples - nSlipStuffSamples[1]) * bytes_per_sample);
   }

   //If sample slip is created by framework, then call the process at the top created by service
   if (pVptx->modules.ss_capi.module_ptr)
   {
      if(pVptx->modules.ss_capi.port_info.input_port[0].data_threshold > 0)
      {
          curr_module_ptr = &pVptx->modules.ss_capi;
          vptx_process_port_threshold_module(curr_module_ptr, (*stream_data_ptrs)[curr_module_ptr->input_buf_index], (*stream_data_ptrs)[curr_module_ptr->output_buf_index], TRUE, nSlipStuffSamples, pVptx->modules.pScrath_mem);
      }
      else
      {
          pVptx->modules.ss_capi.module_ptr->vtbl_ptr->process(pVptx->modules.ss_capi.module_ptr,(*stream_data_ptrs)[pVptx->modules.ss_capi.input_buf_index],(*stream_data_ptrs)[pVptx->modules.ss_capi.output_buf_index]);

      }
      if(nSlipStuffSamples[0] || nSlipStuffSamples[1])
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx corrected near samples(%ld), far_samples(%ld), session(%#lx)",nSlipStuffSamples[0], nSlipStuffSamples[1], pVptx->session.session_num);
      }
      nSlipStuffSamples[0] = 0;
      nSlipStuffSamples[1] = 0;
   }

   for( i=0; i<pVptx->modules.num_modules; i++)
   {
      if(curr_module_ptr == pVptx->modules.special_capis[ECNS_CAPI])
      {

#if defined(__qdsp6__) && !defined(SIM)
         uint32_t sampling_rate_log_id;
         sampling_rate_log_id = voice_get_sampling_rate_log_id(pVptx->session.sampling_rate);
         {
            // near logging right before ecns process
            int8_t *buf_ptr[6] = {NULL};

            for (uint32_t k=0; k<pVptx->io.near_in.mediatype.num_channels; k++)
            {
               buf_ptr[k] = pVptx->ec_near_input[k]->data_ptr;
            }
            voice_log_buffer( buf_ptr,
                  VOICE_LOG_TAP_POINT_VPTX_ECNEAR_IN,
                  ( pVptx->io.near_in.mediatype.num_channels << 5 ) | (sampling_rate_log_id << 3) | (pVptx->session.session_num | ((TRUE == pVptx->session.audio_instance_flag) ? VPTX_AUDIO_LOG_ID : 0)),
                  qurt_elite_timer_get_time(),
                  pVptx->log_data_format_near,
                  pVptx->session.sampling_rate,
                  pVptx->ec_near_input[0]->actual_data_len,
                  NULL);
         }
         {
            // far logging right before ecns process
            int8_t *buf_ptr[6] = {NULL};
            for (uint32_t k=0; k<pVptx->io.far_in.mediatype.num_channels; k++)
            {
               buf_ptr[k] = pVptx->far_input[k].data_ptr;
            }
            voice_log_buffer( buf_ptr,
                  VOICE_LOG_TAP_POINT_VPTX_ECFAR_IN,
                  ( pVptx->io.near_in.mediatype.num_channels << 5 ) | (sampling_rate_log_id << 3) | (pVptx->session.session_num | ((TRUE == pVptx->session.audio_instance_flag) ? VPTX_AUDIO_LOG_ID : 0)),
                  // deliberately put near number of channels so that the ids of near and far are same and easily
                  // recognizable from the logs
                  qurt_elite_timer_get_time(),
                  pVptx->log_data_format_far,
                  pVptx->session.sampling_rate,
                  pVptx->far_input[0].actual_data_len,
                  NULL);
         }
#endif
         if(curr_module_ptr->port_info.input_port[0].data_threshold > 0)
         {
             vptx_process_port_threshold_module(curr_module_ptr, (*stream_data_ptrs)[curr_module_ptr->input_buf_index], (*stream_data_ptrs)[curr_module_ptr->output_buf_index], pVptx->ec_capi_info.ecns_rate_matching_info.is_rate_matching_supported, nSlipStuffSamples, pVptx->modules.pScrath_mem);
         }
         else
         {
              curr_module_ptr->module_ptr->vtbl_ptr->process(curr_module_ptr->module_ptr,(*stream_data_ptrs)[curr_module_ptr->input_buf_index],(*stream_data_ptrs)[curr_module_ptr->output_buf_index]);
         }
         //Log NR ref output just after ECNS. if ECNS is disabled actual len of NR and LEC ports will be set to 0.
         // And it will be updated with proper size later.
         if (pVptx->ec_capi_info.ecns_output_capabilities.output_nr)
         {
            voice_log_ec_noise_ref_signal(pVptx);
         }
      }
      else
      {
         if(curr_module_ptr->is_enabled)
         {
            if(curr_module_ptr->is_capi)
            {
                if(curr_module_ptr->port_info.input_port[0].data_threshold > 0)
                {
                    vptx_process_port_threshold_module(curr_module_ptr, (*stream_data_ptrs)[curr_module_ptr->input_buf_index], (*stream_data_ptrs)[curr_module_ptr->output_buf_index], FALSE, nSlipStuffSamples, pVptx->modules.pScrath_mem);
                }
                else
                {
                    curr_module_ptr->module_ptr->vtbl_ptr->process(curr_module_ptr->module_ptr,(*stream_data_ptrs)[curr_module_ptr->input_buf_index],
                     (*stream_data_ptrs)[curr_module_ptr->output_buf_index]);
                }
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: process() for unknown non-capi moduleid(0x%lx) is called, session(0x%lx)",curr_module_ptr->module_id, pVptx->session.session_num);
            }
         }
         else
         {
            // for virtual stub modules is_enabled is false and hence calling process is bypassed
            //AVC module can be disabled but still might have different indices
            if( (curr_module_ptr != pVptx->modules.special_capis[AVCRVE_CAPI])  &&
                  (curr_module_ptr->input_buf_index != curr_module_ptr->output_buf_index) )
            {
               MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx mod_id(0x%lx) is not enabled but has different io pointers. input_idx(%d), output_idx(%d)",curr_module_ptr->module_id,
                     curr_module_ptr->input_buf_index,curr_module_ptr->output_buf_index);
            }
         }
      }

#if defined(LOG_VOICEPROC_DATA) && defined(SIM)
      if(curr_module_ptr == pVptx->modules.special_capis[ECNS_CAPI])
      {
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx session(%d).mod_idx(%d), input samples (%d)  output samples (%d)",
               pVptx->session.session_num,curr_module_ptr->module_index,(*stream_data_ptrs)[curr_module_ptr->input_buf_index][0]->buf_ptr[0].actual_data_len>>1,
               (*stream_data_ptrs)[curr_module_ptr->output_buf_index][0]->buf_ptr[0].actual_data_len>>1);
         {
            char buffer[100]; // The filename buffer.
            // Put output of ith module in ith file
            snprintf(buffer, sizeof(char) * 100, "vptx_session_%x_samp_rate_%d_module_%d_output.raw", pVptx->session.session_num,pVptx->session.sampling_rate,curr_module_ptr->module_index);
            FILE *fp;
            int16_t *pIn = (int16_t*)(*stream_data_ptrs)[curr_module_ptr->output_buf_index][0]->buf_ptr->data_ptr;
            fp = fopen(buffer,"ab"); // in append mode
            fwrite(pIn,sizeof(char),(*stream_data_ptrs)[curr_module_ptr->output_buf_index][0]->buf_ptr->actual_data_len,fp);
            fclose(fp);
         }
      }
#endif

      curr_module_ptr++;
   }
   curr_module_ptr--; //go back to last module in processing chain

   //update the output of the chain with the last module's output buffer pointer. if the last module is ECNS then update with the primary output port pointer
   int8_t *buf_ptr[4] = { (*stream_data_ptrs)[curr_module_ptr->output_buf_index][0]->buf_ptr[0].data_ptr,NULL, NULL, NULL };
   process_frame_samples = (*stream_data_ptrs)[curr_module_ptr->output_buf_index][0]->buf_ptr[0].actual_data_len >> 1;
   if(curr_module_ptr == pVptx->modules.special_capis[ECNS_CAPI])
   {
      buf_ptr[0] = (*stream_data_ptrs)[curr_module_ptr->output_buf_index][pVptx->ec_capi_info.ecns_prim_output_port_idx]->buf_ptr[0].data_ptr;
      process_frame_samples = (*stream_data_ptrs)[curr_module_ptr->output_buf_index][pVptx->ec_capi_info.ecns_prim_output_port_idx]->buf_ptr[0].actual_data_len >> 1;
   }

#if defined(__qdsp6__) && !defined(SIM)
   {
      uint32_t sampling_rate_log_id;
      sampling_rate_log_id = voice_get_sampling_rate_log_id(pVptx->io.output.mediatype.sample_rate);
      uint32_t session_id = ( pVptx->io.near_in.mediatype.num_channels << 5 ) | (sampling_rate_log_id << 3) | (pVptx->session.session_num | ((TRUE == pVptx->session.audio_instance_flag) ? VPTX_AUDIO_LOG_ID : 0));
      uint64_t log_timestamp = qurt_elite_timer_get_time();
      voice_log_buffer( buf_ptr,
            VOICE_LOG_TAP_POINT_VPTX_OUT,
            session_id,
            log_timestamp,
            VOICE_LOG_DATA_FORMAT_PCM_MONO,
            pVptx->io.output.mediatype.sample_rate,
            process_frame_samples << 1,
            NULL);
   }
#endif

   vptx_write_output(pVptx,buf_ptr[0], (int32_t)process_frame_samples);

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_process end session(%x)",(int)pVptx->session.session_num);
   return ADSP_EOK;
}


ADSPResult vptx_modules_set_param(vptx_t* pVptx, uint32_t nPayload_address, uint32_t nPayload_size, uint32_t cal_param_id_ver )
{
   ADSPResult nLocalResult = ADSP_EUNSUPPORTED, nAggregateResult = ADSP_EOK;
   int8_t *pCalibrationDataPayLoad;
   uint32_t nByteSizeCounter=0;

#if defined(__qdsp6__) && !defined(SIM)
   // Log cal data for Audio use cases. Note that this logging will be done
   // for in-band as well as out-of-band.
   if ((TRUE == pVptx->session.audio_instance_flag))
   {
      int8_t *bufptr[4] = { (int8_t *) nPayload_address, NULL, NULL, NULL };
      voice_log_buffer( bufptr,
            VOICE_LOG_CHAN_VPM_SET_PARAM,
            pVptx->session.session_num,
            qurt_elite_timer_get_time(),
            VOICE_LOG_DATA_FORMAT_PCM_MONO,
            1,   /* dummy value */
            nPayload_size,
            NULL);
   }
#endif

   // Iterate through the entire payload size and copy all updated parameters
   uint32_t module_id, param_id, param_size, header_size;
   uint16_t instance_id;

   // Iterate through the entire payload size and copy all updated parameters
   do
   {
      //update module information based on cal version
      if(ELITEMSG_PARAM_ID_CAL == cal_param_id_ver)
      {
         voice_param_data_t *param_data_payload_ptr = (voice_param_data_t *)nPayload_address;
         module_id = param_data_payload_ptr->module_id;
         instance_id = 0;
         param_id = param_data_payload_ptr->param_id;
         param_size = (uint32_t)param_data_payload_ptr->param_size;
         header_size = sizeof(voice_param_data_t);
      }
      else if(ELITEMSG_PARAM_ID_CAL_V2 == cal_param_id_ver)
      {
         voice_param_data_v2_t *param_data_payload_ptr = (voice_param_data_v2_t *)nPayload_address;
         module_id = param_data_payload_ptr->module_id;
         instance_id = param_data_payload_ptr->instance_id;
         param_id = param_data_payload_ptr->param_id;
         param_size = param_data_payload_ptr->param_size;
         header_size = sizeof(voice_param_data_v2_t);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Invalid cal_param_id_ver(%#lx). Aborting set param", cal_param_id_ver);
         return ADSP_EBADPARAM;
      }


      pCalibrationDataPayLoad = (int8_t *)nPayload_address + header_size;

      // check to make sure size is a multiple of 4. If not, stop calibrating
      if( (param_size & 0x00000003L) != 0 )
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Vptx received bad param size, mod %d, instance(%ld) param %d, size %d!!", (int)module_id, instance_id,(int)param_id, (int)param_size);
         nAggregateResult = ADSP_EBADPARAM;
         break;
      }

      nLocalResult = vptx_modules_set_param_int(pVptx, module_id, instance_id, param_id, pCalibrationDataPayLoad, param_size);

      if (ADSP_EBADPARAM == nLocalResult)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vptx SetParam bailing!!!");
         nAggregateResult = ADSP_EBADPARAM;
         break; // in case of any error dont go forward with parsing
      }
      else if(ADSP_FAILED(nLocalResult) && ADSP_EUNSUPPORTED != nLocalResult)
      {
         nAggregateResult = ADSP_EFAILED;
      }

      nByteSizeCounter += (header_size + param_size);
      nPayload_address += (header_size + param_size);
      //dbg: MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx SetParam addr(%#x), byte_size(%#x)", nPayload_address, nByteSizeCounter );
      //dbg: MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx received mod %d, param %d, size %d!!", (int)module_id, (int)param_id, (int)param_size);
   } while(nByteSizeCounter < nPayload_size );

   //Issue algo reset to ECNS at the end of set param.
   //This prevents calling init function for EC multiple times after each PID during calibration to reduce latency.
   nLocalResult = vptx_modules_set_param_end(pVptx);
   nAggregateResult |=  nLocalResult;

   // Check guard band for memory corruption
   nAggregateResult = nAggregateResult | (voice_check_guard_band((int8_t**)pVptx->memory.guard_band_ptrs, (int16_t)pVptx->modules.num_modules, TX, pVptx->session.session_num));

   return nAggregateResult;

}

ADSPResult vptx_modules_set_param_int(vptx_t* pVptx, uint32_t mod_id, uint16_t instance_id, uint32_t param_id, void* payload_address, uint32_t param_size )
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   int8_t* nPayload_address = (int8_t*)payload_address;

   // populate buffer to issue set param to capi modules
   capi_v2_buf_t param_data_buf;
   capi_v2_port_info_t port_info;
   port_info.is_valid = FALSE;
   param_data_buf.data_ptr = nPayload_address;
   param_data_buf.actual_data_len = param_data_buf.max_data_len = param_size;

   if((mod_id == VOICEPROC_MODULE_TX) && (VOICE_PARAM_LOOPBACK_ENABLE == param_id))
   {
       int16_t nEnableFlag = *((int16_t*)nPayload_address);
       if (sizeof(int32_t) != param_size)
       {
           result = ADSP_EBADPARAM;
       }
       else
       {
           if(( TRUE == nEnableFlag) && (FALSE == pVptx->session.audio_instance_flag) && (TRUE == pVptx->session.real_time_flag))
           {  // loopback not enabled in audio path case or ftrt case
               pVptx->loopback_enable_flag        = TRUE;
               voice_multi_ch_circ_buf_reset(&(pVptx->io.loopback_output.circ_buf));
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_set_param_cmd VpTx->VpRx Loopback Enable session(%x)",(int)pVptx->session.session_num);
           }
           else
           {
               pVptx->loopback_enable_flag        = FALSE;
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_set_param_cmd VpTx->VpRx Loopback Disable session(%x)",(int)pVptx->session.session_num);
           }
           result = ADSP_EOK;
       }
   }
   else
   {
      voice_capi_module_t* curr_module_ptr = NULL;

      result = voice_topo_get_module_info_tx( pVptx, mod_id, instance_id, param_id, &curr_module_ptr);
      if(ADSP_FAILED(result) || (curr_module_ptr == NULL))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: set_param(): Unsupported set vptx module(%#lx), instance(%lx) - ignoring", mod_id,instance_id);
         //replace the result with UNSUPPORTED
         result = ADSP_EUNSUPPORTED;
      }
      else
      {
         if(curr_module_ptr->is_capi)
         {
            if(curr_module_ptr->is_virtual_stub)
            {
               capi_result = CAPI_V2_EOK;
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: set_param(): for virutally stubbed  vptx module(%#lx)", mod_id);
            }
            else if(curr_module_ptr->module_ptr)
            {
               capi_result = curr_module_ptr->module_ptr->vtbl_ptr->set_param(curr_module_ptr->module_ptr, param_id, &port_info, &param_data_buf);
            }
            else
            {
               capi_result = CAPI_V2_EFAILED;
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: set_param(): module_ptr is null for module_id(%#lx), Ignoring", mod_id);
            }
            result = capi_v2_err_to_adsp_result(capi_result);
         }
      }
   }
   /* Wrappers should return bad param only for invalid param size, return general failure otherwise */
   return result;
}

ADSPResult vptx_modules_set_param_end(vptx_t* pVptx)
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_result;

   //Call voice_ecns_cal_init at the end of set param.
   //This prevents calling init function for EC multiple times after each PID during calibration to reduce latency.
   if( (TRUE == pVptx->process_data_flag) && pVptx->modules.special_capis[ECNS_CAPI] )
   {
      capi_v2_buf_t payload;
      capi_v2_prop_t static_props[] = {
         { CAPI_V2_ALGORITHMIC_RESET, { reinterpret_cast<int8_t *>(&payload),   0, sizeof(payload) }, { FALSE, FALSE, 0 } }
      };
      capi_v2_proplist_t static_proplist = { VOICE_SIZE_OF_ARRAY(static_props), static_props };
      capi_result = pVptx->modules.special_capis[ECNS_CAPI]->module_ptr->vtbl_ptr->set_properties(pVptx->modules.special_capis[ECNS_CAPI]->module_ptr, &static_proplist);
      result = capi_v2_err_to_adsp_result(capi_result);

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_ecns_cal_init in set param. capi_result(%#lx), session(%#lx)",capi_result,pVptx->session.session_num);
   }

   return result;
}

ADSPResult vptx_modules_get_param(vptx_t* pVptx, void *cal_param_payload, uint32_t cal_param_id_ver, uint32_t* param_size)
{
   ADSPResult nLocalResult = ADSP_EUNSUPPORTED;

   uint32_t module_id, param_id,*param_size_ptr, header_size;
   uint16_t instance_id;

   //update module information based on cal version
   if(ELITEMSG_PARAM_ID_CAL == cal_param_id_ver)
   {
      voice_param_data_t *param_data_payload_ptr = (voice_param_data_t *)cal_param_payload;
      module_id = param_data_payload_ptr->module_id;
      param_data_payload_ptr->reserved = 0; // set it to 0 incase client did not as we are reading size as 32 bits
      param_size_ptr = (uint32_t *)&param_data_payload_ptr->param_size;
      instance_id = 0;
      param_id = param_data_payload_ptr->param_id;
      header_size = sizeof(voice_param_data_t);
   }
   else if(ELITEMSG_PARAM_ID_CAL_V2 == cal_param_id_ver)
   {
      voice_param_data_v2_t *param_data_payload_ptr = (voice_param_data_v2_t *)cal_param_payload;
      module_id = param_data_payload_ptr->module_id;
      instance_id = param_data_payload_ptr->instance_id;
      param_size_ptr = &param_data_payload_ptr->param_size;
      param_id = param_data_payload_ptr->param_id;
      header_size = sizeof(voice_param_data_v2_t);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Error!! Unknown cal param version (0x%lx). session(0x%lx)\n",
            cal_param_id_ver, pVptx->session.session_num);
      return ADSP_EBADPARAM;
   }

   // param_max_size sent from client is copied over to param_size from static service.
   // however this param_size corresponds to the actual size of the parameter, plus the header
   // thus the header size must be subtracted from the size that is available to write the parameter
   // data to.

   uint32_t nBufferSize = *param_size_ptr - header_size;
   int8_t* pCalibrationDataPayLoad = (int8_t *)cal_param_payload + header_size;

   capi_v2_buf_t param_data_buf;
   capi_v2_port_info_t port_info;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   port_info.is_valid = FALSE;
   param_data_buf.data_ptr = pCalibrationDataPayLoad;
   param_data_buf.max_data_len = nBufferSize;
   param_data_buf.actual_data_len = 0;

   if(!vptx_is_ready_for_get_param(pVptx, module_id, param_id))
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: GET Error. Not ready to return parameters in Stop state");
       *param_size = *param_size_ptr = 0;
       return ADSP_ENOTREADY;
   }

   if((VOICEPROC_MODULE_TX == module_id) &&
           (VOICE_PARAM_LOOPBACK_ENABLE ==  param_id))
   {
       *param_size = sizeof(int32_t);

       if( *param_size > nBufferSize)
       {
           nLocalResult = ADSP_ENOMEMORY;
       }
       else
       {
           *((int32_t*)pCalibrationDataPayLoad) = 0; // Clearing the whole buffer
           *((int16_t*)pCalibrationDataPayLoad) = (int16_t) pVptx->loopback_enable_flag;
           nLocalResult = ADSP_EOK;
       }
   }
   else
   {
      voice_capi_module_t* curr_module_ptr = NULL;

      nLocalResult = voice_topo_get_module_info_tx( pVptx, module_id,instance_id,param_id,&curr_module_ptr);
      if(ADSP_FAILED(nLocalResult) || (curr_module_ptr == NULL))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: get_param(): Unsupported get vptx module(%#lx) instance(%lx). Ignoring", module_id,instance_id);
         //replace the result with UNSUPPORTED
         nLocalResult = ADSP_EUNSUPPORTED;
      }
      else
      {
         if(curr_module_ptr->is_capi)
         {
            if(curr_module_ptr->is_virtual_stub)
            {
               capi_result = CAPI_V2_EFAILED;
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_param(): for  virtually stubbed  vptx module(%#lx)", module_id);
            }
            else if(curr_module_ptr->module_ptr)
            {
               capi_result = curr_module_ptr->module_ptr->vtbl_ptr->get_param(curr_module_ptr->module_ptr, param_id, &port_info, &param_data_buf);
            }
            else
            {
               capi_result = CAPI_V2_EFAILED;
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: get_param(): module_ptr is null for module_id(%#lx), Ignoring", module_id);
            }
            nLocalResult = capi_v2_err_to_adsp_result(capi_result);
            if(CAPI_V2_SUCCEEDED(capi_result))
            {
               *param_size_ptr = param_data_buf.actual_data_len;
            }
            MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Tx get param done, mod id(0x%lx), instance_id(%lx), param id (0x%lx), result(0x%x) session(0x%lx)\n", module_id, instance_id, param_id, (int)nLocalResult, pVptx->session.session_num);
         }
      }
   }

   if(ADSP_SUCCEEDED(nLocalResult))
   {
      // Return size
      *param_size = *param_size_ptr;

#if defined(__qdsp6__) && !defined(SIM)
      // Log cal data for Audio use cases.
      if ((TRUE == pVptx->session.audio_instance_flag))
      {
         int8_t *bufptr[4] = { (int8_t *) cal_param_payload, NULL, NULL, NULL };
         voice_log_buffer( bufptr,
               VOICE_LOG_CHAN_VPM_GET_PARAM,
               pVptx->session.session_num,
               qurt_elite_timer_get_time(),
               VOICE_LOG_DATA_FORMAT_PCM_MONO,
               1,   /* dummy value */
               (*param_size_ptr+header_size),
               NULL);
      }
#endif
   }
   else
   {
      *param_size = 0;
   }

   return nLocalResult;
}


capi_v2_err_t vptx_capi_v2_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
   voice_capi_module_t* module_ptr = (voice_capi_module_t*)context_ptr;
   capi_v2_err_t result = CAPI_V2_EOK;
   if(NULL == context_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Received bad context pointer in vptx callback");
      return CAPI_V2_EFAILED;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: Vptx event callback invoked, event_id %d, module %lx instance %lx", (int)id, module_ptr->module_id,module_ptr->instance_id);

   switch(id)
   {
      case CAPI_V2_EVENT_KPPS:
         {
            capi_v2_event_KPPS_t* kpps_ptr = (capi_v2_event_KPPS_t* )event_info_ptr->payload.data_ptr;
            module_ptr->kpps = kpps_ptr->KPPS;
            break;
         }
      case CAPI_V2_EVENT_BANDWIDTH:
      {
         //no handling for now
         break;
      }
      case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
         {
            capi_v2_event_algorithmic_delay_t* delay_ptr = (capi_v2_event_algorithmic_delay_t* )event_info_ptr->payload.data_ptr;
            module_ptr->delay = delay_ptr->delay_in_us;
            break;
         }
      case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
         {
            if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_data_to_dsp_service_t))
            {
               MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Module(%#lx) provided insufficient size for "
                     "detection_status. Required: %d bytes, provided: %lu bytes.",module_ptr->module_id,
                     sizeof(capi_v2_event_data_to_dsp_service_t),
                     event_info_ptr->payload.actual_data_len);
               return CAPI_V2_ENOMEMORY;
            }
            capi_v2_event_data_to_dsp_service_t* data_ptr = (capi_v2_event_data_to_dsp_service_t*)event_info_ptr->payload.data_ptr;

            switch(data_ptr->param_id)
            {
               case PARAM_ID_DTMF_TONE_DETECTED:
               {
                  vptx_t  *pVptx= (vptx_t*)(module_ptr->svc_ptr);
                  vptx_send_dtmf_tone_status(pVptx, data_ptr->payload.data_ptr);
                  break;
               }
               case FWK_EXTN_EVENT_ID_HPCM_HOST_BUF_AVAILABLE:
               {
                  vptx_t  *pVptx= (vptx_t*)(module_ptr->svc_ptr);
                  vptx_send_hpcm_buf_available_evt(pVptx, data_ptr->payload.data_ptr);
                  break;
               }
               case PARAM_ID_SPEECH_ACTIVITY_UPDATE:
               {
                  vptx_t  *pVptx= (vptx_t*)(module_ptr->svc_ptr);

                  //TODO: send only if activity for that module has changed
                  if( (1 == pVptx->voice_activity_status.report_voice_activity) &&
                      (pVptx->vpm_apr_info.apr_handle != 0xFFFFFFFF) )
                  {
                     result = vptx_send_tx_speech_activity_status(pVptx, data_ptr->payload.data_ptr);
                     if(ADSP_FAILED(result))
                     {
                        return CAPI_V2_EFAILED;
                     }
                  }
                  else
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Voice activity detection event is not registered by client or Audio session.session(%#lx)",pVptx->session.session_num);
                  }
                  break;
               }
               default:
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Unknown DATA_TO_DSP service event !!!");
                  return CAPI_V2_EBADPARAM;
               }
            }
            break;
         }
      case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
         {
            voice_capi_data_format_struct_t* output_media_type = (voice_capi_data_format_struct_t*)event_info_ptr->payload.data_ptr;

            vptx_t  *pVptx= (vptx_t*)(module_ptr->svc_ptr);

            //currently accept this for ECNS module only
            if(module_ptr == pVptx->modules.special_capis[ECNS_CAPI])
            {
               if (event_info_ptr->payload.actual_data_len != sizeof(voice_capi_data_format_struct_t) * pVptx->ec_capi_info.num_port.num_output_ports)
               {
                  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ERROR !!! incorrect payload size for output media type of EC");
                  return CAPI_V2_EBADPARAM;
               }
        
               //init with -1
               pVptx->ec_capi_info.ecns_prim_output_port_idx = pVptx->ec_capi_info.ecns_lec_output_port_idx = pVptx->ec_capi_info.ecns_nr_output_port_idx = -1;

               for(uint32_t i=0; i < pVptx->ec_capi_info.num_port.num_output_ports ; i++)
               {
               if(PCM_CHANNEL_LEC_OUT == output_media_type[i].data_format.channel_type[0])
                  {
                     pVptx->ec_capi_info.ecns_lec_output_port_idx = i;
                  }
                  else if(PCM_CHANNEL_NREF_OUT == output_media_type[i].data_format.channel_type[0])
                  {
                     pVptx->ec_capi_info.ecns_nr_output_port_idx = i;
                  }
               else
               {
                  //EC can output mic or speaker path channel map
                  pVptx->ec_capi_info.ecns_prim_output_port_idx = i;
               }
               }

               for(uint32_t port_id = 0; port_id < module_ptr->port_info.port_num_info.num_output_ports; port_id++)
               {
              memscpy(&pVptx->ec_capi_info.output_port[port_id],sizeof(voice_capi_data_format_struct_t),
                     &output_media_type[port_id], sizeof(voice_capi_data_format_struct_t));

               if(port_id == pVptx->ec_capi_info.ecns_prim_output_port_idx)
               {
                  // if input channel map from client is for speaker path then update the output with the same channel map so that downstream modules 
                  // get the proper channel map
                  if( FALSE == vptx_is_mic_path_channel_mapping( &pVptx->io.near_in.mediatype.channel_mapping[0] ) )
                  {
                     pVptx->ec_capi_info.output_port[port_id].data_format.channel_type[0] = pVptx->io.near_in.mediatype.channel_mapping[0];
                  }
               }
               memscpy(&module_ptr->port_info.output_port[port_id].media_fmt, sizeof(voice_capi_data_format_struct_t),
                     &pVptx->ec_capi_info.output_port[port_id], sizeof(voice_capi_data_format_struct_t));

               }

               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ECNS output port numbers. pri_output(%d),lec_output(%d),nr_output(%d)",(int)pVptx->ec_capi_info.ecns_prim_output_port_idx,
                     (int)pVptx->ec_capi_info.ecns_lec_output_port_idx, (int)pVptx->ec_capi_info.ecns_nr_output_port_idx);
            }
            else if(module_ptr == pVptx->modules.special_capis[DYN_RESAMP])
            {
               if (event_info_ptr->payload.actual_data_len != sizeof(voice_capi_data_format_struct_t))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR !!! incorrect payload size for output media type of module with module id 0x%lx", module_ptr->module_id);
                  return CAPI_V2_EBADPARAM;
               }
               
               if(event_info_ptr->port_info.port_index < module_ptr->port_info.port_num_info.num_output_ports)
               {
                   memscpy(&module_ptr->port_info.output_port[event_info_ptr->port_info.port_index].media_fmt, sizeof(voice_capi_data_format_struct_t), output_media_type,event_info_ptr->payload.actual_data_len);
               }
               else
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR !!! incorrect port idx %lu for output media type of module with module id 0x%lx", module_ptr->module_id, event_info_ptr->port_info.port_index);
                  return CAPI_V2_EBADPARAM;
               }
            }
            else
            {
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Non-ECNS module(%#lx) is raising output media fmt event. Ignoring",module_ptr->module_id);
            }
            break;
         }
      case CAPI_V2_EVENT_PROCESS_STATE:
         {
            uint32_t prev_is_enabled = module_ptr->is_enabled;
            module_ptr->is_enabled = ((capi_v2_event_process_state_t *)(event_info_ptr->payload.data_ptr))->is_enabled;
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Module ID(0x%lx) process state changed from (%lu) to (%lu)",module_ptr->module_id,prev_is_enabled, module_ptr->is_enabled);

            vptx_t  *pVptx= (vptx_t*)(module_ptr->svc_ptr);

            //process state is not acceptable for special modules which change media fmt.
            if(module_ptr == pVptx->modules.special_capis[ECNS_CAPI])
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error!! ECNS or AVCRVE module raised process state event");
               break;
            }

            //skip propagating the pointers for AVC as it is pass through
            if(module_ptr == pVptx->modules.special_capis[AVCRVE_CAPI])
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: AVCRVE module raised process state. Not propagating the process state");
               break;
            }

            /*
             * update stream data buffers for modules if the state has changed and module is not inplace
             * Propagate the process state only when the topo graph is created and when we are initializing
             * all modules in the topology
             */
            if(pVptx->modules.module_list_locked)
            {
               if( (prev_is_enabled != module_ptr->is_enabled )  && (FALSE == module_ptr->is_in_place) )
               {
                  vptx_reconfig_stream_io_buf(module_ptr);
               }
            }
            break;
         }
      case CAPI_V2_EVENT_GET_LIBRARY_INSTANCE:
         {
            if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_get_library_instance_t))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VPTx received insufficient size for get library instance, size(0x%lx)",event_info_ptr->payload.actual_data_len);
               return CAPI_V2_EBADPARAM;
            }

            capi_v2_event_get_library_instance_t* get_library_instance = (capi_v2_event_get_library_instance_t*)event_info_ptr->payload.data_ptr;;

            result = capi_v2_library_factory_get_instance(get_library_instance->id,&get_library_instance->ptr);
            if(CAPI_V2_FAILED(result))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VPTx failed to get library instance with lib_id(0x%x), payload_ptr(0x%p)",get_library_instance->id,get_library_instance->ptr);
               return result;
            }
            break;
         }
      case CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE:
         {
            if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_port_data_threshold_change_t))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VPTx received insufficient size for port data threshold change, size(0x%lx)",event_info_ptr->payload.actual_data_len);
               return CAPI_V2_EBADPARAM;
            }

            vptx_t  *pVptx= (vptx_t*)(module_ptr->svc_ptr);
            // check if topology is already created as buffers are allocated during topology creation
            if(pVptx->modules.module_list_locked)
            {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Port data threshold event after topology creation is not supported, module id (0x%lx)", module_ptr->module_id);
                return CAPI_V2_EUNSUPPORTED;
            }

            capi_v2_port_data_threshold_change_t *port_data_threshold = (capi_v2_port_data_threshold_change_t*)event_info_ptr->payload.data_ptr;

            if(event_info_ptr->port_info.is_input_port && event_info_ptr->port_info.is_valid)
            {
               if(event_info_ptr->port_info.port_index < module_ptr->port_info.port_num_info.num_input_ports)
               {
                   module_ptr->port_info.input_port[event_info_ptr->port_info.port_index].data_threshold = port_data_threshold->new_threshold_in_bytes;
                   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Module with id (0x%lx) raised PORT_DATA_THRESHOLD (%lu) on input port index %lu", module_ptr->module_id, port_data_threshold->new_threshold_in_bytes, event_info_ptr->port_info.port_index);
               }
               else
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Module with id (0x%lx) raised PORT_DATA_THRESHOLD_CHANGE event with Inavlid input port index %lu", module_ptr->module_id, event_info_ptr->port_info.port_index);
               }
            }
            else
            {
                if(event_info_ptr->port_info.port_index < module_ptr->port_info.port_num_info.num_output_ports)
                {
                    module_ptr->port_info.output_port[event_info_ptr->port_info.port_index].data_threshold = port_data_threshold->new_threshold_in_bytes;
                    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Module with id (0x%lx) raised PORT_DATA_THRESHOLD (%lu) on output port index %lu", module_ptr->module_id, port_data_threshold->new_threshold_in_bytes, event_info_ptr->port_info.port_index);
                }
                else
                {
                    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Module with id (0x%lx) raised PORT_DATA_THRESHOLD_CHANGE event with Inavlid output port index %lu", module_ptr->module_id, event_info_ptr->port_info.port_index);
                }
            }
            break;
         }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Unsupported event %d", (int)id);
            return CAPI_V2_EBADPARAM;
         }
   }
   return result;
}

static ADSPResult vptx_set_topo_framesize(vptx_t* pVptx)
{
  ADSPResult nResult = ADSP_EOK;
  voice_capi_module_t* curr_module_ptr;
  uint32_t i = 0, sample_rate_khz, sample_rate_nb_factor, bytes_per_sample, data_threshold, port_id;
  uint32_t max_data_threshold_in_ms = 0, curr_data_threshold_in_ms = 0, port_threshold_in_ms = 0;

  for(i=0; i < pVptx->modules.num_modules; i++)
  {
      curr_module_ptr = &pVptx->modules.modules_array[i];

      data_threshold = curr_module_ptr->port_info.input_port[0].data_threshold;

      sample_rate_khz = curr_module_ptr->port_info.input_port[0].media_fmt.data_format.sampling_rate/1000;
      sample_rate_nb_factor = sample_rate_khz/8;
      bytes_per_sample = curr_module_ptr->port_info.input_port[0].media_fmt.data_format.bits_per_sample >> 3;

      curr_data_threshold_in_ms = data_threshold/(sample_rate_khz * bytes_per_sample);

      if((curr_data_threshold_in_ms != 10) && (curr_data_threshold_in_ms != 20) && (curr_data_threshold_in_ms != 0))
      {
          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx topo framesize module id (0x%lx) data threshold (%lu) ms is not supported, disabling the module",curr_module_ptr->module_id,curr_data_threshold_in_ms);
          curr_module_ptr->is_enabled = FALSE; // disable module
          curr_module_ptr->module_ptr = NULL;
          if(curr_module_ptr == pVptx->modules.special_capis[ECNS_CAPI])
          {
              nResult |= ADSP_EBADPARAM;
          }
          continue;
      }

      for(port_id = 1; port_id < curr_module_ptr->port_info.port_num_info.num_input_ports; port_id++)
      {
          sample_rate_khz = curr_module_ptr->port_info.input_port[port_id].media_fmt.data_format.sampling_rate/1000;
          sample_rate_nb_factor = sample_rate_khz/8;
          bytes_per_sample = curr_module_ptr->port_info.input_port[port_id].media_fmt.data_format.bits_per_sample >> 3;

          port_threshold_in_ms = curr_module_ptr->port_info.input_port[port_id].data_threshold/(sample_rate_khz * bytes_per_sample);

          // allowing only 10 and 20 ms data threshold as of now
          if(port_threshold_in_ms != curr_data_threshold_in_ms)
          {
              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx topo framesize module id (0x%lx) multiple ports raising different data threshold, not supported",curr_module_ptr->module_id);
              curr_module_ptr->is_enabled = FALSE; // disable module
              curr_module_ptr->module_ptr = NULL;
              if(curr_module_ptr == pVptx->modules.special_capis[ECNS_CAPI])
              {
                  nResult |= ADSP_EBADPARAM;
              }
              break;
          }
      }

      if(port_id == curr_module_ptr->port_info.port_num_info.num_input_ports)
      {
          max_data_threshold_in_ms = (curr_data_threshold_in_ms > max_data_threshold_in_ms)?curr_data_threshold_in_ms:max_data_threshold_in_ms;
      }
  }

  // if none of the module has raised port data threhold then framesize will be default whatever it has been set to in set_media_type function
  if(max_data_threshold_in_ms > 0)
  {
     pVptx->modules.is_data_threshold_raised = TRUE;
     pVptx->io.num_1ms_frame_block_process = max_data_threshold_in_ms;
     pVptx->io.near_in.frame_samples  = pVptx->io.num_1ms_frame_block_process * pVptx->io.near_in.mediatype.sample_rate/1000;;
     pVptx->io.far_in.frame_samples   = pVptx->io.num_1ms_frame_block_process * pVptx->io.far_in.mediatype.sample_rate/1000;;
  }

  MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx topo num_1ms_frame_block_process (%d), near in samples (%d), far in samples (%d), session (%lx)",pVptx->io.num_1ms_frame_block_process, pVptx->io.near_in.frame_samples, pVptx->io.far_in.frame_samples,pVptx->session.session_num);

  return nResult;
}

static ADSPResult vptx_get_additonal_module_static_property(vptx_t* pVptx,voice_capi_module_t* module_info)
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   for (uint32_t i=0;i < module_info->num_extensions.num_extensions; i++)
   {
      switch (module_info->fwk_extn_ptr[i].id)
      {
         case FWK_EXTN_ECNS:
            {
               // get more property statically
               ecns_output_capabilities_t ecns_output_info = { FALSE, FALSE };
               ecns_monitoring_capabilities_t ecns_monitoring_info = { FALSE };
               ecns_rate_matching_capabilities_t ecns_rate_matching_info = { FALSE };
               ecns_vp3_capabilities_t ecns_vp3_info = { FALSE };
               ecns_stt_capabilities_t ecns_stt_info = { FALSE };
               ecns_multichannel_ref_capabilities_t ecns_multichannel_ref_info = {FALSE};
               ecns_speech_act_det_capabilities_t ecns_speech_act_info = { FALSE };
               capi_v2_init_memory_requirement_t mem_req = { 0 };
               capi_v2_get_ecns_property_t  ecns_get_prop[]={
                  {   CAPI_V2_PROPERTY_ID_ECNS_OUTPUT_CAPABILITIES, &ecns_output_info },
                  {   CAPI_V2_PROPERTY_ID_ECNS_MONITORING_CAPABILITIES,&ecns_monitoring_info},
                  {   CAPI_V2_PROPERTY_ID_ECNS_VP3_CAPABILITIES,&ecns_vp3_info},
                  {   CAPI_V2_PROPERTY_ID_ECNS_STT_CAPABILITIES,&ecns_stt_info},
                  {   CAPI_V2_PROPERTY_ID_ECNS_RATE_MATCHING_CAPABILITIES,&ecns_rate_matching_info},
                  {   CAPI_V2_PROPERTY_ID_MULTICHANNEL_REF_CAPABILITIES,&ecns_multichannel_ref_info},
                  {   CAPI_V2_PROPERTY_ID_SPEECH_ACT_DET_CAPABILITIES,&ecns_speech_act_info},
               };

               // SA: Adding CAPI_V2_INIT_MEMORY_REQUIREMENT propId as well because adsp_amdb API require one of the get_static prop to be INIT prop
               capi_v2_prop_t static_ec_capability[]={
                  { CAPI_V2_INIT_MEMORY_REQUIREMENT, { reinterpret_cast<int8_t *>(&mem_req), sizeof(mem_req), sizeof(mem_req) }, { FALSE, FALSE, 0 }      },
                  { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[0]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                  { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[1]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                  { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[2]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                  { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[3]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                  { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[4]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                  { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[5]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                  { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[6]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
               };

               capi_v2_proplist_t static_proplist_extn = { VOICE_SIZE_OF_ARRAY(static_ec_capability), static_ec_capability };
               capi_result = adsp_amdb_capi_v2_get_static_properties_f((adsp_amdb_capi_v2_t*)module_info->amdb_handle_ptr, NULL, &static_proplist_extn);
               result = capi_v2_err_to_adsp_result(capi_result);
               // update vptx with ecns capabilites
               if(ADSP_EOK != result)
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: CUSTOM property not supported by the module (0x%lx)  result(%ld)", module_info->module_id, result);
                  return result; // return size 0 in case of failure.
               }
               pVptx->ec_capi_info.ecns_monitoring_info = ecns_monitoring_info;
               pVptx->ec_capi_info.ecns_rate_matching_info = ecns_rate_matching_info;
               pVptx->ec_capi_info.ecns_vp3_info = ecns_vp3_info;
               pVptx->ec_capi_info.ecns_stt_info = ecns_stt_info;
               pVptx->ec_capi_info.ecns_multichannel_ref_info = ecns_multichannel_ref_info;
               pVptx->ec_capi_info.ecns_speech_act_det_capabilities = ecns_speech_act_info;
               uint8_t num_output_port =1;

               pVptx->ec_capi_info.ecns_output_capabilities = ecns_output_info;
               if (ecns_output_info.output_lec) num_output_port++;
               if (ecns_output_info.output_nr) num_output_port++;

               pVptx->ec_capi_info.num_port.num_input_ports = 2;
               pVptx->ec_capi_info.num_port.num_output_ports = num_output_port;

               pVptx->modules.special_capis[ECNS_CAPI] = module_info;
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ECNS custom static property: num output ports(%d)", num_output_port);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ECNS custom static property: is_vp3_supported(%d)", pVptx->ec_capi_info.ecns_vp3_info.is_vp3_supported);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ECNS custom static property: is_stt_supported(%d)", pVptx->ec_capi_info.ecns_stt_info.is_stt_supported);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ECNS custom static property: rate_matching_info(%d)", pVptx->ec_capi_info.ecns_rate_matching_info.is_rate_matching_supported);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ECNS custom static property: multichannel_reference_capability(%d)", pVptx->ec_capi_info.ecns_multichannel_ref_info.is_multichannel_ref_supported);
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ECNS custom static property: speech_act_det_info(%d)", pVptx->ec_capi_info.ecns_speech_act_det_capabilities.is_speech_activity_supported);

               pVptx->ec_capi_info.force_mono_reference = 0;
               if (!ecns_multichannel_ref_info.is_multichannel_ref_supported)
               {
                   pVptx->ec_capi_info.force_mono_reference = TRUE;
                   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Forcing reference number of channels to 1 since ECNS does not support multichannel reference");
               }

               break;
            }
         case FWK_EXTN_AVC_RVE_IMC_SOURCE:
            {
               //just updating the pointer.
               //hanlding will be done after the module is created.
               pVptx->modules.special_capis[AVCRVE_CAPI] = module_info;
               break;
            }
         case FWK_EXTN_AVC_RVE_TX_IMC_SOURCE:
         case FWK_EXTN_AVC_RVE_TX_IMC_DESTINATION:
            {
               //NOP here. Will be handled after the module is created.
               break;
            }
         case FWK_EXTN_DTMF_DETECTION:
            {
               //just updating the pointer.
               //hanlding will be done after the module is created.
               pVptx->modules.special_capis[TX_DTMFDET_CAPI] = module_info;
               break;
            }
         case FWK_EXTN_HPCM:
            {
               //just updating the pointer.
               //hanlding will be done after the module is created.
               pVptx->modules.special_capis[HPCM] = module_info;
               break;
            }
         default:
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR !! un supported fwk extention : %lx", module_info->fwk_extn_ptr[i].id);
               break;
            }
      }
   }
   return result;
}

static void voice_log_ec_noise_ref_signal(vptx_t* pVptx)
{
#if defined(__qdsp6__) && !defined(SIM)
   uint32_t nr_port_idx =  pVptx->ec_capi_info.ecns_nr_output_port_idx;

   if((CAPI_V2_DATA_FORMAT_INVALID_VAL != nr_port_idx) && (0 != pVptx->ec_output[nr_port_idx]->actual_data_len))
   {
      // Log this as a mono PCM stream;
      int8_t *buf_ptr[4] = { pVptx->ec_output[nr_port_idx]->data_ptr, NULL, NULL, NULL};
      uint32_t sampling_rate = pVptx->ec_capi_info.output_port[nr_port_idx].data_format.sampling_rate;

      if (NULL != pVptx->ec_output[nr_port_idx]->data_ptr)
      {
         uint64_t timestamp = qurt_elite_timer_get_time();
         voice_log_buffer( buf_ptr,
               VOICE_LOG_TAP_POINT_VPTX_ECINTR_OUT,
               pVptx->session.session_num,
               timestamp,
               VOICE_LOG_DATA_FORMAT_PCM_MONO,
               sampling_rate,
               pVptx->ec_output[nr_port_idx]->actual_data_len,
               NULL);
      }
   }
#endif
}

static ADSPResult vptx_reconfig_stream_io_buf(voice_capi_module_t *curr_module_ptr)
{
   uint32_t i, start_idx = curr_module_ptr->module_index ;
   vptx_t *pVptx = (vptx_t *)curr_module_ptr->svc_ptr;
   uint32_t end_idx = pVptx->modules.num_modules;

   voice_capi_io_stream_data_idx_t currentIdx = curr_module_ptr->input_buf_index, ec_input_near_port_idx;

   /*
    * iterate over the modules and update the io buffer pointers.
    *
    * In vptx_update_io_stream_data function, input and output buffer indices are updated (more detailed comments present there).
    *
    * Basically we allocate two  num_channel * 21 msec buffers. for modules before EC, all channel are used and for modules after EC
    * only first channel is used. there buffers are switched for when module is enabled and does not support rate matching.
    * For ECNS we always switch the buffers. For instance, if ECNS input used 1st buffer, its outuput buffer is pointed to 2nd buffer.
    *
    *     Allocated buffers  :
    *     --------------------
    *     buf1 and buf2 of size num_channel * 21 msec.
    *
    *     buffer indices and pointers correspondin pointers
    *     ---------------------------
    *        INPUT_STRM_BUF_1     --->   1 port. num_channel pointers pointing to  buf1. each buf size is 21 msec to support sample slip case.
    *        INPUT_STRM_BUF_2,    --->   1 port. num_channel pointers pointing to  buf2. each buf size is 21 msec to support sample slip case.
    *        EC_INPUT_STRM_BUF,   --->   2 port. near port will be either INPUT_STRM_BUF_1 or INPUT_STRM_BUF_2. far port will be allocate seperately
    *        EC_OUTPUT_STRM_BUF,  --->   1/2/3 ports depending on EC ouptut capabilities. port 1 points to buf1 or buf2 depending on EC_INPUT_STRM_BUF. 2 and 3 will be allocated seperately
    *        OUTPUT_STRM_BUF_1,   --->   1 port. 1 channel pointers pointing to  buf1 of size 20msec
    *        OUTPUT_STRM_BUF_2,   --->   1 port. 1 channel pointers pointing to  buf2 of size 20msec
    */
   for( i = start_idx; i < end_idx ; i++)
   {
      //check if the module is EC.
      if(pVptx->modules.special_capis[ECNS_CAPI] == curr_module_ptr)
      {
         if( (currentIdx!=INPUT_STRM_BUF_1) && (currentIdx!=INPUT_STRM_BUF_2))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid ECNS port(%d). session(%lx)", (int)currentIdx,pVptx->session.session_num);
            return ADSP_EFAILED;
         }
         pVptx->ec_near_input_buf_idx = ec_input_near_port_idx = currentIdx;
         curr_module_ptr->input_buf_index = EC_INPUT_STRM_BUF; //buffer pointers will be updated later
         curr_module_ptr->output_buf_index = EC_OUTPUT_STRM_BUF; //buffer pointers will be updated later

         //update current index with ECNS outputs index
         currentIdx =  (ec_input_near_port_idx==INPUT_STRM_BUF_1)?OUTPUT_STRM_BUF_2:OUTPUT_STRM_BUF_1;

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ECNS module index in topo = %ld, input_buf_idx=%ld", curr_module_ptr->module_index,ec_input_near_port_idx);

         //update ecns input and output buffer pointers
         for(uint32_t j=0; j<pVptx->io.near_in.mediatype.num_channels; j++)
         {
            if(INPUT_STRM_BUF_1 == ec_input_near_port_idx)
            {
               pVptx->ec_near_input[j] = &pVptx->near_input_buf[VPTX_TEMP_BUF_1][j];
            }
            else
            {
               pVptx->ec_near_input[j] = &pVptx->near_input_buf[VPTX_TEMP_BUF_2][j];
            }
         }

         uint32_t pri_out_idx = pVptx->ec_capi_info.ecns_prim_output_port_idx;

         if(INPUT_STRM_BUF_1 == ec_input_near_port_idx)
         {
            pVptx->ec_output[pri_out_idx] = &pVptx->output_buf[VPTX_TEMP_BUF_2];
         }
         else
         {
            pVptx->ec_output[pri_out_idx] = &pVptx->output_buf[VPTX_TEMP_BUF_1];
         }
      }
      else
      {
         //update AVC buffer index with ECOUTPUT index
         if(pVptx->modules.special_capis[AVCRVE_CAPI] == curr_module_ptr)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: AVC module index in topo(%ld)", curr_module_ptr->module_index);
            curr_module_ptr->input_buf_index = EC_OUTPUT_STRM_BUF;
            curr_module_ptr->output_buf_index = currentIdx;
         }
         else
         {
            curr_module_ptr->input_buf_index = currentIdx;
            if( (TRUE == curr_module_ptr->is_enabled) && (FALSE == curr_module_ptr->is_in_place) )
            {
               switch(currentIdx)
               {
                  case INPUT_STRM_BUF_1:
                  {
                     currentIdx = INPUT_STRM_BUF_2;
                     break;
                  }
                  case INPUT_STRM_BUF_2:
                  {
                     currentIdx = INPUT_STRM_BUF_1;
                     break;
                  }
                  case OUTPUT_STRM_BUF_1:
                  {
                     currentIdx = OUTPUT_STRM_BUF_2;
                     break;
                  }
                  case OUTPUT_STRM_BUF_2:
                  {
                     currentIdx = OUTPUT_STRM_BUF_1;
                     break;
                  }
                  default:
                  {
                     MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Error!! invalid currentIdx(%d), session(%lx)",(int)currentIdx, pVptx->session.session_num);
                     break;
                  }
               }
            }
            curr_module_ptr->output_buf_index = currentIdx;
         }
      }

	  if(pVptx->modules.special_capis[DYN_RESAMP])
     {
         pVptx->modules.special_capis[DYN_RESAMP]->output_buf_index = RESAMPLER_OUTPUT_STRM_BUF;
     }

      //dbg message
      MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_reconfig_stream_io_buf(): moduleId(%lx), idx(%ld),is_inplace(%ld), enable(%lx), input_index(%ld), output_index(%ld)",
            curr_module_ptr->module_id, curr_module_ptr->module_index,curr_module_ptr->is_in_place, curr_module_ptr->is_enabled,
            curr_module_ptr->input_buf_index, curr_module_ptr->output_buf_index);

      curr_module_ptr++;
   }

   return ADSP_EOK;
}

int32_t vptx_send_hpcm_buf_available_evt(vptx_t* pVptx, void* data_ptr)
{
   int32_t rc = ADSP_EOK;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VOICE_EVT_HOST_BUF_AVAILABLE_V2 sent to client, session(%lx)",pVptx->session.session_num);

   rc = elite_apr_if_alloc_send_event( pVptx->modules.apr_info_hpcm.apr_handle,
         pVptx->modules.apr_info_hpcm.self_addr,
         pVptx->modules.apr_info_hpcm.self_port,
         pVptx->modules.apr_info_hpcm.client_addr,
         pVptx->modules.apr_info_hpcm.client_port,
         0, /* token */
         VOICE_EVT_HOST_BUF_AVAILABLE_V2,
         data_ptr,
         sizeof(voice_evt_notify_host_pcm_buf_v2_t));

   if (ADSP_FAILED(rc))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR request for VOICE_EVT_HOST_BUF_AVAILABLE_V2  0x%08lx :session(%lx)",rc,pVptx->session.session_num);
   }
   return rc;
}
