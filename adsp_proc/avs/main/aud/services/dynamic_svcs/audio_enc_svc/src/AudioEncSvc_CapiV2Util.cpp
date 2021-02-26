/** @file AudioEncSvc_CapiV2Util.cpp
This file contains functions for Elite Encoder service.

Copyright (c) 2014-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_enc_svc/src/AudioEncSvc_CapiV2Util.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
07/22/14    rbhatnk      Created file.

========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "AudioEncSvc_CapiV2Util.h"
#include "adsp_media_fmt.h"
#include "adsp_asm_stream_commands.h"

#include "capi_v2_adsp_error_code_converter.h"
#include "adsp_amdb_static.h"

typedef struct capi_v2_list_t capi_v2_list_t;

typedef bool_t (*blob_parse_fn_t)(const capi_v2_list_t *list_row, AudioEncSvcInitParams_t *pInitParams);

/**
 * since enc_cfg_id cannot identify a CAPI by itself, we a blob
 * which can anything basically. The way blob is parsed is defined by
 * blob_parse_fn
 */
typedef struct capi_v2_list_t
{
   uint32_t enc_cfg_id;
   const void* blob_ptr;            //blob , struct of which depends on enc_cfg_id
   blob_parse_fn_t blob_parse_fn;   //function that parses above blob
   capi_v2_get_static_properties_f get_static_prop_fn;
   capi_v2_init_f init_fn;
   uint32_t heap_id;
} capi_v2_list_t;


static void init_media_fmt(capi_v2_standard_data_format_t *std_media_fmt)
{
   std_media_fmt->bits_per_sample = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->bitstream_format = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->data_interleaving = CAPI_V2_INVALID_INTERLEAVING;
   std_media_fmt->data_is_signed = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->num_channels = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->q_factor = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->sampling_rate = CAPI_V2_DATA_FORMAT_INVALID_VAL;

   for (uint32_t j=0; (j<CAPI_V2_MAX_CHANNELS); j++)
   {
      std_media_fmt->channel_type[j] = (uint16_t)CAPI_V2_DATA_FORMAT_INVALID_VAL;
   }
}

#if 0
static ADSPResult allocate_memory_for_capi_v2_from_list(capi_v2_t **capi_v2_ptr_ptr, const capi_v2_list_t *capi_v2_list, capi_v2_proplist_t *init_proplist_ptr)
{
   capi_v2_err_t err_code = CAPI_V2_EOK;
   *capi_v2_ptr_ptr = NULL;

   //Allocate memory for the wrapper
   capi_v2_proplist_t props_list;
   capi_v2_prop_t prop[1];

   capi_v2_init_memory_requirement_t init_mem;
   uint32_t i=0;
   prop[i].id = CAPI_V2_INIT_MEMORY_REQUIREMENT;
   prop[i].payload.actual_data_len = 0;
   prop[i].payload.max_data_len = sizeof(capi_v2_init_memory_requirement_t);
   prop[i].payload.data_ptr = (int8_t*)&init_mem;
   prop[i].port_info.is_valid = false;
   i++;

   props_list.prop_ptr = prop;
   props_list.props_num = i;

   err_code = capi_v2_list->get_static_prop_fn(init_proplist_ptr, &props_list); //as per CAPI V2, init_proplist_ptr passed during init must also be passed during get_static_prop
   if (CAPI_V2_FAILED(err_code))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: Get Static Properties error");
      return capi_v2_err_to_adsp_result(err_code);
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: Get Static Properties done with init_mem_req=%lu", init_mem.size_in_bytes);

   capi_v2_t *capi_v2_ptr = (capi_v2_t*)qurt_elite_memory_malloc(init_mem.size_in_bytes, QURT_ELITE_HEAP_DEFAULT);
   if (!capi_v2_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc:Insufficient memory to allocate to CapiV2 Module.It requires %lu bytes",init_mem.size_in_bytes);
      return ADSP_ENOMEMORY;
   }

   memset(capi_v2_ptr, 0, init_mem.size_in_bytes);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc: Module allocated for %lu bytes of memory at location 0x%p.",
         init_mem.size_in_bytes, capi_v2_ptr);

   *capi_v2_ptr_ptr = capi_v2_ptr;

   return capi_v2_err_to_adsp_result(err_code);
}
#endif

static ADSPResult create_capi_v2_from_amdb(capi_v2_t **capi_v2_ptr_ptr, adsp_amdb_capi_v2_t *amdb_handle, capi_v2_proplist_t *init_proplist_ptr)
{
   capi_v2_err_t err_code = CAPI_V2_EOK;
   *capi_v2_ptr_ptr = NULL;

   capi_v2_proplist_t props_list;
   capi_v2_prop_t prop[1];

   capi_v2_init_memory_requirement_t init_mem;
   uint32_t i=0;
   prop[i].id = CAPI_V2_INIT_MEMORY_REQUIREMENT;
   prop[i].payload.actual_data_len = 0;
   prop[i].payload.max_data_len = sizeof(capi_v2_init_memory_requirement_t);
   prop[i].payload.data_ptr = (int8_t*)&init_mem;
   prop[i].port_info.is_valid = false;
   i++;

   props_list.prop_ptr = prop;
   props_list.props_num = i;

   err_code = adsp_amdb_capi_v2_get_static_properties_f(amdb_handle, init_proplist_ptr, &props_list); //as per CAPI V2, init_proplist_ptr passed during init must also be passed during get_static_prop
   if (CAPI_V2_FAILED(err_code))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: Get Static Properties error");
      return capi_v2_err_to_adsp_result(err_code);
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: Get Static Properties done with init_mem_req=%lu", init_mem.size_in_bytes);

   capi_v2_t *capi_v2_ptr = (capi_v2_t*)qurt_elite_memory_malloc(init_mem.size_in_bytes, QURT_ELITE_HEAP_DEFAULT);
   if (!capi_v2_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc: Insufficient memory to allocate to CapiV2 Module.It requires %lu bytes",init_mem.size_in_bytes);
      return ADSP_ENOMEMORY;
   }

   memset(capi_v2_ptr, 0, init_mem.size_in_bytes);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc: Module allocated for %lu bytes of memory at location 0x%p.",
         init_mem.size_in_bytes, capi_v2_ptr);

   *capi_v2_ptr_ptr = capi_v2_ptr;

   err_code = adsp_amdb_capi_v2_init_f(amdb_handle, *capi_v2_ptr_ptr, init_proplist_ptr);
   if (CAPI_V2_FAILED(err_code))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: Init error");
      qurt_elite_memory_free(*capi_v2_ptr_ptr);
      *capi_v2_ptr_ptr = NULL;
      return capi_v2_err_to_adsp_result(err_code);
   }

   return ADSP_EOK;
}

static ADSPResult audio_enc_svc_destroy_capi_v2(capi_v2_t **capi_v2_ptr_ptr)
{
   if (NULL == *capi_v2_ptr_ptr) return ADSP_EFAILED;

   qurt_elite_memory_free (*capi_v2_ptr_ptr);

   *capi_v2_ptr_ptr = NULL;

   return ADSP_EOK;
}

#define NUM_PARAMS_AUTO_CAPI 3
ADSPResult audio_enc_svc_create_init_auto_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioEncSvcInitParams_t *pInitParams, enc_CAPI_init_params_t *pCapiInitParams)
{
   ADSPResult result = ADSP_EOK;
   pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_UNKNOWN;

   capi_v2_proplist_t init_proplist;
   capi_v2_event_callback_info_t cb_info = pCapiInitParams->cb_info;
   capi_v2_prop_t props[NUM_PARAMS_AUTO_CAPI];
   audio_enc_svc_capi_v2_media_fmt_t media_fmt;
   capi_v2_heap_id_t heap_id;

   props[0].id = CAPI_V2_EVENT_CALLBACK_INFO;
   props[0].payload.actual_data_len = props[0].payload.max_data_len = sizeof(cb_info);
   props[0].payload.data_ptr = reinterpret_cast<int8_t*>(&cb_info);
   props[0].port_info.is_valid = false;

   media_fmt.main.format_header.data_format = CAPI_V2_FIXED_POINT;
   init_media_fmt(&media_fmt.std);
   media_fmt.std.bits_per_sample = pInitParams->bits_per_sample;
   props[1].id = CAPI_V2_INPUT_MEDIA_FORMAT; //TODO: in port or out port?
   props[1].payload.actual_data_len = props[1].payload.max_data_len = sizeof(media_fmt);
   props[1].payload.data_ptr = reinterpret_cast<int8_t*>(&media_fmt);
   props[1].port_info.is_valid = false;//TODO: which port index?

   heap_id.heap_id = QURT_ELITE_HEAP_DEFAULT;
   props[2].id = CAPI_V2_HEAP_ID;
   props[2].payload.actual_data_len = props[2].payload.max_data_len = sizeof(capi_v2_heap_id_t);
   props[2].payload.data_ptr = reinterpret_cast<int8_t*>(&heap_id);
   props[2].port_info.is_valid = false;

   init_proplist.props_num = NUM_PARAMS_AUTO_CAPI;
   init_proplist.prop_ptr = props;

   {
      // Check in amdb first
      adsp_amdb_capi_v2_t *amdb_handle = (adsp_amdb_capi_v2_t*)pCapiInitParams->amdb_handle;
      if (amdb_handle)
      {
         result = create_capi_v2_from_amdb(capi_v2_ptr_ptr, amdb_handle, &init_proplist);

         // Release the handle since we no longer need it.
         adsp_amdb_module_handle_info_t module_handle_info;
         module_handle_info.interface_type = CAPI_V2;
         module_handle_info.type = 0; // Ignored
         module_handle_info.id1 = 0; // Ignored
         module_handle_info.id2 = 0; // Ignored
         module_handle_info.h.capi_v2_handle = amdb_handle;
         module_handle_info.result = ADSP_EOK;
         adsp_amdb_release_handles(&module_handle_info, 1);

         if (ADSP_SUCCEEDED(result))
         {
            pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_PRESENT;
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc: Couldn't find CAPI V2 for format 0x%lx",
               pCapiInitParams->enc_cfg_id);
         pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_NOT_PRESENT;
         return ADSP_EFAILED;
      }
   }

   if (ADSP_FAILED(result))
   {
      return result;
   }

   pCapiInitParams->enc_destroy_fn = audio_enc_svc_destroy_capi_v2;

   return result;
}

#define NUM_PROPS_NON_AUTO 3
ADSPResult audio_enc_svc_create_init_depack_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioEncSvcInitParams_t *pInitParams, enc_CAPI_init_params_t *pCapiInitParams)
{
   ADSPResult result = ADSP_EOK;
   pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_UNKNOWN;

   capi_v2_proplist_t init_proplist;
   capi_v2_event_callback_info_t cb_info = pCapiInitParams->cb_info;
   capi_v2_prop_t props[NUM_PROPS_NON_AUTO];
   capi_v2_heap_id_t heap_id;

   props[0].id = CAPI_V2_EVENT_CALLBACK_INFO;
   props[0].payload.actual_data_len = props[0].payload.max_data_len = sizeof(cb_info);
   props[0].payload.data_ptr = reinterpret_cast<int8_t*>(&cb_info);
   props[0].port_info.is_valid = false;

   heap_id.heap_id = QURT_ELITE_HEAP_DEFAULT;
   props[1].id = CAPI_V2_HEAP_ID;
   props[1].payload.actual_data_len = props[1].payload.max_data_len = sizeof(capi_v2_heap_id_t);
   props[1].payload.data_ptr = reinterpret_cast<int8_t*>(&heap_id);
   props[1].port_info.is_valid = false;

   //bits per sample need not be set on pass through capi

   init_proplist.props_num = NUM_PROPS_NON_AUTO;
   init_proplist.prop_ptr = props;

   {
      // Check in amdb first
      adsp_amdb_capi_v2_t *amdb_handle = (adsp_amdb_capi_v2_t*)pCapiInitParams->amdb_handle;
      if (amdb_handle)
      {
         result = create_capi_v2_from_amdb(capi_v2_ptr_ptr, amdb_handle, &init_proplist);

         // Release the handle since we no longer need it.
         adsp_amdb_module_handle_info_t module_handle_info;
         module_handle_info.interface_type = CAPI_V2;
         module_handle_info.type = 0; // Ignored
         module_handle_info.id1 = AVS_MODULE_ID_DEPACKETIZER_IEC61937;
         module_handle_info.id2 = 0; // Ignored
         module_handle_info.h.capi_v2_handle = amdb_handle;
         module_handle_info.result = ADSP_EOK;
         adsp_amdb_release_handles(&module_handle_info, 1);

         if (ADSP_SUCCEEDED(result))
   {
         pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_PRESENT;
      }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc: Couldn't find CAPI V2 for format 0x%lx",
               pCapiInitParams->enc_cfg_id);
         pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_NOT_PRESENT;
         return ADSP_EFAILED;
      }
   }

   if (ADSP_FAILED(result))
   {
   return result;
   }

   pCapiInitParams->enc_destroy_fn = audio_enc_svc_destroy_capi_v2;

   return result;
}

#define NUM_PROPS_COP_PACK 3
ADSPResult audio_enc_svc_create_init_cop_pack_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioEncSvcInitParams_t *pInitParams, enc_CAPI_init_params_t *pCapiInitParams)
{
   ADSPResult result = ADSP_EOK;

   capi_v2_proplist_t init_proplist;
   capi_v2_event_callback_info_t cb_info = pCapiInitParams->cb_info;
   audio_enc_svc_capi_v2_media_fmt_t media_fmt;
   capi_v2_prop_t props[NUM_PROPS_COP_PACK];
   capi_v2_heap_id_t heap_id;

   pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_UNKNOWN;

   props[0].id = CAPI_V2_EVENT_CALLBACK_INFO;
   props[0].payload.actual_data_len = props[0].payload.max_data_len = sizeof(cb_info);
   props[0].payload.data_ptr = reinterpret_cast<int8_t*>(&cb_info);
   props[0].port_info.is_valid = false;

   media_fmt.main.format_header.data_format = CAPI_V2_COMPR_OVER_PCM_PACKETIZED;
   init_media_fmt(&media_fmt.std);

   media_fmt.std.bits_per_sample = (pInitParams->out_bits_per_sample != 0) ? pInitParams->out_bits_per_sample : CAPI_V2_DATA_FORMAT_INVALID_VAL;
   if (pInitParams->out_num_channels !=  0)
   {
      media_fmt.std.num_channels = pInitParams->out_num_channels;
      memscpy(pInitParams->out_channel_type, sizeof(pInitParams->out_channel_type), media_fmt.std.channel_type, sizeof(media_fmt.std.channel_type));
   }
   media_fmt.std.sampling_rate = (pInitParams->out_sampling_rate != 0) ? pInitParams->out_sampling_rate : CAPI_V2_DATA_FORMAT_INVALID_VAL;

   props[1].id = CAPI_V2_OUTPUT_MEDIA_FORMAT;
   props[1].payload.actual_data_len = props[1].payload.max_data_len = sizeof(media_fmt);
   props[1].payload.data_ptr = reinterpret_cast<int8_t*>(&media_fmt);
   props[1].port_info.is_valid = false;//TODO: which port index?

   heap_id.heap_id = QURT_ELITE_HEAP_DEFAULT;
   props[2].id = CAPI_V2_HEAP_ID;
   props[2].payload.actual_data_len = props[1].payload.max_data_len = sizeof(capi_v2_heap_id_t);
   props[2].payload.data_ptr = reinterpret_cast<int8_t*>(&heap_id);
   props[2].port_info.is_valid = false;

   //bits per sample need not be set on packetizer capi

   init_proplist.props_num = NUM_PROPS_COP_PACK;
   init_proplist.prop_ptr = props;

   {
      // Check in amdb first
      adsp_amdb_capi_v2_t *amdb_handle = (adsp_amdb_capi_v2_t*)pCapiInitParams->amdb_handle;
      if (amdb_handle)
      {
         result = create_capi_v2_from_amdb(capi_v2_ptr_ptr, amdb_handle, &init_proplist);

         // Release the handle since we no longer need it.
         adsp_amdb_module_handle_info_t module_handle_info;
         module_handle_info.interface_type = CAPI_V2;
         module_handle_info.type = 0; // Ignored
         module_handle_info.id1 = 0; // Ignored
         module_handle_info.id2 = 0; // Ignored
         module_handle_info.h.capi_v2_handle = amdb_handle;
         module_handle_info.result = ADSP_EOK;
         adsp_amdb_release_handles(&module_handle_info, 1);

         if (ADSP_SUCCEEDED(result))
         {
            pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_PRESENT;
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc: Couldn't find CAPI V2 for format 0x%lx",
               pCapiInitParams->enc_cfg_id);
         pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_NOT_PRESENT;
         return ADSP_EFAILED;
      }
   }

   if (ADSP_FAILED(result))
   {
      return result;
   }

   pCapiInitParams->enc_destroy_fn = audio_enc_svc_destroy_capi_v2;

   return result;
}



ADSPResult audio_enc_svc_create_init_pack_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioEncSvcInitParams_t *pInitParams, enc_CAPI_init_params_t *pCapiInitParams)
{
   ADSPResult result = ADSP_EOK;
   pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_UNKNOWN;

   capi_v2_proplist_t init_proplist;
   capi_v2_event_callback_info_t cb_info = pCapiInitParams->cb_info;
   capi_v2_prop_t props[NUM_PROPS_NON_AUTO];
   capi_v2_heap_id_t heap_id;

   //Callback Info
   props[0].id = CAPI_V2_EVENT_CALLBACK_INFO;
   props[0].payload.actual_data_len = props[0].payload.max_data_len = sizeof(cb_info);
   props[0].payload.data_ptr = reinterpret_cast<int8_t*>(&cb_info);
   props[0].port_info.is_valid = false;

   //Heap-id
   heap_id.heap_id = QURT_ELITE_HEAP_DEFAULT;
   props[1].id = CAPI_V2_HEAP_ID;
   props[1].payload.actual_data_len = props[1].payload.max_data_len = sizeof(capi_v2_heap_id_t);
   props[1].payload.data_ptr = reinterpret_cast<int8_t*>(&heap_id);
   props[1].port_info.is_valid = false;

   //Input media fmt
   audio_enc_svc_capi_v2_media_fmt_t input_fmt;
   input_fmt.main.format_header.data_format = CAPI_V2_RAW_COMPRESSED;
   input_fmt.raw_fmt.bitstream_format = pCapiInitParams->enc_cfg_id;

   props[2].id = CAPI_V2_INPUT_MEDIA_FORMAT;
   props[2].payload.actual_data_len = props[2].payload.max_data_len = sizeof(audio_enc_svc_capi_v2_media_fmt_t);
   props[2].payload.data_ptr = reinterpret_cast<int8_t*>(&input_fmt);
   props[2].port_info.is_valid = true;
   props[2].port_info.is_input_port = true;
   props[2].port_info.port_index = 0;

   //bits per sample need not be set on pass through capi

   init_proplist.props_num = NUM_PROPS_NON_AUTO;
   init_proplist.prop_ptr = props;

   {
      // Check in amdb first
      adsp_amdb_capi_v2_t *amdb_handle = (adsp_amdb_capi_v2_t*)pCapiInitParams->amdb_handle;
      if (amdb_handle)
      {
         result = create_capi_v2_from_amdb(capi_v2_ptr_ptr, amdb_handle, &init_proplist);

         // Release the handle since we no longer need it.
         adsp_amdb_module_handle_info_t module_handle_info;
         module_handle_info.interface_type = CAPI_V2;
         module_handle_info.type = 0; // Ignored
         module_handle_info.id1 = 0; // Ignored
         module_handle_info.id2 = 0; // Ignored
         module_handle_info.h.capi_v2_handle = amdb_handle;
         module_handle_info.result = ADSP_EOK;
         adsp_amdb_release_handles(&module_handle_info, 1);

         if (ADSP_SUCCEEDED(result))
         {
            pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_PRESENT;
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc: Couldn't find CAPI V2 for format 0x%lx",
               pCapiInitParams->enc_cfg_id);
         pCapiInitParams->capi_v2_presence = ENC_CAPI_V2_PRESENCE_NOT_PRESENT;
         return ADSP_EFAILED;
      }
   }

   if (ADSP_FAILED(result))
   {
      return result;
   }

   pCapiInitParams->enc_destroy_fn = audio_enc_svc_destroy_capi_v2;

   return ADSP_EOK;
}

#define DEFAULT_BUF_SIZE 1024

ADSPResult audio_enc_svc_get_init_time_properties(capi_v2_t *capi_v2_ptr, enc_CAPI_init_time_get_params_t *init_time_get_params, enc_CAPI_container_t *capi_container)
{
   capi_v2_err_t err_code = CAPI_V2_EOK;

   capi_v2_proplist_t props_list;
   capi_v2_prop_t prop[1];

   capi_v2_port_data_threshold_t in_port_thresh;
   capi_v2_port_data_threshold_t out_port_thresh;
   capi_v2_stack_size_t stack_size;
   capi_v2_num_needed_framework_extensions_t num_needed_fmwk_ext;
   capi_v2_requires_data_buffering_t require_data_buf;

   memset(&in_port_thresh, 0, sizeof(capi_v2_port_data_threshold_t));
   memset(&out_port_thresh, 0, sizeof(capi_v2_port_data_threshold_t));
   memset(&stack_size, 0, sizeof(capi_v2_stack_size_t));
   memset(&num_needed_fmwk_ext, 0, sizeof(capi_v2_num_needed_framework_extensions_t));
   memset(&require_data_buf, 0, sizeof(require_data_buf));

   uint32_t i=0;

   i = 0;
   prop[i].id = CAPI_V2_STACK_SIZE;
   prop[i].payload.actual_data_len = 0;
   prop[i].payload.max_data_len = sizeof(capi_v2_stack_size_t);
   prop[i].payload.data_ptr = (int8_t*)&stack_size;
   prop[i].port_info.is_valid = false;
   i++;

   props_list.prop_ptr = prop;
   props_list.props_num = i;

   err_code = capi_v2_ptr->vtbl_ptr->get_properties(capi_v2_ptr, &props_list);
   if (CAPI_V2_FAILED(err_code))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: Get Init Properties error. stack size mandatory");
      return capi_v2_err_to_adsp_result(err_code);
   }

   init_time_get_params->stack_size = stack_size;

   i = 0;
   prop[i].id = CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS;
   prop[i].payload.actual_data_len = 0;
   prop[i].payload.max_data_len = sizeof(capi_v2_num_needed_framework_extensions_t);
   prop[i].payload.data_ptr = (int8_t*)&num_needed_fmwk_ext;
   prop[i].port_info.is_valid = false;
   i++;

   props_list.prop_ptr = prop;
   props_list.props_num = i;

   (void) capi_v2_ptr->vtbl_ptr->get_properties(capi_v2_ptr, &props_list);
   //ignore any error since params are options.

   capi_container->num_fmwk_extns.num_extensions = num_needed_fmwk_ext.num_extensions;

   if (num_needed_fmwk_ext.num_extensions != 0)
   {
      capi_v2_framework_extension_id_t *fmwk_xtn_ids = (capi_v2_framework_extension_id_t*)
                        qurt_elite_memory_malloc(sizeof(capi_v2_framework_extension_id_t)*num_needed_fmwk_ext.num_extensions, QURT_ELITE_HEAP_DEFAULT);
      if (NULL == fmwk_xtn_ids)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"error alloc mem for fmwk extension");
         return ADSP_ENOMEMORY;
      }

      i = 0;
      prop[i].id = CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS;
      prop[i].payload.actual_data_len = 0;
      prop[i].payload.max_data_len = sizeof(capi_v2_framework_extension_id_t)*ENC_SVC_MAX_SUPPORTED_FMWK_EXTNS;
      prop[i].payload.data_ptr = (int8_t*)fmwk_xtn_ids;
      prop[i].port_info.is_valid = false;
      i++;

      props_list.prop_ptr = prop;
      props_list.props_num = i;

      err_code = capi_v2_ptr->vtbl_ptr->get_properties(capi_v2_ptr, &props_list);
      if (CAPI_V2_FAILED(err_code))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Get Static Properties error");
         qurt_elite_memory_free(fmwk_xtn_ids);
         return err_code;
      }

      bool_t any_extn_not_supported = false;
      for (i = 0; i < num_needed_fmwk_ext.num_extensions; i++)
      {
         bool_t extn_supported = false;
         for (uint32_t j = 0; j < ENC_SVC_MAX_SUPPORTED_FMWK_EXTNS; j++)
         {
            if(enc_svc_supported_fmwk_extns[j].id == fmwk_xtn_ids[i].id)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"fmwk extensions needed & supported 0x%lx", fmwk_xtn_ids[i].id);
               extn_supported = true;
               memscpy(&capi_container->fmwk_extns[j], sizeof(capi_container->fmwk_extns[j]), &(fmwk_xtn_ids[i]), sizeof(fmwk_xtn_ids[i]));
               break;
            }
         }
         if (!extn_supported)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"fmwk extensions not supported are 0x%lx. Module may not work correctly", fmwk_xtn_ids[i].id);
            any_extn_not_supported = true;
         }
      }
      qurt_elite_memory_free(fmwk_xtn_ids);

      if (any_extn_not_supported)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failing since the fmwk doesnot support some extensions");
         return ADSP_EFAILED;
      }
   }

   i = 0;
   prop[i].id = CAPI_V2_REQUIRES_DATA_BUFFERING;
   prop[i].payload.actual_data_len = 0;
   prop[i].payload.max_data_len = sizeof(capi_v2_requires_data_buffering_t);
   prop[i].payload.data_ptr = (int8_t*)&require_data_buf;
   prop[i].port_info.is_valid = false;
   i++;

   props_list.prop_ptr = prop;
   props_list.props_num = i;

   err_code = capi_v2_ptr->vtbl_ptr->get_properties(capi_v2_ptr, &props_list);
   if (CAPI_V2_FAILED(err_code))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: Get Init Properties error. Assuming capi requires data buffering");
      require_data_buf.requires_data_buffering = TRUE;
   }

   if (require_data_buf.requires_data_buffering)
   {
      if (init_time_get_params->input_port_index.valid)
      {
         i = 0;
         prop[i].id = CAPI_V2_PORT_DATA_THRESHOLD;
         prop[i].payload.actual_data_len = 0;
         prop[i].payload.max_data_len = sizeof(capi_v2_port_data_threshold_t);
         prop[i].port_info.is_valid = true;
         prop[i].port_info.is_input_port = true;
         prop[i].port_info.port_index = init_time_get_params->input_port_index.index;
         prop[i].payload.data_ptr = (int8_t*)&in_port_thresh;
         i++;

         props_list.prop_ptr = prop;
         props_list.props_num = i;

         err_code = capi_v2_ptr->vtbl_ptr->get_properties(capi_v2_ptr, &props_list);
         if (CAPI_V2_FAILED(err_code))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: Get Init Properties error. Assuming %u for input port thresh.",DEFAULT_BUF_SIZE);
            in_port_thresh.threshold_in_bytes = DEFAULT_BUF_SIZE;
         }
         //when 1 is returned, it means capi can work with any size.
         if (in_port_thresh.threshold_in_bytes == 1)
         {
            in_port_thresh.threshold_in_bytes = DEFAULT_BUF_SIZE;
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: CAPI returned input thresh 1 byte, changing it to %u", DEFAULT_BUF_SIZE);
         }
         init_time_get_params->in_port_thresh = in_port_thresh;
      }

      if (init_time_get_params->output_port_index.valid)
      {
         i = 0;
         prop[i].id = CAPI_V2_PORT_DATA_THRESHOLD;
         prop[i].payload.actual_data_len = 0;
         prop[i].payload.max_data_len = sizeof(capi_v2_port_data_threshold_t);
         prop[i].port_info.is_valid = true;
         prop[i].port_info.is_input_port = false;
         prop[i].port_info.port_index = init_time_get_params->output_port_index.index;
         prop[i].payload.data_ptr = (int8_t*)&out_port_thresh;
         i++;

         props_list.prop_ptr = prop;
         props_list.props_num = i;

         err_code = capi_v2_ptr->vtbl_ptr->get_properties(capi_v2_ptr, &props_list);
         if (CAPI_V2_FAILED(err_code))
         {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: Get Init Properties error. Assuming %u for output port thresh.", DEFAULT_BUF_SIZE);
               out_port_thresh.threshold_in_bytes = DEFAULT_BUF_SIZE;
         }
         //when 1 is returned, it means capi can work with any size.
         if (out_port_thresh.threshold_in_bytes == 1)
         {
            out_port_thresh.threshold_in_bytes = DEFAULT_BUF_SIZE;
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: CAPI returned output thresh 1 byte, changing it to %u", DEFAULT_BUF_SIZE);
         }

         init_time_get_params->out_port_thresh = out_port_thresh;
      }
   }
   else //if a module needs no data buffering assume some threshold for the sake of service requirements.
   {
      init_time_get_params->in_port_thresh.threshold_in_bytes = DEFAULT_BUF_SIZE;
      init_time_get_params->out_port_thresh.threshold_in_bytes = DEFAULT_BUF_SIZE;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: Get Properties done with stack_size=%lu",
         stack_size.size_in_bytes);

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: Get Port Data thresh Properties done with in_buffer_len=%lu, in_valid=%d, out_buffer_len=%lu, out_valid=%d",
         in_port_thresh.threshold_in_bytes, init_time_get_params->input_port_index.valid,
         out_port_thresh.threshold_in_bytes, init_time_get_params->output_port_index.valid);

   return capi_v2_err_to_adsp_result(err_code);
}

ADSPResult aud_enc_svc_get_port_thresh(capi_v2_t *capi_v2_ptr, bool_t is_input, uint16_t port_index, uint32_t *thresh)
{
   capi_v2_err_t err_code = CAPI_V2_EOK;

   *thresh = 0;

   capi_v2_proplist_t props_list;
   capi_v2_prop_t prop[1];

   capi_v2_port_data_threshold_t port_thresh;

   memset(&port_thresh, 0, sizeof(capi_v2_port_data_threshold_t));

   uint32_t i=0;
   prop[i].id = CAPI_V2_PORT_DATA_THRESHOLD;
   prop[i].payload.actual_data_len = 0;
   prop[i].payload.max_data_len = sizeof(capi_v2_port_data_threshold_t);
   prop[i].port_info.is_valid = true;
   prop[i].port_info.is_input_port = is_input;
   prop[i].port_info.port_index = port_index;

   prop[i].payload.data_ptr = (int8_t*)&thresh;
   i++;

   props_list.prop_ptr = prop;
   props_list.props_num = i;

   err_code = capi_v2_ptr->vtbl_ptr->get_properties(capi_v2_ptr, &props_list);
   if (CAPI_V2_FAILED(err_code))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: Get in data thresh Properties error");
      return capi_v2_err_to_adsp_result(err_code);
   }

   *thresh = port_thresh.threshold_in_bytes;

   return capi_v2_err_to_adsp_result(err_code);
}

/** value must be CAPI_V2_DATA_FORMAT_INVALID_VAL when not available*/
ADSPResult aud_enc_svc_set_media_fmt(capi_v2_t *capi_v2_ptr, audio_enc_svc_capi_v2_media_fmt_t *media_fmt, capi_v2_property_id_t prop_id)
{
   capi_v2_err_t err_code = CAPI_V2_EOK;

   capi_v2_proplist_t props_list;
   capi_v2_prop_t props[1];

   props[0].id = prop_id;
   props[0].payload.actual_data_len = props[0].payload.max_data_len = sizeof(*media_fmt);
   props[0].payload.data_ptr = reinterpret_cast<int8_t*>(media_fmt);
   props[0].port_info.is_valid = true;
   if (prop_id == CAPI_V2_INPUT_MEDIA_FORMAT)
   {
      props[0].port_info.is_input_port = true;
   }
   else
   {
      props[0].port_info.is_input_port = false;
   }

   props[0].port_info.port_index = 0;

   props_list.props_num = 1;
   props_list.prop_ptr = props;

   err_code = capi_v2_ptr->vtbl_ptr->set_properties(capi_v2_ptr, &props_list);
   if (CAPI_V2_FAILED(err_code))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: setting media fmt 0x%lx failed", prop_id);
   }
   return capi_v2_err_to_adsp_result(err_code);
}

ADSPResult aud_enc_svc_set_in_sample_rate(capi_v2_t *capi_v2_ptr, uint16_t in_port_index, uint32_t sample_rate)
{
   capi_v2_err_t err_code = CAPI_V2_EOK;

   capi_v2_proplist_t props_list;
   capi_v2_prop_t props[1];

   audio_enc_svc_capi_v2_media_fmt_t media_fmt;
   media_fmt.main.format_header.data_format = CAPI_V2_FIXED_POINT;

   init_media_fmt(&media_fmt.std);

   media_fmt.std.sampling_rate = sample_rate;

   props[0].id = CAPI_V2_INPUT_MEDIA_FORMAT;
   props[0].payload.actual_data_len = props[0].payload.max_data_len = sizeof(media_fmt);
   props[0].payload.data_ptr = reinterpret_cast<int8_t*>(&media_fmt);
   props[0].port_info.is_valid = true;
   props[0].port_info.is_input_port = true;
   props[0].port_info.port_index = in_port_index;

   props_list.props_num = 1;
   props_list.prop_ptr = props;

   err_code = capi_v2_ptr->vtbl_ptr->set_properties(capi_v2_ptr, &props_list);
   if (CAPI_V2_FAILED(err_code))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: setting bits per sample failed");
      return capi_v2_err_to_adsp_result(err_code);
   }
   return capi_v2_err_to_adsp_result(err_code);
}

ADSPResult aud_enc_svc_algorithmic_reset(capi_v2_t *capi_v2_ptr)
{
   capi_v2_err_t result = ADSP_EOK;

   capi_v2_proplist_t props_list;
   capi_v2_prop_t props[1];

   props[0].id = CAPI_V2_ALGORITHMIC_RESET;
   props[0].payload.actual_data_len = props[0].payload.max_data_len = 0;
   props[0].payload.data_ptr = NULL;
   props[0].port_info.is_valid = false;

   props_list.props_num = 1;
   props_list.prop_ptr = props;

   result = capi_v2_ptr->vtbl_ptr->set_properties(capi_v2_ptr, &props_list);
   if (CAPI_V2_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: algorithmic reset failed");
      return result;
   }
   return capi_v2_err_to_adsp_result(result);
}

ADSPResult aud_enc_svc_get_param(capi_v2_t *capi_v2_ptr, uint32_t param_id, int8_t *payload, uint32_t size)
{
   capi_v2_err_t res = CAPI_V2_EOK;

   capi_v2_buf_t buf;
   buf.actual_data_len = size;
   buf.data_ptr = (int8_t*)payload;
   buf.max_data_len = size;

   capi_v2_port_info_t port_info;
   port_info.is_valid = false;

   //Assume the Set param value would be a 32-bit integer, as is specified in CAPI document.
   res = capi_v2_ptr->vtbl_ptr->get_param(capi_v2_ptr, param_id, &port_info, &buf);

   if (CAPI_V2_FAILED(res))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: get param for (param id 0x%lx) result %d", param_id, res);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: get param for (param id 0x%lx) success", param_id);
   }

   return capi_v2_err_to_adsp_result(res);
}

ADSPResult aud_enc_svc_set_param(capi_v2_t *capi_v2_ptr, uint32_t param_id, int8_t *payload, uint32_t size)
{
   capi_v2_err_t res = ADSP_EOK;

   capi_v2_buf_t buf;
   buf.actual_data_len = size;
   buf.data_ptr = payload;
   buf.max_data_len = size;

   capi_v2_port_info_t port_info;
   port_info.is_valid = false;

   //Assume the Set param value would be a 32-bit integer, as is specified in CAPI document.
   res = capi_v2_ptr->vtbl_ptr->set_param(capi_v2_ptr, param_id, &port_info, &buf);

   if (CAPI_V2_FAILED(res))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: set param for (param id 0x%lx) result %d", param_id, res);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc: set param for (param id 0x%lx) success", param_id);
   }

   return capi_v2_err_to_adsp_result(res);
}

enc_AMDB_presence audio_enc_svc_get_amdb_presence(Enc_CAPI_Type type, uint32_t id1, uint32_t id2, void **amdb_capi_handle_ptr)
{
   ADSPResult result = ADSP_EOK;
   enc_AMDB_presence ret = ENC_AMDB_PRESENCE_NOT_PRESENT;
   *amdb_capi_handle_ptr = NULL;

   uint32_t amdb_type;

   switch(type)
   {
   case ENC_CAPI_TYPE_AUTO:
      amdb_type = AMDB_MODULE_TYPE_ENCODER;
      break;
   case ENC_CAPI_TYPE_DEPACKETIZER:
      amdb_type = AMDB_MODULE_TYPE_DEPACKETIZER;
	  break;
   case ENC_CAPI_TYPE_PACKETIZER:
      amdb_type = AMDB_MODULE_TYPE_PACKETIZER;
      break;
   default:
      return ret;
   }

   //first check CAPI V2
   adsp_amdb_module_handle_info_t module_handle_info;
   module_handle_info.interface_type = CAPI_V2;
   module_handle_info.type = amdb_type;
   module_handle_info.id1 = id1;
   module_handle_info.id2 = id2;
   module_handle_info.h.capi_v2_handle = NULL;
   module_handle_info.result = ADSP_EFAILED;

   /*
    * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
    * happens using a thread pool using threads of very low priority. This can cause the current thread
    * to be blocked because of a low priority thread. If this is not desired, a callback function
    * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
    * thread can then handle other tasks in parallel.
    */
   adsp_amdb_get_modules_request(&module_handle_info, 1, NULL, NULL);

   result = module_handle_info.result;
   if (ADSP_SUCCEEDED(result))
   {
      switch(module_handle_info.interface_type)
      {
      case CAPI_V2:
      {
         ret = ENC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2;
         *amdb_capi_handle_ptr = (void*)module_handle_info.h.capi_v2_handle;
         break;
      }
      case STUB:
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc: module present as stub.");
            adsp_amdb_release_handles(&module_handle_info, 1);
            ret = ENC_AMDB_PRESENCE_PRESENT_AS_STUB;
            break;
         }
      default:
         adsp_amdb_release_handles(&module_handle_info, 1);
         break;
      }
   }

   return ret;
}

