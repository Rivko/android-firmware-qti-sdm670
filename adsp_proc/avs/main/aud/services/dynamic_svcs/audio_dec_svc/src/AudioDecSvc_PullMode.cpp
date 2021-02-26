/*========================================================================

 *//**
This file contains functions for Pull mode playbacl.

Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header:


when       who     what, where, why
--------   ---     -------------------------------------------------------
01/08/13    Rbhatnk      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioDecSvc_PullMode.h"
#include "AudioDecSvc_ChanMap.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioDecSvc_CapiV2Util.h"
#include "AudioDecSvc_Util.h"

static ADSPResult AudioDecSvc_CheckSendWaterMarkEvent(AudioDecSvc_t *pMe,  AudioDecSvc_InpStream_t *pInpStrm, uint32_t startLevel, uint32_t endLevel);

static ADSPResult AudioDecSvc_RegisterAvTimer(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   if (NULL != pInpStrm->pull_mode_ptr->avt_drv_handle)
   {
      return ADSP_EOK; //already registered.
   }

   ADSPResult result;
   avtimer_open_param_t open_param;
   open_param.client_name = (char*)"ADec";
   open_param.flag = 0;
   if (ADSP_EOK != (result = avtimer_drv_hw_open(&(pInpStrm->pull_mode_ptr->avt_drv_handle), &open_param)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dec Svc: Failed to Open AVTimer Driver for input port with result: %d",result);
      return result;
   }
   return result;
}

static ADSPResult AudioDecSvc_DeregisterAvTimer(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult result = ADSP_EOK;
   if (NULL != pInpStrm->pull_mode_ptr->avt_drv_handle)
   {
      if (ADSP_EOK != (result = avtimer_drv_hw_close(pInpStrm->pull_mode_ptr->avt_drv_handle)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dec Svc: Failed to Close AVTimer Driver for input port with result: %d",result);
      }
      pInpStrm->pull_mode_ptr->avt_drv_handle = NULL;
   }
   return result;
}

ADSPResult AudioDecSvc_InitPullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvcIOStreamInitParams_t *pIOInitParams)
{
   ADSPResult result = ADSP_EOK;

   if (DEC_SVC_PULL_MODE != pIOInitParams->in.ulMode)
   {
      return ADSP_EOK;
   }

   if (!AudioDecSvc_IsPcmFmt(pIOInitParams->format_id))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pull mode supported only for PCM formats.");
      return ADSP_EUNSUPPORTED;
   }

   pInpStrm->pull_mode_ptr = (dec_pull_mode_t*) qurt_elite_memory_malloc(sizeof(dec_pull_mode_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == pInpStrm->pull_mode_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for pull mode!");
      return ADSP_ENOMEMORY;
   }
   memset(pInpStrm->pull_mode_ptr, 0, sizeof(dec_pull_mode_t));

   pInpStrm->pull_mode_ptr->pos_buf_ptr = pIOInitParams->in.pull_mode.pos_buf_ptr;
   pInpStrm->pull_mode_ptr->shared_circ_buf_start_ptr = (uint8_t*)pIOInitParams->in.pull_mode.shared_circ_buf_addr;
   pInpStrm->pull_mode_ptr->shared_circ_buf_size = pIOInitParams->in.pull_mode.shared_circ_buf_size;

   //initialize the position buffer to zero
   memset(pInpStrm->pull_mode_ptr->pos_buf_ptr, 0, sizeof(asm_pull_mode_write_position_buffer_t));

   pInpStrm->pull_mode_ptr->num_water_mark_levels = pIOInitParams->in.pull_mode.num_water_mark_levels;
   if (pInpStrm->pull_mode_ptr->num_water_mark_levels > 0)
   {
      pInpStrm->pull_mode_ptr->water_mark_levels_ptr = (asm_pull_mode_watermark_level_t*)qurt_elite_memory_malloc(
            pInpStrm->pull_mode_ptr->num_water_mark_levels*
            sizeof(asm_pull_mode_watermark_level_t),
            QURT_ELITE_HEAP_DEFAULT);

      if (NULL == pInpStrm->pull_mode_ptr->water_mark_levels_ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for water mark events!");
         return ADSP_ENOMEMORY;
      }
      for (uint32_t i = 0; i < pInpStrm->pull_mode_ptr->num_water_mark_levels; i++)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Water Mark Level %lu",
               pIOInitParams->in.pull_mode.water_mark_levels_ptr[i].watermark_level_bytes);

         /* kw check: (pIOInitParams->in.pull_mode.water_mark_levels_ptr[i].watermark_level_bytes >= 0) &&
          * removed as data type of watermark_level_bytes is unsigned int */
         if (pIOInitParams->in.pull_mode.water_mark_levels_ptr[i].watermark_level_bytes < pInpStrm->pull_mode_ptr->shared_circ_buf_size)
         {
            pInpStrm->pull_mode_ptr->water_mark_levels_ptr[i].watermark_level_bytes = pIOInitParams->in.pull_mode.water_mark_levels_ptr[i].watermark_level_bytes;
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid water mark level.");
            return ADSP_EFAILED;
         }
      }
   }

   result = AudioDecSvc_CopyPcmOr61937MediaFmt(pMe, pInpStrm,
		   (void*)&pIOInitParams->in.pull_mode.MediaFmt.pcm);
   if(ADSP_FAILED(result))
   {
	   return result;
   }

   asm_multi_channel_pcm_fmt_blk_v4_t *pPcm = &pIOInitParams->in.pull_mode.MediaFmt.pcm;
   uint32_t unit_size = pPcm->sample_word_size/8 * pPcm->num_channels;

   if (pInpStrm->pull_mode_ptr->shared_circ_buf_size % unit_size)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Circular buffer size %lu is not a multiple of (sample word size %u * channels %u) ",
            pInpStrm->pull_mode_ptr->shared_circ_buf_size, pPcm->sample_word_size, pPcm->num_channels);
      return ADSP_EFAILED;
   }

   pInpStrm->inp_buf_params.unMemSize = 0xFFFFFFFF; //in case of pull mode the client buf is of infinite size.

   /** register for AV timer */
   result = AudioDecSvc_RegisterAvTimer(pMe, pInpStrm);

   return result;
}

void AudioDecSvc_DeInitPullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   if (pInpStrm->pull_mode_ptr)
   {
      if (pInpStrm->pull_mode_ptr->water_mark_levels_ptr)
      {
         qurt_elite_memory_free(pInpStrm->pull_mode_ptr->water_mark_levels_ptr);
         pInpStrm->pull_mode_ptr->water_mark_levels_ptr = NULL;
      }

      /** deregister for AV timer */
      (void)AudioDecSvc_DeregisterAvTimer(pMe, pInpStrm);

      qurt_elite_memory_free(pInpStrm->pull_mode_ptr);
      pInpStrm->pull_mode_ptr = NULL;
   }
}

ADSPResult AudioDecSvc_SuspendPullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult result = ADSP_EOK;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc :[SUSPEND] AVTimer handle Close ");

   /** deregister for AV timer */
   if ((NULL != pInpStrm->pull_mode_ptr) )
   {
      /** deregister for AV timer */
      result = AudioDecSvc_DeregisterAvTimer(pMe, pInpStrm);
   }
   return result;
}

ADSPResult AudioDecSvc_ResumePullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult result = ADSP_EOK;
   if ((pInpStrm->pull_mode_ptr != NULL))
   {
      result = AudioDecSvc_RegisterAvTimer(pMe, pInpStrm);
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc :[RUN] AVTimer handle resume ");
   return result;
}

ADSPResult AudioDecSvc_ProcessOutputDataQ_PCMPullMode(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe); //TODO won't work for MIMO
   ADSPResult result = ADSP_EOK;
   dec_pull_mode_t *pull_mode_ptr = pInpStrm->pull_mode_ptr;
   asm_pull_mode_write_position_buffer_t *pos_buf_ptr = pull_mode_ptr->pos_buf_ptr;

   uint8_t *read_ptr;
   uint32_t rem_lin_size, temp_size; //remaining linear size.
   uint32_t read_index = pos_buf_ptr->read_index, temp_rd_ind;
   uint32_t numBytesProcessed, totalNumBytesProcessed = 0;
   elite_msg_data_buffer_t *pOutBuf = NULL;

   // downstream service is not connected, return.
   if (NULL == pOutStrm->pDownStreamSvc)
   {
      // LLNP/ULL mode decoder service is not yet connected to downstream svc yet.
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc: Decoder service not yet connected to AFE");
      return result;
   }

   if(ADSP_FAILED(result = AudioDecSvc_ProcessPopOutBuf(pMe, pOutStrm)))
   {
      return result;
   }

   pOutBuf = (elite_msg_data_buffer_t*) (pOutStrm->outDataBufferNode.pBuffer);

   if (NULL == pOutBuf) //happens when buf recreate buf
   {
      return result;
   }

   //check if channel mapping changed
   bool_t chan_map_changed = AudioDecSvc_IsChanMapChanged(pMe->capiContainer[0]->PrevFmtBlk[0].num_channels,
         pMe->capiContainer[0]->PrevFmtBlk[0].channel_mapping,
         pInpStrm->pcmFmt.chan_map.nChannels,
         pInpStrm->pcmFmt.chan_map.nChannelMap);

   if (chan_map_changed)
   {
      //recompute output channel mapping that will be sent to downstream svc
      AudioDecSvc_GetOutChanMap(&pInpStrm->pcmFmt.chan_map, &pOutStrm->out_chan_map);
   }

   //make sure num of transferred at a time is 1 ms or 5 ms.
   uint32_t maxUsableOutBufSize = pOutStrm->maxExtBufSize; //for pull-mode maxExtBufSize wont change & is set to 1ms or 5ms.

   while(1)
   {
      //printf("start=%x,read-index=%u,size=%u,totalNumBytesProcessed=%u\n",
      //pull_mode_ptr->shared_circ_buf_start_ptr,read_index,pull_mode_ptr->shared_circ_buf_size,totalNumBytesProcessed);

      read_ptr = pull_mode_ptr->shared_circ_buf_start_ptr + read_index;
      temp_rd_ind = read_index;
      rem_lin_size = pull_mode_ptr->shared_circ_buf_size - read_index;
      temp_size = rem_lin_size;
      numBytesProcessed = 0;

      //below function only handles linear indexing
      result = AudioDecSvc_ProcessPCM(pMe, pInpStrm, pOutStrm, &read_ptr, &temp_size, &numBytesProcessed, FALSE, maxUsableOutBufSize, TRUE);

      if (ADSP_FAILED(result))
      {
         AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
         return result;
      }

      totalNumBytesProcessed += numBytesProcessed;

      read_index += (rem_lin_size - temp_size);

      AudioDecSvc_CheckSendWaterMarkEvent(pMe, pInpStrm, temp_rd_ind, read_index);

      if (read_index == pull_mode_ptr->shared_circ_buf_size)
      {
         read_index = 0;
      }
      if (totalNumBytesProcessed == maxUsableOutBufSize)
      {
         break;
      }
   }

   if (ADSP_FAILED(result) && (totalNumBytesProcessed == 0)) //if fail and no sample was processed
   {
      AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
      return result;
   }

   //Udpate the position buffer
   uint64_t time = avtimer_drv_get_time(pInpStrm->pull_mode_ptr->avt_drv_handle);
   uint32_t prev_frame_counter      = pos_buf_ptr->frame_counter;

   /** below is a critical section which is executed at high prio so that if the client is also trying to read
    *  we will try to complete is ASAP & they don't have to wait for longer
    */
   qurt_elite_thread_set_prio(elite_get_thrd_prio(ELITETHREAD_DEFAULT_IST_PRIO_ID));
   pos_buf_ptr->frame_counter       = 0;
   pos_buf_ptr->read_index          = read_index;
   pos_buf_ptr->wall_clock_us_lsw   = (uint32_t)time;
   pos_buf_ptr->wall_clock_us_msw   = (uint32_t)(time>>32);
   pos_buf_ptr->frame_counter       = prev_frame_counter+1;
   qurt_elite_thread_set_prio(AudioDecSvc_GetThreadPriority(pMe, pInpStrm->common.perf_mode));

   // if there is any change in fs/channels from previous frame
   // send media format to down stream before the fresh data is enqueued
   if ( (pInpStrm->pcmFmt.ulPCMSampleRate != pMe->capiContainer[0]->PrevFmtBlk[0].sample_rate)
         || chan_map_changed )
   {
      result = AudioDecSvc_NotifyPeerSvcWithMediaFmtUpdate(pMe,pInpStrm,pOutStrm,
            pInpStrm->pcmFmt.ulPCMSampleRate,
            pOutStrm->out_chan_map.num_out_chan,
            pOutStrm->out_chan_map.out_chan,
            pOutStrm->output_bits_per_sample);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc:Fail to send Media Fmt update");
         AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
         return result;
      }

      //update prev format block
      AudioDecSvc_UpdatePrevFmtBlk(pMe,pInpStrm,
            pMe->capiContainer[0],
            pInpStrm->pcmFmt.ulPCMSampleRate,
            pInpStrm->pcmFmt.chan_map.nChannels,
            pInpStrm->pcmFmt.chan_map.nChannelMap,
            pOutStrm->output_bits_per_sample,0);

   }


   uint32_t samples_per_chan = totalNumBytesProcessed/(pOutStrm->output_bytes_per_sample*pInpStrm->pcmFmt.chan_map.nChannels);
   //do channel downmixing if required
   AudioDecSvc_GetOutput(pMe,pOutStrm, pMe->capiContainer[0],
         &pInpStrm->pcmFmt.chan_map,
         (void*) ((int8_t *)(&pOutBuf->nDataBuf)),
         samples_per_chan,
         pOutStrm->output_bytes_per_sample,
         (uint32_t*)(&pOutBuf->nActualSize));

   AudioDecSvc_PopulateOutBuf(pMe, pOutStrm);

   result = AudioDecSvc_SendPcmToPeerSvc(pMe, pInpStrm, pOutStrm, pOutBuf);

   return result;
}

static ADSPResult AudioDecSvc_CheckSendWaterMarkEvent(AudioDecSvc_t *pMe,  AudioDecSvc_InpStream_t *pInpStrm, uint32_t startLevel, uint32_t endLevel)
{
   ADSPResult result;
   //raise event only if it is enabled
   if(pInpStrm->pull_mode_ptr->num_water_mark_levels == 0)
   {
      return ADSP_EOK;
   }

   uint32_t level;

   for (uint32_t i = 0; i < pInpStrm->pull_mode_ptr->num_water_mark_levels; i++)
   {
      level = pInpStrm->pull_mode_ptr->water_mark_levels_ptr[i].watermark_level_bytes;

      /** If a certain level is within start and end, then we are crossing the level, hence raise an event*/
      if ( (level >= startLevel) && (level < endLevel) )
      {
         //form the payload for the event
         asm_data_event_watermark_t waterMarkEvent;
         waterMarkEvent.watermark_level_bytes = level;

         result = AudioStreamMgr_GenerateClientEventFromCb(&pInpStrm->common.CallbackHandle,
               ASM_DATA_EVENT_WATERMARK,
               pInpStrm->common.CallbackHandle.unAsynClientToken,
               &waterMarkEvent,
               sizeof(waterMarkEvent));

         if (ADSP_FAILED(result))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send water mark event!");
         }
         else
         {
            MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Dec [thread-id 0x%lx]: Sent Water Mark event to the client start %lu, level %lu, end %lu", pMe->thread_id, startLevel, level, endLevel);
         }
      }
   }
   return ADSP_EOK;
}

void AudioDecSvc_ResetPullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   //reset the read index to zero.
   if (pInpStrm->pull_mode_ptr)
   {
      asm_pull_mode_write_position_buffer_t *pos_buf_ptr = pInpStrm->pull_mode_ptr->pos_buf_ptr;
      pos_buf_ptr->frame_counter = 0;
      pos_buf_ptr->read_index = 0;
      pos_buf_ptr->wall_clock_us_lsw = 0;
      pos_buf_ptr->wall_clock_us_msw = 0;

   }
}
