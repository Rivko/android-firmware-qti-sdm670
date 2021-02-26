/**
@file MixerSvc.cpp
@brief This file contains the implementation for Audio Matrix
       Mixer.

 */

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/01/2010 AAA      Created file.
==========================================================================*/

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"
#include "MixerSvc_MsgHandlers.h"
#include "MixerSvc_InPortHandler.h"
#include "MixerSvc_OutPortHandler.h"
#include "MixerSvc_Util.h"
#include "EliteAprIf.h"
#include "adsp_media_fmt.h"
#include "adsp_adm_api.h"
#include "MixerSvc_ChanMixUtils.h"

static const uint32_t MAX_CMD_Q_ELEMENTS = 32;
static char *THREAD_NAME;
static const uint32_t THREAD_STACK_SIZE = 8192;
/* The stack size increment is needed to avoid corruption during 32 bit ASM Loopback Test cases */
QURT_ELITE_ALIGN(static char MXATCmdQbuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_CMD_Q_ELEMENTS)], 8);
QURT_ELITE_ALIGN(static char MXARCmdQbuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_CMD_Q_ELEMENTS)], 8);

qurt_elite_atomic_word_t mtMxInstanceCounter;

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static int MtMxWorkLoop(void* pInstance);
static ADSPResult MtMx_EoSV2RespHandler(void* pInstance, elite_msg_any_t* pMsg);
/* -----------------------------------------------------------------------
 ** Message handler f
 ** ----------------------------------------------------------------------- */
// Build the function pointer table for message handling. Some common handlers
// like ReturnSuccess() which ignores the command and returns success, and
// Unsupported() which returns an error and outputs an error message are useful
// for multiple functions that this service does not support.

static elite_svc_msg_handler_func pMtMxHandler[] =
{
		MtMx_CustomMsgHandler,           //0 - ELITE_CUSTOM_MSG
		elite_svc_unsupported,           //1 - UNSUPPORTED CMD
		MtMx_DestroyYourself,            //2 - ELITE_CMD_DESTROY_SERVICE
		elite_svc_unsupported,           //3 - UNSUPPORTED CMD
		elite_svc_unsupported,           //4 - UNSUPPORTED CMD
		elite_svc_unsupported,           //5 - UNSUPPORTED CMD
		elite_svc_unsupported,           //6 - UNSUPPORTED CMD
		elite_svc_unsupported,           //7 - UNSUPPORTED CMD
		elite_svc_unsupported,           //8 - UNSUPPORTED CMD
		elite_svc_unsupported,           //9 - UNSUPPORTED CMD
		elite_svc_unsupported,           //A - UNSUPPORTED CMD
		elite_svc_unsupported,           //B - UNSUPPORTED CMD
		MtMx_EoSV2RespHandler,           //C - ELITE_DATA_EOS: handles eos v2 response from rx AFE
		elite_svc_unsupported,           //D - UNSUPPORTED CMD
		elite_svc_unsupported,           //E - UNSUPPORTED CMD
		elite_svc_unsupported,           //F - UNSUPPORTED CMD
};

//static declaration of matrix objects
static This_t gMXAT;
static char gMXATThreadStack[THREAD_STACK_SIZE];
static This_t gMXAR;
static char gMXARThreadStack[THREAD_STACK_SIZE] __attribute__((__section__(".bss.lpa_audio_static_svc_mxar")));

ADSPResult AudCreateMatrixMixerSvc (uint32_t inputParam, void **handle, MtMxParams_t* pMtMxParams)
{
	// static variable counter to make queue and thread name strings unique
	// limit to 16 bits so it will roll over and and only cost 4 characters in hexadecimal.
	// Queues in existence likely to have unique names, but not required...
	uint32_t count;
	ADSPResult result;
	This_t *me;
	qurt_elite_queue_t *pTempCmdQ;
	char          *stack_ptr;

	if ( ADM_MATRIX_ID_AUDIO_RX == pMtMxParams->matrixID)
	{
		uint32_t i = 0;
		THREAD_NAME = (char*)"MXAR";
		me = (This_t*)&gMXAR;
		
		pTempCmdQ = (qurt_elite_queue_t*)&MXARCmdQbuf;
		me->mtMxID = ADM_MATRIX_ID_AUDIO_RX;

		for (i = 0; i < MT_MXAR_MAX_INPUT_PORTS; i++)
		{
			me->inPortParams[i] = NULL;
		}

		for (i = 0; i < MT_MXAR_MAX_OUTPUT_PORTS; i++)
		{
			me->outPortParams[i] = NULL;
		}

		me->qxdm_log_code = QXDM_LOG_CODE_AUD_MTMX_RX_IN;
	}
	else if (ADM_MATRIX_ID_AUDIO_TX == pMtMxParams->matrixID)
	{
		uint32_t i = 0;
		THREAD_NAME = (char*)"MXAT";
		me = (This_t*)&gMXAT;
		if (!me) return ADSP_ENOMEMORY;

		pTempCmdQ = (qurt_elite_queue_t*)&MXATCmdQbuf;
		me->mtMxID = ADM_MATRIX_ID_AUDIO_TX;

		for (i = 0;i < MT_MXAT_MAX_INPUT_PORTS; i++)
		{
			me->inPortParams[i] = NULL;
		}
		for (i = 0;i < MT_MXAT_MAX_OUTPUT_PORTS; i++)
		{
			me->outPortParams[i] = NULL;
		}

		me->qxdm_log_code = QXDM_LOG_CODE_AUD_MTMX_TX_IN;
	}
	else
	{
		result = ADSP_EBADPARAM;
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMxCreate: rcvd invalid matrix ID [%lu]", pMtMxParams->matrixID);
		return result;
	}

	count = qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;

	// name the cmd queue. Name is concatenated with 5 items:
	snprintf(me->cmdQName, QURT_ELITE_DEFAULT_NAME_LEN, "MtMxCmdQ0%lu", count);

	// static malloc changes
	if (ADSP_FAILED(result = qurt_elite_queue_init(me->cmdQName, MAX_CMD_Q_ELEMENTS, pTempCmdQ)))
	{
		MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Error: Failed to create audio matrix cmdQ");
		AudDestroyMatrixMixerSvc(me);
		// static malloc changes
		me = NULL;
		return result;
	}

	// static malloc changes
	me->serviceHandle.cmdQ = pTempCmdQ;
	// set up channel
	qurt_elite_channel_init(&me->channel);
	if (ADSP_FAILED(result = qurt_elite_channel_addq(&me->channel, me->serviceHandle.cmdQ, 0)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSP_FAILED to add mqs to channel result = %x!!\n", result);
		return(result);
	}

	// populate me
	me->serviceHandle.unSvcId = ELITE_MIXER_SVCID;
	me->serviceHandle.dataQ = NULL;


	me->bIsMxAtOperatingInBurstMode = FALSE;

	// memory for thread stack
	if ( ADM_MATRIX_ID_AUDIO_RX == pMtMxParams->matrixID)
	{
		stack_ptr = gMXARThreadStack;
	}
	else
	{
		stack_ptr = gMXATThreadStack;
	}

	// Launch the thread
	if (ADSP_FAILED(result = qurt_elite_thread_launch(&(me->serviceHandle.threadId), THREAD_NAME,
			stack_ptr, THREAD_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_DYNA_MIXER_SVC_PRIO_ID), MtMxWorkLoop, (void*)me,
			QURT_ELITE_HEAP_DEFAULT)))
	{
		MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Error: Failed to create audio matrix thread");
		AudDestroyMatrixMixerSvc(me);
		me = NULL;
		return result;
	}

	*handle = &(me->serviceHandle);

	//initialize inPortIDMap and outPortIDMap to -1, for robustness
	memset(&me->inPortIDMap, -1, sizeof(me->inPortIDMap));
	memset(&me->outPortIDMap, -1, sizeof(me->outPortIDMap));

	//Initialize KPPS and BW needs
	me->unKpps = 0;
	me->unBandWidth = 0;

	return ADSP_EOK;
}

//This function is the main work loop for the audio matrix mixer service.
static int MtMxWorkLoop(void* pInstance)
{
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Entering matrix mixer workloop...");

	ADSPResult              result;
	uint32_t                    unOutPortID, unInPortID, unCmdBitfield, k, qChannelBit = 0;
	qurt_elite_channel_t *pChannel;
	This_t                       *me           = (This_t*)pInstance;

	//Obtain the channel and channel bit for cmdQ
	pChannel        = qurt_elite_queue_get_channel(me->serviceHandle.cmdQ);
	unCmdBitfield = qurt_elite_queue_get_channel_bit(me->serviceHandle.cmdQ);
	me->unCurrentBitfield = unCmdBitfield;

	// Enter forever loop
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Entering matrix mixer workloop...");

	//Register with ADSPPM (Note: Matrix will vote on behalf of PSPD as well)
	(void)MtMx_RegisterWithAdsppm(me);

	// Enter forever loop
	for(;;)
	{
		//Blocking wait
		me->unChannelStatus = qurt_elite_channel_wait(pChannel, me->unCurrentBitfield);

		//Commands > i/p port > o/p port is the order of priority
		//Command queue handling
		if (me->unChannelStatus & unCmdBitfield)
		{
			// call common utility to process command queue in standard way.
			const uint32_t MtMxCmdTableSize = sizeof(pMtMxHandler)/sizeof(pMtMxHandler[0]);
			result = elite_svc_process_cmd_queue(pInstance, &(me->serviceHandle), pMtMxHandler, MtMxCmdTableSize);
			// If service has been destroyed, exit.
			if (ADSP_ETERMINATED == result) return ADSP_EOK;
		}
		// if a cmd didn't unblock the thread, check if a bufQ or dataQ caused the unblocking
		else
		{
			//i/p queue handling
			if ( 0 != (qChannelBit = (me->unChannelStatus & me->unDataBitfield & me->unCurrentBitfield)))
			{
				k = 31 - (Q6_R_cl0_R((qChannelBit)));
				unInPortID = me->inPortIDMap[k];
				MtMx_ProcessDataQ(me, unInPortID);
			}

			//o/p queue handling
			if ( 0 != (qChannelBit = (me->unChannelStatus & me->unBufBitfield & me->unCurrentBitfield)))
			{
				k = 31 - (Q6_R_cl0_R((qChannelBit)));
				unOutPortID = me->outPortIDMap[k];

				if(ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
				{
					MxAr_ProcessBufQ(me, unOutPortID);
				}
				else
				{
					MxAt_ProcessOutputPortWakeUp(me, unOutPortID);
				}
			}
		}
	}

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Exiting matrix mixer workloop...");
	return 0;
}

ADSPResult MtMx_CustomMsgHandler(void* pInstance, elite_msg_any_t* pMsg)
{
	ADSPResult                       result = ADSP_EOK;
	This_t*                          me = (This_t*)pInstance;
	elite_msg_custom_header_t* pPayload = (elite_msg_custom_header_t*)pMsg->pPayload;

	MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"MtMx #%lu: rcvd custom msg %d",
			me->mtMxID, (int)pPayload->unSecOpCode);

	switch (pPayload->unSecOpCode)
	{
	case ELITEMSG_CUSTOM_CFG_INPUT_PORTS:
	{
		result = MtMx_CfgInputPorts(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_CFG_OUTPUT_PORTS:
	{
		result = MtMx_CfgOutputPorts(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_PAUSE:
	{
		result = MtMx_Pause(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_RUN:
	{
		result = MtMx_Run(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_FLUSH:
	{
		result = MtMx_Flush(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_SUSPEND:
	{
		result = MtMx_Suspend(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_MAP_ROUTINGS:
	{
		result = MtMx_MapRoutings(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_REGISTER_UNDERFLOW_OVERFLOW_EVENT:
	{
		result = MtMx_RegisterUnderflowOverflowEvent(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_RAMP_GAINS:
	{
		result = MtMx_RampGains(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_ADJUST_SESSION_CLK:
	{
		result = MtMx_AdjustSessionClk(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_MUTE:
	{
		result = MtMx_Mute(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_PORT_STATE_CHANGE:
	{
		result = MtMx_PortStateChange(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_SET_PARAM:
	{
		result = MtMx_SetParamHandler(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_GET_PARAM:
	{
		result = MtMx_GetParamHandler(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_PSPD_SET_PARAM:
	{
		result = MtMx_SetPsPdParamHandler(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_SET_PRIMARY_PORT:
	{
		result = MtMx_SetPrimaryOutputPort(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_SET_RATEMATCHING_PARAM:
	{
		result = MtMx_SetRateMatchingParamHandler(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_OUTPUT_PORT_SET_PARAM:
	{
		result = MtMx_SetOutputPortParamHandler(me, pMsg);
		break;
	}
	case ELITEMSG_CUSTOM_MT_MX_OUTPUT_PORT_GET_PARAM:
	{
		result = MtMx_GetOutputPortParamHandler(me, pMsg);
		break;
	}  
	default:
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Custom cmd rcvd by MtMx Svc is invalid");
		result = ADSP_EBADPARAM;
		elite_svc_send_ack(pMsg, result);
		break;
	}
	}
	return result;
}

static ADSPResult MtMx_EoSV2RespHandler(void* pInstance, elite_msg_any_t* pMsg)
{
   This_t* me = (This_t*)pInstance;

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"MtMx : received EoS resp");
   return MtMx_CheckRaiseRenderedEoSv2(me, pMsg, MT_MX_INVALID_PORT_ID, MT_MX_INVALID_PORT_ID, FALSE);
}
