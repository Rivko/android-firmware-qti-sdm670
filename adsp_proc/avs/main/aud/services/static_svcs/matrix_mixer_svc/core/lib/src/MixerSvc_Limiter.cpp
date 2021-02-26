/**
@file MixerSvc_Limiter.cpp
@brief This file defines Limiter utility functions that the audio 
       matrix mixer uses.
       
 */       

/*========================================================================
  Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_Limiter.cpp#1 $

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 5/8/2017   sn       Created
 ====================================================================== */

/*==========================================================================
 Include files
 ========================================================================== */
#include "MixerSvc.h"
#include "MixerSvc_Limiter.h"
#include "capi_v2_limiter24.h"
#include "adsp_amdb_static.h"
#include "adsp_vparams_api.h"
#include "adsp_audproc_api.h"
#include "EliteData_Util.h"
#include "adsp_adm_api.h"
#include "MixerSvc_Util.h"

/*==========================================================================
 Globals & definitions
 ========================================================================== */
#define s32_in_sat_s16_out(x) (s16_extract_s32_l(Q6_R_sath_R(x)))

#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))


capi_v2_event_callback_info_t MtMx_LimiterGetCapiv2CallbackHandler(This_t *me, uint32_t unOutPortID)
{
   capi_v2_event_callback_info_t cbInfo;
   cbInfo.event_cb = MtMx_LimiterCapiv2Callback;

   me->outPortParams[unOutPortID]->structLimiter.structLimiterCbObj.unOutPortID = unOutPortID;
   me->outPortParams[unOutPortID]->structLimiter.structLimiterCbObj.me = me;

   cbInfo.event_context = (void*)&me->outPortParams[unOutPortID]->structLimiter.structLimiterCbObj;

   return cbInfo;
}

void MtMx_GetLimiterDefaultParams(limiter_config_params_t* params, uint16_t bytes_per_channel,uint16_t channels)
{
  if (2 == bytes_per_channel)
  {
    params->threshold       = MATRIX_LIM_16BIT_THRESHOLD_Q15;       //  Q15 Limiter threshold, -0.02 dB
    params->hard_threshold  = MATRIX_LIM_16BIT_HARD_THRESHOLD_Q15;  //  Q15 Limiter hard threshold, 0 dB

  }
  else
  {
    params->threshold       = MATRIX_LIM_24BIT_THRESHOLD_Q27;       // Q27 Limiter threshold, -0.02 dB
    params->hard_threshold  = MATRIX_LIM_24BIT_HARD_THRESHOLD_Q27;  // Q27 Limiter hard threshold, 0 dB
  }
  params->delay = MATRIX_LIM_DELAY_Q15;
  params->max_wait = MATRIX_LIM_DELAY_Q15;
  params->makeup_gain = MATRIX_LIM_MAKEUP_GAIN_Q8;
  params->mode = MATRIX_LIM_MODE;
  params->gc = MATRIX_LIM_GC_Q15;
  params->gain_attack = MATRIX_LIM_GAIN_ATTACK_CONSTANT_Q31;
  params->gain_release = MATRIX_LIM_GAIN_RELEASE_CONSTANT_Q31;
  params->attack_coef  = MATRIX_LIM_GAIN_ATTACK_COEFFICIENT_Q15;
  params->release_coef = MATRIX_LIM_GAIN_RELEASE_COEFFICIENT_Q15;
  params->history_window_length = MATRIX_LIM_HISTORY_WINLEN_10MS_Q15;
}

/* Initialized the limiter for the matrix output port.
 */
ADSPResult MtMx_OutPortLimiterInit(This_t *me, uint32_t unOutPortID)
{
  ADSPResult result = ADSP_EOK;
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  mt_mx_struct_limiter_t *pLimiter = NULL;
  
  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port ID [%lu] Processing Limiter Init", me->mtMxID, unOutPortID);
  
  if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu invalid port ID [%lu]", me->mtMxID, unOutPortID);
    return ADSP_EUNEXPECTED;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
  if(NULL==pCurrentOutPort)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] pointer is NULL", me->mtMxID, unOutPortID);
		return ADSP_EFAILED;
	}
  
  pLimiter = &(pCurrentOutPort->structLimiter);

  adsp_amdb_module_handle_info_t module_handle_info_ptr[1];
  module_handle_info_ptr[0].interface_type = CAPI_V2;
  module_handle_info_ptr[0].type = AMDB_MODULE_TYPE_GENERIC;
  module_handle_info_ptr[0].id1 = VOICE_MODULE_TX_STREAM_LIMITER;
  module_handle_info_ptr[0].id2 = 0;
  module_handle_info_ptr[0].h.capi_v2_handle = NULL;
  module_handle_info_ptr[0].result = ADSP_EFAILED;

  capi_v2_init_memory_requirement_t memory_req = { 0 };
  capi_v2_prop_t static_props[] =
  {
      { CAPI_V2_INIT_MEMORY_REQUIREMENT,
          { reinterpret_cast<int8_t*>(&memory_req), 0, sizeof(memory_req) },
          { FALSE, FALSE, 0 }
      },
  };
  capi_v2_proplist_t static_proplist =
  { SIZE_OF_ARRAY(static_props), static_props };

  capi_v2_heap_id_t heap_id = { 0 };
  capi_v2_event_callback_info_t cb_info = MtMx_LimiterGetCapiv2CallbackHandler(me,unOutPortID);
  capi_v2_port_num_info_t port_info = { 1, 1 };
  capi_v2_prop_t init_props[] =
  {
      { CAPI_V2_HEAP_ID,
          { reinterpret_cast<int8_t*>(&heap_id), sizeof(heap_id), sizeof(heap_id) },
          { FALSE, FALSE, 0 }
      },
      { CAPI_V2_EVENT_CALLBACK_INFO,
          { reinterpret_cast<int8_t*>(&cb_info), sizeof(cb_info), sizeof(cb_info) },
          { FALSE, FALSE, 0 }
      },
      { CAPI_V2_PORT_NUM_INFO,
          { reinterpret_cast<int8_t*>(&port_info), sizeof(port_info), sizeof(port_info) },
          { FALSE, FALSE, 0 }
      }
  };
  capi_v2_proplist_t init_proplist =
  { SIZE_OF_ARRAY(init_props), init_props };

  capi_v2_port_info_t capiv2_limiter_port_info;
  capiv2_limiter_port_info.is_valid = FALSE;

  struct
  {
    capi_v2_data_format_header_t d;
    capi_v2_standard_data_format_t f;
  } media_format;

  media_format.d.data_format = CAPI_V2_FIXED_POINT;

  media_format.f.bits_per_sample = MT_MX_BIT_WIDTH_32;
  media_format.f.num_channels = pCurrentOutPort->unNumChannels;
  media_format.f.bitstream_format = CAPI_V2_FIXED_POINT;
  media_format.f.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
  media_format.f.q_factor = elite_data_get_q_format_factor(pCurrentOutPort->unBitwidth);
  media_format.f.data_is_signed = TRUE;
  media_format.f.sampling_rate = pCurrentOutPort->unSampleRate;

  capi_v2_prop_t prop =
  { CAPI_V2_INPUT_MEDIA_FORMAT, /* capi_v2_property_id_t */
      /* capi_v2_buf_t */
      { reinterpret_cast<int8_t*>(&media_format), sizeof(media_format), sizeof(media_format) },
      /* capi_v2_port_info_t*/
      {
          TRUE,
          TRUE, 0,
      }
  };
  capi_v2_proplist_t proplist =
  { 1, &prop };

  int16_t limiter_enable = 1;

  adsp_amdb_get_modules_request(module_handle_info_ptr, 1, NULL, NULL);

  if(ADSP_FAILED(module_handle_info_ptr[0].result) ||
      (CAPI_V2 != module_handle_info_ptr[0].interface_type))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter Module not present in AMDB", me->mtMxID, unOutPortID);
    return ADSP_EFAILED;
  }

  result  = adsp_amdb_capi_v2_get_static_properties_f(module_handle_info_ptr[0].h.capi_v2_handle,NULL, &static_proplist);
  if(result != ADSP_EOK)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter get static properties failed", me->mtMxID, unOutPortID);
    adsp_amdb_release_handles(module_handle_info_ptr,1);
    return ADSP_EFAILED;
  }

  /*Allocating resampler module memory with the starting address is 8-byte aligned*/
  pLimiter->pLimiterCapiV2 = (capi_v2_t *)qurt_elite_memory_malloc(memory_req.size_in_bytes,
                                                                QURT_ELITE_HEAP_DEFAULT);
  if(NULL == pLimiter->pLimiterCapiV2)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Failed to allocate memory for matrix limiter instance", me->mtMxID, unOutPortID);
    adsp_amdb_release_handles(module_handle_info_ptr,1);
    return ADSP_ENOMEMORY;
  }
  memset(pLimiter->pLimiterCapiV2, 0, memory_req.size_in_bytes);

  result = adsp_amdb_capi_v2_init_f(module_handle_info_ptr[0].h.capi_v2_handle,
                                    (capi_v2_t *)pLimiter->pLimiterCapiV2,
                                    &init_proplist);
  if(result != ADSP_EOK)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter init failed", me->mtMxID, unOutPortID);
    qurt_elite_memory_free(pLimiter->pLimiterCapiV2);
    adsp_amdb_release_handles(module_handle_info_ptr,1);
    return ADSP_EFAILED;
  }

  adsp_amdb_release_handles(module_handle_info_ptr,1);

  limiter_config_params_t limiter_params;

  MtMx_GetLimiterDefaultParams(&limiter_params,pCurrentOutPort->unBytesPerSample,pCurrentOutPort->unNumChannels);
  capi_v2_buf_t limiter_buf;
  limiter_buf.data_ptr = (int8_t*)&limiter_params;
  limiter_buf.actual_data_len = sizeof(limiter_params);
  limiter_buf.max_data_len = sizeof(limiter_params);

  if(NULL != pLimiter->pLimiterCapiV2->vtbl_ptr)
  {
    result = pLimiter->pLimiterCapiV2->vtbl_ptr->set_param(pLimiter->pLimiterCapiV2,
                                              CAPI_V2_LIMITER_PARAM_ID_CONFIG_PARAMS,
                                              &capiv2_limiter_port_info, &limiter_buf);

    if(CAPI_V2_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance failed to send set params", me->mtMxID, unOutPortID);
      result = ADSP_EFAILED;
      goto __bailout;
    }
  }

  limiter_buf.data_ptr = (int8_t*)&limiter_enable;
  limiter_buf.actual_data_len = sizeof(limiter_enable);
  limiter_buf.max_data_len = sizeof(limiter_enable);

  if(NULL != pLimiter->pLimiterCapiV2->vtbl_ptr)
  {
    result = pLimiter->pLimiterCapiV2->vtbl_ptr->set_param(pLimiter->pLimiterCapiV2,
                                              AUDPROC_PARAM_ID_ENABLE,
                                              &capiv2_limiter_port_info, &limiter_buf);

    if(CAPI_V2_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance failed to send enable set params", me->mtMxID, unOutPortID);      
      result = ADSP_EFAILED;
      goto __bailout;
    }
  }

  if(NULL != pLimiter->pLimiterCapiV2->vtbl_ptr)
  {
    result = pLimiter->pLimiterCapiV2->vtbl_ptr->set_properties(pLimiter->pLimiterCapiV2, &proplist);
    if(CAPI_V2_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance failed to set property for input media format", me->mtMxID, unOutPortID);
      result = ADSP_EFAILED;
      goto __bailout;
    }
  }
  media_format.f.bits_per_sample = (pCurrentOutPort->unBitwidth > MT_MX_BIT_WIDTH_16)? MT_MX_BIT_WIDTH_32 : MT_MX_BIT_WIDTH_16;
  media_format.f.q_factor = elite_data_get_q_format_factor(pCurrentOutPort->unBitwidth);
  prop.id = CAPI_V2_OUTPUT_MEDIA_FORMAT;
  if(NULL != pLimiter->pLimiterCapiV2->vtbl_ptr)
  {
    result = pLimiter->pLimiterCapiV2->vtbl_ptr->set_properties(pLimiter->pLimiterCapiV2, &proplist);
    if(CAPI_V2_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance failed to set property for output media format", me->mtMxID, unOutPortID);
      result = ADSP_EFAILED;
      goto __bailout;
    }
  }

  return result;
  __bailout:
  result = MtMx_OutPortLimiterDeinit(me, unOutPortID);
  return result;
}

/* De-initializes the limiter for the port.
 */
ADSPResult MtMx_OutPortLimiterDeinit(This_t *me, uint32_t unOutPortID)
{
  ADSPResult result = ADSP_EOK;
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  mt_mx_struct_limiter_t *pLimiter = NULL;
  
  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port ID [%lu] Processing Limiter De-Init", me->mtMxID, unOutPortID);
  
  if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu invalid port ID [%lu]", me->mtMxID, unOutPortID);
    return ADSP_EUNEXPECTED;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
  if(NULL==pCurrentOutPort)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] pointer is NULL", me->mtMxID, unOutPortID);
    return ADSP_EFAILED;
  }
  
  pLimiter = &(pCurrentOutPort->structLimiter);
  
  if(NULL == pLimiter->pLimiterCapiV2)
  {
     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Matrix Limiter: Limiter instance already freed");
     return ADSP_EFAILED;
  }
  if(NULL != (pLimiter->pLimiterCapiV2->vtbl_ptr))
  {
    result = pLimiter->pLimiterCapiV2->vtbl_ptr->end((capi_v2_t *)pLimiter->pLimiterCapiV2);
    if(CAPI_V2_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Matrix Limiter: failed to end Limiter instance");
    }
    qurt_elite_memory_free(pLimiter->pLimiterCapiV2);
    pLimiter->pLimiterCapiV2 = NULL;
    
    //clear limiter structure
    memset(pLimiter, 0, sizeof(mt_mx_struct_limiter_t));
    
    //Update matrix KPPS and/or BW and raise ADSPPM event if needed.
    MtMx_SetReqKppsAndBW(me);
  }
  return result;
}
capi_v2_err_t MtMx_LimiterCapiv2Callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
  capi_v2_err_t result = CAPI_V2_EOK;
  mt_mx_limiter_callback_obj_t      *pMtMxLimiterCbInfo = reinterpret_cast<mt_mx_limiter_callback_obj_t*>(context_ptr);
  MatrixOutPortInfoType      *pCurrentOutPort = NULL;
  uint32_t required_payload_size = 0;
  mt_mx_struct_limiter_t *pLimiter = NULL;
  bool_t bKppsVoteNeeded = FALSE;
  bool_t bBwVoteNeeded = FALSE;
  bool_t bDelayUpdated = FALSE;
  uint32_t unOutPortID;
  
  This_t *me = pMtMxLimiterCbInfo->me;
  
  unOutPortID =pMtMxLimiterCbInfo->unOutPortID;
  
  if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu invalid port ID [%lu]", me->mtMxID, unOutPortID);
    return ADSP_EUNEXPECTED;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
  
  if(NULL==pCurrentOutPort)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] pointer is NULL", me->mtMxID, unOutPortID);
		return CAPI_V2_EFAILED;
  }
  
  pLimiter = &(pCurrentOutPort->structLimiter);

  switch(id)
  {
    case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
    {
      //Note: The delay reported here is tuning delay value
      required_payload_size = sizeof(capi_v2_event_algorithmic_delay_t);
      if (event_info_ptr->payload.actual_data_len < required_payload_size)
      {
        CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
        break;
      }
      uint32_t delay = reinterpret_cast<capi_v2_event_algorithmic_delay_t*>(event_info_ptr->payload.data_ptr)->delay_in_us;
      if(pLimiter->unLimiterDelay != delay)
      {
        pLimiter->unLimiterDelay = delay;
        bDelayUpdated = TRUE;
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx Limiter instance delay value: %lu",pLimiter->unLimiterDelay);
      }
      break;
    }
    case CAPI_V2_EVENT_KPPS:
    {
      required_payload_size = sizeof(capi_v2_event_KPPS_t);
      if (event_info_ptr->payload.actual_data_len < required_payload_size)
      {
        CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
        break;
      }
      uint32_t kpps = reinterpret_cast<capi_v2_event_KPPS_t*>(event_info_ptr->payload.data_ptr)->KPPS;
      if(pLimiter->unLimiterKpps != kpps)
      {
        pLimiter->unLimiterKpps = kpps;
        bKppsVoteNeeded = TRUE;
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx Limiter instance KPPS value: %lu",pLimiter->unLimiterKpps);
      }        
      break;
    }
    case CAPI_V2_EVENT_BANDWIDTH:
    {
      required_payload_size = sizeof(capi_v2_event_bandwidth_t);
      if (event_info_ptr->payload.actual_data_len < required_payload_size)
      {
        CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
        break;
      }
      capi_v2_event_bandwidth_t *bandwidth_info_ptr = reinterpret_cast<capi_v2_event_bandwidth_t*>(event_info_ptr->payload.data_ptr);
      uint32_t bandwidth = bandwidth_info_ptr->code_bandwidth + bandwidth_info_ptr->data_bandwidth;
      if(pLimiter->unLimiterBw != bandwidth)
      {
        pLimiter->unLimiterBw = bandwidth;
        bBwVoteNeeded = TRUE;
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx Limiter instance BW value: %lu",pLimiter->unLimiterBw); 
      }        
      break;
    }    
    default:
    {
      result = CAPI_V2_EOK;
      break;
    }
  }
  
  if(bDelayUpdated)
  {
    // if the output bit-width is 32 skipping limiter processing and hence resets delay value for the limiter instance if created.
    // Currently limiter module doesn't support true 32 bit processing. This code check condition should be removed when limiter module is enabled for 32bit in future.
    if(MT_MX_BIT_WIDTH_32 == pCurrentOutPort->unBitwidth)
    {
      pLimiter->unLimiterDelay =0;
    }
    MtMx_OutputPortLimiterDelayUpdate(me, unOutPortID);
  }
  
  if(bKppsVoteNeeded || bBwVoteNeeded)
  {
    // if the output bit-width is 32 skipping limiter processing and hence devoting the kpps and bw for the limiter instance if created.
    // Currently limiter module doesn't support true 32 bit processing. This code check condition should be removed when limiter module is enabled for 32bit in future.
    if(MT_MX_BIT_WIDTH_32 == pCurrentOutPort->unBitwidth)
    {
      pLimiter->unLimiterKpps =0;
      pLimiter->unLimiterBw =0;
    }
    result = MtMx_SetReqKppsAndBW(me); 
  }

  return result;
}

ADSPResult MtMx_OutputPortLimiterDelayUpdate(This_t *me, uint32_t unOutPortID)
{
  ADSPResult result = ADSP_EOK;
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  uint32_t unInPortId_temp = 0;
  uint32_t currConnInPorts = 0;
  
  if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu invalid port ID [%lu]", me->mtMxID, unOutPortID);
    return ADSP_EUNEXPECTED;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
    
  if (NULL != pCurrentOutPort)
  {
    // retrieve connected input port mask
    currConnInPorts = pCurrentOutPort->inPortsMask;
    // iterate through the connected input ports
    while (currConnInPorts)
    {
      unInPortId_temp = Q6_R_ct0_R(currConnInPorts);
      currConnInPorts ^= (1 << unInPortId_temp);    
      MtMx_CalculateStreamDelay(me, unInPortId_temp);
    } // end while
  } // end if
  
  return result;
}

ADSPResult MtMx_OutPortLimiterSetParams(This_t *me, uint32_t unOutPortID, uint32_t  unParamID, void* param_data_ptr, uint32_t param_size)
{
  ADSPResult result = ADSP_EOK;
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  mt_mx_struct_limiter_t *pLimiter = NULL;
  
  if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu invalid port ID [%lu]", me->mtMxID, unOutPortID);
    return ADSP_EUNEXPECTED;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
  if(NULL==pCurrentOutPort)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] pointer is NULL", me->mtMxID, unOutPortID);
    return ADSP_EFAILED;
  }
  
  pLimiter = &(pCurrentOutPort->structLimiter);  
  
  if(NULL == pLimiter->pLimiterCapiV2)
  {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance not created ", me->mtMxID, unOutPortID);
     return ADSP_EFAILED;
  }  
 
  switch(unParamID)
  {
    case VOICE_PARAM_TX_STREAM_LIMITER:
    {
      if(param_size < sizeof(tx_stream_limiter_t))
      {
        result = ADSP_EBADPARAM;
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter Set param id 0x%lu, Bad param size %lu", me->mtMxID, unOutPortID, unParamID, param_size);
        break;
      }
      else
      {
        capi_v2_buf_t param_buf;
        limiter_config_params_t limiter_config_payload;
        tx_stream_limiter_t *tx_stream_limiter_ptr = (tx_stream_limiter_t*)param_data_ptr;
        
        MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                  "Limiter Set Config params, sLimMode=%x, sLimMakeUpGain=%x, sLimGc=%x, sLimDelay=%x, sLimMaxWait=%x, nLimThreshold=%x",
                  tx_stream_limiter_ptr->sLimMode, tx_stream_limiter_ptr->sLimMakeUpGain,
                  tx_stream_limiter_ptr->sLimGc, tx_stream_limiter_ptr->sLimDelay,
                  tx_stream_limiter_ptr->sLimMaxWait, tx_stream_limiter_ptr->nLimThreshold); 
        
        limiter_config_payload.mode = tx_stream_limiter_ptr->sLimMode;
        limiter_config_payload.makeup_gain = tx_stream_limiter_ptr->sLimMakeUpGain;
        limiter_config_payload.gc = tx_stream_limiter_ptr->sLimGc;
        limiter_config_payload.delay = tx_stream_limiter_ptr->sLimDelay;
        limiter_config_payload.max_wait = tx_stream_limiter_ptr->sLimMaxWait;
        limiter_config_payload.threshold = tx_stream_limiter_ptr->nLimThreshold;
        limiter_config_payload.hard_threshold = tx_stream_limiter_ptr->nLimThreshold;
        
        //remaning params set as default values
        limiter_config_payload.gain_attack = MATRIX_LIM_GAIN_ATTACK_CONSTANT_Q31;
        limiter_config_payload.gain_release = MATRIX_LIM_GAIN_RELEASE_CONSTANT_Q31;
        limiter_config_payload.attack_coef  = MATRIX_LIM_GAIN_ATTACK_COEFFICIENT_Q15;
        limiter_config_payload.release_coef = MATRIX_LIM_GAIN_RELEASE_COEFFICIENT_Q15;
        limiter_config_payload.history_window_length = MATRIX_LIM_HISTORY_WINLEN_10MS_Q15;
        
        param_buf.data_ptr = (int8_t*)&limiter_config_payload;
        param_buf.actual_data_len = sizeof(limiter_config_payload);
        param_buf.max_data_len = sizeof(limiter_config_payload);
        result = pLimiter->pLimiterCapiV2->vtbl_ptr->set_param(pLimiter->pLimiterCapiV2,
                                              CAPI_V2_LIMITER_PARAM_ID_CONFIG_PARAMS,
                                              NULL, &param_buf);
        if(CAPI_V2_FAILED(result))
        {
          result = ADSP_EFAILED;
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] failed to send limiter config set params", me->mtMxID, unOutPortID);
        }
      }
    }
     break;
    default:
    {
      result = ADSP_EBADPARAM;
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter module unsupported Param ID:0x%lu ", me->mtMxID, unOutPortID, unParamID);
       
    }
      break;
  } //end of switch case
  
  return result;
}

ADSPResult MtMx_OutPortLimiterGetParams(This_t *me, uint32_t unOutPortID, uint32_t  unParamID, int8_t* param_buffer_ptr, int32_t param_buf_len, uint32_t* params_buffer_len_req_ptr )
{
  ADSPResult result = ADSP_EOK;
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  mt_mx_struct_limiter_t *pLimiter = NULL;
  capi_v2_buf_t param_buf;
  
  if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu invalid port ID [%lu]", me->mtMxID, unOutPortID);
    return ADSP_EUNEXPECTED;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
  if(NULL==pCurrentOutPort)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] pointer is NULL", me->mtMxID, unOutPortID);
    return ADSP_EFAILED;
  }
  
  pLimiter = &(pCurrentOutPort->structLimiter);  
  
  if(NULL == pLimiter->pLimiterCapiV2)
  {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance not created ", me->mtMxID, unOutPortID);
     return ADSP_EFAILED;
  }  
 
  switch(unParamID)
  {
    case AUDPROC_PARAM_ID_ENABLE:
    {
      param_buf.data_ptr = (int8_t *)param_buffer_ptr;
      param_buf.actual_data_len = param_buf.max_data_len = param_buf_len;
      result = capi_v2_err_to_adsp_result(pLimiter->pLimiterCapiV2->vtbl_ptr->get_param(pLimiter->pLimiterCapiV2, AUDPROC_PARAM_ID_ENABLE, NULL, &param_buf));
      *params_buffer_len_req_ptr = param_buf.actual_data_len;
      break;
    }
    case VOICE_PARAM_TX_STREAM_LIMITER:
    {
      if(param_buf_len >= sizeof(tx_stream_limiter_t))
      {
        limiter_config_params_t limiter_config_payload;
        param_buf.data_ptr = (int8_t *)&limiter_config_payload;
        param_buf.actual_data_len = param_buf.max_data_len = sizeof(limiter_config_payload);
        result = capi_v2_err_to_adsp_result(pLimiter->pLimiterCapiV2->vtbl_ptr->get_param(pLimiter->pLimiterCapiV2, CAPI_V2_LIMITER_PARAM_ID_CONFIG_PARAMS, NULL, &param_buf)); 
        
        if(ADSP_EOK == result)
        {
         tx_stream_limiter_t *tx_stream_limiter_ptr = (tx_stream_limiter_t *)param_buffer_ptr;
         
         tx_stream_limiter_ptr->sLimMode = limiter_config_payload.mode;
         tx_stream_limiter_ptr->sLimMakeUpGain = limiter_config_payload.makeup_gain;
         tx_stream_limiter_ptr->sLimGc = limiter_config_payload.gc;
         tx_stream_limiter_ptr->sLimDelay = limiter_config_payload.delay;
         tx_stream_limiter_ptr->sLimMaxWait = limiter_config_payload.max_wait;
         tx_stream_limiter_ptr->nLimThreshold = limiter_config_payload.threshold;
         
         *params_buffer_len_req_ptr = sizeof(tx_stream_limiter_t);

         MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                  "Limiter Get Config params, sLimMode=%x, sLimMakeUpGain=%x, sLimGc=%x, sLimDelay=%x, sLimMaxWait=%x, nLimThreshold=%x",
                  tx_stream_limiter_ptr->sLimMode, tx_stream_limiter_ptr->sLimMakeUpGain,
                  tx_stream_limiter_ptr->sLimGc, tx_stream_limiter_ptr->sLimDelay,
                  tx_stream_limiter_ptr->sLimMaxWait, tx_stream_limiter_ptr->nLimThreshold);         
        }
      }
      else
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Limiter Get param, Bad param size %lu", param_buf_len);
      }
    }
     break;
    default:
    {
      result = ADSP_EBADPARAM;
      break;
    }
  }
 return result; 
}

/* send bypass mode to the limiter.
 */
ADSPResult MtMx_OutPortLimiterSetMode(This_t *me, uint32_t unOutPortID, bool_t bypass_mode)
{
  ADSPResult result = ADSP_EOK;
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  mt_mx_struct_limiter_t *pLimiter = NULL;
  
  if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu invalid port ID [%lu]", me->mtMxID, unOutPortID);
    return ADSP_EUNEXPECTED;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
  if(NULL==pCurrentOutPort)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] pointer is NULL", me->mtMxID, unOutPortID);
    return ADSP_EFAILED;
  }
  
  pLimiter = &(pCurrentOutPort->structLimiter);  
  if(NULL == pLimiter->pLimiterCapiV2)
  {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance not created ", me->mtMxID, unOutPortID);
     return ADSP_EFAILED;
  }
  if(NULL != pLimiter->pLimiterCapiV2->vtbl_ptr)
  {
    capi_v2_port_info_t capiv2_limiter_port_info;
    capiv2_limiter_port_info.is_valid = FALSE;

    bypass_mode_t bypass_mode_params;

    bypass_mode_params.bypass_mode = bypass_mode;

    capi_v2_buf_t limiter_buf;
    limiter_buf.data_ptr = (int8_t*)&bypass_mode_params;
    limiter_buf.actual_data_len = sizeof(bypass_mode_params);
    limiter_buf.max_data_len = sizeof(bypass_mode_params);
    result = pLimiter->pLimiterCapiV2->vtbl_ptr->set_param(pLimiter->pLimiterCapiV2,
                                              CAPI_V2_LIMITER_BYPASS_MODE,
                                              &capiv2_limiter_port_info, &limiter_buf);
    if(CAPI_V2_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] failed to send bypass mode limiter set params", me->mtMxID, unOutPortID);
      return ADSP_EFAILED;
    }
  }
  
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port ID [%lu] processed limiter bypass mode:%d ", me->mtMxID, unOutPortID,bypass_mode );
  return result;
}

ADSPResult MtMx_OutPortLimiterSetMediafmt(This_t *me, uint32_t unOutPortID)
{
  ADSPResult result = ADSP_EOK;
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  mt_mx_struct_limiter_t *pLimiter = NULL;
  
  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port ID [%lu] Processing Limiter Set MediaFmt", me->mtMxID, unOutPortID);
  
  if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu invalid port ID [%lu]", me->mtMxID, unOutPortID);
    return ADSP_EUNEXPECTED;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
  if(NULL==pCurrentOutPort)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] pointer is NULL", me->mtMxID, unOutPortID);
    return ADSP_EFAILED;
  }
  
  pLimiter = &(pCurrentOutPort->structLimiter);

  if(NULL == pLimiter->pLimiterCapiV2)
  {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance not created ", me->mtMxID, unOutPortID);
     return ADSP_EFAILED;
  }
  
  struct
  {
    capi_v2_data_format_header_t d;
    capi_v2_standard_data_format_t f;
  } media_format;

  media_format.d.data_format = CAPI_V2_FIXED_POINT;

  media_format.f.bits_per_sample = MT_MX_BIT_WIDTH_32;
  media_format.f.num_channels = pCurrentOutPort->unNumChannels;
  media_format.f.bitstream_format = CAPI_V2_FIXED_POINT;
  media_format.f.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
  media_format.f.q_factor = elite_data_get_q_format_factor(pCurrentOutPort->unBitwidth);
  media_format.f.data_is_signed = TRUE;
  media_format.f.sampling_rate = pCurrentOutPort->unSampleRate;
  
  for(uint16_t i = 0; i < pCurrentOutPort->unNumChannels; i++)
  {
    media_format.f.channel_type[i] = pCurrentOutPort->unChannelMapping[i];
  }

  capi_v2_prop_t prop =
  { CAPI_V2_INPUT_MEDIA_FORMAT, /* capi_v2_property_id_t */
      /* capi_v2_buf_t */
      { reinterpret_cast<int8_t*>(&media_format), sizeof(media_format), sizeof(media_format) },
      /* capi_v2_port_info_t*/
      {
          TRUE,
          TRUE, 0,
      }
  };
  capi_v2_proplist_t proplist =
  { 1, &prop };

  if(NULL != pLimiter->pLimiterCapiV2->vtbl_ptr)
  {
    result = pLimiter->pLimiterCapiV2->vtbl_ptr->set_properties(pLimiter->pLimiterCapiV2, &proplist);
    if(CAPI_V2_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance failed to set property for input media format ", me->mtMxID, unOutPortID);
      result = ADSP_EFAILED;
      goto __bailout;
    }
  }
  media_format.f.bits_per_sample = (pCurrentOutPort->unBitwidth > MT_MX_BIT_WIDTH_16)? MT_MX_BIT_WIDTH_32 : MT_MX_BIT_WIDTH_16;
  media_format.f.q_factor = elite_data_get_q_format_factor(pCurrentOutPort->unBitwidth);
  prop.id = CAPI_V2_OUTPUT_MEDIA_FORMAT;
  if(NULL != pLimiter->pLimiterCapiV2->vtbl_ptr)
  {
    result = pLimiter->pLimiterCapiV2->vtbl_ptr->set_properties(pLimiter->pLimiterCapiV2, &proplist);
    if(CAPI_V2_FAILED(result))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port ID [%lu] Limiter instance failed to set property for output media format ", me->mtMxID, unOutPortID);
      result = ADSP_EFAILED;
      goto __bailout;
    }
  }

  return result;
  __bailout:
  result = MtMx_OutPortLimiterDeinit(me, unOutPortID);
  return result;    

}


void MtMx_OutPortLimiterProcess(This_t *me, uint32_t unOutPortID, int8_t *in_buf_ptr, int8_t *out_buf_ptr, uint32_t num_samples)
{
  uint16_t i = 0;
  MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
  uint32_t nChan = pCurrentOutPort->unNumChannels;
  capi_v2_stream_data_t inp_buf_stream;
  capi_v2_stream_data_t out_buf_stream;
  capi_v2_stream_data_t *inp_buf_stream_ptr;
  capi_v2_stream_data_t *out_buf_stream_ptr;
  mt_mx_struct_limiter_t *pLimiter = &(pCurrentOutPort->structLimiter);


  if(NULL == pLimiter->pLimiterCapiV2)
  {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Matrix Limiter: Limiter instance pointer in NULL");
     return;
  }
  
  memset(&inp_buf_stream, 0, sizeof(capi_v2_stream_data_t));
  memset(&out_buf_stream, 0, sizeof(capi_v2_stream_data_t));

  capi_v2_buf_t input_buf[MT_MX_NUM_CHANNELS_EIGHT];
  capi_v2_buf_t output_buf[MT_MX_NUM_CHANNELS_EIGHT];
  memset(&input_buf, 0, MT_MX_NUM_CHANNELS_EIGHT * sizeof(capi_v2_buf_t));
  memset(&output_buf, 0, MT_MX_NUM_CHANNELS_EIGHT * sizeof(capi_v2_buf_t));

  inp_buf_stream_ptr = &inp_buf_stream;
  inp_buf_stream.bufs_num = nChan;

  for(i = 0; i < nChan; i++)
  {
    /* client samples are accumulated in 32bit to avoid overflow*/
    input_buf[i].data_ptr = (in_buf_ptr + (i * num_samples * sizeof(int32_t)));
    input_buf[i].actual_data_len = num_samples * sizeof(int32_t);
    input_buf[i].max_data_len = num_samples * sizeof(int32_t);
  }
  inp_buf_stream.buf_ptr = &input_buf[0];

  out_buf_stream_ptr = &out_buf_stream;
  out_buf_stream.bufs_num = nChan;

  for(i = 0; i < nChan; i++)
  {
      output_buf[i].data_ptr = out_buf_ptr + (i * num_samples * pCurrentOutPort->unBytesPerSample);
      output_buf[i].actual_data_len = 0;
      output_buf[i].max_data_len = num_samples * pCurrentOutPort->unBytesPerSample;
  }
  out_buf_stream.buf_ptr = &output_buf[0];
  
  //Call Process function
  (void) pLimiter->pLimiterCapiV2->vtbl_ptr->process(pLimiter->pLimiterCapiV2, &inp_buf_stream_ptr,
                  &out_buf_stream_ptr);

  return;
}
