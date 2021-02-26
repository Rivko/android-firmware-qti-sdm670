/** @file AudioDecSvc_CapiV2CallbackHandler.cpp
This file contains functions for Elite Decoder service.

Copyright (c) 2014-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_CapiV2CallbackHandler.cpp#2 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
02/05/14    rbhatnk      Created file.
02/07/18    npendeka      Modified
========================================================================== */

#include "AudioDecSvc_CapiV2CallbackHandler.h"
#include "AudioDecSvc_Util.h"
#include "EliteMsg_Custom.h"
#include "AudioDecSvc_Structs.h"
#include "AudioDecSvc_Util.h"
#include "capi_v2_library_factory.h"
#include "AudioDecSvc_CapiV2Util.h"
#include "AudioStreamMgr.h"
#include "AudioStreamMgr_AprIf.h"

static capi_v2_err_t audio_dec_svc_raise_IEC61937_media_fmt_event(AudioDecSvc_t *pMe, uint32_t new_output_fmt_id, uint32_t old_output_fmt_id, uint32_t capi_index)
{
   capi_v2_err_t result = CAPI_V2_EOK;

   //Check if media format update event is registered
   aud_dec_svc_event_query_payload_t evt_query;
   evt_query.event_id = ASM_STREAM_IEC_61937_MEDIA_FMT_EVENT_ID;

   aud_dec_svc_event_node_t *node = aud_dec_svc_find_node(&evt_query, &pMe->event_list);

   if (NULL == node)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, " CAPI %lu: event %lx not registered ", capi_index, evt_query.event_id);
      CAPI_V2_SET_ERROR(result, CAPI_V2_EFAILED);
      return result;
   }

   asm_iec_61937_media_fmt_event_t iec61937_evnt;
   iec61937_evnt.new_format_id = new_output_fmt_id;

   if(new_output_fmt_id != old_output_fmt_id)
   {
      MSG_3( MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu: media fmt has changed from %lx to %lx ",
            capi_index, old_output_fmt_id, new_output_fmt_id );

      result |= AudioStreamMgr_GenerateClientEventFromCb((void*)&node->payload.cb,
            ASM_STREAM_IEC_61937_MEDIA_FMT_EVENT, node->payload.cb.unAsynClientToken, &iec61937_evnt, sizeof(iec61937_evnt) );
   }

   return result;

}

static capi_v2_err_t audio_dec_svc_capi_v2_callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
   capi_v2_err_t result = CAPI_V2_EOK;

   capi_v2_buf_t *payload = &event_info_ptr->payload;

   dec_CAPI_callback_obj_t *cb_obj = reinterpret_cast<dec_CAPI_callback_obj_t*>(context_ptr);
   AudioDecSvc_OutStream_t *pOutStrm = NULL;

   AudioDecSvc_t *pMe = cb_obj->pMe;
   if (!( (cb_obj->capi_index < DEC_SVC_MAX_CAPI) && (NULL != pMe->capiContainer[cb_obj->capi_index])))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in callback function. Data possibly corrupted.");
      return CAPI_V2_EFAILED;
   }

   dec_CAPI_container_t *capi_container = pMe->capiContainer[cb_obj->capi_index];
   if ((capi_container == AudioDecSvc_GetLastCapi(pMe))	 && (cb_obj->capi_index < DEC_SVC_MAX_OUTPUT_STREAMS))
   {
      pOutStrm = pMe->out_streams_ptr[cb_obj->capi_index];
   }

   if (payload->actual_data_len > payload->max_data_len)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is greater than the max size %lu for id %lu.",
            payload->actual_data_len, payload->max_data_len, static_cast<uint32_t>(id));
      return CAPI_V2_EBADPARAM;
   }

   switch(id)
   {
   case CAPI_V2_EVENT_KPPS:
   {
      if (payload->actual_data_len < sizeof(capi_v2_event_KPPS_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size %zu for id %lu.",
               payload->actual_data_len, sizeof(capi_v2_event_process_state_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }

      capi_v2_event_KPPS_t *kpps = reinterpret_cast<capi_v2_event_KPPS_t*>(payload->data_ptr);

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu KPPS  %lu", cb_obj->capi_index, static_cast<uint32_t>(kpps->KPPS));

      if (capi_container->kpps != kpps->KPPS)
      {
         capi_container->kpps = kpps->KPPS;
         pMe->event_mask = (pMe->event_mask | AUD_DEC_SVC_EVENT__KPPS_MASK);
      }

      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_BANDWIDTH:
   {
      if (payload->actual_data_len < sizeof(capi_v2_event_bandwidth_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size %zu for id %lu.",
               payload->actual_data_len, sizeof(capi_v2_event_process_state_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }

      capi_v2_event_bandwidth_t *bw = reinterpret_cast<capi_v2_event_bandwidth_t*>(payload->data_ptr);
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu Module BW (code,data) %lu %lu.", cb_obj->capi_index, bw->code_bandwidth, bw->data_bandwidth);

      if (capi_container->code_bw != bw->code_bandwidth)
      {
         capi_container->code_bw = bw->code_bandwidth;
         pMe->event_mask = (pMe->event_mask | AUD_DEC_SVC_EVENT__BW_MASK);
      }
      if (capi_container->data_bw != bw->data_bandwidth)
      {
         capi_container->data_bw = bw->data_bandwidth;
         pMe->event_mask = (pMe->event_mask | AUD_DEC_SVC_EVENT__BW_MASK);
      }
      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_PROCESS_STATE:
   {
      if (payload->actual_data_len < sizeof(capi_v2_event_process_state_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size %zu for id %lu.",
               payload->actual_data_len, sizeof(capi_v2_event_process_state_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }
      capi_v2_event_process_state_t *pData = (capi_v2_event_process_state_t*)payload->data_ptr;
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu process check set to %lu", cb_obj->capi_index, static_cast<uint32_t>(pData->is_enabled));

      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
   {
      if (payload->actual_data_len < sizeof(capi_v2_set_get_media_format_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size %zu for id %lu.",
               payload->actual_data_len, sizeof(capi_v2_set_get_media_format_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }

      capi_v2_set_get_media_format_t *pData = reinterpret_cast<capi_v2_set_get_media_format_t*>(payload->data_ptr);
      dec_CAPI_container_t *capi_cont = pMe->capiContainer[cb_obj->capi_index];
      dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
      uint32_t new_output_fmt_id = CAPI_V2_DATA_FORMAT_INVALID_VAL;
      uint32_t old_output_fmt_id = capi_cont->dec_fmt_id;

      switch(pData->format_header.data_format)
      {
      case CAPI_V2_FIXED_POINT:
      case CAPI_V2_IEC61937_PACKETIZED:
      case CAPI_V2_DSD_DOP_PACKETIZED:
      case CAPI_V2_GENERIC_COMPRESSED:
      {
         if (payload->actual_data_len < (sizeof(capi_v2_set_get_media_format_t)+sizeof(capi_v2_standard_data_format_t)))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size for id %lu.",
                  payload->actual_data_len, static_cast<uint32_t>(id));
            return CAPI_V2_ENEEDMORE;
         }

         if (!event_info_ptr->port_info.is_valid || event_info_ptr->port_info.is_input_port)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. port is not valid or is an input port.");
            return CAPI_V2_EBADPARAM;
         }


         elite_multi_channel_pcm_fmt_blk_t *prev_fmt = &(capi_cont->PrevFmtBlk[event_info_ptr->port_info.port_index]);
         elite_multi_channel_pcm_fmt_blk_t prev_fmt_cpy = capi_cont->PrevFmtBlk[event_info_ptr->port_info.port_index];

         capi_v2_standard_data_format_t *pPcmData = (capi_v2_standard_data_format_t*) (payload->data_ptr+sizeof(capi_v2_set_get_media_format_t));

         //no need to raise event for bps change because the decoder always outputs at the bps needed by the PP (set during asm open)
         prev_fmt->bits_per_sample = pPcmData->bits_per_sample;
         if(pPcmData->bits_per_sample > 16)
         {
            prev_fmt->bits_per_sample = (pPcmData->q_factor == ELITE_32BIT_PCM_Q_FORMAT)? 24 : 32;
         }

         if(last_capi)
         {
            last_capi->out_data_fmt_type[0] = pData->format_header.data_format;
         }

         //For generic compressed pass-through, since there are no options to set bits per sample at stream open, have to set it here if needed
         if (CAPI_V2_GENERIC_COMPRESSED == pData->format_header.data_format)
         {
            if (NULL != pOutStrm && pOutStrm->output_bits_per_sample != pPcmData->bits_per_sample)
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc: updating output media format for generic compressed pass-through");
               pOutStrm->output_bits_per_sample = pPcmData->bits_per_sample;
               pOutStrm->output_bytes_per_sample = (pPcmData->bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
            }
         }

         if (prev_fmt->num_channels != pPcmData->num_channels)
         {
            prev_fmt->num_channels = pPcmData->num_channels;
            capi_cont->media_fmt_event[event_info_ptr->port_info.port_index] = true;
         }
         if (prev_fmt->sample_rate != pPcmData->sampling_rate)
         {
            prev_fmt->sample_rate = pPcmData->sampling_rate;
            capi_cont->media_fmt_event[event_info_ptr->port_info.port_index] = true;
         }

         for (uint32_t i=0;((i<sizeof(prev_fmt->channel_mapping)) && (i<pPcmData->num_channels)); i++)
         {
            if (prev_fmt->channel_mapping[i] != pPcmData->channel_type[i])
            {
               prev_fmt->channel_mapping[i] = pPcmData->channel_type[i];
               capi_cont->media_fmt_event[event_info_ptr->port_info.port_index] = true;
            }
         }
         if (pData->format_header.data_format == CAPI_V2_FIXED_POINT)
         {
            if (pPcmData->data_interleaving != CAPI_V2_DEINTERLEAVED_PACKED)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Unsupported data interleaving %u for fixed point data ", pPcmData->data_interleaving);
            }
         }

         //only for 61937 packetized o/p do this. for 'auto' capis o/p fmt is PCM. which is not right thing to set for dec_fmt_id.
         //ideally we need o/p fmt_id in addition to dec_fmt_id.
         if (CAPI_V2_IEC61937_PACKETIZED == pData->format_header.data_format)
         {
            //in 61937 passthru cases, the client may open stream with fmt-id = 0.
            //passthru capi will determine fmt and pass it down as media fmt.
            if (CAPI_V2_DATA_FORMAT_INVALID_VAL != pPcmData->bitstream_format)
            {
               if ( capi_cont->dec_fmt_id != pPcmData->bitstream_format )
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI Type %u changed bit stream format id from 0x%lx to 0x%lx", capi_cont->capi_type, capi_cont->dec_fmt_id, pPcmData->bitstream_format);
                  capi_cont->media_fmt_event[event_info_ptr->port_info.port_index] = true;
                  capi_cont->dec_fmt_id = pPcmData->bitstream_format;
               }
            }
         }

         //for last CAPI, update the total bytes
         if ( (last_capi == capi_cont) &&
               (capi_cont->media_fmt_event[event_info_ptr->port_info.port_index]) &&
               (prev_fmt_cpy.num_channels != 0) &&
               (prev_fmt_cpy.sample_rate != 0)
         )
         {
            AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_GetDefaultOutputStream(pMe);
            if (pOutStrm->common.total_bytes != 0)
            {
               pOutStrm->common.total_bytes = pOutStrm->common.total_bytes *
                     prev_fmt->num_channels / prev_fmt_cpy.num_channels *
                     prev_fmt->sample_rate / prev_fmt_cpy.sample_rate; //bytes per sample is always pOutStrm->output_bytes_per_sample
            }
         }

         break;
      }
      case CAPI_V2_RAW_COMPRESSED:
      {
         if (payload->actual_data_len < (sizeof(capi_v2_set_get_media_format_t)+sizeof(capi_v2_raw_compressed_data_format_t)))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size for id %lu.",
                  payload->actual_data_len, static_cast<uint32_t>(id));
            return CAPI_V2_ENEEDMORE;
         }

         if (!event_info_ptr->port_info.is_valid || event_info_ptr->port_info.is_input_port)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. port is not valid or is an input port.");
            return CAPI_V2_EBADPARAM;
         }

         capi_v2_raw_compressed_data_format_t *pRawData = (capi_v2_raw_compressed_data_format_t*) (payload->data_ptr+sizeof(capi_v2_set_get_media_format_t));

         //RAW Compressed media format will be raised by depacketizer module only for dec svc.
         new_output_fmt_id = pRawData->bitstream_format;
         capi_cont->dec_fmt_id = new_output_fmt_id;

         break;
      }
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. Changing media type not supported for format %d",
               pData->format_header.data_format);
         return CAPI_V2_EUNSUPPORTED;
      }

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, " CAPI %lu output format updated.", cb_obj->capi_index);

      //if bit-stream format is valid, then raise event
      if( CAPI_V2_DATA_FORMAT_INVALID_VAL != new_output_fmt_id )
      {
         result |= audio_dec_svc_raise_IEC61937_media_fmt_event(pMe, new_output_fmt_id, old_output_fmt_id, cb_obj->capi_index);
      }

      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE:
   {
      if (payload->actual_data_len < sizeof(capi_v2_port_data_threshold_change_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size %zu for id %lu.", payload->actual_data_len, sizeof(capi_v2_port_data_threshold_change_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }

      if (!event_info_ptr->port_info.is_valid )
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. port is not valid ");
         return CAPI_V2_EBADPARAM;
      }

      capi_v2_port_data_threshold_change_t *pData = reinterpret_cast<capi_v2_port_data_threshold_change_t*>(payload->data_ptr);
      if (event_info_ptr->port_info.is_input_port)
      {
         if (event_info_ptr->port_info.port_index >= DEC_SVC_MAX_INPUT_STREAMS)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " CAPI %lu: Input port %lu more than supported value", cb_obj->capi_index, event_info_ptr->port_info.port_index);
            return CAPI_V2_EFAILED;
         }
         capi_container->in_port_event_new_size[event_info_ptr->port_info.port_index] = pData->new_threshold_in_bytes;
         pMe->event_mask |= AUD_DEC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK;

         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu: Input port %lu event with new buf len = %lu.", cb_obj->capi_index, event_info_ptr->port_info.port_index, pData->new_threshold_in_bytes);
      }
      else
      {
         if (event_info_ptr->port_info.port_index >= DEC_SVC_MAX_OUTPUT_STREAMS)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " CAPI %lu: Output port %lu more than supported value", cb_obj->capi_index, event_info_ptr->port_info.port_index);
            return CAPI_V2_EFAILED;
         }

         capi_container->out_port_event_new_size[event_info_ptr->port_info.port_index] = pData->new_threshold_in_bytes;
         pMe->event_mask |= AUD_DEC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK;

         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu: Output port %lu event with new buf len = %lu.", cb_obj->capi_index,  event_info_ptr->port_info.port_index, pData->new_threshold_in_bytes);
      }
      MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, " CAPI %lu port data threshold update.", cb_obj->capi_index);

      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_METADATA_AVAILABLE:
   {
      if (!event_info_ptr->port_info.is_valid || event_info_ptr->port_info.is_input_port)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. port is not valid or is an input port.");
         return CAPI_V2_EBADPARAM;
      }

      if (event_info_ptr->port_info.port_index >= DEC_SVC_MAX_OUTPUT_STREAMS)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Output port %lu more than supported value", event_info_ptr->port_info.port_index);
         return CAPI_V2_EFAILED;
      }

      capi_container->metadata_available_event[event_info_ptr->port_info.port_index] = true;
      pMe->event_mask |= AUD_DEC_SVC_EVENT__METADATA_AVAILABLE_MASK;

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu: Output port %lu metadata_available event", cb_obj->capi_index,  event_info_ptr->port_info.port_index);
      break;
   }
   case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
   {
      if (payload->actual_data_len < sizeof(capi_v2_event_data_to_dsp_service_t))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size %lu for id %lu.", payload->actual_data_len, sizeof(capi_v2_event_data_to_dsp_service_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }
      capi_v2_event_data_to_dsp_service_t *dsp_event = reinterpret_cast<capi_v2_event_data_to_dsp_service_t*>(payload->data_ptr);
      switch(dsp_event->param_id)
      {
      case FWK_EXTN_EVENT_ID_COMPR_OVER_PCM_COP_FRAME_LENGTH:
      {
         if (dsp_event->payload.actual_data_len < sizeof(fwk_extn_event_id_compr_over_pcm_cop_frame_length_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size %lu for id %lu.",
                  payload->actual_data_len, sizeof(fwk_extn_event_id_compr_over_pcm_cop_frame_length_t), static_cast<uint32_t>(dsp_event->param_id));
            return CAPI_V2_ENEEDMORE;
         }
         fwk_extn_event_id_compr_over_pcm_cop_frame_length_t *pData = reinterpret_cast<fwk_extn_event_id_compr_over_pcm_cop_frame_length_t*>(dsp_event->payload.data_ptr);
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu: COP frame length event: is nominal frame %lu, COP frame length %lu", cb_obj->capi_index, pData->is_nominal_frame, pData->cop_frame_length);

         break;
      }
      case FWK_EXTN_EVENT_ID_COMPR_OVER_PCM_BIT_RATE_LEVEL:
      {
    	 if (dsp_event->payload.actual_data_len < sizeof(fwk_extn_event_id_compr_over_pcm_bit_rate_level_t))
    	 {
    	    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size %lu for id %lu.",
    	          payload->actual_data_len, sizeof(fwk_extn_event_id_compr_over_pcm_bit_rate_level_t), static_cast<uint32_t>(dsp_event->param_id));
    	    return CAPI_V2_ENEEDMORE;
    	 }
    	 fwk_extn_event_id_compr_over_pcm_bit_rate_level_t *pData = reinterpret_cast<fwk_extn_event_id_compr_over_pcm_bit_rate_level_t*>(dsp_event->payload.data_ptr);
    	 MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu: COP bit rate level event: COP bit rate level %lu", cb_obj->capi_index, pData->bit_rate_level);

    	 //send IMC message to encoder
    	 AudioDecSvc_IMCSendBuf(pMe, pData);
    	 break;
      }
      default:
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " CAPI %lu: Error in callback function. ID 0x%lx not supported.", cb_obj->capi_index, (dsp_event->param_id));
         return CAPI_V2_EUNSUPPORTED;
      }
      }
      break;
   }
   case CAPI_V2_EVENT_GET_LIBRARY_INSTANCE:
   {
      if (event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_get_library_instance_t))
      {
         CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
         break;
      }

      capi_v2_event_get_library_instance_t *payload_ptr = reinterpret_cast<capi_v2_event_get_library_instance_t*>(event_info_ptr->payload.data_ptr);

      result = capi_v2_library_factory_get_instance(payload_ptr->id, &payload_ptr->ptr);

      if (result != CAPI_V2_EOK)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get an instance of the library to get capi v2 modules.");
      }

      break;
   }
   case CAPI_V2_EVENT_GET_DLINFO:
   {
      if (event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_dlinfo_t))
      {
         CAPI_V2_SET_ERROR(result, CAPI_V2_ENEEDMORE);
         break;
      }

      capi_v2_event_dlinfo_t *payload_ptr = reinterpret_cast<capi_v2_event_dlinfo_t*>(event_info_ptr->payload.data_ptr);

      uint32_t *start_addr = NULL;
      bool_t is_dl = FALSE;
      audio_dec_svc_query_dl_info(capi_container->amdb_handle, &is_dl, &start_addr, &payload_ptr->load_size);
      uint64_t phy_start_addr = qurt_elite_memorymap_get_physical_addr((uint32_t)start_addr);
      payload_ptr->is_dl = is_dl;
      payload_ptr->load_addr_lsw = (uint32_t)phy_start_addr;
      payload_ptr->load_addr_msw = (uint32_t)(phy_start_addr>>32);

      break;
   }
   case CAPI_V2_EVENT_DATA_TO_DSP_CLIENT:
   {
      if (event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_data_to_dsp_client_t))
      {
         CAPI_V2_SET_ERROR(result, CAPI_V2_ENEEDMORE);
         break;
      }

      capi_v2_event_data_to_dsp_client_t *payload_ptr = reinterpret_cast<capi_v2_event_data_to_dsp_client_t*>(event_info_ptr->payload.data_ptr);
      aud_dec_svc_event_query_payload_t evt_query;
      evt_query.event_id = payload_ptr->param_id;

      aud_dec_svc_event_node_t *node = aud_dec_svc_find_node(&evt_query, &pMe->event_list);

      if (NULL == node)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu: event %lu not registered ", cb_obj->capi_index, evt_query.event_id);
         CAPI_V2_SET_ERROR(result, CAPI_V2_EFAILED);
         break;
      }

      //TODO: can we assume it's ASM, what if AFE?

      uint32_t sz = sizeof(asm_stream_encdec_event_t)+payload_ptr->payload.actual_data_len;
      void *ptr = qurt_elite_memory_malloc(sz, QURT_ELITE_HEAP_DEFAULT);
      if (ptr == NULL)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu: event %lu no memory to allocate %lu ", cb_obj->capi_index, evt_query.event_id, sz);
         break;
      }
      asm_stream_encdec_event_t *evt_ptr = (asm_stream_encdec_event_t*) ptr;
      evt_ptr->event_id = evt_query.event_id;
      evt_ptr->payload_size = payload_ptr->payload.actual_data_len;
      evt_ptr->reserved = 0;
      evt_ptr->service_id = ASM_DECODER_SVC;
      char *byte_ptr = (char*) ptr;
      byte_ptr += sizeof(asm_stream_encdec_event_t);
      memscpy(byte_ptr, sz-sizeof(asm_stream_encdec_event_t), payload_ptr->payload.data_ptr, payload_ptr->payload.actual_data_len);

      result |= AudioStreamMgr_GenerateClientEventFromCb((void*)&node->payload.cb, ASM_STREAM_ENCDEC_EVENT, node->payload.cb.unAsynClientToken, ptr, sz);

      qurt_elite_memory_free(ptr);

      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " CAPI %lu: event %lu sent to client with error code %lu", cb_obj->capi_index, evt_query.event_id, result);

      break;
   }
   default:
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " CAPI %lu: Error in callback function. ID %lu not supported.", cb_obj->capi_index, static_cast<uint32_t>(id));
      return CAPI_V2_EUNSUPPORTED;
   }
   }
   return result;
}

capi_v2_event_callback_info_t audio_dec_svc_get_capi_v2_callback_handler(AudioDecSvc_t *pMe, uint32_t capi_index)
{
   capi_v2_event_callback_info_t cb_info;
   cb_info.event_cb = audio_dec_svc_capi_v2_callback;

   pMe->capiContainer[capi_index]->capi_callback_obj.capi_index = capi_index;
   pMe->capiContainer[capi_index]->capi_callback_obj.pMe = pMe;

   cb_info.event_context = (void*)&pMe->capiContainer[capi_index]->capi_callback_obj;

   return cb_info;
}

