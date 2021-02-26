/** @file AudioEncSvc_CapiV2CallbackHandler.cpp
This file contains functions for Elite Encoder service.

Copyright (c) 2014-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_enc_svc/src/AudioEncSvc_CapiV2CallbackHandler.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
07/22/14    rbhatnk      Created file.

========================================================================== */

#include "AudioEncSvc_CapiV2CallbackHandler.h"
#include "EliteMsg_Custom.h"
#include "AudioEncSvc_Includes.h"
#include "AudioEncSvc_CapiV2Util.h"

static capi_v2_err_t audio_enc_svc_capi_v2_callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
   capi_v2_err_t result = CAPI_V2_EOK;

   capi_v2_buf_t *payload = &event_info_ptr->payload;

   enc_CAPI_callback_obj_t *cb_obj = reinterpret_cast<enc_CAPI_callback_obj_t*>(context_ptr);

   AudioEncSvc_t *pMe = cb_obj->pMe;
   if (!( (cb_obj->capi_index < ENC_SVC_MAX_CAPI) && (NULL != pMe->capiContainer[cb_obj->capi_index])))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in callback function. Data possibly corrupted.");
      return CAPI_V2_EFAILED;
   }

   enc_CAPI_container_t *capi_container = pMe->capiContainer[cb_obj->capi_index];

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

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Module KPPS  %lu", static_cast<uint32_t>(kpps->KPPS));

      if (capi_container->kpps != kpps->KPPS)
      {
         capi_container->kpps = kpps->KPPS;
         pMe->event_mask = (pMe->event_mask | AUD_ENC_SVC_EVENT__KPPS_MASK);
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
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Module BW (code,data) %lu %lu.", bw->code_bandwidth, bw->data_bandwidth);

      if (capi_container->code_bw != bw->code_bandwidth)
      {
         capi_container->code_bw = bw->code_bandwidth;
         pMe->event_mask = (pMe->event_mask | AUD_ENC_SVC_EVENT__BW_MASK);
      }
      if (capi_container->data_bw != bw->data_bandwidth)
      {
         capi_container->data_bw = bw->data_bandwidth;
         pMe->event_mask = (pMe->event_mask | AUD_ENC_SVC_EVENT__BW_MASK);
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
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Module process check set to %lu", static_cast<uint32_t>(pData->is_enabled));

      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
   {
      if (payload->actual_data_len < sizeof(capi_v2_set_get_media_format_t))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function for capi index %lu. The actual size %lu is less than the required size %zu for id %lu.",
               cb_obj->capi_index, payload->actual_data_len, sizeof(capi_v2_set_get_media_format_t), static_cast<uint32_t>(id));
         return CAPI_V2_ENEEDMORE;
      }

      capi_v2_set_get_media_format_t *pData = reinterpret_cast<capi_v2_set_get_media_format_t*>(payload->data_ptr);
      bool_t changed = false;

      enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);
      enc_CAPI_container_t *capi_cont = pMe->capiContainer[cb_obj->capi_index];

      if (capi_cont->out_med_fmt.main.format_header.data_format != pData->format_header.data_format)
      {
         capi_cont->out_med_fmt.main.format_header.data_format = pData->format_header.data_format;
         changed = true;
      }

      switch(pData->format_header.data_format)
      {
      case CAPI_V2_FIXED_POINT:
      case CAPI_V2_IEC61937_PACKETIZED:
      case CAPI_V2_COMPR_OVER_PCM_PACKETIZED:
      {
         if (payload->actual_data_len < (sizeof(capi_v2_set_get_media_format_t)+sizeof(capi_v2_standard_data_format_t)))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. The actual size %lu is less than the required size for id %lu.",
                  payload->actual_data_len, static_cast<uint32_t>(id));
            return CAPI_V2_ENEEDMORE;
         }

         if (!event_info_ptr->port_info.is_valid || event_info_ptr->port_info.is_input_port || (event_info_ptr->port_info.port_index > 0))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. port is not valid or is an input port or not zero.");
            return CAPI_V2_EBADPARAM;
         }

         capi_v2_standard_data_format_t *pPcmData = (capi_v2_standard_data_format_t*) (payload->data_ptr+sizeof(capi_v2_set_get_media_format_t));

         if (capi_cont->out_med_fmt.std.bits_per_sample != pPcmData->bits_per_sample)
         {
            capi_cont->out_med_fmt.std.bits_per_sample = pPcmData->bits_per_sample;
            changed=true;
         }

         if (capi_cont->out_med_fmt.std.num_channels != pPcmData->num_channels)
         {
            capi_cont->out_med_fmt.std.num_channels = pPcmData->num_channels;
            changed=true;
         }

         if (capi_cont->out_med_fmt.std.sampling_rate != pPcmData->sampling_rate)
         {
            capi_cont->out_med_fmt.std.sampling_rate = pPcmData->sampling_rate;
            changed=true;
         }

         for (uint32_t i=0;((i < (sizeof(capi_cont->out_med_fmt.std.channel_type)) / (sizeof(capi_cont->out_med_fmt.std.channel_type[0])) ) && (i < pPcmData->num_channels)); i++)
         {
            if (capi_cont->out_med_fmt.std.channel_type[i] != pPcmData->channel_type[i])
            {
               capi_cont->out_med_fmt.std.channel_type[i] = pPcmData->channel_type[i];
               changed=true;
            }
         }

         if (last_capi == capi_cont) //service can only handle CAPI_V2_DEINTERLEAVED_PACKED
         {
            if (pData->format_header.data_format == CAPI_V2_FIXED_POINT)
            {
               if (pPcmData->data_interleaving != CAPI_V2_DEINTERLEAVED_PACKED)
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Unsupported data interleaving %u for fixed point data ", pPcmData->data_interleaving);
                  return CAPI_V2_EBADPARAM;
               }
            }
         }

         if (capi_cont->out_med_fmt.std.data_interleaving != pPcmData->data_interleaving)
         {
            capi_cont->out_med_fmt.std.data_interleaving = pPcmData->data_interleaving;
            changed=true;
         }

         if (capi_cont->out_med_fmt.std.q_factor != pPcmData->q_factor)
         {
            capi_cont->out_med_fmt.std.q_factor = pPcmData->q_factor;
            changed=true;
         }

         if (capi_cont->out_med_fmt.std.data_is_signed != pPcmData->data_is_signed)
         {
            capi_cont->out_med_fmt.std.data_is_signed = pPcmData->data_is_signed;
            changed=true;
         }

         break;
      }
      case CAPI_V2_RAW_COMPRESSED:
      {
         capi_v2_raw_compressed_data_format_t *pRawData = (capi_v2_raw_compressed_data_format_t*) (payload->data_ptr+sizeof(capi_v2_set_get_media_format_t));

         if ((0 != capi_cont->enc_cfg_id) && (capi_cont->enc_cfg_id != pRawData->bitstream_format))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " CAPI is not outputing the same bit stream format as it was opened for");
         }

         //depacketizer and encoder
         if (capi_cont->out_med_fmt.raw_fmt.bitstream_format != pRawData->bitstream_format)
         {
            capi_cont->out_med_fmt.raw_fmt.bitstream_format = pRawData->bitstream_format;
            changed=true;
         }
         break;
      }
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. Changing media type not supported for format %d",
               pData->format_header.data_format);
         return CAPI_V2_EUNSUPPORTED;
      }

      if (changed)
      {
         capi_cont->media_fmt_event = true;
      }

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Module output format updated for capi_index %lu. changed? = %u", cb_obj->capi_index, changed);

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
         if (event_info_ptr->port_info.port_index > 0)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Input port %lu more than supported value", event_info_ptr->port_info.port_index);
            return CAPI_V2_EFAILED;
         }
         capi_container->in_port_thresh = pData->new_threshold_in_bytes;
         pMe->event_mask |= AUD_ENC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK;

         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " CAPI index %lu Input port %lu event with new buf len = %lu.", cb_obj->capi_index, event_info_ptr->port_info.port_index, pData->new_threshold_in_bytes);
      }
      else
      {
         if (event_info_ptr->port_info.port_index > 0)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Output port %lu more than supported value", event_info_ptr->port_info.port_index);
            return CAPI_V2_EFAILED;
         }

         capi_container->out_port_thresh = pData->new_threshold_in_bytes;
         pMe->event_mask |= AUD_ENC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK;

         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " CAPI index %lu  Output port %lu event with new buf len = %lu.", cb_obj->capi_index, event_info_ptr->port_info.port_index, pData->new_threshold_in_bytes);
      }

      return CAPI_V2_EOK;
   }
   case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
   {
      break;
   }
   case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
   {
      break;
   }
   default:
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error in callback function. ID %lu not supported.", static_cast<uint32_t>(id));
      return CAPI_V2_EUNSUPPORTED;
   }
   }
   return result;
}

capi_v2_event_callback_info_t audio_enc_svc_get_capi_v2_callback_handler(AudioEncSvc_t *pMe, uint32_t capi_index)
{
   capi_v2_event_callback_info_t cb_info;
   cb_info.event_cb = audio_enc_svc_capi_v2_callback;

   pMe->capiContainer[capi_index]->capi_callback_obj.capi_index = capi_index;
   pMe->capiContainer[capi_index]->capi_callback_obj.pMe = pMe;

   cb_info.event_context = (void*)&pMe->capiContainer[capi_index]->capi_callback_obj;

   return cb_info;
}

