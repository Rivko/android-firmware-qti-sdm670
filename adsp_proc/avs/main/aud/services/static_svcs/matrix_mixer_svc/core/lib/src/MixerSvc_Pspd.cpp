/**
@file MixerSvc_Pspd.cpp
@brief This file defines matrix mixer PSPD utilities.
 */

/*===========================================================================
Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_Pspd.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/10/2014  kr      Re-factoring, De-coupling PSPD and ChannelMixer.
02/04/2014 rkc     Created file.
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "adsp_media_fmt.h"
#include "MixerSvc_Pspd.h"
#include "EliteData_Util.h"

typedef struct MtMx_PspdStruct MtMx_PspdStruct;
struct MtMx_PspdStruct
{
	//General properties
	qurt_elite_channel_t  channel;
	qurt_elite_queue_t    *inpQ;
	qurt_elite_queue_t    *outQ;
	qurt_elite_queue_t    *cmdQ;

	//General media type
	uint32_t              unNumInChannels;
	uint32_t              unNumOutChannels;

    //Address to the PSPD kpps requirement.
	volatile uint32_t	  *punPspdKpps;
	
	//Address to the PSPD BW (Bps) requirement.
	volatile uint32_t   *punPspdBw;

	//Module-specific information	
	//Channel mixer
	eChMixerType          eChMixerTypeVar;

    capi_v2_t             *pChMixerCapiV2;

};

struct capiv2_mediafmt_t
{
    capi_v2_data_format_header_t h;
    capi_v2_standard_data_format_t f;
};

//General PSPD functions
static ADSPResult MtMx_PspdWorkloop(void *arg);
static ADSPResult MtMx_PspdProcCmdQ(MtMx_PspdStruct *pMePspd, elite_msg_any_t *inp_buf, elite_msg_any_t *out_buf);
static ADSPResult MtMx_PspdProcInpQ(MtMx_PspdStruct *pMePspd, elite_msg_any_t *data_buf);
static ADSPResult MtMx_PspdProcOutQ(MtMx_PspdStruct *pMePspd, elite_msg_any_t *data_buf);
static ADSPResult MtMx_PspdProcCustMsg(MtMx_PspdStruct *pMePspd, elite_msg_custom_header_t *cust_msg);
static ADSPResult MtMx_PspdProcess(MtMx_PspdStruct *pMePspd, elite_msg_any_t *inp_msg, elite_msg_any_t *out_msg);
static ADSPResult MtMx_PspdGetMsg(qurt_elite_queue_t *que_ptr, elite_msg_any_t *msg_ptr);
static void MtMx_DestroyPspdSvc(MtMx_PspdStruct *pMePspd, bool_t channel_init);
static ADSPResult MtMx_PspdProcReInitBuffers(MtMx_PspdStruct *pMePspd, uint32_t num_in_ch, 
						uint32_t num_out_ch, uint32_t unPspdBufSize);
static void MtMx_PspdProcUpdateKppsBw(MtMx_PspdStruct *pMePspd);
	
//PSPD Channel mixer related functions
static ADSPResult MtMx_PspdProcChMixerCreateInit(MtMx_PspdStruct *pMePspd, pspd_chmixer_inout_t *param_ptr);
static ADSPResult MtMx_PspdProcChMixerDestroy(MtMx_PspdStruct *pMePspd);
static ADSPResult MtMx_PspdProcChMixerCfgMsg(MtMx_PspdStruct *pMePspd, pspd_chmixer_inout_t *param_ptr);
static ADSPResult MtMx_PspdProcChMixerCoeffSet(MtMx_PspdStruct *pMePspd, audproc_chmixer_param_id_coeff_t *coef);


static ADSPResult MtMx_PspdWorkloop(void *arg)
{
	ADSPResult result = ADSP_EOK;
	MtMx_PspdStruct *pMePspd = (MtMx_PspdStruct*)arg;

	elite_msg_any_t inp_buf = {0}, out_buf = {0};

	//start by listening to cmd, input and output queues
	uint32_t inp_bit_pos = qurt_elite_queue_get_channel_bit(pMePspd->inpQ);
	uint32_t out_bit_pos = qurt_elite_queue_get_channel_bit(pMePspd->outQ);
	uint32_t cmd_bit_pos = qurt_elite_queue_get_channel_bit(pMePspd->cmdQ);
	uint32_t wait_mask = inp_bit_pos | out_bit_pos | cmd_bit_pos;

	for(;;)
	{
		uint32_t set_mask = qurt_elite_channel_wait(&pMePspd->channel, wait_mask);

		if(set_mask & wait_mask & cmd_bit_pos)
		{
			//process command and continue
			result = MtMx_PspdProcCmdQ(pMePspd, &inp_buf, &out_buf);
			if(ADSP_ETERMINATED == result) return ADSP_EOK;
			continue;
		}

		if(set_mask & wait_mask & inp_bit_pos)
		{
			result = MtMx_PspdProcInpQ(pMePspd, &inp_buf);
			if(NULL != inp_buf.pPayload)
			{
				//got an input buffer, stop processing input queue until this is consumed.
				wait_mask ^= inp_bit_pos;
			}
			if(!inp_buf.pPayload || !out_buf.pPayload)
			{
				//continue processing queues until we have both input and output
				continue;
			}
		}

		if(set_mask & wait_mask & out_bit_pos)
		{
			result = MtMx_PspdProcOutQ(pMePspd, &out_buf);
			if(NULL != out_buf.pPayload)
			{
				//got an output buffer, stop processing output queue until this is consumed.
				wait_mask ^= out_bit_pos;
			}
			if(!inp_buf.pPayload || !out_buf.pPayload)
			{
				//continue processing queues until we have both input and output
				continue;
			}
		}

		//At this point we have both input and output buffers. Process input and generate output.
		//A key assumption is that the input buffer is completely consumed by the APPI.
		//In a general case, this isn't so. So need to revisit this code at a later time.
		result = MtMx_PspdProcess(pMePspd, &inp_buf, &out_buf);
		memset(&inp_buf, 0, sizeof(inp_buf));
		memset(&out_buf, 0, sizeof(out_buf));
		wait_mask |= (inp_bit_pos | out_bit_pos);
	}

	return result;
}

static ADSPResult MtMx_PspdProcCmdQ(MtMx_PspdStruct *pMePspd, elite_msg_any_t *inp_buf, elite_msg_any_t *out_buf)
{
	elite_msg_any_t msg = {0}, dataQMsg = {0};
	ADSPResult result = MtMx_PspdGetMsg(pMePspd->cmdQ, &msg);
	if(ADSP_EOK != result) return result;

	switch(msg.unOpCode)
	{
		case ELITE_CUSTOM_MSG:
		{
			if(!msg.pPayload)
			{	
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null payload detected in input message. Memory will be leaked.");
				return ADSP_EBADPARAM;
			}
			elite_msg_custom_header_t *custom_msg = (elite_msg_custom_header_t *)msg.pPayload;
			result = MtMx_PspdProcCustMsg(pMePspd, custom_msg);
			return elite_msg_finish_msg(&msg, result);
		}
		case ELITE_CMD_DESTROY_SERVICE:
		{
			//release input/output buffers already dequeued
			if(inp_buf->pPayload)
			{
				(void)elite_msg_finish_msg(inp_buf, ADSP_ENOTREADY);
				memset(inp_buf, 0, sizeof(*inp_buf));
			}
			if(out_buf->pPayload)
			{
				(void)elite_msg_finish_msg(out_buf, ADSP_ENOTREADY);
				memset(out_buf, 0, sizeof(*out_buf));
			}
			MtMx_DestroyPspdSvc(pMePspd, true);
			(void) elite_msg_finish_msg(&msg, ADSP_EOK);
			return ADSP_ETERMINATED;
		}
		case ELITE_CMD_FLUSH:
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PSPD: Entering (Flush) cmd handler");
			//return input buffers if any in queue
			while(1)
			{			
				// Non-blocking MQ receive
				result = qurt_elite_queue_pop_front(pMePspd->inpQ, (uint64_t*)&dataQMsg);
				if (ADSP_ENEEDMORE == result)
				{
					result = ADSP_EOK;
					break;
				}
				if (ADSP_FAILED(result))
				{
					break;
				}
				result = elite_msg_finish_msg(&dataQMsg, ADSP_EOK);
				if (ADSP_FAILED(result))
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PSPD inpQ: Failed to release input data Q message.");
				}
			}
			//return output buffers if any in queue			
			while(1)
			{
				// Non-blocking MQ receive
				result = qurt_elite_queue_pop_front(pMePspd->outQ, (uint64_t*)&dataQMsg);
				if (ADSP_ENEEDMORE == result)
				{
					result = ADSP_EOK;
					break;
				}
				if (ADSP_FAILED(result))
				{
					break;
				}
				result = elite_msg_finish_msg(&dataQMsg, ADSP_EOK);
				if (ADSP_FAILED(result))
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PSPD outQ: Failed to release output data Q message.");
				}
			}
			
            if (pMePspd->pChMixerCapiV2)
			{			
                capi_v2_prop_t setProp[] = { { CAPI_V2_ALGORITHMIC_RESET, {NULL,0,0,}, {true,true,0} } };

                capi_v2_proplist_t setPropList = {sizeof(setProp)/sizeof(setProp[0]), setProp};
                result = capi_v2_err_to_adsp_result(pMePspd->pChMixerCapiV2->vtbl_ptr->set_properties(pMePspd->pChMixerCapiV2,&setPropList));

				MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PSPD: channel mixer flush cmd returned with status %d", (int)result);
			}

			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PSPD: Leaving (Flush) cmd handler with status %d", (int)result);
			return elite_msg_finish_msg(&msg, result);
		}
		default:
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PSPD CmdQ received an unsupported command with opcode = %lu", msg.unOpCode);
			return elite_msg_finish_msg(&msg, ADSP_EUNSUPPORTED);
		}
	}
}

static ADSPResult MtMx_PspdProcInpQ(MtMx_PspdStruct *pMePspd, elite_msg_any_t *data_buf)
{
	elite_msg_any_t msg = {0};
	ADSPResult result = MtMx_PspdGetMsg(pMePspd->inpQ, &msg);
	if(ADSP_EOK != result) return result;

	switch(msg.unOpCode)
	{
	case ELITE_DATA_BUFFER_V2:
	{
		if(!msg.pPayload)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null payload detected in input message. Memory will be leaked.");
			return ADSP_EBADPARAM;
		}
		*data_buf = msg;
		return ADSP_EOK;
	}
	default:
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid opcode for input message: %lu", msg.unOpCode);
		return elite_msg_finish_msg(&msg, ADSP_EBADPARAM);
	}
	}
}

static ADSPResult MtMx_PspdProcOutQ(MtMx_PspdStruct *pMePspd,
		elite_msg_any_t *data_buf)
{
	elite_msg_any_t msg = {0};
	ADSPResult result = MtMx_PspdGetMsg(pMePspd->outQ, &msg);
	if(ADSP_EOK != result) return result;

	switch(msg.unOpCode)
	{
	case ELITE_DATA_BUFFER_V2:
	{
		if(!msg.pPayload)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null payload detected in output message. Memory will be leaked.");
			return ADSP_EBADPARAM;
		}
		*data_buf = msg;
		return ADSP_EOK;
	}
	default:
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid opcode for output message: %lu", msg.unOpCode);
		return elite_msg_finish_msg(&msg, ADSP_EBADPARAM);
	}
	}
}

static ADSPResult MtMx_PspdProcCustMsg(MtMx_PspdStruct *pMePspd, elite_msg_custom_header_t *cust_msg)
{
	switch(cust_msg->unSecOpCode)
	{
	case ELITEMSG_CUSTOM_PSPD_CREATE_CHMIXER:
	{
		EliteMsg_CustomPspdCfgChmixInout *chmix_params = (EliteMsg_CustomPspdCfgChmixInout *)cust_msg;
		return MtMx_PspdProcChMixerCreateInit(pMePspd, &chmix_params->param);
	}
	case ELITEMSG_CUSTOM_PSPD_DESTROY_CHMIXER:
	{
		return MtMx_PspdProcChMixerDestroy(pMePspd);
	}
	case ELITEMSG_CUSTOM_PSPD_CFG_CHMIXER_INOUT:
	{
		EliteMsg_CustomPspdCfgChmixInout *chmix_params = (EliteMsg_CustomPspdCfgChmixInout *)cust_msg;
		return MtMx_PspdProcChMixerCfgMsg(pMePspd, &chmix_params->param);
	}
	case ELITEMSG_CUSTOM_PSPD_SET_CHMIXER_COEF:
	{
		EliteMsg_CustomPspdSetChmixCoef *coef_params = (EliteMsg_CustomPspdSetChmixCoef *)cust_msg;
        return MtMx_PspdProcChMixerCoeffSet(pMePspd, coef_params->coef);
	}
	case ELITEMSG_CUSTOM_PSPD_REINIT_BUFFERS:
	{
		EliteMsg_CustomPspdReInitBuffers *pspd_reinit_params = (EliteMsg_CustomPspdReInitBuffers *)cust_msg;
		return MtMx_PspdProcReInitBuffers(pMePspd, pspd_reinit_params->num_in_ch, 
										pspd_reinit_params->num_out_ch, pspd_reinit_params->unPspdBufSize);
	}	
	default:
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid secondary opcode %lu", cust_msg->unSecOpCode);
		return ADSP_EBADPARAM;
	}
	}
}

static ADSPResult MtMx_PspdProcess(MtMx_PspdStruct *pMePspd, elite_msg_any_t *inp_msg,	elite_msg_any_t *out_msg)
{
	elite_msg_data_buffer_v2_t *inp_buf = (elite_msg_data_buffer_v2_t *)inp_msg->pPayload;
	elite_msg_data_buffer_v2_t *out_buf = (elite_msg_data_buffer_v2_t *)out_msg->pPayload;

    /* channel mixer processing */	
	
	int8_t *psBufPtrIn = (int8_t *)inp_buf->data_ptr;
	int8_t *psBufPtrOut = NULL;

    if ((PSPD_CHMIXER_NONE == pMePspd->eChMixerTypeVar) && (NULL == pMePspd->pChMixerCapiV2))
	{
		psBufPtrOut = (int8_t *)out_buf->data_ptr;		
		memscpy((void*)psBufPtrOut, inp_buf->nActualSize, (void*)psBufPtrIn, inp_buf->nActualSize);

		elite_msg_finish_msg(inp_msg, ADSP_EOK);
		elite_msg_finish_msg(out_msg, ADSP_EOK);

		return ADSP_EOK;
	}
	
    if(PSPD_CHMIXER_NONE != pMePspd->eChMixerTypeVar && NULL != pMePspd->pChMixerCapiV2)
	{
        capi_v2_buf_t inBufs[PSPD_MAX_CHAN], outBufs[PSPD_MAX_CHAN];
		uint32_t actual_len_per_ch = 0, max_len_per_ch = 0;

		actual_len_per_ch = inp_buf->nActualSize / pMePspd->unNumInChannels;
		max_len_per_ch = inp_buf->nMaxSize / pMePspd->unNumInChannels;

		for(uint32_t i = 0; i < pMePspd->unNumInChannels; i++)
		{
			inBufs[i].data_ptr = psBufPtrIn;
			inBufs[i].actual_data_len = actual_len_per_ch;
			inBufs[i].max_data_len = max_len_per_ch;
			psBufPtrIn += max_len_per_ch;
		}

			psBufPtrOut = (int8_t *)out_buf->data_ptr;
			actual_len_per_ch = 0;
			max_len_per_ch = out_buf->nMaxSize / pMePspd->unNumOutChannels;

		for(uint32_t i = 0; i < pMePspd->unNumOutChannels; i++)
		{
			outBufs[i].data_ptr = psBufPtrOut;
			outBufs[i].actual_data_len = actual_len_per_ch;
			outBufs[i].max_data_len = max_len_per_ch;
			psBufPtrOut += max_len_per_ch;
		}

        {
            capi_v2_stream_data_t in_buf, out_buf;

            in_buf.bufs_num  = pMePspd->unNumInChannels;
            in_buf.buf_ptr   = inBufs;
            out_buf.bufs_num = pMePspd->unNumOutChannels;
            out_buf.buf_ptr  = outBufs;

            capi_v2_stream_data_t *input[] = { &in_buf };
            capi_v2_stream_data_t *output[] = { &out_buf };

            (void)pMePspd->pChMixerCapiV2->vtbl_ptr->process(pMePspd->pChMixerCapiV2, input, output);
        }
    }
    
	elite_msg_finish_msg(inp_msg, ADSP_EOK);
	elite_msg_finish_msg(out_msg, ADSP_EOK);

	return ADSP_EOK;
}

//if this func is successful, caller has to return/respond to the message
//if this func fails, caller need not do anything further.
static ADSPResult MtMx_PspdGetMsg(qurt_elite_queue_t *que_ptr, elite_msg_any_t *msg_ptr)
{
	ADSPResult result = qurt_elite_queue_pop_front(que_ptr, (uint64_t*)msg_ptr);
	if(ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to dequeue with error = %d", result);
	}
	return result;
}

static void MtMx_DestroyPspdSvc(MtMx_PspdStruct *pMePspd, bool_t channel_init)
{
	if(!pMePspd) return;

	//reset PSPD KPPS requirement
	*(pMePspd->punPspdKpps) = 0;	
	*(pMePspd->punPspdBw) = 0;
	
	if(pMePspd->inpQ)
	{
		//drain the queue and destroy
		uint64_t payload = 0;
		while(ADSP_EOK == qurt_elite_queue_pop_front(pMePspd->inpQ, &payload))
		{
			elite_msg_finish_msg((elite_msg_any_t *)&payload, ADSP_ENOTREADY);
		}

		qurt_elite_queue_destroy(pMePspd->inpQ);
	}

	if(pMePspd->outQ)
	{
		//drain the queue and destroy
		uint64_t payload = 0;
		while(ADSP_EOK == qurt_elite_queue_pop_front(pMePspd->outQ, &payload))
		{
			elite_msg_finish_msg((elite_msg_any_t *)&payload, ADSP_ENOTREADY);
		}

		qurt_elite_queue_destroy(pMePspd->outQ);
	}

	if(pMePspd->cmdQ)
	{
		//drain the queue and destroy
		uint64_t payload = 0;
		while(ADSP_EOK == qurt_elite_queue_pop_front(pMePspd->cmdQ, &payload))
		{
			elite_msg_finish_msg((elite_msg_any_t *)&payload, ADSP_ENOTREADY);
		}

		qurt_elite_queue_destroy(pMePspd->cmdQ);
	}

	//destroy channel if applicable
	if(channel_init)
	{
		qurt_elite_channel_destroy(&pMePspd->channel);
	}

	if(pMePspd)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Freeing PSPD pMePspd 0x%p", pMePspd);
		QURT_ELITE_FREE(pMePspd);
	}
}

static ADSPResult MtMx_PspdProcChMixerCreateInit(MtMx_PspdStruct *pMePspd, pspd_chmixer_inout_t *param_ptr)
{
    ADSPResult result = ADSP_EFAILED;

    capi_v2_init_memory_requirement_t ChMixCapiV2Size = {0};

    //Get Capiv2 Init memory requirement.
    {
        capi_v2_prop_t initMemReqProp =
        {
                CAPI_V2_INIT_MEMORY_REQUIREMENT,
                { (int8_t*)&ChMixCapiV2Size,    sizeof(ChMixCapiV2Size), sizeof(ChMixCapiV2Size) },
                { false,false,0 }
        };
        capi_v2_proplist_t propList = {1, &initMemReqProp};

        switch(param_ptr->eChannelMixerType)
        {
            case PSPD_CHMIXER_QCOM:
            {
                result = capi_v2_err_to_adsp_result(capi_v2_chmixer_get_static_properties(NULL,&propList));
                break;
            }
            default:
            {
                result = ADSP_EBADPARAM;
                break;
            }
        }
    }

    if(ADSP_EOK != result)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Channel mixer CapiV2 getsize returned error %d", result);
        goto __bailoutMtMxPspdProcChMixerCreateInit;
    }

    pMePspd->pChMixerCapiV2 = (capi_v2_t *)qurt_elite_memory_malloc(ChMixCapiV2Size.size_in_bytes, QURT_ELITE_HEAP_DEFAULT);
    if(NULL == pMePspd->pChMixerCapiV2)
    {
        result = ADSP_ENOMEMORY;
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for channel mixer CapiV2. Result = %d", result);
        goto __bailoutMtMxPspdProcChMixerCreateInit;
    }
    else
    {
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Allocate memory for channel mixer CapiV2 0x%p, size: %lu success", pMePspd->pChMixerCapiV2, ChMixCapiV2Size.size_in_bytes);
    }

    switch(param_ptr->eChannelMixerType)
    {
        case PSPD_CHMIXER_QCOM:
        {
            result = capi_v2_err_to_adsp_result(capi_v2_chmixer_init(pMePspd->pChMixerCapiV2, NULL));
            break;
        }
        default:
        {
            result = ADSP_EBADPARAM;
            break;
        }
    }

    if(ADSP_EOK != result)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Channel mixer init failed with result = %d", result);
        if(NULL != pMePspd->pChMixerCapiV2)
        {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Freeing memory for channel mixer CapiV2 0x%p", pMePspd->pChMixerCapiV2);
            QURT_ELITE_FREE(pMePspd->pChMixerCapiV2);
        }
        goto __bailoutMtMxPspdProcChMixerCreateInit;
    }
    else
    {
        pMePspd->eChMixerTypeVar = param_ptr->eChannelMixerType;
        MtMx_PspdProcChMixerCfgMsg(pMePspd,param_ptr);
    }

    return result;

    __bailoutMtMxPspdProcChMixerCreateInit:
    pMePspd->unNumInChannels = param_ptr->num_in_ch;
    pMePspd->unNumOutChannels = param_ptr->num_in_ch; //Setting output to be same as input as channel mixer is not present.
    pMePspd->eChMixerTypeVar = PSPD_CHMIXER_NONE; //None indicates init did not succeed/did not happen.
    pMePspd->pChMixerCapiV2 = NULL;
    return result;
}

static ADSPResult MtMx_PspdProcChMixerDestroy(MtMx_PspdStruct *pMePspd)
{
    if(NULL != pMePspd->pChMixerCapiV2)
    {
        if(PSPD_CHMIXER_NONE != pMePspd->eChMixerTypeVar)
        {
            (void) pMePspd->pChMixerCapiV2->vtbl_ptr->end(pMePspd->pChMixerCapiV2);
        }

        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Freeing memory for channel mixer CapiV2 0x%p", pMePspd->pChMixerCapiV2);
        QURT_ELITE_FREE(pMePspd->pChMixerCapiV2);
    }

    //Do not change the value of pMePspd->unNumInChannels.
    //Re-assign pMePspd->unNumOutChannels to pMePspd->unNumInChannels as channel mixer is not present.
    pMePspd->unNumOutChannels = pMePspd->unNumInChannels;
    pMePspd->eChMixerTypeVar = PSPD_CHMIXER_NONE;
    pMePspd->pChMixerCapiV2 = NULL;

    return ADSP_EOK;
}

static ADSPResult MtMx_PspdProcChMixerCfgMsg(MtMx_PspdStruct *pMePspd, pspd_chmixer_inout_t *param_ptr)
{
    ADSPResult result = ADSP_EFAILED;
    uint32_t bit_width = (param_ptr->bit_width > 16) ? 32:16; /*CAPI takes only 16/32 as bit width*/
    uint32_t q_factor = elite_data_get_q_format_factor(param_ptr->bit_width);
    capiv2_mediafmt_t outMediaFmt =
    {
            {
                    CAPI_V2_FIXED_POINT
            },
            {
                    CAPI_V2_DATA_FORMAT_INVALID_VAL,
                    param_ptr->num_out_ch,
                    bit_width,
                    q_factor,
                    CAPI_V2_DATA_FORMAT_INVALID_VAL,
                    1,
                    CAPI_V2_DEINTERLEAVED_UNPACKED,
                    { 0 }
            }
    };

    for(uint16_t i = 0; i < param_ptr->num_out_ch; i++)
    {
        outMediaFmt.f.channel_type[i] = param_ptr->out_ch_map[i];
    }

    capiv2_mediafmt_t InpMediaFmt =
    {
            {
                    CAPI_V2_FIXED_POINT
            },
            {
                    CAPI_V2_DATA_FORMAT_INVALID_VAL,
                    param_ptr->num_in_ch,
                    bit_width,
                    q_factor,
                    CAPI_V2_DATA_FORMAT_INVALID_VAL,
                    1,
                    CAPI_V2_DEINTERLEAVED_UNPACKED,
                    { 0 }
            }
    };

    for(uint16_t i = 0; i < param_ptr->num_in_ch; i++)
    {
        InpMediaFmt.f.channel_type[i] = param_ptr->in_ch_map[i];
    }

    capi_v2_prop_t setProp[] =
    {
            {
                CAPI_V2_INPUT_MEDIA_FORMAT,
                { (int8_t*)&InpMediaFmt, sizeof(InpMediaFmt), sizeof(InpMediaFmt) },
                { true,true,0 }
            },
            {
                CAPI_V2_OUTPUT_MEDIA_FORMAT,
                { (int8_t*)&outMediaFmt, sizeof(outMediaFmt), sizeof(outMediaFmt) },
                { true,false,0 }
            }
    };

    capi_v2_proplist_t setPropList = {sizeof(setProp)/sizeof(setProp[0]), setProp};

    switch(pMePspd->eChMixerTypeVar)
    {
        case PSPD_CHMIXER_QCOM:
        {
            result = capi_v2_err_to_adsp_result(pMePspd->pChMixerCapiV2->vtbl_ptr->set_properties(pMePspd->pChMixerCapiV2,  &setPropList));
            break;
        }
        default:
        {
            result = ADSP_EBADPARAM;
            break;
        }
    }
    if(ADSP_EOK != result)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to set channel mixer i/o channel config with result = %d", result);
    }
    else
    {
        pMePspd->unNumInChannels = param_ptr->num_in_ch;
        pMePspd->unNumOutChannels = param_ptr->num_out_ch;      
    }

    // update KPPS requirement
    MtMx_PspdProcUpdateKppsBw(pMePspd);
    
    return result;
}

static ADSPResult MtMx_PspdProcChMixerCoeffSet(MtMx_PspdStruct *pMePspd, audproc_chmixer_param_id_coeff_t *coef)
{
    //custom coef are applicable only for QCOM channel mixer.
    if(pMePspd->eChMixerTypeVar != PSPD_CHMIXER_QCOM)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Custom mixer coefficients received for non-QCOM channel mixer, not processing");
        return ADSP_EUNEXPECTED;
    }

    ADSPResult result = ADSP_EOK;
    const uint32_t paramSize = sizeof(audproc_chmixer_param_id_coeff_t) +
                                    + (sizeof(uint16_t)*coef->num_output_channels)
                                    + (sizeof(uint16_t)*coef->num_input_channels)
                                    + (sizeof(int16_t)*coef->num_output_channels*coef->num_input_channels);

    capi_v2_buf_t paramBuf = {(int8_t*)(coef), paramSize, paramSize};

    result = capi_v2_err_to_adsp_result(pMePspd->pChMixerCapiV2->vtbl_ptr->set_param(pMePspd->pChMixerCapiV2, AUDPROC_CHMIXER_PARAM_ID_COEFF, NULL , &paramBuf));

    if(ADSP_EOK != result)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to set channel mixer custom coef with result = %d", result);
    }
    return result;
}

static ADSPResult MtMx_PspdProcReInitBuffers(MtMx_PspdStruct *pMePspd, uint32_t num_in_ch, 
											uint32_t num_out_ch, uint32_t unPspdBufSize)
{
	pMePspd->unNumInChannels = num_in_ch;
	pMePspd->unNumOutChannels = num_out_ch;
		
	return ADSP_EOK;
}

static void MtMx_PspdProcUpdateKppsBw(MtMx_PspdStruct *pMePspd)
{
	uint32_t unPspdKpps = 0;
	uint32_t unPspdBw = 0; // Bps
	
	// if no modules are enabled in PSPD
    if ((PSPD_CHMIXER_NONE == pMePspd->eChMixerTypeVar) && (NULL == pMePspd->pChMixerCapiV2))
	{
		unPspdKpps += PSPD_KPPS_NO_MODULE_ENABLED;
		unPspdBw += PSPD_BW_NO_MODULE_ENABLED;
	}
	
	// update channel mixer kpps requirement
    if(PSPD_CHMIXER_NONE != pMePspd->eChMixerTypeVar && NULL != pMePspd->pChMixerCapiV2)
	{
		if (pMePspd->unNumInChannels <= 2)
		{
			unPspdKpps += PSPD_KPPS_CHMIXER_MODULE_ENABLED_2CH;
			unPspdBw += PSPD_BW_CHMIXER_MODULE_ENABLED_2CH;
		}
		else if (pMePspd->unNumInChannels <= 6)
		{
			unPspdKpps += PSPD_KPPS_CHMIXER_MODULE_ENABLED_6CH;
			unPspdBw += PSPD_BW_CHMIXER_MODULE_ENABLED_6CH;
		}
		else
		{
			unPspdKpps += PSPD_KPPS_CHMIXER_MODULE_ENABLED_8CH;		
			unPspdBw += PSPD_BW_CHMIXER_MODULE_ENABLED_8CH;
		}
	}	

	//update PSPD KPPS
	*(pMePspd->punPspdKpps) = unPspdKpps;
	*(pMePspd->punPspdBw) = unPspdBw;
	return;
}

ADSPResult MtMx_CreatePspdSvc(elite_svc_handle_t *handle, uint32_t unInPortID, 
								uint32_t unOutPortID, volatile uint32_t* punPspdKpps, volatile uint32_t* punPspdBw)
{
	ADSPResult result = ADSP_EOK;
	bool_t channel_init = false;

	char MtMxPspdInpQName[QURT_ELITE_DEFAULT_NAME_LEN];
	char MtMxPspdOutQName[QURT_ELITE_DEFAULT_NAME_LEN];
	char MtMxPspdCmdQName[QURT_ELITE_DEFAULT_NAME_LEN];
	char MtMxPspdThreadName[QURT_ELITE_DEFAULT_NAME_LEN];

	snprintf(MtMxPspdInpQName, QURT_ELITE_DEFAULT_NAME_LEN, "PspdInpQ%lu%lu", unInPortID, unOutPortID);
	snprintf(MtMxPspdOutQName, QURT_ELITE_DEFAULT_NAME_LEN, "PspdOutQ%lu%lu", unInPortID, unOutPortID);
	snprintf(MtMxPspdCmdQName, QURT_ELITE_DEFAULT_NAME_LEN, "PspdCmdQ%lu%lu", unInPortID, unOutPortID);
	snprintf(MtMxPspdThreadName, QURT_ELITE_DEFAULT_NAME_LEN, "PspdThrd%lu%lu", unInPortID, unOutPortID);

	MtMx_PspdStruct *pMePspd = (MtMx_PspdStruct *)qurt_elite_memory_malloc(sizeof(MtMx_PspdStruct), QURT_ELITE_HEAP_DEFAULT);
	if(!pMePspd)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for PSPD state struct");
		return ADSP_ENOMEMORY;
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Allocate memory success PSPD pMePspd 0x%p, size: %u", pMePspd, sizeof(MtMx_PspdStruct));
	}

	memset(pMePspd, 0, sizeof(*pMePspd));
	pMePspd->unNumInChannels = 2;
	pMePspd->unNumOutChannels = 2;
	pMePspd->eChMixerTypeVar = PSPD_CHMIXER_NONE;
	pMePspd->pChMixerCapiV2 = NULL;
	pMePspd->punPspdKpps = punPspdKpps;
	pMePspd->punPspdBw = punPspdBw;
	*(pMePspd->punPspdKpps) = PSPD_KPPS_NO_MODULE_ENABLED;
	*(pMePspd->punPspdBw) = PSPD_KPPS_NO_MODULE_ENABLED;

	qurt_elite_channel_init(&pMePspd->channel);
	channel_init = true;

	result = qurt_elite_queue_create(MtMxPspdInpQName, QURT_ELITE_DEFAULT_NAME_LEN, &pMePspd->inpQ);
	if(ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create MtMx PSPD input queue with result = %d", result);
		goto __bailout;
	}

	result = qurt_elite_queue_create(MtMxPspdOutQName, QURT_ELITE_DEFAULT_NAME_LEN, &pMePspd->outQ);
	if(ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create MtMx PSPD output queue with result = %d", result);
		goto __bailout;
	}

	result = qurt_elite_queue_create(MtMxPspdCmdQName, QURT_ELITE_DEFAULT_NAME_LEN, &pMePspd->cmdQ);
	if(ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create MtMx PSPD command queue with result = %d", result);
		goto __bailout;
	}

	result = qurt_elite_channel_addq(&pMePspd->channel, pMePspd->inpQ, 0);
	if(ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add MtMx PSPD input queue to channel with result = %d", result);
		goto __bailout;
	}

	result = qurt_elite_channel_addq(&pMePspd->channel, pMePspd->outQ, 1);
	if(ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add MtMx PSPD output queue to channel with result = %d", result);
		goto __bailout;
	}

	result = qurt_elite_channel_addq(&pMePspd->channel, pMePspd->cmdQ, 2);
	if(ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add MtMx PSPD command queue to channel with result = %d", result);
		goto __bailout;
	}

	if (ADSP_FAILED(result = qurt_elite_thread_launch(&handle->threadId, MtMxPspdThreadName, NULL, 4096,
	                                                  elite_get_thrd_prio(ELITETHREAD_DYNA_MTMX_PSPD_SVC_PRIO_ID), MtMx_PspdWorkloop, (void*)pMePspd, QURT_ELITE_HEAP_DEFAULT)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to launch MtMx PSPD service thread! \n");
		goto __bailout;
	}

	handle->cmdQ = pMePspd->cmdQ;
	handle->dataQ = pMePspd->inpQ;
	handle->gpQ = pMePspd->outQ;
	handle->unSvcId = ELITE_PSPD_SVCID;

	return ADSP_EOK;

	__bailout:
	MtMx_DestroyPspdSvc(pMePspd, channel_init);
	// Clear pspd handle structure
	memset((void*)handle, 0, sizeof(elite_svc_handle_t));
	return result;
}
