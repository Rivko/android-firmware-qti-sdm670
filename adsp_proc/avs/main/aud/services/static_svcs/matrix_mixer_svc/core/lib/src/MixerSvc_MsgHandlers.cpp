/**
@file MixerSvc_MsgHandlers.cpp
@brief This file defines functions that the audio matrix mixer
uses to handle various control commands it receives.
 */

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_MsgHandlers.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/2010 AAA     Created file.
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"
#include "adsp_media_fmt.h"
#include "adsp_asm_data_commands.h"
#include "adsp_adm_api.h"
#include "AudDevMgr_PrivateDefs.h"
#include "adsp_asm_session_commands.h"
#include "MixerSvc_MsgHandlers.h"
#include "MixerSvc_Util.h"
#include "EliteMsg_AdmCustom.h"
#include "MixerSvc_InPortHandler.h"
#include "AudDynaPPSvc.h"
#include "adsp_mtmx_strtr_api.h"
#include "MixerSvc_OutPortHandler.h"
#include "adsp_private_api_ext.h"
#include "avsync_lib.h"
#include "MixerSvc_ChanMixUtils.h"
#include "MixerSvc_Pspd.h"
#include "MixerSvc_AvSync.h"
#include "MixerSvc_Limiter.h"
#include "adsp_vparams_api.h"

static ADSPResult MtMx_ChkReinitInportLocalBuf(This_t *me, uint32_t unInPortID,  MatrixInPortInfoType *pCurrentInPort);
static ADSPResult MtMx_ValidateMultiChMediaFormatParam(This_t *me, uint32_t unInPortID, elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk, uint32_t unOpCode);
static ADSPResult MxAr_Pause(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseType *pPayload);
static ADSPResult MxAt_Pause(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseType *pPayload);
static ADSPResult MxAr_Flush(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxFlushType *pPayload);
static ADSPResult MxAt_Flush(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxFlushType *pPayload);
static ADSPResult MxAr_Suspend(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxSuspendType *pPayload);
static ADSPResult MxAt_Suspend(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxSuspendType *pPayload);
static ADSPResult MxAr_PortStateChange(This_t *me, EliteMsg_CustomMtMxPortStateChange *pPayload);
static ADSPResult MtMx_CheckPortDirMatrixIdCompatibility(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseFlushType *pPayload);
static ADSPResult MtMx_CheckPortIdValidity(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseFlushType *pPayload);
static ADSPResult MtMx_CheckPortDirectionValidity(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseFlushType *pPayload);
static ADSPResult MxArFlushInputQRetainSessionClock(This_t *me, uint32_t unInPortID);
static void MtMx_DetectIfInputPortsNeedToTransitionAwayFromReady(This_t *me);
static ADSPResult MxAt_InPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort);
static ADSPResult MxAr_OutPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
static ADSPResult MxAt_OutPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
static ADSPResult MxAt_InPortGetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort);
static ADSPResult MxAr_OutPortGetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
static ADSPResult MxAt_OutPortGetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);

ADSPResult MtMx_MsgMediaType(This_t *me, uint32_t unInPortID, elite_msg_any_t myDataQMsg)
{
	ADSPResult              result = ADSP_EOK;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	uint16_t                unNewBytesPerSample;

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MtMx #%lu i/p port %lu rcvd msg %lu (MediaType)", me->mtMxID, unInPortID, myDataQMsg.unOpCode);

	elite_msg_data_media_type_apr_t* pMediaTypePayload = (elite_msg_data_media_type_apr_t*)myDataQMsg.pPayload;

	if(ELITEMSG_DATA_MEDIA_TYPE_APR != pMediaTypePayload->unMediaTypeFormat)
	{
		MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu rcvd invalid media format [%d] in msg %lu (MediaType)",
				me->mtMxID, unInPortID, (int)(pMediaTypePayload->unMediaTypeFormat), myDataQMsg.unOpCode);

		return ADSP_EBADPARAM;
	}

	switch (pMediaTypePayload->unMediaFormatID)
	{
	case ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM:
	{
		elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk = (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaTypePayload);

		/* Validate the multi-channel media format block params */
		result = MtMx_ValidateMultiChMediaFormatParam(me, unInPortID, pMediaFormatBlk, ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM);

		/* If any of the parameters is not valid, return */
		if(ADSP_FAILED(result))
		{
			return result;
		}

		//If there are msgs to return on the RX matrix, check the incoming MT message to make sure this is the expected SR
		if (pCurrentInPort->unNumMsgsToReturn > 0 && ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			if (pCurrentInPort->unTargetSampleRate  != pMediaFormatBlk->sample_rate)
			{
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu rcvd invalid SR [%lu] in MediaType msg. Expected SR [%lu]",
						me->mtMxID, unInPortID, pMediaFormatBlk->sample_rate, pCurrentInPort->unTargetSampleRate);
				return ADSP_EBADPARAM;
			}
		}

		//Store some i/p port params
		pCurrentInPort->nInterleaveFlag = pMediaFormatBlk->is_interleaved;
		
		unNewBytesPerSample = (pMediaFormatBlk->bits_per_sample > 16) ? MT_MX_BYTES_PER_SAMPLE_FOUR : MT_MX_BYTES_PER_SAMPLE_TWO;

		MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu msg %lu (MediaType) contents: [num_ch, fs, interleave_flag, bitwidth] = [%d, %d, %d, %d]",
				me->mtMxID, unInPortID, myDataQMsg.unOpCode, (int)pMediaFormatBlk->num_channels, (int)pMediaFormatBlk->sample_rate, (int)pMediaFormatBlk->is_interleaved, (int)pMediaFormatBlk->bits_per_sample);

		//Allocate memory for local buffer it doesn't already exist
		if (NULL == pCurrentInPort->pStartLoc)
		{
			uint16_t i;

			for (i = 0; i < MAX_CHANNEL_MAPPING_NUMBER; i++)
			{
				pCurrentInPort->unChMapToChRevLUMask[i] = 0;
			}

			// Update media format parameters and calculate frame duration and buffer sizes
			MtMx_InPortUpdateMediaFmtParams(me, unInPortID, pMediaFormatBlk->sample_rate,
					pMediaFormatBlk->num_channels, unNewBytesPerSample, pMediaFormatBlk->bits_per_sample);

			for (i = 0; i < pCurrentInPort->unNumChannels; i++)
			{
				pCurrentInPort->unChannelMapping[i] = pMediaFormatBlk->channel_mapping[i];

				if((pCurrentInPort->unChannelMapping[i] > MAX_CHANNEL_MAPPING_NUMBER) ||
						(pCurrentInPort->unChannelMapping[i] < MT_MX_NUM_CHANNELS_ONE))
				{
					MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: Error, Invalid chan map! i/p port %lu ch# %d, chmap: %d",
							me->mtMxID, unInPortID, i+1, (int)pCurrentInPort->unChannelMapping[i]);
					return ADSP_EBADPARAM;
				}
				else
				{
					uint8_t unTempChMap = pCurrentInPort->unChannelMapping[i];
					pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1] |= (1 << i);
				}
			}

			MtMx_PrintDebugMediaTypeInformationInputPort(me, unInPortID);

			//Allocate input port's local buffer, 8 byte aligned
			pCurrentInPort->pStartLoc = (int8_t *)qurt_elite_memory_aligned_malloc(pCurrentInPort->unBytesPerSample * pCurrentInPort->unInPortBufSize,
					8, MTMX_SVC_INPORT_LOCAL_BUF);
			if (!pCurrentInPort->pStartLoc)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu failed to allocate memory for pStartLoc in msg %lu (MediaType)",
						me->mtMxID, unInPortID, myDataQMsg.unOpCode);
				return ADSP_ENOMEMORY;
			}
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu allocate memory success for pStartLoc, #bytes: %lu, ptr: 0x%p",
					me->mtMxID, unInPortID, pCurrentInPort->unBytesPerSample * pCurrentInPort->unInPortBufSize, pCurrentInPort->pStartLoc);

			if(pCurrentInPort->bIsPortLive)
			{
				{
					//Since no pre-roll, zero delay.
					if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID && NULL != pCurrentInPort->punMtMxInDelay)
					{
						*(pCurrentInPort->punMtMxInDelay) = 0;
						MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: FrameSize(us) %lu, TotDelay (micro-sec) %lu",
								me->mtMxID, unInPortID, (uint32_t)pCurrentInPort->unFrameDurationInUsec.int_part, *(pCurrentInPort->punMtMxInDelay));
					}
				}
			}

			MtMx_InitInPortLocalBufPtrs(me, unInPortID);
		}
		else if ( (pCurrentInPort->unNumChannels != pMediaFormatBlk->num_channels) ||
				(pCurrentInPort->unBytesPerSample != unNewBytesPerSample) ||
				(pCurrentInPort->unBitwidth != pMediaFormatBlk->bits_per_sample) ||
				(pCurrentInPort->unSampleRate != pMediaFormatBlk->sample_rate) )
		{
			// Store new params
			pCurrentInPort->unNewNumChannels = pMediaFormatBlk->num_channels;
			pCurrentInPort->unNewSampleRate = pMediaFormatBlk->sample_rate;
			for (uint32_t i = 0; i < pCurrentInPort->unNewNumChannels; i++)
			{
				pCurrentInPort->unNewChannelMapping[i] = pMediaFormatBlk->channel_mapping[i];
			}
			pCurrentInPort->unNewBytesPerSample = unNewBytesPerSample;
			pCurrentInPort->unNewBitwidth = pMediaFormatBlk->bits_per_sample;

			//Re-initialize local buffer if required
			if (ADSP_FAILED(result = MtMx_ChkReinitInportLocalBuf(me, unInPortID, pCurrentInPort)))
			{
				return result;
			}
		}
		else if (pCurrentInPort->unNumChannels == pMediaFormatBlk->num_channels)
		{
			//Store the number of channels into nNewNumChannels. this is done because
			//mt_mx_re_init_local_buffer loads the number of channels from nNewNumChannels
			pCurrentInPort->unNewNumChannels = pCurrentInPort->unNumChannels;
			pCurrentInPort->unNewBytesPerSample = unNewBytesPerSample;
            pCurrentInPort->unNewBitwidth = pMediaFormatBlk->bits_per_sample;
			pCurrentInPort->unNewSampleRate = pCurrentInPort->unSampleRate;
			for (uint16_t i = 0; i < pCurrentInPort->unNumChannels; i++)
			{
				pCurrentInPort->unNewChannelMapping[i] = pMediaFormatBlk->channel_mapping[i];
			}

			//Check if local buffer can be re-initialized
			if (ADSP_FAILED(result = MtMx_ChkReinitInportLocalBuf(me, unInPortID, pCurrentInPort)))
			{
				return result;
			}
		}

		//Check if PSPD libraries need to be re-init. 
		//(Do not init PSPD libraries if there is an accumulation pending from this i/p port)
		if(FALSE == pCurrentInPort->bIsAccumulationPending)
		{
			MtMx_InPortToCheckReInitPspdLibraries(me, unInPortID);
		}

		(void)MtMx_CheckSSLibStatus_3(me, unInPortID); //AV-Sync

		//If any connected TX/RX o/p ports are in native mode, check if they need to re-init. If so, mark them for re-init.
		{
			uint32_t                       strMask = pCurrentInPort->strMask;
			uint32_t                  unOutPortID;
			MatrixOutPortInfoType   *pCurrentOutPort;
			ADSPResult result = ADSP_EOK;

			MtMx_CheckIfAnyConnOutPortsInNativeModeNeedReInit(me, unInPortID);

			while (strMask)
			{
				unOutPortID = Q6_R_ct0_R(strMask);
				strMask ^= (1 << unOutPortID);
				pCurrentOutPort = me->outPortParams[unOutPortID];

				//If there is no accumulation pending on a native mode o/p port and it is pending re-init, then re-initialize it right away.
				if((0 == pCurrentOutPort->accInPortsMask) && (0 != pCurrentOutPort->unNativeModeFlags) &&
						(pCurrentOutPort->bIsOutPortInNativeModePendingReInit))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu  i/p port %lu calling Reinit on Native mode o/p port %lu because no acc. pending.",
							me->mtMxID, unInPortID, unOutPortID);
					if (ADSP_FAILED(result = MtMx_ReInitNativeModeOutport(me, unOutPortID)))
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "o/p port %lu MtMx_ReInitNativeModeOutport failed. Res = %d.", unOutPortID, result);
						return result;
					}
				}
			}
		}

		//At this point, we can assume that the MT was setup successfully. Now is the time to release any held msgs back to the PP
		if (pCurrentInPort->unNumMsgsToReturn > 0)
		{
			for (uint16_t i = 0; i < pCurrentInPort->unNumMsgsToReturn; i++)
			{
				// release the input buffer to its queue.
				elite_msg_any_t  myDataQMsg1 = pCurrentInPort->msgsToReturn[i];

				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd (MediaType) handler: i/p port %lu releasing i/p buffer #%d that was in dataQ",
						me->mtMxID, unInPortID, i+1);

				if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&myDataQMsg1)))
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd (MediaType) handler: i/p port %lu failed to return buffer",
							me->mtMxID, unInPortID);
					return result;
				}
			}
			pCurrentInPort->unNumMsgsToReturn = 0;
		}
		break;
	}
	default:
	{
		result = ADSP_EBADPARAM;
		MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu rcvd invalid format [%d] in msg %lu (MediaType)",
				me->mtMxID, unInPortID, (int)(pMediaTypePayload->unMediaFormatID), myDataQMsg.unOpCode);
		return result;
	}
	}

	//Update matrix KPPS and/or BW and raise ADSPPM event if needed.
	if(ADSP_EOK != (result = MtMx_SetReqKppsAndBW(me)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: MtMx_SetReqKppsAndBW failed with result = %d", me->mtMxID, result);
		return result;
	}

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving msg [%lu] (MediaType) handler status %d", me->mtMxID, myDataQMsg.unOpCode, (int)result);
	return result;
}

void MtMx_MsgEos(This_t *me, uint32_t unInPortID, elite_msg_any_t myDataQMsg)
{
	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];
	MatrixOutPortInfoType *pCurrentOutPort;
	ADSPResult result = ADSP_EOK;

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu rcvd msg %lu (MtMx_MsgEos)", me->mtMxID, unInPortID, myDataQMsg.unOpCode);

	elite_msg_data_eos_apr_t* pEosPayload = (elite_msg_data_eos_apr_t*)myDataQMsg.pPayload;
	pCurrentInPort->eosInfo = pEosPayload->eosInfo;

	switch (pEosPayload->unEosFormat)
	{
	case ELITEMSG_DATA_EOS_APR:
	case ELITEMSG_DATA_DTMF_EOT:
	case ELITEMSG_DATA_TX_EOS:
	case ELITEMSG_DATA_GAPLESS_TERMINATION_EOS:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu rcvd regular EOS: EOSFormat=%lu",
				me->mtMxID, unInPortID, pEosPayload->unEosFormat);

		//Set this stream's EOS pending mask so that EOS will be sent to all connected output ports.
		//Mark EOS held and stop processing dataQ till EOS is sent down
		pCurrentInPort->eosPendingMask = pCurrentInPort->strMask;
		pCurrentInPort->bIsLastMsgEOS = TRUE;
		pCurrentInPort->bIsEOSHeld = TRUE;

		if ( (ASM_DATA_EVENT_RENDERED_EOS_V2 == pCurrentInPort->eosInfo.event_opcode) &&
		      (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID) )
      {
         pCurrentInPort->eosPendingAckMask = pCurrentInPort->eosPendingMask;
      }

		if ((0 == pCurrentInPort->strMask) || (0 == ((1 << unInPortID) & (me->steadyStateInPortsMask))))
		{
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu does not have active o/p connected to it, Releasing this EOS message. i/p port will continue to be in wait mask, strMask: %lu, SSM: %lu",
					me->mtMxID, unInPortID, pCurrentInPort->strMask, me->steadyStateInPortsMask);

			if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
			{
			   MtMx_CheckRaiseRenderedEoSv2(me, NULL, unInPortID, MT_MX_INVALID_PORT_ID, TRUE);
			}

			elite_msg_finish_msg( &(pCurrentInPort->myDataQMsg), ADSP_EOK );
			pCurrentInPort->bIsEOSHeld = FALSE;
		}
		else
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu holding EOS, stop processing dataQ", me->mtMxID, unInPortID);
			MtMx_RemoveInputPortFromWaitMask(me,unInPortID);
		}

		//Attempt to accumulate any pending data
		if (MtMx_InportIsLocalBufFull(pCurrentInPort) || MtMx_InportIsLocalBufPartiallyFull(pCurrentInPort))
		{
			//Clean the stale data in the local buffer before accumulating to accBufs
			int8_t *pLocalPtr = pCurrentInPort->pWrLoc;

			uint32_t num_bytes_per_ch_ip_buf = pCurrentInPort->unInPortPerChBufSize * pCurrentInPort->unBytesPerSample;
			uint32_t unStaleDataInBytes = (pCurrentInPort->pStartLoc + num_bytes_per_ch_ip_buf) - pCurrentInPort->pWrLoc;
			uint32_t unNumStaleSamples = (unStaleDataInBytes)/(pCurrentInPort->unBytesPerSample);

			pCurrentInPort->ullStaleEosSamples = (uint64_t)unNumStaleSamples;
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Cleaning %lu stale samples (%lu bytes) in local buf",
					me->mtMxID, unInPortID, unNumStaleSamples, unStaleDataInBytes);

			for (uint32_t j = 0; j < pCurrentInPort->unNumChannels ; j++)
			{
				memset(pLocalPtr, 0, unStaleDataInBytes);
				pLocalPtr += num_bytes_per_ch_ip_buf;
			}

			if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
			{
				if ((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask))
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: triggering acc of local buffer after EOS was rcvd", me->mtMxID, unInPortID);
					MxAr_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
					MxAr_ProcessPendingPullOutPorts(me);
				}
				else
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: cannot trigger acc of local buffer after EOS was rcvd, marking acc pending",
							me->mtMxID, unInPortID);
					pCurrentInPort->bIsAccumulationPending = TRUE;
				}
			}
			else
			{
				if((((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask)) && (FALSE == pCurrentInPort->bIsPortLive))
						||(TRUE == pCurrentInPort->bIsPortLive))
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: triggering acc of local buffer after EOS was rcvd", me->mtMxID, unInPortID);
					MxAt_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
				}
				else
				{
					MatrixOutPortInfoType *pTopPrioOutPort;
					pTopPrioOutPort = me->outPortParams[pCurrentInPort->unTopPrioOutPort];
					//if there is data in accumulation buffer (because of secondary acc buffer), send it down and trigger accumulation
					if (((1 << pCurrentInPort->unTopPrioOutPort) & (pTopPrioOutPort->accInPortsMask == pTopPrioOutPort->inPortsMask)) && (FALSE == pCurrentInPort->bIsPortLive))
					{
						MxAt_ProcessBufQ(me, pCurrentInPort->unTopPrioOutPort);
						if(((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask)) && (FALSE == pCurrentInPort->bIsPortLive))
						{
							MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: triggering acc of local buffer after EOS was rcvd", me->mtMxID, unInPortID);
							MxAt_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
						}
					}
					else if(FALSE == pCurrentInPort->bIsPortLive)
					{
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: cant trig acc local buf after EOS was rcvd, mark acc pending", me->mtMxID, unInPortID);
						pCurrentInPort->bIsAccumulationPending = TRUE;
					}
				}
			}
		}
		else
		{
			//Try sending the EOS, as there is no data pending to be accumulated
			uint32_t strMask = pCurrentInPort->strMask;
			while (strMask)
			{
				uint32_t unOutPortID = Q6_R_ct0_R(strMask);
				strMask ^= (1 << unOutPortID);
				pCurrentOutPort = me->outPortParams[unOutPortID];
				if(OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu trying to send EOS to connected active o/p port %lu",
							me->mtMxID, unInPortID, unOutPortID);
					MtMx_OutPortToCheckForPendingEOS(me, unOutPortID);
				}
			}
		}

		if(ELITEMSG_DATA_GAPLESS_TERMINATION_EOS == pEosPayload->unEosFormat)
		{
			//Reset session clock on receipt of next buffer
			pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
			(void)MtMx_SetupDefaultRenderWindow(me, unInPortID); //AV-Sync
			pCurrentInPort->bShouldSessionTimeBeResync = TRUE;
			(void)MtMx_ResetStrClkSync(me, unInPortID); //AV-Sync
			pCurrentInPort->bForceCheckTimeStampValidity = TRUE; //Force to check TS validity for 2nd stream
		}

		if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			pCurrentInPort->unDataLogId++;
		}

		break;
	}
	case ELITEMSG_DATA_RESET_SESSION_CLK:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu rcvd gap-less EOS: EOSFormat=%lu",
				me->mtMxID, unInPortID, pEosPayload->unEosFormat);

		//Duplicate the EOS message locally
		if (ADSP_FAILED(result = MtMx_DuplicateEosMsg( me, unInPortID, 0, &(pCurrentInPort->myDataQMsg), &(pCurrentInPort->gaplessEOSmsg))))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu Couldn't duplicate gap-less EOS msg i/p port %lu", me->mtMxID, unInPortID);
			pCurrentInPort->eosPendingMask = 0;
			pCurrentInPort->eosPendingAckMask = 0;
			pCurrentInPort->bIsGaplessEOSMsgWaitingToBeSentDS = FALSE;
		}
		else
		{
		   //MtMx_DuplicateEosMsg assigns pResponseQ as the cmdQ of mtmx. but gaplessEOSmsg doesnt need to be acked back to matrix. hence make responseQ null.
		   ((elite_msg_data_eos_header_t*)pCurrentInPort->gaplessEOSmsg.pPayload)->pResponseQ = NULL;

			//Set this stream's EOS pending mask so that EOS will be sent to all connected output ports
			pCurrentInPort->eosPendingMask = pCurrentInPort->strMask;
			//Mark this i/p port as Gap-less EOS pending
			pCurrentInPort->bIsGaplessEOSMsgWaitingToBeSentDS = TRUE;

         if ( (ASM_DATA_EVENT_RENDERED_EOS_V2 == pCurrentInPort->eosInfo.event_opcode) &&
               (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID) )
         {
            pCurrentInPort->eosPendingAckMask = pCurrentInPort->eosPendingMask;
         }
		}

		//When subsequent stream's first buffer is rcvd, derive session clock off of its TS. If the stream doesn't have a TS,
		//session clk will begin from 0. The session clk is not reset to 0 right now, it should be reset to 0 when
		//the subsequent stream's first buffer arrives, so this part is done at the start of MtMx_InPortToHonorTimestamp function.
		pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
		(void)MtMx_SetupDefaultRenderWindow(me, unInPortID); //AV-Sync
		pCurrentInPort->bShouldSessionTimeBeResync = TRUE;
		(void)MtMx_ResetStrClkSync(me, unInPortID); //AV-Sync
		pCurrentInPort->bForceCheckTimeStampValidity = TRUE; //Force to check TS validity for 2nd stream

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu returning gap-less EOS buf. to upstream svc.", me->mtMxID, unInPortID);
		elite_msg_finish_msg(&myDataQMsg, result);

		//For Gap-less EOS, EOS is not held
		pCurrentInPort->bIsEOSHeld = FALSE;

		if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			pCurrentInPort->unDataLogId++;
		}
		break;
	}
	default:
	{
		result = ADSP_EBADPARAM;
		MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR: MtMx #%lu i/p port %lu rcvd invalid unEosFormat [%d] in msg %lu, returning buffer",
				me->mtMxID, unInPortID, (int)(pEosPayload->unEosFormat), myDataQMsg.unOpCode);
		elite_msg_finish_msg(&myDataQMsg, result);
		break;
	}
	}
}

ADSPResult MtMx_Pause(This_t *me, elite_msg_any_t* pMsg)
{
	ADSPResult                       result = ADSP_EOK;
	EliteMsg_CustomMtMxPauseType     *pPayload = (EliteMsg_CustomMtMxPauseType*)pMsg->pPayload;

	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (Pause) [port ID, port dir] = [%d, %d]",
			me->mtMxID, pPayload->unSecOpCode, (int)(pPayload->unPortID), (int)(pPayload->unPortDirection));

	if (ADSP_FAILED(result = MtMx_CheckPortDirectionValidity(me, pMsg, pPayload)) ||
			ADSP_FAILED(result = MtMx_CheckPortDirMatrixIdCompatibility(me, pMsg, pPayload)) ||
			ADSP_FAILED(result = MtMx_CheckPortIdValidity(me, pMsg, pPayload)))
	{
		goto __bailoutCmdPause;
	}

	if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		result = MxAr_Pause(me,pMsg, pPayload);
	}
	else
	{
		result = MxAt_Pause(me,pMsg,pPayload);
	}

	__bailoutCmdPause:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (Pause) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

ADSPResult MtMx_Suspend(This_t *me, elite_msg_any_t* pMsg)
{
	ADSPResult                       result = ADSP_EOK;
	EliteMsg_CustomMtMxSuspendType     *pPayload = (EliteMsg_CustomMtMxSuspendType*)pMsg->pPayload;

	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (Suspend) [port ID, port dir] = [%d, %d]",
			me->mtMxID, pPayload->unSecOpCode, (int)(pPayload->unPortID), (int)(pPayload->unPortDirection));

	if (ADSP_FAILED(result = MtMx_CheckPortDirectionValidity(me, pMsg, pPayload)) ||
			ADSP_FAILED(result = MtMx_CheckPortDirMatrixIdCompatibility(me, pMsg, pPayload)) ||
			ADSP_FAILED(result = MtMx_CheckPortIdValidity(me, pMsg, pPayload)))
	{
		goto __bailoutCmdSuspend;
	}

	if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		result = MxAr_Suspend(me,pMsg, pPayload);
	}
	else
	{
		result = MxAt_Suspend(me,pMsg,pPayload);
	}

	__bailoutCmdSuspend:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (Suspend) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}


void MtMx_ProcessMarkBuffer(This_t *me, uint32_t unInPortID, elite_msg_any_t myDataQMsg, uint16_t status)
{
	ADSPResult result = ADSP_EOK;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];

	elite_msg_data_mark_buffer_t *pMarkBufferPayload = (elite_msg_data_mark_buffer_t *)myDataQMsg.pPayload;
	asm_data_event_mark_buffer_v2_t eventPayload;
	eventPayload.token_lsw = pMarkBufferPayload->token_lsw;
	eventPayload.token_msw = pMarkBufferPayload->token_msw;
	eventPayload.result = status;
	Elite_CbType *pEliteCb = &(pCurrentInPort->eliteCb);
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MtMx #%lu: generating mark buffer event for input port %lu", me->mtMxID, unInPortID);
	result = pEliteCb->pCbFct(pEliteCb->pCbHandle,
			ASM_DATA_EVENT_MARK_BUFFER_V2,
			0, &eventPayload, sizeof(eventPayload));
	elite_msg_release_msg(&myDataQMsg);
	if(result != ADSP_EOK)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"MtMx #%lu: Mark Buffer Event failed for input port %lu", me->mtMxID, unInPortID);

	}
}



ADSPResult MtMx_Run(This_t *me, elite_msg_any_t* pMsg)
{
	ADSPResult                    result = ADSP_EOK;
	EliteMsg_CustomMtMxRunType    *pPayload = (EliteMsg_CustomMtMxRunType*)pMsg->pPayload;

	MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (Run) [prt ID, prt dr, strt flg, msw st tme, lsw st tme] = [%d, %d, %lu, %lu, %lu]",
			me->mtMxID, pPayload->unSecOpCode, (int)(pPayload->unPortID), (int)(pPayload->unPortDirection),
			pPayload->unStartFlag, (uint32_t)(pPayload->ullStartTime >> 32), (uint32_t)(pPayload->ullStartTime));

	if (ADSP_FAILED(result = MtMx_CheckPortDirectionValidity(me, pMsg, (EliteMsg_CustomMtMxPauseFlushType *)pPayload)) ||
			ADSP_FAILED(result = MtMx_CheckPortIdValidity(me, pMsg, (EliteMsg_CustomMtMxPauseFlushType *)pPayload)))
	{
		goto __bailoutCmdRun;
	}

	if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		result = MxAr_Run(me,pMsg,pPayload);
	}
	else
	{
		result = MxAt_Run(me,pMsg,pPayload);
	}

	__bailoutCmdRun:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (Run) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

ADSPResult MtMx_Flush(This_t *me, elite_msg_any_t* pMsg)
{
	ADSPResult                    result = ADSP_EOK;
	elite_msg_any_t               flush_msg;
	uint32_t                      unPayloadSize;
	MatrixInPortInfoType          *pCurrentInPort = NULL;
	MatrixOutPortInfoType         *pCurrentOutPort = NULL;
	mt_mx_struct_pspd_t           *pCurrentPspd = NULL;
	EliteMsg_CustomMtMxFlushType  *pPayload = (EliteMsg_CustomMtMxFlushType*)pMsg->pPayload;

	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (Flush) [port ID, port dir] = [%d, %d]",
			me->mtMxID, pPayload->unSecOpCode, (int)(pPayload->unPortID), (int)(pPayload->unPortDirection));

	if (ADSP_FAILED(result = MtMx_CheckPortDirectionValidity(me, pMsg, pPayload)) ||
			ADSP_FAILED(result = MtMx_CheckPortDirMatrixIdCompatibility(me, pMsg, pPayload)) ||
			ADSP_FAILED(result = MtMx_CheckPortIdValidity(me, pMsg, pPayload)))
	{
		goto __bailoutCmdFlush;
	}

	if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		result = MxAr_Flush(me,pMsg,pPayload);

		// Send flush command to all connected PSPDs.
		pCurrentInPort = me->inPortParams[pPayload->unPortID];
		unPayloadSize = sizeof(elite_msg_cmd_flush_t);
		for (uint32_t unOutPortID = 0; unOutPortID <= me->maxOutPortID; unOutPortID++)
		{
			//Identify the output ports that are connected to the flushed input port
			if (pCurrentInPort->strMask & (1 << unOutPortID))
			{
				//create flush command
				pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
				if(ADSP_EOK != (result = elite_msg_create_msg( &flush_msg, &unPayloadSize, ELITE_CMD_FLUSH, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q, 0, NULL)))
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create elite msg");
					goto __bailoutCmdFlush;;
				}

				//send flush command and wait for ack
				if (ADSP_EOK != (result = MtMx_PspdSendAndWaitForResp(&flush_msg, pCurrentPspd->thread_param.cmdQ, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q)))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu, Failed to flush PSPD service,[i/p port, o/p port] = [%d, %lu]",
						me->mtMxID,(int)pPayload->unPortID,unOutPortID);
					goto __bailoutCmdFlush;;
				}

				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, PSPD [i/p port, o/p port] = [%d, %lu] flush command is successful",
					me->mtMxID,(int)pPayload->unPortID,unOutPortID);
			}
		}
	}
	else
	{
		result = MxAt_Flush(me,pMsg,pPayload);

		// Send flush command to all connected PSPDs.
		uint32_t unOutPortID = pPayload->unPortID;
		pCurrentOutPort = me->outPortParams[unOutPortID];
		unPayloadSize = sizeof(elite_msg_cmd_flush_t);
		for (uint32_t unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
		{
			//Identify the input ports that are connected to the flushed output port
			if (pCurrentOutPort->inPortsMask & (1 << unInPortID))
			{
				//create flush command
				pCurrentInPort = me->inPortParams[unInPortID];
				pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
				if(ADSP_EOK != (result = elite_msg_create_msg( &flush_msg, &unPayloadSize, ELITE_CMD_FLUSH, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q, 0, NULL)))
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create elite msg");
					goto __bailoutCmdFlush;;
				}

				//send flush command and wait for ack
				if (ADSP_EOK != (result = MtMx_PspdSendAndWaitForResp(&flush_msg, pCurrentPspd->thread_param.cmdQ, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q)))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu, Failed to flush PSPD service,[i/p port, o/p port] = [%d, %lu]",
						me->mtMxID,(int)unInPortID,unOutPortID);
					goto __bailoutCmdFlush;;
				}

				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, PSPD [i/p port, o/p port] = [%d, %lu] flush command is successful",
					me->mtMxID,(int)unInPortID,unOutPortID);
			}
		}

	}



	__bailoutCmdFlush:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (Flush) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}


ADSPResult MtMx_PortStateChange(This_t *me, elite_msg_any_t* pMsg)
{
	ADSPResult                    result = ADSP_EOK;
	EliteMsg_CustomMtMxPortStateChange  *pPayload = (EliteMsg_CustomMtMxPortStateChange*)pMsg->pPayload;
	EliteMsg_CustomMtMxPauseFlushType  *pPayload1 = (EliteMsg_CustomMtMxPauseFlushType*)pMsg->pPayload;

	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (PortStateChange) [port ID, port dir] = [%d, %d]",
			me->mtMxID, pPayload->unSecOpCode, (int)(pPayload->unPortID), (int)(pPayload->unPortDirection));

	if (ADSP_FAILED(result = MtMx_CheckPortDirectionValidity(me, pMsg, pPayload1)) ||
			ADSP_FAILED(result = MtMx_CheckPortDirMatrixIdCompatibility(me, pMsg, pPayload1)) ||
			ADSP_FAILED(result = MtMx_CheckPortIdValidity(me, pMsg, pPayload1)))
	{
		goto __bailoutCmdPortStateChange;
	}

	if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		result = MxAr_PortStateChange(me, pPayload);
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: cmd [%lu] (PortStateChange) not supported in TX", me->mtMxID, pPayload->unSecOpCode);
		result = ADSP_EUNSUPPORTED;
	}

	__bailoutCmdPortStateChange:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (PortStateChange) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

ADSPResult MtMx_CfgInputPorts(This_t *me, elite_msg_any_t* pMsg)
{
	int32_t                 len, i, j, k, t, index;
	ADSPResult              result = ADSP_EOK;
	uint32_t                unInPortID, count;
	uint32_t                maxInputPorts;
	MatrixInPortInfoType    *pCurrentInPort;

	EliteMsg_CustomCfgInPortsType* pPayload = (EliteMsg_CustomCfgInPortsType*)pMsg->pPayload;
	CfgInputPortsPayload_t payload = pPayload->cfgInPortsPayload;

	len = payload.numInPorts;

	MtMxInPortHandle_t** tempPtr = payload.pAck;

	for (i = 0; i < len; i++)
	{
		index = payload.pInPortCfgParams[i].index;

		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (CfgIpPort) [port ID, port cfg] = [%d, %d]",
				me->mtMxID, pPayload->unSecOpCode, (int)(payload.pInPortCfgParams[i].index), (int)(payload.pInPortCfgParams[i].configuration));

		switch(payload.pInPortCfgParams[i].configuration)
		{
		case NEW_INPUT:
		{
			if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
			{
				maxInputPorts = MT_MXAR_MAX_INPUT_PORTS;
			}
			else
			{
				maxInputPorts = MT_MXAT_MAX_INPUT_PORTS;
			}
			unInPortID = index;
			if (index < 0)
			{
				unInPortID = 0;
				/* permissible range: 0 to (MT_MX_MAX_INPUT_PORTS - 1) */
				while (unInPortID < maxInputPorts &&
						(NULL != me->inPortParams[unInPortID]))
				{
					unInPortID++;
				}
			}

			/*If port ID is out of range */
			if ((unInPortID >= maxInputPorts) ||
					(NULL != me->inPortParams[unInPortID]))
			{
				result = ADSP_EFAILED;
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Either invalid index sent or no more input ports available");
				goto __bailoutCmdCfgInputPorts;
			}
			else
			{
				//allocate input port structure.
				if ( (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID) &&  (0 == unInPortID))
				{
					me->inPortParams[unInPortID] = (MatrixInPortInfoType*)qurt_elite_memory_malloc(sizeof(MatrixInPortInfoType), MTMX_SVC_PORT_STRUCTURE);
				}
				else
				{
					me->inPortParams[unInPortID] = (MatrixInPortInfoType*)qurt_elite_memory_malloc(sizeof(MatrixInPortInfoType), QURT_ELITE_HEAP_DEFAULT);
				}

				if (NULL == me->inPortParams[unInPortID])
				{
					result = ADSP_ENOMEMORY;
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu failed to alloc mem for input port state strucutre.", me->mtMxID);
					goto __bailoutCmdCfgInputPorts;
				}
				memset((void*)me->inPortParams[unInPortID], 0, sizeof(MatrixInPortInfoType));
				pCurrentInPort = me->inPortParams[unInPortID];

				//Initialize AV-Sync
				result = MtMx_InitInputAvSync(me, unInPortID);
				if(ADSP_ENOMEMORY == result)
				{
					goto __bailoutCmdCfgInputPorts;
				}

				//initialize PSPD channels and queues
				ADSPResult res = MtMx_PspdInitChanQ(pCurrentInPort);
				if (ADSP_EOK != res)
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize PSPD channel and queues with error = %d", res);
					goto __bailoutCmdCfgInputPorts;
				}

				pCurrentInPort->inPortHandle.portHandle = me->serviceHandle;
				pCurrentInPort->bIsPortLive = payload.pInPortCfgParams[i].bIsInPortLive;
				pCurrentInPort->bIsInPortBurstMode = payload.pInPortCfgParams[i].bIsInPortBurstMode;
				pCurrentInPort->bIsAdmLoopback = payload.pInPortCfgParams[i].bIsAdmLoopback;

				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu creating new i/p port %lu Live? %d Burstmode? %d MtMx Burstmode? %d",
						me->mtMxID, unInPortID, (int)(pCurrentInPort->bIsPortLive), (int)(pCurrentInPort->bIsInPortBurstMode), (int)(me->bIsMxAtOperatingInBurstMode));

				if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
				{
					pCurrentInPort->punMtMxInDelay = NULL;
				}
				else
				{
					if (NULL != payload.pInPortCfgParams[i].punMtMxDelay)
					{
						pCurrentInPort->punMtMxInDelay = payload.pInPortCfgParams[i].punMtMxDelay;
						*(pCurrentInPort->punMtMxInDelay) = 0;
					}
					else
					{
						result = ADSP_EBADPARAM;
						MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu recd NULL punMtMxDelay", me->mtMxID);
						goto __bailoutCmdCfgInputPorts;
					}
				}

				Elite_CbType *pEliteCb = &(pCurrentInPort->eliteCb);
				pEliteCb->pCbHandle = NULL;

				if (NULL != (payload.pInPortCfgParams[i].pEliteCb))
				{
					Elite_CbType *pIncomingEliteCb = payload.pInPortCfgParams[i].pEliteCb;
					pEliteCb->pCbFct = pIncomingEliteCb->pCbFct;
					pEliteCb->ulHandleSize = pIncomingEliteCb->ulHandleSize;
					pEliteCb->pCbHandle = qurt_elite_memory_malloc(pEliteCb->ulHandleSize, QURT_ELITE_HEAP_DEFAULT);
					if (!pEliteCb->pCbHandle)
					{
						result = ADSP_ENOMEMORY;
						MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu failed to alloc mem for ASM callback handle", me->mtMxID);
						goto __bailoutCmdCfgInputPorts;
					}
					memscpy(pEliteCb->pCbHandle, pEliteCb->ulHandleSize, pIncomingEliteCb->pCbHandle, pEliteCb->ulHandleSize);
				}

				pCurrentInPort->unUnderflowReportStatus = MT_MX_INPUT_PORT_UNDERFLOW_REPORT_STATUS_DO_NOT_REPORT;
				pCurrentInPort->bIsLocalBufEmpty = TRUE;
				pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
				pCurrentInPort->bIsTimeStampValid = TRUE;
				pCurrentInPort->bForceCheckTimeStampValidity = TRUE;
				pCurrentInPort->unPortPerfMode = payload.pInPortCfgParams[i].perfMode;
				(void)MtMx_SetDefaultRenderWindows(me, unInPortID);
				MtMx_SetInPortFrameDuration(me, unInPortID);

				pCurrentInPort->pStartLoc = NULL;
				pCurrentInPort->unDataLogId = payload.pInPortCfgParams[i].unDataLogId;

				if(NULL != payload.pInPortCfgParams[i].ppAfeDriftPtr)
				{
					(void)MtMx_SetInputDriftPtr(me, unInPortID, *(payload.pInPortCfgParams[i].ppAfeDriftPtr));
				}

				mt_mx_struct_ramp_gain_t *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[0]);

				for (k = 0; k < MT_MX_MAX_OUTPUT_PORTS; k++)
				{
					pCurrentInputOutputPath->unStepSizeInSamples = 48; //MAP:TBD: use some macro

					uint16_t *pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
					uint16_t *pCachedGain = &(pCurrentInputOutputPath->unCachedTargetGainAfterUnMute[0]);
					uint16_t *pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);

					for (j = 0; j < MT_MX_NUM_CHANNELS_EIGHT; j++)
					{
						*pTargetGain++ = 0x2000;
						*pCurrentGain++ = 0x2000;
						*pCachedGain++ = 0x2000;
					}
					pCurrentInputOutputPath++;
				}

				for (k = 0; k < MAX_CHANNEL_MAPPING_NUMBER; k++)
				{
					pCurrentInPort->unChMapToChRevLUMask[k] = 0;
				}

				// Check and update the MAX input port ID
				if (me->maxInPortID < unInPortID) me->maxInPortID = unInPortID;

				//Name the input port's data Q amd create it.
				count = qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;
				snprintf(pCurrentInPort->dataQName, QURT_ELITE_DEFAULT_NAME_LEN, "MtMxDataQ0%lx", count);
				if (ADSP_FAILED(result = qurt_elite_queue_create(pCurrentInPort->dataQName, MAX_DATA_Q_ELEMENTS,
						&(pCurrentInPort->inPortHandle.portHandle.dataQ))))
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu failed to create i/p port dataQ", me->mtMxID);

					// a. check if mem was alloc'd for ASM callback handle. If yes, free it.
					if (pEliteCb->pCbHandle)
					{
						MTMX_FREE (pEliteCb->pCbHandle);
					}
					goto __bailoutCmdCfgInputPorts;
				}

				//Add the queue to the channel
				if (ADSP_FAILED(result = qurt_elite_channel_addq(&me->channel, pCurrentInPort->inPortHandle.portHandle.dataQ, 0)))
				{
					result = ADSP_EFAILED;
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu failed to add dataQ to channel", me->mtMxID);

					// a. Destroy dataQ
					elite_svc_destroy_data_queue(pCurrentInPort->inPortHandle.portHandle.dataQ);

					// b. check if mem was alloc'd for ASM handle. If yes, free it.
					if (pEliteCb->pCbHandle)
					{
						MTMX_FREE (pEliteCb->pCbHandle);
					}
					goto __bailoutCmdCfgInputPorts;
				}

				// Map port ID and bit position in channel
				k = qurt_elite_queue_get_channel_bit(pCurrentInPort->inPortHandle.portHandle.dataQ);
				t = 31 - (Q6_R_cl0_R( (k) ));

				me->inPortIDMap[t] = unInPortID;
				me->unDataBitfield = me->unDataBitfield | k;

				// Send an ack with a. port ID and b. port handle to calling process.
				pCurrentInPort->inPortHandle.inPortID = unInPortID;
				pCurrentInPort->inPortState = INPUT_PORT_STATE_INIT;
				pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_INIT;
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu new i/p port id %lu open success. Log ID: %lu",
						me->mtMxID,unInPortID, pCurrentInPort->unDataLogId);
			}

			*tempPtr++ = &(me->inPortParams[unInPortID]->inPortHandle);
			break;
		}
		case CLOSE_INPUT:
		{
			if ((0 > index) || ((int)(me->maxInPortID) < index) ||  (INPUT_PORT_STATE_INACTIVE == me->inPortParams[index]->inPortState))
			{
				result = ADSP_EBADPARAM;
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu rcvd invalid i/p port id [%d] to close", me->mtMxID, (int)index);
				goto __bailoutCmdCfgInputPorts;
			}
			else
			{
				unInPortID = index;
				pCurrentInPort = me->inPortParams[unInPortID];

				//Update the state of this i/p port as being INACTIVE
				pCurrentInPort->inPortState = INPUT_PORT_STATE_INACTIVE;
				pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_INACTIVE;

				elite_msg_any_t myDataQMsg = pCurrentInPort->myDataQMsg;

				//If a buffer is being held, release it
				if (INPUT_BUFFER_HELD == pCurrentInPort->bInBufStatus)
				{
					MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMxSvc i/p port %lu releasing an input buffer that it was holding on to", unInPortID);
					if (ADSP_FAILED(elite_msg_return_payload_buffer(&myDataQMsg)))
					{
						MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to return buffer while closing i/p port %lu", unInPortID);
					}
					pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
				}

				//after close input, eos ack will be dropped. rendered eos will be dropped.
				//any held EoS will also be dropped above.
				pCurrentInPort->eosPendingAckMask = 0;

				MtMx_ClearEoSMsg(me, unInPortID, FALSE);

				//Dequeue the dataQ to check if any buffers are present. If yes, release those to the upstream svc's bufQ.
				while (ADSP_EOK == (qurt_elite_queue_pop_front(pCurrentInPort->inPortHandle.portHandle.dataQ, (uint64_t*)&myDataQMsg )))
				{
					if(myDataQMsg.unOpCode == ELITE_DATA_MARK_BUFFER)
					{
						MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx flushing Marked Buffer");
						MtMx_ProcessMarkBuffer(me, unInPortID, pCurrentInPort->myDataQMsg, ASM_DATA_EVENT_MARK_BUFFER_DISCARDED);
						continue;
					}
					else
					{
						MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMxSvc i/p port %lu dequeued an input buffer from dataQ. Releasing it now.", unInPortID);
						if (ADSP_FAILED(elite_msg_return_payload_buffer(&myDataQMsg)))
						{
							MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to return buffer while closing i/p port %lu", unInPortID);
						}
					}
				}

				//Free up memory that was allocated to this port's local buffer
				if (pCurrentInPort->pStartLoc)
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Freeing pStartLoc 0x%p", me->mtMxID, pCurrentInPort->pStartLoc);
					MTMX_ALIGNED_FREE(pCurrentInPort->pStartLoc);
				}

				//Reset the i/p port delay
				if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID && NULL != pCurrentInPort->punMtMxInDelay)
				{
					*(pCurrentInPort->punMtMxInDelay) = 0;
				}

				//Remove this port's bit from the overall wait_mask and the data bitfield
				MtMx_RemoveInputPortFromWaitMask(me,unInPortID);
				me->unDataBitfield = me->unDataBitfield & (~qurt_elite_queue_get_channel_bit(pCurrentInPort->inPortHandle.portHandle.dataQ));

				//Reset the inPortIDMap for this input port to -1
				{
					uint32_t unInPortIDIndex,unInPortIDChannelBitNo;
					unInPortIDChannelBitNo = qurt_elite_queue_get_channel_bit(pCurrentInPort->inPortHandle.portHandle.dataQ);

					unInPortIDIndex = 31 - (Q6_R_cl0_R( (unInPortIDChannelBitNo)) );
					me->inPortIDMap[unInPortIDIndex] = -1;
				}

				//Destroy the data Q. This would also remove it from the channel.
				qurt_elite_queue_destroy(pCurrentInPort->inPortHandle.portHandle.dataQ);
				pCurrentInPort->inPortHandle.portHandle.dataQ = NULL;

				if (pCurrentInPort->eliteCb.pCbHandle)
				{
					pCurrentInPort->eliteCb.pCbFct = NULL;
					MTMX_FREE (pCurrentInPort->eliteCb.pCbHandle);
				}

				if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
				{
					//Check if this i/p port was a top priority i/p port for an o/p port. If so, re-assign top priority i/p port.
					MxAt_ClosingInPortToUpdateInPortsTopPriorityMaskOfOutPort(me, unInPortID);

					//If any connected TX o/p ports are in native mode, check if they need to re-init. If so, mark them for re-init.
					MtMx_CheckIfAnyConnOutPortsInNativeModeNeedReInit(me, unInPortID);

					//Update some masks of connected o/p ports
					MtMx_ClosingInPortToUpdateMtMxMasksOfOutPort(me, unInPortID);

					//Update burst mode status of the TX matrix
					MxAt_UpdateBurstModeOfMxAt(me);

					MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, i/p port %lu closing. Live? %d Burstmode? %d MXAT Burstmode? %d",
							me->mtMxID, unInPortID, (int)(pCurrentInPort->bIsPortLive), (int)(pCurrentInPort->bIsInPortBurstMode), (int)(me->bIsMxAtOperatingInBurstMode));
				}
				else
				{
					//If any connected RX o/p ports are in native mode, check if they need to re-init. If so, mark them for re-init.
					MtMx_CheckIfAnyConnOutPortsInNativeModeNeedReInit(me, unInPortID);

					//Update some masks of connected o/p ports
					MtMx_ClosingInPortToUpdateMtMxMasksOfOutPort(me, unInPortID);

					MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, i/p port %lu closing. Live? %d Burstmode? %d Burstmode? %d",
							me->mtMxID, unInPortID, (int)(pCurrentInPort->bIsPortLive), (int)(pCurrentInPort->bIsInPortBurstMode), (int)(me->bIsMxAtOperatingInBurstMode));
				}

				//Check if PSPD libraries needs to be destroyed.
				MtMx_ClosingInPortToCheckDestroyPspdLibraries(me, unInPortID);

				//Now kill the PSPD threads. It is important that all the PSPD modules are destroyed prior to this.
				MtMx_ClosingInPortToCheckDestroyPspdThreads(me, unInPortID);

				(void)MtMx_ResetAvSyncSampleStats(me, unInPortID); //AV-Sync

				//check only when legacy port is closed. It is not required to check if low latency port is closed, since closing
				//a low latency port would not result in reiniting other ports. It is just an opz
				if (ADM_LEGACY_DEVICE_SESSION == pCurrentInPort->unPortPerfMode)
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu closing input port[%lu], checking for mixed latency connections",
							me->mtMxID,unInPortID);

					if (ADSP_FAILED(result = MtMx_DetectMixedLatencyConnections(me)))
					{
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu  closing input port[%lu], checking for mixed latency connections failed with error code [%u]",
								me->mtMxID, unInPortID,result);
						goto __bailoutCmdCfgInputPorts;
					}
				}

				uint32_t num_pspd = sizeof(pCurrentInPort->structPspd)/sizeof(pCurrentInPort->structPspd[0]);
				for (uint32_t i = 0; i < num_pspd; i++)
				{
					qurt_elite_queue_deinit((qurt_elite_queue_t*) pCurrentInPort->structPspd[i].cmd_resp_q);
					qurt_elite_queue_deinit((qurt_elite_queue_t*) pCurrentInPort->structPspd[i].inp_data_q);
					qurt_elite_queue_deinit((qurt_elite_queue_t*) pCurrentInPort->structPspd[i].out_data_q);
				}

				qurt_elite_channel_destroy(&pCurrentInPort->pspd_channel);

				//De-initialize AV-Sync
				(void)MtMx_DeInitInputAvSync(me, unInPortID);
				if(NULL != pCurrentInPort->avt_drv_handle)
				{
					//This call will be made only when pCurrentInPort->avt_drv_handle is non-NULL. This is non-NULL only when AV-Sync has been stubbed out.
					//When AV-Sync is stubbed out, svc is responsible for taking care of the request/release of AV-timer resource.
					//When AV-Sync is not stubbed out, AV-Sync lib is responsible for taking care of the request/release of AV-timer resource.
					MtMx_AvSyncReleaseHW(me, unInPortID);
				}

				//free input port state structure.
				if (NULL != me->inPortParams[unInPortID])
				{
					MTMX_FREE (me->inPortParams[unInPortID]);
				}

				if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
				{
					// rescan the output ports for any input ports with valid timestamps
					MtMx_ScanOutputPortsForInputsWithValidTimestamps(me);
          
					//Set Limiter mode on output ports if limiter instance is created  
					MtMx_SetLimiterModeOnOutputPorts(me);
				}

				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, i/p port %lu closed. SteadyStateInPortsMask = %lu",
						me->mtMxID, unInPortID, me->steadyStateInPortsMask);
			}
			break;
		}
		default:
		{
			result =  ADSP_EBADPARAM;
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu rcvd invalid port cfg [%d]",
					me->mtMxID, (int)(payload.pInPortCfgParams[i].configuration));
			goto __bailoutCmdCfgInputPorts;
			break;
		}
		}
	}

	//Update matrix KPPS and/or BW and raise ADSPPM event if needed.
	if (ADSP_FAILED(result = MtMx_SetReqKppsAndBW(me)))
	{
		goto __bailoutCmdCfgInputPorts;
	}

	__bailoutCmdCfgInputPorts:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (CfgIpPort) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

ADSPResult MtMx_CfgOutputPorts(This_t *me, elite_msg_any_t* pMsg)
{
	ADSPResult                 result = ADSP_EOK;
	int                        i, len, index;
	uint32_t                   unOutPortID, count, j;
	MatrixOutPortInfoType      *pCurrentOutPort;

	EliteMsg_CustomCfgOutPortsType* pPayload = (EliteMsg_CustomCfgOutPortsType*)pMsg->pPayload;
	CfgOutputPortsPayload_t payload = pPayload->cfgOutPortsPayload;

	len = payload.numOutPorts;

	MtMxOutPortHandle_t **tempPtr = payload.pAck;

	for (i = 0; i < len; i++)
	{
		index = payload.pOutPortCfgParams[i].index;

		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd 0x%lx (CfgOpPort) [port ID, port cfg] = [%d, %d]",
				me->mtMxID, pPayload->unSecOpCode, (int)(payload.pOutPortCfgParams[i].index), (int)(payload.pOutPortCfgParams[i].configuration));

		switch(payload.pOutPortCfgParams[i].configuration)
		{
		case NEW_OUTPUT:
		{
			unOutPortID = index;
			if (index < 0)
			{
				unOutPortID = 0;
				while (unOutPortID < MT_MX_MAX_OUTPUT_PORTS && (NULL != me->outPortParams[unOutPortID]))
				{
					unOutPortID++;
				}
				if (MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
				{
					result = ADSP_EFAILED;
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: No more output ports available");
					goto __bailoutCmdCfgOutputPorts;
				}
			}
			else if (MT_MX_MAX_OUTPUT_PORTS <= index || NULL != me->outPortParams[index])
			{
				result = ADSP_EBADPARAM;
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: Error, invalid port ID rcvd [%d]", me->mtMxID, (int)index);
				goto __bailoutCmdCfgOutputPorts;
			}

			//allocate output port state structure.
			if ( (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID) &&  (0 == unOutPortID))
			{
				me->outPortParams[unOutPortID] = (MatrixOutPortInfoType*)qurt_elite_memory_malloc(sizeof(MatrixOutPortInfoType), MTMX_SVC_PORT_STRUCTURE);
			}
			else
			{
				me->outPortParams[unOutPortID] = (MatrixOutPortInfoType*)qurt_elite_memory_malloc(sizeof(MatrixOutPortInfoType), QURT_ELITE_HEAP_DEFAULT);
			}

			if (NULL == me->outPortParams[unOutPortID])
			{
				result = ADSP_ENOMEMORY;
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu failed to alloc mem for output port state strucutre.", me->mtMxID);
				goto __bailoutCmdCfgOutputPorts;
			}
			memset((void*)me->outPortParams[unOutPortID], 0, sizeof(MatrixOutPortInfoType));
			pCurrentOutPort = me->outPortParams[unOutPortID];
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu creating new output port %lu", me->mtMxID, unOutPortID);

			//Initialize AV-Sync
			MtMx_InitOutputAvSync(me, unOutPortID);
			if(ADSP_ENOMEMORY == result)
			{
				goto __bailoutCmdCfgOutputPorts;
			}

			Elite_CbType *pEliteCb = &(pCurrentOutPort->eliteCb);
			pEliteCb->pCbHandle = NULL;

			if (NULL != (payload.pOutPortCfgParams[i].pEliteCb) && NULL!= (payload.pOutPortCfgParams[i].pEliteCb->pCbHandle))
			{
				Elite_CbType *pIncomingEliteCb = payload.pOutPortCfgParams[i].pEliteCb;
				pEliteCb->pCbFct = pIncomingEliteCb->pCbFct;
				pEliteCb->ulHandleSize = pIncomingEliteCb->ulHandleSize;
				pEliteCb->pCbHandle = qurt_elite_memory_malloc(pEliteCb->ulHandleSize, QURT_ELITE_HEAP_DEFAULT);

				if (!pEliteCb->pCbHandle)
				{
					result = ADSP_ENOMEMORY;
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu failed to alloc mem for ASM callback handle", me->mtMxID);
					goto __bailoutCmdCfgOutputPorts;
				}
				memscpy(pEliteCb->pCbHandle, pEliteCb->ulHandleSize, pIncomingEliteCb->pCbHandle, pEliteCb->ulHandleSize);
			}

			pCurrentOutPort->unOverflowReportStatus = MT_MX_OUTPUT_PORT_OVERFLOW_REPORT_STATUS_DO_NOT_REPORT;
			pCurrentOutPort->outPortStatusPriorToReconfig = OUTPUT_PORT_STATE_INACTIVE;
			pCurrentOutPort->unPortPerfMode = payload.pOutPortCfgParams[i].perfMode;

			if(NULL != payload.pOutPortCfgParams[i].ppAfeDriftPtr)
			{
				(void)MtMx_SetOutputDriftPtr(me, unOutPortID, *(payload.pOutPortCfgParams[i].ppAfeDriftPtr));
			}

			for(j=0;j<MT_MX_NUM_CHANNELS_EIGHT; j++)
			{
				pCurrentOutPort->usChannelMappingPrRecfg[j] = 0;
			}

			MtMx_SetOutPortFrameDuration(me, unOutPortID);

			//Check and update if this is the new MAX outport ID
			if (me->maxOutPortID < unOutPortID) me->maxOutPortID = unOutPortID;

			pCurrentOutPort->unDataLogId = payload.pOutPortCfgParams[i].unDataLogId;
			pCurrentOutPort->avt_drv_handle = NULL;

			if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
			{
				//RX matrix does not support the absolute TS mode
				pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS = FALSE;

				//Only the RX matrix needs to query downstream COPP delay.
				if(NULL != payload.pOutPortCfgParams[i].punCoppBufDelay)
				{
					pCurrentOutPort->punCoppBufDelay = payload.pOutPortCfgParams[i].punCoppBufDelay;
				}
				else
				{
					result = ADSP_EBADPARAM;
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu recd NULL punCoppBufDelay", me->mtMxID);
					goto __bailoutCmdCfgOutputPorts;
				}

				if(NULL != payload.pOutPortCfgParams[i].punCoppAlgDelay)
				{
					pCurrentOutPort->punCoppAlgDelay = payload.pOutPortCfgParams[i].punCoppAlgDelay;
				}
				else
				{
					result = ADSP_EBADPARAM;
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu recd NULL punCoppAlgDelay", me->mtMxID);
					goto __bailoutCmdCfgOutputPorts;
				}

				//Only the RX matrix needs to report it's outport port delay.
				pCurrentOutPort->punMtMxOutDelay = NULL;

				//Only the RX matrix needs to query downstream AFE delay.
				if(NULL != payload.pOutPortCfgParams[i].punAFEDelay)
				{
					pCurrentOutPort->punAFEDelay = payload.pOutPortCfgParams[i].punAFEDelay;
				}
				else
				{
					result = ADSP_EBADPARAM;
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu recd NULL punAFEDelay", me->mtMxID);
					goto __bailoutCmdCfgOutputPorts;
				}
			}
			else
			{
				//TX matrix supports relative mode for TS (default) and absolute mode
				if(ADM_STREAM_CONNECT_PORT_MODE_ABSOLUTE_TS == payload.pOutPortCfgParams[i].unPortMode)
				{
					pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS = TRUE;
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx %lu o/p port %lu cfg to absolute TS mode.", me->mtMxID, unOutPortID);
				}
				else
				{
					pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS = FALSE;
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx %lu o/p port %lu cfg to relative TS mode.", me->mtMxID, unOutPortID);
				}

				if(NULL != payload.pOutPortCfgParams[i].punMtMxDelay)
				{
					pCurrentOutPort->punMtMxOutDelay = payload.pOutPortCfgParams[i].punMtMxDelay;
					*(pCurrentOutPort->punMtMxOutDelay) = 0;
				}
				else
				{
					result = ADSP_EBADPARAM;
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu recd NULL punMtMxDelay", me->mtMxID);
					goto __bailoutCmdCfgOutputPorts;
				}


				//TX matrix punAFEDelay, punCoppDelay and punMtMxOutDelay does not make sense/not applicable.
				pCurrentOutPort->punAFEDelay = NULL;
				pCurrentOutPort->punCoppBufDelay = NULL;
				pCurrentOutPort->punCoppAlgDelay = NULL;

				// Open AV timer driver (only needed on the TX matrix o/p port)
				avtimer_open_param_t open_param;
				char client_name[] = "MtMxOp";
				open_param.client_name = client_name;
				open_param.flag = 0;
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Opening AVTimer Driver for o/p port %lu",me->mtMxID, unOutPortID);
				if (ADSP_EOK != (result = avtimer_drv_hw_open(&(pCurrentOutPort->avt_drv_handle), &open_param)))
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open AVTimer Driver for o/p port %lu with result: %d",unOutPortID, result);
					pCurrentOutPort->avt_drv_handle = NULL;
					goto __bailoutCmdCfgOutputPorts;
				}
			}

			pCurrentOutPort->numBufRcvd = 0;
			pCurrentOutPort->unNumOutputBufs = payload.pOutPortCfgParams[i].numOutputBufs;
			pCurrentOutPort->unMaxBufQCapacity = payload.pOutPortCfgParams[i].maxBufQCapacity;
			pCurrentOutPort->pDownstreamPeerHandle = payload.pOutPortCfgParams[i].pSvcHandle;

			//Set o/p port properties
			pCurrentOutPort->unInterleaveFlag = payload.pOutPortCfgParams[i].interleaveFlag;
			pCurrentOutPort->unSampleRate = payload.pOutPortCfgParams[i].unSampleRate;

			// Set nativity
      pCurrentOutPort->unNativeModeFlags = payload.pOutPortCfgParams[i].unNativeModeFlags;


      pCurrentOutPort->bIsOutPortInNativeModePendingReInit = FALSE;
      pCurrentOutPort->unNumChannels = payload.pOutPortCfgParams[i].numChannels;

      for(j = 0; j < pCurrentOutPort->unNumChannels; j++)
      {
        pCurrentOutPort->unChannelMapping[j] = payload.pOutPortCfgParams[i].channelMapping[j];

        //Ensure that channel mapping has legitimate values
        if((pCurrentOutPort->unChannelMapping[j] > MAX_CHANNEL_MAPPING_NUMBER) ||
           (pCurrentOutPort->unChannelMapping[j] < MT_MX_NUM_CHANNELS_ONE))
        {
          result = ADSP_EBADPARAM;
          MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: Error, Invalid chan map! o/p port %lu ch# %lu, chmap: %d",
                me->mtMxID, unOutPortID, j + 1, (int)pCurrentOutPort->unChannelMapping[j]);

          // a. check if mem was alloc'd for ASM callback handle. If yes, free it.
          if(pEliteCb->pCbHandle)
          {
            MTMX_FREE(pEliteCb->pCbHandle);
          }
          goto __bailoutCmdCfgOutputPorts;
        }
      }
      pCurrentOutPort->unBitwidth = payload.pOutPortCfgParams[i].unBitWidth;
      pCurrentOutPort->unBytesPerSample = (pCurrentOutPort->unBitwidth > 16)? MT_MX_BYTES_PER_SAMPLE_FOUR : MT_MX_BYTES_PER_SAMPLE_TWO;

      // Native mode case
			if(0 != pCurrentOutPort->unNativeModeFlags)
			{
				//At this point in time, only RX matrix allows setting of native mode during port creation.
				//For TX matrix, a separate re-config command needs to be sent.
				if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
				{
					//O/p port operating in native mode, needs reinit
					pCurrentOutPort->bIsOutPortInNativeModePendingReInit = TRUE;

					//For o/p port operating in native mode, the Num. of channels, channel mapping and bytes/sample
					//needs to be derived from the connected i/p ports.
					MxAr_UpdateMediaTypeForNativeModeOutputPort(me, unOutPortID);
				}
				else
				{
					result = ADSP_EBADPARAM;
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu recd native mode during port creation. This is not supported.", me->mtMxID);

					// a. check if mem was alloc'd for ASM callback handle. If yes, free it.
					if (pEliteCb->pCbHandle)
					{
						MTMX_FREE (pEliteCb->pCbHandle);
					}
					goto __bailoutCmdCfgOutputPorts;
				}
			}

			MtMx_SetOutPortFrameDuration(me, unOutPortID);

			//Print debug information
			MtMx_PrintDebugMediaTypeInformationOutputPort(me, unOutPortID);

			//Name the output port's buf Q
			count = qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;
			snprintf(pCurrentOutPort->bufQName, QURT_ELITE_DEFAULT_NAME_LEN, "MtMxBufQ0%lx", count);

			//Create the output port's buf Q and add it to the matrix's channel
			if (ADSP_FAILED(result = qurt_elite_queue_create(pCurrentOutPort->bufQName,
					pCurrentOutPort->unMaxBufQCapacity,
					&(pCurrentOutPort->bufQ))))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx failed to create o/p port bufQ, result = %d", (int)result);

				// a. check if mem was alloc'd for ASM callback handle. If yes, free it.
				if (pEliteCb->pCbHandle)
				{
					MTMX_FREE (pEliteCb->pCbHandle);
				}
				goto __bailoutCmdCfgOutputPorts;
			}

			// Add the queue to the channel
			if (ADSP_FAILED(result = qurt_elite_channel_addq(&me->channel, pCurrentOutPort->bufQ, 0)))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx failed to add bufQ to channel, result = %d", (int)result);

				// a. destroy the bufQ and 0 associated output buffers
				elite_svc_destroy_buf_queue(pCurrentOutPort->bufQ, 0);

				// a. check if mem was alloc'd for ASM callback handle. If yes, free it.
				if (pEliteCb->pCbHandle)
				{
					MTMX_FREE (pEliteCb->pCbHandle);
				}
				goto __bailoutCmdCfgOutputPorts;
			}

			//Allocate buffers (internal as well as data buffers)
			if (ADSP_FAILED(result = MtMx_OutportAllocBuffers(me, unOutPortID, pCurrentOutPort)))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx failed to add bufQ to alloc buffers, result = %d", (int)result);

				// a. check if mem was alloc'd for ASM callback handle. If yes, free it.
				if (pEliteCb->pCbHandle)
				{
					MTMX_FREE (pEliteCb->pCbHandle);
				}
				goto __bailoutCmdCfgOutputPorts;
			}

			pCurrentOutPort->outPortHandle.outPortID = unOutPortID;
			pCurrentOutPort->outPortHandle.portHandle = me->serviceHandle;
			pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_INIT;
			pCurrentOutPort->bIsOutPortInNativeModePendingReInit = FALSE;

			// Set the unConnInPortIdWithValidTs to invalid so we don't accidently propagate timestamps
			// until an input with valid timestamps is connected.
			pCurrentOutPort->unConnInPortIdWithValidTs = MT_MX_INVALID_PORT_ID;

			// Create and send media_type msg downstream.
			if (ADSP_FAILED(result = MtMx_OutportSendMediaTypeMsgDS(me, unOutPortID, pCurrentOutPort)))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx failed to send media type DS, result = %d", (int)result);

				// a. check if mem was alloc'd for ASM callback handle. If yes, free it.
				if (pEliteCb->pCbHandle)
				{
					MTMX_FREE (pEliteCb->pCbHandle);
				}
				goto __bailoutCmdCfgOutputPorts;
			}

			if ((ADM_MATRIX_ID_AUDIO_TX == me->mtMxID) && (FALSE == pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS))
			{
				mt_mx_decrement_time(&pCurrentOutPort->llSessionTime, pCurrentOutPort->unFrameDurationInUsec);
			}

			*tempPtr = &(pCurrentOutPort->outPortHandle);

			if(FALSE == MtMx_ChannelNativityIsEnabled(pCurrentOutPort->unNativeModeFlags))
			{
				//Check if PSPD libraries needs to be re-init.
				MtMx_OutPortToCheckReInitPspdLibraries(me, unOutPortID);
			}

			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu new o/p port id %lu open success", me->mtMxID, unOutPortID);

			break;
		}
		case DISABLE_OUTPUT:
		{
			if ((0 > index) || ((int)(me->maxOutPortID) < index)
					|| ((OUTPUT_PORT_STATE_INACTIVE == me->outPortParams[index]->outPortState)))
			{
				result = ADSP_EBADPARAM;

				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: received invalid output port id (%d) to disable",
						me->mtMxID, (int)index);

				goto __bailoutCmdCfgOutputPorts;
			}
			else
			{
				unOutPortID = index;
				pCurrentOutPort = me->outPortParams[unOutPortID];
				pCurrentOutPort->accInPortsMask = 0;
				MtMx_RemoveOutputPortFromWaitMask(me, unOutPortID);
				me->unBufBitfield = me->unBufBitfield & ~qurt_elite_queue_get_channel_bit(pCurrentOutPort->bufQ);
				pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_DISABLED;
				pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = FALSE;
				pCurrentOutPort->unNativeModeFlags = 0;
				pCurrentOutPort->bIsOutPortInNativeModePendingReInit = FALSE;
				pCurrentOutPort->unOverflowReportStatus = MT_MX_OUTPUT_PORT_OVERFLOW_REPORT_STATUS_DO_NOT_REPORT;
				pCurrentOutPort->unOutPortReconfigMode = 0;
				pCurrentOutPort->outPortStatusPriorToReconfig = OUTPUT_PORT_STATE_DISABLED;

				//Update various masks
				MtMx_OutPortToUpdateActiveOutPortsMasks(me, unOutPortID);
				if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
				{
					MxAt_OutPortToUpdateOutputReqPendingMasks(me, unOutPortID);
				}
				MtMx_OutPortToUpdateWaitMask(me, unOutPortID);
				//a. Remove this output port from strMask's of all input ports that were routing PCM samples to this output port.
				//b. If this was a top priority port for any of the input ports, re-assign the top priority port for those input ports.
				MtMx_RecfgInPorts(me, unOutPortID);

				//Check if PSPD libraries needs to be destroyed.
				MtMx_ClosingOutPortToCheckDestroyPspdLibraries(me, unOutPortID);

				//Now kill the PSPD threads. It is important that all the PSPD modules are destroyed prior to this.
				MtMx_ClosingOutPortToCheckDestroyPspdThreads(me, unOutPortID);
			}
			break;
		}
		case CLOSE_OUTPUT:
		{
			if ((0 > index) || ((int)(me->maxOutPortID) < index)
					|| (OUTPUT_PORT_STATE_INACTIVE == me->outPortParams[index]->outPortState))
			{
				result = ADSP_EBADPARAM;
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: received invalid output port id (%d) to close",
						me->mtMxID, (int)index);
				goto __bailoutCmdCfgOutputPorts;
			}
			else
			{
				unOutPortID = index;
				pCurrentOutPort = me->outPortParams[unOutPortID];

				// Close AV timer driver. For RX matrix this is not applicable and should pass right through (NULLness check).
				if (NULL != pCurrentOutPort->avt_drv_handle)
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Releasing AVTimer Driver for o/p port %lu",me->mtMxID, unOutPortID);
					if (ADSP_EOK != (result = avtimer_drv_hw_close(pCurrentOutPort->avt_drv_handle)))
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to close AVTimer Driver for o/p port %lu with result: %d",unOutPortID,result);
						pCurrentOutPort->avt_drv_handle = NULL;
						goto __bailoutCmdCfgOutputPorts;
					}
					pCurrentOutPort->avt_drv_handle = NULL;
				}

				if (pCurrentOutPort->eliteCb.pCbHandle)
				{
					MTMX_FREE (pCurrentOutPort->eliteCb.pCbHandle);
				}

				// free output port acc buffer
				if(pCurrentOutPort->pAccBuf)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu o/p port %lu Freeing acc. buffer: 0x%p", me->mtMxID, unOutPortID,  pCurrentOutPort->pAccBuf);
					MTMX_ALIGNED_FREE(pCurrentOutPort->pAccBuf);
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu o/p port %lu Freed acc. buffer", me->mtMxID, unOutPortID);
				}
        
				// free output port limiter instance
				if(NULL != pCurrentOutPort->structLimiter.pLimiterCapiV2)
				{
				  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu o/p port %lu Freeing Limiter module", me->mtMxID, unOutPortID);
				  MtMx_OutPortLimiterDeinit(me,unOutPortID);
				}

				MtMx_RemoveOutputPortFromWaitMask(me, unOutPortID);
				me->unBufBitfield = me->unBufBitfield & ~qurt_elite_queue_get_channel_bit(pCurrentOutPort->bufQ);

				//Reset the outPortIDMap for this o/p port to -1
				{
					uint32_t unOutPortIDIndex,unOutPortIDChannelBitNo;
					unOutPortIDChannelBitNo = qurt_elite_queue_get_channel_bit(pCurrentOutPort->bufQ);
					unOutPortIDIndex = 31 - (Q6_R_cl0_R( (unOutPortIDChannelBitNo)) );
					me->outPortIDMap[unOutPortIDIndex] = -1;
				}

				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu o/p port %lu Destroying o/p BufQ, num of buffers %lu", me->mtMxID, unOutPortID, pCurrentOutPort->unNumOutputBufsPendingRelease);
				elite_svc_destroy_buf_queue(pCurrentOutPort->bufQ, pCurrentOutPort->unNumOutputBufsPendingRelease);
				pCurrentOutPort->unNumOutputBufsPendingRelease = 0;
				pCurrentOutPort->bIsOutPortPendingReleaseBuffers = FALSE;

				//Clear out some masks
				MtMx_OutPortToUpdateActiveOutPortsMasks(me, unOutPortID);
				if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
				{
					MxAt_OutPortToUpdateOutputReqPendingMasks(me, unOutPortID);
				}
				MtMx_OutPortToUpdateWaitMask(me, unOutPortID);

				// a. Remove this output port from strMask's of all input ports that were routing PCM samples to this output port.
				// b. If this was a top priority port for any of the input ports, re-assign the top priority port for those input ports.
				MtMx_RecfgInPorts(me, unOutPortID);

				//Check if PSPD libraries needs to be destroyed.
				MtMx_ClosingOutPortToCheckDestroyPspdLibraries(me, unOutPortID);

				//Now kill the PSPD threads. It is important that all the PSPD modules are destroyed prior to this.
				MtMx_ClosingOutPortToCheckDestroyPspdThreads(me, unOutPortID);

				// check only when legacy port is closed. It is not required to check if low latency port is closed, since closing
				// a low latency port would not result in reiniting other ports. It is just an opz
				if ( ADM_LEGACY_DEVICE_SESSION == pCurrentOutPort->unPortPerfMode )
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu closing output port[%lu], checking for mixed latency connections",
							me->mtMxID,unOutPortID);
					if (ADSP_FAILED(result = MtMx_DetectMixedLatencyConnections(me)))
					{
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu  closing output port[%lu], checking for mixed latency connections failed with error code [%u]",
								me->mtMxID, unOutPortID,result);
						goto __bailoutCmdCfgOutputPorts;
					}
				}

				//De-initialize AV-Sync
				(void)MtMx_DeInitOutputAvSync(me, unOutPortID);

				// free output port state structure.
				if (me->outPortParams[unOutPortID])
				{
					MTMX_FREE (me->outPortParams[unOutPortID]);
				}
			}
			break;
		}
		case RECONFIGURE_OUTPUT:
		case RECONFIGURE_OUTPUT_SAMPLE_RATE:
		{
			if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
			{
				//Check o/p port index to be in the valid range
				if((MT_MX_MAX_OUTPUT_PORTS <= index) || (0 > index))
				{
					result = ADSP_EBADPARAM;
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: Error, invalid port ID [%d] rcvd for O/p port reconfig.", me->mtMxID, (int)index);
					goto __bailoutCmdCfgOutputPorts;
				}

				unOutPortID = index;
				pCurrentOutPort = me->outPortParams[unOutPortID];

				if (OUTPUT_PORT_STATE_INACTIVE == pCurrentOutPort->outPortState)
				{
					result = ADSP_EBADPARAM;
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: Error, o/p port ID [%lu] rcvd for O/p port reconfig is inactive.", me->mtMxID, unOutPortID);
					goto __bailoutCmdCfgOutputPorts;
				}

				//Store the current o/p port cfg. Only store it if the o/p port is not in Inactive state.
				//If in Inactive State, dont overwrite previous state.
				if(OUTPUT_PORT_STATE_INACTIVE != pCurrentOutPort->outPortState)
				{
					MtMx_OpPortReCfg_StoreCurrentCfg(me, unOutPortID, pCurrentOutPort);
				}

				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Reconfiguring output port %lu Begin. Current state %d.",
						me->mtMxID, unOutPortID, pCurrentOutPort->outPortState);

				MtMx_PrintDebugMediaTypeInformationOutputPort(me, unOutPortID);

				//Mark this o/p port state as inactive
				pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_INACTIVE;

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu output port %lu fs: %lu, FrameDur %lu samples", me->mtMxID, unOutPortID,
						pCurrentOutPort->unSampleRate, pCurrentOutPort->unOutPortPerChBufSize);

				//Since this port state has changed to inactive, need to update ActiveOutPortsMask
				MtMx_OutPortToUpdateActiveOutPortsMasks(me, unOutPortID);

				//Update o/p port sample rate
				if (RECONFIGURE_OUTPUT_SAMPLE_RATE == payload.pOutPortCfgParams[i].configuration)
				{
					pCurrentOutPort->unSampleRate = payload.pOutPortCfgParams[i].unSampleRate;
				}

				//Output port mode and other information will be available only when
				//reconfigure output is used. this is used from ASM
				if (RECONFIGURE_OUTPUT == payload.pOutPortCfgParams[i].configuration)
				{
					pCurrentOutPort->unDataLogId = payload.pOutPortCfgParams[i].unDataLogId;

					//Update o/p buffer requirements
					pCurrentOutPort->unNumOutputBufs = payload.pOutPortCfgParams[i].numOutputBufs;
					pCurrentOutPort->unMaxBufQCapacity = payload.pOutPortCfgParams[i].maxBufQCapacity;

					//Update o/p buffer downstream peer handle
					pCurrentOutPort->pDownstreamPeerHandle = payload.pOutPortCfgParams[i].pSvcHandle;

					//Update o/p Interleaved Flag
					pCurrentOutPort->unInterleaveFlag = payload.pOutPortCfgParams[i].interleaveFlag;

					// Update Nativity settings
					pCurrentOutPort->unNativeModeFlags = payload.pOutPortCfgParams[i].unNativeModeFlags;

					if (0 == payload.pOutPortCfgParams[i].unNativeModeFlags)
					{
						pCurrentOutPort->bIsOutPortInNativeModePendingReInit = FALSE;

						//Update o/p Bytes/sample
						pCurrentOutPort->unBitwidth = payload.pOutPortCfgParams[i].unBitWidth;
						pCurrentOutPort->unBytesPerSample = ((pCurrentOutPort->unBitwidth)>16)? MT_MX_BYTES_PER_SAMPLE_FOUR : MT_MX_BYTES_PER_SAMPLE_TWO;

						//Update o/p Num. of channels and Channel mapping
						pCurrentOutPort->unNumChannels = payload.pOutPortCfgParams[i].numChannels;

						for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
						{
							pCurrentOutPort->unChannelMapping[j] = payload.pOutPortCfgParams[i].channelMapping[j];

							//Ensure that channel mapping has legitimate values
							if((pCurrentOutPort->unChannelMapping[j] > MAX_CHANNEL_MAPPING_NUMBER) ||
									(pCurrentOutPort->unChannelMapping[j] < MT_MX_NUM_CHANNELS_ONE))
							{
								result = ADSP_EBADPARAM;
								MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: Error, Invalid chan map! o/p port %lu ch# %lu, chmap: %d",
										me->mtMxID, unOutPortID, j+1, (int)pCurrentOutPort->unChannelMapping[j]);
								goto __bailoutCmdCfgOutputPorts;
							}
						}
					}
					else
					{
						//For TX o/p port reconfig mode, native mode
						pCurrentOutPort->bIsOutPortInNativeModePendingReInit = TRUE;

						//For o/p port operating in native mode, the Num. of channels, channel mapping and bytes/sample
						//needs to be derived from the connected i/p ports.
						MxAt_UpdateMediaTypeForNativeModeOutputPort(me, unOutPortID);
					}
				}

				MtMx_SetOutPortFrameDuration(me, unOutPortID);
				MtMx_DetectMixedLatencyConnections(me);

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu output port %lu fs: %lu, FrameDur %lu samples", me->mtMxID, unOutPortID,
						pCurrentOutPort->unSampleRate, pCurrentOutPort->unOutPortPerChBufSize);

				//Store the o/p port reconfig mode
				pCurrentOutPort->unOutPortReconfigMode = payload.pOutPortCfgParams[i].configuration;

				//In addition, if no buffer has been delivered out of this o/p port yet, reconfig right away
				if(0 == pCurrentOutPort->numBufSent)
				{
					//At this point, pCurrentOutPort->bIsOutPortPendingReleaseBuffers is FALSE
					while(0 < pCurrentOutPort->unNumOutputBufsPendingRelease)
					{
						result = MtMx_OpPortPopBuffer(me, unOutPortID);
						if(ADSP_FAILED(result))
						{
							MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %lu Error MtMx_OutPortPopBuffer Result = %d",
									me->mtMxID, unOutPortID, result);
							goto __bailoutCmdCfgOutputPorts;
						}
					}
				}
				else
				{
					//Make sure the o/p port wakes up when the buffer does return.
					MtMx_AddOutputPortToWaitMask(me, unOutPortID);

					//Mark this o/p port as pending re-init
					pCurrentOutPort->bIsOutPortPendingReleaseBuffers = TRUE;
				}

				//Allocate new buffers memory, send out new MT and o/p buffers
				result = MtMx_OpPortReCfg(me, unOutPortID);

				if(ADSP_FAILED(result))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %lu Error in recfg, Result = %d",
							me->mtMxID, unOutPortID, (int)result);
					MtMx_ForceDisableOutputPortInBadState(me, unOutPortID);
				}
			}
			else if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
			{
				//Check o/p port index to be in the valid range
				if((MT_MX_MAX_OUTPUT_PORTS <= index) || (0 > index))
				{
					result = ADSP_EBADPARAM;
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: Error, invalid port ID [%d] rcvd for O/p port reconfig.", me->mtMxID, (int)index);
					goto __bailoutCmdCfgOutputPorts;
				}

				unOutPortID = index;
				pCurrentOutPort = me->outPortParams[unOutPortID];

				if (OUTPUT_PORT_STATE_INACTIVE == pCurrentOutPort->outPortState)
				{
					result = ADSP_EBADPARAM;
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: Error, o/p port ID [%lu] rcvd for O/p port reconfig is inactive.", me->mtMxID, unOutPortID);
					goto __bailoutCmdCfgOutputPorts;
				}

				//Store the current o/p port cfg. Only store it if the o/p port is not in Inactive state.
				//If in Inactive State, dont overwrite previous state.
				if(OUTPUT_PORT_STATE_INACTIVE != pCurrentOutPort->outPortState)
				{
					MtMx_OpPortReCfg_StoreCurrentCfg(me, unOutPortID, pCurrentOutPort);
				}

				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Reconfiguring output port %lu Begin. Current state %d.",
						me->mtMxID, unOutPortID, pCurrentOutPort->outPortState);

				MtMx_PrintDebugMediaTypeInformationOutputPort(me, unOutPortID);

				//Mark this o/p port state as inactive
				pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_INACTIVE;

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu output port %lu fs: %lu, FrameDur %lu samples", me->mtMxID, unOutPortID,
						pCurrentOutPort->unSampleRate, pCurrentOutPort->unOutPortPerChBufSize);

				//Since this port state has changed to inactive, need to update ActiveOutPortsMask
				MtMx_OutPortToUpdateActiveOutPortsMasks(me, unOutPortID);

				//Update o/p port sample rate
				pCurrentOutPort->unSampleRate = payload.pOutPortCfgParams[i].unSampleRate;

				MtMx_SetOutPortFrameDuration(me, unOutPortID);

				MtMx_DetectMixedLatencyConnections(me);

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu output port %lu fs: %lu, FrameDur %lu samples", me->mtMxID, unOutPortID,
						pCurrentOutPort->unSampleRate, pCurrentOutPort->unOutPortPerChBufSize);

				//Store the o/p port reconfig mode
				pCurrentOutPort->unOutPortReconfigMode = payload.pOutPortCfgParams[i].configuration;

				//In addition, if no buffer has been delivered out of this o/p port yet, reconfig right away
				if(0 == pCurrentOutPort->numBufSent)
				{
					//At this point, pCurrentOutPort->bIsOutPortPendingReleaseBuffers is FALSE
					while(0 < pCurrentOutPort->unNumOutputBufsPendingRelease)
					{
						result = MtMx_OpPortPopBuffer(me, unOutPortID);
						if(ADSP_FAILED(result))
						{
							MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %lu Error MtMx_OutPortPopBuffer Result = %d",
									me->mtMxID, unOutPortID, result);
							goto __bailoutCmdCfgOutputPorts;
						}
					}
				}
				else
				{
					//Make sure the o/p port wakes up when the buffer does return.
					MtMx_AddOutputPortToWaitMask(me, unOutPortID);

					//Mark this o/p port as pending re-init
					pCurrentOutPort->bIsOutPortPendingReleaseBuffers = TRUE;
				}

				//Allocate new buffers memory, send out new MT and o/p buffers
				result = MtMx_OpPortReCfg(me, unOutPortID);

				if(ADSP_FAILED(result))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %lu Error in recfg, Result = %d",
							me->mtMxID, unOutPortID, (int)result);
					MtMx_ForceDisableOutputPortInBadState(me, unOutPortID);
				}
			}
			break;
		}
		default:
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Error: MtMx #%lu rcvd invalid port cfg [%d]",
					me->mtMxID, (int)(payload.pOutPortCfgParams[i].configuration));

			result =  ADSP_EBADPARAM;
			break;
		}
		}
	}

	//Update matrix KPPS and/or BW and raise ADSPPM event if needed.
	if (ADSP_FAILED(result = MtMx_SetReqKppsAndBW(me)))
	{
		goto __bailoutCmdCfgOutputPorts;
	}

	__bailoutCmdCfgOutputPorts:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (CfgOpPort) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

ADSPResult MtMx_DestroyYourself(void* pInstance, elite_msg_any_t* pMsg)
{
	This_t* me = (This_t*)pInstance;
	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: received msg ELITE_CMD_DESTROY_SERVICE",
			me->mtMxID);
	AudDestroyMatrixMixerSvc(me);

	// since Matrix Mixer doesn't send a respond for the ELITE_CMD_DESTROY_SERVICE, it is responsible to return the payload buffer.
	elite_msg_cmd_destroy_svc_t* pPayload = (elite_msg_cmd_destroy_svc_t*)pMsg->pPayload;
	elite_msg_push_payload_to_returnq(pPayload->pBufferReturnQ, (elite_msg_any_payload_t*)pPayload);
	return ADSP_ETERMINATED;
}





ADSPResult MtMx_MapRoutings(This_t *me, elite_msg_any_t* pMsg)
{
	adm_cmd_matrix_map_routings_v5_t         *pRoutingPayload;
	adm_session_map_node_v5_t                *pSessionMapNode;
	ADSPResult                            result = ADSP_EOK;
	uint32_t                              unInPortID, unOutPortID;
	uint32_t                              unNumSessions, i, j;
	uint16_t                              unNumCopps;
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType    *pCurrentOutPort;
	mt_mx_struct_pspd_t    *pCurrentPspd;
	uint32_t matrix_id; //Specifies whether the matrix ID is Audio Rx (0) or Audio Tx (1)

	//Get the payload and access the unNumSessions
	EliteMsg_CustomMtMxMapRoutingsType *pPayload = (EliteMsg_CustomMtMxMapRoutingsType*)pMsg->pPayload;
	pRoutingPayload = pPayload->pRoutingPayload;
	unNumSessions = pRoutingPayload->num_sessions;
	
	if((ADM_LPCM_AUDIO_OUT == pRoutingPayload->matrix_id) ||
			(ADM_MATRIX_ID_AUDIO_RX == pRoutingPayload->matrix_id) )
	{
		matrix_id = ADM_MATRIX_ID_AUDIO_RX;
	}
	else if( (ADM_LPCM_AUDIO_IN == pRoutingPayload->matrix_id) || 
						(ADM_MATRIX_ID_AUDIO_TX == pRoutingPayload->matrix_id) ||
						(ADM_LSM_IN == pRoutingPayload->matrix_id) )
	{
		matrix_id = ADM_MATRIX_ID_AUDIO_TX;
	}
	else
	{
		result = ADSP_EUNEXPECTED;
		elite_msg_finish_msg(pMsg, result);
		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Rcvd invalid Matrix ID [%lu], Leaving cmd [%lu] (MapRoutings) handler with status %d", me->mtMxID, pRoutingPayload->matrix_id, pPayload->unSecOpCode, (int)result);
		return result;
	}
	
	//Loop through the sessions
	adm_session_map_node_v5_t *pTmp2 = (adm_session_map_node_v5_t*)(pRoutingPayload + 1);
	if (ADM_MATRIX_ID_AUDIO_RX == matrix_id)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler [num i/p ports] = [%d]",
				me->mtMxID, pPayload->unSecOpCode, (int)pRoutingPayload->num_sessions);
	}
	else
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler [num o/p ports] = [%d]",
				me->mtMxID, pPayload->unSecOpCode, (int)pRoutingPayload->num_sessions);
	}
	for (i = 0; i < unNumSessions; i++)
	{
		//Get the number of COPPs attached to this session
		pSessionMapNode =  pTmp2;
		unNumCopps = pSessionMapNode->num_copps;

		// For LLNP and ULL matrix map routing is not required. Skip this session.
		if ((MT_MX_ULL_SESSION == pSessionMapNode->session_id)
				|| (MT_MX_LLNP_SESSION == pSessionMapNode->session_id)
				|| (MT_MX_ULLPP_SESSION == pSessionMapNode->session_id))
		{
			//Update session mapping node pointer. An stream session that bypasses the matrix will be connected to single device.
			//if number of COPP's is odd, there's a reserved field after the last valid COPP ID.
			pTmp2 = (adm_session_map_node_v5_t*)( (uint16_t*) (pTmp2 + 1)  + unNumCopps ) ;
			if (unNumCopps & 1)
			{
				pTmp2 = (adm_session_map_node_v5_t*) ( (uint16_t*)pTmp2 + 1 );
			}
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: ULL session detected, skipping processing for this session",
					me->mtMxID, pPayload->unSecOpCode);
			continue;
		}

		if (ADM_MATRIX_ID_AUDIO_RX == matrix_id)
		{
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: i/p port id = %d, num o/p ports = %d",
					me->mtMxID, pPayload->unSecOpCode, (int)pSessionMapNode->session_id, (int)pSessionMapNode->num_copps);
		}
		else
		{
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: o/p port id = %d, num i/p ports = %d",
					me->mtMxID, pPayload->unSecOpCode, (int)pSessionMapNode->session_id, (int)pSessionMapNode->num_copps);
		}

		//Access the 1st COPP
		uint16_t *pCoppID = (uint16_t*) (pTmp2 + 1);

		if (ADM_MATRIX_ID_AUDIO_RX == matrix_id)
		{
			//Access the unInPortID and its pCurrentInPort.
			unInPortID = pSessionMapNode->session_id;
			if(MT_MX_MAX_INPUT_PORTS <= unInPortID)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on invalid i/p port id %lu. Exiting!",
						me->mtMxID, pPayload->unSecOpCode, unInPortID);
				result = ADSP_EUNEXPECTED;
				goto __bailoutCmdMapRoutings;
			}
			pCurrentInPort = me->inPortParams[unInPortID];

			//Check if this pCurrentInPort is valid
			if(NULL == pCurrentInPort)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on i/p port id %lu that has not been opened yet. Exiting!",
						me->mtMxID, pPayload->unSecOpCode, unInPortID);

				result = ADSP_EUNEXPECTED;
				goto __bailoutCmdMapRoutings;
			}

			//Defensive code against illegal command sequence from the driver: If this i/p port has not been opened, Error out.
			if(INPUT_PORT_STATE_INACTIVE == pCurrentInPort->inPortState)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on i/p port id %lu that has not been opened yet. Exiting!",
						me->mtMxID, pPayload->unSecOpCode, unInPortID);

				result = ADSP_EUNEXPECTED;
				goto __bailoutCmdMapRoutings;
			}

			(void)MtMx_ResetAvSyncSampleStats(me, unInPortID); //AV-Sync

			//if waiting for EoS ACK, clear & raise EoS V2
			if (pCurrentInPort->eosPendingAckMask)
			{
			   MtMx_ClearEoSMsg(me, unInPortID, TRUE);
			}

			//Reset some masks
			pCurrentInPort->strMask = 0;
			pCurrentInPort->activeOutPortsMask = 0;
			MtMx_UpdateInPortsMaskToReflectStrMaskChange(me, unInPortID, MT_MX_INPUT_PORT_STRMASK_CLEARED, 0);

			//Loop through the COPPs
			for (j = 0; j < unNumCopps; j++)
			{
				unOutPortID = *pCoppID++;
				if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on invalid o/p port id %lu. Exiting!",
							me->mtMxID, pPayload->unSecOpCode, unOutPortID);
					result = ADSP_EUNEXPECTED;
					goto __bailoutCmdMapRoutings;
				}
				pCurrentOutPort = me->outPortParams[unOutPortID];

				//Check if this pCurrentOutPort is valid
				if(NULL == pCurrentOutPort)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on o/p port id %lu that has not been opened yet. Exiting!",
							me->mtMxID, pPayload->unSecOpCode, unOutPortID);

					result = ADSP_EUNEXPECTED;
					goto __bailoutCmdMapRoutings;
				}
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: o/p port id = %lu", me->mtMxID, pPayload->unSecOpCode, unOutPortID);

				//Update some masks
				pCurrentInPort->strMask |= (1 << unOutPortID);
				pCurrentOutPort->inPortsMask |= (1 << unInPortID);
				pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);

				if(!MtMx_PspdSvcCreated(pCurrentPspd))
				{
					//PSPD thread needs to be created for this input-output pair.
					memset(&pCurrentPspd->thread_param, 0, sizeof(pCurrentPspd->thread_param));
					result = MtMx_CreatePspdSvc(&pCurrentPspd->thread_param, unInPortID, unOutPortID, &(pCurrentPspd->unPspdKpps), &(pCurrentPspd->unPspdBw));

					if (ADSP_EOK != result)
					{
						MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, created PSPD svc. failed, result = %d", me->mtMxID, unInPortID, unOutPortID, result);
						goto __bailoutCmdMapRoutings;
					}
					else
					{
						(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, created PSPD svc. success", me->mtMxID, unInPortID, unOutPortID);
					}
				}

				if(FALSE == MtMx_ChannelNativityIsEnabled(pCurrentOutPort->unNativeModeFlags))
				{
					MtMx_InOutPortsToCheckReInitPspdLibraries(me, unInPortID, unOutPortID, TRUE);
				}

				//If any connected o/p ports are in native mode, check if they need to re-init. If so, mark them for re-init.
				//Do this only when the current i/p port has a valid media type. Invalid media type at i/p should not trigger unncecssary reconfig of the native o/p ports.
				if(NULL != pCurrentInPort->pStartLoc)
				{
					MtMx_CheckIfAnyConnOutPortsInNativeModeNeedReInit(me, unInPortID);
				}

				//If there is no accumulation pending on a native mode o/p port and it is pending re-init, then re-initialize it right away.
				if((0 == pCurrentOutPort->accInPortsMask) &&
						(pCurrentOutPort->unNativeModeFlags) && (pCurrentOutPort->bIsOutPortInNativeModePendingReInit))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu MtMx_MapRoutings i/p port %lu calling Reinit on Native mode o/p port %lu because no acc. pending.",
							me->mtMxID, unInPortID, unOutPortID);
					if (ADSP_FAILED(result = MtMx_ReInitNativeModeOutport(me, unOutPortID)))
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_MapRoutings o/p port %lu MtMx_ReInitNativeModeOutport failed. Res = %d.", unOutPortID, result);
						goto __bailoutCmdMapRoutings;
					}
				}
			}

			//If unNumCopps is odd, there's a reserved field after the last valid COPP ID
			if (0 != (unNumCopps % 2)) pCoppID++;

			//Calculate this input port's top priority output port and update some masks
			MtMx_CalculateTopPriorityOutputPort(me, unInPortID);
			MtMx_InPortToUpdateItsActiveOutPortsMask(me, unInPortID);
			MtMx_InPortToUpdateWaitMask(me, unInPortID);

			//Check if PSPD libraries needs to be destroyed.
			MtMx_ClosingInPortToCheckDestroyPspdLibraries(me, unInPortID);

			//Now kill the PSPD threads. It is important that all the PSPD modules are destroyed prior to this.
			MtMx_ClosingInPortToCheckDestroyPspdThreads(me, unInPortID);			

		}
		else
		{
			//Access the unOutPortID and its pCurrentOutPort.
			unOutPortID = pSessionMapNode->session_id;
			if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on invalid o/p port id %lu. Exiting!",
						me->mtMxID, pPayload->unSecOpCode, unOutPortID);
				result = ADSP_EUNEXPECTED;
				goto __bailoutCmdMapRoutings;
			}
			pCurrentOutPort = me->outPortParams[unOutPortID];

			//Check if this pCurrentOutPort is valid
			if(NULL == pCurrentOutPort)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on o/p port id %lu that has not been opened yet. Exiting!",
						me->mtMxID, pPayload->unSecOpCode, unOutPortID);
				result = ADSP_EUNEXPECTED;
				goto __bailoutCmdMapRoutings;
			}

			//Defensive code against illegal command sequence from the driver: If this o/p port has not been opened, Error out.
			if(OUTPUT_PORT_STATE_INACTIVE == pCurrentOutPort->outPortState)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on o/p port id %lu that has not been opened yet. Exiting!",
						me->mtMxID, pPayload->unSecOpCode, unOutPortID);

				result = ADSP_EUNEXPECTED;
				goto __bailoutCmdMapRoutings;
			}

			//Reset some masks
			pCurrentOutPort->inPortsMask = 0;
			MtMx_UpdateStrMaskToReflectInPortsMaskChange(me, unOutPortID, MT_MX_OUTPUT_PORT_INPORTSMASK_CLEARED, 0);

			//Loop through the COPPs
			for (j = 0; j < unNumCopps; j++)
			{
				//Access the unInPortID and its pCurrentInPort.
				unInPortID = *pCoppID++;
				if(MT_MX_MAX_INPUT_PORTS <= unInPortID)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on invalid i/p port id %lu. Exiting!",
							me->mtMxID, pPayload->unSecOpCode, unInPortID);
					result = ADSP_EUNEXPECTED;
					goto __bailoutCmdMapRoutings;
				}
				pCurrentInPort = me->inPortParams[unInPortID];

				//Check if this pCurrentInPort is valid
				if(NULL == pCurrentInPort)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) called on i/p port id %lu that has not been opened yet. Exiting!",
							me->mtMxID, pPayload->unSecOpCode, unInPortID);
					result = ADSP_EUNEXPECTED;
					goto __bailoutCmdMapRoutings;
				}

				(void)MtMx_ResetAvSyncSampleStats(me, unInPortID); //AV-Sync

				//Update some masks and calculate top prio o/p port
				pCurrentOutPort->inPortsMask |= (1 << unInPortID);
				pCurrentOutPort->accBufAvailabilityMask |= (1 << unInPortID);
				pCurrentInPort->strMask |= (1 << unOutPortID);
				pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);

				if(!MtMx_PspdSvcCreated(pCurrentPspd))
				{
					//PSPD thread needs to be created for this input-output pair.
					memset(&pCurrentPspd->thread_param, 0, sizeof(pCurrentPspd->thread_param));
					result = MtMx_CreatePspdSvc(&pCurrentPspd->thread_param, unInPortID, unOutPortID, &(pCurrentPspd->unPspdKpps), &(pCurrentPspd->unPspdBw));

					if (ADSP_EOK != result)
					{
						MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, created PSPD svc. failed, result = %d", me->mtMxID, unInPortID, unOutPortID, result);
						goto __bailoutCmdMapRoutings;
					}
					else
					{
						(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, created PSPD svc. success", me->mtMxID, unInPortID, unOutPortID);
					}
				}

				MtMx_CalculateTopPriorityOutputPort(me, unInPortID);
				MtMx_InPortToUpdateItsActiveOutPortsMask(me, unInPortID);
				if (OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState)
				{
					pCurrentInPort->outputReqPendingMask |= (1 << unOutPortID);
				}
				MxAt_NewInPortToUpdateInPortsTopPriorityMaskOfOutPort(me, unInPortID, unOutPortID);
				MtMx_InPortToUpdateWaitMask(me, unInPortID);

				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu MtMx_MapRoutings: i/p port %lu o/p port %lu; ABAM: %lu, ORPM: %lu",
						me->mtMxID, unInPortID, unOutPortID, pCurrentOutPort->accBufAvailabilityMask, pCurrentInPort->outputReqPendingMask);

				//If any connected TX o/p ports are in native mode, check if they need to re-init. If so, mark them for re-init.
				//Do this only when the current i/p port has a valid media type. Invalid media type at i/p should not trigger unncecssary reconfig of the native o/p ports.
				if(NULL != pCurrentInPort->pStartLoc)
				{
					MtMx_CheckIfAnyConnOutPortsInNativeModeNeedReInit(me, unInPortID);
				}

				//If there is no accumulation pending on a native mode o/p port and it is pending re-init, then re-initialize it right away.
				if((0 == pCurrentOutPort->accInPortsMask) &&
						(pCurrentOutPort->unNativeModeFlags) && (pCurrentOutPort->bIsOutPortInNativeModePendingReInit))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu MtMx_MapRoutings i/p port %lu calling Reinit on Native mode o/p port %lu because no acc. pending.",
							me->mtMxID, unInPortID, unOutPortID);
					if (ADSP_FAILED(result = MtMx_ReInitNativeModeOutport(me, unOutPortID)))
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_MapRoutings o/p port %lu MxAt_ReInitNativeModeOutport failed. Res = %d.", unOutPortID, result);
						goto __bailoutCmdMapRoutings;
					}
				}

				if(FALSE == MtMx_ChannelNativityIsEnabled(pCurrentOutPort->unNativeModeFlags))
				{
					MtMx_InOutPortsToCheckReInitPspdLibraries(me, unInPortID, unOutPortID, TRUE);					
				}

				MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: Waitmask = %lu, ActiveOutPortsMask = %lu, SteadyStateInPortsMask = %lu, "
						"i/p port %lu Map Routing. Live? %d Burstmode? %d Burstmode? %d",
						me->mtMxID, pPayload->unSecOpCode, me->unCurrentBitfield, pCurrentInPort->activeOutPortsMask, me->steadyStateInPortsMask,
						unInPortID, (int)(pCurrentInPort->bIsPortLive), (int)(pCurrentInPort->bIsInPortBurstMode), (int)(me->bIsMxAtOperatingInBurstMode));

				if((TRUE == pCurrentInPort->bIsInPortBurstMode) && (TRUE == pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS))
				{
					//If this i/p port is operating in burst mode, then the o/p port does not support Absolute TS.
					//Currently, there is no support for Aud/Voc conncurrencies and VpTX
					pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS = FALSE;
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu is operating in burst mode. Therefore changing o/p port %lu to relative TS mode.",
							me->mtMxID, unInPortID, unOutPortID);
				}
			}

			//Check if PSPD libraries needs to be destroyed.
			MtMx_ClosingOutPortToCheckDestroyPspdLibraries(me, unOutPortID);

			//Now kill the PSPD threads. It is important that all the PSPD modules are destroyed prior to this.
			MtMx_ClosingOutPortToCheckDestroyPspdThreads(me, unOutPortID);

			//if unNumCopps is odd, there's a reserved field after the last valid COPP ID
			if (0 != (unNumCopps % 2)) pCoppID++;
		}

		//The next Session Map Node will begin from here.
		pTmp2 = (adm_session_map_node_v5_t*) pCoppID;
	}

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: checking for mixed latency connections",
			me->mtMxID, pPayload->unSecOpCode);

	// We must first detect if an input can transition away from ready before MtMx_ReInitInputPortBuffers.
	// On a device switch from a device with one rate to another device operating at another rate the input port
	// is put in the ready state. MtMx_ReInitInputPortBuffers will skip re-initing the input port intermediate buffer
	// size if the input port is in the ready state. When the device switch completes the input port will then move
	// to active, try to process data, and during gain & accumulation the intermediate buffer will be the wrong size
	// and could potentially cause memory corruption.
	// This situation  was observed during dev_adm_lb_mixedlat_8 test corrupting PP output buffers.
	if (ADM_MATRIX_ID_AUDIO_RX == matrix_id)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: checking if any existing i/p ports need to transition away from ready",
				me->mtMxID, pPayload->unSecOpCode);
		MtMx_DetectIfInputPortsNeedToTransitionAwayFromReady(me);
	}

	if (ADSP_FAILED(result = MtMx_DetectMixedLatencyConnections(me)))
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: checking for mixed latency connections failed with error code [%d]",
				me->mtMxID, pPayload->unSecOpCode,result);
		goto __bailoutCmdMapRoutings;
	}

	if (ADM_MATRIX_ID_AUDIO_RX == matrix_id)
	{
		//AV-Sync
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: setting the AFE drift pointer for each i/p port derived from its corresponding top prio port",
				me->mtMxID, pPayload->unSecOpCode);
		(void)MxAr_AVSyncSetAFEDriftPtrInputDerivedFromOutput(me);

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: checking for any opened pull output ports are waiting on resources and if so set OPRM on their connected i/p ports",
				me->mtMxID, pPayload->unSecOpCode);
		MtMx_SetOPRMaskOnInputPortsConnectedToPendingPullPorts(me);

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: checking for any newly connected pull ports to send data downstream",
				me->mtMxID, pPayload->unSecOpCode);
		MtMx_DetectIfPullPortsNeedsToSendData(me);

		// Scan the output ports to find connected input ports with valid timestamps
		MtMx_ScanOutputPortsForInputsWithValidTimestamps(me);
    
		//Set Limiter mode on output port if limiter instance is created  
		MtMx_SetLimiterModeOnOutputPorts(me);
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: checking for any existing non-live i/p ports need to mark pending acc on o/p",
				me->mtMxID, pPayload->unSecOpCode);
		MxAt_DetectIfPendingNonLivePortsNeedsToMarkPendingAccOnOp(me);

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (MapRoutings) handler: checking for any existing non-live i/p ports may be waiting to send their data downstream",
				me->mtMxID, pPayload->unSecOpCode);
		MxAt_DetectIfPendingNonLivePortsNeedsToSendData(me);
	}

	//Update matrix KPPS and/or BW and raise ADSPPM event if needed.
	if (ADSP_FAILED(result = MtMx_SetReqKppsAndBW(me)))
	{
		goto __bailoutCmdMapRoutings;
	}

	__bailoutCmdMapRoutings:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (MapRoutings) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

ADSPResult MtMx_RegisterUnderflowOverflowEvent(This_t *me, elite_msg_any_t *pMsg)
{
	ADSPResult              result = ADSP_EOK;
	uint32_t                unPort2RegForFlowEvent;
	EliteMsg_CustomMtMxUnderOverFlowEventType *pPayload = (EliteMsg_CustomMtMxUnderOverFlowEventType*)pMsg->pPayload;

	unPort2RegForFlowEvent = pPayload->unPortID;
	MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (RegUFOFEvt) [port ID, port dir, reg/unregister] = [%d, %d, %d]",
			me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID, (int)pPayload->unPortDirection, (int)pPayload->usEnable);

	if (ADSP_FAILED(result = MtMx_CheckPortDirectionValidity(me, pMsg, (EliteMsg_CustomMtMxPauseFlushType *)pPayload)) ||
			ADSP_FAILED(result = MtMx_CheckPortDirMatrixIdCompatibility(me, pMsg, (EliteMsg_CustomMtMxPauseFlushType *)pPayload)) ||
			ADSP_FAILED(result = MtMx_CheckPortIdValidity(me, pMsg, (EliteMsg_CustomMtMxPauseFlushType *)pPayload)))
	{
		goto __bailoutCmdRegisterUnderflowOverflowEvent;
	}

	if (MT_MX_REGISTER_UNDERFLOW_OVERFLOW_EVENT_ENABLE != pPayload->usEnable &&
			MT_MX_REGISTER_UNDERFLOW_OVERFLOW_EVENT_DISABLE != pPayload->usEnable)
	{
		result = ADSP_EBADPARAM;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RegUFOFEvt) handler: rcvd invalid reg/unregister value [%d]",
				me->mtMxID, pPayload->unSecOpCode, (int)pPayload->usEnable);
		goto __bailoutCmdRegisterUnderflowOverflowEvent;
	}

	if (MATRIX_MIXER_PORT_DIR_INPUT == pPayload->unPortDirection)
	{
		if (NULL == me->inPortParams[unPort2RegForFlowEvent])
		{
			result = ADSP_EUNEXPECTED;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RegUFOFEvt) handler: rcvd cmd on invalid port [%lu]",
					me->mtMxID, pPayload->unSecOpCode, unPort2RegForFlowEvent);
			goto __bailoutCmdRegisterUnderflowOverflowEvent;
		}
		//Enable underflow/overflow only if a valid call back function is registered
		if (NULL != (me->inPortParams[unPort2RegForFlowEvent]->eliteCb.pCbHandle))
		{
			me->inPortParams[unPort2RegForFlowEvent]->bIsRegisteredForUnderflowEvents = pPayload->usEnable;
		}
	}
	else
	{
		if (NULL == me->outPortParams[unPort2RegForFlowEvent])
		{
			result = ADSP_EUNEXPECTED;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RegUFOFEvt) handler: rcvd cmd on invalid port [%lu]",
					me->mtMxID, pPayload->unSecOpCode, unPort2RegForFlowEvent);
			goto __bailoutCmdRegisterUnderflowOverflowEvent;
		}
		//Enable underflow/overflow only if a valid call back function is registered
		if (NULL != (me->outPortParams[unPort2RegForFlowEvent]->eliteCb.pCbHandle))
		{
			me->outPortParams[unPort2RegForFlowEvent]->bIsRegisteredForOverflowEvents = pPayload->usEnable;
		}
	}

	__bailoutCmdRegisterUnderflowOverflowEvent:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (RegUFOFEvt) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}


ADSPResult MtMx_RampGains(This_t *me, elite_msg_any_t *pMsg)
{
	ADSPResult                       result = ADSP_EOK;
	uint32_t                         unInPortID, unOutPortID;
	uint16_t                         unNumGains, i;
	MatrixInPortInfoType             *pCurrentInPort;
	adm_cmd_matrix_ramp_gains_v5_t      *pRampGainsPayload;
	adm_session_copp_gain_v5_t          *pSessionCOPPGain;
	uint32_t                         strMask;

	EliteMsg_CustomMtMxRampGainsType *pPayload = (EliteMsg_CustomMtMxRampGainsType*)pMsg->pPayload;
	pRampGainsPayload = pPayload->pRampGainsPayload;
	unNumGains = pRampGainsPayload->num_gains;

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (RampGains) [num gains] = [%d]",
			me->mtMxID, pPayload->unSecOpCode, (int)pRampGainsPayload->num_gains);

	adm_session_copp_gain_v5_t *pTmp2 = (adm_session_copp_gain_v5_t*)(pRampGainsPayload + 1);

	if (ADM_MATRIX_ID_AUDIO_RX == pRampGainsPayload->matrix_id)
	{
		for (i = 0; i < unNumGains; i++)
		{
			pSessionCOPPGain = pTmp2;
			unInPortID = (uint32_t)pSessionCOPPGain->session_id;
			pCurrentInPort = me->inPortParams[unInPortID];
			if (NULL == pCurrentInPort)
			{
				result = ADSP_EUNEXPECTED;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler: rcvd cmd on invalid port [%lu]",
						me->mtMxID, pPayload->unSecOpCode, unInPortID);
				goto __bailoutCmdMtMxRampGains;
			}

			if (ADM_CMD_MATRIX_RAMP_GAINS_COPP_ID_ALL_CONNECTED_COPPS != pSessionCOPPGain->copp_id)
			{
				unOutPortID = (uint32_t)pSessionCOPPGain->copp_id;
				if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
				{
					result = ADSP_EUNEXPECTED;
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler: rcvd cmd on invalid port [%lu]",
							me->mtMxID, pPayload->unSecOpCode, unOutPortID);
					goto __bailoutCmdMtMxRampGains;
				}
				if (NULL == me->outPortParams[unOutPortID])
				{
					result = ADSP_EUNEXPECTED;
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler: rcvd cmd on invalid port [%lu]",
							me->mtMxID, pPayload->unSecOpCode, unOutPortID);
					goto __bailoutCmdMtMxRampGains;
				}
				if (me->outPortParams[unOutPortID]->inPortsMask & (1 << unInPortID))
				{
					MtMx_SetupInputToOutputGainParams(me, unInPortID, unOutPortID, pSessionCOPPGain);
				}
				else
				{
					result = ADSP_EUNEXPECTED;
					MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler:Error:rcvd cmd on not-connected I/p Port ID, O/p Port ID pair:[%lu, %lu]",
							me->mtMxID, pPayload->unSecOpCode, unInPortID, unOutPortID);
					goto __bailoutCmdMtMxRampGains;
				}
			}
			else
			{
				strMask = pCurrentInPort->strMask;
				if (strMask)
				{
					while (strMask)
					{
						unOutPortID = Q6_R_ct0_R(strMask);
						if (NULL == me->outPortParams[unOutPortID])
						{
							result = ADSP_EUNEXPECTED;
							MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler: rcvd cmd on invalid port [%lu]",
										me->mtMxID, pPayload->unSecOpCode, unOutPortID);
							goto __bailoutCmdMtMxRampGains;
						}
						strMask ^= (1 << unOutPortID);
						MtMx_SetupInputToOutputGainParams(me, unInPortID, unOutPortID, pSessionCOPPGain);
					}
				}
				else
				{
					result = ADSP_EUNEXPECTED;
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler:Error:rcvd cmd on not-connected Rx I/p port ID:%lu, to any O/p ports", 
									me->mtMxID, pPayload->unSecOpCode, unInPortID);
					goto __bailoutCmdMtMxRampGains;
				}
			}

			pTmp2++;
		}
	}
	else
	{
		for (i = 0; i < unNumGains; i++)
		{
			pSessionCOPPGain = pTmp2;
			unOutPortID = (uint32_t)pSessionCOPPGain->session_id;
			if (NULL == me->outPortParams[unOutPortID])
			{
				result = ADSP_EUNEXPECTED;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler: rcvd cmd on invalid port [%lu]",
						me->mtMxID, pPayload->unSecOpCode, unOutPortID);
				goto __bailoutCmdMtMxRampGains;
			}

			if (ADM_CMD_MATRIX_RAMP_GAINS_COPP_ID_ALL_CONNECTED_COPPS != pSessionCOPPGain->copp_id)
			{
				unInPortID = (uint32_t)pSessionCOPPGain->copp_id;
				if(MT_MXAT_MAX_INPUT_PORTS <= unInPortID)
				{
					result = ADSP_EUNEXPECTED;
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler: rcvd cmd on invalid port [%lu]",
							me->mtMxID, pPayload->unSecOpCode, unInPortID);
					goto __bailoutCmdMtMxRampGains;
				}
				if (NULL == me->inPortParams[unInPortID])
				{
					result = ADSP_EUNEXPECTED;
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler: rcvd cmd on invalid port [%lu]",
							me->mtMxID, pPayload->unSecOpCode, unInPortID);
					goto __bailoutCmdMtMxRampGains;
				}
				if (me->inPortParams[unInPortID]->strMask & (1 << unOutPortID))
				{
					MtMx_SetupInputToOutputGainParams(me, unInPortID, unOutPortID, pSessionCOPPGain);
				}
				else
				{
					result = ADSP_EUNEXPECTED;
					MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler:Error:rcvd cmd on not-connected I/p Port ID, O/p Port ID pair:[%lu, %lu]",
									me->mtMxID, pPayload->unSecOpCode, unInPortID, unOutPortID);
					goto __bailoutCmdMtMxRampGains;
				}
			}
			else
			{
				if(me->outPortParams[unOutPortID]->inPortsMask)
				{
					for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
					{
						pCurrentInPort = me->inPortParams[unInPortID];
						if (NULL != pCurrentInPort)
						{
							if (pCurrentInPort->strMask & (1 << unOutPortID))
							{
								MtMx_SetupInputToOutputGainParams(me, unInPortID, unOutPortID, pSessionCOPPGain);
							}
						}
					}
				}
				else
				{
					result = ADSP_EUNEXPECTED;
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (RampGains) handler:Error:rcvd cmd on not-connected Tx O/p Port ID:%lu, to any I/p ports",
              me->mtMxID, pPayload->unSecOpCode, unOutPortID);
					goto __bailoutCmdMtMxRampGains;
				}
			}

			pTmp2++;
		}
	}
	__bailoutCmdMtMxRampGains:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (RampGains) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}


void MtMx_RunOutputPort(This_t *me, uint32_t unPort2Run)
{
	int                     k, t;
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unPort2Run];

	k = qurt_elite_queue_get_channel_bit(pCurrentOutPort->bufQ);
	me->unBufBitfield = me->unBufBitfield | k;

	if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		MtMx_AddOutputPortToWaitMask(me, unPort2Run);
	}
	else
	{
		if((pCurrentOutPort->inPortsMask) && (pCurrentOutPort->inPortsWaitingToAccMask == pCurrentOutPort->inPortsMask))
		{
			MtMx_AddOutputPortToWaitMask(me, unPort2Run);
		}
	}
	t = 31 - (Q6_R_cl0_R( (k) ));
	me->outPortIDMap[t] = unPort2Run;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port #%lu: Moving to ACTIVE (Run) state", me->mtMxID, unPort2Run);
	pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_ACTIVE;

	//Update some masks
	MtMx_OutPortToUpdateActiveOutPortsMasks(me, unPort2Run);
	if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
	{
		MxAt_OutPortToUpdateAccBufAvailabilityMask(me, unPort2Run);
		MxAt_OutPortToUpdateOutputReqPendingMasks(me, unPort2Run);
	}
	MtMx_OutPortToUpdateWaitMask(me, unPort2Run);

	if(FALSE == pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS)
	{
		mt_mx_clear_time(&pCurrentOutPort->llSessionTime);
	}

	if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu (Run) handler: checking for any existing non-live i/p ports need to mark pending acc on o/p", me->mtMxID);
		MxAt_DetectIfPendingNonLivePortsNeedsToMarkPendingAccOnOp(me);

		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu (Run) handler: checking for any existing non-live i/p ports may be waiting to send their data downstream", me->mtMxID);
		MxAt_DetectIfPendingNonLivePortsNeedsToSendData(me);
	}
}

static ADSPResult MtMx_ChkReinitInportLocalBuf(This_t *me, uint32_t unInPortID,  MatrixInPortInfoType *pCurrentInPort)
{
	ADSPResult result = ADSP_EOK;

	if (MtMx_InportIsLocalBufFull(pCurrentInPort) || MtMx_InportIsLocalBufPartiallyFull(pCurrentInPort))
	{
		int8_t *pLocalPtr = pCurrentInPort->pWrLoc;
		uint32_t inport_buf_bytes_per_ch_size = pCurrentInPort->unInPortPerChBufSize * pCurrentInPort->unBytesPerSample;
		uint32_t unStaleDataInBytes = (pCurrentInPort->pStartLoc + inport_buf_bytes_per_ch_size) - pCurrentInPort->pWrLoc;

		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: cleaning %lu stale bytes in local buf", me->mtMxID, unInPortID, unStaleDataInBytes);

		for (uint32_t j = 0; j < pCurrentInPort->unNumChannels ; j++)
		{
			memset(pLocalPtr, 0, unStaleDataInBytes);
			pLocalPtr += inport_buf_bytes_per_ch_size;
		}

		if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			if ((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port: %lu Triggering accumulation of partially filled local buffer after media_type was rcvd", me->mtMxID, unInPortID);
				MxAr_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
				MxAr_ProcessPendingPullOutPorts(me);
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu pending MtMx_ReInitLocalBuffer of i/p port %lu", me->mtMxID, unInPortID);
				result = MtMx_ReInitLocalBuffer(me, unInPortID);
				if (ADSP_EOK != result)
				{
					return result;
				}
			}
			else
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port: %lu Setting bIsAccumulationPending = 1 after media_type was rcvd", me->mtMxID, unInPortID);
				pCurrentInPort->bIsAccumulationPending = TRUE;
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu pending MtMx_ReInitLocalBuffer of i/p port %lu", me->mtMxID, unInPortID);
				pCurrentInPort->bShouldLocalBufBeReInit = TRUE;
				MtMx_RemoveInputPortFromWaitMask(me,unInPortID);
			}
		}
		else
		{
			if((((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask)) && (FALSE == pCurrentInPort->bIsPortLive))
					||(TRUE == pCurrentInPort->bIsPortLive))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port: %lu Triggering accumulation of partially filled local buffer after media_type was rcvd", me->mtMxID, unInPortID);
				MxAt_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu pending MtMx_ReInitLocalBuffer of i/p port %lu", me->mtMxID, unInPortID);
				result = MtMx_ReInitLocalBuffer(me, unInPortID);
				if (ADSP_EOK != result)
				{
					return result;
				}
			}
			else
			{
				if(FALSE == pCurrentInPort->bIsPortLive)
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port: %lu Setting bIsAccumulationPending = 1 after media_type was rcvd", me->mtMxID, unInPortID);
					pCurrentInPort->bIsAccumulationPending = TRUE;
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu pending MtMx_ReInitLocalBuffer of i/p port %lu", me->mtMxID, unInPortID);
					pCurrentInPort->bShouldLocalBufBeReInit = TRUE;
					MtMx_RemoveInputPortFromWaitMask(me,unInPortID);
				}
			}
		}
	}
	else
	{
		if (ADSP_FAILED(result = MtMx_ReInitLocalBuffer(me, unInPortID)))
		{
			return result;
		}
	}
	return ADSP_EOK;
}

static ADSPResult MxAr_Pause(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseType *pPayload)
{
	uint32_t                unPort2Pause = pPayload->unPortID;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unPort2Pause];

	if (NULL == pCurrentInPort)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: rcvd cmd on invalid port [%ld]",
				me->mtMxID, pPayload->unSecOpCode, unPort2Pause);
		return ADSP_EUNEXPECTED;
	}

	switch (pCurrentInPort->inPortState)
	{
	case INPUT_PORT_STATE_INIT:
	case INPUT_PORT_STATE_WAITING_TO_BE_READY:
	case INPUT_PORT_STATE_READY:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: rcvd cmd in %ld state, returning success",
				me->mtMxID, pPayload->unSecOpCode, pCurrentInPort->inPortState);
		return ADSP_EOK;
	}
	case INPUT_PORT_STATE_PAUSED:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: rcvd cmd in paused state [%ld], discarding",
				me->mtMxID, pPayload->unSecOpCode, pCurrentInPort->inPortState);
		return ADSP_EOK;
	}
	case INPUT_PORT_STATE_ACTIVE:
	case INPUT_PORT_STATE_WAITING_TO_BE_ACTIVE:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: rcvd cmd in Active/Waiting Active state [%ld]",
				me->mtMxID, pPayload->unSecOpCode, pCurrentInPort->inPortState);
		pCurrentInPort->inPortState = INPUT_PORT_STATE_PAUSED;
		pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_PAUSED;
		return ADSP_EOK;
	}
	case INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER:
	{
		ADSPResult result = ADSP_EOK;

		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: rcvd cmd in ACTIVE_HOLDING_INPUT_BUFFER state [%ld]. Flushing i/p buffers and i/p Q",
				me->mtMxID, pPayload->unSecOpCode, pCurrentInPort->inPortState);
		result = MxArFlushInputQRetainSessionClock(me, unPort2Pause);
		if (ADSP_EOK != result)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: Exiting with error [%d]",
					me->mtMxID, pPayload->unSecOpCode, result);
			return result;
		}

		//Successful flush of i/p buffers and i/p Q
		pCurrentInPort->inPortState = INPUT_PORT_STATE_PAUSED;
		pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_PAUSED;
		return ADSP_EOK;
	}
	default:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: rcvd cmd in unexpected state [%ld], discarding",
				me->mtMxID, pPayload->unSecOpCode, pCurrentInPort->inPortState);
		return ADSP_EUNEXPECTED;
	}
	}
}

static ADSPResult MxAt_Pause(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseType *pPayload)
{
	uint32_t unPort2Pause = pPayload->unPortID;
	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unPort2Pause];

	if (NULL == pCurrentOutPort)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: rcvd cmd on invalid port [%ld]",
				me->mtMxID, pPayload->unSecOpCode, unPort2Pause);
		return ADSP_EUNEXPECTED;
	}

	switch (pCurrentOutPort->outPortState)
	{
	case OUTPUT_PORT_STATE_INIT:
	case OUTPUT_PORT_STATE_ACTIVE:
	case OUTPUT_PORT_STATE_WAITING_TO_BE_ACTIVE:
	{
		pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_PAUSED;
		MtMx_RemoveOutputPortFromWaitMask(me, unPort2Pause);
		MtMx_OutPortToUpdateActiveOutPortsMasks(me, unPort2Pause);
		MxAt_OutPortToUpdateOutputReqPendingMasks(me, unPort2Pause);
		MtMx_OutPortToUpdateWaitMask(me, unPort2Pause);
		return ADSP_EOK;
	}
	case OUTPUT_PORT_STATE_PAUSED:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: rcvd cmd in paused state [%d], discarding",
				me->mtMxID, pPayload->unSecOpCode, pCurrentOutPort->outPortState);
		return ADSP_EOK;
	}
	default:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Pause) handler: rcvd cmd in unexpected state [%d], discarding",
				me->mtMxID, pPayload->unSecOpCode, pCurrentOutPort->outPortState);
		return ADSP_EUNEXPECTED;
	}
	}
}

static ADSPResult MxAr_Suspend(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxSuspendType *pPayload)
{
	uint32_t                unInPortID = pPayload->unPortID;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	ADSPResult result = ADSP_EOK;

	switch (pCurrentInPort->inPortState)
	{
	case INPUT_PORT_STATE_INIT:
	case INPUT_PORT_STATE_WAITING_TO_BE_READY:
	case INPUT_PORT_STATE_READY:
	case INPUT_PORT_STATE_PAUSED:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Suspend) handler: rcvd cmd in %ld state", me->mtMxID, pPayload->unSecOpCode, pCurrentInPort->inPortState);
		result = MtMx_AvSyncReleaseHW(me, unInPortID); //AV-Sync
		return result;
	}
	case INPUT_PORT_STATE_ACTIVE:
	case INPUT_PORT_STATE_WAITING_TO_BE_ACTIVE:
	case INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER:
	default:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Suspend) handler: rcvd cmd in unexpected state [%ld]", me->mtMxID, pPayload->unSecOpCode, pCurrentInPort->inPortState);
		result = ADSP_EUNEXPECTED;
		return result;
	}
	}
}

static ADSPResult MxAt_Suspend(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxSuspendType *pPayload)
{
	uint32_t unOutPortID = pPayload->unPortID;
	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];
	ADSPResult result = ADSP_EOK;

	switch (pCurrentOutPort->outPortState)
	{
	case OUTPUT_PORT_STATE_INIT:
	case OUTPUT_PORT_STATE_PAUSED:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Suspend) handler: rcvd cmd in %d state",
				me->mtMxID, pPayload->unSecOpCode, pCurrentOutPort->outPortState);

		if (NULL != pCurrentOutPort->avt_drv_handle)
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Releasing AVTimer Driver for o/p port %lu",me->mtMxID, unOutPortID);
			if (ADSP_EOK != (result = avtimer_drv_hw_close(pCurrentOutPort->avt_drv_handle)))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to close AVTimer Driver for o/p port %lu with result: %d", unOutPortID, result);
			}
			pCurrentOutPort->avt_drv_handle = NULL;
		}

		return result;
	}
	case OUTPUT_PORT_STATE_ACTIVE:
	case OUTPUT_PORT_STATE_WAITING_TO_BE_ACTIVE:
	default:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Suspend) handler: rcvd cmd in unexpected state [%d]",
				me->mtMxID, pPayload->unSecOpCode, pCurrentOutPort->outPortState);
		result = ADSP_EUNEXPECTED;
		return result;
	}
	}
}

static ADSPResult MxAr_Flush(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxFlushType *pPayload)
{
	ADSPResult              result = ADSP_EOK;
	uint32_t                unPort2Flush = pPayload->unPortID;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unPort2Flush];
	if (NULL == pCurrentInPort)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: rcvd cmd on invalid port [%lu]",
				me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
		return ADSP_EUNEXPECTED;
	}

	elite_msg_any_t         myDataQMsg;



	// Reset the i/p port's session time to 0 (Flush should clear out the session clock)
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu flush handler: i/p port %lu reseting SessionTime to 0.", me->mtMxID, unPort2Flush);
	MtMx_ResetSessionTimeClock(me, unPort2Flush);

	//after flush EoS ack will be dropped. client will not get rendered eos event.
   pCurrentInPort->eosPendingAckMask = 0;
   MtMx_ClearEoSMsg(me, unPort2Flush, TRUE);

	// mark local buffer empty and reset local buffer pointers
	pCurrentInPort->bIsLocalBufEmpty = TRUE;
	pCurrentInPort->bIsLocalBufFull = FALSE;
	pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;
	MtMx_InitInPortLocalBufPtrs(me, unPort2Flush);
	
	//Reset the sampleslip library, if present.
	//This is needed to Flush out the internal data in sample slip library which is invoked in ASM loopback. The sample slip module has 1ms worth
	//of pre-buffer which, if not FLUSHed, gets played when the data resumes on this input port
	(void)MtMx_InPortResetSampleSlipLibrary(me, unPort2Flush);
	
	// check if any input buffer is held, if so, release it.
	if (INPUT_BUFFER_HELD == pCurrentInPort->bInBufStatus)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: i/p port %lu releasing an i/p buffer it was holding on to",
				me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
		myDataQMsg = pCurrentInPort->myDataQMsg;
		if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&myDataQMsg)))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: i/p port %lu failed to return buffer while flushing",
					me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
			return result;
		}
		pCurrentInPort->numBufReturned++;
		pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
	}

	// check for any i/p buffers in the dataQ.
	qurt_elite_channel_t *pChannel = qurt_elite_queue_get_channel(pCurrentInPort->inPortHandle.portHandle.dataQ);
	elite_msg_any_t nonBufMsgs[MAX_DATA_Q_ELEMENTS];
	uint32_t numNonBufMsgs = 0;

	while (qurt_elite_channel_poll(pChannel, qurt_elite_queue_get_channel_bit(pCurrentInPort->inPortHandle.portHandle.dataQ)))
	{
		// take next buffer off the q
		if (ADSP_FAILED(result = qurt_elite_queue_pop_front(pCurrentInPort->inPortHandle.portHandle.dataQ, (uint64_t*)&myDataQMsg)))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: i/p port %lu failed to pop buffer off its dataQ",
					me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
			return result;
		}

		pCurrentInPort->myDataQMsg = myDataQMsg;

		if(myDataQMsg.unOpCode == ELITE_DATA_MARK_BUFFER)
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx flushing Marked Buffer");
			MtMx_ProcessMarkBuffer(me, unPort2Flush, pCurrentInPort->myDataQMsg, ASM_DATA_EVENT_MARK_BUFFER_DISCARDED);
			continue;
		}


		if(ELITE_DATA_BUFFER != pCurrentInPort->myDataQMsg.unOpCode)
		{
			//Retain non data buffer messages during Flush.
			if (numNonBufMsgs < MAX_DATA_Q_ELEMENTS)
			{
				nonBufMsgs[numNonBufMsgs++] = myDataQMsg;
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: i/p port %lu retaining non data buffer message",
						me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
			}
			else
			{
				// release the msg
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: i/p port %lu releasing a msg that was in dataQ since the internal array is full.",
						me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
				if (ADSP_FAILED(result = elite_msg_release_msg(&myDataQMsg)))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: i/p port %lu failed to return msg while flushing",
							me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
					return result;
				}
			}
		}
		else
		{
			// release the input buffer to its queue.
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: i/p port %lu releasing an i/p buffer that was in dataQ",
					me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
			if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&myDataQMsg)))
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: i/p port %lu failed to return buffer while flushing",
						me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
				return result;
			}
		}
	}

	// Now restore the non data buffer messages
	result = ADSP_EOK;
	for (uint32_t i = 0; i < numNonBufMsgs; i++)
	{
		ADSPResult errCode = qurt_elite_queue_push_back(pCurrentInPort->inPortHandle.portHandle.dataQ, (uint64_t*)&(nonBufMsgs[i]));
		if (ADSP_FAILED(errCode))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: i/p port %lu error pushing non buffer msg back onto dataQ",
					me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
			result = errCode;
			errCode = elite_msg_release_msg(&nonBufMsgs[i]);
			if (ADSP_FAILED(errCode))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: Failed to release data Q message.", me->mtMxID);
			}
		}
	}
	pCurrentInPort->unDataLogId++;

	return result;
}

static ADSPResult MxAt_Flush(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxFlushType *pPayload)
{
	uint32_t                unPort2Flush = pPayload->unPortID;
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unPort2Flush];
	if (NULL == pCurrentOutPort)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Flush) handler: rcvd cmd on invalid port [%lu]",
				me->mtMxID, pPayload->unSecOpCode, unPort2Flush);
		return ADSP_EUNEXPECTED;
	}

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu flush handler: o/p port %lu", me->mtMxID, unPort2Flush);

	// clear the output port's accBuf
	memset(pCurrentOutPort->pAccBuf, 0, pCurrentOutPort->unBufSize * sizeof(int32_t));

	// reset session time to -pCurrentOutPort->unOutPortPerChBufSize (for relative TS mode)
	if(FALSE == pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS)
	{
		mt_mx_decrement_time(&pCurrentOutPort->llSessionTime, pCurrentOutPort->unFrameDurationInUsec);
	}

	return ADSP_EOK;
}

static ADSPResult MxAr_PortStateChange(This_t *me, EliteMsg_CustomMtMxPortStateChange *pPayload)
{
	ADSPResult              result = ADSP_EOK;
	uint32_t                unPortID = pPayload->unPortID;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unPortID];
	if (NULL == pCurrentInPort)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: rcvd cmd on invalid port [%lu]",
				me->mtMxID, pPayload->unSecOpCode, unPortID);
		return ADSP_EUNEXPECTED;
	}
	elite_msg_any_t         myDataQMsg;


	if (!((INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState)
			|| (INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER == pCurrentInPort->inPortState)
			|| (INPUT_PORT_STATE_PAUSED == pCurrentInPort->inPortState)))
	{
		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu received cmd in Non-Active or paused state: %ld",
				me->mtMxID, pPayload->unSecOpCode, unPortID, pCurrentInPort->inPortState);

		//Store the current i/p port state prior to transitioning it to READY.
		pCurrentInPort->inPortStatePrToStateChange = pCurrentInPort->inPortState;
		pCurrentInPort->inPortState = INPUT_PORT_STATE_READY;
		return ADSP_EOK;
	}


	// mark local buffer empty and reset local buffer pointers
	pCurrentInPort->bIsLocalBufEmpty = TRUE;
	pCurrentInPort->bIsLocalBufFull = FALSE;
	pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;
	MtMx_InitInPortLocalBufPtrs(me, unPortID);

	// check if any input buffer is held.
	if (INPUT_BUFFER_HELD == pCurrentInPort->bInBufStatus)
	{
		// release the input buffer to its queue.
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu releasing an i/p buffer it was holding on to",
				me->mtMxID, pPayload->unSecOpCode, unPortID);

		myDataQMsg = pCurrentInPort->myDataQMsg;
		if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&myDataQMsg)))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu failed to return buffer",
					me->mtMxID, pPayload->unSecOpCode, unPortID);
			return result;
		}
		pCurrentInPort->numBufReturned++;
		pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
	}

	if(INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER == pCurrentInPort->inPortState)
	{
		//I/p port transition back to ACTIVE
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu i/p port %lu forcibly transitioned from Held state back to Active state", me->mtMxID, unPortID);
		pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE;
		pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE;
		MtMx_ClearHoldDuration(me, unPortID); //AV-Sync
		MtMx_InPortToUpdateWaitMask(me, unPortID);
	}

	// check for any i/p buffers in the dataQ.
	qurt_elite_channel_t *pChannel = qurt_elite_queue_get_channel(pCurrentInPort->inPortHandle.portHandle.dataQ);

	while (qurt_elite_channel_poll(pChannel, qurt_elite_queue_get_channel_bit(pCurrentInPort->inPortHandle.portHandle.dataQ)))
	{
		// take next buffer off the q
		if (ADSP_FAILED(result = qurt_elite_queue_pop_front(pCurrentInPort->inPortHandle.portHandle.dataQ, (uint64_t*)&myDataQMsg)))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu failed to pop buffer off its dataQ",
					me->mtMxID, pPayload->unSecOpCode, unPortID);
			return result;
		}

		pCurrentInPort->myDataQMsg = myDataQMsg;

		if(myDataQMsg.unOpCode == ELITE_DATA_MARK_BUFFER)
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx flushing Marked Buffer");
			MtMx_ProcessMarkBuffer(me, unPortID, pCurrentInPort->myDataQMsg, ASM_DATA_EVENT_MARK_BUFFER_DISCARDED);
			continue;
		}

		if(ELITE_DATA_BUFFER != pCurrentInPort->myDataQMsg.unOpCode)
		{
			// release the msg
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu releasing a non-data buffer msg that was in dataQ",
					me->mtMxID, pPayload->unSecOpCode, unPortID);
			if (ADSP_FAILED(result = elite_msg_release_msg(&myDataQMsg)))
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu failed to return non-data buffer msg while port state change",
						me->mtMxID, pPayload->unSecOpCode, unPortID);
				return result;
			}
		}
		else
		{
			//Pop and store data buffer messages during Port State Change.
			if (pCurrentInPort->unNumMsgsToReturn < MAX_DATA_Q_ELEMENTS)
			{
				pCurrentInPort->msgsToReturn[pCurrentInPort->unNumMsgsToReturn++] = myDataQMsg;
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu retaining data buffer msg",
						me->mtMxID, pPayload->unSecOpCode, unPortID);
			}
			else
			{
				// If there is no room, release the data buffer msg
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu releasing a data buffer msg that was in dataQ since the internal array is full",
						me->mtMxID, pPayload->unSecOpCode, unPortID);
				if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&myDataQMsg)))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu failed to return msg while port state change",
							me->mtMxID, pPayload->unSecOpCode, unPortID);
					return result;
				}
			}
		}
	}

	//Store the current i/p port state prior to transitioning it to READY.
	pCurrentInPort->inPortStatePrToStateChange = pCurrentInPort->inPortState;
	pCurrentInPort->inPortState = INPUT_PORT_STATE_READY;
	MtMx_AddInputPortToWaitMask(me, unPortID);

	if (pCurrentInPort->unNumMsgsToReturn > 0)
	{
		pCurrentInPort->unTargetSampleRate = pPayload->unTargetSampleRate;
		MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (PortStateChange) handler: i/p port %lu has #%d msgs to return. Tgt SR: %lu",
				me->mtMxID, pPayload->unSecOpCode, unPortID, pCurrentInPort->unNumMsgsToReturn, pCurrentInPort->unTargetSampleRate);
		pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
		MtMx_SetupDefaultRenderWindow(me, unPortID); //AV-Sync
	}

	return ADSP_EOK;
}



static ADSPResult MtMx_CheckPortDirMatrixIdCompatibility(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseFlushType *pPayload)
{
	if ((ADM_MATRIX_ID_AUDIO_TX == me->mtMxID && MATRIX_MIXER_PORT_DIR_INPUT == pPayload->unPortDirection) ||
			(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID && MATRIX_MIXER_PORT_DIR_OUTPUT == pPayload->unPortDirection))
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] handler: rcvd incorrect port dir [%d] for this audio matrix",
				me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortDirection);
		return ADSP_EBADPARAM;
	}
	return ADSP_EOK;
}
static ADSPResult MtMx_CheckPortIdValidity(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseFlushType *pPayload)
{
	uint32_t    unPortID = pPayload->unPortID;
	uint32_t    unMaxPortID, unPortInactiveState, unPortState;

	if (MATRIX_MIXER_PORT_DIR_INPUT == pPayload->unPortDirection)
	{
		unMaxPortID = me->maxInPortID;
		unPortInactiveState = INPUT_PORT_STATE_INACTIVE;
		if (NULL == me->inPortParams[unPortID])
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] handler: rcvd invalid port ID [%d]",
					me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID);
			return ADSP_EBADPARAM;
		}
		unPortState = me->inPortParams[unPortID]->inPortState;
	}
	else
	{
		unMaxPortID = me->maxOutPortID;
		unPortInactiveState = OUTPUT_PORT_STATE_INACTIVE;
		if (NULL == me->outPortParams[unPortID])
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] handler: rcvd invalid port ID [%d]",
					me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID);
			return ADSP_EBADPARAM;
		}
		unPortState = me->outPortParams[unPortID]->outPortState;
	}

	if (unMaxPortID < unPortID || unPortInactiveState == unPortState)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] handler: rcvd invalid port ID [%d]",
				me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID);
		return ADSP_EBADPARAM;
	}
	return ADSP_EOK;
}

static ADSPResult MtMx_CheckPortDirectionValidity(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxPauseFlushType *pPayload)
{
	if (MATRIX_MIXER_PORT_DIR_INPUT != pPayload->unPortDirection && MATRIX_MIXER_PORT_DIR_OUTPUT != pPayload->unPortDirection)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] handler: rcvd invalid port direction [%d]",
				me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortDirection);
		return ADSP_EBADPARAM;
	}
	return ADSP_EOK;
}

ADSPResult MtMx_Mute(This_t *me, elite_msg_any_t *pMsg)
{
	adm_cmd_matrix_mute_v5_t             *pMutePayload;
	ADSPResult                       result = ADSP_EOK;
	uint32_t                         unInPortID, unOutPortID;
	MatrixInPortInfoType              *pCurrentInPort;
	uint32_t                         strMask;

	EliteMsg_CustomMtMxMuteType *pPayload = (EliteMsg_CustomMtMxMuteType*)pMsg->pPayload;
	pMutePayload = pPayload->pMutePayload;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (Mute)", me->mtMxID, pPayload->unSecOpCode);

	if (ADM_MATRIX_ID_AUDIO_RX == pMutePayload->matrix_id)
	{
		unInPortID = (uint32_t)pMutePayload->session_id;
		pCurrentInPort = me->inPortParams[unInPortID];
		if (NULL == pCurrentInPort)
		{
			result = ADSP_EUNEXPECTED;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler: rcvd cmd on invalid port [%lu]",
					me->mtMxID, pPayload->unSecOpCode, unInPortID);
			goto __bailoutCmdMtMxMute;
		}

		if (ADM_CMD_MATRIX_MUTE_COPP_ID_ALL_CONNECTED_COPPS != pMutePayload->copp_id)
		{
			unOutPortID = (uint32_t)pMutePayload->copp_id;
			if(MT_MX_MAX_OUTPUT_PORTS <= unOutPortID)
			{
				result = ADSP_EUNEXPECTED;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler: rcvd cmd on invalid port [%lu]",
						me->mtMxID, pPayload->unSecOpCode, unOutPortID);
				goto __bailoutCmdMtMxMute;
			}
			if (NULL == me->outPortParams[unOutPortID])
			{
				result = ADSP_EUNEXPECTED;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler: rcvd cmd on invalid port [%lu]",
						me->mtMxID, pPayload->unSecOpCode, unOutPortID);
				goto __bailoutCmdMtMxMute;
			}
			if (me->outPortParams[unOutPortID]->inPortsMask & (1 << unInPortID))
			{
				MtMx_SetupInputToOutputMuteParams(me, unInPortID, unOutPortID, pMutePayload);
			}
			else
			{
				result = ADSP_EUNEXPECTED;
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler:Error:rcvd cmd on not-connected I/p Port ID, O/p Port ID pair:[%lu, %lu]",
						me->mtMxID, pPayload->unSecOpCode, unInPortID, unOutPortID);
				goto __bailoutCmdMtMxMute;
			}
		}
		else
		{
			strMask = pCurrentInPort->strMask;
			if (strMask)
			{
				while (strMask)
				{
					unOutPortID = Q6_R_ct0_R(strMask);
					if (NULL == me->outPortParams[unOutPortID])
					{
						result = ADSP_EUNEXPECTED;
						MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler: rcvd cmd on invalid port [%lu]",
										me->mtMxID, pPayload->unSecOpCode, unOutPortID);
						goto __bailoutCmdMtMxMute;
					}
					strMask ^= (1 << unOutPortID);
					MtMx_SetupInputToOutputMuteParams(me, unInPortID, unOutPortID, pMutePayload);
				}
			}
			else
			{
				result = ADSP_EUNEXPECTED;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler:Error:rcvd cmd on not-connected Rx I/p port ID:%lu, to any O/p ports",
						me->mtMxID, pPayload->unSecOpCode, unInPortID);
				goto __bailoutCmdMtMxMute;
			}
      
		}
	}
	else
	{
		unOutPortID = (uint32_t)pMutePayload->session_id;
		if (NULL == me->outPortParams[unOutPortID])
		{
			result = ADSP_EUNEXPECTED;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler: rcvd cmd on invalid port [%lu]",
					me->mtMxID, pPayload->unSecOpCode, unOutPortID);
			goto __bailoutCmdMtMxMute;
		}

		if (ADM_CMD_MATRIX_RAMP_GAINS_COPP_ID_ALL_CONNECTED_COPPS != pMutePayload->copp_id)
		{
			unInPortID = (uint32_t)pMutePayload->copp_id;
			if(MT_MXAT_MAX_INPUT_PORTS <= unInPortID)
			{
				result = ADSP_EUNEXPECTED;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler: rcvd cmd on invalid port [%lu]",
						me->mtMxID, pPayload->unSecOpCode, unInPortID);
				goto __bailoutCmdMtMxMute;
			}
			if (NULL == me->inPortParams[unInPortID])
			{
				result = ADSP_EUNEXPECTED;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler: rcvd cmd on invalid port [%lu]",
						me->mtMxID, pPayload->unSecOpCode, unInPortID);
				goto __bailoutCmdMtMxMute;
			}
			if (me->inPortParams[unInPortID]->strMask & (1 << unOutPortID))
			{
				MtMx_SetupInputToOutputMuteParams(me, unInPortID, unOutPortID, pMutePayload);
			}
			else
			{
				result = ADSP_EUNEXPECTED;
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler:Error:rcvd cmd on not-connected I/p Port ID, O/p Port ID pair:[%lu, %lu]",
						me->mtMxID, pPayload->unSecOpCode, unInPortID, unOutPortID);
				goto __bailoutCmdMtMxMute;
			}
		}
		else
		{
			if(me->outPortParams[unOutPortID]->inPortsMask)
			{
				for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
				{
					pCurrentInPort = me->inPortParams[unInPortID];
					if (NULL != pCurrentInPort)
					{
						if (pCurrentInPort->strMask & (1 << unOutPortID))
						{
							MtMx_SetupInputToOutputMuteParams(me, unInPortID, unOutPortID, pMutePayload);
						}
					}
				}
			}
			else
			{
				result = ADSP_EUNEXPECTED;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (Mute) handler:Error:rcvd cmd on not-connected Tx O/p Port ID:%lu, to any I/p ports",
						me->mtMxID, pPayload->unSecOpCode, unOutPortID);
				goto __bailoutCmdMtMxMute;
			}
		}
	}
	__bailoutCmdMtMxMute:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (Mute) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

static ADSPResult MtMx_ValidateMultiChMediaFormatParam(This_t *me,
		uint32_t unInPortID,
		elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk,
		uint32_t unOpCode)
{
	ADSPResult result=ADSP_EOK;

	if (pMediaFormatBlk->is_interleaved)
	{
		MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu rcvd intrlvd flag set [%d] in msg %lu (MediaType)",
				me->mtMxID, unInPortID, (int)pMediaFormatBlk->is_interleaved, unOpCode);
		result = ADSP_EBADPARAM;
	}

	if (pMediaFormatBlk->bits_per_sample < 16)
	{
		MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu rcvd invalid bits-per-sample value [%d] in msg %lu (MediaType)",
				me->mtMxID, unInPortID, (int)pMediaFormatBlk->bits_per_sample, unOpCode);
		result = ADSP_EBADPARAM;
	}

	if ((pMediaFormatBlk->num_channels > MT_MX_NUM_CHANNELS_EIGHT) || (pMediaFormatBlk->num_channels < MT_MX_NUM_CHANNELS_ONE))
	{
		MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu rcvd invalid num. of ch. value [%d] in msg %lu (MediaType)",
				me->mtMxID, unInPortID, (int)pMediaFormatBlk->num_channels, unOpCode);

		result = ADSP_EBADPARAM;
	}

	return result;
}

//Audio matrix SetParam handler.
ADSPResult MtMx_SetParamHandler(This_t *me, elite_msg_any_t *pMsg)
{
	ADSPResult                          result = ADSP_EOK;
	EliteMsg_CustomMtMxGetSetParamType*  pPayload = (EliteMsg_CustomMtMxGetSetParamType* )pMsg->pPayload;

	uint32_t unPort2SetParam = pPayload->unPortID;
	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (SetParam) [port ID] = [%d, %d]",
			me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID, (int)pPayload->unPortDirection);

	if(ELITEMSG_PARAM_ID_CAL != pPayload->unParamId)
	{
		result = ADSP_EBADPARAM;
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetParam) handler: Only ELITEMSG_PARAM_ID_CAL supported",
				me->mtMxID, pPayload->unSecOpCode);
		goto __bailoutCmdSetParamHandler;
	}

	if (MATRIX_MIXER_PORT_DIR_INPUT == pPayload->unPortDirection)
	{
		MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unPort2SetParam];

		//Port ID validity check
		if ((NULL == pCurrentInPort) || (me->maxInPortID < unPort2SetParam || INPUT_PORT_STATE_INACTIVE == pCurrentInPort->inPortState))
		{
			result = ADSP_EBADPARAM;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetParam) handler: rcvd invalid/inactive port ID [%d]",
					me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID);
			goto __bailoutCmdSetParamHandler;
		}

		//Route to the appropriate handler
		if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			result = MxAr_InPortSetParamHandler(me, pMsg, pPayload,  unPort2SetParam, pCurrentInPort);
		}
		else //if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
		{
			result = MxAt_InPortSetParamHandler(me, pMsg, pPayload,  unPort2SetParam, pCurrentInPort);
		}
	}
	else if (MATRIX_MIXER_PORT_DIR_OUTPUT == pPayload->unPortDirection)
	{
		MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unPort2SetParam];

		//Port ID validity check
		if ((NULL == pCurrentOutPort) || (me->maxOutPortID < unPort2SetParam || OUTPUT_PORT_STATE_INACTIVE == pCurrentOutPort->outPortState))
		{
			result = ADSP_EBADPARAM;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetParam) handler: rcvd invalid/inactive port ID [%d]",
					me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID);
			goto __bailoutCmdSetParamHandler;
		}

		//Route to the appropriate handler
		if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			result = MxAr_OutPortSetParamHandler(me, pMsg, pPayload,  unPort2SetParam, pCurrentOutPort);
		}
		else //if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
		{
			result = MxAt_OutPortSetParamHandler(me, pMsg, pPayload,  unPort2SetParam, pCurrentOutPort);
		}
	}
	else
	{
		result = ADSP_EBADPARAM;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetParam) handler: rcvd invalid port direction[%d]",
				me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortDirection);
	}

	__bailoutCmdSetParamHandler:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (SetParam) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

ADSPResult MtMx_GetParamHandler(This_t *me, elite_msg_any_t *pMsg)
{
	ADSPResult                          result = ADSP_EOK;
	EliteMsg_CustomMtMxGetSetParamType*  pPayload = (EliteMsg_CustomMtMxGetSetParamType* )pMsg->pPayload;

	uint32_t unPort2GetParam = pPayload->unPortID;
	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (GetParam) [port ID] = [%d, %d]",
			me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID, (int)pPayload->unPortDirection);

	if(ELITEMSG_PARAM_ID_CAL != pPayload->unParamId)
	{
		result = ADSP_EBADPARAM;
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (GetParam) handler: Only ELITEMSG_PARAM_ID_CAL supported",
				me->mtMxID, pPayload->unSecOpCode);
		goto __bailoutCmdGetParamHandler;
	}

	if (MATRIX_MIXER_PORT_DIR_INPUT == pPayload->unPortDirection)
	{
		MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unPort2GetParam];

		//Port ID validity check
		if ((NULL == pCurrentInPort) || (me->maxInPortID < unPort2GetParam || INPUT_PORT_STATE_INACTIVE == pCurrentInPort->inPortState))
		{
			result = ADSP_EBADPARAM;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (GetParam) handler: rcvd invalid/inactive port ID [%d]",
					me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID);
			goto __bailoutCmdGetParamHandler;
		}

		//Route to the appropriate handler
		if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			result = MxAr_InPortGetParamHandler(me, pMsg, pPayload,  unPort2GetParam, pCurrentInPort);
		}
		else //if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
		{
			result = MxAt_InPortGetParamHandler(me, pMsg, pPayload,  unPort2GetParam, pCurrentInPort);
		}
	}
	else if (MATRIX_MIXER_PORT_DIR_OUTPUT == pPayload->unPortDirection)
	{
		MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unPort2GetParam];

		//Port ID validity check
		if ((NULL==pCurrentOutPort)||(me->maxOutPortID < unPort2GetParam || OUTPUT_PORT_STATE_INACTIVE == pCurrentOutPort->outPortState))
		{
			result = ADSP_EBADPARAM;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (GetParam) handler: rcvd invalid/inactive port ID [%d]",
					me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID);
			goto __bailoutCmdGetParamHandler;
		}

		//Route to the appropriate handler
		if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			result = MxAr_OutPortGetParamHandler(me, pMsg, pPayload,  unPort2GetParam, pCurrentOutPort);
		}
		else //if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
		{
			result = MxAt_OutPortGetParamHandler(me, pMsg, pPayload,  unPort2GetParam, pCurrentOutPort);
		}
	}
	else
	{
		result = ADSP_EBADPARAM;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (GetParam) handler: rcvd invalid port direction[%d]",
				me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortDirection);
	}

	__bailoutCmdGetParamHandler:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (GetParam) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

ADSPResult MtMx_SetPrimaryOutputPort(This_t *me, elite_msg_any_t *pMsg)
{
	ADSPResult result = ADSP_EOK;
	EliteMsg_CustomSetPrimaryPortType *pPayload = (EliteMsg_CustomSetPrimaryPortType*)pMsg->pPayload;
	uint32_t unNumSessions = 0;
	EliteMsg_CustomPrimaryPortMappingInfoType *pMapPayload = NULL;
	MatrixInPortInfoType *pCurrentInPort = NULL;
	MatrixOutPortInfoType *pCurrentOutPort = NULL;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (SetPrimayOutputPort)",
			me->mtMxID, pPayload->unSecOpCode);

	if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: SetPrimayOutputPort is received on TX matirx",me->mtMxID);
		result = ADSP_EFAILED;
		goto __bailoutCmdSetPrimaryOutputPort;
	}

	//derive number of sessions
	unNumSessions = pPayload->unNumSessions;

	//derive mapping information
	pMapPayload = (EliteMsg_CustomPrimaryPortMappingInfoType*)(pPayload + 1);

	//update primary port information of input ports
	while(unNumSessions)
	{
		pCurrentInPort =  me->inPortParams[pMapPayload->unInputPort];
		pCurrentInPort->unTopPrioOutPort = pMapPayload->unPrimaryOutputPort;
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Input Port [%u] maps to primary output port [%u]",
				me->mtMxID, pMapPayload->unInputPort, pMapPayload->unPrimaryOutputPort);

		//Get top priority port
		pCurrentOutPort = me->outPortParams[pCurrentInPort->unTopPrioOutPort];

		//Set the s2pd drift pointer here
		(void)MtMx_SetInOutDriftPtr(me, pMapPayload->unInputPort, pCurrentInPort->unTopPrioOutPort);

		//Since the s2pd drift pointer got potentially re-set here, call the re-init on the SampleSlip library to set this, if applicable.
		(void)MtMx_InPortSetS2DPtrSampleSlipLibrary(me, pMapPayload->unInputPort); //AV-Sync

		//If an output port was waiting on resources, then add it back to the wait mask.
		if(TRUE == pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut)
		{
			MtMx_AddOutputPortToWaitMask(me, pCurrentInPort->unTopPrioOutPort);
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu adding o/p port %lu to wait mask", me->mtMxID, pCurrentInPort->unTopPrioOutPort);
			pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = FALSE;
		}

		//AV-Sync
		MtMx_CalculateStreamDelay(me, pMapPayload->unInputPort);

		pMapPayload++;
		unNumSessions--;
	}

	__bailoutCmdSetPrimaryOutputPort:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (SetPrimayOutputPort) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

static ADSPResult MxArFlushInputQRetainSessionClock(This_t *me, uint32_t unInPortID)
{
	ADSPResult              result = ADSP_EOK;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	elite_msg_any_t         myDataQMsg, mediaTypeMsg;
	bool_t isMediaTypeMsgHeld = FALSE;

	// mark local buffer empty
	pCurrentInPort->bIsLocalBufEmpty = TRUE;
	pCurrentInPort->bIsLocalBufFull = FALSE;
	pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;

	// reset local buffer pointers
	MtMx_InitInPortLocalBufPtrs(me, unInPortID);

	// check if any input buffer is held.
	if (INPUT_BUFFER_HELD == pCurrentInPort->bInBufStatus)
	{
		// release the input buffer to its queue.
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu releasing an i/p buffer it was holding on to", me->mtMxID, unInPortID);

		myDataQMsg = pCurrentInPort->myDataQMsg;
		if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&myDataQMsg)))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu failed to return buffer while flushing", me->mtMxID, unInPortID);
			return result;
		}
		pCurrentInPort->numBufReturned++;
		pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
	}

	MtMx_AddInputPortToWaitMask(me, unInPortID);

	// check for any i/p buffers in the dataQ.
	qurt_elite_channel_t *pChannel = qurt_elite_queue_get_channel(pCurrentInPort->inPortHandle.portHandle.dataQ);

	while (qurt_elite_channel_poll(pChannel, qurt_elite_queue_get_channel_bit(pCurrentInPort->inPortHandle.portHandle.dataQ)))
	{
		// take next buffer off the q
		if (ADSP_FAILED(result = qurt_elite_queue_pop_front(pCurrentInPort->inPortHandle.portHandle.dataQ, (uint64_t*)&myDataQMsg)))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu failed to pop buffer off its dataQ", me->mtMxID, unInPortID);
			return result;
		}

		pCurrentInPort->myDataQMsg = myDataQMsg;
		//If the current data msg is mark buffer, raise event
		if(myDataQMsg.unOpCode == ELITE_DATA_MARK_BUFFER)
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx flushing Marked Buffer");
			MtMx_ProcessMarkBuffer(me, unInPortID, pCurrentInPort->myDataQMsg, ASM_DATA_EVENT_MARK_BUFFER_DISCARDED);
			continue;
		}

		if(ELITE_DATA_MEDIA_TYPE == pCurrentInPort->myDataQMsg.unOpCode)
		{
			//Retain Media Type messages.

			//If there was already a retained Media Type, release that first
			if(TRUE == isMediaTypeMsgHeld)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu releasing a Media Type message that was in dataQ", me->mtMxID, unInPortID);
				if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&mediaTypeMsg)))
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu failed to return buffer while flushing", me->mtMxID, unInPortID);
					return result;
				}
			}

			//Now, retain the details of the new Media Type
			mediaTypeMsg = pCurrentInPort->myDataQMsg;
			isMediaTypeMsgHeld = TRUE;
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu retaining Media Type message", me->mtMxID, unInPortID);
		}
		else
		{
			// release the input buffer to its queue.
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu releasing an i/p buffer that was in dataQ", me->mtMxID, unInPortID);
			if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&myDataQMsg)))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu failed to return buffer while flushing", me->mtMxID, unInPortID);
				return result;
			}
		}
	}

	//If there was a media type message held during flush, push it back into the dataQ
	if(TRUE == isMediaTypeMsgHeld)
	{
		result = qurt_elite_queue_push_back(pCurrentInPort->inPortHandle.portHandle.dataQ, (uint64_t*)&(mediaTypeMsg));
		if (ADSP_FAILED(result))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu error pushing MediaTypeMsg back onto dataQ", me->mtMxID, unInPortID);
			return result;
		}
		else
		{
			isMediaTypeMsgHeld = FALSE;
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu pushed MediaTypeMsg back onto dataQ success", me->mtMxID, unInPortID);
		}
	}

	return ADSP_EOK;
}

ADSPResult MtMx_SetPsPdParamHandler(This_t *me, elite_msg_any_t *pMsg)
{
	ADSPResult result = ADSP_EOK;
	EliteMsg_CustomMtMxPspdGetSetParamType*  pPayload = (EliteMsg_CustomMtMxPspdGetSetParamType* )pMsg->pPayload;
	uint32_t unInPortID = pPayload->unInPortID;
	uint32_t unOutPortID = pPayload->unOutPortID;
	MatrixInPortInfoType *pCurrentInPort;
	MatrixOutPortInfoType *pCurrentOutPort;
	void *pDataStart;

	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (SetPSPDParam) [i/p, o/p] = [%lu, %lu]",
			me->mtMxID, pPayload->unSecOpCode, unInPortID, unOutPortID);

	if(ELITEMSG_PARAM_ID_CAL != pPayload->unParamId)
	{
		result = ADSP_EBADPARAM;
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetPSPDParam) handler: Only ELITEMSG_PARAM_ID_CAL supported",
				me->mtMxID, pPayload->unSecOpCode);
		elite_msg_finish_msg(pMsg, result);
		return result;
	}

	//i/p port ID validity check
	if (me->maxInPortID < unInPortID)
	{
		result = ADSP_EBADPARAM;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetPSPDParam) handler: rcvd invalid i/p port ID [%lu]",
				me->mtMxID, pPayload->unSecOpCode, pPayload->unInPortID);
		elite_msg_finish_msg(pMsg, result);
		return result;
	}
	pCurrentInPort = me->inPortParams[unInPortID];
	if ((NULL==pCurrentInPort)||(INPUT_PORT_STATE_INACTIVE == pCurrentInPort->inPortState))
	{
		result = ADSP_EBADPARAM;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetPSPDParam) handler: rcvd inactive i/p port ID [%lu]",
				me->mtMxID, pPayload->unSecOpCode, pPayload->unInPortID);
		elite_msg_finish_msg(pMsg, result);
		return result;
	}

	//o/p port ID validity check
	if (me->maxOutPortID < unOutPortID)
	{
		result = ADSP_EBADPARAM;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetPSPDParam) handler: rcvd invalid o/p port ID [%lu]",
				me->mtMxID, pPayload->unSecOpCode, pPayload->unOutPortID);
		elite_msg_finish_msg(pMsg, result);
		return result;
	}
	pCurrentOutPort = me->outPortParams[unOutPortID];
	if ((NULL==pCurrentOutPort)||(OUTPUT_PORT_STATE_INACTIVE == pCurrentOutPort->outPortState))
	{
		result = ADSP_EBADPARAM;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetPSPDParam) handler: rcvd inactive o/p port ID [%lu]",
				me->mtMxID, pPayload->unSecOpCode, pPayload->unOutPortID);
		elite_msg_finish_msg(pMsg, result);
		return result;
	}

	if ((pCurrentInPort->strMask & (1 << unOutPortID)) == 0)
	{
		result = ADSP_EBADPARAM;
		MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetPSPDParam) handler:Error:set param rcvd on not-connected I/p Port ID, O/p Port ID pair:[%lu, %lu]",
				me->mtMxID, pPayload->unSecOpCode, unInPortID, unOutPortID);
		elite_msg_finish_msg(pMsg, result);
		return result;
	}

	uint16_t unTotalPayloadSize = pPayload->unSize;
	adm_pspd_param_data_t *pDataPayload = (adm_pspd_param_data_t *)(pPayload->pnParamData);

	//Code should be capable of handling multiple Set Params in the same command
	//Until Total Payload Size becomes zero, keep reading params
	while(unTotalPayloadSize > 0)
	{
		//If we reach the end or very close to the end, break out
		if(unTotalPayloadSize < sizeof(adm_pspd_param_data_t))
		{
			elite_msg_finish_msg(pMsg, result);
			return result;
		}

		//If the total payload size is incorrect, return an error
		if(unTotalPayloadSize < (sizeof(adm_pspd_param_data_t) + (pDataPayload->param_size)))
		{
			result = ADSP_ENEEDMORE;
			MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu o/p port %luTotal Payload Size: %d < Reqd Size: %d",
					me->mtMxID, unInPortID, unOutPortID, unTotalPayloadSize,
					(sizeof(adm_pspd_param_data_t) + (pDataPayload->param_size)));
			elite_msg_finish_msg(pMsg, result);
			return result;
		}

		//If the data size of the paramter/module combination is not multiple of 4 bytes, return an error.
		if ((pDataPayload->param_size % 4) != 0)
		{
			result = ADSP_EBADPARAM;
			MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu o/p port %lu:Error:Param size for set param is %lu which is not a multiple of 4. Aborting further processing.",
					me->mtMxID, unInPortID, unOutPortID, pDataPayload->param_size);
			elite_msg_finish_msg(pMsg, result);
			return result;
		}

		//Offset by the struct size to get to the start of the data
		pDataStart = (void*)(pDataPayload + 1);

		switch(pDataPayload->module_id)
		{
		case AUDPROC_MODULE_ID_CHMIXER:
		{
			switch(pDataPayload->param_id)
			{
			case AUDPROC_CHMIXER_PARAM_ID_COEFF:
			{
				mt_mx_struct_channel_mixer_t *pCurrentInputOutputChMixer;

				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu o/p port %lu PSPD, MID: %lu, PID: %lu",
						me->mtMxID, unInPortID, unOutPortID, pDataPayload->module_id, pDataPayload->param_id);

				pCurrentInputOutputChMixer = &(pCurrentInPort->structChanMixer[unOutPortID]);
				MtMx_InOutPortsToCheckReInitChannelMixerLibrary(me, unInPortID, unOutPortID, FALSE);
				(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);
				if(TRUE == pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated)
				{
					pCurrentInputOutputChMixer->bCanChMixerBeOptimizedOut = FALSE;
					audproc_chmixer_param_id_coeff_t *coef_payload = (audproc_chmixer_param_id_coeff_t *)pDataStart;
					ADSPResult result = MtMx_SetChannelMixerCoef(me, unInPortID, unOutPortID, coef_payload);
					if(ADSP_EOK == result)
					{
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu o/p port %lu SetParam on ChannelMixer library success",
								me->mtMxID, unInPortID, unOutPortID);
					}
					else
					{
						MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu o/p port %lu SetParam on ChannelMixer library failed. Default chmixer coeff will be used.",
								me->mtMxID, unInPortID, unOutPortID);

						goto __bailoutPspdCoeff;
					}
				}
				else
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
							"MtMx #%lu: i/p port %lu o/p port %lu SetParam on ChannelMixer library failed because the library could not be created",
							me->mtMxID, unInPortID, unOutPortID);
					result = ADSP_EFAILED;
					goto __bailoutPspdCoeff;
				}

				break;

				__bailoutPspdCoeff:
				elite_msg_finish_msg(pMsg, result);
				return result;
			}
			default:
			{
				result = ADSP_EBADPARAM;
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu o/p port %lu Invalid Param ID: %lu",
						me->mtMxID, unInPortID, unOutPortID, pDataPayload->param_id);
				elite_msg_finish_msg(pMsg, result);
				return result;
			}
			}
			break;
		}
		default:
		{
			result = ADSP_EBADPARAM;
			MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu o/p port %lu Invalid Module ID: %lu",
					me->mtMxID, unInPortID, unOutPortID, pDataPayload->module_id);
			elite_msg_finish_msg(pMsg, result);
			return result;
		}
		}

		//Now, move to the next SetParam and update the total payloadsize
		unTotalPayloadSize -= (sizeof(adm_pspd_param_data_t) + pDataPayload->param_size);
		pDataPayload = (adm_pspd_param_data_t *)( (uint32_t*)pDataStart + (pDataPayload->param_size >> 2) );;
	}

	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (SetPsPdParam) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
}

static void MtMx_DetectIfInputPortsNeedToTransitionAwayFromReady(This_t *me)
{
	MatrixInPortInfoType    *pCurrentInPort;
	uint32_t unInPortID;

	for (unInPortID =0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];

		//Check if this i/p port needs to transition away from ready and if so, check if it can be done and if so, do it
		if (NULL != pCurrentInPort &&
				TRUE == pCurrentInPort->bDoesInputPortNeedToTransitionAwayFromReady &&
				TRUE == MtMx_CheckIfInputPortCanTransitionAwayFromReady(me, unInPortID))
		{
			if ((INPUT_PORT_STATE_PAUSED == pCurrentInPort->inPortStatePrToStateChange) ||
					(INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER == pCurrentInPort->inPortStatePrToStateChange))
			{
				pCurrentInPort->inPortState = pCurrentInPort->inPortStatePrToStateChange;
			}
			else
			{
				pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE;
				pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE;
			}
			MtMx_InPortToUpdateWaitMask(me, unInPortID);
		}
	}
}

void MtMx_MsgResync(This_t *me, uint32_t unInPortID, elite_msg_any_t myDataQMsg)
{
    MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];
    ADSPResult result = ADSP_EOK;

    if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
    {
       result = ADSP_EBADPARAM;
       MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR: MtMx #%lu i/p port %lu rcvd invalid data command Resync %lu, returning buffer",
                       me->mtMxID, unInPortID, myDataQMsg.unOpCode);
       goto __bailoutresync;
    }

    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu rcvd msg %lu (MtMx_MsgResync)", me->mtMxID, unInPortID, myDataQMsg.unOpCode);


    //Attempt to accumulate any pending data
    if (MtMx_InportIsLocalBufFull(pCurrentInPort))
    {
        if ((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask))
        {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: triggering acc of local buffer after Resync was rcvd", me->mtMxID, unInPortID);
            MxAr_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
            MxAr_ProcessPendingPullOutPorts(me);
        }
        else
        {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: unable to trigger acc of local buffer after Resync was rcvd, mark acc pending",
                            me->mtMxID, unInPortID);
            pCurrentInPort->bIsAccumulationPending = TRUE;
        }
    }
    pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
    (void)MtMx_SetupDefaultRenderWindow(me, unInPortID); //AV-Sync
    (void)MtMx_ResetStrClkSync(me, unInPortID); //AV-Sync
    pCurrentInPort->bForceCheckTimeStampValidity = TRUE; //Force to check TS validity for next buffer
    pCurrentInPort->bShouldSessionTimeBeResync = TRUE; //Force session clock to reset

__bailoutresync:
      elite_msg_finish_msg(&myDataQMsg, result);
      return;

}

static ADSPResult MxAt_InPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	return ADSP_EUNSUPPORTED;
}

static ADSPResult MxAr_OutPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	return ADSP_EUNSUPPORTED;
}

static ADSPResult MxAt_OutPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	return ADSP_EUNSUPPORTED;
}

static ADSPResult MxAt_InPortGetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	return ADSP_EUNSUPPORTED;
}

static ADSPResult MxAr_OutPortGetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	return ADSP_EUNSUPPORTED;
}

static ADSPResult MxAt_OutPortGetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	return ADSP_EUNSUPPORTED;
}
void MtMx_SetLimiterModeOnOutputPorts(This_t *me)
{
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  uint32_t unNumConnectedSessions = 0; // Number of active sessions connected to each output port
  uint32_t currConnInPorts = 0;
  bool_t   limiter_bypass_mode = FALSE;
  
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Scanning output ports for connected input ports..", me->mtMxID);
  
  // Loop through all the output ports
  for (uint32_t unOutPortId = 0; unOutPortId <= me->maxOutPortID; unOutPortId++)
  {
    pCurrentOutPort = me->outPortParams[unOutPortId];
    limiter_bypass_mode = FALSE;
    unNumConnectedSessions =0;
    if ( (NULL != pCurrentOutPort) && (OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState) )
    {
      // retrieve connected input port mask
      currConnInPorts = pCurrentOutPort->inPortsMask;
      
      unNumConnectedSessions = Q6_R_popcount_P(currConnInPorts);
      
      if(unNumConnectedSessions <= 1)
      {
        limiter_bypass_mode = TRUE;
      }
      
      MtMx_OutPortLimiterSetMode(me, unOutPortId, limiter_bypass_mode);
  
    } // end if
  } // end for
}

ADSPResult MtMx_SetOutputPortParamHandler(This_t *me, elite_msg_any_t *pMsg)
{
  ADSPResult result = ADSP_EOK;
  EliteMsg_CustomMtMxOutputPortSetParamType*  pPayload = (EliteMsg_CustomMtMxOutputPortSetParamType* )pMsg->pPayload;
  uint32_t unOutPortID = pPayload->unOutPortID;
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  void *pDataStart = NULL;
  mt_mx_struct_limiter_t *pLimiter = NULL;
  
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (SetMtMxParam) on o/p port ID [%lu]",
      me->mtMxID, pPayload->unSecOpCode, unOutPortID);
  
  if(ELITEMSG_PARAM_ID_CAL != pPayload->unParamId)
  {
    result = ADSP_EBADPARAM;
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetMtMxParam) handler: Only ELITEMSG_PARAM_ID_CAL supported",
        me->mtMxID, pPayload->unSecOpCode);
    elite_msg_finish_msg(pMsg, result);
    return result;
  }
  
  //o/p port ID validity check
  if (me->maxOutPortID < unOutPortID)
  {
    result = ADSP_EBADPARAM;
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetMtMxParam) handler: rcvd invalid o/p port ID [%lu]",
        me->mtMxID, pPayload->unSecOpCode, pPayload->unOutPortID);
    elite_msg_finish_msg(pMsg, result);
    return result;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
  if ((NULL==pCurrentOutPort)||(OUTPUT_PORT_STATE_INACTIVE == pCurrentOutPort->outPortState))
  {
    result = ADSP_EBADPARAM;
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetMtMxParam) handler: rcvd inactive o/p port ID [%lu]",
        me->mtMxID, pPayload->unSecOpCode, pPayload->unOutPortID);
    elite_msg_finish_msg(pMsg, result);
    return result;
  }

  uint16_t unTotalPayloadSize = pPayload->unSize;
  adm_param_data_v6_t *pDataPayload = (adm_param_data_v6_t *)(pPayload->pnParamData);
  
  pLimiter = &(pCurrentOutPort->structLimiter);
  
  //Code should be capable of handling multiple Set Params in the same command
  //Until Total Payload Size becomes zero, keep reading params
  while(unTotalPayloadSize > 0)
  {
    //If we reach the end or very close to the end, break out
    if(unTotalPayloadSize < sizeof(adm_param_data_v6_t))
    {
      break;
    }
    
    //If the total payload size is incorrect, return an error
    if(unTotalPayloadSize < (sizeof(adm_param_data_v6_t) + (pDataPayload->param_size)))
    {
      result = ADSP_ENEEDMORE;
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu:  o/p port %luTotal Payload Size: %d < Reqd Size: %d",
          me->mtMxID,  unOutPortID, unTotalPayloadSize,
          (sizeof(adm_param_data_v6_t) + (pDataPayload->param_size)));
      break;
    }
    
    //If the data size of the paramter/module combination is not multiple of 4 bytes, return an error.
    if ((pDataPayload->param_size % 4) != 0)
    {
      result = ADSP_EBADPARAM;
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: o/p port %lu:Error:Param size for set param is %lu which is not a multiple of 4. Aborting further processing.",
          me->mtMxID,unOutPortID, pDataPayload->param_size);
      break;
    }
    
    //Offset by the struct size to get to the start of the data
    pDataStart = (void*)((int8_t*)pDataPayload + sizeof(adm_param_data_v6_t));
    
    switch(pDataPayload->module_id)
    {
      case VOICE_MODULE_TX_STREAM_LIMITER:
      {        
        /*Make sure that the output port is not connected yet to any input ports. 
         If output port is already connected, reject this Set Param.
         The Matrix limiter instance must send set params after opening the device and 
         before sending the map routing the command.
        */
        if(pCurrentOutPort->inPortsMask)
        {
          result = ADSP_EBADPARAM;
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetMtMxLimiterParam) called on o/p port ID [%lu] after map routing, skipping processing.",
              me->mtMxID, pPayload->unSecOpCode, pPayload->unOutPortID);
          break;
        }
        switch(pDataPayload->param_id)
        {
          MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu recieved set param for MID: %lu, PID: %lu",
                me->mtMxID, unOutPortID, pDataPayload->module_id, pDataPayload->param_id);
          case AUDPROC_PARAM_ID_ENABLE:
          {
            audproc_enable_param_t *enable_ptr =  (audproc_enable_param_t *)pDataStart;
            
            pLimiter->bIsLimiterEnable = enable_ptr->enable;
            
            /*
              Limiter instance is created on the output port when
               a. The client should set the limiter enable flag and
               b. If native mode is true or (non-native mode and output bit-width is not 32bit)              
            */
            
             MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu recieved limiter enable:%lu for device native_mode: %lu, and bit-width:%lu",
                me->mtMxID, unOutPortID, enable_ptr->enable,pCurrentOutPort->unNativeModeFlags,pCurrentOutPort->unBitwidth );
            
            if( (pLimiter->bIsLimiterEnable) && 
                ((TRUE == pCurrentOutPort->unNativeModeFlags) || ((FALSE == pCurrentOutPort->unNativeModeFlags) && (MT_MX_BIT_WIDTH_32 != pCurrentOutPort->unBitwidth))))
            {
              result = MtMx_OutPortLimiterInit(me, unOutPortID);
              if(ADSP_EOK == result)
              {
                MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu:o/p port %lu Limiter init success",
                    me->mtMxID, unOutPortID);
              }
              else
              {
                MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: o/p port %lu Limiter init failed, result= %lu",
                    me->mtMxID, unOutPortID, result);
              }
            }
            else
            {
              if(NULL != pLimiter->pLimiterCapiV2)
              {
                result = MtMx_OutPortLimiterDeinit(me, unOutPortID);
              }              
            }
            break;
          }
          case VOICE_PARAM_TX_STREAM_LIMITER:
          {
            if(NULL != pLimiter->pLimiterCapiV2)
            {
              result = MtMx_OutPortLimiterSetParams(me, unOutPortID, pDataPayload->param_id, pDataStart, pDataPayload->param_size);
            }
            else
            {
              result = ADSP_EFAILED;
              MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: o/p port %lu Limiter config set failed, result= %lu",
                  me->mtMxID, unOutPortID, result);
            }            
            break;
          }
          default:
          {
            result = ADSP_EBADPARAM;
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu:  o/p port %lu Invalid Param ID: %lu",
                me->mtMxID, unOutPortID, pDataPayload->param_id);
            break;
          }
        }
        break;
      }
      default:
      {
        result = ADSP_EBADPARAM;
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu:o/p port %lu Invalid Module ID: %lu",
            me->mtMxID, unOutPortID, pDataPayload->module_id);
        break;
      }
    }
    
    //Now, move to the next SetParam and update the total payloadsize
    unTotalPayloadSize -= (sizeof(adm_param_data_v6_t) + pDataPayload->param_size);
    pDataPayload = (adm_param_data_v6_t *)( (uint32_t*)pDataStart + (pDataPayload->param_size >> 2) );
  }

  elite_msg_finish_msg(pMsg, result);
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (SetMtMxParam) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
  return result;
}

ADSPResult MtMx_GetOutputPortParamHandler(This_t *me, elite_msg_any_t *pMsg)
{
  ADSPResult result = ADSP_EOK;
  EliteMsg_CustomMtMxOutputPortGetParamType*  pPayload = (EliteMsg_CustomMtMxOutputPortGetParamType*)pMsg->pPayload;
  uint32_t unOutPortID = pPayload->unOutPortID;
  MatrixOutPortInfoType *pCurrentOutPort = NULL;
  int8_t *calib_data_ptr = NULL;
  
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (GetMtMxParam) on o/p port ID [%lu]",
      me->mtMxID, pPayload->unSecOpCode, unOutPortID);
  
  if(ELITEMSG_PARAM_ID_CAL != pPayload->unParamId)
  {
    result = ADSP_EBADPARAM;
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (GetMtMxParam) handler: Only ELITEMSG_PARAM_ID_CAL supported",
        me->mtMxID, pPayload->unSecOpCode);
    elite_msg_finish_msg(pMsg, result);
    return result;
  }
  
  //o/p port ID validity check
  if (me->maxOutPortID < unOutPortID)
  {
    result = ADSP_EBADPARAM;
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (GetMtMxParam) handler: rcvd invalid o/p port ID [%lu]",
        me->mtMxID, pPayload->unSecOpCode, pPayload->unOutPortID);
    elite_msg_finish_msg(pMsg, result);
    return result;
  }
  
  pCurrentOutPort = me->outPortParams[unOutPortID];
  if ((NULL==pCurrentOutPort)||(OUTPUT_PORT_STATE_INACTIVE == pCurrentOutPort->outPortState))
  {
    result = ADSP_EBADPARAM;
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetMtMxParam) handler: rcvd inactive o/p port ID [%lu]",
        me->mtMxID, pPayload->unSecOpCode, pPayload->unOutPortID);
    elite_msg_finish_msg(pMsg, result);
    return result;
  }
  
  adm_param_data_v6_t *pDataPayload = (adm_param_data_v6_t *)(pPayload->pnParamData);
  calib_data_ptr = (int8_t *)pPayload->pnParamData + sizeof(adm_param_data_v6_t);
  
  memset(calib_data_ptr, 0, sizeof(adm_param_data_v6_t));
  
  switch(pDataPayload->module_id)
  {
    MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu recieved get param for MID: %lu, PID: %lu",
                    me->mtMxID, unOutPortID, pDataPayload->module_id, pDataPayload->param_id);
                
    case VOICE_MODULE_TX_STREAM_LIMITER:
    {
      result = MtMx_OutPortLimiterGetParams(me, unOutPortID, pDataPayload->param_id, calib_data_ptr, pPayload->unSize, &pDataPayload->param_size);
      break;
    } 
    default:
    {
      result = ADSP_EBADPARAM;
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu:o/p port %lu Invalid Module ID: %lu",
          me->mtMxID, unOutPortID, pDataPayload->module_id);
      elite_msg_finish_msg(pMsg, result);
      return result;
    }
  } //end of switch case
  
  elite_msg_finish_msg(pMsg, result);
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (GetMtMxParam) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
  return result;
  
}

