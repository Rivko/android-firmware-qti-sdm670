/*==========================================================================
ELite Source File

This file implements the COPP topology.

Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/13/16   rishabht     Created file.
=========================================================================== */

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "audproc_comdef.h"
#include "audproc_topo_bg_thread.h"

/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Macros Definitions
** ----------------------------------------------------------------------- */
#define TOPO_BG_PROCESS_MASK                        0x80000000
#define TOPO_BG_EXIT_MASK                           0x40000000

#ifndef DBG_BG_THREAD
//#define DBG_BG_THREAD
#endif

#define TOPO_BG_PROCESS_DONE_MASK                   0x80000000


#define  TOPO_BG_TOTAL_OUT_BUFFERS              2
#define  TOPO_BG_STACK_SIZE                     1024
#define  TOPO_BG_PRIMARY_PORT                   0
#define  TOPO_BG_EC_UNIT_FRAME_SIZE             1

#define  ALIGNTO8(x)                              (((x +7) >> 3) << 3)

enum{
    TOPO_BG_MAIN_BUF_IDX = 0,
    TOPO_BG_PROC_BUF_IDX,
    TOPO_BG_TOTAL_IN_BUFFERS
};


/*---------------------------------------------------------------------------
* Function Declarations
* -------------------------------------------------------------------------*/
static ADSPResult topo_bg_alloc_port_mem(topo_bg_thread_struct_t *pMe);

static void topo_bg_dealloc_port_mem(topo_bg_thread_struct_t *pMe);

static ADSPResult topo_bg_thread_launch(void *pInstance, uint16_t main_thread_objId);

static ADSPResult topo_bg_thread_init(topo_bg_thread_struct_t *me);

static void topo_bg_initialize_buf_info(AudPP_BufInfo_t *buf_info);

static void topo_bg_set_thread_name(topo_bg_thread_struct_t *me, uint16_t main_thread_objId);

static void topo_bg_thread_destroy_buffer_mem(topo_bg_thread_struct_t *me);

static void topo_bg_thread_deinit(topo_bg_thread_svc_t *pTh, bool_t *thread_launched);

static void topo_bg_thread_wait_for_task_done(topo_bg_thread_svc_t *pTh);

static ADSPResult topo_bg_thread_work_loop(void *instance);

static ADSPResult topo_bg_process_handler(void *instance);

static ADSPResult topo_bg_exit_handler(void *instance);

static bool_t topo_bg_is_req_data_collected(topo_bg_thread_struct_t *pMe);

static void topo_bg_read_write_buffers(topo_bg_thread_struct_t *pMe, AudPP_BufInfo_t *inbuf_ptr[], AudPP_BufInfo_t *outbuf_ptr[]);

static uint32_t topo_bg_read_input_buffer(topo_bg_thread_port_info_t *pIn_port_info, AudPP_BufInfo_t *inbuf_ptr, uint32_t max_samples_to_read);

static uint32_t topo_bg_write_output_buffer(topo_bg_thread_port_info_t *pOut_port_info, AudPP_BufInfo_t *outbuf_ptr, uint32_t max_samples_to_write);

static void exchange_buffer(AudPP_BufInfo_t *BufInfo1, AudPP_BufInfo_t *BufInfo2);

static ADSPResult topo_bg_process(void *state);

static ADSPResult topo_bg_realloc_buffinfo(AudPP_BufInfo_t *pBufInfo, uint32_t new_size,
	                                         uint32_t old_size, QURT_ELITE_HEAP_ID heap_id, uint32_t size_in_samples);

static uint32_t topo_bg_min_32(uint32_t var1, uint32_t var2);

static uint32_t topo_bg_get_synced_inport_curr_offset(void *pInstance, uint32_t port_id);

static void topo_bg_raise_main_th_cmdQ_event(void * instance);

static void topo_bg_raise_main_th_dataQ_event(void * instance);

static bool_t topo_bg_is_fractional_sampling_rate(uint32_t sampling_rate);

static uint32_t topo_bg_fill_proc_buffer(topo_bg_thread_port_info_t *pPort_info, AudPP_BufInfo_t *inbuf_ptr);

static void topo_bg_copy_buffers(topo_bg_thread_port_info_t *pIn_port_info);

static ADSPResult topo_bg_realloc_internal_buff(topo_bg_internal_buff_t *pInternalBuf, uint32_t new_size,
                                             uint32_t old_size, QURT_ELITE_HEAP_ID heap_id, uint32_t size_in_samples);

static void topo_bg_process_int_buf_samples(topo_bg_thread_struct_t *pMe);

static bool_t topo_bg_is_port_or_stream_active(topo_bg_thread_struct_t *pMe, uint32_t port_id);


/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

static ADSPResult (*pBgHandler[])(void *) =
{
   topo_bg_process_handler,
   topo_bg_exit_handler
};

ADSPResult topo_bg_thread_create(topo_struct *topo_ptr, void **handle, QURT_ELITE_HEAP_ID heap_id,
                                     uint32_t numActiveInPorts, uint32_t numActiveOutPorts,
                                     uint16_t main_thread_objId, topo_bg_th_cb_info_t * cb_info)
{
   ADSPResult result = ADSP_EOK;

   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)qurt_elite_memory_malloc(sizeof(topo_bg_thread_struct_t), heap_id);
   if(NULL == pMe)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX topo_bg_thread_create: memory allocation failed", main_thread_objId);
       *handle = NULL;
       return ADSP_ENOMEMORY;
   }

   memset(pMe, 0, sizeof(topo_bg_thread_struct_t));        // clear the entire structure

   pMe->topo_ptr = topo_ptr;
   pMe->heap_id = heap_id;
   pMe->numActiveInPorts = numActiveInPorts;
   pMe->numActiveOutPorts = numActiveOutPorts;
   pMe->main_th_objId = main_thread_objId;
   pMe->pp_svc_cb_info = cb_info;

   result = topo_bg_alloc_port_mem(pMe);
   if(ADSP_FAILED(result))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX topo_bg_thread_create: port memory allocation failed", pMe->main_th_objId);
       topo_bg_thread_destroy((void **)&pMe);
       *handle = NULL;
       return ADSP_ENOMEMORY;
   }

   result = topo_bg_thread_launch((void *)pMe, main_thread_objId);
   if(ADSP_FAILED(result))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX topo_bg_thread_create: thread launch failed", pMe->main_th_objId);
       topo_bg_thread_destroy((void **)&pMe);
       *handle = NULL;
       return ADSP_EFAILED;
   }

   pMe->ouput_buffer_num_unit_frames = topo_bg_th_out_buff_num_unit_frames(topo_ptr);
   pMe->ouput_main_th_buffer_num_unit_frames = topo_main_th_out_buff_num_unit_frames(topo_ptr);
   pMe->num_unit_frame_offset = 0;


   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_thread_create: BG thread is created. ", pMe->main_th_objId);

   *handle = (void *)pMe;

   return result;

}


static ADSPResult topo_bg_alloc_port_mem(topo_bg_thread_struct_t *pMe)
{
   ADSPResult result  = ADSP_EOK;
   uint32_t i,j;
   int8_t *pMem;

   uint32_t in_port_info_size = sizeof(topo_bg_thread_port_info_t)*pMe->numActiveInPorts;
   uint32_t out_port_info_size = sizeof(topo_bg_thread_port_info_t)*pMe->numActiveOutPorts;

   in_port_info_size = ALIGNTO8(in_port_info_size);
   out_port_info_size = ALIGNTO8(out_port_info_size);

   uint32_t BufInfoInsize =  sizeof(AudPP_BufInfo_t) * TOPO_BG_TOTAL_IN_BUFFERS;
   uint32_t BufInfoOutsize = sizeof(AudPP_BufInfo_t) * TOPO_BG_TOTAL_OUT_BUFFERS;
   uint32_t InternalBufInfoSize = sizeof(AudPP_BufInfo_t);

   uint32_t total_size = in_port_info_size +  out_port_info_size + ((BufInfoInsize + InternalBufInfoSize) * pMe->numActiveInPorts) + ((BufInfoOutsize + InternalBufInfoSize) * pMe->numActiveOutPorts);

    pMe->pPort_mem = (int8_t *)qurt_elite_memory_malloc(total_size, pMe->heap_id);
    if(NULL == pMe->pPort_mem)
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_create_buf_info No memory available", pMe->main_th_objId);
       return ADSP_ENOMEMORY;
    }

    memset(pMe->pPort_mem, 0, total_size);  // clear the memory
    pMem = pMe->pPort_mem;

    pMe->pIn_port_info = (topo_bg_thread_port_info_t *)pMem;
    pMem += in_port_info_size;

    pMe->pOut_port_info = (topo_bg_thread_port_info_t *)pMem;
    pMem += out_port_info_size;


    for(i = 0; i < pMe->numActiveInPorts; i++ )
    {
       pMe->pIn_port_info[i].pBufInfo = (AudPP_BufInfo_t *)pMem;
       pMem += BufInfoInsize;
    }

    for(i = 0; i < pMe->numActiveInPorts; i++ )
    {
       pMe->pIn_port_info[i].internal_buf.pInternalBufInfo = (AudPP_BufInfo_t *)pMem;
       pMem += InternalBufInfoSize;
    }

    for(i = 0; i < pMe->numActiveOutPorts; i++ )
    {
       pMe->pOut_port_info[i].pBufInfo = (AudPP_BufInfo_t *)pMem;
       pMem += BufInfoOutsize;
    }

    for(i = 0; i < pMe->numActiveOutPorts; i++ )
    {
       pMe->pOut_port_info[i].internal_buf.pInternalBufInfo = (AudPP_BufInfo_t *)pMem;
       pMem += InternalBufInfoSize;
    }

    for(i = 0; i < pMe->numActiveInPorts; i++ )
    {
       for(j = 0; j < TOPO_BG_TOTAL_IN_BUFFERS; j++)
       {
           topo_bg_initialize_buf_info(&pMe->pIn_port_info[i].pBufInfo[j]);
       }
    }

    for(i = 0; i < pMe->numActiveInPorts; i++ )
    {
       topo_bg_initialize_buf_info(pMe->pIn_port_info[i].internal_buf.pInternalBufInfo);
    }

    for(i = 0; i < pMe->numActiveOutPorts; i++ )
    {
       for(j = 0; j < TOPO_BG_TOTAL_OUT_BUFFERS; j++)
       {
           topo_bg_initialize_buf_info(&pMe->pOut_port_info[i].pBufInfo[j]);
       }
    }

    for(i = 0; i < pMe->numActiveOutPorts; i++ )
    {
       topo_bg_initialize_buf_info(pMe->pOut_port_info[i].internal_buf.pInternalBufInfo);
    }

    return result;
}

static void topo_bg_dealloc_port_mem(topo_bg_thread_struct_t *pMe)
{
   if(pMe->pPort_mem != NULL)
   {
      qurt_elite_memory_free(pMe->pPort_mem);
      pMe->pPort_mem = NULL;
   }

   return;
}

static ADSPResult topo_bg_thread_launch(void *pInstance, uint16_t main_thread_objId)
{
   ADSPResult result = ADSP_EOK;
   uint32_t stack_size = TOPO_BG_STACK_SIZE;     // TODO : verify if this is sufficient

   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

   if(NULL == pMe)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX topo_bg_thread_launch: NULL pointer error", main_thread_objId);
       return ADSP_EBADPARAM;
   }
   stack_size += topo_get_stack_size(pMe->topo_ptr);

   pMe->stack_size = stack_size;

   topo_bg_set_thread_name(pMe,main_thread_objId);

   result = topo_bg_thread_init(pMe);
   if(ADSP_FAILED(result))
   {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_launch: background thread init failed with result (0x%x)", pMe->main_th_objId,result);
   }

   return result;
}

static void topo_bg_set_thread_name(topo_bg_thread_struct_t *me, uint16_t main_thread_objId)
{
    uint32 count;

    uint32_t bg_th_objId = (uint32_t)main_thread_objId;
    bg_th_objId <<= 16;

    // static variable counter to make queue and thread name strings unique.
    count = (uint32)(qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs) & 0x000000FFL);

    count |= 0x0000BB00;

    bg_th_objId |= count;

    me->th.bg_th_objId = bg_th_objId;
    snprintf(me->thread_name, 9, "%X", (unsigned int)bg_th_objId);

    return;
}

static ADSPResult topo_bg_thread_init(topo_bg_thread_struct_t *me)
{
    ADSPResult result = ADSP_EOK;

    me->th.state = (void *)me;            // store the handle

    // allocate memory for signals
    me->th.process_signal_ptr = (qurt_elite_signal_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t),me->heap_id);
    me->th.exit_signal_ptr = (qurt_elite_signal_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t),me->heap_id);
    me->th.done_signal_ptr = (qurt_elite_signal_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t),me->heap_id);

    if((NULL == me->th.process_signal_ptr) ||
       (NULL == me->th.exit_signal_ptr) ||
       (NULL == me->th.done_signal_ptr))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_init failed to allocate memory for signals", me->main_th_objId);
        topo_bg_thread_deinit(&me->th, &me->thread_launched);
        return ADSP_ENOMEMORY;
    }

    // intialize the signals
    if(ADSP_FAILED(result = qurt_elite_signal_init(me->th.process_signal_ptr)) ||
       ADSP_FAILED(result = qurt_elite_signal_init(me->th.exit_signal_ptr)) ||
       ADSP_FAILED(result = qurt_elite_signal_init(me->th.done_signal_ptr)))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_init failed to intialize signals, result (%lu)", me->main_th_objId,result);
        topo_bg_thread_deinit(&me->th, &me->thread_launched);
        return result;
    }

    // initialize channel
    qurt_elite_channel_init(&me->th.qurt_elite_channel);
    qurt_elite_channel_init(&me->th.done_signal_channel);

    if(ADSP_FAILED(result = qurt_elite_channel_add_signal(&me->th.qurt_elite_channel, me->th.process_signal_ptr, TOPO_BG_PROCESS_MASK)) ||
       ADSP_FAILED(result = qurt_elite_channel_add_signal(&me->th.qurt_elite_channel, me->th.exit_signal_ptr, TOPO_BG_EXIT_MASK)) ||
       ADSP_FAILED(result = qurt_elite_channel_add_signal(&me->th.done_signal_channel, me->th.done_signal_ptr, TOPO_BG_PROCESS_DONE_MASK)))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_init failed to add signals to channel, result (%lu)",me->main_th_objId,result);
        topo_bg_thread_deinit(&me->th, &me->thread_launched);
        return result;
    }

    me->th.wait_mask |= (TOPO_BG_PROCESS_MASK | TOPO_BG_EXIT_MASK);
    
    {
        result =  qurt_elite_thread_launch(
                        &me->th.thread,
                        me->thread_name, NULL,
                        me->stack_size,
                        elite_get_thrd_prio(ELITETHREAD_DYNA_PP_SVC_PRIO_ID),
                        topo_bg_thread_work_loop,
                        (void *)&me->th,
                        me->heap_id);
        if (ADSP_FAILED(result))
        {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_init failed to launch thread with error %lu",me->main_th_objId,result);
            topo_bg_thread_deinit(&me->th, &me->thread_launched);
            return result;
        }
    }
    me->thread_launched = TRUE;

    // Set this signal in the beginning so that the thread is in a finished processing state.
    qurt_elite_signal_send(me->th.done_signal_ptr);
    return result;
}

ADSPResult topo_bg_thread_reset_input_buffers(void *pInstance, uint32_t port_id, AdspAudioMediaFormatInfo_t *new_media_fmt)
{
    ADSPResult result = ADSP_EOK;
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

   if(NULL == pMe)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_thread_reset_input_buffers: NULL Instance pointer error");
      return ADSP_EBADPARAM;
   }

   if(port_id <= pMe->numActiveInPorts)
   {
       uint32_t unAfeFrameSizeInSamples = 0;
       uint32_t new_size, size_in_samples;
       topo_bg_thread_port_info_t *pPort_info = &pMe->pIn_port_info[port_id];

       if(!(AudPP_MediaFormatsEqual(new_media_fmt,&pPort_info->media_format)))
       {
#ifdef DBG_BG_THREAD
	       MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_thread_reset_input_buffers sampling rate changed to (%d) and num channels to (%d) for port id (%lu)", pMe->main_th_objId, (int)new_media_fmt->samplingRate, new_media_fmt->channels,port_id);	   
#endif
	       pPort_info->media_format = *new_media_fmt;
           pPort_info->cached_media_fmt = pPort_info->media_format;
           elite_svc_get_frame_size(pPort_info->media_format.samplingRate,&unAfeFrameSizeInSamples);

           new_size = unAfeFrameSizeInSamples * pMe->ouput_buffer_num_unit_frames * pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample);
           size_in_samples = unAfeFrameSizeInSamples * pMe->ouput_buffer_num_unit_frames;
          // destroy the buffer memory

          result = topo_bg_realloc_buffinfo(&pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX], new_size, pPort_info->frame_size_in_bytes, pMe->heap_id, size_in_samples);
          if(ADSP_FAILED(result))
          {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_reset_input_buffers: No Memory allocated", pMe->main_th_objId);
               topo_bg_thread_destroy_buffer_mem(pMe);
               return ADSP_ENOMEMORY;
          }

          // sync the offset with current output buffer offset
          pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples = topo_bg_get_synced_inport_curr_offset(pMe, port_id);

          result = topo_bg_realloc_buffinfo(&pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX], new_size, pPort_info->frame_size_in_bytes, pMe->heap_id, size_in_samples);
          if(ADSP_FAILED(result))
          {
             MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_reset_input_buffers: No Memory allocated", pMe->main_th_objId);
             topo_bg_thread_destroy_buffer_mem(pMe);
             return ADSP_ENOMEMORY;
          }

          pPort_info->frame_size_in_bytes = new_size;

          if(topo_bg_is_fractional_sampling_rate(pPort_info->media_format.samplingRate))
          {
              uint32_t num_unit_frame_int_buf = pMe->ouput_main_th_buffer_num_unit_frames;
              uint32_t size;
              uint32_t size_in_samples;
              if((topo_is_ec_present(pMe->topo_ptr)) && (pMe->numActiveInPorts > 1))
              {
                 num_unit_frame_int_buf = TOPO_BG_EC_UNIT_FRAME_SIZE;
              }
              size = num_unit_frame_int_buf * unAfeFrameSizeInSamples * pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample);
              size_in_samples = num_unit_frame_int_buf * unAfeFrameSizeInSamples;  
              result = topo_bg_realloc_internal_buff(&pPort_info->internal_buf, size, pPort_info->internal_buf.size_in_bytes, pMe->heap_id, size_in_samples);
              if(ADSP_FAILED(result))
              {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_reset_input_buffers: No Memory allocated for internal buffer", pMe->main_th_objId);
                  topo_bg_thread_destroy_buffer_mem(pMe);
                  return ADSP_ENOMEMORY;
              }
          }
          else if(NULL != pPort_info->internal_buf.pInternalBufInfo->pBuf)
          {
              qurt_elite_memory_free(pPort_info->internal_buf.pInternalBufInfo->pBuf);
              pPort_info->internal_buf.pInternalBufInfo->pBuf = NULL;
          }

       }
   }
   else
   {
       return ADSP_EUNSUPPORTED;
   }

   return result;
}


static ADSPResult topo_bg_realloc_internal_buff(topo_bg_internal_buff_t *pInternalBuf, uint32_t new_size,
                                             uint32_t old_size, QURT_ELITE_HEAP_ID heap_id, uint32_t size_in_samples)
{
    ADSPResult result = ADSP_EOK;
   result = topo_bg_realloc_buffinfo(pInternalBuf->pInternalBufInfo, new_size, old_size, heap_id, size_in_samples);
   if(ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "topo_bg_realloc_internal_buff: No Memory allocated for internal buffer");
      return ADSP_ENOMEMORY;
   }

   pInternalBuf->size_in_bytes = new_size;

   return result;
}

ADSPResult topo_bg_thread_reset_output_buffers(void *pInstance, uint32_t port_id, AdspAudioMediaFormatInfo_t *new_media_fmt)
{
   ADSPResult result = ADSP_EOK;
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

   if(NULL == pMe)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_thread_reset_output_buffers: NULL Instance pointer error");
      return ADSP_EBADPARAM;
   }

   if(port_id <= pMe->numActiveOutPorts)
   {
       uint32_t unAfeFrameSizeOutSamples = 0;// unAfeFrameSizeOutSamples_old = 0;
       uint32_t  i, new_size, size_in_samples;
       topo_bg_thread_port_info_t *pPort_info = &pMe->pOut_port_info[port_id];
       //uint32_t curr_primary_out_offset = pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples;

       if(!(AudPP_MediaFormatsEqual(new_media_fmt,&pPort_info->media_format)))
       {
#ifdef DBG_BG_THREAD
	       MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_thread_reset_output_buffers sampling rate changed to (%d) and num channels to (%d) for port id (%lu)", pMe->main_th_objId, (int)new_media_fmt->samplingRate, new_media_fmt->channels,port_id);	   
#endif
           //elite_svc_get_frame_size(pPort_info->media_format.samplingRate,&unAfeFrameSizeOutSamples_old);
           pPort_info->media_format = *new_media_fmt;
           elite_svc_get_frame_size(pPort_info->media_format.samplingRate,&unAfeFrameSizeOutSamples);

           new_size = unAfeFrameSizeOutSamples * pMe->ouput_buffer_num_unit_frames * pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample);
           size_in_samples = unAfeFrameSizeOutSamples * pMe->ouput_buffer_num_unit_frames;

           for(i=0; i < TOPO_BG_TOTAL_OUT_BUFFERS; i++)
           {
               result = topo_bg_realloc_buffinfo(&pPort_info->pBufInfo[i], new_size, pPort_info->frame_size_in_bytes, pMe->heap_id, size_in_samples);
               if(ADSP_FAILED(result))
               {
                   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_reset_output_buffers: No Memory allocated", pMe->main_th_objId);
                   topo_bg_thread_destroy_buffer_mem(pMe);
                   return ADSP_ENOMEMORY;
               }
           }

           // retain the offset
           pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples = 0; //((curr_primary_out_offset/unAfeFrameSizeOutSamples_old) * unAfeFrameSizeOutSamples);
           pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples = 0;  // clear size in samples in the begining, this will set once BG has produced any valid output

            pPort_info->frame_size_in_bytes= new_size;
           pPort_info->bg_thread_port_delay = 0; // this should be zero now as BG thread will not deliver any data untill it has any valid data in its output //TOPO_BG_TOTAL_OUT_BUFFERS* ((pPort_info->frame_size_in_bytes*1000*1000)/(pPort_info->media_format.samplingRate * pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample)));

           // if output sampling rate is fractional then allocate internal buffer
           if(topo_bg_is_fractional_sampling_rate(pPort_info->media_format.samplingRate))
           {
               uint32_t size = pMe->ouput_main_th_buffer_num_unit_frames * unAfeFrameSizeOutSamples * pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample);
               uint32_t size_in_samples = pMe->ouput_main_th_buffer_num_unit_frames * unAfeFrameSizeOutSamples;
               result = topo_bg_realloc_internal_buff(&pPort_info->internal_buf, size, pPort_info->internal_buf.size_in_bytes, pMe->heap_id, size_in_samples);
               if(ADSP_FAILED(result))
               {
                   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_reset_input_buffers: No Memory allocated for internal buffer", pMe->main_th_objId);
                   topo_bg_thread_destroy_buffer_mem(pMe);
                   return ADSP_ENOMEMORY;
               }
               // fill  the output it buffer so that one extra buffer is available in the begining
               pPort_info->internal_buf.pInternalBufInfo->offsetInSamples = size_in_samples;
               pPort_info->bg_thread_port_delay += (pPort_info->internal_buf.size_in_bytes *1000*1000)/(pPort_info->media_format.samplingRate * pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample));
           }
           else if(NULL != pPort_info->internal_buf.pInternalBufInfo->pBuf)
           {
               qurt_elite_memory_free(pPort_info->internal_buf.pInternalBufInfo->pBuf);
               pPort_info->internal_buf.pInternalBufInfo->pBuf = NULL;
           }

       }
   }
   else
   {
       return ADSP_EUNSUPPORTED;
   }

   return result;
}

static bool_t topo_bg_is_fractional_sampling_rate(uint32_t sampling_rate)
{
   if(((sampling_rate/1000)*1000) != sampling_rate)
   {
      return TRUE;
   }
   return FALSE;
}


static ADSPResult topo_bg_realloc_buffinfo(AudPP_BufInfo_t *pBufInfo, uint32_t new_size,
                                             uint32_t old_size, QURT_ELITE_HEAP_ID heap_id, uint32_t size_in_samples)
{
   ADSPResult result = ADSP_EOK;

   if(new_size != old_size)
   {
       if(NULL != pBufInfo->pBuf)
       {
           qurt_elite_memory_free(pBufInfo->pBuf);
           pBufInfo->pBuf = NULL;
       }

       pBufInfo->pBuf = (int8_t *)qurt_elite_memory_malloc(new_size,heap_id);
       if(pBufInfo->pBuf == NULL)
       {
           return ADSP_ENOMEMORY;
       }
   }

   memset(pBufInfo->pBuf,0,new_size);
   pBufInfo->offsetInSamples = 0;
   pBufInfo->sizeInSamples = size_in_samples;
   memset(&pBufInfo->flags,0,sizeof(pBufInfo->flags));
   pBufInfo->timestamp = 0;

   return result;
}

static void topo_bg_thread_destroy_buffer_mem(topo_bg_thread_struct_t *pMe)
{
    uint32_t in_port_idx, out_port_idx, i;
    topo_bg_thread_port_info_t *pPort_info;

    for(in_port_idx = 0; in_port_idx < pMe->numActiveInPorts; in_port_idx++)
    {
       pPort_info = &pMe->pIn_port_info[in_port_idx];

       for(i = 0; i< TOPO_BG_TOTAL_IN_BUFFERS; i++)
       {
          if(NULL != pPort_info->pBufInfo[i].pBuf)
          {
             qurt_elite_memory_free(pPort_info->pBufInfo[i].pBuf);
             pPort_info->pBufInfo[i].pBuf = NULL;
          }
       }

       if(NULL != pPort_info->internal_buf.pInternalBufInfo->pBuf)
       {
          qurt_elite_memory_free(pPort_info->internal_buf.pInternalBufInfo->pBuf);
          pPort_info->internal_buf.pInternalBufInfo->pBuf = NULL;
       }
    }

    for(out_port_idx = 0; out_port_idx < pMe->numActiveOutPorts; out_port_idx++)
    {
       pPort_info = &pMe->pOut_port_info[out_port_idx];
       for(i = 0; i< TOPO_BG_TOTAL_OUT_BUFFERS; i++)
       {
          if(NULL != pPort_info->pBufInfo[i].pBuf)
          {
             qurt_elite_memory_free(pPort_info->pBufInfo[i].pBuf);
             pPort_info->pBufInfo[i].pBuf = NULL;
          }
       }

       if(NULL != pPort_info->internal_buf.pInternalBufInfo->pBuf)
       {
          qurt_elite_memory_free(pPort_info->internal_buf.pInternalBufInfo->pBuf);
          pPort_info->internal_buf.pInternalBufInfo->pBuf = NULL;
       }
    }

    return;
}

ADSPResult topo_bg_thread_destroy(void **ppInstance)
{
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)(*ppInstance);

    if(NULL == pMe)
    {
        MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_thread_destroy: NULL pointer error");
        return ADSP_EBADPARAM;
    }
    topo_bg_thread_deinit(&pMe->th, &pMe->thread_launched);
    topo_bg_thread_destroy_buffer_mem(pMe);
    topo_bg_dealloc_port_mem(pMe);

    qurt_elite_memory_free(pMe);

    *ppInstance = NULL;

    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_thread_destroy: BG thread is destroyed", pMe->main_th_objId);

    return ADSP_EOK;
}

static void topo_bg_thread_deinit(topo_bg_thread_svc_t *pTh, bool_t *thread_launched)
{
    if(*thread_launched)
    {
       int status;
       topo_bg_thread_wait_for_task_done(pTh);      // if thread is launched then wait for bg processing to be finished

       if(pTh->exit_signal_ptr != NULL)
       {
          qurt_elite_signal_send(pTh->exit_signal_ptr);

       }
       qurt_elite_thread_join(pTh->thread, &status);
       *thread_launched = FALSE;
    }

    if(pTh->exit_signal_ptr != NULL)
    {
       qurt_elite_signal_deinit(pTh->exit_signal_ptr);
       qurt_elite_memory_free(pTh->exit_signal_ptr);
       pTh->exit_signal_ptr = NULL;
    }

    if(pTh->done_signal_ptr != NULL)
    {
       qurt_elite_signal_deinit(pTh->done_signal_ptr);
       qurt_elite_memory_free(pTh->done_signal_ptr);
       pTh->exit_signal_ptr = NULL;
    }

    if(pTh->process_signal_ptr != NULL)
    {
       qurt_elite_signal_deinit(pTh->process_signal_ptr);
       qurt_elite_memory_free(pTh->process_signal_ptr);
       pTh->process_signal_ptr = NULL;
    }

    qurt_elite_channel_destroy(&pTh->qurt_elite_channel);
    qurt_elite_channel_destroy(&pTh->done_signal_channel);

    return;
}


static void topo_bg_thread_wait_for_task_done(topo_bg_thread_svc_t *pTh)
{
    (void) qurt_elite_channel_wait(&pTh->done_signal_channel, TOPO_BG_PROCESS_DONE_MASK);
    return;
}


static void topo_bg_initialize_buf_info(AudPP_BufInfo_t *buf_info)
{
   buf_info->pBuf = NULL;
   buf_info->offsetInSamples = 0;
   buf_info->sizeInSamples = 0;
   memset(&buf_info->flags, 0 , sizeof(buf_info->flags));
   buf_info->timestamp = 0;

   return;
}


static ADSPResult topo_bg_thread_work_loop(void *instance)
{
   topo_bg_thread_svc_t *pTh = (topo_bg_thread_svc_t *)instance;
   ADSPResult nResult = ADSP_EOK;
   int32_t bit_pos;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%X TOPO bg thread entering workloop.", pTh->bg_th_objId);

   // work loop
   for(;;)
   {
       // ***************** Wait for the MASK
       // block on any one or more of selected queues to get a msg
       pTh->rcvd_mask = qurt_elite_channel_wait(&pTh->qurt_elite_channel, pTh->wait_mask);

       while(pTh->rcvd_mask)
       {
          bit_pos = get_signal_pos(pTh->rcvd_mask);

          nResult = pBgHandler[bit_pos](pTh);

          if(nResult != ADSP_EOK)
          {
             if (ADSP_ETERMINATED == nResult)
             {
                MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"P%X topo_bg_thread_work_loop : terminating thread", pTh->bg_th_objId);
                return ADSP_EOK;
             }
             MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%X topo_bg_thread_work_loop(): Handler Error bit_pos=%d, res=%d",pTh->bg_th_objId,(int)bit_pos, nResult );
             return nResult;
          }


          pTh->rcvd_mask = qurt_elite_channel_poll(&pTh->qurt_elite_channel, pTh->wait_mask);
      } // end of while loop
   }
   return nResult;
}

static ADSPResult topo_bg_process_handler(void *instance)
{
   topo_bg_thread_svc_t *pTh = (topo_bg_thread_svc_t *)instance;
   ADSPResult nResult = ADSP_EOK;

   qurt_elite_signal_clear(pTh->process_signal_ptr);

#ifdef DBG_BG_THREAD
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"P%X topo_bg_process_handler start processing ",pTh->bg_th_objId);
#endif
   topo_bg_process(pTh->state);

#ifdef DBG_BG_THREAD
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"P%X topo_bg_process_handler : Done processing, sending the signal",pTh->bg_th_objId);
#endif

   qurt_elite_signal_send(pTh->done_signal_ptr);

   topo_bg_raise_main_th_cmdQ_event(pTh->state);
   topo_bg_raise_main_th_dataQ_event(pTh->state);

   return nResult;
}


static void topo_bg_raise_main_th_cmdQ_event(void * instance)
{
    ADSPResult result = ADSP_EOK;
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)instance;
    topo_bg_cb_info_t cb_info;

    cb_info.event_id = TOPO_BG_EVENT_ENABLE_MAIN_TH_CMDQ;
    cb_info.port_id = 0;  // dont care for this event
    cb_info.payload = NULL; // dont care for this event

    result = pMe->pp_svc_cb_info->cb_func(pMe->pp_svc_cb_info->context_ptr, cb_info);
    if(ADSP_FAILED(result))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%X topo_bg_raise_process_done_event failed with result %lu",pMe->th.bg_th_objId ,result);
    }

#ifdef DBG_BG_THREAD
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"P%X topo_bg_call_fg_cb_func : to enable main th CMDQ Done",pMe->th.bg_th_objId);
#endif

    return;
}

static void topo_bg_raise_main_th_dataQ_event(void * instance)
{
    ADSPResult result = ADSP_EOK;
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)instance;
    topo_bg_cb_info_t cb_info;
    bool_t send_event = FALSE;

    for(uint32_t port_idx = 0; port_idx < pMe->numActiveInPorts; port_idx++)
    {
       if(topo_is_port_or_stream_active(pMe->topo_ptr,port_idx) && topo_bg_is_partial_proc_req((void *)pMe, port_idx))
       {
          send_event = TRUE;
       }
    }

    if(send_event)
    {
       cb_info.event_id = TOPO_BG_EVENT_ENABLE_MAIN_TH_DATAQ;
       cb_info.port_id = 0;  // dont care for this event
       cb_info.payload = NULL; // dont care for this event

       result = pMe->pp_svc_cb_info->cb_func(pMe->pp_svc_cb_info->context_ptr, cb_info);
       if(ADSP_FAILED(result))
       {
           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%X topo_bg_raise_process_done_event failed with result %lu",pMe->th.bg_th_objId ,result);
       }

#ifdef DBG_BG_THREAD
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"P%X topo_bg_call_fg_cb_func : to enable main th DATAQ Done",pMe->th.bg_th_objId);
#endif
    }

    return;
}


static ADSPResult topo_bg_exit_handler(void *instance)
{
   topo_bg_thread_svc_t *pTh = (topo_bg_thread_svc_t *)instance;
   ADSPResult nResult = ADSP_ETERMINATED;

   qurt_elite_signal_clear(pTh->exit_signal_ptr);

   return nResult;
}

// In case topo process is stuck due to insufficient samples and there is no data in internal buffer as well
// the very first buffer that is received by main thread should be copied directly to PROC buffer and topo process should initiated
// 1) check if MAIN buffer is empty. If not then copy input buffer worth of samples from MAIN buffer to PROC buffer
// 2) Adjust MAIN buffer samples so that unread data is moved to begining of buffer.
// 3) Copy input buffer data to MAIN buffer
static uint32_t topo_bg_fill_proc_buffer(topo_bg_thread_port_info_t *pPort_info, AudPP_BufInfo_t *inbuf_ptr)
{
   AdspAudioMediaFormatInfo_t *media_fmt = &pPort_info->media_format;

   pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples = pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples;

   AudPP_BufInfo_t *pProcBufInfo = &pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX];
   uint32_t proc_buff_ch_offset = 0; //(pProcBufInfo->offsetInSamples)*topo_get_bytes_per_sample(media_fmt->bitsPerSample);
   uint32_t proc_buff_ch_size =  (pProcBufInfo->sizeInSamples)*topo_get_bytes_per_sample(media_fmt->bitsPerSample);

   uint32_t inbuf_ch_offset =   (inbuf_ptr->offsetInSamples)*topo_get_bytes_per_sample(media_fmt->bitsPerSample);
   uint32_t inbuf_ch_size =     (inbuf_ptr->sizeInSamples)*topo_get_bytes_per_sample(media_fmt->bitsPerSample);

   uint32_t bytes_written = 0;


// first see if there is any data in MAIN buffer, if yes then copy input samples worth of data from MAIN buffer to proc buffer
// adjust MAIN buffer samples and then copy data to MAIN buffer

// if there are no samples in MAIN BUFF then directly copy the input data to PROC buffer

   if(pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples !=0)
   {
        AudPP_BufInfo_t *pMainBufInfo = &pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX];
        uint32_t main_buff_ch_offset = pMainBufInfo->offsetInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);
        uint32_t main_buff_ch_size = pMainBufInfo->sizeInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);
        if(media_fmt->isInterleaved == 1)
        {
            bytes_written = memscpy(pProcBufInfo->pBuf + (proc_buff_ch_offset*media_fmt->channels),
                                   (proc_buff_ch_size -  proc_buff_ch_offset)*media_fmt->channels,
                                    pMainBufInfo->pBuf,
                                   (inbuf_ch_size - inbuf_ch_offset) * media_fmt->channels);

            memsmove(pMainBufInfo->pBuf,
                    pPort_info->frame_size_in_bytes,
                    pMainBufInfo->pBuf + bytes_written,
                    (main_buff_ch_offset*media_fmt->channels) - bytes_written);

            main_buff_ch_offset -= bytes_written/media_fmt->channels;

            bytes_written = memscpy(pMainBufInfo->pBuf + (main_buff_ch_offset*media_fmt->channels),
                                   (main_buff_ch_size -  main_buff_ch_offset)*media_fmt->channels,
                                   inbuf_ptr->pBuf + (inbuf_ch_offset*media_fmt->channels),
                                   (inbuf_ch_size - inbuf_ch_offset) * media_fmt->channels);

            bytes_written = bytes_written/media_fmt->channels;
            main_buff_ch_offset += bytes_written;
            pMainBufInfo->offsetInSamples = main_buff_ch_offset/topo_get_bytes_per_sample(media_fmt->bitsPerSample);
        }
        else
        {
            proc_buff_ch_size = topo_bg_min_32(proc_buff_ch_size -  proc_buff_ch_offset,inbuf_ch_size - inbuf_ch_offset);
            for(uint32_t i = 0; i < media_fmt->channels; i++)
            {
                bytes_written = memscpy(pProcBufInfo->pBuf + (i*proc_buff_ch_size) + proc_buff_ch_offset,
                                   proc_buff_ch_size,
                                   pMainBufInfo->pBuf + (i*main_buff_ch_size),
                                   proc_buff_ch_size);
            }

            for(uint32_t i = 0; i < media_fmt->channels; i++)
            {
                memsmove(pMainBufInfo->pBuf + (i*main_buff_ch_size),
                         main_buff_ch_size,
                         pMainBufInfo->pBuf + (i*main_buff_ch_size) + bytes_written,
                         main_buff_ch_offset - bytes_written);
            }
            main_buff_ch_offset -= bytes_written;

            for(uint32_t i = 0; i < media_fmt->channels; i++)
            {
                bytes_written = memscpy(pMainBufInfo->pBuf + (i*main_buff_ch_size) + main_buff_ch_offset,
                                        main_buff_ch_size - main_buff_ch_offset,
                                        inbuf_ptr->pBuf + (i*inbuf_ch_size) + inbuf_ch_offset,
                                        inbuf_ch_size - inbuf_ch_offset);
            }
            main_buff_ch_offset += bytes_written;
            pMainBufInfo->offsetInSamples = main_buff_ch_offset/topo_get_bytes_per_sample(media_fmt->bitsPerSample);
        }

#ifdef DBG_BG_THREAD
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_fill_proc_buffer: %lu bytes transfered from MAIN buffer to PROC buffer", bytes_written);
#endif

        inbuf_ptr->offsetInSamples += (bytes_written/topo_get_bytes_per_sample(media_fmt->bitsPerSample));
        pProcBufInfo->sizeInSamples = (bytes_written/topo_get_bytes_per_sample(media_fmt->bitsPerSample));
        pProcBufInfo->offsetInSamples = 0;

        pProcBufInfo->flags = pMainBufInfo->flags;
        pProcBufInfo->timestamp = pMainBufInfo->timestamp;
        pMainBufInfo->flags = inbuf_ptr->flags;
        pMainBufInfo->timestamp = inbuf_ptr->timestamp;
   }
   else
   {
       if(media_fmt->isInterleaved == 1)
       {
           bytes_written = memscpy(pProcBufInfo->pBuf + (proc_buff_ch_offset*media_fmt->channels),
                               (proc_buff_ch_size -  proc_buff_ch_offset)*media_fmt->channels,
                               inbuf_ptr->pBuf + (inbuf_ch_offset*media_fmt->channels),
                               (inbuf_ch_size - inbuf_ch_offset) * media_fmt->channels);

           bytes_written = bytes_written/media_fmt->channels;
       }
       else
       {
           proc_buff_ch_size = topo_bg_min_32(proc_buff_ch_size -  proc_buff_ch_offset,inbuf_ch_size - inbuf_ch_offset);
           for(uint32_t i = 0; i < media_fmt->channels; i++)
           {
               bytes_written = memscpy(pProcBufInfo->pBuf + (i*proc_buff_ch_size) + proc_buff_ch_offset,
                               proc_buff_ch_size,
                               inbuf_ptr->pBuf + (i*inbuf_ch_size) + inbuf_ch_offset,
                               proc_buff_ch_size);
           }
       }

#ifdef DBG_BG_THREAD
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_fill_proc_buffer: %lu bytes transfered from input buffer to PROC buffer", bytes_written);
#endif

       inbuf_ptr->offsetInSamples += (bytes_written/topo_get_bytes_per_sample(media_fmt->bitsPerSample));
       pProcBufInfo->sizeInSamples = (bytes_written/topo_get_bytes_per_sample(media_fmt->bitsPerSample));
       pProcBufInfo->offsetInSamples = 0;

       pProcBufInfo->flags = inbuf_ptr->flags;
       pProcBufInfo->timestamp = inbuf_ptr->timestamp;

   }

   return bytes_written;
}


static bool_t topo_bg_is_internal_buff_empty(topo_bg_thread_port_info_t *pPort_info)
{
   if((pPort_info->internal_buf.pInternalBufInfo->sizeInSamples == 0) && (pPort_info->internal_buf.pInternalBufInfo->pBuf != NULL))
   {
       return TRUE;
   }

   return FALSE;
}

bool_t topo_bg_is_partial_proc_req(void *pHandle, uint32_t port_idx)
{
    if(NULL == pHandle)
    {
         return FALSE;
    }
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pHandle;
    topo_bg_thread_port_info_t *pPort_info = &pMe->pIn_port_info[port_idx];
    AdspAudioMediaFormatInfo_t *media_fmt = (pPort_info->dataQ_pending_event_flag == TRUE)?(&pPort_info->cached_media_fmt):(&pPort_info->media_format);

    if(topo_bg_is_fractional_sampling_rate(media_fmt->samplingRate))
    {
        // check if topo requires more data and there is no data with internal buffer as well
        if((topo_is_port_req_more_data(pMe->topo_ptr, port_idx)) &&
             (topo_bg_is_internal_buff_empty(pPort_info)))
        {
#ifdef DBG_BG_THREAD
                MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_is_partial_proc_req: partial processing is req", pMe->main_th_objId);
#endif

            return TRUE;
        }
    }
    return FALSE;
}

bool_t topo_bg_is_extra_out_buff_req(void *pHandle, uint32_t port_idx)
{
    if(NULL == pHandle)
    {
        return FALSE;
    }
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pHandle;
    topo_bg_thread_port_info_t *pPort_info;

    pPort_info = &pMe->pOut_port_info[port_idx];
    uint32_t unAfeFrameSizeOutSamples;

    elite_svc_get_frame_size(pPort_info->media_format.samplingRate,&unAfeFrameSizeOutSamples);

    // if all the output in main buffer is consumed, check if internal buffer is full. If yes then return TRUE
    if((pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples == 0) &&
	   (pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples != 0) &&
           (pPort_info->internal_buf.pInternalBufInfo->pBuf != NULL))
    {
       if(pPort_info->internal_buf.pInternalBufInfo->offsetInSamples == (pMe->ouput_main_th_buffer_num_unit_frames * unAfeFrameSizeOutSamples))
       {
#ifdef DBG_BG_THREAD
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_is_extra_out_buff_req: requires one more out buffer", pMe->main_th_objId);
#endif

          return TRUE;
       }
    }
    return FALSE;
}

void topo_bg_fill_out_buff_info(void *pHandle, AudPP_BufInfo_t *outBufInfo,uint32_t port_idx)
{
   if(NULL == pHandle)
   {
      return;
   }
   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pHandle;
   topo_bg_thread_port_info_t *pPort_info = &pMe->pOut_port_info[port_idx];
   AdspAudioMediaFormatInfo_t *media_fmt = &pPort_info->media_format;
   uint32_t outbuf_ch_offset = outBufInfo->offsetInSamples*topo_get_bytes_per_sample(media_fmt->bitsPerSample);
   uint32_t outbuf_ch_size = outBufInfo->sizeInSamples*topo_get_bytes_per_sample(media_fmt->bitsPerSample);

   uint32_t int_buff_offset = pPort_info->internal_buf.pInternalBufInfo->offsetInSamples *topo_get_bytes_per_sample(media_fmt->bitsPerSample);
   uint32_t int_buff_size = pPort_info->internal_buf.pInternalBufInfo->sizeInSamples*topo_get_bytes_per_sample(media_fmt->bitsPerSample);
   uint32_t bytes_written = 0;

   if(media_fmt->isInterleaved == 1)
   {
       bytes_written = memscpy(outBufInfo->pBuf + (outbuf_ch_offset * media_fmt->channels),
               (outbuf_ch_size - outbuf_ch_offset)*media_fmt->channels,
               pPort_info->internal_buf.pInternalBufInfo->pBuf,
               int_buff_offset * media_fmt->channels);
       bytes_written = bytes_written/media_fmt->channels;
   }
   else
   {
       for(uint32_t ch = 0; ch < media_fmt->channels; ch++)
       {
           bytes_written = memscpy(outBufInfo->pBuf + (ch * outbuf_ch_size) + outbuf_ch_offset,
                               (outbuf_ch_size - outbuf_ch_offset),
                               pPort_info->internal_buf.pInternalBufInfo->pBuf + (ch*int_buff_size),
                               int_buff_offset);

       }
   }
   outBufInfo->offsetInSamples += bytes_written/topo_get_bytes_per_sample(media_fmt->bitsPerSample);
   pPort_info->internal_buf.pInternalBufInfo->offsetInSamples = 0;

#ifdef DBG_BG_THREAD
       MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_fill_out_buff_info: samples written %lu", pMe->main_th_objId, bytes_written/topo_get_bytes_per_sample(media_fmt->bitsPerSample));
#endif

   return;
}


static void topo_bg_read_write_buffers(topo_bg_thread_struct_t *pMe, AudPP_BufInfo_t *inbuf_ptr[], AudPP_BufInfo_t *outbuf_ptr[])
{
   uint32_t unAfeFrameSizeInSamples = 0;
   topo_bg_thread_port_info_t *pPort_info;

   // assuming num unit frames will be same on all output ports
   // get unit frames remaining to fill in primary output port
   elite_svc_get_frame_size(pMe->pOut_port_info[TOPO_BG_PRIMARY_PORT].media_format.samplingRate,&unAfeFrameSizeInSamples);

   uint32_t rem_out_unit_frames = (outbuf_ptr[TOPO_BG_PRIMARY_PORT]->sizeInSamples - outbuf_ptr[TOPO_BG_PRIMARY_PORT]->offsetInSamples)/unAfeFrameSizeInSamples;
   uint32_t max_samples_to_read = 0, max_samples_to_write = 0;
   uint32_t num_unit_frames_read = 0, num_samples_read = 0, num_samples_written = 0, num_unit_frames_written = 0;
   uint32_t min_port_unit_frame = rem_out_unit_frames;

    for(uint32_t port_idx = 0; port_idx < pMe->numActiveInPorts; port_idx++)
    {
        pPort_info = &pMe->pIn_port_info[port_idx];
       if(topo_bg_is_port_or_stream_active(pMe,port_idx) && (!topo_bg_is_bg_thread_processing((void *)pMe)) && (!pPort_info->dataQ_pending_event_flag))
       {
           // At this point if partial processing is required that means topo is waiting for more samples to start processing
           // So whatever samples are received, send that to topo process without waiting for MAIN buffer to fill
           if(topo_bg_is_partial_proc_req((void *)pMe, port_idx))
           {
               if(topo_bg_fill_proc_buffer(pPort_info, inbuf_ptr[port_idx]) > 0)
               {
                   pMe->allow_partial_processing = TRUE;
                   return;
               }
           }
       }
    }

    for(uint32_t port_idx = 0; port_idx < pMe->numActiveInPorts; port_idx++)
    {
       pPort_info = &pMe->pIn_port_info[port_idx];
        if(topo_bg_is_port_or_stream_active(pMe,port_idx))
        {
           //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MAIN buffer memory token is %d", *((int *)(pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + 1760)));
           AdspAudioMediaFormatInfo_t *media_fmt = (pPort_info->dataQ_pending_event_flag == TRUE)?(&pPort_info->cached_media_fmt):(&pPort_info->media_format);
           elite_svc_get_frame_size(media_fmt->samplingRate,&unAfeFrameSizeInSamples);
           min_port_unit_frame = topo_bg_min_32((inbuf_ptr[port_idx]->sizeInSamples - inbuf_ptr[port_idx]->offsetInSamples)/unAfeFrameSizeInSamples, min_port_unit_frame);
        }
    }

    for(uint32_t port_idx = 0; port_idx < pMe->numActiveInPorts; port_idx++)
    {
       pPort_info = &pMe->pIn_port_info[port_idx];
       if(topo_bg_is_port_or_stream_active(pMe,port_idx))
       {
          AdspAudioMediaFormatInfo_t *media_fmt = (pPort_info->dataQ_pending_event_flag == TRUE)?(&pPort_info->cached_media_fmt):(&pPort_info->media_format);
          elite_svc_get_frame_size(media_fmt->samplingRate,&unAfeFrameSizeInSamples);
          max_samples_to_read = unAfeFrameSizeInSamples*topo_bg_min_32(min_port_unit_frame, pMe->ouput_main_th_buffer_num_unit_frames);
          // assuming all ports are operating on same num unit frames
          num_samples_read = topo_bg_read_input_buffer(pPort_info, inbuf_ptr[port_idx], max_samples_to_read);
          num_unit_frames_read = num_samples_read/unAfeFrameSizeInSamples;
       }
    }

    for(uint32_t port_idx = 0; port_idx < pMe->numActiveInPorts; port_idx++)
    {
        pPort_info = &pMe->pIn_port_info[port_idx];
        if(!topo_bg_is_port_or_stream_active(pMe,port_idx))
        {
               pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples = 0;
               pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples = 0;
        }
    }

    pMe->num_unit_frame_offset += num_unit_frames_read;

    QURT_ELITE_ASSERT(num_unit_frames_read <= pMe->ouput_main_th_buffer_num_unit_frames);

    for(uint32_t port_idx = 0; port_idx < pMe->numActiveOutPorts; port_idx++)
    {
       pPort_info = &pMe->pOut_port_info[port_idx];
       elite_svc_get_frame_size(pPort_info->media_format.samplingRate,&unAfeFrameSizeInSamples);
       max_samples_to_write = num_unit_frames_read*unAfeFrameSizeInSamples;

       // assuming all ports are operating on same num unit frames
       num_samples_written = topo_bg_write_output_buffer(pPort_info, outbuf_ptr[port_idx], max_samples_to_write);
       num_unit_frames_written = num_samples_written/unAfeFrameSizeInSamples;
    }

    QURT_ELITE_ASSERT(num_unit_frames_read == num_unit_frames_written);

#ifdef DBG_BG_THREAD
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_read_write_buffers: samples read %lu and samples written %lu", pMe->main_th_objId, num_samples_read, num_samples_written);
#endif

    return;
}

// This function buffers the input buffer data and triggers back ground thread processing once required amount of data is accumulated
ADSPResult topo_bg_thread_start_process(void *pInstance, AudPP_BufInfo_t *inbuf_ptr[], AudPP_BufInfo_t *outbuf_ptr[])
{
    ADSPResult result = ADSP_EOK;

    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;
    uint32_t inPortIntBufdelay = 0, in_port_idx, out_port_idx;

    topo_bg_thread_port_info_t *pPort_info;

    if(NULL == pMe)
    {
        MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_thread_start_process: NULL Instance pointer error");
        return ADSP_EBADPARAM;
    }

    if(!pMe->thread_launched)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_thread_start_process : bg thread is not launched, process should be executed by main thread", pMe->main_th_objId);
        return ADSP_EUNEXPECTED;
    }

    // if internal buffer is created, account for its delay
    if(pMe->pIn_port_info[TOPO_BG_PRIMARY_PORT].internal_buf.pInternalBufInfo->pBuf != NULL)
    {
       pPort_info = &pMe->pIn_port_info[TOPO_BG_PRIMARY_PORT];
       inPortIntBufdelay = (pMe->pIn_port_info[TOPO_BG_PRIMARY_PORT].internal_buf.size_in_bytes *1000*1000)/(pPort_info->media_format.samplingRate * pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample));
    }

    for(in_port_idx = 0; in_port_idx < pMe->numActiveInPorts; in_port_idx++)
    {
       pPort_info = &pMe->pIn_port_info[in_port_idx];

       // if port was not active before and then port starts running
       if((inbuf_ptr[in_port_idx]->sizeInSamples !=0) && (pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples == 0))
       {
          pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples = topo_bg_get_synced_inport_curr_offset(pMe, in_port_idx);
          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_thread_start_process: new offset is  %lu",pMe->main_th_objId,pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples);
          pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples = pPort_info->frame_size_in_bytes/(pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample));
          pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples = pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples;
          memset(pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf, 0, pPort_info->frame_size_in_bytes);
          memset(pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf, 0, pPort_info->frame_size_in_bytes);

          memset(&pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].flags, 0 , sizeof(capi_v2_stream_flags_t));
          pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].timestamp = 0;

          if(pPort_info->internal_buf.pInternalBufInfo->pBuf != NULL)
          {
             memset(pPort_info->internal_buf.pInternalBufInfo->pBuf, 0, pPort_info->internal_buf.size_in_bytes);
             pPort_info->internal_buf.pInternalBufInfo->sizeInSamples = pPort_info->internal_buf.size_in_bytes/(pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample));
             pPort_info->internal_buf.pInternalBufInfo->offsetInSamples = 0;
             memset(&pPort_info->internal_buf.pInternalBufInfo->flags, 0 , sizeof(capi_v2_stream_flags_t));
             pPort_info->internal_buf.pInternalBufInfo->timestamp = 0;
          }

          pPort_info->eos_flag = FALSE;
       }

    }

    {
        // read buffer from input buffer to intermediate buffers and write data from intermediate buffers to output buffer
        topo_bg_read_write_buffers(pMe, inbuf_ptr, outbuf_ptr);

        // check if sufficient data is collected on all active input ports
        if(topo_bg_is_req_data_collected(pMe))
        {
           topo_bg_thread_wait_for_processing_done(pMe);

           for(in_port_idx = 0; in_port_idx < pMe->numActiveInPorts; in_port_idx++)
           {
              pPort_info = &pMe->pIn_port_info[in_port_idx];

              if(topo_bg_any_main_th_event_pending((void *)pMe, in_port_idx))
              {
                  pMe->topo_algo_delay = topo_get_algo_delay(pMe->topo_ptr);
                  for(out_port_idx = 0; out_port_idx < pMe->numActiveOutPorts; out_port_idx++)
                  {
                     outbuf_ptr[out_port_idx]->timestamp = inbuf_ptr[TOPO_BG_PRIMARY_PORT]->timestamp - inPortIntBufdelay - pMe->pOut_port_info[out_port_idx].bg_thread_port_delay - pMe->topo_algo_delay;
                     outbuf_ptr[out_port_idx]->flags = inbuf_ptr[TOPO_BG_PRIMARY_PORT]->flags;
                  }

                  // Dont initiate process yet, indicate caller about data being read
                  return ADSP_EOK;
              }

              pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].timestamp = inbuf_ptr[in_port_idx]->timestamp;
              pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].flags = inbuf_ptr[in_port_idx]->flags;

              if(topo_bg_is_port_or_stream_active(pMe, in_port_idx))
              {
                 if(pPort_info->internal_buf.pInternalBufInfo->pBuf != NULL)
                 {
                    // In fractional sampling rate case memcpy is required, direct pointers can not be exchanged
                    topo_bg_copy_buffers(pPort_info);
                 }
                 else
                 {
                    exchange_buffer(&pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX], &pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX]);
                    pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples = 0;
                    pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples = 0;
                    pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].flags = pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].flags;
                    pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].timestamp = pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].timestamp;
                 }
              }
              else
              {
                 // Make size in samples 0 if port is not active, this will make sure that modules will not recieve any data on this port
                 pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples = 0;
				 pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples = 0;
                 pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples = 0;
				 pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples = 0;
              }

           }

           for(out_port_idx = 0; out_port_idx < pMe->numActiveOutPorts; out_port_idx++)
           {
              pPort_info = &pMe->pOut_port_info[out_port_idx];
              exchange_buffer(&pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX], &pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX]);
              if(pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples == pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples)
              {
                  // set sizeinsamples for main output buffer once we have valid data in BG thread output buffer
                  // this will make sure that there is no additional buffering delay in non-real time use cases 
                  pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples = pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples;
              }
              pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples = 0;
              pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples = 0;
              pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].flags = pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].flags;
              pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].timestamp = pPort_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].timestamp;
           }

           pMe->num_unit_frame_offset = 0;  // clear the num unit frame offset
#ifdef DBG_BG_THREAD
           MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_thread_start_process : call bg thread process now", pMe->main_th_objId);
#endif
           // signal background thread to start processing
           qurt_elite_signal_clear(pMe->th.done_signal_ptr);
           //pMe->is_bg_thread_processing = TRUE;
           qurt_elite_signal_send(pMe->th.process_signal_ptr);

        }
        else if(pMe->allow_partial_processing)  // check if process should be enabled, if topo is stuck due to some samples
        {
            topo_bg_thread_wait_for_processing_done(pMe);
#ifdef DBG_BG_THREAD
           MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_thread_start_process : call bg thread process now with partial samples", pMe->main_th_objId);
#endif
           // signal background thread to start processing
           qurt_elite_signal_clear(pMe->th.done_signal_ptr);
           qurt_elite_signal_send(pMe->th.process_signal_ptr);
           pMe->allow_partial_processing = FALSE;
        }
    }

    pMe->topo_algo_delay = topo_get_algo_delay(pMe->topo_ptr);

    for(out_port_idx = 0; out_port_idx < pMe->numActiveOutPorts; out_port_idx++)
    {
       outbuf_ptr[out_port_idx]->timestamp = inbuf_ptr[TOPO_BG_PRIMARY_PORT]->timestamp - inPortIntBufdelay - pMe->pOut_port_info[out_port_idx].bg_thread_port_delay - pMe->topo_algo_delay;
       outbuf_ptr[out_port_idx]->flags = inbuf_ptr[TOPO_BG_PRIMARY_PORT]->flags;
    }

    return result;
}

static bool_t topo_bg_is_req_data_collected(topo_bg_thread_struct_t *pMe)
{
   AudPP_BufInfo_t *pBuf_info_main;

   for(uint32_t in_port_idx = 0; in_port_idx < pMe->numActiveInPorts; in_port_idx++)
   {
      if(topo_bg_is_port_or_stream_active(pMe, in_port_idx))
      {
         pBuf_info_main = &pMe->pIn_port_info[in_port_idx].pBufInfo[TOPO_BG_MAIN_BUF_IDX];
         if(pBuf_info_main->offsetInSamples != pBuf_info_main->sizeInSamples)
         {
            return FALSE;
         }
      }
   }

   return TRUE;
}

static void topo_bg_int_to_proc_copy_buffers(topo_bg_thread_port_info_t *pIn_port_info)
{
    uint32_t bytes_written = 0, i;
    AdspAudioMediaFormatInfo_t *media_fmt = &pIn_port_info->media_format;
    //AdspAudioMediaFormatInfo_t *media_fmt = (pIn_port_info->pending_event_flag == TRUE)?(&pIn_port_info->cached_media_fmt):(&pIn_port_info->media_format);

    bool_t is_interleaved = (media_fmt->isInterleaved == 1);
    uint32_t write_offset = (pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples == pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples)?0:pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples;
    write_offset = write_offset * topo_get_bytes_per_sample(media_fmt->bitsPerSample);
    uint32_t int_buff_offset_per_ch = pIn_port_info->internal_buf.pInternalBufInfo->sizeInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);     // per channel data len of bg thread in bytes
    uint32_t new_proc_buff_size_in_bytes_per_ch;
    uint32_t proc_buff_offset;
    uint32_t unread_proc_data;
    proc_buff_offset = pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);
    unread_proc_data = (pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples - pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples) * topo_get_bytes_per_sample(media_fmt->bitsPerSample);

    if(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples == 0)
      {
        unread_proc_data = 0;
      }

    new_proc_buff_size_in_bytes_per_ch = int_buff_offset_per_ch + unread_proc_data;


#ifdef DBG_BG_THREAD
              MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_int_to_proc_copy_buffers: PROC buffer write offset is %lu and unread data is %lu", write_offset, unread_proc_data);
#endif


    // first copy data from internal buffer to PROC buffer
    if(TRUE == is_interleaved)
    {
        memsmove(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf,
                                 unread_proc_data * media_fmt->channels,
                                 pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + proc_buff_offset * media_fmt->channels,
                                 unread_proc_data * media_fmt->channels
                                 );

       bytes_written = memscpy(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + (unread_proc_data * media_fmt->channels),
                               pIn_port_info->frame_size_in_bytes - (unread_proc_data * media_fmt->channels),
                               pIn_port_info->internal_buf.pInternalBufInfo->pBuf,
                               int_buff_offset_per_ch * media_fmt->channels
                               );
       bytes_written = bytes_written/media_fmt->channels;

    }
    else
    {
        if(new_proc_buff_size_in_bytes_per_ch > write_offset)
        {
           for(i=0; i < media_fmt->channels; i++)
           {
               memsmove(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + ((media_fmt->channels - 1 - i)*new_proc_buff_size_in_bytes_per_ch),
                                 unread_proc_data,
                                 pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf    + ((media_fmt->channels - 1 - i)*write_offset) + proc_buff_offset,
                                 unread_proc_data
                                 );
           }
        }
        else
        {
           for(i=0; i < media_fmt->channels; i++)
           {
               memsmove(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + (i*new_proc_buff_size_in_bytes_per_ch),
                                   unread_proc_data,
                                   pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf    + (i*write_offset) + proc_buff_offset,
                                   unread_proc_data
                                   );
           }
       }

       for(i=0; i < media_fmt->channels; i++)
       {
           // this is assuming that BG thread buffers will be greater than internal buffer size
           bytes_written = memscpy(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + (i*new_proc_buff_size_in_bytes_per_ch) + unread_proc_data,
                                (pIn_port_info->frame_size_in_bytes/media_fmt->channels) - unread_proc_data,
                                pIn_port_info->internal_buf.pInternalBufInfo->pBuf    + (i*int_buff_offset_per_ch),
                                int_buff_offset_per_ch
                                );
       }

    }

#ifdef DBG_BG_THREAD
           MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_int_to_proc_copy_buffers: %lu bytes transfered from internal buffer to PROC buffer", bytes_written);
#endif

    if(new_proc_buff_size_in_bytes_per_ch != 0)
    {
       pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples = new_proc_buff_size_in_bytes_per_ch/topo_get_bytes_per_sample(media_fmt->bitsPerSample);
       pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples = 0;
    }
    pIn_port_info->internal_buf.pInternalBufInfo->sizeInSamples = 0;

    return;
}

// 1) First copy data from internal buffer to BG thread PROC buffer
// 2) Copy data from MAIN buffer to PROC buffer untill it is full
// 3) then copy remaining data from MAIN buffer to internal buffer
static void topo_bg_copy_buffers(topo_bg_thread_port_info_t *pIn_port_info)
{
   uint32_t bytes_written = 0, bytes_moved = 0, i;
   AdspAudioMediaFormatInfo_t *media_fmt = &pIn_port_info->media_format;
   //AdspAudioMediaFormatInfo_t *media_fmt = (pIn_port_info->pending_event_flag == TRUE)?(&pIn_port_info->cached_media_fmt):(&pIn_port_info->media_format);

   bool_t is_interleaved = (media_fmt->isInterleaved == 1);
   uint32_t write_offset;
   uint32_t bg_thread_in_buff_size_per_ch = pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);   // per channel data len of bg thread in bytes
   uint32_t valid_data_to_read = pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples  * topo_get_bytes_per_sample(media_fmt->bitsPerSample);
   uint32_t unread_proc_data;
   uint32_t proc_buff_offset;

   topo_bg_int_to_proc_copy_buffers(pIn_port_info);

   // If PROC buffer offset and size in samples are same that means entire proc buffer has been consumed by topo
   // so we can start writing from 0 offset, else do following :
   // 1) shift the unread data to the begining of the buffer
   // 2) start copying data after that offset
   write_offset = (pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples == pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples)?0:pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples;
   write_offset = write_offset * topo_get_bytes_per_sample(media_fmt->bitsPerSample);
   proc_buff_offset = pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);
   unread_proc_data = (pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples - pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples) * topo_get_bytes_per_sample(media_fmt->bitsPerSample);

#ifdef DBG_BG_THREAD
              MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_copy_buffers: PROC buffer write offset is %lu and unread data is %lu", write_offset, unread_proc_data);
#endif

   if(TRUE == is_interleaved)
   {
      memsmove(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf,
                               unread_proc_data * media_fmt->channels,
                               pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + proc_buff_offset * media_fmt->channels,
                               unread_proc_data * media_fmt->channels
                               );

      bytes_written = memscpy(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + (unread_proc_data * media_fmt->channels),
                                pIn_port_info->frame_size_in_bytes - (unread_proc_data * media_fmt->channels),
                                pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf,
                                valid_data_to_read * media_fmt->channels
                                );
      bytes_written = bytes_written/media_fmt->channels;
   }
   else
   {
       // here bg_thread_in_buff_size_per_ch will always be greater than write_offset, so copying in reverse order
      for(i=0; i < media_fmt->channels; i++)
      {
          memsmove(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + ((media_fmt->channels - 1 - i)*bg_thread_in_buff_size_per_ch),
                               unread_proc_data,
                               pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf    + ((media_fmt->channels - 1 - i)*write_offset) + proc_buff_offset,
                               unread_proc_data
                               );
      }

      for(i=0; i < media_fmt->channels; i++)
      {
          bytes_written = memscpy(pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].pBuf + (i*bg_thread_in_buff_size_per_ch) + unread_proc_data,
                                  bg_thread_in_buff_size_per_ch - unread_proc_data,
                                  pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf  + (i*bg_thread_in_buff_size_per_ch),
                                  valid_data_to_read
                                  );
      }
   }

#ifdef DBG_BG_THREAD
              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_copy_buffers: %lu bytes transfered from MAIN buffer to PROC buffer", bytes_written);
#endif

   pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples = 0; //(write_offset == 0)?0:pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].offsetInSamples;
   pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].sizeInSamples = pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples;

   if(TRUE == is_interleaved)
   {
      bytes_written = bytes_written * media_fmt->channels;
      bytes_moved = memscpy(pIn_port_info->internal_buf.pInternalBufInfo->pBuf,
                            pIn_port_info->internal_buf.size_in_bytes,
                            pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf + bytes_written,
                            (valid_data_to_read * media_fmt->channels) - bytes_written
                            );
      bytes_moved = bytes_moved/media_fmt->channels;
   }
   else
   {
      uint32_t int_buf_ch_offset = topo_bg_min_32((pIn_port_info->internal_buf.size_in_bytes/media_fmt->channels), valid_data_to_read - bytes_written);
      for(i=0; i < media_fmt->channels; i++)
      {
          bytes_moved = memscpy(pIn_port_info->internal_buf.pInternalBufInfo->pBuf + (i*int_buf_ch_offset),
                                  int_buf_ch_offset,
                                  pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf + (i*bg_thread_in_buff_size_per_ch) + bytes_written,
                                  int_buf_ch_offset
                                  );

      }
   }

#ifdef DBG_BG_THREAD
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_copy_buffers: %lu bytes transfered from MAIN buffer to internal buffer", bytes_moved);
#endif

   pIn_port_info->internal_buf.pInternalBufInfo->sizeInSamples = (bytes_moved/topo_get_bytes_per_sample(media_fmt->bitsPerSample));

   pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples = 0;

   pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].flags = pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].flags;
   pIn_port_info->pBufInfo[TOPO_BG_PROC_BUF_IDX].timestamp = pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].timestamp;

   return;
}

static void exchange_buffer(AudPP_BufInfo_t *BufInfo1, AudPP_BufInfo_t *BufInfo2)
{
     int8_t *TempBuf;

     TempBuf = BufInfo1->pBuf;
     BufInfo1->pBuf = BufInfo2->pBuf;
     BufInfo2->pBuf = TempBuf;
     return;
}

static uint32_t topo_bg_read_input_buffer(topo_bg_thread_port_info_t *pIn_port_info, AudPP_BufInfo_t *inbuf_ptr, uint32_t max_samples_to_read)
{
    AdspAudioMediaFormatInfo_t *media_fmt = (pIn_port_info->dataQ_pending_event_flag == TRUE)?(&pIn_port_info->cached_media_fmt):(&pIn_port_info->media_format);
    uint32_t bg_thread_in_buff_offset_per_ch = pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);   // per channel data len of bg thread in bytes

    uint32_t bytes_read = 0;

    uint32_t bytes_to_read_per_ch= max_samples_to_read * topo_get_bytes_per_sample(media_fmt->bitsPerSample);
    uint32_t main_thread_in_buff_offset_per_ch = inbuf_ptr->sizeInSamples*topo_get_bytes_per_sample(media_fmt->bitsPerSample);
    uint32_t inbuf_ch_offset = inbuf_ptr->offsetInSamples*topo_get_bytes_per_sample(media_fmt->bitsPerSample);

    bytes_to_read_per_ch = (bytes_to_read_per_ch > (main_thread_in_buff_offset_per_ch - inbuf_ch_offset))?(main_thread_in_buff_offset_per_ch - inbuf_ch_offset):bytes_to_read_per_ch;

    uint32_t port_buff_ch_offset =  pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);

   if(media_fmt->isInterleaved == 1)
   {
        bytes_read = memscpy(pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf + (port_buff_ch_offset*media_fmt->channels),
                                  (bg_thread_in_buff_offset_per_ch -  port_buff_ch_offset)*media_fmt->channels,
                                  inbuf_ptr->pBuf + (inbuf_ch_offset*media_fmt->channels),
                                  bytes_to_read_per_ch * media_fmt->channels);
        bytes_read = bytes_read/media_fmt->channels;
   }
   else
   {
     for(uint32_t i = 0; i < media_fmt->channels; i++)
     {
        bytes_read = memscpy(pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf + (i*bg_thread_in_buff_offset_per_ch) + port_buff_ch_offset,
                                  bg_thread_in_buff_offset_per_ch -  port_buff_ch_offset,
                                  inbuf_ptr->pBuf + (i*main_thread_in_buff_offset_per_ch) + inbuf_ch_offset,
                                  bytes_to_read_per_ch);
     }
   }

   pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples += (bytes_read/topo_get_bytes_per_sample(media_fmt->bitsPerSample));
   pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].flags = inbuf_ptr->flags;
   pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].timestamp = inbuf_ptr->timestamp;
   inbuf_ptr->offsetInSamples += (bytes_read/topo_get_bytes_per_sample(media_fmt->bitsPerSample));
   return  (bytes_read/topo_get_bytes_per_sample(media_fmt->bitsPerSample));
}

#if 0
static void topo_bg_read_input_buffer_eos(topo_bg_thread_port_info_t *pIn_port_info, AudPP_BufInfo_t *inbuf_ptr, uint32_t samples_to_read)
{

    AdspAudioMediaFormatInfo_t *media_fmt = (pIn_port_info->pending_event_flag == TRUE)?(&pIn_port_info->cached_media_fmt):(&pIn_port_info->media_format);
    uint32_t bg_thread_in_buff_offset_per_ch = pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);   // per channel data len of bg thread in bytes

    uint32_t bytes_to_read_per_ch= samples_to_read * topo_get_bytes_per_sample(media_fmt->bitsPerSample);
    uint32_t port_buff_ch_offset =  pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples * topo_get_bytes_per_sample(media_fmt->bitsPerSample);

    if(media_fmt->isInterleaved == 1)
    {
        memset(pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf + (port_buff_ch_offset*media_fmt->channels), 0, bytes_to_read_per_ch*media_fmt->channels);
    }
    else
    {
        for(uint32_t i = 0; i < media_fmt->channels; i++)
        {
            memset(pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf + (i*bg_thread_in_buff_offset_per_ch) + port_buff_ch_offset, 0, bytes_to_read_per_ch);
        }
    }
    pIn_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples += (bytes_to_read_per_ch/topo_get_bytes_per_sample(media_fmt->bitsPerSample));

    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_read_input_buffer_eos : read 0s %lu bytes", bytes_to_read_per_ch);
    return;
}
#endif

static uint32_t topo_bg_write_output_buffer(topo_bg_thread_port_info_t *pOut_port_info, AudPP_BufInfo_t *outbuf_ptr, uint32_t max_samples_to_write)
{
    uint32_t bg_thread_out_buff_offset_per_ch = pOut_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples*topo_get_bytes_per_sample(pOut_port_info->media_format.bitsPerSample);;   // per channel data len of bg thread in bytes
    uint32_t bytes_written = 0;
    uint32_t bytes_to_write_per_ch= max_samples_to_write * topo_get_bytes_per_sample(pOut_port_info->media_format.bitsPerSample);
    uint32_t main_thread_out_buff_offset_per_ch = outbuf_ptr->sizeInSamples*topo_get_bytes_per_sample(pOut_port_info->media_format.bitsPerSample);
    uint32_t outbuf_ch_offset = outbuf_ptr->offsetInSamples*topo_get_bytes_per_sample(pOut_port_info->media_format.bitsPerSample);

    bytes_to_write_per_ch = (bytes_to_write_per_ch > (main_thread_out_buff_offset_per_ch - outbuf_ch_offset))?(main_thread_out_buff_offset_per_ch - outbuf_ch_offset):bytes_to_write_per_ch;

    uint32_t port_buff_ch_offset =  pOut_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples * topo_get_bytes_per_sample(pOut_port_info->media_format.bitsPerSample);

    if(pOut_port_info->media_format.isInterleaved == 1)
    {
         bytes_written = memscpy(outbuf_ptr->pBuf + (outbuf_ch_offset*pOut_port_info->media_format.channels),
                                bytes_to_write_per_ch*pOut_port_info->media_format.channels,
                                pOut_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf + (port_buff_ch_offset*pOut_port_info->media_format.channels),
                                (bg_thread_out_buff_offset_per_ch - port_buff_ch_offset)*pOut_port_info->media_format.channels);
        bytes_written = bytes_written/pOut_port_info->media_format.channels;
    }
    else
    {
        for(uint32_t i = 0; i < pOut_port_info->media_format.channels; i++)
        {
            bytes_written = memscpy(outbuf_ptr->pBuf + (i*main_thread_out_buff_offset_per_ch) + outbuf_ch_offset,
                                bytes_to_write_per_ch,
                                pOut_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf + (i*bg_thread_out_buff_offset_per_ch) + port_buff_ch_offset,
                                bg_thread_out_buff_offset_per_ch - port_buff_ch_offset);

        }
    }

    pOut_port_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples += (bytes_written/topo_get_bytes_per_sample(pOut_port_info->media_format.bitsPerSample));
    outbuf_ptr->offsetInSamples += (bytes_written/topo_get_bytes_per_sample(pOut_port_info->media_format.bitsPerSample));

    //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_write_output_buffer : write data %lu bytes", bytes_written);

    return (bytes_written/topo_get_bytes_per_sample(pOut_port_info->media_format.bitsPerSample));
}


static ADSPResult topo_bg_process(void *state)
{
    ADSPResult result = ADSP_EOK;
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)state;
    AudPP_BufInfo_t *pInputBufInfoProc[pMe->numActiveInPorts];
    AudPP_BufInfo_t *pOututBufInfoProc[pMe->numActiveOutPorts];
    bool_t is_topo_process_req = FALSE;

    // just for debugging : remove it later
    //topo_print_module_buffers_samples(pMe->topo_ptr); // rename it to print	

    // First check if any module input buffer has out unit frame worth of samples
    // If yes then call process with internal buffer as output buffer
    // After that call topo process with proc output buffer
    for(uint32_t out_port_idx = 0;out_port_idx < pMe->numActiveOutPorts; out_port_idx++)
    {
       AdspAudioMediaFormatInfo_t *media_fmt = &pMe->pOut_port_info[out_port_idx].media_format;
       if(topo_bg_is_fractional_sampling_rate(media_fmt->samplingRate))
       {
          // check if topo requires more data and there is no data with internal buffer as well
          if(topo_any_module_with_main_th_unit_frames(pMe->topo_ptr, out_port_idx))
          {
#ifdef DBG_BG_THREAD
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "topo_bg_process: module is present with main th output buffer unit frames");
#endif		  
             is_topo_process_req = TRUE;
          }
       }
    }

    if(is_topo_process_req)
    {
       for(uint32_t in_port_idx = 0; in_port_idx < pMe->numActiveInPorts; in_port_idx++)
       {
          pInputBufInfoProc[in_port_idx] = &pMe->pIn_port_info[in_port_idx].pBufInfo[TOPO_BG_PROC_BUF_IDX];
       }

       for(uint32_t out_port_idx = 0; out_port_idx < pMe->numActiveOutPorts; out_port_idx++)
       {
          pOututBufInfoProc[out_port_idx] = pMe->pOut_port_info[out_port_idx].internal_buf.pInternalBufInfo;
       }

       result = topo_process(pMe->topo_ptr,
                    pMe->numActiveInPorts,
                    pInputBufInfoProc,
                    pMe->numActiveOutPorts,
                    pOututBufInfoProc);

       if(ADSP_FAILED(result))
       {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%X topo_bg_process failed with result %lu",pMe->th.bg_th_objId ,result);
       }
    }

    for(uint32_t in_port_idx = 0; in_port_idx < pMe->numActiveInPorts; in_port_idx++)
    {
       pInputBufInfoProc[in_port_idx] = &pMe->pIn_port_info[in_port_idx].pBufInfo[TOPO_BG_PROC_BUF_IDX];
    }

    for(uint32_t out_port_idx = 0; out_port_idx < pMe->numActiveOutPorts; out_port_idx++)
    {
       pOututBufInfoProc[out_port_idx] = &pMe->pOut_port_info[out_port_idx].pBufInfo[TOPO_BG_PROC_BUF_IDX];
    }

    result = topo_process(pMe->topo_ptr,
                    pMe->numActiveInPorts,
                    pInputBufInfoProc,
                    pMe->numActiveOutPorts,
                    pOututBufInfoProc);

    if(ADSP_FAILED(result))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%X topo_bg_process failed with result %lu",pMe->th.bg_th_objId ,result);
    }

    topo_bg_process_int_buf_samples(pMe);

    // just for debugging : remove it later
    //topo_print_module_buffers_samples(pMe->topo_ptr);



    return result;
}


static void topo_bg_process_int_buf_samples(topo_bg_thread_struct_t *pMe)
{
   uint32_t in_port_idx;
   AdspAudioMediaFormatInfo_t *media_fmt;
   AudPP_BufInfo_t *pInputBufInfoProc[pMe->numActiveInPorts];
   AudPP_BufInfo_t *pOututBufInfoProc[pMe->numActiveOutPorts];
   ADSPResult result = ADSP_EOK;
   bool_t is_topo_process_req = FALSE;

   for(in_port_idx = 0;in_port_idx < pMe->numActiveInPorts; in_port_idx++)
   {
      media_fmt = &pMe->pIn_port_info[in_port_idx].media_format;
      if(topo_bg_is_fractional_sampling_rate(media_fmt->samplingRate))
      {
         // check if topo requires more data and there is no data with internal buffer as well
         if(topo_is_port_req_more_data(pMe->topo_ptr, in_port_idx))
         {
            topo_bg_int_to_proc_copy_buffers(&pMe->pIn_port_info[in_port_idx]);
            is_topo_process_req = TRUE;
         }
      }
   }

   if(is_topo_process_req)
   {
      for(uint32_t in_port_idx = 0; in_port_idx < pMe->numActiveInPorts; in_port_idx++)
      {
         pInputBufInfoProc[in_port_idx] = &pMe->pIn_port_info[in_port_idx].pBufInfo[TOPO_BG_PROC_BUF_IDX];
      }

      for(uint32_t out_port_idx = 0; out_port_idx < pMe->numActiveOutPorts; out_port_idx++)
      {
         pOututBufInfoProc[out_port_idx] = &pMe->pOut_port_info[out_port_idx].pBufInfo[TOPO_BG_PROC_BUF_IDX];
      }

      result = topo_process(pMe->topo_ptr,
                pMe->numActiveInPorts,
                pInputBufInfoProc,
                pMe->numActiveOutPorts,
                pOututBufInfoProc);

      if(ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%X topo_bg_process failed with result %lu",pMe->th.bg_th_objId ,result);
      }

   }

   return;
}


void topo_bg_thread_wait_for_processing_done(void *pInstance)
{
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

    if(NULL == pMe)
    {
       MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_thread_wait_for_processing_done: NULL Instance pointer error");
       return;
    }

    if(!pMe->thread_launched)
    {
        return ;
    }
    topo_bg_thread_wait_for_task_done(&pMe->th);
}

bool_t topo_is_bg_thread_created(void *pInstance)
{
   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;
   if(NULL == pMe)
   {
       return FALSE;
   }

   return pMe->thread_launched;

}


uint32_t topo_bg_thread_buffering_delay_us(void *pInstance)
{
    uint32_t delay_in_us = 0, port_delay = 0;

    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;
    topo_bg_thread_port_info_t *pPort_info;

    if(NULL == pMe)
    {
        MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_thread_start_process: NULL Instance pointer error");
        return 0;
    }

    for(uint32_t out_port_idx = 0; out_port_idx < pMe->numActiveOutPorts; out_port_idx++)
    {
       pPort_info = &pMe->pOut_port_info[out_port_idx];
       port_delay = TOPO_BG_TOTAL_OUT_BUFFERS* ((pPort_info->frame_size_in_bytes*1000*1000)/(pPort_info->media_format.samplingRate * pPort_info->media_format.channels * topo_get_bytes_per_sample(pPort_info->media_format.bitsPerSample)));
       delay_in_us = (delay_in_us > port_delay)?delay_in_us:port_delay;
    }

    return delay_in_us;

}

bool_t topo_bg_is_bg_thread_processing(void *pInstance)
{
   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;
   uint32_t mask;
   if(NULL == pMe)
   {
      return FALSE;
   }

   mask = qurt_elite_channel_poll(&pMe->th.done_signal_channel, TOPO_BG_PROCESS_DONE_MASK);

   if(mask)
   {
       return FALSE;        // done signal is set
   }
   else
   {
       return TRUE;        // done signal is not set, thread is procesing
   }
}

static uint32_t topo_bg_get_synced_inport_curr_offset(void *pInstance, uint32_t port_id)
{
    topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;
    //topo_bg_thread_port_info_t *pPort_info = &pMe->pOut_port_info[TOPO_BG_PRIMARY_PORT];
    //uint32_t curr_primary_out_offset = pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples;
    uint32_t unAfeFrameSizeInSamples_inport; //unAfeFrameSizeInSamples_outport;

    //elite_svc_get_frame_size(pPort_info->media_format.samplingRate,&unAfeFrameSizeInSamples_outport);
    elite_svc_get_frame_size(pMe->pIn_port_info[port_id].cached_media_fmt.samplingRate,&unAfeFrameSizeInSamples_inport);

    //return ((curr_primary_out_offset/unAfeFrameSizeInSamples_outport) * unAfeFrameSizeInSamples_inport);
    return pMe->num_unit_frame_offset * unAfeFrameSizeInSamples_inport;
}

ADSPResult topo_bg_set_media_type(void *pInstance, AdspAudioMediaFormatInfo_t *pNew_media_format,
                                       uint32_t port_id)
{
   ADSPResult result = ADSP_EOK;
   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;
   topo_bg_thread_port_info_t *pPort_info;

   if(NULL == pMe)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_set_media_type: NULL Instance pointer error");
      return ADSP_EFAILED;
   }

   if(port_id <= pMe->numActiveInPorts)
   {
	   uint32_t unAfeFrameSizeInSamples;
	   pPort_info = &pMe->pIn_port_info[port_id];
       memscpy(&pPort_info->cached_media_fmt, sizeof(AdspAudioMediaFormatInfo_t),pNew_media_format,sizeof(AdspAudioMediaFormatInfo_t));

       elite_svc_get_frame_size(pPort_info->cached_media_fmt.samplingRate,&unAfeFrameSizeInSamples);

       // reset only main buffer and re-allocate the size if media format is changed
       if(!(AudPP_MediaFormatsEqual(pNew_media_format, &pPort_info->media_format)))
       {
            uint32_t new_size;
            new_size = pMe->ouput_buffer_num_unit_frames * unAfeFrameSizeInSamples * pPort_info->cached_media_fmt.channels * topo_get_bytes_per_sample(pPort_info->cached_media_fmt.bitsPerSample);

            if(new_size != pPort_info->frame_size_in_bytes)
            {
               if(pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf != NULL)
               {
                  qurt_elite_memory_free(pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf);
                  pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf = NULL;
               }

               pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf = (int8_t *)qurt_elite_memory_malloc(new_size, pMe->heap_id);
               if(NULL == pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf)
               {
                   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_set_media_type: No Memory allocated", pMe->main_th_objId);
                   return ADSP_ENOMEMORY;
               }
            }

            memset(pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].pBuf, 0, new_size);
            pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples = topo_bg_get_synced_inport_curr_offset(pMe, port_id);
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_bg_set_media_type: new offset is  %lu",pMe->main_th_objId,pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].offsetInSamples);

            pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].sizeInSamples = pMe->ouput_buffer_num_unit_frames * unAfeFrameSizeInSamples;
            memset(&pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].flags, 0 , sizeof(capi_v2_stream_flags_t));
            pPort_info->pBufInfo[TOPO_BG_MAIN_BUF_IDX].timestamp = 0;

       }

   }
   else
   {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX topo_bg_set_media_type: unsupported port ID  %lu",pMe->main_th_objId,port_id);
       return ADSP_EUNSUPPORTED;
   }

    return result;
}

bool_t topo_bg_is_any_dataQ_event_pending(void *pInstance, uint32_t port_id)
{
   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

   if(NULL == pMe)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_is_any_dataQ_event_pending: NULL Instance pointer error");
      return FALSE;
   }

   if(port_id <= pMe->numActiveInPorts)
   {
       return pMe->pIn_port_info[port_id].dataQ_pending_event_flag;
   }
   return FALSE;
}

void topo_bg_set_dataQ_pending_flag(void *pInstance, bool_t flag, uint32_t port_id)
{
   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

   if(NULL == pMe)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_set_dataQ_pending_flag: NULL Instance pointer error");
      return;
   }

   if(port_id <= pMe->numActiveInPorts)
   {
      pMe->pIn_port_info[port_id].dataQ_pending_event_flag = flag;
   }
   return;
}

bool_t topo_bg_is_any_cmdQ_event_pending(void *pInstance, uint32_t port_id)
{
   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

   if(NULL == pMe)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_is_any_cmdQ_event_pending: NULL Instance pointer error");
      return FALSE;
   }

   if(port_id <= pMe->numActiveInPorts)
   {
       return pMe->pIn_port_info[port_id].cmdQ_pending_event_flag;
   }
   return FALSE;
}

void topo_bg_set_cmdQ_pending_flag(void *pInstance, bool_t flag, uint32_t port_id)
{
   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

   if(NULL == pMe)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_set_cmdQ_pending_flag: NULL Instance pointer error");
      return;
   }

   if(port_id <= pMe->numActiveInPorts)
   {
      pMe->pIn_port_info[port_id].cmdQ_pending_event_flag = flag;
   }
   return;
}

bool_t topo_bg_any_main_th_event_pending(void *pInstance, uint32_t port_id)
{
   topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

   if(NULL == pMe)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_any_main_th_event_pending: NULL Instance pointer error");
      return FALSE;
   }

   if(port_id <= pMe->numActiveInPorts)
   {
       return (pMe->pIn_port_info[port_id].cmdQ_pending_event_flag || pMe->pIn_port_info[port_id].dataQ_pending_event_flag);
   }
   return FALSE;
}

static uint32_t topo_bg_min_32(uint32_t var1, uint32_t var2)
{
   return ((var1 > var2)?var2:var1);
}

void topo_bg_send_eos_flag(void *pInstance, uint32_t port_id, bool_t is_eos)
{
  topo_bg_thread_struct_t *pMe = (topo_bg_thread_struct_t *)pInstance;

  if(NULL == pMe)
  {
     MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "topo_bg_send_eos_flag: NULL Instance pointer error");
     return;
  }

  if(port_id <= pMe->numActiveInPorts)
  {
      pMe->pIn_port_info[port_id].eos_flag = is_eos;
  }

  return;
}

static bool_t topo_bg_is_port_or_stream_active(topo_bg_thread_struct_t *pMe, uint32_t port_id)
{
   return (topo_is_port_or_stream_active(pMe->topo_ptr,port_id) && (!pMe->pIn_port_info[port_id].eos_flag));
}
