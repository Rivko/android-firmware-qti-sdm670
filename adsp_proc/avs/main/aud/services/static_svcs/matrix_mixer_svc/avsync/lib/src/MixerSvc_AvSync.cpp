/**
@file MixerSvc_AvSync.cpp
@brief This file defines matrix mixer AV-Sync utilities.
 */

/*========================================================================
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/avsync/lib/src/MixerSvc_AvSync.cpp#1 $

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
#include "avsync_lib.h"
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

struct capiv2_mediafmt_t
{
    capi_v2_data_format_header_t h;
    capi_v2_standard_data_format_t f;
};

//Call back handler for the SampleSlip instance created in Matrix mixer
static capi_v2_err_t MtMx_SampleSlipCallbackHandler(void *context_ptr,
                                                     capi_v2_event_id_t id,
                                                     capi_v2_event_info_t *event_info_ptr);

static capi_v2_err_t MtMx_SampleSlipCallbackHandler(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{

  capi_v2_err_t result = CAPI_V2_EOK;
  mt_mx_sampleslip_t    *pSampleSlip = reinterpret_cast<mt_mx_sampleslip_t*>(context_ptr);

  switch(id)
  {
    case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
    {
      uint32_t required_payload_size = sizeof(capi_v2_event_algorithmic_delay_t);

      if (event_info_ptr->payload.actual_data_len < required_payload_size)
      {
        CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
        break;
      }

      pSampleSlip->unAlgDelay = reinterpret_cast<capi_v2_event_algorithmic_delay_t*>(event_info_ptr->payload.data_ptr)->delay_in_us;

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx sampleslip struct ptr:0x%p and Alg_delay:%lu.", pSampleSlip, pSampleSlip->unAlgDelay);

      break;
    }
    default:
    {
      result = CAPI_V2_EOK;
      break;
    }
  }
  return result;
}
ADSPResult MtMx_InPortToHonorTimestamp(This_t *me, uint32_t unInPortID)
{
	ADSPResult                 result;
	MatrixInPortInfoType       *pCurrentInPort = me->inPortParams[unInPortID];
	elite_msg_data_buffer_t*   pInputBuf;
	int64_t                    unDataRcvdInUsec;
	uint32_t ullCurrentDevicePathDelay = 0, ullCurrentDevicePathDelayComp = 0;
	mt_mx_sampleslip_t	    *pSampleSlip        = &(pCurrentInPort->pInPortAvSync->structSampleSlip);
	avsync_rendering_decision_t rendering_decision = RENDER;
	int64_t delta = 0;

	//Make sure that the pCurrentInPort->unTopPrioOutPort is valid.
	if(pCurrentInPort->unTopPrioOutPort < MT_MX_MAX_OUTPUT_PORTS)
	{
		MatrixOutPortInfoType       *pTopPrioOutPort = me->outPortParams[pCurrentInPort->unTopPrioOutPort];

		//Make sure that the punAFEDelay, punCoppDelay, punMtMxOutDelay are all valid.
		if((NULL != pTopPrioOutPort) && (NULL != pTopPrioOutPort->punAFEDelay) && (NULL != pTopPrioOutPort->punCoppBufDelay) && (NULL != pTopPrioOutPort->punCoppAlgDelay) && (NULL != pCurrentInPort->punMtMxInDelay))
		{
			ullCurrentDevicePathDelay = *(pTopPrioOutPort->punAFEDelay) + *(pTopPrioOutPort->punCoppBufDelay) + *(pTopPrioOutPort->punCoppAlgDelay) + *(pCurrentInPort->punMtMxInDelay);

			// The session time calculation happens in the output port context. So the rendering decision should only take into account the delay from the
			// output port down. We should thus remove the delays of the acc buf and the internal buf. Based on when the buffer was sent down on the device,
			// this can vary from (frame duration) to 2*(frame duration). Here we take the average.
			ullCurrentDevicePathDelayComp = ullCurrentDevicePathDelay - (pTopPrioOutPort->unFrameDurationInUsec.int_part) - 0.5*(pTopPrioOutPort->unFrameDurationInUsec.int_part);
		}
	}

	pCurrentInPort->pInputBuf = (elite_msg_data_buffer_t*)((pCurrentInPort->myDataQMsg).pPayload);
	pInputBuf = pCurrentInPort->pInputBuf;
	pCurrentInPort->bIsTimeStampValid = (bool_t)asm_get_timestamp_valid_flag(pInputBuf->nFlag);

	if (TRUE == pCurrentInPort->bShouldSessionTimeBeResync)
	{
		/* For gapless playback or Resync command,
	           a. reset session clk to zero before checking the timestamp validity flag for the next incoming buffer
	           b. Depending on whether the TS Valid flag is TRUE/FALSE, the new session time would resync to incoming TS/start from zero.
		*/
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
			pCurrentInPort->pInPortAvSync->bUseDefaultWindowForRendering = FALSE;
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
			(uint32_t)(pInputBuf->ullTimeStamp>>32),(uint32_t)(pInputBuf->ullTimeStamp), pCurrentInPort->bIsTimeStampValid,
			(uint32_t)(unDataRcvdInUsec>>32), (uint32_t)unDataRcvdInUsec);
#endif
   
  /*
   * The selection of render mode depends on the set value of render decision.
   *
   *              When there is no point of reference w.r.t wall clock (default render decision mode),
   *              the first buffer TS is used as the base for gating future buffers. Also in this
   *              RUN mode, there is no notion of wall clock reference.
   *
   *              Whenever there is a point of reference w.r.t wall clock (Local STC mode),
   *              the start of session is derived from STC. Hence it makes sense to gate the first buffer
   *              against STC. For the subsequent buffers, the set value of render decision can be used.
   *  
   * */  
	
	uint32_t avsync_rendering_mode =ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT;
	avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,RENDERING_DECISION_TYPE,&avsync_rendering_mode);
	
	if( ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT == avsync_rendering_mode)
	{
		avsync_lib_make_rendering_decision(pCurrentInPort->pInPortAvSync->pAVSyncLib,
				pInputBuf->ullTimeStamp,
				(uint64_t)0,
				(bool_t)TRUE,
				&delta,
				&rendering_decision);

		//If this is the first TS received after a RUN cmd with RUN_IMMEDIATE, then obtain session time from i/p buffer TS
		if (TRUE == pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS)
		{
			//If the stat window ends are setup, call an update on the AVSync stat library, keeping in mind update has to be called only once per buffer
			if(FALSE == pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats && HOLD != rendering_decision)
			{
				//Note that ADSP_ENOTREADY denotes that start and end of render windows haven't been set.
				if(ADSP_EOK ==avsync_lib_update_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib, delta, unDataRcvdInUsec))
				{
					pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats = TRUE;
				}
			}

			//Update the drift, keeping in mind update has to be called only once per buffer
			if(FALSE == pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedDrift)
			{
				avsync_lib_update_s2d_drift(pCurrentInPort->pInPortAvSync->pAVSyncLib, (uint32_t)unDataRcvdInUsec, pCurrentInPort->ullIncomingBufferTS);
				pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedDrift = TRUE;
			}

			pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = FALSE;
			pCurrentInPort->pInPortAvSync->bUseDefaultWindowForRendering = FALSE;
			pCurrentInPort->bShouldSessionTimeBeResync = FALSE;
			pCurrentInPort->pInPortAvSync->bIsStrClkSyncdWithSessClk = TRUE;
			pCurrentInPort->ullTimeStampForStcBaseInUsec.int_part = pInputBuf->ullTimeStamp;
			pCurrentInPort->ullTimeStampForStcBaseInUsec.frac_part = 0;
			//Since we are maintaining a base value for STC, the num samples for update
			//should also be reset when the RUN command is issued.
			pCurrentInPort->ullNumSamplesForStcUpdate = 0;
			avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,SESSION_CLOCK,&pInputBuf->ullTimeStamp,sizeof(pInputBuf->ullTimeStamp));
			uint64_t proposed_expected_session_clock = pInputBuf->ullTimeStamp + unDataRcvdInUsec;
			avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,EXPECTED_SESSION_CLOCK,&proposed_expected_session_clock,sizeof(proposed_expected_session_clock));

			//In Session Clock mode, the first buffer is always rendered. So clear any holds.
			(void)MtMx_ClearHoldDuration(me, unInPortID);
			return ADSP_EOK;
		}

		//If the stat window ends are setup, call an update on the AVSync stat library, keeping in mind update has to be called only once per buffer
		if(HOLD == rendering_decision)
		{
			pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part = (-delta);
			pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.frac_part = 0;

			//If hold duration is less than the 1ms, treat it as immediate.
			if(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part < MT_MX_FRAME_DURATION_1000US)
			{
				(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
				delta = 0;
				rendering_decision = RENDER;
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Hold duration was < 1ms, decision to render instead", me->mtMxID, unInPortID);
			}
		}

		//If the stat window ends are setup, call an update on the AVSync stat library, keeping in mind update has to be called only once per buffer
		if(FALSE == pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats)
		{
			avsync_lib_update_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib, delta, unDataRcvdInUsec);
			pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats = TRUE;
		}

		if (RENDER == rendering_decision)
		{
			uint64_t proposed_expected_session_clock;
			avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,EXPECTED_SESSION_CLOCK,&proposed_expected_session_clock);
			proposed_expected_session_clock += unDataRcvdInUsec;
			avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,EXPECTED_SESSION_CLOCK,&proposed_expected_session_clock,sizeof(proposed_expected_session_clock));
			if(0 != delta && pCurrentInPort->pInPortAvSync->bIsStrClkSyncdWithSessClk == FALSE)
			{
				avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,SESSION_CLOCK,&pInputBuf->ullTimeStamp,sizeof(pInputBuf->ullTimeStamp));
			}

			//Update the drift, keeping in mind update has to be called only once per buffer
			if(FALSE == pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedDrift)
			{
				avsync_lib_update_s2d_drift(pCurrentInPort->pInPortAvSync->pAVSyncLib, unDataRcvdInUsec, pInputBuf->ullTimeStamp);
				pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedDrift = TRUE;
			}
			pCurrentInPort->pInPortAvSync->bIsStrClkSyncdWithSessClk = TRUE;

			return ADSP_EOK;
		}
		else if (DROP == rendering_decision)
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Past TS, drop buf", me->mtMxID, unInPortID);

			//Update timestamps
			pCurrentInPort->ullTimeStampAtCopy.int_part = pInputBuf->ullTimeStamp;
			pCurrentInPort->ullTimeStampAtCopy.frac_part = 0;
			pCurrentInPort->ullTimeStampAtAccumulation.int_part = pInputBuf->ullTimeStamp;
			pCurrentInPort->ullTimeStampAtAccumulation.frac_part = 0;
			pCurrentInPort->ullTimeStampProcessed.int_part = pInputBuf->ullTimeStamp;
			pCurrentInPort->ullTimeStampProcessed.frac_part = 0;

			MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu InputTS [%lu, %lu], TSValidity: %d", me->mtMxID, unInPortID,
					(uint32_t)(pInputBuf->ullTimeStamp>>32),(uint32_t)(pInputBuf->ullTimeStamp), pCurrentInPort->bIsTimeStampValid);

			//Drop the buffer
			if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to release i/p buffer back to upstr svc!\n");
			}
			pCurrentInPort->numBufReturned++;
			pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;

			//Now, that the absolute time and session time are updated, commit the AVSync update changes for this i/p port.
			ADSPResult commit_result = avsync_lib_commit_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib);
			if (ADSP_FAILED(commit_result) && ADSP_EBADPARAM != commit_result)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: in i/p port %lu context, failed to commit AVSync stats with result=%d", me->mtMxID, unInPortID, commit_result);
			}

			//No holding
			(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
			return ADSP_EFAILED;
		}
		else
		{
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Future TS, hold buf for MSW: %lu LSW: %lu usec",
					me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));

			//If hold duration is less than the 1ms, treat it as immediate.
			MtMx_RemoveInputPortFromWaitMask(me, unInPortID);
			pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER;
			pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER;
			return ADSP_EFAILED;
		}
	}
	else
	{
		//ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_LOCAL_STC
		
		//IMPORTANT: Start time has already been verified to be positive at this point.
		//If client is using relative TS, the first buffer will have 0 TS.
		//If client is using absolute TS, the first buffer may have non 0 absolute TS.
		/*
		 If client is using RUN immediate, start time value is back calculated based on the first buffer arrival TS. So that first buffer will render immediately.
		 For the subsequent buffers, the set value of start time can be used
		*/ 

		//Account for any partly filled input buffer at this point.
		uint32_t unSamplesAlreadyFilled = pCurrentInPort->unInPortPerChBufSize - pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf;
		uint64_t ullTimeOffsetUsec = 0;

		if(NULL != pSampleSlip->pSampleSlipCapiv2)
		{
			unSamplesAlreadyFilled += pSampleSlip->inBufOffset + pSampleSlip->outBufOffset;
		}

		if(0 != pCurrentInPort->unSamplesPer1Msec)
		{
			ullTimeOffsetUsec = MT_MX_SAMPLES_TO_USEC(unSamplesAlreadyFilled, pCurrentInPort->unSampleRate);
		}
		else
		{
			ullTimeOffsetUsec = MT_MX_SAMPLES_TO_USEC(unSamplesAlreadyFilled, MT_MX_SAMPLING_RATE_48000);
		}

		avsync_lib_update_stc_clock(pCurrentInPort->pInPortAvSync->pAVSyncLib);
    
		if ( ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE == pCurrentInPort->pInPortAvSync->unStartFlag &&
				ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_LOCAL_STC == avsync_rendering_mode &&
				(1 == pCurrentInPort->numBufRcvd))
		{
			avsync_lib_set_immediate_localstc_starttime(pCurrentInPort->pInPortAvSync->pAVSyncLib, pInputBuf->ullTimeStamp, (uint64_t)ullCurrentDevicePathDelayComp+ullTimeOffsetUsec );
		}

		if(TRUE == pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS)
		{
			//For local STC based rendering, this mode will be used at all times.
			//That means, each buffer will be treated as the first buffer.
			//Therefore, bShouldSessionTimeBeDerivedFromNextTS will always be TRUE for local STC.
			//For default rendering, this mode will be used only for the 1st buffer after a RUN command.
			avsync_lib_make_rendering_decision(pCurrentInPort->pInPortAvSync->pAVSyncLib,
					pInputBuf->ullTimeStamp,
					(uint64_t)ullCurrentDevicePathDelayComp+ullTimeOffsetUsec,
					FALSE,
					&delta,
					&rendering_decision);

			if(HOLD == rendering_decision)
			{
				pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part = (-delta);
				pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.frac_part = 0;

				//If hold duration is less than the 1ms, treat it as immediate.
				if(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part < MT_MX_FRAME_DURATION_1000US)
				{
					(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
					delta = 0;
					rendering_decision = RENDER;
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Hold duration was < 1ms, decision to render instead", me->mtMxID, unInPortID);
				}
			}

			//Compare Wall clock (+ Device latency) <==> Start time + TS
			if((RENDER == rendering_decision))
			{

				if(FALSE == pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats)
				{
					avsync_lib_update_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib, delta, unDataRcvdInUsec);
					pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats = TRUE;
				}

				//Update the drift, keeping in mind update has to be called only once per buffer
				if(FALSE == pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedDrift)
				{
					avsync_lib_update_s2d_drift(pCurrentInPort->pInPortAvSync->pAVSyncLib, unDataRcvdInUsec, pInputBuf->ullTimeStamp);
					pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedDrift = TRUE;
				}

				//Update [session time, absolute time] pair.
				avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,SESSION_CLOCK,&pInputBuf->ullTimeStamp,sizeof(pInputBuf->ullTimeStamp));
				uint64_t proposed_expected_session_clock = pInputBuf->ullTimeStamp + unDataRcvdInUsec;
				avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, EXPECTED_SESSION_CLOCK, &proposed_expected_session_clock, sizeof(proposed_expected_session_clock));
				avsync_lib_update_absolute_time(pCurrentInPort->pInPortAvSync->pAVSyncLib,ullCurrentDevicePathDelay+ullTimeOffsetUsec,FALSE);

				//No holding required.
				(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
				uint32_t current_avsync_rendering_decision =ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT;
				avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,RENDERING_DECISION_TYPE,&current_avsync_rendering_decision);
				//Sync done.
				if(ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT == current_avsync_rendering_decision)
				{
					pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS =  FALSE;
				}
				else
				{
					pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS =  TRUE;
				}
				pCurrentInPort->pInPortAvSync->bUseDefaultWindowForRendering = FALSE;
				pCurrentInPort->bShouldSessionTimeBeResync = FALSE;
				pCurrentInPort->pInPortAvSync->bIsStrClkSyncdWithSessClk = TRUE;

				return ADSP_EOK;
			}
			else if(DROP == rendering_decision)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Past TS, drop buf", me->mtMxID, unInPortID);

				//Update timestamps
				pCurrentInPort->ullTimeStampAtCopy.int_part = pInputBuf->ullTimeStamp;
				pCurrentInPort->ullTimeStampAtCopy.frac_part = 0;
				pCurrentInPort->ullTimeStampAtAccumulation.int_part = pInputBuf->ullTimeStamp;
				pCurrentInPort->ullTimeStampAtAccumulation.frac_part = 0;
				pCurrentInPort->ullTimeStampProcessed.int_part = pInputBuf->ullTimeStamp;
				pCurrentInPort->ullTimeStampProcessed.frac_part = 0;

				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu InputTS [%lu, %lu], TSValidity: %d", me->mtMxID, unInPortID,
						(uint32_t)(pInputBuf->ullTimeStamp>>32),(uint32_t)(pInputBuf->ullTimeStamp), pCurrentInPort->bIsTimeStampValid);

				//No holding required.
				(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
				//If the stat window ends are setup, call an update on the AVSync stat library, keeping in mind update has to be called only once per buffer
				if(FALSE == pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats)
				{
					avsync_lib_update_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib, delta, unDataRcvdInUsec);
					pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats = TRUE;
				}

				//Past TS, Drop the buffer.
				if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to release i/p buffer back to upstr svc!\n");
				}

				pCurrentInPort->numBufReturned++;
				pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
				return ADSP_EFAILED;
			}
			else //(HOLD == rendering_decision)
			{
				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Future TS, hold buf for [%lu %lu] usec",
						me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));
				//If hold duration is less than the rounding < 1ms, treat it as immediate.
				//i/p port is in a held state.
				pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER;
				pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER;

				//Update Expected ST for the next buffer.
				uint64_t proposed_expected_session_clock = pInputBuf->ullTimeStamp + unDataRcvdInUsec;
				avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,EXPECTED_SESSION_CLOCK,&proposed_expected_session_clock,sizeof(proposed_expected_session_clock));

				//Stop further processing from this port.
				MtMx_RemoveInputPortFromWaitMask(me, unInPortID);

				//Sync (partially) done.
				uint32_t avsync_rendering_decision_type =ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT;
				avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,RENDERING_DECISION_TYPE,&avsync_rendering_decision_type);
				if(ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT == avsync_rendering_decision_type)
				{
					pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS =  FALSE;
				}
				else
				{
					pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS =  TRUE;
				}
				pCurrentInPort->pInPortAvSync->bUseDefaultWindowForRendering = FALSE;
				pCurrentInPort->bShouldSessionTimeBeResync =  FALSE;

				return ADSP_EFAILED;
			}
		}
		else
		{
			//Steady state operation (for default rendering decision mode)
			//For local STC based rendering, it will treat every buffer as the first buffer. Therefore, it will never hit this logic.
			avsync_lib_make_rendering_decision(pCurrentInPort->pInPortAvSync->pAVSyncLib,
					pInputBuf->ullTimeStamp,
					(uint64_t)0,
					(bool_t)TRUE,
					&delta,
					&rendering_decision);

			if(HOLD == rendering_decision)
			{
				pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part = (-delta);
				pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.frac_part = 0;

				//If hold duration is less than the 1ms, treat it as immediate.
				if(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part < MT_MX_FRAME_DURATION_1000US)
				{
					(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
					delta = 0;
					rendering_decision = RENDER;
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Hold duration was < 1ms, decision to render instead", me->mtMxID, unInPortID);
				}
			}

			//If the stat window ends are setup, call an update on the AVSync stat library, only in steady state operation
			if(FALSE == pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats)
			{
				avsync_lib_update_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib, delta, unDataRcvdInUsec);
				pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats = TRUE;
			}
			//If past by less than or equal to llRenderWindowEnd, process immediately
			if(RENDER == rendering_decision)
			{
				//Update [session time, absolute time] pair.
				avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,SESSION_CLOCK,&pInputBuf->ullTimeStamp,sizeof(pInputBuf->ullTimeStamp));
				avsync_lib_increment_expected_session_clock(pCurrentInPort->pInPortAvSync->pAVSyncLib,unDataRcvdInUsec);

				//No holding required.
				(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync

				//Sync done.
				pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS =  FALSE;
				pCurrentInPort->pInPortAvSync->bUseDefaultWindowForRendering = FALSE;
				pCurrentInPort->bShouldSessionTimeBeResync = FALSE;
				pCurrentInPort->pInPortAvSync->bIsStrClkSyncdWithSessClk = TRUE;

				//Update the drift, keeping in mind update has to be called only once per buffer
				if(FALSE == pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedDrift)
				{
					avsync_lib_update_s2d_drift(pCurrentInPort->pInPortAvSync->pAVSyncLib, unDataRcvdInUsec, pInputBuf->ullTimeStamp);
					pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedDrift = TRUE;
				}

				return ADSP_EOK;
			}
			else if(DROP == rendering_decision)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Past TS, drop buf", me->mtMxID, unInPortID);

				//Update timestamps
				pCurrentInPort->ullTimeStampAtCopy.int_part = pInputBuf->ullTimeStamp;
				pCurrentInPort->ullTimeStampAtCopy.frac_part = 0;
				pCurrentInPort->ullTimeStampAtAccumulation.int_part = pInputBuf->ullTimeStamp;
				pCurrentInPort->ullTimeStampAtAccumulation.frac_part = 0;
				pCurrentInPort->ullTimeStampProcessed.int_part = pInputBuf->ullTimeStamp;
				pCurrentInPort->ullTimeStampProcessed.frac_part = 0;

				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu InputTS [%lu, %lu], TSValidity: %d", me->mtMxID, unInPortID,
						(uint32_t)(pInputBuf->ullTimeStamp>>32),(uint32_t)(pInputBuf->ullTimeStamp), pCurrentInPort->bIsTimeStampValid);

				//No holding required.
				(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync

				//Past TS, Drop the buffer.
				if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to release i/p buffer back to upstr svc!\n");
				}

				pCurrentInPort->numBufReturned++;
				pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;

				//Commit the AVSync update changes for this i/p port for this dropped buffer immediately.
				ADSPResult commit_result = avsync_lib_commit_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib);
				if (ADSP_FAILED(commit_result) && ADSP_EBADPARAM != commit_result)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: in i/p port %lu context, failed to commit AVSync stats with result=%d", me->mtMxID, unInPortID, commit_result);
				}

				return ADSP_EFAILED;
			}
			else// if HOLD ==  rendering decision
			{
				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Future TS, hold buf for MSW: %lu LSW: %lu usec",
						me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));
				//If hold duration is less than the 1ms, treat it as immediate.
				//Update expected ST.
				avsync_lib_increment_expected_session_clock(pCurrentInPort->pInPortAvSync->pAVSyncLib,unDataRcvdInUsec);
				//i/p port is in a held state.
				pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER;
				pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER;

				//Stop further processing from this port.
				MtMx_RemoveInputPortFromWaitMask(me, unInPortID);

				//Out of sync.
				(void)MtMx_ResetStrClkSync(me, unInPortID); //AV-Sync

				return ADSP_EFAILED;
			}
		}
	}

	return ADSP_EOK;
}

ADSPResult MtMx_AddInterpolatedSamples(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];

	// Pointer to current sample in upstream peer output buffer and write location in local buffer
	int8_t *pRdLoc = pCurrentInPort->pCurrentSample;
	int8_t *pWrLoc = pCurrentInPort->pWrLoc;

	uint32_t num_bytes_to_interpolate_per_ch = (MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER * pCurrentInPort->unBytesPerSample);
	uint32_t num_bytes_per_ch_in_port_buf = (pCurrentInPort->unInPortPerChBufSize * pCurrentInPort->unBytesPerSample);
	uint32_t in_buf_ch_spacing_in_bytes = (pCurrentInPort->nNumRemainingSamplesPerCh + pCurrentInPort->nNumSamplesUsedPerCh) \
			* pCurrentInPort->unBytesPerSample;

	for (uint16_t j = 0; j < pCurrentInPort->unNumChannels; j++)
	{
		memscpy((void*)pWrLoc, num_bytes_to_interpolate_per_ch,(void*)pRdLoc, num_bytes_to_interpolate_per_ch);
		pRdLoc += in_buf_ch_spacing_in_bytes;
		pWrLoc += num_bytes_per_ch_in_port_buf;
	}

	pCurrentInPort->pInPortAvSync->bShouldSessionTimeBeAdjWhenSendingBufDown = TRUE;
	pCurrentInPort->pWrLoc += num_bytes_to_interpolate_per_ch;

	return ADSP_EOK;
}

ADSPResult MtMx_DropSamples(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];
	pCurrentInPort->pCurrentSample += (MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER * pCurrentInPort->unBytesPerSample);
	pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf -= MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER;
	pCurrentInPort->nNumRemainingSamplesPerCh -= MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER;
	pCurrentInPort->nNumSamplesUsedPerCh += MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER;
	pCurrentInPort->pInPortAvSync->bShouldSessionTimeBeAdjWhenSendingBufDown = TRUE;
	return ADSP_EOK;
}

ADSPResult MxAr_OutPortToHonorInPortsTimestamps(This_t *me, uint32_t unOutPortID)
{
	int32_t                 unInPortID;
	MatrixInPortInfoType    *pCurrentInPort;
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	uint32_t                inPortsMask = pCurrentOutPort->inPortsMask;
	elite_msg_data_buffer_t*   pInputBuf;

	uint32_t bufDelay = pCurrentOutPort->unFrameDurationInUsec.int_part * pCurrentOutPort->unNumOutputBufs;
	uint32_t ullCurrentDownstreamDelay = *(pCurrentOutPort->punAFEDelay) + *(pCurrentOutPort->punCoppBufDelay) + *(pCurrentOutPort->punCoppAlgDelay) + bufDelay;

	while (inPortsMask)
	{
		unInPortID = Q6_R_ct0_R(inPortsMask);
		inPortsMask ^= 1 << unInPortID;
		pCurrentInPort = me->inPortParams[unInPortID];

		if (unOutPortID == pCurrentInPort->unTopPrioOutPort)
		{
			avsync_lib_update_stc_clock(pCurrentInPort->pInPortAvSync->pAVSyncLib);

			if((FALSE == pCurrentInPort->bHasFirstSampleAccumulated) &&
					(INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER != pCurrentInPort->inPortState) &&
					(INPUT_PORT_STATE_WAITING_TO_BE_READY != pCurrentInPort->inPortState) &&
					(INPUT_PORT_STATE_WAITING_TO_BE_ACTIVE != pCurrentInPort->inPortState))
			{
				continue;
			}

			avsync_rendering_decision_t rendering_decision = RENDER;
			avsync_lib_make_simple_rendering_decision(pCurrentInPort->pInPortAvSync->pAVSyncLib, pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part, &rendering_decision);

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

					if(pCurrentInPort->pInPortAvSync->bShouldSessionTimeBeAdjWhenSendingBufDown)
					{
						pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj--;
						if (0 == pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj)
						{
							pCurrentInPort->pInPortAvSync->bIsSampleAddDropEnabled = FALSE;
							pCurrentInPort->pInPortAvSync->bShouldSessionTimeBeAdjWhenSendingBufDown = FALSE;
						}
					}

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

					//Now, that the absolute time and session time are updated, commit the AVSync update changes for this i/p port.
					ADSPResult commit_result = avsync_lib_commit_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib);
					if (ADSP_FAILED(commit_result) && (ADSP_EBADPARAM != commit_result))
					{
						MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: in o/p port %lu context, i/p port %lu failed to commit AVSync stats with result=%d", me->mtMxID, unOutPortID, unInPortID, commit_result);
					}
				}

				//For cases where stream clk is not yet sync'd with session clk, and i/p buffers are being dropped because their TS's are in the past,
				//increment the expected str_time by the amount of samples rendered down in the form of silence
				// if session time > expected stream time, expected stream time = session time
				if (!pCurrentInPort->pInPortAvSync->bIsStrClkSyncdWithSessClk)
				{
					uint64_t ullStaleEOSSamplesuSec = MT_MX_SAMPLES_TO_USEC(pCurrentInPort->ullStaleEosSamples, pCurrentInPort->unSampleRate);
					avsync_lib_increment_expected_session_clock(pCurrentInPort->pInPortAvSync->pAVSyncLib, (pCurrentInPort->unFrameDurationInUsec.int_part - ullStaleEOSSamplesuSec));
				}

				pCurrentInPort->ullStaleEosSamples = 0;
				break;
			}
			case INPUT_PORT_STATE_WAITING_TO_BE_READY:
			{
				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Entering Hold Dur [%lu %lu] usec",
						me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));

				if (RENDER == rendering_decision)
				{
					pCurrentInPort->inPortState = INPUT_PORT_STATE_READY;
					pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_READY;
					MtMx_AddInputPortToWaitMask(me, unInPortID);
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu moving i/p port %lu to ready state", me->mtMxID, unOutPortID, unInPortID);
				}
				else
				{
					// Need to use output port frame duration as input port media type is not received yet
					mt_mx_decrement_time(&pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec, pCurrentOutPort->unFrameDurationInUsec);
				}

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Exiting Hold Dur [%lu %lu] usec",
						me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));

				break;
			}

			case INPUT_PORT_STATE_WAITING_TO_BE_ACTIVE:
			{
				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Entering Hold Dur [%lu %lu] usec",
						me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));

				if (RENDER == rendering_decision)
				{
					pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE;
					pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE;
					MtMx_InPortToUpdateWaitMask(me, unInPortID);
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu moving i/p port %lu to active state", me->mtMxID, unOutPortID, unInPortID);
				}
				else
				{
					mt_mx_decrement_time(&pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec, pCurrentOutPort->unFrameDurationInUsec);
				}

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Exiting Hold Dur [%lu %lu] usec",
						me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));

				break;
			}

			case INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER:
			{
				//Update session time and absolute time
				MxArUpdateInputPortSessionTime(me, unInPortID, pCurrentInPort, pCurrentOutPort, ullCurrentDownstreamDelay);

				pInputBuf = pCurrentInPort->pInputBuf;
				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Entering Hold Dur [%lu %lu],frame duration %lu usec",
						me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part),
						(uint32_t)pCurrentInPort->unFrameDurationInUsec.int_part);

				//Now, that the absolute time and session time are updated, commit the AVSync update changes for this i/p port.
				ADSPResult commit_result = avsync_lib_commit_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib);
				if (ADSP_FAILED(commit_result) && NULL!=pCurrentInPort->pInPortAvSync->pAVSyncLib)
				{
					MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: in o/p port %lu context, i/p port %lu failed to commit AVSync stats with result=%d", me->mtMxID, unOutPortID, unInPortID, commit_result);
				}

				//Adjust ullInBufHoldDuration if less than 1ms
				if(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part < MT_MX_FRAME_DURATION_1000US)
				{
					(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
				}

				if (pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part <= pCurrentInPort->unFrameDurationInUsec.int_part)
				{
					//Update session time and expected stream time
					avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,SESSION_CLOCK,&pInputBuf->ullTimeStamp,sizeof(pInputBuf->ullTimeStamp));
					avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,EXPECTED_SESSION_CLOCK,&pInputBuf->ullTimeStamp,sizeof(pInputBuf->ullTimeStamp));

					//I/p port transition back to ACTIVE
					pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE;
					pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE;
					MtMx_InPortToUpdateWaitMask(me, unInPortID);

					//Trigger i/p port processing by honoring i/p TS
					MtMx_InPortToHonorTimestamp(me, unInPortID);

					if(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part > 0)
					{
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu calling MtMx_MsgDataBufferHold from o/p port", me->mtMxID, unInPortID);
						(void)MtMx_MsgDataBufferHold(me, unInPortID, pCurrentInPort);
					}
					else
					{
						//Explictly set the hold duration to 0
						(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu calling MtMx_MsgDataBuffer from o/p port", me->mtMxID, unInPortID);
						MtMx_MsgDataBuffer(me, unInPortID, pCurrentInPort);
					}

					//Mark this input port as just moved to ACTIVE (to skip the EOS and Underflow handling, that follow this function on the outporthandler)
					pCurrentInPort->bInputPortJustMovedFromHeldToActive = TRUE;

					//Now, sync is complete
					pCurrentInPort->pInPortAvSync->bIsStrClkSyncdWithSessClk = TRUE;
				}
				else if(FALSE == pCurrentInPort->bIsLocalBufEmpty)
				{
					//if(pCurrentInPort->ullInBufHoldDuration >= pCurrentInPort->unFrameDurationInUsec) && (FALSE == pCurrentInPort->bIsLocalBufEmpty)

					//In the normal hold case, there maybe some residual data from previous buffer sitting in the internal buffer
					//of the audio matrix input port. In such a case, it would be better to send the rest of the previous data
					//out before proceeding with the hold duration for the current buffer, to avoid an "island" type glitch.
					(void)MtMx_HoldCommonRoutine(me, unInPortID, pCurrentInPort);
				}
				else //if(pCurrentInPort->ullInBufHoldDurationInSamples >= pCurrentInPort->unInPortPerChBufSize)
				{
					mt_mx_decrement_time(&pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec, pCurrentInPort->unFrameDurationInUsec);
				}

				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: Exiting Hold Dur [%lu %lu] usec",
						me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));

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

					if(pCurrentInPort->pInPortAvSync->bShouldSessionTimeBeAdjWhenSendingBufDown)
					{
						pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj--;
						if (0 == pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj)
						{
							pCurrentInPort->pInPortAvSync->bIsSampleAddDropEnabled = FALSE;
							// session clock adjustment has completed, output port can go back to incrementing session clock normally
							pCurrentInPort->pInPortAvSync->bShouldSessionTimeBeAdjWhenSendingBufDown = FALSE;
						}
					}

					//Update timestamps
					if(pCurrentInPort->bIsTimeStampValid)
					{
						mt_mx_copy_time_value(&pCurrentInPort->ullTimeStampProcessed, pCurrentInPort->ullTimeStampAtAccumulation);
					}

					//Now, that the absolute time and session time are updated, commit the AVSync update changes for this i/p port.
					ADSPResult commit_result = avsync_lib_commit_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib);
					if (ADSP_FAILED(commit_result) && NULL!=pCurrentInPort->pInPortAvSync->pAVSyncLib)
					{
						MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: in o/p port %lu context, i/p port %lu failed to commit AVSync stats with result=%d", me->mtMxID, unOutPortID, unInPortID, commit_result);
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

ADSPResult MtMx_InPortToCheckReInitSampleSlipLibrary(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	mt_mx_sampleslip_t      *pSampleSlip     = &(pCurrentInPort->pInPortAvSync->structSampleSlip);
	ADSPResult                  result                        = ADSP_EOK;

	if (NULL == pSampleSlip->pSampleSlipCapiv2)
	{
		//First time allocation. i.e Init. Get the size of SampleSlip lib.
		capi_v2_init_memory_requirement_t SampleSlipCapiV2Size = {0};

		//Get Capiv2 Init memory requirement.

		capi_v2_prop_t initMemReqProp =
		{
				CAPI_V2_INIT_MEMORY_REQUIREMENT,
				{ (int8_t*)&SampleSlipCapiV2Size,    sizeof(SampleSlipCapiV2Size), sizeof(SampleSlipCapiV2Size) },
				{ false,false,0 }
		};
		capi_v2_proplist_t propList = {1, &initMemReqProp};

		result = capi_v2_err_to_adsp_result(capi_v2_sample_slip_get_static_properties(NULL,&propList));

		if(ADSP_EOK != result)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx: Sample Slip CapiV2 getsize returned error %d", result);
			return result;
		}

		pSampleSlip->pSampleSlipCapiv2 = (capi_v2_t *)qurt_elite_memory_malloc(SampleSlipCapiV2Size.size_in_bytes, QURT_ELITE_HEAP_DEFAULT);
		if (NULL == pSampleSlip->pSampleSlipCapiv2)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, SampleSlip malloc failed. Size: %lu. Cont w/o lib.",
					me->mtMxID, unInPortID, SampleSlipCapiV2Size.size_in_bytes);
			result = ADSP_ENOMEMORY;
			return result;
		}
		else
		{
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, SampleSlip malloc success. Ptr: 0x%p, Size: %lu",
					me->mtMxID, unInPortID, pSampleSlip->pSampleSlipCapiv2, SampleSlipCapiV2Size.size_in_bytes);
		}

		capi_v2_event_callback_info_t cbInfo = {MtMx_SampleSlipCallbackHandler, (void *)pSampleSlip};

		capi_v2_prop_t cbProp[] = {
				{
					CAPI_V2_EVENT_CALLBACK_INFO,
					{ reinterpret_cast<int8_t*>(&cbInfo), sizeof(cbInfo), sizeof(cbInfo) },
					{ false, false, 0}
				}
		};
		capi_v2_proplist_t cbPropList = { sizeof(cbProp)/sizeof(cbProp[0]), cbProp };

		// If mediafmt is not received yet, initialize with default values
		capiv2_mediafmt_t InpMediaFmt =
		{
			{
				CAPI_V2_FIXED_POINT
			},
			{
				CAPI_V2_DATA_FORMAT_INVALID_VAL,
				(pCurrentInPort->unNumChannels == 0)? MT_MX_NUM_CHANNELS_TWO:pCurrentInPort->unNumChannels,
				(pCurrentInPort->unBitwidth == 0)? MT_MX_DEFAULT_BIT_WIDTH:pCurrentInPort->unBitwidth,
				CAPI_V2_DATA_FORMAT_INVALID_VAL,
				(pCurrentInPort->unSampleRate == 0)? MT_MX_SAMPLING_RATE_48000:pCurrentInPort->unSampleRate,
				1,
				CAPI_V2_DEINTERLEAVED_UNPACKED,
				{ 0 }
			}
		};

		if(pCurrentInPort->unNumChannels == 0)
		{
			InpMediaFmt.f.channel_type[0] = PCM_CHANNEL_L;
			InpMediaFmt.f.channel_type[1] = PCM_CHANNEL_R;
		}
		else
		{
		 for(uint16_t i = 0; i < pCurrentInPort->unNumChannels; i++)
		 {
			 InpMediaFmt.f.channel_type[i] = pCurrentInPort->unChannelMapping[i];
		 }
		}
		capi_v2_prop_t setProp[] =   {
			{
				CAPI_V2_INPUT_MEDIA_FORMAT,
				{ (int8_t*)&InpMediaFmt, sizeof(InpMediaFmt), sizeof(InpMediaFmt) },
				{ true,true,0 }
			}
		};

		capi_v2_proplist_t setPropList = {sizeof(setProp)/sizeof(setProp[0]), setProp};

		result = capi_v2_err_to_adsp_result(capi_v2_sample_slip_init(pSampleSlip->pSampleSlipCapiv2, &cbPropList));
		if (ADSP_EOK != result)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, SampleSlip module init failed with result: %x. Cont w/o lib.",
					me->mtMxID, unInPortID, result);
			if (NULL != pSampleSlip->pSampleSlipCapiv2)
			{
				MTMX_FREE(pSampleSlip->pSampleSlipCapiv2);
			}
			return result;
		}

		result = capi_v2_err_to_adsp_result(pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->set_properties(pSampleSlip->pSampleSlipCapiv2,&setPropList));
		if (ADSP_EOK != result)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, SampleSlip module Re-init failed with result: %x. Cont w/o lib.",
					me->mtMxID, unInPortID, result);
			if (NULL != pSampleSlip->pSampleSlipCapiv2)
			{
			 (void)pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->end(pSampleSlip->pSampleSlipCapiv2);
				MTMX_FREE(pSampleSlip->pSampleSlipCapiv2);
			}
			return result;
		}

	}
	else
	{
		capiv2_mediafmt_t InpMediaFmt =
		{
			{
				CAPI_V2_FIXED_POINT
			},
			{
				CAPI_V2_DATA_FORMAT_INVALID_VAL,
				(pCurrentInPort->unNumChannels == 0)? MT_MX_NUM_CHANNELS_TWO:pCurrentInPort->unNumChannels,
				(pCurrentInPort->unBitwidth == 0)? MT_MX_DEFAULT_BIT_WIDTH:pCurrentInPort->unBitwidth,
				CAPI_V2_DATA_FORMAT_INVALID_VAL,
				(pCurrentInPort->unSampleRate == 0)? MT_MX_SAMPLING_RATE_48000:pCurrentInPort->unSampleRate,
				1,
				CAPI_V2_DEINTERLEAVED_UNPACKED,
				{ 0 }
			}
		};

		for(uint16_t i = 0; i < pCurrentInPort->unNumChannels; i++)
		{
			InpMediaFmt.f.channel_type[i] = pCurrentInPort->unChannelMapping[i];
		}
		capi_v2_prop_t setProp[] =   {
			{
				CAPI_V2_INPUT_MEDIA_FORMAT,
				{ (int8_t*)&InpMediaFmt, sizeof(InpMediaFmt), sizeof(InpMediaFmt) },
				{ true,true,0 }
			}
		};

		capi_v2_proplist_t setPropList = {sizeof(setProp)/sizeof(setProp[0]), setProp};
		result = capi_v2_err_to_adsp_result(pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->set_properties(pSampleSlip->pSampleSlipCapiv2,&setPropList));
		if (ADSP_EOK != result)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, SampleSlip module Re-init failed with result: %x. Cont w/o lib.",
					me->mtMxID, unInPortID, result);
			if (NULL != pSampleSlip->pSampleSlipCapiv2)
			{
			 (void)pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->end(pSampleSlip->pSampleSlipCapiv2);
				MTMX_FREE(pSampleSlip->pSampleSlipCapiv2);
			}
			return result;
		}
	}
	return result;
}

ADSPResult MtMx_InPortSetS2DPtrSampleSlipLibrary(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	mt_mx_sampleslip_t      *pSampleSlip     = &(pCurrentInPort->pInPortAvSync->structSampleSlip);
	ADSPResult              result           = ADSP_EOK;

	if(NULL != pSampleSlip->pSampleSlipCapiv2)
	{
		capi_v2_buf_t param_buf;

		audproc_sampleslip_drift_pointer_t sampleslip_device_drift_ptr = {0};
		param_buf.data_ptr = (int8_t *)&sampleslip_device_drift_ptr;
		param_buf.actual_data_len = param_buf.max_data_len = sizeof(sampleslip_device_drift_ptr);

		//Obtain the stream to primary device drift pointer
		sd_dc_t *s2dLibPtr = NULL;
		avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,DEVICE_DRIFT_PTR,&s2dLibPtr);
		const volatile int32_t* pnStreamToDeviceDriftPtr = sd_drift_calc_get_drift_pointer(s2dLibPtr);


		sampleslip_device_drift_ptr.primary_drift_info_ptr = NULL;
		sampleslip_device_drift_ptr.current_drift_info_ptr = NULL;
		sampleslip_device_drift_ptr.stream_to_device_drift_info_ptr = pnStreamToDeviceDriftPtr;

		result = capi_v2_err_to_adsp_result(pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->set_param(pSampleSlip->pSampleSlipCapiv2, AUDPROC_PARAM_ID_SAMPLESLIP_DRIFT_POINTER, NULL, &param_buf));

		if (ADSP_EOK != result)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu SetParam on SampleSlip library failed with result: %x. Cont w/o lib.",
					me->mtMxID, unInPortID, result);
			if (NULL != pSampleSlip->pSampleSlipCapiv2)
			{
			 (void)pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->end(pSampleSlip->pSampleSlipCapiv2);
				MTMX_FREE(pSampleSlip->pSampleSlipCapiv2);
			}
			return result;
		}

		audproc_sampleslip_direction_t sampleslip_direction;
		param_buf.data_ptr = (int8_t *)&sampleslip_direction;
		param_buf.actual_data_len = param_buf.max_data_len = sizeof(sampleslip_direction);

		sampleslip_direction.direction = Playback;
		result = capi_v2_err_to_adsp_result(pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->set_param(pSampleSlip->pSampleSlipCapiv2, AUDPROC_PARAM_ID_SAMPLESLIP_DIRECTION, NULL, &param_buf));

		if (ADSP_EOK != result)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu SetParam on SampleSlip library failed with result: %x. Cont w/o lib.",
					me->mtMxID, unInPortID, result);
			if (NULL != pSampleSlip->pSampleSlipCapiv2)
			{
			 (void)pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->end(pSampleSlip->pSampleSlipCapiv2);
				MTMX_FREE(pSampleSlip->pSampleSlipCapiv2);
			}
			return result;
		}

		//Set enable paramater
		audproc_sampleslip_enable_t sampleslip_enable;
		param_buf.data_ptr = (int8_t *)&sampleslip_enable;
		param_buf.actual_data_len = param_buf.max_data_len = sizeof(sampleslip_enable);
		sampleslip_enable.enable= 1;
		result = capi_v2_err_to_adsp_result(pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->set_param(pSampleSlip->pSampleSlipCapiv2, AUDPROC_PARAM_ID_SAMPLESLIP_ENABLE, NULL, &param_buf));

		if (ADSP_EOK != result)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu Enable SetParam on SampleSlip library failed with result: %lu.",
					me->mtMxID, unInPortID, result);

			return result;
		}

		//update internal buffer size parameter
		audproc_sampleslip_internal_buffer_size_t  sampleslip_buf_size_payload;
		param_buf.data_ptr = (int8_t *)&sampleslip_buf_size_payload;
		param_buf.actual_data_len = param_buf.max_data_len = sizeof(sampleslip_buf_size_payload);
		(pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->get_param(pSampleSlip->pSampleSlipCapiv2, AUDPROC_PARAM_ID_SAMPLESLIP_INTERNAL_BUFFER_SIZE, NULL, &param_buf));
		pSampleSlip->unBufSizeInSamples = sampleslip_buf_size_payload.buffer_size_in_samples;

		//update internal sample slip buffers offset values
		audproc_sampleslip_delayline_offset_value_t  sampleslip_delayline_offset_payload;
		param_buf.data_ptr = (int8_t *)&sampleslip_delayline_offset_payload;
		param_buf.actual_data_len = param_buf.max_data_len = sizeof(sampleslip_delayline_offset_payload);
		pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->get_param(pSampleSlip->pSampleSlipCapiv2, AUDPROC_PARAM_ID_SAMPLESLIP_DELAYLINE_OFFSET_VALUE, NULL, &param_buf);
		pSampleSlip->inBufOffset = sampleslip_delayline_offset_payload.delayline_in_offset_value;
		pSampleSlip->outBufOffset = sampleslip_delayline_offset_payload.delayline_out_offset_value;
	}
	return result;
}

ADSPResult MtMx_InPortResetSampleSlipLibrary(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	mt_mx_sampleslip_t      *pSampleSlip     = &(pCurrentInPort->pInPortAvSync->structSampleSlip);
	ADSPResult              result           = ADSP_EOK;

	if(NULL != pSampleSlip->pSampleSlipCapiv2)
	{
		//Reset the sampleslip library
		//reset algorithm
		// The reset set property does not need any parameters.
		capi_v2_prop_t prop;
		prop.id = CAPI_V2_ALGORITHMIC_RESET;
		prop.payload.actual_data_len = 0;
		prop.payload.max_data_len = 0;
		prop.payload.data_ptr = NULL;
		prop.port_info.is_valid = FALSE;
		capi_v2_proplist_t proplist;
		proplist.props_num = 1;
		proplist.prop_ptr = &prop;

		result = capi_v2_err_to_adsp_result(pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->set_properties(pSampleSlip->pSampleSlipCapiv2, &proplist));
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Reconfing Input port: Sampleslip module reset cmd returned with status %d", me->mtMxID, (int)result);
	}
	return result;
}

ADSPResult MtMx_CheckSSLibStatus_3(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];

	//Enable the stream-to-device ratematching using SampleSlip module
	//1. if non-default clock recovery mode is set as the input media type is available at this point (OR)
	//2. Adj Session Clock Fine control mode is enabled.
	uint32_t current_clk_recovery_mechanism = ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT;
	avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, CLOCK_RECOVERY_TYPE, &current_clk_recovery_mechanism);
	if(((ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT != current_clk_recovery_mechanism) && (NULL != pCurrentInPort->pStartLoc)) || (pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled))
	{
		(void)MtMx_InPortToCheckReInitSampleSlipLibrary(me, unInPortID); //AV-Sync
		(void)MtMx_InPortSetS2DPtrSampleSlipLibrary(me, unInPortID);
		MtMx_CalculateStreamDelay(me, unInPortID);
	}

	return ADSP_EOK;
}

ADSPResult MtMx_CheckSSLibStatus_4(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	mt_mx_sampleslip_t *pSampleSlip = &(pCurrentInPort->pInPortAvSync->structSampleSlip);

	if(NULL != pSampleSlip->pSampleSlipCapiv2)
	{
		return ADSP_EOK;
	}
	else
	{
		return ADSP_EFAILED;
	}
}

ADSPResult MxAr_AVSyncSetAFEDriftPtrInputDerivedFromOutput(This_t *me)
{
	uint32_t unInPortID;
	MatrixInPortInfoType *pCurrentInPort;
	MatrixOutPortInfoType *pCurrentOutPort;

	for (unInPortID = 0; unInPortID <= me->maxInPortID; unInPortID++)
	{
		if((NULL != me->inPortParams[unInPortID]) && (INPUT_PORT_STATE_INACTIVE != me->inPortParams[unInPortID]->inPortState))
		{
			pCurrentInPort = me->inPortParams[unInPortID];

			//Get top priority port
			uint32_t unOutPortID = pCurrentInPort->unTopPrioOutPort;
			if((unOutPortID < MT_MX_MAX_OUTPUT_PORTS) && (NULL != me->outPortParams[unOutPortID]))
			{
				pCurrentOutPort = me->outPortParams[unOutPortID];

				//Set the s2pd drift pointer here
				if(NULL == pCurrentOutPort->pOutPortAvSync->pAfeDriftPtr)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MtMx #%lu: i/p port %lu, Top prio o/p port %lu AFE Drift Ptr is NULL", me->mtMxID, unInPortID, unOutPortID);
				}
				else
				{
					MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MtMx #%lu: i/p port %lu, Top prio o/p port %lu AFE Drift Ptr %p", me->mtMxID, unInPortID, unOutPortID, pCurrentOutPort->pOutPortAvSync->pAfeDriftPtr);
					avsync_lib_set_device_drift_pointer(pCurrentInPort->pInPortAvSync->pAVSyncLib, &(pCurrentOutPort->pOutPortAvSync->pAfeDriftPtr->avt_drift_info));
				}
				//Since the s2pd drift pointer got potentially re-set here, call the re-init on the SampleSlip library to set this, if applicable.
				uint32_t current_clk_recovery_mechanism = ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT;
				avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, CLOCK_RECOVERY_TYPE, &current_clk_recovery_mechanism);
				//Since the s2pd drift pointer got potentially re-set here, call the re-init on the SampleSlip library to set this, if applicable.
				if(((ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT != current_clk_recovery_mechanism) &&
						(NULL != pCurrentInPort->pStartLoc)) ||(pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled))
				{
					(void)MtMx_InPortToCheckReInitSampleSlipLibrary(me, unInPortID); //AV-Sync
					(void)MtMx_InPortSetS2DPtrSampleSlipLibrary(me, unInPortID);
				}
			}
		}
	}

	return ADSP_EOK;
}

ADSPResult MtMx_HandleSampleAddDrops_1(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];

	if (pCurrentInPort->pInPortAvSync->bIsSampleAddDropEnabled && MT_MX_INPUT_PORT_DROP_SAMPLES == pCurrentInPort->pInPortAvSync->samplesAddOrDropMask)
	{
		pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = (pCurrentInPort->unInPortPerChBufSize + MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER);
		pCurrentInPort->pInPortAvSync->bIsThisFirstSampleAddOrDrop = FALSE;
	}
	else if (pCurrentInPort->pInPortAvSync->bIsSampleAddDropEnabled && MT_MX_INPUT_PORT_ADD_SAMPLES == pCurrentInPort->pInPortAvSync->samplesAddOrDropMask)
	{
		pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = (pCurrentInPort->unInPortPerChBufSize - MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER);
		pCurrentInPort->pInPortAvSync->bIsThisFirstSampleAddOrDrop = FALSE;
	}
	else
	{
		pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;
	}

	return ADSP_EOK;
}

ADSPResult MtMx_HandleSampleAddDrops_2(This_t *me, uint32_t unInPortID)
{
	// If Adjust Session Clk is enabled, change the number of samples that
	// should be read from the input buffer to fill the local buffer.
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	if (pCurrentInPort->pInPortAvSync->bIsSampleAddDropEnabled)
	{
		if (MT_MX_INPUT_PORT_ADD_SAMPLES == pCurrentInPort->pInPortAvSync->samplesAddOrDropMask)
		{
			if (TRUE == pCurrentInPort->pInPortAvSync->bIsThisFirstSampleAddOrDrop)
			{
				pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf -= MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER;
				pCurrentInPort->pInPortAvSync->bIsThisFirstSampleAddOrDrop = FALSE;
			}
		}
		else if(MT_MX_INPUT_PORT_DROP_SAMPLES == pCurrentInPort->pInPortAvSync->samplesAddOrDropMask)
		{
			if (TRUE == pCurrentInPort->pInPortAvSync->bIsThisFirstSampleAddOrDrop)
			{
				pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf += MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER;
				pCurrentInPort->pInPortAvSync->bIsThisFirstSampleAddOrDrop = FALSE;
			}
		}
	}

	return ADSP_EOK;
}

ADSPResult MtMx_HandleSampleAddDrops_3(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];

	if(TRUE == pCurrentInPort->pInPortAvSync->bIsSampleAddDropEnabled)
	{
		switch(pCurrentInPort->pInPortAvSync->samplesAddOrDropMask)
		{
		case MT_MX_INPUT_PORT_ADD_SAMPLES:
		{
			(void)MtMx_AddInterpolatedSamples(me, unInPortID);
			break;
		}
		case MT_MX_INPUT_PORT_DROP_SAMPLES:
		{
			(void)MtMx_DropSamples(me, unInPortID);
			break;
		}
		}
	}

	return ADSP_EOK;
}

ADSPResult MtMx_FillInPortLocalBuf(This_t *me, uint32_t unInPortID, uint32_t numSamplesReqPerCh)
{
	ADSPResult              result = ADSP_EOK;
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	mt_mx_sampleslip_t	    *pSampleSlip        = &(pCurrentInPort->pInPortAvSync->structSampleSlip);
	int8_t                  *pWrLocation = pCurrentInPort->pWrLoc;
	int8_t                  *pCurrentSample = pCurrentInPort->pCurrentSample;

	//Convert from samples to bytes.
	uint32_t  unInPortPerChBufSizeBytes = (pCurrentInPort->unBytesPerSample) * (pCurrentInPort->unInPortPerChBufSize);
	uint32_t  numBytesReqPerCh          = (pCurrentInPort->unBytesPerSample) * (numSamplesReqPerCh);
	uint32_t  unInpQNumBytesPerCh       = (pCurrentInPort->unBytesPerSample) * (pCurrentInPort->nNumRemainingSamplesPerCh + pCurrentInPort->nNumSamplesUsedPerCh);

	if (NULL != pSampleSlip->pSampleSlipCapiv2)
	{
		//Keep processing of the input data until local buf is full.
		while( (FALSE == pCurrentInPort->bIsLocalBufFull) && (pCurrentInPort->nNumRemainingSamplesPerCh > 0))
		{
			uint32_t i = 0, in_buf_samples, out_buf_samples;

			// Number of samples to process is minimum of available input data or left over empty space in Local output buffer
			uint32_t unMinSamples = ((int32_t)pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf <= pCurrentInPort->nNumRemainingSamplesPerCh) ?
					(pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf) : ((uint32_t)pCurrentInPort->nNumRemainingSamplesPerCh);

			in_buf_samples = out_buf_samples = unMinSamples;

			//when sample slip output internal buffer has empty then refill the buffer from the input data buffer.
			if(pSampleSlip->outBufOffset == 0)
			{

				in_buf_samples = ((int32_t)pSampleSlip->unBufSizeInSamples <= pCurrentInPort->nNumRemainingSamplesPerCh) ?
              (pSampleSlip->unBufSizeInSamples) : ((uint32_t)pCurrentInPort->nNumRemainingSamplesPerCh);

				out_buf_samples =0;
				//After the sample slip internal output buffer becomes empty(slip scenario), should refill the internal buffer first from input data buffer.
				//this offset should consider for the timestamp adjustment.
				pSampleSlip->adjustSamplesInUsec.int_part= -(MT_MX_SAMPLES_TO_USEC(in_buf_samples ,pCurrentInPort->unSampleRate));
				pSampleSlip->adjustSamplesInUsec.frac_part= 0;

			}

			//when sample slip input internal buffer has full then sendout those samples first.
			if(pSampleSlip->inBufOffset == pSampleSlip->unBufSizeInSamples)
			{
				in_buf_samples =0;

				out_buf_samples = (pSampleSlip->unBufSizeInSamples <= pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf) ?
              (pSampleSlip->unBufSizeInSamples) : (pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf);
				//After the sample slip internal input buffer becomes full(stuff scenario), should process out those samples first.
				//The extra samples shold consider for the timestamp adjustment.
				pSampleSlip->adjustSamplesInUsec.int_part = MT_MX_SAMPLES_TO_USEC(out_buf_samples ,pCurrentInPort->unSampleRate);
				pSampleSlip->adjustSamplesInUsec.frac_part = 0;

			}

			for (i = 0; i < pCurrentInPort->unNumChannels; i++)
			{
				pSampleSlip->inBufs[i].data_ptr          = pCurrentSample;
				pSampleSlip->inBufs[i].actual_data_len   = pCurrentInPort->unBytesPerSample * in_buf_samples;
				pSampleSlip->inBufs[i].max_data_len      = unInpQNumBytesPerCh;

				pSampleSlip->outBufs[i].data_ptr        = pWrLocation;
				pSampleSlip->outBufs[i].actual_data_len = 0;
				pSampleSlip->outBufs[i].max_data_len    = pCurrentInPort->unBytesPerSample * out_buf_samples;

				pCurrentSample += unInpQNumBytesPerCh;
				pWrLocation       += unInPortPerChBufSizeBytes;
			}

			//Copy the data from input buffer into the local buffer based on the stream-to-device drift.
			capi_v2_stream_data_t in_buf_list, out_buf_list;

			in_buf_list.bufs_num    = pCurrentInPort->unNumChannels;
			in_buf_list.buf_ptr     = pSampleSlip->inBufs;
			out_buf_list.bufs_num   = pCurrentInPort->unNumChannels;
			out_buf_list.buf_ptr    = pSampleSlip->outBufs;

			capi_v2_stream_data_t *input[] = { &in_buf_list };
			capi_v2_stream_data_t *output[] = { &out_buf_list };

			//Call Process function
			(void)pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->process(pSampleSlip->pSampleSlipCapiv2, input, output);

			//Increment read and write pointers
			pCurrentInPort->pCurrentSample += pSampleSlip->inBufs[0].actual_data_len;
			pCurrentInPort->pWrLoc += pSampleSlip->outBufs[0].actual_data_len;

			//Update state variables
			pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf -= ((pSampleSlip->outBufs[0].actual_data_len)/(pCurrentInPort->unBytesPerSample));
			pCurrentInPort->nNumRemainingSamplesPerCh -= ((pSampleSlip->inBufs[0].actual_data_len)/(pCurrentInPort->unBytesPerSample));
			pCurrentInPort->nNumSamplesUsedPerCh += ((pSampleSlip->inBufs[0].actual_data_len)/(pCurrentInPort->unBytesPerSample));

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

			//Update read and write pointers
			pWrLocation = pCurrentInPort->pWrLoc;
			pCurrentSample = pCurrentInPort->pCurrentSample;

			//update sample slip internal buffers samples offset value
			{
				capi_v2_buf_t param_buf;
				audproc_sampleslip_delayline_offset_value_t  sampleslip_delayline_offset_payload;
				param_buf.data_ptr = (int8_t *)&sampleslip_delayline_offset_payload;
				param_buf.actual_data_len = param_buf.max_data_len = sizeof(sampleslip_delayline_offset_payload);

				pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->get_param(pSampleSlip->pSampleSlipCapiv2, AUDPROC_PARAM_ID_SAMPLESLIP_DELAYLINE_OFFSET_VALUE, NULL, &param_buf);

				pSampleSlip->inBufOffset = sampleslip_delayline_offset_payload.delayline_in_offset_value;
				pSampleSlip->outBufOffset = sampleslip_delayline_offset_payload.delayline_out_offset_value;

			}

		} //end of while loop

		//check for the local buffer fullness and update the samples remaining to process.
		if((FALSE == pCurrentInPort->bIsLocalBufFull)&& (pCurrentInPort->nNumRemainingSamplesPerCh == 0))
		{
			uint32_t LocalBufOffsetInSamples = pCurrentInPort->unInPortPerChBufSize - pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf;
			pCurrentInPort->ullPreviousSamplesInUsec.int_part = MT_MX_SAMPLES_TO_USEC(LocalBufOffsetInSamples ,pCurrentInPort->unSampleRate);

			//In special scenarios like sample slip internal buffer becomes full or empty we need to consider the offset of samples for timestamp adjustment
			//but in case of slip scenario, there is a possibilty that local buffer is not full from the samples present in current input buffer then
			//not required to consider the offset in time stamp calculation since offset being considered in the ullPreviousSamplesInUsec.
			pSampleSlip->adjustSamplesInUsec.int_part = 0;
		}

		//update time stamps now when the local buffer has fully filled.
		if(TRUE == pCurrentInPort->bIsLocalBufFull)
		{
			uint32_t offset = pSampleSlip->inBufOffset + pSampleSlip->outBufOffset;
			pSampleSlip->totalBufOffsetInUsec.int_part = MT_MX_SAMPLES_TO_USEC(offset, pCurrentInPort->unSampleRate) ;
			pSampleSlip->totalBufOffsetInUsec.frac_part = 0;

			mt_mx_decrement_time(&pCurrentInPort->ullTimeStampAtCopy, pCurrentInPort->ullPreviousSamplesInUsec);
			mt_mx_decrement_time(&pCurrentInPort->ullTimeStampAtCopy, pSampleSlip->adjustSamplesInUsec);

			pCurrentInPort->ullPreviousSamplesInUsec.int_part =0;
			pSampleSlip->adjustSamplesInUsec.int_part=0;
		}

		return result;
	}

	//Normal operation. This is used for 1) Non-SampleSlip use case OR 2) Erroneous condition in SampleSlip usages.
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

ADSPResult MtMx_ClearHoldDuration(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	mt_mx_clear_time(&pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec);
	return ADSP_EOK;
}

ADSPResult MtMx_MsgDataBufferHold(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	elite_msg_data_buffer_t*     pInputBuf;
	pCurrentInPort->pInputBuf = (elite_msg_data_buffer_t*)((pCurrentInPort->myDataQMsg).pPayload);

	pInputBuf = pCurrentInPort->pInputBuf;
	pCurrentInPort->nNumRemainingSamplesPerCh = (pInputBuf->nActualSize /
			(pCurrentInPort->unNumChannels * pCurrentInPort->unBytesPerSample));
	pCurrentInPort->nNumSamplesUsedPerCh = 0;
	pCurrentInPort->pCurrentSample = (int8_t *)(&(pInputBuf->nDataBuf));

	// Logging the input PCM data to Matrix mixer service
	MtMx_LogPcmData(me, unInPortID, pCurrentInPort->pCurrentSample);

	/* Update timestamps only when the hold is about to elapse.
           This is needed to ensure the outgoing buffer TS during hold is based on increments
           When the hold is about to end, sync to the incoming buffer TS.
        */
	if(pCurrentInPort->bIsTimeStampValid && (pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part < pCurrentInPort->unFrameDurationInUsec.int_part ))
	{
		pCurrentInPort->ullTimeStampAtCopy.int_part = pCurrentInPort->ullIncomingBufferTS;
		pCurrentInPort->ullTimeStampAtCopy.frac_part = 0;
	}

	//Now, call the common hold routine
	(void)MtMx_HoldCommonRoutine(me, unInPortID, pCurrentInPort);

	return ADSP_EOK;
}

ADSPResult MtMx_HoldCommonRoutine(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	uint32_t unNumSampPerChZerosToPreFill = 0;
	uint32_t numSamplesReqPerCh;
	int64_t  holdTsAdjustinUsec = 0;

	if (pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part > 0)
	{
		//Calculate the number of samples/ch of zeros to pre-fill
		//Hold Dur (uS) / 1000 = Hold Dur in ms.
		//Hold Dur (ms) * #samp/ms/ch = #samp/ch
		unNumSampPerChZerosToPreFill = (pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part * pCurrentInPort->unSamplesPer1Msec)/(1000);

		MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu Hold Dur [%lu %lu], ZerosToFill %lu sam/ch samplesneeded %lu",
				me->mtMxID, unInPortID, (uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part>>32),
				(uint32_t)(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part), unNumSampPerChZerosToPreFill,pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf);

		/* For buffer timestamp adjustment in hold, there are three possible situations
		 *   a. When hold starts with residual data from previous buffer,
		 *      the outgoing TS needs to account for the post filled hold zeroes
		 *      w.r.t previous buffer TS
		 *   b. When number of hold zeroes = input frame size, do not account for any
		 *      special adjustment. The outgoing buffer TS would be in increments of
		 *      input frame duration.
		 *   c. When hold ends with hold < frame duration, then the new buffer TS
		 *      should sync data buffer being rendered and then adjust the TS
		 *      for pre-filled hold zeroes.
		 */

		//Pre-fill with hold duration worth of zeros (This will guaranteed to be < 1 i/p frame size worth by the caller).
		if(unNumSampPerChZerosToPreFill <= pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf)
		{
			//The below condition ensures that hold being handled only from the output port context entirely.
			//ignore the filling of local buffer and procesing the i/p buffer during i/p port non-active state.
			if(INPUT_PORT_STATE_ACTIVE != pCurrentInPort->inPortState)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu, trying to process the i/p buffer during inPortState:%lu and skipping",
				me->mtMxID, unInPortID, pCurrentInPort->inPortState );
				return ADSP_EOK;
			}
			//Most common use case. There is room in the existing internal buffer to accommodate all the hold zeros.
			(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
			numSamplesReqPerCh = unNumSampPerChZerosToPreFill;
			MtMx_FillInPortLocalBufHold(me, unInPortID, numSamplesReqPerCh);
			/* This use case is hit when the hold is about to end. In the same
			 *  buffer, valid data could be sent down.
			 *  Situations b & c apply
			 */
			if(unNumSampPerChZerosToPreFill != pCurrentInPort->unInPortPerChBufSize)
			{
			    holdTsAdjustinUsec = MT_MX_SAMPLES_TO_USEC(unNumSampPerChZerosToPreFill,pCurrentInPort->unSampleRate);
			}

		}
		else
		{
			uint32_t inPortSampleRate = MT_MX_SAMPLING_RATE_48000;
		        //Rare use case. The number of 'hold zeros' to fill is more than what the internal buffer can hold at this point.
			if(0 != pCurrentInPort->unSamplesPer1Msec)
			{
				pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part -= ((pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf * 1000) / (pCurrentInPort->unSamplesPer1Msec));
				inPortSampleRate = pCurrentInPort->unSampleRate;
			}
			else
			{
				//This use case should never occur.
				// Assume 48kHz sampling rate which gives 48 samples per 1ms
				pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part -= ((pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf * 1000) / (48));
			}
			numSamplesReqPerCh = pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf;
			MtMx_FillInPortLocalBufHold(me, unInPortID, numSamplesReqPerCh);

			/* If an entire buffer is full of hold zeroes, then we should not calculate the adjustment value
			  and honor the incremented TS */
			if(numSamplesReqPerCh != pCurrentInPort->unInPortPerChBufSize)
			{
			    holdTsAdjustinUsec = MT_MX_SAMPLES_TO_USEC(numSamplesReqPerCh,inPortSampleRate);
			}
			goto __cmnroutineafterfill;
		}
	}

	//Now, fill the remainder of internal buffer as usual.
	if (pCurrentInPort->nNumRemainingSamplesPerCh >= (int)pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf)
	{
		numSamplesReqPerCh = pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf;
	}
	else
	{
		//i/p buffer contains lesser samples than the size of local buffer (short buffer)
		numSamplesReqPerCh = pCurrentInPort->nNumRemainingSamplesPerCh;
		pCurrentInPort->bIsInBufShort = TRUE;
	}

	//Copy samples from i/p buffer to local buffer and update nNumRemainingSamplesPerCh
	MtMx_FillInPortLocalBuf(me, unInPortID, numSamplesReqPerCh);

	__cmnroutineafterfill:

	//Account for the offset in the last synced/updated buffer TS.
	pCurrentInPort->ullTimeStampAtCopy.int_part -= holdTsAdjustinUsec;
	if (FALSE == pCurrentInPort->bIsLocalBufFull && 0 == pCurrentInPort->nNumRemainingSamplesPerCh)
	{
		// The incoming buffer did not contain sufficient samples to fill local buffer
		// return the buffer and add the input port to the wait mask
		ADSPResult result;

		if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: failed to return i/p data buffer", me->mtMxID, unInPortID);
		}

		pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
		pCurrentInPort->numBufReturned++;
		MtMx_AddInputPortToWaitMask(me, unInPortID);

#ifdef MT_MX_EXTRA_DEBUG
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu i/p port %lu, #Buf Released: %lu, i/p port added to wait mask",
				me->mtMxID, unInPortID, pCurrentInPort->numBufReturned);
#endif

	}

	return ADSP_EOK;
}

ADSPResult MtMx_ProcessMsgDataBuffer(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];

	//If it has been determined that a Hold is required (< 1 i/p frame size), then handle appropriately
	if((pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part > 0) && (pCurrentInPort->unNewNumAfeFrames > MT_MX_LL_NUM_AFE_FRAMES))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu calling MtMx_MsgDataBufferHold from i/p port", me->mtMxID, unInPortID);
		(void)MtMx_MsgDataBufferHold(me, unInPortID, pCurrentInPort);
	}
	else
	{
		(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
		MtMx_MsgDataBuffer(me, unInPortID, pCurrentInPort); //Process the input buffer normally
	}

	return ADSP_EOK;
}

ADSPResult MxAr_CommonInPortProcessingRoutinePostSteps(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	ADSPResult result = ADSP_EOK;

	//At this point, check if there are any hold-zeros that are remaining to be filled.
	if(0 == pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part)
	{
		//Try to copy the timestamp of the current input buffer being proocessed offset from the beginning if not consumed any samples so far.
		//else increment the timestamp by framesize duration.
		if(pCurrentInPort->bIsTimeStampValid && pCurrentInPort->nNumRemainingSamplesPerCh > 0 && pCurrentInPort->nNumSamplesUsedPerCh == 0)
		{
			pCurrentInPort->ullTimeStampAtCopy.int_part = pCurrentInPort->ullIncomingBufferTS;
			pCurrentInPort->ullTimeStampAtCopy.frac_part = 0;
		}
		else
		{
			if(pCurrentInPort->bIsTimeStampValid)
			{
				mt_mx_increment_time(&pCurrentInPort->ullTimeStampAtCopy, pCurrentInPort->unFrameDurationInUsec);
			}
		}
		//Try refilling the i/p port's local buffer with new samples from upstream peer's o/p buffer
		if (pCurrentInPort->nNumRemainingSamplesPerCh <= (int)pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf)
		{
			//i/p buffer has insufficient samples to fill local buffer
			//Because of sample slip, there may be a use case where there might be a small # of samples still left over in the i/p Q, that needs to be copied over
			//i/p buffer has insufficient samples to fill local buffer, however call the fill local buffer routine again to exhaust any remainder samples in the i/p Q.
			while (pCurrentInPort->nNumRemainingSamplesPerCh > 0 && FALSE == pCurrentInPort->bIsLocalBufFull)
			{
				MtMx_FillInPortLocalBuf(me, unInPortID, pCurrentInPort->nNumRemainingSamplesPerCh);
			}

			//Now it is OK to return the buffer, as it has been established that the i/p Q buffer has truly been emptied, at this point.
			if (pCurrentInPort->nNumRemainingSamplesPerCh == 0)
			{
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

		}
		else
		{
			//i/p buffer has sufficient samples to fill local buffer
			if(pCurrentInPort->pInPortAvSync->bIsSampleAddDropEnabled)
			{
				//AV-Sync
				switch(pCurrentInPort->pInPortAvSync->samplesAddOrDropMask)
				{
				case MT_MX_INPUT_PORT_ADD_SAMPLES:
				{
					(void)MtMx_AddInterpolatedSamples(me, unInPortID);
					break;
				}
				case MT_MX_INPUT_PORT_DROP_SAMPLES:
				{
					(void)MtMx_DropSamples(me, unInPortID);
					break;
				}
				}
			}

			//copy samples from i/p buffer to local buffer and update nNumRemainingSamplesPerCh
			MtMx_FillInPortLocalBuf(me, unInPortID, pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf);
		}
	}
	else
	{
		//If hold duration is less than the rounding < 1ms, treat it as immediate.
		if (pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part < MT_MX_FRAME_DURATION_1000US)
		{
			(void)MtMx_ClearHoldDuration(me, unInPortID); //AV-Sync
		}

		//Update timestamps
		if (pCurrentInPort->bIsTimeStampValid)
		{
		    mt_mx_increment_time(&pCurrentInPort->ullTimeStampAtCopy, pCurrentInPort->unFrameDurationInUsec);
		}

		//Now, call the common hold routine
		(void)MtMx_HoldCommonRoutine(me, unInPortID, pCurrentInPort);
	}

	return result;
}

ADSPResult MtMx_ResetAvSyncStatsDrift(This_t *me, uint32_t unInPortID)
{
	//This buffer is yet to update AVSync Stats and Drift
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedStats = FALSE;
	pCurrentInPort->pInPortAvSync->bHasInputBufferUpdatedDrift = FALSE;
	return ADSP_EOK;
}

ADSPResult MtMx_ResetAvSyncSampleStats(This_t *me, uint32_t unInPortID)
{
	//Clear sample stats
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	pCurrentInPort->unDataRcvdInSamples = 0;
	pCurrentInPort->unDataRcvdInUsec.int_part = 0;
	pCurrentInPort->unDataRcvdInUsec.frac_part = 0;
	return ADSP_EOK;
}

ADSPResult MtMx_ResetStrClkSync(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	pCurrentInPort->pInPortAvSync->bIsStrClkSyncdWithSessClk = FALSE;
	return ADSP_EOK;
}

ADSPResult MtMx_SetupDefaultRenderWindow(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	pCurrentInPort->pInPortAvSync->bUseDefaultWindowForRendering = TRUE;
	return ADSP_EOK;
}

ADSPResult MtMx_SetDefaultRenderWindow(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	avsync_lib_check_set_default_rendering_window(pCurrentInPort->pInPortAvSync->pAVSyncLib, MT_MX_FRAME_DURATION_5000US);
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Render window and Default window changed to 5ms");
	return ADSP_EOK;
}

ADSPResult MtMx_SetDefaultRenderWindows(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	int64_t default_render_window = -MT_MX_FRAME_DURATION_5000US;
	avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, RENDER_WINDOW_START, &default_render_window, sizeof(int64_t));
	default_render_window = -default_render_window;
	avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, RENDER_WINDOW_END, &default_render_window, sizeof(int64_t));
	return ADSP_EOK;
}

ADSPResult MtMx_AdjustSessionClk(This_t *me, elite_msg_any_t *pMsg)
{
    ADSPResult                                   result = ADSP_EOK;
    uint32_t                                     unPort2AdjSessionClk;
    EliteMsg_CustomMtMxAdjustSessionClkType*     pPayload = (EliteMsg_CustomMtMxAdjustSessionClkType*)pMsg->pPayload;

    unPort2AdjSessionClk = pPayload->unPortID;
    MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (AdjSessClk) [port ID, port dir, adj time msw, lsw] = [%lu, %lu, %lu, %lu]",
                    me->mtMxID, pPayload->unSecOpCode, pPayload->unPortID, pPayload->unPortDirection,
                    (uint32_t)(pPayload->llAdjustTime >> 32), (uint32_t)(pPayload->llAdjustTime));

    uint32_t current_clk_recovery_mechanism = ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT;

    if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
    {
        result = ADSP_EUNSUPPORTED;
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (AdjSessClk) handler: Tx audio matrix does not support this cmd",
                        me->mtMxID, pPayload->unSecOpCode);
        goto __bailoutCmdAdjustSessionClk;
    }

    if (MATRIX_MIXER_PORT_DIR_INPUT == pPayload->unPortDirection)
    {
        MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unPort2AdjSessionClk];
        if ((NULL == pCurrentInPort) || (me->maxInPortID < unPort2AdjSessionClk || INPUT_PORT_STATE_INACTIVE == pCurrentInPort->inPortState))
        {
            result = ADSP_EBADPARAM;
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (AdjSessClk) handler: rcvd invalid/inactive port ID [%lu]",
                            me->mtMxID, pPayload->unSecOpCode, pPayload->unPortID);
            goto __bailoutCmdAdjustSessionClk;
        }
        avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, CLOCK_RECOVERY_TYPE, &current_clk_recovery_mechanism);

        if ((0  == pPayload->llAdjustTime) ||
                (ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_INDEPENDENT == current_clk_recovery_mechanism))
        {
            result = ADSP_EOK;
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (AdjSessClk) handler: rcvd command with zero adjust time or in independent clock recovery mode",
                            me->mtMxID, pPayload->unSecOpCode);
            pPayload->llActualAdjustTime = 0;
            pPayload->ullCmdLatency = 0;

            if(ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_INDEPENDENT == current_clk_recovery_mechanism)
            {
                pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled = FALSE;
            }
            goto __bailoutCmdAdjustSessionClk;
        }
        else
        {
            int64_t tempVar = (pPayload->llAdjustTime) * (pCurrentInPort->unSamplesPer1Msec);
            int8_t sign = ((tempVar < 0)? (-1): (1));
            tempVar *= sign;
            pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj = tempVar/1000;
            pPayload->llActualAdjustTime = 0;
            pPayload->ullCmdLatency = 0;

            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (AdjSessClk) handler: Adjust %lu samples",
                            me->mtMxID, pPayload->unSecOpCode, pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj);

            /* If adjust session clock has to be realized using Sample slip library */
            if(pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled)
            {
                pCurrentInPort->pInPortAvSync->llAdjDuration = pPayload->llAdjustTime;
                pCurrentInPort->pInPortAvSync->bIsSampleAddDropEnabled = FALSE;

                /* Query the frame size and interval duration from the Sample Slip CAPIv2 */

                mt_mx_sampleslip_t *pSampleSlip = &(pCurrentInPort->pInPortAvSync->structSampleSlip);
                if(NULL != pSampleSlip->pSampleSlipCapiv2)
                {
                    /* Obtain the stream to primary device drift pointer */
                    sd_dc_t *s2dLibPtr = NULL;
                    avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, DEVICE_DRIFT_PTR, &s2dLibPtr);

                    /* Update the adjustment to be realized and pass the pointer to the Sample Slip library */
                    int32_t drift_val = (int32_t)pPayload->llAdjustTime;
                    if(pPayload->llAdjustTime > (MAX_32))
                    {
                        drift_val = MAX_32;
                    }
                    else if(pPayload->llAdjustTime < (MIN_32))
                    {
                        drift_val = MIN_32;
                    }
                    sd_drift_update_given_drift(s2dLibPtr,drift_val); //RR: uint64 -> uint32 handle
                    MtMx_InPortSetS2DPtrSampleSlipLibrary(me, unPort2AdjSessionClk);

                    audproc_sampleslip_framesize_info_t ss_framesize_info;
                    capi_v2_buf_t param_buf;
                    param_buf.data_ptr = (int8_t *) &ss_framesize_info;
                    param_buf.actual_data_len = param_buf.max_data_len = sizeof(ss_framesize_info);

                    result = capi_v2_err_to_adsp_result(pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->get_param(pSampleSlip->pSampleSlipCapiv2, AUDPROC_PARAM_ID_SAMPLESLIP_FRAMESIZE_INFO, NULL, &param_buf));

                    if(ADSP_EOK != result)
                    {
                        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (AdjSessClk) handler: Sample Slip Get Param failed with %d",
                                             me->mtMxID, pPayload->unSecOpCode, result);
                        goto __bailoutCmdAdjustSessionClk;
                    }

                    pSampleSlip->frame_size = ss_framesize_info.frame_size;
                    pSampleSlip->adj_frame_interval = ss_framesize_info.adj_frame_interval;

                    if( 0 < pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj)
                    {
                    tempVar = pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj;
                    pPayload->llActualAdjustTime = sign * ((tempVar * 1000) / (pCurrentInPort->unSamplesPer1Msec));
                    pPayload->ullCmdLatency = ((tempVar * pSampleSlip->frame_size * pSampleSlip->adj_frame_interval)/(pCurrentInPort->unSamplesPer1Msec));
                    /* The sample slip frame size can differ from the in port frame size per 1ms.
                     * Handle the scaling appropriately in this calculation */
                }
                }

            }
            else
            {
                /* If Fine Adjustment via Sample Slip is not enabled, configure the coarse adjustment in the matrix */
                if (0 < pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj )
                {
                    /* Delaying/Advancing the session clock is implemented by adding/dropping samples */
                    pCurrentInPort->pInPortAvSync->bIsSampleAddDropEnabled = TRUE;
                    pCurrentInPort->pInPortAvSync->samplesAddOrDropMask = ((sign < 0)? (MT_MX_INPUT_PORT_ADD_SAMPLES) : (MT_MX_INPUT_PORT_DROP_SAMPLES));
                    pCurrentInPort->pInPortAvSync->bIsThisFirstSampleAddOrDrop = TRUE;

                tempVar = pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj;
                pPayload->llActualAdjustTime = (sign)*((tempVar * 1000) / (pCurrentInPort->unSamplesPer1Msec));
                pPayload->ullCmdLatency = (pCurrentInPort->pInPortAvSync->unNumRemSamplesAdj * pCurrentInPort->unFrameDurationInUsec.int_part);
            }
        }
        }

        MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] (AdjSessClk) handler: actual adj time MSW: %lu, LSW: %lu, cmd latency MSW: %lu, LSW: %lu",
                        me->mtMxID, pPayload->unSecOpCode, (uint32_t)(pPayload->llActualAdjustTime >> 32), (uint32_t)(pPayload->llActualAdjustTime),
                        (uint32_t)(pPayload->ullCmdLatency >> 32), (uint32_t)(pPayload->ullCmdLatency));
    }
    else
    {
        result = ADSP_EBADPARAM;
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (AdjSessClk) handler: rcvd invalid port dir [%lu]",
                        me->mtMxID, pPayload->unSecOpCode, pPayload->unPortDirection);
        goto __bailoutCmdAdjustSessionClk;
    }

    __bailoutCmdAdjustSessionClk:
    elite_msg_finish_msg(pMsg, result);
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (AdjSessClk) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
    return result;
}

ADSPResult MtMx_SetRateMatchingParamHandler(This_t *me, elite_msg_any_t *pMsg)
{
	ADSPResult                          result = ADSP_EOK;
	EliteMsg_CustomMtMxSetRateMatchingParamType*  pPayload = (EliteMsg_CustomMtMxSetRateMatchingParamType* )pMsg->pPayload;
	mtmx_ratematching_set_params RenderingDecisionParamPayload;
	EliteMsg_CustomMtMxGetSetParamType RateMatchingPayload;

	uint32_t unPort2SetParam = pPayload->unPortID;
	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Processing cmd %lu (SetRateMatchingParam) [port ID] = [%d, %d]",
			me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID, (int)pPayload->unPortDirection);

	if (MATRIX_MIXER_PORT_DIR_INPUT == pPayload->unPortDirection)
	{
		MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unPort2SetParam];

		//Port ID validity check
		if ((NULL == pCurrentInPort) || (me->maxInPortID < unPort2SetParam || INPUT_PORT_STATE_INACTIVE == pCurrentInPort->inPortState))
		{
			result = ADSP_EBADPARAM;
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetRateMatchingParam) handler: rcvd invalid/inactive port ID [%d]",
					me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortID);
			goto __bailoutCmdSetRateMatchingParamHandler;
		}

		RenderingDecisionParamPayload.pAddr= NULL;

		//The payload size is the size of the entire structure minus the size of pAddr and data_payload_size
		//4+4+2+2+sizeof(struct asm_session_mtmx_strtr_param_render_decision_t)+4+4+2+2+sizeof(struct asm_session_mtmx_strtr_param_clock_recovery_t);
		RenderingDecisionParamPayload.data_payload_size = sizeof(mtmx_ratematching_set_params) - sizeof(uint32 *) - sizeof(uint32);

		RenderingDecisionParamPayload.unModuleId1 = ASM_SESSION_MTMX_STRTR_MODULE_ID_AVSYNC;
		RenderingDecisionParamPayload.unParamId1 = ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION;
		RenderingDecisionParamPayload.usParamSize1 = sizeof(asm_session_mtmx_strtr_param_render_decision_t);
		RenderingDecisionParamPayload.usReserved1 = 0;
		RenderingDecisionParamPayload.sMtMxRenderingDecision.flags = pPayload->renderingDecison;

		RenderingDecisionParamPayload.unModuleId2 = ASM_SESSION_MTMX_STRTR_MODULE_ID_AVSYNC;
		RenderingDecisionParamPayload.unParamId2 = ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY;
		RenderingDecisionParamPayload.usParamSize2 = sizeof(asm_session_mtmx_strtr_param_clock_recovery_t);
		RenderingDecisionParamPayload.usReserved2 = 0;
		RenderingDecisionParamPayload.sMtMxClkRecoveryMethod.flags = pPayload->ClkRecoveryMode;

		RateMatchingPayload.pnParamData = (int32_t *)&RenderingDecisionParamPayload.unModuleId1;
		RateMatchingPayload.unSize = RenderingDecisionParamPayload.data_payload_size;

		//Route to the appropriate handler
		if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			result = MxAr_InPortSetParamHandler(me, pMsg, &RateMatchingPayload,  unPort2SetParam, pCurrentInPort);
		}
		else
		{
			result = ADSP_EBADPARAM;
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetRateMatchingParam) handler: rcvd on Tx Matrix. Not supported",
					me->mtMxID, pPayload->unSecOpCode);
			goto __bailoutCmdSetRateMatchingParamHandler;
		}
	}
	else
	{
		result = ADSP_EBADPARAM;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu cmd [%lu] (SetRateMatchingParam) handler: rcvd invalid port direction[%d]",
				me->mtMxID, pPayload->unSecOpCode, (int)pPayload->unPortDirection);
	}

	__bailoutCmdSetRateMatchingParamHandler:
	elite_msg_finish_msg(pMsg, result);
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Leaving cmd [%lu] (SetRateMatchingParam) handler with status %d", me->mtMxID, pPayload->unSecOpCode, (int)result);
	return result;
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

		uint32_t unDownStrmDelay = 0;
		if(NULL != pCurrentInPort->punMtMxInDelay)
		{
		    unDownStrmDelay += *(pCurrentInPort->punMtMxInDelay);
		}
		if(pCurrentInPort->unTopPrioOutPort < MT_MX_MAX_OUTPUT_PORTS)
		{
		    MatrixOutPortInfoType       *pTopPrioOutPort = me->outPortParams[pCurrentInPort->unTopPrioOutPort];

		    //Make sure that the punAFEDelay, punCoppDelay, punMtMxOutDelay are all valid.
		    if((NULL != pTopPrioOutPort) && (NULL != pTopPrioOutPort->punAFEDelay) && (NULL != pTopPrioOutPort->punCoppBufDelay) && (NULL != pTopPrioOutPort->punCoppAlgDelay))
		    {
		        unDownStrmDelay += *(pTopPrioOutPort->punAFEDelay) + *(pTopPrioOutPort->punCoppBufDelay) + *(pTopPrioOutPort->punCoppAlgDelay);

		        // The session time calculation happens in the output port context. So the rendering decision should only take into account the delay from the
		        // output port down. We should thus remove the delays of the acc buf and the internal buf. Based on when the buffer was sent down on the device,
		        // this can vary from (frame duration) to 2*(frame duration). Here we take the average.
		        unDownStrmDelay = unDownStrmDelay - (pTopPrioOutPort->unFrameDurationInUsec.int_part) - 0.5*(pTopPrioOutPort->unFrameDurationInUsec.int_part);
		    }
		}

		avsync_rendering_decision_t rendering_decision = RENDER;
		avsync_lib_process_run(pCurrentInPort->pInPortAvSync->pAVSyncLib, pPayload->unStartFlag,unDownStrmDelay);
		pCurrentInPort->pInPortAvSync->unStartFlag = pPayload->unStartFlag;
		avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, START_TIME, &(pPayload->ullStartTime), sizeof(pPayload->ullStartTime));
		int64_t llStartTime = 0;
		avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, START_TIME_OFFSET, &llStartTime);
		//MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Start time offset %ld", (int32_t)llStartTime);
		avsync_lib_make_simple_rendering_decision(pCurrentInPort->pInPortAvSync->pAVSyncLib,-llStartTime,&rendering_decision);

		if(llStartTime > 0)
		{
			pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part = llStartTime;
		}

		switch (pCurrentInPort->inPortState)
		{
		case INPUT_PORT_STATE_INIT:
		case INPUT_PORT_STATE_WAITING_TO_BE_READY:
		case INPUT_PORT_STATE_READY:
		{
			if (HOLD != rendering_decision)
			{
				pCurrentInPort->inPortState = INPUT_PORT_STATE_READY;
				pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_READY;
				MtMx_AddInputPortToWaitMask(me, unPort2Run);
				pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
				MtMx_SetupDefaultRenderWindow(me, unPort2Run); //AV-Sync
				MtMx_ClearHoldDuration(me, unPort2Run); //AV-Sync
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "relative start time good for rendering, moving i/p port %lu to READY", unPort2Run);
			}
			else
			{
				//if relative start time is in the future by at least (-1*pCurrentInPort->llRenderWindowStart):
				//remove i/p port from wait mask, move to waiting_to_be_ready state, wait duration already calculated above
				pCurrentInPort->inPortState = INPUT_PORT_STATE_WAITING_TO_BE_READY;
				pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_WAITING_TO_BE_READY;
				MtMx_RemoveInputPortFromWaitMask(me, unPort2Run);
				pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
				MtMx_SetupDefaultRenderWindow(me, unPort2Run); //AV-Sync
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "i/p port %lu being held for [MSW: %lu, LSW: %lu] usec",
						unPort2Run, (uint32_t )(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t )(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));
			}
			break;
			return ADSP_EOK;
		}
		case INPUT_PORT_STATE_PAUSED:
		case INPUT_PORT_STATE_WAITING_TO_BE_ACTIVE:
		case INPUT_PORT_STATE_ACTIVE:
		case INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER:
		{

			if (HOLD != rendering_decision)
			{
				pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE;
				pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE;
				MtMx_InPortToUpdateWaitMask(me, unPort2Run);
				pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
				MtMx_SetupDefaultRenderWindow(me, unPort2Run); //AV-Sync
				MtMx_ClearHoldDuration(me, unPort2Run); //AV-Sync
				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "relative start time slightly > 0, moving i/p port %lu to ACTIVE", unPort2Run);
			}
			else
			{
				//if relative start time is in the future by at least  (-1*pCurrentInPort->llRenderWindowStart):
				//remove i/p port from wait mask, move to waiting_to_be_active state, wait duration already calculated above
				pCurrentInPort->inPortState = INPUT_PORT_STATE_WAITING_TO_BE_ACTIVE;
				pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_WAITING_TO_BE_ACTIVE;
				MtMx_RemoveInputPortFromWaitMask(me, unPort2Run);
				pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
				MtMx_SetupDefaultRenderWindow(me, unPort2Run); //AV-Sync
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "i/p port %lu being held for [MSW: %lu, LSW: %lu] usec",
						unPort2Run, (uint32_t )(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part >> 32), (uint32_t )(pCurrentInPort->pInPortAvSync->ullInBufHoldDurationInUsec.int_part));
			}
			break;

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
			{
				MtMx_RunOutputPort(me, unPort2Run);
				break;
			}
			case ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_RELATIVE_TIME:
			{
				if (0 >= pPayload->ullStartTime)
				{
					//start immediately or start in the past
					MtMx_RunOutputPort(me, unPort2Run);
				}
				else
				{
					//start in the future
					pCurrentOutPort->pOutPortAvSync->ullOutBufHoldDurationInUsec.int_part = pPayload->ullStartTime;
					pCurrentOutPort->pOutPortAvSync->ullOutBufHoldDurationInUsec.frac_part = 0;
					if (pCurrentOutPort->pOutPortAvSync->ullOutBufHoldDurationInUsec.int_part < pCurrentOutPort->unFrameDurationInUsec.int_part)
					{
						//if relative start time is in the future by less than pCurrentOutPort->unOutPortPerChBufSize
						MtMx_RunOutputPort(me, unPort2Run);
					}
					else
					{
						//if relative start time is in the future by at least pCurrentOutPort->unOutPortPerChBufSize,
						//a. do not add o/p port's bufQ to the waitmask, b. move o/p port state to waiting_to_be_active state and
						//c. do not initialize session time to 0. */
						pCurrentOutPort->outPortState = OUTPUT_PORT_STATE_WAITING_TO_BE_ACTIVE;
						MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu cmd [%lu] Run handler: relative start time [MSW: %lu, LSW: %lu] is in future.",
								me->mtMxID, pPayload->unSecOpCode, (uint32_t)(pPayload->ullStartTime >> 32), (uint32_t)(pPayload->ullStartTime));
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "i/p port %lu being held for [MSW: %lu, LSW: %lu] samples",
								unPort2Run, (uint32_t)(pCurrentOutPort->pOutPortAvSync->ullOutBufHoldDurationInUsec.int_part >> 32),
								(uint32_t)(pCurrentOutPort->pOutPortAvSync->ullOutBufHoldDurationInUsec.int_part));
					}
				}
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

ADSPResult MtMx_AvSyncReleaseHW(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	ADSPResult result = ADSP_EOK;
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Releasing HW Resources for i/p port %lu",me->mtMxID, unInPortID);
	if (ADSP_EOK != (result = avsync_lib_close_hw_resources(pCurrentInPort->pInPortAvSync->pAVSyncLib)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to close HW Resources for i/p port %lu with result: %d", unInPortID, result);
	}
	return result;
}

ADSPResult MtMx_AvSyncRequestHW(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	ADSPResult result = ADSP_EOK;
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Requesting HW Resources for i/p port %lu",me->mtMxID, unInPortID);
	if (ADSP_EOK != (result = avsync_lib_open_hw_resources(pCurrentInPort->pInPortAvSync->pAVSyncLib)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to open HW Resources for i/p port %lu with result: %d", unInPortID, result);
	}
	return result;
}

ADSPResult MxAr_InPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	ADSPResult result = ADSP_EOK;
	void *pDataStart;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu received SetParam", me->mtMxID, unInPortID);

	uint16_t unTotalPayloadSize = pPayload->unSize;

	asm_session_param_data_v2_t *pDataPayload = (asm_session_param_data_v2_t *)(pPayload->pnParamData);

	//Code should be capable of handling multiple Set Params in the same command
	//Until Total Payload Size becomes zero, keep reading params
	while(unTotalPayloadSize > 0)
	{
		//If we reach the end or very close to the end, break out
		if(unTotalPayloadSize < sizeof(asm_session_param_data_v2_t))
		{
			break;
		}

		//If the total payload size is incorrect, return an error
		if(unTotalPayloadSize < (sizeof(asm_session_param_data_v2_t) + (pDataPayload->param_size)))
		{
			result = ADSP_ENEEDMORE;
			MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu Total Payload Size: %d < Reqd Size: %d",
					me->mtMxID, unInPortID, unTotalPayloadSize,
					(sizeof(asm_session_param_data_v2_t) + (pDataPayload->param_size)));
			break;
		}

		//Offset by the struct size to get to the start of the data
		pDataStart = (void*)(pDataPayload + 1);

		switch(pDataPayload->module_id)
		{
		case ASM_SESSION_MTMX_STRTR_MODULE_ID_AVSYNC:
		{
			ADSPResult local_result = ADSP_EOK;
			bool_t isAVSyncLibSetParam = TRUE;
			switch(pDataPayload->param_id)
			{

			case ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION:
			{
				//Make sure that the Run Command has not been issued yet. If port is already in run state, reject this Set Param. Default mode will be used.
				if(INPUT_PORT_STATE_INIT != pCurrentInPort->inPortState && INPUT_PORT_STATE_PAUSED != pCurrentInPort->inPortState)
				{
					local_result = ADSP_EUNEXPECTED;
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu is in %ld state and cannot accept this SetParam",
							me->mtMxID, unInPortID, pCurrentInPort->inPortState);
				}
				break;
			}
			case ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY:
			{
			    //Make sure that the Run Command has not been issued yet. If port is already in run state, reject this Set Param. Default mode will be used.
			    if(INPUT_PORT_STATE_INIT != pCurrentInPort->inPortState && INPUT_PORT_STATE_PAUSED != pCurrentInPort->inPortState)
			    {
			        local_result = ADSP_EUNEXPECTED;
			        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu is in %ld state and cannot accept this SetParam",
			                        me->mtMxID, unInPortID, pCurrentInPort->inPortState);
			    }

			    break;
			}
			case ASM_SESSION_MTMX_PARAM_ADJUST_SESSION_TIME_FINE_CTL:
			{
			    isAVSyncLibSetParam = FALSE;

			    /* If the input is not yet created or is in RUN state do not honor this */
			    if((INPUT_PORT_STATE_INACTIVE == pCurrentInPort->inPortState) ||(INPUT_PORT_STATE_ACTIVE == pCurrentInPort->inPortState))
			    {
			        local_result = ADSP_EFAILED;
			        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu is in %ld state and cannot accept this SetParam",
			                                    me->mtMxID, unInPortID, pCurrentInPort->inPortState);
			        pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled = FALSE;

			        break;
			    }
			    if(pDataPayload->param_size != sizeof(asm_session_mtmx_param_adjust_session_time_fine_ctl_t))
			    {
			        local_result = ADSP_EBADPARAM;
			        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MxArInPortSetParam: param size mismatch, incoming size %lu",
                                    pDataPayload->param_size);
			        pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled = FALSE;
			        break;
			    }
			    /* Reject this set param for independent clock recovery mode.
			     * This is done to avoid conflicts between the auto drift
			     * detection due to independent recovery vs client configured
			     * drift */
			    uint32_t current_clk_recovery_mechanism = ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT;
			    avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, CLOCK_RECOVERY_TYPE, &current_clk_recovery_mechanism);

			    if(ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT != current_clk_recovery_mechanism)
			    {
			        local_result = ADSP_EFAILED;
			        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MxArInPortSetParam: param id 0x%x attempted on non default clock recovery mode %ld",
                                                        pDataPayload->param_id, current_clk_recovery_mechanism);
			        pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled = FALSE;
			        break;
			    }

			    asm_session_mtmx_param_adjust_session_time_fine_ctl_t* pAdjSessTimeFineCtl =
                                (asm_session_mtmx_param_adjust_session_time_fine_ctl_t*)pDataStart;

			    pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled = pAdjSessTimeFineCtl->enable;
			    if(pAdjSessTimeFineCtl->enable)
			    {
			        local_result = MtMx_InPortToCheckReInitSampleSlipLibrary(me, unInPortID);
			        if(ADSP_FAILED(local_result))
			        {
			            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MxArInPortSetParam: creation of Sample slip library    failed with error code %d", local_result);
			            pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled = FALSE;
			            break;
			        }
			    }

			    /* Send Enable/Disable param to the sample slip module*/
			    mt_mx_sampleslip_t *pSampleSlip = &(pCurrentInPort->pInPortAvSync->structSampleSlip);
			    if(NULL != pSampleSlip->pSampleSlipCapiv2)
			    {
			        audproc_sampleslip_enable_t sampleslip_enable;
			        capi_v2_buf_t param_buf;
			        param_buf.data_ptr = (int8_t *) &sampleslip_enable;
			        param_buf.actual_data_len = param_buf.max_data_len = sizeof(sampleslip_enable);

			        sampleslip_enable.enable = pAdjSessTimeFineCtl->enable;
			        local_result = capi_v2_err_to_adsp_result(pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->set_param(pSampleSlip->pSampleSlipCapiv2, AUDPROC_PARAM_ID_SAMPLESLIP_ENABLE, NULL, &param_buf));

			        if (ADSP_EOK != local_result)
			        {
			            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu SetParam on SampleSlip library failed with result: %x. Cont w/o lib.",
			                            me->mtMxID, unInPortID, result);
			            if(NULL != pSampleSlip->pSampleSlipCapiv2)
			            {
			                (void) pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->end(pSampleSlip->pSampleSlipCapiv2);
			                MTMX_FREE(pSampleSlip->pSampleSlipCapiv2);
			                pCurrentInPort->pInPortAvSync->bIsAdjSessClkFineCtlEnabled = FALSE;
			            }
			        }
			        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu AVSyncLib: SampleSlip library enable: %x.",me->mtMxID, unInPortID, pAdjSessTimeFineCtl->enable);
			    }
			    break;
			}
			default:
			{
				//Do nothing, let avsync lib handle.
			}
			}
			if (ADSP_SUCCEEDED(local_result) && isAVSyncLibSetParam)
			{
				local_result = avsync_lib_set_param(pCurrentInPort->pInPortAvSync->pAVSyncLib,pDataPayload->param_id,pDataStart,pDataPayload->param_size);
			}
			if (ADSP_FAILED(local_result))
			{
				result = local_result;
			}
			break;
		}
		default:
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu Invalid Module ID: %lu",
					me->mtMxID, unInPortID, pDataPayload->module_id);
		}
		}

		//Now, move to the next SetParam and update the total payloadsize
		unTotalPayloadSize -= (sizeof(asm_session_param_data_v2_t) + pDataPayload->param_size);
		pDataPayload = (asm_session_param_data_v2_t*) ( (uint32_t*)pDataStart + (pDataPayload->param_size >> 2) );

		if (((pDataPayload->param_size % 4) != 0) && (unTotalPayloadSize > 0))
		{
			MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu: Param size for set param module ID 0x%lx, param ID %lx is %lu which is not a multiple of 4. Aborting further processing.", me->mtMxID, unInPortID, pDataPayload->module_id, pDataPayload->param_id, pDataPayload->param_size);
			result = ADSP_EFAILED;
			break;
		}
	}

	return result;
}

ADSPResult MxAt_InPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	ADSPResult result = ADSP_EUNSUPPORTED;
	MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu received SetParam", me->mtMxID, unInPortID);
	return result;
}

ADSPResult MxAr_OutPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	ADSPResult result = ADSP_EUNSUPPORTED;
	MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: o/p port %lu received SetParam", me->mtMxID, unOutPortID);
	return result;
}

ADSPResult MxAt_OutPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload,
		uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort)
{
	ADSPResult result = ADSP_EUNSUPPORTED;
	MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: o/p port %lu received SetParam", me->mtMxID, unOutPortID);
	return result;
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
			case ASM_SESSION_MTMX_STRTR_PARAM_SESSION_TIME_V3:
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
				uint64_t ullSessionTime;
				uint64_t ullAbsoluteTime;
				uint64_t ullTimeStamp = pCurrentInPort->ullTimeStampProcessed.int_part;
				avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, SESSION_CLOCK, &(ullSessionTime));
				avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, ABSOLUTE_TS, &(ullAbsoluteTime));
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
			{
				//Make sure that the payload size is valid.
				if(unTotalPayloadSize < (sizeof(asm_session_param_data_v2_t) + sizeof(asm_session_mtmx_strtr_session_inst_statistics_v2_t)))
				{
					result = ADSP_ENEEDMORE;
					MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu, Payload Size: %d < Reqd Size: %d",
							me->mtMxID, unInPortID, unTotalPayloadSize,
							(sizeof(asm_session_param_data_v2_t) + sizeof(asm_session_mtmx_strtr_session_inst_statistics_v2_t)));
					return result;
				}

				//Call a query on the instantaneous stats if both the ends of the stat window has been set properly
				result = avsync_lib_query_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib,
						ASM_SESSION_MTMX_STRTR_PARAM_SESSION_INST_STATISTICS_V2,
						pDataStart);
				if(ADSP_FAILED(result))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu, Query for Inst. stats failed, Res = %d",
							me->mtMxID, unInPortID, result);
					return result;
				}

				//Update the param_size so that the caller is aware of the actual size filled by this get param
				pDataPayload->param_size = sizeof(asm_session_mtmx_strtr_session_inst_statistics_v2_t);
				pPayload->unSize = sizeof(asm_session_mtmx_strtr_session_inst_statistics_v2_t);

				break;
			}
			case ASM_SESSION_MTMX_STRTR_PARAM_SESSION_CUMU_STATISTICS_V2:
			{
				//Make sure that the payload size is valid.
				if(unTotalPayloadSize < (sizeof(asm_session_param_data_v2_t) + sizeof(asm_session_mtmx_strtr_session_cumu_statistics_v2_t)))
				{
					result = ADSP_ENEEDMORE;
					MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu, Payload Size: %d < Reqd Size: %d",
							me->mtMxID, unInPortID, unTotalPayloadSize,
							(sizeof(asm_session_param_data_v2_t) + sizeof(asm_session_mtmx_strtr_session_cumu_statistics_v2_t)));
					return result;
				}

				//Call a query on the instantaneous stats if both the ends of the stat window has been set properly

				result = avsync_lib_query_stat(pCurrentInPort->pInPortAvSync->pAVSyncLib,
						ASM_SESSION_MTMX_STRTR_PARAM_SESSION_CUMU_STATISTICS_V2,
						pDataStart);
				if(ADSP_FAILED(result))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port %lu, Query for Cumm. stats failed, Res = %d",
							me->mtMxID, unInPortID, result);
					return result;
				}


				//Update the param_size so that the caller is aware of the actual size filled by this get param
				pDataPayload->param_size = sizeof(asm_session_mtmx_strtr_session_cumu_statistics_v2_t);
				pPayload->unSize = sizeof(asm_session_mtmx_strtr_session_cumu_statistics_v2_t);

				break;
			}
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

ADSPResult MtMx_SetInOutDriftPtr(This_t *me, uint32_t unInPortID, uint32_t unOutPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	MatrixOutPortInfoType    *pCurrentOutPort = me->outPortParams[unOutPortID];

	if(NULL == pCurrentOutPort->pOutPortAvSync->pAfeDriftPtr)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MtMx #%lu: i/p port %u, Top prio o/p port %lu AFE Drift Ptr is NULL", me->mtMxID, unInPortID, unOutPortID);
	}
	else
	{
		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MtMx #%lu: i/p port %u, Top prio o/p port %lu AFE Drift Ptr %p",
				me->mtMxID, unInPortID, unOutPortID, pCurrentOutPort->pOutPortAvSync->pAfeDriftPtr);
		avsync_lib_set_device_drift_pointer(pCurrentInPort->pInPortAvSync->pAVSyncLib, &(pCurrentOutPort->pOutPortAvSync->pAfeDriftPtr->avt_drift_info));
	}

	return ADSP_EOK;
}

ADSPResult  MtMx_InitInputAvSync(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	ADSPResult result = ADSP_EOK;

	//Allocate memory for pInPortAvSync
	pCurrentInPort->pInPortAvSync = (mt_mx_inport_avsync_t *)qurt_elite_memory_malloc(sizeof(mt_mx_inport_avsync_t), QURT_ELITE_HEAP_DEFAULT);
	if(NULL == pCurrentInPort->pInPortAvSync)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu failed to alloc %lu bytes mem for input port AV-Sync structure.",
				me->mtMxID, unInPortID, sizeof(mt_mx_inport_avsync_t));
		result = ADSP_ENOMEMORY;
		return result;
	}
	else
	{
		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu alloc %lu bytes mem for input port AV-Sync structure success, Ptr: 0x%p.",
				me->mtMxID, unInPortID, sizeof(mt_mx_inport_avsync_t), pCurrentInPort->pInPortAvSync);
	}
	memset((void*)pCurrentInPort->pInPortAvSync, 0, sizeof(mt_mx_inport_avsync_t));

	//Allocate AV-Sync lib, init it and init s2d drift
	if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
	{
		if(NULL == pCurrentInPort->pInPortAvSync->pAVSyncLib)
		{
			pCurrentInPort->pInPortAvSync->pAVSyncLib = qurt_elite_memory_malloc(avsync_lib_getsize(), QURT_ELITE_HEAP_DEFAULT);
			if(NULL == pCurrentInPort->pInPortAvSync->pAVSyncLib)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu, i/p port %lu failed to create pAvSyncLib", me->mtMxID, unInPortID);
				result = ADSP_ENOMEMORY;
				return result;
			}
		}

		char sInstName[32];
		snprintf(sInstName, 32, "MtMxIp %ld", unInPortID);
		if (ADSP_EOK != (result = avsync_lib_init(pCurrentInPort->pInPortAvSync->pAVSyncLib, sInstName)))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu, i/p port %lu avsync_lib_init failed", me->mtMxID, unInPortID);
			if(pCurrentInPort->pInPortAvSync->pAVSyncLib)
			{
				qurt_elite_memory_free(pCurrentInPort->pInPortAvSync->pAVSyncLib);
				pCurrentInPort->pInPortAvSync->pAVSyncLib = NULL;
			}
			result = ADSP_ENOMEMORY;
			return result;
		}

		if (ADSP_EOK != (result = avsync_lib_init_s2d_drift(pCurrentInPort->pInPortAvSync->pAVSyncLib)))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu, i/p port %lu avsync_lib_init_s2d_drift failed", me->mtMxID, unInPortID);
			if(pCurrentInPort->pInPortAvSync->pAVSyncLib)
			{
				qurt_elite_memory_free(pCurrentInPort->pInPortAvSync->pAVSyncLib);
				pCurrentInPort->pInPortAvSync->pAVSyncLib = NULL;
			}
			result = ADSP_ENOMEMORY;
			return result;
		}

		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu, i/p port %lu create pAvSyncLib: 0x%p success", me->mtMxID, unInPortID, pCurrentInPort->pInPortAvSync->pAVSyncLib);
	}

	return result;
}

ADSPResult  MtMx_DeInitInputAvSync(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	ADSPResult result = ADSP_EOK;

	//Free SampleSlip library if it has been created.
	mt_mx_sampleslip_t *pSampleSlip = &(pCurrentInPort->pInPortAvSync->structSampleSlip);
	if (NULL != pSampleSlip->pSampleSlipCapiv2)
	{
		(void)pSampleSlip->pSampleSlipCapiv2->vtbl_ptr->end(pSampleSlip->pSampleSlipCapiv2);
		MTMX_FREE(pSampleSlip->pSampleSlipCapiv2);
	}
	memset(pSampleSlip->inBufs,0,sizeof(pSampleSlip->inBufs));
	memset(pSampleSlip->outBufs,0,sizeof(pSampleSlip->outBufs));

	if(NULL != pCurrentInPort->pInPortAvSync)
	{
	   //Deinit s2d drift, deinit AV-Sync lib and free its memory
	   if(NULL != pCurrentInPort->pInPortAvSync->pAVSyncLib)
	   {
	  	   avsync_lib_deinit_s2d_drift(pCurrentInPort->pInPortAvSync->pAVSyncLib);
		   avsync_lib_deinit(pCurrentInPort->pInPortAvSync->pAVSyncLib);
		   qurt_elite_memory_free(pCurrentInPort->pInPortAvSync->pAVSyncLib);
		   pCurrentInPort->pInPortAvSync->pAVSyncLib = NULL;
	   }

	   //Free memory allocated for the AV-Sync pointer
	   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu, Freeing pInPortAvSync 0x%p", me->mtMxID, unInPortID, pCurrentInPort->pInPortAvSync);
  	   MTMX_FREE(pCurrentInPort->pInPortAvSync);
	}

	return result;
}

ADSPResult MtMx_InitOutputAvSync(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType    *pCurrentOutPort = me->outPortParams[unOutPortID];
	ADSPResult result = ADSP_EOK;

	//Allocate memory for the AV-Sync pointer
	pCurrentOutPort->pOutPortAvSync = (mt_mx_outport_avsync_t *)qurt_elite_memory_malloc(sizeof(mt_mx_outport_avsync_t), QURT_ELITE_HEAP_DEFAULT);
	if(NULL == pCurrentOutPort->pOutPortAvSync)
	{
		result = ADSP_ENOMEMORY;
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: MtMx #%lu o/p port %lu failed to alloc %lu bytes mem for output port AV-Sync structure.",
				me->mtMxID, unOutPortID, sizeof(mt_mx_outport_avsync_t));
		return result;
	}
	else
	{
		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu o/p port %lu alloc %lu bytes mem for output port AV-Sync structure success, Ptr: 0x%p.",
				me->mtMxID, unOutPortID, sizeof(mt_mx_outport_avsync_t), pCurrentOutPort->pOutPortAvSync);
	}
	memset((void*)pCurrentOutPort->pOutPortAvSync, 0, sizeof(mt_mx_outport_avsync_t));

	return result;
}

ADSPResult MtMx_DeInitOutputAvSync(This_t *me, uint32_t unOutPortID)
{
	MatrixOutPortInfoType    *pCurrentOutPort = me->outPortParams[unOutPortID];
	ADSPResult result = ADSP_EOK;

	//Free memory allocated for the AV-Sync pointer
	if(NULL != pCurrentOutPort->pOutPortAvSync)
	{
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: o/p port %lu, Freeing pOutPortAvSync 0x%p", me->mtMxID, unOutPortID, pCurrentOutPort->pOutPortAvSync);
		MTMX_FREE(pCurrentOutPort->pOutPortAvSync);
	}

	return result;
}

ADSPResult MtMx_SetInputDriftPtr(This_t *me, uint32_t unInPortID, volatile const afe_drift_info_t *pAfeDriftPtr)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	pCurrentInPort->pInPortAvSync->pAfeDriftPtr = pAfeDriftPtr;
	return ADSP_EOK;
}

ADSPResult MtMx_SetOutputDriftPtr(This_t *me, uint32_t unOutPortID, volatile const afe_drift_info_t *pAfeDriftPtr)
{
	MatrixOutPortInfoType    *pCurrentOutPort = me->outPortParams[unOutPortID];
	pCurrentOutPort->pOutPortAvSync->pAfeDriftPtr = pAfeDriftPtr;
	return ADSP_EOK;
}

ADSPResult MtMx_CheckIfInportMovesAnyOutportToActiveState(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType       *pCurrentInPort = me->inPortParams[unInPortID];
	MatrixOutPortInfoType      *pCurrentOutPort;
	uint32                     strMask = pCurrentInPort->strMask;
	uint32                     unOutPortID;
	while (strMask)
	{
		unOutPortID = Q6_R_ct0_R(strMask);
		strMask ^= (1 << unOutPortID);
		pCurrentOutPort = me->outPortParams[unOutPortID];

		if (OUTPUT_PORT_STATE_WAITING_TO_BE_ACTIVE == pCurrentOutPort->outPortState)
		{
			mt_mx_decrement_time(&pCurrentOutPort->pOutPortAvSync->ullOutBufHoldDurationInUsec, pCurrentOutPort->unFrameDurationInUsec);
			// accurate to the nearest 1us
			if (pCurrentOutPort->pOutPortAvSync->ullOutBufHoldDurationInUsec.int_part < pCurrentOutPort->unFrameDurationInUsec.int_part)
			{
				MtMx_RunOutputPort(me, unOutPortID);
				pCurrentOutPort->bIsFirstOutBufYetToBeSent = TRUE;
			}
		}
	}

	return ADSP_EOK;
}

ADSPResult MtMx_UpdateExpectedST(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType    *pCurrentInPort = me->inPortParams[unInPortID];
	uint64_t ullSessionTime = 0;
	avsync_lib_get_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, SESSION_CLOCK, &ullSessionTime);
	uint64_t proposed_expected_session_clock = ullSessionTime + pCurrentInPort->unFrameDurationInUsec.int_part;
	avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, EXPECTED_SESSION_CLOCK, &proposed_expected_session_clock, sizeof(proposed_expected_session_clock));
	return ADSP_EOK;
}

ADSPResult MtMx_ResetSessionTimeClock(This_t *me, uint32_t unInPortID)
{
	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];
	avsync_lib_set_internal_param(pCurrentInPort->pInPortAvSync->pAVSyncLib, SESSION_CLOCK, &AVSYNC_LIB_ZERO_SESSION_CLOCK, sizeof(AVSYNC_LIB_ZERO_SESSION_CLOCK));
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

	//Default rendering mode
	if(pCurrentInPort->pInPortAvSync->bShouldSessionTimeBeAdjWhenSendingBufDown)
	{
		if (MT_MX_INPUT_PORT_ADD_SAMPLES == pCurrentInPort->pInPortAvSync->samplesAddOrDropMask)
		{
			unSampleIncrement += (unPortPerChBufSize - MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER - pCurrentInPort->ullStaleEosSamples);
		}
		else
		{
			unSampleIncrement += (unPortPerChBufSize + MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER - pCurrentInPort->ullStaleEosSamples);
		}
	}
	else
	{
		unSampleIncrement += unPortPerChBufSize - pCurrentInPort->ullStaleEosSamples;
	}
	ullNumSamplesForStcUpdate += unSampleIncrement;
	pCurrentInPort->ullNumSamplesForStcUpdate += unSampleIncrement;

	// overflow protection logic is unnecessary since the calculation below will overflow after ~1000 days
	// The Proposed Session Clock = Equivalent time for the STC Update + The Base Time for STC
	uint64_t proposed_session_clock = (MT_MX_SAMPLES_TO_USEC(ullNumSamplesForStcUpdate, unSampleRate))+ (pCurrentInPort->ullTimeStampForStcBaseInUsec.int_part);

#ifdef MT_MX_EXTRA_DEBUG
	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu updating STC to [%lu, %lu]",
			me->mtMxID, unInPortID, (uint32_t)(proposed_session_clock>>32), (uint32_t)proposed_session_clock);
#endif

	avsync_lib_update_session_clock(pCurrentInPort->pInPortAvSync->pAVSyncLib, proposed_session_clock);
	uint64_t ullStaleEosSamplesuSec = MT_MX_SAMPLES_TO_USEC(pCurrentInPort->ullStaleEosSamples, unSampleRate);
	avsync_lib_update_absolute_time(pCurrentInPort->pInPortAvSync->pAVSyncLib, ullCurrentDevicePathDelay-ullStaleEosSamplesuSec, FALSE);
	return ADSP_EOK;
}
