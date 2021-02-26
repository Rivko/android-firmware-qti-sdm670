/*========================================================================

 *//** @file AudioDecSvc_Util.cpp
This file contains utility functions for Elite Audio Decoder service.

Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_BufferUtil.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
12/16/15   rbhatnk      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_AprIf.h"
#include "AdspCoreSvc.h"
#include "AudioDecSvc.h"
#include "AudioDecSvc_Util.h"
#include "AudioDecSvc_MiMoUtil.h"
#include "audio_basic_op.h"
#include <audio_basic_op_ext.h>
#include "adsp_asm_api.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "AudioDecSvc_PullMode.h"
#include "AudioDecSvc_CapiV2CallbackHandler.h"
#include "AudioDecSvc_CapiV2Util.h"
#include "AFEInterface.h"


/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

static void AudioDecSvc_DestroyInternalBufs(AudioDecSvc_t* pMe);
static void AudioDecSvc_DestroyScratchBufs(AudioDecSvc_t *pMe);
static ADSPResult AudioDecSvc_RecreateBuffers(AudioDecSvc_t *pMe,
      dec_capi_port_index_t in_port_index, dec_capi_port_index_t out_port_index,
      uint8_t capi_index);

/**
 * pOutStrm->outDataBufferNode.pBuffer may be null when this func returns.
 */
ADSPResult AudioDecSvc_ProcessPopOutBuf(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   ADSPResult nResult = ADSP_EOK;
   elite_msg_data_buffer_t *pOutBuf = NULL;

   // if a buf is already popped, return
   if (NULL != pOutStrm->outDataBufferNode.pBuffer)
   {
      return ADSP_EOK;
   }

   //poll until we get a buf that doesn't get recreated
   while( (qurt_elite_channel_poll(qurt_elite_queue_get_channel(pOutStrm->pOutBufQ),
         qurt_elite_queue_get_channel_bit(pOutStrm->pOutBufQ))) )
   {
      // Take next buffer off the q
      if(ADSP_FAILED(nResult = qurt_elite_queue_pop_front(pOutStrm->pOutBufQ, (uint64_t*)&(pOutStrm->outDataBufferNode))))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PBQ:Failure on Out Buf Pop ");
         return nResult;
      }

      pOutBuf = (elite_msg_data_buffer_t*) (pOutStrm->outDataBufferNode.pBuffer);
      pOutBuf->nActualSize = 0;
#ifdef EARLY_EOS_DEBUG
      MSG_3(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "Early-EOS [0x%lx]: popped buffer  0x%p. Current bitmask 0x%x", pOutStrm->common.ulDataLogId, pOutStrm->outDataBufferNode.pBuffer, pMe->unCurrentBitfield);
#endif

      AudioDecSvc_SubtractBufDelayUsingClientToken(pMe, pOutStrm, &pOutBuf->unClientToken);

      nResult = AudioDecSvc_CheckReallocateExternalBuffer(pMe, pOutStrm);
      if (ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Relloacte failed ");
         return nResult;
      }

      if (NULL != pOutStrm->outDataBufferNode.pBuffer)
      {
         break; //if buf got recreated, go back to loop.
      }
   }

   return nResult;
}

ADSPResult AudioDecSvc_PushOutBuf(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, uint64_t *pPayload)
{
   ADSPResult nResult;

   nResult= qurt_elite_queue_push_back(pOutStrm->pDownStreamSvc->dataQ, pPayload);

   // send pOutputBuf to downstream service.
   if(ADSP_FAILED(nResult))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping!\n");
      AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
   }

#ifdef DBG_AUDIO_DECODE
   MSG_2(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "Sent(%d) bytes to PP, TS=%llu\n", pOutBuf->nActualSize,pOutBuf->ullTimeStamp);
#endif
#ifdef EARLY_EOS_DEBUG
   MSG_3(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "Early-EOS [0x%lx]: pushed buffer downstream 0x%p. Current bit mask 0x%x", pOutStrm->common.ulDataLogId, pOutStrm->outDataBufferNode.pBuffer, pMe->unCurrentBitfield);
#endif
   pOutStrm->outDataBufferNode.pBuffer = NULL;

   return nResult;
}

/**
 * ensures that the buffers allocated for PCM are always afe-frame aligned.
 */
ADSPResult AudioDecSvc_RecreatePcmBufsBasedOnMediaFmt(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   dec_CAPI_container_t * last_capi = AudioDecSvc_GetLastCapi(pMe);  //check if the last decoder is PCM (this func is for changing output buf of last decoder).

   if (last_capi != NULL)
   {
      if (!AudioDecSvc_IsPcmFmt(last_capi->dec_fmt_id))
      {
         return ADSP_EOK;
      }
   }


   ADSPResult result = ADSP_EOK;
   dec_capi_port_index_t out_capi_port_index;
   dec_capi_port_index_t in_capi_port_index;

   in_capi_port_index.index = 0;
   in_capi_port_index.valid = FALSE;

   //no need to consider metadata size as PCM doesnt have

   for (uint8_t capi_index = 0; capi_index < DEC_SVC_MAX_CAPI; capi_index++)
   {
      if(NULL == pMe->capiContainer[capi_index])
      {
         break;
      }
      for(uint32_t out_capi_index = 0; out_capi_index < DEC_SVC_MAX_OUTPUT_STREAMS; out_capi_index++)
      {
         out_capi_port_index.index = out_capi_index;
         out_capi_port_index.valid = TRUE;

         //output ch mapping API may cause more output num ch than input. so more space needed.
         //allocate out buf based on output_bytes_per_sample , not input bytes per sample (Eg. input is 16 bit output is 24 bit)
         uint32_t num_ch = pMe->out_streams_ptr[out_capi_index]->out_chan_map.is_out_chan_map_received ? PCM_FORMAT_MAX_NUM_CHANNEL: pInpStrm->pcmFmt.chan_map.nChannels;
         uint32_t unDecOutBufSizeBytes = AudioDecSvc_GetPcmOutBufSize(pInpStrm->common.perf_mode,
               pInpStrm->pcmFmt.ulPCMSampleRate, pMe->out_streams_ptr[out_capi_index]->output_bytes_per_sample, num_ch);

         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc output buffer size after media format %ld", unDecOutBufSizeBytes);

         pMe->capiContainer[capi_index]->out_port_event_new_size[out_capi_index] = unDecOutBufSizeBytes;
         result |= AudioDecSvc_RecreateBuffers(pMe, in_capi_port_index, out_capi_port_index, capi_index);
         pMe->capiContainer[capi_index]->out_port_event_new_size[out_capi_index] = 0;

      }
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to re-create the internal buffers.");
         return result;
      }
   }
   return result;
}

ADSPResult AudioDecSvc_CheckCreateScratchBufs(AudioDecSvc_t *pMe, dec_CAPI_container_t *capi_container, uint32_t outBufSize)
{
   if (outBufSize)
   {
      //create a scratch buffer of the same size which is used in output mapping
      // memory allocated to scratch buffer should be maximum
      //TODO: note that even if capi v2 raises event to decrease, scratch buf size cannot be decreased due to this logic.
      //       this logic is necessary because scratch buf is common to few all IO of a capi.
      if(capi_container->scratch_buf_size < outBufSize)
      {
         if(capi_container->scratch_out_buf)
         {
            qurt_elite_memory_free(capi_container->scratch_out_buf);
            capi_container->scratch_out_buf = NULL;
            capi_container->scratch_buf_size = 0;
         }
      }

      if(NULL == capi_container->scratch_out_buf)
      {
         capi_container->scratch_out_buf = qurt_elite_memory_malloc(outBufSize, QURT_ELITE_HEAP_DEFAULT);
         if(NULL == capi_container->scratch_out_buf)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create scratch out-buf for AudioDecSvc.It requires %lu bytes", outBufSize);
            return ADSP_ENOMEMORY;
         }
         capi_container->scratch_buf_size = outBufSize;
      }
   }

   return ADSP_EOK;
}

ADSPResult AudioDecSvc_CreateInternalInputBufs(AudioDecSvc_t *pMe, dec_CAPI_container_t *capi_container,
      uint32_t inBufSize, dec_capi_port_index_t in_capi_port_index)
{
   if (inBufSize && in_capi_port_index.valid)
   {
      // create internal input bitstream buffer
      uint8_t *int_buf = (uint8_t *) qurt_elite_memory_malloc(inBufSize, ADEC_SVC_INTERNAL_BIT_STREAM_BUF);
      if ( NULL==int_buf)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create internal input buffer for AudioDecSvc.It requires %lu bytes", inBufSize);
         return ADSP_ENOMEMORY;
      }

#ifdef DBG_BUFFER_ADDRESSES
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR Dec internal bitstream buffer: 0x%8x, size %d",
            int_buf, inBufSize);
#endif /* DBG_BUFFER_ADDRESSES */

      capi_container->in_buf[in_capi_port_index.index].actual_data_len = 0;
      //Set DataLenReq fields here, as they are constant
      capi_container->in_buf[in_capi_port_index.index].max_data_len = inBufSize;
      //Set the data fields for input node of CAPI
      capi_container->in_buf[in_capi_port_index.index].data_ptr    = (int8_t *) (int_buf);

      capi_container->inputs[in_capi_port_index.index].buf_ptr = &capi_container->in_buf[in_capi_port_index.index];
      capi_container->inputs[in_capi_port_index.index].bufs_num = 1;
      capi_container->inputs[in_capi_port_index.index].flags.end_of_frame = false;
      capi_container->inputs[in_capi_port_index.index].flags.is_timestamp_valid = false;
      capi_container->inputs[in_capi_port_index.index].flags.marker_eos = false;
      capi_container->inputs[in_capi_port_index.index].flags.marker_1 = 0;
      capi_container->inputs[in_capi_port_index.index].flags.marker_2 = 0;
      capi_container->inputs[in_capi_port_index.index].flags.marker_3 = 0;
      capi_container->inputs[in_capi_port_index.index].flags.reserved = 0;
      capi_container->inputs[in_capi_port_index.index].timestamp = 0;

      capi_container->inputs_ptr[in_capi_port_index.index] = &capi_container->inputs[in_capi_port_index.index];

      //max in size is not stored elsewhere as capi_container->in_buf[in_capi_port_index.index].max_data_len is not changed.

   }

   return ADSP_EOK;
}

ADSPResult AudioDecSvc_CreateInternalOutputBufs(AudioDecSvc_t *pMe, dec_CAPI_container_t *capi_container,
      uint32_t outBufSize, dec_capi_port_index_t out_capi_port_index)
{
   if (outBufSize && out_capi_port_index.valid)
   {
      uint32_t reqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(outBufSize);

      //allocate the databuffer payload (metadata + pcm buffer size)
      void *psPcmBuf =  qurt_elite_memory_malloc(reqSize, ADEC_SVC_OUT_BUF);

      if (!psPcmBuf)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create internal out-buf for AudDecSvc.It requires %lu bytes", reqSize);
         return ADSP_ENOMEMORY;
      }

#ifdef DBG_BUFFER_ADDRESSES
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR decoder output buffer: 0x%8x, size %d", psPcmBuf, reqSize);
#endif /* DBG_BUFFER_ADDRESSES */

      elite_msg_data_buffer_t *pcmBuf = (elite_msg_data_buffer_t*) (psPcmBuf);
      memset(pcmBuf,0,sizeof(elite_msg_data_buffer_t)); //reset only the header.
      pcmBuf->nMaxSize = outBufSize;

      capi_container->out_buf[out_capi_port_index.index].data_ptr = (int8_t*)pcmBuf;
      capi_container->out_buf[out_capi_port_index.index].actual_data_len = 0;
      capi_container->out_buf[out_capi_port_index.index].max_data_len = outBufSize;

      capi_container->outputs[out_capi_port_index.index].buf_ptr = &capi_container->out_buf[out_capi_port_index.index];
      capi_container->outputs[out_capi_port_index.index].bufs_num = 1;
      capi_container->outputs[out_capi_port_index.index].flags.end_of_frame = 0;
      capi_container->outputs[out_capi_port_index.index].flags.is_timestamp_valid = false;
      capi_container->outputs[out_capi_port_index.index].flags.marker_eos = 0;
      capi_container->outputs[out_capi_port_index.index].flags.marker_1 = 0;
      capi_container->outputs[out_capi_port_index.index].flags.marker_2 = 0;
      capi_container->outputs[out_capi_port_index.index].flags.marker_3 = 0;
      capi_container->outputs[out_capi_port_index.index].flags.reserved = 0;
      capi_container->outputs[out_capi_port_index.index].timestamp = 0;

      capi_container->outputs_ptr[out_capi_port_index.index] = &capi_container->outputs[out_capi_port_index.index];

      //max out size is not stored elsewhere as capi_container->out_buf[out_capi_port_index.index].max_data_len is not changed.
   }

   return ADSP_EOK;
}

void AudioDecSvc_FreeBufInCircBuf(AudioDecSvc_OutStream_t* pOutStrm, AudioDecSvc_Circular_Buffer_List_t *pCircBufList, int index)
{
   for (uint16_t i = 0; i < AUD_DEC_SVC_MAX_MSG_WITH_DATA; i++)
   {
      if (pCircBufList->buflist[index].msg[i].pPayload)
      {
         elite_msg_release_msg(&pCircBufList->buflist[index].msg[i]);
      }
   }

   if (pCircBufList->buflist[index].data)
   {
      qurt_elite_memory_free(pCircBufList->buflist[index].data);
#ifdef EARLY_EOS_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Early-Eos: freed 0x%p", pCircBufList->buflist[index].data);
#endif
      pCircBufList->buflist[index].data = NULL;
   }
}

ADSPResult AudioDecSvc_CheckReCreateOneBufInCircBuf(AudioDecSvc_OutStream_t* pOutStrm, AudioDecSvc_Circular_Buffer_List_t *pCircBufList, int index, uint32_t BufSize)
{
   ADSPResult result = ADSP_EOK;
   uint32_t reqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(BufSize);

   if (NULL == pCircBufList->buflist)
   {
      return ADSP_EBADPARAM;
   }

   if ( (NULL != pCircBufList->buflist[index].data) &&
         ((pCircBufList->buflist[index].data->nMaxSize != (int32_t)BufSize)) )
   {
      AudioDecSvc_FreeBufInCircBuf(pOutStrm, pCircBufList, index);
   }

   if ( NULL == pCircBufList->buflist[index].data )
   {
      //allocate the buffer
      void *psPcmBuf =  qurt_elite_memory_malloc(reqSize, QURT_ELITE_HEAP_DEFAULT);

      if (!psPcmBuf)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create buffer required for circ-buf list for AudDecSvc. It requires %lu bytes", reqSize);
         return ADSP_ENOMEMORY;
      }
#ifdef EARLY_EOS_DEBUG
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Early-Eos: allocated 0x%p. size %lu", psPcmBuf, BufSize);
#endif
      pCircBufList->buflist[index].data = (elite_msg_data_buffer_t*) (psPcmBuf);
      memset(pCircBufList->buflist[index].data, 0 , sizeof(elite_msg_data_buffer_t));
      pCircBufList->buflist[index].data->nMaxSize = BufSize;
   }

   return result;
}

ADSPResult AudioDecSvc_CheckReCreateAllBufsInCircBuf(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t* pOutStrm, uint32_t BufSize)
{
   ADSPResult result = ADSP_EOK;

   uint16_t num_bufs = pOutStrm->CircBufList.numBufs; //since below loop might change this.

   //only empty buffers cab be recreated, now. rest will be recreated when popped from circ buf list.
   //filled buffers are from tail to head-1. and empty from head to tail-1.

   //--filled------|head|==empty==|tail|---filled--
   if (pOutStrm->CircBufList.tail > pOutStrm->CircBufList.head)
   {
      for (int i = pOutStrm->CircBufList.head; i < pOutStrm->CircBufList.tail; i++)
      {
         result |= AudioDecSvc_CheckReCreateOneBufInCircBuf(pOutStrm, &pOutStrm->CircBufList, i, BufSize);
      }
   }
   else // ===empty===|tail|---filled---|head|===empty==
   {
      for (int i = 0; i < pOutStrm->CircBufList.tail; i++)
      {
         result |= AudioDecSvc_CheckReCreateOneBufInCircBuf(pOutStrm, &pOutStrm->CircBufList, i, BufSize);
      }
      for (int i = pOutStrm->CircBufList.head; i < num_bufs; i++)
      {
         result |= AudioDecSvc_CheckReCreateOneBufInCircBuf(pOutStrm, &pOutStrm->CircBufList, i, BufSize);
      }
   }
   return result;
}

// Creates or recreates buffers for circular buf list
// if recreating then new list may be smaller or larger than old. larger size is preserved.
// old to new copy is also done.
ADSPResult AudioDecSvc_ReCreateCircBufs(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t* pOutStrm, uint32_t BufSize, int32_t numBufsInAdditionToMimo)
{
   ADSPResult result = ADSP_EOK;
   //back up old buf list ptr

   AudioDecSvc_Circular_Buffer_List_t    oldCircBuf = pOutStrm->CircBufList;
   AudioDecSvc_Circular_Buffer_List_t    *pNewCircBuf = &pOutStrm->CircBufList;

   //below is not only for early-eos but also for MIMO
   int32_t numBufs = numBufsInAdditionToMimo + DEC_SVC_MIMO_MAX_BUF_IN_CIRC_BUFFER;
   if (numBufs > ( ((int32_t)1)  <<  (sizeof(uint16_t)*8) )) //as pNewCircBuf->numBufs is uint16_t
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Early-Eos: Num bufs %ld > 65536", numBufs);
      return ADSP_EFAILED;
   }

   ////1 for empty buf. It can be fixed with few changes in circular buffer logic.
   pNewCircBuf->numBufs = numBufs + 1;

   /**
    * buf list recreation
    *
    * buf list needs recreation only if there's any change in number of buffers.
    *
    */
   if ( oldCircBuf.numBufs != pNewCircBuf->numBufs )
   {
      uint16_t num_buf_needed_in_new_list;

      if (oldCircBuf.numBufs > pNewCircBuf->numBufs) //decrease, only empty can be removed now.
      {
         uint16_t num_filled_slots = oldCircBuf.numBufs - AudioDecSvc_CircBuf_GetNumEmptySlots(&oldCircBuf);
         num_buf_needed_in_new_list = pNewCircBuf->numBufs > num_filled_slots ? pNewCircBuf->numBufs : num_filled_slots;
      }
      else //increase,
      {
         num_buf_needed_in_new_list = pNewCircBuf->numBufs;
      }

      pNewCircBuf->numBufs = num_buf_needed_in_new_list;

      //create buf_list
      if (oldCircBuf.numBufs != num_buf_needed_in_new_list)
      {
         //create new buf list
         uint32_t sz = sizeof(AudioDecSvc_CircBufItem_t) * num_buf_needed_in_new_list;
         if (sz)
         {
            pNewCircBuf->buflist = (AudioDecSvc_CircBufItem_t*)qurt_elite_memory_malloc(sz, QURT_ELITE_HEAP_DEFAULT);
            if (NULL == pNewCircBuf->buflist)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to allocate circ buf list %lu bytes", sz);
               return ADSP_ENOMEMORY;
            }
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[0x%lx]: Recreated circ buf list %lu bytes, num bufs before %u, after %u",
                  pOutStrm->common.ulDataLogId, sz,  oldCircBuf.numBufs, pNewCircBuf->numBufs);
            memset(pNewCircBuf->buflist, 0, sz);
         }

         //copy from old to new
         //1. if num bufs is decreased, then delete empty buf if possible & update tail.
         //2. if num_bufs is increased, then insert them in between empty bufs
         //filled buffers are from tail to head-1. and empty from head to tail-1.
         if (oldCircBuf.buflist)
         {
            uint16_t s = 0, d = 0; //s = old = src, d = new = dest.
            if (oldCircBuf.tail > oldCircBuf.head) //--filled------|head|==empty==|tail|---filled--
            {
               if (oldCircBuf.numBufs > pNewCircBuf->numBufs) //decrease: remove some empty
               {
                  for (; (s < oldCircBuf.head) && (d < pNewCircBuf->numBufs); s++, d++)
                  {
                     pNewCircBuf->buflist[d] = oldCircBuf.buflist[s];
                  }
                  pNewCircBuf->head = d;

                  uint16_t filled_old = oldCircBuf.head + oldCircBuf.numBufs - oldCircBuf.tail;
                  uint16_t empty_to_copy = (pNewCircBuf->numBufs > filled_old) ? (pNewCircBuf->numBufs - filled_old) : 0;
                  for (uint16_t i=0; (s < oldCircBuf.tail) && (d < pNewCircBuf->numBufs) && (i < empty_to_copy); s++, d++, i++)
                  {
                     pNewCircBuf->buflist[d] = oldCircBuf.buflist[s];
                  }
                  pNewCircBuf->tail = d;

                  //free remaining empty bufs.
                  for (; s < oldCircBuf.tail; s++)
                  {
                     AudioDecSvc_FreeBufInCircBuf(pOutStrm, &oldCircBuf, s);
                     //note that numBufs is not decreased
                  }

                  for (; (s < oldCircBuf.numBufs) && (d < pNewCircBuf->numBufs); s++, d++)
                  {
                     pNewCircBuf->buflist[d] = oldCircBuf.buflist[s];
                  }
               }
               else //increase: introduce some empty.
               {
                  pNewCircBuf->head = oldCircBuf.head;

                  for (; (s < oldCircBuf.tail) && (d < pNewCircBuf->numBufs); s++, d++)
                  {
                     pNewCircBuf->buflist[d] = oldCircBuf.buflist[s];
                  }

                  uint16_t empty_to_insert = pNewCircBuf->numBufs - oldCircBuf.numBufs;
                  for (uint16_t i=0; (d < pNewCircBuf->numBufs) && (i < empty_to_insert); d++, i++)
                  {
                     memset(&(pNewCircBuf->buflist[d]), 0, sizeof(pNewCircBuf->buflist[d]));
                  }
                  pNewCircBuf->tail = d;

                  for (; (s < oldCircBuf.numBufs) && (d < pNewCircBuf->numBufs); s++, d++)
                  {
                     pNewCircBuf->buflist[d] = oldCircBuf.buflist[s];
                  }
               }
            }
            else // ===empty===|tail|---filled---|head|===empty== //filled buffers are from tail to head-1. and empty from head to tail-1.
            {
               if (oldCircBuf.numBufs > pNewCircBuf->numBufs) //decrease:remove some empty
               {
                  uint16_t num_empty_in_beginning = oldCircBuf.tail;
                  uint16_t empty_to_remove = oldCircBuf.numBufs - pNewCircBuf->numBufs;
                  uint16_t num_to_remove_in_beginning = MIN(num_empty_in_beginning, empty_to_remove);
                  uint16_t empty_left_in_begin = (num_empty_in_beginning > empty_to_remove) ? (num_empty_in_beginning - empty_to_remove) : 0;
                  empty_to_remove = empty_to_remove - num_to_remove_in_beginning; //still left to remove
                  //free remaining empty bufs.
                  for (; s < num_to_remove_in_beginning; s++)
                  {
                     AudioDecSvc_FreeBufInCircBuf(pOutStrm, &oldCircBuf, s);
                     //note that numBufs is not decreased
                  }

                  for (uint16_t i = 0; (s < oldCircBuf.head) && (d < pNewCircBuf->numBufs) && (i < empty_left_in_begin); i++, d++, s++)
                  {
                     pNewCircBuf->buflist[d] = oldCircBuf.buflist[s];
                  }
                  pNewCircBuf->tail = d;

                  for (; (s < oldCircBuf.head) && (d < pNewCircBuf->numBufs) ; s++, d++)
                  {
                     pNewCircBuf->buflist[d] = oldCircBuf.buflist[s];
                  }

                  pNewCircBuf->head = d;

                  uint16_t num_empty_at_end = oldCircBuf.numBufs - oldCircBuf.head;
                  uint16_t num_to_remove_at_end = MIN(num_empty_at_end, empty_to_remove);

                  //free remaining empty bufs.
                  for (uint16_t i = 0; i < num_to_remove_at_end; s++, i++)
                  {
                     AudioDecSvc_FreeBufInCircBuf(pOutStrm, &oldCircBuf, s);
                     //note that numBufs is not decreased
                  }

                  for (; (s < oldCircBuf.numBufs) && (d < pNewCircBuf->numBufs) ; s++, d++)
                  {
                     pNewCircBuf->buflist[d] = oldCircBuf.buflist[s];
                  }

               }
               // ===empty===|tail|---filled---|head|===empty==
               else //increase: introduce some empty at the end
               {
                  uint16_t empty_needed = pNewCircBuf->numBufs - oldCircBuf.numBufs;

                  pNewCircBuf->tail = oldCircBuf.tail;
                  pNewCircBuf->head = oldCircBuf.head;
                  for (; (s < oldCircBuf.numBufs) && (d < pNewCircBuf->numBufs) ; s++, d++)
                  {
                     pNewCircBuf->buflist[d] = oldCircBuf.buflist[s];
                  }
                  for (int i = 0; (i < empty_needed) && (d < pNewCircBuf->numBufs); i++, d++)
                  {
                     memset(&(pNewCircBuf->buflist[d]), 0, sizeof(pNewCircBuf->buflist[d]));
                  }
               }
            }

            //free old list.
            qurt_elite_memory_free(oldCircBuf.buflist);
         }

         //trigger filling up of circ buf only for early-eos case.
         //after recreating if there are empty bufs, raise signal
         if (AudioDecSvc_EarlyEos_IsDelaySatisfied(pMe, pOutStrm))
         {
            AudioDecSvc_EarlyEos_ClearSignal(pMe, pOutStrm);
         }
         else
         {
            AudioDecSvc_EarlyEoS_RaiseSignal(pMe, pOutStrm);
         }
      }
   }

   /**
    * buffer recreation
    * create/destroy buffers after buffer list is created.
    * empty buffer slots will be recreated now
    */
   if (pNewCircBuf->buflist)
   {
      result = AudioDecSvc_CheckReCreateAllBufsInCircBuf(pMe, pOutStrm, BufSize);
   }

   return result;
}

/**
 * for last capi both scratch buf and out buf are created here.
 */
ADSPResult AudioDecSvc_CreateExternalBufs(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStream,
      dec_CAPI_container_t *capi_container, uint32_t numOutBufs)
{
   if ( numOutBufs > MAX_OUT_BUF_Q_ELEMENTS)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"not enough bufQ elements");
      return ADSP_ENORESOURCE;
   }

   uint32_t reqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(pOutStream->maxExtBufSize);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Creating %lu external buffers of size %lu", (numOutBufs - pOutStream->nBufsAllocated), pOutStream->maxExtBufSize);

   // Allocate and queue up the output buffers.
   while ((uint32_t)pOutStream->nBufsAllocated < numOutBufs)
   {
      //allocate the databuffer payload (metadata + pcm buffer size)
      void *psPcmBuf =  qurt_elite_memory_malloc(reqSize, ADEC_SVC_OUT_BUF);

      if (!psPcmBuf)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create external out-buf for AudDecSvc.It requires %lu bytes", reqSize);
         return ADSP_ENOMEMORY;
      }
#ifdef EARLY_EOS_DEBUG
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Early-Eos: allocated 0x%p. size %lu", psPcmBuf, outBufSize);
#endif
#ifdef DBG_BUFFER_ADDRESSES
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR decoder output buffer: 0x%8x, size %d", psPcmBuf, reqSize);
#endif /* DBG_BUFFER_ADDRESSES */

      elite_msg_data_buffer_t *pcmBuf = (elite_msg_data_buffer_t*) (psPcmBuf);
      memset(pcmBuf,0,sizeof(elite_msg_data_buffer_t));
      pcmBuf->nMaxSize = pOutStream->maxExtBufSize;

      ADSPResult result;
      if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(pOutStream->pOutBufQ, (elite_msg_any_payload_t*) psPcmBuf)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to fill Decoder Svc buffer queue!! \n");
         //free this PCM buffer here because main destroy function doesn't have the ptr, since push to returnq failed.
         qurt_elite_memory_free(psPcmBuf);
         return result;
      }
      pOutStream->nBufsAllocated++;
   }
   uint8_t index = AudioDecSvc_GetOutputStreamIndex(pMe,pOutStream);

   capi_container->out_buf[index].data_ptr = NULL; //will be popped off the queue
   capi_container->out_buf[index].actual_data_len = 0;
   capi_container->out_buf[index].max_data_len = pOutStream->maxExtBufSize;

   capi_container->outputs[index].buf_ptr = &capi_container->out_buf[index];
   capi_container->outputs[index].bufs_num = 1;
   capi_container->outputs[index].flags.end_of_frame = 0;
   capi_container->outputs[index].flags.is_timestamp_valid = false;
   capi_container->outputs[index].flags.marker_eos = 0;
   capi_container->outputs[index].flags.marker_1 = 0;
   capi_container->outputs[index].flags.marker_2 = 0;
   capi_container->outputs[index].flags.marker_3 = 0;
   capi_container->outputs[index].flags.reserved = 0;
   capi_container->outputs[index].timestamp = 0;

   capi_container->outputs_ptr[index] = &capi_container->outputs[index];

   return ADSP_EOK;
}

static ADSPResult AudioDecSvc_RecreateBuffers(AudioDecSvc_t *pMe,
      dec_capi_port_index_t in_port_index, dec_capi_port_index_t out_port_index,
      uint8_t capi_index)
{
	ADSPResult result = ADSP_EOK;

	uint32_t in_size = 0, out_size = 0, num_out_bufs = 0;

	if (in_port_index.valid)
	{
		in_size = pMe->capiContainer[capi_index]->in_port_event_new_size[in_port_index.index];

		//delete the internal buffers and allocate new ones
		//delete only input buf size based buffers.
		if( pMe->capiContainer[capi_index]->in_buf[in_port_index.index].data_ptr != NULL)
		{
			qurt_elite_memory_free( pMe->capiContainer[capi_index]->in_buf[in_port_index.index].data_ptr);
			pMe->capiContainer[capi_index]->in_buf[in_port_index.index].data_ptr = NULL;

			pMe->capiContainer[capi_index]->inputs[in_port_index.index].buf_ptr = NULL;
			pMe->capiContainer[capi_index]->inputs_ptr[in_port_index.index] = NULL;
		}

		//TODO: to fix allocation based on sum of prev+current CAPIs
		//if last CAPI create ext output buffers as well.
		result = AudioDecSvc_CreateInternalInputBufs(pMe, pMe->capiContainer[capi_index], in_size, in_port_index);
	}

	if (out_port_index.valid)
	{
		uint32_t index = out_port_index.index;
		out_size = pMe->capiContainer[capi_index]->out_port_event_new_size[index];

		//delete the internal buffers and allocate new ones
		//delete only output buf size based buffers.

		if (AudioDecSvc_IsLastCapiIndex(pMe, capi_index) )
		{
			uint32_t max_metadata_size = 0;
			//In case PCM this pointer is NULL. So do not get metadata from CAPI
			if (pMe->capiContainer[capi_index]->capi_ptr)
			{
				result = aud_dec_svc_get_max_metadata_size(pMe->capiContainer[capi_index]->capi_ptr,
						&max_metadata_size);
			}

			if (ADSP_FAILED(result)) return result;

			AudioDecSvc_GetRequiredOutBufSizeNum(pMe, &num_out_bufs, &out_size,
					pMe->capiContainer[capi_index]->out_port_event_new_size[index],
					max_metadata_size);

			AudioDecSvc_DestroyExternalBufs(pMe, pMe->out_streams_ptr[out_port_index.index]);

			pMe->capiContainer[capi_index]->out_buf[out_port_index.index].data_ptr = NULL;
			pMe->capiContainer[capi_index]->outputs[out_port_index.index].buf_ptr = NULL;
			pMe->capiContainer[capi_index]->outputs_ptr[out_port_index.index] = NULL;

			pMe->out_streams_ptr[out_port_index.index]->maxExtBufSize = out_size;
			//TODO: to fix allocation based on sum of prev+current CAPIs
			//if last CAPI create ext output buffers as well.
			result = AudioDecSvc_CreateExternalBufs(pMe, pMe->out_streams_ptr[out_port_index.index],
					pMe->capiContainer[capi_index], num_out_bufs);
			result |= AudioDecSvc_CheckCreateScratchBufs(pMe, pMe->capiContainer[capi_index], out_size);
			result |= AudioDecSvc_CheckReCreateAllBufsInCircBuf(pMe, pMe->out_streams_ptr[out_port_index.index], out_size);
		}
		else
		{
			//for non-last CAPIs free the internal buffer
			if (NULL != pMe->capiContainer[capi_index]->out_buf[out_port_index.index].data_ptr)
			{
				qurt_elite_memory_free( pMe->capiContainer[capi_index]->out_buf[out_port_index.index].data_ptr);
				pMe->capiContainer[capi_index]->out_buf[out_port_index.index].data_ptr = NULL;

				pMe->capiContainer[capi_index]->outputs[out_port_index.index].buf_ptr = NULL;
				pMe->capiContainer[capi_index]->outputs_ptr[out_port_index.index] = NULL;
			}

			//scratch buf is automatically recreated in createExt/int buffers.

			/* Will create the o/p buffer wrt 4 bps if above conditional is true */
			result = AudioDecSvc_CreateInternalOutputBufs(pMe, pMe->capiContainer[capi_index], out_size, out_port_index);
			result |= AudioDecSvc_CheckCreateScratchBufs(pMe, pMe->capiContainer[capi_index], out_size);
		}
	}
	return result;
}

/* if a buffer is recreated, then
 * pOutStrm->outDataBufferNode.pBuffer is NULL
 * and the caller must go back to work loop.
 */
ADSPResult AudioDecSvc_CheckReallocateExternalBuffer(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_data_buffer_t *pBuffer = (elite_msg_data_buffer_t*)pOutStrm->outDataBufferNode.pBuffer;
   dec_CAPI_container_t *lastCapi = AudioDecSvc_GetLastCapi(pMe);

   if ( (NULL == pBuffer) || (NULL == lastCapi))
   {
      return result;
   }

   //if buf size doesn't match then recreate.
   if((uint32_t)pBuffer->nMaxSize != pOutStrm->maxExtBufSize)
   {
      // Free the buffer
      qurt_elite_memory_free(pBuffer);
      pOutStrm->nBufsAllocated--;

      pOutStrm->outDataBufferNode.pBuffer = NULL;

      //nBufsAllocated buffers are already allocated, need to allocate one more.
      result = AudioDecSvc_CreateExternalBufs(pMe, pOutStrm,  lastCapi, pOutStrm->nBufsAllocated+1); //one buf was destroyed, so create one more.
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Buffer recreate failed %d", result);
      }
   }

   return result;
}

void AudioDecSvc_DestroyCircularBufs(AudioDecSvc_t *pMe , AudioDecSvc_OutStream_t* pOutStrm)
{
   if (NULL == pOutStrm->CircBufList.buflist)
   {
      return ;
   }

   for (int i = 0; i < pOutStrm->CircBufList.numBufs; i++)
   {
      AudioDecSvc_FreeBufInCircBuf(pOutStrm, &pOutStrm->CircBufList, i);
   }
   pOutStrm->CircBufList.numBufs = 0;

   qurt_elite_memory_free(pOutStrm->CircBufList.buflist);
   pOutStrm->CircBufList.buflist = NULL;
}


void AudioDecSvc_DestroyInternalBuffers(AudioDecSvc_t *pMe)
{
   AudioDecSvc_DestroyInternalBufs(pMe); //destroy both in and out size based bufs

   AudioDecSvc_DestroyScratchBufs(pMe);
}

static void AudioDecSvc_DestroyScratchBufs(AudioDecSvc_t *pMe)
{
   for (uint8_t i=0; i < DEC_SVC_MAX_CAPI; i++)
   {
      if (pMe->capiContainer[i] )
      {
         //destroy scratch buffer
         if(pMe->capiContainer[i]->scratch_out_buf != NULL)
         {
            qurt_elite_memory_free(pMe->capiContainer[i]->scratch_out_buf);
            pMe->capiContainer[i]->scratch_out_buf = NULL;
         }
      }
   }
}

/**
 * for last CAPI, the scratch buf is destroyed here itself
 */
static void AudioDecSvc_DestroyInternalBufs(AudioDecSvc_t *pMe)
{
   uint8_t capi_index = 0;
   for (uint8_t i=0; i < DEC_SVC_MAX_CAPI; i++)
   {
      if (pMe->capiContainer[i] )
      {
         for (uint16_t j = 0; j < DEC_SVC_MAX_INPUT_STREAMS; j++)
         {
            //destroy the input buffer
            if( pMe->capiContainer[i]->in_buf[j].data_ptr != NULL)
            {
               qurt_elite_memory_free( pMe->capiContainer[i]->in_buf[j].data_ptr);
               pMe->capiContainer[i]->in_buf[j].data_ptr = NULL;

               pMe->capiContainer[i]->inputs[j].buf_ptr = NULL;
               pMe->capiContainer[i]->inputs_ptr[j] = NULL;
            }
         }
         capi_index = i;
      }
   }

   //except for last CAPI destroy out bufs, for last CAPI out bufs are created as dec svc out bufs.
   for (uint8_t i=0; i < capi_index; i++)
   {
      //destroy output buffers
      if ( pMe->capiContainer[i])
      {
         for (uint16_t j = 0; j < DEC_SVC_MAX_OUTPUT_STREAMS; j++)
         {
            if( pMe->capiContainer[i]->out_buf[j].data_ptr != NULL)
            {
               qurt_elite_memory_free( pMe->capiContainer[i]->out_buf[j].data_ptr);
               pMe->capiContainer[i]->out_buf[j].data_ptr = NULL;

               pMe->capiContainer[i]->outputs[j].buf_ptr = NULL;
               pMe->capiContainer[i]->outputs_ptr[j] = NULL;
            }
         }
      }
   }

   //for the last CAPI, assign the ptrs as NULL
   for (uint16_t j = 0; j < DEC_SVC_MAX_OUTPUT_STREAMS; j++)
   {
      if (pMe->capiContainer[capi_index])
      {
         pMe->capiContainer[capi_index]->out_buf[j].data_ptr = NULL;
         pMe->capiContainer[capi_index]->outputs[j].buf_ptr = NULL;
         pMe->capiContainer[capi_index]->outputs_ptr[j] = NULL;
      }
   }
}

void AudioDecSvc_DestroyExternalBufs(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t* pOutStrm)
{
   ADSPResult result;
   qurt_elite_bufmgr_node_t bufNode;

   if (pOutStrm->outDataBufferNode.pBuffer)
   {
      AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
   }

   // Drain the buffers
   while (qurt_elite_channel_poll(qurt_elite_queue_get_channel(pOutStrm->pOutBufQ),
         qurt_elite_queue_get_channel_bit(pOutStrm->pOutBufQ)))
   {
      // retrieve the buffer
      result = qurt_elite_queue_pop_front(pOutStrm->pOutBufQ, (uint64_t*)&bufNode );

      if (ADSP_EOK == result)
      {
         // Free the buffer
         qurt_elite_memory_free(bufNode.pBuffer);
         pOutStrm->nBufsAllocated--;
      }
      // shouldn't reach this point.
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error %d in destroying buffers in Decoder svc!!\n", result);
      }
   }
}

ADSPResult AudioDecSvc_HandlePortDataThreshChangeEvent(AudioDecSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;

   if (!(pMe->event_mask & AUD_DEC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK))
   {
	   return ADSP_EOK;
   }

   MSG(MSG_SSID_QDSP6,DBG_MED_PRIO,"Handling PortDataThreshChangeEvent in dec service");

   dec_capi_port_index_t in_capi_port_index;
   dec_capi_port_index_t out_capi_port_index;
   in_capi_port_index.index = 0; in_capi_port_index.valid = false;
   out_capi_port_index.index = 0; out_capi_port_index.valid = false;

   for (uint8_t capi_index=0; capi_index<DEC_SVC_MAX_CAPI; capi_index++)
   {
      if(NULL == pMe->capiContainer[capi_index]) break;

      out_capi_port_index.index = 0; out_capi_port_index.valid = false;

      //for input streams
      for (uint8_t in_stream_index=0; in_stream_index<DEC_SVC_MAX_INPUT_STREAMS; in_stream_index++)
      {
         //assume that event would be raised only if port is created(which is generally true)
         if (pMe->capiContainer[capi_index]->in_port_event_new_size[in_stream_index])
         {
            in_capi_port_index.index = in_stream_index; in_capi_port_index.valid = true;

            result |= AudioDecSvc_RecreateBuffers(pMe, in_capi_port_index, out_capi_port_index, capi_index);

            //clear the size
            pMe->capiContainer[capi_index]->in_port_event_new_size[in_stream_index] = 0;
         }
      }

      in_capi_port_index.index = 0; in_capi_port_index.valid = false;

      //for output streams
      for (uint8_t out_stream_index=0; out_stream_index<DEC_SVC_MAX_OUTPUT_STREAMS; out_stream_index++)
      {
         if (pMe->capiContainer[capi_index]->out_port_event_new_size[out_stream_index])
         {
            out_capi_port_index.index = out_stream_index; out_capi_port_index.valid = true;

            result |= AudioDecSvc_RecreateBuffers(pMe, in_capi_port_index, out_capi_port_index, capi_index);

            //clear the size
            pMe->capiContainer[capi_index]->out_port_event_new_size[out_stream_index] = 0;
         }
      }
   }

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"failed to recreate buffers");
      result = ADSP_EFAILED;
   }

   //clear all events (even if error)
   pMe->event_mask &= (~AUD_DEC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK);

   return result;
}

