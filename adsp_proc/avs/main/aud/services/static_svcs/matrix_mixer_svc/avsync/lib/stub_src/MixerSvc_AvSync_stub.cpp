/**
@file MixerSvc_AvSync_stub.cpp
@brief This file defines matrix mixer AV-Sync stubbed utilities.
 */

/*========================================================================
Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/avsync/lib/stub_src/MixerSvc_AvSync_stub.cpp#1 $

when              who   what, where, why
--------              ---      -------------------------------------------------------
06/24/2015  kr      Created file.
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc_AvSync.h"
#include "MixerSvc.h"
#include "MixerSvc_Util.h"
#include "MixerSvc_InPortHandler.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "adsp_media_fmt.h"
#include "adsp_asm_data_commands.h"
#include "adsp_adm_api.h"
#include "AudDevMgr_PrivateDefs.h"
#include "adsp_asm_session_commands.h"
#include "MixerSvc_MsgHandlers.h"
#include "MixerSvc_Util.h"
#include "EliteMsg_AdmCustom.h"
#include "MixerSvc_InPortHandler.h"
#include "adsp_mtmx_strtr_api.h"
#include "MixerSvc_OutPortHandler.h"
#include "adsp_private_api_ext.h"

ADSPResult MtMx_AddInterpolatedSamples(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_DropSamples(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_InPortToCheckReInitSampleSlipLibrary(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_InPortSetS2DPtrSampleSlipLibrary(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_InPortResetSampleSlipLibrary(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_CheckSSLibStatus_3(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_CheckSSLibStatus_4(This_t *me, uint32_t unInPortID)
{
	return ADSP_EFAILED; //Stub needs to return failure for normal operation
}

ADSPResult MxAr_AVSyncSetAFEDriftPtrInputDerivedFromOutput(This_t *me)
{
	return ADSP_EOK;
}

ADSPResult MtMx_HandleSampleAddDrops_1(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;
	return ADSP_EOK;
}

ADSPResult MtMx_HandleSampleAddDrops_2(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_HandleSampleAddDrops_3(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_ClearHoldDuration(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_MsgDataBufferHold(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	return ADSP_EOK;
}

ADSPResult MtMx_HoldCommonRoutine(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	return ADSP_EOK;
}

ADSPResult MtMx_ProcessMsgDataBuffer(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	MtMx_MsgDataBuffer(me, unInPortID, pCurrentInPort); //Process the input buffer normally
	return ADSP_EOK;
}

ADSPResult MtMx_ResetAvSyncStatsDrift(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_ResetAvSyncSampleStats(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_ResetStrClkSync(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_SetupDefaultRenderWindow(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_SetDefaultRenderWindow(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_SetDefaultRenderWindows(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_SetInOutDriftPtr(This_t *me, uint32_t unInPortID, uint32_t unOutPortID)
{
	return ADSP_EOK;
}

ADSPResult  MtMx_InitInputAvSync(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult  MtMx_DeInitInputAvSync(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_InitOutputAvSync(This_t *me, uint32_t unOutPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_DeInitOutputAvSync(This_t *me, uint32_t unOutPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_SetInputDriftPtr(This_t *me, uint32_t unInPortID, volatile const afe_drift_info_t *pAfeDriftPtr)
{
	return ADSP_EOK;
}

ADSPResult MtMx_SetOutputDriftPtr(This_t *me, uint32_t unOutPortID, volatile const afe_drift_info_t *pAfeDriftPtr)
{
	return ADSP_EOK;
}

ADSPResult MtMx_CheckIfInportMovesAnyOutportToActiveState(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MtMx_UpdateExpectedST(This_t *me, uint32_t unInPortID)
{
	return ADSP_EOK;
}

ADSPResult MxAr_InPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	return ADSP_EOK; //Stub returns OK.
}

ADSPResult MtMx_AdjustSessionClk(This_t *me, elite_msg_any_t *pMsg)
{
	ADSPResult result = ADSP_EOK; //Stub returns OK.
	elite_msg_finish_msg(pMsg, result);
	return result;
}

ADSPResult MtMx_SetRateMatchingParamHandler(This_t *me, elite_msg_any_t *pMsg)
{
	ADSPResult result = ADSP_EOK; //Stub returns OK.
	elite_msg_finish_msg(pMsg, result);
	return result;
}

ADSPResult MtMx_AvSyncReleaseHW(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType       *pCurrentInPort = me->inPortParams[unInPortID];
	ADSPResult result = ADSP_EOK;
	if (NULL != pCurrentInPort->avt_drv_handle)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Releasing AVTimer Driver for i/p port %lu",me->mtMxID, unInPortID);
		if (ADSP_EOK != (result = avtimer_drv_hw_close(pCurrentInPort->avt_drv_handle)))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to close AVTimer Driver for i/p port %lu with result: %d",unInPortID, result);
			pCurrentInPort->avt_drv_handle = NULL;
			return result;
		}
		pCurrentInPort->avt_drv_handle = NULL;
	}
	return result;
}

ADSPResult MtMx_AvSyncRequestHW(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType       *pCurrentInPort = me->inPortParams[unInPortID];
	ADSPResult result = ADSP_EOK;
	if(NULL == pCurrentInPort->avt_drv_handle)
	{
		avtimer_open_param_t open_param;
		char client_name[] = "MtMxIp";
		open_param.client_name = client_name;
		open_param.flag = 0;
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Opening AVTimer Driver for i/p port %lu",me->mtMxID, unInPortID);
		if (ADSP_EOK != (result = avtimer_drv_hw_open(&(pCurrentInPort->avt_drv_handle), &open_param)))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open AVTimer Driver for i/p port %lu with result: %d",unInPortID, result);
			pCurrentInPort->avt_drv_handle = NULL;
			return result;
		}
	}
	return result;
}
ADSPResult MtMx_InPortToHonorTimestamp(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType       *pCurrentInPort = me->inPortParams[unInPortID];
	elite_msg_data_buffer_t*   pInputBuf;
	int64_t                    unDataRcvdInUsec;

	pCurrentInPort->pInputBuf = (elite_msg_data_buffer_t*)((pCurrentInPort->myDataQMsg).pPayload);
	pInputBuf = pCurrentInPort->pInputBuf;
	pCurrentInPort->bIsTimeStampValid = (bool_t)asm_get_timestamp_valid_flag(pInputBuf->nFlag);

	if (TRUE == pCurrentInPort->bShouldSessionTimeBeResync)
	{
		//For gapless playback, reset session clk to zero before checking the timestamp validity flag so that in case the TS validity flag
		//is FALSE, session clk for the new stream begins from 0
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Reset Gapless SessionTime to 0", me->mtMxID, unInPortID);
		MtMx_ResetSessionTimeClock(me, unInPortID);
	}

	/* if i/p buffer timestamp is not valid, simply return. In such case:
      a. Render the buffer immediately (skip the below logic of checking whether buffer has to be held or dropped),
      b. Do not increment session clock if silence is inserted (done in MtMx_OutPortToHonorInPortsTimestamps), and
      c. Ignore timestamps till the end of playback
	 */
	if (!pCurrentInPort->bIsTimeStampValid)
	{
		if((TRUE == pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS) ||
				(TRUE == pCurrentInPort->bShouldSessionTimeBeResync))
		{
			pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = FALSE;
			pCurrentInPort->bShouldSessionTimeBeResync = FALSE;
		}
		pCurrentInPort->bForceCheckTimeStampValidity = FALSE; //Set this to FALSE and never check for TS validity ever for this session (exception: 2nd gapless stream)
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: [TS flag = 0], ignoring all future TS's", me->mtMxID, unInPortID);

		// Since the input port timestamp validity has changed state from TRUE -> FALSE we need to trigger a rescan
		// on the output ports for any connected input ports with a valid timestamp.
		MtMx_ScanOutputPortsForInputsWithValidTimestamps(me);
		return ADSP_EOK;
	}

	/* The following procedure will calculate the current frame duration in microseconds. This procedure will
	 * avoid accumulating error over time due to truncation for frame sizes that aren't integer multiples
	 * of the sampling rate. The procedure will:
		1. Update number of sample received (over last one second).
      2. Calculate data received in microseconds from total number of samples received (over last one second).
      3. Calculate data received in microseconds for current frame by subtracting previous number.
         This will give current frame duration in microseconds.
      4. Roll over frame size and frame duration in microseconds every one second.
	 */

	//Update number of sample received (over last one second).
	pCurrentInPort->unDataRcvdInSamples += ((pInputBuf->nActualSize) / (pCurrentInPort->unNumChannels * pCurrentInPort->unBytesPerSample));

	//Calculate data received in microseconds from number of samples received (over last one second).
	unDataRcvdInUsec = MT_MX_SAMPLES_TO_USEC(pCurrentInPort->unDataRcvdInSamples, pCurrentInPort->unSampleRate);

	//Calculate data received in micro sec for current frame by subtracting previous number.
	//This will give current frame duration in usec.
	unDataRcvdInUsec -= pCurrentInPort->unDataRcvdInUsec.int_part;

	// roll over frame size and frame duration in usec to within one second
	pCurrentInPort->unDataRcvdInUsec.int_part += unDataRcvdInUsec;
	if (pCurrentInPort->unDataRcvdInSamples >= pCurrentInPort->unSampleRate)
	{
		pCurrentInPort->unDataRcvdInSamples -= pCurrentInPort->unSampleRate;
	}
	if (pCurrentInPort->unDataRcvdInUsec.int_part >= 1000000)
	{
		pCurrentInPort->unDataRcvdInUsec.int_part -= 1000000;
	}

	//Update timestamps
	pCurrentInPort->ullIncomingBufferTS = pInputBuf->ullTimeStamp;

#ifdef MT_MX_EXTRA_DEBUG
	MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu InputTS [%lu, %lu] usec, TSValidity: %d, current frame dur [%lu, %lu] usec", me->mtMxID, unInPortID,
			(uint32_t)(pInputBuf->ullTimeStamp>>32),(uint32_t)(pInputBuf->ullTimeStamp), pCurrentInPort->bIsTimeStampValid, (uint32_t)(unDataRcvdInUsec>>32), (uint32_t)unDataRcvdInUsec);
#endif

	//If this is the first TS received after a RUN cmd with RUN_IMMEDIATE, then obtain session time from i/p buffer TS
	if (TRUE == pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS)
	{
		pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = FALSE;
		pCurrentInPort->bShouldSessionTimeBeResync = FALSE;
		pCurrentInPort->ullTimeStampForStcBaseInUsec.int_part = pInputBuf->ullTimeStamp;
		pCurrentInPort->ullTimeStampForStcBaseInUsec.frac_part = 0;
		//Since we are maintaining a base value for STC, the num samples for update
		//should also be reset when the RUN command is issued.
		pCurrentInPort->ullNumSamplesForStcUpdate = 0;
		pCurrentInPort->ullSessionTime = pInputBuf->ullTimeStamp;
	}

	return ADSP_EOK;
}

ADSPResult MxAr_OutPortToHonorInPortsTimestamps(This_t *me, uint32_t unOutPortID)
{
	uint32_t                 unInPortID;
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	uint32_t                inPortsMask = pCurrentOutPort->inPortsMask;

	uint32_t bufDelay = pCurrentOutPort->unFrameDurationInUsec.int_part * pCurrentOutPort->unNumOutputBufs;
	uint32_t ullCurrentDownstreamDelay = *(pCurrentOutPort->punAFEDelay) + *(pCurrentOutPort->punCoppBufDelay) + *(pCurrentOutPort->punCoppAlgDelay) + bufDelay;

	while (inPortsMask)
	{
		unInPortID = Q6_R_ct0_R(inPortsMask);
		inPortsMask ^= 1 << unInPortID;
		pCurrentInPort = me->inPortParams[unInPortID];

		if (unOutPortID == pCurrentInPort->unTopPrioOutPort)
		{
			if(FALSE == pCurrentInPort->bHasFirstSampleAccumulated)
			{
				continue;
			}

			switch (pCurrentInPort->inPortState)
			{
			case INPUT_PORT_STATE_ACTIVE:
			{
				// if i/p buffer timestamp is not valid, and if i/p port is starving for data, do not
				// increment the session clock when silence is inserted
				if (!pCurrentInPort->bIsTimeStampValid && !(pCurrentOutPort->accInPortsMask & (1 << unInPortID)))
				{
					break;
				}
				// if an EOS was sent down, no longer increment i/p port's session time
				if (FALSE == pCurrentInPort->bIsSessionUnderEOS)
				{
					//Update session time and absolute time
					MxArUpdateInputPortSessionTime(me, unInPortID, pCurrentInPort, pCurrentOutPort, ullCurrentDownstreamDelay);

					//Update timestamps
					if(pCurrentInPort->bIsTimeStampValid)
					{
						mt_mx_copy_time_value(&pCurrentInPort->ullTimeStampProcessed, pCurrentInPort->ullTimeStampAtAccumulation);
					}

#ifdef MT_MX_EXTRA_DEBUG
					MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu ProcessedTS [%lu, %lu] samples, TSValidity: %d", me->mtMxID, unInPortID,
							(uint32_t)(pCurrentInPort->ullTimeStampProcessed.int_part>>32),(uint32_t)(pCurrentInPort->ullTimeStampProcessed.int_part),
							pCurrentInPort->bIsTimeStampValid);
#endif
				}
				pCurrentInPort->ullStaleEosSamples = 0;
				break;
			}
			case INPUT_PORT_STATE_PAUSED:
			{
				// During paused state, increment session time ONLY for as long as i/p port receives i/p buffers.
				// When it doesn't receive i/p buffer anymore, stop incrementing session clock.
				if (pCurrentOutPort->accInPortsMask & (1 << unInPortID))
				{
					//Update session time and absolute time
					MxArUpdateInputPortSessionTime(me, unInPortID, pCurrentInPort, pCurrentOutPort, ullCurrentDownstreamDelay);

					//Update timestamps
					if(pCurrentInPort->bIsTimeStampValid)
					{
						mt_mx_copy_time_value(&pCurrentInPort->ullTimeStampProcessed, pCurrentInPort->ullTimeStampAtAccumulation);
					}
				}
				pCurrentInPort->ullStaleEosSamples = 0;
				break;
			}
			}
		}
	}

	return ADSP_EOK;
}

ADSPResult MtMx_FillInPortLocalBuf(This_t *me, uint32_t unInPortID, uint32_t numSamplesReqPerCh)
{
	ADSPResult              result = ADSP_EOK;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	int8_t                  *pWrLocation = pCurrentInPort->pWrLoc;
	int8_t                  *pCurrentSample = pCurrentInPort->pCurrentSample;

	//Convert from samples to bytes.
	uint32_t  unInPortPerChBufSizeBytes = (pCurrentInPort->unBytesPerSample) * (pCurrentInPort->unInPortPerChBufSize);
	uint32_t  numBytesReqPerCh             = (pCurrentInPort->unBytesPerSample) * (numSamplesReqPerCh);
	uint32_t  unInpQNumBytesPerCh        = (pCurrentInPort->unBytesPerSample) * (pCurrentInPort->nNumRemainingSamplesPerCh + pCurrentInPort->nNumSamplesUsedPerCh);

	//Copy the samples from Q to inport local buffer
	for (uint32_t k = 0; k < pCurrentInPort->unNumChannels; k++)
	{
		//Memcpy and increment read and write pointers to next channel
		memscpy((void*)pWrLocation, numBytesReqPerCh, (void*)pCurrentSample, numBytesReqPerCh);
		pCurrentSample += unInpQNumBytesPerCh;
		pWrLocation       += unInPortPerChBufSizeBytes;
	}

	//Increment read and write pointers
	pCurrentInPort->pWrLoc += numBytesReqPerCh;
	pCurrentInPort->pCurrentSample += numBytesReqPerCh;

	//Update state variables
	pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf -= numSamplesReqPerCh;
	pCurrentInPort->nNumRemainingSamplesPerCh -= numSamplesReqPerCh;
	pCurrentInPort->nNumSamplesUsedPerCh += numSamplesReqPerCh;

	//Update local buffer fullness and emptiness
	if (0 == pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf)
	{
		pCurrentInPort->bIsLocalBufFull = TRUE;
	}
	else
	{
		pCurrentInPort->bIsLocalBufFull = FALSE;
	}
	pCurrentInPort->bIsLocalBufEmpty = FALSE;

	return result;
}

ADSPResult MxAr_CommonInPortProcessingRoutinePostSteps(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];

	//Try refilling the i/p port's local buffer with new samples from upstream peer's o/p buffer
	if (pCurrentInPort->nNumRemainingSamplesPerCh < (int)pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf)
	{
		//Update timestamps
		if(pCurrentInPort->bIsTimeStampValid)
		{
			mt_mx_increment_time(&pCurrentInPort->ullTimeStampAtCopy, pCurrentInPort->unFrameDurationInUsec);
		}

		//Now it is OK to return the buffer, as it has been established that the i/p Q buffer has truly been emptied, at this point.
		if (INPUT_BUFFER_HELD == pCurrentInPort->bInBufStatus)
		{
			if (ADSP_FAILED(elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
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

	}
	else
	{
		//i/p buffer has sufficient samples to fill local buffer
		//Update timestamps
		if(pCurrentInPort->bIsTimeStampValid)
		{
			mt_mx_increment_time(&pCurrentInPort->ullTimeStampAtCopy, pCurrentInPort->unFrameDurationInUsec);
		}

		//copy samples from i/p buffer to local buffer and update nNumRemainingSamplesPerCh
		MtMx_FillInPortLocalBuf(me, unInPortID, pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf);
	}

	return ADSP_EOK;
}

ADSPResult MxAr_Run(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxRunType *pPayload)
{
	uint32_t unPort2Run = pPayload->unPortID;
	ADSPResult result = ADSP_EOK;
	if (MATRIX_MIXER_PORT_DIR_INPUT == pPayload->unPortDirection)
	{
		MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unPort2Run];
		if (NULL == pCurrentInPort)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] Run handler: rcvd cmd on invalid port [%lu]", me->mtMxID, pPayload->unSecOpCode, unPort2Run);
			return ADSP_EUNEXPECTED;
		}

		if(ADSP_EOK != (result = MtMx_AvSyncRequestHW(me, unPort2Run)))
		{
			return result;
		}

		switch (pCurrentInPort->inPortState)
		{
		case INPUT_PORT_STATE_INIT:
		case INPUT_PORT_STATE_WAITING_TO_BE_READY:
		case INPUT_PORT_STATE_READY:
		{
			pCurrentInPort->inPortState = INPUT_PORT_STATE_READY;
			pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_READY;
			MtMx_AddInputPortToWaitMask(me, unPort2Run);
			pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "relative start time good for rendering, moving i/p port %lu to READY", unPort2Run);
			return ADSP_EOK;
		}
		case INPUT_PORT_STATE_PAUSED:
		case INPUT_PORT_STATE_WAITING_TO_BE_ACTIVE:
		case INPUT_PORT_STATE_ACTIVE:
		case INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER:
		{
			pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE;
			pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE;
			MtMx_InPortToUpdateWaitMask(me, unPort2Run);
			pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "relative start time slightly > 0, moving i/p port %lu to ACTIVE", unPort2Run);
			return ADSP_EOK;
		}
		}
	}
	else
	{
		//RX o/p port
		MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unPort2Run];
		if (NULL == pCurrentOutPort)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] Run handler: rcvd cmd on invalid port [%lu]",
					me->mtMxID, pPayload->unSecOpCode, unPort2Run);
			return ADSP_EUNEXPECTED;
		}

		switch (pCurrentOutPort->outPortState)
		{
		case OUTPUT_PORT_STATE_INIT:
		{
			MtMx_MoveOutportToActiveState(me, unPort2Run,pCurrentOutPort);
			MtMx_OutPortToUpdateActiveOutPortsMasks(me, unPort2Run);
			MtMx_OutPortToUpdateWaitMask(me, unPort2Run);
			return ADSP_EOK;
		}
		default:
		{
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] Run handler: rcvd cmd in unexpected state [%d], discarding",
					me->mtMxID, pPayload->unSecOpCode, pCurrentOutPort->outPortState);
			return ADSP_EUNEXPECTED;
		}
		}
	}
	return ADSP_EOK;
}

ADSPResult MxAt_Run(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxRunType *pPayload)
{
	uint32_t unPort2Run = pPayload->unPortID;

	if (MATRIX_MIXER_PORT_DIR_INPUT == pPayload->unPortDirection)
	{
		//TX i/p port
		MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unPort2Run];
		if (NULL == pCurrentInPort)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] Run handler: rcvd cmd on invalid port [%lu]",
					me->mtMxID, pPayload->unSecOpCode, unPort2Run);
			return ADSP_EUNEXPECTED;
		}

		switch (pCurrentInPort->inPortState)
		{
		case INPUT_PORT_STATE_INIT:
		{
			int k = qurt_elite_queue_get_channel_bit(pCurrentInPort->inPortHandle.portHandle.dataQ);
			int t = 31 - (Q6_R_cl0_R( (k) ));
			me->inPortIDMap[t] = unPort2Run;
			me->unDataBitfield = me->unDataBitfield | k;
			pCurrentInPort->inPortState = INPUT_PORT_STATE_READY;
			pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_READY;
			MtMx_AddInputPortToWaitMask(me, unPort2Run);
			return ADSP_EOK;
		}
		default:
		{
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] Run handler: rcvd cmd in unexpected state [%ld], discarding",
					me->mtMxID, pPayload->unSecOpCode, pCurrentInPort->inPortState);
			return ADSP_EUNEXPECTED;
		}
		}
	}
	else
	{
		//TX o/p port
		MatrixOutPortInfoType *pCurrentOutPort = me->outPortParams[unPort2Run];
		if (NULL == pCurrentOutPort)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] Run handler: rcvd cmd on invalid port [%lu]",
					me->mtMxID, pPayload->unSecOpCode, unPort2Run);
			return ADSP_EUNEXPECTED;
		}

		if(NULL == pCurrentOutPort->avt_drv_handle)
		{
			// Open AV timer driver (only needed on the TX matrix o/p port)
			avtimer_open_param_t open_param;
			char client_name[] = "MtMxOp";
			ADSPResult result = ADSP_EOK;
			open_param.client_name = client_name;
			open_param.flag = 0;
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Opening AVTimer Driver for o/p port %lu",me->mtMxID, unPort2Run);
			if (ADSP_EOK != (result = avtimer_drv_hw_open(&(pCurrentOutPort->avt_drv_handle), &open_param)))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open AVTimer Driver for o/p port %lu with result: %d",unPort2Run,result);
				pCurrentOutPort->avt_drv_handle = NULL;
				return result;
			}
		}

		switch (pCurrentOutPort->outPortState)
		{
		case OUTPUT_PORT_STATE_INIT:
		case OUTPUT_PORT_STATE_PAUSED:
		case OUTPUT_PORT_STATE_WAITING_TO_BE_ACTIVE:
		{
			/* Mark the next output buffer that is sent downstream as the first buffer
            following a RUN cmd, so that the timestamp is calculated in the following way:
            a. For PAUSE/RUN sequence: Timestamp should increment from where it stopped
            b. For PAUSE/FLUSH/RUN sequence: Timestamp should increment from 0 (flush cmd handler
            resets pCurrentOutPort->llSessionTimeInSamples to 0. */
			pCurrentOutPort->bIsFirstOutBufYetToBeSent = TRUE;

			switch (pPayload->unStartFlag)
			{
			case ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE:
			case ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_RELATIVE_TIME:
			{
				MtMx_RunOutputPort(me, unPort2Run);
				break;
			}
			default:
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] Run handler: rcvd invalid start flag [%d]",
						me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unStartFlag);
				return ADSP_EBADPARAM;
			}
			}
			return ADSP_EOK;
		}
		default:
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] Run handler: rcvd cmd in unexpected state [%d], discarding",
					me->mtMxID, pPayload->unSecOpCode, pCurrentOutPort->outPortState);
			return ADSP_EUNEXPECTED;
		}
		}
	}
	return ADSP_EOK;
}

ADSPResult MxAr_InPortGetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	ADSPResult result = ADSP_EOK;
	void *pDataStart;

	uint16_t unTotalPayloadSize = pPayload->unSize;

	asm_session_param_data_v2_t *pDataPayload = (asm_session_param_data_v2_t *)(pPayload->pnParamData);

	//Code needs to handle only one Get Param command.
	//Multiple Get Params in the same command will not be serviced/supported.
	if(unTotalPayloadSize > 0)
	{
		//Offset by the struct size to get to the start of the data
		pDataStart = (void*)(pDataPayload + 1);

		switch(pDataPayload->module_id)
		{
		case ASM_SESSION_MTMX_STRTR_MODULE_ID_AVSYNC:
		{
			switch(pDataPayload->param_id)
			{
			case ASM_SESSION_MTMX_STRTR_PARAM_SESSION_TIME_V3: //Stub only supports Get Session Time
			{
				//Make sure that the payload size is valid.
				if(unTotalPayloadSize < (sizeof(asm_session_param_data_v2_t) + sizeof(asm_session_mtmx_strtr_param_session_time_v3_t)))
				{
					result = ADSP_ENEEDMORE;
					MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu, Payload Size: %d < Reqd Size: %d",
							me->mtMxID, unInPortID, unTotalPayloadSize,
							(sizeof(asm_session_param_data_v2_t) + sizeof(asm_session_mtmx_strtr_param_session_time_v3_t)));
					return result;
				}

				//Access the structure to get hold of the data
				asm_session_mtmx_strtr_param_session_time_v3_t *pData = (asm_session_mtmx_strtr_param_session_time_v3_t*)pDataStart;
				//Get Session Clock from AvSync Lib
				uint64_t ullSessionTime = pCurrentInPort->ullSessionTime;
				uint64_t ullAbsoluteTime = pCurrentInPort->ullAbsoluteTime;
				uint64_t ullTimeStamp = pCurrentInPort->ullTimeStampProcessed.int_part;
				//Fill up data
				pData->session_time_msw = (uint32_t)(ullSessionTime >> 32);
				pData->session_time_lsw = (uint32_t)(ullSessionTime);
				pData->absolute_time_msw = (uint32_t)(ullAbsoluteTime >> 32);
				pData->absolute_time_lsw = (uint32_t)(ullAbsoluteTime);
				pData->time_stamp_msw = (uint32_t)(ullTimeStamp>>32);
				pData->time_stamp_lsw = (uint32_t)(ullTimeStamp);
				if(pCurrentInPort->bIsTimeStampValid && TRUE == pCurrentInPort->bHasFirstSampleAccumulated)
				{
					pData->flags = ASM_SESSION_MTMX_STRTR_PARAM_SESSION_TIME_TIMESTAMP_VALID_FLAG_BIT_MASK;
				}
				else
				{
					pData->flags = 0;
				}

				//Update the param_size so that the caller is aware of the actual size filled by this get param
				pDataPayload->param_size = sizeof(asm_session_mtmx_strtr_param_session_time_v3_t);
				pPayload->unSize = sizeof(asm_session_mtmx_strtr_param_session_time_v3_t);

				//Debug messages
				MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu (GetSessionTime): i/p port id %lu: [session time msw, session time lsw] = [%lu, %lu] : "
						"[absolute time msw, absolute time lsw] = [%lu, %lu]:"
						"[time stamp msw, time stamp lsw] = [%lu, %lu]:"
						"Flags %lu",
						me->mtMxID, unInPortID,
						pData->session_time_msw, pData->session_time_lsw,
						pData->absolute_time_msw, pData->absolute_time_lsw,
						pData->time_stamp_msw, pData->time_stamp_lsw,
						pData->flags);

				break;
			}
			case ASM_SESSION_MTMX_STRTR_PARAM_SESSION_INST_STATISTICS_V2:
			case ASM_SESSION_MTMX_STRTR_PARAM_SESSION_CUMU_STATISTICS_V2:
			default:
			{
				result = ADSP_EBADPARAM;
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu Invalid Param ID: %lu",
						me->mtMxID, unInPortID, pDataPayload->param_id);
				return result;
			}
			}
			break;
		}
		default:
		{
			result = ADSP_EBADPARAM;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu Invalid Module ID: %lu",
					me->mtMxID, unInPortID, pDataPayload->module_id);
			return result;
		}
		}
	}
	else
	{
		result = ADSP_EBADPARAM;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu Invalid unTotalPayloadSize: %d",
				me->mtMxID, unInPortID, unTotalPayloadSize);
		return result;
	}

	return result;
}

ADSPResult MtMx_ResetSessionTimeClock(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];
	pCurrentInPort->ullSessionTime = 0;
	pCurrentInPort->ullNumSamplesForStcUpdate = 0;
	pCurrentInPort->ullTimeStampForStcBaseInUsec.int_part = 0;
	pCurrentInPort->ullTimeStampForStcBaseInUsec.frac_part = 0;
	return ADSP_EOK;
}

ADSPResult MxArUpdateInputPortSessionTime(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort,
		MatrixOutPortInfoType *pCurrentOutPort, uint32_t ullCurrentDevicePathDelay)
{
	uint32_t unSampleRate = pCurrentInPort->unSampleRate;
	uint32_t unPortPerChBufSize = pCurrentInPort->unInPortPerChBufSize;
	uint64_t ullNumSamplesForStcUpdate = pCurrentInPort->ullNumSamplesForStcUpdate;
	uint32_t unSampleIncrement = 0;

	// When adding an additional device (running at a different rate) to an existing session it is
	// possible that that output port will send data downstream before reconfiguring to the new rate.
	// During this transition the input port may have already reconfigured itself and it's state
	// will reflect the new sampling rate; however, the data being sent down is at the old rate.
	// In this case we must use the old rate information to update the STC. The old rate information
	// is the same as the output port's rate.
	if (pCurrentInPort->unSampleRate != pCurrentOutPort->unSampleRate)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port fs %lu does not match o/p port fs %lu. Using old rate for ST update!",
				me->mtMxID, pCurrentInPort->unSampleRate, pCurrentOutPort->unSampleRate);
		unSampleRate = pCurrentOutPort->unSampleRate;
		unPortPerChBufSize = pCurrentOutPort->unOutPortPerChBufSize;

		// Convert the sample count to the old base using the previous sampling rate.
		// sample_base_old = sample_base_new * (fs_prev / fs_new)
		ullNumSamplesForStcUpdate = (pCurrentInPort->ullNumSamplesForStcUpdate * pCurrentOutPort->unSampleRate) / pCurrentInPort->unSampleRate;
	}

	unSampleIncrement += unPortPerChBufSize - pCurrentInPort->ullStaleEosSamples;
	ullNumSamplesForStcUpdate += unSampleIncrement;
	pCurrentInPort->ullNumSamplesForStcUpdate += unSampleIncrement;

	// overflow protection logic is unnecessary since the calculation below will overflow after ~1000 days
	// The Proposed Session Clock = Equivalent time for the STC Update + The Base Time for STC
	uint64_t proposed_session_clock = (MT_MX_SAMPLES_TO_USEC(ullNumSamplesForStcUpdate, unSampleRate))+ (pCurrentInPort->ullTimeStampForStcBaseInUsec.int_part);
	pCurrentInPort->ullSessionTime = proposed_session_clock;
	uint64_t ullStaleEosSamplesuSec = MT_MX_SAMPLES_TO_USEC(pCurrentInPort->ullStaleEosSamples, unSampleRate);
	pCurrentInPort->ullAbsoluteTime = (NULL != pCurrentInPort->avt_drv_handle) ? ((avtimer_drv_get_time(pCurrentInPort->avt_drv_handle))+ullCurrentDevicePathDelay-ullStaleEosSamplesuSec):(0);
#ifdef MT_MX_EXTRA_DEBUG
	MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu updating STC to [%lu, %lu], AbsTS: [%lu, %lu]", me->mtMxID, unInPortID,
			(uint32_t)(pCurrentInPort->ullSessionTime>>32), (uint32_t)(pCurrentInPort->ullSessionTime),
			(uint32_t)(pCurrentInPort->ullAbsoluteTime>>32), (uint32_t)(pCurrentInPort->ullAbsoluteTime));
#endif
	return ADSP_EOK;
}
