/**
@file MixerSvc_ChanMixUtils.cpp
@brief This file defines channel mixer utility functions that 
       the audio matrix mixer uses.
 */

/*===========================================================================
Copyright (c) 2013-2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_ChanMixUtils.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/10/2014  kr      Re-factoring, De-coupling PSPD and ChannelMixer.
11/25/2013 rkc     Created file.
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"
#include "MixerSvc_MsgHandlers.h"
#include "MixerSvc_OutPortHandler.h"
#include "MixerSvc_Util.h"
#include "adsp_media_fmt.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "MixerSvc_InPortHandler.h"
#include "MixerSvc_ChanMixUtils.h"
#include "AudDevMgr_PrivateDefs.h"
#include "MixerSvc_Pspd.h"

static ADSPResult MtMx_CreatePspdChannelMixer(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, eChMixerType eChannelMixerType);
static ADSPResult MtMx_CreateInitChannelMixer(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, eChMixerType eChannelMixerType);
static void MtMx_DestroyChannelMixer(This_t *me, uint32_t unInPortID, uint32_t unOutPortID);

void MtMx_InOutPortsToCheckReInitChannelMixerLibrary(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, bool_t bOptzCheck)
{
	MatrixInPortInfoType    *pCurrentInPort  = me->inPortParams[unInPortID];
	MatrixOutPortInfoType   *pCurrentOutPort = me->outPortParams[unOutPortID];
	mt_mx_struct_pspd_t                *pCurrentPspd                         = &(pCurrentInPort->structPspd[unOutPortID]);
	mt_mx_struct_channel_mixer_t *pCurrentInputOutputChMixer = &(pCurrentInPort->structChanMixer[unOutPortID]);
	ADSPResult                               result                                       = ADSP_EOK;

	if(!MtMx_PspdSvcCreated(pCurrentPspd))
	{
		//PSPD thread needs to be created for this input-output pair.
		memset(&pCurrentPspd->thread_param, 0, sizeof(pCurrentPspd->thread_param));
		result = MtMx_CreatePspdSvc(&pCurrentPspd->thread_param, unInPortID, unOutPortID, &(pCurrentPspd->unPspdKpps), &(pCurrentPspd->unPspdBw));

		if (ADSP_EOK != result)
		{
			MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, created PSPD svc. failed, result = %d", me->mtMxID, unInPortID, unOutPortID, result);
			return;
		}
		else
		{
			(void)MtMx_PspdReInitBuffers(me, unInPortID, unOutPortID);
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, created PSPD svc. success", me->mtMxID, unInPortID, unOutPortID);
		}
	}

	//If the o/p port is a native mode port, the ChMixer lib will not be init/used.
	if(FALSE == MtMx_ChannelNativityIsEnabled(pCurrentOutPort->unNativeModeFlags))
	{
		if(FALSE == pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated)
		{
			//Create new PSPD QCOM channel mixer
			result = MtMx_CreatePspdChannelMixer(me, unInPortID, unOutPortID, PSPD_CHMIXER_QCOM);
			if (ADSP_EOK != result)
			{
				MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, QCOM channel mixer creation failed, result=%d. Cont w/o lib.",
						me->mtMxID, unInPortID, unOutPortID, result);
				goto __bailoutMtMxInOutPortsToCheckReInitChannelMixerLibrary;
			}
			else
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, created QCOM channel mixer", me->mtMxID, unInPortID, unOutPortID);
			}
		}

		if(TRUE == bOptzCheck && TRUE == pCurrentInputOutputChMixer->bCanChMixerBeOptimizedOut)
		{
			//As an optimization step, the library will not be called for the following scenarios
			//1) M->M
			//2) M->S
			//3) S->M
			//4) S->S with the same channel maps on the i/p and o/p
			if (
					(((NUM_CHANNELS_MONO == pCurrentInPort->unNumChannels) && (NUM_CHANNELS_MONO == pCurrentOutPort->unNumChannels)) ||
							((NUM_CHANNELS_MONO == pCurrentInPort->unNumChannels) && (NUM_CHANNELS_STEREO == pCurrentOutPort->unNumChannels)) ||
							((NUM_CHANNELS_STEREO == pCurrentInPort->unNumChannels) && (NUM_CHANNELS_MONO == pCurrentOutPort->unNumChannels)) ||
							((NUM_CHANNELS_STEREO == pCurrentInPort->unNumChannels) && (NUM_CHANNELS_STEREO == pCurrentOutPort->unNumChannels) &&
									((pCurrentInPort->unChannelMapping[0] == pCurrentOutPort->unChannelMapping[0] &&
											pCurrentInPort->unChannelMapping[1] == pCurrentOutPort->unChannelMapping[1]) ||
											(pCurrentInPort->unChannelMapping[0] == pCurrentOutPort->unChannelMapping[1] &&
													pCurrentInPort->unChannelMapping[1] == pCurrentOutPort->unChannelMapping[0]))))
			)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, ChannelMixerLib will not be used (optimization). Cont w/o lib.",
						me->mtMxID, unInPortID, unOutPortID);
				MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx i/p #ch %lu, o/p #ch %lu, i/p (ch[0],ch[1]) = (%d,%d), o/p (ch[0],ch[1]) = (%d,%d)",
						pCurrentInPort->unNumChannels, pCurrentOutPort->unNumChannels, pCurrentInPort->unChannelMapping[0],
						pCurrentInPort->unChannelMapping[1], pCurrentOutPort->unChannelMapping[0], pCurrentOutPort->unChannelMapping[1]);
				goto __bailoutMtMxInOutPortsToCheckReInitChannelMixerLibrary;
			}

			// channel mixer library is not required if number of channels, channel map are same
			if (pCurrentInPort->unNumChannels == pCurrentOutPort->unNumChannels)
			{
				// check if channel mapping is same. If same, delete channel mixer.
				uint32_t unChannelNum = 0;
				for (unChannelNum = 0; unChannelNum < pCurrentInPort->unNumChannels; unChannelNum++)
				{ 
					if (pCurrentInPort->unChannelMapping[unChannelNum] != pCurrentOutPort->unChannelMapping[unChannelNum])
					{
						break;
					}
				}
				if (unChannelNum == pCurrentInPort->unNumChannels)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, ChannelMixerLib will not be used (optimization). Cont w/o lib.",
							me->mtMxID, unInPortID, unOutPortID);
					goto __bailoutMtMxInOutPortsToCheckReInitChannelMixerLibrary;				
				}
			}
		}

		//Set the output channel configuration through set param
		ADSPResult result = ADSP_EOK;
		result = MtMx_SetChannelMixerCfg(me, unInPortID, unOutPortID);
		if (ADSP_EOK != result)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, MtMx_SetChannelMixerCfg returned failure. Cont w/o lib.",
					me->mtMxID, unInPortID, unOutPortID);
			goto __bailoutMtMxInOutPortsToCheckReInitChannelMixerLibrary;
		}

		//Successful init
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port ID %lu, o/p port ID %lu, MtMx_SetChannelMixerCfg returned success.",
				me->mtMxID, unInPortID, unOutPortID);

		return;
	}

	__bailoutMtMxInOutPortsToCheckReInitChannelMixerLibrary:
	//Destroy current PSPD channel mixer (if it exists)
	MtMx_DestroyPspdChannelMixer(me, unInPortID, unOutPortID);
	return;
}

static ADSPResult MtMx_CreatePspdChannelMixer(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, eChMixerType eChannelMixerType)
{
	//This function creates the channel mixer. It assumes that the PSPD thread is already created.
	MatrixInPortInfoType                *pCurrentInPort                        = me->inPortParams[unInPortID];
	mt_mx_struct_channel_mixer_t *pCurrentInputOutputChMixer = &(pCurrentInPort->structChanMixer[unOutPortID]);
	ADSPResult result = ADSP_EOK;

	result = MtMx_CreateInitChannelMixer(me, unInPortID, unOutPortID, eChannelMixerType);
	if(ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed MtMx_CreateInitChannelMixer; result = %d", result);
		goto __bailoutCreatePspdChannelMixer;
	}

	switch(eChannelMixerType)
	{
	case PSPD_CHMIXER_QCOM:
	{
		pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated = TRUE;
		break;
	}
	case PSPD_CHMIXER_NONE:
	{
		pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated = FALSE;
		result = ADSP_EBADPARAM;
		break;
	}
	}

	pCurrentInputOutputChMixer->bCanChMixerBeOptimizedOut = TRUE;
	return result;

	__bailoutCreatePspdChannelMixer:
	pCurrentInputOutputChMixer->unCachedPsPdSetParamID = 0;
	pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated = FALSE;
	pCurrentInputOutputChMixer->bCanChMixerBeOptimizedOut = TRUE;
	return result;
}

static ADSPResult MtMx_CreateInitChannelMixer(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, eChMixerType eChannelMixerType)
{
	ADSPResult result = ADSP_EOK;
	elite_msg_any_t param_msg;
	uint32_t unPayloadSize = sizeof(EliteMsg_CustomPspdCfgChmixInout);

	MatrixInPortInfoType    *pCurrentInPort     = me->inPortParams[unInPortID];
	mt_mx_struct_pspd_t    *pCurrentPspd      = &(pCurrentInPort->structPspd[unOutPortID]);

	//Create new message and setup payload and secondary OpCode.
	if(ADSP_EOK != (result = elite_msg_create_msg( &param_msg, &unPayloadSize, ELITE_CUSTOM_MSG, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q, 0, 0)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create elite msg with error = %d", result);
		return result;
	}
	EliteMsg_CustomPspdCfgChmixInout *payload = (EliteMsg_CustomPspdCfgChmixInout *)param_msg.pPayload;
	payload->unSecOpCode = ELITEMSG_CUSTOM_PSPD_CREATE_CHMIXER;

	//Fill up params.
	payload->param.bit_width =16;
	payload->param.num_in_ch = 2;
	payload->param.in_ch_map[0] = PCM_CHANNEL_L;
	payload->param.in_ch_map[1] = PCM_CHANNEL_R;
	payload->param.num_out_ch = 2;
	payload->param.out_ch_map[0] = PCM_CHANNEL_L;
	payload->param.out_ch_map[1] = PCM_CHANNEL_R;
	payload->param.eChannelMixerType = eChannelMixerType;

	//Send command to PSPD thread and wait for response.
	return MtMx_PspdSendAndWaitForResp(&param_msg, pCurrentPspd->thread_param.cmdQ, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q);
}

void MtMx_DestroyPspdChannelMixer(This_t *me, uint32_t unInPortID, uint32_t unOutPortID)
{
	//This function destroys the channel mixer. It assumes that the PSPD thread is already created and not joined yet.
	MatrixInPortInfoType                *pCurrentInPort                        = me->inPortParams[unInPortID];
	mt_mx_struct_channel_mixer_t *pCurrentInputOutputChMixer = &(pCurrentInPort->structChanMixer[unOutPortID]);

	//Free up the APPI pointer
	if(pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated)
	{
		MtMx_DestroyChannelMixer(me, unInPortID, unOutPortID);
	}

	//Free up any malloced PSPD SetParam and reset the channel mixer structure params
	if(pCurrentInputOutputChMixer->punCachedPsPdSetParam)
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: Freeing punCachedPsPdSetParam 0x%p", me->mtMxID, pCurrentInputOutputChMixer->punCachedPsPdSetParam);
		MTMX_FREE(pCurrentInputOutputChMixer->punCachedPsPdSetParam);
	}
	pCurrentInputOutputChMixer->unCachedPsPdSetParamID = 0;
	pCurrentInputOutputChMixer->bIsQcomChannelMixerLibCreated = FALSE;
	pCurrentInputOutputChMixer->bCanChMixerBeOptimizedOut = TRUE;
}

static void MtMx_DestroyChannelMixer(This_t *me, uint32_t unInPortID, uint32_t unOutPortID)
{
	ADSPResult result = ADSP_EOK;
	elite_msg_any_t param_msg;
	uint32_t unPayloadSize = sizeof(EliteMsg_CustomPspdCfgChmixInout);

	MatrixInPortInfoType    *pCurrentInPort     = me->inPortParams[unInPortID];
	mt_mx_struct_pspd_t    *pCurrentPspd      = &(pCurrentInPort->structPspd[unOutPortID]);

	//Create new message and setup payload and secondary OpCode.
	if(ADSP_EOK != (result = elite_msg_create_msg( &param_msg, &unPayloadSize, ELITE_CUSTOM_MSG, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q, 0, 0)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create elite msg with error = %d", result);
		return;
	}
	EliteMsg_CustomPspdCfgChmixInout *payload = (EliteMsg_CustomPspdCfgChmixInout *)param_msg.pPayload;
	payload->unSecOpCode = ELITEMSG_CUSTOM_PSPD_DESTROY_CHMIXER;

	//No params to send.

	//Send command to PSPD thread and wait for response.
	(void)MtMx_PspdSendAndWaitForResp(&param_msg, pCurrentPspd->thread_param.cmdQ, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q);
}

ADSPResult MtMx_SetChannelMixerCfg(This_t *me, uint32_t unInPortID, uint32_t unOutPortID)
{
	ADSPResult result = ADSP_EOK;
	elite_msg_any_t param_msg;
	uint32_t unPayloadSize = sizeof(EliteMsg_CustomPspdCfgChmixInout);

	MatrixInPortInfoType    *pCurrentInPort     = me->inPortParams[unInPortID];
	MatrixOutPortInfoType *pCurrentOutPort  = me->outPortParams[unOutPortID];
	mt_mx_struct_pspd_t    *pCurrentPspd      = &(pCurrentInPort->structPspd[unOutPortID]);

	//Create new message and setup payload and secondary OpCode.
	if(ADSP_EOK != (result = elite_msg_create_msg( &param_msg, &unPayloadSize, ELITE_CUSTOM_MSG, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q, 0, 0)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create elite msg with error = %d", result);
		return result;
	}
	EliteMsg_CustomPspdCfgChmixInout *payload = (EliteMsg_CustomPspdCfgChmixInout *)param_msg.pPayload;
	payload->unSecOpCode = ELITEMSG_CUSTOM_PSPD_CFG_CHMIXER_INOUT;

	//Fill up params.
	payload->param.bit_width = (pCurrentInPort->unBitwidth)?(pCurrentInPort->unBitwidth):(16);
	payload->param.num_in_ch = (pCurrentInPort->unNumChannels)?(pCurrentInPort->unNumChannels):(2);
	payload->param.num_out_ch = (pCurrentOutPort->unNumChannels)?(pCurrentOutPort->unNumChannels):(2);

	if (0 == pCurrentInPort->unNumChannels)
	{
		payload->param.in_ch_map[0] = PCM_CHANNEL_L;
		payload->param.in_ch_map[1] = PCM_CHANNEL_R;
	}
	else
	{
		MtMx_CopyChannelMap(pCurrentInPort->unChannelMapping, payload->param.in_ch_map, pCurrentInPort->unNumChannels);
	}

	if (0 == pCurrentOutPort->unNumChannels)
	{
		payload->param.out_ch_map[0] = PCM_CHANNEL_L;
		payload->param.out_ch_map[1] = PCM_CHANNEL_R;
	}
	else
	{
		MtMx_CopyChannelMap(pCurrentOutPort->unChannelMapping, payload->param.out_ch_map, pCurrentOutPort->unNumChannels);
	}
	//This command does not set the channel mixer type. Therefore, payload->param.eChannelMixerType is not set here.

	//Send command to PSPD thread and wait for response.
	if(ADSP_EOK != (result = MtMx_PspdSendAndWaitForResp(&param_msg, pCurrentPspd->thread_param.cmdQ, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PSPD channel mixer config command failed with error = %d", result);
		return result;	
	}

	//Update matrix KPPS and/or BW and raise ADSPPM event if needed.
	return(MtMx_SetReqKppsAndBW(me));
}

ADSPResult MtMx_SetChannelMixerCoef(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, audproc_chmixer_param_id_coeff_t *pCoefPayload)
{
	ADSPResult result = ADSP_EOK;
	elite_msg_any_t param_msg;
	uint32_t unPayloadSize = sizeof(EliteMsg_CustomPspdSetChmixCoef);

	MatrixInPortInfoType  *pCurrentInPort  = me->inPortParams[unInPortID];
	mt_mx_struct_pspd_t  *pCurrentPspd   = &(pCurrentInPort->structPspd[unOutPortID]);

	//Create new message and setup payload and secondary OpCode.
	if(ADSP_EOK != (result = elite_msg_create_msg( &param_msg, &unPayloadSize, ELITE_CUSTOM_MSG, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q, 0, 0)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create elite msg with error = %d", result);
		return result;
	}
	EliteMsg_CustomPspdSetChmixCoef *payload = (EliteMsg_CustomPspdSetChmixCoef *)param_msg.pPayload;
	payload->unSecOpCode = ELITEMSG_CUSTOM_PSPD_SET_CHMIXER_COEF;

	//Fill up params.
	payload->coef = pCoefPayload;

	//Send command to PSPD thread and wait for response.
	return MtMx_PspdSendAndWaitForResp(&param_msg, pCurrentPspd->thread_param.cmdQ, (qurt_elite_queue_t *)pCurrentPspd->cmd_resp_q);
}
