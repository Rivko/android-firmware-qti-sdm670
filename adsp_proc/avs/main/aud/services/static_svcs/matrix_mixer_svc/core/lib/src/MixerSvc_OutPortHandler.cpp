/**
@file MixerSvc_OutPortHandler.cpp
@brief This file defines functions that handle the arrival of a
       buffer at an output port of the audio matrix mixer.

 */

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_OutPortHandler.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/2010 AAA     Created file.
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"
#include "adsp_media_fmt.h"
#include "adsp_adm_api.h"
#include "adsp_asm_session_commands.h"
#include "MixerSvc_OutPortHandler.h"
#include "MixerSvc_InPortHandler.h"
#include "MixerSvc_Util.h"
#include "avsync_lib.h"
#include "adsp_private_api_ext.h"
#include "MixerSvc_ChanMixUtils.h"
#include "MixerSvc_AvSync.h"
#include "MixerSvc_Limiter.h"

static void MxAr_OutPortToCheckForUnderflowEvents(This_t *me, uint32_t unOutPortID);
static void MxAr_CommonOutPortProcessingRoutine(This_t *me, uint32_t unOutPortID);
static bool_t MxAr_OutPortToCheckIfItIsOkToProceed(This_t *me, uint32_t unOutPortID);

void MxAr_ProcessBufQ(This_t *me, uint32_t unOutPortID)
{
    elite_msg_any_t          *pPeerDataQMsg;
    qurt_elite_bufmgr_node_t outBufMgrNode;
    int16_t                  *ptr_data_buf_16;
    int32_t                  *ptr_acc_buf_32, *ptr_data_buf_32;
    int64_t                  *ptr_acc_buf_64;
    uint32_t                 i;
    ADSPResult               result;
    MatrixOutPortInfoType    *pCurrentOutPort;

	MtMx_ClearOutputPortChannelStatus(me, unOutPortID);

	pCurrentOutPort = me->outPortParams[unOutPortID];

	//Output ports are always in PULL mode
	//Check if this o/p port is a secondary port and if any accumulation has happened on this port.
	//If yes, proceed normally (Yes means either that this is a top prio port or a secondary port with atleast one acc.)
	//If no, wait (for primary port to trigger accumulation).
	//This will accommodate for jitters in the returned buffers from COPP(AFE).
	if(FALSE == MxAr_OutPortToCheckIfItIsOkToProceed(me, unOutPortID))
	{
		//We do not need to set the o/p channel status bit again. Buffer is already available.
		//Since no accumulation has happened yet, simply exit (dont pop the buffer yet).
		//When accumulation does happen, this entire function will be called again and
		//at that time, it will be OK to proceed further normally.
		pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = TRUE;
		MtMx_RemoveOutputPortFromWaitMask(me, unOutPortID);

		// Set the corresponding bit in outputReqPendingMask of all the active i/p ports feeding to this o/p port.
		MtMx_OutPortToUpdateOutputReqPendingMask(me, unOutPortID);

#ifdef MT_MX_EXTRA_DEBUG
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu reg o/p port %lu is non top prio port and has no acc yet. It will wait.", me->mtMxID, unOutPortID);
#endif

		return;
	}

	pCurrentOutPort->numBufRcvd++;

	result = qurt_elite_queue_pop_front(pCurrentOutPort->bufQ, (uint64_t*)&outBufMgrNode);

	elite_msg_data_buffer_t* pOutputBuf = (elite_msg_data_buffer_t*)outBufMgrNode.pBuffer;

	if(TRUE == pCurrentOutPort->bIsOutPortPendingReleaseBuffers && pCurrentOutPort->unNumOutputBufsPendingRelease > 0)
	{
		//If this port is pending release buffers, check to make sure that this is indeed the right buffer to release
		uint32_t unCurrentOpBufSize = pCurrentOutPort->unBufSize * pCurrentOutPort->unBytesPerSample;

		//If incoming new o/p buffer on this port has a different size , then release it.
		//If the size does match, then DO NOT release it, and proceed normally.
		if((pOutputBuf->nActualSize) != (int32_t)(unCurrentOpBufSize))
		{
			MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu Pop&Destroy Buf#%lu, 0x%p, buf act size %lu, o/p port curr size %lu ",
					me->mtMxID, unOutPortID, pCurrentOutPort->unNumOutputBufsPendingRelease, outBufMgrNode.pBuffer, pOutputBuf->nActualSize, unCurrentOpBufSize);

			MTMX_FREE  (outBufMgrNode.pBuffer);
			pCurrentOutPort->unNumOutputBufsPendingRelease --;

			if (0 == pCurrentOutPort->unNumOutputBufsPendingRelease)
			{
				pCurrentOutPort->bIsOutPortPendingReleaseBuffers = FALSE;
			}

			return;
		}
	}

	//This port is no longer waiting on resources to carry on. Clear this flag
	pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = FALSE;
  
	if ((NULL != pCurrentOutPort->structLimiter.pLimiterCapiV2) && (pCurrentOutPort->structLimiter.bIsLimiterEnable) && (MT_MX_BIT_WIDTH_32 != pCurrentOutPort->unBitwidth))
	{
	
		int8_t  *working_buf_1, *working_buf_2;
		
		working_buf_1 = (int8_t*)pCurrentOutPort->pAccBuf;
		working_buf_2 = (int8_t*)(&(pOutputBuf->nDataBuf));;
		
		MtMx_OutPortLimiterProcess(me, unOutPortID, working_buf_1, working_buf_2, pCurrentOutPort->unOutPortPerChBufSize );
	}
	else
	{
		ptr_acc_buf_32 = (int32_t*)pCurrentOutPort->pAccBuf;
		
		if(MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentOutPort->unBytesPerSample)
		{
			ptr_data_buf_16 = (int16_t*)(&(pOutputBuf->nDataBuf));
			
			// Copy samples from o/p port's AccBuf to OutputBuf and clear AccBuf
			for (i = 0; i < pCurrentOutPort->unBufSize; i++)
			{
			
				// Saturate 32-bit data to 16-bit and sign extend again to 32-bit word
				//  and truncate again back to 16-bit word
				*ptr_data_buf_16++ = (int16_t)Q6_R_sath_R(*ptr_acc_buf_32++);
			}
		}
		else
		{
			ptr_data_buf_32 = (int32_t *) (&(pOutputBuf->nDataBuf));
			
			//RR: TBD: Remove this saturation to Q27 and allow data to overflow
			if(24 == pCurrentOutPort->unBitwidth)
			{
				for(i = 0; i < pCurrentOutPort->unBufSize; i++)
				{
					// Saturate Q28 result
					*ptr_acc_buf_32 = Q6_R_asl_RI_sat(*ptr_acc_buf_32, QFORMAT_SHIFT_FACTOR);
					
					// Scale it back to Q28
					*ptr_data_buf_32 = (*ptr_acc_buf_32 >> QFORMAT_SHIFT_FACTOR);
					
					// Increment read/write pointers
					ptr_acc_buf_32++;
					ptr_data_buf_32++;
				}
			}
			else
			{
				/* Use the 64 bit accumulator data*/
				ptr_acc_buf_64 = pCurrentOutPort->pAccBuf;
				for(i = 0; i < pCurrentOutPort->unBufSize; i++)
				{
				
					/* Saturate to Q31 */
					*ptr_data_buf_32 = s32_saturate_s64(*ptr_acc_buf_64);
					
					/* Increment Read/write pointers */
					ptr_acc_buf_64++;
					ptr_data_buf_32++;
				}
			
			} /* 32 bit output port bit width*/
		} /* 4 bytes per sample */
	
	} 
  
	memset(pCurrentOutPort->pAccBuf, 0, pCurrentOutPort->unBufSize * sizeof(int64_t));


	pOutputBuf->nFlag          = 0;

	// If we have an input with a valid TS, and the input port ID is within bounds, copy the timestamp to the output buffer.
	MtMx_SetOutputBufferTimestamp(me, unOutPortID, pOutputBuf);
	pOutputBuf->nOffset        = 0;
	pOutputBuf->nActualSize    = pCurrentOutPort->unBufSize * pCurrentOutPort->unBytesPerSample;
	pOutputBuf->nMaxSize       = pCurrentOutPort->unBufSize * pCurrentOutPort->unBytesPerSample;
	pCurrentOutPort->numBufSent++;

	// Send OutputBuf downstream.
	pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
			&outBufMgrNode,
			ELITE_DATA_BUFFER,
			NULL,
			0,
			0
	);

#ifdef MT_MX_EXTRA_DEBUG
	MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu, Sending Buf#%lu: 0x%p Act Size: %lu", me->mtMxID, unOutPortID, pCurrentOutPort->numBufSent, pOutputBuf, pOutputBuf->nActualSize);
#endif

	result = qurt_elite_queue_push_back(pCurrentOutPort->pDownstreamPeerHandle->dataQ, (uint64_t*)pPeerDataQMsg );

	if (ADSP_FAILED(result))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping");
		(void)elite_msg_push_payload_to_returnq(
				pCurrentOutPort->bufQ, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
	}

	//Check if output port is operating in native mode and needs to be re-initialized
	if ((pCurrentOutPort->unNativeModeFlags) && pCurrentOutPort->bIsOutPortInNativeModePendingReInit)
	{
		if (ADSP_FAILED(result = MtMx_ReInitNativeModeOutport(me, unOutPortID)))
		{
			// If re-init failed, stop processing on this o/p port
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MxAr_ProcessBufQ o/p port %lu MtMx_ReInitNativeModeOutport failed. Res = %d.", unOutPortID, result);
			return;
		}
	}

	// Update session time for i/p's feeding to this o/p port
	(void)MxAr_OutPortToHonorInPortsTimestamps(me, unOutPortID);

	// Check if any input port feeding to this output port registered for underflow event
	// If it did, and couldn't deliver samples to the output port in time, raise an underflow event
	MxAr_OutPortToCheckForUnderflowEvents(me, unOutPortID);

	// Check if EOS msg is pending on any of the i/p ports feeding to this o/p port.
	// If yes, send the EOS msg downstream.
	MtMx_OutPortToCheckForPendingEOS(me, unOutPortID);

	// In accInPortsMask, clear the bits of the input ports that accumulated into accBuf
	pCurrentOutPort->accInPortsMask = 0;

	// Set the corresponding bit in outputReqPendingMask of all the active i/p ports feeding to this o/p port.
	MtMx_OutPortToUpdateOutputReqPendingMask(me, unOutPortID);

	// Called every time an output buffer is received
	MxAr_CommonOutPortProcessingRoutine(me, unOutPortID);
}

void MxAt_ProcessOutputPortWakeUp(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	MatrixInPortInfoType    *pCurrentInPort;
	uint32_t unInPortID, iMask;

	if(TRUE == pCurrentOutPort->bIsOutPortPendingReleaseBuffers)
	{
		//This may also be called if the TX matrix o/p port is pending to release some buffers
		MxAt_ProcessBufQ(me, unOutPortID);
		return;
	}

	if(TRUE == pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut)
	{
		//If o/p port is waiting for resources to free up to send buffer downstream, it would have
		//been added to the wait mask. In this case, go ahead and process the buffer immediately.
		pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = FALSE;
		MxAt_ProcessBufQ(me, unOutPortID);
		return;
	}

	if(FALSE == me->bIsMxAtOperatingInBurstMode)
	{
		if(FALSE == pCurrentOutPort->bIsTopPriorityInputPortLive)
		{
			iMask = pCurrentOutPort->inPortsMask;
			while(iMask)
			{
				unInPortID = Q6_R_ct0_R(iMask);
				iMask ^= (1 << unInPortID);
				pCurrentInPort = me->inPortParams[unInPortID];
				if (((pCurrentOutPort->inPortsTopPriorityMask) & (1 << unInPortID)) &&
						(pCurrentInPort->bIsNonLiveInputPortWaitingToAcc))
				{
					MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "WARNING! Calling MxAt_OutPortToProcessIfNonLiveInputPortsNeedToAcc from MxAt_ProcessBufQWrapper!");
					MxAt_OutPortToProcessIfNonLiveInputPortsNeedToAcc(me, unOutPortID);
				}
			}
		}
	}
}

void MxAt_ProcessBufQ(This_t *me, uint32_t unOutPortID)
{
    elite_msg_any_t          *pPeerDataQMsg;
    qurt_elite_bufmgr_node_t outBufMgrNode;
    int16_t                  *ptr_data_buf_16;
    int32_t                  *ptr_acc_buf_32, *ptr_data_buf_32;
    int64_t                  *ptr_acc_buf_64;
    uint32_t                 i;
    ADSPResult               result;
    MatrixOutPortInfoType    *pCurrentOutPort;
    elite_msg_data_buffer_t  *pOutputBuf;

	MtMx_ClearOutputPortChannelStatus(me, unOutPortID);
	pCurrentOutPort = me->outPortParams[unOutPortID];

	// Take next buffer off the queue
	result = qurt_elite_queue_pop_front(pCurrentOutPort->bufQ, (uint64_t*)&outBufMgrNode);
	if(ADSP_FAILED(result) && (TRUE == pCurrentOutPort->bIsTopPriorityInputPortLive))
	{
		pCurrentOutPort->numBufRcvd++;
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MxAt_ProcessBufQ o/p port %lu qurt_elite_queue_pop_front returned %x... Top prio i/p port is live! Dropping o/p buf", unOutPortID, result);

		//Raise an overflow event
		MxAt_OutPortToCheckForOverflowEvents(me, unOutPortID);
		goto __commonroutine;
	}
	if((ADSP_EOK != result) && (ADSP_ENEEDMORE != result) && (FALSE == pCurrentOutPort->bIsTopPriorityInputPortLive))
	{
		pCurrentOutPort->numBufRcvd++;
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MxAt_ProcessBufQ o/p port %lu qurt_elite_queue_pop_front returned %x... Top prio i/p port is non-live! Dropping o/p buf", unOutPortID, result);

		//Raise an overflow event
		MxAt_OutPortToCheckForOverflowEvents(me, unOutPortID);
		goto __commonroutine;
	}
	if((ADSP_ENEEDMORE == result) && (FALSE == pCurrentOutPort->bIsTopPriorityInputPortLive))
	{

#ifdef MT_MX_EXTRA_DEBUG
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MxAt_ProcessBufQ o/p port %lu qurt_elite_queue_pop_front returned %x... Top prio i/p port is non-live! Waiting for resources...", unOutPortID, result);
#endif

		pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = TRUE;
		MtMx_AddOutputPortToWaitMask(me, unOutPortID);
		return;
	}

	pCurrentOutPort->numBufRcvd++;

	pOutputBuf = (elite_msg_data_buffer_t*)outBufMgrNode.pBuffer;

	if(TRUE == pCurrentOutPort->bIsOutPortPendingReleaseBuffers && pCurrentOutPort->unNumOutputBufsPendingRelease > 0)
	{
		//If this port is pending release buffers, check to make sure that this is indeed the right buffer to release
		uint32_t unCurrentOpBufSize = pCurrentOutPort->unBufSize * pCurrentOutPort->unBytesPerSample;

		//If incoming new o/p buffer on this port has a different size , then release it.
		//If the size does match, then DO NOT release it, and proceed normally.
		if((pOutputBuf->nActualSize) != (int32_t)(unCurrentOpBufSize))
		{
			MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu Pop&Destroy Buf#%lu, 0x%p, buf act size %lu, o/p port curr size %lu ",
					me->mtMxID, unOutPortID, pCurrentOutPort->unNumOutputBufsPendingRelease, outBufMgrNode.pBuffer, pOutputBuf->nActualSize, unCurrentOpBufSize);

			MTMX_FREE  (outBufMgrNode.pBuffer);
			pCurrentOutPort->unNumOutputBufsPendingRelease --;

			if (0 == pCurrentOutPort->unNumOutputBufsPendingRelease)
			{
				pCurrentOutPort->bIsOutPortPendingReleaseBuffers = FALSE;
			}

			pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = TRUE;
			MtMx_AddOutputPortToWaitMask(me, unOutPortID);

			return;
		}
	}

	//This port is no longer waiting on resources to carry on. Clear this flag
	pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = FALSE;

	ptr_acc_buf_32 = (int32_t*)pCurrentOutPort->pAccBuf;

	// Copy samples from o/p port's AccBuf to OutputBuf and clear AccBuf
	if(MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentOutPort->unBytesPerSample)
	{
		ptr_data_buf_16 = (int16_t *)(&(pOutputBuf->nDataBuf));

		for (i = 0; i < pCurrentOutPort->unBufSize; i++)
		{

			// Saturate 32-bit sample to 16-bit and sign extend again to 32-bits
			// and truncate the result back to 16-bits
			*ptr_data_buf_16++ = (int16_t)Q6_R_sath_R(*ptr_acc_buf_32++);
		}
	}
    else
    {
        ptr_data_buf_32 = (int32_t *) (&(pOutputBuf->nDataBuf));

        //RR: TBD: Remove this saturation to Q27 and allow data to overflow
        if(24 == pCurrentOutPort->unBitwidth)
        {
            for(i = 0; i < pCurrentOutPort->unBufSize; i++)
            {
                // Saturate Q28 result
                *ptr_acc_buf_32 = Q6_R_asl_RI_sat(*ptr_acc_buf_32, QFORMAT_SHIFT_FACTOR);

                // Scale it back to Q28
                *ptr_data_buf_32 = (*ptr_acc_buf_32 >> QFORMAT_SHIFT_FACTOR);

                // Increment read/write pointers
                ptr_acc_buf_32++;
                ptr_data_buf_32++;
            }
        }
        else
        {
            /* Use the 64 bit accumulator data*/
            ptr_acc_buf_64 = pCurrentOutPort->pAccBuf;

            for(i = 0; i < pCurrentOutPort->unBufSize; i++)
            {

                /* Saturate to Q31 */
                *ptr_data_buf_32 = s32_saturate_s64(*ptr_acc_buf_64);

                /* Increment Read/write pointers */
                ptr_acc_buf_64++;
                ptr_data_buf_32++;
            }
        } /* 32 bit output port bit width*/
    } /* 4 bytes per sample */

	pOutputBuf->nFlag         = 0;
	pOutputBuf->nOffset         = 0;
	pOutputBuf->nActualSize     = pCurrentOutPort->unBufSize * pCurrentOutPort->unBytesPerSample;
	pOutputBuf->nMaxSize       = pCurrentOutPort->unBufSize * pCurrentOutPort->unBytesPerSample;

	// Apply timestamp to the buffer
	MtMx_ApplyOutputBufferTS(me, unOutPortID, pOutputBuf);

	pCurrentOutPort->numBufSent++;

	// Send OutputBuf downstream
	pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
			&outBufMgrNode,
			ELITE_DATA_BUFFER,
			NULL,
			0,
			0
	);

#ifdef MT_MX_EXTRA_DEBUG
	MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu, Sending Buf#%lu: 0x%p Act Size: %lu", me->mtMxID, unOutPortID, pCurrentOutPort->numBufSent, pOutputBuf, pOutputBuf->nActualSize);
#endif

	result = qurt_elite_queue_push_back(pCurrentOutPort->pDownstreamPeerHandle->dataQ, (uint64_t*)pPeerDataQMsg );

	if (ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MxAt_ProcessBufQ o/p port %lu failed to deliver bfr DS! Error: %x...Dropping o/p buffer!", unOutPortID, result);
		(void)elite_msg_push_payload_to_returnq(pCurrentOutPort->bufQ, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);

		//Raise an overflow event
		MxAt_OutPortToCheckForOverflowEvents(me, unOutPortID);
	}
	else
	{
		//If it reaches this point, atleast one buffer was sent successfully. Therefore, it is safe to reset the overflow status of this o/p port
		if(TRUE == pCurrentOutPort->bIsRegisteredForOverflowEvents)
		{
			pCurrentOutPort->unOverflowReportStatus = MT_MX_OUTPUT_PORT_OVERFLOW_REPORT_STATUS_REPORT;
		}
	}

	__commonroutine:
	//Now, repeat the standard procedure after a buffer has been sent down stream.

	//For MXAT, almost never should the o/p port be added to the wait mask. Only very specific situations demand this.
	//Remove the o/p from the wait mask
	MtMx_RemoveOutputPortFromWaitMask(me,unOutPortID);

	//Zero out the accBuf
	memset(pCurrentOutPort->pAccBuf, 0, pCurrentOutPort->unBufSize * sizeof(int64_t));

	//Clear some masks
	pCurrentOutPort->accInPortsMask = 0;

	// Set the accBufAvailabilityMask to 1 for all the active i/p ports feeding to this o/p port: a mask that indicates
	// which i/p port can accumulate into this output port's accBuf. At this point, since the o/p buffer and acc. buffer
	// have been sent downstream, all the i/p ports that feed this o/p port can accumulate their samples, if they wish.
	MxAt_OutPortToUpdateAccBufAvailabilityMask(me, unOutPortID);

	// Set the corresponding bit in outputReqPendingMask of all the active i/p ports feeding to this o/p port.
	MtMx_OutPortToUpdateOutputReqPendingMask(me, unOutPortID);

	//Check if output port is operating in native mode and needs to be re-initialized
	if ((pCurrentOutPort->unNativeModeFlags) && pCurrentOutPort->bIsOutPortInNativeModePendingReInit)
	{
		if (ADSP_FAILED(result = MtMx_ReInitNativeModeOutport(me, unOutPortID)))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MxAt_ProcessBufQ o/p port %lu MxAt_ReInitNativeModeOutport failed. Res = %d.", unOutPortID, result);
			return;
		}
	}

	// Check if EOS msg is pending on any of the i/p ports feeding to this o/p port. If yes, send the EOS msg downstream.
	MtMx_OutPortToCheckForPendingEOS(me, unOutPortID);

	// Now, Check and process if any non-live i/p ports needs to accumulate to this port's acc buffer.
	MxAt_OutPortToProcessIfNonLiveInputPortsNeedToAcc(me, unOutPortID);
}

static void MxAr_CommonOutPortProcessingRoutine(This_t *me, uint32_t unOutPortID)
{
	uint32_t                  steadyStateInPortsMask, unInPortID;
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType    *pCurrentOutPort = me->outPortParams[unOutPortID];

	steadyStateInPortsMask = me->steadyStateInPortsMask;
	while(steadyStateInPortsMask)
	{
		unInPortID = Q6_R_ct0_R(steadyStateInPortsMask);
		steadyStateInPortsMask ^= 1 << unInPortID;
		pCurrentInPort = me->inPortParams[unInPortID];

		// If this input port's local buffer is full AND its top priority output port has request pending
		if((pCurrentInPort->bIsLocalBufFull || pCurrentInPort->bIsAccumulationPending) && ((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask)))
		{
			//Normal operation
			MxAr_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);

			//Once the contents of an input port that was just transitioned to ACTIVE have been accumulated, move it to FALSE.
			pCurrentInPort->bInputPortJustMovedFromHeldToActive = FALSE;

			if (pCurrentInPort->bIsAccumulationPending)
			{
				pCurrentInPort->bIsAccumulationPending = FALSE;

				if (TRUE == pCurrentInPort->bShouldLocalBufBeReInit)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu triggering re-init of i/p port %lu local buf",
							me->mtMxID, unOutPortID, unInPortID);
					ADSPResult result = MtMx_ReInitLocalBuffer(me, unInPortID);

					//Check if output port is operating in native mode and needs to be re-initialized
					if (pCurrentOutPort->unNativeModeFlags && pCurrentOutPort->bIsOutPortInNativeModePendingReInit)
					{
						if (ADSP_FAILED(result = MtMx_ReInitNativeModeOutport(me, unOutPortID)))
						{
							MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port %lu MtMx_ReInitNativeModeOutport failed. Res = %d.", me->mtMxID, unOutPortID, result);
							return;
						}
					}

					pCurrentInPort->bShouldLocalBufBeReInit = FALSE;

					//Check if PSPD libraries needs to be re-init.
					MtMx_InPortToCheckReInitPspdLibraries(me, unInPortID);
				}
			}
		}
	}

	//Check if any pull ports are waiting to send data downstream
	MxAr_ProcessPendingPullOutPorts(me);
}

static void MxAr_OutPortToCheckForUnderflowEvents(This_t *me, uint32_t unOutPortID)
{
	ADSPResult              result;
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	uint32_t                unInPortID, inPortsMask = pCurrentOutPort->inPortsMask;
	while (inPortsMask)
	{
		unInPortID = Q6_R_ct0_R(inPortsMask);
		inPortsMask ^= 1 << unInPortID;
		pCurrentInPort = me->inPortParams[unInPortID];

		//Report the underflow event if the client has registered for it & provided a valid call back function.
		if ((TRUE == pCurrentInPort->bIsRegisteredForUnderflowEvents)&& (NULL != (pCurrentInPort->eliteCb.pCbHandle)))
		{
			if(
					((pCurrentInPort->outputReqPendingMask) & (1 << unOutPortID)) &&
					(FALSE == pCurrentInPort->bIsLastMsgEOS) &&
					(unOutPortID == pCurrentInPort->unTopPrioOutPort) &&
					(TRUE == pCurrentInPort->bHasFirstSampleAccumulated) &&
					(INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState) &&
					(FALSE == pCurrentInPort->bInputPortJustMovedFromHeldToActive)
			)
			{
				if(MT_MX_INPUT_PORT_UNDERFLOW_REPORT_STATUS_REPORT == pCurrentInPort->unUnderflowReportStatus)
				{
					pCurrentInPort->unUnderflowReportStatus = MT_MX_INPUT_PORT_UNDERFLOW_REPORT_STATUS_DO_NOT_REPORT;
					Elite_CbType *pEliteCb = &(pCurrentInPort->eliteCb);
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"MtMx #%lu: generating underflow event for input port %lu", me->mtMxID, unInPortID);
					result = pEliteCb->pCbFct(pEliteCb->pCbHandle,
							ASM_SESSION_EVENT_RX_UNDERFLOW,
							0, NULL, 0);
					if (ADSP_FAILED(result))
					{
						MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Error - MtMx #%lu: callback ftn for generating underflow event for i/p port %lu returned with status 0x%x",
								me->mtMxID, unInPortID, result);
					}
				}
			}
			else
			{
				pCurrentInPort->unUnderflowReportStatus = MT_MX_INPUT_PORT_UNDERFLOW_REPORT_STATUS_REPORT;
			}
		}

		//During an underflow, session clock will advance in the case that the i/p buffer has a valid TS.
		//In that case, the expected TS has to be adjusted based on the current session clock
		if(
				((pCurrentInPort->outputReqPendingMask) & (1 << unOutPortID)) &&
				(FALSE == pCurrentInPort->bIsLastMsgEOS) &&
				(unOutPortID == pCurrentInPort->unTopPrioOutPort) &&
				(TRUE == pCurrentInPort->bHasFirstSampleAccumulated) &&
				(INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState) &&
				(FALSE == pCurrentInPort->bInputPortJustMovedFromHeldToActive)
		)
		{
			//AV-Sync
			MtMx_UpdateExpectedST(me, unInPortID);
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MtMx #%lu: i/p port %lu o/p port %lu underflow occurred", me->mtMxID, unInPortID, unOutPortID);
		}
	}
}

void MtMx_OutPortToCheckForPendingEOS(This_t *me, uint32 unOutPortID)
{
	ADSPResult              result;
	uint32_t                  unInPortID;
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	uint32_t                inPortsMask = pCurrentOutPort->inPortsMask;
	while (inPortsMask)
	{
		unInPortID = Q6_R_ct0_R(inPortsMask);
		inPortsMask ^= 1 << unInPortID;
		pCurrentInPort = me->inPortParams[unInPortID];
		if (
				((pCurrentInPort->eosPendingMask) & (1 << unOutPortID)) &&
				(!pCurrentInPort->bIsAccumulationPending) &&
				(FALSE == pCurrentInPort->bInputPortJustMovedFromHeldToActive)
		)
		{
			MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO, "MtMx #%lu o/p port %lu sending EOS from i/p port %lu downstream, eos pending flag = %lu",
					me->mtMxID, unOutPortID, unInPortID, pCurrentInPort->eosPendingMask);

			elite_msg_any_t msg;

			if(FALSE == pCurrentInPort->bIsGaplessEOSMsgWaitingToBeSentDS)
			{
				if (ADSP_FAILED(result = MtMx_DuplicateEosMsg( me, unInPortID, unOutPortID, &(pCurrentInPort->myDataQMsg), &msg)))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port %lu: couldn't duplicate regular EOS from i/p port %lu",
							me->mtMxID, unOutPortID, unInPortID);
					goto __EOScleanup;
				}
			}
			else
			{
				if (ADSP_FAILED(result = MtMx_DuplicateEosMsg( me, unInPortID, unOutPortID, &(pCurrentInPort->gaplessEOSmsg), &msg)))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port %lu: couldn't duplicate gap-less EOS from i/p port %lu",
							me->mtMxID, unOutPortID, unInPortID);
					goto __EOScleanup;
				}
			}

			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu: Sending EOS from i/p port %lu DS. EOSFormat: %lu",
					me->mtMxID, unOutPortID, unInPortID, (((elite_msg_data_eos_apr_t*)(msg.pPayload))->unEosFormat));

			if (ADSP_FAILED(result = qurt_elite_queue_push_back(pCurrentOutPort->pDownstreamPeerHandle->dataQ, (uint64_t*)&(msg))))
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu o/p port %lu: couldn't send EOS from i/p port %lu downstream",
						me->mtMxID, unOutPortID, unInPortID);
				elite_msg_finish_msg(&msg, ADSP_EOK);
			}

			__EOScleanup:
			//Clear the pending EOS flag on this output port
			pCurrentInPort->eosPendingMask &=  (~(1 << unOutPortID));

			if(pCurrentInPort->bIsEOSHeld)
			{
				//Stop incrementing session clock for this input port
				pCurrentInPort->bIsSessionUnderEOS = TRUE;
			}

			if ( !( pCurrentInPort->eosPendingMask) )
			{
				if(pCurrentInPort->bIsEOSHeld)
				{
					//For regular EOS, release the EOS message from upstream if all EOS has been sent.
					elite_msg_finish_msg( &(pCurrentInPort->myDataQMsg), ADSP_EOK );
					pCurrentInPort->bIsEOSHeld = FALSE;
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu adding i/p port %lu to wait mask", me->mtMxID, unOutPortID, unInPortID);
					MtMx_AddInputPortToWaitMask(me,unInPortID);
				}
				else if(pCurrentInPort->bIsGaplessEOSMsgWaitingToBeSentDS)
				{
					//For Gap-less EOS, release the gapless EOS msg, mark this i/p port as not pending EOS if all EOS has been sent.
					elite_msg_finish_msg( &(pCurrentInPort->gaplessEOSmsg), ADSP_EOK );
					pCurrentInPort->bIsGaplessEOSMsgWaitingToBeSentDS = FALSE;
				}
			}
		}
	}
}


void MtMx_OutPortToUpdateOutputReqPendingMask(This_t *me, uint32_t unOutPortID)
{
	MatrixInPortInfoType    *pCurrentInPort;
	uint32_t unInPortID;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if (NULL != pCurrentInPort)
		{
			if (INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState)
			{
				pCurrentInPort->outputReqPendingMask |= pCurrentInPort->strMask & (1 << unOutPortID);
			}
		}
	}
}

void MxAt_OutPortToProcessIfNonLiveInputPortsNeedToAcc(This_t *me, uint32_t unOutPortID)
{
	uint32_t               steadyStateInPortsMask, unInPortID;
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	ADSPResult result = ADSP_EOK;

	steadyStateInPortsMask = me->steadyStateInPortsMask;
	while(steadyStateInPortsMask)
	{
		unInPortID = Q6_R_ct0_R(steadyStateInPortsMask);
		steadyStateInPortsMask ^= (1 << unInPortID);
		pCurrentInPort = me->inPortParams[unInPortID];

		// If this input port is Non-Live and is waiting to accumulate to this o/p port's acc buf, trigger accumulation
		if(((FALSE == pCurrentInPort->bIsPortLive) && ((pCurrentOutPort->inPortsWaitingToAccMask) & (1<<unInPortID))) ||
				((FALSE == pCurrentInPort->bIsPortLive) && (pCurrentInPort->bIsAccumulationPending)))
		{
			MxAt_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
			if (pCurrentInPort->bIsAccumulationPending)
			{
				pCurrentInPort->bIsAccumulationPending = FALSE;

				if (TRUE == pCurrentInPort->bShouldLocalBufBeReInit)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MxAt #%lu o/p port %lu triggering re-init of i/p port %lu local buf",
							me->mtMxID, unOutPortID, unInPortID);
					(void) MtMx_ReInitLocalBuffer(me, unInPortID);

					//Check if output port is operating in native mode and needs to be re-initialized
					if (pCurrentOutPort->unNativeModeFlags && pCurrentOutPort->bIsOutPortInNativeModePendingReInit)
					{
						if (ADSP_FAILED(result = MtMx_ReInitNativeModeOutport(me, unOutPortID)))
						{
							MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MxAt_ProcessBufQ o/p port %lu MxAt_ReInitNativeModeOutport failed. Res = %d.", unOutPortID, result);
							return;
						}
					}
					pCurrentInPort->bShouldLocalBufBeReInit = FALSE;

					//Check if the PSPD libraries needs to be re-init.
					MtMx_InPortToCheckReInitPspdLibraries(me, unInPortID);
				}
			}
		}
	} // while(steadyStateInPortsMask)
}

static bool_t MxAr_OutPortToCheckIfItIsOkToProceed(This_t *me, uint32_t unOutPortID)
{
	//This function checks for 3 things:
	//1) Is this o/p port connected to any i/p port. If not, return TRUE (OK to proceed).
	//2) Is this o/p port a top priority port to any i/p port it is connected to. If yes, return TRUE (OK to proceed).
	//3) If this o/p port is not a top priority port, then check if at least one connected i/p port is Active.
	//       If not, then all of its i/p ports may be paused/holding in which case return TRUE (OK to proceed).
	//       if yes, then at least one connected i/p port is active, in which case check if at least one accumulation has happened.
	//           If yes, then proceed and return TRUE
	//           if not, then wait for atleast one accumulation to happen prior to sending zeros. (return FALSE).

	bool_t bIsItOKToProceed = TRUE;
	bool_t bIsAnyConnectedPortActive = FALSE;
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	uint32_t                inPortsMask = pCurrentOutPort->inPortsMask;
	uint32_t                unInPortID;
	MatrixInPortInfoType   *pCurrentInPort;

	//Check if this o/p port has anything connected yet. If not, send zeros as normal and OK to proceed
	if(0 == inPortsMask)
	{
		return bIsItOKToProceed;
	}

	//If this port is pending a release buffer, it is OK to proceed (the current existing steady state buffers do not count)
	if (TRUE == pCurrentOutPort->bIsOutPortPendingReleaseBuffers && pCurrentOutPort->unNumOutputBufsPendingRelease > 0
			&& pCurrentOutPort->unNumOutputBufsPendingRelease != pCurrentOutPort->unNumOutputBufs)
	{
		return bIsItOKToProceed;
	}

	while (inPortsMask)
	{
		unInPortID = Q6_R_ct0_R(inPortsMask);
		inPortsMask ^= 1 << unInPortID;
		pCurrentInPort = me->inPortParams[unInPortID];
		if(unOutPortID == pCurrentInPort->unTopPrioOutPort)
		{
			return bIsItOKToProceed;
		}

		if (INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState)
		{
			bIsAnyConnectedPortActive = TRUE;
		}
	}

	//Only if all connected i/p ports are active will the waiting for resource logic apply on this o/p port
	if (FALSE == bIsAnyConnectedPortActive)
	{
		return bIsItOKToProceed;
	}

	//If code execution has reached this point, it means this o/p port is not a top priority o/p port to any connected i/p port
	//And at least one of its connected i/p ports are ACTIVE.
	//It is OK to proceed if atleast one accumulation has happened into this o/p port's acc. buffer
	bIsItOKToProceed = (0 == pCurrentOutPort->accInPortsMask) ? FALSE:TRUE;

	return bIsItOKToProceed;
}

ADSPResult MtMx_OpPortPopBuffer(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	ADSPResult                 result = ADSP_EOK;
	qurt_elite_bufmgr_node_t bufNode;

	if (0 < pCurrentOutPort->unNumOutputBufsPendingRelease)
	{
		//Pop the buffer and destroy it
		result = qurt_elite_queue_pop_front(pCurrentOutPort->bufQ, (uint64_t*)&bufNode);

		if (ADSP_EOK == result)
		{
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu Pop&Destroy Buf#%lu, 0x%p",
					me->mtMxID, unOutPortID, pCurrentOutPort->unNumOutputBufsPendingRelease, bufNode.pBuffer);

			MTMX_FREE  (bufNode.pBuffer);
			pCurrentOutPort->unNumOutputBufsPendingRelease --;
		}
	}

	return result;
}

ADSPResult MtMx_OpPortReCfg(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	ADSPResult                 result = ADSP_EOK;

	//Free up the Acc. Buf. using aligned free
	if (pCurrentOutPort->pAccBuf)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu o/p port %lu Freeing acc. buffer: 0x%p", me->mtMxID, unOutPortID,  pCurrentOutPort->pAccBuf);
		MTMX_ALIGNED_FREE(pCurrentOutPort->pAccBuf);
	}

	//Alloc new buffers
	if (ADSP_FAILED(result = MtMx_OutportAllocBuffers(me, unOutPortID, pCurrentOutPort)))
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %lu Error in alloc buffers! Result = %d",
				me->mtMxID, unOutPortID, result);
		return result;
	}

	//O/p port (if in native mode) has successfully re-inited.
	pCurrentOutPort->bIsOutPortInNativeModePendingReInit = FALSE;

	//Check and try to send the MT down
	if (ADSP_FAILED(result = MtMx_OpPortReCfg_CheckAndSendMT(me, unOutPortID, pCurrentOutPort)))
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %lu Error in MT check and send step! Result = %d",
				me->mtMxID, unOutPortID, result);
		return result;
	}

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu output port %lu new settings", me->mtMxID, unOutPortID);
	MtMx_PrintDebugMediaTypeInformationOutputPort(me, unOutPortID);

	//Setup masks etc after recfg
	if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		MxAr_OpPortReCfg_PostSteps(me, unOutPortID, pCurrentOutPort);
	}
	else
	{
		MxAt_OpPortReCfg_PostSteps(me, unOutPortID, pCurrentOutPort);
	}

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Reconfiguring output port %lu End. Current state %d.",
			me->mtMxID, unOutPortID, pCurrentOutPort->outPortState);

	return result;
}
