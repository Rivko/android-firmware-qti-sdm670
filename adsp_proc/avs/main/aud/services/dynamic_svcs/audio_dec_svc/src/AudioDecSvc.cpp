/*========================================================================

 *//** @file AudioDecSvc.cpp
This file contains functions for Elite Decoder service.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc.cpp#2 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
03/07/10    SJ      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_AprIf.h"

#include "AudioDecSvc.h"
#include "AudioDecSvc_Util.h"
#include "AudioDecSvc_Structs.h"
#include "AudioDecSvc_MiMoUtil.h"
#include "audio_basic_op.h"
#include <audio_basic_op_ext.h>
#include "adsp_asm_api.h"
#include "adsp_media_fmt.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "Interleaver.h"
#include "AudioDecSvc_ChanMap.h"
#include "AudioDecSvc_PullMode.h"
#include "adsp_mtmx_strtr_api.h"
#include "AudioDecSvc_CapiV2Util.h"
#include "audio_module_chain_wrapper.h"
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
#define MAX_BIT_INDEX 31

static char ADEC_THREAD_NAME_PREFIX[]="ADec";

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

static char AUDIO_DEC_SVC_CMD_Q_NAME[] = "DDSc";
static char AUDIO_DEC_SVC_RESP_Q_NAME[] = "DDSr";

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */

// destructor
static void AudioDecSvc_Destroy (AudioDecSvc_t* pMe);

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static int AudioDecSvc_WorkLoop(void* pInstance);

// message handler functions
static int AudioDecSvc_DestroyYourself(AudioDecSvc_t* pMe);
static void AudioDecSvc_DestroyResponseQ (AudioDecSvc_t* pMe);

/* -----------------------------------------------------------------------
 ** Queue handler f
 ** ----------------------------------------------------------------------- */
typedef ADSPResult (*qurt_elite_queueHandler_f) (AudioDecSvc_t* pMe /*pInstance*/, uint32_t channelBitIndex);

/* -----------------------------------------------------------------------
 ** Queue handler f
 ** ----------------------------------------------------------------------- */
typedef ADSPResult (*qurt_elite_cmdQueueHandler_f) (AudioDecSvc_t* pMe /*pInstance*/);

static ADSPResult AudioDecSvc_CustomMsgHandler(AudioDecSvc_t* pMe);
static ADSPResult AudioDecSvc_EoSRespHandler(AudioDecSvc_t* pMe);
static ADSPResult AudioDecSvc_ProcessCmdQueue(AudioDecSvc_t* pMe, uint32_t channelBitIndex);
static ADSPResult AudioDecSvc_ProcessRespQueue(AudioDecSvc_t* pMe, uint32_t channelBitIndex);

static qurt_elite_queueHandler_f pDecQHandler[]=
{
      AudioDecSvc_ProcessCmdQueue,              //31
      NULL,                                     //30
      AudioDecSvc_ProcessRespQueue,             //29 respQ
      NULL,                                     //28

      AudioDecSvc_ProcessInputDataQ,            //27
      AudioDecSvc_ProcessInputDataQ,            //26
      NULL,                                     //25
      NULL,                                     //24

      AudioDecSvc_ProcessOutputDataQ,           //23
      AudioDecSvc_ProcessOutputDataQ,
      AudioDecSvc_ProcessOutputDataQ,
      AudioDecSvc_ProcessOutputDataQ,
      AudioDecSvc_ProcessOutputDataQ,
      AudioDecSvc_ProcessOutputDataQ,           //18
      NULL,                                     //17
      NULL,                                     //16

      AudioDecSvc_EarlyEoS_ProcessTimerSignal,  //15 //early eos timer signal
      AudioDecSvc_ProcessCircBufFill,           //14 //circ buf fill signal


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
static qurt_elite_cmdQueueHandler_f pASMToDecCmdHandler[] =
{

      AudioDecSvc_CustomMsgHandler,     // 0 - ELITE_CUSTOM_MSG
      AudioDecSvc_ReturnUnsupported,   // 1 - ELITE_CMD_START_SERVICE
      AudioDecSvc_DestroyYourself,     // 2 - ELITE_CMD_DESTROY_SERVICE
      AudioDecSvc_ReturnUnsupported,                // 3 - ELITE_CMD_CONNECT
      AudioDecSvc_DisconnectFromService,           // 4 - ELITE_CMD_DISCONNECT
      AudioDecSvc_Pause,                           // 5 - ELITE_CMD_PAUSE
      AudioDecSvc_Run,                             // 6 - ELITE_CMD_RUN
      AudioDecSvc_Flush,               // 7 - ELITE_CMD_FLUSH
      AudioDecSvc_SetParam,            // 8 - ELITE_CMD_SET_PARAM
      AudioDecSvc_ReturnUnsupported,   // 9 - ELITE_CMD_GET_PARAM
      AudioDecSvc_ReturnUnsupported,   // 10 - ELITE_DATA_BUFFER
      AudioDecSvc_ReturnUnsupported,   // 11 - ELITE_DATA_MEDIA_TYPE
      AudioDecSvc_ReturnUnsupported,   // 12 - ELITE_DATA_EOS
      AudioDecSvc_ReturnUnsupported,   // 13 - ELITE_DATA_RAW_BUFFER
      AudioDecSvc_ReturnUnsupported,   // 14 - ELITE_CMD_STOP_SERVICE
      AudioDecSvc_ReturnUnsupported,   // 15 - ELITE_APR_PACKET
      AudioDecSvc_ReturnUnsupported,          //16 - ELITE_MIPS_CHANGE_EVENT
      AudioDecSvc_ReturnUnsupported,          //17 - ELITE_DATA_SET_PARAM
      AudioDecSvc_ReturnUnsupported,          //18 - DML change event
      AudioDecSvc_ReturnUnsupported,          //19 - ELITE_BW_CHANGE_EVENT
      AudioDecSvc_ReturnUnsupported,          //20 - ELITE_DATA_MARK_BUFFER
      AudioDecSvc_ReturnUnsupported,          //21 - ELITE_DATA_BUFFER_V2
      AudioDecSvc_ReturnUnsupported,          //22 - ELITE_CUSTOM_EVENT
      AudioDecSvc_RegisterAprEvents,          //23 - ELITE_CMD_REGISTER_APR_EVENT 0x17
};

static qurt_elite_cmdQueueHandler_f pDecRespHandler[] =
{
      AudioDecSvc_ReturnUnsupported,     // 0 - ELITE_CUSTOM_MSG
      AudioDecSvc_ReturnUnsupported,   // 1 - ELITE_CMD_START_SERVICE
      AudioDecSvc_ReturnUnsupported,     // 2 - ELITE_CMD_DESTROY_SERVICE
      AudioDecSvc_ReturnUnsupported,                // 3 - ELITE_CMD_CONNECT
      AudioDecSvc_ReturnUnsupported,           // 4 - ELITE_CMD_DISCONNECT
      AudioDecSvc_ReturnUnsupported,                           // 5 - ELITE_CMD_PAUSE
      AudioDecSvc_ReturnUnsupported,                             // 6 - ELITE_CMD_RUN
      AudioDecSvc_ReturnUnsupported,               // 7 - ELITE_CMD_FLUSH
      AudioDecSvc_ReturnUnsupported,            // 8 - ELITE_CMD_SET_PARAM
      AudioDecSvc_ReturnUnsupported,   // 9 - ELITE_CMD_GET_PARAM
      AudioDecSvc_ReturnUnsupported,   // 10 - ELITE_DATA_BUFFER
      AudioDecSvc_ReturnUnsupported,   // 11 - ELITE_DATA_MEDIA_TYPE
      AudioDecSvc_EoSRespHandler,      // 12 - ELITE_DATA_EOS
      AudioDecSvc_ReturnUnsupported,   // 13 - ELITE_DATA_RAW_BUFFER
      AudioDecSvc_ReturnUnsupported,   // 14 - ELITE_CMD_STOP_SERVICE
      AudioDecSvc_ReturnUnsupported,   // 15 - ELITE_APR_PACKET
};


static ADSPResult AudioDecSvc_CreateIoStream(AudioDecSvc_t *pMe, AudioDecSvcIOStreamInitParams_t *pIOInitParams, void **handle)
{
   ADSPResult result = ADSP_EOK;

   elite_svc_handle_t *dummy_handle;

   result = AudioDecSvc_CreateOutStream(pMe, pIOInitParams, (void**)&dummy_handle);
   if (ADSP_FAILED(result))
   {
      return result;
   }

   //handle is obtained from input stream bcoz that's the one which contains input dataQ -that ASM needs.
   result = AudioDecSvc_CreateInpStream(pMe, pIOInitParams, handle);
   if (ADSP_FAILED(result))
   {
      return result;
   }

   return result;
}

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */
ADSPResult AudioDecSvc_Create (AudioDecSvcInitParams_t *pInitParams, void **handle)
{
   int result;
   char aCmdQName[QURT_ELITE_DEFAULT_NAME_LEN];     //command queue name
   char aRespQName[QURT_ELITE_DEFAULT_NAME_LEN];     //response queue name
   char aThreadname[QURT_ELITE_DEFAULT_NAME_LEN];

   AudioDecSvc_OutStream_t *pOutStrm;
   AudioDecSvc_InpStream_t *pInpStrm;
   dec_init_time_get_params_t dec_params = {0};

   int32_t nPriority;
   uint32_t threadStackSize;
   bool_t is_supported = true;

   if (NULL==pInitParams
         || NULL == handle
         || NULL == pInitParams->io_param.pDelay)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder service received null pointer(s).");
      return ADSP_EBADPARAM;
   }

   //Real time decoder is not supported with MIMO.
   is_supported &= (pInitParams->is_rt_dec == TRUE) ?
		   (pInitParams->io_param.io_type == AUDIO_DEC_SVC_IN_OUT_STREAM) : true ;

   //PCM decoder is only supported with SISO and Auto Capi type
   is_supported &= (AudioDecSvc_IsPcmFmt(pInitParams->io_param.mimo_dec_fmt_id)) ?
		   (pInitParams->io_param.io_type == AUDIO_DEC_SVC_IN_OUT_STREAM) &&
		   (pInitParams->io_param.in.io_fmt_conv == DEC_SVC_IO_FORMAT_CONV_TYPE_AUTO) : true;

   //MIMO decoder is only supported with Auto Capi type.
   is_supported &= (pInitParams->io_param.io_type != AUDIO_DEC_SVC_IN_OUT_STREAM)?
		   (pInitParams->io_param.in.io_fmt_conv == DEC_SVC_IO_FORMAT_CONV_TYPE_AUTO) : true;

   /* Validating init param for Capi creation. */
   if (!is_supported)
   {
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder service received bad parameters.");
	   return ADSP_EBADPARAM;
   }

   //Initialize to NULL
   *handle = NULL;

   CHECK_ERR_BITS_PER_SAMPLE(pInitParams->io_param.bits_per_sample)

   // allocate instance struct
   AudioDecSvc_t *pMe = (AudioDecSvc_t*) qurt_elite_memory_malloc( sizeof(AudioDecSvc_t), ADEC_SVC_OBJ_INSTANCE);
   if (!pMe)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create AudDec Svc.It requires %lu bytes",sizeof(AudioDecSvc_t) );
      return ADSP_ENOMEMORY;
   }

   // zero out all the fields.
   memset(pMe, 0, sizeof(*pMe));

   pMe->is_rt_dec = pInitParams->is_rt_dec;

   // set up channel
   qurt_elite_channel_init(&pMe->channel);

   pMe->ulMode = DEC_SVC_UNKNOWN_MODE;

   int count = qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;

   // name the queues. Name is concatenated with 5 items:
   // - unique 3 letter code for service
   // - letter abbreviating which queue, e.g. 'd' 'c' 'b' for data, cmd, buf.
   // - number in case there are multiple queues of the same type, e.g. 2 buf q's
   // - count (up to 5 digits)
   // e.g. the first instance of this service would name its dataQ "/EPLd00".and cmd q "/EPLc00"

   snprintf(aCmdQName, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x", AUDIO_DEC_SVC_CMD_Q_NAME,count);
   snprintf(aRespQName, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x", AUDIO_DEC_SVC_RESP_Q_NAME,count);

   //Since this ADec can have many streams (MIMO) & in a session many Adecs could be there, we cannot use stream-id/session-id
   snprintf(aThreadname, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x", ADEC_THREAD_NAME_PREFIX, count);

   if ( ADSP_FAILED(result = qurt_elite_queue_create(aCmdQName,
         MAX_CMD_Q_ELEMENTS, &(pMe->pCmdQ)))
         || ADSP_FAILED(result = qurt_elite_queue_create(aRespQName,
               MAX_RESP_Q_ELEMENTS, &(pMe->pRespQ)))
   )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create Decoder service message queues!! \n");
      goto __bailout;
   }

   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->pCmdQ, AUD_DEC_CMD_SIG))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->pRespQ, AUD_DEC_RESP_SIG)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to add mqs to channel result = %d!!\n", result);
      goto __bailout;
   }

   //Create Capis.
   if (ADSP_FAILED(result = AudioDecSvc_CreateCAPIs(pMe, pInitParams, &dec_params)))
   {
	   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Capis creation failed with result = %d!!\n", result);
	   goto __bailout;
   }

   switch(pInitParams->io_param.io_type)
   {
   case AUDIO_DEC_SVC_MIMO_INPUT_STREAM:
   {
      result = AudioDecSvc_CreateInpStream(pMe, &pInitParams->io_param, handle);
      break;
   }
   case AUDIO_DEC_SVC_MIMO_OUTPUT_STREAM:
   {
      result = AudioDecSvc_CreateOutStream(pMe, &pInitParams->io_param, handle);
      break;
   }
   case AUDIO_DEC_SVC_IN_OUT_STREAM:
   {
      result = AudioDecSvc_CreateIoStream(pMe, &pInitParams->io_param, handle);
      break;
   }
   default:
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder service creation received invalid io_type=%u ", pInitParams->io_param.io_type);
      result = ADSP_EBADPARAM;
   }
   }
   if (ADSP_FAILED(result))      goto __bailout;

   pInpStrm = AudioDecSvc_GetInputStreamById(pMe, pInitParams->io_param.stream_id);
   pOutStrm = AudioDecSvc_GetOutputStreamById(pMe, pInitParams->io_param.stream_id);

   //TODO: to set the output format on the CAPI
   //pIOInitParams->format_id

   //get required stack size for this decoder service instance
   threadStackSize = dec_params.max_stack_size;
   (void)AudioDecSvc_GetRequiredThreadStackSize(pMe, &threadStackSize);

   nPriority = AudioDecSvc_GetThreadPriority(pMe, pInitParams->io_param.perf_mode);

   // Launch the thread
   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(pMe->thread_id), aThreadname, NULL,
         threadStackSize, nPriority, AudioDecSvc_WorkLoop, (void*)pMe, ADEC_SVC_THRD_STACK)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to launch Decoder service thread!! \n");
      goto    __bailout;
   }

   ((elite_svc_handle_t*) (*handle))->threadId = pMe->thread_id;

   if (NULL != pInpStrm)
   {
      pInpStrm->common.serviceHandle.threadId = pMe->thread_id;
   }

   if (NULL != pOutStrm)
   {
      pOutStrm->common.serviceHandle.threadId = pMe->thread_id;
   }

   return ADSP_EOK;

   __bailout:
   AudioDecSvc_Destroy(pMe);
   *handle = NULL;
   return ADSP_EFAILED;
}


// destructor
static void AudioDecSvc_Destroy (AudioDecSvc_t* pMe)
{
   if (pMe)
   {
	  if(pMe->imc_info_ptr)
	  {
	     (void)AudioDecSvc_IMCDestroy(pMe);
	  }
      /** release, force_vote doesn't matter.*/
      (void)AudioDecSvc_ProcessKppsBw(pMe, TRUE, FALSE);
      (void)AudioDecSvc_DeregisterWithAdsppm(pMe);

      //destroy internal buffers
      AudioDecSvc_DestroyInternalBuffers(pMe); //destroy both in/out size bufs.

      for (uint16_t i=0; i<DEC_SVC_MAX_INPUT_STREAMS; i++)
      {
         if (pMe->in_streams_ptr[i])
         {
            // call utility function to destroy data Q
            AudioDecSvc_DestroyInpStream(pMe, &pMe->in_streams_ptr[i]);
         }
      }
      for (uint16_t i=0; i<DEC_SVC_MAX_OUTPUT_STREAMS; i++)
      {
         if (pMe->out_streams_ptr[i])
         {
            // call utility function to destroy data Q
            AudioDecSvc_DestroyOutStream(pMe, &pMe->out_streams_ptr[i]);
         }
      }

      if(NULL != pMe->pUll_modules)
      {
         audio_module_chain_wrapper_destroy(pMe->pUll_modules);
         qurt_elite_memory_free(pMe->pUll_modules);
         pMe->pUll_modules = NULL;
      }
      // call utility function to destroy cmd Q
      elite_svc_destroy_cmd_queue(pMe->pCmdQ);
      pMe->pCmdQ = NULL;

      for (uint32_t capi_index=0; capi_index < DEC_SVC_MAX_CAPI; capi_index++)
      {
         if (NULL != pMe->capiContainer[capi_index])
         {
            //destroy CAPI decoder
            if (pMe->capiContainer[capi_index]->capi_ptr)
            {
               //release CAPI memory: core library is released here
               pMe->capiContainer[capi_index]->capi_ptr->vtbl_ptr->end(pMe->capiContainer[capi_index]->capi_ptr) ;

               pMe->capiContainer[capi_index]->dec_destroy_fn(&(pMe->capiContainer[capi_index]->capi_ptr));
            }

            if (pMe->capiContainer[capi_index]->amdb_handle)
            {
               audio_dec_svc_release_amdb_handle(pMe->capiContainer[capi_index]->amdb_handle);
               pMe->capiContainer[capi_index]->amdb_handle = NULL;
            }

            qurt_elite_memory_free(pMe->capiContainer[capi_index]);
            pMe->capiContainer[capi_index] = NULL;
         }
      }

      aud_dec_svc_delete_all_nodes(&pMe->event_list);

      pMe->mainCapiContainer = NULL;

      //destroy thread related state variables
      // call utility function to destroy Resp Q
      AudioDecSvc_DestroyResponseQ(pMe);

      qurt_elite_channel_destroy(&pMe->channel);

      qurt_elite_memory_free (pMe);
      pMe = NULL;

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Destroyed audio decoder service.");
   }
}

static void AudioDecSvc_DestroyResponseQ (AudioDecSvc_t* pMe)
{
   // clean up buffer queue
   if (pMe->pRespQ)
   {
      qurt_elite_queue_destroy(pMe->pRespQ);
      pMe->pRespQ = NULL;
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

static int AudioDecSvc_WorkLoop(void* pInstance)
{

   AudioDecSvc_t *pMe = (AudioDecSvc_t*)pInstance;       // instance structure
   ADSPResult result = ADSP_EFAILED;
   uint32_t unChannelStatus;

   pMe->unCurrentBitfield = AUD_DEC_CMD_SIG;

   // set up mask for listening to the msg queues.
   // save for mask bitfields
   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe);
   if (pInpStrm &&
         ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pInpStrm->common.perf_mode) ||
               (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pInpStrm->common.perf_mode)))
   {
      // For LLNP/ULL, decoder service may need to process media type command before
      // getting connected to downstream service. This is because decoder will connect
      // to downstream service during matrix map routing command. Which could be later to media type command.
      AudioDecSvc_ListenToInput(pMe);
   }

   (void)AudioDecSvc_RegisterWithAdsppm(pMe);

   /** force vote including both svc and capi votes.*/
   (void)AudioDecSvc_ProcessKppsBw(pMe, FALSE, TRUE);

   // Enter forever loop
   for(;;)
   {
      // block on any one or more of selected queues to get a msg
      (void) qurt_elite_channel_wait(&(pMe->channel), pMe->unCurrentBitfield);

      for (;;)
      {
         // Check for signals
         unChannelStatus = qurt_elite_channel_poll(&(pMe->channel), pMe->unCurrentBitfield);

         if( unChannelStatus == 0 ) break;

         int32_t bitIndex = MAX_BIT_INDEX - s32_cl0_s32(unChannelStatus); //count leading zeros starting from MSB (subtracting from 31 gives index of the 1 from right, (conventional bit index))

         result = pDecQHandler[MAX_BIT_INDEX - bitIndex](pMe, bitIndex);

         if(result == ADSP_ETERMINATED) return ADSP_EOK;

      }

   }

   return result;

}


static ADSPResult AudioDecSvc_ProcessRespQueue(AudioDecSvc_t* pMe, uint32_t channelBitIndex)
{

   ADSPResult result=ADSP_EOK;

   // Take next msg off the q
   result = qurt_elite_queue_pop_front(pMe->pRespQ, (uint64_t*)&(pMe->cmdMsg) );

   // Process the msg
   if (ADSP_EOK == result)
   {
#ifdef DBG_AUDIO_DECODE
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Message ID 0x%8x!!", pMe->cmdMsg.unOpCode);
#endif
      /* Sanity check */
      const uint32_t DecRespTableSize = sizeof(pDecRespHandler)/sizeof(pDecRespHandler[0]);
      if ( pMe->cmdMsg.unOpCode >= DecRespTableSize )
      {
         return AudioDecSvc_ReturnUnsupported(pMe);
      }

      // table lookup to call handling function, with FALSE to indicate processing of msg
      result = pDecRespHandler[pMe->cmdMsg.unOpCode](pMe);
   }

   return result;
}

static ADSPResult AudioDecSvc_ProcessCmdQueue(AudioDecSvc_t* pMe, uint32_t channelBitIndex)
{

   ADSPResult result=ADSP_EOK;

   // Take next msg off the q
   result = qurt_elite_queue_pop_front(pMe->pCmdQ, (uint64_t*)&(pMe->cmdMsg) );

   // Process the msg
   if (ADSP_EOK == result)
   {
#ifdef DBG_AUDIO_DECODE
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Message ID 0x%8x!!", pMe->cmdMsg.unOpCode);
#endif
      /* Sanity check */
      const uint32_t DecCmdTableSize = sizeof(pASMToDecCmdHandler)/sizeof(pASMToDecCmdHandler[0]);
      if ( pMe->cmdMsg.unOpCode >= DecCmdTableSize )
      {
         return AudioDecSvc_ReturnUnsupported(pMe);
      }

      // table lookup to call handling function, with FALSE to indicate processing of msg
      result = pASMToDecCmdHandler[pMe->cmdMsg.unOpCode](pMe);
   }

   return result;
}

static int AudioDecSvc_DestroyYourself (AudioDecSvc_t* pMe)
{
   elite_msg_any_t cmdMsg;
   cmdMsg = pMe->cmdMsg;

   AudioDecSvc_Destroy(pMe);
   //elite_msg_finish_msg( &(pMe->cmdMsg), ADSP_EOK );
   elite_msg_finish_msg( &cmdMsg, ADSP_EOK );

   // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
   return ADSP_ETERMINATED;
}

/**
 * after EoS reaches AFE in ULL/ULL-PP/LLNP, it responds back to Decoder
 * the decoder then sends the ack to ASM which raises to HLOS.
 */
static int AudioDecSvc_EoSRespHandler(AudioDecSvc_t* pMe)
{
   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"DecSvc : received EoS resp");
   return AudioDecSvc_RaiseRenderedEoSv2(pMe, &pMe->cmdMsg);
}

static int AudioDecSvc_CustomMsgHandler(AudioDecSvc_t* pMe)
{
   ADSPResult result = ADSP_EOK;

   elite_msg_custom_header_t* pPayload = (elite_msg_custom_header_t*)((pMe->cmdMsg).pPayload);

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"DecSvc : rcvd custom msg 0x%lx",
         pPayload->unSecOpCode);
   switch (pPayload->unSecOpCode)
   {
   case ELITEMSG_CUSTOM_DTMF_CTL:
   {
      result = AudioDecSvc_DtmfHandler(pMe);
      break;
   }
   case ELITEMSG_CUSTOM_DEC_SUSPEND:
   {
      result = AudioDecSvc_StartSuspend(pMe);
      break;
   }
   case ELITEMSG_CUSTOM_DEC_OPEN_STREAM:
   {
      result = AudioDecSvc_OpenStream(pMe);
      break;
   }
   case ELITEMSG_CUSTOM_DEC_CLOSE_STREAM:
   {
      result = AudioDecSvc_CloseStream(pMe);
      break;
   }
   case ELITEMSG_CUSTOM_DEC_CONNECT_STREAM:
   {
      result = AudioDecSvc_ConnectToService(pMe);
      break;
   }
   case ELITEMSG_CUSTOM_DEC_SET_PP_PARAM:
   {
      result = AudioDecSvc_SetPpParam(pMe);
      break;
   }
   case ELITEMSG_CUSTOM_DEC_GET_PP_PARAM:
   {
      result = AudioDecSvc_GetPpParam(pMe);
      break;
   }
   default:
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Custom cmd rcvd by Audio Dec Svc is invalid");
      result = ADSP_EBADPARAM;
      break;
   }
   }

   return result;
}


