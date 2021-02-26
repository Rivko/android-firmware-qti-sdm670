/*========================================================================

*//** @file AudioEncSvc_PushMode.cpp
This file contains Elite Audio encoder service include components.

Copyright (c) 2013-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header:


when       who     what, where, why
--------   ---     -------------------------------------------------------
01/09/13   RB      Created file.

========================================================================== */

#include "AudioEncSvc_PushMode.h"
#include "AudioEncSvc_CapiV2Util.h"

static ADSPResult AudioEncSvc_CheckSendWaterMarkEvent(AudioEncSvc_t *pMe, uint32_t startLevel, uint32_t endLevel);

ADSPResult AudioEncSvc_InitPushMode(AudioEncSvc_t *pMe, AudioEncSvcInitParams_t *pInitParams)
{
   ADSPResult result;

   if (ENC_SVC_PUSH_MODE != pInitParams->encMode)
   {
      return ADSP_EOK;
   }

   if (!AudioEncSvc_IsPcmFmt(pInitParams->ulConfigId))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Push mode supported only for PCM formats.");
      return ADSP_EUNSUPPORTED;
   }

   pMe->push_mode_ptr = (enc_push_mode_t*) qurt_elite_memory_malloc(sizeof(enc_push_mode_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == pMe->push_mode_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for push mode!");
      return ADSP_ENOMEMORY;
   }
   memset(pMe->push_mode_ptr, 0, sizeof(enc_push_mode_t));

   pMe->push_mode_ptr->pos_buf_ptr   = pInitParams->push_mode.pos_buf_ptr;
   pMe->push_mode_ptr->shared_circ_buf_size = pInitParams->push_mode.shared_circ_buf_size;
   pMe->push_mode_ptr->shared_circ_buf_start_ptr = (uint8_t*)pInitParams->push_mode.shared_circ_buf_addr;

   //initialize the position buffer to zero
   memset(pMe->push_mode_ptr->pos_buf_ptr, 0, sizeof(asm_push_mode_read_position_buffer_t));

   pMe->push_mode_ptr->num_water_mark_levels = pInitParams->push_mode.num_water_mark_levels;
   if (pMe->push_mode_ptr->num_water_mark_levels > 0)
   {
      pMe->push_mode_ptr->water_mark_levels_ptr = (asm_push_mode_watermark_level_t*) qurt_elite_memory_malloc(
                                                      pMe->push_mode_ptr->num_water_mark_levels*
                                                      sizeof(asm_push_mode_watermark_level_t),
                                                      QURT_ELITE_HEAP_DEFAULT);

      if (NULL == pMe->push_mode_ptr->water_mark_levels_ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for water mark events!");
         return ADSP_ENOMEMORY;
      }
      for (uint32_t i = 0; i < pMe->push_mode_ptr->num_water_mark_levels; i++)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Water Mark Level %lu",
               pInitParams->push_mode.water_mark_levels_ptr[i].watermark_level_bytes);

         if ( pInitParams->push_mode.water_mark_levels_ptr[i].watermark_level_bytes < pMe->push_mode_ptr->shared_circ_buf_size)
         {
            pMe->push_mode_ptr->water_mark_levels_ptr[i].watermark_level_bytes = pInitParams->push_mode.water_mark_levels_ptr[i].watermark_level_bytes;
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid water mark level. ");
            return ADSP_EFAILED;
         }
      }
   }

   asm_multi_channel_pcm_enc_cfg_v4_t *pPcm = &pInitParams->push_mode.EncCfg.pcm;
   uint32_t unit_size = pPcm->sample_word_size/8 * pPcm->num_channels;

   if (pMe->push_mode_ptr->shared_circ_buf_size % unit_size)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Circular buffer size %lu is not a multiple of (sample word size %u * channels %u) ",
            pMe->push_mode_ptr->shared_circ_buf_size, pPcm->sample_word_size, pPcm->num_channels);
      return ADSP_EFAILED;
   }

   pMe->bWaitForEncCfg = FALSE;

   /** register for AV timer */
   avtimer_open_param_t open_param;
   open_param.client_name = (char*)"AEnc";
   open_param.flag = 0;
   pMe->push_mode_ptr->avt_drv_handle = NULL;
   if (ADSP_EOK != (result = avtimer_drv_hw_open(&(pMe->push_mode_ptr->avt_drv_handle), &open_param)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Enc Svc: Failed to Open AVTimer Driver for input port with result: %d",result);
      return result;
   }

   return result;
}

void AudioEncSvc_DeInitPushMode(AudioEncSvc_t *pMe)
{
   if (pMe->push_mode_ptr)
   {
      if (pMe->push_mode_ptr->water_mark_levels_ptr)
      {
         qurt_elite_memory_free(pMe->push_mode_ptr->water_mark_levels_ptr);
         pMe->push_mode_ptr->water_mark_levels_ptr = NULL;
      }
      /** deregister for AV timer */
      if (NULL != pMe->push_mode_ptr->avt_drv_handle)
      {
         ADSPResult result;
         if (ADSP_EOK != (result = avtimer_drv_hw_close(pMe->push_mode_ptr->avt_drv_handle)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Enc Svc: Failed to Close AVTimer Driver for input port with result: %d ",result);
         }
         pMe->push_mode_ptr->avt_drv_handle = NULL;
      }
      qurt_elite_memory_free(pMe->push_mode_ptr);
      pMe->push_mode_ptr = NULL;
   }
}

ADSPResult AudioEncSvc_SuspendPushMode(AudioEncSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   if ((NULL != pMe->push_mode_ptr) &&
       (NULL != pMe->push_mode_ptr->avt_drv_handle))
   {
      if (ADSP_EOK != (result = avtimer_drv_hw_close(pMe->push_mode_ptr->avt_drv_handle)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc :[SUSPEND] Failed to Close AVTimer Driver with result: %d",result);
         return result;
      }
      else
      {
         pMe->push_mode_ptr->avt_drv_handle = NULL;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc :[SUSPEND] AVTimer Driver handle is null");
   }
   return result;
}

ADSPResult AudioEncSvc_ResumePushMode(AudioEncSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   if ((NULL != pMe->push_mode_ptr) &&
       (NULL == pMe->push_mode_ptr->avt_drv_handle))
   {
      /** Re-register avtimer handle after suspend */
      avtimer_open_param_t open_param;
      open_param.client_name = (char*)"ADec";
      open_param.flag = 0;
      if (ADSP_EOK != (result = avtimer_drv_hw_open(&(pMe->push_mode_ptr->avt_drv_handle), &open_param)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc :[RESUME] Failed to Open AVTimer Driver with result: %d",result);
         return result;
      }
   }
   return result;
}
/*===========================================================================
FUNCTION       AudioEncSvc_ProcessPCMPushMode()

DESCRIPTION
   1 Called either from dataQ handler/ BufQ handler for PCM format
   2 If input and output buffer are availabe then copies whatever possible
     from input buffer to output buffer. If input is deinterleaved, interleave
     when copying.
   3 If input buffer is completely exhausted release it
   4 If output buffer is completely filled release it after filling metadata
     and start listening for output buffer

PARAMETERS
Input:  pointer to AudioEncSvc_t

RETURN VALUE
result : ADSP_EOK/ADSP_EFAILED
===========================================================================*/
ADSPResult AudioEncSvc_ProcessPCMPushMode(AudioEncSvc_t* pMe)
{

   ADSPResult result = ADSP_EOK;
   elite_msg_data_buffer_t* inp_buf_ptr = (elite_msg_data_buffer_t*) pMe->InpDataMsg.pPayload;
   uint32_t samples_to_copy;
   int num_channels, out_bits_per_sample;
   uint32_t inp_buffer_rd_index;

   uint32_t in_bytes_per_sample, out_bytes_per_sample;
   uint32_t out_buf_num_empty_samples;

   enc_push_mode_t *push_mode_ptr = pMe->push_mode_ptr;
   asm_push_mode_read_position_buffer_t *pos_buf_ptr = push_mode_ptr->pos_buf_ptr;
   uint32_t write_index = pos_buf_ptr->write_index, temp_wr_ind;
   uint32_t rem_lin_size;

   int8_t *src_ptr, *dest_ptr;

   /* ASM open write sets  MXAT out bps == PP out bps == AudEnc in bps */
   in_bytes_per_sample = (pMe->in_med_fmt.bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   //Set command signal by default
   pMe->unCurrentBitfield = AUD_ENC_CMD_SIG;

   if (pMe->endOfStream)
   {
      return AudioEncSvc_ProcessEosCmd(pMe);
   }

   //check of any data left in input buffer
   if( (inp_buf_ptr == NULL) || (inp_buf_ptr->nActualSize == 0))
   {
		pMe->unCurrentBitfield |= AUD_ENC_DATA_SIG;
      return result;
   }

   num_channels = pMe->out_med_fmt_per_enc_cfg.num_channels;
   out_bits_per_sample = pMe->out_med_fmt_per_enc_cfg.bits_per_sample;

   out_bytes_per_sample = pMe->out_med_fmt_per_enc_cfg.bytes_per_sample;

   //copy from input to output buffer
   inp_buffer_rd_index = 0;

   //if deinterleaved then set read index accordingly
   if(!pMe->in_med_fmt.is_interleaved)
   {
      inp_buffer_rd_index = inp_buffer_rd_index/num_channels;
   }

   uint32_t remaining_in_size = inp_buf_ptr->nActualSize;
   while (remaining_in_size > 0)
   {
      rem_lin_size = push_mode_ptr->shared_circ_buf_size - write_index;

      out_buf_num_empty_samples = rem_lin_size / out_bytes_per_sample;

      samples_to_copy = remaining_in_size/in_bytes_per_sample;
      samples_to_copy = (samples_to_copy > out_buf_num_empty_samples) ? out_buf_num_empty_samples : samples_to_copy;

      src_ptr =  ((int8_t *)&inp_buf_ptr->nDataBuf) + (inp_buffer_rd_index * in_bytes_per_sample);

      dest_ptr = (int8_t *)(push_mode_ptr->shared_circ_buf_start_ptr + write_index);

      //printf("samples_to_copy=%lu, actual size = %ld write_index %lu, num_channels %d, out_bits_per_sample %d out_bytes_per_sample %lu is_interleaved = %u, remaining_in_size=%lu\n",samples_to_copy,
            //((elite_msg_data_buffer_t*) pMe->InpDataMsg.pPayload)->nActualSize, write_index, num_channels, out_bits_per_sample, out_bytes_per_sample,pMe->in_med_fmt.is_interleaved, remaining_in_size);

      if(pMe->out_med_fmt_per_enc_cfg.endianness == ENC_BIG_ENDIAN)
      {
         if(ADSP_FAILED(result = endianness_convertor_with_intlv_in((int8_t *)src_ptr, samples_to_copy, in_bytes_per_sample*8)))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc:Failed to convert endianness of the input samples.");
            return result;
         }
      }

      //copy from input to output buffer: below function only operates on linear buffers.
      AudioEncSvc_CopyWithInterleaving(pMe, num_channels, samples_to_copy, src_ptr,
                                       dest_ptr, out_bytes_per_sample, in_bytes_per_sample,
                                       out_bytes_per_sample*8-1, out_bits_per_sample);

      AudioEncSvc_LogOutputData(dest_ptr, samples_to_copy*out_bytes_per_sample, pMe);

      remaining_in_size -= (samples_to_copy*in_bytes_per_sample);

      if(pMe->in_med_fmt.is_interleaved)
      {
         inp_buffer_rd_index += samples_to_copy;
      }
      else
      {
         inp_buffer_rd_index += (samples_to_copy/num_channels);
      }

      temp_wr_ind = write_index;
      write_index += (samples_to_copy * out_bytes_per_sample);

      AudioEncSvc_CheckSendWaterMarkEvent(pMe, temp_wr_ind, write_index);

      if (write_index >= push_mode_ptr->shared_circ_buf_size)
      {
         write_index = 0;
      }
   }

   //if input buffer is exhausted release it (well, it should be)
   if(remaining_in_size == 0)
   {
      if (ADSP_FAILED(result = AudioEncSvc_ReleaseInputBuf(pMe, ADSP_EOK) ) )
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudEncSvc fail to release input data message %d", result);
      }
      else
      {
         pMe->unCurrentBitfield |= AUD_ENC_DATA_SIG;
      }
   }

   //Udpate the position buffer
   uint64_t time = avtimer_drv_get_time(pMe->push_mode_ptr->avt_drv_handle);
   uint32_t prev_frame_counter      = pos_buf_ptr->frame_counter;

   /** below is a critical section which is executed at high prio so that if the client is also trying to read
    *  we will try to complete is ASAP & they don't have to wait for longer
    */
   qurt_elite_thread_set_prio(elite_get_thrd_prio(ELITETHREAD_DEFAULT_IST_PRIO_ID));
   pos_buf_ptr->frame_counter       = 0;
   pos_buf_ptr->write_index          = write_index;
   pos_buf_ptr->wall_clock_us_lsw   = (uint32_t)time;
   pos_buf_ptr->wall_clock_us_msw   = (uint32_t)(time>>32);
   pos_buf_ptr->frame_counter       = prev_frame_counter+1;
   qurt_elite_thread_set_prio(AudioEncSvc_GetThreadPriority(pMe));

   return ADSP_EOK;
}

static ADSPResult AudioEncSvc_CheckSendWaterMarkEvent(AudioEncSvc_t *pMe, uint32_t startLevel, uint32_t endLevel)
{
   ADSPResult result;
   //raise event only if it is enabled
   if(pMe->push_mode_ptr->num_water_mark_levels == 0)
   {
      return ADSP_EOK;
   }

   uint32_t level;

   for (uint32_t i = 0; i < pMe->push_mode_ptr->num_water_mark_levels; i++)
   {
      level = pMe->push_mode_ptr->water_mark_levels_ptr[i].watermark_level_bytes;

      /** If a certain level is within start and end, then we are crossing the level, hence raise an event*/
      if ( (level >= startLevel) && (level < endLevel) )
      {
         //form the payload for the event
         asm_data_event_watermark_t waterMarkEvent;
         waterMarkEvent.watermark_level_bytes = level;

         result = AudioStreamMgr_GenerateClientEventFromCb(&pMe->CbData,
               ASM_DATA_EVENT_WATERMARK,
               pMe->CbData.unAsynClientToken,
               &waterMarkEvent,
               sizeof(waterMarkEvent));
         if (ADSP_FAILED(result))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send water mark event!");
         }
         else
         {
            MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Enc [thread-id 0x%lx]: Sent Water Mark event to the client start %lu, level %lu, end %lu", pMe->serviceHandle.threadId, startLevel, level, endLevel);
         }
      }
   }
   return ADSP_EOK;
}

void AudioEncSvc_ResetPushMode(AudioEncSvc_t *pMe)
{
   if (pMe->push_mode_ptr)
   {
      asm_push_mode_read_position_buffer_t *pos_buf_ptr = pMe->push_mode_ptr->pos_buf_ptr;
      pos_buf_ptr->frame_counter = 0;
      pos_buf_ptr->write_index = 0;
      pos_buf_ptr->wall_clock_us_lsw = 0;
      pos_buf_ptr->wall_clock_us_msw = 0;

   }
}

#ifdef ELITE_CAPI_H
#error "Do not include CAPI V1 in this file"
#endif
