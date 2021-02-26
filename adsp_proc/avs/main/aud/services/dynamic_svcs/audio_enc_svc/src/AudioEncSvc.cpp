/*========================================================================

 *//** @file AudioEncSvc.cpp
This file contains Elite Audio encoder service.

Copyright (c) 2010-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_enc_svc/src/AudioEncSvc.cpp#2 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
03/26/10   WJ      Created file.
02/07/18   npendeka      Modified
========================================================================== */

#include "AudioEncSvc_Includes.h"
#include "AudioEncSvc_PushMode.h"
#include "AudioEncSvc_CapiV2Util.h"
#include <stringl/stringl.h>
#include "AudioEncSvcImc.h"
static char THREAD_NAME[]="AEnc";
/**
 * bandwidth required for single stream decode
 * values are so due to legacy implementation.
 *
 * assuming 48kHz sampling, 2 channels, 2 bytes per channel.
 * in bytes per second.
 */
#define SINGLE_STREAM_DECODED_BW  (48000 * 2 * 2)

#define MIN_AUDENC_THREAD_STACK_SIZE 4096

#define AUD_ENC_PROCESS_STACK_SIZE 512

/* -----------------------------------------------------------------------
 ** Global Variables
 ** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
// Main work loop for service thread. Pulls msgs off of queues and processes them.
static int AudioEncSvc_WorkLoop(void* pInstance);

// Main processing function for Encoding the PCM data
static ADSPResult AudioEncSvc_Process(AudioEncSvc_t* pMe);
// Main processing function for Encoding into PCM format
static ADSPResult AudioEncSvc_ProcessPCM(AudioEncSvc_t* pMe);
// Main processing function for Encoding and loopback to Rx StreamRouter
static ADSPResult AudioEncSvc_ProcessCompressedLoopback(AudioEncSvc_t* pMe);
// wrapper around the above two functions
static ADSPResult AudioEncSvc_Proc(AudioEncSvc_t* pMe);

// message handler functions
static int AudioEncSvc_DestroyYourself(AudioEncSvc_t* pMe);
static int AudioEncSvc_ReturnUnsupported(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_Flush(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_Run(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_Pause(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_Suspend(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_PrepDisconnect(AudioEncSvc_t *pMe);
static void AudioEncSvc_DestroyInpDataQ(AudioEncSvc_t* pMe);
static void AudioEncSvc_DestroyOutDataQ (AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_ProcessCmdQueue(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_ProcessInpDataQueue(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_ProcessOutDataQueue(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_ProcessOutDataQueue_Client(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_ProcessOutDataQueue_PeerService(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_Encode(AudioEncSvc_t* pMe, enc_CAPI_container_t *capi_container, int *nReturnOutBuf, bool_t *compr_need_more, uint32_t *bytes_consumed_per_frame);
static ADSPResult AudioEncSvc_EncodeUsingAllCapis(AudioEncSvc_t* pMe,int *nReturnOutBuf, bool_t *compr_need_more);
static ADSPResult AudioEncSvc_ProcessCustomMsg(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_ProcessEos(AudioEncSvc_t* pMe);
static ADSPResult AudioEncSvc_ConnectToService(AudioEncSvc_t *pMe);
static ADSPResult AudioEncSvc_DisconnectFromService(AudioEncSvc_t* pMe);

//function to check if more data needs to be copied to internal buffer before process
static bool_t is_more_data_required(AudioEncSvc_t* pMe, enc_CAPI_container_t *capi_container, bool_t compr_need_more);
//function to update the next frames timestamp
static void AudioEncSvc_UpdateFrameTimestamp(AudioEncSvc_t* pMe, enc_CAPI_container_t *first_capi, uint32_t consumed_samples);

/* -----------------------------------------------------------------------
 ** Queue handler f
 ** ----------------------------------------------------------------------- */
typedef int (*qurt_elite_queueHandler_f) (AudioEncSvc_t* pMe /*pInstance*/);

qurt_elite_queueHandler_f pEncQHandler[AUD_ENC_NUM_Q]=
{
		AudioEncSvc_ProcessCmdQueue,
		AudioEncSvc_ProcessOutDataQueue,
		AudioEncSvc_ProcessInpDataQueue,
};


/* -----------------------------------------------------------------------
 ** Message handler f
 ** ----------------------------------------------------------------------- */
// Build the function pointer table for message handling. Some common handlers
// like ReturnSuccess() which ignores the command and returns success, and
// Unsupported() which returns an error and outputs an error message are useful
// for multiple functions that this service does not support.
// In this example, this service has no concept of state. so just return success
// when RUN or STOP is commanded.
qurt_elite_queueHandler_f pASMToEncCmdHandler[] =
{

		AudioEncSvc_ProcessCustomMsg,      // 0 - ELITE_CUSTOM_MSG
		AudioEncSvc_ReturnUnsupported,      // 1 - ELITE_CMD_START_SERVICE
		AudioEncSvc_DestroyYourself,        // 2 - ELITE_CMD_DESTROY_SERVICE
		AudioEncSvc_ConnectToService,       // 3 - ELITE_CMD_CONNECT
		AudioEncSvc_DisconnectFromService,  // 4 - ELITE_CMD_DISCONNECT
		AudioEncSvc_Pause,                  // 5 - ELITE_CMD_PAUSE
		AudioEncSvc_Run,                    // 6 - ELITE_CMD_RUN
		AudioEncSvc_Flush,                  // 7 - ELITE_CMD_FLUSH
		AudioEncSvc_SetParam,                 // 8 - ELITE_CMD_SET_PARAM
};


/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */
ADSPResult AudioEncSvc_Create (uint32_t inputParam, void **handle)
{
	/* Sanity checks of input params */
	ADSPResult result;
	AudioEncSvcInitParams_t* pInitParams = (AudioEncSvcInitParams_t*) inputParam;
	if (!pInitParams || !handle )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid input params!");
		return ADSP_EBADPARAM;
	}

	//initialize
	*handle = NULL;
	bool_t is_out_dataQ_needed = TRUE;
	uint32_t threadStackSize = 0;
	int32_t nPriority;

	/* Allocate a memory buffer enough big to hold both thread stack and
      encoder instance size. The start of thread stack shall be 8 byte alignned
      and the thread stack size need to round up to a multiple integer of 8 bytes.
	 */

	if (ENC_SVC_PUSH_MODE == pInitParams->encMode)
	{
		is_out_dataQ_needed = FALSE;
	}

	if ( (ENC_SVC_IO_FORMAT_CONV_TYPE_AUTO != pInitParams->io_fmt_conv) &&
			( (AudioEncSvc_IsPcmFmt(pInitParams->ulConfigId) ||
					(ENC_SVC_PUSH_MODE == pInitParams->encMode)  )
			)
	)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Only auto mode supported for PCM (regular, push mode) ");
		return ADSP_EUNSUPPORTED;
	}

	if ((pInitParams->bFrameMetaDataEnable) && (pInitParams->no_apr_enc))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc cannot enable metadata for non-client output buffers");
		return ADSP_EFAILED;
	}

	AudioEncSvc_t *pMe = (AudioEncSvc_t*)qurt_elite_memory_malloc(QURT_ELITE_ROUNDUP_MULTIPLE8(sizeof(AudioEncSvc_t)),
			QURT_ELITE_HEAP_DEFAULT);

	if (!pMe)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for encoder service instance!");
		return ADSP_ENOMEMORY;
	}

	// zero out all the fields.
	memset(pMe, 0, sizeof(*pMe));

	pMe->ulMode = pInitParams->encMode;
	pMe->no_apr_enc = pInitParams->no_apr_enc;
	pMe->is_rt_enc = pInitParams->is_rt_enc;
	pMe->io_fmt_conv = pInitParams->io_fmt_conv;

	if (pInitParams->pCbData)
	{
		// Make a copy of ASM call back structure
		memscpy( &pMe->CbData, sizeof(AudioStreamMgr_CallBackHandleType),
				pInitParams->pCbData, sizeof(AudioStreamMgr_CallBackHandleType));
	}

	pMe->ulDataLogId  = pInitParams->ulDataLogId;

	//wait for enc cfg before we can begin any data proc.
	pMe->bWaitForEncCfg = TRUE;
	pMe->bWaitForInpMediaFmt = TRUE;

	//intialize the memory map client for Encoder service.
	pMe->SharedMemMapTypeNode.unMemMapClient = asmMemoryMapClient;

	pMe->perf_mode = pInitParams->perf_mode;

	pMe->delay_ptr = pInitParams->delay_ptr;
	if (pMe->delay_ptr)
	{
	   *pMe->delay_ptr = 0;
	}

	//cmdQ and gpQ
	result = AudioEncSvc_CreateSvcQueues(pMe, is_out_dataQ_needed);
	if(ADSP_EOK != result)
	{
		goto __bailout;
	}

	enc_init_time_get_params_t enc_params;
	memset(&enc_params,0,sizeof(enc_init_time_get_params_t));

	result = AudioEncSvc_InitPushMode(pMe, pInitParams);

	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	//Create encoder CAPI
	result = AudioEncSvc_CreateCAPIs(pMe, pInitParams, &enc_params);
	if(ADSP_EOK != result)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create encoder CAPI instance!");
		goto __bailout;
	}

	//create dataQ now
	result = AudioEncSvc_CreateSvcDataQQueue(pMe);
	if(ADSP_EOK != result)
	{
		goto __bailout;
	}

	//initialize timestamp values
	AudioEncSvc_ResetTimeStampValues(&pMe->encTSParams);

	threadStackSize = enc_params.max_stack_size;

	threadStackSize += AUD_ENC_PROCESS_STACK_SIZE;

	if(MIN_AUDENC_THREAD_STACK_SIZE > threadStackSize)
	{
		threadStackSize = MIN_AUDENC_THREAD_STACK_SIZE;
	}

	// Low latency encoder sessions will have higher priority than legacy sessions.
	nPriority = AudioEncSvc_GetThreadPriority(pMe);

	// Launch the thread
	if (ADSP_FAILED(result = qurt_elite_thread_launch(&pMe->serviceHandle.threadId,
			THREAD_NAME,
			NULL,
			threadStackSize,
			nPriority,
			AudioEncSvc_WorkLoop,
			(void*)pMe,
			QURT_ELITE_HEAP_DEFAULT)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc failed to launch service thread %d!! \n",result);
		goto __bailout;
	}

	*handle = &(pMe->serviceHandle);

	MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioEncSvc creation success!! \n");
	return ADSP_EOK;

	__bailout:
	AudioEncSvc_Destroy(pMe);
	return result;

}


// destructor
void AudioEncSvc_Destroy (AudioEncSvc_t* pMe)
{
	if (pMe)
	{
		/** release, force_vote doesn't matter.*/
		if(pMe->imc_info_ptr)
		{
			(void)AudioEncSvc_IMCDestroy(pMe);
		}
		(void)AudioEncSvc_ProcessKppsBw(pMe, TRUE, FALSE);
		(void)AudioEncSvc_DeregisterWithAdsppm(pMe);

		//release CAPI memory
		for (uint8_t capi_index = 0; capi_index < ENC_SVC_MAX_CAPI; capi_index++)
		{
			if (NULL == pMe->capiContainer[capi_index]) break;

			//destroy CAPI decoder
			if (pMe->capiContainer[capi_index]->capi_ptr)
			{
				//release CAPI memory: core library is released here
				pMe->capiContainer[capi_index]->capi_ptr->vtbl_ptr->end(pMe->capiContainer[capi_index]->capi_ptr) ;

				pMe->capiContainer[capi_index]->enc_destroy_fn(&(pMe->capiContainer[capi_index]->capi_ptr));
			}
		}

		//destroy any output buffers if allocated
		AudioEncSvc_DestroyOutBufs(pMe);

		// call utility function to destroy data Q
		AudioEncSvc_DestroyInpDataQ(pMe);

		// call utility function to destroy cmd Q
		elite_svc_destroy_cmd_queue(pMe->serviceHandle.cmdQ);

		// call utility function to destroy buf Q
		AudioEncSvc_DestroyOutDataQ(pMe);

		qurt_elite_channel_destroy(&pMe->channel);

		(void) AudioEncSvc_DestroyInpBuf(pMe);

		for (uint8_t capi_index = 0; capi_index < ENC_SVC_MAX_CAPI; capi_index++)
		{
			if (NULL == pMe->capiContainer[capi_index]) break;
			qurt_elite_memory_free(pMe->capiContainer[capi_index]);
			pMe->capiContainer[capi_index] = NULL;
		}

		pMe->mainCapiContainer = NULL;

		AudioEncSvc_DeInitPushMode(pMe);

		qurt_elite_memory_free (pMe);
		pMe = NULL;
	}
}

static void AudioEncSvc_DestroyOutDataQ (AudioEncSvc_t* pMe)
{
	(void)AudioEncSvc_FlushOutQueue(pMe);

	// clean up buffer queue
	if (pMe->serviceHandle.gpQ)
	{
		qurt_elite_queue_destroy(pMe->serviceHandle.gpQ);
	}
}

static void AudioEncSvc_DestroyInpDataQ(AudioEncSvc_t* pMe)
{
	(void)AudioEncSvc_FlushInpQueue(pMe);

	// clean up data queue
	if (pMe->serviceHandle.dataQ)
	{
		// destroy the q.
		qurt_elite_queue_destroy(pMe->serviceHandle.dataQ);
	}
}

/**
 * This function is the main work loop for the service. Commands
 * are handled with the highest priority. Data processing is handled only when command
 * queue is empty. This should be typical of most/all services.
 *
 * The way data is handled in this example is simple. The thread listens for command and
 * data queues simultaneously (commands always prioritized). When a data buffer is received,
 * it is stored, and the thread switches to listening for command queue and output buffer
 * queue. As soon as an output buffer is received, the processing function is called with
 * the data buffer and output buffer, for one-to-one transform. Many services may work in this
 * way, but variations are also possible. For example, decoders probably are ready to run as
 * soon as there is any input data. Only after decoder returns is it known whether an output
 * buffer is needed. So the work loop for a decoder service may look different.
 */

static int AudioEncSvc_WorkLoop(void* pInstance)
{

	AudioEncSvc_t *pMe = (AudioEncSvc_t*)pInstance;       // instance structure
	uint32_t unSigBit;
	ADSPResult result=ADSP_EOK;

	//listen to only command queue until we receive enc cfg block
	pMe->unCurrentBitfield = AUD_ENC_CMD_SIG;

	//if compressed input, no encoder config block is expected. So listen to data queue as well
	if (pMe->io_fmt_conv == ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW)
	{
		pMe->unCurrentBitfield |= AUD_ENC_DATA_SIG;
	}

	(void)AudioEncSvc_RegisterWithAdsppm(pMe);

	/** force vote including both svc and capi votes.*/
	(void)AudioEncSvc_ProcessKppsBw(pMe, FALSE, TRUE);

	// Enter forever loop
	for(;;)
	{
		// block on any one or more of selected queues to get a msg
		pMe->unChannelStatus = qurt_elite_channel_wait(&(pMe->channel), pMe->unCurrentBitfield);

		for (;;)
		{
			// Check for signals
			pMe->unChannelStatus = qurt_elite_channel_poll(&(pMe->channel), pMe->unCurrentBitfield);

			if( pMe->unChannelStatus == 0 ) break;

			unSigBit = s32_cl0_s32( pMe->unChannelStatus);

			result = pEncQHandler[unSigBit](pMe);

			if(result == ADSP_ETERMINATED) return ADSP_EOK;
		}
	}

	return result;
}


static ADSPResult AudioEncSvc_ProcessCmdQueue(AudioEncSvc_t* pMe)
{
	ADSPResult result=ADSP_EOK;

	// Take next msg off the q
	result = qurt_elite_queue_pop_front(pMe->serviceHandle.cmdQ, (uint64_t*)&(pMe->CmdMsg) );

	// Process the msg
	if (ADSP_EOK == result)
	{
#ifdef DBG_AUDIO_ENCODE
		MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Message ID 0x%8x!!", pMe->CmdMsg.unOpCode);
#endif

		//find the number of elements in pASMToEncCmdHandler
		uint32_t ulNumHandledCmds = sizeof(pASMToEncCmdHandler)/sizeof(pASMToEncCmdHandler[0]);

		/* sanity check */
		if ( pMe->CmdMsg.unOpCode >= ulNumHandledCmds )
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported message ID 0x%8lx!!\n", pMe->CmdMsg.unOpCode);
			return AudioEncSvc_ReturnUnsupported(pMe);
		}

		// table lookup to call handling function, with FALSE to indicate processing of msg
		return pASMToEncCmdHandler[pMe->CmdMsg.unOpCode](pMe);
	}
	else
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to dequeue command message!");
	}

	return result;
}

static ADSPResult AudioEncSvc_ProcessOutDataQueue_Client(AudioEncSvc_t *pMe)
{
	ADSPResult result=ADSP_EOK;
	elite_apr_packet_t *pAprPacket;
	uint32_t uAprOpCode;

	// Take next msg off the q
	if ( ADSP_FAILED(result=qurt_elite_queue_pop_front(pMe->serviceHandle.gpQ, (uint64_t*)&(pMe->OutDataMsg) )) )
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc failed receive output buffers %d", result );
		pMe->OutDataMsg.pPayload = NULL;
		return result;
	}

	if (ELITE_APR_PACKET != (pMe->OutDataMsg.unOpCode))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Received non-APR message on \
                                            output data q w/ opcode = 0x%lx" ,
											pMe->OutDataMsg.unOpCode);
		result = elite_msg_finish_msg(&pMe->OutDataMsg, ADSP_EUNSUPPORTED);
		pMe->OutDataMsg.pPayload = NULL;
		return result;
	}

	pAprPacket = (elite_apr_packet_t*) (pMe->OutDataMsg.pPayload);
	if(!pAprPacket)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "APR packet pointer is NULL!");
		//nothing else can be done, return error
		return ADSP_EBADPARAM;
	}

	uAprOpCode = elite_apr_if_get_opcode( pAprPacket ) ;

	switch (uAprOpCode) {

	case ASM_DATA_CMD_READ_V2:
	case ASM_DATA_CMD_READ_COMPRESSED_V2:
	{

		pMe->buf_recv_cnt++;

		asm_data_cmd_read_v2_t *pEncOut;
		pEncOut = (asm_data_cmd_read_v2_t *)elite_apr_if_get_payload_ptr(pAprPacket);

		if (!pEncOut) {
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL payload from APR packet!");
			return AudioEncSvc_ReleaseOutputBuffer(pMe, 0, ADSP_EBADPARAM);
		}

		if((ASM_DATA_CMD_READ_COMPRESSED_V2 == uAprOpCode) &&
				(pMe->io_fmt_conv != ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ASM_DATA_CMD_READ_COMPRESSED_V2 received in non-compressed read mode");
			return AudioEncSvc_ReleaseOutputBuffer(pMe, 0, ADSP_EBADPARAM);
		}

		//if output buffer is not 32 byte aligned return it
		if(pEncOut->buf_addr_lsw & MASK_32BYTE_ALIGNMENT)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Output buffer not 32 byte aligned, returning it!");
			return AudioEncSvc_ReleaseOutputBuffer(pMe, 0, ADSP_EBADPARAM);
		}

		pMe->SharedMemMapTypeNode.unMemMapHandle = pEncOut->mem_map_handle;
		if (ADSP_FAILED(result = elite_mem_map_get_shm_attrib_ref_counted(pEncOut->buf_addr_lsw, pEncOut->buf_addr_msw,
				pEncOut->buf_size,
				&(pMe->SharedMemMapTypeNode) )))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"AudioEncSvc:Phy to Virt Failed(paddr,vaddr)-->(%lx%lx,%lx)\n",
					(pMe->SharedMemMapTypeNode).unPhysAddrMsw,(pMe->SharedMemMapTypeNode).unPhysAddrLsw,(pMe->SharedMemMapTypeNode).unVirtAddr);
			return AudioEncSvc_ReleaseOutputBuffer(pMe, 0, ADSP_EBADPARAM);
		}
		pMe->SharedMemMapTypeNodeBackUp = pMe->SharedMemMapTypeNode;

		/*** Reset offset into output buffer */
		enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);
		EncSvc_OutBufParams_t *pOutBufParams = &last_capi->out_buf_params;
		if(pOutBufParams->bFrameMetaDataEnable)
		{
			//Set the non zero offset for encoded data
			if(ASM_DATA_CMD_READ_V2 == uAprOpCode)
			{
				pOutBufParams->out_buf.actual_data_len = (pOutBufParams->usTargetFramesPerBuf)*(sizeof(asm_data_read_buf_metadata_v2_t));
			}
			else //(ASM_DATA_CMD_READ_COMPRESSED_V2 == uAprOpCode)
			{
				pOutBufParams->out_buf.actual_data_len = (pOutBufParams->usTargetFramesPerBuf)*(sizeof(asm_data_read_buf_metadata_compressed_v2_t));
			}
		}
		else
		{
			pOutBufParams->out_buf.actual_data_len = 0;
		}

		pOutBufParams->out_buf.data_ptr    = reinterpret_cast <int8_t * > (pMe->SharedMemMapTypeNode.unVirtAddr);
		pOutBufParams->out_buf.max_data_len = pEncOut->buf_size;

		if(AudioEncSvc_IsPcmFmt(last_capi->enc_cfg_id))
		{
			uint32_t num_channels = pMe->out_med_fmt_per_enc_cfg.num_channels;
			//We can only copy a multiple of number of channels into an output buffer
			//empty bytes must be floored to a multiple of 4 times the number of channels
			//(24-bit output (packed in 32-bit words) case)
			uint32_t usable_length_pcm = (pOutBufParams->out_buf.max_data_len - pOutBufParams->out_buf.actual_data_len);
			uint32_t rem = usable_length_pcm % (pMe->out_med_fmt_per_enc_cfg.bytes_per_sample * num_channels);
			pOutBufParams->out_buf.max_data_len = usable_length_pcm - rem ;

			if(pOutBufParams->bFrameMetaDataEnable)
			{
				pOutBufParams->out_buf.max_data_len += pOutBufParams->out_buf.actual_data_len;
			}

		}

		//check if write offset is not less than buffer size
		if(( (uint32_t)pOutBufParams->out_buf.actual_data_len >= pEncOut->buf_size) )
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Output buffer size is too  less to write encoded data: Meta data size = %d, buffer size = %lu",
					pOutBufParams->out_buf.actual_data_len,
					pEncOut->buf_size);
			return AudioEncSvc_ReleaseOutputBuffer(pMe, 0, ADSP_EBADPARAM);
		}

		pOutBufParams->usNumFramesInOutBuf = 0;
		//Call process function
		result = AudioEncSvc_Proc(pMe);
		break;
	} /* case ASM_DATA_CMD_READ_V2 */
	default:
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unexpected opCode for data write 0x%lx", uAprOpCode );
		//don't know what this command is, so cannot provide payload for acknowledgement
		result = AudioStreamMgr_GenerateAck(pAprPacket, ADSP_EFAILED, NULL, 0, 0);
		pMe->OutDataMsg.pPayload = NULL;
		break;
	}

	}// end of switch(pMe->gpQMsg.unOpCode)
	return result;
}

ADSPResult AudioEncSvc_GetExtOutputBuf(AudioEncSvc_t *pMe)
{
	ADSPResult result=ADSP_EOK;

	//note that this function is called only after waiting on gpQ from work loop.
	//if polling fails call will return
	while( (qurt_elite_channel_poll(qurt_elite_queue_get_channel(pMe->serviceHandle.gpQ),
			qurt_elite_queue_get_channel_bit(pMe->serviceHandle.gpQ))) )
	{
		// Take next buffer off the q
		if(ADSP_FAILED(result = qurt_elite_queue_pop_front(pMe->serviceHandle.gpQ, (uint64_t*)&(pMe->outDataBufferNode))))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc: Failure on Out Buf Pop ");
			pMe->outDataBufferNode.pBuffer = NULL;
			return result;
		}

		elite_msg_data_buffer_t *pOutBuf = (elite_msg_data_buffer_t*) (pMe->outDataBufferNode.pBuffer);

		AudioEncSvc_CheckReallocateExternalBuffer(pMe);
		if (NULL == pMe->outDataBufferNode.pBuffer)
		{
			continue; //continue to poll & get more output buf if recreate happens
		}

		//once we get pMe->outDataBufferNode.pBuffer
		enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);
		EncSvc_OutBufParams_t *pOutBufParams = &last_capi->out_buf_params;

		pOutBufParams->out_buf.max_data_len = pOutBuf->nMaxSize;
		pOutBufParams->out_buf.data_ptr = (int8_t *) (&(pOutBuf->nDataBuf));
		pOutBufParams->out_buf.actual_data_len = 0;
		pOutBufParams->usNumFramesInOutBuf = 0;

		if (pMe->outDataBufferNode.pBuffer)
		{
			break;
		}
	}

	//should never return pMe->outDataBufferNode.pBuffer == null, because
	// this func is called from wait-loop after waiting for output queue. if checkReallocateExtBuf reallocates then
	// we need to pop again. doing this may pop buf which again may need realloc. but ultimately we will reach the resized buf.

	return result;
}

static ADSPResult AudioEncSvc_ProcessOutDataQueue_PeerService(AudioEncSvc_t *pMe)
{
	ADSPResult result=ADSP_EOK;

	AudioEncSvc_GetExtOutputBuf(pMe);
	(pMe->buf_recv_cnt)++;

	//Call process function
	result = AudioEncSvc_Proc(pMe);

	// Take next msg off the q
	return result;
}

static ADSPResult AudioEncSvc_ProcessOutDataQueue(AudioEncSvc_t *pMe)
{
	ADSPResult result = ADSP_EFAILED;
	if (AudioEncSvc_IsOutBufFromClient(pMe))
	{
		//if encoder service does not malloc any output buffers, then
		//the buffers must come from the client
		result = AudioEncSvc_ProcessOutDataQueue_Client(pMe);
	}
	else
	{
		result = AudioEncSvc_ProcessOutDataQueue_PeerService(pMe);
	}// end of switch(pMe->gpQMsg.unOpCode)

	return result;
}

static ADSPResult AudioEncSvc_ProcessInpDataQueue(AudioEncSvc_t *pMe)
{
	ADSPResult result;

	if (pMe->InpDataMsg.pPayload)
	{
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudEncSvc overwriting already popped input buffer");
	}

	// Take next msg off the q
	if (ADSP_FAILED(result= qurt_elite_queue_pop_front(pMe->serviceHandle.dataQ, (uint64_t*)&pMe->InpDataMsg )))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudEncSvc failed to dequeue input data msg!");
		pMe->InpDataMsg.pPayload = NULL;
		return result;
	}

	if(!pMe->InpDataMsg.pPayload)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Input data msg payload is NULL!");
		return ADSP_EBADPARAM;
	}

	//process messages
	switch ( pMe->InpDataMsg.unOpCode )
	{
	case ELITE_DATA_MEDIA_TYPE:
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Received input media format message from upstream svc");
		return AudioEncSvc_ProcessMediaTypeMsg(pMe);
	}
	case ELITE_DATA_EOS:
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Received APR EOS from upstream!");
		result = AudioEncSvc_ProcessEos(pMe);
		break;
	}
	case ELITE_DATA_MARK_BUFFER:
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Received Mark Buffer from upstream!");
		//Raise the Mark Buffer Consumption Event
		elite_msg_data_mark_buffer_t *pMarkBufferPayload = (elite_msg_data_mark_buffer_t *)pMe->InpDataMsg.pPayload;
		asm_data_event_mark_buffer_v2_t eventPayload;
		eventPayload.token_lsw = pMarkBufferPayload->token_lsw;
		eventPayload.token_msw = pMarkBufferPayload->token_msw;
		eventPayload.result = ADSP_EOK;
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Encoder: Mark buffer consumed, raising event");
		result = AudioStreamMgr_GenerateClientEventFromCb(&pMe->CbData,
				ASM_DATA_EVENT_MARK_BUFFER_V2,
				0, &eventPayload, sizeof(eventPayload));
		elite_msg_finish_msg(&pMe->InpDataMsg, ADSP_EOK);
		pMe->InpDataMsg.pPayload = NULL;
		break;
	}
	case ELITE_DATA_BUFFER:
	{
		//check if we are still waiting for input media format
		if(pMe->bWaitForInpMediaFmt)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"Waiting for input media format message but received data buffer, returning it without processing");

			//cant process data buffers until i/p media format is received
			ADSPResult res = elite_msg_finish_msg( &pMe->InpDataMsg, ADSP_EOK );
			pMe->InpDataMsg.pPayload = NULL;
			return res;
		}

		/*
		 * If LLNP session is in pause state then drop input data buffer.
		 * This is to stop data-queue overflow, if AFE started and stopped repeatedly and encoder is paused.
		 */
		if(pMe->isPaused)
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
					"LLNP Encoder is in pause state, Dropping input data buffer.");

			ADSPResult res = elite_msg_finish_msg( &pMe->InpDataMsg, ADSP_EOK );
			pMe->InpDataMsg.pPayload = NULL;
			return res;
		}

		elite_msg_data_buffer_t* pInpBuf = (elite_msg_data_buffer_t*) pMe->InpDataMsg.pPayload;
		if(!pInpBuf->nActualSize)
		{
			ADSPResult res = elite_msg_finish_msg( &pMe->InpDataMsg, ADSP_EOK );
			pMe->InpDataMsg.pPayload = NULL;
			return res;
		}


		pInpBuf->nOffset = 0; //Assumption: valid data always starts from zero.

		enc_CAPI_container_t *first_capi = AudioEncSvc_GetFirstCapi(pMe);
		enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

		AudioEncSvc_FillTimeStampState(pMe, first_capi);

		/* If last CAPI is holding onto some data and TS discontinuity occurs, release buffers (for PCM)*/
		EncSvc_OutBufParams_t *pOutBufParams = &last_capi->out_buf_params;
		uint32_t offset = 0;

		if(pOutBufParams->bFrameMetaDataEnable)
		{
			offset = (pOutBufParams->usTargetFramesPerBuf)*sizeof(asm_data_read_buf_metadata_v2_t);
		}

		//For PCM if there is a mismatch in timestamp then release output buffer right away
		//(only if some valid data is present in the output buffer)
		if((pMe->encTSParams.timestamp_mismatch)
				&& AudioEncSvc_IsPcmFmt(last_capi->enc_cfg_id))
		{
			if ((((uint32_t)pOutBufParams->out_buf.actual_data_len - offset) > 0)
					&& (NULL != pMe->OutDataMsg.pPayload))
			{
				AudioEncSvc_FillMetadataAndReleaseOutputBuf(pMe);
			}
			//sync to the latest buffer's TS
			pMe->encTSParams.ullTimeStamp = pMe->encTSParams.ullLastBufferTimeStamp;
			pMe->encTSParams.isTimeStampValid = pMe->encTSParams.isLastBufferTimeStampValid;
		}

		int8_t * buf_addr = (int8_t *)&pInpBuf->nDataBuf;
		uint32_t buf_size = pInpBuf->nActualSize;

		/* Logging the input PCM data or bitstream (in compressed modes) to Audio Encoder Module */
		result = AudioEncSvc_LogInputData(buf_addr, buf_size, pMe);

		//Call Process function
		result = AudioEncSvc_Proc(pMe);
		break;
	}
	case ELITE_APR_PACKET:
	{
		//extract the APR op code
		elite_apr_packet_t* pAprPacket = (elite_apr_packet_t*) (pMe->InpDataMsg.pPayload);

		//currently not expecting any APR packet...can be changed later
		//free the APR packet
		result = AudioStreamMgr_GenerateAck(pAprPacket, ADSP_EFAILED, NULL, 0, 0);
		pMe->InpDataMsg.pPayload = NULL;
		break;
	}
	default:
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudEncSvc Rx'd unsupported message!! ");
		if (ADSP_FAILED( result= elite_msg_finish_msg( &pMe->InpDataMsg, ADSP_EUNEXPECTED ) ) )
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to release input data message %d", result);
		}
		pMe->InpDataMsg.pPayload = NULL;
	}
	}
	return (result);
}

/*Wrapper to call appropriate processing function
  Process function is different for PCM*/
static ADSPResult AudioEncSvc_Proc(AudioEncSvc_t* pMe)
{
	ADSPResult result=ADSP_EOK;
	/** PCM and push mode have only one CAPI */
	if (ENC_SVC_PUSH_MODE == pMe->ulMode)
	{
		result = AudioEncSvc_ProcessPCMPushMode(pMe);
	}
	else
	{
		if (AudioEncSvc_IsPcmFmt(pMe->mainCapiContainer->enc_cfg_id))
		{
			result = AudioEncSvc_ProcessPCM(pMe);
		}

		else if (AudioEncSvc_IsOutBufFromClient(pMe))
		{
			result = AudioEncSvc_Process(pMe);
		}
		else
		{
			result = AudioEncSvc_ProcessCompressedLoopback(pMe);
		}
	}

	//vote based on events.
	AudioEncSvc_ProcessKppsBw(pMe, FALSE, FALSE);

	return result;
}


/*===========================================================================
FUNCTION       AudioEncSvc_Process()

DESCRIPTION
   1 Called either from dataQ handler/ BufQ handler
   2 If the data available in internal buffer is not enough to encode a frame copies
     data over from the input buffer to internal buffer.If still not enough data then
     listen for a new input buffer and exit.
   3 If input data is sufficient, it checks if we have an output buffer available to
     encode, if not exits after setting the buffer signal
   4 If we have enough input data as well as output buffer, goes for encoding
   5 After returning from encode, returns the output buffer if it has frames(1<x<=N),
     where N is > 1 incase of multiple frame support
   - steps 1 to 5 continues until we have enough input data and output buffer

PARAMETERS
Input:  pointer to AudioEncSvc_t

RETURN VALUE
result : ADSP_EOK/ADSP_EFAILED
===========================================================================*/
static ADSPResult AudioEncSvc_Process(AudioEncSvc_t* pMe)
{
	ADSPResult result = ADSP_EOK;
	int enc_in_bits_per_sample;
	//Set command signal by default
	pMe->unCurrentBitfield = AUD_ENC_CMD_SIG;

	bool_t compr_need_more = TRUE;

	enc_CAPI_container_t *first_capi = AudioEncSvc_GetFirstCapi(pMe);
	enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

	do
	{
		if (pMe->endOfStream != TRUE) //At EoS there's no more input to be copied.
		{
			//copy from input to internal buffer only if enough data not present
			if(is_more_data_required(pMe, first_capi, compr_need_more))
			{
				AudioEncSvc_CopyInpDatatoInternalBuf(pMe, first_capi, &compr_need_more);
				//if we still don't have enough data for encode then wait for a
				//new input buffer
				if(is_more_data_required(pMe, first_capi, compr_need_more) )
				{
					pMe->unCurrentBitfield |= AUD_ENC_DATA_SIG;
					break;
				}
			}
		}

		if(NULL == pMe->OutDataMsg.pPayload)
		{
			// listen to output peer/client buffers and exit
			pMe->unCurrentBitfield |= AUD_ENC_BUF_SIG;
			break;
		}

		int nReturnOutBuf; //will be 1 if output buffer has to be released, else 0
		enc_in_bits_per_sample = pMe->in_med_fmt.bits_per_sample;
		uint32_t enc_in_bytes_per_sample = (enc_in_bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
		uint32_t samples_before_processing = first_capi->in_buf_params.in_buf.actual_data_len / enc_in_bytes_per_sample;

		//Go for Encode as we have all required input and output
		result = AudioEncSvc_EncodeUsingAllCapis(pMe, &nReturnOutBuf,&compr_need_more);

		uint32_t consumed_samples = samples_before_processing - (first_capi->in_buf_params.in_buf.actual_data_len / enc_in_bytes_per_sample);
		AudioEncSvc_UpdateFrameTimestamp(pMe, first_capi, consumed_samples);

		//if encoding is successful and output buffer has atleast one frame
		if(1 == nReturnOutBuf)
		{
			//Flush the shared mem map region
			elite_mem_flush_cache( &(pMe->SharedMemMapTypeNode) );

			//release output buffer
			result = AudioEncSvc_ReleaseOutputBuffer(pMe, (uint32_t)last_capi->out_buf_params.out_buf.actual_data_len, result);

			//if the last input buffer TS was valid, then set next output buffer TS as valid
			pMe->encTSParams.isTimeStampValid = pMe->encTSParams.isLastBufferTimeStampValid;
		}

		//If this was the last input buffer and we have completely processed then propagate EOS downstream.
		//If an error occurs while processing this buffer then it is unrecoverable and so send EOS anyway.
		if((pMe->endOfStream == TRUE) &&  (last_capi->out_buf_params.out_buf.actual_data_len == 0))
		{
			AudioEncSvc_ProcessEosCmd(pMe);
			break;
		}
		//continue the loop as long as we have sufficient input data and output buffer
	}while(1);

	return (result);
}

/*===========================================================================
FUNCTION       AudioEncSvc_ProcessLoopback()

DESCRIPTION
   1 Called either from dataQ handler/ BufQ handler
   2 If the data available in internal buffer is not enough to encode a frame copies
     data over from the input buffer to internal buffer.If still not enough data then
     listen for a new input buffer and exit.
   3 If input data is sufficient, it checks if we have an output buffer available to
     encode, if not exits after setting the buffer signal
   4 If we have enough input data as well as output buffer, goes for encoding
   5 After returning from encode, returns the output buffer if it has frames(1<x<=N),
     where N is > 1 incase of multiple frame support
   - steps 1 to 5 continues until we have enough input data and output buffer

PARAMETERS
Input:  pointer to AudioEncSvc_t

RETURN VALUE
result : ADSP_EOK/ADSP_EFAILED
===========================================================================*/
static ADSPResult AudioEncSvc_ProcessCompressedLoopback(AudioEncSvc_t* pMe)
{
	ADSPResult result = ADSP_EFAILED;
	//Set command signal by default
	pMe->unCurrentBitfield = AUD_ENC_CMD_SIG;

	bool_t compr_need_more = TRUE;

	if (AudioEncSvc_IsOutBufFromClient(pMe))
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudEncSvc output buffers not allocated for encoding loopback");
		return ADSP_EFAILED;
	}

	enc_CAPI_container_t *first_capi = AudioEncSvc_GetFirstCapi(pMe);
	enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

	do
	{
		if (pMe->endOfStream != TRUE) //At EoS there's no more input to be copied.
		{
			//copy from input to internal buffer only if enough data not present
			if(is_more_data_required(pMe, first_capi, compr_need_more))
			{
				AudioEncSvc_CopyInpDatatoInternalBuf(pMe, first_capi, &compr_need_more);
				//if we still don't have enough data for encode then wait for a
				//new input buffer
				if(is_more_data_required(pMe, first_capi, compr_need_more))
				{
				   if (pMe->InpDataMsg.pPayload)
				   {
				      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudEncSvc Waiting for more input when holding onto a buffer");
				   }
                   
					pMe->unCurrentBitfield |= AUD_ENC_DATA_SIG;
					result = ADSP_EOK;
					break;
				}
			}
		}

		//switch listen to output buffers if output buffer is not ready
		if(NULL == last_capi->out_buf_params.out_buf.data_ptr)
		{
			pMe->unCurrentBitfield |= AUD_ENC_BUF_SIG;
			result = ADSP_EOK;
			break;
		}

		int nReturnOutBuf; //will be 1 if output buffer has to be released, else 0
		int enc_in_bits_per_sample = pMe->in_med_fmt.bits_per_sample;
		uint32_t enc_in_bytes_per_sample = (enc_in_bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
		uint32_t samples_before_processing = first_capi->in_buf_params.in_buf.actual_data_len / enc_in_bytes_per_sample;

		//calling imc function to receive data
		AudioEncSvc_HandleIMCBeforeProcess(pMe);

		//Go for Encode as we have all required input and output
		result = AudioEncSvc_EncodeUsingAllCapis(pMe,&nReturnOutBuf, &compr_need_more);
		if (ADSP_EOK != result)
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "encoding loopback result %d, nReturnOutBuf %d, 0x%p",
					result, nReturnOutBuf, pMe->outDataBufferNode.pBuffer);
		}

		uint32_t consumed_samples = samples_before_processing - (first_capi->in_buf_params.in_buf.actual_data_len / enc_in_bytes_per_sample);
		AudioEncSvc_UpdateFrameTimestamp(pMe, first_capi, consumed_samples);

		if ( last_capi->media_fmt_event )
		{
			last_capi->media_fmt_event = FALSE;
			AudioEncSvc_NotifyPeerSvcWithMediaFmtUpdate(pMe, last_capi->out_med_fmt.std.sampling_rate, last_capi->out_med_fmt.std.num_channels,
					last_capi->out_med_fmt.std.channel_type, last_capi->out_med_fmt.std.bits_per_sample);

		}

		//if encoding is successful and output buffer has atleast one frame
		if(1 == nReturnOutBuf)
		{
		   if (pMe->buf_done_cnt == 0) //first output buf
		   {
		      //Just before sending first buf, send zeros. the zeros help in
		      //a) cases like 2.5 ms frames, where AFE needs 2 bufs back to back.
		      //b) reserving processing time for encoder. i.e if processing gets over very fast for the first time and takes longer in the subsequent times, then this buf gives cushion.
		      if (pMe->is_rt_enc)
		      {
		         AudioEncSvc_PushInitialZeros(pMe);
		      }
		   }

			result = AudioEncSvc_SendOutBufToPeerSvc(pMe);

			//if the last input buffer TS was valid, then set next output buffer TS as valid
			pMe->encTSParams.isTimeStampValid = pMe->encTSParams.isLastBufferTimeStampValid;
		}
		//If this was the last input buffer and we have completely processed then propagate EOS downstream.
		//If an error occurs while processing this buffer then it is unrecoverable and so send EOS anyway.
		if((pMe->endOfStream == TRUE) &&  (last_capi->out_buf_params.out_buf.actual_data_len == 0))
		{
			AudioEncSvc_ProcessEosCmd(pMe);
			break;
		}
		//continue the loop as long as we have sufficient input data and output buffer
	}while(1);

	return (result);
}

/*===========================================================================
FUNCTION       AudioEncSvc_ProcessPCM()

DESCRIPTION
   1 Called either from dataQ handler/ BufQ handler for PCM format
   2 If input and output buffer are availabe then copies whatever possible
     from input buffer to output buffer. If input is deinterleaved, interleave
     when copying.
   3 If input buffer is completely exhausted release it
   4 If output buffer is completely filled release it after filling metadata
     and start listening for output buffer

PARAMETERS
Input:  pointer to AudioEncSvc_t

RETURN VALUE
result : ADSP_EOK/ADSP_EFAILED
===========================================================================*/
static ADSPResult AudioEncSvc_ProcessPCM(AudioEncSvc_t* pMe)
{

	ADSPResult result = ADSP_EOK;
	//PCM has only one CAPI
	EncSvc_OutBufParams_t *out_buf_params_ptr = &pMe->mainCapiContainer->out_buf_params;
	elite_msg_data_buffer_t* inp_buf_ptr = (elite_msg_data_buffer_t*) pMe->InpDataMsg.pPayload;
	uint32_t samples_to_copy, consumed_samples;
	int num_channels, sampling_rate, out_bits_per_sample;
	uint32_t inp_buffer_rd_index;

	uint32_t enc_svc_input_bytes_per_sample, out_bytes_per_sample;
	uint32_t out_buf_num_empty_samples;

	/* ASM open write sets  MXAT out bps == PP out bps == AudEnc in bps */
	enc_svc_input_bytes_per_sample = (pMe->in_med_fmt.bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

	//Set command signal by defaultg579
	pMe->unCurrentBitfield = AUD_ENC_CMD_SIG;

	num_channels = pMe->out_med_fmt_per_enc_cfg.num_channels;
	sampling_rate = pMe->out_med_fmt_per_enc_cfg.sample_rate;
	out_bits_per_sample = pMe->out_med_fmt_per_enc_cfg.bits_per_sample;

	out_bytes_per_sample = pMe->out_med_fmt_per_enc_cfg.bytes_per_sample;

	if (pMe->endOfStream)
	{
		return AudioEncSvc_ProcessEosCmd(pMe);
	}

	//check of any data left in input buffer
	if (!inp_buf_ptr || (inp_buf_ptr->nActualSize == 0))
	{
		pMe->unCurrentBitfield |= AUD_ENC_DATA_SIG;
		return result;
	}

	//check if any output buffers available
	if(pMe->OutDataMsg.pPayload == NULL)
	{
		pMe->unCurrentBitfield |= AUD_ENC_BUF_SIG;
		return result;
	}

	//copy from input to output buffer
	inp_buffer_rd_index = inp_buf_ptr->nOffset/enc_svc_input_bytes_per_sample;

	//if deinterleaved then set read index accordingly
	if(!pMe->in_med_fmt.is_interleaved)
	{
		inp_buffer_rd_index = inp_buffer_rd_index/num_channels;
	}

	out_buf_num_empty_samples = (out_buf_params_ptr->out_buf.max_data_len - out_buf_params_ptr->out_buf.actual_data_len)/out_bytes_per_sample;

	samples_to_copy = inp_buf_ptr->nActualSize/enc_svc_input_bytes_per_sample;
	samples_to_copy = (samples_to_copy > out_buf_num_empty_samples) ? out_buf_num_empty_samples : samples_to_copy;

	int8_t *src_ptr =  (int8_t *)&inp_buf_ptr->nDataBuf + (inp_buffer_rd_index * enc_svc_input_bytes_per_sample);

	int8_t *dest_ptr = (int8_t *)(out_buf_params_ptr->out_buf.data_ptr) + out_buf_params_ptr->out_buf.actual_data_len;

	if(pMe->out_med_fmt_per_enc_cfg.endianness == ENC_BIG_ENDIAN)
	{
		if(ADSP_FAILED(result = endianness_convertor_with_intlv_in((int8_t *)src_ptr, samples_to_copy, enc_svc_input_bytes_per_sample*8)))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc:Failed to convert endianness of the input samples.");
			return result;
		}
	}

	//copy from input to output buffer
	//output is Q15 or Q31.
	AudioEncSvc_CopyWithInterleaving(pMe, num_channels, samples_to_copy, src_ptr,
			dest_ptr, out_bytes_per_sample, enc_svc_input_bytes_per_sample, (out_bytes_per_sample*8-1), out_bits_per_sample);

	inp_buf_ptr->nActualSize -= (samples_to_copy*enc_svc_input_bytes_per_sample);
	inp_buf_ptr->nOffset += (samples_to_copy*enc_svc_input_bytes_per_sample);
	out_buf_params_ptr->out_buf.actual_data_len += (samples_to_copy * out_bytes_per_sample);

	//number of samples consumed from current input buffer
	consumed_samples = inp_buf_ptr->nOffset/enc_svc_input_bytes_per_sample;

	//if input buffer is exhausted release it
	if(inp_buf_ptr->nActualSize == 0)
	{
		if (ADSP_FAILED(result = AudioEncSvc_ReleaseInputBuf(pMe, ADSP_EOK ) ) )
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudEncSvc fail to release input data message %d", result);
		}
		else
		{
			pMe->unCurrentBitfield |= AUD_ENC_DATA_SIG;
		}
	}

	//In steady state we wait till output buffer is filled before releasing
	if(out_buf_params_ptr->out_buf.actual_data_len == out_buf_params_ptr->out_buf.max_data_len)
	{
		AudioEncSvc_FillMetadataAndReleaseOutputBuf(pMe);
		pMe->unCurrentBitfield |= AUD_ENC_BUF_SIG;

		AudioEncSvc_UpdateTimeStampState(pMe, consumed_samples, num_channels, sampling_rate);
	}

	return ADSP_EOK;
}


static int AudioEncSvc_DestroyYourself (AudioEncSvc_t* pMe)
{
	//take a copy of cmdmsg because after destroy this becomes invalid
	elite_msg_any_t cmdMsg = pMe->CmdMsg;

	AudioEncSvc_Destroy(pMe);

	MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioEncSvc destroy success!\n");

	//ack the msg
	elite_msg_finish_msg( &cmdMsg, ADSP_EOK);

	// send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
	return ADSP_ETERMINATED;
}

ADSPResult AudioEncSvc_ReturnUnsupported (AudioEncSvc_t* pMe)
{
	//unsupported command..finish msg with error code
	MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported command with opcode = 0x%lx", pMe->CmdMsg.unOpCode);
	return elite_msg_finish_msg(&(pMe->CmdMsg), ADSP_EUNSUPPORTED);
}

static ADSPResult AudioEncSvc_Flush(AudioEncSvc_t* pMe)
{
	ADSPResult result = ADSP_EFAILED;

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Processing flush command!");

	(void)AudioEncSvc_FlushOutQueue(pMe);
	(void)AudioEncSvc_FlushInpQueue(pMe);

	//reset the encoder
	for (uint8_t i=0; i<ENC_SVC_MAX_CAPI; i++)
	{
		if (!pMe->capiContainer[i]) break;

		//pcm encoder doesnt need to do anything
		if(AudioEncSvc_IsPcmFmt(pMe->capiContainer[i]->enc_cfg_id)) break;

		(void)AudioEncSvc_InitInpBufParams(& pMe->capiContainer[i]->in_buf_params);
		(void)AudioEncSvc_InitOutBufParams(& pMe->capiContainer[i]->out_buf_params);
		aud_enc_svc_algorithmic_reset(pMe->capiContainer[i]->capi_ptr);
	}

	//initialize timestamp values
	AudioEncSvc_ResetTimeStampValues(&pMe->encTSParams);

	AudioEncSvc_ResetPushMode(pMe);

	pMe->ulDataLogId++;

	/* Generate ACK to Flush cmd */
	result = elite_msg_finish_msg(&(pMe->CmdMsg), ADSP_EOK);

	return result;
}

static ADSPResult AudioEncSvc_Run(AudioEncSvc_t* pMe)
{
	ADSPResult result = ADSP_EOK;

	//force vote after suspend.
	AudioEncSvc_ProcessKppsBw(pMe, FALSE, TRUE);

	// Run only supports Push mode or LLNP Tx.
	// Reject requests to run for all other use cases.
	if ((ENC_SVC_PUSH_MODE != pMe->ulMode) && (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION != pMe->perf_mode))
	{
		return elite_msg_finish_msg(&(pMe->CmdMsg), ADSP_EOK);
	}

	if (ENC_SVC_PUSH_MODE == pMe->ulMode)
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc :[RUN] AVTimer handle resume ");
		result = AudioEncSvc_ResumePushMode(pMe);
	}

	// If we have an input buffer, set the bitfield to wait for a cmd or output buffer.
	// Otherwise, wait for a cmd or an input buffer
	if (pMe->InpDataMsg.pPayload)
	{
		// listen to commands and output dataQ
		pMe->unCurrentBitfield = AUD_ENC_CMD_SIG | AUD_ENC_BUF_SIG;
	}
	else
	{
		// Push mode operation will always listen to the input data and command signals
		pMe->unCurrentBitfield = AUD_ENC_CMD_SIG | AUD_ENC_DATA_SIG;
	}

	pMe->isPaused = FALSE;

	/* Generate ACK to Run cmd */
	return elite_msg_finish_msg(&(pMe->CmdMsg), result);

}

/**
 * applicable only to push mode and LLNP performance modes.
 * in LLNP there's no PP to pause, only encoder has to pause.
 */
static ADSPResult AudioEncSvc_Pause(AudioEncSvc_t* pMe)
{
	// Pause only supports Push mode or LLNP Tx.
	// Reject requests to pause for all other use cases.
	if ((ENC_SVC_PUSH_MODE != pMe->ulMode) && (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION != pMe->perf_mode))
	{
		return AudioEncSvc_ReturnUnsupported(pMe);
	}

	ADSPResult result = ADSP_EFAILED;

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Processing pause command!");

	//listen to only commands (don't process data)
	pMe->unCurrentBitfield = AUD_ENC_CMD_SIG;
	pMe->isPaused = TRUE;

	//Listen to data queue as well, for llnp case.
	if(ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pMe->perf_mode)
	{
		pMe->unCurrentBitfield |= AUD_ENC_DATA_SIG;

		//If any data is present then drop it.
		AudioEncSvc_ReleaseInputBuf(pMe, ADSP_EOK );
	}

	// LLNP Tx (non-push mode) should raise an EOS event on pause
	if ((ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pMe->perf_mode) &&
			(ENC_SVC_PUSH_MODE != pMe->ulMode))
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "EncSvc raising LLNP TX EOS event!");
		AudioStreamMgr_GenerateClientEventFromCb(&pMe->CbData,
				ASM_DATA_EVENT_EOS,
				pMe->CbData.unAsynClientToken,
				NULL,
				0);
	}

	/* Generate ACK to Pause cmd */
	result = elite_msg_finish_msg(&(pMe->CmdMsg), ADSP_EOK);

	return result;
}

static ADSPResult AudioEncSvc_Suspend(AudioEncSvc_t* pMe)
{
	ADSPResult result;
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc :[SUSPEND] Executing Suspend Command, checking AVTimer");

	/** release BW */
	AudioEncSvc_ProcessKppsBw(pMe, TRUE, FALSE);

	if (ENC_SVC_PUSH_MODE != pMe->ulMode)
	{
		return elite_msg_finish_msg(&(pMe->CmdMsg), ADSP_EOK);
	}
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioEncSvc :[SUSPEND] AVTimer handle release ");
	result = AudioEncSvc_SuspendPushMode(pMe);
	return elite_msg_finish_msg(&(pMe->CmdMsg), result);
}

/*
 *Function Name: AudioEncSvc_PrepDisconnect
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *
 *Description: For LLNP Tx streams the encoder must return any buffers
 *             to the upstream service and push any existing output data
 *             to the client.
 *
 *Returns: ADSPResult
 *
 */
static ADSPResult AudioEncSvc_PrepDisconnect(AudioEncSvc_t *pMe)
{
	ADSPResult result;
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
			"AudioEncSvc:[PREP_DISC] Preparing for disconnection...");

	// Call flush to release any buffers, do not flush all the remaining buffers in the output q
	// We could enter a race condition where there are many buffers in the output q and as we release
	// the buffers the client is pushing more buffers back into the queue. We should just hold on
	// to these buffers until close at which point they are returned.
	result = AudioEncSvc_Flush(pMe);

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
			"AudioEncSvc:[PREP_DISC] Completed prepare for disconnect cmd with result %d", result);
	// note that the call to flush above has already released pMe->CmdMsg, do not double free
	return result;
}

/**
 * command handler to set output media fmt
 *
 * Currently only use case for this is in AFE
 * AFE slimbus will have mono. encoded data could be stereo.
 * setting out media fmt will make sure that enc+pack output will be mono.
 */
static ADSPResult AudioEncSvc_SetOutMediaFmt(AudioEncSvc_t *pMe)
{
	ADSPResult result;
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,  "AudioEncSvc: setting output media format");

	elite_msg_custom_enc_set_out_media_fmt_t* pPayload = (elite_msg_custom_enc_set_out_media_fmt_t*)((pMe->CmdMsg).pPayload);
	if (pPayload->bits_per_sample || pPayload->sampling_rate)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,  "AudioEncSvc: currently encoder doesn't support setting bps and SR");
	}

	if (pPayload->num_channels)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,  "AudioEncSvc: setting output media format channels %lu", pPayload->num_channels);
		enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);
		audio_enc_svc_capi_v2_media_fmt_t media_fmt;
		media_fmt.main.format_header.data_format = last_capi->out_med_fmt.main.format_header.data_format;
		AudioEncSvc_InitMediaFmt(&media_fmt.std);
		media_fmt.std.num_channels = pPayload->num_channels;
		memscpy(media_fmt.std.channel_type, sizeof(media_fmt.std.channel_type), pPayload->channel_type, sizeof(pPayload->channel_type));
		result = aud_enc_svc_set_media_fmt(last_capi->capi_ptr, &media_fmt, CAPI_V2_OUTPUT_MEDIA_FORMAT);
	}

	result = elite_msg_finish_msg(&(pMe->CmdMsg), ADSP_EOK);

	return result;
}

/*
 *Function Name: AudioEncSvc_ProcessEosCmd
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *
 *Description: If any partial frame of PCM samples are left in the
 *             internal PCM buffer, they are discarded and output
 *             buffer is released
 *             Getting the encoder to ReInit state
 *             Finishes the EOS cmd if no PCM are left.
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_ProcessEosCmd(AudioEncSvc_t* pMe)
{
	//send the output buffer to client
	//for push mode pMe->OutDataMsg.pPayload is NULL
	if(NULL != pMe->OutDataMsg.pPayload)
	{
		AudioEncSvc_FillMetadataAndReleaseOutputBuf(pMe);
	}

	enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

	//when EOS is received and there are encoded frames in output buffers
	//and the output buffers are malloced by encoder service, deliver the
	//encoded frames to stream router service
	EncSvc_OutBufParams_t *pOutBufParams = &(last_capi->out_buf_params);
	if (  ( !AudioEncSvc_IsOutBufFromClient(pMe) )
			&&(NULL != pOutBufParams->out_buf.data_ptr))
	{
		if (0 != pOutBufParams->usNumFramesInOutBuf)
		{
			//there are some encoded frames in out buffers, deliver to stream router
			ADSPResult result = AudioEncSvc_SendOutBufToPeerSvc(pMe);
			if (ADSP_EOK != result)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed deliver output buffers to downstream svc on EOS!");
			}
		}
		else
		{
			//the output buffer is poped but does not have encoded frame yet, return it to queue
			(void) elite_msg_push_payload_to_returnq(pMe->serviceHandle.gpQ, (elite_msg_any_payload_t*)(pMe->outDataBufferNode.pBuffer));
			pMe->outDataBufferNode.pBuffer = NULL;
		}
	}

	if (ENC_SVC_PUSH_MODE != pMe->ulMode)
	{
		//clear the output buffer state
		for (uint8_t i=0; i < ENC_SVC_MAX_CAPI; i++)
		{
			if (!pMe->capiContainer[i]) break;
			AudioEncSvc_InitOutBufParams(&pMe->capiContainer[i]->out_buf_params);
		}

		//if incoming EOS cmd is sent by client, raise EOS ack,
		//otherwise raise EOS event
		if(pMe->isExternalEos)
		{
			elite_msg_data_eos_apr_t *pPayload = (elite_msg_data_eos_apr_t*) (pMe->InpDataMsg.pPayload);

			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Raising rendered EOS event!");

			if (pPayload->eosInfo.event_opcode == ASM_DATA_EVENT_RENDERED_EOS_V2)
			{
				elite_svc_raise_rendered_eos_v2_event( &(pPayload->eosInfo) );
			}
			else
			{
				AudioStreamMgr_GenerateRenderedEosEvent(&(pPayload->eosInfo));
			}
		}
		else
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "EncSvc raising TX EOS event!");
			AudioStreamMgr_GenerateClientEventFromCb( &pMe->CbData,
					ASM_DATA_EVENT_EOS,
					pMe->CbData.unAsynClientToken, /** Right now, use token that ASM provide.
                                                  TODO: EncSvc can put in its own token in the LSB uint8_t. */
					NULL, 0);
		}
	}

	pMe->ulDataLogId++;

	AudioEncSvc_ReleaseInputBuf(pMe, ADSP_EOK);

	pMe->endOfStream  = FALSE;

	for (uint8_t i=0; i < ENC_SVC_MAX_CAPI; i++)
	{
		if (!pMe->capiContainer[i]) break;

		pMe->capiContainer[i]->in_buf_params.inputs.flags.marker_eos = false;
	}

	pMe->unCurrentBitfield  |= AUD_ENC_DATA_SIG;

	return ADSP_EOK;
}

//#define MULTI_CAPI_FILE_LOG 1

static ADSPResult AudioEncSvc_EncodeUsingAllCapis(AudioEncSvc_t* pMe,int *nReturnOutBuf, bool_t *compr_need_more)
{
	bool_t compr_need_more_local = FALSE;
	*nReturnOutBuf = 0;
	*compr_need_more = 0;

	uint32_t bytes_consumed_per_frame = 0;
	bool_t has_bytes_consumed_changed = FALSE;

	ADSPResult result = ADSP_EOK;

	for (uint8_t capi_index = 0; capi_index < ENC_SVC_MAX_CAPI; capi_index++)
	{
		if (!pMe->capiContainer[capi_index]) break;

		//copy from output of first to input of next
		if (0 != capi_index)
		{
			if (pMe->capiContainer[capi_index-1]->out_buf_params.out_buf.actual_data_len <
					pMe->capiContainer[capi_index]->in_buf_params.in_buf.max_data_len)
			{
				memscpy(pMe->capiContainer[capi_index]->in_buf_params.in_buf.data_ptr,
						pMe->capiContainer[capi_index]->in_buf_params.in_buf.max_data_len,
						pMe->capiContainer[capi_index-1]->out_buf_params.out_buf.data_ptr,
						pMe->capiContainer[capi_index-1]->out_buf_params.out_buf.actual_data_len);

				pMe->capiContainer[capi_index]->in_buf_params.in_buf.actual_data_len = pMe->capiContainer[capi_index-1]->out_buf_params.out_buf.actual_data_len;
				pMe->capiContainer[capi_index-1]->out_buf_params.out_buf.actual_data_len= 0;
			}

#if (MULTI_CAPI_FILE_LOG == 1)
	FILE *fp;
char fileName[100];
snprintf(fileName, 100, "enc_CAPI_in_%p.bin", pMe->capiContainer[capi_index]);
fp = fopen(fileName,"a"); // in append mode
fwrite(pMe->capiContainer[capi_index]->in_buf_params.in_buf.data_ptr,sizeof(int8_t),pMe->capiContainer[capi_index]->in_buf_params.in_buf.actual_data_len,fp);
fclose(fp);
#endif

		}

		if (capi_index > 0)
		{
			bytes_consumed_per_frame = pMe->capiContainer[capi_index - 1]->bytes_consumed_per_frame;
		}

		//bytes_consumed_per_frame & has_bytes_consumed_changed refer to previous CAPIs
		AudioEncSvc_HandleFwkExtnBeforeProcess(pMe, pMe->capiContainer[capi_index], bytes_consumed_per_frame, has_bytes_consumed_changed);

		bytes_consumed_per_frame = 0;
		//for nReturnOutBuf: only last CAPI value is good.
		result = AudioEncSvc_Encode(pMe, pMe->capiContainer[capi_index], nReturnOutBuf, &compr_need_more_local, &bytes_consumed_per_frame);

		//need more is from first CAPI only.
		if (0 == capi_index)
		{
			*compr_need_more = compr_need_more_local;
		}

		if (ADSP_FAILED(result))
		{
			return result;
		}
		else
		{
			has_bytes_consumed_changed = FALSE;
			if (pMe->capiContainer[capi_index]->bytes_consumed_per_frame != bytes_consumed_per_frame)
			{
				pMe->capiContainer[capi_index]->bytes_consumed_per_frame = bytes_consumed_per_frame;
				has_bytes_consumed_changed = TRUE;
			}
		}

	}

	return result;
}

/*
 *Function Name: AudioEncSvc_Encode
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *             nReturnOutBuf will be 1 if client buffer satisfies 1<=x<=N
 *             where x is the number of frames filles in client buffer
 *Description: Encodes one frame of PCM samples. Internal
 *             PCM buffer state is updated , based on the encoder CAPI result
 *             frames in output buffer is also updated
 *
 *Returns: ADSPResult
 *
 */
//#define DEBUG_CAPI_IO_SIZES

static ADSPResult AudioEncSvc_Encode(AudioEncSvc_t* pMe, enc_CAPI_container_t *capi_container, int *nReturnOutBuf, bool_t *compr_need_more, uint32_t *bytes_consumed_per_frame)
{
	//get the output buffer
	ADSPResult result = ADSP_EOK;
	capi_v2_err_t capi_result = CAPI_V2_EOK;
	elite_apr_packet_t *pAprPacket;
	asm_data_cmd_read_v2_t *pEncOut = NULL;
	uint16_t *pInternalBuf;
	EncSvc_OutBufParams_t *pOutBufParams = &capi_container->out_buf_params;
	EncSvc_InpBufParams_t *pInBufParams = &capi_container->in_buf_params;

	//Input buffer pointer
	pInternalBuf = (uint16_t*)(capi_container->in_buf_params.in_buf.data_ptr);
	*nReturnOutBuf = 0;
	*compr_need_more = FALSE;

	//Output buffer ptr and offsets
	if (AudioEncSvc_IsOutBufFromClient(pMe))
	{
		result = AudioEncSvc_GetOutputBuffer(pMe, &pAprPacket, &pEncOut);
		if (ADSP_FAILED(result) || (NULL == pEncOut))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc failed to obtain output buffer ");
			return result;
		}
	}

	uint32_t bytesBefore = pInBufParams->in_buf.actual_data_len;

	/** *Adjust output buffer for CAPI V2 */
	uint32_t outSizeBefore = (uint32_t)pOutBufParams->out_buf.actual_data_len;
	int8_t *out_ptr_before = pOutBufParams->out_buf.data_ptr;
	uint32_t out_max_size_before = pOutBufParams->out_buf.max_data_len;
	pOutBufParams->out_buf.data_ptr += outSizeBefore; //move ptr to where data can be written
	pOutBufParams->out_buf.max_data_len -= outSizeBefore; //decrease max length accordingly.
	pOutBufParams->out_buf.actual_data_len = 0; //can be even uninitialized.

#ifdef DEBUG_CAPI_IO_SIZES
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Enc CAPI %p Process before: outSize %lu,inSize=%lu\n",capi_container,outSizeBefore,bytesBefore);
#endif

	/**
	 * At input of process, the input->actual_len is the size of input & data starts from data_ptr.
	 *                      the output->actual_len is uninitialized & CAPI can write from data_ptr.
	 * At output of process, the input->actual_len is the amount of data consumed (read) by CAPI. remaining data is from data_ptr+actual_len
	 *                      the output->actual_len is output data, & data starts from data_ptr.
	 */

	//call encoder CAPI process
	capi_result = capi_container->capi_ptr->vtbl_ptr->process(capi_container->capi_ptr,
			pInBufParams->inputs_ptr,pOutBufParams->outputs_ptr);

#ifdef DEBUG_CAPI_IO_SIZES
	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Enc CAPI %p Process after : result %d, outSize %d,inSize=%d\n",capi_container,result, pOutBufParams->out_buf.actual_data_len,pInBufParams->in_buf.actual_data_len);
#endif

	/** *Adjust output buffer for CAPI V2 */
	pOutBufParams->out_buf.data_ptr = out_ptr_before;
	pOutBufParams->out_buf.actual_data_len += outSizeBefore;
	pOutBufParams->out_buf.max_data_len = out_max_size_before;

	uint32_t remainingBytesAfter = bytesBefore - pInBufParams->in_buf.actual_data_len;

	if (ADSP_SUCCEEDED(capi_result) || (capi_result == CAPI_V2_ENEEDMORE))
	{
		/* this condition handles
         success - some input consumed (or not), some out produced
         need_more - zero or all the input consumed (in either case we need to look into o/p buf), no output produced
         recovery_success - some in consumed, no output produced
		 */

		//need more is detected whenever,
		//a) encode result is success or need more
		//b) no output is produced in each of the outputs
		//c) no input is consumed from stream containing primary
		//when dec says need_more, one frame is not over yet, and we cannot sync to new buf TS, as the TS are synced to frames.
		//in rec success cases, there might be more frames in the buf (hence we won't even look into inp buf q until need-more is returned).
		bool_t out_produced = true, was_need_more = false;

		//error out if some outputs produce while others not. (all outputs must be in sync)

		if (capi_container->out_buf_params.out_buf.actual_data_len != 0)
		{
			out_produced = true;
		}
		else
		{
			out_produced = false;
		}

		if ((remainingBytesAfter == 0) ||
				(remainingBytesAfter == bytesBefore)) //if in didn't get consumed
		{
			if (!out_produced) //no output produced
			{
				was_need_more = true;
			}
		}

#if 0
		//with COP packetizer second CAPI may return need_more. this is due to buffering inside COP.
		if ( (capi_container != pMe->capiContainer[0]) && (was_need_more))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "need more cannot be returned except by first encoder");
			//as first encoder must generate enough data for the later ones. If this happens, we still continue and it may work without TS, but with TS
			//it may not work. Also, if this is due to buffering in the CAPI, then there's no way to get those buffers back later as no additional calls will be made to the
			//CAPIs which buffer. process of additional CAPIs is called only if first one generates an output.
		}
#endif

		if (was_need_more)
		{
			//If no data was consumed in this call, then more data needs to be copied from input buffer
			if(0 == pInBufParams->in_buf.actual_data_len)
			{
				*compr_need_more = TRUE;
			}

			//move the remaining data to the start of the buffer
			memsmove(pInternalBuf,remainingBytesAfter, (int8_t *)pInternalBuf + (pInBufParams->in_buf.actual_data_len),remainingBytesAfter );
			pInBufParams->in_buf.actual_data_len = remainingBytesAfter;
		}
		else
		{

			//Encoded Output Frame Size
			uint32 unEncOutSize = (uint32_t)(pOutBufParams->out_buf.actual_data_len)- outSizeBefore;
			*bytes_consumed_per_frame =  pInBufParams->in_buf.actual_data_len;

			if(pOutBufParams->bFrameMetaDataEnable)
			{
				elite_apr_packet_t *pAprPacket = (elite_apr_packet_t*) (pMe->OutDataMsg.pPayload);
				uint32_t uAprOpCode = elite_apr_if_get_opcode( pAprPacket );

				uint32_t samples_encoded = pInBufParams->in_buf.actual_data_len/(pMe->out_med_fmt_per_enc_cfg.bytes_per_sample * pMe->out_med_fmt_per_enc_cfg.num_channels);
				if(ASM_DATA_CMD_READ_V2 == uAprOpCode)
				{
					AudioEncSvc_FillMetaDataInOutputBuffer(pMe,samples_encoded,
							unEncOutSize,pEncOut, outSizeBefore);
				}
				else//(ASM_DATA_CMD_READ_COMPRESSED_V2 == uAprOpCode)
            		{
					AudioEncSvc_FillMetaDataCompressedInOutputBuffer(pMe,samples_encoded,
							unEncOutSize,pEncOut, outSizeBefore);
            		}
			}

			//move the remaining data to the start of the buffer
			memsmove(pInternalBuf, remainingBytesAfter, (int8_t *)pInternalBuf + (pInBufParams->in_buf.actual_data_len),remainingBytesAfter );
			pInBufParams->in_buf.actual_data_len = remainingBytesAfter;

			//Increment the number of frames in output buffer
			pOutBufParams->usNumFramesInOutBuf++;
			//if this the first frame in buffer, then its timestamp is the buffer timestamp
			if(1 == pOutBufParams->usNumFramesInOutBuf)
			{
				pMe->encTSParams.ullTimeStamp = pMe->encTSParams.ullFrameTimeStamp;
			}
			//If framesPerBuffer N is met return the output buffer
			if(pOutBufParams->usTargetFramesPerBuf == pOutBufParams->usNumFramesInOutBuf)
			{
				*nReturnOutBuf = 1;
			}
#ifdef DBG_AUDIO_ENCODE
			MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Encode successful with encoded bytes(%d), Output buffer returned ? : (%d) ",
					pOutBufParams->out_buf.actual_data_len,*nReturnOutBuf);
#endif

		}
	}
	else //CAPI failed
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Encoding failed");
	}

	if  (result == CAPI_V2_ENOMEMORY) //ENC_BUFFERTOOSMALL_FAILURE //TODO: this should be removed in future. with capi v2.
	{
		if(0 == pOutBufParams->usNumFramesInOutBuf)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Ouput buffer is too small, EncRtrnCodeType(%d) ", result);
			result = ADSP_EFAILED;
		}
		else
		{
			result = ADSP_EOK;
		}
		*nReturnOutBuf = 1;
	}

	return result;
}

/*
 *Function Name: AudioEncSvc_ProcessCustomMsg
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *
 *Description: Handles custom message commands from ASM
 *
 *Returns: ADSPResult
 *
 */
static ADSPResult AudioEncSvc_ProcessCustomMsg(AudioEncSvc_t* pMe)
{
	ADSPResult res = ADSP_EOK;

	EliteMsg_CustomFlushReadbufsType *pPayload =
			(EliteMsg_CustomFlushReadbufsType *)pMe->CmdMsg.pPayload;

	uint32_t ulSecOpCode = pPayload->unSecOpCode;

	switch(ulSecOpCode)
	{
	case ELITEMSG_CUSTOM_FLUSH_READBUFS:
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Processing ELITEMSG_CUSTOM_FLUSH_READBUFS from ASM");

		res = AudioEncSvc_FlushOutQueue(pMe);
		res = elite_msg_finish_msg(&(pMe->CmdMsg), res);

		//see if encoder svc is waiting for this cmd to be issued
		if(pMe->waitForFlushReadBufs)
		{
			//clear the flag and start processing input data buffers
			pMe->waitForFlushReadBufs = false;
			pMe->unCurrentBitfield = AUD_ENC_CMD_SIG | AUD_ENC_DATA_SIG;
		}

		return res;
	case ELITEMSG_CUSTOM_ENC_SUSPEND:
		res = AudioEncSvc_Suspend(pMe);
		return res;
	case ELITEMSG_CUSTOM_ENC_PREPARE_DISCONNECT:
		res = AudioEncSvc_PrepDisconnect(pMe);
		return res;
	case ELITEMSG_CUSTOM_ENC_SET_OUT_MEDIA_FMT: //used only for AFE now.
		res = AudioEncSvc_SetOutMediaFmt(pMe);
		return res;
	default:
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unknown secondary opcode %lx for custom message",
				ulSecOpCode);
		return elite_msg_finish_msg(&(pMe->CmdMsg), ADSP_EBADPARAM);
	}
}

/*
 *Function Name: AudioEncSvc_ProcessEos
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *
 *Description: Process the EOS condition based on Encoder type
 *
 *Returns: ADSPResult
 *
 */
static ADSPResult AudioEncSvc_ProcessEos(AudioEncSvc_t* pMe)
{
	ADSPResult result = ADSP_EOK;

	// Use extern EOS flag to differiate EOS From client
	// or TX EOS that originated from PreProc
	elite_msg_data_eos_header_t *pEos = (elite_msg_data_eos_header_t *) (pMe->InpDataMsg.pPayload);
	if ( ( ELITEMSG_DATA_EOS_APR == pEos->unEosFormat ) )
	{
		pMe->isExternalEos = TRUE;
	}
	else
	{
		pMe->isExternalEos = FALSE;
	}

	bool_t need_to_process_b4_eos = false;

	need_to_process_b4_eos = AudioEncSvc_IsProcessB4Eos(pMe);

	if (need_to_process_b4_eos)
	{
		pMe->endOfStream =  TRUE;
		pMe->unCurrentBitfield = AUD_ENC_CMD_SIG | AUD_ENC_BUF_SIG ;
		result = AudioEncSvc_Proc(pMe);
	}
	else
	{
		//Go to process EOS directly
		result = AudioEncSvc_ProcessEosCmd(pMe);
	}

	return result;
}


/*
 *Function Name: is_more_data_required
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *
 *Description: Checks if more data needs to be copied into internal buffer before process call
 *
 *Returns: ADSPResult
 *
 */
static bool_t is_more_data_required(AudioEncSvc_t* pMe, enc_CAPI_container_t *capi_container, bool_t compr_need_more)
{
	int enc_in_bits_per_sample = pMe->in_med_fmt.bits_per_sample;

	uint32_t enc_in_bytes_per_sample = (enc_in_bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

	//only for depacketizer use case.
	if( pMe->io_fmt_conv == ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW )
	{
		if(compr_need_more)
		{
			return TRUE;
		}
	}
	else if(((uint32_t)capi_container->in_buf_params.in_buf.actual_data_len/enc_in_bytes_per_sample) < capi_container->unEncInpFrameSize)
	{
		return TRUE;
	}
	return FALSE;
}


/*
 *Function Name: AudioEncSvc_UpdateTimeStampState
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *
 *Description: Updates the next frame's timestamp based on consumed samples
 *
 *Returns: ADSPResult
 *
 */
void AudioEncSvc_UpdateFrameTimestamp(AudioEncSvc_t* pMe, enc_CAPI_container_t *first_capi,
		uint32_t consumed_samples)

{
	int num_channels = pMe->in_med_fmt.num_channels;
	int sampling_rate = pMe->in_med_fmt.sample_rate;

	if(pMe->encTSParams.isLastCopyFromNewBuffer)
	{
		//if the last copy was from a new input buffer then timestamp has to be synchronized with that
		uint32_t consumed_samples_from_new_buf = consumed_samples - pMe->encTSParams.samples_in_internal_buf_before_copy;
		pMe->encTSParams.ullFrameTimeStamp = pMe->encTSParams.ullLastBufferTimeStamp +
				((uint64_t)consumed_samples_from_new_buf * NUM_US_PER_SEC)/(num_channels * sampling_rate);
		pMe->encTSParams.isLastCopyFromNewBuffer = FALSE;
		pMe->encTSParams.samples_in_internal_buf_before_copy = 0;
	}
	else
	{
		pMe->encTSParams.ullFrameTimeStamp = pMe->encTSParams.ullFrameTimeStamp + ((uint64_t)consumed_samples * NUM_US_PER_SEC)/
				(num_channels * sampling_rate);
	}

	return;
}

/*===========================================================================
  FUNCTION AudioEncSvc_ConnectToService()

  DESCRIPTION
  Message handler function for getting CONNECT_TO_SVC_CMD

  PARAMETERS
Input:  pMe to the AudioEncSvc_t instance

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS
ACK cmd back upon finish processing

NOTES:
===========================================================================*/
static ADSPResult AudioEncSvc_ConnectToService(AudioEncSvc_t *pMe)
{
	ADSPResult result = ADSP_EOK;

	if (NULL == pMe)
	{
		MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"Connect Audio Encoder Service received bad pointers: 0x%p", pMe);
		return ADSP_EFAILED;
	}

	elite_msg_any_t* pMsg = &(pMe->CmdMsg);

	elite_svc_handle_t *pSvcHandle =
			(elite_svc_handle_t*)(((elite_msg_cmd_connect_t*)(pMsg->pPayload))->pSvcHandle);

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc: instance 0x%8p connecting to SvcID 0x%8lx",
			pMe, pSvcHandle->unSvcId);

	// this service only connects to 1 downstream peer
	if (NULL != pMe->pPeerSvc)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc: Connection FAILED, this service only allows 1 peer, already connected 0x%p.", pMe->pPeerSvc);
		result = ADSP_ENOTREADY;
	}
	else
	{
		pMe->pPeerSvc = pSvcHandle;
		result = ADSP_EOK;
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc Connection Succeeded.\n");
	}    // (NULL != pMe->pPeerSvc)

	elite_svc_send_ack(pMsg, result);

	return result;
}


/*===========================================================================
  FUNCTION AudioEncSvc_DisconnectFromService()

  DESCRIPTION
  Message handler function for getting DISCONNECT_FROM_SVC_CMD

  PARAMETERS
Input:  pInstance to the AudioEncSvc_t instance

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS
ACK cmd back upon finish processing

NOTES:
===========================================================================*/
static ADSPResult AudioEncSvc_DisconnectFromService(AudioEncSvc_t* pMe)
{
	ADSPResult result = ADSP_EOK;

	if (NULL == pMe)
	{
		MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"Disconnect Audio Encoder Service received bad pointers: 0x%p", pMe);
		return ADSP_EFAILED;
	}

	elite_msg_any_t* pMsg = &(pMe->CmdMsg);

	elite_svc_handle_t *pSvcHandle =
			(elite_svc_handle_t*)(((elite_msg_cmd_connect_t*)(pMsg->pPayload))->pSvcHandle);

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc: Instance 0x%8p disconnecting handle 0x%8p",
			pMe, pSvcHandle);

	// this service only allows 1 downstream peer
	if (pSvcHandle != pMe->pPeerSvc)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc: Disonnection FAILED, not connected to that service 0x%p.", pSvcHandle);
		result = ADSP_EBADPARAM;
	}
	//else accept the disconnection
	else
	{
		pMe->pPeerSvc = NULL;
		result = ADSP_EOK;
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc: Disonnection Succeeded.");
	}

	elite_svc_send_ack(pMsg, result);

	/* since we are disconnected, we can no longer process input or output buffers.
      so just listen to command queue
	 */
	if(ADSP_EOK == result)
	{
		pMe->unCurrentBitfield = AUD_ENC_CMD_SIG;
	}

	return result;
}

