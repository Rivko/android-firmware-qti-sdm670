/*========================================================================

*//** @file Vprx_Modules.cpp

Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code for module handling within Voice Proc Rx(VPRX)
Dynamic service.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/multimedia2/Audio/vocproc/VoiceDevRx/...

when       who     what, where, why
--------   ---     -------------------------------------------------------
21/10/11   ss       Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Vprx_Modules.h"
#include "Vprx_Svc.h"
#include "adsp_amdb_static.h"
#include "capi_v2_library_factory.h"

//thread stack for from config file
extern const unsigned int VPRX_STACK_SIZE;

/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */
capi_v2_err_t vprx_capi_v2_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr);
int32_t vprx_send_hpcm_buf_available_evt(vprx_t* pVprx, void* data_ptr);
static void vprx_free_proc_buf_mem(vprx_t *pVprx);
static ADSPResult vprx_allocate_proc_buf_mem(vprx_t *pVprx, uint32_t max_samp_rate, uint32_t max_chan_num);
static void vprx_out_circ_buf_mem_free(vprx_t *pVprx);
static ADSPResult vprx_out_circ_buf_mem_alloc(vprx_t *pVprx);
static ADSPResult vprx_propagate_media_fmt(vprx_t *pVprx, voice_capi_module_t *module_ptr);
static ADSPResult vprx_send_ss_op_mode(vprx_t *pVprx,voice_capi_module_t *ss_module_ptr);
static uint32_t vprx_get_port_info_size(vprx_t *pVprx);
static void vprx_set_port_info_mem(vprx_t *pVprx, int8_t **mem);


static void vprx_free_proc_buf_mem(vprx_t *pVprx)
{
   //free memory
   if(pVprx->io.proc_buf_ptr[0])
   {
      qurt_elite_memory_free(pVprx->io.proc_buf_ptr[0]);
      pVprx->io.proc_buf_ptr[0] = NULL;
      pVprx->io.proc_buf_ptr_sec[0] = NULL;
   }

      pVprx->io.max_samp_rate = 0;
      pVprx->io.max_chan_num = 0;
}

static ADSPResult vprx_allocate_proc_buf_mem(vprx_t *pVprx, uint32_t max_samp_rate, uint32_t max_chan_num)
{

   if( (max_samp_rate > VOICE_FB_SAMPLING_RATE) ||
         (max_chan_num > VPRX_MAX_CHAN) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_allocate_proc_buf_mem(): Invalid max_samp_rate(%ld) or max_chan_num(%ld)",
            max_samp_rate,max_chan_num);
      return ADSP_EBADPARAM;
   }

   //re-allocate buffer only if the size has changed
   if(  (max_samp_rate != pVprx->io.max_samp_rate) ||
         (max_chan_num != pVprx->io.max_chan_num) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_allocate_proc_buf_mem(): proc_buf_size has changed. max_samp_rate(%ld). max_num_chan(%ld)",
            max_samp_rate,max_chan_num);

      pVprx->io.max_samp_rate = max_samp_rate;
      pVprx->io.max_chan_num = max_chan_num;

      uint32_t max_buf_size_bytes_per_ch = ((max_samp_rate/VOICE_NB_SAMPLING_RATE)*(FRAME_SAMPLES + DMA_SAMPLES))<<1;
      uint32_t max_buf_size_bytes = max_buf_size_bytes_per_ch * max_chan_num;

      //free memory if already allocated
      if(pVprx->io.proc_buf_ptr[0])
      {
         qurt_elite_memory_free(pVprx->io.proc_buf_ptr[0]);
         pVprx->io.proc_buf_ptr[0] = NULL;
      }

      //by default allocate memory for 2 buffers to support non-in_place modules
      int8_t * mem_ptr = (int8_t *)qurt_elite_memory_malloc(max_buf_size_bytes<<1, QURT_ELITE_HEAP_DEFAULT);
      if(!mem_ptr)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_allocate_proc_buf_mem(): failed to allocate memory for process buf.Size(%ld), session(%#lx)",
             (max_buf_size_bytes<<1),pVprx->session.session_num);
         return ADSP_ENOMEMORY;
      }

      for(uint32_t i=0; i< max_chan_num; i++ )
      {
         pVprx->io.proc_buf_ptr[i] = mem_ptr;

         //update capi v2 buffers
         pVprx->io_buf[PROC_BUF_1][i].data_ptr = mem_ptr;
         pVprx->io_buf[PROC_BUF_1][i].max_data_len = max_buf_size_bytes_per_ch;   //max size will be set corresponding to max samp rate for all modules
         mem_ptr += max_buf_size_bytes_per_ch;

         //update sec buf pointer
         pVprx->io.proc_buf_ptr_sec[i] = mem_ptr;

         //update capi v2 buffers
         pVprx->io_buf[PROC_BUF_2][i].data_ptr = mem_ptr;
         pVprx->io_buf[PROC_BUF_2][i].max_data_len = max_buf_size_bytes_per_ch;
         mem_ptr += max_buf_size_bytes_per_ch;
      }

      pVprx->io_stream_data[PROC_BUF_1].buf_ptr = &pVprx->io_buf[PROC_BUF_1][0];
      pVprx->io_stream_data[PROC_BUF_1].bufs_num = 1;   //default init to 1. it will be changed in process function

      pVprx->io_stream_data[PROC_BUF_2].buf_ptr = &pVprx->io_buf[PROC_BUF_2][0];
      pVprx->io_stream_data[PROC_BUF_2].bufs_num = 1;   //default init to 1. it will be changed in process function
   }

   return ADSP_EOK;
}

static void vprx_out_circ_buf_mem_free(vprx_t *pVprx)
{
   if(pVprx->io.output.circ_struct.buf_ptr[0])
   {
      qurt_elite_memory_free(pVprx->io.output.circ_struct.buf_ptr[0]);
      pVprx->io.output.circ_struct.buf_ptr[0] = NULL;
   }
}


static ADSPResult vprx_out_circ_buf_mem_alloc(vprx_t *pVprx)
{

   if( (pVprx->io.output.mediatype.sample_rate > VOICE_FB_SAMPLING_RATE) ||
         (pVprx->io.output.mediatype.num_channels > VPRX_MAX_CHAN) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_out_circ_buf_mem_alloc(): Invalid max_samp_rate(%ld) or max_chan_num(%ld)",
            pVprx->io.output.mediatype.sample_rate, pVprx->io.output.mediatype.num_channels);
      return ADSP_EBADPARAM;
   }

   uint32_t out_nb_sampling_rate_factor = pVprx->io.output.mediatype.sample_rate/VOICE_NB_SAMPLING_RATE;

   uint32_t out_circ_samples_per_ch           = OUTBUF_SAMPLES * out_nb_sampling_rate_factor;
   pVprx->io.output.frame_samples        = FRAME_SAMPLES * out_nb_sampling_rate_factor;
   pVprx->io.output.min_frame_samples  = DMA_SAMPLES * out_nb_sampling_rate_factor;     // 1ms


   //free circular buffer memory if already allocated
   if(pVprx->io.output.circ_struct.buf_ptr[0])
   {
      /*
       * print a message if circular buffer has any samples. Since media fmt
       * is changing only at STOP state for now we dont lose samples.
       * TODO: handle this in future where STOP is not required
       */
      //MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_out_circ_buf_mem_alloc(): freeing memory of output circ buf");
      if(pVprx->io.output.circ_struct.circ_buf[0].unread_samples)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_out_circ_buf_mem_alloc(): discarding unread samples(%ld, session(%#lx)",
               pVprx->io.output.circ_struct.circ_buf[0].unread_samples,pVprx->session.session_num);
      }
      qurt_elite_memory_free(pVprx->io.output.circ_struct.buf_ptr[0]);
      pVprx->io.output.circ_struct.buf_ptr[0] = NULL;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_out_circ_buf_mem_alloc(): circ buf samples(%ld), samp_rate(%d), num_chan(%ld)",
         out_circ_samples_per_ch,pVprx->io.output.mediatype.sample_rate,pVprx->io.output.mediatype.num_channels);

   uint32_t total_out_samples = out_circ_samples_per_ch*pVprx->io.output.mediatype.num_channels;
   int16_t *mem_ptr  = (int16_t *)qurt_elite_memory_malloc(total_out_samples<<1, QURT_ELITE_HEAP_DEFAULT);
   if(!mem_ptr)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_out_circ_buf_mem_alloc(): failed to allocate memory output buf.Size(%ld, session(%#lx)",(total_out_samples<<1),pVprx->session.session_num);
      return ADSP_ENOMEMORY;
   }

   for(uint32_t i=0; i < pVprx->io.output.mediatype.num_channels ; i++ )
   {
      pVprx->io.output.circ_struct.buf_ptr[i] = (int8_t *)mem_ptr;
      mem_ptr += out_circ_samples_per_ch;
   }

   // Initialize Circular Buffer for out circ buffer
   voice_multi_ch_circ_buf_init(&(pVprx->io.output.circ_struct), (int32_t) pVprx->io.output.mediatype.num_channels, (int32_t) out_circ_samples_per_ch, (int32_t) 16);

   //TODO: ADD prebuf samples?
   return ADSP_EOK;

}

static ADSPResult vprx_send_ss_op_mode(vprx_t *pVprx,voice_capi_module_t *ss_module_ptr)
{
   capi_v2_buf_t param_data_buf;
   set_ss_operating_mode_t ss_operating_mode;
   capi_v2_port_info_t port_info         = {TRUE,TRUE,0};
   capi_v2_err_t result;

   ss_operating_mode.operating_mode= SS_MODE_5;

   param_data_buf.data_ptr =(int8_t*) &ss_operating_mode;
   param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(set_ss_operating_mode_t);

   result = ss_module_ptr->module_ptr->vtbl_ptr->set_param(ss_module_ptr->module_ptr,SS_PARAM_MODE_OPER,&port_info,&param_data_buf);
   if(CAPI_V2_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: failed to send sample slip op mode(%d), result(%lx)",ss_operating_mode.operating_mode,result);
      return ADSP_EFAILED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VPRX sent sample slip operating mode(%d), session(%lx)",ss_operating_mode.operating_mode,pVprx->session.session_num);

   return ADSP_EOK;
}


static void vprx_get_max_proc_buf_size(vprx_t *pVprx, voice_capi_module_t *end_module_ptr,uint32_t *max_samp_rate, uint32_t *max_chan_num)
{
   voice_capi_module_t *curr_module_ptr = pVprx->modules.modules_array;
   uint32_t i = 0,last_module_index;

   *max_samp_rate = curr_module_ptr->port_info.input_port[0].media_fmt.data_format.sampling_rate;
   *max_chan_num = curr_module_ptr->port_info.input_port[0].media_fmt.data_format.num_channels;

   last_module_index = end_module_ptr->module_index;

   do
   {
      *max_samp_rate = VPRX_MAX32(*max_samp_rate, curr_module_ptr->port_info.output_port[0].media_fmt.data_format.sampling_rate);
      *max_chan_num = VPRX_MAX32(*max_chan_num,curr_module_ptr->port_info.output_port[0].media_fmt.data_format.num_channels);
      curr_module_ptr++;
      i++;
   } while(i<=last_module_index);

   return;
}


static ADSPResult vprx_propagate_media_fmt(vprx_t *pVprx, voice_capi_module_t *module_ptr)
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   uint32_t i=0;
   QURT_ELITE_ASSERT(pVprx->modules.is_prop_media_fmt == TRUE );
   voice_capi_module_t *curr_module_ptr = module_ptr;

   capi_v2_port_info_t port_info = {TRUE, TRUE, 0};
   capi_v2_prop_t set_props[]= {
         { CAPI_V2_INPUT_MEDIA_FORMAT,  { NULL, sizeof(voice_capi_data_format_struct_t), sizeof(voice_capi_data_format_struct_t)}, port_info },
   };
   capi_v2_proplist_t set_props_list = {VOICE_SIZE_OF_ARRAY(set_props),set_props};


   //get output media fmt of the module
   voice_capi_data_format_struct_t *curr_module_out_mf = &curr_module_ptr->port_info.input_port[0].media_fmt;
   if(curr_module_ptr->is_enabled)
   {
      curr_module_out_mf = &curr_module_ptr->port_info.output_port[0].media_fmt;
   }

   //set media pending flag to false
   curr_module_ptr->output_media_fmt_prop_pending = FALSE;

   // calculate module process buf samples. To avoid division each time calculate max size per sec and then get per frame
   uint32_t max_samp_rate, max_chan_num;

   vprx_get_max_proc_buf_size(pVprx,curr_module_ptr,&max_samp_rate,&max_chan_num);

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_propagate_media_fmt: starting module module_idx(%ld). num_chan=%ld, samp_rate(%ld)",curr_module_ptr->module_index,
         curr_module_out_mf->data_format.num_channels,curr_module_out_mf->data_format.sampling_rate);

   //start with the next module
   for(i=curr_module_ptr->module_index+1; i<pVprx->modules.num_modules; i++)
   {
      curr_module_ptr = &pVprx->modules.modules_array[i];

      if(NULL == curr_module_ptr->module_ptr)
      {
         //handle virtual stubs or modules which failed init or non_capi modules
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: module_ptr is null for module(%lx). ignoring ",curr_module_ptr->module_id);
         continue;
      }

      //propagate media fmt if next module media fmt is not same
      if(is_capi_media_fmt_same(&curr_module_ptr->port_info.input_port[0].media_fmt,curr_module_out_mf))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: module(%lx) media fmt is same. ending propagation",curr_module_ptr->module_id);
         break;
      }

      curr_module_out_mf->data_format.channel_type[0] = 1;  //TODO: Remove. Added for MFC
      //update input sampling rate
      curr_module_ptr->port_info.input_port[0].media_fmt = *curr_module_out_mf;

      //update payload
      set_props[0].payload.data_ptr = (int8_t*)curr_module_out_mf;

      //update max_samp_rate and max_chan_num
      max_samp_rate = VPRX_MAX32(max_samp_rate, curr_module_out_mf->data_format.sampling_rate);
      max_chan_num = VPRX_MAX32(max_chan_num,curr_module_out_mf->data_format.num_channels);

      //MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: module(%lx) propagating num_chan=%ld, samp_rate(%ld)",curr_module_ptr->module_id,
      //      curr_module_out_mf->data_format.num_channels,curr_module_out_mf->data_format.sampling_rate);

      //set the pending flag to false
      curr_module_ptr->output_media_fmt_prop_pending = FALSE;

      capi_result = curr_module_ptr->module_ptr->vtbl_ptr->set_properties(curr_module_ptr->module_ptr, &set_props_list);
      if (CAPI_V2_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_propagate_media_fmt(): set_prop failed for module(%lx). Disabling module. result (%ld) ",
               curr_module_ptr->module_id,capi_result);
         curr_module_ptr->is_enabled = FALSE;
      }
      else
      {
         //update current media fmt being propagated if module is enabled
         if(curr_module_ptr->is_enabled)
         {
            curr_module_out_mf = &curr_module_ptr->port_info.output_port[0].media_fmt;
         }

         //set the pending flag to false for the module
         curr_module_ptr->output_media_fmt_prop_pending = FALSE;
      }

      voice_print_media_fmt(curr_module_ptr);
   }

   //DBG
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_propagate_media_fmt(): done propagating media fmt. last module idx(%ld), num_modules(%ld)",
         curr_module_ptr->module_index,pVprx->modules.num_modules);

   max_samp_rate = VPRX_MAX32(max_samp_rate, curr_module_out_mf->data_format.sampling_rate);
   max_chan_num = VPRX_MAX32(max_chan_num,curr_module_out_mf->data_format.num_channels);

   result = vprx_allocate_proc_buf_mem(pVprx,max_samp_rate,max_chan_num);
   if(ADSP_FAILED(result))
   {
      return result;
   }

   //if the current module is the last in the chain, then check if VPRx output media fmt has changed
   if(curr_module_ptr->module_index == (pVprx->modules.num_modules-1) )
   {
      //has last module changed media fmt
      if( (pVprx->io.output.mediatype.sample_rate == curr_module_out_mf->data_format.sampling_rate) &&
            (pVprx->io.output.mediatype.num_channels == curr_module_out_mf->data_format.num_channels) )
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: last module has not changed output media fmt");
         return CAPI_V2_EOK;
      }

      pVprx->io.output.mediatype.sample_rate = curr_module_out_mf->data_format.sampling_rate;
      pVprx->io.output.mediatype.num_channels = curr_module_out_mf->data_format.num_channels;

      //send media fmt to AFE
      pVprx->io.output.send_mediatype_flag = TRUE;

      result = vprx_out_circ_buf_mem_alloc(pVprx);
      if(ADSP_FAILED(result))
      {
         return result;
      }

   }

   return result;
}

static void vprx_check_for_capi_module(voice_capi_module_t *module_info)
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

static uint32_t vprx_get_port_info_size(vprx_t *pVprx)
{
   uint32_t port_info_size = 0;
   uint32_t i = 0;
   voice_capi_module_t *curr_module_ptr = &pVprx->modules.modules_array[0]; // set it to the first module
   uint32_t total_size = 0;

   for(i=0; i < pVprx->modules.num_modules; i++)
   {
       curr_module_ptr = &pVprx->modules.modules_array[i]; // set it to the current module
       port_info_size = sizeof(voice_module_port_t) * curr_module_ptr->port_info.port_num_info.num_input_ports;
       total_size += VOICE_ROUNDTO8(port_info_size);
       port_info_size = sizeof(voice_module_port_t) * curr_module_ptr->port_info.port_num_info.num_output_ports;
       total_size += VOICE_ROUNDTO8(port_info_size);
   }

   return total_size;
}

static void vprx_set_port_info_mem(vprx_t *pVprx, int8_t **mem)
{
   uint32_t i = 0;
   voice_capi_module_t *curr_module_ptr = &pVprx->modules.modules_array[0]; // set it to the first module
   int8_t *pMem = *mem;

   for(i=0; i < pVprx->modules.num_modules; i++)
   {
       curr_module_ptr = &pVprx->modules.modules_array[i];
       curr_module_ptr->port_info.input_port = (voice_module_port_t *)pMem;
       pMem += (VOICE_ROUNDTO8(sizeof(voice_module_port_t) * curr_module_ptr->port_info.port_num_info.num_input_ports));

       curr_module_ptr->port_info.output_port = (voice_module_port_t *)pMem;
       pMem += (VOICE_ROUNDTO8(sizeof(voice_module_port_t) * curr_module_ptr->port_info.port_num_info.num_output_ports));
   }

   *mem = pMem;
   return;
}

/*
 * This function queries modules to identify memory requirements and
 * allocates memory accordingly.
 */
ADSPResult vprx_allocate_mem(vprx_t* pVprx, uint32_t in_circ_samples)
{
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_allocate_mem begin session(%x)",pVprx->session.session_num);
   ADSPResult nResult     = ADSP_EOK;
   int32_t  nTotalSize;
   int8_t i,vprx_pp_mod_idx;
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   //Stack variables
   uint32_t max_module_stack_needed = 0, stack_size = 0;

   nTotalSize         = (in_circ_samples << 1);

   {
      // Iterate over array and do these things:
      // 1. Update internal stuff, like module id etc.
      // 2. Update if it's not a CAPI for special handling in later stages

      if(FALSE == pVprx->reconfig_state.retain_amdb_handle)
      {
         pVprx->modules.handle_info_ptr = (adsp_amdb_module_handle_info_t*)qurt_elite_memory_malloc(sizeof(adsp_amdb_module_handle_info_t)*pVprx->modules.num_modules, QURT_ELITE_HEAP_DEFAULT);
         if (NULL == pVprx->modules.handle_info_ptr)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Failed to allocate memory for the module info structure for AMDB.");
            return ADSP_ENOMEMORY;
         }

         /*
          *  Create internal topology by reading MIDs from the list received from topo db and then
          *  add PP modules required by VPRx.
          */
         for (i = 0 ; i < (int32_t)pVprx->modules.num_modules-VPRX_NUM_PP_MODULES; i++)
         {
            pVprx->modules.modules_array[i].module_id = pVprx->modules.modules_list[i].module_id;
            pVprx->modules.modules_array[i].instance_id = pVprx->modules.modules_list[i].instance_id;            
            pVprx->modules.modules_array[i].module_index = i; //for backtracing array if needed
         }
         for (vprx_pp_mod_idx = 0; i < (int32_t)pVprx->modules.num_modules; i++,vprx_pp_mod_idx++)
         {
            pVprx->modules.modules_array[i].module_id = vprx_pp_modules[vprx_pp_mod_idx];
            pVprx->modules.modules_array[i].instance_id = 0;
            pVprx->modules.modules_array[i].module_index = i; //for backtracing array if needed
         }

         for (i = 0 ; i < (int32_t)pVprx->modules.num_modules; i++)
         {
            // Fill the structure to query the handles from AMDB
            pVprx->modules.handle_info_ptr[i].interface_type = CAPI_V2;
            pVprx->modules.handle_info_ptr[i].type = AMDB_MODULE_TYPE_GENERIC;
            pVprx->modules.handle_info_ptr[i].id1 = pVprx->modules.modules_array[i].module_id;
            pVprx->modules.handle_info_ptr[i].id2 = 0;
            pVprx->modules.handle_info_ptr[i].h.capi_v2_handle = NULL;
            pVprx->modules.handle_info_ptr[i].result = ADSP_EFAILED;
         }

         /*
          * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
          * happens using a thread pool using threads of very low priority. This can cause the current thread
          * to be blocked because of a low priority thread. If this is not desired, a callback function
          * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
          * thread can then handle other tasks in parallel.
          */
         adsp_amdb_get_modules_request(pVprx->modules.handle_info_ptr, pVprx->modules.num_modules, NULL, NULL);
      }

      voice_capi_module_t* curr_module_ptr = pVprx->modules.modules_array;

      for(i = 0; i < (int32_t)pVprx->modules.num_modules; i++)
      {
         curr_module_ptr->svc_ptr = pVprx;
         vprx_check_for_capi_module(curr_module_ptr);

         if(TRUE == curr_module_ptr->is_capi)
         {
            // populate function pointers from AMDB

            if(FALSE == pVprx->reconfig_state.retain_amdb_handle)
            {
               nResult = pVprx->modules.handle_info_ptr[i].result;
               if(ADSP_FAILED(nResult))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: failed to get AMDB handle for moduleId(0x%lx), session(0x%lx)",curr_module_ptr->module_id,pVprx->session.session_num);
                  return nResult;
               }

               if ((CAPI_V2 != pVprx->modules.handle_info_ptr[i].interface_type)&&(STUB != pVprx->modules.handle_info_ptr[i].interface_type))
               {
                  MSG_4(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: AMDB handle for moduleId(0x%lx), session(0x%lx) has type %d, when expected %d.",curr_module_ptr->module_id,pVprx->session.session_num, (int)pVprx->modules.handle_info_ptr[i].interface_type, (int)CAPI_V2);
                  return ADSP_EUNSUPPORTED;
               }
               if (STUB != pVprx->modules.handle_info_ptr[i].interface_type)
               {
                   //dbg MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: AMDB handle for moduleId(0x%lx) = (0x%p)",curr_module_ptr->module_id,pVprx->modules.handle_info_ptr[i].h.capi_v2_handle);
                   curr_module_ptr->amdb_handle_ptr = (void*)pVprx->modules.handle_info_ptr[i].h.capi_v2_handle;
               }
               else
               {
                   curr_module_ptr->is_virtual_stub = TRUE;
                   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VPRX moduleId(0x%lx) is virtually stubbed  session(0x%lx)",curr_module_ptr->module_id,pVprx->session.session_num);
               }
            }

            if(!curr_module_ptr->is_virtual_stub)
            {
               // get static props and the module memory
               pVprx->memory.module_size[i] = voice_capi_get_static_prop(curr_module_ptr);
            }

            //check max stack size
            if(curr_module_ptr->stack_size > max_module_stack_needed)
            {
               max_module_stack_needed = curr_module_ptr->stack_size;
            }
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: get size for unknown non-capi moduleid(0x%lx) is called, session(0x%lx)",curr_module_ptr->module_id, pVprx->session.session_num);
         }

         curr_module_ptr->media_format = VOICE_CAPI_MEDIA_FMT_PRE_EC;
         curr_module_ptr->port_info.port_num_info.num_input_ports = 1;
         curr_module_ptr->port_info.port_num_info.num_output_ports = 1;
         curr_module_ptr++;
      }
   }

   //add framework stack
   max_module_stack_needed += VPRX_SELF_STACK_IN_BYTES;
   if(0 == stack_size)
   {
      stack_size = max_module_stack_needed>VPRX_MIN_STACK_IN_BYTES? max_module_stack_needed:VPRX_MIN_STACK_IN_BYTES;
   }

   // Update stack size if zero (indicating dynamic topologies did not update it)
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vprx max module stack(%lu), stack needed(%lu), current stack(%lu)", max_module_stack_needed, stack_size, pVprx->session.stack_size);

   if( stack_size != pVprx->session.stack_size )
   {
      pVprx->session.stack_changed = TRUE;
      pVprx->session.stack_size = stack_size;
   }

   nTotalSize += vprx_get_port_info_size(pVprx);

   // accumulate module size
   for(i = 0; i < (int8_t)pVprx->modules.num_modules; i++)
   {
      nTotalSize             +=   voice_add_guard_band(pVprx->memory.module_size[i]);
   }

   pVprx->memory.pStartAddr = (int8_t *)qurt_elite_memory_malloc(nTotalSize, QURT_ELITE_HEAP_DEFAULT);

   if (NULL == pVprx->memory.pStartAddr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to allocate memory for VPRX modules!! session(%lx)",pVprx->session.session_num);
      return ADSP_ENOMEMORY;
   }

   memset( pVprx->memory.pStartAddr, 0, nTotalSize); // clear the memory before use

   pVprx->memory.nSize = nTotalSize; // size allocated
   pVprx->memory.pUsageAddr = pVprx->memory.pStartAddr; // Usage level pointer

   pVprx->io.input.circ_buf_ptr = pVprx->memory.pUsageAddr;
   pVprx->memory.pUsageAddr += (in_circ_samples << 1); // UsageAddr is a byte pointer
   pVprx->memory.pUsageAddr = (int8_t*)ROUNDTO8(pVprx->memory.pUsageAddr);

   vprx_set_port_info_mem(pVprx, &pVprx->memory.pUsageAddr);

   // 1. Populate Guard band memory locations allocated after each module and update in guard band pointers array.
   // 2. Initialize guard band memory with known value.
   nResult = voice_init_guard_band( (int8**)pVprx->memory.guard_band_ptrs, &(pVprx->memory.module_size[0]), (int16_t)pVprx->modules.num_modules, pVprx->memory.pUsageAddr);
   if ( ADSP_EOK != nResult )
   {
      return nResult; // Returning error because voice_init_guard_band() is called with NULL pointer
   }

   // populate init prop list for capi modules
   capi_v2_proplist_t init_props_list;

   // populate data buffer for media type
   voice_capi_populate_data_format_struct(&pVprx->modules.modules_array[0].port_info.input_port[0].media_fmt, &pVprx->io.input.mediatype);

   voice_capi_data_format_struct_t *curr_module_op_mf_ptr =  &pVprx->modules.modules_array[0].port_info.input_port[0].media_fmt;

   capi_v2_port_info_t port_info = {TRUE,TRUE,0};

   capi_v2_event_callback_info_t cb_info = { vprx_capi_v2_cb_func, NULL };
   capi_v2_heap_id_t heap_id = { (uint32_t)QURT_ELITE_HEAP_DEFAULT };
   capi_v2_port_num_info_t port_num = {1,1}; // populate these accordingly

   capi_v2_session_identifier_t sess_identifier;
   sess_identifier.service_id = (uint16_t)((ELITE_VOICEPROCRX_SVCID & 0xFFFF0000ul) >> 16);
   sess_identifier.session_id = pVprx->session.session_num;
   //MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Vprx session identifier svc %d, session %d", sess_identifier.service_id, sess_identifier.session_id);

   //Set up default init prop list. The order of the prop should not be changed
   capi_v2_prop_t init_props[]= {
         {     CAPI_V2_HEAP_ID,
               { reinterpret_cast<int8_t *>(&heap_id), sizeof(heap_id),  sizeof(heap_id) },
               port_info
         },
         {     CAPI_V2_EVENT_CALLBACK_INFO,
               { reinterpret_cast<int8_t *>(&cb_info), sizeof(cb_info),  sizeof(cb_info) },
               port_info
         },
         {     CAPI_V2_PORT_NUM_INFO,
               { reinterpret_cast<int8_t *>(&port_num), sizeof(port_num), sizeof(port_num) },
               port_info
         },
         {     CAPI_V2_INPUT_MEDIA_FORMAT,
               { reinterpret_cast<int8_t *>(curr_module_op_mf_ptr), sizeof(*curr_module_op_mf_ptr), sizeof(*curr_module_op_mf_ptr) },
               port_info
         },
         {     CAPI_V2_SESSION_IDENTIFIER,
               { reinterpret_cast<int8_t *>(&sess_identifier), sizeof(sess_identifier), sizeof(sess_identifier) },
               port_info
         }
   };

   // setup init prop lists
   init_props_list.props_num = VOICE_SIZE_OF_ARRAY(init_props);
   init_props_list.prop_ptr = init_props;
   uint32_t mf_idx = 3;

   uint32_t max_samp_rate = pVprx->io.input.mediatype.sample_rate;
   uint32_t max_chan_num = pVprx->io.input.mediatype.num_channels;

   {
      voice_capi_module_t* curr_module_ptr = pVprx->modules.modules_array;

      QURT_ELITE_ASSERT(CAPI_V2_INPUT_MEDIA_FORMAT == init_props[mf_idx].id);

      for(i = 0; i < (int32_t)pVprx->modules.num_modules; i++)
      {
         //check media format to use and update internal init prop accordingly
         if(TRUE == curr_module_ptr->is_capi)
         {
            cb_info.event_context = curr_module_ptr;
            curr_module_ptr->module_ptr = (capi_v2_t*)pVprx->memory.pUsageAddr;

            if (!curr_module_ptr->is_virtual_stub)
            {
               curr_module_ptr->is_enabled = TRUE;

               //update input media fmt
               curr_module_ptr->port_info.input_port[0].media_fmt = *(voice_capi_data_format_struct_t *)init_props[mf_idx].payload.data_ptr;

               //DBG print input media fmt
               //MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: module_idx(%ld). num_channel(%ld) samp_rate(%ld)",
               //      curr_module_ptr->module_index,curr_module_ptr->port_info.input_port[0].media_fmt.data_format.num_channels,curr_module_ptr->port_info.input_port[0].media_fmt.data_format.sampling_rate);

               capi_result = adsp_amdb_capi_v2_init_f((adsp_amdb_capi_v2_t*)curr_module_ptr->amdb_handle_ptr, curr_module_ptr->module_ptr, &init_props_list);

               if(CAPI_V2_EOK != capi_result)
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! Module id(0x%lx) failed to init with error %ld. proceeding", curr_module_ptr->module_id, capi_result);
                  curr_module_ptr->module_ptr = NULL; //make it NULL so that later it is not used
                  curr_module_ptr->is_enabled = FALSE;
                  curr_module_ptr->port_info.output_port[0].media_fmt = curr_module_ptr->port_info.input_port[0].media_fmt;
               }
               else
               {
                  //check if the module has raised output media fmt if not copy over input media fmt to output
                  if(!curr_module_ptr->output_media_fmt_prop_pending)
                  {

                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: module_id(%lx) did not raise output media fmt. copying input fmt",curr_module_ptr->module_id);
                     curr_module_ptr->port_info.output_port[0].media_fmt = curr_module_ptr->port_info.input_port[0].media_fmt;
                  }

                  //set the flag to false
                  curr_module_ptr->output_media_fmt_prop_pending = FALSE;

                  //update media fmt being propagated
                  init_props[mf_idx].payload.data_ptr = (int8_t *)&curr_module_ptr->port_info.output_port[0].media_fmt;

                  //calculate max sampling rate and number of channels
                  max_samp_rate = VPRX_MAX32(max_samp_rate,curr_module_ptr->port_info.output_port[0].media_fmt.data_format.sampling_rate);
                  max_chan_num = VPRX_MAX32(max_chan_num,curr_module_ptr->port_info.output_port[0].media_fmt.data_format.num_channels);

                  if(curr_module_ptr->fwk_extn_ptr)
                  {
                     nResult = vprx_handle_fwk_extension(pVprx,curr_module_ptr);
                     if(ADSP_FAILED(nResult))
                     {
                        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: failed to handle fwk extn, session(%#lx)",pVprx->session.session_num);
                        return nResult;
                     }
                  }

                  //send operating mode for SS capi
                  if(curr_module_ptr->module_id == VOICE_MODULE_SAMPLE_SLIP)
                  {
                     vprx_send_ss_op_mode(pVprx,curr_module_ptr);
                  }
               }
            }
            else
            {
               curr_module_ptr->is_enabled = FALSE;         // disabling enable flag for stubbed module
               curr_module_ptr->module_ptr = NULL;
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: bypassing init for stubbed module (0x%lx), session(%#lx)",curr_module_ptr->module_id,pVprx->session.session_num);
            }
            voice_print_media_fmt(curr_module_ptr);
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: get size for unknown non-capi moduleid(0x%lx) is called, session(0x%lx)",curr_module_ptr->module_id, pVprx->session.session_num);
         }

         // take care of pUsageAddr
         pVprx->memory.pUsageAddr += voice_add_guard_band(curr_module_ptr->mem_size_allocated);
         curr_module_ptr++;
      }

      //go back to last module and update output circular buffer size
      curr_module_ptr--;

      pVprx->io.output.mediatype.sample_rate = curr_module_ptr->port_info.output_port[0].media_fmt.data_format.sampling_rate;
      pVprx->io.output.mediatype.num_channels = curr_module_ptr->port_info.output_port[0].media_fmt.data_format.num_channels;

   }

   nResult = vprx_out_circ_buf_mem_alloc(pVprx);
   if(ADSP_FAILED(nResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to allocate memory for out circ buf, session(%#lx)",pVprx->session.session_num);
      return ADSP_ENOMEMORY;
   }

   nResult = vprx_allocate_proc_buf_mem(pVprx,max_samp_rate,max_chan_num);
   if(ADSP_FAILED(nResult))
   {
      return ADSP_ENOMEMORY;
   }

   // Check guard band for memory corruption
   nResult = voice_check_guard_band((int8**)pVprx->memory.guard_band_ptrs, (int16_t)pVprx->modules.num_modules, RX, pVprx->session.session_num);
   if ( ADSP_EOK != nResult )
   {
      return nResult; // returning error as corruption is detected in check_guard_band()
   }

   // Sanity check to see the allocated memory was enough
   if ((uint32_t)(pVprx->memory.pUsageAddr - pVprx->memory.pStartAddr) <= pVprx->memory.nSize)
   {
      nResult = ADSP_EOK;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Error allocating memory, result %d session(%lx)",nResult,pVprx->session.session_num);
      nResult = ADSP_ENOMEMORY;
      return nResult;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_allocate_mem end nResult=%x session(%x)",nResult,(int)pVprx->session.session_num);
   return nResult;
}

/*
 * This function frees up memory allocated for modules (Calls any end routines as required)
 */
ADSPResult vprx_allocate_mem_free(vprx_t* pVprx)
{
   if (pVprx->memory.pStartAddr)
   {
      qurt_elite_memory_free(pVprx->memory.pStartAddr);
      pVprx->memory.pStartAddr = NULL;
   }
   return ADSP_EOK;
}

/*
 * This function calls the end routines of modules
 */
void vprx_modules_end(vprx_t* pVprx)
{
   // Check guard band for memory corruption
   voice_check_guard_band((int8_t**)pVprx->memory.guard_band_ptrs, (int16_t)pVprx->modules.num_modules, RX, pVprx->session.session_num);
   uint32_t temp_modid,temp_mod_index;
   uint16_t temp_instance_id;
   bool_t temp_stub_info;
   void *temp_handle;

   voice_capi_module_t* curr_module_ptr = pVprx->modules.modules_array;
   for(uint32_t i=0; i < pVprx->modules.num_modules ; i++)
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

         if(TRUE == pVprx->reconfig_state.retain_amdb_handle)
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

   if ((NULL != pVprx->modules.handle_info_ptr) && (FALSE == pVprx->reconfig_state.retain_amdb_handle))
   {
      adsp_amdb_release_handles(pVprx->modules.handle_info_ptr, pVprx->modules.num_modules);
      qurt_elite_memory_free(pVprx->modules.handle_info_ptr);
      pVprx->modules.handle_info_ptr = NULL;
   }

   //free output circular buffer memory
   vprx_out_circ_buf_mem_free(pVprx);

   //free process buf memory
   vprx_free_proc_buf_mem(pVprx);
}

/*
 * This function initializes all the modules with fresh calibration at the start of every call
 */
ADSPResult vprx_modules_full_init(vprx_t* pVprx)
{
   capi_v2_err_t capi_result=CAPI_V2_EOK;
   capi_v2_prop_t reset_prop[] = {
      { CAPI_V2_ALGORITHMIC_RESET, { NULL, 0, 0 }, {FALSE, FALSE, 0} },
   };
   capi_v2_proplist_t reset_prop_list = {sizeof(reset_prop)/sizeof(capi_v2_prop_t), reset_prop};

   voice_capi_module_t *curr_module_ptr = pVprx->modules.modules_array;

   {
      for(uint32_t i=0; i<pVprx->modules.num_modules;i++)
      {
         if((curr_module_ptr->is_capi)&& (!curr_module_ptr->is_virtual_stub))
         {
            //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Issuing ALGO_RESET for module ID(%lu), session (%lx)",pVprx->modules.modules_array[i].module_id, pVprx->session.session_num);
            if(curr_module_ptr->module_ptr)
            {
               capi_result = curr_module_ptr->module_ptr->vtbl_ptr->set_properties(curr_module_ptr->module_ptr, &reset_prop_list);
            }
            else
            {
               capi_result = CAPI_V2_EUNSUPPORTED;
            }
            if(CAPI_V2_FAILED(capi_result))
            {
               MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: ALGO_RESET failed for moduleId(%lx),module_ptr(%p),result(0x%lx) session (%lx)",
                     curr_module_ptr->module_id,curr_module_ptr->module_ptr,capi_result,pVprx->session.session_num);
            }
         }
         curr_module_ptr++;
      }
   }

   return ADSP_EOK;
}

static ADSPResult vprx_ss_send_inst_drift(vprx_t *pVprx, voice_capi_module_t *curr_module_ptr)
{
   int32_t drift_in_us;
   instantaneous_drift_t inst_drift = { &drift_in_us };

   capi_v2_buf_t param_data_buf = { reinterpret_cast<int8_t *>(&inst_drift),
                                    sizeof(instantaneous_drift_t),
                                    sizeof(instantaneous_drift_t) } ;

   capi_v2_port_info_t port_info = {FALSE, FALSE, 0};

   vprx_cal_drift(pVprx,&drift_in_us); // accumulate drift info w.r.t av/hp timer

   if (curr_module_ptr->module_ptr)
   {
      capi_v2_err_t result = curr_module_ptr->module_ptr->vtbl_ptr->set_param(curr_module_ptr->module_ptr,
            SS_PARAM_INST_DRIFT,&port_info,&param_data_buf);
      if(CAPI_V2_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to send inst drift to SS capi. result(%lx)",result);
      }
   }

   return ADSP_EOK;
}

/*
 * This function calls the process functions of all the modules.
 */
ADSPResult vprx_modules_process(vprx_t* pVprx)
{
   int16_t *vprx_final_out_ptr[VPRX_MAX_CHAN] = {(int16_t*)pVprx->io.proc_buf_ptr[0], NULL};
   ADSPResult nResult = ADSP_EOK;
   int16_t proc_buf_size = ((FRAME_SAMPLES + DMA_SAMPLES) * pVprx->session.in_nb_sampling_rate_factor)<< 1;
   uint32_t i;

   voice_capi_module_t *curr_module_ptr = pVprx->modules.modules_array;

   voice_circbuf_read(&(pVprx->io.input.circ_struct), (int8_t*)(pVprx->io.proc_buf_ptr[0]), pVprx->io.min_process_frame_samples,proc_buf_size);
   /*
    * capi takes array of pointers of capi_v2_stream_data_t size.
    * stream_data_ptrs is a 2D array of pointers with each row correpsonding to 1D array of pointers.
    * each capi stores input and output buffer indices. And these indices point to proper array
    * of stream data pointers.
    */
   capi_v2_stream_data_t *stream_data_ptrs[MAX_PROC_BUFS][1] = {
         {&pVprx->io_stream_data[PROC_BUF_1]},
         {&pVprx->io_stream_data[PROC_BUF_2]}
   };

   pVprx->io_stream_data[PROC_BUF_1].buf_ptr[0].actual_data_len = ((pVprx->io.min_process_frame_samples) << 1);
   pVprx->io_stream_data[PROC_BUF_2].buf_ptr[0].actual_data_len = ((pVprx->io.min_process_frame_samples) << 1);

   pVprx->modules.is_processing = TRUE;

   {
      for( i=0; i<pVprx->modules.num_modules; i++)
      {
         //for SS module, send inst drift
         if(VOICE_MODULE_SAMPLE_SLIP == curr_module_ptr->module_id)
         {
            vprx_ss_send_inst_drift(pVprx,curr_module_ptr);
         }

         if(curr_module_ptr->is_enabled)
         {
            if(curr_module_ptr->is_capi)
            {
               /*
                * if a module has same input and output buf_idex, then it should be in_place. so will have same number of channels.
                * Module which converts mono input to stereo output will not be in_place. Hence will have different input and output buf_indeces.
                * And updating the buf_num will update for different buffers
                * */
               stream_data_ptrs[curr_module_ptr->input_buf_index][0]->bufs_num = curr_module_ptr->port_info.input_port[0].media_fmt.data_format.num_channels;
               stream_data_ptrs[curr_module_ptr->output_buf_index][0]->bufs_num = curr_module_ptr->port_info.output_port[0].media_fmt.data_format.num_channels;

               curr_module_ptr->module_ptr->vtbl_ptr->process(curr_module_ptr->module_ptr,stream_data_ptrs[curr_module_ptr->input_buf_index],
                     stream_data_ptrs[curr_module_ptr->output_buf_index]);
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: process() for unknown non-capi moduleid(0x%lx) is called, session(0x%lx)",curr_module_ptr->module_id, pVprx->session.session_num);
            }
         }
         else
         {
            // for virtual stub modules is_enabled is false and hence calling process is bypassed
            if( curr_module_ptr->input_buf_index != curr_module_ptr->output_buf_index )
            {
               MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx mod_id(0x%lx) is not enabled but has different io pointers. input_idx(%d), output_idx(%d)",
                     curr_module_ptr->module_id,curr_module_ptr->input_buf_index,curr_module_ptr->output_buf_index);
            }
         }

#if defined(LOG_VOICEPROC_DATA)
         if(curr_module_ptr->is_enabled)
         {
            MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx ses(%d), mod_idx(%d), in_samples, (%d), out_samples (%d), in_strm_idx(%d), out_strm_idx(%d)",
                  pVprx->session.session_num,curr_module_ptr->module_index,stream_data_ptrs[curr_module_ptr->input_buf_index][0]->buf_ptr[0].actual_data_len>>1,
                  stream_data_ptrs[curr_module_ptr->output_buf_index][0]->buf_ptr[0].actual_data_len>>1,curr_module_ptr->input_buf_index,curr_module_ptr->output_buf_index);
#if defined(SIM)
            {
               char buffer[100]; // The filename buffer.
               // Put output of ith module in ith file
               snprintf(buffer, sizeof(char) * 100, "vprx_session_%x_samp_rate_%d_module_%d_output.raw",
                     pVprx->session.session_num,curr_module_ptr->port_info.output_port[0].media_fmt.data_format.sampling_rate,curr_module_ptr->module_index);
               FILE *fp;
               int16_t *pIn = (int16_t*)stream_data_ptrs[curr_module_ptr->output_buf_index][0]->buf_ptr->data_ptr;
               fp = fopen(buffer,"ab"); // in append mode
               fwrite(pIn,sizeof(char),stream_data_ptrs[curr_module_ptr->output_buf_index][0]->buf_ptr->actual_data_len,fp);
               fclose(fp);
            }
#endif
         }
#endif
         curr_module_ptr++;
      }
      curr_module_ptr--; //go back to last module in processing chain

      QURT_ELITE_ASSERT(pVprx->io.output.mediatype.num_channels == curr_module_ptr->port_info.output_port[0].media_fmt.data_format.num_channels);

      //update the output of the chain with the last module's output buffer pointer. if the last module is ECNS then update with the primary output port pointer
      for(i=0; i < pVprx->io.output.mediatype.num_channels; i++)
      {
         vprx_final_out_ptr[i] = (int16_t*)stream_data_ptrs[curr_module_ptr->output_buf_index][0]->buf_ptr[i].data_ptr;
      }

   }

   pVprx->io.output.buf_samples = stream_data_ptrs[curr_module_ptr->output_buf_index][0]->buf_ptr[0].actual_data_len>>1;
   //MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_modules_process out samples (%d) session(%x) vfr mode(%x)",pVprx->io.output.buf_samples, pVprx->session.session_num,pVprx->session.vfr_mode);

   nResult = voice_multi_ch_circ_buf_write(&(pVprx->io.output.circ_struct), ( int8_t *(*)[MAX_CIRCBUF_CHANNELS])&vprx_final_out_ptr,pVprx->io.output.buf_samples);
   if (ADSP_FAILED(nResult))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: output circular buffer write failure (%d)  unread samples (%ld) ",nResult,pVprx->io.output.circ_struct.circ_buf[0].unread_samples);
   }

   pVprx->modules.is_processing = FALSE;

#if defined(__qdsp6__) && !defined(SIM)

   int8_t *bufptr[4] = { (int8_t *) (vprx_final_out_ptr[0]), (int8_t *) (vprx_final_out_ptr[1]), NULL, NULL };
   uint32_t sampling_rate_log_id;
   uint16_t log_code = VOICE_LOG_DATA_FORMAT_PCM_MONO;
   //update size of log_code for stereo output
   if(pVprx->io.output.mediatype.num_channels == 2)
   {
      log_code = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
   }
   sampling_rate_log_id = voice_get_sampling_rate_log_id(pVprx->io.output.mediatype.sample_rate);
   voice_log_buffer(  bufptr,
         VOICE_LOG_TAP_POINT_VPRX_OUT,
         (sampling_rate_log_id << 3) | pVprx->session.session_num,
         qurt_elite_timer_get_time(),
         log_code,
         pVprx->io.output.mediatype.sample_rate,
         pVprx->io.output.buf_samples << 1,
         NULL);

#endif

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_modules_process end session(%x)",pVprx->session.session_num);
   return ADSP_EOK;
}

/*
 * This function iterates through the modules to set configuration parameters
 */
ADSPResult vprx_modules_set_param(vprx_t* pVprx, uint32_t payload_address, uint32_t payload_size, uint32_t cal_param_id_ver )
{
   uint32_t byteSizeCounter=0;
   int8_t *pCalibrationDataPayLoad;
   ADSPResult nLocalResult = ADSP_EUNSUPPORTED, nAggregateResult = ADSP_EOK;

   // Iterate through the entire payload size and copy all updated parameters
   uint32_t module_id, param_id;
   uint16_t instance_id, param_size,header_size;

   do
   {
      //update module information based on cal version
      if(ELITEMSG_PARAM_ID_CAL == cal_param_id_ver)
      {
         voice_param_data_t *param_data_payload_ptr = (voice_param_data_t *)payload_address;
         module_id = param_data_payload_ptr->module_id;
         instance_id = 0;
         param_id = param_data_payload_ptr->param_id;
         param_size = param_data_payload_ptr->param_size;
         header_size = sizeof(voice_param_data_t);
      }
      else if(ELITEMSG_PARAM_ID_CAL_V2 == cal_param_id_ver)
      {
         voice_param_data_v2_t *param_data_payload_ptr = (voice_param_data_v2_t *)payload_address;
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

      pCalibrationDataPayLoad = (int8_t *)payload_address + header_size;

      // check to make sure size is a multiple of 4. If not, stop calibrating
      if( (param_size & 0x00000003L) != 0 )
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Vprx received bad param size, mod %lx, instance %lx, param %lx, size %ld!!", module_id, instance_id, param_id, param_size);
         nAggregateResult = ADSP_EBADPARAM;
         break;
      }

      nLocalResult = vprx_modules_set_param_int(pVprx, module_id, instance_id, param_id, pCalibrationDataPayLoad, param_size);

      if (ADSP_EBADPARAM == nLocalResult)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vprx SetParam bailing!!!");
         nAggregateResult = ADSP_EBADPARAM;
         break; // in case of any error dont go forward with parsing
      }
      else if(ADSP_FAILED(nLocalResult) && ADSP_EUNSUPPORTED != nLocalResult)
      {
         nAggregateResult = ADSP_EFAILED;
      }
      byteSizeCounter += (header_size + param_size);
      payload_address += (header_size + param_size);
      //dbg: MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx SetParam addr(%#x), byte_size(%#x)", payload_address, byteSizeCounter);
      //dbg: MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx received mod %d, param %d, size %d!!", (int)ptr->module_id, (int)ptr->param_id, (int)ptr->param_size);
   } while(byteSizeCounter < payload_size);

   // Check guard band for memory corruption
   nAggregateResult = nAggregateResult | (voice_check_guard_band((int8_t**)pVprx->memory.guard_band_ptrs, (int16_t)pVprx->modules.num_modules, RX, pVprx->session.session_num));

   return nAggregateResult;

}

ADSPResult vprx_modules_set_param_int(vprx_t* pVprx, uint32_t module_id, uint16_t instance_id, uint32_t param_id, void* payload_address_in, uint32_t param_size )
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   int8_t* payload_address = (int8_t*)payload_address_in;

   // populate buffer to issue set param to capi modules
   capi_v2_buf_t param_data_buf;
   capi_v2_port_info_t port_info;
   port_info.is_valid = FALSE;
   param_data_buf.data_ptr = payload_address;
   param_data_buf.actual_data_len = param_data_buf.max_data_len = param_size;

   //MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx internal set param, mod id %lx, param id %lx, payload add %p, size %lu", module_id, param_id, payload_address_in, param_size);
   if(VOICEPROC_MODULE_TX == module_id) // listen for Tx module so it's triggered off a single module/param combo.  User sees as putting Tx in loopback
   {        /* handle loopback param */
      if( VOICE_PARAM_LOOPBACK_ENABLE == param_id)
      {
         int16_t nEnableFlag = *((int16_t*)payload_address);
         if (sizeof(int32_t) != param_size)
         {
            result = ADSP_EBADPARAM;
         }
         else
         {
            if( TRUE == nEnableFlag)
            {
               pVprx->loopback_enable_flag        = TRUE;
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_set_param_cmd VpTx->VpRx Loopback Enable session(%x)",(int)pVprx->session.session_num);
            }
            else
            {
               pVprx->loopback_enable_flag        = FALSE;
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_set_param_cmd vprx->VpRx Loopback Disable session(%x)",(int)pVprx->session.session_num);
            }
            result = ADSP_EOK;
         }
      }
      else
      {
         result = ADSP_EUNSUPPORTED;
      }
   }
   else
   {
      voice_capi_module_t* curr_module_ptr = NULL;
      result = voice_topo_get_module_info( pVprx->modules.modules_array, module_id, instance_id, pVprx->modules.num_modules,&curr_module_ptr);
      if(ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: set_param(): Unsupported set vprx module(%#lx), instance(%lx) - Ignoring", module_id,instance_id);
         return ADSP_EUNSUPPORTED;
      }
      else
      {
         if(curr_module_ptr->is_capi)
         {
            if(curr_module_ptr->is_virtual_stub)
            {
               capi_result = CAPI_V2_EOK;
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: set_param(): for virtualy stubbed  vprx module(%#lx)", module_id);
            }
            else if(curr_module_ptr->module_ptr)
            {
               capi_result = curr_module_ptr->module_ptr->vtbl_ptr->set_param(curr_module_ptr->module_ptr, param_id, &port_info, &param_data_buf);
            }
            else
            {
               //updating result with EUNSUPPORTED to continue set param for other modules
               capi_result = CAPI_V2_EUNSUPPORTED;
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: set_param(): module_ptr is null for module_id(%#lx), Ignoring", module_id);
            }
            result = capi_v2_err_to_adsp_result(capi_result);
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: set_param(): set_param is received for non-capi module(0x%lx)", module_id);
            return ADSP_EUNSUPPORTED;
         }
      }
   }
   //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx internal set param returning with status %ld", result);
   return result;
}

/*
 * This function gets the configuration parameters of a given module ID
 */
ADSPResult vprx_modules_get_param(vprx_t* pVprx, void *cal_param_payload, uint32_t cal_param_id_ver, uint16_t* param_size)
{
   // TODO: put the following in a big function which includes all the mods and parameters function
   ADSPResult nLocalResult    = ADSP_EUNSUPPORTED;

   uint32_t module_id, param_id,*param_size_ptr;
   uint16_t instance_id,header_size;

   //update module information based on cal version
   if(ELITEMSG_PARAM_ID_CAL == cal_param_id_ver)
   {
      voice_param_data_t *param_data_payload_ptr = (voice_param_data_t *)cal_param_payload;
      module_id = param_data_payload_ptr->module_id;
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
            cal_param_id_ver, pVprx->session.session_num);
      return ADSP_EBADPARAM;
   }

   // param_max_size sent from client is copied over to param_size from static service.
   // however this param_size corresponds to the actual size of the parameter, plus the header
   // thus the header size must be subtracted from the size that is available to write the parameter
   // data to.
   int32_t nBufferSize = *param_size_ptr - header_size;

   int8_t* pCalibrationDataPayLoad = (int8_t *)cal_param_payload + header_size;

   // Populate buffer to issue get param to capi modules
   capi_v2_buf_t param_data_buf;
   capi_v2_port_info_t port_info;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   port_info.is_valid = FALSE;
   param_data_buf.data_ptr = pCalibrationDataPayLoad;
   param_data_buf.max_data_len = nBufferSize;
   param_data_buf.actual_data_len = 0;

   voice_capi_module_t* curr_module_ptr = NULL;

   nLocalResult = voice_topo_get_module_info( pVprx->modules.modules_array, module_id, instance_id, pVprx->modules.num_modules,&curr_module_ptr);
   if(ADSP_FAILED(nLocalResult))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: get_param(): Unsupported get vprx module(%#lx) instance(%lx). Ignoring",module_id,instance_id);
      nLocalResult = ADSP_EUNSUPPORTED;
   }
   else
   {
      if(curr_module_ptr->is_capi)
      {
         if(curr_module_ptr->is_virtual_stub)
         {
            capi_result = CAPI_V2_EFAILED;
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: get_param(): for  virtually stubbed  vprx module(%#lx)", module_id);
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

         MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Rx get param done, mod id(0x%lx), instance_id(%lx), param id (0x%lx), result(0x%x) session(0x%lx)\n",
                    module_id, instance_id, param_id, (int)nLocalResult, pVprx->session.session_num);

      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: get_param(): get_param is received for non-capi module(0x%lx)", module_id);
         return ADSP_EUNSUPPORTED;
      }
   }

   if(ADSP_SUCCEEDED(nLocalResult))
   {
      // Return size
      *param_size = *param_size_ptr ;
   }
   else
   {
      *param_size = 0;
   }

   return nLocalResult;
}

ADSPResult vprx_aggregate_modules_kpps(vprx_t* pVprx, uint32_t* kpps_changed)
{
   uint32_t kpps = 0;
   uint32_t modules_kpps=0;

   voice_capi_module_t* curr_module_ptr = pVprx->modules.modules_array;

   for(uint32_t i=0; i<pVprx->modules.num_modules;i++)
   {
      if(curr_module_ptr->is_capi)
      {
         //account for KPPS only if module is enabled.
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

   modules_kpps += 1000; // adding extra 1000 kpps to ceil the number to mpps

   // Retaining behavior as it is as of today. That is to report an increase in KPPS only.
   // Once the VOICE_CMD_SET_TIMING_PARAMS commands are removed,
   // this can be modified to (pVprx->timing.modules_kpps != modules_kpps)
   if (pVprx->timing.modules_kpps >= modules_kpps)
   {
      *kpps_changed = FALSE;
   }
   else
   {
      *kpps_changed = TRUE;
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx KPPS changed from (%lu) to (%lu), session(%x)", pVprx->timing.modules_kpps, modules_kpps, (int)pVprx->session.session_num);
   }

   // Update state
   pVprx->timing.modules_kpps = modules_kpps;

   return ADSP_EOK;
}

ADSPResult vprx_aggregate_modules_delay(vprx_t* pVprx)
{
   uint32_t delay = 0;
   uint32_t modules_delay=0;

   voice_capi_module_t* curr_module_ptr = pVprx->modules.modules_array;

   for(uint32_t i=0; i<pVprx->modules.num_modules;i++)
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
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: get_kpps(): Unknown non-capi module ModId(0x%lx)",curr_module_ptr->module_id);
      }
      curr_module_ptr++;
   }

   modules_delay += (PRE_BUF * 1000); // prebuffering

   // Update state
   pVprx->timing.modules_delay = modules_delay;

   return ADSP_EOK;
}

bool_t vprx_is_media_type_valid(voice_capi_data_format_struct_t *media_fmt)
{
   if( (CAPI_V2_FIXED_POINT != media_fmt->media_format.format_header.data_format)
         ||  (VPRX_MAX_CHAN < media_fmt->data_format.num_channels)
         ||  (16 != media_fmt->data_format.bits_per_sample)
         ||  ((VOICE_NB_SAMPLING_RATE != media_fmt->data_format.sampling_rate) &&
              (VOICE_WB_SAMPLING_RATE != media_fmt->data_format.sampling_rate) &&
              (VOICE_SWB_SAMPLING_RATE != media_fmt->data_format.sampling_rate) &&
              (VOICE_FB_SAMPLING_RATE != media_fmt->data_format.sampling_rate) )
              )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: validate_media_fmt: %lu - Invalid format or bits_per_sample-%lu or samping_rate-%lu or num_channel-%lu",
            media_fmt->media_format.format_header.data_format,media_fmt->data_format.bits_per_sample,media_fmt->data_format.sampling_rate,
            media_fmt->data_format.num_channels);
      return FALSE;
   }
   return TRUE;
}

capi_v2_err_t vprx_capi_v2_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
   capi_v2_err_t result = CAPI_V2_EOK;
   voice_capi_module_t* module_ptr = (voice_capi_module_t*)context_ptr;
   vprx_t *pVprx = (vprx_t*)module_ptr->svc_ptr;
   if(NULL == context_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Received bad context pointer in vprx callback");
      return CAPI_V2_EFAILED;
   }
   //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx event callback invoked, event_id(%d), module(0x%lx)", (int)id, module_ptr->module_id);
   switch(id)
   {
      case CAPI_V2_EVENT_KPPS:
      {
         if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_KPPS_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Module(0x%#lx) instance(0x%lx) provided insufficient size(%lu) for KPPS event",
                  module_ptr->module_id,module_ptr->instance_id, event_info_ptr->payload.actual_data_len);
            return CAPI_V2_ENEEDMORE;
         }

         capi_v2_event_KPPS_t* kpps_ptr = (capi_v2_event_KPPS_t* )event_info_ptr->payload.data_ptr;
         module_ptr->kpps = kpps_ptr->KPPS;
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_capi_v2_cb_func: module(%#lx) instance(%#lx) updated KPPS(%d)",
               module_ptr->module_id,module_ptr->instance_id,module_ptr->kpps);
         break;
      }
      case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
      {
         if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_algorithmic_delay_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Module(0x%#lx) instance(0x%lx) provided insufficient size(%lu) for delay event",
                  module_ptr->module_id,module_ptr->instance_id, event_info_ptr->payload.actual_data_len);
            return CAPI_V2_ENEEDMORE;
         }

         capi_v2_event_algorithmic_delay_t* delay_ptr = (capi_v2_event_algorithmic_delay_t* )event_info_ptr->payload.data_ptr;
         module_ptr->delay = delay_ptr->delay_in_us;
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_capi_v2_cb_func: module(%#lx) instance(%#lx) updated delay(%d)",
               module_ptr->module_id,module_ptr->instance_id,module_ptr->delay);
         break;
      }
      case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
      {
         if(event_info_ptr->payload.actual_data_len < sizeof(voice_capi_data_format_struct_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Module(0x%#lx) instance(0x%lx) provided insufficient size(%lu) for output media fmt event",
                  module_ptr->module_id,module_ptr->instance_id, event_info_ptr->payload.actual_data_len);
            return CAPI_V2_ENEEDMORE;
         }

         if(pVprx->modules.is_processing)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Module(%#lx) instance(0x%lx) raised output media fmt. Updating output media fmt during process is not supported",
                  module_ptr->module_id,module_ptr->instance_id);
            return CAPI_V2_EBADPARAM;
         }

         voice_capi_data_format_struct_t *media_fmt = reinterpret_cast<voice_capi_data_format_struct_t*>(event_info_ptr->payload.data_ptr);

         if(!vprx_is_media_type_valid(media_fmt))
         {
            return CAPI_V2_EBADPARAM;
         }

         if(is_capi_media_fmt_same(media_fmt,&module_ptr->port_info.output_port[0].media_fmt))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: module(%#lx) instance(0x%lx) has raised same output media fmt. Ignoring",
                  module_ptr->module_id,module_ptr->instance_id);
            return CAPI_V2_EOK;
         }

         //update output media fmt and set the flag
         memscpy(&module_ptr->port_info.output_port[0].media_fmt,sizeof(voice_capi_data_format_struct_t),media_fmt,sizeof(voice_capi_data_format_struct_t));
         module_ptr->output_media_fmt_prop_pending = TRUE;


         //if VPRx is not propagating media fmt, then propagate it if module is enabled
         if( (FALSE == pVprx->modules.is_prop_media_fmt) && (pVprx->modules.module_list_locked))
         {
            if(module_ptr->is_enabled)
            {
               MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: module(%#lx) instance(0x%lx) has raised media fmt. num_channel(%ld), samp_rate(%ld)",
                     module_ptr->module_id, module_ptr->instance_id,media_fmt->data_format.num_channels, media_fmt->data_format.sampling_rate);

               //set the module list locked to false
               pVprx->modules.is_prop_media_fmt = TRUE;
               vprx_propagate_media_fmt(pVprx,module_ptr);
               pVprx->modules.is_prop_media_fmt = FALSE;

               //some of the modules might get disabled after media format propagation. re-assign capiv2 stream bufs
               vprx_update_io_stream_data(pVprx,module_ptr->module_index);
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: module(%#lx) instance(0x%lx) has raised output media fmt but is disabled. Will be propagated when enabled",
                     module_ptr->module_id,module_ptr->instance_id);
            }
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Module(0x%lx) instance(0x%lx) raised output media type. Already propagating media fmt",module_ptr->module_id,module_ptr->instance_id);
         }
         break;
      }
      case CAPI_V2_EVENT_PROCESS_STATE:
      {
         uint32_t prev_is_enabled = module_ptr->is_enabled;
         module_ptr->is_enabled = ((capi_v2_event_process_state_t *)(event_info_ptr->payload.data_ptr))->is_enabled;

         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Module(0x%lx) instance(0x%lx) updated process state prev(%lu), new state(%lu)",
               module_ptr->module_id,module_ptr->instance_id,prev_is_enabled, module_ptr->is_enabled);


         //If VPRx is not propagating media fmt, then start propagating if the module state has changed.
         if( (FALSE == pVprx->modules.is_prop_media_fmt) && (pVprx->modules.module_list_locked))
         {
            if(prev_is_enabled != module_ptr->is_enabled)
            {
               bool_t media_fmt_updated = FALSE;
               //when process state changes, propagate media fmt only if module input and output are not same
               if(!is_capi_media_fmt_same(&module_ptr->port_info.input_port[0].media_fmt,&module_ptr->port_info.output_port[0].media_fmt))
               {
                  //check if VPRx is processing data
                  if(pVprx->modules.is_processing)
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Module(0x%lx) changes media fmt and event raised in process function. Rejecting.",
                           module_ptr->module_id);
                     module_ptr->is_enabled = prev_is_enabled; //restore process state
                     return CAPI_V2_EBADPARAM;
                  }
                  media_fmt_updated = TRUE;
                  pVprx->modules.is_prop_media_fmt = TRUE;
                  vprx_propagate_media_fmt(pVprx,module_ptr);
                  pVprx->modules.is_prop_media_fmt = FALSE;
               }

               //update capi stream pointers only if media fmt is propagated or module is not inplace
               if( (TRUE == media_fmt_updated) || (FALSE == module_ptr->is_in_place) )
               {
                  vprx_update_io_stream_data(pVprx,module_ptr->module_index);
               }
            }
         }
         break;
      }
      case CAPI_V2_EVENT_GET_LIBRARY_INSTANCE:
      {
         capi_v2_event_get_library_instance_t* get_library_instance = (capi_v2_event_get_library_instance_t*)event_info_ptr->payload.data_ptr;;

         result = capi_v2_library_factory_get_instance(get_library_instance->id,&get_library_instance->ptr);
         if(CAPI_V2_FAILED(result))
         {
             MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VPRx failed to get library instance with lib_id(0x%x), payload_ptr(0x%p)",get_library_instance->id,get_library_instance->ptr);
             return result;
         }
         break;
      }
      case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
      {
         if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_data_to_dsp_service_t))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vprx_capi_v2_cb_func() - Module provided insufficient size for CAPI_V2_EVENT_DATA_TO_DSP_SERVICE event"
                  "Required: %d bytes, provided: %lu bytes.",
                  sizeof(capi_v2_event_data_to_dsp_service_t),
                  event_info_ptr->payload.actual_data_len);
            return CAPI_V2_ENOMEMORY;
         }
         capi_v2_event_data_to_dsp_service_t* data_ptr = (capi_v2_event_data_to_dsp_service_t*)event_info_ptr->payload.data_ptr;

         if ( FWK_EXTN_EVENT_ID_HPCM_HOST_BUF_AVAILABLE == data_ptr->param_id )
         {
            //vprx_t  *pVprx= (vprx_t*)(module_ptr->svc_ptr);
            vprx_send_hpcm_buf_available_evt(pVprx, data_ptr->payload.data_ptr);
         }
         break;
      }
      case CAPI_V2_EVENT_BANDWIDTH:
      {
         //no handling for now
         break;
      }
      default:
      {
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Unsupported event %d raised by module(%#lx) instance(%#lx)",
               (int)id,module_ptr->module_id,module_ptr->instance_id);
         return CAPI_V2_EBADPARAM;
      }
   }
   return result;
}

int32_t vprx_send_hpcm_buf_available_evt(vprx_t* pVprx, void* data_ptr)
{
   int32_t rc = ADSP_EOK;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VOICE_EVT_HOST_BUF_AVAILABLE_V2 sent to client, session(%lx)",pVprx->session.session_num);

   rc = elite_apr_if_alloc_send_event( pVprx->modules.apr_info_hpcm.apr_handle,
         pVprx->modules.apr_info_hpcm.self_addr,
         pVprx->modules.apr_info_hpcm.self_port,
         pVprx->modules.apr_info_hpcm.client_addr,
         pVprx->modules.apr_info_hpcm.client_port,
         0, /* token */
         VOICE_EVT_HOST_BUF_AVAILABLE_V2,
         data_ptr,
         sizeof(voice_evt_notify_host_pcm_buf_v2_t));

   if (ADSP_FAILED(rc))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR request for VOICE_EVT_HOST_BUF_AVAILABLE_V2  0x%08lx :session(%lx)",rc,pVprx->session.session_num);
   }
   return rc;
}
