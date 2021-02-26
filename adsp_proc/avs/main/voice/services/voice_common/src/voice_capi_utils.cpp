/*
 * voice_capi_utils.cpp
 *
 *  Created on: Sep 26, 2014
 *      Author: krupeshn
 */
/*========================================================================
$Header:

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/26/2014  kn       Created
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------------*/

/* =======================================================================
INCLUDE FILES FOR MODULE
==========================================================================*/

#include "VoiceCmnUtils.h"
#include "voice_capi_utils.h"
#include "adsp_amdb_static.h"

/* =======================================================================
**                          Function Definitions
** =======================================================================*/
/**
 * This function queries static properties from capi modules to create capi instance
 * @param[in] module_id, module id of the algorithm
 * @param[in] capi_v2_get_static_prop_f, function pointer to get_static_properties of the module
 * @param[out] module_info, Module structure updated based on the get static properties
 * @return uint32_t, returns size in bytes required for capi structure of the module
 */
uint32_t voice_capi_get_static_prop(voice_capi_module_t* module_info_ptr)
{
   ADSPResult adsp_result;
   capi_v2_init_memory_requirement_t mem_req = { 0 };
   capi_v2_stack_size_t stack_size = { 0 };
   capi_v2_is_inplace_t inplace = { 1 }; //assumed true
   capi_v2_requires_data_buffering_t needs_buffering = { 0 }; //assumed false
   capi_v2_port_info_t port_info = {FALSE, FALSE, 0};
   capi_v2_num_needed_framework_extensions_t  num_of_extn = { 0 };

   if(module_info_ptr==NULL || NULL == module_info_ptr->amdb_handle_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_get_static_prop: received bad pointer");
      return 0; // return size 0 in case of failure.
   }

   adsp_amdb_capi_v2_t* amdb_ptr = (adsp_amdb_capi_v2_t*)module_info_ptr->amdb_handle_ptr;
   // Properties that are queried statically
   capi_v2_prop_t static_props[] = {
         { CAPI_V2_INIT_MEMORY_REQUIREMENT,         { reinterpret_cast<int8_t *>(&mem_req), sizeof(mem_req), sizeof(mem_req) },               port_info },
         { CAPI_V2_STACK_SIZE,                      { reinterpret_cast<int8_t *>(&stack_size), sizeof(stack_size), sizeof(stack_size) },      port_info },
         { CAPI_V2_IS_INPLACE,                      { reinterpret_cast<int8_t *>(&inplace), sizeof(inplace), sizeof(inplace) },               port_info },
         { CAPI_V2_REQUIRES_DATA_BUFFERING,         { reinterpret_cast<int8_t *>(&needs_buffering), sizeof(needs_buffering), sizeof(needs_buffering) }, port_info },
         { CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS, { reinterpret_cast<int8_t *>(&num_of_extn), sizeof(num_of_extn), sizeof(num_of_extn) },   port_info },
   };

   capi_v2_proplist_t static_proplist = { VOICE_SIZE_OF_ARRAY(static_props), static_props };

   adsp_result = adsp_amdb_capi_v2_get_static_properties_f(amdb_ptr, NULL, &static_proplist);
   
   if(ADSP_EOK != adsp_result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_get_static_prop: failed for module_id(0x%lx). result(%ld)", module_info_ptr->module_id, adsp_result);
      return 0; // return size 0 in case of failure.
   }
   module_info_ptr->mem_size_allocated =VOICE_ROUNDTO8 (mem_req.size_in_bytes); //SA: this will not be the memory returned by the module. AMDB will add 12 bytes of memory for its wrapper
   module_info_ptr->is_in_place = inplace.is_inplace;
   module_info_ptr->stack_size = stack_size.size_in_bytes;
   module_info_ptr->requires_data_buffering = needs_buffering.requires_data_buffering;
   module_info_ptr->num_extensions.num_extensions = num_of_extn.num_extensions;

   if(num_of_extn.num_extensions)
   {
      // allocate memory for extention id's based on the num of extentions.
      // free this memory in the module's end function.
      module_info_ptr->fwk_extn_ptr = (capi_v2_framework_extension_id_t *)qurt_elite_memory_malloc(sizeof(capi_v2_framework_extension_id_t) * num_of_extn.num_extensions, QURT_ELITE_HEAP_DEFAULT);
      if(NULL == module_info_ptr->fwk_extn_ptr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: get_static_prop: Out of memory for fwk extensions for module_id(0x%lx)", module_info_ptr->module_id);
         //return ADSP_ENOMEMORY; TODO: add error handling
      }
      else
      {  // query extension id's from the module
         // SA: Adding CAPI_V2_INIT_MEMORY_REQUIREMENT propId as well because adsp_amdb API require one of the get_static prop to be INIT prop
         capi_v2_prop_t static_props_extn[] = {
            { CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS, { reinterpret_cast<int8_t *>(module_info_ptr->fwk_extn_ptr), 0, sizeof(capi_v2_framework_extension_id_t) * num_of_extn.num_extensions }, { FALSE, FALSE, 0} },
            { CAPI_V2_INIT_MEMORY_REQUIREMENT,         { reinterpret_cast<int8_t *>(&mem_req), sizeof(mem_req), sizeof(mem_req) },               port_info },
         };

         static_proplist.props_num = VOICE_SIZE_OF_ARRAY(static_props_extn);
         static_proplist.prop_ptr  = static_props_extn;

         adsp_result = adsp_amdb_capi_v2_get_static_properties_f(amdb_ptr, NULL, &static_proplist);
         if(CAPI_V2_EOK != adsp_result)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: get_static_prop: failed for module_id(0x%lx), prop_id: CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS, result(%ld)", module_info_ptr->module_id, adsp_result);
         }
         else
         {
            for(uint32_t i = 0; i <num_of_extn.num_extensions; i++)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_static_prop: module_id(0x%lx) would need extension_id(0x%lx)", module_info_ptr->module_id, module_info_ptr->fwk_extn_ptr[i].id);
            }
         }
      }
   }

   return(module_info_ptr->mem_size_allocated);
}

ADSPResult voice_capi_query_intf_ext(voice_capi_module_t* module_info_ptr)
{
   ADSPResult result = ADSP_EOK;

   if(module_info_ptr==NULL || NULL == module_info_ptr->amdb_handle_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_capi_query_intf_ext: received bad pointer");
      return ADSP_EFAILED;
   }

   voice_intf_extn_list intf_ext_list = {
         {   2   },
         {
               {INTF_EXTN_VOCODER, FALSE, { NULL, 0, 0 } },
               {INTF_EXTN_TTY,     FALSE, { NULL, 0, 0 } },
         }
   };

   capi_v2_port_info_t port_info = {FALSE, FALSE, 0};
   adsp_amdb_capi_v2_t* amdb_ptr = (adsp_amdb_capi_v2_t*)module_info_ptr->amdb_handle_ptr;

   capi_v2_prop_t static_props[] = {
         { CAPI_V2_INTERFACE_EXTENSIONS,    { reinterpret_cast<int8_t *>(&intf_ext_list), sizeof(intf_ext_list), sizeof(intf_ext_list) },   port_info },
   };

   capi_v2_proplist_t static_proplist = { VOICE_SIZE_OF_ARRAY(static_props), static_props };

   result = adsp_amdb_capi_v2_get_static_properties_f(amdb_ptr, NULL, &static_proplist);

   if(ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_capi_query_intf_ex. Error!! result(%ld)", result);
      module_info_ptr->intf_extn_list.is_tty_intf_supported = FALSE;
      module_info_ptr->intf_extn_list.is_voc_intf_supported = FALSE;
      return result;
   }

   module_info_ptr->intf_extn_list.is_voc_intf_supported = intf_ext_list.intf_extn_desc[INTF_EXTN_VOCODER_IDX].is_supported;
   module_info_ptr->intf_extn_list.is_tty_intf_supported = intf_ext_list.intf_extn_desc[INTF_EXTN_TTY_IDX].is_supported;

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_capi_query_intf_ext: Module(%lx) is_tty_intf_supported(%d) is_voc_intf_supported(%d)",
         module_info_ptr->module_id, (int)module_info_ptr->intf_extn_list.is_tty_intf_supported, (int)module_info_ptr->intf_extn_list.is_voc_intf_supported);

   return result;
}
/**
 * This function populates data format structure
 * @param[in, out] data_format_struct_ptr, data format structure to be updated
 * @param[in] media_type_ptr, input media type structure pointer
 */
void voice_capi_populate_data_format_struct(voice_capi_data_format_struct_t *data_format_struct_ptr, elite_multi_channel_pcm_fmt_blk_t *media_type_ptr)
{
   if((NULL == data_format_struct_ptr) || (NULL == media_type_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_capi_populate_data_format_struct: received bad pointer");
      return;
   }
   data_format_struct_ptr->data_format.bitstream_format  = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   data_format_struct_ptr->data_format.num_channels      = media_type_ptr->num_channels;
   data_format_struct_ptr->data_format.bits_per_sample   = media_type_ptr->bits_per_sample;
   data_format_struct_ptr->data_format.sampling_rate     = media_type_ptr->sample_rate;
   data_format_struct_ptr->data_format.data_is_signed    = media_type_ptr->is_signed;
   data_format_struct_ptr->data_format.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
   data_format_struct_ptr->data_format.q_factor          = 15;
   data_format_struct_ptr->media_format.format_header.data_format = CAPI_V2_FIXED_POINT;

   // array size of channel_type[16] is more than array size of channel_mapping[8]
   // for voice max supported num_channels equal to 4. so updating in loop
   for(uint32_t i=0; i < data_format_struct_ptr->data_format.num_channels; i++)
   {
      data_format_struct_ptr->data_format.channel_type[i] = (uint16_t)media_type_ptr->channel_mapping[i];
   }
}

bool_t is_capi_media_fmt_same(voice_capi_data_format_struct_t *media_fmt1, voice_capi_data_format_struct_t *media_fmt2)
{
   if( (media_fmt1->media_format.format_header.data_format == media_fmt2->media_format.format_header.data_format) &&
         (media_fmt1->data_format.sampling_rate == media_fmt2->data_format.sampling_rate)  &&
         (media_fmt1->data_format.num_channels == media_fmt2->data_format.num_channels) )
   {
      return TRUE;
   }

   return FALSE;
}

capi_v2_err_t voice_imc_register_src_dest(imc_src_dst_info_t *imc_src_dst_into, int8_t *key, uint32_t keylen,uint32_t max_bufq_len,uint32_t alloc_num_buf,uint32 buf_size, imc_src_dst_t src_dst)
{
   capi_v2_err_t imc_result = CAPI_V2_EOK;

   uint32_t comm_id = *(uint32_t*)key;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_imc_register_src_dest: module is trying to register for IMC CommID(%#lx), src_dst(%d)",comm_id,(int)src_dst);

   //Check if the key is already received
   if(IMC_STATE_NONE != imc_src_dst_into->state)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_imc_register_src_dest: Error!! trying to set multiple Key values for the same CommID(%lx). state(%d)",
            comm_id,(int)imc_src_dst_into->state);
      return CAPI_V2_EBADPARAM;
   }

   //clear Key
   memset(imc_src_dst_into->key,0,sizeof(imc_src_dst_into->key));

   imc_src_dst_into->key_length = keylen;
   memscpy(imc_src_dst_into->key,sizeof(imc_src_dst_into->key),key,keylen);

   //Now register for the intermodule communication
   if(IMC_SOURCE == src_dst)
   {
      imc_result = imc_src_dst_into->imc_lib_handle->vtable->src_register(&imc_src_dst_into->source_handle,max_bufq_len);
      if(CAPI_V2_FAILED(imc_result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_imc_register_src_dest: Error!! source failed to register, result(0x%lx)",imc_result);
         return imc_result;
      }

      //Now Fill the source queue with buffers
      imc_result = imc_src_dst_into->imc_lib_handle->vtable->src_allocate_push_self_buf(imc_src_dst_into->source_handle, buf_size, alloc_num_buf);
      if(CAPI_V2_FAILED(imc_result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_imc_register_src_dest: Error!! source failed to push buffers, result(0x%lx)",imc_result);
         return imc_result;
      }
      else
      {
         imc_src_dst_into->state =  IMC_STATE_REGISTERED;
      }
   }
   else if(IMC_DESTINATION == src_dst)
   {
      imc_result = imc_src_dst_into->imc_lib_handle->vtable->dest_register(&imc_src_dst_into->dest_handle,(int8_t*)imc_src_dst_into->key,imc_src_dst_into->key_length,max_bufq_len);
      if(CAPI_V2_FAILED(imc_result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_imc_register_src_dest: Error!! destination failed to register, result(0x%lx)",imc_result);
         return imc_result;
      }
      else
      {
         imc_src_dst_into->state =  IMC_STATE_REGISTERED;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_imc_register_src_dest: Error!! invalid source/destination param(%d)",(int)src_dst);
      imc_result = CAPI_V2_EBADPARAM;
   }

   return imc_result;
}

ADSPResult voice_topo_get_module_info( voice_capi_module_t* modules_array, uint32_t module_id,uint16_t instance_id, uint32_t num_module, voice_capi_module_t **modules_info)
{
   if( (NULL == modules_array) || (NULL == modules_info) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_topo_get_module_info: received bad pointer");
      return ADSP_ENOMEMORY;
   }

   *modules_info = NULL;

   for (uint32_t i = 0; i < num_module; i++)
   {
      if( (module_id == modules_array[i].module_id) &&
            (instance_id == modules_array[i].instance_id))
      {
         *modules_info = &modules_array[i];
         return ADSP_EOK;
      }
   }

   //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_mod_ptr(): The module0x%lx with instance(%lx)is not found in the topo ", module_id,instance_id);
   return ADSP_EUNSUPPORTED;
}

bool_t voice_check_voc_capi(uint32_t media_type)
{
   switch(media_type)
   {
      case VSM_MEDIA_TYPE_13K_MODEM:
      case VSM_MEDIA_TYPE_AMR_WB_MODEM:
      case VSM_MEDIA_TYPE_AMR_NB_MODEM:
      case VSM_MEDIA_TYPE_AMR_NB_IF2:
      case VSM_MEDIA_TYPE_EAMR:
      case VSM_MEDIA_TYPE_EVRC_MODEM:
      case VSM_MEDIA_TYPE_EVS:
      case VSM_MEDIA_TYPE_EFR_MODEM:
      case VSM_MEDIA_TYPE_4GV_NB_MODEM:
      case VSM_MEDIA_TYPE_4GV_WB_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW:
      case VSM_MEDIA_TYPE_EVRC_NW_2K:
      case VSM_MEDIA_TYPE_FR_MODEM:
      case VSM_MEDIA_TYPE_HR_MODEM:
      case VSM_MEDIA_TYPE_PCM_8_KHZ:
      case VSM_MEDIA_TYPE_PCM_16_KHZ:
      case VSM_MEDIA_TYPE_PCM_32_KHZ:
      case VSM_MEDIA_TYPE_PCM_48_KHZ:
      case VSM_MEDIA_TYPE_G729AB:
      case VSM_MEDIA_TYPE_G711_ALAW:
      case VSM_MEDIA_TYPE_G711_MLAW:
      case VSM_MEDIA_TYPE_G711_ALAW_V2:
      case VSM_MEDIA_TYPE_G711_MLAW_V2:
      {
         return TRUE;
      }
      default:
      {
        return FALSE;
      }
   }
}

ADSPResult voice_capi_service_state_fwk_extn_handler(voice_capi_module_t *module_info_ptr)
{
   ADSPResult result = ADSP_EOK;

   if (NULL == module_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_capi_service_state_fwk_extn_handler: received bad pointer");
      return ADSP_EFAILED;
   }

   module_info_ptr->fwk_extn_list.is_service_state_fwk_extn_supported = TRUE;

   return result;
}

ADSPResult voice_capi_cng_fwk_extn_handler(voice_capi_module_t *module_info_ptr)
{
   ADSPResult result = ADSP_EOK;

   if (NULL == module_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_capi_service_state_fwk_extn_handler: received bad pointer");
      return ADSP_EFAILED;
   }

   module_info_ptr->fwk_extn_list.is_cng_fwk_extn_supported = TRUE;

   return result;
}

void voice_print_media_fmt(voice_capi_module_t* curr_module_ptr)
{
#if defined(LOG_VOICEPROC_DATA)
   if(!curr_module_ptr)
   {
      return;
   }

   voice_capi_data_format_struct_t *media_fmt_ptr;

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Media format for module (%lx): num of io ports (%ld) (%ld)",
         curr_module_ptr->module_id,curr_module_ptr->port_info.port_num_info.num_input_ports,curr_module_ptr->port_info.port_num_info.num_output_ports);

   for(uint32_t port_id = 0; port_id < curr_module_ptr->port_info.port_num_info.num_input_ports; port_id++)
   {
      media_fmt_ptr = &curr_module_ptr->port_info.input_port[port_id].media_fmt;

      MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "input port (%d). samp_rate(%ld) num_channels(%ld) Channel mapping for channels 1 to 6: %d, %d, %d, %d, %d, %d",
            port_id, media_fmt_ptr->data_format.sampling_rate, media_fmt_ptr->data_format.num_channels,
            media_fmt_ptr->data_format.channel_type[0], media_fmt_ptr->data_format.channel_type[1], media_fmt_ptr->data_format.channel_type[2],
            media_fmt_ptr->data_format.channel_type[3], media_fmt_ptr->data_format.channel_type[4], media_fmt_ptr->data_format.channel_type[5] );
   }

   for(uint32_t port_id = 0; port_id < curr_module_ptr->port_info.port_num_info.num_output_ports; port_id++)
   {
      media_fmt_ptr = &curr_module_ptr->port_info.output_port[port_id].media_fmt;

      MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "output port (%d). samp_rate(%ld) num_channels(%ld) Channel mapping for channels 1 to 6: %d, %d, %d, %d, %d, %d",
            port_id, media_fmt_ptr->data_format.sampling_rate, media_fmt_ptr->data_format.num_channels,
            media_fmt_ptr->data_format.channel_type[0], media_fmt_ptr->data_format.channel_type[1], media_fmt_ptr->data_format.channel_type[2],
            media_fmt_ptr->data_format.channel_type[3], media_fmt_ptr->data_format.channel_type[4], media_fmt_ptr->data_format.channel_type[5] );
   }
#endif
}
