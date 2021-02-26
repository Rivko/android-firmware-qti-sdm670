
/*==========================================================================
ELite Source File

This file implements the buffer management functions for the Audio Post
Processing Dynamic Service.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/09/10   SS      Adding data tapping support (Sim) for Postprocessing
06/09/10   DG      Created file and copied functions from AudDynaPPSvc.cpp.
=========================================================================== */

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "audproc_comdef.h"
#include "EliteTopology_db_if.h"
#include "EliteCmnTopology_db.h"
#include "AFEInterface.h"

/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/
//enable to debug buffer delay pointer
//#define DBG_AUDPROC_BUFDELAY
/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

/**
 * Returns the current input buffer that is being worked on, if
 * any. This function checks if the sender was the Scorpion or
 * the upstream service, and returns the buffer accordingly.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 * Pointer to the current PP instance structure.
 */
void AudPP_FreeCurrentInputBuffer(ThisAudDynaPPSvc_t* me, uint32_t port_id)
{
	AudPPInputPort_t *pInPort = me->in_port[port_id];

   if (pInPort->pInDataMsg !=NULL)
   {
      // if data buffer being processed, need to enQueue it back.
      // data buffer comes from upstream svc
      ADSPResult result;
      result = elite_msg_return_payload_buffer(pInPort->pInDataMsg);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: Failed to return buffer to upstream svc.");
      }
      pInPort->inputBufferCount++;

      pInPort->prevInputBufInfo.flags = pInPort->inputBufInfo.flags;
      pInPort->prevInputBufInfo.offsetInSamples = pInPort->inputBufInfo.offsetInSamples;
      pInPort->prevInputBufInfo.sizeInSamples = pInPort->inputBufInfo.sizeInSamples;
      pInPort->prevInputBufInfo.timestamp = pInPort->inputBufInfo.timestamp;

      //reset input buffer offset, pointers etc.
      pInPort->pInDataMsg = NULL;

      pInPort->inputBufInfo.pBuf = NULL;
      pInPort->inputBufInfo.offsetInSamples = 0;
      pInPort->inputBufInfo.sizeInSamples = 0;
      pInPort->inputBufInfo.timestamp = 0;
      memset(&(pInPort->inputBufInfo.flags), 0 , sizeof(pInPort->inputBufInfo.flags));

   }      // if pInDataMsg != NULL;
}

/**
 * Frees the current output buffer, if the PP is working on any.
 * It does this by returning the buffer to the buffer queue.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 * Pointer to the current PP instance structure.
 */
void AudPP_FreeCurrentOutputBuffer(ThisAudDynaPPSvc_t* me)
{
   AudPPStatus_t *pPPStat = &me->audPPStatus;

   if ( pPPStat->pOutDataMsgDataBuf != NULL )
   {
      ADSPResult result;
      result = elite_msg_push_payload_to_returnq(pPPStat->pOutDataMsgDataBuf->pBufferReturnQ,
            (elite_msg_any_payload_t*) pPPStat->pOutDataMsgDataBuf);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Error in freeing outstanding output buffer. Could not queue to buffer queue!");
      }

      // clear output data cmd pointer.
      pPPStat->pOutDataMsgDataBuf = NULL;

      // reset flags when necessary.
      pPPStat->outputBufInfo.pBuf = NULL;
      pPPStat->outputBufInfo.offsetInSamples = 0;
      pPPStat->outputBufInfo.sizeInSamples = 0;
   }
}

/**
 * This function is used for sanity checking to ensure that the
 * PP is not holding to any input or output buffers when it
 * destroys itself.
 *   - Dependencies: None
 *   - Side Effects: Frees the current input/output buffers if
 *     there are any.
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 * Pointer to the instance structure.
 */
void AudPP_CheckForOutstandingBuffers(ThisAudDynaPPSvc_t* me)
{
   AudPPStatus_t *pPPStat = &me->audPPStatus;

   for(uint32_t port_id=0;port_id<me->numActiveInPorts;port_id++)
   {
	   if (NULL != me->in_port[port_id]->pInDataMsg)
	      {
	         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: Holding an input buffer when destroy was called. Returning it. ");
	         if (AudPP_isMarkBuffer(me->in_port[port_id]->pInDataMsg))
	         {
	            AudPP_DiscardMarkBuffer(me,me->in_port[port_id]->pInDataMsg);
	         }
	         AudPP_FreeCurrentInputBuffer(me,port_id);
	      }
   }

   if (NULL != pPPStat->pOutDataMsgDataBuf)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: Holding an output buffer when destroy was called. Returning it to buffer queue. ");
      AudPP_FreeCurrentOutputBuffer(me);
      // We should not have an output buffer during destroy, since we should get disconnect before destroy.
      QURT_ELITE_ASSERT(0);
   }
}

/**
 * Discard the current Mark Buffer
 *   - Dependencies: Must be a mark Buffer
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] me
 * Pointer to the PP Service
 * @param[i] dataQMsg.
 * The current dataQMsg, after making sure that this is a mark buffer
 *
 * @return ADSPResult
 * Returns an ADSP error code indicating the status.
 */
ADSPResult AudPP_DiscardMarkBuffer(ThisAudDynaPPSvc_t *me,elite_msg_any_t *pDataQMsg)
{
   ADSPResult result = ADSP_EOK;
   if (!AudPP_isMarkBuffer(pDataQMsg))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR: Expecting mark buffer only");
      return ADSP_EFAILED;
   }
   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudPP discarding mark buffers");
   elite_msg_data_mark_buffer_t *pMarkBufferPayload = (elite_msg_data_mark_buffer_t *)pDataQMsg->pPayload;
   asm_data_event_mark_buffer_v2_t eventPayload;
   eventPayload.token_lsw = pMarkBufferPayload->token_lsw;
   eventPayload.token_msw = pMarkBufferPayload->token_msw;
   eventPayload.result = ASM_DATA_EVENT_MARK_BUFFER_DISCARDED;
   AudioStreamMgr_CallBackHandleType *pEliteCb = &me->CbData;
   result = AudioStreamMgr_GenerateClientEventFromCb(pEliteCb,
         ASM_DATA_EVENT_MARK_BUFFER_V2,
         0, &eventPayload, sizeof(eventPayload));
   result = elite_msg_release_msg(pDataQMsg);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Failed to release mark buffer message.");
   }
   return result;
}


/**
 * Flushes all the input buffers from the data queue, without doing
 * any processing for them. All other packets are retained.
 *   - Dependencies: The upstream service must not be sending
 *     any data to the PP when this function is called,
 *     otherwise it will cause a race condition.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] dataQ
 * Pointer to the PP data queue.
 *
 * @return ADSPResult
 * Returns an ADSP error code indicating the status.
 */
ADSPResult AudPP_FlushDataQueue(ThisAudDynaPPSvc_t *me, uint32_t port_id)
{
   ADSPResult result;
   elite_msg_any_t dataQMsg;
   elite_msg_any_t nonBufMsgs[MAX_DATA_Q_ELEMENTS];
   uint32_t numNonBufMsgs = 0;
   qurt_elite_queue_t *dataQ = me->in_port[port_id]->handle.dataQ;

   // Go through and return all the data buffers
   while(1)
   {
      // Non-blocking MQ receive
      result = qurt_elite_queue_pop_front(dataQ, (uint64_t*)&dataQMsg);
      if (ADSP_ENEEDMORE == result)
      {
         result = ADSP_EOK;
         break;
      }

      if (ADSP_FAILED(result))
      {
         break;
      }

      if (AudPP_isMarkBuffer(&dataQMsg))
      {
         AudPP_DiscardMarkBuffer(me,&dataQMsg);
      }
      else if (AudPP_isDataBuffer(&dataQMsg))
      {
         result = elite_msg_return_payload_buffer ( &dataQMsg );
         if (ADSP_FAILED(result))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Failed to return buffer to upstream svc.");
         }
      }
      else if ((DYNA_SVC_PP_TYPE_POPREP == me->ppCfgInfo.dynaPPSvcType) && (AudPP_isEosBuffer(&dataQMsg)))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: Releasing EOS data Q message during flush");
         result = elite_msg_release_msg(&dataQMsg);
         if (ADSP_FAILED(result))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Failed to release data Q message.");
         }
      }
      else
      {
         if (numNonBufMsgs < MAX_DATA_Q_ELEMENTS)
         {
            nonBufMsgs[numNonBufMsgs++] = dataQMsg;
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Releasing data Q message since internal array is full.");
            result = elite_msg_release_msg(&dataQMsg);
            if (ADSP_FAILED(result))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Failed to release data Q message.");
            }
         }
      }

      if (ADSP_FAILED(result))
      {
         break;
      }
   }

   // Now restore the non data buffer messages
   for (uint32_t i = 0; i < numNonBufMsgs; i++)
   {
      ADSPResult errCode = qurt_elite_queue_push_back(dataQ, (uint64_t*)&nonBufMsgs[i]);
      if (ADSP_FAILED(errCode))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Failed to restore a data Q message.");
         result = errCode;

         errCode = elite_msg_release_msg(&nonBufMsgs[i]);
         if (ADSP_FAILED(errCode))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Failed to release data Q message.");
         }
      }
   }

   return result;
}

/**
 * Delivers the current output buffer to the downstream service.
 * This function is useful for sending partially filled output
 * buffers, for cases like change in media type and EOS.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] me
 * Pointer to the current instance structure
 * @param[in] numChannels
 * Optional parameter specifying the number of output channels in the buffer. If
 * not provided, the number of channels is read from the output media type info
 * in the me structure.
 */
void AudPP_deliverCurrentOutputBuffer(ThisAudDynaPPSvc_t* me, uint16_t numChannels, uint16_t bitspersample)
{
   ADSPResult result;
   AudPPStatus_t *pMyStat = &me->audPPStatus;

   if (NUM_CHANNELS_INVALID == numChannels)
   {
      numChannels = me->outputMediaFormatInfo.channels;
   }

   if (BITS_PER_SAMPLE_INVALID == bitspersample)
   {
	   bitspersample = me->outputMediaFormatInfo.bitsPerSample;
   }

   if (NULL != pMyStat->pOutDataMsgDataBuf)
   {
	   if (0 == pMyStat->pOutDataMsgDataBuf->nActualSize)
	   {
		   AudPP_FreeCurrentOutputBuffer(me);
		   return;
	   }

#ifdef DBG_AUDPROC_BUFDELAY
           MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "audproc_svc: P%hX: sending buffer. Size = %ld, buffer TS [%lu %lu]",
               me->objId,pMyStat->pOutDataMsgDataBuf->nActualSize,(uint32_t)(pMyStat->pOutDataMsgDataBuf->ullTimeStamp>>32),
               (uint32_t)pMyStat->pOutDataMsgDataBuf->ullTimeStamp
               );
#endif

#ifdef AUDPROC_SIM
      if (pMyStat->fdbg)
      {
         uint32_t sampPerChannel = pMyStat->pOutDataMsgDataBuf->nActualSize>>2;

         for (uint32_t i=0; i<sampPerChannel; i++)
         {
            for(uint32_t ch=0;ch<2;ch++){

               pMyStat->dbgptr[i*2+ch] = pMyStat->outputBufInfo.pBuf[ch*sampPerChannel+i];
            }
         }

         if (fwrite(pMyStat->dbgptr, sizeof(int16_t), sampPerChannel<<1, pMyStat->fdbg) != sampPerChannel<<1)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SIM: Write error ");
         }
      }
#endif // AUDPROC_SIM

      if((DYNA_SVC_PP_TYPE_COPP == me->ppCfgInfo.dynaPPSvcType) || (DYNA_SVC_PP_TYPE_POPP == me->ppCfgInfo.dynaPPSvcType))
                     AudPP_LogData_Output(me,AUDPP_PRIMARY_OUT_PORT);

      // re-arrange stereo output buffer if needed.
      AudPP_PackChannelData(me, numChannels, bitspersample);

      // send output buffers to downstream service.
      elite_msg_any_t *pOutDataMsg;
      //Preserve client token in the current buffer. It stores the buffer delay.
      elite_msg_data_buffer_t *pCurrentBuffer =(elite_msg_data_buffer_t*) (pMyStat->outBufMgrNode.pBuffer);
      pOutDataMsg = elite_msg_convt_buf_node_to_msg(&(pMyStat->outBufMgrNode),
                                                    ELITE_DATA_BUFFER,
                                                    NULL, /* do not need response */
                                                    pCurrentBuffer->unClientToken,    /* token */
                                                    0     /* do not care response result*/
                                                    );

      result = qurt_elite_queue_push_back(me->pDownstreamPeer->dataQ, (uint64_t*)pOutDataMsg);

      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "audproc_svc: P%hX: Can't enqueue output to downstream dataQ. Dropping.!", me->objId);

         // enqueue it back to buffer Q.
         (void) elite_msg_return_payload_buffer(pOutDataMsg);
      }

      me->output_buffer_count++;
      // MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Delivered out %8lx ", me->objId, (uint32) pMyStat->pOutDataMsgDataBuf);

      // clear output data cmd pointer.
      pMyStat->pOutDataMsgDataBuf = NULL;

      // reset flags when necessary.
      pMyStat->outputBufInfo.pBuf = NULL;
      pMyStat->outputBufInfo.offsetInSamples = 0;
      pMyStat->outputBufInfo.sizeInSamples = 0;

      if (DYNA_SVC_PP_STATE_PAUSE==pMyStat->dynaPPSvcState)
      {
         (pMyStat->nSoftPauseBufCounter)--;
         if (pMyStat->nSoftPauseBufCounter <= 0)
         {
            // Ramping down is done, don't consume any more data.
            AudPP_GotoSleepState(me);
         }
      }
   }
}

/**
 * Tries to get a new output buffer. If none is found it just
 * returns. The caller can determine whether an output buffer
 * was found by checking me->audPPStatus.pOutDataMsgDataBuf. It
 * will be NULL if no output buffer is found.
 *   - Dependencies: There should not be any current output
 *     buffer being worked on. In other words,
 *     me->audPPStatus.pOutDataMsgDataBuf should be NULL.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 * Pointer to the current instance structure.
 */
ADSPResult AudPP_TryGetOutputBuffer (ThisAudDynaPPSvc_t* me)
{
   AudPPStatus_t *pMyStat = &me->audPPStatus;
   ADSPResult result = ADSP_EOK;

   QURT_ELITE_ASSERT(NULL == pMyStat->pOutDataMsgDataBuf);

   while ((0 != qurt_elite_channel_poll(&(me->channel), qurt_elite_queue_get_channel_bit(me->bufQ)))
          && (NULL == pMyStat->pOutDataMsgDataBuf))
   {

      // dequeue output buffer
      result = qurt_elite_queue_pop_front(me->bufQ, (uint64_t*)&(pMyStat->outBufMgrNode));
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: %hX: Failed to get output buffer even when channel signal is set.", me->objId);
         return result;
      }

      // setup output data buffer header as needed.
      pMyStat->pOutDataMsgDataBuf = (elite_msg_data_buffer_t*) (pMyStat->outBufMgrNode.pBuffer);

      QURT_ELITE_ASSERT(0 <= pMyStat->pOutDataMsgDataBuf->nMaxSize);
      if (me->outputBufSizeInBytes == (uint32_t)pMyStat->pOutDataMsgDataBuf->nMaxSize)
      {
      pMyStat->pOutDataMsgDataBuf->pBufferReturnQ = (me->bufQ);
      pMyStat->pOutDataMsgDataBuf->pResponseQ = NULL;
      // no need to set unResponseResult, current architecture doesn't wait for
      // data processing response.
      // timeStamp would be set later for delivering result.
      // output of AudPP are always in deinterleave format (default format PP outputs)
#ifdef DBG_AUDPROC_BUFDELAY
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: %hX: buffer delay calculation: Maxsize %ld, dynaPPSvcOutSizeInSample: %d, Client Token Value %ld",
            me->objId, pMyStat->pOutDataMsgDataBuf->nMaxSize,me->ppCfgInfo.dynaPPSvcOutSizeInSample, pMyStat->pOutDataMsgDataBuf->unClientToken);
#endif

      audpp_buf_delay_mgr_update_delay(&me->bufDelayMgr, pMyStat->pOutDataMsgDataBuf->unClientToken);
      pMyStat->pOutDataMsgDataBuf->unClientToken = audpp_buf_delay_mgr_get_current_delay(&me->bufDelayMgr);

      pMyStat->pOutDataMsgDataBuf->nFlag = 0;
      pMyStat->pOutDataMsgDataBuf->nOffset = 0;
      // reset buffer actual size.
      pMyStat->pOutDataMsgDataBuf->nActualSize = 0;
      // nMaxSize was setup while create the buffers in svc creation func.

      pMyStat->outputBufInfo.pBuf = (int8_t *) (&(pMyStat->pOutDataMsgDataBuf->nDataBuf));
      pMyStat->outputBufInfo.offsetInSamples = 0;
      pMyStat->outputBufInfo.sizeInSamples = me->ppCfgInfo.dynaPPSvcOutSizeInSample;
      }
      else
      {
         // This buffer size has been changed, just free it and allocate a buffer with sufficient size.
         // Could happen when the number of channels increases, and so bigger output buffers are allocated.
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: %hX: Buffer size mismatch:nMaxsize %ld, outputBufferSize: %ld", me->objId, pMyStat->pOutDataMsgDataBuf->nMaxSize,me->outputBufSizeInBytes);
         audpp_buf_delay_mgr_buffer_freed(&me->bufDelayMgr, pMyStat->pOutDataMsgDataBuf->unClientToken);
         qurt_elite_memory_free(pMyStat->pOutDataMsgDataBuf);
         pMyStat->pOutDataMsgDataBuf = NULL;
         pMyStat->outBufMgrNode.pBuffer = NULL;
         me->nBufsAllocated--;
         //And now, allocate new output buffers.
         result = AudPP_AllocateOutputBuffers(me, me->outputBufSizeInBytes, 1);
      }
   } // if empty buffer available.

   return result;
}

/*===========================================================================
  FUNCTION AudPP_PackChannelData()

  DESCRIPTION
  For the case of more than one input channel, if the output buffer is
  partially filled, the channel data need to be packed together. This makes
  the second channel data immediately follow the first and so on.
  PARAMETERS
Input:  me points to the AudDynaPPSvc instance
        numChannels gives the number of output channels

RETURN VALUE
none.

DEPENDENCIES

SIDE EFFECTS

NOTES:

===========================================================================*/
void AudPP_PackChannelData(const ThisAudDynaPPSvc_t* me, const uint16_t numChannels, const uint16_t bitspersample)
{
   elite_msg_data_buffer_t* pOutDataMsgDataBuf = me->audPPStatus.pOutDataMsgDataBuf;

   // get actual # samples in output buffer
   QURT_ELITE_ASSERT(pOutDataMsgDataBuf->nActualSize >= 0);

   uint32_t bufferSize = (uint32_t)(pOutDataMsgDataBuf->nActualSize);
   uint32_t bytesPerSample = (bitspersample > 16) ? 4 : 2;

   uint32_t bytesPerChannel = bufferSize / numChannels;
   uint32_t maxBytesPerChannel = me->ppCfgInfo.dynaPPSvcOutSizeInSample * bytesPerSample;

   if ((numChannels > 1) && (!me->outputMediaFormatInfo.isInterleaved))
   {
      // if output buffer not full, need to pack the channels
      if (bytesPerChannel < maxBytesPerChannel)
      {
         int8_t* pOutBuf = (int8_t*) (&(pOutDataMsgDataBuf->nDataBuf));
         int8_t* pBufDest = pOutBuf + bytesPerChannel;
         int8_t* pBufSrc = pOutBuf + maxBytesPerChannel;

         for (uint32_t i = 1; i < numChannels; i++)
         {
            memsmove(pBufDest,bytesPerChannel, pBufSrc, bytesPerChannel);

            pBufDest += bytesPerChannel;
            pBufSrc += maxBytesPerChannel;
         }
      }
   }
}

/**
 * Allocates the PP output buffers.
 *   - Dependencies: The buffer queue must be created before
 *     calling this function.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 *   Pointer to the current instance structure.
 * @param[in] bufSize
 *   Size of each buffer, in bytes.
 * @param[in] numBufs
 *   Number of buffers to be created.
 *
 * @return ADSPResult
 * Returns an ADSP error code indicating the status.
 */
ADSPResult AudPP_AllocateOutputBuffers(ThisAudDynaPPSvc_t *me, const uint32_t bufSize, const uint32_t numBufs)
{
   ADSPResult result = ADSP_EOK;
   uint32_t reqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(bufSize);
   elite_msg_data_buffer_t* pBuf;

#ifdef LOWLAT_DBG
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudPP_AllocateOutputBuffers numBufs is %d, bufSize is %d ",numBufs,bufSize);
#endif

   for (uint32_t i = 0; i < numBufs; i++)
   {
      //allocate the databuffer payload (metadata + pcm buffer size)
      if (DYNA_SVC_PP_TYPE_POPP == (me->ppCfgInfo.dynaPPSvcType))
      {
         pBuf = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(reqSize, PP_SVC_POPP_OUT_BUF);
      }
      else if (DYNA_SVC_PP_TYPE_COPP == (me->ppCfgInfo.dynaPPSvcType))
      {
         pBuf = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(reqSize, PP_SVC_COPP_OUT_BUF);
      }
      else
      {
           pBuf = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(reqSize, QURT_ELITE_HEAP_DEFAULT);
      }

      if (!pBuf)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: Insufficient Memory for PP output buffer.It requires %lu bytes", me->objId , reqSize);
         return ADSP_ENOMEMORY;
      }

#ifdef DBG_BUFFER_ADDRESSES
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR PP output buffer: 0x%8x, size %d",
            pBuf, reqSize);
#endif /* DBG_BUFFER_ADDRESSES */

      // zero out all the fields.
      memset(pBuf, 0, reqSize);

      me->nBufsAllocated++;

      pBuf->nMaxSize = (int32_t)bufSize;
      pBuf->nActualSize = 0;
      pBuf->nOffset = 0;

      // didn't setup buffer flag & time stamp yet.
      // not set up responseQ (no response needed) & tocken yet.

      pBuf->pBufferReturnQ = me->bufQ;

      if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(me->bufQ, (elite_msg_any_payload_t*) pBuf)))
      {
         MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "audproc_svc: Fail to fill AudPP's buffer queue!");
         // release the data cmd element outside of the data Q.
         qurt_elite_memory_free(pBuf);
         me->nBufsAllocated--;
         return result;
      }
   }    // for each buffer Q element

   me->outputBufSizeInBytes = bufSize;

   return result;
}

ADSPResult AudPP_CheckAndAdjustPCMBufSize(ThisAudDynaPPSvc_t *me)
{
   ADSPResult result = ADSP_EOK;
   uint32_t unAfeFrameSizeInSamples;
   //calculate the updated output # of samples per channel for constant buffer duration
   
   uint32_t unFreq = 48000;
   //If final output media format is configured, update the size
   if (me->outputMediaFormatInfo.samplingRate)
   {
      unFreq = me->outputMediaFormatInfo.samplingRate;
   }
   elite_svc_get_frame_size(unFreq,&unAfeFrameSizeInSamples);

   me->ppCfgInfo.dynaPPSvcOutSizeInSample = ((me->ppCfgInfo.dynaPPSvcOutBufNumAfeFrames) * unAfeFrameSizeInSamples);
   me->ppCfgInfo.dynaPPSvcOutDurationInUsec = (((uint64_t)((me->ppCfgInfo.dynaPPSvcOutSizeInSample) * 1000 * 1000)) / unFreq);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "audproc_svc: P%hX adjusted bufSize for constant buffer duration is %hu samples",me->objId,me->ppCfgInfo.dynaPPSvcOutSizeInSample);

   uint32_t numChannels = me->outputMediaFormatInfo.channels;
   uint32_t bytes_per_sample_out = (me->outputMediaFormatInfo.bitsPerSample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
   uint32_t bufSize = me->ppCfgInfo.dynaPPSvcOutSizeInSample * numChannels * bytes_per_sample_out;

   /* Allowing media type data path command from upstream to configure the PP output buffers correctly.
    * adjust the PP o/p buffers if the originally allocated buffer size are either more or lesser than the requested buffer size.
    */
   me->outputBufSizeInBytes = bufSize;

   audpp_buf_delay_mgr_buf_duration_change(&me->bufDelayMgr, me->ppCfgInfo.dynaPPSvcOutSizeInSample, me->outputMediaFormatInfo.samplingRate);

   return result;
}

ADSPResult AudPP_CheckAndAdjustCompressedBufSize(ThisAudDynaPPSvc_t *me, uint32_t actualSize, uint32_t maxSize, uint32_t inSizeInSamples)
{
	me->ppCfgInfo.dynaPPSvcOutSizeInSample = inSizeInSamples;

	//reinit the topology with updated sample size
	AudPP_ReInitCompressTopoWithOutputSampleSize(me);
	me->outputBufSizeInBytes = maxSize;

   audpp_buf_delay_mgr_buf_duration_change(&me->bufDelayMgr, me->ppCfgInfo.dynaPPSvcOutSizeInSample, me->outputMediaFormatInfo.samplingRate);

	return ADSP_EOK;
}

void audpp_buf_delay_mgr_init(audpp_buf_delay_mgr *obj, AudPP_AudProcType svc_type, volatile uint32_t *buf_delay_ptr)
{
   obj->buf_delay_ptr = buf_delay_ptr;
   obj->svc_type = svc_type;
   obj->current_delay = 0;

   if (NULL != obj->buf_delay_ptr)
   {
      *obj->buf_delay_ptr = 0;
   }
}

void audpp_buf_delay_mgr_buf_duration_change(audpp_buf_delay_mgr *obj, uint32_t out_size_in_sample, uint32_t sampling_rate)
{
   //TODO: see with Q6 instruction if we can optimize this.
   obj->current_delay = 1000000ull * out_size_in_sample / sampling_rate;

   if (DYNA_SVC_PP_TYPE_COPP != obj->svc_type)
   {
      if (NULL != obj->buf_delay_ptr)
      {
         *obj->buf_delay_ptr = obj->current_delay;
      }
   }
}

void audpp_buf_delay_mgr_buffer_freed(audpp_buf_delay_mgr *obj, uint32_t buf_delay)
{
   if (DYNA_SVC_PP_TYPE_COPP == obj->svc_type)
   {
      if (NULL != obj->buf_delay_ptr)
      {
         QURT_ELITE_ASSERT(*obj->buf_delay_ptr > buf_delay);
         *obj->buf_delay_ptr -= buf_delay;
      }
   }
}

void audpp_buf_delay_mgr_update_delay(audpp_buf_delay_mgr *obj, uint32_t buf_delay)
{
   if (DYNA_SVC_PP_TYPE_COPP == obj->svc_type)
   {
      if (NULL != obj->buf_delay_ptr)
      {
         if (buf_delay != obj->current_delay)
         {
            QURT_ELITE_ASSERT(*obj->buf_delay_ptr > buf_delay);
            uint32_t delay = *obj->buf_delay_ptr;
            delay -= buf_delay;
            delay += obj->current_delay;
            *obj->buf_delay_ptr = delay;

#ifdef DBG_AUDPROC_BUFDELAY
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: updating buffer delay :old delay %ld, new delay: %ld",
            buf_delay, obj->current_delay);
#endif

         }
      }
   }
}

uint32_t audpp_buf_delay_mgr_get_current_delay(audpp_buf_delay_mgr *obj)
{
   return obj->current_delay;
}
