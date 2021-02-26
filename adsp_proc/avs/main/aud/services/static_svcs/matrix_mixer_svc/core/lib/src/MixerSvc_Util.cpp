/**
@file MixerSvc_Util.cpp
@brief This file defines various utility functions that the
       audio matrix mixer uses.
 */

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_Util.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/2010 AAA     Created file.
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"
#include "MixerSvc_MsgHandlers.h"
#include "MixerSvc_OutPortHandler.h"
#include "MixerSvc_Util.h"
#include "avsync_lib.h"
#include "adsp_media_fmt.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "MixerSvc_InPortHandler.h"
#include "MixerSvc_ChanMixUtils.h"
#include "MixerSvc_Pspd.h"
#include "AFEInterface.h"
#include "MixerSvc_AvSync.h"
#include "MixerSvc_Limiter.h"


static void MtMx_ApplyGainAndAccumulateInputToOutput(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf);

static void MtMx_ApplyRampGainAndAccOnEqualNumChannels(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf);
static void MtMx_ApplyImmGainAndAccOnEqualNumChannels(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf);

static void MtMx_ApplyRampGainAndAccOnStereoOutMonoIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf);
static void MtMx_ApplyImmGainAndAccOnStereoOutMonoIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf);

static void MtMx_ApplyRampGainAndAccOnMonoOutStereoIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf);
static void MtMx_ApplyImmGainAndAccOnMonoOutStereoIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf);

static void MtMx_ApplyRampGainAndAccOnMultiOutMultiIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf);
static void MtMx_ApplyImmGainAndAccOnMultiOutMultiIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf);

static void MtMx_GainAndAccOnEqualNumChannels(uint16_t unNumSamplesGainAndAccPerCh,
		uint32_t unNumChannels,
		MatrixInPortInfoType *pCurrentInPort,
		MatrixOutPortInfoType *pCurrentOutPort,
		int8_t *pInputBuf, int64_t *pOutputBuf,
		uint16_t *pCurrentGain);

static void MtMx_GainAndAccOnStereoOutMonoIn(uint16_t unNumSamplesGainAndAccPerCh,
		MatrixInPortInfoType *pCurrentInPort,
		MatrixOutPortInfoType *pCurrentOutPort,
		int8_t *pInputBuf, int64_t *pOutputBuf,
		uint16_t *pCurrentGain);

static void MtMx_GainAndAccOnMonoOutStereoIn(uint16_t unNumSamplesGainAndAccPerCh,
		MatrixInPortInfoType *pCurrentInPort,
		MatrixOutPortInfoType *pCurrentOutPort,
		int8_t *pInputBuf, int64_t *pOutputBuf,
		uint16_t *pCurrentGain);

static void MtMx_GainAndAccOnMultiOutMultiIn(uint16_t unNumSamplesGainAndAccPerCh,
		MatrixInPortInfoType *pCurrentInPort,
		MatrixOutPortInfoType *pCurrentOutPort,
		int8_t *pInputBuf, int64_t *pOutputBuf,
		uint16_t *pCurrentGain);

static void MtMx_TerminatePspdSvc(mt_mx_struct_pspd_t *pCurrentPspd);

bit_conv_mode_t MtMx_IdentifyBitConvType(uint32_t unInputBW, uint32_t unOutputBW);

void MtMx_InitInPortLocalBufPtrs(This_t *me, uint32_t unInPortID)
{
	me->inPortParams[unInPortID]->pWrLoc = me->inPortParams[unInPortID]->pStartLoc;
}


ADSPResult MtMx_FillInPortLocalBufHold(This_t *me, uint32_t unInPortID, uint32_t numSamplesReqPerCh)
{
	uint32_t                num_bytes_req_per_ch;
	ADSPResult              result = ADSP_EOK;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	int8_t                  *pWrLocation = pCurrentInPort->pWrLoc;
	uint16_t                in_port_bytes_per_sample = pCurrentInPort->unBytesPerSample;

	//buf size per channel in bytes for in port local buffer
	uint32_t                 num_bytes_per_ch_in_port_buf = (pCurrentInPort->unInPortPerChBufSize * in_port_bytes_per_sample);

	//Num bytes required per channel
	num_bytes_req_per_ch = (numSamplesReqPerCh * in_port_bytes_per_sample);

	//Memset with zeros
	for (uint32_t k = 0; k < pCurrentInPort->unNumChannels; k++)
	{
		memset((void*)pWrLocation, 0, num_bytes_req_per_ch);

		//Increment the read and write pointers to point to next channel
		pWrLocation += num_bytes_per_ch_in_port_buf;
	}

	//Increment the inport local buf write pointer by bytes per channel required
	pCurrentInPort->pWrLoc += num_bytes_req_per_ch;

	//Decrement the samples required per channel to fill local buffer
	pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf -= numSamplesReqPerCh;

	//If local buffer is full, set the flag
	if (0 == pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf)
	{
		pCurrentInPort->bIsLocalBufFull = TRUE;
	}
	else
	{
		pCurrentInPort->bIsLocalBufFull = FALSE;
	}

	//Local buffer is either partially or fully filled now
	pCurrentInPort->bIsLocalBufEmpty = FALSE;

	return result;
}

ADSPResult MxAr_ApplyGainAndAccumulateSamples(This_t *me, uint32_t unInPortID)
{
    uint32_t              unOutPortID;
    ADSPResult            result = ADSP_EOK;
    MatrixInPortInfoType  *pCurrentInPort;
    MatrixOutPortInfoType *pCurrentOutPort;
    mt_mx_struct_pspd_t   *pCurrentPspd;
    uint32_t              unActiveOutPortsMask;
    int64_t*              pAccBuf = NULL;
    uint32_t*             pAccInPortsMask = NULL;

	pCurrentInPort = me->inPortParams[unInPortID];

	//Step 1: Loop through all of it's currently connected active output ports
	unActiveOutPortsMask = pCurrentInPort->activeOutPortsMask;
	pCurrentInPort->unCurrentWaitMask = 0;

	while(unActiveOutPortsMask)
	{
		unOutPortID = Q6_R_ct0_R(unActiveOutPortsMask);
		unActiveOutPortsMask ^= 1 << unOutPortID;
		pCurrentOutPort = me->outPortParams[unOutPortID];

		//Step 2: If this output port is requesting data, then accumulate to regular accumulation buffer.
		if((pCurrentInPort->outputReqPendingMask & (1 << unOutPortID)))
		{
			//If there is a sampling rate mismatch, do not accumulate and continue to next port, added here for robustness.
			if(pCurrentInPort->unSampleRate != pCurrentOutPort->unSampleRate)
			{
				MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR! MtMx #%lu: i/p,o/p [%lu, %lu] sample rates [%lu, %lu] mismatch. Accumulation will not happen.",
						me->mtMxID, unInPortID, unOutPortID, pCurrentInPort->unSampleRate, pCurrentOutPort->unSampleRate);

				continue;
			}

			//Step 3: Get the pCurrentPspd and call the PSPD Process command
			pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
      
			result = MtMx_PspdProcCmd(pCurrentInPort, pCurrentOutPort, pCurrentPspd);
			if (ADSP_EOK != result)
			{
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR! MtMx #%lu: i/p,o/p [%lu, %lu], skipping pspd processing for this session. result=%d",me->mtMxID, unInPortID, unOutPortID, result);
				result = ADSP_EOK;
			}
		}
	}

	//Step 4: A blocking call on all the entities that this i/p port is waiting on
	while(pCurrentInPort->unCurrentWaitMask != 0)
	{
		elite_msg_any_t msg;
		elite_msg_data_buffer_v2_t *buf;

		//Get the PSPD responsible and its Qs (svc side returnQ).
		uint32_t unSetMask = qurt_elite_channel_wait(&pCurrentInPort->pspd_channel, pCurrentInPort->unCurrentWaitMask);
		unOutPortID = (Q6_R_ct0_R(unSetMask))/3;
		pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
		pCurrentOutPort = me->outPortParams[unOutPortID];
		qurt_elite_queue_t *inpQ = (qurt_elite_queue_t *)pCurrentPspd->inp_data_q;
		qurt_elite_queue_t *outQ = (qurt_elite_queue_t *)pCurrentPspd->out_data_q;

		//Pop msg, chk result and push it back to svc side returnQ.
		if(unSetMask & pCurrentInPort->unCurrentWaitMask & inpQ->myChannelBit)
		{
			qurt_elite_queue_pop_front(inpQ, (uint64_t *)&msg);
			buf = (elite_msg_data_buffer_v2_t *)msg.pPayload;
			result = buf->unResponseResult;
			qurt_elite_queue_push_back(inpQ, (uint64_t *)&msg);
			pCurrentInPort->unCurrentWaitMask ^= inpQ->myChannelBit;
		}
		if(unSetMask & pCurrentInPort->unCurrentWaitMask & outQ->myChannelBit)
		{
			qurt_elite_queue_pop_front(outQ, (uint64_t *)&msg);
			buf = (elite_msg_data_buffer_v2_t *)msg.pPayload;
			result = buf->unResponseResult;
			qurt_elite_queue_push_back(outQ, (uint64_t *)&msg);
			pCurrentInPort->unCurrentWaitMask ^= outQ->myChannelBit;
		}

		//Step 5: Check if both the i/p and o/p of this PSPD has completed and a valid accumulation is seeked
		if((0 == (pCurrentInPort->unCurrentWaitMask & inpQ->myChannelBit)) &&
				(0 == (pCurrentInPort->unCurrentWaitMask & outQ->myChannelBit)))
		{
			//Step 6:
			//If this output port is requesting data, then accumulate to regular accumulation buffer.
			if (pCurrentInPort->outputReqPendingMask & (1 << unOutPortID))
			{
				pAccBuf = pCurrentOutPort->pAccBuf;
				pAccInPortsMask = &(pCurrentOutPort->accInPortsMask);
#ifdef MT_MX_EXTRA_DEBUG
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: accumulation on primary acc buffer: i/p port %lu o/p port %lu", me->mtMxID, unInPortID, unOutPortID);
#endif
			}
			else
			{
				pAccBuf = NULL;
				pAccInPortsMask = NULL;
#ifdef MT_MX_EXTRA_DEBUG
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: attempt for second accumulation: i/p port %lu o/p port %lu", me->mtMxID, unInPortID, unOutPortID);
#endif
			}

			if((NULL != pAccBuf) && (NULL != pAccInPortsMask))
			{
				//Set/clear some masks
				pCurrentInPort->outputReqPendingMask &= ~(1 << unOutPortID);
				(*pAccInPortsMask) |= 1 << unInPortID;

				//Update TS
				if(pCurrentInPort->bIsTimeStampValid)
				{
					mt_mx_copy_time_value(&pCurrentInPort->ullTimeStampAtAccumulation, pCurrentInPort->ullTimeStampAtCopy);
          
					/*if sampleslip is enabled, update the TS considering the past samples 
					present in the internal buffers(delayline length) */
					mt_mx_sampleslip_t     *pSampleSlip = &(pCurrentInPort->pInPortAvSync->structSampleSlip);
					if(NULL != pSampleSlip->pSampleSlipCapiv2)
					{
						mt_mx_decrement_time(&pCurrentInPort->ullTimeStampAtAccumulation, pSampleSlip->totalBufOffsetInUsec);
						pSampleSlip->totalBufOffsetInUsec.int_part =0;
						pSampleSlip->totalBufOffsetInUsec.frac_part = 0;
					}
				}

#ifdef MT_MX_EXTRA_DEBUG
				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu o/p port %lu Sending for acc. pAccBuf: 0x%p", me->mtMxID, unInPortID, unOutPortID, pAccBuf);
#endif

				//Proceed with normal operation from this output port's standpoint
				//Step 7: Apply Gain and accumulate into output port's accBuf
				MtMx_ApplyGainAndAccumulateInputToOutput(me, unInPortID, unOutPortID, pAccBuf);
				memset(pCurrentPspd->pPspdOutBuf, 0, pCurrentPspd->unPspdOutBufSize);
				pCurrentInPort->bHasFirstSampleAccumulated = TRUE;
			}
		}
	}

	//Now, continue with normal operation
	pCurrentInPort->pWrLoc = pCurrentInPort->pStartLoc;

	//AV-Sync
	(void)MtMx_HandleSampleAddDrops_1(me, unInPortID);

	pCurrentInPort->bIsLocalBufFull = FALSE;
	pCurrentInPort->bIsLocalBufEmpty = TRUE;

	return result;
}

ADSPResult MxAt_ApplyGainAndAccumulateSamples(This_t *me, uint32_t unInPortID)
{
	uint32_t                              unOutPortID;
	ADSPResult                        result = ADSP_EOK;
	MatrixInPortInfoType     *pCurrentInPort;
	MatrixOutPortInfoType  *pCurrentOutPort;
	mt_mx_struct_pspd_t       *pCurrentPspd;
	uint32_t                              unActiveOutPortsMask;
	int64_t*                              pAccBuf = NULL;
	uint32_t*                           pAccInPortsMask = NULL;

	pCurrentInPort = me->inPortParams[unInPortID];

	//Step 1: Loop through all of it's currently connected active output ports
	unActiveOutPortsMask = pCurrentInPort->activeOutPortsMask;
	pCurrentInPort->unCurrentWaitMask = 0;
	while(unActiveOutPortsMask)
	{
		unOutPortID = Q6_R_ct0_R(unActiveOutPortsMask);
		unActiveOutPortsMask ^= 1 << unOutPortID;
		pCurrentOutPort = me->outPortParams[unOutPortID];

		//Step 2:
		//If this output port is requesting data, then accumulate to regular accumulation buffer.
		if(((pCurrentOutPort->accBufAvailabilityMask & (1 << unInPortID)) && (pCurrentInPort->outputReqPendingMask & (1 << unOutPortID))))
		{
			//If there is a sampling rate mismatch, do not accumulate and continue to next port, added here for robustness.
			if(pCurrentInPort->unSampleRate != pCurrentOutPort->unSampleRate)
			{
				MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR! MtMx #%lu: i/p,o/p [%lu, %lu] sample rates [%lu, %lu] mismatch. Accumulation will not happen.",
						me->mtMxID, unInPortID, unOutPortID, pCurrentInPort->unSampleRate, pCurrentOutPort->unSampleRate);

				continue;
			}

			//Step 3: Get the pCurrentPspd and call the PSPD Process command
			pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
      
			result = MtMx_PspdProcCmd(pCurrentInPort, pCurrentOutPort, pCurrentPspd);
			if (ADSP_EOK != result)
			{
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR! MtMx #%lu: i/p,o/p [%lu, %lu], skipping pspd processing for this session. result =%d",me->mtMxID, unInPortID, unOutPortID, result);
				result =ADSP_EOK;
			}
		}
		else
		{
			//Current o/p is not ready to accept accumulation from this i/p port.
			if(TRUE == pCurrentInPort->bIsPortLive)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "OVERFLOW! MtMx #%lu: Live i/p %lu data will not be acc.(will be dropped) as o/p %lu is not ready to accept accumulation",
						me->mtMxID, unInPortID, unOutPortID);
				//Raise an overflow event
				MxAt_OutPortToCheckForOverflowEvents(me, unOutPortID);
			}
			else
			{

#ifdef MT_MX_EXTRA_DEBUG
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Non-Live i/p %lu will be added to o/p %lu waiting to acc mask as it is not ready to accept accumulation",
						me->mtMxID, unInPortID, unOutPortID);
#endif

				pCurrentOutPort->inPortsWaitingToAccMask |= (1 << unInPortID);
				pCurrentInPort->bIsNonLiveInputPortWaitingToAcc = TRUE;
				if((pCurrentOutPort->inPortsMask) && (pCurrentOutPort->inPortsWaitingToAccMask == pCurrentOutPort->inPortsMask))
				{
					MtMx_AddOutputPortToWaitMask(me, unOutPortID);
				}
			}
		}
	}

	//Step 4: A blocking call on all the entities that this i/p port is waiting on
	while(pCurrentInPort->unCurrentWaitMask != 0)
	{
		elite_msg_any_t msg;
		elite_msg_data_buffer_v2_t *buf;

		//Get the PSPD responsible and its Qs (svc side returnQ).
		uint32_t unSetMask = qurt_elite_channel_wait(&pCurrentInPort->pspd_channel, pCurrentInPort->unCurrentWaitMask);
		unOutPortID = (Q6_R_ct0_R(unSetMask))/3;
		pCurrentOutPort = me->outPortParams[unOutPortID];
		pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
		qurt_elite_queue_t *inpQ = (qurt_elite_queue_t *)pCurrentPspd->inp_data_q;
		qurt_elite_queue_t *outQ = (qurt_elite_queue_t *)pCurrentPspd->out_data_q;

		//Pop msg, chk result and push it back to svc side returnQ.
		if(unSetMask & pCurrentInPort->unCurrentWaitMask & inpQ->myChannelBit)
		{
			qurt_elite_queue_pop_front(inpQ, (uint64_t *)&msg);
			buf = (elite_msg_data_buffer_v2_t *)msg.pPayload;
			result = buf->unResponseResult;
			qurt_elite_queue_push_back(inpQ, (uint64_t *)&msg);
			pCurrentInPort->unCurrentWaitMask ^= inpQ->myChannelBit;
		}
		if(unSetMask & pCurrentInPort->unCurrentWaitMask & outQ->myChannelBit)
		{
			qurt_elite_queue_pop_front(outQ, (uint64_t *)&msg);
			buf = (elite_msg_data_buffer_v2_t *)msg.pPayload;
			result = buf->unResponseResult;
			qurt_elite_queue_push_back(outQ, (uint64_t *)&msg);
			pCurrentInPort->unCurrentWaitMask ^= outQ->myChannelBit;
		}

		//Step 5: Check if both the i/p and o/p of this PSPD has completed
		if((0 == (pCurrentInPort->unCurrentWaitMask & inpQ->myChannelBit)) &&
				(0 == (pCurrentInPort->unCurrentWaitMask & outQ->myChannelBit)))
		{
			//Step 6:
			//If this output port is requesting data, then accumulate to regular accumulation buffer.
			if ((pCurrentOutPort->accBufAvailabilityMask & (1 << unInPortID)) &&(pCurrentInPort->outputReqPendingMask & (1 << unOutPortID)))
			{
			    pAccBuf = pCurrentOutPort->pAccBuf;
			    pAccInPortsMask = &(pCurrentOutPort->accInPortsMask);
#ifdef MT_MX_EXTRA_DEBUG
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: accumulation on primary acc buffer: i/p port %lu o/p port %lu", me->mtMxID, unInPortID, unOutPortID);
#endif
			}
			else
			{
				pAccBuf = NULL;
				pAccInPortsMask = NULL;
#ifdef MT_MX_EXTRA_DEBUG
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: attempt for second accumulation: i/p port %lu o/p port %lu", me->mtMxID, unInPortID, unOutPortID);
#endif
			}

			if((NULL != pAccBuf) && (NULL != pAccInPortsMask))
			{
				//Set/clear some masks.
				//Now, the current i/p port has accumulated its samples to this o/p port's acc buf.
				//1) This i/p port is not waiting to accumulate any more samples to this o/p port's acc buf.
				//2) This o/p port does not expect any accumulation from this i/p port.
				//3) Check if a non-live i/p port is done accumulating to all o/p ports. If so, reset the Waiting flag.
				pCurrentInPort->outputReqPendingMask &= ~(1 << unOutPortID);
				(*pAccInPortsMask) |= 1 << unInPortID;
				pCurrentOutPort->inPortsWaitingToAccMask &= ~(1 << unInPortID);
				pCurrentOutPort->accBufAvailabilityMask  &= ~(1 << unInPortID);
				if((FALSE == pCurrentInPort->bIsPortLive) && (TRUE == pCurrentInPort->bIsNonLiveInputPortWaitingToAcc))
				{
					uint32_t activeOutPortsMask = pCurrentInPort->activeOutPortsMask;
					uint32_t unOutPortID_temp;
					MatrixOutPortInfoType   *pCurrentOutPort_temp;
					pCurrentInPort->bIsNonLiveInputPortWaitingToAcc = FALSE;
					while (activeOutPortsMask)
					{
						unOutPortID_temp = Q6_R_ct0_R(activeOutPortsMask);
						activeOutPortsMask ^= (1 << unOutPortID_temp);
						pCurrentOutPort_temp = me->outPortParams[unOutPortID_temp];
						if (pCurrentOutPort_temp->inPortsWaitingToAccMask & (1 << unInPortID))
						{
							pCurrentInPort->bIsNonLiveInputPortWaitingToAcc = TRUE;
						}
					}
				}

#ifdef MT_MX_EXTRA_DEBUG
				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu o/p port %lu Sending for acc. pAccBuf: 0x%p", me->mtMxID, unInPortID, unOutPortID, pAccBuf);
#endif

				//Proceed with normal operation from this output port's standpoint
				//Step 7: Apply Gain and accumulate into output port's accBuf
				MtMx_ApplyGainAndAccumulateInputToOutput(me, unInPortID, unOutPortID, pAccBuf);
				memset(pCurrentPspd->pPspdOutBuf, 0, pCurrentPspd->unPspdOutBufSize);
				pCurrentInPort->bHasFirstSampleAccumulated = TRUE;

				if(FALSE == me->bIsMxAtOperatingInBurstMode)
				{
					//If this is the top priority i/p port for this o/p port OR all ports have already accumulated to this o/p port, send the data down immediately!
					if(((pCurrentOutPort->inPortsTopPriorityMask) & (1 << unInPortID)) || (pCurrentOutPort->accInPortsMask == pCurrentOutPort->inPortsMask))
					{
						//Before sending data, check if this o/p is pending any acc from non-live i/p ports
						MxAt_OutPortToProcessIfNonLiveInputPortsNeedToAcc(me, unOutPortID);

						/*For non live input ports, at this point verify that the output is being triggered ONLY by the top prio input*/
						if((FALSE == pCurrentInPort->bIsPortLive)&&(!((pCurrentOutPort->inPortsTopPriorityMask) & (1 << unInPortID))))
						{
							//Do nothing
#ifdef MT_MX_EXTRA_DEBUG
							MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu Non Live, Non Prio i/p port %lu triggered acc. Doing nothing",
							me->mtMxID, unInPortID);
#endif
						}
						else
						{
							MxAt_ProcessBufQ(me, unOutPortID);
						}
					}
				}
				else //if(TRUE == me->bIsMxAtOperatingInBurstMode)
				{
					//Before sending data, check if this o/p is pending any acc from non-live i/p ports
					MxAt_OutPortToProcessIfNonLiveInputPortsNeedToAcc(me, unOutPortID);

					if(pCurrentOutPort->accInPortsMask == pCurrentOutPort->inPortsMask)
					{
						//If operating in Burst mode, send data as soon as all i/p ports have accumulated.
						//I/p ports do not have priority in Burst mode.
						MxAt_ProcessBufQ(me, unOutPortID);
					}
				}
			}
		}
	}

	//1)Reset only if the non-live i/p port is done accumulating to ALL o/p ports OR
	//2)Live i/p port --> Live port data gets dropped immediately or processed immediately
	if(((FALSE == pCurrentInPort->bIsPortLive) && (FALSE == pCurrentInPort->bIsNonLiveInputPortWaitingToAcc))||
			(TRUE  == pCurrentInPort->bIsPortLive))
	{
		pCurrentInPort->pWrLoc = pCurrentInPort->pStartLoc;
		pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;
		pCurrentInPort->bIsLocalBufFull = FALSE;
		pCurrentInPort->bIsLocalBufEmpty = TRUE;
	}
	return result;
}

void MtMx_RecfgInPorts(This_t *me, uint32_t unOutPortID)
{
	uint32_t unInPortID;
	ADSPResult result = ADSP_EFAILED;
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if ((NULL != pCurrentInPort) && (pCurrentInPort->strMask & (1 << unOutPortID)))
		{
			// Update some masks
			pCurrentInPort->outputReqPendingMask &= ~(1 << unOutPortID);
			pCurrentInPort->strMask &= ~(1 << unOutPortID);
			pCurrentOutPort->inPortsMask &= ~(1 << unInPortID);

			// check if this output port was a top priority port for this input port and recalculate
			if (unOutPortID == pCurrentInPort->unTopPrioOutPort)
			{
				MtMx_CalculateTopPriorityOutputPort(me, unInPortID);
			}

			// check if this input port was holding on to an EOS that was supposed to be routed to this output port
			if (pCurrentInPort->bIsEOSHeld && ((1 << unOutPortID) & pCurrentInPort->eosPendingMask))
			{
				// clear the output port's bit from eos pending mask
				pCurrentInPort->eosPendingMask &= ~(1 << unOutPortID);
				// check if eos pending mask became zero
				if ( !( pCurrentInPort->eosPendingMask) )
				{
					elite_msg_finish_msg( &(pCurrentInPort->myDataQMsg), ADSP_EOK );
					pCurrentInPort->bIsEOSHeld = FALSE;
					MtMx_AddInputPortToWaitMask(me,unInPortID);
				}
			}

			// Check if this input port had a Gap-less EOS pending. If so, mark it non-pending
			if (pCurrentInPort->bIsGaplessEOSMsgWaitingToBeSentDS && ((1 << unOutPortID) & pCurrentInPort->eosPendingMask))
			{
				// clear the output port's bit from eos pending mask
				pCurrentInPort->eosPendingMask &= ~(1 << unOutPortID);
				// check if eos pending mask became zero
				if ( !( pCurrentInPort->eosPendingMask) )
				{
					elite_msg_finish_msg( &(pCurrentInPort->gaplessEOSmsg), ADSP_EOK );
					pCurrentInPort->bIsGaplessEOSMsgWaitingToBeSentDS = FALSE;
				}
			}

         //raise rendered EoS event for EoS V2
			//held EoS clean up is done above.
         MtMx_CheckRaiseRenderedEoSv2(me, NULL, unInPortID, unOutPortID, FALSE);

			//check if the current input port is the only source of data to the output port that is closed.
			//i.e If pCurrentInport->strMask = 0, and if pSampleSlip is non-NULL (i.e asm loopback use case)
			//Flush that input port LocalBuffer and reset Sampleslip module.
			//If the currentInput port feeds into many output ports, then continue
			if(ADSP_EOK == MtMx_CheckSSLibStatus_4(me, unInPortID))
			{
				if (0 == pCurrentInPort->strMask)
				{
					// mark local buffer empty and reset local buffer pointers
					pCurrentInPort->bIsLocalBufEmpty = TRUE;
					pCurrentInPort->bIsLocalBufFull = FALSE;
					pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;
					MtMx_InitInPortLocalBufPtrs(me, unInPortID);

					if (INPUT_BUFFER_HELD == pCurrentInPort->bInBufStatus)
					{
						if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
						{
							MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: failed to return i/p data buffer", me->mtMxID, unInPortID);
						}

						pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
						pCurrentInPort->numBufReturned++;
					}
					MtMx_AddInputPortToWaitMask(me, unInPortID);

#ifdef MT_MX_EXTRA_DEBUG
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu i/p port %lu, #Buf Released: %lu, i/p port added to wait mask",
							me->mtMxID, unInPortID, pCurrentInPort->numBufReturned);
#endif
					(void)MtMx_InPortResetSampleSlipLibrary(me, unInPortID);
				}
			}
		}
	}
}

void AudDestroyMatrixMixerSvc (This_t* me)
{
	uint32_t unPortID;
	if (me)
	{
		// call utility function to destroy all input ports' data Qs. also delete all input ports' local buffers.
		for (unPortID = 0; (unPortID <= me->maxInPortID); unPortID++)
		{
			if ( (NULL != me->inPortParams[unPortID]) &&(INPUT_PORT_STATE_INACTIVE != me->inPortParams[unPortID]->inPortState) )
			{
				elite_svc_destroy_data_queue(me->inPortParams[unPortID]->inPortHandle.portHandle.dataQ);
				me->inPortParams[unPortID]->inPortHandle.portHandle.dataQ = NULL;
				MTMX_ALIGNED_FREE(me->inPortParams[unPortID]->pStartLoc);
			}

			//free input port state structure.
			if (me->inPortParams[unPortID])
			{
				MTMX_FREE (me->inPortParams[unPortID]);
			}
		}
		// call utility function to destroy cmd Q
		elite_svc_deinit_cmd_queue(me->serviceHandle.cmdQ);

		// call utility function to destroy all active buf Qs and their associated buffers
		// also free up memory for each o/p port's accBuf.
		for (unPortID = 0; (unPortID <= me->maxOutPortID); unPortID++ )
		{
			if ( (NULL != me->outPortParams[unPortID]) && (OUTPUT_PORT_STATE_INACTIVE != me->outPortParams[unPortID]->outPortState) )
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu o/p port %lu Destroying o/p BufQ, num of buffers %lu",
						me->mtMxID, unPortID, me->outPortParams[unPortID]->unNumOutputBufsPendingRelease);
				elite_svc_destroy_buf_queue(me->outPortParams[unPortID]->bufQ, me->outPortParams[unPortID]->unNumOutputBufsPendingRelease);
				me->outPortParams[unPortID]->unNumOutputBufsPendingRelease = 0;
				me->outPortParams[unPortID]->bIsOutPortPendingReleaseBuffers = FALSE;
				MTMX_ALIGNED_FREE(me->outPortParams[unPortID]->pAccBuf);
			}

			//free output port state structure.
			if (me->outPortParams[unPortID])
			{
				MTMX_FREE (me->outPortParams[unPortID]);
			}
		}

		qurt_elite_channel_destroy(&me->channel);

		MtMx_DeRegisterFromAdsppm(me);
	}
}

void MtMx_InPortToUpdateWaitMask(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];

	switch(pCurrentInPort->bIsPortLive)
	{
	case TRUE:
	{
		if (INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState)
		{
			if (!me->inPortParams[unInPortID]->bInBufStatus)
			{
				//Active live input port is not holding on to a buffer, add to wait mask
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Live i/p port %lu, Adding i/p port to wait mask", me->mtMxID, unInPortID);
				MtMx_AddInputPortToWaitMask(me, unInPortID);
			}

			//Update steadyStateInPortsMask
			if (pCurrentInPort->strMask && pCurrentInPort->activeOutPortsMask)
			{
				me->steadyStateInPortsMask |= (1 << unInPortID);
			}
			else
			{
				me->steadyStateInPortsMask &= ~(1 << unInPortID);
			}
		}
		else
		{
			if (INPUT_PORT_STATE_READY !=  pCurrentInPort->inPortState)
			{
				//Non-ready, non-active live ports need to be removed from wait mask
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Live i/p port %lu, Removing i/p port from wait mask", me->mtMxID, unInPortID);
				MtMx_RemoveInputPortFromWaitMask(me, unInPortID);

				//Update steadyStateInPortsMask
				me->steadyStateInPortsMask &= ~(1 << unInPortID);
			}
		}
		break;
	}
	case FALSE:
	{
		if (INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState && pCurrentInPort->bIsInPortBurstMode)
		{
			if (!me->inPortParams[unInPortID]->bInBufStatus)
			{
				//Active non-live burst-mode input port is not holding on to a buffer, add to wait mask
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Non-live i/p port %lu, Adding i/p port to wait mask", me->mtMxID, unInPortID);
				MtMx_AddInputPortToWaitMask(me, unInPortID);
			}

			//Update steadyStateInPortsMask
			if (pCurrentInPort->strMask && pCurrentInPort->activeOutPortsMask)
			{
				me->steadyStateInPortsMask |= (1 << unInPortID);
			}
			else
			{
				me->steadyStateInPortsMask &= ~(1 << unInPortID);
			}
		}
		else if (
				(pCurrentInPort->strMask) &&
				(pCurrentInPort->activeOutPortsMask) &&
				((INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState) ||
						(INPUT_PORT_STATE_READY == pCurrentInPort->inPortState))
		)
		{
			//Ready or active non-live ports connected to active output ports
			if (!me->inPortParams[unInPortID]->bInBufStatus)
			{
				//Ready or active non-live input port is not holding on to a buffer, add to wait mask
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Non-live i/p port %lu, Adding i/p port to wait mask", me->mtMxID, unInPortID);
				MtMx_AddInputPortToWaitMask(me, unInPortID);
			}

			//Update steadyStateInPortsMask
			me->steadyStateInPortsMask |= (1 << unInPortID);
		}
		else
		{
			if (INPUT_PORT_STATE_READY !=  pCurrentInPort->inPortState &&
					INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState)
			{
				//Non-ready/Non-inactive ports need to be removed from wait mask
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Non-live i/p port %lu, Removing i/p port from wait mask", me->mtMxID, unInPortID);
				MtMx_RemoveInputPortFromWaitMask(me, unInPortID);
			}

			//Update steadyStateInPortsMask
			me->steadyStateInPortsMask &= ~(1 << unInPortID);
		}
		break;
	}
	}

	if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
	{
		MxAt_UpdateBurstModeOfMxAt(me);
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, i/p port %lu MtMx_InPortToUpdateWaitMask. Burstmode? %d",
				me->mtMxID, unInPortID, (int)me->bIsMxAtOperatingInBurstMode);
	}

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, i/p port %lu MtMx_InPortToUpdateWaitMask. SteadyStateInPortsMask = %lu",
			me->mtMxID, unInPortID, me->steadyStateInPortsMask);
}

void MtMx_OutPortToUpdateWaitMask(This_t *me, uint32_t unOutPortID)
{
	uint32_t unInPortID;
	MatrixInPortInfoType    *pCurrentInPort;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if ((NULL != pCurrentInPort) &&((1 << unOutPortID) & (pCurrentInPort->strMask)))
		{
			switch (pCurrentInPort->bIsPortLive)
			{
			case TRUE:
			{
				//Update steadyStateInPortsMask
				if (
						(pCurrentInPort->strMask) &&
						(pCurrentInPort->activeOutPortsMask) &&
						(INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState)
				)
				{
					me->steadyStateInPortsMask |= (1 << unInPortID);
				}
				else
				{
					me->steadyStateInPortsMask &= ~(1 << unInPortID);
				}
				break;
			}
			case FALSE:
			{
				if (INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState && pCurrentInPort->bIsInPortBurstMode)
				{
					if (!me->inPortParams[unInPortID]->bInBufStatus)
					{
						//Active non-live burst-mode input port is not holding on to a buffer, add to wait mask
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Non-live i/p port %lu, Adding i/p port to wait mask", me->mtMxID, unInPortID);
						MtMx_AddInputPortToWaitMask(me, unInPortID);
					}

					//Update steadyStateInPortsMask
					if (pCurrentInPort->strMask && pCurrentInPort->activeOutPortsMask)
					{
						me->steadyStateInPortsMask |= (1 << unInPortID);
					}
					else
					{
						me->steadyStateInPortsMask &= ~(1 << unInPortID);
					}
				}
				else if(
						(pCurrentInPort->strMask) &&
						(pCurrentInPort->activeOutPortsMask) &&
						(
								(INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState) ||
								(INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER == pCurrentInPort->inPortState)
						)
				)
				{
					if (!pCurrentInPort->bInBufStatus)
					{
						//An active input port is not holding on to a buffer, add to wait mask
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Non-live i/p port %lu, Adding i/p port to wait mask", me->mtMxID, unInPortID);
						MtMx_AddInputPortToWaitMask(me, unInPortID);
					}

					//Update steadyStateInPortsMask
					me->steadyStateInPortsMask |= (1 << unInPortID);
				}
				else
				{
					//Do not remove input port from wait mask if it is in READY state
					if (INPUT_PORT_STATE_READY !=  pCurrentInPort->inPortState &&
							INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState)
					{
						//Non-ready/Non-inactive ports need to be removed from wait mask
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Non-live i/p port %lu, Removing i/p port from wait mask", me->mtMxID, unInPortID);
						MtMx_RemoveInputPortFromWaitMask(me, unInPortID);
					}

					//Update steadyStateInPortsMask
					me->steadyStateInPortsMask &= ~(1 << unInPortID);
				}
				break;
			}
			}
		}
	}

	if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
	{
		MxAt_UpdateBurstModeOfMxAt(me);
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, o/p port %lu MtMx_OutPortToUpdateWaitMask. Burstmode? %d",
				me->mtMxID, unOutPortID, (int)me->bIsMxAtOperatingInBurstMode);
	}

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, o/p port %lu MtMx_OutPortToUpdateWaitMask. SteadyStateInPortsMask = %lu",
			me->mtMxID, unOutPortID, me->steadyStateInPortsMask);
}

void MtMx_InPortToUpdateItsActiveOutPortsMask(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	MatrixOutPortInfoType *pCurrentOutPort;
	uint32_t unOutPortID;
	uint32_t unStrMask = pCurrentInPort->strMask;
	while(unStrMask)
	{
		unOutPortID = Q6_R_ct0_R(unStrMask);
		unStrMask ^= 1 << unOutPortID;
		pCurrentOutPort = me->outPortParams[unOutPortID];

		if(NULL != pCurrentOutPort)
		{
			if (OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState)
			{
				pCurrentInPort->activeOutPortsMask |= (1 << unOutPortID);
			}
			else
			{
				pCurrentInPort->activeOutPortsMask &= ~(1 << unOutPortID);
			}

			MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, i/p port %lu o/p port %lu: MtMx_InPortToUpdateItsActiveOutPortsMask: strMask = %lu, activeOutPortsMask = %lu",
					me->mtMxID, unInPortID, unOutPortID, pCurrentInPort->strMask, pCurrentInPort->activeOutPortsMask);
		}
	}
}

void MtMx_OutPortToUpdateActiveOutPortsMasks(This_t *me, uint32_t unOutPortID)
{
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];
	uint32_t unInPortID;
	uint32_t unInPortsMask = pCurrentOutPort->inPortsMask;

	while(unInPortsMask)
	{
		unInPortID = Q6_R_ct0_R(unInPortsMask);
		unInPortsMask ^= 1 << unInPortID;
		pCurrentInPort = me->inPortParams[unInPortID];

		if(NULL != pCurrentInPort)
		{
			if (OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState)
			{
				pCurrentInPort->activeOutPortsMask |= (1 << unOutPortID);
			}
			else
			{
				pCurrentInPort->activeOutPortsMask &= ~(1 << unOutPortID);
			}

			MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, i/p port %lu o/p port %lu: MtMx_OutPortToUpdateItsActiveOutPortsMask: inPortsMask = %lu, activeOutPortsMask = %lu",
					me->mtMxID, unInPortID, unOutPortID, pCurrentOutPort->inPortsMask, pCurrentInPort->activeOutPortsMask);
		}
	}
}

void MxAt_OutPortToUpdateOutputReqPendingMasks(This_t *me, uint32_t unOutPortID)
{
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];
	uint32_t unInPortID;
	uint32_t unInPortsMask = pCurrentOutPort->inPortsMask;

	while(unInPortsMask)
	{
		unInPortID = Q6_R_ct0_R(unInPortsMask);
		unInPortsMask ^= 1 << unInPortID;
		pCurrentInPort = me->inPortParams[unInPortID];

		if(NULL != pCurrentInPort)
		{
			if (OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState)
			{
				pCurrentInPort->outputReqPendingMask |= (1 << unOutPortID);
			}
			else
			{
				pCurrentInPort->outputReqPendingMask &= ~(1 << unOutPortID);
			}
		}
	}
}

void MxAr_ProcessPendingPullOutPorts(This_t *me)
{
	MatrixOutPortInfoType   *pCurrentOutPort;
	uint32_t unOutPortID;

	for (unOutPortID = 0; unOutPortID <= me->maxOutPortID; unOutPortID++)
	{
		if((NULL != me->outPortParams[unOutPortID])
				&& (OUTPUT_PORT_STATE_INACTIVE != me->outPortParams[unOutPortID]->outPortState))
		{
			pCurrentOutPort = me->outPortParams[unOutPortID];
			if(
					(TRUE == pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut) &&
					(0 != pCurrentOutPort->accInPortsMask)
			)
			{
				//It is safe to call this because for pull ports, the flag bIsOutPortWaitingForRscsToSendOut
				//is set to TRUE only when there is atleast 1 buffer pending in the o/p Q waiting to be sent out
				MtMx_AddOutputPortToWaitMask(me, unOutPortID);
				MxAr_ProcessBufQ(me, unOutPortID);

#ifdef MT_MX_EXTRA_DEBUG
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu reg o/p port %lu is now has acc. Waking up o/p Q handler", me->mtMxID, unOutPortID);
#endif

			}
		}
	}
}

ADSPResult MtMx_DuplicateEosMsg( This_t *me, uint32_t unInPortID, uint32_t unOutPortID, elite_msg_any_t *pInMsg, elite_msg_any_t *pOutMsg)
{
	bool_t bIsInternalEOS = FALSE;

	elite_msg_data_eos_apr_t *pEosPayload = (elite_msg_data_eos_apr_t*)(pInMsg->pPayload);

	if ( (pInMsg->unOpCode != ELITE_DATA_EOS)
			||
			(
					(pEosPayload->unEosFormat != ELITEMSG_DATA_EOS_APR)  &&
					(pEosPayload->unEosFormat != ELITEMSG_DATA_DTMF_EOT) &&
					(pEosPayload->unEosFormat != ELITEMSG_DATA_TX_EOS)   &&
					(pEosPayload->unEosFormat != ELITEMSG_DATA_RESET_SESSION_CLK) &&
					(pEosPayload->unEosFormat != ELITEMSG_DATA_GAPLESS_TERMINATION_EOS)
			)
	)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_DuplicateEosMsg: Invalid opcode 0x%lx, fmt 0x%lx ",
				pInMsg->unOpCode, pEosPayload->unEosFormat);
		return ADSP_EBADPARAM;
	}

	if(ELITEMSG_DATA_TX_EOS == pEosPayload->unEosFormat)
	{
		bIsInternalEOS = TRUE;
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_DuplicateEosMsg bIsInternalEOS is TRUE!");
	}

	/** In EOS V2, matrix sends rendered_eos event to the client after all/any connected devices render EOS.*/
   qurt_elite_queue_t *respQPtr = NULL;
   uint32_t clientToken = 0;
   if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID) //do this only for Rx matrix. For Tx matrix, in ADM loopback dont do this.
   {
      if (ASM_DATA_EVENT_RENDERED_EOS_V2 == pEosPayload->eosInfo.event_opcode)
      {
         respQPtr = me->serviceHandle.cmdQ;
         clientToken = MtMx_GetClientTokenFromInOutPortId(unInPortID, unOutPortID);
      }
   }

	uint32_t unPayLoadBufSize = (bIsInternalEOS)?
			(sizeof(elite_msg_data_eos_header_t)):(sizeof(elite_msg_data_eos_apr_t));

	ADSPResult res = elite_msg_create_msg( pOutMsg,
			&unPayLoadBufSize,
			ELITE_DATA_EOS,
			respQPtr,
			clientToken,
			0);

	if (ADSP_FAILED( res ))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_DuplicateEosMsg Failed to create msg! Error: %d\n", res);
		return res;
	}

	// copy everything starting from unEosFormat
	if (TRUE == bIsInternalEOS)
	{
	   elite_msg_data_eos_header_t *pOutEoS = (elite_msg_data_eos_header_t*)(pOutMsg->pPayload);

		memscpy( &(pOutEoS->unEosFormat), sizeof(uint32_t),
				&(((elite_msg_data_eos_header_t*)(pInMsg->pPayload))->unEosFormat),
				sizeof(uint32_t));
	}
	else
	{
	   elite_msg_data_eos_apr_t *pOutEoS = (elite_msg_data_eos_apr_t*)(pOutMsg->pPayload);

		pOutEoS->unEosFormat = (pEosPayload->unEosFormat);
		pOutEoS->eosInfo = (pEosPayload->eosInfo);
	}

	if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID) //Tx Mtmx
   {
	   MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	   if (pCurrentInPort->bIsAdmLoopback)
	   {
         //if tx mtmx input port is ADM loopback, then the EoS V2 can be acked back right away.
         //EOS needs to be propagated (to reset PPs), hence transform it before sending.
         if (ASM_DATA_EVENT_RENDERED_EOS_V2 == pEosPayload->eosInfo.event_opcode)
         {
            elite_msg_data_eos_apr_t *pOutEoS = (elite_msg_data_eos_apr_t*)(pOutMsg->pPayload);

            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_DuplicateEosMsg i/p port %lu o/p port %lu. transforming EoS V2 in ADM loopback mode as TX_EOS", unInPortID, unOutPortID);
            pOutEoS->unEosFormat = ELITEMSG_DATA_TX_EOS;
            pOutEoS->eosInfo.event_opcode = 0;
            pOutEoS->pResponseQ = NULL;
         }
	   }
   }

	return ADSP_EOK;
}

void MtMx_UpdateInPortsMaskToReflectStrMaskChange(This_t *me, uint32_t unInPortID, uint32_t unMode, uint32_t unBitPosition)
{
	MatrixOutPortInfoType   *pCurrentOutPort;
	uint32_t                  unOutPortID;
	switch (unMode)
	{
	case MT_MX_INPUT_PORT_STRMASK_BIT_ADDED:
	{
		pCurrentOutPort = me->outPortParams[unBitPosition];
		pCurrentOutPort->inPortsMask |= (1 << unInPortID);
		break;
	}
	case MT_MX_INPUT_PORT_STRMASK_BIT_REMOVED:
	{
		//if bit[unBitPosition] from strMask is cleared, inPortsMask of output port with ID = unBitPosition will change
		pCurrentOutPort = me->outPortParams[unBitPosition];
		pCurrentOutPort->inPortsMask &= ~(1 << unInPortID);
		break;
	}
	case MT_MX_INPUT_PORT_STRMASK_CLEARED:
	{
		//if the strMask was reset to 0, clear corresponding bits from required inportsmasks
		for (unOutPortID = 0; unOutPortID <= me->maxOutPortID; unOutPortID++)
		{
			pCurrentOutPort = me->outPortParams[unOutPortID];
			if (NULL != pCurrentOutPort)
			{
				pCurrentOutPort->inPortsMask &= ~(1 << unInPortID);
			}
		}
		break;
	}
	}
}

void MtMx_UpdateStrMaskToReflectInPortsMaskChange(This_t *me, uint32_t unOutPortID, uint32_t unMode, uint32_t unBitPosition)
{
	MatrixInPortInfoType    *pCurrentInPort;
	uint32_t                  unInPortID;
	switch (unMode)
	{
	case MT_MX_OUTPUT_PORT_INPORTSMASK_BIT_ADDED:
	{
		break;
	}
	case MT_MX_OUTPUT_PORT_INPORTSMASK_BIT_REMOVED:
	{
		break;
	}
	case MT_MX_OUTPUT_PORT_INPORTSMASK_CLEARED:
	{
		//if the inPortsMask was reset to 0, clear corresponding bits from required strmasks
		for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
		{
			pCurrentInPort = me->inPortParams[unInPortID];
			if (NULL != pCurrentInPort)
			{
				pCurrentInPort->strMask &= ~(1 << unOutPortID);
				pCurrentInPort->activeOutPortsMask &= ~(1 <<unOutPortID);
			}
		}
		break;
	}
	}
}

static void MtMx_GainAndAccOnEqualNumChannels(uint16_t unNumSamplesGainAndAccPerCh,
                uint32_t unNumChannels,
                MatrixInPortInfoType *pCurrentInPort,
                MatrixOutPortInfoType *pCurrentOutPort,
                int8_t *pInputBuf, int64_t *pOutputBuf,
                uint16_t *pCurrentGain)
{
    uint32_t i, j, samp;
    int16_t  *ptr_local_buf_16, *ptr_local_buf_start_16;
    int32_t  post_gain_sample_32, *ptr_local_buf_32, *ptr_local_buf_start_32;
    int64_t  post_gain_sample_64, *ptr_local_buf_64;
    int32_t  *ptr_out_buf_32;
    int64_t  *ptr_out_buf_64;
    int32_t  curr_gain_q16, curr_gain_q28;
    int64_t  combined_gain, combined_input, temp_res_64;

    bit_conv_mode_t bit_conv_type = MtMx_IdentifyBitConvType(pCurrentInPort->unBitwidth, pCurrentOutPort->unBitwidth);

    /* If the number of bytes per sample for input port and output port are equal */
    if(pCurrentInPort->unBytesPerSample == pCurrentOutPort->unBytesPerSample)
    {
        /* If 2 bytes per sample (Q15 - Q15) */
        if(MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentInPort->unBytesPerSample)
        {
            ptr_local_buf_start_16 = (int16_t *)pInputBuf;
            for (i = 0; i < unNumChannels; i++)
            {
                /* Advance the input and output buffers pointers to the
                   next channel
                */
                ptr_local_buf_16 = (ptr_local_buf_start_16 + (i * pCurrentInPort->unInPortPerChBufSize));
                ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                samp = unNumSamplesGainAndAccPerCh;

                /** If the output pointer is not 8-byte aligned, process one
                 *  sample to align the pointer.
                 */
                if (((uint32_t)ptr_out_buf_32) & 7)
                {
                    /* Apply the gain on sample and shift for gain in Q13 */
                    post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + MT_MX_RND_FACTOR_GAIN_SHIFT),MT_MX_GAIN_SHIFT_FACT));

                    /* Accumulate the gain applied 32-bit sample */
                    *ptr_out_buf_32 = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;

                    /* Reduce the sample count by one*/
                    samp = samp - 1;
                }

                /** Vector 32x16 multiplication results in a 48 bit result that
                 *  is right shifted by 16 bits Since the gain is only in Q13,
                 *  account for the extra 3 bit right shift.
                 *
                 *  1. Gain is left shifted by maximum 3 bits.
                 */

                /* Scale the gain up from Q13 -> Q16 */
                curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 3;

                /* Combine two int32_t's into one int64_t */
                combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                ptr_out_buf_64 = (int64_t *)ptr_out_buf_32;

                /* Process two input samples per loop iteration */
                for (; samp >= 2; samp -= 2)
                {
                    /* Combine two, 16 to 32-bit sign extended hwords in one 64-bit dword */
                    combined_input = Q6_P_combine_RR(ptr_local_buf_16[1], ptr_local_buf_16[0]);
                    ptr_local_buf_16 += 2;

                    /* Apply the gain */
                    temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(combined_gain, combined_input);

                    /* Accumulate the gain applied samples */
                    *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                    ptr_out_buf_64++;
                }

                /* If the number of samples are odd */
                if(samp)
                {
                    ptr_out_buf_32 = (int32_t *)ptr_out_buf_64;

                    /* Apply the gain on sample and shift for gain in Q13 */
                    post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + MT_MX_RND_FACTOR_GAIN_SHIFT),MT_MX_GAIN_SHIFT_FACT));

                    /* Accumulate the gain applied 32-bit sample */
                    *ptr_out_buf_32 = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;
                }
                /* Increment the pointer to fetch the next channel's gain value */
                pCurrentGain++;
            } /* unNumChannels for loop */
        } /* Q15 - Q15 */
        else /* 4 bytes per sample */
        {
            /** The number of bytes per sample determines the processing bit
             *  width. But, the actual bit widths can be different. Hence
             *  account for potential up/down conversion combinations with
             *  24 or 32 as input & output port bit widths
             */

            int32_t local_shift_factor           = MT_MX_GAIN_SHIFT_FACT; /* Default shift factor for gain mpy in Q13 */
            int32_t local_rnd_factor             = 0;
            int32_t local_shift_factor_unit_gain = 0;
            int32_t local_rnd_factor_unit_gain   = 0;

            ptr_local_buf_start_32 = (int32_t *)pInputBuf;

            /* Identify shift/rounding factors based on the bit conv type */
            if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type)
            {
                local_shift_factor           -= (BYTE_INTER_CONV_SHIFT_FACT); /* Right shift from Q31 -> Q27 */
                local_shift_factor_unit_gain -= (BYTE_INTER_CONV_SHIFT_FACT);
            }
            else if(BIT_CONV_IN24_TO_OUT32 == bit_conv_type)
            {
                local_shift_factor           += (BYTE_INTER_CONV_SHIFT_FACT); /* Left shift from Q27 -> Q31 */
                local_shift_factor_unit_gain += (BYTE_INTER_CONV_SHIFT_FACT);
            }
            else
            {
                local_shift_factor           += 0; /* Retain Q format */
                local_shift_factor_unit_gain += 0;
            }

            local_rnd_factor           = MtMx_CalculateRoundingFactor(local_shift_factor);
            local_rnd_factor_unit_gain = MtMx_CalculateRoundingFactor(local_shift_factor_unit_gain);

            if(MT_MX_BIT_WIDTH_24 == pCurrentOutPort->unBitwidth)/* output in Q27 AND in bytes = out bytes */
            {
                for (i = 0; i < unNumChannels; i++)
                {
                    /* If gain < unit gain, use vector operations */
                    if(*pCurrentGain < MT_MX_UNITY_GAIN)
                    {
                        /* Advance the input and output buffers pointers to the
                           next channel
                         */
                        ptr_local_buf_32 = (ptr_local_buf_start_32 + (i * pCurrentInPort->unInPortPerChBufSize));
                        ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                        samp = unNumSamplesGainAndAccPerCh;

                        /** If the output pointer is not 8-byte aligned, process one
                         *  sample to align the pointer.
                         */
                        if (((uint32_t)ptr_out_buf_32) & 7)
                        {
                            /* Apply the gain on sample */
                            post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                            /* Shift to Q27 Format and saturate to 32 bits */
                            post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                            /* Accumulate the gain applied 32-bit sample */
                            (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                            ptr_out_buf_32++;

                            /* Reduce sample count by 1 */
                            samp = samp - 1;
                        }

                        /** Vector 32x16 multiplication results in a 48 bit result that
                         *  is right shifted by 16 bits Since the gain is only in Q13,
                         *  account for the extra 3 bit right shift.
                         *
                         *  1. Gain is left shifted by maximum 2 bits. Since it is less
                         *  than unity we are not losing precision.
                         *
                         *  2. Multiply result is shifted left by 1 bit before the right
                         *  shift as part of the intrinsic itself.
                         */

                        /* Scale the gain up from Q13 -> Q15 */
                        curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 2;

                        /* Combine int32_t into int64_t */
                        combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                        ptr_local_buf_64 = (int64_t *)ptr_local_buf_32;
                        ptr_out_buf_64   = (int64_t *)ptr_out_buf_32;

                        if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type) /* Q31 -> Q27 */
                        {
                            /* Process two input samples per loop iteration */
                            for (; samp >= 2; samp -= 2)
                            {
                                /* Apply the gain */
                                temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);

                                /* Add the rounding factor before truncation to Q27 */
                                temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q27);

                                /* Right shift from Q31 to Q27 */
                                temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_INTER_CONV_SHIFT_FACT);

                                /* Accumulate the gain applied samples */
                                *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                ptr_out_buf_64++;
                            }
                        }
                        else  /* Retain Q27 Format */
                        {
                            for (; samp>=2; samp-=2)
                            {
                                /* Apply the gain */
                                temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);
                                
                                /* Accumulate the gain applied samples */
                                *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                ptr_out_buf_64++;
                            }
                        }

                        /* If the number of samples are odd */
                        if(samp)
                        {
                            ptr_out_buf_32   = (int32_t *)ptr_out_buf_64;
                            ptr_local_buf_32 = (int32_t *)ptr_local_buf_64;

                            /* Apply the gain on sample */
                            post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                            /* Shift to Q27 format and saturate to 32 bits */
                            post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                            /* Accumulate the gain applied 32-bit sample */
                            (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                            ptr_out_buf_32++;
                        }
                    } /* gain < unit gain for output in Q27 AND in bytes = out bytes */

                    else if(MT_MX_UNITY_GAIN == *pCurrentGain)
                    {
                        /** This configuration is the most commonly used scenario for
                         *  audio use cases. Since the gain = unity, optimize by
                         *  skipping multiplication and taking advantage of vectorized
                         *  operations
                         */
                        samp = unNumSamplesGainAndAccPerCh;

                        /* Advance the input and output buffers pointers to the
                           next channel
                         */
                        ptr_local_buf_32 = (ptr_local_buf_start_32 + (i * pCurrentInPort->unInPortPerChBufSize));
                        ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                        /** If the output pointer is not 8-byte aligned, process one
                         *  sample to align the pointer.
                         */
                        if (((uint32_t)ptr_out_buf_32) & 7)
                        {
                            /* Shift to Q27 format and saturate to 32 bits */
                            post_gain_sample_64 = s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain), local_shift_factor_unit_gain);
                            post_gain_sample_32 = s32_saturate_s64(post_gain_sample_64);

                            /* Accumulate the 32-bit sample */
                            (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                            ptr_out_buf_32++;

                            /* Reduce sample count by 1 */
                            samp = samp - 1;
                        }

                        ptr_local_buf_64 = (int64_t *)ptr_local_buf_32;
                        ptr_out_buf_64   = (int64_t *)ptr_out_buf_32;

                        if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type) /* Q31 -> Q27 */
                        {
                            /* Process two input samples per loop iteration */
                            for (; samp >= 2; samp -= 2)
                            {
                                /* Add the rounding factor before truncation to Q27 */
                                temp_res_64 = Q6_P_vaddw_PP_sat(*ptr_local_buf_64++, MT_MX_RND_FACTOR_V_Q31_TO_Q27);

                                /* Right shift from Q31 to Q27 */
                                temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_INTER_CONV_SHIFT_FACT);

                                /* Accumulate the samples */
                                *ptr_out_buf_64= Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                ptr_out_buf_64++;
                            }
                        }
                        else  /* Retain Q27 Format */
                        {
                            for (; samp>=2; samp-=2)
                            {
                                *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, *ptr_local_buf_64++);
                                ptr_out_buf_64++;
                            }
                        }

                        /* If the number of samples are odd */
                        if(samp)
                        {
                            ptr_out_buf_32   = (int32_t *)ptr_out_buf_64;
                            ptr_local_buf_32 = (int32_t *)ptr_local_buf_64;

                            /* Shift to Q27 format and saturate to 32 bits */
                            post_gain_sample_64 = s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain), local_shift_factor_unit_gain);
                            post_gain_sample_32 = s32_saturate_s64(post_gain_sample_64);

                            /* Accumulate the 32-bit sample */
                            (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                            ptr_out_buf_32++;
                        }
                    } /* gain = unity gain for output in Q27 AND in bytes = out bytes */

                    else /* gain > unity gain */
                    {
                        /* Advance the input and output buffers pointers to the
                           next channel
                        */
                        ptr_local_buf_32 = (ptr_local_buf_start_32 + (i * pCurrentInPort->unInPortPerChBufSize));
                        ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                        /** 1. Apply the gain on the input sample
                         *  2. Round off the sample with appropriate shift/rounding for
                         *  inter Q format conversions along with gain shift.
                         */
                        for (j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                        {
                            /* Apply the gain on sample */
                            post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                            /* Shift to Q27 format and saturate to 32 bits */
                            post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor ), local_shift_factor));

                            /* Accumulate the gain applied 32-bit sample */
                            (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                            ptr_out_buf_32++;
                        }
                    } /* gain > unity gain for output in Q27 AND in bytes = out bytes*/

                    /* Increment the pointer to fetch the next channel's gain value */
                    pCurrentGain++;
                } /* unNumChannels for loop */
            } /* output port BW == 24*/

            else /* output in Q31 for in bytes = out bytes */
            {
                for (i = 0; i < unNumChannels; i++)
                {
                    /* Use scalar operations for accumulation to 64-bit buffer */
                    ptr_local_buf_32 = (ptr_local_buf_start_32 + (i * pCurrentInPort->unInPortPerChBufSize));
                    ptr_out_buf_64   = (pOutputBuf + (i * pCurrentOutPort->unOutPortPerChBufSize));

                    for (j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                    {
                        /* Apply the gain on sample */
                        post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                        /* Shift to Q31 format after rounding */
                        post_gain_sample_64 = s64_shl_s64((post_gain_sample_64 + local_rnd_factor ), local_shift_factor);

                        /* Accumulate the gain applied 64-bit sample */
                        *ptr_out_buf_64 = Q6_P_add_PP(*ptr_out_buf_64, post_gain_sample_64);
                        ptr_out_buf_64++;
                    }
                    /* Increment the pointer to fetch the next channel's gain value */
                    pCurrentGain++;
                } /* unNumChannels for loop */
            } /*output port BW == 32 */
        } /* 4 bytes per sample*/
    } /* in bytes = out bytes*/
    else /* Unequal number of bytes */
    {
        /* If output port bytes per sample is greater than the input port bytes per sample*/
        if(pCurrentOutPort->unBytesPerSample > pCurrentInPort->unBytesPerSample)
        {
            /** If output port bytes per sample is greater than the input
             *  port,
             *  1. Apply gain on 16-bit input port samples
             *  2. Up convert to appropriate Q format (Q27/Q31)
             *  3. Accumulate to output port buffer.
             */
            int32_t local_shift_factor = 0;
            int32_t local_rnd_factor   = 0;

            /* Identify shift/rounding factors for Left shift from Q15 -> Qn */
            if(BIT_CONV_IN16_TO_OUT24 == bit_conv_type)
            {
                local_shift_factor = MT_MX_GAIN_SHIFT_FACT + BYTE_UPDOWN_CONV_SHIFT_FACT;
            }
            else /* BIT_CONV_IN16_TO_OUT32 */
            {
                local_shift_factor = MT_MX_GAIN_SHIFT_FACT + BYTE_UPDOWN_CONV_SHIFT_FACT_32;
            }

            local_rnd_factor = MtMx_CalculateRoundingFactor(local_shift_factor);

            ptr_local_buf_start_16 = (int16_t *)pInputBuf;

            if(BIT_CONV_IN16_TO_OUT24 == bit_conv_type) /* Q15 -> Q27 */
            {
                for (i = 0; i < unNumChannels; i++)
                {
                    /* Advance the input and output buffers pointers
                       to the next channel
                     */
                    ptr_local_buf_16 = (ptr_local_buf_start_16 + (i * pCurrentInPort->unInPortPerChBufSize));
                    ptr_out_buf_32   = ((int32_t*)pOutputBuf + (i * pCurrentOutPort->unOutPortPerChBufSize));

                    samp = unNumSamplesGainAndAccPerCh;

                    /** If the output pointer is not 8-byte aligned, process one
                     *  sample to align the pointer.
                     */
                    if (((uint32_t)ptr_out_buf_32) & 7)
                    {
                        /* Apply the gain on sample */
                        post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                        /* Shift to Q27 format and saturate to 32 bits */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                        /* Accumulate the gain applied 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;

                        /* Reduce the sample count by one*/
                        samp = samp - 1;
                    }

                    /** This is a special case where the gain is shifted to Q28.
                     *  Doing this avoids consecutive right shift (as part of mpy)
                     *  and left shift (for up conversion).
                     *
                     *  The mpy operation translates to ((Q28 gain * Q15 sample)  >>
                     *  16). This results in a Q27 number directly
                     *
                     *  As a result, achieve higher signal precision in the
                     *  converted sample and avoid an explicit left shift for bit
                     *  conversion.
                     */

                    /* Scale the gain up from Q13 -> Q28 */
                    curr_gain_q28 = (Q6_R_zxth_R(*pCurrentGain)) << 15;

                    /* Combine int32_t into int64_t */
                    combined_gain = Q6_P_combine_RR(curr_gain_q28, curr_gain_q28);

                    /* Process two input samples per loop iteration */
                    ptr_out_buf_64 = (int64_t *)ptr_out_buf_32;

                    for (; samp >=2; samp -=2)
                    {
                        /* Combine two 16 to 32-bit sign extended hwords in one 64-bit dword */
                        combined_input   =  Q6_P_combine_RR(ptr_local_buf_16[1], ptr_local_buf_16[0]);
                        ptr_local_buf_16 += 2;

                        /* Apply the gain */
                        temp_res_64  = Q6_P_vmpyweh_PP_rnd_sat(combined_gain, combined_input);
                            /* Note that result is already in Q27 format*/

                        /* Accumulate the gain applied samples */
                        *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                        ptr_out_buf_64++;
                    }

                    /* If the number of samples are odd */
                    if(samp)
                    {
                        ptr_out_buf_32 = (int32_t *)ptr_out_buf_64;

                        /* Apply the gain on sample */
                        post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                        /* Shift to Q27 Format and saturate to 32 bits */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                        /* Accumulate the gain applied 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;
                    }
                    /* Increment the pointer to fetch the next channel's gain value */
                    pCurrentGain++;
                } /* unNumChannels for loop */
            } /* Q15 -> Q27 */
            else
            {
                for (i = 0; i < unNumChannels; i++)
                {
                    /* Advance the input and output buffers pointers
                       to the next channel
                     */
                    ptr_local_buf_16 = (ptr_local_buf_start_16 + (i * pCurrentInPort->unInPortPerChBufSize));
                    ptr_out_buf_64   = (pOutputBuf + (i * pCurrentOutPort->unOutPortPerChBufSize));

                    /* Use scalar operations for accumulation to 64-bit buffer */
                    for (j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                    {
                        /* Apply the gain on sample */
                        post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                        /* Shift to Q31 format after rounding */
                        post_gain_sample_64 = s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor);

                        /* Accumulate the gain applied 64-bit sample */
                        (*ptr_out_buf_64) = Q6_P_add_PP(*ptr_out_buf_64, post_gain_sample_64);
                        ptr_out_buf_64++;
                    }
                    /* Increment the pointer to fetch the next channel's gain value */
                    pCurrentGain++;
                } /* unNumChannels for loop */
            } /* Q15 -> Q31 */
        }/* in bytes > out bytes */
        else /* pCurrentOutPort->unBytesPerSample < pCurrentInPort->unBytesPerSample */
        {
            /**
             *    1. Gain is applied on 32 bit input port samples
             *    2. 64 bit intermediate result is saturated to 32 bits
             */

            ptr_local_buf_start_32 = (int32_t *)pInputBuf;

            int32_t local_shift_factor           = MT_MX_GAIN_SHIFT_FACT; /* Default shift factor for gain mpy in Q13 */
            int32_t local_rnd_factor             = 0;
            int32_t local_shift_factor_unit_gain = 0;
            int32_t local_rnd_factor_unit_gain   = 0;

            /* Identify shift/rounding factors for Right shift from Qn -> Q15 */
            if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type)
            {
                local_shift_factor           -= BYTE_UPDOWN_CONV_SHIFT_FACT; /* Right shift from Q27 - Q15 */
                local_shift_factor_unit_gain -= BYTE_UPDOWN_CONV_SHIFT_FACT;
            }
            else
            {
                local_shift_factor           -= BYTE_UPDOWN_CONV_SHIFT_FACT_32; /* Right shift from Q31 - Q15 */
                local_shift_factor_unit_gain -= BYTE_UPDOWN_CONV_SHIFT_FACT_32;
            }

            local_rnd_factor           = MtMx_CalculateRoundingFactor(local_shift_factor);
            local_rnd_factor_unit_gain = MtMx_CalculateRoundingFactor(local_shift_factor_unit_gain);

            for (i = 0; i < unNumChannels; i++)
            {
                if(*pCurrentGain < MT_MX_UNITY_GAIN)
                {
                    /* Advance the input and output buffers pointers
                        to the next channel
                     */
                    ptr_local_buf_32 = (ptr_local_buf_start_32 + (i * pCurrentInPort->unInPortPerChBufSize));
                    ptr_out_buf_32   = ((int32_t*)pOutputBuf   + (i * pCurrentOutPort->unOutPortPerChBufSize));

                    samp = unNumSamplesGainAndAccPerCh;

                    /** If the output pointer is not 8-byte aligned, process one
                     *  sample to align the pointer.
                     */
                    if (((uint32_t)ptr_out_buf_32) & 7)
                    {
                        /* Apply the gain on sample */
                        post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                        /* Right shift to Q15 format */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                        /* Accumulate the gain applied 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;

                        /* Reduce sample count by 1 */
                        samp = samp - 1;
                    }

                    /* Scale the gain up from Q13 -> Q15 */
                    curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 2;

                    /* Combine int32_t into int64_t */
                    combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                    /* Process two input samples per loop iteration */
                    ptr_local_buf_64 = (int64_t *)ptr_local_buf_32;
                    ptr_out_buf_64   = (int64_t *)ptr_out_buf_32;

                    if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type) /* Q27 -> Q15 */
                    {
                        for(; samp >=2; samp-=2)
                        {
                            /* Apply the gain */
                            temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);

                            /* Add the rounding factor before shift to Q15 */
                            temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q27_TO_Q15);

                            /* Right shift from Q27 to Q15 */
                            temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT);

                            /* Accumulate the gain applied samples */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                            ptr_out_buf_64++;
                        }
                    }
                    else  /* Q31 -> Q15 */
                    {
                        for(; samp >=2; samp-=2)
                        {
                            /* Apply the gain */
                            temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);

                            /* Add the rounding factor before shift to Q15 */
                            temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q15);

                            /* Right shift from Q31 to Q15 */
                            temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT_32);

                            /* Accumulate the gain applied samples */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                            ptr_out_buf_64++;
                        }
                    }
                    /* If the number of samples are odd */
                    if(samp)
                    {
                        ptr_out_buf_32     = (int32_t *)ptr_out_buf_64;
                        ptr_local_buf_32   = (int32_t *)ptr_local_buf_64;

                        /* Apply the gain on sample */
                        post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                        /* Right shift to Q15 format */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor),local_shift_factor));

                        /* Accumulate the gain applied 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;
                    }
                } /* gain < unity gain output in Q15 AND in bytes > out bytes */
                else if(MT_MX_UNITY_GAIN == *pCurrentGain)
                {
                    /** This configuration is the most commonly used scenario for
                     *  audio use cases. Since the gain = unity, optimize by
                     *  skipping multiplication and taking advantage of vectorized
                     *  operations
                     */

                    samp = unNumSamplesGainAndAccPerCh;

                    /* Advance the input and output buffers pointers
                       to the next channel
                     */
                    ptr_local_buf_32 = (ptr_local_buf_start_32 + (i * pCurrentInPort->unInPortPerChBufSize));
                    ptr_out_buf_32   = ((int32_t*)pOutputBuf + (i * pCurrentOutPort->unOutPortPerChBufSize));

                    /** If the output pointer is not 8-byte aligned, process one
                     *  sample to align the pointer.
                     */
                    if (((uint32_t)ptr_out_buf_32) & 7)
                    {
                        /* Right shift to Q15 format */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                        /* Accumulate the 32 bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;

                        /* Reduce sample count by 1 */
                        samp = samp - 1;
                    }

                    /* Process two input samples per loop iteration */
                    ptr_local_buf_64 = (int64_t *)ptr_local_buf_32;
                    ptr_out_buf_64   = (int64_t *)ptr_out_buf_32;

                    if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type) /* Q27 -> Q15 */
                    {
                        for(; samp >=2; samp-=2)
                        {
                            /* Add the rounding factor before shift to Q15 */
                            temp_res_64 = Q6_P_vaddw_PP_sat(*ptr_local_buf_64++, MT_MX_RND_FACTOR_V_Q27_TO_Q15);

                            /* Right shift from Q27 to Q15 */
                            temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT);

                            /* Accumulate the samples */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                            ptr_out_buf_64++;
                        }
                    }
                    else /* Q31 -> Q15 */
                    {
                        for(; samp >=2; samp-=2)
                        {
                            /* Add the rounding factor before shift to Q15 */
                            temp_res_64 = Q6_P_vaddw_PP_sat(*ptr_local_buf_64++, MT_MX_RND_FACTOR_V_Q31_TO_Q15);

                            /* Right shift from Q31 to Q15 */
                            temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT_32);

                            /* Accumulate the samples */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                            ptr_out_buf_64++;
                        }
                    }
                    /* If the number of samples are odd */
                    if(samp)
                    {
                        ptr_out_buf_32     = (int32_t *)ptr_out_buf_64;
                        ptr_local_buf_32   = (int32_t *)ptr_local_buf_64;

                        /* Right shift to Q15 format */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain),local_shift_factor_unit_gain));

                        /* Accumulate the 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;
                    }

                } /* gain = unity for output in Q15 AND in bytes > out bytes */
                else /* gain > unity */
                {
                     /* Advance the input and output buffers pointers
                       to the next channel
                     */
                    ptr_local_buf_32   = (ptr_local_buf_start_32 + (i * pCurrentInPort->unInPortPerChBufSize));
                    ptr_out_buf_32     = ((int32_t*)pOutputBuf + (i * pCurrentOutPort->unOutPortPerChBufSize));

                    for(j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                    {
                        /* Apply the gain on sample */
                        post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                        /* Right shift to Q15 format */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor),local_shift_factor));

                        /* Accumulate the gain applied 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;
                    }
                } /* gain > unity for output in Q15 AND in bytes > out bytes*/
                /* Increment the pointer to fetch the next channel's gain value */
                pCurrentGain++;
            } /* unNumChannels for loop */
        } /* out bytes < in bytes */
    } /* Unequal number of bytes */
}

static void MtMx_GainAndAccOnStereoOutMonoIn(uint16_t unNumSamplesGainAndAccPerCh,
                MatrixInPortInfoType *pCurrentInPort,
                MatrixOutPortInfoType *pCurrentOutPort,
                int8_t *pInputBuf, int64_t *pOutputBuf,
                uint16_t *pCurrentGain)
{
    uint32_t  i, j, samp;
    int16_t   *ptr_local_buf_16;
    int32_t   post_gain_sample_32, *ptr_local_buf_32;
    int64_t   post_gain_sample_64, *ptr_local_buf_64;
    int32_t   *ptr_out_buf_32;
    int64_t   *ptr_out_buf_64;
    int32_t   curr_gain_q16, curr_gain_q28;
    int64_t   combined_gain, combined_input, temp_res_64;

    bit_conv_mode_t bit_conv_type = MtMx_IdentifyBitConvType(pCurrentInPort->unBitwidth, pCurrentOutPort->unBitwidth);

    /* If the number of bytes per sample for input port and output port are equal */
    if(pCurrentInPort->unBytesPerSample == pCurrentOutPort->unBytesPerSample)
    {
        /* If 2 bytes per sample (Q15 - Q15) */
        if(MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentInPort->unBytesPerSample)
        {
          /* Process this loop for the stereo o/p channels */
          for (i = 0; i < NUM_CHANNELS_STEREO; i++)
          {
            ptr_local_buf_16 = (int16_t *)pInputBuf;
            samp = unNumSamplesGainAndAccPerCh;            
            ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

            /** If the output pointer is not 8-byte aligned, process one
             *  sample to align the pointer.
             */
            if (((uint32_t)ptr_out_buf_32) & 7)
            {
                /* Apply the gain on sample */
                post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                /* Right shift for gain in Q13 */
                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + MT_MX_RND_FACTOR_GAIN_SHIFT), MT_MX_GAIN_SHIFT_FACT));

                /* Accumulate the gain applied 32-bit sample */
                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                ptr_out_buf_32++;
                
               /* Reduce sample count by 1 */
                samp = samp - 1;
            }

            /** Vector 32x16 multiplication results in a 48 bit result that
             *  is right shifted by 16 bits Since the gain is only in Q13,
             *  account for the extra 3 bit right shift.
             *
             *  1. Gain is left shifted by maximum 3 bits.
             */

            /* Scale the gain up from Q13 -> Q16 */
            curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 3;

            /* Combine two int32_t's into one int64_t */
            combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

            ptr_out_buf_64 = (int64_t *)ptr_out_buf_32;

            /* Process two input samples per loop iteration */
            for (; samp >= 2; samp -= 2)
            {
                /* Combine two, 16 to 32-bit sign extended hwords in one 64-bit dword */
                combined_input = Q6_P_combine_RR(ptr_local_buf_16[1], ptr_local_buf_16[0]);
                ptr_local_buf_16 += 2;

                /* Apply the gain */
                temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(combined_gain, combined_input);

                /* Accumulate the gain applied samples */
                *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                ptr_out_buf_64++;
            }

            /* If the number of samples are odd */
            if(samp)
            {
                ptr_out_buf_32 = (int32_t *)ptr_out_buf_64;

                /* Apply the gain on sample */
                post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                /* Right shift for gain in Q13 */
                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + MT_MX_RND_FACTOR_GAIN_SHIFT), MT_MX_GAIN_SHIFT_FACT));

                /* Accumulate the gain applied 32-bit sample */
                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                ptr_out_buf_32++;
            }
            
            /* Increment the pointer to fetch the next channel's gain value */
            pCurrentGain++;
          } /* NUM_CHANNELS_STEREO for loop */
        } /* Q15 -> Q15 */
        else /* 4 bytes per channel */
        {
            /** The number of bytes per sample determines the processing bit
             *  width. But, the actual bit widths can be different. Hence
             *  account for potential up/down conversion combinations with 24
             *  or 32 as input & output port bit widths
             */

            int32_t local_shift_factor           = MT_MX_GAIN_SHIFT_FACT; /* Default shift factor for gain mpy in Q13 */
            int32_t local_rnd_factor             = 0;
            int32_t local_shift_factor_unit_gain = 0;
            int32_t local_rnd_factor_unit_gain   = 0;

            /* Identify shift/rounding factors based on the bit conv type */
            if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type)
            {
                local_shift_factor           -= (BYTE_INTER_CONV_SHIFT_FACT); /* Right shift from Q31 -> Q27 */
                local_shift_factor_unit_gain -= (BYTE_INTER_CONV_SHIFT_FACT);
            }
            else if(BIT_CONV_IN24_TO_OUT32 == bit_conv_type)
            {
                local_shift_factor           += (BYTE_INTER_CONV_SHIFT_FACT); /* Left shift from Q27 -> Q31 */
                local_shift_factor_unit_gain += (BYTE_INTER_CONV_SHIFT_FACT);
            }
            else
            {
                local_shift_factor           += 0; /* Retain Q format */
                local_shift_factor_unit_gain += 0;
            }

            local_rnd_factor           = MtMx_CalculateRoundingFactor(local_shift_factor);
            local_rnd_factor_unit_gain = MtMx_CalculateRoundingFactor(local_shift_factor_unit_gain);

            if(MT_MX_BIT_WIDTH_24 == pCurrentOutPort->unBitwidth) /* output in Q27 AND in bytes = out bytes */
            {
              /* Process this loop for the stereo o/p channels */
              for (i = 0; i < NUM_CHANNELS_STEREO; i++)
              {
                /* If gain < unity, use vector operations */
                if(*pCurrentGain < MT_MX_UNITY_GAIN)
                {
                    ptr_local_buf_32 = (int32_t *)pInputBuf;
                    samp = unNumSamplesGainAndAccPerCh;
                    ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                    /** If the output pointer is not 8-byte aligned, process one
                     *  sample to align the pointer.
                     */
                    if (((uint32_t)ptr_out_buf_32) & 7)
                    {
                      /* Apply the gain on sample */
                      post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                      /* Shift to Q27 format and saturate to 32 bits */
                      post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                      /* Accumulate the gain applied 32-bit sample */
                      (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                      ptr_out_buf_32++;

                      /* Reduce sample count by 1 */
                      samp = samp - 1;
                    }
                    /** Vector 32x16 multiplication results in a 48 bit result that
                     *  is right shifted by 16 bits Since the gain is only in Q13,
                     *  account for the extra 3 bit right shift.
                     *
                     *  1. Gain is left shifted by maximum 2 bits. Since it is less
                     *  than unity we are not losing precision.
                     *
                     *  2. Multiply result is shifted left by 1 bit before the right
                     *  shift as part of the intrinsic itself.
                     */

                    /* Scale the gain up from Q13 -> Q15 */
                    curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 2;

                    /* Combine two int32_t's into one int64_t */
                    combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                    ptr_local_buf_64    = (int64_t *)ptr_local_buf_32;
                    ptr_out_buf_64 = (int64_t *)ptr_out_buf_32;

                    if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type) /* Q31 -> Q27 */
                    {
                        /* Process two input samples per loop iteration */
                        for (; samp >= 2; samp -= 2)
                        {

                          /* Apply the gain */
                          temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);

                          /* Add the rounding factor before truncation to Q27 */
                          temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q27);

                          /* Right shift from Q31 to Q27 */
                          temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_INTER_CONV_SHIFT_FACT);

                          /* Accumulate the gain applied samples  */
                          *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                          ptr_out_buf_64++;

                        }
                    }
                    else /* Retain Q Format*/
                    {
                        /* Process two input samples per loop iteration */
                        for (; samp >= 2; samp -= 2)
                        {

                          /* Apply the gain */
                          temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);
                          
                          /* Accumulate the gain applied samples */
                          *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                          ptr_out_buf_64++;

                        }
                    }
                    /* If the number of samples are odd */
                    if(samp)
                    {
                        ptr_local_buf_32 = (int32_t *)ptr_local_buf_64;
                        ptr_out_buf_32 = (int32_t *)ptr_out_buf_64;

                        /* Apply the gain on sample */
                        post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                        /* Shift to Q27 format and saturate to 32 bits */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                        /* Accumulate the gain applied 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;

                    }
                } /* ((gain < unity gain) for output in Q27 AND in bytes = out bytes */

                else if(MT_MX_UNITY_GAIN == *pCurrentGain)
                {
                    /** This configuration is the most commonly used scenario for
                     *  audio use cases. Since the gain = unity, optimize by
                     *  skipping multiplication and taking advantage of vectorized
                     *  operations
                     */
                    samp = unNumSamplesGainAndAccPerCh;
                    ptr_local_buf_32 = (int32_t *)pInputBuf;
                    
                    ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                    /** If the output pointer is not 8-byte aligned, process one
                     *  sample to align the pointer.
                     */
                    if (((uint32_t)ptr_out_buf_32) & 7)
                    {
                      /* Shift to Q27 format and saturate to 32 bits */
                      post_gain_sample_32 = s32_saturate_s64(s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                      /* Accumulate the 32-bit sample to the output port buffer */
                      (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                      ptr_out_buf_32++;                            

                      /* Reduce sample count by 1 */
                      samp = samp - 1;
                    }

                    ptr_local_buf_64    = (int64_t *)ptr_local_buf_32;
                    ptr_out_buf_64 = (int64_t *)ptr_out_buf_32;

                    if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type) /* Q31 -> Q27 */
                    {
                        /* Process two input samples per loop iteration */
                        for (; samp >= 2; samp -= 2)
                        {
                            /* Add the rounding factor before truncation to Q27 */
                            temp_res_64 = Q6_P_vaddw_PP_sat(*ptr_local_buf_64++, MT_MX_RND_FACTOR_V_Q31_TO_Q27);

                            /* Right shift from Q31 to Q27 */
                            temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_INTER_CONV_SHIFT_FACT);

                            /* Accumulate the samples to the output port buffer */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                            ptr_out_buf_64++;

                        }
                    }
                    else /* Retain Q Format*/
                    {
                        /* Process two input samples per loop iteration */
                        for (; samp >= 2; samp -= 2)
                        {
                            /*------------------------- Process the sample -------------------------*/
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, *ptr_local_buf_64++);
                            ptr_out_buf_64++;
                        }
                    }
                    /* If the number of samples are odd */
                    if(samp)
                    {
                        ptr_local_buf_32 = (int32_t *)ptr_local_buf_64;
                        ptr_out_buf_32 = (int32_t *)ptr_out_buf_64;

                        /* Shift to Q27 format and saturate to 32 bits */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                        /* Accumulate the 32-bit sample to the output port buffer */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;

                    }
                } /* gain = unity for output in Q27 AND in bytes = out bytes */
                else /* (gain > unity gain) */
                {
                    ptr_local_buf_32 = (int32_t *)pInputBuf;
                    ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                    for (j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                    {
                      /* Apply the channel gain on sample */
                      post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                      /* Shift to Q27 format and saturate to 32 bits */
                      post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                      /* Accumulate the gain applied 32-bit sample to the output port buffer */
                      (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                      ptr_out_buf_32++;
                    }
                } /* (gain > unity gain) for output in Q27 AND in bytes = out bytes */
                
                /* Increment the pointer to fetch the next channel's gain value */
                pCurrentGain++;
              } /* NUM_CHANNELS_STEREO channels loop */
            } /* output port BW == 24 */
            else /* output in Q31 AND in bytes = out bytes */
            {
              /* Process this loop for the stereo o/p channels */
              for (i = 0; i < NUM_CHANNELS_STEREO; i++)
              {
                ptr_local_buf_32 = (int32_t*)pInputBuf;
                
                ptr_out_buf_64   = (pOutputBuf + (i * pCurrentOutPort->unOutPortPerChBufSize));

                /* Use scalar operations for accumulation to 64-bit buffer */

                for (j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                {
                  /* Apply the channel gain on sample */
                  post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                  /* Shift to Q31 format after rounding */
                  post_gain_sample_64 = s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor);

                  /* Accumulate the gain applied 64-bit sample to the output port buffer */
                  (*ptr_out_buf_64) = Q6_P_add_PP(*ptr_out_buf_64, post_gain_sample_64);
                  ptr_out_buf_64++;
                }
                
                /* Increment the pointer to fetch the next channel's gain value */
                pCurrentGain++;
              } /* NUM_CHANNELS_STEREO for loop */
            } /* output port BW == 32 */
        } /* 4 bytes per channel */
    } /* in bytes = out bytes */
    else /* Unequal number of bytes */
    {
        /* If output port bytes per sample is greater than the input port bytes per sample*/
        if(pCurrentOutPort->unBytesPerSample > pCurrentInPort->unBytesPerSample)
        {
            /** If output port bytes per sample is greater than the input
             *  port,
             *  1. Apply gain on 16-bit input port samples
             *  2. Up convert to appropriate Q format (Q27/Q31)
             *  3. Accumulate to output port buffer.
             */

            int32_t local_shift_factor = 0;
            int32_t local_rnd_factor   = 0;

            /* Identify shift/rounding factors for Left shift from Q15 -> Qn */
            if(BIT_CONV_IN16_TO_OUT24 == bit_conv_type) /* Q15 -> Q27 */
            {
                local_shift_factor = MT_MX_GAIN_SHIFT_FACT + BYTE_UPDOWN_CONV_SHIFT_FACT;
            }
            else /* Q15 -> Q31 */
            {
                local_shift_factor = MT_MX_GAIN_SHIFT_FACT + BYTE_UPDOWN_CONV_SHIFT_FACT_32;
            }

            local_rnd_factor = MtMx_CalculateRoundingFactor(local_shift_factor);
            

            if(BIT_CONV_IN16_TO_OUT24 == bit_conv_type) /* Q15 -> Q27 */
            {
              /* Process this loop for the stereo o/p channels */
              for (i = 0; i < NUM_CHANNELS_STEREO; i++)
              {
                ptr_local_buf_16 = (int16_t *)pInputBuf;
                samp = unNumSamplesGainAndAccPerCh;
                ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));
                 
                /** If the output pointer is not 8-byte aligned, process one
                 *   sample to align the pointer.
                 */
                if (((uint32_t)ptr_out_buf_32) & 7)
                {
                    /* Apply the gain on sample */
                    post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                    /* Shift to Q27 format and saturate to 32 bits */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                    /* Accumulate the gain applied 32-bit sample to the output port buffer */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;

                    /* Reduce sample count by 1 */
                    samp = samp - 1;
                }

                /** This is a special case where the gain is shifted to Q28.
                 *  Doing this avoids consecutive right shift (as part of mpy)
                 *  and left shift (for up conversion).
                 *
                 *  The mpy operation translates to ((Q28 gain * Q15 sample)  >>
                 *  16). This results in a Q27 number directly
                 *
                 *  As a result, achieve higher signal precision in the
                 *  converted sample and avoid an explicit left shift for bit
                 *  conversion.
                 */

                /* Scale the gain up from Q13 -> Q28 */
                curr_gain_q28 = (Q6_R_zxth_R(*pCurrentGain)) << 15;

                /* Combine two int32_t's into one int64_t */
                combined_gain = Q6_P_combine_RR(curr_gain_q28, curr_gain_q28);

                ptr_out_buf_64 = (int64_t *)ptr_out_buf_32;

                /* Process two input samples per loop iteration */
                for (; samp >=2; samp -=2)
                {
                    /* Combine two, 16 to 32-bit sign extended hwords in one 64-bit dword */
                    combined_input = Q6_P_combine_RR(ptr_local_buf_16[1], ptr_local_buf_16[0]);
                    ptr_local_buf_16 += 2;

                    /* Apply the gain */
                    temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(combined_gain, combined_input);
                        /* Note that result is already in Q27*/

                    /* Accumulate the gain applied samples to the output port buffer */
                    *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                    ptr_out_buf_64++;

                }
                /* If the number of samples are odd */
                if(samp)
                {
                    ptr_out_buf_32 = (int32_t *)ptr_out_buf_64;

                    /* Apply the gain on sample */
                    post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                    /* Shift to Q27 format and saturate to 32 bits */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                    /* Accumulate the gain applied 32-bit sample to the output port buffer */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;
                }
                
                /* Increment the pointer to fetch the next channel's gain value */
                  pCurrentGain++;
              } /* NUM_CHANNELS_STEREO for loop */
            } /* Q15 -> Q27 */
            else
            {
              /* Process this loop for the stereo o/p channels */
              for (i = 0; i < NUM_CHANNELS_STEREO; i++)
              {
                ptr_local_buf_16 = (int16_t *)pInputBuf;              
                ptr_out_buf_64   = (pOutputBuf + (i * pCurrentOutPort->unOutPortPerChBufSize));
                 
                /* Use scalar operations for accumulation to 64-bit buffer */
                for (j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                {
                    /* Apply the gain on sample */
                    post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                    /* Shift to Q31 format after rounding */
                    post_gain_sample_64 = s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor);

                    /* Accumulate the gain applied 64-bit sample to the output port buffer */
                    (*ptr_out_buf_64) = Q6_P_add_PP(*ptr_out_buf_64, post_gain_sample_64);
                    ptr_out_buf_64++;
                }
                
                /* Increment the pointer to fetch the next channel's gain value */
                pCurrentGain++;
              } /* NUM_CHANNELS_STEREO for loop */
            } /* Q15 -> Q31 */
        } /* in bytes < out bytes */
        else /* pCurrentOutPort->unBytesPerSample < pCurrentInPort->unBytesPerSample */
        {
            /**
                 1. Gain is applied on 32-bit input port samples
                 2. The 64-bit intermediate result is saturated to 32-bits
            */

            int32_t local_shift_factor           = MT_MX_GAIN_SHIFT_FACT; /* Default shift factor for gain mpy in Q13 */
            int32_t local_rnd_factor             = 0;
            int32_t local_shift_factor_unit_gain = 0;
            int32_t local_rnd_factor_unit_gain   = 0;

            /* Identify the shift/rounding factors for Right shift from Qn -> Q15 */
            if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type)
            {
                local_shift_factor           -= BYTE_UPDOWN_CONV_SHIFT_FACT; /* Right shift from Q27 - Q15 */
                local_shift_factor_unit_gain -= BYTE_UPDOWN_CONV_SHIFT_FACT;
            }
            else
            {
                local_shift_factor           -= BYTE_UPDOWN_CONV_SHIFT_FACT_32; /* Right shift from Q31 - Q15 */
                local_shift_factor_unit_gain -= BYTE_UPDOWN_CONV_SHIFT_FACT_32;
            }

            local_rnd_factor           = MtMx_CalculateRoundingFactor(local_shift_factor);
            local_rnd_factor_unit_gain = MtMx_CalculateRoundingFactor(local_shift_factor_unit_gain);
            
            /* Process this loop for the stereo o/p channels */
            for (i = 0; i < NUM_CHANNELS_STEREO; i++)
            {
              if(*pCurrentGain < MT_MX_UNITY_GAIN)
              {
                  ptr_local_buf_32 = (int32_t *)pInputBuf;
                  samp = unNumSamplesGainAndAccPerCh;
                  ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                  /** If the output pointer is not 8-byte aligned, process one
                   *  sample to align the pointer.
                   */
                  if (((uint32_t)ptr_out_buf_32) & 7)
                  {
                      /* Apply the gain on sample */
                      post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                      /* Right shift to Q15 format */
                      post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                      /* Accumulate the gain applied 32-bit sample to the output port buffer */
                      (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                      ptr_out_buf_32++;

                      /* Reduce sample count by 1 */
                      samp = samp - 1;
                  }

                  /* Scale the gain up from Q13 -> Q15 */
                  curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 2;

                  /* Combine two int32_t's into one int64_t */
                  combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                  ptr_local_buf_64    = (int64_t *)ptr_local_buf_32;
                  ptr_out_buf_64 = (int64_t *)ptr_out_buf_32;

                  if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type) /* Q27 -> Q15 */
                  {
                      /* Process two input samples per loop iteration */
                      for (; samp >= 2; samp -= 2)
                      {

                        /* Apply the gain */
                        temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);

                        /* Add the rounding factor before shift to Q15 */
                        temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q27_TO_Q15);

                        /* Right shift from Q27 to Q15 */
                        temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT);

                        /* Accumulate the gain applied samples to the output port buffer */
                        *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                        ptr_out_buf_64++;                       
                      }
                  }
                  else if(BIT_CONV_IN32_TO_OUT16 == bit_conv_type) /* Q31 -> Q15 */
                  {
                      /* Process two input samples per loop iteration */
                      for (; samp >= 2; samp -= 2)
                      {

                        /* Apply the gain */
                        temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);

                        /* Add the rounding factor before shift to Q15 */
                        temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q15);

                        /* Right shift from Q31 to Q15 */
                        temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT_32);

                        /* Accumulate the gain applied samples to the output port buffer */
                        *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                        ptr_out_buf_64++;
                          
                      }
                  }

                  /* If the number of samples are odd */
                  if(samp)
                  {
                      ptr_local_buf_32 = (int32_t *)ptr_local_buf_64;
                      ptr_out_buf_32 = (int32_t *)ptr_out_buf_64;

                      /* Apply the gain on sample */
                      post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                      /* Shift to Q15 format */
                      post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor),local_shift_factor));

                      /* Accumulate the gain applied 32-bit sample to the output buffer */
                      (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                      ptr_out_buf_32++;
                      
                  }
              } /* (gain < unity gain) for output in Q15 AND in bytes > out bytes */
              else if(MT_MX_UNITY_GAIN == *pCurrentGain)
              {
                  /** This configuration is the most commonly used scenario for
                   *  audio use cases. Since the gain = unity, optimize by
                   *  skipping multiplication and taking advantage of vectorized
                   *  operations
                   */

                  ptr_local_buf_32 = (int32_t *)pInputBuf;
                  samp = unNumSamplesGainAndAccPerCh;                
                  ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                  /** If the output pointer is not 8-byte aligned, process one
                   *  sample to align the pointer.
                   */
                  if (((uint32_t)ptr_out_buf_32) & 7)
                  {

                    /* Right shift to Q15 format */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((*ptr_local_buf_32 + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                    /* Accumulate the 32-bit sample to the output port buffer */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;

                    /* Reduce sample count by 1 */
                    samp = samp - 1;
                    ptr_local_buf_32++;
                  }

                  ptr_local_buf_64    = (int64_t *)ptr_local_buf_32;
                  ptr_out_buf_64 = (int64_t *)ptr_out_buf_32;

                  if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type) /* Q27 -> Q15 */
                  {
                      /* Process two input samples per loop iteration */
                      for (; samp >= 2; samp -= 2)
                      {
                          /* Add the rounding factor before shift to Q15 */
                          temp_res_64 = Q6_P_vaddw_PP_sat(*ptr_local_buf_64++, MT_MX_RND_FACTOR_V_Q27_TO_Q15);

                          /* Right shift from Q27 to Q15 */
                          temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT);

                          /* Accumulate the samples in the output port buffer */
                          *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                          ptr_out_buf_64++;

                      }
                  }
                  else if(BIT_CONV_IN32_TO_OUT16 == bit_conv_type) /* Q31 -> Q15 */
                  {
                      /* Process two input samples per loop iteration */
                      for (; samp >= 2; samp -= 2)
                      {
                          /* Add the rounding factor before shift to Q15 */
                          temp_res_64 = Q6_P_vaddw_PP_sat(*ptr_local_buf_64++, MT_MX_RND_FACTOR_V_Q31_TO_Q15);

                          /* Right shift from Q31 to Q15 */
                          temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT_32);

                          /* Accumulate the samples in the output port buffer */
                          *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                          ptr_out_buf_64++;

                      }
                  }
                  /* If the number of samples are odd */
                  if(samp)
                  {
                      ptr_local_buf_32 = (int32_t *)ptr_local_buf_64;
                      ptr_out_buf_32 = (int32_t *)ptr_out_buf_64;

                      /* Right shift to Q15 format */
                      post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((*ptr_local_buf_32 + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                      /* Accumulate the 32-bit sample to the output port buffer */
                      (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                      ptr_out_buf_32++;

                      ptr_local_buf_32++;
                  }
              } /* (gain == unity) for output in Q15 AND in bytes > out bytes */
              else /* (gain > unity gain) */
              {
                  ptr_local_buf_32 = (int32_t *)pInputBuf;                
                  ptr_out_buf_32   = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                  for (j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                  {
                      /* Apply the gain on sample */
                      post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                      /* Right shift to Q15 format */
                      post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor),local_shift_factor));

                      /* Accumulate the gain applied 32-bit sample to the output port buffer */
                      (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                      ptr_out_buf_32++;

                  }
              } /* else(gain > unity gain) */
            
              /* Increment the pointer to fetch the next channel's gain value */
             pCurrentGain++;
          } /* NUM_CHANNELS_STEREO for loop */
        } /*out byte < in byte */        
    }/* unequal number of in out bytes */
}

static void MtMx_GainAndAccOnMonoOutStereoIn(uint16_t unNumSamplesGainAndAccPerCh,
                MatrixInPortInfoType *pCurrentInPort,
                MatrixOutPortInfoType *pCurrentOutPort,
                int8_t *pInputBuf, int64_t *pOutputBuf,
                uint16_t *pCurrentGain)
{
    uint32_t     j, samp;
    int16_t      post_gain_sample_16, *ptr_local_buf_16_ch_l, *ptr_local_buf_16_ch_r, *ptr_local_buf_start_16;
    int32_t      post_gain_sample_32, *ptr_local_buf_32_ch_l, *ptr_local_buf_32_ch_r, *ptr_local_buf_start_32;
    int64_t      post_gain_sample_64, *ptr_local_buf_64_ch_l, *ptr_local_buf_64_ch_r;
    int32_t      *ptr_out_buf_32;
    int64_t      *ptr_out_buf_64;
    int32_t      curr_gain_q16, curr_gain_q28;
    int64_t      combined_gain, combined_input_l, combined_input_r, temp_res_64;

    bit_conv_mode_t bit_conv_type = MtMx_IdentifyBitConvType(pCurrentInPort->unBitwidth, pCurrentOutPort->unBitwidth);

    /* If the number of bytes per sample for input port and output port are equal */
    if(pCurrentInPort->unBytesPerSample == pCurrentOutPort->unBytesPerSample)
    {
        ptr_local_buf_start_16 = (int16_t *) pInputBuf;

        /* If 2 bytes per sample (Q15 - Q15) */
        if(MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentInPort->unBytesPerSample)
        {
            /* Advance the input pointers to the left & right channels */
            ptr_local_buf_16_ch_l = ptr_local_buf_start_16;
            ptr_local_buf_16_ch_r = (ptr_local_buf_start_16 + (pCurrentInPort->unInPortPerChBufSize));
            ptr_out_buf_32   = (int32_t*) pOutputBuf;

            samp = unNumSamplesGainAndAccPerCh;

            /** If the output pointer is not 8-byte aligned, process one
             *  sample to align the pointer.
             */
            if(((uint32_t) ptr_out_buf_32) & 7)
            {
                /* Average the left and right channel input samples */
                post_gain_sample_32 = ((int32_t)(*ptr_local_buf_16_ch_l++) + (int32_t)(*ptr_local_buf_16_ch_r++));
                post_gain_sample_16 = (int16_t) (post_gain_sample_32 >> 1);

                /* Apply the gain on the averaged input sample and shift for gain in Q13 */
                post_gain_sample_64 = s64_mult_s16_u16_shift(post_gain_sample_16, *pCurrentGain, 0);
                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + MT_MX_RND_FACTOR_GAIN_SHIFT),MT_MX_GAIN_SHIFT_FACT));

                /* Accumulate the gain applied and averaged 32-bit sample */
                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                ptr_out_buf_32++;

                /* Reduce sample count by 1 */
                samp = samp - 1;
            }
            /** Vector 32x16 multiplication results in a 48 bit result that
             *  is right shifted by 16 bits Since the gain is only in Q13,
             *  account for the extra 3 bit right shift.This is achieved by
             *  left shifting the gain by maximum 3 bits
             *  (Q15 sample * Q16 gain) >> 16 = Q15 output
             */

            /* Scale the gain up from Q13 to Q16 */
            curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 3;

            /* Combine two int32_t's into one int64_t */
            combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

            /* Mix both channels (L and R) into one channel output buffer */
            ptr_out_buf_64 = (int64_t *) ptr_out_buf_32;

            /* Process two samples per channel (L & R) per loop iteration */
            for(; samp >= 2; samp -= 2)
            {
                /* Combine two, 16 to 32-bit sign extended hwords in one 64-bit dword */
                combined_input_l = Q6_P_combine_RR(ptr_local_buf_16_ch_l[1], ptr_local_buf_16_ch_l[0]);
                combined_input_r = Q6_P_combine_RR(ptr_local_buf_16_ch_r[1], ptr_local_buf_16_ch_r[0]);
                ptr_local_buf_16_ch_l +=2;
                ptr_local_buf_16_ch_r +=2;

                /* Average the left and right channel input samples */
                temp_res_64 = Q6_P_vavgw_PP_rnd(combined_input_l, combined_input_r);

                /* Apply the gain on the averaged input samples */
                temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(combined_gain, temp_res_64);

                /* Accumulate the gain applied and averaged samples */
                *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                ptr_out_buf_64++;
            }

            /* If the number of samples are odd */
            if(samp)
            {
                ptr_out_buf_32 = (int32_t *) ptr_out_buf_64;

                /* Average the left and right channel input samples */
                post_gain_sample_32 = ((int32_t)(*ptr_local_buf_16_ch_l++) + (int32_t)(*ptr_local_buf_16_ch_r++));
                post_gain_sample_16 = (int16_t) (post_gain_sample_32 >> 1);

                /* Apply the gain on the averaged input sample and shift for gain in Q13 */
                post_gain_sample_64 = s64_mult_s16_u16_shift(post_gain_sample_16, *pCurrentGain, 0);
                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + MT_MX_RND_FACTOR_GAIN_SHIFT), MT_MX_GAIN_SHIFT_FACT));

                /* Accumulate the gain applied and averaged 32-bit sample */
                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                ptr_out_buf_32++;
            }
        } /* Q15 - Q15 */
        else /* 4 bytes per sample */
        {
            /** The number of bytes per sample determines the processing bit
             *  width. But, the actual bit widths can be different. Hence
             *  account for potential up/down conversion combinations with
             *  24 or 32 as input & output port bit widths
             */

            int32_t local_shift_factor           = MT_MX_GAIN_SHIFT_FACT; /* Default shift factor for gain mpy in Q13 */
            int32_t local_rnd_factor             = 0;
            int32_t local_shift_factor_unit_gain = 0;
            int32_t local_rnd_factor_unit_gain   = 0;

            ptr_local_buf_start_32 = (int32_t *) pInputBuf;

            /* Identify shift/rounding factors based on the bit conv type */
            if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type)
            {
                local_shift_factor           -= (BYTE_INTER_CONV_SHIFT_FACT); /* Right shift from Q31 -> Q27 */
                local_shift_factor_unit_gain -= (BYTE_INTER_CONV_SHIFT_FACT);
            }
            else if(BIT_CONV_IN24_TO_OUT32 == bit_conv_type)
            {
                local_shift_factor           += (BYTE_INTER_CONV_SHIFT_FACT); /* Left shift from Q27 -> Q31 */
                local_shift_factor_unit_gain += (BYTE_INTER_CONV_SHIFT_FACT);
            }
            else
            {
                local_shift_factor           += 0; /* Retain Q format */
                local_shift_factor_unit_gain += 0;
            }

            local_rnd_factor           = MtMx_CalculateRoundingFactor(local_shift_factor);
            local_rnd_factor_unit_gain = MtMx_CalculateRoundingFactor(local_shift_factor_unit_gain);

            if(MT_MX_BIT_WIDTH_24 == pCurrentOutPort->unBitwidth) /* output in Q27 AND in bytes = out bytes */
            {
                /* If gain < unity gain, use vector operations */
                if(*pCurrentGain < MT_MX_UNITY_GAIN)
                {
                    /* Advance the input pointers to the left and right channels */
                    ptr_local_buf_32_ch_l = ptr_local_buf_start_32;
                    ptr_local_buf_32_ch_r = (ptr_local_buf_start_32 + (pCurrentInPort->unInPortPerChBufSize));
                    ptr_out_buf_32   = (int32_t*)pOutputBuf;

                    samp = unNumSamplesGainAndAccPerCh;
                   /** If the output pointer is not 8-byte aligned, process one
                     * sample to align the pointer.
                     */
                    if(((uint32_t) ptr_out_buf_32) & 7)
                    {
                        /* Average the left and right channel input samples */
                        post_gain_sample_64 = ((int64_t)(*ptr_local_buf_32_ch_l++) + (int64_t)(*ptr_local_buf_32_ch_r++));
                        post_gain_sample_32 = (int32_t)(post_gain_sample_64 >> 1);

                        /* Apply the gain on the averaged input sample */
                        post_gain_sample_64 = s64_mult_s32_u16(post_gain_sample_32, *pCurrentGain);

                        /* Shift to Q27 Format and saturate to 32 bits */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                        /* Accumulate the gain applied and averaged 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;

                        /* Reduce sample count by 1 */
                        samp = samp - 1;
                    }
                    /** Vector 32x16 multiplication results in a 48 bit result that
                     *  is right shifted by 16 bits Since the gain is only in Q13,
                     *  account for the extra 3 bit right shift. This is achieved by
                     *  left shifting the gain by maximum 3 bits
                     *  (Qn sample * Q16 gain) >> 16 = Qn output
                     */

                    /* Scale the gain up Q13 -> Q16  */
                    curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 3;

                    /* Combine int32_t into int64_t */
                    combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                    ptr_local_buf_64_ch_l = (int64_t *)(ptr_local_buf_32_ch_l);
                    ptr_local_buf_64_ch_r = (int64_t *)(ptr_local_buf_32_ch_r);

                    /* Mix both channels (L and R) into one channel output buffer */
                    ptr_out_buf_64 = (int64_t *) ptr_out_buf_32;

                    if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type)/* Q31 -> Q27 */
                    {
                        /* Process two input samples per channel (L & R) per loop iteration */
                        for(; samp >= 2; samp -= 2)
                        {
                            /* Average the left and right channel input samples */
                            temp_res_64 = Q6_P_vavgw_PP_rnd(*ptr_local_buf_64_ch_l++, *ptr_local_buf_64_ch_r++);

                            /* Apply the gain on the averaged input samples */
                            temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(temp_res_64, combined_gain);

                            /* Add the rounding factor before truncation to Q27 */
                            temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q27);

                            /* Right shift from Q31 to Q27 */
                            temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_INTER_CONV_SHIFT_FACT);

                            /* Accumulate the gain applied and averaged samples */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                            ptr_out_buf_64++;
                        }
                    }
                    else /* Retain Q27 Format */
                    {
                        /* Process two input samples per channel (L & R) per loop iteration */
                        for(; samp >= 2; samp -= 2)
                        {
                            /* Average the left and right channel input samples */
                            temp_res_64 = Q6_P_vavgw_PP_rnd(*ptr_local_buf_64_ch_l++, *ptr_local_buf_64_ch_r++);

                            /* Apply the gain on the averaged samples*/
                            temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(temp_res_64, combined_gain);
                            
                            /* Accumulate the gain applied samples */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                             ptr_out_buf_64++;
                        }
                    }
                    /* If the number of samples are odd */
                    if(samp)
                    {
                        ptr_out_buf_32   = (int32_t *) ptr_out_buf_64;
                        ptr_local_buf_32_ch_l = (int32_t *)ptr_local_buf_64_ch_l;
                        ptr_local_buf_32_ch_r = (int32_t *)ptr_local_buf_64_ch_r;

                        /* Average the left and right channel input samples */
                        post_gain_sample_64 = ((int64_t)(*ptr_local_buf_32_ch_l++) + (int64_t)(*ptr_local_buf_32_ch_r++));
                        post_gain_sample_32 = (int32_t)(post_gain_sample_64 >> 1);

                        /* Apply the gain on the averaged input sample */
                        post_gain_sample_64 = s64_mult_s32_u16(post_gain_sample_32, *pCurrentGain);

                        /* Shift to Q27 Format and saturate to 32 bits */
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                        /* Accumulate the gain applied and averaged 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;

                    }
                } /*gain < unity gain */
                else if(MT_MX_UNITY_GAIN == *pCurrentGain)
                {
                    /** This configuration is the most commonly used scenario for
                     *  audio use cases. Since the gain = unity, optimize by
                     *  skipping multiplication and taking advantage of vectorized
                     *  operations
                     */

                    samp = unNumSamplesGainAndAccPerCh;

                    /* Advance the input pointers to the left and right channels */
                    ptr_local_buf_32_ch_l = ptr_local_buf_start_32;
                    ptr_local_buf_32_ch_r = (ptr_local_buf_start_32 + (pCurrentInPort->unInPortPerChBufSize));
                    ptr_out_buf_32   = (int32_t*)pOutputBuf;

                    /** If the output pointer is not 8-byte aligned, process one
                     *  sample to align the pointer.
                     */
                    if(((uint32_t) ptr_out_buf_32) & 7)
                    {
                        /* Average the left and right channel input samples */
                        post_gain_sample_64 = ((int64_t)(*ptr_local_buf_32_ch_l++) + (int64_t)(*ptr_local_buf_32_ch_r++));
                        post_gain_sample_64 = (int64_t)(post_gain_sample_64 >> 1);

                        /* Shift to Q27 Format and saturate to 32 bits*/
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                        /* Accumulate the averaged 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;

                        /* Reduce sample count by 1 */
                        samp = samp - 1;
                    }

                    ptr_local_buf_64_ch_l = (int64_t *)(ptr_local_buf_32_ch_l);
                    ptr_local_buf_64_ch_r = (int64_t *)(ptr_local_buf_32_ch_r);

                    /* Mix both channels (L and R) into one channel output buffer */
                    ptr_out_buf_64 = (int64_t *) ptr_out_buf_32;

                    if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type) /* Q31 -> Q27 */
                    {
                        /* Process two input samples per channel (L & R) per loop iteration */
                        for(; samp >= 2; samp -= 2)
                        {
                            /* Average the left and right channel input samples */
                            temp_res_64 = Q6_P_vavgw_PP_rnd(*ptr_local_buf_64_ch_l++, *ptr_local_buf_64_ch_r++);

                            /* Add the rounding factor before truncation to Q27 */
                            temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q27);

                            /* Right shift from Q31 to Q27 */
                            temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_INTER_CONV_SHIFT_FACT);

                            /* Accumulate the averaged samples */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                            ptr_out_buf_64++;
                        }
                    }
                    else /* Retain Q27 Format */
                    {
                        /* Process two input samples per channel (L & R) per loop iteration */
                        for(; samp >= 2; samp -= 2)
                        {
                            /* Average the left and right channel samples */
                            temp_res_64 = Q6_P_vavgw_PP_rnd(*ptr_local_buf_64_ch_l++, *ptr_local_buf_64_ch_r++);

                            /* Accumulate the averaged samples */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                            ptr_out_buf_64++;
                        }
                    }
                    /* If the number of samples are odd */
                    if(samp)
                    {
                        ptr_out_buf_32 = (int32_t *) ptr_out_buf_64;
                        ptr_local_buf_32_ch_l = (int32_t *)ptr_local_buf_64_ch_l;
                        ptr_local_buf_32_ch_r = (int32_t *)ptr_local_buf_64_ch_r;

                        /* Average the left and right channel input samples */
                        post_gain_sample_64 = ((int64_t)(*ptr_local_buf_32_ch_l++) + (int64_t)(*ptr_local_buf_32_ch_r++));
                        post_gain_sample_64 = (int64_t)(post_gain_sample_64 >> 1);

                        /* Shift to Q27 Format and saturate to 32 bits*/
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                        /* Accumulate the averaged 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;
                    }
                } /* gain = unity gain for output in Q27 AND in bytes = out bytes*/
                else /* gain > unity gain */
                {
                    /* Advance the input pointers to the left and right channels */
                    ptr_local_buf_32_ch_l = ptr_local_buf_start_32;
                    ptr_local_buf_32_ch_r = (ptr_local_buf_start_32 + pCurrentInPort->unInPortPerChBufSize);

                    /* Mix both channels (L and R) into one channel output buffer */
                    ptr_out_buf_32 = (int32_t*)pOutputBuf;

                    for(j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                    {
                        /* Average the left and right channel input samples */
                        post_gain_sample_64 = ((int64_t)(*ptr_local_buf_32_ch_l++) + (int64_t)(*ptr_local_buf_32_ch_r++));
                        post_gain_sample_32 = (int32_t)(post_gain_sample_64 >> 1);

                        /* Apply the gain on the averaged input sample */
                        post_gain_sample_64 = s64_mult_s32_u16(post_gain_sample_32, *pCurrentGain);

                        /* Shift to Q27 Format and saturate to 32 bits*/
                        post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                        /* Accumulate the gain applied and averaged 32-bit sample */
                        (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                        ptr_out_buf_32++;
                    }
                } /*if gain > unity for output in Q27 AND in bytes = out bytes */
            } /* output port BW == 24 */
            else /* output in Q31 AND in bytes = out bytes */
            {
                /* Advance the input pointers to the left and right channels */
                ptr_local_buf_32_ch_l = ptr_local_buf_start_32;
                ptr_local_buf_32_ch_r = (ptr_local_buf_start_32 + pCurrentInPort->unInPortPerChBufSize);
                ptr_out_buf_64 = pOutputBuf;

                /* Use scalar operations for accumulation to 64-bit buffer */
                for(j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                {
                    /* Average the left and right channel input samples */
                    post_gain_sample_64 = ((int64_t) (*ptr_local_buf_32_ch_l++) + (int64_t) (*ptr_local_buf_32_ch_r++));
                    post_gain_sample_32 = (int32_t) (post_gain_sample_64 >> 1);

                    /* Apply the gain on the averaged input sample */
                    post_gain_sample_64 = s64_mult_s32_u16(post_gain_sample_32, *pCurrentGain);

                    /* Shift to Q31 Format after rounding */
                    post_gain_sample_64 = s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor);

                    /* Accumulate the gain applied and averaged 64-bit sample */
                    (*ptr_out_buf_64) = Q6_P_add_PP(*ptr_out_buf_64, post_gain_sample_64);
                    ptr_out_buf_64++;
                }
            } /* output port BW == 32 */
        } /* 4 bytes per sample */
    } /* in bytes = out bytes */
    else /* unequal number of bytes per sample */
    {
        /* If output port bytes per sample is greater than the input port bytes per sample */
        if(pCurrentOutPort->unBytesPerSample > pCurrentInPort->unBytesPerSample)
        {
            /** If output port bytes per sample is greater than the input
             *  port,
             *  1. Apply gain on 16-bit input port samples
             *  2. Up convert to appropriate Q format (Q27/Q31)
             *  3. Accumulate to output port buffer.
             */
            int32_t local_shift_factor = MT_MX_GAIN_SHIFT_FACT; /* Default shift factor for gain mpy in Q13 */
            int32_t local_rnd_factor   = 0;

            /* Identify shift/rounding factors for Left shift from Q15 -> Qn */
            if(BIT_CONV_IN16_TO_OUT24 == bit_conv_type)
            {
                local_shift_factor += BYTE_UPDOWN_CONV_SHIFT_FACT;
            }
            else /* BIT_CONV_IN16_TO_OUT32 */
            {
                local_shift_factor += BYTE_UPDOWN_CONV_SHIFT_FACT_32;
            }

            local_rnd_factor = MtMx_CalculateRoundingFactor(local_shift_factor);

            ptr_local_buf_start_16 = (int16_t *) pInputBuf;

            if(BIT_CONV_IN16_TO_OUT24 == bit_conv_type) /* Q15 -> Q27 */
            {
                /* Advance the input pointers to the left and right channels */
                ptr_local_buf_16_ch_l = ptr_local_buf_start_16;
                ptr_local_buf_16_ch_r = (ptr_local_buf_start_16 + pCurrentInPort->unInPortPerChBufSize);
                ptr_out_buf_32 = (int32_t *) pOutputBuf;
                samp = unNumSamplesGainAndAccPerCh;

                /** If the output pointer is not 8-byte aligned, process one
                 *   sample to align the pointer.
                 */
                if(((uint32_t) ptr_out_buf_32) & 7)
                {
                    /* Average the left and right channel input samples */
                    post_gain_sample_32 = ((int32_t)(*ptr_local_buf_16_ch_l++) + (int32_t)(*ptr_local_buf_16_ch_r++));
                    post_gain_sample_16 = (int16_t)(post_gain_sample_32 >> 1);

                    /* Apply the gain on the averaged input sample */
                    post_gain_sample_64 = s64_mult_s16_u16_shift(post_gain_sample_16, *pCurrentGain, 0);

                    /* Shift to Q27 Format and saturate to 32 bits */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                    /* Accumulate the gain applied and averaged 32-bit sample */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;

                    /* Reduce sample count by 1 */
                    samp = samp - 1;
                }

                /** This is a special case where the gain is shifted to Q28.
                 *  Doing this avoids consecutive right shift (as part of mpy)
                 *  and left shift (for up conversion).
                 *
                 *  The mpy operation translates to ((Q28 gain * Q15 sample)  >>
                 *  16). This results in a Q27 number directly
                 *
                 *  As a result, achieve higher signal precision in the
                 *  converted sample and avoid an explicit left shift for bit
                 *  conversion.
                 */

                /* Scale up gain from Q13 -> Q28 */
                curr_gain_q28 = (Q6_R_zxth_R(*pCurrentGain)) << 15;

                /* Combine int32_t into int64_t */
                combined_gain = Q6_P_combine_RR(curr_gain_q28, curr_gain_q28);

                /* Mix both channels (L and R) into one channel output buffer */
                ptr_out_buf_64 = (int64_t *) ptr_out_buf_32;

                /* Process two input samples per channel (L & R) per loop iteration */
                for(; samp >= 2; samp -= 2)
                {
                    /* Combine two, 16 to 32-bit sign extended hwords in one 64-bit dword */
                    combined_input_l = Q6_P_combine_RR(ptr_local_buf_16_ch_l[1], ptr_local_buf_16_ch_l[0]);
                    combined_input_r = Q6_P_combine_RR(ptr_local_buf_16_ch_r[1], ptr_local_buf_16_ch_r[0]);
                    ptr_local_buf_16_ch_l += 2;
                    ptr_local_buf_16_ch_r += 2;

                    /* Average the left and right channel input samples */
                    temp_res_64 = Q6_P_vavgw_PP_rnd(combined_input_l, combined_input_r);

                    /* Apply the gain on the averaged samples */
                    temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(combined_gain, temp_res_64);
                        /* Note that result is already in Q27 format*/

                    /* Accumulate the gain applied and averaged samples */
                    *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                    ptr_out_buf_64++;
                }
                /* If the number of samples are odd */
                if(samp)
                {
                    ptr_out_buf_32 = (int32_t *) ptr_out_buf_64;

                    /* Average the left and right channel input samples */
                    post_gain_sample_32 = ((int32_t)(*ptr_local_buf_16_ch_l++) + (int32_t)(*ptr_local_buf_16_ch_r++));
                    post_gain_sample_16 = (int16_t)(post_gain_sample_32 >> 1);

                    /* Apply the gain on the averaged input sample */
                    post_gain_sample_64 = s64_mult_s16_u16_shift(post_gain_sample_16, *pCurrentGain, 0);

                    /* Shift to Q27 Format and saturate to 32 bits */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                    /* Accumulate the gain applied and averaged 32-bit sample */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;
                }
            } /* Q15 -> Q27 */
            else
            {
                /* Advance the input pointers to the left and right channels */
                ptr_local_buf_16_ch_l = ptr_local_buf_start_16;
                ptr_local_buf_16_ch_r = (ptr_local_buf_start_16 + pCurrentInPort->unInPortPerChBufSize);
                ptr_out_buf_64 = pOutputBuf;

                /* Use scalar operations for accumulation to 64-bit buffer */
                for (j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                {
                    /* Average the left and right channel input samples */
                    post_gain_sample_32 = ((int32_t)(*ptr_local_buf_16_ch_l++) + (int32_t)(*ptr_local_buf_16_ch_r++));
                    post_gain_sample_16 = (int16_t)(post_gain_sample_32 >> 1);

                    /* Apply the gain on the averaged input sample */
                    post_gain_sample_64 = s64_mult_s16_u16_shift(post_gain_sample_16, *pCurrentGain, 0);

                    /* Shift to Q31 Format after rounding */
                    post_gain_sample_64 = s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor);

                    /* Accumulate the gain applied & averaged 64-bit sample */
                    (*ptr_out_buf_64) = Q6_P_add_PP(*ptr_out_buf_64, post_gain_sample_64);
                    ptr_out_buf_64++;
                }
            } /* Q15 -> Q31 */
        } /* in bytes < out bytes */
        else /* pCurrentOutPort->unBytesPerSample < pCurrentInPort->unBytesPerSample */
        {
            /**
             *    1. Gain is applied on 32 bit input port samples
             *    2. 64 bit intermediate result is saturated to 32 bits
             */

            ptr_local_buf_start_32 = (int32_t *) pInputBuf;

            int32_t local_shift_factor           = MT_MX_GAIN_SHIFT_FACT; /* Default shift factor for gain mpy in Q13 */
            int32_t local_rnd_factor             = 0;
            int32_t local_shift_factor_unit_gain = 0;
            int32_t local_rnd_factor_unit_gain   = 0;

            /* Identify the shift/rounding factors for Right shift from Qn -> Q15 */
            if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type)
            {
                local_shift_factor           -= BYTE_UPDOWN_CONV_SHIFT_FACT; /* Right shift from Q27 - Q15 */
                local_shift_factor_unit_gain -= BYTE_UPDOWN_CONV_SHIFT_FACT;
            }
            else /* BIT_CONV_IN32_TO_OUT16 */
            {
                local_shift_factor           -= BYTE_UPDOWN_CONV_SHIFT_FACT_32; /* Right shift from Q31 - Q15 */
                local_shift_factor_unit_gain -= BYTE_UPDOWN_CONV_SHIFT_FACT_32;
            }

            local_rnd_factor           = MtMx_CalculateRoundingFactor(local_shift_factor);
            local_rnd_factor_unit_gain = MtMx_CalculateRoundingFactor(local_shift_factor_unit_gain);

            if(*pCurrentGain < MT_MX_UNITY_GAIN)
            {
                /* Advance the input pointers to the left and right channels */
                ptr_local_buf_32_ch_l = ptr_local_buf_start_32;
                ptr_local_buf_32_ch_r = (ptr_local_buf_start_32 + (pCurrentInPort->unInPortPerChBufSize));
                ptr_out_buf_32 = (int32_t*)pOutputBuf;
                samp = unNumSamplesGainAndAccPerCh;

                /** If the output pointer is not 8-byte aligned, process one
                 *  sample to align the pointer.
                 */
                if(((uint32_t) ptr_out_buf_32) & 7)
                {

                    /* Average the left and right channel gain applied samples */
                    post_gain_sample_64 = ((int64_t)(*ptr_local_buf_32_ch_l++) + (int64_t)(*ptr_local_buf_32_ch_r++));
                    post_gain_sample_32 = (int32_t)(post_gain_sample_64 >> 1);

                    /* Apply the gain on the averaged input sample */
                    post_gain_sample_64 = s64_mult_s32_u16(post_gain_sample_32, *pCurrentGain);

                    /* Right Shift to Q15 format */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                    /* Accumulate the gain applied and averaged 32-bit sample */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;

                    /* Reduce sample count by 1 */
                    samp = samp - 1;
                }

                /* Scale the gain up from Q13 -> Q16 */
                curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 3;

                /* Combine int32_t into int64_t */
                combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                ptr_local_buf_64_ch_l = (int64_t *)(ptr_local_buf_32_ch_l);
                ptr_local_buf_64_ch_r = (int64_t *)(ptr_local_buf_32_ch_r);

                /* Mix both channels (L and R) into one channel output buffer */
                ptr_out_buf_64 = (int64_t *) ptr_out_buf_32;

                if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type) /* Q27 -> Q15 */
                {
                    /* Process two input samples per channel (L & R) per loop iteration */
                    for(; samp >=2; samp-=2)
                    {
                        /* Average the left and right channel input samples */
                        temp_res_64 = Q6_P_vavgw_PP_rnd(*ptr_local_buf_64_ch_l++,*ptr_local_buf_64_ch_r++);

                        /* Apply the gain on the averaged input samples */
                        temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(temp_res_64, combined_gain);

                        /* Add the rounding factor before shift to Q15 */
                        temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q27_TO_Q15);

                        /* Right shift from Q27 to Q15 */
                        temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT);

                        /* Accumulate the gain applied and averaged samples */
                        *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                        ptr_out_buf_64++;
                    }
                }
                else /* Q31 -> Q15 */
                {
                    /* Process two input samples per channel (L & R) per loop iteration */
                    for(; samp >=2; samp-=2)
                    {
                        /* Average the left and right channel input samples */
                        temp_res_64 = Q6_P_vavgw_PP_rnd(*ptr_local_buf_64_ch_l++,*ptr_local_buf_64_ch_r++);

                        /* Apply the gain on the averaged input samples */
                        temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(temp_res_64, combined_gain);

                        /* Add the rounding factor before shift to Q15 */
                        temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q15);

                        /* Right shift from Q31 to Q15 */
                        temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT_32);

                        /* Accumulate the gain applied and averaged samples */
                        *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                        ptr_out_buf_64++;
                    }
                }
                /* If the number of samples are odd */
                if(samp)
                {
                    ptr_local_buf_32_ch_l = (int32_t *)ptr_local_buf_64_ch_l;
                    ptr_local_buf_32_ch_r = (int32_t *) ptr_local_buf_64_ch_r;
                    ptr_out_buf_32 = (int32_t *) ptr_out_buf_64;

                    /* Average the left and right channel gain applied samples */
                    post_gain_sample_64 = ((int64_t)(*ptr_local_buf_32_ch_l++) + (int64_t)(*ptr_local_buf_32_ch_r++));
                    post_gain_sample_32 = (int32_t)(post_gain_sample_64 >> 1);

                    /* Apply the gain on the averaged input sample */
                    post_gain_sample_64 = s64_mult_s32_u16(post_gain_sample_32, *pCurrentGain);

                    /* Right Shift to Q15 format */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                    /* Accumulate the gain applied and averaged 32-bit sample */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;
                }
            }
            else if(MT_MX_UNITY_GAIN == *pCurrentGain)
            {
                /** This configuration is the most commonly used scenario for
                 *  audio use cases. Since the gain = unity, optimize by
                 *  skipping multiplication and taking advantage of vectorized
                 *  operations
                 */

                samp = unNumSamplesGainAndAccPerCh;
                /* Advance the input pointers to the left and right channels */
                ptr_local_buf_32_ch_l = ptr_local_buf_start_32;
                ptr_local_buf_32_ch_r = (ptr_local_buf_start_32 + (pCurrentInPort->unInPortPerChBufSize));
                ptr_out_buf_32   = (int32_t*)pOutputBuf;

                /** If the output pointer is not 8-byte aligned, process one
                 *  sample to align the pointer.
                 */
                if(((uint32_t) ptr_out_buf_32) & 7)
                {
                    /* Average the left and right channel samples */
                    post_gain_sample_64 = (int64_t)(((*ptr_local_buf_32_ch_l++)+(*ptr_local_buf_32_ch_r++))>>1);

                    /* Right Shift to Q15 format */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                    /* Accumulate the gain applied and averaged 32-bit sample */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;

                    /* Reduce sample count by 1 */
                    samp = samp - 1;
                }

                ptr_local_buf_64_ch_l = (int64_t *)(ptr_local_buf_32_ch_l);
                ptr_local_buf_64_ch_r = (int64_t *)(ptr_local_buf_32_ch_r);

                /* Mix both channels (L and R) into one channel output buffer */
                ptr_out_buf_64 = (int64_t *) ptr_out_buf_32;

                if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type) /* Q27 -> Q15 */
                {
                    /* Process two input samples per channel (L & R) per loop iteration */
                    for(; samp >=2; samp-=2)
                    {
                        /* Average the left and right channel samples */
                        temp_res_64 = Q6_P_vavgw_PP_rnd(*ptr_local_buf_64_ch_l++, *ptr_local_buf_64_ch_r++);

                        /* Add the rounding factor before shift to Q15 */
                        temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q27_TO_Q15);

                        /* Right shift from Q27 to Q15 */
                        temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT);

                        /* Accumulate the gain applied and averaged samples */
                        *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                        ptr_out_buf_64++;
                    }
                }
                else /* Q31 -> Q15 */
                {
                    /* Process two input samples per channel (L & R) per loop iteration */
                    for(; samp >=2; samp-=2)
                    {
                        /* Average the left and right channel samples */
                        temp_res_64 = Q6_P_vavgw_PP_rnd(*ptr_local_buf_64_ch_l++, *ptr_local_buf_64_ch_r++);

                        /* Add the rounding factor before shift to Q15 */
                        temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q15);

                        /* Right shift from Q31 to Q15 */
                        temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT_32);

                        /* Accumulate the gain applied and averaged samples */
                        *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                        ptr_out_buf_64++;
                    }
                }
                /* If the number of samples are odd */
                if(samp)
                {
                    ptr_local_buf_32_ch_l = (int32_t *)ptr_local_buf_64_ch_l;
                    ptr_local_buf_32_ch_r = (int32_t *)ptr_local_buf_64_ch_r;
                    ptr_out_buf_32 = (int32_t *) ptr_out_buf_64;

                    /* Average the left and right channel input samples */
                    post_gain_sample_64 = (int64_t)(((*ptr_local_buf_32_ch_l++) + (*ptr_local_buf_32_ch_r++))>>1);

                    /* Right Shift to Q15 format */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor_unit_gain),local_shift_factor_unit_gain));

                    /* Accumulate the gain applied and averaged 32-bit sample */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;
                }
            }
            else /* gain > unity gain */
            {
                /* Advance the input pointers to the left and right channels */
                ptr_local_buf_32_ch_l = ptr_local_buf_start_32;
                ptr_local_buf_32_ch_r = (ptr_local_buf_start_32 + pCurrentInPort->unInPortPerChBufSize);

                /* Mix both channels (L and R) into one channel output buffer */
                ptr_out_buf_32 = (int32_t*)pOutputBuf;

                for(j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                {
                    /* Average the left and right channel gain applied samples */
                    post_gain_sample_64 = ((int64_t)(*ptr_local_buf_32_ch_l++) + (int64_t)(*ptr_local_buf_32_ch_r++));
                    post_gain_sample_32 = (int32_t)(post_gain_sample_64 >> 1);

                    /* Apply the gain on the averaged input sample */
                    post_gain_sample_64 = s64_mult_s32_u16(post_gain_sample_32, *pCurrentGain);

                    /* Right Shift to Q15 format */
                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                    /* Accumulate the gain applied and averaged 32-bit sample */
                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                    ptr_out_buf_32++;
                }
            } /* gain> unity gain */
        } /* out byte < in byte */
    } /*unequal number of bytes */
}


static void MtMx_ApplyRampGainAndAccOnEqualNumChannels(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf)
{
	MatrixInPortInfoType       *pCurrentInPort = me->inPortParams[unInPortID];
	MatrixOutPortInfoType      *pCurrentOutPort = me->outPortParams[unOutPortID];
	mt_mx_struct_ramp_gain_t   *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
	mt_mx_struct_pspd_t           *pCurrentPspd                   = &(pCurrentInPort->structPspd[unOutPortID]);
	int16_t                    *pGainIncrementStep;
	uint16_t                   *pCurrentGain, *pTargetGain;
	uint32_t                   j, k, num_rem_steps;

	uint16_t                   unNumSamplesGainAndAccPerCh = pCurrentInPort->unAfeFrameSizeInSamples;
	int8_t                      *pInputBuf    = pCurrentPspd->pPspdOutBuf;
	int64_t                    *pOutputBuf = pAccBuf;
	int32_t                    *pOutputBuf32 = (int32_t*)pAccBuf;
	uint16_t                   num_bytes_per_ch_gain_and_acc = (unNumSamplesGainAndAccPerCh * pCurrentInPort->unBytesPerSample);

	/* a. Apply ramp gain to minimum of remaining and number of afe frames steps, in 1 afe frame step size */
	num_rem_steps = s32_min_s32_s32(pCurrentInputOutputPath->unNumRemSteps, pCurrentOutPort->unNumAfeFrames);

	for (k = 0; k < num_rem_steps; k++)
	{
		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
		pGainIncrementStep = &(pCurrentInputOutputPath->nGainIncDecStep[0]);

		MtMx_GainAndAccOnEqualNumChannels(unNumSamplesGainAndAccPerCh,
				pCurrentOutPort->unNumChannels,
				pCurrentInPort, pCurrentOutPort,
				pInputBuf, pOutputBuf, pCurrentGain);

		for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
		{
			if(FALSE == pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j])
			{
				/* Regular ramp gain mode */
				pCurrentGain[j] += pGainIncrementStep[j];
			}
			else
			{
				/* Slow ramp gain mode */
				if(0 == pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j])
				{
					pCurrentGain[j] += pGainIncrementStep[j];
					pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] = pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j];
				}
				else
				{
					pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] --;
				}
			}
		}

		/* Increment the pointers to point to next channel */
		pInputBuf += num_bytes_per_ch_gain_and_acc;
		if(32 == pCurrentOutPort->unBitwidth)
		{
		    pOutputBuf += unNumSamplesGainAndAccPerCh;
		}
		else
		{
		    pOutputBuf32 += unNumSamplesGainAndAccPerCh;
		    pOutputBuf = (int64_t*)pOutputBuf32;
		}

	}

	/* If number of remaining steps is greater than or equal to
      frame size of the local buffer
	 */
	if(pCurrentInputOutputPath->unNumRemSteps >= pCurrentOutPort->unNumAfeFrames)
	{
		/* Check if number of remaining steps is now equal to zero.
         If it is, check if current gain on each of the channels is
         equal to target gain, if not, make it equal.
		 */
		pCurrentInputOutputPath->unNumRemSteps -= pCurrentOutPort->unNumAfeFrames;

		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

		if (0 == pCurrentInputOutputPath->unNumRemSteps)
		{
			pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE;
			pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);
			for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
			{
				*pCurrentGain++ = *pTargetGain++;
			}
		}
	}
	else /* number of remaining steps is less than frame size of local buffer */
	{
		uint16_t unNumStepsAfterRampGain = pCurrentOutPort->unNumAfeFrames - pCurrentInputOutputPath->unNumRemSteps;

		pCurrentInputOutputPath->unNumRemSteps  = 0;
		pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE;

		/* b. After applying ramp gain, equate current gain to target gain */
		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
		pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);

		for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
		{
			*pCurrentGain++ = *pTargetGain++;
		}

		/* c. Apply gain immediately to the remaining samples in the local buffer */
		for (k = 0; k < unNumStepsAfterRampGain; k++)
		{
			pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

			MtMx_GainAndAccOnEqualNumChannels(unNumSamplesGainAndAccPerCh,
					pCurrentOutPort->unNumChannels,
					pCurrentInPort, pCurrentOutPort,
					pInputBuf, pOutputBuf, pCurrentGain);

			/* Increment the pointers to point to next channel */
			pInputBuf += num_bytes_per_ch_gain_and_acc;
			if(32 == pCurrentOutPort->unBitwidth)
			{
			    pOutputBuf += unNumSamplesGainAndAccPerCh;
			}
			else
			{
			    pOutputBuf32 += unNumSamplesGainAndAccPerCh;
			    pOutputBuf = (int64_t*)pOutputBuf32;
			}

		}
	}
}

static void MtMx_ApplyImmGainAndAccOnEqualNumChannels(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf)
{
    MatrixInPortInfoType       *pCurrentInPort = me->inPortParams[unInPortID];
    MatrixOutPortInfoType      *pCurrentOutPort = me->outPortParams[unOutPortID];
    mt_mx_struct_ramp_gain_t   *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
    mt_mx_struct_pspd_t        *pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
    uint16_t                   *pCurrentGain;
    int8_t                     *pInputBuf;
    int64_t                    *pOutputBuf;

    /* Pointer to start location in inport local buffer */
    pInputBuf = pCurrentPspd->pPspdOutBuf;

    /* Pointer to output port accumulator buffer */
    pOutputBuf = pAccBuf;

    /* Pointer to in-port gain array*/
    pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

    MtMx_GainAndAccOnEqualNumChannels(pCurrentInPort->unInPortPerChBufSize,
                    pCurrentOutPort->unNumChannels,
                    pCurrentInPort, pCurrentOutPort,
                    pInputBuf, pOutputBuf, pCurrentGain);
}

static void MtMx_ApplyRampGainAndAccOnStereoOutMonoIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf)
{
    MatrixInPortInfoType     *pCurrentInPort  = me->inPortParams[unInPortID];
    MatrixOutPortInfoType    *pCurrentOutPort = me->outPortParams[unOutPortID];
    mt_mx_struct_ramp_gain_t *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
    mt_mx_struct_pspd_t      *pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
    int16_t                  *pGainIncrementStep;
    uint16_t                 *pCurrentGain, *pTargetGain;
    uint32_t                 j, k, num_rem_steps;

    uint16_t                 unNumSamplesGainAndAccPerCh = pCurrentInPort->unAfeFrameSizeInSamples;
    int8_t                   *pInputBuf    = pCurrentPspd->pPspdOutBuf;
    int64_t                  *pOutputBuf   = pAccBuf;
    int32_t                  *pOutputBuf32 = (int32_t*) pAccBuf;
    uint16_t                 num_bytes_per_ch_gain_and_acc = (unNumSamplesGainAndAccPerCh * pCurrentInPort->unBytesPerSample);

	/* a. Apply ramp gain to minimum of remaining and number of afe frames steps, in 1 afe frame step size */
	num_rem_steps = s32_min_s32_s32(pCurrentInputOutputPath->unNumRemSteps, pCurrentOutPort->unNumAfeFrames);

	for (k = 0; k < num_rem_steps; k++)
	{
		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
		pGainIncrementStep = &(pCurrentInputOutputPath->nGainIncDecStep[0]);

		MtMx_GainAndAccOnStereoOutMonoIn(unNumSamplesGainAndAccPerCh,
				pCurrentInPort, pCurrentOutPort,
				pInputBuf, pOutputBuf, pCurrentGain);


		for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
		{
			if(FALSE == pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j])
			{
				/* Regular ramp gain mode */
				pCurrentGain[j] += pGainIncrementStep[j];
			}
			else
			{
				/* Slow ramp gain mode */
				if(0 == pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j])
				{
					pCurrentGain[j] += pGainIncrementStep[j];
					pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] = pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j];
				}
				else
				{
					pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] --;
				}
			}
		}

		/* Increment the pointers to point to next channel */
		pInputBuf += num_bytes_per_ch_gain_and_acc;
		if(32 == pCurrentOutPort->unBitwidth)
		{
		    pOutputBuf += unNumSamplesGainAndAccPerCh;
		}
		else
		{
		    pOutputBuf32 += unNumSamplesGainAndAccPerCh;
		    pOutputBuf = (int64_t*)pOutputBuf32;
		}
	}

	/* If number of remaining steps is greater than or equal to
      frame size of the local buffer
	 */
	if(pCurrentInputOutputPath->unNumRemSteps >= pCurrentOutPort->unNumAfeFrames)
	{
		/* Check if number of remaining steps is now equal to zero.
         If it is, check if current gain on each of the channels is
         equal to target gain, if not, make it equal.
		 */
		pCurrentInputOutputPath->unNumRemSteps -= pCurrentOutPort->unNumAfeFrames;

		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

		if (0 == pCurrentInputOutputPath->unNumRemSteps)
		{
			pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE;
			pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);
			for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
			{
				*pCurrentGain++ = *pTargetGain++;
			}
		}
	}
	else /* number of remaining steps is less than frame size of local buffer */
	{
		uint16_t unNumStepsAfterRampGain = pCurrentOutPort->unNumAfeFrames - pCurrentInputOutputPath->unNumRemSteps;

		pCurrentInputOutputPath->unNumRemSteps  = 0;
		pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE;

		/* b. After applying ramp gain, equate current gain to target gain */
		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
		pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);

		for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
		{
			*pCurrentGain++ = *pTargetGain++;
		}

		/* c. Apply gain immediately to the remaining samples in the local buffer */
		for (k = 0; k < unNumStepsAfterRampGain; k++)
		{
			pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

			MtMx_GainAndAccOnStereoOutMonoIn(unNumSamplesGainAndAccPerCh,
					pCurrentInPort, pCurrentOutPort,
					pInputBuf, pOutputBuf, pCurrentGain);


			/* Increment the pointers to point to next channel */
			pInputBuf += num_bytes_per_ch_gain_and_acc;
			if(32 == pCurrentOutPort->unBitwidth)
			{
			    pOutputBuf += unNumSamplesGainAndAccPerCh;
			}
			else
			{
			    pOutputBuf32 += unNumSamplesGainAndAccPerCh;
			    pOutputBuf = (int64_t*)pOutputBuf32;
			}
		}
	}
}

static void MtMx_ApplyImmGainAndAccOnStereoOutMonoIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf)
{
    MatrixInPortInfoType     *pCurrentInPort = me->inPortParams[unInPortID];
    MatrixOutPortInfoType    *pCurrentOutPort = me->outPortParams[unOutPortID];
    mt_mx_struct_ramp_gain_t *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
    mt_mx_struct_pspd_t      *pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
    uint16_t                 *pCurrentGain;
    int8_t                   *pInputBuf = pCurrentPspd->pPspdOutBuf;
    int64_t                  *pOutputBuf = pAccBuf;

    pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

    MtMx_GainAndAccOnStereoOutMonoIn(pCurrentInPort->unInPortPerChBufSize,
                    pCurrentInPort, pCurrentOutPort,
                    pInputBuf, pOutputBuf, pCurrentGain);
}

static void MtMx_ApplyRampGainAndAccOnMonoOutStereoIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf)
{
    MatrixInPortInfoType     *pCurrentInPort = me->inPortParams[unInPortID];
    MatrixOutPortInfoType    *pCurrentOutPort = me->outPortParams[unOutPortID];
    mt_mx_struct_ramp_gain_t *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
    mt_mx_struct_pspd_t      *pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
    int16_t                  *pGainIncrementStep;
    uint16_t                 *pCurrentGain, *pTargetGain;
    uint32_t                 j, k, num_rem_steps;
    uint16_t                 unNumSamplesGainAndAccPerCh = pCurrentInPort->unAfeFrameSizeInSamples;
    int8_t                   *pInputBuf = pCurrentPspd->pPspdOutBuf;
    int64_t                  *pOutputBuf = pAccBuf;
    int32_t                  *pOutputBuf32 = (int32_t*) pAccBuf;
    uint16_t                 num_bytes_per_ch_gain_and_acc = (unNumSamplesGainAndAccPerCh * pCurrentInPort->unBytesPerSample);

	/* a. Apply ramp gain to minimum of remaining and number of afe frames steps, in 1 afe frame step size */
	num_rem_steps = s32_min_s32_s32(pCurrentInputOutputPath->unNumRemSteps, pCurrentOutPort->unNumAfeFrames);

	for (k = 0; k < num_rem_steps; k++)
	{
		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
		pGainIncrementStep = &(pCurrentInputOutputPath->nGainIncDecStep[0]);

		MtMx_GainAndAccOnMonoOutStereoIn(unNumSamplesGainAndAccPerCh,
				pCurrentInPort, pCurrentOutPort,
				pInputBuf, pOutputBuf, pCurrentGain);

		for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
		{
			if(FALSE == pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j])
			{
				/* Regular ramp gain mode */
				pCurrentGain[j] += pGainIncrementStep[j];
			}
			else
			{
				/* Slow ramp gain mode */
				if(0 == pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j])
				{
					pCurrentGain[j] += pGainIncrementStep[j];
					pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] = pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j];
				}
				else
				{
					pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] --;
				}
			}
		}

		/* Increment the pointers to point to next channel */
		pInputBuf += num_bytes_per_ch_gain_and_acc;
                if(32 == pCurrentOutPort->unBitwidth)
                {
                    pOutputBuf += unNumSamplesGainAndAccPerCh;
                }
                else
                {
                    pOutputBuf32 += unNumSamplesGainAndAccPerCh;
                    pOutputBuf = (int64_t*)pOutputBuf32;
                }
	}

	/* If number of remaining steps is greater than or equal to
      frame size of the local buffer
	 */
	if(pCurrentInputOutputPath->unNumRemSteps >= pCurrentOutPort->unNumAfeFrames)
	{
		/* Check if number of remaining steps is now equal to zero.
         If it is, check if current gain on each of the channels is
         equal to target gain, if not, make it equal.
		 */
		pCurrentInputOutputPath->unNumRemSteps -= pCurrentOutPort->unNumAfeFrames;

		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

		if (0 == pCurrentInputOutputPath->unNumRemSteps)
		{
			pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE;
			pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);
			for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
			{
				*pCurrentGain++ = *pTargetGain++;
			}
		}
	}
	else /* number of remaining steps is less than frame size of local buffer */
	{
		uint16_t unNumStepsAfterRampGain = pCurrentOutPort->unNumAfeFrames - pCurrentInputOutputPath->unNumRemSteps;

		pCurrentInputOutputPath->unNumRemSteps  = 0;
		pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE;

		/* b. After applying ramp gain, equate current gain to target gain */
		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
		pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);

		for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
		{
			*pCurrentGain++ = *pTargetGain++;
		}

		/* c. Apply gain immediately to the remaining samples in the local buffer */
		for (k = 0; k < unNumStepsAfterRampGain; k++)
		{
			pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

			MtMx_GainAndAccOnMonoOutStereoIn(unNumSamplesGainAndAccPerCh,
					pCurrentInPort, pCurrentOutPort,
					pInputBuf, pOutputBuf, pCurrentGain);

			/* Increment the pointers to point to next channel */
			pInputBuf += num_bytes_per_ch_gain_and_acc;
	        if(32 == pCurrentOutPort->unBitwidth)
	        {
	            pOutputBuf += unNumSamplesGainAndAccPerCh;
	        }
	        else
	        {
	            pOutputBuf32 += unNumSamplesGainAndAccPerCh;
	            pOutputBuf = (int64_t*)pOutputBuf32;
	        }
		}
	}
}

static void MtMx_ApplyImmGainAndAccOnMonoOutStereoIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf)
{
    MatrixInPortInfoType     *pCurrentInPort = me->inPortParams[unInPortID];
    MatrixOutPortInfoType    *pCurrentOutPort = me->outPortParams[unOutPortID];
    mt_mx_struct_ramp_gain_t *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
    mt_mx_struct_pspd_t      *pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
    uint16_t                 *pCurrentGain;
    int8_t                   *pInputBuf = pCurrentPspd->pPspdOutBuf;
    int64_t                  *pOutputBuf = pAccBuf;

    pCurrentGain             = &(pCurrentInputOutputPath->unCurrentGain[0]);

	MtMx_GainAndAccOnMonoOutStereoIn(pCurrentInPort->unInPortPerChBufSize,
			pCurrentInPort, pCurrentOutPort,
			pInputBuf, pOutputBuf, pCurrentGain);

}

void MtMx_ApplyOutputBufferTS(This_t *me, uint32_t unOutPortID, elite_msg_data_buffer_t *pOutputBuf)
{
	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];

	if(FALSE == pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS)
	{
		//Relative time-stamp mode. This is the default behavior.
		uint64_t ullCurrentWallClockTime = (NULL != pCurrentOutPort->avt_drv_handle) ? (avtimer_drv_get_time(pCurrentOutPort->avt_drv_handle)):(0);
		if (TRUE == pCurrentOutPort->bIsFirstOutBufYetToBeSent)
		{
			mt_mx_increment_time(&pCurrentOutPort->llSessionTime, pCurrentOutPort->unFrameDurationInUsec);
			pCurrentOutPort->bIsFirstOutBufYetToBeSent = FALSE;
		}
		else
		{
			uint64_t ullTimeDiff;
			if (ullCurrentWallClockTime > pCurrentOutPort->ullPreviousWCTime)
			{
				ullTimeDiff = ullCurrentWallClockTime - pCurrentOutPort->ullPreviousWCTime;
			}
			else
			{
				uint64_t ullTimeDiff1;
				ullTimeDiff1 = 0xFFFFFFFFFFFFFFFFLL - pCurrentOutPort->ullPreviousWCTime;
				ullTimeDiff = ullTimeDiff1 + ullCurrentWallClockTime;
			}
			uint64_t ullJitterWindow = 2*MT_MX_SAMPLES_TO_USEC(pCurrentOutPort->unOutPortPerChBufSize, pCurrentOutPort->unSampleRate);
			if (ullJitterWindow >= ullTimeDiff)
			{
				mt_mx_increment_time(&pCurrentOutPort->llSessionTime, pCurrentOutPort->unFrameDurationInUsec);
			}
			else
			{
				pCurrentOutPort->llSessionTime.int_part += ullTimeDiff;
			}
		}
		pOutputBuf->ullTimeStamp = pCurrentOutPort->llSessionTime.int_part;
		pCurrentOutPort->ullPreviousWCTime = ullCurrentWallClockTime;
	}
	else
	{
		//Absolute time-stamp mode. O/p port will sync to top priority i/p port's TS.
		uint32_t tempCurrTopPrioInPort = 0;
		if(pCurrentOutPort->inPortsTopPriorityMask)
		{
			MatrixInPortInfoType    *pCurrTopPrioInPort_temp;
			tempCurrTopPrioInPort = Q6_R_ct0_R(pCurrentOutPort->inPortsTopPriorityMask);
			pCurrTopPrioInPort_temp = me->inPortParams[tempCurrTopPrioInPort];

			pCurrentOutPort->llSessionTime.int_part = pCurrTopPrioInPort_temp->ullIncomingBufferTS;
			pOutputBuf->ullTimeStamp = pCurrTopPrioInPort_temp->ullIncomingBufferTS;
		}
	}

	//Set the timestamp-valid-flag (bit 31 of pOutputBuf->nFlag); Do this only for live port (RT) driven TX matrix.
	if(TRUE == pCurrentOutPort->bIsTopPriorityInputPortLive)
	{
		asm_set_timestamp_valid_flag(&(pOutputBuf->nFlag), 1);
	}

	//If non-live, burst mode, the TS validity flag is not applicable and hence should not be set.
	if(FALSE == pCurrentOutPort->bIsTopPriorityInputPortLive)
	{
		uint32_t tempCurrTopPrioInPort = 0;
		if(pCurrentOutPort->inPortsTopPriorityMask)
		{
			MatrixInPortInfoType    *pCurrTopPrioInPort_temp;
			tempCurrTopPrioInPort = Q6_R_ct0_R(pCurrentOutPort->inPortsTopPriorityMask);
			pCurrTopPrioInPort_temp = me->inPortParams[tempCurrTopPrioInPort];
			if (FALSE == pCurrTopPrioInPort_temp->bIsInPortBurstMode)
			{
				asm_set_timestamp_valid_flag(&(pOutputBuf->nFlag), 1);
			}
		}
	}
#ifdef MT_MX_EXTRA_DEBUG
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MxAt o/p port %lu Propagating TS [%lu, %lu]",
			unOutPortID, (uint32_t)(pOutputBuf->ullTimeStamp>>32),(uint32_t)(pOutputBuf->ullTimeStamp));
#endif
}

ADSPResult MtMx_ReInitLocalBuffer(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];
	uint16_t i;

	if ( (pCurrentInPort->unNewNumChannels != pCurrentInPort->unNumChannels) ||
			(pCurrentInPort->unNewBytesPerSample != pCurrentInPort->unBytesPerSample) ||
			(pCurrentInPort->unNewSampleRate != pCurrentInPort->unSampleRate))
	{
		//Free the local buffer memory
		if(pCurrentInPort->pStartLoc)
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Freeing pStartLoc 0x%p", me->mtMxID, pCurrentInPort->pStartLoc);
			MTMX_ALIGNED_FREE(pCurrentInPort->pStartLoc);
		}

		//Update media format parameters and calculate frame duration and buffer sizes
		MtMx_InPortUpdateMediaFmtParams(me, unInPortID, pCurrentInPort->unNewSampleRate,
				pCurrentInPort->unNewNumChannels, pCurrentInPort->unNewBytesPerSample,pCurrentInPort->unNewBitwidth );

		//Malloc for the new inport local buffers
		pCurrentInPort->pStartLoc = (int8_t*)qurt_elite_memory_aligned_malloc(pCurrentInPort->unBytesPerSample * pCurrentInPort->unInPortBufSize,
				8, MTMX_SVC_INPORT_LOCAL_BUF);
		if (!pCurrentInPort->pStartLoc)
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu failed to alloc mem for pStartLoc", me->mtMxID, unInPortID);
			return ADSP_ENOMEMORY;
		}
		else
		{
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu pStartLoc: 0x%p, Size: %lu",
					me->mtMxID, unInPortID, pCurrentInPort->pStartLoc, pCurrentInPort->unBytesPerSample * pCurrentInPort->unInPortBufSize);
		}

		if(pCurrentInPort->bIsPortLive)
		{
			//TX matrix i/p port has to report delay for AV-Sync purposes.
			if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID && NULL != pCurrentInPort->punMtMxInDelay)
			{
				*(pCurrentInPort->punMtMxInDelay) = 0;
				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: FrameSize(us) %lu, TotDelay (micro-sec) %lu",
						me->mtMxID, unInPortID, (uint32_t)pCurrentInPort->unFrameDurationInUsec.int_part, *(pCurrentInPort->punMtMxInDelay));
			}
		}
	}

	//Update channel mapping regardless of whether num channels changed or not
	for (i = 0; i < MT_MX_NUM_CHANNELS_EIGHT; i++)
	{
		pCurrentInPort->unChMapToChRevLUMask[i] = 0;
	}

	for (i = 0; i < pCurrentInPort->unNumChannels; i++)
	{
		pCurrentInPort->unChannelMapping[i] = pCurrentInPort->unNewChannelMapping[i];

		if((pCurrentInPort->unChannelMapping[i] > MAX_CHANNEL_MAPPING_NUMBER) ||
				(pCurrentInPort->unChannelMapping[i] < MT_MX_NUM_CHANNELS_ONE))
		{
			pCurrentInPort->unChannelMapping[i] = 0;
		}
		else
		{
			uint8_t unTempChMap = pCurrentInPort->unChannelMapping[i];
			pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1] |= (1 << i);
		}
	}

	MtMx_PrintDebugMediaTypeInformationInputPort(me, unInPortID);

	//Reset number of samples required to fill local buffer to buffer size.
	pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;
	MtMx_InitInPortLocalBufPtrs(me, unInPortID);

	pCurrentInPort->bIsLocalBufFull = FALSE;
	pCurrentInPort->bIsLocalBufEmpty = TRUE;

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
			if((0 == pCurrentOutPort->accInPortsMask) &&
					(pCurrentOutPort->unNativeModeFlags) &&
					(pCurrentOutPort->bIsOutPortInNativeModePendingReInit))
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu MtMx_ReInitLocalBuffer i/p port %lu calling Reinit on Native mode o/p port %lu because no acc. pending.",
						me->mtMxID, unInPortID, unOutPortID);
				if (ADSP_FAILED(result = MtMx_ReInitNativeModeOutport(me, unOutPortID)))
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_ReInitLocalBuffer o/p port %lu MtMx_ReInitNativeModeOutport failed. Res = %d.", unOutPortID, result);
					return result;
				}
			}
		}
	}

	return ADSP_EOK;
}

ADSPResult MtMx_OutportSendMediaTypeMsgDS(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	ADSPResult                 result = ADSP_EOK;
	qurt_elite_bufmgr_node_t       bufMgrNode;
	int                        nActualSize;
	elite_msg_any_t            msg;

	if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_data_media_type_apr_t) +
			sizeof(elite_multi_channel_pcm_fmt_blk_t),
			&bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: couldn't obtain buf to send media_type down");
		return ADSP_ENORESOURCE;
	}

	//Setup Media type payload
	elite_msg_data_media_type_apr_t* pMediaTypePayload = (elite_msg_data_media_type_apr_t*)bufMgrNode.pBuffer;
	pMediaTypePayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pMediaTypePayload->pResponseQ = NULL;
	pMediaTypePayload->unClientToken = 0;
	pMediaTypePayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR ;
	pMediaTypePayload->unMediaFormatID = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

	//Setup the Media Fmt payload
	elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk = (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaTypePayload);

	pMediaFormatBlk->num_channels = pCurrentOutPort->unNumChannels;
	for (uint16_t j = 0; j < pCurrentOutPort->unNumChannels; j++)
	{
		pMediaFormatBlk->channel_mapping[j] = pCurrentOutPort->unChannelMapping[j];
	}
	pMediaFormatBlk->bits_per_sample = pCurrentOutPort->unBitwidth;
	pMediaFormatBlk->sample_rate = pCurrentOutPort->unSampleRate;
	pMediaFormatBlk->is_signed = TRUE;
	pMediaFormatBlk->is_interleaved = FALSE;

	msg.unOpCode = ELITE_DATA_MEDIA_TYPE;
	msg.pPayload = (void*) pMediaTypePayload;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: O/p port %lu sending media_format msg downstream", me->mtMxID, unOutPortID);
	MtMx_PrintDebugMediaTypeInformationOutputPort(me, unOutPortID);
	if(ADSP_FAILED(result = qurt_elite_queue_push_back(pCurrentOutPort->pDownstreamPeerHandle->dataQ, (uint64_t*)&msg )))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "o/p port failed to send ELITE_DATA_MEDIA_TYPE msg");
		elite_msg_push_payload_to_returnq(pMediaTypePayload->pBufferReturnQ,(elite_msg_any_payload_t*)pMediaTypePayload);
		return result;
	}
	return ADSP_EOK;
}

ADSPResult MtMx_OutportAllocBuffers(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	ADSPResult                 result = ADSP_EOK;
	elite_msg_data_buffer_t    *pDataPayload;

	MtMx_SetOutPortFrameDuration(me, unOutPortID);

	if ((pCurrentOutPort->unBufSizeInBytes != (pCurrentOutPort->unOutPortPerChBufSize * pCurrentOutPort->unNumChannels * pCurrentOutPort->unBytesPerSample))
			||(0 == pCurrentOutPort->unNumOutputBufsPendingRelease))
	{
		pCurrentOutPort->unBufSize = pCurrentOutPort->unOutPortPerChBufSize * pCurrentOutPort->unNumChannels;
		pCurrentOutPort->unBufSizeInBytes = pCurrentOutPort->unOutPortPerChBufSize * pCurrentOutPort->unNumChannels * pCurrentOutPort->unBytesPerSample;

		uint32_t req_size = GET_ELITEMSG_DATABUF_REQ_SIZE(pCurrentOutPort->unBytesPerSample * pCurrentOutPort->unBufSize);

		for (uint16_t j = 0; j < pCurrentOutPort->unNumOutputBufs; j++)
		{
			// buffer size = frame size (msec) * sample rate (samples/channel/msec) * # of channels
			pDataPayload = (elite_msg_data_buffer_t*)qurt_elite_memory_malloc(req_size, MTMX_SVC_OUTPORT_BUF);

			if (!pDataPayload)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Couldn't allocate memory for o/p port's o/p buffers");

				// Destroy the bufQ and j associated output buffers
				elite_svc_destroy_buf_queue(pCurrentOutPort->bufQ, j+pCurrentOutPort->unNumOutputBufsPendingRelease);
				pCurrentOutPort->unNumOutputBufsPendingRelease = 0;
				pCurrentOutPort->bIsOutPortPendingReleaseBuffers = FALSE;
				return ADSP_ENOMEMORY;
			}

			pDataPayload->nActualSize    = pCurrentOutPort->unBufSize * pCurrentOutPort->unBytesPerSample;
			pDataPayload->nMaxSize       = pCurrentOutPort->unBufSize * pCurrentOutPort->unBytesPerSample;

			if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(pCurrentOutPort->bufQ, (elite_msg_any_payload_t*)pDataPayload )))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to push output buffer onto bufQ");

				// a. free the memory allocated for this output buffer
				MTMX_FREE(pDataPayload);

				// b. destroy the bufQ and previous j associated output buffers
				elite_svc_destroy_buf_queue(pCurrentOutPort->bufQ, j+pCurrentOutPort->unNumOutputBufsPendingRelease);
				pCurrentOutPort->unNumOutputBufsPendingRelease = 0;
				pCurrentOutPort->bIsOutPortPendingReleaseBuffers = FALSE;
				return result;
			}

			MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu output port %lu, buffer#%d: 0x%lx, size %lu",
					me->mtMxID, unOutPortID, j, (uint32_t)pDataPayload, req_size);
		}

		pCurrentOutPort->unNumOutputBufsPendingRelease +=  pCurrentOutPort->unNumOutputBufs;
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu output port %lu, #bufs: %lu",
				me->mtMxID, unOutPortID, pCurrentOutPort->unNumOutputBufsPendingRelease);
	}
   else // If new output buffers are not allocated, update the bufsize variables as per latest media format information
   {
		pCurrentOutPort->unBufSize = pCurrentOutPort->unOutPortPerChBufSize * pCurrentOutPort->unNumChannels;
	 	pCurrentOutPort->unBufSizeInBytes = pCurrentOutPort->unOutPortPerChBufSize * pCurrentOutPort->unNumChannels * pCurrentOutPort->unBytesPerSample;
   }

	//RX matrix o/p port has to report buffering delay for AV-Sync purposes.
	if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID && NULL != pCurrentOutPort->punMtMxOutDelay)
	{
		//Output queue buffers
		*(pCurrentOutPort->punMtMxOutDelay) = pCurrentOutPort->unNumOutputBufs * pCurrentOutPort->unFrameDurationInUsec.int_part;

		//Accumulation buffer
		*(pCurrentOutPort->punMtMxOutDelay) += pCurrentOutPort->unFrameDurationInUsec.int_part;

		MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu: #buf %lu, BufSize(us) %lu, TotDelay (micro-sec) %lu",
				me->mtMxID, unOutPortID, pCurrentOutPort->unNumOutputBufs,  (uint32_t)pCurrentOutPort->unFrameDurationInUsec.int_part, *(pCurrentOutPort->punMtMxOutDelay));
	}

	//Allocate accumulator buffer for this o/p port, 8 bytes aligned
	pCurrentOutPort->pAccBuf = (int64_t *)qurt_elite_memory_aligned_malloc((pCurrentOutPort->unBufSize * sizeof(int64_t)),
			8, MTMX_SVC_OUTPORT_ACC_BUF);

	//if malloc fails, clean up any allocated memory and bailout gracefully
	if (!pCurrentOutPort->pAccBuf)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Couldn't allocate memory for o/p port's accBuf");

		// a. destroy bufQ and associated buffers
		elite_svc_destroy_buf_queue(pCurrentOutPort->bufQ, pCurrentOutPort->unNumOutputBufsPendingRelease);
		pCurrentOutPort->unNumOutputBufsPendingRelease = 0;
		pCurrentOutPort->bIsOutPortPendingReleaseBuffers = FALSE;
		return ADSP_ENOMEMORY;
	}

	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu output port %lu acc. buffer: 0x%llx, size %lu",
			me->mtMxID, unOutPortID,  pCurrentOutPort->pAccBuf, (pCurrentOutPort->unBufSize * sizeof(int64_t)));

	//Zero out the acc buf
	memset(pCurrentOutPort->pAccBuf, 0, pCurrentOutPort->unBufSize * sizeof(int64_t));

	return ADSP_EOK;
}

void MtMx_MoveOutportToActiveState(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	int k = qurt_elite_queue_get_channel_bit(pCurrentOutPort->bufQ);
	me->unBufBitfield = me->unBufBitfield | k;

	if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		MtMx_AddOutputPortToWaitMask(me, unOutPortID);
	}

	int t = 31 - (Q6_R_cl0_R( (k) ));
	me->outPortIDMap[t] = unOutPortID;

	pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_ACTIVE;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: moved o/p port %lu to ACTIVE state", me->mtMxID, unOutPortID);
}

void MxAt_NewInPortToUpdateInPortsTopPriorityMaskOfOutPort(This_t *me, uint32_t unInPortID, uint32_t unOutPortID)
{
	MatrixInPortInfoType    *pCurrentInPort  = me->inPortParams[unInPortID];
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	MatrixInPortInfoType    *pCurrentInPort_temp;

	bool_t bTopPrioPortFound = FALSE;

	uint32_t currConnInputPorts = pCurrentOutPort->inPortsMask;
	uint32_t currInPortsTopPriorityMask, unInPortID_temp;

	// If this is the first i/p port to connect to this o/p port, assign this i/p port as top priority
	if(0 == pCurrentOutPort->inPortsTopPriorityMask)
	{
		pCurrentOutPort->inPortsTopPriorityMask |= (1 << unInPortID);
		pCurrentOutPort->bIsTopPriorityInputPortLive = pCurrentInPort->bIsPortLive;
		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu is top prio!, o/p port %lu, top prio mask %lu",
				me->mtMxID, unInPortID, unOutPortID, pCurrentOutPort->inPortsTopPriorityMask);

		bTopPrioPortFound = TRUE;
		return;
	}

	currInPortsTopPriorityMask = pCurrentOutPort->inPortsTopPriorityMask;

	MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu o/p port %lu, OIPTopPrioM %lu OCurrConnIM %lu",
			me->mtMxID, unInPortID, unOutPortID, pCurrentOutPort->inPortsTopPriorityMask, pCurrentOutPort->inPortsMask);

	// Traverse through the list of i/p ports currently connected to this o/p to check and update the top priority i/p port
	while(currConnInputPorts)
	{
		unInPortID_temp = Q6_R_ct0_R(currConnInputPorts);
		currConnInputPorts ^= (1 << unInPortID_temp);
		pCurrentInPort_temp = me->inPortParams[unInPortID_temp];

		// If the current top priority i/p port matches,
		if((currInPortsTopPriorityMask) & (1 << unInPortID_temp))
		{
			// Check if the match is a non-live port and the newly added port is live
			if((FALSE == pCurrentInPort_temp->bIsPortLive) && (TRUE == pCurrentInPort->bIsPortLive))
			{
				// If so, the top priority i/p ports needs to be updated to the new live i/p port
				pCurrentOutPort->inPortsTopPriorityMask &= ~(1 << unInPortID_temp);
				pCurrentOutPort->inPortsTopPriorityMask |=  (1 << unInPortID);
				pCurrentOutPort->bIsTopPriorityInputPortLive = TRUE;
				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Top prio i/p port changed from %lu to %lu!, o/p port %lu, top prio mask %lu",
						me->mtMxID, unInPortID_temp, unInPortID, unOutPortID, pCurrentOutPort->inPortsTopPriorityMask);

				bTopPrioPortFound = TRUE;
				return;
			}
			else
			{
				// Match found, but no change in top priority i/p port
				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: No change in top prio i/p port. Top Prio: %lu, New port: %lu, o/p port %lu, top prio mask %lu",
						me->mtMxID, unInPortID_temp, unInPortID, unOutPortID, pCurrentOutPort->inPortsTopPriorityMask);

				bTopPrioPortFound = TRUE;
				return;
			}
		}
	}

	//At this point, this connection seems to be initiated because there was a switch in the top priority port, possibly due to device switching.
	//Therefore, this new i/p port will automatically become the top priority i/p port for this o/p port.
	if(FALSE == bTopPrioPortFound)
	{
		pCurrentOutPort->inPortsTopPriorityMask = 0;
		pCurrentOutPort->inPortsTopPriorityMask |= (1<<unInPortID);
		pCurrentOutPort->bIsTopPriorityInputPortLive = pCurrentInPort->bIsPortLive;
		bTopPrioPortFound = TRUE;
		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: New Top prio i/p port %lu!, o/p port %lu, top prio mask %lu",
				me->mtMxID, unInPortID, unOutPortID, pCurrentOutPort->inPortsTopPriorityMask);
	}
}

void MxAt_ClosingInPortToUpdateInPortsTopPriorityMaskOfOutPort(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort  = me->inPortParams[unInPortID];
	uint32_t                       strMask = pCurrentInPort->strMask;
	uint32_t                  unOutPortID, unInPortID_temp;
	uint32_t     currInPortsTopPriorityMask;
	MatrixOutPortInfoType   *pCurrentOutPort;
	MatrixInPortInfoType    *pCurrentInPort_temp;

	while (strMask)
	{
		unOutPortID = Q6_R_ct0_R(strMask);
		strMask ^= (1 << unOutPortID);
		pCurrentOutPort = me->outPortParams[unOutPortID];
		currInPortsTopPriorityMask = pCurrentOutPort->inPortsTopPriorityMask;

		// If the current top priority i/p port matches,
		if((currInPortsTopPriorityMask) & (1 << unInPortID))
		{
			bool_t bNewTopPrioIpPortFound = FALSE;
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu, top prio i/p port %lu is closing! Finding new top prio port...",
					me->mtMxID, unOutPortID, unInPortID);
			//This o/p port's inPortsTopPriorityMask needs to be updated
			pCurrentOutPort->inPortsTopPriorityMask &= ~(1 << unInPortID);

			//Find the first live i/p port connected to this o/p port
			for (unInPortID_temp = 0; unInPortID_temp <= me->maxInPortID; unInPortID_temp++)
			{
				pCurrentInPort_temp = me->inPortParams[unInPortID_temp];
				if ((NULL != pCurrentInPort_temp) && (((pCurrentInPort_temp->strMask) & (1 << unOutPortID)) &&
						(TRUE == pCurrentInPort_temp->bIsPortLive) &&
						(unInPortID_temp != unInPortID)))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu, live top prio i/p port %lu found!",
							me->mtMxID, unOutPortID, unInPortID_temp);
					pCurrentOutPort->inPortsTopPriorityMask |= (1 << unInPortID_temp);
					pCurrentOutPort->bIsTopPriorityInputPortLive = TRUE;
					bNewTopPrioIpPortFound = TRUE;
				}
			}

			if(FALSE == bNewTopPrioIpPortFound)
			{
				//If no live ports are available, find the 1st i/p port connected to this o/p port
				for (unInPortID_temp = 0; unInPortID_temp <= me->maxInPortID; unInPortID_temp++)
				{
					pCurrentInPort_temp = me->inPortParams[unInPortID_temp];
					if  ( (NULL != pCurrentInPort_temp)
							&& (((pCurrentInPort_temp->strMask) & (1 << unOutPortID)) && (unInPortID_temp != unInPortID)) )
					{
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu, non-live top prio i/p port %lu found!",
								me->mtMxID, unOutPortID, unInPortID_temp);
						pCurrentOutPort->inPortsTopPriorityMask |= (1 << unInPortID_temp);
						pCurrentOutPort->bIsTopPriorityInputPortLive = FALSE;
						bNewTopPrioIpPortFound = TRUE;
					}
				}
			}

			if(FALSE == bNewTopPrioIpPortFound)
			{
				//If the code execution reaches here, it means that after the closing of the current top prio i/p port,
				//No more i/p ports were connected to this o/p port. Therefore resetting the state.
				pCurrentOutPort->bIsTopPriorityInputPortLive = FALSE;
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu, top prio i/p port %lu closed! No other i/p ports connected!",
						me->mtMxID, unOutPortID, unInPortID);
			}
		}
	}
}

void MtMx_ClosingInPortToUpdateMtMxMasksOfOutPort(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort  = me->inPortParams[unInPortID];
	uint32_t                       strMask = pCurrentInPort->strMask;
	uint32_t                  unOutPortID;
	MatrixOutPortInfoType   *pCurrentOutPort;

	while (strMask)
	{
		unOutPortID = Q6_R_ct0_R(strMask);
		strMask ^= (1 << unOutPortID);
		pCurrentOutPort = me->outPortParams[unOutPortID];

		pCurrentOutPort->accInPortsMask &= ~(1 << unInPortID);
		pCurrentOutPort->accBufAvailabilityMask &= ~(1 << unInPortID);
		pCurrentOutPort->inPortsMask &= ~(1 << unInPortID);
		pCurrentOutPort->inPortsWaitingToAccMask &= ~(1 << unInPortID);
	}

	me->steadyStateInPortsMask &= ~(1 << unInPortID);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, i/p port %lu MtMx_ClInPtToUpdMtMxMasksOfOutPort. SteadyStateInPortsMask = %lu",
			me->mtMxID, unInPortID, me->steadyStateInPortsMask);
}

void MxAt_UpdateBurstModeOfMxAt(This_t *me)
{
	uint32_t               steadyStateInPortsMask, unInPortID_temp;
	steadyStateInPortsMask = me->steadyStateInPortsMask;
	MatrixInPortInfoType    *pCurrentInPort;

	me->bIsMxAtOperatingInBurstMode = FALSE;
	while (steadyStateInPortsMask)
	{
		unInPortID_temp = Q6_R_ct0_R(steadyStateInPortsMask);
		steadyStateInPortsMask ^= (1 << unInPortID_temp);
		pCurrentInPort = me->inPortParams[unInPortID_temp];
		if(pCurrentInPort->bIsInPortBurstMode)
		{
			me->bIsMxAtOperatingInBurstMode = TRUE;
		}
		else
		{
			me->bIsMxAtOperatingInBurstMode = FALSE;
			return;
		}
	}
}

ADSPResult MtMx_LogPcmData(This_t *me, uint32_t unInPortID, int8_t *pCurrentSample)
{
	ADSPResult 					result = ADSP_EOK;
	MatrixInPortInfoType    *pCurrentInPort;
	elite_log_info          log_info_obj;
	uint32_t                buf_size;
	uint8_t                 bits_per_sample;
	pcm_data_info           *pcm_data = &(log_info_obj.data_info.pcm_data_fmt);

	pCurrentInPort = me->inPortParams[unInPortID];

	buf_size = (pCurrentInPort->nNumRemainingSamplesPerCh) *
			(pCurrentInPort->unNumChannels * pCurrentInPort->unBytesPerSample);

	bits_per_sample = (pCurrentInPort->unBytesPerSample << 3);

	//Populate the packet to be sent to logging utility
	log_info_obj.qxdm_log_code = me->qxdm_log_code;
	log_info_obj.buf_ptr = (uint8_t *)pCurrentSample;
	log_info_obj.buf_size = buf_size;
	log_info_obj.session_id = pCurrentInPort->unDataLogId;
	log_info_obj.log_tap_id = AUDIOLOG_MTMX_IN_TAP_ID;
	log_info_obj.log_time_stamp = qurt_elite_timer_get_time();
	log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_PCM;

	pcm_data->num_channels = pCurrentInPort->unNumChannels;
	pcm_data->sampling_rate = pCurrentInPort->unSampleRate;
	pcm_data->bits_per_sample = bits_per_sample;
	pcm_data->interleaved = pCurrentInPort->nInterleaveFlag;
	pcm_data->channel_mapping = pCurrentInPort->unChannelMapping;

	//Allocate the log buffer and log the packet. If log code is disabled, log buffer allocation returns NULL
	result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

	return result;
}

static void MtMx_ApplyGainAndAccumulateInputToOutput(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf)
{
    MatrixInPortInfoType         *pCurrentInPort = me->inPortParams[unInPortID];
    MatrixOutPortInfoType        *pCurrentOutPort = me->outPortParams[unOutPortID];
    mt_mx_struct_ramp_gain_t     *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
    mt_mx_struct_channel_mixer_t *pCurrentInputOutputChMixer = &(pCurrentInPort->structChanMixer[unOutPortID]);

    if((MT_MX_NUM_CHANNELS_EIGHT < pCurrentInPort->unNumChannels) ||
                    (0 == pCurrentInPort->unNumChannels) ||
                    (MT_MX_NUM_CHANNELS_EIGHT < pCurrentOutPort->unNumChannels) ||
                    (0 == pCurrentOutPort->unNumChannels))
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p [%lu] o/p [%lu] channel cfg unsupported",
				me->mtMxID, pCurrentInPort->unNumChannels, pCurrentOutPort->unNumChannels);
		return;
	}

	/* Check if channel mixer library is being used for this i/p - o/p pair */
	if (TRUE == pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated)
	{
		/** At this point, the channel library has successfully converted the i/p to the
		    desired o/p channel type. All that remains, is the bit width conversion
		    along with gain and accumulation. This can be treated like "EqualNumChannels"
		    (with a small twist) because i/p -> intermediate -> o/p and
		    intermediate and o/p are considered as equal!
		*/
		if (TRUE == pCurrentInputOutputPath->bShouldRampGainBeApplied)
		{
			MtMx_ApplyRampGainAndAccOnEqualNumChannels(me, unInPortID, unOutPortID, pAccBuf);
		}
		else
		{
			MtMx_ApplyImmGainAndAccOnEqualNumChannels(me, unInPortID, unOutPortID, pAccBuf);
		}

		return;
	}

	/* Channel mixer is not used */

	/** Non-channel mixer use case
	    1) Mono -> Mono or Stereo -> Stereo with 1-to-1 ch mapping
	    2) Mono -> Stereo
	    3) Stereo -> Mono
	    4) M channels -> N channels
	 */
	if((pCurrentOutPort->unNumChannels == pCurrentInPort->unNumChannels) &&
	             ((NUM_CHANNELS_MONO == pCurrentInPort->unNumChannels) ||
	                    (NUM_CHANNELS_STEREO == pCurrentInPort->unNumChannels &&
	                          pCurrentInPort->unChannelMapping[0] == pCurrentOutPort->unChannelMapping[0] &&
	                          pCurrentInPort->unChannelMapping[1] == pCurrentOutPort->unChannelMapping[1])))
	{
		if (TRUE == pCurrentInputOutputPath->bShouldRampGainBeApplied)
		{
			MtMx_ApplyRampGainAndAccOnEqualNumChannels(me, unInPortID, unOutPortID, pAccBuf);
		}
		else
		{
			MtMx_ApplyImmGainAndAccOnEqualNumChannels(me, unInPortID, unOutPortID, pAccBuf);
		}
	}
	else if	((NUM_CHANNELS_STEREO == pCurrentOutPort->unNumChannels) &&
	                (NUM_CHANNELS_MONO == pCurrentInPort->unNumChannels))
	{
		if (TRUE == pCurrentInputOutputPath->bShouldRampGainBeApplied)
		{
			MtMx_ApplyRampGainAndAccOnStereoOutMonoIn(me, unInPortID, unOutPortID, pAccBuf);
		}
		else
		{
			MtMx_ApplyImmGainAndAccOnStereoOutMonoIn(me, unInPortID, unOutPortID, pAccBuf);
		}
	}
	else if ((NUM_CHANNELS_MONO == pCurrentOutPort->unNumChannels) &&
	                (NUM_CHANNELS_STEREO == pCurrentInPort->unNumChannels))
	{
		if (TRUE == pCurrentInputOutputPath->bShouldRampGainBeApplied)
		{
			MtMx_ApplyRampGainAndAccOnMonoOutStereoIn(me, unInPortID, unOutPortID, pAccBuf);
		}
		else
		{
			MtMx_ApplyImmGainAndAccOnMonoOutStereoIn(me, unInPortID, unOutPortID, pAccBuf);
		}
	}
	else
	{
		if (TRUE == pCurrentInputOutputPath->bShouldRampGainBeApplied)
		{
			MtMx_ApplyRampGainAndAccOnMultiOutMultiIn(me, unInPortID, unOutPortID, pAccBuf);
		}
		else
		{
			MtMx_ApplyImmGainAndAccOnMultiOutMultiIn(me, unInPortID, unOutPortID, pAccBuf);
		}
	}
}

static void MtMx_ApplyRampGainAndAccOnMultiOutMultiIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf)
{
    MatrixInPortInfoType     *pCurrentInPort;
    MatrixOutPortInfoType    *pCurrentOutPort;
    mt_mx_struct_ramp_gain_t *pCurrentInputOutputPath;
    mt_mx_struct_pspd_t      *pCurrentPspd;
    int16_t                  *pGainIncrementStep;
    uint16_t                 *pCurrentGain, *pTargetGain;
    uint32_t                 j, k, num_rem_steps;

    /* Gather i/p port, o/p port params, gain etc. */
    pCurrentInPort           = me->inPortParams[unInPortID];
    pCurrentOutPort          = me->outPortParams[unOutPortID];
    pCurrentInputOutputPath  = &(pCurrentInPort->structRampGain[unOutPortID]);
    pCurrentPspd             = &(pCurrentInPort->structPspd[unOutPortID]);

    uint16_t                 unNumSamplesGainAndAccPerCh = pCurrentInPort->unAfeFrameSizeInSamples;
    int8_t                   *pInputBuf = pCurrentPspd->pPspdOutBuf;
    int64_t                  *pOutputBuf = pAccBuf;
    int32_t                  *pOutputBuf32 = (int32_t*) pAccBuf;
    uint16_t                 num_bytes_per_ch_gain_and_acc = (unNumSamplesGainAndAccPerCh * pCurrentInPort->unBytesPerSample);

	/* a. Apply ramp gain to minimum of remaining and number of afe frames steps, in 1 afe frame step size */
	num_rem_steps = s32_min_s32_s32(pCurrentInputOutputPath->unNumRemSteps, pCurrentOutPort->unNumAfeFrames);

	for (k = 0; k < num_rem_steps; k++)
	{
		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
		pGainIncrementStep = &(pCurrentInputOutputPath->nGainIncDecStep[0]);

		/* Mix in unNumSamplesGainAndAccPerCh = 1 * afe frame size in samples for all o/p channels */
		MtMx_GainAndAccOnMultiOutMultiIn(unNumSamplesGainAndAccPerCh,
				pCurrentInPort,
				pCurrentOutPort,
				pInputBuf, pOutputBuf,
				pCurrentGain);

		for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
		{
			*pCurrentGain++ += *pGainIncrementStep++;
		}

		/* Increment the input and output buffer pointers to next channel
		 */
		pInputBuf += num_bytes_per_ch_gain_and_acc;
        if(32 == pCurrentOutPort->unBitwidth)
        {
            pOutputBuf += unNumSamplesGainAndAccPerCh;
        }
        else
        {
            pOutputBuf32 += unNumSamplesGainAndAccPerCh;
            pOutputBuf = (int64_t*)pOutputBuf32;
        }
	}

	if(pCurrentInputOutputPath->unNumRemSteps >= pCurrentOutPort->unNumAfeFrames)
	{
		/* Check if number of remaining steps is now equal to zero.
         If it is, check if current gain on each of the channels
         is equal to target gain, if not, make it equal.
		 */
		pCurrentInputOutputPath->unNumRemSteps -= pCurrentOutPort->unNumAfeFrames;
		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

		if (0 == pCurrentInputOutputPath->unNumRemSteps)
		{
			pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE;
			pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);

			for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
			{
				*pCurrentGain++ = *pTargetGain++;
			}
		}
	}
	else /* Remaining steps less than the Frame size */
	{
		uint16 unNumStepsAfterRampGain = pCurrentOutPort->unNumAfeFrames - pCurrentInputOutputPath->unNumRemSteps;

		pCurrentInputOutputPath->unNumRemSteps  = 0;
		pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE;

		/* b. After applying ramp gain, equate current gain to target gain */
		pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
		pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);

		for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
		{
			*pCurrentGain++ = *pTargetGain++;
		}

		/* c. Apply gain immediately to the remaining samples in the local buffer */
		for (k = 0; k < unNumStepsAfterRampGain; k++)
		{
			pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);

			/* Mix in unNumSamplesGainAndAccPerCh = 1 * afe frame size in samples for all o/p channels */
			MtMx_GainAndAccOnMultiOutMultiIn(unNumSamplesGainAndAccPerCh,
					pCurrentInPort,
					pCurrentOutPort,
					pInputBuf, pOutputBuf,
					pCurrentGain);

			/* Increment the input and output buffer pointers to next channel
			 */
			pInputBuf += num_bytes_per_ch_gain_and_acc;
	        if(32 == pCurrentOutPort->unBitwidth)
	        {
	            pOutputBuf += unNumSamplesGainAndAccPerCh;
	        }
	        else
	        {
	            pOutputBuf32 += unNumSamplesGainAndAccPerCh;
	            pOutputBuf = (int64_t*)pOutputBuf32;
	        }
		}
	}
}

static void MtMx_ApplyImmGainAndAccOnMultiOutMultiIn(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, int64_t *pAccBuf)
{
    MatrixInPortInfoType     *pCurrentInPort;
    MatrixOutPortInfoType    *pCurrentOutPort;
    mt_mx_struct_ramp_gain_t *pCurrentInputOutputPath;
    mt_mx_struct_pspd_t      *pCurrentPspd;
    uint16_t                 *pCurrentGain;
    int8_t                   *pInputBuf;
    int64_t                  *pOutputBuf;
    int32_t                  *pOutputBuf32;

    /* Gather i/p port, o/p port params, gain etc., */
    pCurrentInPort          = me->inPortParams[unInPortID];
    pCurrentOutPort         = me->outPortParams[unOutPortID];
    pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
    pCurrentPspd            = &(pCurrentInPort->structPspd[unOutPortID]);
    pCurrentGain            = &(pCurrentInputOutputPath->unCurrentGain[0]);

    pInputBuf               = pCurrentPspd->pPspdOutBuf;
    pOutputBuf              = pAccBuf;
    pOutputBuf32            = (int32_t*) pAccBuf;

	MtMx_GainAndAccOnMultiOutMultiIn(pCurrentInPort->unInPortPerChBufSize,
			pCurrentInPort,
			pCurrentOutPort,
			pInputBuf, pOutputBuf,
			pCurrentGain);
}

static void MtMx_GainAndAccOnMultiOutMultiIn(uint16_t unNumSamplesGainAndAccPerCh,
                MatrixInPortInfoType *pCurrentInPort,
                MatrixOutPortInfoType *pCurrentOutPort,
                int8_t *pInputBuf, int64_t *pOutputBuf,
                uint16_t *pCurrentGain)
{
    int16_t     *ptr_local_buf_16, *ptr_local_buf_start_16;
    int32_t     post_gain_sample_32, *ptr_local_buf_32, *ptr_local_buf_start_32;
    int64_t     post_gain_sample_64, *ptr_local_buf_64;
    int32_t     *ptr_out_buf_32;
    int64_t     *ptr_out_buf_64;
    uint32_t    i, j, samp;
    uint8_t     unTempChMap;
    uint32_t    unTempChNum;
    int32_t     curr_gain_q16, curr_gain_q28;
    int64_t     combined_gain, combined_input, temp_res_64;

    bit_conv_mode_t bit_conv_type = MtMx_IdentifyBitConvType(pCurrentInPort->unBitwidth, pCurrentOutPort->unBitwidth);

    /* If the number of bytes per sample for input port and output port are equal */
    if(pCurrentInPort->unBytesPerSample == pCurrentOutPort->unBytesPerSample)
    {
        /* If 2 bytes per sample (Q15 - Q15) */
        if(MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentInPort->unBytesPerSample)
        {
            ptr_local_buf_start_16 = (int16_t *) pInputBuf;

            /* Process this loop for all the o/p channels */
            for(i = 0; i < pCurrentOutPort->unNumChannels; i++)
            {
                /* Scale the gain up from Q13 -> Q16 */
                curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 3;

                /* Combine two int32_t's into one int64_t */
                combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                /* Identify the o/p channel mapping for the current channel */
                unTempChMap = pCurrentOutPort->unChannelMapping[i];

                /* Process only if a non-zero o/p channel map is found */
                if(0 != unTempChMap)
                {
                    /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                    unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                    /* Process only if a valid i/p channel number is obtained */
                    if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                    {
                        /* Advance the i/p pointer to the appropriate channel for accumulation */
                        ptr_local_buf_16 = (ptr_local_buf_start_16 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                        /* Advance the o/p pointer to the appropriate channel for accumulation */
                        ptr_out_buf_32 = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                        samp = unNumSamplesGainAndAccPerCh;

                        /** If the output pointer is not 8-byte aligned, process one
                         *  sample to align the pointer.
                         */
                        if(((uint32_t) ptr_out_buf_32) & 7)
                        {
                            /* Apply the gain on sample and shift for gain in Q13 */
                            post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);
                            post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + MT_MX_RND_FACTOR_GAIN_SHIFT),MT_MX_GAIN_SHIFT_FACT));

                            /* Accumulate the gain applied 32-bit sample */
                            *ptr_out_buf_32 = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                            ptr_out_buf_32++;

                            /* Reduce sample count by 1 */
                            samp = samp - 1;
                        }

                        ptr_out_buf_64 = (int64_t *) (ptr_out_buf_32);

                        /* Process two input samples per loop iteration */
                        for(; samp >= 2; samp -= 2)
                        {
                            /* Combine two, 16 to 32-bit sign extended hwords in one 64-bit dword */
                            combined_input = Q6_P_combine_RR(ptr_local_buf_16[1], ptr_local_buf_16[0]);
                            ptr_local_buf_16 += 2;

                            /* Apply the gain */
                            temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(combined_gain, combined_input);

                            /* Accumulate the gain applied samples in output port acc buffer */
                            *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                            ptr_out_buf_64++;
                        }

                        /* If the number of samples are odd  */
                        if(samp)
                        {
                            ptr_out_buf_32 = (int32_t *) ptr_out_buf_64;

                            /* Apply the gain on sample and shift for gain in Q13 */
                            post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);
                            post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + MT_MX_RND_FACTOR_GAIN_SHIFT),MT_MX_GAIN_SHIFT_FACT));

                            /* Accumulate the gain applied 32-bit sample */
                            *ptr_out_buf_32 = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                            ptr_out_buf_32++;
                        }
                    } /* valid input channel */
                } /*Non zero output channel */

                /* Increment the pointer to fetch the next channel's gain value */
                pCurrentGain++;
            } /* unNumChannels for loop */
        }/* Q15 - Q15 */
        else /* 4 bytes per channel */
        {
            /** The number of bytes per sample determines the processing bit
             *  width. But, the actual bit widths can be different. Hence
             *  account for potential up/down conversion combinations with
             *  24 or 32 as input & output port bit widths
             */

            int32_t local_shift_factor           = MT_MX_GAIN_SHIFT_FACT; /* Default shift factor for gain mpy in Q13 */
            int32_t local_rnd_factor             = 0;
            int32_t local_shift_factor_unit_gain = 0;
            int32_t local_rnd_factor_unit_gain   = 0;

            ptr_local_buf_start_32 = (int32_t *) pInputBuf;

            /* Identify shift/rounding factors based on the bit conv type */
            if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type)
            {
                local_shift_factor           -= (BYTE_INTER_CONV_SHIFT_FACT); /* Right shift from Q31 -> Q27 */
                local_shift_factor_unit_gain -= (BYTE_INTER_CONV_SHIFT_FACT);
            }
            else if(BIT_CONV_IN24_TO_OUT32 == bit_conv_type)
            {
                local_shift_factor           += (BYTE_INTER_CONV_SHIFT_FACT); /* Left shift from Q27 -> Q31 */
                local_shift_factor_unit_gain += (BYTE_INTER_CONV_SHIFT_FACT);
            }
            else
            {
                local_shift_factor           += 0; /* Retain Q format */
                local_shift_factor_unit_gain += 0;
            }

            local_rnd_factor           = MtMx_CalculateRoundingFactor(local_shift_factor);
            local_rnd_factor_unit_gain = MtMx_CalculateRoundingFactor(local_shift_factor_unit_gain);

            if(MT_MX_BIT_WIDTH_24 == pCurrentOutPort->unBitwidth)
            {
                /* Process this loop for all the o/p channels */
                for(i = 0; i < pCurrentOutPort->unNumChannels; i++)
                {
                    if(*pCurrentGain < MT_MX_UNITY_GAIN)
                    {
                        /* Scale the gain up from Q13 -> Q15 */
                        curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 2;

                        /* Combine int32_t into int64_t */
                        combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                        /* Identify the o/p channel mapping for the current channel */
                        unTempChMap = pCurrentOutPort->unChannelMapping[i];

                        /* Process only if a non-zero o/p channel map is found */
                        if(0 != unTempChMap)
                        {
                            /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                            unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                            /* Process only if a valid i/p channel number is obtained */
                            if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                            {
                                /* Advance the i/p pointer to the appropriate channel for accumulation */
                                ptr_local_buf_32 = (ptr_local_buf_start_32 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                                /* Advance the o/p pointer to the appropriate channel for accumulation */
                                ptr_out_buf_32 = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                                samp = unNumSamplesGainAndAccPerCh;

                               /** If the output pointer is not 8-byte aligned, process one
                                 * sample to align the pointer.
                                 */
                                if(((uint32_t) ptr_out_buf_32) & 7)
                                {
                                    /* Apply the gain on sample */
                                    post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                                    /* Shift to Q27 Format and saturate to 32 bits*/
                                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                                    /* Accumulate the gain applied 32-bit sample */
                                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                    ptr_out_buf_32++;

                                    /* Reduce sample count by 1 */
                                    samp = samp - 1;
                                }

                                ptr_local_buf_64 = (int64_t*) ptr_local_buf_32;
                                ptr_out_buf_64   = (int64_t*) ptr_out_buf_32;

                                if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type) /* Q31 -> Q27 */
                                {
                                    /* Process two input samples per loop iteration */
                                    for (; samp >= 2; samp -= 2)
                                    {
                                        /* Apply the gain */
                                        temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);

                                        /* Add the rounding factor before truncation to Q27 */
                                        temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q27);

                                        /* Right shift from Q31 to Q27 */
                                        temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_INTER_CONV_SHIFT_FACT);

                                        /* Accumulate the gain applied samples */
                                        *ptr_out_buf_64= Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                        ptr_out_buf_64++;
                                    }
                                }
                                else /* Retain Q27 Format */
                                {
                                    for (; samp>=2; samp-=2)
                                    {
                                        /* Apply the gain */
                                        temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);
                                        
                                        /* Accumulate the gain applied samples */
                                        *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                        ptr_out_buf_64++;
                                    }
                                }

                                /* If the number of samples are odd  */
                                if(samp)
                                {
                                    ptr_local_buf_32 = (int32_t *) ptr_local_buf_64;
                                    ptr_out_buf_32   = (int32_t *) ptr_out_buf_64;

                                    /* Apply the gain on sample */
                                    post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                                    /* Shift to Q27 Format and saturate to 32 bits */
                                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                                    /* Accumulate the gain applied 32-bit sample */
                                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                    ptr_out_buf_32++;
                                }
                            } /* valid i/p channel map */
                        } /* if non-zero o/p channel map */
                    } /* if gain < unity gain for output in Q27 AND in bytes = out bytes */
                    else if(MT_MX_UNITY_GAIN == *pCurrentGain)
                    {
                        /** This configuration is the most commonly used scenario for
                         *  audio use cases. Since the gain = unity, optimize by
                         *  skipping multiplication and taking advantage of vectorized
                         *  operations
                         */

                        /* Identify the o/p channel mapping for the current channel */
                        unTempChMap = pCurrentOutPort->unChannelMapping[i];

                        /* Process only if a non-zero o/p channel map is found */
                        if(0 != unTempChMap)
                        {
                            /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                            unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                            /* Process only if a valid i/p channel number is obtained */
                            if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                            {
                                /* Advance the i/p pointer to the appropriate channel for accumulation */
                                ptr_local_buf_32 = (ptr_local_buf_start_32 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                                /* Advance the o/p pointer to the appropriate channel for accumulation */
                                ptr_out_buf_32 = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                                samp = unNumSamplesGainAndAccPerCh;

                               /** If the output pointer is not 8-byte aligned, process one
                                 * sample to align the pointer.
                                 */
                                if(((uint32_t) ptr_out_buf_32) & 7)
                                {
                                    /* Shift to Q27 Format and saturate to 32 bits*/
                                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                                    /* Accumulate the 32-bit sample */
                                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                    ptr_out_buf_32++;

                                    /* Reduce sample count by 1 */
                                    samp = samp - 1;
                                }

                                ptr_local_buf_64 = (int64_t*) ptr_local_buf_32;
                                ptr_out_buf_64   = (int64_t*) ptr_out_buf_32;

                                if(BIT_CONV_IN32_TO_OUT24 == bit_conv_type) /* Q31 -> Q27 */
                                {
                                    /* Process two input samples per loop iteration */
                                    for (; samp >= 2; samp -= 2)
                                    {
                                        /* Add the rounding factor before truncation to Q27 */
                                        temp_res_64 = Q6_P_vaddw_PP_sat(*ptr_local_buf_64++, MT_MX_RND_FACTOR_V_Q31_TO_Q27);

                                        /* Right shift from Q31 to Q27 */
                                        temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_INTER_CONV_SHIFT_FACT);

                                        /* Accumulate the samples */
                                        *ptr_out_buf_64= Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                        ptr_out_buf_64++;
                                    }
                                }
                                else  /* Retain Q27 Format */
                                {
                                    for (; samp>=2; samp-=2)
                                    {
                                        *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, *ptr_local_buf_64++);
                                        ptr_out_buf_64++;
                                    }
                                }

                                /* If the number of samples are odd  */
                                if(samp)
                                {
                                    ptr_local_buf_32 = (int32_t *) ptr_local_buf_64;
                                    ptr_out_buf_32   = (int32_t *) ptr_out_buf_64;

                                    /* Shift to Q27 Format and saturate to 32 bits */
                                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                                    /* Accumulate the 32-bit sample */
                                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                    ptr_out_buf_32++;
                                }
                            } /* valid i/p channel map */
                        } /* if non-zero o/p channel map */
                    }/* gain = unity gain for output in Q27 AND in bytes = out bytes */
                    else /* gain > unity gain */
                    {
                        /* Identify the o/p channel mapping for the current channel */
                        unTempChMap = pCurrentOutPort->unChannelMapping[i];

                        /* Process only if a non-zero o/p channel map is found */
                        if(0 != unTempChMap)
                        {
                            /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                            unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                            /* Process only if a valid i/p channel number is obtained */
                            if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                            {
                                /* Advance the i/p pointer to the appropriate channel for accumulation */
                                ptr_local_buf_32 = (ptr_local_buf_start_32 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                                /* Advance the o/p pointer to the appropriate channel for accumulation */
                                ptr_out_buf_32 = ((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize);

                                for(j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                                {
                                    /* Apply the gain on sample */
                                    post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                                    /* Shift to Q27 Format and saturate to 32 bits*/
                                    post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                                    /* Accumulate the gain applied 32-bit sample */
                                    (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                    ptr_out_buf_32++;
                                }
                            } /* valid i/p channel map */
                        } /* Non-zero o/p channel map */
                    } /* gain > unity for output in Q27 AND in bytes = out bytes */

                    /* Increment the pointer to fetch the next channels's gain value */
                    pCurrentGain++;
                } /* unNumChannels for loop */
            } /* output port BW == 24 */
            else /* output in Q31 for in bytes = out bytes */
            {
                /* Process this loop for all the o/p channels */
                for(i = 0; i < pCurrentOutPort->unNumChannels; i++)
                {
                    /* Identify the o/p channel mapping for the current channel */
                    unTempChMap = pCurrentOutPort->unChannelMapping[i];

                    /* Process only if a non-zero o/p channel map is found */
                    if(0 != unTempChMap)
                    {
                        /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                        unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                        /* Process only if a valid i/p channel number is obtained */
                        if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                        {
                            /* Advance the i/p pointer to the appropriate channel for accumulation */
                            ptr_local_buf_32 = (ptr_local_buf_start_32 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                            /* Advance the o/p pointer to the appropriate channel for accumulation */
                            ptr_out_buf_64 = (pOutputBuf + (i * pCurrentOutPort->unOutPortPerChBufSize));

                            /* Use scalar operations for accumulation to 64-bit buffer */
                            for(j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                            {
                                /* Apply the gain on sample */
                                post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                                /* Shift to Q31 Format after rounding */
                                post_gain_sample_64 = s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor);

                                /* Accumulate the gain applied 64-bit sample */
                                (*ptr_out_buf_64) = Q6_P_add_PP(*ptr_out_buf_64, post_gain_sample_64);
                                ptr_out_buf_64++;
                            }
                        } /* valid i/p channel map */
                    } /* Non-zero o/p channel map */
                    /* Increment the pointer to fetch the next channels's gain value */
                    pCurrentGain++;
                } /* outer for loop */
            } /* output port BW == 32 */
        } /*4 bytes per channel */
    }/* in bytes = out bytes */
    else /* Unequal number of bytes */
    {
        /* If output port bytes per sample is greater than the input port bytes per sample */
        if(pCurrentOutPort->unBytesPerSample > pCurrentInPort->unBytesPerSample)
        {
            /** If output port bytes per sample is greater than the input
             *  port,
             *  1. Apply gain on 16-bit input port samples
             *  2. Up convert to appropriate Q format (Q27/Q31)
             *  3. Accumulate to output port buffer.
             */
            int32_t local_shift_factor = 0;
            int32_t local_rnd_factor   = 0;

            /* Identify shift/rounding factors for Left shift from Q15 -> Qn */
            if(BIT_CONV_IN16_TO_OUT24 == bit_conv_type)
            {
                local_shift_factor = MT_MX_GAIN_SHIFT_FACT + BYTE_UPDOWN_CONV_SHIFT_FACT;
            }
            else /* BIT_CONV_IN16_TO_OUT32 */
            {
                local_shift_factor = MT_MX_GAIN_SHIFT_FACT + BYTE_UPDOWN_CONV_SHIFT_FACT_32;
            }

            local_rnd_factor = MtMx_CalculateRoundingFactor(local_shift_factor);

            ptr_local_buf_start_16 = (int16_t *) pInputBuf;

            if(BIT_CONV_IN16_TO_OUT24 == bit_conv_type) /* Q15 -> Q27 */
            {
                /* Process this loop for all the o/p channels */
                for(i = 0; i < pCurrentOutPort->unNumChannels; i++)
                {
                    /* Identify the o/p channel mapping for the current channel */
                    unTempChMap = pCurrentOutPort->unChannelMapping[i];

                    /* Process only if a non-zero o/p channel map is found */
                    if(0 != unTempChMap)
                    {
                        /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                        unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                        /* Process only if a valid i/p channel number is obtained */
                        if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                        {
                            /* Advance the i/p pointer to the appropriate channel for accumulation */
                            ptr_local_buf_16 = (ptr_local_buf_start_16 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                            /* Advance the o/p pointer to the appropriate channel for accumulation */
                            ptr_out_buf_32 = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                            samp = unNumSamplesGainAndAccPerCh;

                             /** If the output pointer is not 8-byte aligned, process one
                             *   sample to align the pointer.
                             */
                            if(((uint32_t) ptr_out_buf_32) & 7)
                            {
                                /* Apply the gain on sample */
                                post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                                /* Shift to Q27 Format and saturate to 32 bits */
                                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                                /* Accumulate the gain applied 32-bit sample */
                                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                ptr_out_buf_32++;

                                /* Reduce sample count by 1 */
                                samp = samp - 1;
                            }

                            /** This is a special case where the gain is shifted to Q28.
                             *  Doing this avoids consecutive right shift (as part of mpy)
                             *  and left shift (for up conversion).
                             *
                             *  The mpy operation translates to ((Q28 gain * Q15 sample)  >>
                             *  16). This results in a Q27 number directly
                             *
                             *  As a result, achieve higher signal precision in the
                             *  converted sample and avoid an explicit left shift for bit
                             *  conversion.
                             */

                            /* Scale the gain up from Q13 -> Q28 */
                            curr_gain_q28 = (Q6_R_zxth_R(*pCurrentGain)) << 15;

                            /* Combine int32_t into int64_t */
                            combined_gain = Q6_P_combine_RR(curr_gain_q28, curr_gain_q28);

                            /* Process two input samples per loop iteration */
                            ptr_out_buf_64 = (int64_t *) (ptr_out_buf_32);

                            for (; samp >=2; samp -=2)
                            {
                                /* Combine two 16 to 32-bit sign extended hwords in one 64-bit dword */
                                combined_input = Q6_P_combine_RR(ptr_local_buf_16[1], ptr_local_buf_16[0]);
                                ptr_local_buf_16 += 2;

                                /* Apply the gain */
                                temp_res_64 = Q6_P_vmpyweh_PP_rnd_sat(combined_gain, combined_input);
                                        /* Note that result is already in Q27*/

                                /* Accumulate the gain applied samples */
                                *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                ptr_out_buf_64++;
                            }

                            /* If the number of samples are odd */
                            if(samp)
                            {
                                ptr_out_buf_32 = (int32_t *) ptr_out_buf_64;

                                /* Apply the gain on sample */
                                post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                                /* Shift to Q27 Format and saturate to 32 bits */
                                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                                /* Accumulate the gain applied 32-bit sample */
                                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                ptr_out_buf_32++;
                            }
                        } /* valid i/p channel */
                    } /* non-zero o/p channel map */

                    /* Increment the pointer to fetch the next channel's gain value */
                    pCurrentGain++;
                } /* o/p channel for loop*/
            } /* Q15 -> Q27 */
            else
            {
                /* Process this loop for all the o/p channels */
                for(i = 0; i < pCurrentOutPort->unNumChannels; i++)
                {
                    /* Identify the o/p channel mapping for the current channel */
                    unTempChMap = pCurrentOutPort->unChannelMapping[i];

                    /* Process only if a non-zero o/p channel map is found */
                    if(0 != unTempChMap)
                    {
                        /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                        unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                        /* Process only if a valid i/p channel number is obtained */
                        if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                        {
                            /* Advance the i/p pointer to the appropriate channel for accumulation */
                            ptr_local_buf_16 = (ptr_local_buf_start_16 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                            /* Advance the o/p pointer to the appropriate channel for accumulation */
                            ptr_out_buf_64 = (pOutputBuf + (i * pCurrentOutPort->unOutPortPerChBufSize));

                            /* Use scalar operations for accumulation to 64-bit buffer */
                            for (j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                            {
                                /* Apply the gain on sample */
                                post_gain_sample_64 = s64_mult_s16_u16_shift(*ptr_local_buf_16++, *pCurrentGain, 0);

                                /* Shift to Q31 Format after rounding */
                                post_gain_sample_64 = s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor);

                                /* Accumulate the gain applied 64-bit sample */
                                (*ptr_out_buf_64) = Q6_P_add_PP(*ptr_out_buf_64, post_gain_sample_64);
                                ptr_out_buf_64++;
                            }
                        } /* valid i/p channel */
                    } /* non-zero o/p channel map */

                    /* Increment the pointer to fetch the next channel's gain value */
                    pCurrentGain++;
                } /* o/p channel for loop*/
            } /* Q15 -> Q31 */
        }/* in bytes < out bytes */
        else /* pCurrentOutPort->unBytesPerSample < pCurrentInPort->unBytesPerSample */
        {
            /**
             *    1. Gain is applied on 32 bit input port samples
             *    2. 64 bit intermediate result is saturated to 32 bits
             */
            ptr_local_buf_start_32 = (int32_t *) pInputBuf;

            int32_t local_shift_factor           = MT_MX_GAIN_SHIFT_FACT; /* Default shift factor for gain mpy in Q13 */
            int32_t local_rnd_factor             = 0;
            int32_t local_shift_factor_unit_gain = 0;
            int32_t local_rnd_factor_unit_gain   = 0;

            /* Identify the shift/rounding factors for Right shift from Qn -> Q15 */
            if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type)
            {
                local_shift_factor           -= BYTE_UPDOWN_CONV_SHIFT_FACT; /* Right shift from Q27 - Q15 */
                local_shift_factor_unit_gain -= BYTE_UPDOWN_CONV_SHIFT_FACT;
            }
            else
            {
                local_shift_factor           -= BYTE_UPDOWN_CONV_SHIFT_FACT_32; /* Right shift from Q31 - Q15 */
                local_shift_factor_unit_gain -= BYTE_UPDOWN_CONV_SHIFT_FACT_32;
            }

            local_rnd_factor           = MtMx_CalculateRoundingFactor(local_shift_factor);
            local_rnd_factor_unit_gain = MtMx_CalculateRoundingFactor(local_shift_factor_unit_gain);

            /* Process this loop for all the o/p channels */
            for(i = 0; i < pCurrentOutPort->unNumChannels; i++)
            {
                if(*pCurrentGain < MT_MX_UNITY_GAIN)
                {
                    /* Scale the gain up from Q13 -> Q15 */
                    curr_gain_q16 = (Q6_R_zxth_R(*pCurrentGain)) << 2;

                    /* Combine int32_t into int64_t */
                    combined_gain = Q6_P_combine_RR(curr_gain_q16, curr_gain_q16);

                    /* Identify the o/p channel mapping for the current channel */
                    unTempChMap = pCurrentOutPort->unChannelMapping[i];

                    /* Process only if a non-zero o/p channel map is found */
                    if(0 != unTempChMap)
                    {
                        /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                        unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                        /* Process only if a valid i/p channel number is obtained */
                        if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                        {
                            /* Advance the i/p pointer to the appropriate channel for accumulation */
                            ptr_local_buf_32 = (ptr_local_buf_start_32 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                            /* Advance the o/p pointer to the appropriate channel for accumulation */
                            ptr_out_buf_32 = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                            samp = unNumSamplesGainAndAccPerCh;

                            /** If the output pointer is not 8-byte aligned, process one
                             *  sample to align the pointer.
                             */
                            if(((uint32_t) ptr_out_buf_32) & 7)
                            {
                                /* Apply the gain on sample */
                                post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                                /* Right Shift to Q15 format */
                                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                                /* Accumulate the gain applied 32-bit sample */
                                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                ptr_out_buf_32++;

                                /* Reduce sample count by 1 */
                                samp = samp - 1;
                            }

                            ptr_local_buf_64 = (int64_t *) (ptr_local_buf_32);
                            ptr_out_buf_64   = (int64_t *) (ptr_out_buf_32);

                            if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type) /* Q27 -> Q15 */
                            {
                                /* Process two input samples per loop iteration */
                                for(; samp >=2; samp-=2)
                                {
                                    /* Apply the gain */
                                    temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);

                                    /* Add the rounding factor before shift to Q15 */
                                    temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q27_TO_Q15);

                                    /* Right shift from Q27 to Q15 */
                                    temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT);

                                    /* Accumulate the gain applied samples */
                                    *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                    ptr_out_buf_64++;
                                }
                            }
                            else /* Q31 -> Q15 */
                            {
                                /* Process two input samples per loop iteration */
                                for(; samp >=2; samp-=2)
                                {
                                    /* Apply the gain */
                                    temp_res_64 = Q6_P_vmpyweh_PP_s1_rnd_sat(*ptr_local_buf_64++, combined_gain);

                                    /* Add the rounding factor before shift to Q15 */
                                    temp_res_64 = Q6_P_vaddw_PP_sat(temp_res_64, MT_MX_RND_FACTOR_V_Q31_TO_Q15);

                                    /* Right shift from Q31 to Q15 */
                                    temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT_32);

                                    /* Accumulate the gain applied samples */
                                    *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                    ptr_out_buf_64++;
                                }
                            }
                            /* If the number of samples are odd */
                            if(samp)
                            {
                                ptr_out_buf_32   = (int32_t *) ptr_out_buf_64;
                                ptr_local_buf_32 = (int32_t *) ptr_local_buf_64;

                                /* Apply the gain on sample */
                                post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                                /* Right Shift to Q15 format */
                                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                                /* Accumulate the gain applied 32-bit sample */
                                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                ptr_out_buf_32++;
                            }
                        } /* valid i/p channel */
                    } /* non-zero o/p channel map */
                } /* gain < unity for output in Q15 AND in bytes > out bytes */
                else if(MT_MX_UNITY_GAIN == *pCurrentGain)
                {
                    /** This configuration is the most commonly used scenario for
                     *  audio use cases. Since the gain = unity, optimize by
                     *  skipping multiplication and taking advantage of vectorized
                     *  operations
                     */

                    /* Identify the o/p channel mapping for the current channel */
                    unTempChMap = pCurrentOutPort->unChannelMapping[i];

                    /* Process only if a non-zero o/p channel map is found */
                    if(0 != unTempChMap)
                    {
                        /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                        unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                        /* Process only if a valid i/p channel number is obtained */
                        if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                        {
                            /* Advance the i/p pointer to the appropriate channel for accumulation */
                            ptr_local_buf_32 = (ptr_local_buf_start_32 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                            /* Advance the o/p pointer to the appropriate channel for accumulation */
                            ptr_out_buf_32 = (((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize));

                            samp = unNumSamplesGainAndAccPerCh;

                            /** If the output pointer is not 8-byte aligned, process one
                             *  sample to align the pointer.
                             */
                            if(((uint32_t) ptr_out_buf_32) & 7)
                            {
                                /* Right Shift to Q15 format */
                                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                                /* Accumulate the 32-bit sample */
                                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                ptr_out_buf_32++;

                                /* Reduce sample count by 1 */
                                samp = samp - 1;
                            }

                            /* Process two input samples per loop iteration */
                            ptr_local_buf_64 = (int64_t *) (ptr_local_buf_32);
                            ptr_out_buf_64   = (int64_t *) (ptr_out_buf_32);

                            if(BIT_CONV_IN24_TO_OUT16 == bit_conv_type) /* Q27 -> Q15 */
                            {
                                for(; samp >=2; samp-=2)
                                {
                                    /* Add the rounding factor before shift to Q15 */
                                    temp_res_64 = Q6_P_vaddw_PP_sat(*ptr_local_buf_64++, MT_MX_RND_FACTOR_V_Q27_TO_Q15);

                                    /* Right shift from Q27 to Q15 */
                                    temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT);

                                    /* Accumulate the samples */
                                    *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                    ptr_out_buf_64++;
                                }
                            }
                            else /* Q31 -> Q15 */
                            {
                                for(; samp >=2; samp-=2)
                                {
                                    /* Add the rounding factor before shift to Q15 */
                                    temp_res_64 = Q6_P_vaddw_PP_sat(*ptr_local_buf_64++, MT_MX_RND_FACTOR_V_Q31_TO_Q15);

                                    /* Right shift from Q31 to Q15 */
                                    temp_res_64 = Q6_P_vasrw_PI(temp_res_64, BYTE_UPDOWN_CONV_SHIFT_FACT_32);

                                    /* Accumulate the samples */
                                    *ptr_out_buf_64 = Q6_P_vaddw_PP(*ptr_out_buf_64, temp_res_64);
                                    ptr_out_buf_64++;
                                }
                            }
                            /* If the number of samples are odd */
                            if(samp)
                            {
                                ptr_out_buf_32   = (int32_t *) ptr_out_buf_64;
                                ptr_local_buf_32 = (int32_t *) ptr_local_buf_64;

                                /* Right Shift to Q15 format */
                                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64(((*ptr_local_buf_32++) + local_rnd_factor_unit_gain), local_shift_factor_unit_gain));

                                /* Accumulate the 32-bit sample */
                                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                ptr_out_buf_32++;
                            }
                        } /* valid i/p channel */
                    } /* non-zero o/p channel map */
                } /* gain = unity gain for output in Q15 and in bytes > out bytes */
                else /* gain > unity gain */
                {
                    /* Identify the o/p channel mapping for the current channel */
                    unTempChMap = pCurrentOutPort->unChannelMapping[i];

                    /* Advance the o/p pointer to the appropriate channel for accumulation */
                    ptr_out_buf_32 = ((int32_t*)pOutputBuf) + (i * pCurrentOutPort->unOutPortPerChBufSize);

                    /* Process only if a non-zero o/p channel map is found */
                    if(0 != unTempChMap)
                    {
                        /* Using the reverse look up table, find the i/p channel number corresponding to this o/p channel map */
                        unTempChNum = Q6_R_ct0_R(pCurrentInPort->unChMapToChRevLUMask[unTempChMap - 1]);

                        /* Process only if a valid i/p channel number is obtained */
                        if(MT_MX_NUM_CHANNELS_EIGHT > unTempChNum)
                        {
                            /* Advance the i/p pointer to the appropriate channel for accumulation */
                            ptr_local_buf_32 = (ptr_local_buf_start_32 + (unTempChNum * pCurrentInPort->unInPortPerChBufSize));

                            for(j = 0; j < unNumSamplesGainAndAccPerCh; j++)
                            {
                                /* Apply the gain on sample */
                                post_gain_sample_64 = s64_mult_s32_u16(*ptr_local_buf_32++, *pCurrentGain);

                                /* Right shift to Q15 format */
                                post_gain_sample_32 = s32_saturate_s64(s64_shl_s64((post_gain_sample_64 + local_rnd_factor), local_shift_factor));

                                /* Accumulate the gain applied 32-bit sample */
                                (*ptr_out_buf_32) = Q6_R_add_RR(*ptr_out_buf_32, post_gain_sample_32);
                                ptr_out_buf_32++;
                            }
                        } /* valid i/p channel map */
                    } /* Non-zero o/p channel map */
                } /* gain > unity */

                /* Increment the pointer to fetch the next channel's gain value */
                pCurrentGain++;
            } /*outer for loop */
        } /*in bytes > out bytes */
    } /* unequal number of bytes */
}

void MtMx_SetupInputToOutputGainParams(This_t *me,
		uint32_t unInPortID, uint32_t unOutPortID,
		adm_session_copp_gain_v5_t *pSessionCOPPGain)
{
	uint16_t j;
	MatrixInPortInfoType             *pCurrentInPort = me->inPortParams[unInPortID];
	mt_mx_struct_ramp_gain_t *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
	MatrixOutPortInfoType          *pCurrentOutPort = me->outPortParams[unOutPortID];

	if (MT_MX_NUM_CHANNELS_EIGHT < pCurrentOutPort->unNumChannels)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_RampGains: Invalid #ch = %lu on o/p port %lu.", pCurrentOutPort->unNumChannels, unOutPortID);
		return;
	}

	// All variables are bounded by MT_MX_NUM_CHANNELS_EIGHT
	uint16_t *pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);
	uint16_t *pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
	uint16_t *pIncomingTargetGain = &(pSessionCOPPGain->target_gain_ch_1);
	int16_t  *pGainIncrementStep = &(pCurrentInputOutputPath->nGainIncDecStep[0]);
	uint16_t *pCachedGain = &(pCurrentInputOutputPath->unCachedTargetGainAfterUnMute[0]);

	pCurrentInputOutputPath->unStepSizeInSamples = pCurrentInPort->unAfeFrameSizeInSamples;
	pCurrentInputOutputPath->unRampCurve = pSessionCOPPGain->ramp_curve;

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: I/p Port ID: %lu, O/p Port ID: %lu, o/p #ch: %lu", unInPortID, unOutPortID, pCurrentOutPort->unNumChannels);

	for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
	{
		if(TRUE == pCurrentInputOutputPath->bIsChannelMuted[j])
		{
			//This channel is currently muted/muting. Do not apply the ramp gain command. Simply, cache the value of the target gain and move on.
			//pTargetGain and pCurrentGain CANNOT be adjusted as the mute takes care of that (MUTE takes precedence over Ramp Gain)
			*pCachedGain = *pIncomingTargetGain;
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: Ch# %d is muted/muting. Ramp will be ignored. Cached gain (applied after un-mute): %d",
					(int)j, (int)(*pCachedGain));

			//The following check is needed to get rid of some saturation artifacts
			if(*pTargetGain == *pCurrentGain)
			{
				*pGainIncrementStep = 0;
				pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j] = FALSE;
				pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j] = 0;
				pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] = 0;
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: Ch# %d Target Gain = Current Gain", (int)j);
			}
		}
		else
		{
			*pCachedGain = *pIncomingTargetGain;
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: Ch# %d normal operation. Cached gain (applied after un-mute): %d",
					(int)j, (int)(*pCachedGain));

			//This channel is currently un-muted. Normal RampGain operation.

                        //In the rare-event that the matrix ramp gain command is issued before the media format can reach
                        //the input port, make sure that the ramp gain is honoured by defaulting the sampling rate
                        //This could lead to a slight inaccuracy in the expected output. But since this is a ramp, the
                        //impact isn't severe. The priority here is to make sure that the ramp gain is honoured correctly.
			if (0 == pCurrentInPort->unSampleRate)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: I/p Port ID %lu doesn't have a valid sampling rate yet. Default to 48kHz",unInPortID);
				pCurrentInputOutputPath->unRampSizeInSamples = ((uint32_t)((pSessionCOPPGain->ramp_duration)*(MT_MX_SAMPLING_RATE_48000))/1000);
				pCurrentInputOutputPath->unStepSizeInSamples = 48;
			}
			else
			{
			pCurrentInputOutputPath->unRampSizeInSamples = ((uint32_t)((pSessionCOPPGain->ramp_duration)*(pCurrentInPort->unSampleRate))/1000);
			}
			//MAP:TBD:If ramp size is not integral multiple of step size, there will be accuracy problem. Here step size is taken as equal to afe frame size
			pCurrentInputOutputPath->unNumRemSteps = (pCurrentInputOutputPath->unRampSizeInSamples) / (pCurrentInputOutputPath->unStepSizeInSamples);

			pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j] = FALSE;
			pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j] = 0;
			pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] = 0;

			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: RampSize=%d,StepSize=%d,Steps=%d,Curve=%d", (int)pCurrentInputOutputPath->unRampSizeInSamples,
					(int)pCurrentInputOutputPath->unStepSizeInSamples, (int)pCurrentInputOutputPath->unNumRemSteps, (int)pCurrentInputOutputPath->unRampCurve);

			//Setup Target Gain
			*pTargetGain = *pIncomingTargetGain;

			if(0 == pCurrentInputOutputPath->unRampSizeInSamples)
			{
				//The bShouldRampGainBeApplied is a global flag across all channels.
				//Individual channels not wanting a ramp should not dictate the TRUE/FALSE nature of this flag.
				//pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE; //Hence, commenting this line as per above comments
				*pCurrentGain = *pTargetGain;
				*pGainIncrementStep = 0;
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: Ch# %d Ramp Size was set to 0. Target gain will be applied immediately!", (int)j);
			}
			else
			{
				uint16_t unGainDelta;

				pCurrentInputOutputPath->bShouldRampGainBeApplied = TRUE;
				unGainDelta = (*pTargetGain >= *pCurrentGain)?(*pTargetGain - *pCurrentGain):(*pCurrentGain - *pTargetGain);

				//Determine if this channel needs to operate in slow ramp mode
				if((unGainDelta) >= (pCurrentInputOutputPath->unNumRemSteps))
				{
					//Deltay >= Deltax. This is the more typical use case for using ramp gain command.
					//The calculation of the increment step is performed in two steps to avoid any implicit typecast
					//from signed to unsigned number
					//a. Use the absolute value to obtain the step value
					//b. Apply the sign based on which gain is greater.
					uint16_t unStepVal = (unGainDelta)/(pCurrentInputOutputPath->unNumRemSteps);
					*pGainIncrementStep = (*pTargetGain >= *pCurrentGain)?(unStepVal):(-unStepVal);
				}
				else
				{
					if(*pTargetGain == *pCurrentGain)
					{
						*pGainIncrementStep = 0;
						MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: Ch# %d Target Gain = Current Gain", (int)j);
					}
					else
					{
						//Deltax > Deltay. This is a rare use case scenario. This is also referred to as "slow ramp".
						*pGainIncrementStep = (*pTargetGain > *pCurrentGain) ? (1) : (-1);
						pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j] = TRUE;
						pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j] = (pCurrentInputOutputPath->unNumRemSteps) / (unGainDelta);
						pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] = pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j];
					}
				}
			}
		}

		MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: pTargetGain[%d]: %d; pCurrentGain[%d]: %d; GainIncStep[%d]: %d",
				(int)j, (int)(*pTargetGain), (int)j, (int)(*pCurrentGain), (int)j, (int)(*pGainIncrementStep));
		MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_RampGains: SlowRamp?[%d]: %d; NumSampWait[%d]: %d; CurrWaitCtr[%d]: %d",
				(int)j, (int)(pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j]), (int)j, (int)(pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j]),
				(int)j, (int)(pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j]));

		pTargetGain++;
		pCurrentGain++;
		pIncomingTargetGain++;
		pGainIncrementStep++;
		pCachedGain++;
	}
}

void MtMx_SetupInputToOutputMuteParams(This_t *me,
		uint32_t unInPortID, uint32_t unOutPortID,
		adm_cmd_matrix_mute_v5_t *pMutePayload)
{
	uint16_t j;
	MatrixInPortInfoType             *pCurrentInPort = me->inPortParams[unInPortID];
	mt_mx_struct_ramp_gain_t *pCurrentInputOutputPath = &(pCurrentInPort->structRampGain[unOutPortID]);
	MatrixOutPortInfoType          *pCurrentOutPort = me->outPortParams[unOutPortID];

	if (MT_MX_NUM_CHANNELS_EIGHT < pCurrentOutPort->unNumChannels)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_MuteParams: Invalid #ch = %lu on o/p port %lu.", pCurrentOutPort->unNumChannels, unOutPortID);
		return;
	}

	// All variables are bounded by MT_MX_NUM_CHANNELS_EIGHT
	uint16_t *pTargetGain = &(pCurrentInputOutputPath->unTargetGain[0]);
	uint16_t *pCurrentGain = &(pCurrentInputOutputPath->unCurrentGain[0]);
	int16_t  *pGainIncrementStep = &(pCurrentInputOutputPath->nGainIncDecStep[0]);
	uint16_t *pCachedGain = &(pCurrentInputOutputPath->unCachedTargetGainAfterUnMute[0]);
	uint8_t  *pChannelMuteStatus = &(pMutePayload->mute_flag_ch_1);

	pCurrentInputOutputPath->unStepSizeInSamples = pCurrentInPort->unAfeFrameSizeInSamples;
	pCurrentInputOutputPath->unRampCurve = 0; //Soft-mute is always linear

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_Mute: I/p Port ID: %lu, O/p Port ID: %lu, o/p #ch: %lu", unInPortID, unOutPortID, pCurrentOutPort->unNumChannels);

	for (j = 0; j < pCurrentOutPort->unNumChannels; j++)
	{
		uint16_t unGainDelta;

		//NOTE: Mute command takes precedence over Ramp Gain. This means any on-going ramps would automatically canceled.
		//If there is an existing ramp gain / existing soft un-mute, cache the target gain.
		//If there is an existing soft mute, do not change the cached gain.
		//If there are no existing ramps of any sort, do not change the cached gain.
		*pCachedGain = ((pCurrentInputOutputPath->unNumRemSteps > 0) && (!pCurrentInputOutputPath->bIsChannelMuted[j]))
			                    																				   ? (*pTargetGain) : (*pCachedGain);
                 //In the rare-event that the matrix mute command is issued before the media format can reach
                 //the input port, make sure that the mute/unmute is honoured by defaulting the sampling rate
                 //This could lead to a slight inaccuracy in the expected output. But since this is a ramp, the
                 //impact isn't severe. The priority here is to make sure that the mute/unmute is honoured correctly.
		if (0 == pCurrentInPort->unSampleRate)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_Mute: I/p Port ID %lu doesn't have a valid sampling rate yet. Default to 48kHz",unInPortID);
			pCurrentInputOutputPath->unRampSizeInSamples = ((uint32_t)((pMutePayload->ramp_duration)*(MT_MX_SAMPLING_RATE_48000))/1000);
			pCurrentInputOutputPath->unStepSizeInSamples = 48;
		}
                else
		{
			pCurrentInputOutputPath->unRampSizeInSamples = ((uint32_t)((pMutePayload->ramp_duration)*(pCurrentInPort->unSampleRate))/1000);
		}
		//MAP:TBD:If ramp size is not integral multiple of step size, there will be accuracy problem. Here step size is taken as equal to afe frame size
		pCurrentInputOutputPath->unNumRemSteps = (pCurrentInputOutputPath->unRampSizeInSamples) / (pCurrentInputOutputPath->unStepSizeInSamples);
		pCurrentInputOutputPath->bIsChannelMuted[j] = ((*pChannelMuteStatus) > 0) ? TRUE : FALSE;
		pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j] = FALSE;
		pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j] = 0;
		pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] = 0;

		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_Mute: RampSize=%d,StepSize=%d,Steps=%d,Curve=%d", (int)pCurrentInputOutputPath->unRampSizeInSamples,
				(int)pCurrentInputOutputPath->unStepSizeInSamples, (int)pCurrentInputOutputPath->unNumRemSteps, (int)pCurrentInputOutputPath->unRampCurve);

		//New command is --  Mute: Target Gain = 0; Un-mute: Target Gain = Cached Gain
		*pTargetGain = (pCurrentInputOutputPath->bIsChannelMuted[j]) ? 0 : *pCachedGain;

		if(0 == pCurrentInputOutputPath->unRampSizeInSamples)
		{
			*pCurrentGain = *pTargetGain;
			*pGainIncrementStep = 0;
			pCurrentInputOutputPath->bShouldRampGainBeApplied = FALSE; //A mute command supercedes any previous ramp gain/ramp mute commands
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_Mute: Ch# %d Ramp duration was set to 0. Target gain will be applied immediately!", (int)j);
		}
		else
		{
			pCurrentInputOutputPath->bShouldRampGainBeApplied = TRUE;
			unGainDelta = (*pTargetGain >= *pCurrentGain)?(*pTargetGain - *pCurrentGain):(*pCurrentGain - *pTargetGain);

			//Determine if this channel needs to operate in slow ramp mode
			if((unGainDelta) >= (pCurrentInputOutputPath->unNumRemSteps))
			{
				//Deltay >= Deltax. This is the more typical use case for using ramp gain command.
				//The calculation of the increment step is performed in two steps to avoid any implicit typecast
				//from signed to unsigned number
				//a. Use the absolute value to obtain the step value
				//b. Apply the sign based on which gain is greater.
				uint16_t unStepVal = (unGainDelta)/(pCurrentInputOutputPath->unNumRemSteps);
				*pGainIncrementStep = (*pTargetGain >= *pCurrentGain)?(unStepVal):(-unStepVal);
			}
			else
			{
				if(*pTargetGain == *pCurrentGain)
				{
					*pGainIncrementStep = 0;
					MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_Mute: Ch# %d Target Gain = Current Gain", (int)j);
				}
				else
				{
					//Deltax > Deltay. This is a rare use case scenario. This is also referred to as "slow ramp".
					*pGainIncrementStep = (*pTargetGain > *pCurrentGain) ? (1) : (-1);
					pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j] = TRUE;
					pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j] = (pCurrentInputOutputPath->unNumRemSteps) / (unGainDelta);
					pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j] = pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j];
				}
			}
		}

		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_Mute: IsChannelMuted[%d]: %d; pCachedGain[%d]: %d;" ,
				(int)j, (int)(pCurrentInputOutputPath->bIsChannelMuted[j]), (int)j, (int)(*pCachedGain));
		MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_Mute: pTargetGain[%d]: %d; pCurrentGain[%d]: %d; GainIncStep[%d]: %d",
				(int)j, (int)(*pTargetGain), (int)j, (int)(*pCurrentGain), (int)j, (int)(*pGainIncrementStep));
		MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx_Mute: SlowRamp?[%d]: %d; NumSampWait[%d]: %d; CurrWaitCtr[%d]: %d",
				(int)j, (int)(pCurrentInputOutputPath->bIsRunningInSlowRampModeSR[j]), (int)j, (int)(pCurrentInputOutputPath->unNumSamplesWaitToIncDecSR[j]),
				(int)j, (int)(pCurrentInputOutputPath->unCurrentSamplesWaitCounterSR[j]));

		pTargetGain++;
		pCurrentGain++;
		pGainIncrementStep++;
		pCachedGain++;
		pChannelMuteStatus++;
	}
}

void MxAt_OutPortToUpdateAccBufAvailabilityMask(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	MatrixInPortInfoType    *pCurrentInPort;
	uint32_t unInPortID;

	pCurrentOutPort->accBufAvailabilityMask = 0;
	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if ((NULL != pCurrentInPort) && ((pCurrentInPort->strMask) & (1 << unOutPortID)))
		{
			pCurrentOutPort->accBufAvailabilityMask |= (1 << unInPortID);
		}
	}
}

void MtMx_PrintDebugMediaTypeInformationOutputPort(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];

	MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: [o/p port ID %lu num_chan, Bytes/sam, sample_rate] = [%lu, %lu, %lu]",
			me->mtMxID, unOutPortID, pCurrentOutPort->unNumChannels, pCurrentOutPort->unBytesPerSample, pCurrentOutPort->unSampleRate);
	MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx: Channel mapping = %d, %d, %d, %d, %d, %d, %d, %d",
			(int)pCurrentOutPort->unChannelMapping[0], (int)pCurrentOutPort->unChannelMapping[1],
			(int)pCurrentOutPort->unChannelMapping[2], (int)pCurrentOutPort->unChannelMapping[3],
			(int)pCurrentOutPort->unChannelMapping[4], (int)pCurrentOutPort->unChannelMapping[5],
			(int)pCurrentOutPort->unChannelMapping[6], (int)pCurrentOutPort->unChannelMapping[7]);
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx: PerfMode: %u, FrameDur (usec): (%lu)",
			pCurrentOutPort->unPortPerfMode, pCurrentOutPort->unFrameDurationInUsec.int_part);
}

void MtMx_PrintDebugMediaTypeInformationInputPort(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];

	MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: [i/p port ID %lu num_chan, Bytes/sam, sample_rate] = [%lu, %d, %lu]",
			me->mtMxID, unInPortID, pCurrentInPort->unNumChannels, pCurrentInPort->unBytesPerSample, pCurrentInPort->unSampleRate);
	MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx: Channel Mapping = %d, %d, %d, %d, %d, %d, %d, %d",
			(int)pCurrentInPort->unChannelMapping[0], (int)pCurrentInPort->unChannelMapping[1],
			(int)pCurrentInPort->unChannelMapping[2], (int)pCurrentInPort->unChannelMapping[3],
			(int)pCurrentInPort->unChannelMapping[4], (int)pCurrentInPort->unChannelMapping[5],
			(int)pCurrentInPort->unChannelMapping[6], (int)pCurrentInPort->unChannelMapping[7]);
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx: PerfMode: %u, FrameDur (usec): (%lu)",
			pCurrentInPort->unPortPerfMode, (uint32_t)pCurrentInPort->unFrameDurationInUsec.int_part);
}

void MtMx_CheckIfAnyConnOutPortsInNativeModeNeedReInit(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort  = me->inPortParams[unInPortID];
	uint32_t                       strMask = pCurrentInPort->strMask;
	uint32_t                  unOutPortID;
	MatrixOutPortInfoType   *pCurrentOutPort;

	while (strMask)
	{
		unOutPortID = Q6_R_ct0_R(strMask);
		strMask ^= (1 << unOutPortID);
		pCurrentOutPort = me->outPortParams[unOutPortID];

		//If a connected o/p port is operating in native mode, mark it pending for re-initing itself
		if(pCurrentOutPort->unNativeModeFlags)
		{
			pCurrentOutPort->bIsOutPortInNativeModePendingReInit = TRUE;
		}
	}
}

ADSPResult MtMx_ReInitNativeModeOutport(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	ADSPResult             result = ADSP_EOK;

	//Store the current o/p port cfg. Only store it if the o/p port is not in Inactive state.
	//If in Inactive State, dont overwrite previous state.
	if(OUTPUT_PORT_STATE_INACTIVE != pCurrentOutPort->outPortState)
	{
		MtMx_OpPortReCfg_StoreCurrentCfg(me, unOutPortID, pCurrentOutPort);
	}

	//Mark this o/p port state as inactive
	pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_INACTIVE;

	//Since this port state has changed to inactive, need to update ActiveOutPortsMask
	MtMx_OutPortToUpdateActiveOutPortsMasks(me, unOutPortID);

	//For o/p port operating in native mode, the Num. of channels, channel mapping and bytes/sample
	//needs to be derived from the connected i/p ports.
	if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		MxAr_UpdateMediaTypeForNativeModeOutputPort(me, unOutPortID);
	}
	else
	{
		MxAt_UpdateMediaTypeForNativeModeOutputPort(me, unOutPortID);
	}

	//If the output media type has not changed, don't do anything and return.
	if(FALSE == MtMx_OpPortReCfg_CheckIfReCfgNeeded(me, unOutPortID, pCurrentOutPort))
	{
		//Check and try to send the MT down (if nothing has changed, MT would not be sent). This is added here to cover the case when only channel map has changed.
		//When channel map alone changes, we would need to resend the media type, however, the buffer reconfiguration would not be required.
		if (ADSP_FAILED(result = MtMx_OpPortReCfg_CheckAndSendMT(me, unOutPortID, pCurrentOutPort)))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %lu Error in MT check and send step! Result = %d",
					me->mtMxID, unOutPortID, result);
			return result;
		}

		pCurrentOutPort->outPortState = pCurrentOutPort->outPortStatusPriorToReconfig;
		pCurrentOutPort->bIsOutPortInNativeModePendingReInit = FALSE;
		MtMx_OutPortToUpdateActiveOutPortsMasks(me, unOutPortID);
		return result;
	}

	MtMx_SetOutPortFrameDuration(me, unOutPortID);

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
				return result;
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
				me->mtMxID, unOutPortID, result);
		MtMx_ForceDisableOutputPortInBadState(me, unOutPortID);
		return result;
	}

	return result;
}

void MxAt_UpdateMediaTypeForNativeModeOutputPort(This_t *me, uint32_t unOutPortID)
{
  MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];
  MatrixInPortInfoType *pCurrentInPort_temp;
  uint32_t unInPortID_temp, i, j, currInPortMask, cummOutNumChannels = 0;

  currInPortMask = pCurrentOutPort->inPortsMask;

  if(0 == pCurrentOutPort->inPortsTopPriorityMask)
  {
    //No top priority i/p port. Retaining the media format from stream

    // Usually, the below case will not hit. But keeping it for avoiding errors
    if(0 == pCurrentOutPort->unSampleRate)
    {
      pCurrentOutPort->unSampleRate = MT_MX_SAMPLING_RATE_48000;
    }

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port ID %lu does not have any i/p connected. Retaining existing #ch/ch map/bps on o/p",
          me->mtMxID, unOutPortID);

    // printing media format for debugging purposes
    MtMx_PrintDebugMediaTypeInformationOutputPort(me, unOutPortID);
    return;
  }

  //Look for the top priority i/p port. Assign the channel mapping for the o/p based on the i/p channel mapping.
  unInPortID_temp = Q6_R_ct0_R(pCurrentOutPort->inPortsTopPriorityMask);
  currInPortMask ^= (1 << unInPortID_temp);

  pCurrentInPort_temp = me->inPortParams[unInPortID_temp];
  pCurrentOutPort->unNumChannels = pCurrentInPort_temp->unNumChannels;
  pCurrentOutPort->unBytesPerSample = pCurrentInPort_temp->unBytesPerSample;

  if(0 == pCurrentOutPort->unBytesPerSample)
  {
    pCurrentOutPort->unBytesPerSample = MT_MX_DEFAULT_BYTES_PER_SAMPLE;
  }

  if(0 == pCurrentOutPort->unNumChannels)
  {
    pCurrentOutPort->unNumChannels = MT_MX_NUM_CHANNELS_TWO;
    pCurrentOutPort->unChannelMapping[0] = PCM_CHANNEL_L;
    pCurrentOutPort->unChannelMapping[1] = PCM_CHANNEL_R;
  }
  else
  {
    for(i = 0; i < pCurrentOutPort->unNumChannels; i++)
    {
      pCurrentOutPort->unChannelMapping[i] = pCurrentInPort_temp->unChannelMapping[i];
    }
  }
  cummOutNumChannels = pCurrentOutPort->unNumChannels;

  if(0 == pCurrentOutPort->unSampleRate)
  {
    pCurrentOutPort->unSampleRate = MT_MX_SAMPLING_RATE_48000;
  }

  while(currInPortMask && (MT_MX_NUM_CHANNELS_EIGHT > cummOutNumChannels))
  {
    //Get the next connected i/p port.
    unInPortID_temp = Q6_R_ct0_R(currInPortMask);
    currInPortMask ^= (1 << unInPortID_temp);
    pCurrentInPort_temp = me->inPortParams[unInPortID_temp];

    for(i = 0; i < pCurrentInPort_temp->unNumChannels; i++)
    {
      bool_t matchFound = FALSE;
      for(j = 0; j < cummOutNumChannels; j++)
      {
        if(pCurrentOutPort->unChannelMapping[j] == pCurrentInPort_temp->unChannelMapping[i])
        {
          matchFound = TRUE;
          break;
        }
      }
      if(FALSE == matchFound)
      {
        //If the o/p port does not have this channel map, add it. Thus, the o/p port will be a
        //union of all the unique i/p port channel maps that are connected to it.
        pCurrentOutPort->unNumChannels++;
        cummOutNumChannels++;

        pCurrentOutPort->unChannelMapping[cummOutNumChannels - 1] = pCurrentInPort_temp->unChannelMapping[i];

        MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port ID %lu added Chmap %d to channel# %lu (index: %lu)",
              me->mtMxID, unOutPortID, pCurrentOutPort->unChannelMapping[cummOutNumChannels - 1], cummOutNumChannels, cummOutNumChannels - 1);

        if(MT_MX_NUM_CHANNELS_EIGHT <= cummOutNumChannels)
        {
          return;
        }
      }
    }
  }
}

void MxAr_UpdateMediaTypeForNativeModeOutputPort(This_t *me, uint32_t unOutPortID)
{
  MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];
  uint32_t unInPortID_temp = 0, currInPortMask = 0, unNumChannelsMapped = 0;

  MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Config native mode for o/p port ID %lu : Ch Nativity enable [%d], Bit Width Nativity enable [%d]",
        me->mtMxID,
        unOutPortID,
        MtMx_ChannelNativityIsEnabled(pCurrentOutPort->unNativeModeFlags),
        MtMx_BitWidthNativityIsEnabled(pCurrentOutPort->unNativeModeFlags));

  // get the current connected input ports mask
  currInPortMask = pCurrentOutPort->inPortsMask;

  // ---------------------
  // DEFAULTS
  // ---------------------
  // If nothing is connected yet, retain the existing values.
  if(0 == currInPortMask)
  {
    // No input ports. Retaining the previous valid media format
    // Initially, this is the media format from ADM open

    // Usually, the below case will not hit. But keeping it for avoiding errors
    if(0 == pCurrentOutPort->unSampleRate)
    {
      pCurrentOutPort->unSampleRate = MT_MX_SAMPLING_RATE_48000;
    }

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port ID %lu does not have any i/p connected. Retaining the previous #ch/ch map/bps on o/p",
          me->mtMxID, unOutPortID);

    // printing media format for debugging purposes
    MtMx_PrintDebugMediaTypeInformationOutputPort(me, unOutPortID);
    return;
  }

  // By this time, output port supposed to have valid media format.
  // Following conditions will not hit. But keeping them for avoiding errors
  // Set a default channel configuration if not previously set
  if(0 == pCurrentOutPort->unNumChannels)
  {
    // Default case
    pCurrentOutPort->unNumChannels = MT_MX_NUM_CHANNELS_TWO;
    pCurrentOutPort->unChannelMapping[0] = PCM_CHANNEL_L;
    pCurrentOutPort->unChannelMapping[1] = PCM_CHANNEL_R;
  }

  // Set a default bytes per sample if not previously set
  if(0 == pCurrentOutPort->unBytesPerSample)
  {
    pCurrentOutPort->unBytesPerSample = MT_MX_DEFAULT_BYTES_PER_SAMPLE;
  }

  // Set a default sampling rate if not previously set
  if(0 == pCurrentOutPort->unSampleRate)
  {
    pCurrentOutPort->unSampleRate = MT_MX_SAMPLING_RATE_48000;
  }

  // ---------------------
  // CH NATIVITY SETTINGS
  // ---------------------
  // Check for Channel nativity and set accordingly
  if(TRUE == MtMx_ChannelNativityIsEnabled(pCurrentOutPort->unNativeModeFlags))
  {
    uint32_t unNumChannels = 0;

    // Iterate over the connected input ports until all input ports are mapped or all
    // channel mappings are occupied.
    while(currInPortMask && (MT_MX_NUM_CHANNELS_EIGHT > unNumChannelsMapped))
    {
      // Reset the scratch variables used to find the input with the highest number of channels
      MatrixInPortInfoType *pCurrentInPort_temp = NULL;
      uint32_t currInPortMask2 = currInPortMask;
      uint32_t unMaxNumChannels = 0;
      uint32_t unHighestPrioChIndex = UINT32_MAX;

      // Search for the input port with the highest # of channels.
      // If two inputs have the same number of channels, use the input with the lowest index.
      while(currInPortMask2)
      {
        unInPortID_temp = Q6_R_ct0_R(currInPortMask2);
        currInPortMask2 ^= (1 << unInPortID_temp);
        pCurrentInPort_temp = me->inPortParams[unInPortID_temp];

        if((pCurrentInPort_temp->unNumChannels > 0) &&
           (pCurrentInPort_temp->unNumChannels <= MT_MX_NUM_CHANNELS_EIGHT) &&
           (pCurrentInPort_temp->unNumChannels > unMaxNumChannels))
        {
          unMaxNumChannels = pCurrentInPort_temp->unNumChannels;
          unHighestPrioChIndex = unInPortID_temp;
        }
        else
        {
          //The input port in question does not qualify yet.
          // remove the channel from connected input mask as it is not valid.
          currInPortMask ^= (1 << unInPortID_temp);
          //Continue looking for another.
          continue;
        }
      }

      if(UINT32_MAX == unHighestPrioChIndex)
      {
        //This means that a valid unHighestPrioChIndex was not found. Exit.
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Native mode channelmap configuration exiting for o/p port ID %lu as nothing connected yet.", me->mtMxID, unOutPortID);
        // printing media format for debugging purposes
        MtMx_PrintDebugMediaTypeInformationOutputPort(me, unOutPortID);
        break;
      }

      // Retrieve the input port properties
      pCurrentInPort_temp = me->inPortParams[unHighestPrioChIndex];

      // Iterate over the input port channels
      for(uint32_t i = 0; i < pCurrentInPort_temp->unNumChannels; i++)
      {
        // Check if a common mapping already exists, ie. a match is found.
        bool_t matchFound = FALSE;
        for(uint32_t j = 0; j < unNumChannelsMapped; j++)
        {
          if(pCurrentOutPort->unChannelMapping[j] == pCurrentInPort_temp->unChannelMapping[i])
          {
            matchFound = TRUE;
            break;
          }
        }

        // If the o/p port does not have this channel mapping, add it to the output.
        // The o/p port will be a union of all the unique connected i/p port channel mappings.
        if(FALSE == matchFound)
        {
          // Increment the output port number of channels and number of channels mapped
          unNumChannels++;
          unNumChannelsMapped++;

          // Add the channel mapping
          pCurrentOutPort->unChannelMapping[unNumChannelsMapped - 1] = pCurrentInPort_temp->unChannelMapping[i];
          MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port ID %lu added Chmap %d to channel# %lu (index: %lu)",
                me->mtMxID, unOutPortID, pCurrentOutPort->unChannelMapping[unNumChannelsMapped - 1], unNumChannelsMapped, unNumChannelsMapped - 1);

          // if we exhausted all the channel mapping slots break
          if(MT_MX_NUM_CHANNELS_EIGHT <= unNumChannelsMapped)
          {
            break;
          }
        }
      }

      // remove the channel from connected input mask as it is now mapped.
      currInPortMask ^= (1 << unHighestPrioChIndex);
    }

    // update output numchannels only if it is updated in the above loop
    if(0 != unNumChannels)
    {
      pCurrentOutPort->unNumChannels = unNumChannels;
    }
  }


  // ----------------------------
  // BIT WIDTH NATIVITY SETTINGS
  // ----------------------------
  // Check for bit width nativity and setup accordingly
  if(TRUE == MtMx_BitWidthNativityIsEnabled(pCurrentOutPort->unNativeModeFlags))
  {
    MatrixInPortInfoType *pCurrentInPort_temp = NULL;
    int32_t unMaxBitWidth = 16;
    uint32_t unMaxBitWidthInputIndex = UINT32_MAX;

    // Iterate over the connected inputs and find the input with the highest bit width
    currInPortMask = pCurrentOutPort->inPortsMask;
    while(currInPortMask)
    {
      unInPortID_temp = Q6_R_ct0_R(currInPortMask);
      currInPortMask ^= (1 << unInPortID_temp);
      pCurrentInPort_temp = me->inPortParams[unInPortID_temp];

      if((pCurrentInPort_temp->unBitwidth > 0) &&
         (pCurrentInPort_temp->unBitwidth > unMaxBitWidth))
      {
        unMaxBitWidthInputIndex = unInPortID_temp;
        unMaxBitWidth = pCurrentInPort_temp->unBitwidth;
      }
    }

    if(UINT32_MAX == unMaxBitWidthInputIndex)
    {
      //This means that a valid unMaxBitWidthInputIndex was not found. Exit.
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Native mode BitWidth configuration exiting for o/p port ID %lu as nothing connected yet.", me->mtMxID, unOutPortID);
      return;
    }

    // use the highest bit width for this output's bit width
    pCurrentOutPort->unBitwidth = me->inPortParams[unMaxBitWidthInputIndex]->unBitwidth;
    if(pCurrentOutPort->unBitwidth > 16)
    {
      pCurrentOutPort->unBytesPerSample = MT_MX_BYTES_PER_SAMPLE_FOUR;
    }
    else
    {
      pCurrentOutPort->unBytesPerSample = MT_MX_BYTES_PER_SAMPLE_TWO;
    }
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu bitwidth nativity config set o/p port ID %d to %d bps.",
          me->mtMxID, unOutPortID, pCurrentOutPort->unBitwidth);
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Native mode configuration complete for o/p port ID %lu.",
        me->mtMxID, unOutPortID);

  return;
}

void MxAt_OutPortToCheckForOverflowEvents(This_t *me, uint32_t unOutPortID)
{
	ADSPResult              result;
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];

	//Check if this o/p port is registered to send overflow events and if it's status is to send one down right now.
	if (TRUE == pCurrentOutPort->bIsRegisteredForOverflowEvents &&
			MT_MX_OUTPUT_PORT_OVERFLOW_REPORT_STATUS_REPORT == pCurrentOutPort->unOverflowReportStatus && (NULL != (pCurrentOutPort->eliteCb.pCbHandle)))
	{
		//For this same event which triggered this overflow event, do not resend the event.
		//This status will be reset after a successful buffer is sent out of this o/p port.
		pCurrentOutPort->unOverflowReportStatus = MT_MX_OUTPUT_PORT_OVERFLOW_REPORT_STATUS_DO_NOT_REPORT;

		//Send the overflow event
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"MtMx #%lu: generating overflow event for output port %lu", me->mtMxID, unOutPortID);
		Elite_CbType *pEliteCb = &(pCurrentOutPort->eliteCb);
		result = pEliteCb->pCbFct(pEliteCb->pCbHandle, ASM_SESSION_EVENT_TX_OVERFLOW, 0, NULL, 0);
		if (ADSP_FAILED(result))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Error - MtMx #%lu: callback ftn for generating overflow event for o/p port %lu returned with status 0x%x",
					me->mtMxID, unOutPortID, result);
		}
	}
}


void MtMx_DetectIfPullPortsNeedsToSendData(This_t *me)
{
	uint32_t unInPortID;
	MatrixInPortInfoType *pCurrentInPort;
	MatrixOutPortInfoType *pCurrentOutPort;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		if ( (NULL != me->inPortParams[unInPortID])
				&& (INPUT_PORT_STATE_INACTIVE != me->inPortParams[unInPortID]->inPortState))
		{
			pCurrentInPort = me->inPortParams[unInPortID];
			uint32_t unOutPortID;
			uint32_t unStrMask = pCurrentInPort->strMask;
			while(unStrMask)
			{
				unOutPortID = Q6_R_ct0_R(unStrMask);
				unStrMask ^= 1 << unOutPortID;
				pCurrentOutPort = me->outPortParams[unOutPortID];

				//If the current i/p buffer is FULL AND current top prio o/p port is seeking data AND that o/p port is a pull port
				//waiting on data, accumulate data and try to send the data downstream.
				if (
						((pCurrentInPort->bIsLocalBufFull) || (pCurrentInPort->bIsAccumulationPending)) &&
						((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask)) &&
						(TRUE == pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut)
				)
				{
					if (pCurrentInPort->bIsAccumulationPending)
					{
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu pending accumulation", me->mtMxID, unInPortID);
					}

					MxAr_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);

					if (pCurrentInPort->bIsAccumulationPending)
					{
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu setting bIsAccumulationPending = 0", me->mtMxID, unInPortID);
						pCurrentInPort->bIsAccumulationPending = FALSE;
					}
				}
			}
		}
	}
	MxAr_ProcessPendingPullOutPorts(me);
}

void MtMx_SetOPRMaskOnInputPortsConnectedToPendingPullPorts(This_t *me)
{
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType  *pCurrentOutPort;
	uint32_t unInPortID, unOutPortID, unStrMask;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if (NULL != pCurrentInPort)
		{
			if (INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState)
			{
				unStrMask = pCurrentInPort->strMask;
				while(unStrMask)
				{
					unOutPortID = Q6_R_ct0_R(unStrMask);
					unStrMask ^= 1 << unOutPortID;
					pCurrentOutPort = me->outPortParams[unOutPortID];

					if(TRUE == pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut)
					{
						pCurrentInPort->outputReqPendingMask |= (1 << unOutPortID);
						MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu ORPM changed to %lu because pull o/p port %lu was waiting for resources.",
								me->mtMxID, unInPortID, pCurrentInPort->outputReqPendingMask, unOutPortID);
					}
				}
			}
		}
	}
}

void MxAt_DetectIfPendingNonLivePortsNeedsToSendData(This_t *me)
{
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType  *pCurrentOutPort;
	uint32_t unInPortID, unOutPortID, unStrMask;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if (NULL != pCurrentInPort)
		{
			if (INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState)
			{
				unStrMask = pCurrentInPort->strMask;
				while(unStrMask)
				{
					unOutPortID = Q6_R_ct0_R(unStrMask);
					unStrMask ^= 1 << unOutPortID;
					pCurrentOutPort = me->outPortParams[unOutPortID];

					if(OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState)
					{
						//If there are pending accumulations, wake up the o/p port
						if((pCurrentOutPort->inPortsMask) && (pCurrentOutPort->inPortsWaitingToAccMask == pCurrentOutPort->inPortsMask))
						{
							MtMx_AddOutputPortToWaitMask(me, unOutPortID);
						}
					}
				}
			}
		}
	}
}

void MxAt_DetectIfPendingNonLivePortsNeedsToMarkPendingAccOnOp(This_t *me)
{
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType  *pCurrentOutPort;
	uint32_t unInPortID, unOutPortID, unStrMask;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if (NULL != pCurrentInPort)
		{
			if ((INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState) && (FALSE == pCurrentInPort->bIsPortLive))
			{
				unStrMask = pCurrentInPort->strMask;
				while(unStrMask)
				{
					unOutPortID = Q6_R_ct0_R(unStrMask);
					unStrMask ^= 1 << unOutPortID;
					pCurrentOutPort = me->outPortParams[unOutPortID];

					if(OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState)
					{
						if(pCurrentInPort->bIsNonLiveInputPortWaitingToAcc)
						{
							pCurrentOutPort->inPortsWaitingToAccMask |= (1 << unInPortID);
							MtMx_AddOutputPortToWaitMask(me, unOutPortID);

							MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Non-live i/p port %lu waiting to acc., o/p port %lu added to wait mask!",
									me->mtMxID, unInPortID, unOutPortID);
						}
					}
				}
			}
		}
	}
}

ADSPResult MtMx_DetectMixedLatencyConnections(This_t *me)
{
	ADSPResult result =ADSP_EOK;
	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Executing MtMx_DetectMixedLatencyConnections", me->mtMxID);
	{
		uint16_t unInpPortIndex;

		//loop through all currently opened input ports
		for (unInpPortIndex =0; unInpPortIndex <= me->maxInPortID; unInpPortIndex++ )
		{
			if (NULL != me->inPortParams[unInpPortIndex])
			{
				//loop through all the output ports that this input port is feeding to
				uint16_t unOutPortIndex;
				uint32_t unStrMask = me->inPortParams[unInpPortIndex]->strMask;
				while(unStrMask)
				{
					unOutPortIndex = Q6_R_ct0_R(unStrMask);
					unStrMask ^= 1 << unOutPortIndex;

					MtMx_CheckForMixedLatencyAndUpdatePendingBufferReinit(me,unInpPortIndex,unOutPortIndex);
				}
			}
		}

		uint16_t unOutPortIndex;
		//loop through all currently opened output ports
		for (unOutPortIndex =0; unOutPortIndex <= me->maxOutPortID; unOutPortIndex++ )
		{
			if (NULL != me->outPortParams[unOutPortIndex])
			{
				//loop through all the input ports feeding in to this output port
				uint16_t unInpPortIndex;
				uint32_t inPortsMask = me->outPortParams[unOutPortIndex]->inPortsMask;
				while(inPortsMask)
				{
					unInpPortIndex = Q6_R_ct0_R(inPortsMask);
					inPortsMask ^= 1 << unInpPortIndex;

					MtMx_CheckForMixedLatencyAndUpdatePendingBufferReinit(me,unInpPortIndex,unOutPortIndex);
				}
			}
		}
	}

	/** By this time, those input and output ports which require buffer reconfiguration is all known
	 *    1. reconfigure input ports
	 *    2. reconfigure output ports
	 */
	{
		uint16_t unInpPortIndex;

		//loop through all currently opened input ports and check to see if it needs to be reconfigured
		for (unInpPortIndex =0; unInpPortIndex <= me->maxInPortID; unInpPortIndex++ )
		{
			if (NULL != me->inPortParams[unInpPortIndex])
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu triggering MtMx_ReInitInputPortBuffers of i/p port %u", me->mtMxID, unInpPortIndex);
				if (ADSP_FAILED(result = MtMx_ReInitInputPortBuffers(me, unInpPortIndex)))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu reiniting i/p bufs for i/p port %u failed with status[%d]",
							me->mtMxID,unInpPortIndex, result);

					return result;
				}
			}
		}
	}

	{
		uint16_t unOutPortIndex;

		//loop through all currently opened output ports and check to see if it needs to be reconfigured
		for (unOutPortIndex =0; unOutPortIndex <= me->maxOutPortID; unOutPortIndex++ )
		{
			if (NULL != me->outPortParams[unOutPortIndex])
			{
				if (ADSP_FAILED(result = MtMx_ReInitOutputPortBuffers(me,unOutPortIndex)))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu reiniting o/p bufs for o/p port %u failed with status[%d]",
							me->mtMxID,unOutPortIndex,result);

					return result;
				}
			}
		}
	}
	return ADSP_EOK;
}


void MtMx_CheckForMixedLatencyAndUpdatePendingBufferReinit(This_t *me, uint16_t unInpPortId, uint16_t unOutPortId)
{
	MatrixInPortInfoType *pCurrentInPort;
	MatrixOutPortInfoType *pCurrentOutPort;
	uint32_t unDesiredNumAfeFrames;
	uint32_t unInPortNumAfeFrames, unOutPortNumAfeFrames;
	bool_t bisMixedConxExist = FALSE;

	pCurrentInPort = me->inPortParams[unInpPortId];
	pCurrentOutPort = me->outPortParams[unOutPortId];

	unInPortNumAfeFrames = pCurrentInPort->unNumAfeFrames;
	unOutPortNumAfeFrames = pCurrentOutPort->unNumAfeFrames;

	if (pCurrentInPort->unPortPerfMode == pCurrentOutPort->unPortPerfMode)
	{
		/* 1. if a symmetric condition is detected and if the desired number of afe frames is same as
		 *    the current number of afe frames, there is no need to reconfig the port
		 * 2. if mixed latency connection returning to symmetric connection is detected (
		 * this is detected if performance mode between ports are same, and there is no
		 * pending buffer reinit and the desired number of afe frames is not same as the current
		 * number of afe frames),then ports needs to be reconfigured to the desired number of afe frames.
		 *
		 */
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu  symm lat conx is detected btw i/p port %u & o/p port %u",
				me->mtMxID,unInpPortId,unOutPortId  );

		if (FALSE == MtMx_CheckforMixedConxInConnectedInpOutPorts(me,unInpPortId,unOutPortId))
		{
			/**This confirms that all output ports that this input port is feeding to  and all input ports
			 *feeding to this outport port has the same perf mode.
			 */
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %u & o/p port %u does not have any mixed conx in its connected i/p o/p ports",
					me->mtMxID,unInpPortId,unOutPortId  );

			bisMixedConxExist = FALSE;
		}
		else
		{
			//This confirms that there is atleast one input or output port that this input or output ports are feeding to or from,
			// has a mixed connection.
			// Check if input ports needs to be reconfigured or not

			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu  i/p port %u & o/p port %u has atleast one mixed conx in its connected i/p o/p ports",
					me->mtMxID,unInpPortId,unOutPortId  );

			bisMixedConxExist = TRUE;

			unDesiredNumAfeFrames = MT_MX_LEGACY_NUM_AFE_FRAMES;
		}

		if ((FALSE == pCurrentInPort->bIsBufferReinitPending) && (FALSE == pCurrentOutPort->bIsBufferReinitPending))
		{
			if (FALSE == bisMixedConxExist)
			{
				/**This confirms that all output ports that this input port is feeding to  and all input ports
				 *feeding to this outport port has the same perf mode.
				 */

				if ( ADM_LOW_LATENCY_DEVICE_SESSION == pCurrentInPort->unPortPerfMode)
				{
					unDesiredNumAfeFrames = MT_MX_LL_NUM_AFE_FRAMES;
				}
				else
				{
					unDesiredNumAfeFrames = MT_MX_LEGACY_NUM_AFE_FRAMES;
				}

				if (unInPortNumAfeFrames != unDesiredNumAfeFrames)
				{
					/** 2. mixed latency connection returning to symmetric connection is detected
					 */
					pCurrentInPort->bIsBufferReinitPending = TRUE;
					pCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;

					MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu detected mixed lat conx 2 symm conx for inp port %u and bIsBufferReinitPending is %u & newNumAfeFrames is %lu us",
							me->mtMxID,unInpPortId, pCurrentInPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
				}

				if (unOutPortNumAfeFrames != unDesiredNumAfeFrames)
				{
					/** 2. mixed latency connection returning to symmetric connection is detected
					 */
					pCurrentOutPort->bIsBufferReinitPending = TRUE;
					pCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;

					MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu detected mixed lat conx 2 symm conx for out port %u and bIsBufferReinitPending is %u & newNumAfeFrames is %lu us",
							me->mtMxID,unOutPortId, pCurrentOutPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
				}
			}
			else
			{
				//This confirms that there is atleast one input or output port that this input or output ports are feeding to or from,
				// has a mixed connection.
				// Check if input ports needs to be reconfigured or not
				if (unInPortNumAfeFrames != unDesiredNumAfeFrames)
				{
					pCurrentInPort->bIsBufferReinitPending = TRUE;
					pCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				}

				if (unOutPortNumAfeFrames != unDesiredNumAfeFrames)
				{
					pCurrentOutPort->bIsBufferReinitPending = TRUE;
					pCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				}
			}
		}
		else if ((TRUE == pCurrentInPort->bIsBufferReinitPending)  && (FALSE == pCurrentOutPort->bIsBufferReinitPending))
		{
			//although both ports has same perf mode, but one port has a pending reconfig. This is possible if that pending reconfig port
			// was connected to some other port in a mixed latency fashion.  then reconfig the other port too.
			if (FALSE == bisMixedConxExist)
			{
				unDesiredNumAfeFrames = pCurrentInPort->unNewNumAfeFrames;
			}
			else
			{
				//unDesiredNumAfeFrames = MT_MX_LEGACY_NUM_AFE_FRAMES; //(already set)
				pCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu correcting i/p port %u and bIsBufferReinitPending is %u & newNumAfeFrames is %lu us",
						me->mtMxID ,unInpPortId, pCurrentInPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
			}
			if (unDesiredNumAfeFrames != unOutPortNumAfeFrames)
			{
				pCurrentOutPort->bIsBufferReinitPending = TRUE;
				pCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu detected symm conx with mixed buffer sizes btw i/p port %u & o/p port %u and setting o/p port bIsBufferReinitPending is %u & newNumAfeFrames is %lu ",
						me->mtMxID,unInpPortId,unOutPortId, pCurrentOutPort->bIsBufferReinitPending,pCurrentOutPort->unNewNumAfeFrames );
			}
		}
		else if ((FALSE == pCurrentInPort->bIsBufferReinitPending)  && (TRUE == pCurrentOutPort->bIsBufferReinitPending))
		{
			//although both ports has same perf mode, but one port has a pending reconfig. This is possible if that pending reconfig port
			// was connected to some other port in a mixed latency fashion.  then reconfig the other port too.
			if (FALSE == bisMixedConxExist)
			{
				unDesiredNumAfeFrames = pCurrentOutPort->unNewNumAfeFrames;
			}
			else
			{
				//unDesiredNumAfeFrames = MT_MX_LEGACY_NUM_AFE_FRAMES; //(already set)
				pCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu correcting o/p port %u and bIsBufferReinitPending is %u & newNumAfeFrames is %lu us",
						me->mtMxID ,unOutPortId, pCurrentOutPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
			}

			if (unDesiredNumAfeFrames != unInPortNumAfeFrames)
			{
				pCurrentInPort->bIsBufferReinitPending = TRUE;
				pCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu detected symm conx with mixed buffer sizes btw i/p port %u & o/p port %u and setting i/p port bIsBufferReinitPending is %u & newNumAfeFrames is %lu ",
						me->mtMxID,unInpPortId,unOutPortId, pCurrentInPort->bIsBufferReinitPending,pCurrentInPort->unNewNumAfeFrames );
			}
		}
		else
		{  //(TRUE == pCurrentInPort->bIsBufferReinitPending)  && (TRUE == pCurrentOutPort->bIsBufferReinitPending)

			if (FALSE == bisMixedConxExist)
			{
				//both input and output ports are already configured. make sure they have same number of afe frames
				if (pCurrentInPort->unNewNumAfeFrames != pCurrentOutPort->unNewNumAfeFrames)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %u o/p port %u correcting to mixed conx", me->mtMxID ,unInpPortId,unOutPortId);
					unDesiredNumAfeFrames = MT_MX_LEGACY_NUM_AFE_FRAMES;
					pCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
					pCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				}
				else
				{
					//(pCurrentInPort->unNewNumAfeFrames == pCurrentOutPort->unNewNumAfeFrames)
					unDesiredNumAfeFrames = pCurrentInPort->unNewNumAfeFrames;
				}

			}
			else
			{
				unDesiredNumAfeFrames = MT_MX_LEGACY_NUM_AFE_FRAMES;
				pCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				pCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %u o/p port %u correcting to mixed conx", me->mtMxID ,unInpPortId,unOutPortId);
			}
		}
	}
	else
	{
		/**
		 * mixed latency connection is detected. set the bBufferReinitPending flag to true
		 * to those input ports and output ports whose current number of afe frames is not same as
		 * desired number of afe frames (legacy number of afe frames)
		 */
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu  mixed lat conx is detected btw i/p port %u & o/p port %u",
				me->mtMxID,unInpPortId,unOutPortId  );

		unDesiredNumAfeFrames = MT_MX_LEGACY_NUM_AFE_FRAMES;

		if (TRUE == pCurrentInPort->bIsBufferReinitPending )
		{
			if (unDesiredNumAfeFrames != pCurrentInPort->unNewNumAfeFrames )
			{
				pCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu correcting i/p port %u and bIsBufferReinitPending is %u & newNumAfeFrames is %lu",
						me->mtMxID ,unInpPortId, pCurrentInPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
			}
		}
		else
		{
			if (unDesiredNumAfeFrames != unInPortNumAfeFrames )
			{
				pCurrentInPort->bIsBufferReinitPending = TRUE;
				pCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu mixed lat conx detected i/p port %u  and bIsBufferReinitPending is %u & newNumAfeFrames is %lu ",
						me->mtMxID,unInpPortId, pCurrentInPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
			}
		}

		if (TRUE == pCurrentOutPort->bIsBufferReinitPending )
		{
			if (unDesiredNumAfeFrames != pCurrentOutPort->unNewNumAfeFrames )
			{
				pCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu correcting o/p port %u and bIsBufferReinitPending is %u & newNumAfeFrames is %lu",
						me->mtMxID ,unOutPortId, pCurrentOutPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
			}
		}
		else
		{
			if (unDesiredNumAfeFrames != unOutPortNumAfeFrames )
			{
				pCurrentOutPort->bIsBufferReinitPending = TRUE;
				pCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu mixed lat conx detected o/p port %u  and bIsBufferReinitPending is %u & newNumAfeFrames is %lu ",
						me->mtMxID,unOutPortId, pCurrentOutPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
			}
		}
	}

	{
		/**
		 * loop thru all the connected input ports feeding to this output port and
		 * set the input port's bIsBufferReinitPending flag to TRUE for those input ports whose current
		 * Number of afe frames is not same as desired number of afe frames and there is no pending buffer
		 * reinitialization. For input ports whose bIsBufferReinitPending is already TRUE, there is
		 * no need to check again, since it might have already being checked and updated before.
		 */
		//        if (TRUE == pCurrentOutPort->bIsBufferReinitPending )
		//        {
		uint32_t currConnInputPorts = pCurrentOutPort->inPortsMask;
		uint16_t unTempInpPortId;
		MatrixInPortInfoType *pTempCurrentInPort;
		while(currConnInputPorts)
		{
			unTempInpPortId = Q6_R_ct0_R(currConnInputPorts);
			currConnInputPorts ^= (1 << unTempInpPortId);
			pTempCurrentInPort = me->inPortParams[unTempInpPortId];

			if ((FALSE == pTempCurrentInPort->bIsBufferReinitPending) &&
					(unDesiredNumAfeFrames != pTempCurrentInPort->unNumAfeFrames )
			)
			{
				pTempCurrentInPort->bIsBufferReinitPending = TRUE;
				pTempCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu detected i/p port %u  and bIsBufferReinitPending is %u & newNumAfeFrames is %lu ",
						me->mtMxID,unTempInpPortId, pTempCurrentInPort->bIsBufferReinitPending ,unDesiredNumAfeFrames );
			}

			if ((TRUE == pTempCurrentInPort->bIsBufferReinitPending) &&
					(unDesiredNumAfeFrames != pTempCurrentInPort->unNewNumAfeFrames )
			)
			{
				pTempCurrentInPort->unNewNumAfeFrames = unDesiredNumAfeFrames;

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu correcting i/p port %u  and bIsBufferReinitPending is %u & newNumAfeFrames is %lu ",
						me->mtMxID,unTempInpPortId, pTempCurrentInPort->bIsBufferReinitPending ,unDesiredNumAfeFrames );
			}
		}

		/**
		 * Now loop thru all the connected output ports that this input port is feeding to and
		 * set the output port's bIsBufferReinitPending flag to TRUE for those output ports whose current
		 * number of afe frames is not same as desired number of afe frames and there is no pending buffer
		 * reinitialization. For output ports whose bIsBufferReinitPending is already TRUE, there is
		 * no need to check again, since it might have already being checked and updated before.
		 */
		//        if (TRUE == pCurrentInPort->bIsBufferReinitPending )
		//        {
		uint32_t currConnOutputPorts = pCurrentInPort->strMask;
		uint16_t unTempOutPortId;
		MatrixOutPortInfoType *pTempCurrentOutPort;
		while(currConnOutputPorts)
		{
			unTempOutPortId = Q6_R_ct0_R(currConnOutputPorts);
			currConnOutputPorts ^= (1 << unTempOutPortId);
			pTempCurrentOutPort = me->outPortParams[unTempOutPortId];

			if ((FALSE == pTempCurrentOutPort->bIsBufferReinitPending) &&
					(unDesiredNumAfeFrames != pTempCurrentOutPort->unNumAfeFrames )
			)
			{
				pTempCurrentOutPort->bIsBufferReinitPending = TRUE;
				pTempCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu detected o/p port %u  and bIsBufferReinitPending is %u & newNumAfeFrames is %lu ",
						me->mtMxID,unTempOutPortId, pTempCurrentOutPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
			}

			if ((TRUE == pTempCurrentOutPort->bIsBufferReinitPending) &&
					(unDesiredNumAfeFrames != pTempCurrentOutPort->unNewNumAfeFrames )
			)
			{
				pTempCurrentOutPort->unNewNumAfeFrames = unDesiredNumAfeFrames;

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu correcting o/p port %u  and bIsBufferReinitPending is %u & newNumAfeFrames is %lu ",
						me->mtMxID,unTempOutPortId, pTempCurrentOutPort->bIsBufferReinitPending,unDesiredNumAfeFrames );
			}
		}
		//}
	}
}


bool_t MtMx_CheckforMixedConxInConnectedInpOutPorts(This_t *me,uint16_t unInpPortId, uint16_t unOutPortId)
{
	MatrixInPortInfoType *pCurrentInPort;
	MatrixOutPortInfoType *pCurrentOutPort;
	bool_t bFoundMixedConx =FALSE;

	pCurrentInPort = me->inPortParams[unInpPortId];
	pCurrentOutPort = me->outPortParams[unOutPortId];

	/**
	 * check if mixed latency conx exist between each i/p o/p port pair,
	 * for all the inputs feeding to this o/p port
	 */

	uint32_t currConnInputPorts = pCurrentOutPort->inPortsMask;
	uint16_t unTempInpPortId;
	MatrixInPortInfoType *pTempCurrentInPort;
	while(currConnInputPorts)
	{
		unTempInpPortId = Q6_R_ct0_R(currConnInputPorts);
		currConnInputPorts ^= (1 << unTempInpPortId);
		pTempCurrentInPort = me->inPortParams[unTempInpPortId];

		if (pTempCurrentInPort->unPortPerfMode != pCurrentOutPort->unPortPerfMode )
		{
			bFoundMixedConx = TRUE;
			return bFoundMixedConx;
		}
	}

	/**
	 * check if mixed latency conx exist between each i/p o/p port pair,
	 * for all the o/p ports that this input port is feeding to
	 */

	uint32_t currConnOutputPorts = pCurrentInPort->strMask;
	uint16_t unTempOutPortId;
	MatrixOutPortInfoType *pTempCurrentOutPort;
	while(currConnOutputPorts)
	{
		unTempOutPortId = Q6_R_ct0_R(currConnOutputPorts);
		currConnOutputPorts ^= (1 << unTempOutPortId);
		pTempCurrentOutPort = me->outPortParams[unTempOutPortId];

		if (pTempCurrentOutPort->unPortPerfMode != pCurrentInPort->unPortPerfMode )
		{
			bFoundMixedConx = TRUE;
			return bFoundMixedConx;
		}
	}
	return bFoundMixedConx;
}


ADSPResult MtMx_ReInitOutputPortBuffers(This_t *me, uint16_t unOutPortID)
{
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	ADSPResult result = ADSP_EOK;

	//Reiniting is needed only for output ports which has pending Buffer Reinit
	if (TRUE == pCurrentOutPort->bIsBufferReinitPending && (pCurrentOutPort->unNewNumAfeFrames != pCurrentOutPort->unNumAfeFrames) )
	{
		//Store the current o/p port cfg. Only store it if the o/p port is not in Inactive state.
		//If in Inactive State, dont overwrite previous state.
		if(OUTPUT_PORT_STATE_INACTIVE != pCurrentOutPort->outPortState)
		{
			MtMx_OpPortReCfg_StoreCurrentCfg(me, unOutPortID, pCurrentOutPort);
		}

		//Mark this o/p port state as inactive
		pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_INACTIVE;

		//Since this port state has changed to inactive, need to update ActiveOutPortsMask
		MtMx_OutPortToUpdateActiveOutPortsMasks(me, unOutPortID);

		//Overwrite the current number of afe frames with the desired number of afe frames
		MtMx_SetOutPortFrameDuration(me, unOutPortID);

		//In addition, if no buffer has been delivered out of this o/p port yet, reconfig right away
		if(0 == pCurrentOutPort->numBufSent)
		{
			//At this point, pCurrentOutPort->bIsOutPortPendingReleaseBuffers is FALSE
			while(0 < pCurrentOutPort->unNumOutputBufsPendingRelease)
			{
				result = MtMx_OpPortPopBuffer(me, unOutPortID);
				if(ADSP_FAILED(result))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %u Error MtMx_OutPortPopBuffer Result = %d",
							me->mtMxID, unOutPortID, result);
					goto __bailoutMtMx_ReInitOutputPortBuffers;
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
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %u Error in recfg, Result = %d",
					me->mtMxID, unOutPortID, result);
			MtMx_ForceDisableOutputPortInBadState(me, unOutPortID);
		}

		__bailoutMtMx_ReInitOutputPortBuffers:
		pCurrentOutPort->bIsBufferReinitPending = FALSE;
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Reinited output port [%u] with status %d", me->mtMxID, unOutPortID, (int)result);
		return result;
	}
	else
	{
		//Reset the bIsBufferReinitPending to false since current number of afe frames and new number of afe frames is same
		pCurrentOutPort->bIsBufferReinitPending = FALSE;
	}
	return result;
}

ADSPResult MtMx_ReInitInputPortBuffers(This_t *me, uint16_t unInPortID)
{
	MatrixInPortInfoType *pCurrentInPort;

	/* Access the i/p port's structure */
	pCurrentInPort = me->inPortParams[unInPortID];

	/* reiniting is needed only for input ports which has pending Buffer Reinit */
	if (TRUE == pCurrentInPort->bIsBufferReinitPending && (pCurrentInPort->unNewNumAfeFrames != pCurrentInPort->unNumAfeFrames) )
	{
		if (INPUT_PORT_STATE_ACTIVE <= pCurrentInPort->inPortState )
		{
			/* For input ports which has received media type already,
			 * needs to free its local buffers and reconfig to the desired buffer size.
			 * */
			/**
			 * Regardless of whether input port local buff has pcm data or not,
			 * for the phase 1, the input data is going to be dropped. This might
			 *  cause glitch to an existing session that is getting reconfigured.
			 *  in phase 2, we will try to fix the glitch issue.
			 */
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Reiniting input port [%u]", me->mtMxID, unInPortID);

			//remove input port from the wait mask to prevent firing
			MtMx_RemoveInputPortFromWaitMask(me,unInPortID);

			/* Free the local buffer memory */
			if(pCurrentInPort->pStartLoc)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Freeing pStartLoc 0x%p", me->mtMxID, pCurrentInPort->pStartLoc);
				MTMX_ALIGNED_FREE(pCurrentInPort->pStartLoc);
			}

			//make the current frame duration as the desired frame duration
			MtMx_SetInPortFrameDuration(me, unInPortID);

			/* Malloc for the new inport local buffers */
			pCurrentInPort->pStartLoc = (int8_t*)qurt_elite_memory_aligned_malloc(pCurrentInPort->unBytesPerSample * pCurrentInPort->unInPortBufSize,
					8, MTMX_SVC_INPORT_LOCAL_BUF);
			/* Check if malloc failed */
			if (!pCurrentInPort->pStartLoc)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %u failed to alloc mem for local buf", me->mtMxID, unInPortID);
				return ADSP_ENOMEMORY;
			}
			else
			{
				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %u pStartLoc: 0x%p, Size: %lu",
						me->mtMxID, unInPortID, pCurrentInPort->pStartLoc, pCurrentInPort->unBytesPerSample * pCurrentInPort->unInPortBufSize);
			}

			if(pCurrentInPort->bIsPortLive)
			{
				//TX matrix i/p port has to report pre-roll buffering delay for AV-Sync purposes.
				if(ADM_MATRIX_ID_AUDIO_TX == me->mtMxID && NULL != pCurrentInPort->punMtMxInDelay)
				{
					*(pCurrentInPort->punMtMxInDelay) = 0;
					MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %u: FrameSize(us) %lu, TotDelay (micro-sec) %lu",
							me->mtMxID, unInPortID, pCurrentInPort->unFrameDurationInUsec.int_part, *(pCurrentInPort->punMtMxInDelay));
				}
			}

			/* Reset number of samples required to fill local buffer to buffer size. */
			pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;
			MtMx_InitInPortLocalBufPtrs(me, unInPortID);

			pCurrentInPort->bIsLocalBufFull = FALSE;
			pCurrentInPort->bIsLocalBufEmpty = TRUE;

			//Check if PSPD libraries needs to be re-init.
			MtMx_InPortToCheckReInitPspdLibraries(me, unInPortID);

			//Since accumulation step is skipped, check if the input buffer is held, and release the input buffer.
			if (INPUT_BUFFER_HELD == pCurrentInPort->bInBufStatus)
			{
				// release the input buffer to its queue.
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu MtMx_ReInitInputPortBuffers: i/p port %u releasing an i/p buffer it was holding on to",
						me->mtMxID, unInPortID);

				//Return the buffer and add dataQ to the wait mask
				if (ADSP_FAILED(elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to release i/p buffer back to upstr svc!");
				}
				pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
				pCurrentInPort->numBufReturned++;

				if(INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER == pCurrentInPort->inPortState)
				{
					//I/p port transition back to ACTIVE
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu i/p port %u forcibly transitioned from Held state back to Active state", me->mtMxID, unInPortID);
					pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE;
					pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE;
					(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
					MtMx_InPortToUpdateWaitMask(me, unInPortID);
				}
			}

			//Add input port to the wait mask
			MtMx_AddInputPortToWaitMask(me,unInPortID);

#ifdef MT_MX_EXTRA_DEBUG
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu i/p port %lu, #Buf Released: %lu, i/p port added to wait mask",
					me->mtMxID, unInPortID, pCurrentInPort->numBufReturned);
#endif

		}
		else
		{
			/**
			 * For input port which did not receive media type until now,
			 * just set the current frame duration same as desired frame duration.  */

			//make the current frame duration as the desired frame duration
			MtMx_SetInPortFrameDuration(me, unInPortID);
		}

		//reset the BufferREinitPending flag.
		pCurrentInPort->bIsBufferReinitPending = FALSE;

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Reinited input port [%d] ", me->mtMxID, unInPortID);
	}
	else
	{
		//reset the BufferREinitPending flag, since number of afe frames and new number of afe frames is same
		pCurrentInPort->bIsBufferReinitPending = FALSE;
	}

	return ADSP_EOK;
}

void MtMx_OpPortReCfg_StoreCurrentCfg(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	//Remember the o/p port state (prior to reconfig).
	pCurrentOutPort->outPortStatusPriorToReconfig = pCurrentOutPort->outPortState;

	//First, store the current #channels, channel mapping and bytes/sample information.
	pCurrentOutPort->unBytesPerSamplePrRecfg = pCurrentOutPort->unBytesPerSample;
	pCurrentOutPort->unNumChannelsPrRecfg = pCurrentOutPort->unNumChannels;
	pCurrentOutPort->unSampleRatePrRecfg = pCurrentOutPort->unSampleRate;

	for(uint32_t i = 0; i < pCurrentOutPort->unNumChannels; i++)
	{
		pCurrentOutPort->usChannelMappingPrRecfg[i] = pCurrentOutPort->unChannelMapping[i];
	}
}

void MxAr_OpPortReCfg_PostSteps(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	//Update the o/p port state to what it was prior to Reconfig
	pCurrentOutPort->outPortState = pCurrentOutPort->outPortStatusPriorToReconfig;

	//Update some o/p port vars and masks
	pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = FALSE;
	pCurrentOutPort->accInPortsMask = 0;
	MxAt_OutPortToUpdateAccBufAvailabilityMask(me, unOutPortID);
	if(OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState)
	{
		{
			uint32_t unOutPortIDIndex,unOutPortIDChannelBitNo;
			unOutPortIDChannelBitNo = qurt_elite_queue_get_channel_bit(pCurrentOutPort->bufQ);

			unOutPortIDIndex = 31 - (Q6_R_cl0_R( (unOutPortIDChannelBitNo)) );
			me->outPortIDMap[unOutPortIDIndex] = unOutPortID;
		}

		MtMx_AddOutputPortToWaitMask(me, unOutPortID);
		MtMx_OutPortToUpdateActiveOutPortsMasks(me, unOutPortID);
		MtMx_OutPortToUpdateWaitMask(me, unOutPortID);
	}

	if(FALSE == MtMx_ChannelNativityIsEnabled(pCurrentOutPort->unNativeModeFlags))
	{
		//Check if PSPD libraries needs to be re-init.
		MtMx_OutPortToCheckReInitPspdLibraries(me, unOutPortID);
	}
	else
	{
		//Re-init PSPD buffers.
		MtMx_OutPortCallPspdReInitBuffers(me, unOutPortID);
	}
}

void MxAt_OpPortReCfg_PostSteps(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	//Update the o/p port state to what it was prior to Reconfig and some o/p port vars.
	pCurrentOutPort->outPortState = pCurrentOutPort->outPortStatusPriorToReconfig;

	//Update some o/p port vars and masks
	pCurrentOutPort->bIsOutPortWaitingForRscsToSendOut = FALSE;
	pCurrentOutPort->accInPortsMask = 0;
	if(FALSE == pCurrentOutPort->bIsOutPortDelivTimeSyncDataWithAbsoluteTS)
	{
		mt_mx_clear_time(&pCurrentOutPort->llSessionTime);
	}
	MxAt_OutPortToUpdateAccBufAvailabilityMask(me, unOutPortID);
	if(OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState)
	{
		//If there are pending accumulations, wake up the o/p port
		if((pCurrentOutPort->inPortsMask) && (pCurrentOutPort->inPortsWaitingToAccMask == pCurrentOutPort->inPortsMask))
		{
			MtMx_AddOutputPortToWaitMask(me, unOutPortID);
		}
		//Update the outPortIDMap
		{
			uint32_t unOutPortIDIndex,unOutPortIDChannelBitNo;
			unOutPortIDChannelBitNo = qurt_elite_queue_get_channel_bit(pCurrentOutPort->bufQ);

			unOutPortIDIndex = 31 - (Q6_R_cl0_R( (unOutPortIDChannelBitNo)) );
			me->outPortIDMap[unOutPortIDIndex] = unOutPortID;
		}
		//Update some masks
		MtMx_OutPortToUpdateActiveOutPortsMasks(me, unOutPortID);
		MxAt_OutPortToUpdateOutputReqPendingMasks(me, unOutPortID);
		MtMx_OutPortToUpdateWaitMask(me, unOutPortID);
	}

	if(FALSE == MtMx_ChannelNativityIsEnabled(pCurrentOutPort->unNativeModeFlags))
	{
		//Check if PSPD libraries needs to be re-init.
		MtMx_OutPortToCheckReInitPspdLibraries(me, unOutPortID);
	}
	else
	{
		//Re-init PSPD buffers.
		MtMx_OutPortCallPspdReInitBuffers(me, unOutPortID);
	}
}

ADSPResult MtMx_OpPortReCfg_CheckAndSendMT(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	bool_t bShouldMediaTypeMsgBeSentDS = TRUE;
	ADSPResult result = ADSP_EOK;

	//If nothing has changed, do not send the Media Type Message downstream
	if((pCurrentOutPort->unNumChannels == pCurrentOutPort->unNumChannelsPrRecfg) &&
			(pCurrentOutPort->unBytesPerSample == pCurrentOutPort->unBytesPerSamplePrRecfg) &&
			(pCurrentOutPort->unSampleRate == pCurrentOutPort->unSampleRatePrRecfg))
	{
		bShouldMediaTypeMsgBeSentDS = FALSE;
		for(uint32_t i = 0; i < pCurrentOutPort->unNumChannels; i++)
		{
			if(pCurrentOutPort->unChannelMapping[i] != pCurrentOutPort->usChannelMappingPrRecfg[i])
			{
				bShouldMediaTypeMsgBeSentDS = TRUE;
				break;
			}
		}
	}

	if(TRUE == bShouldMediaTypeMsgBeSentDS)
	{
		//Create and Send Media Type Message downstream
		if (ADSP_FAILED(result = MtMx_OutportSendMediaTypeMsgDS(me, unOutPortID, pCurrentOutPort)))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: O/p port %lu trying to send Media Type Message downstream failed! Result = %d",
					me->mtMxID, unOutPortID, result);
		}
    
		if((NULL != pCurrentOutPort->structLimiter.pLimiterCapiV2) && (pCurrentOutPort->structLimiter.bIsLimiterEnable))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: send mediafmt update to limiter on O/p port %lu ",
					me->mtMxID, unOutPortID);
			MtMx_OutPortLimiterSetMediafmt(me,unOutPortID);
		}
	}

	return result;
}

ADSPResult MtMx_SetReqKppsAndBW(This_t *me)
{
	ADSPResult result = ADSP_EOK;
	uint32_t unOutPortId = 0, unInPortId = 0, currConnInputPorts = 0, currConnOutputPorts = 0;
	uint32_t unBaseKpps = 0, unMtMxKpps = 0, unNumStereoCh = 0;
	MatrixOutPortInfoType *pCurrentOutPort = NULL;
	MatrixInPortInfoType *pCurrentInPort = NULL;
	mt_mx_struct_pspd_t *pCurrentPspd = NULL;
	mt_mx_struct_limiter_t *pLimiter = NULL;
	uint32_t unBandWidth=0;

	//Store prev KPPS and BW
	uint32_t unPrevKpps = me->unKpps;
	uint32_t unPrevBw = me->unBandWidth;

	//Initialize new KPPS and BW
	me->unKpps = 0;
	me->unBandWidth = 0;

	// Determine MtMx type, calculating the KPPS as appropriate
	if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		// For MXAR, loop through all the output ports to determine contribution
		for (unOutPortId = 0; unOutPortId <= me->maxOutPortID; unOutPortId++)
		{
			pCurrentOutPort = me->outPortParams[unOutPortId];

			/* Check if the pointer to the current output port required KPPS is set and that
			 * the output port is active. Otherwise, just move on... */
			if ((NULL != pCurrentOutPort) && (OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState))
			{
				// Ensure all variables are zeroed
				unBaseKpps = 0; unMtMxKpps = 0;
				pLimiter = &(pCurrentOutPort->structLimiter);

				// Determine the calculation parameters depending on sampling rate, latency, and bit width
				if (pCurrentOutPort->unSampleRate <= MT_MX_SAMPLING_RATE_48000)
				{
					if  (ADM_LEGACY_DEVICE_SESSION == pCurrentOutPort->unPortPerfMode)
					{
						if (MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentOutPort->unBytesPerSample)
						{
							unBaseKpps = MTMX_KPPS_48K_STEREO_16BIT_LEG_LAT;
						}
						else
						{
							unBaseKpps = MTMX_KPPS_48K_STEREO_24BIT_LEG_LAT;
						}
					}
					else
					{
						if (MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentOutPort->unBytesPerSample)
						{
							unBaseKpps = MTMX_KPPS_48K_STEREO_16BIT_LOW_LAT;
						}
						else
						{
							unBaseKpps = MTMX_KPPS_48K_STEREO_24BIT_LOW_LAT;
						}
					}
				}
				else
				{
					if  (ADM_LEGACY_DEVICE_SESSION == pCurrentOutPort->unPortPerfMode)
					{
						if (MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentOutPort->unBytesPerSample)
						{
							unBaseKpps = MTMX_KPPS_192K_STEREO_16BIT_LEG_LAT;
						}
						else
						{
							unBaseKpps = MTMX_KPPS_192K_STEREO_24BIT_LEG_LAT;
						}
					}
					else
					{
						if (MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentOutPort->unBytesPerSample)
						{
							unBaseKpps = MTMX_KPPS_192K_STEREO_16BIT_LOW_LAT;
						}
						else
						{
							unBaseKpps = MTMX_KPPS_192K_STEREO_24BIT_LOW_LAT;
						}
					}
				}

				// Round the number of stereo channels to the next highest number of stereo channels
				unNumStereoCh = pCurrentOutPort->unNumChannels;
				unNumStereoCh = ((unNumStereoCh + 1) >> 1);
				unMtMxKpps = unNumStereoCh * unBaseKpps;

				// update PSPD KPPS requirement of connected PSPDs
				uint32_t unNumConnInputPorts = 0;
				currConnInputPorts = pCurrentOutPort->inPortsMask;
				while (currConnInputPorts)
				{
					unInPortId = Q6_R_ct0_R(currConnInputPorts);
					currConnInputPorts ^= (1 << unInPortId);
					pCurrentInPort = me->inPortParams[unInPortId];
					pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortId]);
					unMtMxKpps += pCurrentPspd->unPspdKpps;
					unBandWidth += pCurrentPspd->unPspdBw;
					unNumConnInputPorts++;
				}
				//To include KPPS and BW contributions from OutPort limiter instance if exist
				if(NULL != pLimiter->pLimiterCapiV2)
				{
					unMtMxKpps += pLimiter->unLimiterKpps;
					unBandWidth += pLimiter->unLimiterBw;
				}

				//BW calculation
				uint32_t unSamplesPerSec = (pCurrentOutPort->unSampleRate)?(pCurrentOutPort->unSampleRate):(MT_MX_SAMPLING_RATE_48000);
				uint32_t unBytesPerSample = (pCurrentOutPort->unBytesPerSample)?(pCurrentOutPort->unBytesPerSample):(MT_MX_DEFAULT_BYTES_PER_SAMPLE);
				uint32_t unNumChannels = (pCurrentOutPort->unNumChannels)?(pCurrentOutPort->unNumChannels):(NUM_CHANNELS_STEREO);
				uint32_t unBandWidthTemp = unSamplesPerSec * unBytesPerSample * unNumChannels;
				if (unSamplesPerSec>MT_MX_SAMPLING_RATE_48000)
				{
					unBandWidthTemp += 2*1024*1024;
				}
				if (unNumChannels>=6)
				{
					unBandWidthTemp += 2*1024*1024;
				}
				//To include BW contributions from input ports, multiply this bandwidth vote by number of connected input ports, also add output port contribution.
				unBandWidth += unBandWidthTemp+(unBandWidthTemp*unNumConnInputPorts);

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu: MPPS: %lu, BW: %lu", me->mtMxID, unOutPortId, unMtMxKpps, unBandWidth);

				// Store the required KPPS and BW into the matrix's requirement
				me->unKpps += unMtMxKpps;
				me->unBandWidth += unBandWidth;
			}
		}
	}
	else if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
	{
		// For the MXAT, loop through all the input ports and calculate the contribution
		for (unInPortId = 0; unInPortId <= me->maxInPortID; unInPortId++)
		{
			pCurrentInPort = me->inPortParams[unInPortId];
			if ((NULL != pCurrentInPort) && (INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState))
			{
				// Ensure all variables are zeroed
				unBaseKpps = 0; unMtMxKpps = 0;

				// Determine the calculation parameters depending on sampling rate, latency, and bit width
				if (pCurrentInPort->unSampleRate <= MT_MX_SAMPLING_RATE_48000)
				{
					if  (ADM_LEGACY_DEVICE_SESSION == pCurrentInPort->unPortPerfMode)
					{
						if (MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentInPort->unBytesPerSample)
						{
							unBaseKpps = MTMX_KPPS_48K_STEREO_16BIT_LEG_LAT;
						}
						else
						{
							unBaseKpps = MTMX_KPPS_48K_STEREO_24BIT_LEG_LAT;
						}
					}
					else
					{
						if (MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentInPort->unBytesPerSample)
						{
							unBaseKpps = MTMX_KPPS_48K_STEREO_16BIT_LOW_LAT;
						}
						else
						{
							unBaseKpps = MTMX_KPPS_48K_STEREO_24BIT_LOW_LAT;
						}
					}
				}
				else
				{
					if  (ADM_LEGACY_DEVICE_SESSION == pCurrentInPort->unPortPerfMode)
					{
						if (MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentInPort->unBytesPerSample)
						{
							unBaseKpps = MTMX_KPPS_192K_STEREO_16BIT_LEG_LAT;
						}
						else
						{
							unBaseKpps = MTMX_KPPS_192K_STEREO_24BIT_LEG_LAT;
						}
					}
					else
					{
						if (MT_MX_BYTES_PER_SAMPLE_TWO == pCurrentInPort->unBytesPerSample)
						{
							unBaseKpps = MTMX_KPPS_192K_STEREO_16BIT_LOW_LAT;
						}
						else
						{
							unBaseKpps = MTMX_KPPS_192K_STEREO_24BIT_LOW_LAT;
						}
					}
				}

				// Round the number of stereo channels to the next highest number of stereo channels
				unNumStereoCh = pCurrentInPort->unNumChannels;
				unNumStereoCh = ((unNumStereoCh + 1) >> 1);
				unMtMxKpps = unNumStereoCh * unBaseKpps;

				// Count the number of output ports connected to this input port to get the number of attached streams
				uint32_t unNumConnOutputPorts = 0;
				currConnOutputPorts = pCurrentInPort->strMask;
				while (currConnOutputPorts)
				{
					unOutPortId = Q6_R_ct0_R(currConnOutputPorts);
					currConnOutputPorts ^= (1 << unOutPortId);
					pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortId]);
					unMtMxKpps += pCurrentPspd->unPspdKpps;
					unBandWidth += pCurrentPspd->unPspdBw;
					unNumConnOutputPorts++;
				}

				//BW calculation
				uint32_t unSamplesPerSec = (pCurrentInPort->unSampleRate)?(pCurrentInPort->unSampleRate):(MT_MX_SAMPLING_RATE_48000);
				uint32_t unBytesPerSample = (pCurrentInPort->unBytesPerSample)?(pCurrentInPort->unBytesPerSample):(MT_MX_DEFAULT_BYTES_PER_SAMPLE);
				uint32_t unNumChannels = (pCurrentInPort->unNumChannels)?(pCurrentInPort->unNumChannels):(NUM_CHANNELS_STEREO);
				uint32_t unBandWidthTemp = unSamplesPerSec * unBytesPerSample * unNumChannels;
				if (unSamplesPerSec>MT_MX_SAMPLING_RATE_48000)
				{
					unBandWidthTemp += 2*1024*1024;
				}
				if (unNumChannels>=6)
				{
					unBandWidthTemp += 2*1024*1024;
				}
				//To include BW contributions from input ports, multiply this bandwidth vote by number of connected input ports, also add output port contribution.
				unBandWidth += unBandWidthTemp+(unBandWidthTemp*unNumConnOutputPorts);

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu: MPPS: %lu, BW: %lu", me->mtMxID, unInPortId, unMtMxKpps, unBandWidth);

				// Store the required KPPS and BW into the matrix's requirement
				me->unKpps += unMtMxKpps;
				me->unBandWidth += unBandWidth;
			}
		}
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx: #%lu: Invalid MtMx type received. Cannot calculate KPPS requirement!", me->mtMxID);
		result = ADSP_EUNEXPECTED;
	}

	MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Vote [MPPS, BW]:  New [%lu, %lu], Prev [%lu, %lu]", me->mtMxID, me->unKpps, me->unBandWidth, unPrevKpps, unPrevBw);

	if((me->unKpps != unPrevKpps) || (me->unBandWidth != unPrevBw))
	{
		bool_t bKppsVoteNeeded = FALSE;
		bool_t bBwVoteNeeded = FALSE;

		if(me->unKpps != unPrevKpps)
		{
			bKppsVoteNeeded = TRUE;
		}

		if(me->unBandWidth != unPrevBw)
		{
			bBwVoteNeeded = TRUE;
		}

		result = MtMx_RaiseAdsppmKppsAndBwUpdateEvent(me, bKppsVoteNeeded, bBwVoteNeeded);
		return result;
	}

	return result;
}

bool_t MtMx_OpPortReCfg_CheckIfReCfgNeeded(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	bool_t bIsRecfgNeeded = TRUE;

	if((pCurrentOutPort->unNumChannels == pCurrentOutPort->unNumChannelsPrRecfg) &&
			(pCurrentOutPort->unBytesPerSample == pCurrentOutPort->unBytesPerSamplePrRecfg) &&
			(pCurrentOutPort->unSampleRate == pCurrentOutPort->unSampleRatePrRecfg))
	{
		bIsRecfgNeeded = FALSE;
	}

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, o/p port ID: %lu, MtMx_OpPortReCfg_CheckIfReCfgNeeded, returns %d",
			me->mtMxID, unOutPortID, bIsRecfgNeeded);

	return bIsRecfgNeeded;
}

void MtMx_ForceDisableOutputPortInBadState(This_t *me, uint32_t unOutPortID)
{
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu is in a bad state. Force disabling this port", me->mtMxID, unOutPortID);

	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];

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

void MtMx_ScanOutputPortsForInputsWithValidTimestamps(This_t *me)
{
	MatrixInPortInfoType *pCurrInPort_temp = NULL;
	MatrixOutPortInfoType *pCurrentOutPort = NULL;
	uint32_t unInPortId_temp = 0;
	uint32_t currConnInPorts = 0;

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx: Scanning output ports for connected input ports with valid timestamps...");

	// Loop through all the output ports
	for (uint32_t unOutPortId = 0; unOutPortId <= me->maxOutPortID; unOutPortId++)
	{
		pCurrentOutPort = me->outPortParams[unOutPortId];

		if ( (NULL != pCurrentOutPort) && (OUTPUT_PORT_STATE_ACTIVE == pCurrentOutPort->outPortState) )
		{
			// On every scan reset the state back to MT_MX_INVALID_PORT_ID
			pCurrentOutPort->unConnInPortIdWithValidTs = MT_MX_INVALID_PORT_ID;

			// retrieve connected input port mask
			currConnInPorts = pCurrentOutPort->inPortsMask;

			// iterate through the connected input ports
			while (currConnInPorts)
			{
				unInPortId_temp = Q6_R_ct0_R(currConnInPorts);
				currConnInPorts ^= (1 << unInPortId_temp);
				pCurrInPort_temp = me->inPortParams[unInPortId_temp];

				// Store the ID of the first input with a valid timestamp and break out of the while loop
				// Since the initialized state of bIsTimeStampValid is TRUE at least one input buffer must
				// have been received before we may determine the validity of the content.
				if (pCurrInPort_temp->bIsTimeStampValid && pCurrInPort_temp->numBufRcvd >= 1)
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx: output port %lu found connected input port %lu with a valid timestamp",
							unOutPortId, unInPortId_temp);
					pCurrentOutPort->unConnInPortIdWithValidTs = unInPortId_temp;
					break;
				}
			} // end while
		} // end if
	} // end for
}

void MtMx_SetOutputBufferTimestamp(This_t *me, uint32_t unOutPortID, elite_msg_data_buffer_t *pOutputBuf)
{
	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];
	MatrixInPortInfoType *pCurrentInPort = NULL;

	if (pOutputBuf == NULL)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_SetOutputBufferTimestamp requires a valid output buffer pointer!");
		return;
	}

	// If we have an input with a valid TS, and the input port ID is within bounds, copy the timestamp to the output buffer.
	if ((pCurrentOutPort->unConnInPortIdWithValidTs != MT_MX_INVALID_PORT_ID) && (pCurrentOutPort->unConnInPortIdWithValidTs < MT_MX_MAX_INPUT_PORTS))
	{
		pCurrentInPort = me->inPortParams[pCurrentOutPort->unConnInPortIdWithValidTs];
		if (pCurrentInPort == NULL)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_SetOutputBufferTimestamp could not retrieve a reference to i/p port %lu", pCurrentOutPort->unConnInPortIdWithValidTs);
			return;
		}

		pOutputBuf->ullTimeStamp = pCurrentInPort->ullTimeStampAtAccumulation.int_part;
		pOutputBuf->nFlag = SET_DATA_BUFFER_TIME_STAMP_BIT(pOutputBuf->nFlag);

		/* Please do not delete this debug message, it is used for debugging purposes.
      MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu Propagating TS [%lu, %lu] from i/p port %lu",
            me->mtMxID, unOutPortID, (uint32_t)(pOutputBuf->ullTimeStamp>>32),(uint32_t)(pOutputBuf->ullTimeStamp),
            pCurrentOutPort->unConnInPortIdWithValidTs);
		 */
	} else
	{
		// We do not have a valid timestamp to propagate, send zero
		pOutputBuf->ullTimeStamp = 0;
		pOutputBuf->nFlag = CLR_DATA_BUFFER_TIME_STAMP_BIT(pOutputBuf->nFlag);
	}
}

void MtMx_CopyChannelMap(uint8_t *psSrcMap, uint8_t *psDstMap, uint32_t unNumChannels)
{
	for (uint32_t i = 0; i < unNumChannels; i++)
	{
		psDstMap[i] = (psSrcMap[i])?(psSrcMap[i]):(i+1);
	}
}

//PSPD Utility functions
ADSPResult MtMx_PspdInitChanQ(MatrixInPortInfoType *pCurrentInPort)
{
	qurt_elite_queue_t *cmd_q = NULL, *inp_q = NULL, *out_q = NULL;
	int32_t num_pspd = sizeof(pCurrentInPort->structPspd)/sizeof(pCurrentInPort->structPspd[0]);
	int32_t fail_point = 0, i = 0;
	ADSPResult result = ADSP_EOK;

	qurt_elite_channel_init(&pCurrentInPort->pspd_channel);

	//Each PSPD has 3 queues; verify that the # of bits available in the channel is sufficient to accomodate all PSPDs
	QURT_ELITE_ASSERT(3*num_pspd <= 32);
	for (i = 0; i < num_pspd; i++)
	{
		cmd_q = (qurt_elite_queue_t *)pCurrentInPort->structPspd[i].cmd_resp_q;
		result = qurt_elite_queue_init((char*)"mtmx_pspd_cmd", MTMX_NUM_PSPD_Q_ELEM, cmd_q);
		if(ADSP_EOK != result){fail_point = 3*i; goto __init_bail;}

		inp_q = (qurt_elite_queue_t *)pCurrentInPort->structPspd[i].inp_data_q;
		result = qurt_elite_queue_init((char*)"mtmx_pspd_inp", MTMX_NUM_PSPD_Q_ELEM, inp_q);
		if(ADSP_EOK != result){fail_point = 3*i+1; goto __init_bail;}

		out_q = (qurt_elite_queue_t *)pCurrentInPort->structPspd[i].out_data_q;
		result = qurt_elite_queue_init((char*)"mtmx_pspd_out", MTMX_NUM_PSPD_Q_ELEM, out_q);
		if(ADSP_EOK != result){fail_point = 3*i+2; goto __init_bail;}
	}

	fail_point = 3*i;
	for(i = 0; i < num_pspd; i++)
	{
		cmd_q = (qurt_elite_queue_t *)pCurrentInPort->structPspd[i].cmd_resp_q;
		result = qurt_elite_channel_addq(&pCurrentInPort->pspd_channel, cmd_q, 1 << (3*i));
		if(ADSP_EOK != result){goto __init_bail;}

		inp_q = (qurt_elite_queue_t *)pCurrentInPort->structPspd[i].inp_data_q;
		result = qurt_elite_channel_addq(&pCurrentInPort->pspd_channel, inp_q, 1 << (3*i+1));
		if(ADSP_EOK != result){goto __init_bail;}

		out_q = (qurt_elite_queue_t *)pCurrentInPort->structPspd[i].out_data_q;
		result = qurt_elite_channel_addq(&pCurrentInPort->pspd_channel, out_q, 1 << (3*i+2));
		if(ADSP_EOK != result){goto __init_bail;}
	}

	return ADSP_EOK;

	__init_bail:
	for(i = 0; i < fail_point/3; i++)
	{
		qurt_elite_queue_deinit((qurt_elite_queue_t *)pCurrentInPort->structPspd[i].cmd_resp_q);
		qurt_elite_queue_deinit((qurt_elite_queue_t *)pCurrentInPort->structPspd[i].inp_data_q);
		qurt_elite_queue_deinit((qurt_elite_queue_t *)pCurrentInPort->structPspd[i].out_data_q);
	}

	fail_point %= 3;
	if(0 != fail_point)
	{
		qurt_elite_queue_deinit((qurt_elite_queue_t *)pCurrentInPort->structPspd[i].cmd_resp_q);
		fail_point --;
	}

	if(0 != fail_point)
	{
		qurt_elite_queue_deinit((qurt_elite_queue_t *)pCurrentInPort->structPspd[i].inp_data_q);
		fail_point --;
	}

	qurt_elite_channel_destroy(&pCurrentInPort->pspd_channel);

	return result;
}

bool_t MtMx_PspdSvcCreated(mt_mx_struct_pspd_t *pCurrentPspd)
{
	return (pCurrentPspd->thread_param.threadId != 0 ? true : false);
}



ADSPResult MtMx_PspdProcCmd(MatrixInPortInfoType *pCurrentInPort, MatrixOutPortInfoType *pCurrentOutPort, mt_mx_struct_pspd_t *pCurrentPspd)
{
	ADSPResult  result = ADSP_EOK;
	if((NULL == pCurrentPspd->pPspdOutBuf) || (false == MtMx_PspdSvcCreated(pCurrentPspd)))
	{
		return ADSP_EFAILED;
	}
	//Pull out the data messages
	elite_msg_any_t inp_msg, out_msg;
	qurt_elite_queue_t *inp_q = (qurt_elite_queue_t *)pCurrentPspd->inp_data_q;
	qurt_elite_queue_t *out_q = (qurt_elite_queue_t *)pCurrentPspd->out_data_q;
	result  = qurt_elite_queue_pop_front(inp_q, (uint64_t *)&inp_msg);  
	result |= qurt_elite_queue_pop_front(out_q, (uint64_t *)&out_msg);
	if(ADSP_EOK != result)
	{
		return result;
	}

	elite_msg_data_buffer_v2_t *inp_buf = (elite_msg_data_buffer_v2_t *)inp_msg.pPayload;
	inp_buf->nActualSize = pCurrentInPort->unInPortPerChBufSize * pCurrentInPort->unBytesPerSample * pCurrentInPort->unNumChannels;
	elite_msg_data_buffer_v2_t *out_buf = (elite_msg_data_buffer_v2_t *)out_msg.pPayload;
	out_buf->nActualSize = 0;

	//Push the messages to pspd thread
	result  = qurt_elite_queue_push_back(pCurrentPspd->thread_param.dataQ, (uint64_t *)&inp_msg);
	result |= qurt_elite_queue_push_back(pCurrentPspd->thread_param.gpQ, (uint64_t *)&out_msg);
	if(ADSP_EOK != result)
	{
		return result;
	}

	//Set this i/p port's unCurrentWaitMask
	pCurrentInPort->unCurrentWaitMask |= (inp_q->myChannelBit | out_q->myChannelBit);

	return ADSP_EOK;
}

void MtMx_ClosingInPortToCheckDestroyPspdThreads(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort  = me->inPortParams[unInPortID];
	uint32_t                        unOutPortID;
	mt_mx_struct_pspd_t    *pCurrentPspd;
	MatrixOutPortInfoType    *pCurrentOutPort;

	for(unOutPortID = 0; unOutPortID < MT_MX_MAX_OUTPUT_PORTS; unOutPortID++)
	{
		pCurrentOutPort = me->outPortParams[unOutPortID];
		if ((NULL != pCurrentOutPort) && (!((1 << unInPortID) & (pCurrentOutPort->inPortsMask))))
		{
			pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
			if(MtMx_PspdSvcCreated(pCurrentPspd))
			{
				MtMx_TerminatePspdSvc(pCurrentPspd);
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, destroyed Pspd svc.", me->mtMxID, unInPortID, unOutPortID);
			}
		}
	}
}

void MtMx_ClosingOutPortToCheckDestroyPspdThreads(This_t *me, uint32_t unOutPortID)
{
	MatrixInPortInfoType   *pCurrentInPort;
	uint32_t                       unInPortID;
	mt_mx_struct_pspd_t   *pCurrentPspd;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if (NULL != pCurrentInPort)
		{
			if ((INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState) && (!((1 << unOutPortID) & (pCurrentInPort->strMask))))
			{
				pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
				if(MtMx_PspdSvcCreated(pCurrentPspd))
				{
					MtMx_TerminatePspdSvc(pCurrentPspd);
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, destroyed Pspd svc.", me->mtMxID, unInPortID, unOutPortID);
				}
			}
		}
	}
}

static void MtMx_TerminatePspdSvc(mt_mx_struct_pspd_t *pCurrentPspd)
{
	ADSPResult result = ADSP_EOK;
	elite_msg_any_t term_msg;
	uint32_t unPayloadSize = sizeof(elite_msg_cmd_destroy_svc_t);

	if(ADSP_EOK != (result = elite_msg_create_msg( &term_msg, &unPayloadSize, ELITE_CMD_DESTROY_SERVICE, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q, 0, NULL)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create elite msg");
		return;
	}

	result = MtMx_PspdSendAndWaitForResp(&term_msg, pCurrentPspd->thread_param.cmdQ, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q);

	if(ADSP_EOK == result)
	{
		int status = 0;
		qurt_elite_thread_join(pCurrentPspd->thread_param.threadId, &status);
		memset(&pCurrentPspd->thread_param, 0, sizeof(pCurrentPspd->thread_param));
	}

	//Empty input and output queues
	uint64_t msg = 0;
	qurt_elite_queue_pop_front((qurt_elite_queue_t *)pCurrentPspd->inp_data_q, &msg);
	qurt_elite_queue_pop_front((qurt_elite_queue_t *)pCurrentPspd->out_data_q, &msg);

	//Free up the Pspd output buffer, if it was malloced.
	if(pCurrentPspd->pPspdOutBuf)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Freeing pPspdOutBuf 0x%p", pCurrentPspd->pPspdOutBuf);
		MTMX_ALIGNED_FREE(pCurrentPspd->pPspdOutBuf);
		pCurrentPspd->unPspdOutBufSize = 0;
	}

	return;
}

ADSPResult MtMx_PspdReInitBuffers(This_t *me, uint32_t unInPortID, uint32_t unOutPortID)
{
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: MtMx_PspdReInitBuffers called on i/p port ID %lu, o/p port ID %lu", me->mtMxID, unInPortID, unOutPortID);

	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];

	if(NULL == pCurrentInPort->pStartLoc)
	{
		//If input media type is not received yet, simply return
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, MtMx_PspdReInitBuffers, pStartLoc is NULL, Returning",
				me->mtMxID, unInPortID, unOutPortID);
		return ADSP_EOK;
	}

	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];
	mt_mx_struct_pspd_t *pCurrentPspd = &(pCurrentInPort->structPspd[unOutPortID]);
	mt_mx_struct_channel_mixer_t *pCurrentInputOutputChMixer = &(pCurrentInPort->structChanMixer[unOutPortID]);

	bool_t bIsChannelMixerUsed = pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated;
	uint32_t unNewPspdBufSize = 0, unNumBytesToRead = 0;

	if (bIsChannelMixerUsed)
	{
		//Per channel buffer size: pCurrentOutPort->unOutPortPerChBufSize = pCurrentOutPort->unFrameDurationInUsec * me->unOutPortsSampleRate: Example 5 * 48 = 240
		//Total buffer size: pCurrentOutPort->unBufSize = pCurrentOutPort->unOutPortPerChBufSize * pCurrentOutPort->unNumChannels: Example 240 * c
		//Total buffer size (in bytes): unPspdOutBufSize = pCurrentInPort->unBytesPerSample * pCurrentOutPort->unBufSize: Example (2 or 4) * (240 * c)
		unNewPspdBufSize = pCurrentOutPort->unOutPortPerChBufSize * pCurrentOutPort->unNumChannels * pCurrentInPort->unBytesPerSample;
		unNumBytesToRead = pCurrentInPort->unInPortPerChBufSize * pCurrentInPort->unNumChannels  * pCurrentInPort->unBytesPerSample;
	}
	else
	{
		//Channel mixer is not used/not init yet
		unNewPspdBufSize = pCurrentInPort->unInPortPerChBufSize * pCurrentInPort->unNumChannels * pCurrentInPort->unBytesPerSample;
		unNumBytesToRead = unNewPspdBufSize;
	}

	//Empty input and output queues
	uint64_t msg1 = 0;
	(void)qurt_elite_queue_pop_front((qurt_elite_queue_t *)pCurrentPspd->inp_data_q, &msg1);
	(void)qurt_elite_queue_pop_front((qurt_elite_queue_t *)pCurrentPspd->out_data_q, &msg1);

	//If unPspdOutBufSize has changed, free up old one and allocate new memory
	if(pCurrentPspd->unPspdOutBufSize != unNewPspdBufSize)
	{
		if(pCurrentPspd->pPspdOutBuf)
		{
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, Freeing PspdOutBuf 0x%p",
					me->mtMxID, unInPortID, unOutPortID, pCurrentPspd->pPspdOutBuf);
			MTMX_ALIGNED_FREE(pCurrentPspd->pPspdOutBuf);
			pCurrentPspd->unPspdOutBufSize = 0;
		}

		pCurrentPspd->pPspdOutBuf = (int8_t *)qurt_elite_memory_aligned_malloc(unNewPspdBufSize, 8, QURT_ELITE_HEAP_DEFAULT);

		if(pCurrentPspd->pPspdOutBuf)
		{
			pCurrentPspd->unPspdOutBufSize =  unNewPspdBufSize;
			memset(pCurrentPspd->pPspdOutBuf, 0, unNewPspdBufSize);
			MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, Malloc PspdOutBuf 0x%p success, Size: %lu",
					me->mtMxID, unInPortID, unOutPortID, pCurrentPspd->pPspdOutBuf, unNewPspdBufSize);
		}
		else
		{
			//Check if the channel mixer library needs to be destroyed and remove this input port from processing any more data.
			MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, Malloc PspdOutBuf failed, Size: %lu. Removing i/p port from wait mask",
					me->mtMxID, unInPortID, unOutPortID, unNewPspdBufSize);
			pCurrentPspd->unPspdOutBufSize = 0;
			MtMx_DestroyPspdChannelMixer(me, unInPortID, unOutPortID);
			return ADSP_ENOMEMORY;
		}

		//send command to reinit PSPD thread buffers
		ADSPResult result = ADSP_EOK;
		elite_msg_any_t param_msg;
		uint32_t unPayloadSize = sizeof(EliteMsg_CustomPspdReInitBuffers);
		if(ADSP_EOK != (result = elite_msg_create_msg( &param_msg, &unPayloadSize, ELITE_CUSTOM_MSG, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q, 0, 0)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create elite msg with error = %d", result);
			return result;
		}
		EliteMsg_CustomPspdReInitBuffers *payload = (EliteMsg_CustomPspdReInitBuffers *)param_msg.pPayload;
		payload->unSecOpCode = ELITEMSG_CUSTOM_PSPD_REINIT_BUFFERS;

		//Fill up params.
		payload->num_in_ch = pCurrentInPort->unNumChannels;
		payload->num_out_ch = pCurrentInPort->unNumChannels;
		if (bIsChannelMixerUsed)
		{
			payload->num_out_ch =  pCurrentOutPort->unNumChannels;
		}
		payload->unPspdBufSize = pCurrentPspd->unPspdOutBufSize;
		//Send command to PSPD thread and wait for response.
		if (ADSP_EOK != (result = MtMx_PspdSendAndWaitForResp(&param_msg, pCurrentPspd->thread_param.cmdQ, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PSPD thread failed to re-init buffers with error = %d", result);
			return result;
		}
	}

	//Setup variables for initializing pLibInBuf and pLibOutBuf (assumption is that both pStartLoc and pPspdOutBuf is valid at this point)
	int8_t *pInputBuf = pCurrentInPort->pStartLoc;
	int8_t *pOutputBuf = pCurrentPspd->pPspdOutBuf;

	//Setup pLibInBuf
	memset(&pCurrentPspd->inp_buf, 0, sizeof(pCurrentPspd->inp_buf));
	pCurrentPspd->inp_buf.data_ptr = pInputBuf;
	pCurrentPspd->inp_buf.nMaxSize = unNumBytesToRead;
	pCurrentPspd->inp_buf.nActualSize = 0;
	pCurrentPspd->inp_buf.pResponseQ = (qurt_elite_queue_t *)pCurrentPspd->inp_data_q;

	//Setup pLibOutBuf
	memset(&pCurrentPspd->out_buf, 0, sizeof(pCurrentPspd->out_buf));
	pCurrentPspd->out_buf.data_ptr = pOutputBuf;
	pCurrentPspd->out_buf.nMaxSize = unNewPspdBufSize;
	pCurrentPspd->out_buf.nActualSize = 0;
	pCurrentPspd->out_buf.pResponseQ = (qurt_elite_queue_t *)pCurrentPspd->out_data_q;

	//Setup buffers
	elite_msg_any_t msg;
	msg.unOpCode = ELITE_DATA_BUFFER_V2;
	msg.pPayload = &pCurrentPspd->inp_buf;
	(void)qurt_elite_queue_push_back((qurt_elite_queue_t *)pCurrentPspd->inp_data_q, (uint64_t *)&msg);
	msg.pPayload = &pCurrentPspd->out_buf;
	(void)qurt_elite_queue_push_back((qurt_elite_queue_t *)pCurrentPspd->out_data_q, (uint64_t *)&msg);

	return ADSP_EOK;
}

void MtMx_OutPortCallPspdReInitBuffers(This_t *me, uint32_t unOutPortID)
{
	MatrixInPortInfoType *pCurrentInPort;
	uint32_t unInPortID;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if (NULL != pCurrentInPort)
		{
			if ((INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState) && ((1 << unOutPortID) & (pCurrentInPort->strMask)))
			{
				(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);
			}
		}
	}
}

//this func always releases msg_ptr, so caller should not release, even in case of errors.
ADSPResult MtMx_PspdSendAndWaitForResp(elite_msg_any_t *pMsg, qurt_elite_queue_t *pSendQ, qurt_elite_queue_t *pRecvQ)
{
	ADSPResult result = qurt_elite_queue_push_back(pSendQ, (uint64_t*)pMsg);
	if(ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send command with result = %d", result);
		(void) elite_msg_release_msg(pMsg);
		return result;
	}

	//wait for response
	qurt_elite_channel_t *chan = qurt_elite_queue_get_channel(pRecvQ);
	uint32_t chan_bit = qurt_elite_queue_get_channel_bit(pRecvQ);
	(void)qurt_elite_channel_wait(chan, chan_bit);

	//process response
	elite_msg_any_t pRespMsg;
	qurt_elite_queue_pop_front(pRecvQ, (uint64_t *)&pRespMsg);
	elite_msg_any_payload_t *payload = (elite_msg_any_payload_t *)pRespMsg.pPayload;
	if(ADSP_EOK != (result = payload->unResponseResult))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PSPD thread returned error = %d for command", result);
	}

	(void) elite_msg_release_msg(&pRespMsg);
	return result;
}

void MtMx_ClosingInPortToCheckDestroyPspdLibraries(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort  = me->inPortParams[unInPortID];
	uint32_t                  unOutPortID;
	mt_mx_struct_channel_mixer_t *pCurrentInputOutputChMixer;
	MatrixOutPortInfoType    *pCurrentOutPort;

	for(unOutPortID = 0; unOutPortID <= me->maxOutPortID; unOutPortID++)
	{
		pCurrentOutPort = me->outPortParams[unOutPortID];
		if ((NULL != pCurrentOutPort) && (!((1 << unInPortID) & (pCurrentOutPort->inPortsMask))))
		{
			//destroy channel mixer module
			pCurrentInputOutputChMixer = &(pCurrentInPort->structChanMixer[unOutPortID]);
			if(pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated)
			{
				MtMx_DestroyPspdChannelMixer(me, unInPortID, unOutPortID);
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, destroyed channel mixer", me->mtMxID, unInPortID, unOutPortID);
				(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);
			}
		}
	}
}

void MtMx_ClosingOutPortToCheckDestroyPspdLibraries(This_t *me, uint32_t unOutPortID)
{
	MatrixInPortInfoType *pCurrentInPort;
	uint32_t unInPortID;
	mt_mx_struct_channel_mixer_t *pCurrentInputOutputChMixer;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		pCurrentInPort = me->inPortParams[unInPortID];
		if ((NULL != pCurrentInPort) && (INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState) && (!((1 << unOutPortID) & (pCurrentInPort->strMask))))
		{
			//destroy channel mixer module
			pCurrentInputOutputChMixer = &(pCurrentInPort->structChanMixer[unOutPortID]);
			if(pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated)
			{
				MtMx_DestroyPspdChannelMixer(me, unInPortID, unOutPortID);
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, destroyed channel mixer", me->mtMxID, unInPortID, unOutPortID);
				(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);
			}
		}
	}
}

void MtMx_InPortToCheckReInitPspdLibraries(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort  = me->inPortParams[unInPortID];
	uint32_t                strMask = pCurrentInPort->strMask;
	uint32_t                unOutPortID;
	MatrixOutPortInfoType   *pCurrentOutPort;

	while (strMask)
	{
		unOutPortID = Q6_R_ct0_R(strMask);
		strMask ^= (1 << unOutPortID);
		pCurrentOutPort = me->outPortParams[unOutPortID];
		if(NULL != pCurrentOutPort && OUTPUT_PORT_STATE_INACTIVE != pCurrentOutPort->outPortState)
		{
			//Check if the channel mixer library needs to be re-init and if any setparam needs to be called on that lib for this i/p-o/p pair.
			MtMx_InOutPortsToCheckReInitChannelMixerLibrary(me, unInPortID, unOutPortID, TRUE);
			(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);

		}
	}
}

void MtMx_OutPortToCheckReInitPspdLibraries(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	uint32_t                   inPortsMask = pCurrentOutPort->inPortsMask;
	uint32_t                  unInPortID;
	MatrixInPortInfoType    *pCurrentInPort;

	while (inPortsMask)
	{
		unInPortID = Q6_R_ct0_R(inPortsMask);
		inPortsMask ^= (1 << unInPortID);
		pCurrentInPort = me->inPortParams[unInPortID];
		if(NULL != pCurrentInPort && INPUT_PORT_STATE_INACTIVE != pCurrentInPort->inPortState)
		{
			//Check if the channel mixer library needs to be re-init and if any setparam needs to be called on that lib for this i/p-o/p pair.
			MtMx_InOutPortsToCheckReInitChannelMixerLibrary(me, unInPortID, unOutPortID, TRUE);
			(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);

		}
	}
}

void MtMx_InOutPortsToCheckReInitPspdLibraries(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, bool_t bOptzCheck)
{
	//Check if the channel mixer library needs to be re-init and if any setparam needs to be called on that lib for this i/p-o/p pair.
	MtMx_InOutPortsToCheckReInitChannelMixerLibrary(me, unInPortID, unOutPortID, bOptzCheck);
	(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);

	return;
}

bool_t MtMx_CheckIfInputPortCanTransitionAwayFromReady(This_t* me, uint32_t unInPortID)
{
	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];
	uint32_t unStrMask = pCurrentInPort->strMask;
	uint32_t unOutPortID;
	MatrixOutPortInfoType *pCurrentOutPort;

	//If this port is not connected to anything yet, then yes, it can be safely transitioned away from ready
	if(0 == unStrMask)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu is not connected to anything yet and can transition away from ready state.",
				me->mtMxID, unInPortID);

		//This means that, in the future, this i/p port does not need to worry about transitioning to ready
		pCurrentInPort->bDoesInputPortNeedToTransitionAwayFromReady = FALSE;

		//It can do it right away
		return TRUE;
	}

	//If this port is connected, check to make sure all of its connected o/p ports have the same rate
	while(unStrMask)
	{
		unOutPortID = Q6_R_ct0_R(unStrMask);
		unStrMask ^= (1 << unOutPortID);
		pCurrentOutPort = me->outPortParams[unOutPortID];
		if(NULL != pCurrentOutPort && pCurrentInPort->unSampleRate != pCurrentOutPort->unSampleRate)
		{
			MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p,o/p [%lu, %lu] sample rates [%lu, %lu] mismatch. i/p port will remain in ready state.",
					me->mtMxID, unInPortID, unOutPortID, pCurrentInPort->unSampleRate, pCurrentOutPort->unSampleRate);

			//This means that, in the future, when opportune, this i/p port needs to transition away from ready.
			pCurrentInPort->bDoesInputPortNeedToTransitionAwayFromReady = TRUE;

			//Currently, it is unable to do so.
			return FALSE;
		}
	}

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu: All connected o/p ports have the same sampling rate and can transition away from ready state.",
			me->mtMxID, unInPortID);

	//This means that, in the future, this i/p port does not need to worry about transitioning to ready
	pCurrentInPort->bDoesInputPortNeedToTransitionAwayFromReady = FALSE;

	//It can do it right away
	return TRUE;
}

ADSPResult MtMx_RaiseAdsppmKppsAndBwUpdateEvent(This_t* me, bool_t bKppsVoteNeeded, bool_t bBwVoteNeeded)
{
	ADSPResult result = ADSP_EOK;

#if (ADSPPM_INTEGRATION==1)
	if ( !qurt_elite_adsppm_wrapper_is_registered(me->unAdsppmClientId) )
	{
		return ADSP_EOK;
	}

	static const uint8_t NUM_REQUEST=2;
	MmpmRscParamType rscParam[NUM_REQUEST];
	MMPM_STATUS      retStats[NUM_REQUEST];
	MmpmRscExtParamType reqParam;
	uint8_t req_num=0;
	MmpmMppsReqType mmpmMppsParam;
	MmpmGenBwValType bwReqVal;
	MmpmGenBwReqType bwReq;

	reqParam.apiType                    = MMPM_API_TYPE_SYNC;
	reqParam.pExt                       = NULL;       //for future
	reqParam.pReqArray                  = rscParam;
	reqParam.pStsArray                  = retStats;   //for most cases mmpmRes is good enough, need not check this array.
	reqParam.reqTag                     = 0;          //for async only

	if(TRUE == bKppsVoteNeeded)
	{
		uint32_t mpps = (me->unKpps+999)/1000;
		mmpmMppsParam.mppsTotal                  = mpps;
		mmpmMppsParam.adspFloorClock             = 0;

		rscParam[req_num].rscId                   = MMPM_RSC_ID_MPPS;
		rscParam[req_num].rscParam.pMppsReq       = &mmpmMppsParam;
		reqParam.numOfReq                   = 1;

		if (0 == mpps)
		{
			result = qurt_elite_adsppm_wrapper_release(me->unAdsppmClientId, &me->pAdsppmClient, &reqParam);
			if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release MPPS by RX Mtx (%lu). Result %lu",me->unAdsppmClientId, result);
			}
			else
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release MPPS by TX Mtx (%lu). Result %lu",me->unAdsppmClientId, result);
			}
		}
		else
		{
			result = qurt_elite_adsppm_wrapper_request(me->unAdsppmClientId, &me->pAdsppmClient, &reqParam);
			if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu by RX Mtx (%lu). Result %lu", mpps, me->unAdsppmClientId,result);
			}
			else
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu by TX Mtx (%lu). Result %lu", mpps, me->unAdsppmClientId,result);
			}
		}
	}

	if(TRUE == bBwVoteNeeded)
	{
		bwReqVal.busRoute.masterPort                 = MMPM_BW_PORT_ID_ADSP_MASTER;
		bwReqVal.busRoute.slavePort                  = MMPM_BW_PORT_ID_DDR_SLAVE;
		bwReqVal.bwValue.busBwValue.bwBytePerSec     = me->unBandWidth;
		bwReqVal.bwValue.busBwValue.usagePercentage  = 100;
		bwReqVal.bwValue.busBwValue.usageType        = MMPM_BW_USAGE_LPASS_DSP;

		bwReq.numOfBw            = 1;
		bwReq.pBandWidthArray    = &bwReqVal;

		rscParam[req_num].rscId                   = MMPM_RSC_ID_GENERIC_BW_EXT;
		rscParam[req_num].rscParam.pGenBwReq      = &bwReq;
		reqParam.numOfReq                   = 1;

		if (0 == me->unBandWidth)
		{
			result = qurt_elite_adsppm_wrapper_release(me->unAdsppmClientId, &me->pAdsppmClient, &reqParam);
			if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW by RX Mtx (%lu). Result %lu", me->unAdsppmClientId, result);
			}
			else
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW by TX Mtx (%lu). Result %lu", me->unAdsppmClientId, result);
			}
		}
		else
		{
			result = qurt_elite_adsppm_wrapper_request(me->unAdsppmClientId, &me->pAdsppmClient, &reqParam);
			if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request BW %lu KBPS by RX Mtx (%lu). Result %lu", me->unBandWidth/1024, me->unAdsppmClientId, result);
			}
			else
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request BW %lu KBPS by TX Mtx (%lu). Result %lu",  me->unBandWidth/1024, me->unAdsppmClientId, result);
			}
		}
	}
#endif

	return result;
}

ADSPResult MtMx_RegisterWithAdsppm(This_t* me)
{
	ADSPResult result = ADSP_EOK;

#if (ADSPPM_INTEGRATION==1)
	MmpmRegParamType regParam;
	char threadname[16];
	qurt_thread_get_name(threadname, 16-1);

	regParam.rev             = MMPM_REVISION;
	regParam.instanceId      = MMPM_CORE_INSTANCE_0;
	regParam.pwrCtrlFlag     = PWR_CTRL_NONE; //PWR_CTRL_STATIC_DISPLAY, PWR_CTRL_THERMAL
	regParam.callBackFlag    = CALLBACK_NONE; //CALLBACK_STATIC_DISPLAY, CALLBACK_THERMAL, CALLBACK_REQUEST_COMPLETE
	regParam.MMPM_Callback   = NULL;
	regParam.cbFcnStackSize  = 0;

	regParam.coreId          = MMPM_CORE_ID_LPASS_ADSP; //no need to request power, register access.
	regParam.pClientName     = threadname;

	result = qurt_elite_adsppm_wrapper_register(&regParam, &me->unAdsppmClientId, &me->pAdsppmClient);

	if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by RX Mtx. Result %lu. Client id %lu", result, me->unAdsppmClientId);
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by TX Mtx. Result %lu. Client id %lu", result, me->unAdsppmClientId);
	}
#endif

	return result;
}

void MtMx_DeRegisterFromAdsppm(This_t* me)
{
	uint32_t client_id = me->unAdsppmClientId;
	if ( !qurt_elite_adsppm_wrapper_is_registered(me->unAdsppmClientId) )
	{
		return ;
	}

	ADSPResult result = qurt_elite_adsppm_wrapper_deregister(&me->unAdsppmClientId, &me->pAdsppmClient);
	if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by RX Mtx. Result %lu. Client id %lu", result, client_id);
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by TX Mtx. Result %lu. Client id %lu", result, client_id);
	}
	return;
}

void MtMx_SetInPortFrameDuration(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];

	//If sample rate is not yet configured, assign value based on perf mode
	if (0 == pCurrentInPort->unSampleRate)
	{
		pCurrentInPort->unFrameDurationInUsec.int_part = (ADM_LEGACY_DEVICE_SESSION == pCurrentInPort->unPortPerfMode)?MT_MX_FRAME_DURATION_5000US : MT_MX_FRAME_DURATION_1000US;
		pCurrentInPort->unFrameDurationInUsec.frac_part = 0;
		return;
	}

	elite_svc_get_frame_size(pCurrentInPort->unSampleRate, &(pCurrentInPort->unAfeFrameSizeInSamples));

	//If Legacy session/Mixed Connection, force the port to operate at 5 AFE Frames
	if ( (ADM_LEGACY_DEVICE_SESSION == pCurrentInPort->unPortPerfMode) || (MT_MX_LEGACY_NUM_AFE_FRAMES == pCurrentInPort->unNewNumAfeFrames))
	{
		pCurrentInPort->unInPortPerChBufSize = (pCurrentInPort->unAfeFrameSizeInSamples)*(MT_MX_LEGACY_NUM_AFE_FRAMES);
		pCurrentInPort->unNumAfeFrames = MT_MX_LEGACY_NUM_AFE_FRAMES;
	}
	else
	{
		pCurrentInPort->unInPortPerChBufSize = (pCurrentInPort->unAfeFrameSizeInSamples)*(MT_MX_LL_NUM_AFE_FRAMES);
		pCurrentInPort->unNumAfeFrames = MT_MX_LL_NUM_AFE_FRAMES;
	}

	pCurrentInPort->unSamplesPer1Msec = pCurrentInPort->unAfeFrameSizeInSamples;
	MtMx_CalculateFrameDuration(pCurrentInPort->unInPortPerChBufSize, pCurrentInPort->unSampleRate, &(pCurrentInPort->unFrameDurationInUsec));
	pCurrentInPort->unInPortBufSize = pCurrentInPort->unInPortPerChBufSize * pCurrentInPort->unNumChannels;

	if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
	{
		// Currently the input port frame size does not affect the Tx path delay.
	}
	else
	{
		//AV-Sync
		MtMx_CalculateStreamDelay(me, unInPortID);
	}
}

void MtMx_SetOutPortFrameDuration(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unOutPortID];

	//If the Sample Rate is not yet configured, set Frameduration based on Perf Mode
	if (0 == pCurrentOutPort->unSampleRate)
	{
		pCurrentOutPort->unFrameDurationInUsec.int_part = (ADM_LEGACY_DEVICE_SESSION == pCurrentOutPort->unPortPerfMode) ? MT_MX_FRAME_DURATION_5000US : MT_MX_FRAME_DURATION_1000US;
		pCurrentOutPort->unFrameDurationInUsec.frac_part = 0;
		return;
	}
	elite_svc_get_frame_size((pCurrentOutPort->unSampleRate),&(pCurrentOutPort->unAfeFrameSizeInSamples));

	//If it is Legacy Session/Mixed Conx, force it to operate at 5 AFE Frames
	if ((ADM_LEGACY_DEVICE_SESSION == pCurrentOutPort->unPortPerfMode) || (MT_MX_LEGACY_NUM_AFE_FRAMES == pCurrentOutPort->unNewNumAfeFrames))
	{
		pCurrentOutPort->unOutPortPerChBufSize = (pCurrentOutPort->unAfeFrameSizeInSamples)*(MT_MX_LEGACY_NUM_AFE_FRAMES);
		pCurrentOutPort->unNumAfeFrames = MT_MX_LEGACY_NUM_AFE_FRAMES;
	}
	else
	{
		pCurrentOutPort->unOutPortPerChBufSize = (pCurrentOutPort->unAfeFrameSizeInSamples)*(MT_MX_LL_NUM_AFE_FRAMES);
		pCurrentOutPort->unNumAfeFrames = MT_MX_LL_NUM_AFE_FRAMES;
	}

	MtMx_CalculateFrameDuration(pCurrentOutPort->unOutPortPerChBufSize, pCurrentOutPort->unSampleRate, &pCurrentOutPort->unFrameDurationInUsec);


	if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
	{
		//AV-Sync
		MtMx_CalculateStreamDelay(me, unOutPortID);
	}
	else
	{
		// We need to update the delay for all the input ports for which this is the top prio output port.
		uint32_t unInPortID;
		uint32_t inPortsMask = pCurrentOutPort->inPortsMask;
		while (inPortsMask)
		{
			unInPortID = Q6_R_ct0_R(inPortsMask);
			inPortsMask ^= 1 << unInPortID;
			MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];

			if (unOutPortID == pCurrentInPort->unTopPrioOutPort)
			{
				//AV-Sync
				MtMx_CalculateStreamDelay(me, unInPortID);
			}
		}
	}
}

void MtMx_CalculateFrameDuration(const uint32_t frame_size, const uint32_t sampling_rate, adsp_time *frame_duration)
{
	// check that sampling rate is less than max allowed sampling rate
	if ((MT_MX_MAX_SAMPLING_RATE < sampling_rate) || (NULL == frame_duration))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx_CalculateFrameDuration rcvd invalid params!");
		return;
	}

	// in general, any quotient may be represented by an integer and fractional part: (m/n) = a + (b/n)
	// compute the integer part, a
	uint64_t a = ((uint64_t)frame_size * 1000000) / (uint64_t)sampling_rate;
	frame_duration->int_part = a;

	// compute the remainder. solving the above equation for b = m - n*a
	uint64_t b = ((uint64_t)frame_size * 1000000) - (a * (uint64_t)sampling_rate);

	// Convert b to Q19.45 and divide by n (the sampling rate in Q19.0) to get the fractional part in Q19.45
	// recall that when dividing two Q format numbers, Qa.b / Qx.y = Qa-x.b-y
	frame_duration->frac_part = (b << MTMX_TIME_Q_FACTOR) / sampling_rate;
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Frame duration int part = %llu, fractional part 0x%llx Q45",
			frame_duration->int_part, frame_duration->frac_part);
	return;
}

void MtMx_CalculateStreamDelay(This_t *me, uint32_t unPortID)
{
	if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
	{
		// For Tx matrix, the delay is equal to the max buffer size, which is one frame size here.
		MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unPortID];

		if (NULL != pCurrentOutPort->punMtMxOutDelay)
		{
			*pCurrentOutPort->punMtMxOutDelay = pCurrentOutPort->unFrameDurationInUsec.int_part;

			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Delay updated: o/p port %lu: TotDelay (micro-sec) %lu", me->mtMxID, unPortID, *(pCurrentOutPort->punMtMxOutDelay));
		}
	}
	else
	{
		// For Rx matrix, the delay is the sum of all the buffers in the path and sampleslip delay if instance has created
		MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unPortID];
		mt_mx_sampleslip_t      *pSampleSlip = &(pCurrentInPort->pInPortAvSync->structSampleSlip);

		if (NULL != pCurrentInPort->punMtMxInDelay)
		{
			uint32_t localBufferDelay = pCurrentInPort->unFrameDurationInUsec.int_part;

			uint32_t accBufDelay = 0;
			uint32_t bufDelay = 0;
			uint32_t LimiterDelay = 0;

			if (pCurrentInPort->unTopPrioOutPort < MT_MX_MAX_OUTPUT_PORTS) // Ensure that the top prio output port is set.
			{
				MatrixOutPortInfoType *pTopPrioOutPort = me->outPortParams[pCurrentInPort->unTopPrioOutPort];
				if (NULL != pTopPrioOutPort)
				{
					accBufDelay = pTopPrioOutPort->unFrameDurationInUsec.int_part;
					bufDelay = pTopPrioOutPort->unFrameDurationInUsec.int_part * pTopPrioOutPort->unNumOutputBufs;
					if(pTopPrioOutPort->structLimiter.pLimiterCapiV2 != NULL)
					{
						LimiterDelay = pTopPrioOutPort->structLimiter.unLimiterDelay;
					}
				}
			}

			*pCurrentInPort->punMtMxInDelay = localBufferDelay + accBufDelay + bufDelay + LimiterDelay;
      
			if (NULL != pSampleSlip->pSampleSlipCapiv2)
			{
				*pCurrentInPort->punMtMxInDelay += pSampleSlip->unAlgDelay;
			}
      
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Delay updated: i/p port %lu: TotDelay (micro-sec) %lu ", me->mtMxID, unPortID, *(pCurrentInPort->punMtMxInDelay));
		}
	}
}

void MtMx_AdjustBaseValueStcUpdate(This_t *me, uint32_t unInPortID, uint32_t unSampleRate)
{
	//Adjust the base value
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	pCurrentInPort->ullNumSamplesForStcUpdate = (pCurrentInPort->ullNumSamplesForStcUpdate * unSampleRate) / pCurrentInPort->unSampleRate;
}

/**
 * if respMsg == NULL, then  unInPortID & unOutPortID sent from caller will be used.
 *    if forceRaise, then unOutPortID is immaterial.
 * else, port-id in client token of respMsg will be used.
 */
ADSPResult MtMx_CheckRaiseRenderedEoSv2(This_t* me, elite_msg_any_t *respMsg, uint32_t unInPortID, uint32_t unOutPortID, bool_t forceRaise)
{
   elite_msg_data_eos_apr_t *eosPayload = NULL;
   uint32_t OutPortMask = 0;

   if (respMsg)
   {
      eosPayload = (elite_msg_data_eos_apr_t *)respMsg->pPayload;
      MtMx_GetInOutPortIdFromClientToken(eosPayload->unClientToken, &unInPortID, &unOutPortID);
   }

   OutPortMask = 1 << unOutPortID;

   MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];

   if (pCurrentInPort && (ASM_DATA_EVENT_RENDERED_EOS_V2 == pCurrentInPort->eosInfo.event_opcode))
   {
      //if we are waiting for eos-ack for this out port.
      if (forceRaise || (pCurrentInPort->eosPendingAckMask & OutPortMask))
      {
         if ( forceRaise || (ASM_BIT_EOS_V2_RAISE_IF_ANY == (pCurrentInPort->eosInfo.mask & ASM_BIT_MASK_EOS_V2_RAISE_IF_ANY_OR_ALL) ))
         {
            pCurrentInPort->eosPendingAckMask = 0;
         }
         else
         {
            pCurrentInPort->eosPendingAckMask &= ~OutPortMask;
         }

         //raise event
         if (0 == pCurrentInPort->eosPendingAckMask)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"Mtmx i/p port %lu : raising rendered EoS event. mask 0x%lx", unInPortID, pCurrentInPort->eosInfo.mask);
            elite_svc_raise_rendered_eos_v2_event( &(pCurrentInPort->eosInfo) );
            pCurrentInPort->eosInfo.event_opcode = 0;
         }
      }
   }

   if (respMsg)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"Mtmx i/p port %lu : eos release", unInPortID);
      elite_msg_release_msg(respMsg);
   }

   return ADSP_EOK;
}

/**
 * If waiting for EoS acks clear & raise
 *
 * clear held EoS variables.
 *
 *
 * if clearOnlyEoSV2-> then clearing happens only if EoS is V2.
 *
 */
ADSPResult MtMx_ClearEoSMsg(This_t* me, uint32_t unInPortID, bool_t clearOnlyEoSV2)
{
   MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];

   if (NULL == pCurrentInPort)
   {
      return ADSP_EOK; //nothing to do
   }

   if (pCurrentInPort->eosPendingAckMask)
   {
      pCurrentInPort->eosPendingAckMask = 0;
      MtMx_CheckRaiseRenderedEoSv2(me, NULL, unInPortID, MT_MX_INVALID_PORT_ID, TRUE);
   }

   if ( ( clearOnlyEoSV2 && (ASM_DATA_EVENT_RENDERED_EOS_V2 == pCurrentInPort->eosInfo.event_opcode) ) ||
         !clearOnlyEoSV2)
   {
      //if we are holding on to EoS for sending down, just release. this is because, even if we send down we will not ack based on it.
      if (TRUE == pCurrentInPort->bIsEOSHeld)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: returning held EOS", me->mtMxID, unInPortID);
         pCurrentInPort->eosPendingMask = 0;
         elite_msg_finish_msg( &(pCurrentInPort->myDataQMsg), ADSP_EOK );
         pCurrentInPort->bIsEOSHeld = FALSE;
      }

      if (pCurrentInPort->bIsGaplessEOSMsgWaitingToBeSentDS)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: returning held gapless EOS", me->mtMxID, unInPortID);
         elite_msg_finish_msg(&(pCurrentInPort->gaplessEOSmsg), ADSP_EOK);
         pCurrentInPort->bIsGaplessEOSMsgWaitingToBeSentDS = FALSE;
         pCurrentInPort->eosPendingMask = 0;
      }
   }

   return ADSP_EOK;
}

bit_conv_mode_t MtMx_IdentifyBitConvType(uint32_t unInputBW, uint32_t unOutputBW)
{
    bit_conv_mode_t eBitConvType = BIT_CONV_IN_OUT_INVALID;
    if(unInputBW == unOutputBW)
    {
        eBitConvType = BIT_CONV_IN_OUT_EQUAL;
    }
    else if(unInputBW < unOutputBW)
    {
        switch(unInputBW)
        {
            case 16:
                if(24 == unOutputBW){eBitConvType = BIT_CONV_IN16_TO_OUT24;}
                else if(32 == unOutputBW){eBitConvType = BIT_CONV_IN16_TO_OUT32;}
                break;

            case 24:
                if(32 == unOutputBW) {eBitConvType = BIT_CONV_IN24_TO_OUT32;}
                break;

            default:
                eBitConvType = BIT_CONV_IN_OUT_INVALID;
                break;
        }
    }
    else /* unInputBW > unOutputBW */
    {
        switch(unOutputBW)
        {
            case 16:
                if(24 == unInputBW) {eBitConvType = BIT_CONV_IN24_TO_OUT16;}
                else if(32 == unInputBW) {eBitConvType = BIT_CONV_IN32_TO_OUT16;}
                break;

            case 24:
                if(32 == unInputBW) {eBitConvType = BIT_CONV_IN32_TO_OUT24;}
                break;

            default:
                eBitConvType = BIT_CONV_IN_OUT_INVALID;
                break;
        }
    }
#ifdef MT_MX_EXTRA_DEBUG
	MSG_3(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"MtMx_IdentifyBitConvType : inputBW = %d outputBW = %d BitConvType = %lx ",unInputBW, unOutputBW, eBitConvType);
#endif	
	return eBitConvType;
}