/*========================================================================

 *//** @file audio_module_chain_wrapper.cpp
This file contains wrapper functions to call CAPIv2 modules

Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/utils/src/audio_module_chain_wrapper.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/17   rishabht      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "EliteMsg_Custom.h"
#include "audio_module_chain_wrapper.h"
#include "audpp_util.h"


/* -----------------------------------------------------------------------
 ** Macro Declarations
 ** ----------------------------------------------------------------------- */
#define IS_ALIGN_4_BYTE(a) (!(uint32_t(a) & 0x3))
#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

//#define DEBUG_modules

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
static const aud_module_media_fmt_t invalid_media_format = {
        //.h
        {
                CAPI_V2_FIXED_POINT
        },
        //.f
        {
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_INVALID_INTERLEAVING,
                { 0 }
        }
};


static capi_v2_err_t audio_module_chain_wrapper_event_callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr);
static ADSPResult audio_module_chain_wrapper_stream_buf_init(AudioModuleCapiWrapper_t *modules, uint32_t out_buf_num_unit_frames);
static void audio_module_chain_wrapper_port_initialize(module_port_t *port_info);
static void audio_module_chain_wrapper_free_ports(module_info_t  *pModule_info);
static ADSPResult audio_module_chain_wrapper_allocate_ports(module_info_t  *pModule_info);
static ADSPResult audio_module_chain_wrapper_create_capi_v2_module(module_CAPI_container_t *module_capi_container, uint32_t *stack_size_ptr, capi_v2_proplist_t *init_proplist_ptr);
static ADSPResult audio_module_chain_wrapper_init(module_CAPI_container_t *module_capi_container, uint32_t *stack_size_ptr, avcs_module_instance_info_t *module_instance_info);
static void audio_module_chain_wrapper_free(module_CAPI_container_t *module_capi_container);
static uint32_t calculate_q_factor(uint32_t bits_per_sample);
static void audio_module_chain_wrapper_destroy_stream_buf(AudioModuleCapiWrapper_t *modules);
static bool_t is_amdb_module_handle_valid(const adsp_amdb_module_handle_info_t *amdb_module_handle_ptr);
static void find_req_module_index(AudioModuleCapiWrapper_t *modules, uint32_t module_id, uint32_t instance_id, int32_t *module_index);


void audio_module_chain_wrapper_process(AudioModuleCapiWrapper_t *modules, uint64_t *pPayload)
{
   qurt_elite_bufmgr_node_t *outBufMgrNode = (qurt_elite_bufmgr_node_t *)pPayload;
   elite_msg_data_buffer_t *pOutBuf = (elite_msg_data_buffer_t *)outBufMgrNode->pBuffer;

   int8_t *in_buf;
   int8_t *in_ptr[MAX_NUM_CHANNEL];
   int8_t *out_ptr[MAX_NUM_CHANNEL];
   uint32_t byte_offset = 0;
   uint32_t out_chan_spacing_in_bytes, bytes_to_copy;
   uint32_t actual_data_len[MODULE_MAX_INPUT_STREAMS];
   bool_t isTimeStampValid = (0 == GET_DATA_BUFFER_TIME_STAMP_BIT(pOutBuf->nFlag)) ? FALSE : TRUE;

   //space for deinterleaving
   in_buf = ( ((int8_t *)(&pOutBuf->nDataBuf)) + byte_offset);


   for(uint32_t i = 0; i < modules->num_modules; i++)
   {
      module_info_t * pModule_info = &modules->module_capi_container[i].module_info;
      if((!pModule_info->forced_disabled) && (pModule_info->is_enabled))
      {
          for(uint32_t port_id = 0; port_id < pModule_info->port_info.num_input_ports; port_id++)
          {
              //out_chan_spacing_in_bytes = pOutBuf->nMaxSize/pModule_info->input_ports_ptr[port_id].media_event.event.f.num_channels;
              actual_data_len[port_id] = pOutBuf->nActualSize/pModule_info->input_ports_ptr[port_id].media_event.event.f.num_channels;
              for(uint32_t ch = 0; ch < pModule_info->input_ports_ptr[port_id].media_event.event.f.num_channels; ch++)
              {
                  in_ptr[ch] = in_buf + (ch * actual_data_len[port_id]);
                  pModule_info->stream.in_buf[port_id][ch].data_ptr = in_ptr[ch];
                  pModule_info->stream.in_buf[port_id][ch].actual_data_len = actual_data_len[port_id];
                  pModule_info->stream.in_buf[port_id][ch].max_data_len = actual_data_len[port_id];
              }

              pModule_info->stream.inputs[port_id].bufs_num = pModule_info->input_ports_ptr[port_id].media_event.event.f.num_channels;
              pModule_info->stream.inputs[port_id].timestamp = (int64_t)pOutBuf->ullTimeStamp;
              pModule_info->stream.inputs[port_id].flags.is_timestamp_valid = isTimeStampValid;

              if(pModule_info->is_in_place)
              {
                  for(uint32_t ch = 0; ch < pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels; ch++)
                  {
                      out_ptr[ch] = in_buf + (ch * actual_data_len[port_id]);
                      pModule_info->stream.out_buf[port_id][ch].data_ptr = out_ptr[ch];
                      pModule_info->stream.out_buf[port_id][ch].actual_data_len = 0;
                      pModule_info->stream.out_buf[port_id][ch].max_data_len = actual_data_len[port_id];
                  }
              }
              else
              {
                  out_chan_spacing_in_bytes = modules->OutBufSize/pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels;
                  for(uint32_t ch = 0; ch < pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels; ch++)
                  {
                      out_ptr[ch] = modules->OutBuf[port_id] + (ch * out_chan_spacing_in_bytes);
                      pModule_info->stream.out_buf[port_id][ch].data_ptr = out_ptr[ch];
                      pModule_info->stream.out_buf[port_id][ch].actual_data_len = 0;
                      pModule_info->stream.out_buf[port_id][ch].max_data_len = out_chan_spacing_in_bytes;
                  }
              }
              pModule_info->stream.outputs[port_id].bufs_num = pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels;
          }

          pModule_info->module_ptr->vtbl_ptr->process(pModule_info->module_ptr, pModule_info->stream.inputs_ptr, pModule_info->stream.outputs_ptr);
#ifdef DEBUG_modules
           MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Input buffer timestamp %lu and output buffer timestamp %lu", (uint32_t)pModule_info->stream.inputs[0].timestamp, (uint32_t)pModule_info->stream.outputs[0].timestamp);
#endif
          // TODO :check if module has consumed all the samples and produced same output
          if(!pModule_info->is_in_place)
          {
             for(uint32_t port_id = 0; port_id < pModule_info->port_info.num_output_ports; port_id++)
             {
                 //out_chan_spacing_in_bytes = pOutBuf->nMaxSize/pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels;
                 actual_data_len[port_id] = pOutBuf->nActualSize/pModule_info->input_ports_ptr[port_id].media_event.event.f.num_channels;
                 for(uint32_t ch = 0; ch < pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels; ch++)
                 {
                    in_ptr[ch] = in_buf + (ch * actual_data_len[port_id]);
                    out_ptr[ch] = pModule_info->stream.out_buf[port_id][ch].data_ptr;
                    bytes_to_copy = pModule_info->stream.out_buf[port_id][ch].actual_data_len;
                    memscpy(in_ptr[ch], actual_data_len[port_id], out_ptr[ch], bytes_to_copy);
                 }
             }
          }

          if(isTimeStampValid)
          {
             pOutBuf->ullTimeStamp = (uint64_t)pModule_info->stream.outputs[0].timestamp;
		  }
      }

   }

   return;
}


static capi_v2_err_t audio_module_chain_wrapper_event_callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
   capi_v2_err_t result = CAPI_V2_EOK;

   capi_v2_buf_t *payload = &event_info_ptr->payload;

   module_info_t *pModule_info = reinterpret_cast<module_info_t*>(context_ptr);

   if (payload->actual_data_len > payload->max_data_len)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in audio_module_chain_wrapper_event_callback function. The actual size %lu is greater than the max size %lu for id %lu.",
            payload->actual_data_len, payload->max_data_len, static_cast<uint32_t>(id));
      return CAPI_V2_EBADPARAM;
   }

   switch(id)
   {
   case CAPI_V2_EVENT_KPPS:
   {
      if (payload->actual_data_len < sizeof(capi_v2_event_KPPS_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in audio_module_chain_wrapper_event_callback function. The actual size %lu is less than the required size %zu for id %lu.",
               payload->actual_data_len, sizeof(capi_v2_event_KPPS_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }

      capi_v2_event_KPPS_t *kpps = reinterpret_cast<capi_v2_event_KPPS_t*>(payload->data_ptr);

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Audio Module %lu KPPS  %lu", pModule_info->module_id, static_cast<uint32_t>(kpps->KPPS));

      if (pModule_info->KPPS != kpps->KPPS)
      {
         pModule_info->KPPS = kpps->KPPS;
      }

      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_BANDWIDTH:
   {
      if (payload->actual_data_len < sizeof(capi_v2_event_bandwidth_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in audio_module_chain_wrapper_event_callback function. The actual size %lu is less than the required size %zu for id %lu.",
               payload->actual_data_len, sizeof(capi_v2_event_bandwidth_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }

      capi_v2_event_bandwidth_t *bw = reinterpret_cast<capi_v2_event_bandwidth_t*>(payload->data_ptr);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Audio Module %lu BW  %lu", pModule_info->module_id, bw->data_bandwidth);

      if (pModule_info->bandwidth != bw->data_bandwidth)
      {
          pModule_info->bandwidth = bw->data_bandwidth;
      }
      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_PROCESS_STATE:
   {
      if (payload->actual_data_len < sizeof(capi_v2_event_process_state_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in audio_module_chain_wrapper_event_callback function. The actual size %lu is less than the required size %zu for id %lu.",
               payload->actual_data_len, sizeof(capi_v2_event_process_state_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }
      capi_v2_event_process_state_t *pData = (capi_v2_event_process_state_t*)payload->data_ptr;
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Audio Module %lu process check set to %lu", pModule_info->module_id, static_cast<uint32_t>(pData->is_enabled));

      if (pModule_info->is_enabled != pData->is_enabled)
      {
          pModule_info->is_enabled = pData->is_enabled;
      }

      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
   {
      if (payload->actual_data_len < sizeof(aud_module_media_fmt_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in audio_module_chain_wrapper_event_callback function. The actual size %lu is less than the required size %zu for id %lu.",
               payload->actual_data_len, sizeof(aud_module_media_fmt_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }

      aud_module_media_fmt_t *pData = reinterpret_cast<aud_module_media_fmt_t*>(payload->data_ptr);

      if (!event_info_ptr->port_info.is_valid)
      {
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Audio Module-Instance with id 0x%lX-0x%lX "
                  "provided invalid port for event id %lu. Only output port 0 is supported.",
                  pModule_info->module_id,  pModule_info->instance_id, static_cast<uint32_t>(id));
          return CAPI_V2_EUNSUPPORTED;
      }
      if (event_info_ptr->port_info.is_input_port)
      {
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Audio Module-Instance with id 0x%lX-0x%lX "
                  "provided input port for event id %lu. Only output port 0 is supported.",
                  pModule_info->module_id,  pModule_info->instance_id, static_cast<uint32_t>(id));
          return CAPI_V2_EUNSUPPORTED;
      }
      if(event_info_ptr->port_info.port_index >= pModule_info->port_info.num_output_ports)
      {
          MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Audio Module-Instance with id 0x%lX-0x%lX "
                  "provided port %lu for event id %lu. It has only %lu output ports.",
                  pModule_info->module_id,  pModule_info->instance_id,
                  event_info_ptr->port_info.port_index, static_cast<uint32_t>(id), pModule_info->port_info.num_output_ports);
          return CAPI_V2_EUNSUPPORTED;
      }

      if ((CAPI_V2_FIXED_POINT != pData->h.data_format))
      {
          MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Audio Module-Instance with id 0x%lX-0x%lX "
                  "provided data_format %lu for event id %lu. Only port FIXED_POINT(0)  are supported.",
                  pModule_info->module_id,  pModule_info->instance_id,
                  static_cast<uint32_t>(pData->h.data_format), static_cast<uint32_t>(id));
          return CAPI_V2_EUNSUPPORTED;
      }

      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
   {
      if (payload->actual_data_len < sizeof(capi_v2_event_algorithmic_delay_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in audio_module_chain_wrapper_event_callback function. The actual size %lu is less than the required size %zu for id %lu.",
               payload->actual_data_len, sizeof(capi_v2_event_algorithmic_delay_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }

      capi_v2_event_algorithmic_delay_t *delay = reinterpret_cast<capi_v2_event_algorithmic_delay_t*>(payload->data_ptr);

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Audio Module %lu Delay  %lu", pModule_info->module_id, static_cast<uint32_t>(delay->delay_in_us));

      if (pModule_info->delay_in_us != delay->delay_in_us)
      {
         pModule_info->delay_in_us = delay->delay_in_us;
      }

      return CAPI_V2_EOK;
   }
   default:
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Module %lu: Error in callback function. ID %lu not supported.", pModule_info->module_id, static_cast<uint32_t>(id));
      return CAPI_V2_EUNSUPPORTED;
   }
   }
   return result;
}


static void find_req_module_index(AudioModuleCapiWrapper_t *modules, uint32_t module_id, uint32_t instance_id, int32_t *module_index)
{
  uint32_t i = 0;
   for(i = 0; i < modules->num_modules; i++)
   {
      if((module_id == modules->module_capi_container[i].module_info.module_id)
          && (instance_id == modules->module_capi_container[i].module_info.instance_id))
      {
          break;
      }
   }

   if(i < modules->num_modules)
   {
      *module_index = (int32_t)i;
      return;
   }

   *module_index = -1;
   return;
}

ADSPResult audio_module_chain_wrapper_set_param(AudioModuleCapiWrapper_t *modules, void *pPayload, uint32_t payloadSize, uint32_t ulParamId)
{
   ADSPResult nResult = ADSP_EOK;
   uint8_t *pPacket = (uint8_t *)pPayload;
   uint32_t required_size = 0;

   if(NULL == modules)
   {
   	   return ADSP_ENOMEMORY;
   }

   if(!IS_ALIGN_4_BYTE(pPacket))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_set_param : pointer is not aligned to 4 bytes. Aborting.");
      return ADSP_EFAILED;
   }


   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audio_module_chain_wrapper set param begin");
   while(payloadSize > 0)
   {
       uint32_t module_id, instance_id, paramSize, param_id;
       int32_t module_index = 0;

       required_size = (ELITEMSG_PARAM_ID_CAL_V2 == ulParamId)?sizeof(asm_stream_param_data_v3_t):sizeof(asm_stream_param_data_v2_t);

       if(payloadSize < required_size)
       {
           MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_set_param: invalid payload size, aborting set param");
           return ADSP_EFAILED;
       }

       if(ELITEMSG_PARAM_ID_CAL_V2 == ulParamId)
       {
           asm_stream_param_data_v3_t *pDataPayload = (asm_stream_param_data_v3_t *)(pPacket);
           module_id = pDataPayload->module_id;
           instance_id = (uint32_t)pDataPayload->instance_id;
           paramSize = pDataPayload->param_size;
           param_id = pDataPayload->param_id;
       }
       else
       {
           asm_stream_param_data_v2_t *pDataPayload = (asm_stream_param_data_v2_t *)(pPacket);
           module_id = pDataPayload->module_id;
           instance_id = 0;
           paramSize = pDataPayload->param_size;
           param_id = pDataPayload->param_id;
       }

       pPacket += required_size;
       payloadSize -= required_size;
       required_size = paramSize;
       if(payloadSize < required_size)
       {
           MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudDevSvc: invalid payload size, aborting set param");
           return ADSP_EFAILED;
       }

       find_req_module_index(modules, module_id, instance_id, &module_index);

       if(module_index >= 0)
       {
          module_info_t *pModule_info = &modules->module_capi_container[module_index].module_info;
          capi_v2_err_t err_code = CAPI_V2_EOK;
          capi_v2_buf_t buf;
          buf.data_ptr = (int8_t *)pPacket;
          buf.max_data_len = paramSize;
          buf.actual_data_len = paramSize;
          capi_v2_t *module_ptr = pModule_info->module_ptr;
          capi_v2_port_info_t port_info;
          port_info.is_valid = FALSE;

          instance_id = pModule_info->instance_id;

          err_code = module_ptr->vtbl_ptr->set_param(module_ptr, param_id, &port_info, &buf);
          if (CAPI_V2_FAILED(err_code))
          {
             MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_set_param: Module-Instance with ID 0x%lX-0x%lX failed setParam.",
                 module_id, instance_id);
             nResult |= capi_v2_err_to_adsp_result(err_code);
          }
       }
       else
       {
           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_set_param: Module-Instance with ID 0x%lX-0x%lX not found",
               module_id, instance_id);
           nResult |= ADSP_EBADPARAM;
       }

       pPacket += required_size;
       payloadSize -= required_size;

   }

   return nResult;
}

ADSPResult audio_module_chain_wrapper_get_param(AudioModuleCapiWrapper_t *modules, void *pPayload, uint32_t *pParamsize, uint32_t max_size, uint32_t ulParamId)
{
   ADSPResult res = ADSP_EOK;
   uint32_t header_size = (ELITEMSG_PARAM_ID_CAL_V2 == ulParamId)?sizeof(asm_stream_param_data_v3_t):sizeof(asm_stream_param_data_v2_t);
   uint32_t module_id, param_id, instance_id;
   int32_t module_index;
   int8_t *payload = (int8_t *)pPayload;

    if(NULL == modules)
    {
		return ADSP_ENOMEMORY;
	}

   if(header_size > max_size)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_get_param received Bad size for param ID = 0x%lx",ulParamId);
       return ADSP_EBADPARAM;
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audio_module_chain_wrapper get param begin");

   if( ELITEMSG_PARAM_ID_CAL == ulParamId)
   {
           asm_stream_param_data_v2_t *pHeader = (asm_stream_param_data_v2_t*)(payload);
           module_id = pHeader->module_id;
           //If Get-Param comes without any instance-Id then, Get param is queried from the module-instance with instance-Id zero.s
           instance_id = 0;
           param_id = pHeader->param_id;
   }
   else //ELITEMSG_PARAM_ID_CAL_V2
   {
           asm_stream_param_data_v3_t *pHeader = (asm_stream_param_data_v3_t*)(payload);
           module_id = pHeader->module_id;
           instance_id = pHeader->instance_id;
           param_id = pHeader->param_id;
   }

   max_size -= header_size;
   find_req_module_index(modules, module_id, instance_id, &module_index);
   payload += header_size;

   if(module_index >= 0)
   {
      module_info_t *pModule_info = &modules->module_capi_container[module_index].module_info;
      capi_v2_err_t err_code = CAPI_V2_EOK;
      capi_v2_buf_t buf;
      buf.data_ptr = (int8_t *)payload;
      buf.max_data_len = max_size;
      capi_v2_t *module_ptr = pModule_info->module_ptr;
      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;

      instance_id = pModule_info->instance_id;

      err_code = module_ptr->vtbl_ptr->get_param(module_ptr, param_id, &port_info, &buf);
      if (CAPI_V2_FAILED(err_code))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_get_param: Module-Instance with ID 0x%lX-0x%lX failed getParam.",
             module_id, instance_id);
         res |= capi_v2_err_to_adsp_result(err_code);
      }
      else
      {
         *pParamsize = buf.actual_data_len;
      }
   }
   else
   {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_get_param: Module-Instance with ID 0x%lX-0x%lX not found",
           module_id, instance_id);
       res |= ADSP_EBADPARAM;
   }
   return res;
}

static void audio_module_chain_wrapper_port_initialize(module_port_t *port_info)
{
  port_info->media_event.event = invalid_media_format;
  return;
}

static void audio_module_chain_wrapper_free_ports(module_info_t  *pModule_info)
{
   if(pModule_info->port_mem != NULL)
   {
       qurt_elite_memory_free(pModule_info->port_mem);
       pModule_info->port_mem = NULL;
   }

   return;
}

static ADSPResult audio_module_chain_wrapper_allocate_ports(module_info_t  *pModule_info)
{
    uint32_t total_port_info_size = 0;
    int8_t *mem;

    pModule_info->input_ports_ptr = NULL;
    pModule_info->output_ports_ptr = NULL;

    total_port_info_size += align_to_8_byte(pModule_info->port_info.num_input_ports * sizeof(module_port_t)) + align_to_8_byte(pModule_info->port_info.num_output_ports * sizeof(module_port_t));

    if(pModule_info->port_mem != NULL)
    {
        audio_module_chain_wrapper_free_ports(pModule_info);
    }

    pModule_info->port_mem = (void *)qurt_elite_memory_malloc(total_port_info_size, QURT_ELITE_HEAP_DEFAULT);
    if(NULL  == pModule_info->port_mem)
    {
        return ADSP_ENOMEMORY;
    }

    mem = (int8_t *)pModule_info->port_mem;
    pModule_info->input_ports_ptr = (module_port_t *)mem;
    mem += align_to_8_byte(pModule_info->port_info.num_input_ports * sizeof(module_port_t));

    pModule_info->output_ports_ptr = (module_port_t *)mem;
    mem += align_to_8_byte(pModule_info->port_info.num_output_ports * sizeof(module_port_t));

    for (uint32_t i = 0; i < pModule_info->port_info.num_input_ports; i++)
    {
        audio_module_chain_wrapper_port_initialize(&pModule_info->input_ports_ptr[i]);
    }

    for (uint32_t i = 0; i < pModule_info->port_info.num_output_ports; i++)
    {
        audio_module_chain_wrapper_port_initialize(&pModule_info->output_ports_ptr[i]);
    }

    return ADSP_EOK;
}

static ADSPResult audio_module_chain_wrapper_create_capi_v2_module(module_CAPI_container_t *module_capi_container, uint32_t *stack_size_ptr, capi_v2_proplist_t *init_proplist_ptr)
{
  //ADSPResult result = ADSP_EOK;
  capi_v2_err_t err_code = CAPI_V2_EOK;

  adsp_amdb_capi_v2_t *handle = module_capi_container->handle_info_ptr->h.capi_v2_handle;
  module_info_t * module_info_ptr = &module_capi_container->module_info;

 // Query static properties

  // First query for the properties which are not optional.
 capi_v2_init_memory_requirement_t memory_req = { 0 };
 capi_v2_prop_t req_props[] = {
         { CAPI_V2_INIT_MEMORY_REQUIREMENT, { reinterpret_cast<int8_t*>(&memory_req),   0, sizeof(memory_req)   }, { FALSE, FALSE, 0} },
 };
 capi_v2_proplist_t req_proplist = { SIZE_OF_ARRAY(req_props), req_props };
 err_code = adsp_amdb_capi_v2_get_static_properties_f(handle, init_proplist_ptr, &req_proplist);
 if(CAPI_V2_FAILED(err_code))
 {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper Module with ID 0x%lX failed in getting some required static properties with %d.",module_info_ptr->module_id, static_cast<int>(err_code));
     return capi_v2_err_to_adsp_result(err_code);
 }

 // Query for the optional properties now.
 capi_v2_init_memory_requirement_t dummy_memory_req = { 0 };
 capi_v2_is_inplace_t is_inplace = { FALSE };
 capi_v2_requires_data_buffering_t req_data_buf = { TRUE };
 capi_v2_stack_size_t stack_size = { 0 };
 capi_v2_num_needed_framework_extensions_t num_extensions = { 0 };
 capi_v2_prop_t optional_props[] = {
         { CAPI_V2_IS_INPLACE,              { reinterpret_cast<int8_t*>(&is_inplace),   0, sizeof(is_inplace)   }, { FALSE, FALSE, 0} },
         { CAPI_V2_REQUIRES_DATA_BUFFERING, { reinterpret_cast<int8_t*>(&req_data_buf), 0, sizeof(req_data_buf) }, { FALSE, FALSE, 0} },
         { CAPI_V2_STACK_SIZE,                       { reinterpret_cast<int8_t*>(&stack_size),     0, sizeof(stack_size)       }, { FALSE, FALSE, 0} },
         { CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS,  { reinterpret_cast<int8_t*>(&num_extensions), 0, sizeof(num_extensions)   }, { FALSE, FALSE, 0} },
         // We still need to query for the memory requirement here since amdb returns an error if it does not find it. This value is ignored.
         { CAPI_V2_INIT_MEMORY_REQUIREMENT, { reinterpret_cast<int8_t*>(&dummy_memory_req),   0, sizeof(dummy_memory_req)   }, { FALSE, FALSE, 0} },
 };
 capi_v2_proplist_t optional_proplist = { SIZE_OF_ARRAY(optional_props), optional_props };
 err_code = adsp_amdb_capi_v2_get_static_properties_f(handle, init_proplist_ptr, &optional_proplist);
 if(CAPI_V2_FAILED(err_code))
 {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper Module with ID 0x%lX failed in getting some optional static properties with %d. Using default values.", module_info_ptr->module_id, static_cast<int>(err_code));
     return capi_v2_err_to_adsp_result(err_code);
 }

 if((req_data_buf.requires_data_buffering == TRUE)
    || (num_extensions.num_extensions != 0)
    )
 {
     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper Module with ID 0x%lX returned unsupported properties, force disabling the module", module_info_ptr->module_id);
     module_info_ptr->forced_disabled = TRUE;
 }

 if(stack_size.size_in_bytes >  *stack_size_ptr)
 {
     *stack_size_ptr = stack_size.size_in_bytes;
 }

 // Allocate memory
 void *module_mem = (void *)qurt_elite_memory_malloc(memory_req.size_in_bytes, QURT_ELITE_HEAP_DEFAULT);
 if (NULL == module_mem)
 {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper Module with ID 0x%lX failed to allocate memory. The module requested for %lu bytes.", module_info_ptr->module_id, memory_req.size_in_bytes);
     return ADSP_ENOMEMORY;
 }
 MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audio_module_chain_wrapper Module with ID 0x%lX allocated for %lu bytes of memory", module_info_ptr->module_id, memory_req.size_in_bytes);

 module_info_ptr->module_ptr = reinterpret_cast<capi_v2_t*>(module_mem);

 // Initialize
 err_code = adsp_amdb_capi_v2_init_f(handle, module_info_ptr->module_ptr, init_proplist_ptr);
 if(CAPI_V2_FAILED(err_code))
 {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper Module with ID 0x%lX failed in init with %d.", module_info_ptr->module_id, static_cast<int>(err_code));
     qurt_elite_memory_free(module_info_ptr->module_ptr);
     module_info_ptr->module_ptr = NULL;
     return capi_v2_err_to_adsp_result(err_code);
 }

 return ADSP_EOK;

}

static bool_t is_amdb_module_handle_valid(const adsp_amdb_module_handle_info_t *amdb_module_handle_ptr)
{
   if(ADSP_FAILED(amdb_module_handle_ptr->result) || (NULL == amdb_module_handle_ptr->h.capi_v2_handle))
   {
      return FALSE;
   }
   return TRUE;
}

static ADSPResult audio_module_chain_wrapper_init(module_CAPI_container_t *module_capi_container, uint32_t *stack_size_ptr, avcs_module_instance_info_t *module_instance_info)
{
   ADSPResult result = ADSP_EOK;

   module_capi_container->module_info.module_id = module_instance_info->module_id;
   module_capi_container->module_info.instance_id = module_instance_info->instance_id;
   module_capi_container->module_info.KPPS = 0;
   module_capi_container->module_info.bandwidth = 0;
   module_capi_container->module_info.delay_in_us = 0;
   module_capi_container->module_info.forced_disabled = FALSE;
   module_capi_container->module_info.is_enabled = FALSE;
   module_capi_container->module_info.is_in_place = TRUE;
   module_capi_container->module_info.port_info.num_input_ports = 1;
   module_capi_container->module_info.port_info.num_output_ports = 1;
   module_capi_container->module_info.port_mem = NULL;


   result = audio_module_chain_wrapper_allocate_ports(&module_capi_container->module_info);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_init: Failed to allocate memory for ports for module 0x%lX-0x%lX.",
          module_capi_container->module_info.module_id, module_capi_container->module_info.instance_id);
      module_capi_container->module_info.forced_disabled = TRUE;
      return result;
   }

   // Set up init properties
   capi_v2_heap_id_t heap_id;
   heap_id.heap_id = QURT_ELITE_HEAP_DEFAULT;

   capi_v2_event_callback_info_t cb_info = { audio_module_chain_wrapper_event_callback,  &module_capi_container->module_info};
   capi_v2_prop_t init_props[] = {
         { CAPI_V2_PORT_NUM_INFO,            { reinterpret_cast<int8_t*>(&module_capi_container->module_info.port_info),   sizeof(module_capi_container->module_info.port_info), sizeof(module_capi_container->module_info.port_info) }, { FALSE, FALSE, 0} },
         { CAPI_V2_HEAP_ID,             { reinterpret_cast<int8_t*>(&heap_id),    sizeof(heap_id),  sizeof(heap_id)  }, { FALSE, FALSE, 0} },
         { CAPI_V2_EVENT_CALLBACK_INFO,  { reinterpret_cast<int8_t*>(&cb_info),     sizeof(cb_info),   sizeof(cb_info)   }, { FALSE, FALSE, 0} }
   };
   capi_v2_proplist_t init_proplist = { SIZE_OF_ARRAY(init_props), init_props };

   // Create the module
   if(TRUE == is_amdb_module_handle_valid(module_capi_container->handle_info_ptr))
   {
      ADSPResult result = ADSP_EOK;
      if(module_capi_container->handle_info_ptr->interface_type == CAPI_V2)
      {
         result = audio_module_chain_wrapper_create_capi_v2_module(module_capi_container, stack_size_ptr, &init_proplist);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "audio_module_chain_wrapper_init: Module ID 0x%lX type is not CAPIv2. Type is %d.", module_capi_container->module_info.module_id, module_capi_container->handle_info_ptr->interface_type);
         module_capi_container->module_info.forced_disabled = TRUE;
         result = ADSP_EFAILED;
      }

      if (ADSP_FAILED(result))
      {
         module_capi_container->module_info.forced_disabled = TRUE;
         return result;
      }
   }
   else
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "audio_module_chain_wrapper_init: Module ID 0x%lX is not supported.", module_capi_container->module_info.module_id);
       module_capi_container->module_info.forced_disabled = TRUE;
       return ADSP_EUNSUPPORTED;
   }

   return result;
}

static void audio_module_chain_wrapper_free(module_CAPI_container_t *module_capi_container)
{
    audio_module_chain_wrapper_free_ports(&module_capi_container->module_info);

    if(module_capi_container->module_info.module_ptr != NULL)
    {
		module_capi_container->module_info.module_ptr->vtbl_ptr->end(module_capi_container->module_info.module_ptr);
        qurt_elite_memory_free(module_capi_container->module_info.module_ptr);
        module_capi_container->module_info.module_ptr = NULL;
    }
    return;
}

void audio_module_chain_wrapper_destroy(AudioModuleCapiWrapper_t *modules)
{
   if(NULL == modules)
   {
	   return;
   }

   if(NULL  == modules->module_capi_container)
   {
      modules->num_modules = 0;
      return;
   }

   for(uint32_t i = 0; i < modules->num_modules; i++)
   {
      audio_module_chain_wrapper_free(&modules->module_capi_container[i]);
   }

   audio_module_chain_wrapper_destroy_stream_buf(modules);

   if(modules->module_capi_container != NULL)
   {
      qurt_elite_memory_free(modules->module_capi_container);
      modules->module_capi_container = NULL;
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audio_module_chain_wrapper_destroy end");
   modules->num_modules = 0;

   return;
}

ADSPResult audio_module_chain_wrapper_create(AudioModuleCapiWrapper_t *modules, uint32_t *modules_stack_size, uint32_t num_modules, avcs_module_instance_info_t *module_instance_info_array)
{
  ADSPResult result = ADSP_EOK;
  adsp_amdb_module_handle_info_t *handle_info_ptr = NULL;

  if(NULL == modules)
  {
	  return ADSP_ENOMEMORY;
  }

  memset(modules, 0, sizeof(AudioModuleCapiWrapper_t));
  if((NULL == module_instance_info_array) || (0 == num_modules))
  {
      return ADSP_EBADPARAM;
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audio_module_chain_wrapper_create : module_instance_info_array is %d and num modules %d", (int)module_instance_info_array, (int)num_modules);
  modules->num_modules = num_modules;

  modules->module_capi_container = (module_CAPI_container_t *)qurt_elite_memory_malloc(num_modules * sizeof(module_CAPI_container_t), QURT_ELITE_HEAP_DEFAULT);
  if(NULL  == modules->module_capi_container)
  {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_create : malloc fails while creating memory for module container");
      audio_module_chain_wrapper_destroy(modules);
      return ADSP_ENOMEMORY;
  }

  memset(modules->module_capi_container, 0, num_modules * sizeof(module_CAPI_container_t));

  handle_info_ptr = (adsp_amdb_module_handle_info_t *)qurt_elite_memory_malloc(num_modules * sizeof(adsp_amdb_module_handle_info_t), QURT_ELITE_HEAP_DEFAULT);
  if(NULL  == handle_info_ptr)
  {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_module_chain_wrapper_create : No memory for creating handle info pointer for modules");
      audio_module_chain_wrapper_destroy(modules);
      return ADSP_ENOMEMORY;
  }

  for(uint32_t i = 0; i < num_modules; i++)
  {
      modules->module_capi_container[i].handle_info_ptr = handle_info_ptr + i;

      modules->module_capi_container[i].handle_info_ptr->interface_type = CAPI_V2;
      modules->module_capi_container[i].handle_info_ptr->type = AMDB_MODULE_TYPE_GENERIC;
      modules->module_capi_container[i].handle_info_ptr->id1 = module_instance_info_array[i].module_id;
      modules->module_capi_container[i].handle_info_ptr->id2 = 0;
      modules->module_capi_container[i].handle_info_ptr->h.capi_v2_handle = NULL;
      modules->module_capi_container[i].handle_info_ptr->result = ADSP_EFAILED;
  }

  /*
   * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
   * happens using a thread pool using threads of very low priority. This can cause the current thread
   * to be blocked because of a low priority thread. If this is not desired, a callback function
   * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
   * thread can then handle other tasks in parallel.
   */
  adsp_amdb_get_modules_request(handle_info_ptr, num_modules, NULL, NULL);

  for(uint32_t i = 0; i < num_modules; i++)
  {
     if(modules->module_capi_container[i].handle_info_ptr->interface_type == STUB)
     {
         modules->module_capi_container[i].module_info.module_id = module_instance_info_array[i].module_id;
         modules->module_capi_container[i].module_info.instance_id = module_instance_info_array[i].instance_id;
         modules->module_capi_container[i].module_info.forced_disabled = TRUE;
         continue;
     }
     audio_module_chain_wrapper_init(&modules->module_capi_container[i], modules_stack_size, &module_instance_info_array[i]);
  }

  // TODO : check if its ok to release this here
  if(handle_info_ptr!=NULL)
  {
     adsp_amdb_release_handles(handle_info_ptr, num_modules);
     qurt_elite_memory_free(handle_info_ptr);
  }

  return result;
}

static uint32_t calculate_q_factor(uint32_t bits_per_sample)
{
    switch(bits_per_sample)
    {
       case 16:
          return PCM_16BIT_Q_FORMAT;
       case 24:
          return ELITE_32BIT_PCM_Q_FORMAT;
       case 32:
          return PCM_32BIT_Q_FORMAT;
       default:
           MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio module capi wrapper: invalid bits per sample %lu", bits_per_sample);
           return CAPI_V2_DATA_FORMAT_INVALID_VAL;
    }
}

void audio_module_chain_wrapper_set_media_fmt(AudioModuleCapiWrapper_t *modules, elite_msg_any_t *msg, uint32_t out_buf_num_unit_frames, uint32_t bitstream_format)
{
    capi_v2_err_t err_code = CAPI_V2_EOK;

    if(NULL == modules)
    {
		return;
	}

    elite_msg_data_media_type_apr_t *pCmdMsgPayload = (elite_msg_data_media_type_apr_t*) (msg->pPayload);
    elite_multi_channel_pcm_fmt_blk_t *pPcmFormatBlock =
          (elite_multi_channel_pcm_fmt_blk_t*) ((uint8_t*)(pCmdMsgPayload)+sizeof(elite_msg_data_media_type_apr_t));

    for(uint32_t i = 0; i < modules->num_modules; i++)
    {
        module_info_t *pModule_info = &modules->module_capi_container[i].module_info;
        if(pModule_info->forced_disabled == TRUE)
        {
			continue;
		}

        for(uint32_t port_id = 0; port_id < pModule_info->port_info.num_input_ports; port_id++)
        {
            module_port_t *input_ports_ptr = &pModule_info->input_ports_ptr[port_id];
            module_port_t *output_ports_ptr = &pModule_info->output_ports_ptr[port_id];

            input_ports_ptr->media_event.event.f.bits_per_sample = (pPcmFormatBlock->bits_per_sample == 16) ? 16 : 32;
            input_ports_ptr->media_event.event.f.bitstream_format = bitstream_format;

            if(pPcmFormatBlock->is_interleaved == AUD_FORMAT_DEINTERLEAVED)
            {
               input_ports_ptr->media_event.event.f.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
            }
            else
            {
               input_ports_ptr->media_event.event.f.data_interleaving = CAPI_V2_INTERLEAVED;
            }

            input_ports_ptr->media_event.event.f.data_is_signed = pPcmFormatBlock->is_signed;
            input_ports_ptr->media_event.event.f.num_channels = pPcmFormatBlock->num_channels;
            input_ports_ptr->media_event.event.f.q_factor = calculate_q_factor(pPcmFormatBlock->bits_per_sample);
            input_ports_ptr->media_event.event.f.sampling_rate = pPcmFormatBlock->sample_rate;
            input_ports_ptr->media_event.event.h.data_format = CAPI_V2_FIXED_POINT;
            for(uint32_t ch = 0; ch < input_ports_ptr->media_event.event.f.num_channels; ch++)
            {
				input_ports_ptr->media_event.event.f.channel_type[ch] = (uint16_t)pPcmFormatBlock->channel_mapping[ch];
			}

            capi_v2_prop_t prop;

            MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audio module capi wrapper: in_port %lu: Setting input media format for Module-Instance with ID 0x%lX-0x%lX, sampling rate : (%lu), num channels: (%lu)",
                    port_id, pModule_info->module_id, pModule_info->instance_id, input_ports_ptr->media_event.event.f.sampling_rate, input_ports_ptr->media_event.event.f.num_channels);

            aud_module_media_fmt_t *new_format_ptr = &input_ports_ptr->media_event.event;

            prop.id = CAPI_V2_INPUT_MEDIA_FORMAT;
            prop.port_info.is_input_port = TRUE;
            prop.port_info.is_valid = TRUE;
            prop.port_info.port_index = port_id;
            prop.payload.actual_data_len = sizeof(*new_format_ptr);
            prop.payload.max_data_len = sizeof(*new_format_ptr);
            prop.payload.data_ptr = reinterpret_cast<int8_t*>(new_format_ptr);

            input_ports_ptr->media_event.event_is_pending = FALSE;

            capi_v2_proplist_t proplist = { 1, &prop };

            CAPI_V2_SET_ERROR(err_code, pModule_info->module_ptr->vtbl_ptr->set_properties(pModule_info->module_ptr, &proplist));
            if (CAPI_V2_FAILED(err_code))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audio module capi wrapper: Module-Instance with ID 0x%lX-0x%lX failed to accept the current media type. Disabling it.",
                   pModule_info->module_id, pModule_info->instance_id);
               pModule_info->forced_disabled = TRUE;
            }

            // assuming output and input media formats to be same
            output_ports_ptr->media_event.event = input_ports_ptr->media_event.event;
        }
    }


    if(modules->num_modules > 0)
    {
       if(ADSP_FAILED(audio_module_chain_wrapper_stream_buf_init(modules, out_buf_num_unit_frames)))
       {
           // destroy ULL modules memory
           audio_module_chain_wrapper_destroy(modules);
       }
       // kpps BW aggregation should be called by caller service after this function
       //else
       //{
       //   AudioDecSvc_ProcessKppsBw(pMe, FALSE, TRUE);
	   //}
    }
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audio module capi wrapper: Media format propagation and stream buf init done");

    return;
}

static void audio_module_chain_wrapper_destroy_stream_buf(AudioModuleCapiWrapper_t *modules)
{
   if(modules->pStream_buf_mem != NULL)
   {
       qurt_elite_memory_free(modules->pStream_buf_mem);
       modules->pStream_buf_mem = NULL;
   }

   return;
}

static uint32_t get_pcm_buf_size(uint32_t num_unit_frames, uint32_t sample_rate, uint16_t bytes_per_sample, uint16_t num_ch)
{
   //Calculate size based on the current Format Sample Rate
   uint32_t unAfeFrameSizeInSamples;
   elite_svc_get_frame_size(sample_rate,&unAfeFrameSizeInSamples);
   uint32_t unOutBufSizeBytes = bytes_per_sample * num_ch * (unAfeFrameSizeInSamples) * num_unit_frames;
   return unOutBufSizeBytes;
}


static ADSPResult audio_module_chain_wrapper_stream_buf_init(AudioModuleCapiWrapper_t *modules, uint32_t out_buf_num_unit_frames)
{

  uint32_t in_buf_size = 0, out_buf_size = 0, total_size = 0, unOutBufSizeBytes = 0;
  uint16_t sampleRate, bytesPerSample, numChannel;
  int8_t *mem;
  module_info_t *pModule_info;
  uint32_t num_output_ports = 0;

  for(uint32_t i = 0; i < modules->num_modules; i++)
  {
      pModule_info = &modules->module_capi_container[i].module_info;

      if(pModule_info->forced_disabled)
      {
          continue;
      }
      for(uint32_t port_id = 0; port_id < pModule_info->port_info.num_input_ports; port_id++)
      {
          in_buf_size += sizeof(capi_v2_buf_t) * pModule_info->input_ports_ptr[port_id].media_event.event.f.num_channels;
      }
      for(uint32_t port_id = 0; port_id < pModule_info->port_info.num_output_ports; port_id++)
      {
          out_buf_size += sizeof(capi_v2_buf_t) * pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels;
      }
  }

  total_size = in_buf_size + out_buf_size;

  for(uint32_t i = 0; i < modules->num_modules; i++)
  {
     pModule_info = &modules->module_capi_container[i].module_info;
     if(!pModule_info->forced_disabled)  // TODO : alloc if one of the module is not inplace
     {
        sampleRate = pModule_info->output_ports_ptr[0].media_event.event.f.sampling_rate;
        bytesPerSample = pModule_info->output_ports_ptr[0].media_event.event.f.bits_per_sample >> 3;
        numChannel = pModule_info->output_ports_ptr[0].media_event.event.f.num_channels;

        unOutBufSizeBytes = get_pcm_buf_size(out_buf_num_unit_frames, sampleRate, bytesPerSample, numChannel);
        num_output_ports = pModule_info->port_info.num_output_ports;
        break;
     }
  }

  modules->OutBufSize = unOutBufSizeBytes;
  total_size += unOutBufSizeBytes * num_output_ports;

  if(modules->stream_buf_size != total_size)
  {
      audio_module_chain_wrapper_destroy_stream_buf(modules);
      modules->stream_buf_size = total_size;

      if(total_size > 0)
      {
         modules->pStream_buf_mem = (int8_t *)qurt_elite_memory_malloc(total_size,QURT_ELITE_HEAP_DEFAULT);
         if(NULL == modules->pStream_buf_mem)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Audio module capi wrapper: Out of memory allocating stream buf");
            return ADSP_ENOMEMORY;
         }
      }
  }

  mem = modules->pStream_buf_mem;

  for(uint32_t i = 0; i < modules->num_modules; i++)
  {
      pModule_info = &modules->module_capi_container[i].module_info;

      if(pModule_info->forced_disabled)
      {
          continue;
      }

      for(uint32_t port_id = 0; port_id < pModule_info->port_info.num_input_ports; port_id++)
      {
          pModule_info->stream.in_buf[port_id] = (capi_v2_buf_t *)mem;
          mem += sizeof(capi_v2_buf_t) * pModule_info->input_ports_ptr[port_id].media_event.event.f.num_channels;

          for(uint32_t ch = 0; ch < pModule_info->input_ports_ptr[port_id].media_event.event.f.num_channels; ch ++)
          {
              pModule_info->stream.in_buf[port_id][ch].actual_data_len = 0;
              pModule_info->stream.in_buf[port_id][ch].max_data_len = 0;
              pModule_info->stream.in_buf[port_id][ch].data_ptr = NULL;
          }
      }

      for(uint32_t port_id = 0; port_id < pModule_info->port_info.num_output_ports; port_id++)
      {
          pModule_info->stream.out_buf[port_id] = (capi_v2_buf_t *)mem;
          mem += sizeof(capi_v2_buf_t) * pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels;

          for(uint32_t ch = 0; ch < pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels; ch ++)
          {
              pModule_info->stream.out_buf[port_id][ch].actual_data_len = 0;
              pModule_info->stream.out_buf[port_id][ch].max_data_len = 0;
              pModule_info->stream.out_buf[port_id][ch].data_ptr = NULL;
          }
      }
  }

  for(uint32_t port_id = 0; port_id < num_output_ports; port_id++)
  {
      modules->OutBuf[port_id] = mem;
      mem += unOutBufSizeBytes;
  }

  for(uint32_t i = 0; i < modules->num_modules; i++)
  {
      module_info_t *pModule_info = &modules->module_capi_container[i].module_info;

      if(pModule_info->forced_disabled)
      {
          continue;
      }

      for(uint32_t port_id = 0; port_id < pModule_info->port_info.num_input_ports; port_id++)
      {
          pModule_info->stream.inputs[port_id].buf_ptr = pModule_info->stream.in_buf[port_id];
          pModule_info->stream.inputs[port_id].bufs_num = pModule_info->input_ports_ptr[port_id].media_event.event.f.num_channels;
          memset(&pModule_info->stream.inputs[port_id].flags, 0, sizeof(pModule_info->stream.inputs[port_id].flags));
          pModule_info->stream.inputs[port_id].timestamp = 0;

          pModule_info->stream.inputs_ptr[port_id] = &pModule_info->stream.inputs[port_id];
      }

      for(uint32_t port_id = 0; port_id < pModule_info->port_info.num_output_ports; port_id++)
      {
          pModule_info->stream.outputs[port_id].buf_ptr = pModule_info->stream.out_buf[port_id];
          pModule_info->stream.outputs[port_id].bufs_num = pModule_info->output_ports_ptr[port_id].media_event.event.f.num_channels;
          memset(&pModule_info->stream.outputs[port_id].flags, 0, sizeof(pModule_info->stream.outputs[port_id].flags));
          pModule_info->stream.outputs[port_id].timestamp = 0;

          pModule_info->stream.outputs_ptr[port_id] = &pModule_info->stream.outputs[port_id];
      }
  }

  return ADSP_EOK;
}

int32_t audio_module_chain_wrapper_get_kpps(AudioModuleCapiWrapper_t *modules)
{
    int32_t kpps = 0;

    if(modules != NULL)
    {
       for(uint32_t i = 0; i < modules->num_modules; i++)
       {
           if(!modules->module_capi_container[i].module_info.forced_disabled)
           {
              kpps += modules->module_capi_container[i].module_info.KPPS;
           }
       }
    }
    return kpps;
}

int32_t audio_module_chain_wrapper_get_bw(AudioModuleCapiWrapper_t *modules)
{
    int32_t bw = 0;

    if(modules != NULL)
    {
       for(uint32_t i = 0; i < modules->num_modules; i++)
       {
           if(!modules->module_capi_container[i].module_info.forced_disabled)
           {
              bw += modules->module_capi_container[i].module_info.bandwidth;
           }
       }
    }
    return bw;
}