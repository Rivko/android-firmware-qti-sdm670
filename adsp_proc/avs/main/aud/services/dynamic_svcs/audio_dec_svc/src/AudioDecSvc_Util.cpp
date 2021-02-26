/*========================================================================

 *//** @file AudioDecSvc_Util.cpp
This file contains utility functions for Elite Audio Decoder service.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_Util.cpp#2 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
07/12/10   Wen Jin      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_AprIf.h"
#include "AdspCoreSvc.h"
#include "AudioDecSvc.h"
#include "AudioDecSvc_Util.h"
#include "AudioDecSvc_MiMoUtil.h"
#include "audio_basic_op.h"
#include <audio_basic_op_ext.h>
#include "adsp_asm_api.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "AudioDecSvc_PullMode.h"
#include "AudioDecSvc_CapiV2CallbackHandler.h"
#include "AudioDecSvc_CapiV2Util.h"
#include "AFEInterface.h"
#include "audio_module_chain_wrapper.h"

//#define AUDENCDEC_TIMESTAMP_LOGS

extern qurt_elite_globalstate_t qurt_elite_globalstate;

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
/** when comparing input to output for RT mode, min resolution of 1 ms is needed as AFE is of 1 ms.
 * Extra 50 is for any round off errors.*/
#define IO_TIME_COMPARISON_THRESHOLD_US 1050

#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */

/* Number of modules in ULL PCM decoder path */
avcs_module_instance_info_t ull_dec_vol_ctrl_def = {
   AUDPROC_MODULE_ID_VOL_CTRL,
   0,
   0
};

static avcs_module_instance_info_t pcm_ull_dec_mod_array[] = {
    ull_dec_vol_ctrl_def,
};

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
#define LAST_CAPI       TRUE
#define NOT_LAST_CAPI   FALSE

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
static ADSPResult AudioDecSvc_CreateDecCAPI(AudioDecSvc_t* pMe, Dec_CAPI_Type capi_type, uint32_t uMediaFmt, uint32_t id2,
      AudioDecSvcInitParams_t *pInitParams, bool_t is_last_capi, dec_init_time_get_params_t *dec_params);
static void AudioDecSvc_DetermineSvcBw(AudioDecSvc_t* pMe, int32_t sample_rate, int32_t num_channels, uint16_t bits_per_sample, DecPcmFmt_t *pcmInFmt);
static ADSPResult AudioDecSvc_AggregateKppsRequired(AudioDecSvc_t *pMe, uint32_t *decoder_kpps);
static void AudioDecSvc_UpdateInitialDelay(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

ADSPResult AudioDecSvc_CreateCAPIs(AudioDecSvc_t *pMe, AudioDecSvcInitParams_t *pInitParams,
      dec_init_time_get_params_t *dec_params)
{
   ADSPResult result = ADSP_EOK;
   uint32_t media_fmt_id ;
   eDecoderIOFormatConvType io_fmt_conv;

   switch(pInitParams->io_param.io_type)
   {
   case AUDIO_DEC_SVC_MIMO_INPUT_STREAM:
   case AUDIO_DEC_SVC_MIMO_OUTPUT_STREAM:
   {
	   media_fmt_id = pInitParams->io_param.mimo_dec_fmt_id;
	   io_fmt_conv = DEC_SVC_IO_FORMAT_CONV_TYPE_AUTO;
   }
   break;
   default:
   {
	   media_fmt_id = pInitParams->io_param.format_id;
	   io_fmt_conv = pInitParams->io_param.in.io_fmt_conv;
   }
   break;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Creating CAPIs. io fmt conv mode %d. format id 0x%lx. converter mode %lu",
         io_fmt_conv, media_fmt_id, pInitParams->io_param.decoderConverterMode);

   switch(io_fmt_conv)
   {
   case DEC_SVC_IO_FORMAT_CONV_TYPE_AUTO:               //decode raw to PCM
   {
      result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_AUTO,
            media_fmt_id, 0, pInitParams, LAST_CAPI, dec_params);
      pMe->mainCapiContainer = pMe->capiContainer[0];
      break;
   }
   case DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_61937:     //pass through
   {
      result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_PASS_THROUGH,
            media_fmt_id, 0, pInitParams, LAST_CAPI, dec_params);
      pMe->mainCapiContainer = pMe->capiContainer[0];
      break;
   }
   case DEC_SVC_IO_FORMAT_CONV_TYPE_RAW_TO_61937:       //packetize
   {
      uint32_t output_fmt_id;
      result = AudioDecSvc_DetermineConverterOutputFormat(media_fmt_id,
            pInitParams->io_param.decoderConverterMode, &output_fmt_id); //new fmt id for packetizer
      if (ADSP_FAILED(result))
      {
         output_fmt_id = media_fmt_id;
      }

      if(output_fmt_id != media_fmt_id)
      {
         result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_CONVERTER,
               media_fmt_id, output_fmt_id, pInitParams, NOT_LAST_CAPI, dec_params);  //media fmt id denotes the input format.
         if (ADSP_FAILED(result)) return result;
      }
      result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_PACKETIZER,
            output_fmt_id, 0, pInitParams,LAST_CAPI, dec_params);
      pMe->mainCapiContainer = pMe->capiContainer[0];
      break;
   }
   case DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_PCM:         //depacketizer->decodes
   case DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_MULTI:       //depacketize & use MIMO
   {
      result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_DEPACKETIZER,
            media_fmt_id, 0, pInitParams, NOT_LAST_CAPI, dec_params); //media fmt is ignored.
      if (ADSP_FAILED(result)) return result;

      result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_AUTO,
            media_fmt_id, 0, pInitParams, LAST_CAPI, dec_params);
      pMe->mainCapiContainer = pMe->capiContainer[1];
      break;
   }
   case DEC_SVC_IO_FORMAT_CONV_TYPE_61937_CONV_61937:   //depacketizer->conv->packetize
   {
      result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_DEPACKETIZER,
            media_fmt_id, 0, pInitParams, NOT_LAST_CAPI, dec_params); //media_fmt_id ignored
      if (ADSP_FAILED(result)) return result;

      uint32_t output_fmt_id;
      result = AudioDecSvc_DetermineConverterOutputFormat(media_fmt_id,
            pInitParams->io_param.decoderConverterMode, &output_fmt_id); //new fmt id for packetizer
      if (ADSP_FAILED(result)) return result;

      result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_CONVERTER,
            media_fmt_id, output_fmt_id, pInitParams, NOT_LAST_CAPI, dec_params);  //media fmt id denotes the input format.
      if (ADSP_FAILED(result)) return result;


      result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_PACKETIZER,
            output_fmt_id, 0, pInitParams, LAST_CAPI, dec_params);

      pMe->mainCapiContainer = pMe->capiContainer[1];

      break;
   }
   case DEC_SVC_IO_FORMAT_CONV_TYPE_COP_TO_PCM:
   {
      if(media_fmt_id == 0) //if there is only a depacketizerand no decoder
      {
    	  result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_COP_DEPACKETIZER,
    	           media_fmt_id, 0, pInitParams, LAST_CAPI, dec_params);
    	  if (ADSP_FAILED(result)) return result;
    	  pMe->mainCapiContainer = pMe->capiContainer[0];
      }
      else
      {
    	 result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_COP_DEPACKETIZER,
    	          media_fmt_id, 0, pInitParams, NOT_LAST_CAPI, dec_params); //media fmt is ignored.
    	 if (ADSP_FAILED(result)) return result;
         result = AudioDecSvc_CreateDecCAPI(pMe, DEC_CAPI_TYPE_AUTO,
            media_fmt_id, 0, pInitParams, LAST_CAPI, dec_params);
         pMe->mainCapiContainer = pMe->capiContainer[1];
      }
      break;
   }
   case DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW:       //depacketize
   case DEC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_61937:       //encode->packetize.
   default:
      result = ADSP_EUNSUPPORTED;
   }

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create the decoder CAPI instances");
      return ADSP_EFAILED;
   }

   if(AudioDecSvc_IsPcmFmt(media_fmt_id) && (pInitParams->io_param.perf_mode == ASM_ULTRA_LOW_LATENCY_STREAM_SESSION))
   {
      uint32_t modules_stack_size = 0;
      ADSPResult res = ADSP_EOK;
      avcs_module_instance_info_t *module_instance_info_array = pcm_ull_dec_mod_array;
      uint32_t num_ull_modules = SIZE_OF_ARRAY(pcm_ull_dec_mod_array);
      pMe->pUll_modules = (AudioModuleCapiWrapper_t *)qurt_elite_memory_malloc(sizeof(AudioModuleCapiWrapper_t), QURT_ELITE_HEAP_DEFAULT);
      if(NULL  == pMe->pUll_modules)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for ULL modules handle");
      }
      else
      {
         res = audio_module_chain_wrapper_create(pMe->pUll_modules, &modules_stack_size, num_ull_modules, module_instance_info_array);
         if(ADSP_FAILED(res))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc ULL modules creation failed with result %lu", res);
            qurt_elite_memory_free(pMe->pUll_modules);
            pMe->pUll_modules = NULL;
         }
         else
         {
            dec_params->max_stack_size += modules_stack_size;  
         }
      }      
        
   }             

   return ADSP_EOK;
}

void AudioDecSvc_GetInitPCMDecBufSize(AudioDecSvc_t* pMe, AudioDecSvcIOStreamInitParams_t *pInitIOParams, uint32_t *outBufSize)
{
   uint32_t unDecOutBufSizeBytes = AUD_DEC_PCM_OUT_BUF_SIZE;
   bool_t calBufSizeFromMediaFmt = false;
   uint32_t sampleRate, numChannel, bytesPerSample;

   *outBufSize = 0;

   if ((ASM_LOW_LATENCY_STREAM_SESSION == pInitIOParams->perf_mode ) ||
		   (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pInitIOParams->perf_mode) ||
		   (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pInitIOParams->perf_mode ))
   {
	   //Derive size in terms of AFE frame size for 48000, stereo, 16 bit
	   //TBD: Remove the hardcoding to 48000 and use the sample rate from media format

	   sampleRate = 48000;
	   numChannel = 2;
	   bytesPerSample = (pInitIOParams->bits_per_sample == 16) ? 2 : 4;
	   calBufSizeFromMediaFmt = true;
   }

   if (DEC_SVC_PULL_MODE == pInitIOParams->in.ulMode)
   {
	   asm_multi_channel_pcm_fmt_blk_v4_t *pcmFmtBlk = &(pInitIOParams->in.pull_mode.MediaFmt.pcm);
	   bytesPerSample = (pInitIOParams->bits_per_sample == 16) ? 2 : 4;
	   sampleRate = pcmFmtBlk->sample_rate;
	   numChannel = pcmFmtBlk->num_channels;
	   calBufSizeFromMediaFmt = true;
   }

   if(calBufSizeFromMediaFmt)
   {
	   unDecOutBufSizeBytes = AudioDecSvc_GetPcmOutBufSize(pInitIOParams->perf_mode,
			   	   	   	   	   	   	   	   	   	   sampleRate, bytesPerSample, numChannel);
   }

   *outBufSize = unDecOutBufSizeBytes;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Pcm Decoder perf mode is %d, Output buf size is %lu",pInitIOParams->perf_mode, unDecOutBufSizeBytes);

   return;
}

//id2 is valid only for converter case.
static ADSPResult AudioDecSvc_CreateDecCAPI(AudioDecSvc_t* pMe, Dec_CAPI_Type capi_type, uint32_t uMediaFmt, uint32_t id2,
      AudioDecSvcInitParams_t *pInitParams, bool_t is_last_capi, dec_init_time_get_params_t *dec_params)
{
   ADSPResult result = ADSP_EFAILED;
   uint8_t    capi_index;

   dec_CAPI_init_time_get_params_t init_time_get_params;
   memset(&init_time_get_params, 0, sizeof(dec_CAPI_init_time_get_params_t));

   //find a slot in CAPI container array
   for (capi_index = 0 ; capi_index < DEC_SVC_MAX_CAPI; capi_index++)
   {
      if (NULL == pMe->capiContainer[capi_index])       break;
   }
   if(DEC_SVC_MAX_CAPI == capi_index)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "All CAPIs occupied, cannot create any new CAPI!");
      return ADSP_EFAILED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Creating CAPI at index %u, type %u (0-auto, 1-depack, 2-conv, 3-passthu, 4-pack, 5-cop-pack, 6-cop-depack)", capi_index, capi_type);

   pMe->capiContainer[capi_index] = (dec_CAPI_container_t*) qurt_elite_memory_malloc(sizeof(dec_CAPI_container_t), QURT_ELITE_HEAP_DEFAULT);

   if (NULL == pMe->capiContainer[capi_index])
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No memory creating container for decoder");
      return ADSP_ENOMEMORY;
   }
   memset(pMe->capiContainer[capi_index], 0, sizeof(dec_CAPI_container_t));

   pMe->capiContainer[capi_index]->dec_fmt_id = uMediaFmt;
   pMe->capiContainer[capi_index]->capi_type = capi_type;

   if (AudioDecSvc_IsPcmFmt(uMediaFmt))
   {
     pMe->capiContainer[capi_index]->out_data_fmt_type[0] = CAPI_V2_FIXED_POINT;
	  dec_params->max_stack_size = PCM_STACK_SIZE;
	   /* With PCM capi, there can not be a use case with multiple Capis.
	    * Therefore no internal buffers are required.*/
      return ADSP_EOK;
   }


   dec_CAPI_init_params_t capi_init_param;
   capi_init_param.uMediaFmt = uMediaFmt;
   capi_init_param.id2 = id2;
   capi_init_param.cb_info = audio_dec_svc_get_capi_v2_callback_handler(pMe, capi_index);

   //since this function is called at beginning, both indices can be zero.
   capi_init_param.input_port_index.index = 0; //use 0 since this is function is called only during creation.
   capi_init_param.output_port_index.index = 0; //use 0 since this is function is called only during creation.
   capi_init_param.input_port_index.valid = true;
   capi_init_param.output_port_index.valid = true;
   capi_init_param.dec_destroy_fn = NULL;


   //Create the instance
   switch (capi_type)
   {
   case DEC_CAPI_TYPE_AUTO:
   {
      //check if CAPI is present in AMDB.
      dec_AMDB_presence amdb_presence = audio_dec_svc_get_amdb_presence(capi_type, capi_init_param.uMediaFmt, 0, &capi_init_param.amdb_handle);

      if(DEC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMDB CAPI V2 found for fmt 0x%lx. Trying to create CAPI V2", uMediaFmt);
         result = audio_dec_svc_create_init_auto_capi_v2(&(pMe->capiContainer[capi_index]->capi_ptr), pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "fmt 0x%lx is present as an unsupported type in AMDB, failing capi creation.", uMediaFmt);
         result = ADSP_EFAILED;
      }
      break;
   }
   case DEC_CAPI_TYPE_DEPACKETIZER:
   {
      //check if CAPI is present in AMDB.
      capi_init_param.uMediaFmt = AVS_MODULE_ID_DEPACKETIZER_IEC61937;
      dec_AMDB_presence amdb_presence = audio_dec_svc_get_amdb_presence(capi_type, capi_init_param.uMediaFmt, 0, &capi_init_param.amdb_handle);

      if(DEC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMDB CAPI V2 found for fmt 0x%lx. Trying to create CAPI V2", uMediaFmt);
         result = audio_dec_svc_create_init_depack_capi_v2(&pMe->capiContainer[capi_index]->capi_ptr, pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "module 0x%lx is present as an unsupported type in AMDB, failing capi creation.", uMediaFmt);
         result = ADSP_EFAILED;
      }
      break;
   }
   case DEC_CAPI_TYPE_CONVERTER:
   {
      //check if CAPI is present in AMDB.
      dec_AMDB_presence amdb_presence = audio_dec_svc_get_amdb_presence(capi_type, capi_init_param.uMediaFmt, capi_init_param.id2, &capi_init_param.amdb_handle);

      if(DEC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMDB CAPI V2 found for fmt 0x%lx. Trying to create CAPI V2", uMediaFmt);
         result = audio_dec_svc_create_init_conv_capi_v2(&(pMe->capiContainer[capi_index]->capi_ptr), pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "fmt 0x%lx is present as an unsupported type in AMDB, failing capi creation.", uMediaFmt);
         result = ADSP_EFAILED;
      }
      break;
   }
   case DEC_CAPI_TYPE_PASS_THROUGH:
   {
      //check if CAPI is present in AMDB.
      dec_AMDB_presence amdb_presence = audio_dec_svc_get_amdb_presence(capi_type, AVS_MODULE_ID_PACKETIZER_IEC61937, 0, &capi_init_param.amdb_handle);

      if(DEC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMDB CAPI V2 found for module-id 0x%lx. Trying to create CAPI V2 for pass-through formatter", AVS_MODULE_ID_PACKETIZER_IEC61937);
         result = audio_dec_svc_create_init_passthru_capi_v2(&pMe->capiContainer[capi_index]->capi_ptr, pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "module 0x%lx is present as an unsupported type in AMDB, failing capi creation.", AVS_MODULE_ID_PACKETIZER_IEC61937);
         result = ADSP_EFAILED;
      }

      break;
   }
   case DEC_CAPI_TYPE_PACKETIZER: //can be converter as well.
   {
      //check if CAPI is present in AMDB.
      dec_AMDB_presence amdb_presence = audio_dec_svc_get_amdb_presence(capi_type, capi_init_param.uMediaFmt, 0, &capi_init_param.amdb_handle);

      if(DEC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMDB CAPI V2 found for fmt 0x%lx. Trying to create CAPI V2", uMediaFmt);
         result = audio_dec_svc_create_init_pack_capi_v2(&(pMe->capiContainer[capi_index]->capi_ptr), pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "fmt 0x%lx is present as an unsupported type in AMDB, failing capi creation.", uMediaFmt);
         result = ADSP_EFAILED;
      }
      break;
   }
   case DEC_CAPI_TYPE_COP_DEPACKETIZER:
   {
      //check if CAPI is present in AMDB.
      dec_AMDB_presence amdb_presence = audio_dec_svc_get_amdb_presence(DEC_CAPI_TYPE_DEPACKETIZER, pInitParams->io_param.in.cop_depacketizer_module_id, 0, &capi_init_param.amdb_handle);
      if(DEC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Trying to create COP depacketizer CAPI V2 module-id 0x%lx ", pInitParams->io_param.in.cop_depacketizer_module_id);
         result = audio_dec_svc_create_init_depack_capi_v2(&pMe->capiContainer[capi_index]->capi_ptr, pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "COP depacketizer 0x%lx not found ", pInitParams->io_param.in.cop_depacketizer_module_id);
         result = ADSP_EFAILED;
      }
      break;
   }
   default:
      return ADSP_EBADPARAM;
   }

   if (ADSP_FAILED(result) || (NULL == (pMe->capiContainer[capi_index]->capi_ptr)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cannot create decoder CAPI at %u or no memory. result %d", capi_index, result);
      return result;
   }

   //store the capi destroy function
   pMe->capiContainer[capi_index]->dec_destroy_fn = capi_init_param.dec_destroy_fn;
   pMe->capiContainer[capi_index]->amdb_handle = capi_init_param.amdb_handle;

   init_time_get_params.input_port_index = capi_init_param.input_port_index;
   init_time_get_params.output_port_index = capi_init_param.output_port_index;

   result = audio_dec_svc_get_init_time_properties(pMe->capiContainer[capi_index]->capi_ptr, &init_time_get_params, pMe->capiContainer[capi_index]);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioDecSvc error in getting properties");
      return result;
   }

   //keep the largest stack size needed.
   if (init_time_get_params.stack_size.size_in_bytes > dec_params->max_stack_size)
   {
      dec_params->max_stack_size = init_time_get_params.stack_size.size_in_bytes;
   }

   //Capi internal input buffer creation.
   //first capi's internal input buffer will be created while creating input stream.
   //Because input port index information is not present here.
   //For rest of the capis internal input buffer will be create here.
   if(capi_index != 0) //If not first Capi.
   {
	   result = AudioDecSvc_CreateInternalInputBufs(pMe, pMe->capiContainer[capi_index],
	                  init_time_get_params.in_port_thresh.threshold_in_bytes, init_time_get_params.input_port_index);
   }

   //Capi internal output buffer creation.
   //last capi's internal output buffers(external buffers) will be created while creating the output stream.
   //Because output port index information is not present here.
   //For rest of the capis internal output buffer will be create here.
   if (!is_last_capi) //If not last Capi.
   {
      // CAPI V2 is assumed to return proper size for 16 and 32 bit outputs.
      // multiple capi case : need to allocate output and scratch buffer.
      result |= AudioDecSvc_CreateInternalOutputBufs(pMe, pMe->capiContainer[capi_index],
    		  init_time_get_params.out_port_thresh.threshold_in_bytes, init_time_get_params.output_port_index);
      result |= AudioDecSvc_CheckCreateScratchBufs(pMe, pMe->capiContainer[capi_index], init_time_get_params.out_port_thresh.threshold_in_bytes);
   }

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioDecSvc error in creating buffers");
      return ADSP_EFAILED;
   }
   return result;
}

static bool_t AudioDecSvc_HasMediaFmtChanged(DecPcmFmt_t *a, DecPcmFmt_t *b)
{
   uint32_t i = 0;

   if ( (a->endianness != b->endianness) ||
         (a->ulPCMSampleRate != b->ulPCMSampleRate) ||
         (a->usBitsPerSample != b->usBitsPerSample) ||
         (a->usIsInterleaved != b->usIsInterleaved) ||
         (a->chan_map.nChannels != b->chan_map.nChannels)
         )
   {
      return TRUE;
   }
   for (i = 0; i < sizeof(a->chan_map.nChannelMap)/sizeof(a->chan_map.nChannelMap[0]); i++)
   {
      if (a->chan_map.nChannelMap[i] != b->chan_map.nChannelMap[i])
      {
         return TRUE;
      }
   }
   return FALSE;
}

ADSPResult AudioDecSvc_CopyPcmOr61937MediaFmt(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, void *pFmtBlk )
{
   asm_multi_channel_pcm_fmt_blk_v2_t* pPcmFmtBlk_v2 = (asm_multi_channel_pcm_fmt_blk_v2_t*) ( pFmtBlk );

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "PCM/61937 Media Format 0x%lx. ch=%u,SR=%lu,bps=%u",pMe->capiContainer[0]->dec_fmt_id,
         pPcmFmtBlk_v2->num_channels,
         pPcmFmtBlk_v2->sample_rate,
         pPcmFmtBlk_v2->bits_per_sample);

   MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "PCM/61937 Media Format channel mapping %u,%u,%u,%u,%u,%u,%u,%u",pPcmFmtBlk_v2->channel_mapping[0],
         pPcmFmtBlk_v2->channel_mapping[1],pPcmFmtBlk_v2->channel_mapping[2],
         pPcmFmtBlk_v2->channel_mapping[3],pPcmFmtBlk_v2->channel_mapping[4],
         pPcmFmtBlk_v2->channel_mapping[5],pPcmFmtBlk_v2->channel_mapping[6],
         pPcmFmtBlk_v2->channel_mapping[7]);

   //Check and populate the common parameters of v2, v3 and v4
   if(((!pPcmFmtBlk_v2->is_signed) || (pPcmFmtBlk_v2->num_channels == 0) || (pPcmFmtBlk_v2->num_channels > PCM_FORMAT_MAX_NUM_CHANNEL)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Received invalid parameters in PCM media format");
      return ADSP_EBADPARAM;
   }

   if ((0 == pPcmFmtBlk_v2->sample_rate) || (PCM_FORMAT_MAX_SAMPLING_RATE_HZ < pPcmFmtBlk_v2->sample_rate))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Aud dec rcvd invalid sample_rate [%lu]Hz.",
      		pPcmFmtBlk_v2->sample_rate);
      return ADSP_EBADPARAM;
   }

   DecPcmFmt_t prev_fmt = pInpStrm->pcmFmt;

   pInpStrm->pcmFmt.endianness = DEC_LITTLE_ENDIAN;

   if (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 == pMe->capiContainer[0]->dec_fmt_id) //PCM supports only one CAPI
   {
      if((16 == pPcmFmtBlk_v2->bits_per_sample) || (24 == pPcmFmtBlk_v2->bits_per_sample))
      {
         pInpStrm->pcmFmt.usBitsPerSample = pPcmFmtBlk_v2->bits_per_sample;
         pInpStrm->pcmFmt.usBytesPerSample = (pInpStrm->pcmFmt.usBitsPerSample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Aud dec rcvd invalid bits per sample_[%lu].",
               pPcmFmtBlk_v2->bits_per_sample);
         return ADSP_EBADPARAM;
      }
   }
   else if (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 == pMe->capiContainer[0]->dec_fmt_id)
   {
      asm_multi_channel_pcm_fmt_blk_v3_t* pPcmFmtBlk_v3 = (asm_multi_channel_pcm_fmt_blk_v3_t*) ( pFmtBlk );
      if(((16 == pPcmFmtBlk_v3->bits_per_sample) || (24 == pPcmFmtBlk_v3->bits_per_sample)) &&
            (pPcmFmtBlk_v3->sample_word_size >= pPcmFmtBlk_v3->bits_per_sample))
      {
         uint32_t bytesPerSample = pPcmFmtBlk_v3->sample_word_size/8;
         if ( (bytesPerSample*8 != pPcmFmtBlk_v3->sample_word_size) ||
               (bytesPerSample > 4))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "sample word size %u not byte aligned or is > 32.",
                  pPcmFmtBlk_v3->sample_word_size);
            return ADSP_EBADPARAM;
         }

         pInpStrm->pcmFmt.usBytesPerSample = bytesPerSample;
         pInpStrm->pcmFmt.usBitsPerSample = pPcmFmtBlk_v3->bits_per_sample;
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sample word size is %u ", pPcmFmtBlk_v3->sample_word_size);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Aud dec rcvd invalid bits per sample_[%lu], or sample word size [%lu].",
               pPcmFmtBlk_v3->bits_per_sample, pPcmFmtBlk_v3->sample_word_size);
         return ADSP_EBADPARAM;
      }
   }
   else if (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 == pMe->capiContainer[0]->dec_fmt_id)
   {
      asm_multi_channel_pcm_fmt_blk_v4_t* pPcmFmtBlk_v4 = (asm_multi_channel_pcm_fmt_blk_v4_t*) ( pFmtBlk );
      if(((16 == pPcmFmtBlk_v4->bits_per_sample) || (24 == pPcmFmtBlk_v4->bits_per_sample) || (32 == pPcmFmtBlk_v4->bits_per_sample)) &&
            (pPcmFmtBlk_v4->sample_word_size >= pPcmFmtBlk_v4->bits_per_sample))
      {
         uint32_t bytesPerSample = pPcmFmtBlk_v4->sample_word_size/8;
         if ( (bytesPerSample*8 != pPcmFmtBlk_v4->sample_word_size) || (bytesPerSample > 4))
		 {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "sample word size %u not byte aligned or is > 32.", pPcmFmtBlk_v4->sample_word_size);
			return ADSP_EBADPARAM;
         }
         pInpStrm->pcmFmt.usBytesPerSample = bytesPerSample;
         pInpStrm->pcmFmt.usBitsPerSample = pPcmFmtBlk_v4->bits_per_sample;

         if((pPcmFmtBlk_v4->endianness != 0) && (pPcmFmtBlk_v4->endianness != 1))
		 {
		    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "endianness type %u not supported.", pPcmFmtBlk_v4->endianness);
			return ADSP_EBADPARAM;
		 }

		 pInpStrm->pcmFmt.endianness = (pPcmFmtBlk_v4->endianness == 0) ? DEC_LITTLE_ENDIAN : DEC_BIG_ENDIAN;
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sample word size is %u, Endianness type is %u (0-little, 1-big)",
				pPcmFmtBlk_v4->sample_word_size, pPcmFmtBlk_v4->endianness);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Aud dec rcvd invalid bits per sample_[%lu], or sample word size [%lu].",
               pPcmFmtBlk_v4->bits_per_sample, pPcmFmtBlk_v4->sample_word_size);
         return ADSP_EBADPARAM;
      }
   }
   else //61937
   {
      pInpStrm->pcmFmt.usBytesPerSample = BYTES_PER_SAMPLE_TWO;
      pInpStrm->pcmFmt.usBitsPerSample = BYTES_PER_SAMPLE_TWO<<3;
   }

   //store PCM parameters
   pInpStrm->pcmFmt.chan_map.nChannels = pPcmFmtBlk_v2->num_channels;
   pInpStrm->pcmFmt.ulPCMSampleRate = pPcmFmtBlk_v2->sample_rate;
   pInpStrm->pcmFmt.usIsInterleaved = TRUE;

   memset(pInpStrm->pcmFmt.chan_map.nChannelMap, 0, PCM_FORMAT_MAX_NUM_CHANNEL);

   memscpy(pInpStrm->pcmFmt.chan_map.nChannelMap, sizeof(pInpStrm->pcmFmt.chan_map.nChannelMap),
         pPcmFmtBlk_v2->channel_mapping, pPcmFmtBlk_v2->num_channels);


   //update total_bytes, such that duration remains constant.
   if (AudioDecSvc_HasMediaFmtChanged(&pInpStrm->pcmFmt, &prev_fmt) &&
         (pInpStrm->common.total_bytes != 0) &&
         (prev_fmt.ulPCMSampleRate != 0) &&
         (prev_fmt.chan_map.nChannels != 0) &&
         (prev_fmt.usBytesPerSample != 0) )
   {
      pInpStrm->common.total_bytes = pInpStrm->common.total_bytes *
               pInpStrm->pcmFmt.ulPCMSampleRate / prev_fmt.ulPCMSampleRate *
               pInpStrm->pcmFmt.chan_map.nChannels / prev_fmt.chan_map.nChannels *
               pInpStrm->pcmFmt.usBytesPerSample / prev_fmt.usBytesPerSample;
   }

   return ADSP_EOK;
}

ADSPResult AudioDecSvc_UpdateMediaFmt_PeerService(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult result;

   //get media format update cmd payload
   //get the mediaFormat structure
   elite_msg_data_media_type_apr_t *pMediaFmt =
         (elite_msg_data_media_type_apr_t*) (pInpStrm->inpDataQMsg.pPayload);

   /* sanity checks */
   if ( ELITEMSG_DATA_MEDIA_TYPE_APR != pMediaFmt->unMediaTypeFormat )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Unsupported MEDIA FORMAT 0x%8lx",(pMediaFmt->unMediaTypeFormat));
      return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EUNSUPPORTED);
   }

   switch(pMediaFmt->unMediaFormatID)
   {
   case ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM :
   {
      asm_multi_channel_pcm_fmt_blk_v3_t pcmBlk;

      elite_multi_channel_pcm_fmt_blk_t *pInPcmFmtBlk =
            (elite_multi_channel_pcm_fmt_blk_t*) elite_msg_get_media_fmt_blk(pMediaFmt);

      pcmBlk.bits_per_sample = pInPcmFmtBlk->bits_per_sample;
      pcmBlk.is_signed = pInPcmFmtBlk->is_signed;
      pcmBlk.num_channels = pInPcmFmtBlk->num_channels;
      pcmBlk.sample_rate = pInPcmFmtBlk->sample_rate;
      pcmBlk.sample_word_size = (pcmBlk.bits_per_sample == 16) ? 16 : 32; //internally it's always 16 or 32.

      memscpy(pcmBlk.channel_mapping, sizeof(pcmBlk.channel_mapping), pInPcmFmtBlk->channel_mapping, sizeof(pcmBlk.channel_mapping));

      result = AudioDecSvc_CopyPcmOr61937MediaFmt(pMe, pInpStrm, (void*)&pcmBlk);
      if (ADSP_SUCCEEDED(result))
      {
         result = AudioDecSvc_RecreatePcmBufsBasedOnMediaFmt(pMe, pInpStrm);
      }

      break;
   }
   case ELITEMSG_MEDIA_FMT_COMPRESSED:
   {
      //use this struct intermediately
      asm_multi_channel_pcm_fmt_blk_v3_t pcmBlk;

      elite_compressed_fmt_blk_t *pComprFmtBlk = (elite_compressed_fmt_blk_t*) elite_msg_get_media_fmt_blk(pMediaFmt);
      pcmBlk.bits_per_sample = pComprFmtBlk->bits_per_sample;
      pcmBlk.is_signed = TRUE; //to be ignored.
      pcmBlk.num_channels = pComprFmtBlk->num_channels;
      pcmBlk.sample_rate = pComprFmtBlk->sample_rate;
      pcmBlk.sample_word_size = (pcmBlk.bits_per_sample == 16) ? 16 : 32; //internally it's always 16 or 32.
      //pComprFmtBlk->media_format; //decoder acts as decoder OR depacketizer -> decoder OR depacketizer -> converter -> packetizer. This media fmt is not required.

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Compressed media fmt message: fmt = %lx",(pComprFmtBlk->media_format));

      result = AudioDecSvc_CopyPcmOr61937MediaFmt(pMe, pInpStrm, (void*)&pcmBlk);
      if (ADSP_SUCCEEDED(result))
      {
         result = AudioDecSvc_RecreatePcmBufsBasedOnMediaFmt(pMe, pInpStrm);
      }

      break;
   }
   default:
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Unsupported MEDIA FORMAT ID 0x%8lx",(pMediaFmt->unMediaFormatID));
      result = ADSP_EUNSUPPORTED;
   }
   }

   AudioDecSvc_FreeInputDataCmd(pMe,pInpStrm, result);

   return result;
}

/**
 * note that this function is called on first capi for data path media-fmt
 * for set-param of media-fmt (as in split-A2DP), main capi receives this.
 */
ADSPResult AudioDecSvc_UpdateMediaFmt(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, dec_CAPI_container_t *capi_cont, uint8_t *pFmtBlk, uint32_t ulFmtBlkSize)
{
   ADSPResult result = ADSP_EOK;

   uint16_t input_port_index = AudioDecSvc_GetInputStreamIndex(pMe, pInpStrm);

   switch( capi_cont->dec_fmt_id )
   {
   case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2:
   case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3:
   case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4:
   {
      if (ADSP_FAILED(result = AudioDecSvc_CopyPcmOr61937MediaFmt(pMe, pInpStrm, (void*)pFmtBlk)))
      {
         return result;
      }
      result = AudioDecSvc_RecreatePcmBufsBasedOnMediaFmt(pMe, pInpStrm);
      break;
   }
   default:
   {
      result = aud_dec_svc_set_in_media_fmt_raw_compr(capi_cont->capi_ptr, input_port_index,
            pFmtBlk, ulFmtBlkSize, capi_cont->dec_fmt_id,
            (int8_t*)capi_cont->scratch_out_buf,
            capi_cont->scratch_buf_size);
   }
   }

   //TODO: to remove special condition
   if (capi_cont->dec_fmt_id == ASM_MEDIA_FMT_ADPCM)
   {
      pInpStrm->pcmFmt.usIsInterleaved = 1;
   }

   if (ADSP_SUCCEEDED(result))
   {
      result = AudioDecSvc_HandlePortDataThreshChangeEvent(pMe);
   }

   return result;
}

void AudioDecSvc_UpdatePrevFmtBlk(AudioDecSvc_t *pMe,
      AudioDecSvc_InpStream_t *pInpStrm,
      dec_CAPI_container_t *capi_cont,
      int32_t nSampleRateAfter,
      int32_t nChannelsAfter,
      uint8_t *ucChannelMappingAfter,
      uint16_t bits_per_sample,
      uint8_t capi_out_index)
{
   capi_cont->PrevFmtBlk[capi_out_index].num_channels   = nChannelsAfter;
   capi_cont->PrevFmtBlk[capi_out_index].bits_per_sample = bits_per_sample;
   capi_cont->PrevFmtBlk[capi_out_index].sample_rate = nSampleRateAfter;
   capi_cont->PrevFmtBlk[capi_out_index].is_signed = 1;
   capi_cont->PrevFmtBlk[capi_out_index].is_interleaved = AUD_DEC_DEINTERLEAVED;
   if( ASM_MEDIA_FMT_ADPCM == capi_cont->dec_fmt_id )

   {
      //for adpcm format, dec doesnt do any de-interleaving,
      //so handle this as a special case
      capi_cont->PrevFmtBlk[capi_out_index].is_interleaved = pInpStrm->pcmFmt.usIsInterleaved;
   }

   memscpy(capi_cont->PrevFmtBlk[capi_out_index].channel_mapping, sizeof(capi_cont->PrevFmtBlk[capi_out_index].channel_mapping), ucChannelMappingAfter, nChannelsAfter);
}

ADSPResult AudioDecSvc_GetMediaFmtUpdateMsg(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm,
      int32_t nSampleRateAfter,
      int32_t nChannelsAfter,
      uint8_t *ucChannelMappingAfter,
      uint16_t output_bits_per_sample,
      data_format_t out_data_fmt,
      uint32_t fmt_id,
      elite_msg_any_t *msg
      )
{

   ADSPResult nResult = ADSP_EOK;

   uint32_t size = sizeof(elite_multi_channel_pcm_fmt_blk_t);
   if (out_data_fmt == CAPI_V2_IEC61937_PACKETIZED)
   {
      size = sizeof(elite_compressed_fmt_blk_t);
   }

   uint32_t nPayloadSize = sizeof(elite_msg_data_media_type_apr_t) + size;
   if ( ADSP_FAILED( elite_msg_create_msg(msg, &nPayloadSize,
         ELITE_DATA_MEDIA_TYPE,
         NULL, 0,0 ) ))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to create media fmt message");
      return ADSP_EFAILED;
   }

   elite_msg_data_media_type_apr_t *pCmdMsgPayload = (elite_msg_data_media_type_apr_t*) (msg->pPayload);
   pCmdMsgPayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;

   if (out_data_fmt == CAPI_V2_IEC61937_PACKETIZED)
   {
      pCmdMsgPayload->unMediaFormatID   = ELITEMSG_MEDIA_FMT_COMPRESSED;

      elite_compressed_fmt_blk_t *pFormatBlock =
            (elite_compressed_fmt_blk_t*) ((uint8_t*)(pCmdMsgPayload)+sizeof(elite_msg_data_media_type_apr_t));
      pFormatBlock->num_channels = nChannelsAfter;
      pFormatBlock->bits_per_sample = output_bits_per_sample;
      pFormatBlock->sample_rate = nSampleRateAfter;
      pFormatBlock->media_format = fmt_id;
      
      MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "Reconfig media format: FMTID 0x%lx, CH %d, BPS %d, SR %lu, fmt_id 0x%lx",
         pCmdMsgPayload->unMediaFormatID,pFormatBlock->num_channels,
         pFormatBlock->bits_per_sample,pFormatBlock->sample_rate, fmt_id);  
         
   }
   else
   {
      pCmdMsgPayload->unMediaFormatID   = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

      elite_multi_channel_pcm_fmt_blk_t *pPcmFormatBlock =
            (elite_multi_channel_pcm_fmt_blk_t*) ((uint8_t*)(pCmdMsgPayload)+sizeof(elite_msg_data_media_type_apr_t));
      pPcmFormatBlock->num_channels = nChannelsAfter;

      pPcmFormatBlock->bits_per_sample = output_bits_per_sample;
      pPcmFormatBlock->sample_rate = nSampleRateAfter;
      pPcmFormatBlock->is_signed = 1;
      pPcmFormatBlock->is_interleaved = AUD_DEC_DEINTERLEAVED;

      dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
      if(NULL == last_capi)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:Failed to get last capi");
         return ADSP_EFAILED;
      }

      if( ASM_MEDIA_FMT_ADPCM == last_capi->dec_fmt_id )
      {
         //for pcm format, pcm dec doesnt do any de-interleaving,
         //same is the case with adpcm decoding
         //so handle this as a special case
         pPcmFormatBlock->is_interleaved = pInpStrm->pcmFmt.usIsInterleaved;
      }     
         /*******************************************************************/
      /***MULTI-CHANNEL-TO-DO: CAPI shall provide appropriate channel mapping****/
      /** For now, work around to provide necessary information: */
      /*   Mono: center */
      /* Stero: Left, right */
      /* 5.1: Left, Right, Center, LFE, LS, RS */
      /***************************************************************/
      memscpy( pPcmFormatBlock->channel_mapping, sizeof(pPcmFormatBlock->channel_mapping), ucChannelMappingAfter, nChannelsAfter);
      
      MSG_7(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "Reconfig media format: FMTID 0x%lx, CH %d, BPS %d, SR %lu, isSigned %d, isInterleaved %d, Result %d",
         pCmdMsgPayload->unMediaFormatID,pPcmFormatBlock->num_channels,
         pPcmFormatBlock->bits_per_sample,pPcmFormatBlock->sample_rate,
         pPcmFormatBlock->is_signed,pPcmFormatBlock->is_interleaved,nResult);      
   }

   AudioDecSvc_DetermineSvcBw(pMe, nSampleRateAfter, nChannelsAfter, output_bits_per_sample, &pInpStrm->pcmFmt);
   //force vote since svc vote might have changed.
   nResult = AudioDecSvc_ProcessKppsBw(pMe, FALSE, TRUE);

   return nResult;
}

ADSPResult AudioDecSvc_SendMediaFmtUpdate(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm, elite_msg_any_t *msg)
{
   ADSPResult nResult = ADSP_EOK;

   if (NULL == msg)
   {
      return nResult;
   }

   //even though zero buf is sent later, delay is updated now.
   if (pMe->is_rt_dec)
   {
      AudioDecSvc_UpdateInitialDelay(pMe, pOutStrm);
   }

   if(NULL != pMe->pUll_modules)
   {
      uint32_t num_out_buf_unit_frames = (ASM_LEGACY_STREAM_SESSION == pOutStrm->common.perf_mode)? (BUF_SIZE_REG_LATENCY) : (BUF_SIZE_LOW_LATENCY);
      audio_module_chain_wrapper_set_media_fmt(pMe->pUll_modules, msg, num_out_buf_unit_frames, pMe->capiContainer[0]->dec_fmt_id);
      AudioDecSvc_ProcessKppsBw(pMe, FALSE, TRUE);
   }
   nResult = qurt_elite_queue_push_back(pOutStrm->pDownStreamSvc->dataQ, (uint64_t*)msg );

   if (ADSP_FAILED(nResult))
   {
      elite_msg_release_msg(msg);
   }
   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "Sent media fmt update");

   return nResult;
}

ADSPResult AudioDecSvc_NotifyPeerSvcWithMediaFmtUpdate (AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm,
      int32_t nSampleRateAfter,
      int32_t nChannelsAfter,
      uint8_t *ucChannelMappingAfter,
      uint16_t output_bits_per_sample)
{
   elite_msg_any_t msg;
   ADSPResult nResult=ADSP_EOK;

   dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
   if (last_capi)
   {
      nResult = AudioDecSvc_GetMediaFmtUpdateMsg(pMe, pInpStrm, pOutStrm,
            nSampleRateAfter,
            nChannelsAfter,
            ucChannelMappingAfter,
            output_bits_per_sample,
            last_capi->out_data_fmt_type[0],
            last_capi->dec_fmt_id,
            &msg
            );

      if (ADSP_SUCCEEDED(nResult) )
      {
         nResult = AudioDecSvc_SendMediaFmtUpdate(pMe, pOutStrm, &msg);
      }
   }

   return nResult;
}

/*
 *Function Name: AudioDecSvc_SendSrCmEvent
 *
 *Parameters: pMe: Decoder Service state structure pointer
 *            lSamplerate: new sample rate
 *            lNumChannels: new # of channels
 *
 *Description: Sends SR/CM change notification event (if enabled)
 *
 *Returns: ADSP_EOK/ADSP_EBADPARAM
 *
 */
ADSPResult AudioDecSvc_SendSrCmEvent(AudioDecSvc_t *pMe,AudioDecSvc_InpStream_t *pInpStrm,
      uint32_t sample_rate,
      DecChannelMap_t *chan_map)
{
   //raise event only if it is enabled
   if(!pInpStrm->bEnableSrCmEvent)
   {
      return ADSP_EOK;
   }

   //form the payload for the event
   asm_data_event_sr_cm_change_notify_t SrCmEvent;
   SrCmEvent.sample_rate = sample_rate;
   SrCmEvent.num_channels = (uint16_t) chan_map->nChannels;
   SrCmEvent.reserved = 0;

   memscpy(SrCmEvent.channel_mapping, sizeof(SrCmEvent.channel_mapping), chan_map->nChannelMap, chan_map->nChannels);

   return AudioStreamMgr_GenerateClientEventFromCb(&pInpStrm->common.CallbackHandle,
         ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY,
         pInpStrm->common.CallbackHandle.unAsynClientToken,
         &SrCmEvent,
         sizeof(SrCmEvent));
}



ADSPResult AudioDecSvc_GetInputDataCmd(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult result;
   uint32_t uASMOpCode, uAprOpCode;
   //check if we already hold on to an input data cmd..
   //if so return error
   if(pInpStrm->inpDataQMsg.pPayload)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Already holding on to an input data command. \
                                          Cannot get another command until this is freed");
      return ADSP_EUNEXPECTED;
   }

   result = qurt_elite_queue_pop_front(pInpStrm->common.serviceHandle.dataQ, (uint64_t*)&(pInpStrm->inpDataQMsg) );
   if(ADSP_EOK != result)
   {
      pInpStrm->inpDataQMsg.pPayload = NULL;
      return result;
   }

   uASMOpCode = pInpStrm->inpDataQMsg.unOpCode;

   if (ELITE_APR_PACKET == uASMOpCode)
   {
      elite_apr_packet_t* pAprPacket = (elite_apr_packet_t*) (pInpStrm->inpDataQMsg.pPayload);
      uAprOpCode = elite_apr_if_get_opcode( pAprPacket ) ;
      if (ASM_DATA_CMD_WRITE_V2 == uAprOpCode)
      {
         pInpStrm->buf_recv_cnt++;
      }
   }
   else //assume opcode is one of the elite data messages
   {
      if (ELITE_DATA_BUFFER == pInpStrm->inpDataQMsg.unOpCode)
      {
         pInpStrm->buf_recv_cnt++;
      }
   }

   return result;
}

ADSPResult AudioDecSvc_FreeInputDataCmd(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, ADSPResult status)
{
   if(!pInpStrm->inpDataQMsg.pPayload)
   {
      return ADSP_EOK;
   }
   ADSPResult res = ADSP_EOK;
   if(ELITE_APR_PACKET == pInpStrm->inpDataQMsg.unOpCode)
   {
      elite_apr_packet_t *pAprPacket = ( elite_apr_packet_t *) (pInpStrm->inpDataQMsg.pPayload);

      if (ASM_DATA_CMD_WRITE_V2 == pAprPacket->opcode) {
         pInpStrm->buf_done_cnt++;
         //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "buf_done_cnt is %d",pMe->buf_done_cnt);
      }

      if(ASM_DATA_CMD_EOS == pAprPacket->opcode ||
            ASM_DATA_CMD_EOS_V2 == pAprPacket->opcode ||
            ASM_DATA_CMD_MARK_BUFFER_V2 == pAprPacket->opcode ||
            ASM_DATA_CMD_RESYNC == pAprPacket->opcode)
      {


         //this means that we have not processed the EOS command.
         //cant send CMDRSP_EOS since this is not successfully handled
         //so just discard the EOS cmd
         res = AudioStreamMgr_FreeAprPkt(pAprPacket);
      }
      else
      {
         if (0 != pInpStrm->inp_buf_params_backup.unVirtAddr)
         {
            //Invalidating the cache before sending the ACK
            elite_mem_invalidate_cache( &(pInpStrm->inp_buf_params_backup) );

            elite_mem_map_release_shm(&(pInpStrm->inp_buf_params_backup));

            memset(&(pInpStrm->inp_buf_params_backup), 0, sizeof(elite_mem_shared_memory_map_t));
         }

         res = AudioStreamMgr_GenerateAck(pAprPacket, status, NULL, 0,0 );

         //reset input buffer params
         pInpStrm->inp_buf_params.unMemSize = 0;
         pInpStrm->inp_buf_params.unPhysAddrLsw = 0;
         pInpStrm->inp_buf_params.unPhysAddrMsw = 0;
         pInpStrm->inp_buf_params.unVirtAddr = 0;
      }

   }
   else
   {
      if (ELITE_DATA_BUFFER == pInpStrm->inpDataQMsg.unOpCode)
      {
         pInpStrm->buf_done_cnt++;
      }

      res = elite_msg_finish_msg(&pInpStrm->inpDataQMsg, status);
   }

   //set payload to NULL to indicate we are not holding on to any input data msg
   pInpStrm->inpDataQMsg.pPayload = NULL;

   return res;
}

ADSPResult AudioDecSvc_FlushInputDataQ(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   do
   {
      AudioDecSvc_DiscardMarkBuffer(pMe, pInpStrm);
      //first free up any data q msgs that we are already holding
      AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);

      // Drain any queued buffers while there are input data messages.
      AudioDecSvc_GetInputDataCmd(pMe, pInpStrm);

   } while (pInpStrm->inpDataQMsg.pPayload);

   return ADSP_EOK;
}

ADSPResult AudioDecSvc_ResetDecoder(AudioDecSvc_t *pMe)
{
   pMe->need_to_send_eos = FALSE;

   //reset internal buffer params
   for (uint8_t i=0; i<DEC_SVC_MAX_CAPI; i++)
   {
      if (pMe->capiContainer[i])
      {
         for (uint8_t j=0; j<DEC_SVC_MAX_INPUT_STREAMS;j++)
         {
            if (pMe->capiContainer[i]->in_buf[j].data_ptr)
            {
               pMe->capiContainer[i]->in_buf[j].actual_data_len = 0;
               pMe->capiContainer[i]->inputs[j].flags.marker_eos = false;
               pMe->capiContainer[i]->inputs[j].flags.is_timestamp_valid = false;
               pMe->capiContainer[i]->inputs[j].flags.end_of_frame = false;
               //memset(pMe->capiContainer[i]->in_buf[j].data_ptr, 0, pMe->capiContainer[i]->in_buf[j].max_data_len);
            }

            //reset decoder CAPI input port.
            if (pMe->capiContainer[i]->capi_ptr)
            {
               aud_dec_svc_algorithmic_reset(pMe->capiContainer[i]->capi_ptr, j);
            }
         }

         for (uint8_t j=0; j<DEC_SVC_MAX_OUTPUT_STREAMS;j++)
         {
            if (pMe->capiContainer[i]->out_buf[j].data_ptr)
            {
               pMe->capiContainer[i]->outputs[j].flags.marker_eos = false;
               pMe->capiContainer[i]->outputs[j].flags.is_timestamp_valid = false;
               pMe->capiContainer[i]->outputs[j].flags.end_of_frame = false;
               pMe->capiContainer[i]->out_buf[j].actual_data_len = 0;
            }
         }

         pMe->capiContainer[i]->bytes_logged = 0;
      }
   }

   for (uint16_t i=0; i<DEC_SVC_MAX_INPUT_STREAMS; i++)
   {
      if (pMe->in_streams_ptr[i])
      {
         //increment log id so that after every reset(flush,EoS), fresh log file is created.
    	 pMe->in_streams_ptr[i]->common.ulDataLogId++;

         pMe->in_streams_ptr[i]->endOfStream = AUD_DEC_EOS_NONE;

         AudioDecSvc_DecErrorEvent_Reset(&pMe->in_streams_ptr[i]->dec_err_event);

         //reset timestamp state
         (void) AudioDecSvc_InitTsState(&pMe->in_streams_ptr[i]->TsState);

         AudioDecSvc_ResetPullMode(pMe, pMe->in_streams_ptr[i]);

         pMe->in_streams_ptr[i]->WasPrevDecResNeedMore = false;
      }
   }

   for (uint16_t i=0; i<DEC_SVC_MAX_OUTPUT_STREAMS; i++)
   {
      if (pMe->out_streams_ptr[i])
      {
         //if holding a buf, drop it, return to queue.
         if (pMe->out_streams_ptr[i]->outDataBufferNode.pBuffer)
         {
            AudioDecSvc_ReturnOutBuf(pMe, pMe->out_streams_ptr[i]);
         }

         memset(&pMe->out_streams_ptr[i]->metadata_xfr, 0, sizeof(dec_metadata_xfer_t));

         if (pMe->out_streams_ptr[i]->early_eos_ptr)
         {
            AudioDecSvc_EarlyEoS_Reset(pMe, pMe->out_streams_ptr[i]);
         }
         else
         {
            AudioDecSvc_CircBuf_Flush(&pMe->out_streams_ptr[i]->CircBufList, pMe, pMe->out_streams_ptr[i]);
         }
      }
   }

   return ADSP_EOK;
}
ADSPResult AudioDecSvc_DiscardMarkBuffer(AudioDecSvc_t* pMe,AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult nResult = ADSP_EOK;
   uint32_t uAprOpCode;
   elite_apr_packet_t* pAprPacket;
   pAprPacket = (elite_apr_packet_t*) (pInpStrm->inpDataQMsg.pPayload);
   if(NULL == pInpStrm->inpDataQMsg.pPayload)
   {
      return ADSP_EOK;
   }

   uAprOpCode = elite_apr_if_get_opcode( pAprPacket ) ;
   if( uAprOpCode == ASM_DATA_CMD_MARK_BUFFER_V2)
   {
      asm_data_cmd_mark_buffer_v2_t *apr_payload;
      elite_apr_if_get_payload((void **)&apr_payload, pAprPacket);
      asm_data_event_mark_buffer_v2_t event;
      event.token_lsw = apr_payload->token_lsw;
      event.token_msw = apr_payload->token_msw;
      event.result = ASM_DATA_EVENT_MARK_BUFFER_DISCARDED;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc discarding mark buffer %lu",event.token_lsw);
      nResult = AudioStreamMgr_GenerateClientEventFromCb(&pInpStrm->common.CallbackHandle,
            ASM_DATA_EVENT_MARK_BUFFER_V2,
            pInpStrm->common.CallbackHandle.unAsynClientToken,
            &event,
            sizeof(asm_data_event_mark_buffer_v2_t));
   }
   return nResult;
}

//#define AUDENCDEC_TIMESTAMP_LOGS

ADSPResult AudioDecSvc_PopulateOutBuf(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm )
{
   ADSPResult result = ADSP_EOK;

   elite_msg_data_buffer_t *pOutBuf = (elite_msg_data_buffer_t*) pOutStrm->outDataBufferNode.pBuffer;

   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe);

   // send the output buffer downstream
   pOutBuf->nFlag                = 0;
   pOutBuf->nOffset              = 0;
   //don't change max size.
   pOutBuf->pResponseQ           = NULL;
   pOutBuf->unClientToken        = 0;
   pOutBuf->unResponseResult     = 0;
   pOutBuf->nMaxSize             = pOutStrm->maxExtBufSize;

   //put timestamp in output buffer
   asm_set_timestamp_valid_flag(&pOutBuf->nFlag, pInpStrm->TsState.bNextOutbufTSValid);
   pOutBuf->ullTimeStamp = pInpStrm->TsState.ullNextOutBufTS;

#ifdef AUDENCDEC_TIMESTAMP_LOGS
   MSG_4(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "outgoing buffer TS: %lu %lu, flag=%lx, size=%ld",
         (uint32_t)(pOutBuf->ullTimeStamp>>32), (uint32_t)pOutBuf->ullTimeStamp, pOutBuf->nFlag, pOutBuf->nActualSize);
#endif
   //update next output time stamp
   dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
   if(NULL == last_capi)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:Failed to get last capi");
      return ADSP_EFAILED;
   }

   pOutBuf->unClientToken = AudioDecSvc_AddBufDelayAndGetClientToken(pMe, pOutStrm, pOutBuf->nActualSize, &(last_capi->PrevFmtBlk[0]));

   //TODO: since this function is called for each out stream, pInpStrm shouldn't be used here
   if(!pMe->is_rt_dec)
   {
      //For rt dec the TS should be derived from the input buffer TS. No need to increment/sync after generating output.
      (void)AudioDecSvc_UpdateNextOutbufTS(pOutBuf->nActualSize, &last_capi->PrevFmtBlk[0], &pInpStrm->TsState,
            pMe->mainCapiContainer->dec_fmt_id); //always we use dec fmt of main dec here.
   }

   return result;
}

ADSPResult AudioDecSvc_SendPcmToPeerSvc (AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, elite_msg_data_buffer_t *pOutBuf )
{
   ADSPResult nResult = ADSP_EOK;

   dec_CAPI_container_t * last_capi = AudioDecSvc_GetLastCapi(pMe);

#ifdef AUDENCDEC_TIMESTAMP_LOGS
   elite_msg_data_buffer_t* pBuf = (elite_msg_data_buffer_t*) pOutBuf;

   MSG_3(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "AudioDecSvc_SendPcmToPeerSvc: TS: %lu %lu. size %lu",
         (uint32_t)(pBuf->ullTimeStamp>>32), (uint32_t)pBuf->ullTimeStamp,
         (uint32_t)(pBuf->nActualSize));
#endif

   qurt_elite_bufmgr_node_t outBufMgrNode = pOutStrm->outDataBufferNode;
   elite_msg_any_t *pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
         &outBufMgrNode,
         ELITE_DATA_BUFFER,
         NULL, /* do not need response */
         pOutBuf->unClientToken,   /* token : store delay in the token */
         0     /* do not care response result*/
   );

   if(NULL != pMe->pUll_modules)
   {
      audio_module_chain_wrapper_process(pMe->pUll_modules, (uint64_t*)pPeerDataQMsg);
   }
   AudioDecSvc_LogPcmData_Out(pMe, pInpStrm, pOutStrm, (int8_t*) &pOutBuf->nDataBuf, pOutBuf->nActualSize);

   nResult = AudioDecSvc_PushOutBuf(pMe, pOutStrm, (uint64_t*)pPeerDataQMsg);
   //After sending buffers downstream, create empty buffers if downstream delay is not sufficient to handle early-eos.
   //create delay buffers to handle EoS.
   uint32_t delay_from_cur_buf = pOutBuf->unClientToken;

   //check if the last decoder is PCM (this func is for changing output buf of last decoder).
   //for PCM any TS discont etc shouldn't cause buf recreation. Hence just use the std size.
   if (last_capi != NULL)
   {
	   if (AudioDecSvc_IsPcmFmt(last_capi->dec_fmt_id))
	   {
	      delay_from_cur_buf = (pInpStrm->common.perf_mode == ASM_LEGACY_STREAM_SESSION) ? BUF_SIZE_REG_LATENCY: BUF_SIZE_LOW_LATENCY;
	      delay_from_cur_buf = delay_from_cur_buf*1000; // ms -> us.
	   }

   }

   AudioDecSvc_EarlyEoS_BufferUp(pMe, pOutStrm, delay_from_cur_buf);
   pOutStrm->out_buf_cnt++;

   return nResult;
}

/*
 *Function Name: AudioDecSvc_InitTsState
 *
 *Parameters: DecTimeStampState_t *pTsState: Instance of timestamp structure
 *
 *Description: Initialize timestamp parameters
 *
 *Returns: ADSP_EOK/ADSP_EBADPARAM
 *
 */
ADSPResult AudioDecSvc_InitTsState(DecTimeStampState_t *pTsState)
{
   if(!pTsState)
   {
      return ADSP_EBADPARAM;
   }

   pTsState->ullInbufTS = 0;
   pTsState->bInbufTSValid = false;
   pTsState->ullNextOutBufTS = 0;
   pTsState->ullNextOutBufSampleCount = 0;
   pTsState->ullLastSyncedInBufTS = 0;
   pTsState->bNextOutbufTSValid = false;
   pTsState->bNextOutbufTsOld = false;

   return ADSP_EOK;
}

/*
 *Function Name: AudioDecSvc_SyncToInputTS
 *
 *Parameters: DecTimeStampState_t *pTsState: Instance of timestamp structure
 *
 *Description: Set the next output buffer timestamp equal to the input
 *             buffer's timestamp. And mark that we are synced to the
 *             input buffer's timestamp.
 *
 *Returns: None
 *
 */
void AudioDecSvc_SyncToInputTS(DecTimeStampState_t *pTsState)
{
   pTsState->ullLastSyncedInBufTS = pTsState->ullInbufTS;
   pTsState->ullNextOutBufTS = pTsState->ullInbufTS;
   pTsState->ullNextOutBufSampleCount = 0;
   pTsState->bNextOutbufTSValid = pTsState->bInbufTSValid;
   pTsState->bNextOutbufTsOld = false;
   pTsState->bIsTSResync = false;
}

/*
 *Function Name: AudioDecSvc_FillTsStateFromInBuf
 *
 *Parameters: DecTimeStampState_t *pTsState: Instance of timestamp structure
 *             asm_data_cmd_write_v2_t *pInBuf: Pointer to data write cmd
 *
 *Description: Fills time stamp struct from the input data write command
 *
 *Returns: ADSP_EOK/ADSP_EBADPARAM
 *
 * TODO: dec_fmt for this is taken from main decoder, ideally we shouldn't need these TS related functions to depend
 * on time-stamp.
 */
ADSPResult AudioDecSvc_FillTsStateFromInBuf(DecTimeStampState_t *pTsState,
      uint32_t dec_fmt,
      uint64_t ts,
      bool_t ts_valid,
      bool_t ts_continue,
      bool_t eof_flag, bool_t WasPrevDecResNeedMore)
{
   //copy input buffer time stamp info
   pTsState->ullInbufTS = ts;
   pTsState->bInbufTSValid = ts_valid;
   bool_t bTsContinue = ts_continue;

#ifdef AUDENCDEC_TIMESTAMP_LOGS
   MSG_6(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "incoming buffer TS: %lu %lu, valid=%d,continue=%d,EoF=%u,WasPrevDecResNeedMore=%d",
         (uint32_t)(pTsState->ullInbufTS>>32), (uint32_t)pTsState->ullInbufTS, pTsState->bInbufTSValid,bTsContinue, (unsigned int)eof_flag,WasPrevDecResNeedMore);
#endif

   if(!pTsState->bInbufTSValid && bTsContinue)
   {
      // Current buffer TS is invalid and TS continue flag is set to true.
      // so next output buffer TS and TS valid flag
      // continue to hold; just mark that next output
      // buffer TS is not stale and return.
      pTsState->bNextOutbufTsOld = false;
      pTsState->bIsTSResync = false;
      return ADSP_EOK;
   }

   /* if the last decoding attempt did not require more data, it means
   that there are no more bytes left unconsumed in the last input
   buffer, so next output buffer TS should be the same as that of
   the new input buffer.

   If the stream is in resync state, then the new output buffer should
   resync to input.
    */
   if((!WasPrevDecResNeedMore) || (pTsState->bIsTSResync))
   {
      AudioDecSvc_SyncToInputTS(pTsState);
   }
   else
   {
      //we continue to use output time stamp based on prev input buffer
      //so set this flag to true
      pTsState->bNextOutbufTsOld = true;

      if(ASM_MEDIA_FMT_WMA_V9_V2 == dec_fmt ||
            ASM_MEDIA_FMT_WMA_V10PRO_V2 == dec_fmt)
      {
         /* An exception for WMA codecs until ASF packet TS
            are properly understood.
            If the incoming TS is very close (< 2 ms) to
            the next output buffer TS, treat the incoming TS
            as the TS for the next output buffer, i.e.,
            sync to the incoming TS immediately.
            Rather than delay it by one
            output buffer and cause TS discontinuity*/

         int64_t ts_diff = ((int64_t) pTsState->ullNextOutBufTS) - ((int64_t) pTsState->ullInbufTS);

         if(pTsState->bNextOutbufTSValid &&
               (ts_diff < 2000) &&
               (ts_diff > -2000))
         {
            AudioDecSvc_SyncToInputTS(pTsState);
         }
      }
   }

   return ADSP_EOK;
}

/*
 *Function Name: AudioDecSvc_UpdateNextOutbufTS
 *
 *Parameters: uint32_t ulOutbufSize: Output buffer size in bytes
 *            elite_multi_channel_pcm_fmt_blk_t* pOutputFmt : Output format blk
 *            DecTimeStampState_t* pTsState: Timestamp state
 *
 *Description: Updates next outbuf TS by output frame duration
 *
 *Returns: ADSP_EOK/ADSP_EBADPARAM
 *
 * TODO: dec_fmt for this is taken from main decoder, ideally we shouldn't need these TS related functions to depend
 * on dec-fmt.
 *
 */
ADSPResult AudioDecSvc_UpdateNextOutbufTS(uint32_t ulOutbufSize,
      elite_multi_channel_pcm_fmt_blk_t* pOutputFmt,
      DecTimeStampState_t* pTsState,
      uint32_t dec_fmt)
{
   if(!pOutputFmt || !pTsState)
   {
      return ADSP_EBADPARAM;
   }

   uint32_t bytes_per_sample = (pOutputFmt->bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   //increment TS based on # of samples and sample rate
   uint32_t ulNumSampPerCh = (ulOutbufSize) / (pOutputFmt->num_channels * bytes_per_sample);
   uint32_t ulSampleRate = pOutputFmt->sample_rate;

   //get output frame duration in microsec
   pTsState->ullNextOutBufSampleCount += ulNumSampPerCh;

   pTsState->ullNextOutBufTS = pTsState->ullLastSyncedInBufTS +
         ((pTsState->ullNextOutBufSampleCount * (uint64_t)NUM_US_PER_SEC ) / ulSampleRate );

#ifdef AUDENCDEC_TIMESTAMP_LOGS
   MSG_4(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "AudioDecSvc_UpdateNextOutbufTS: ullNextOutBufTS: %lu %lu, "
         "ullNextOutBufSampleCount=%lu,ulNumSampPerCh=%lu",
         (uint32_t)(pTsState->ullNextOutBufTS>>32), (uint32_t)pTsState->ullNextOutBufTS,
         (uint32_t)pTsState->ullNextOutBufSampleCount, ulNumSampPerCh);
#endif


   //if the output TS is based on previous input buffer's TS,
   //update to the current input buffer's TS

   if(pTsState->bNextOutbufTsOld)
   {
      if(ASM_MEDIA_FMT_WMA_V10PRO_V2 != dec_fmt &&
            ASM_MEDIA_FMT_WMA_V9_V2 != dec_fmt)
      {
         AudioDecSvc_SyncToInputTS(pTsState);
      }
      else
      {
         //check if the input TS is still in the future
         //hold up syncing until we reach there
         int64_t ts_diff = ((int64_t) pTsState->ullInbufTS) - ((int64_t) pTsState->ullNextOutBufTS);
         if(pTsState->bInbufTSValid &&
               (ts_diff < 2000) &&
               (ts_diff >= 0 ))
         {
            AudioDecSvc_SyncToInputTS(pTsState);
         }

      }

   }

   return ADSP_EOK;
}

/**
 * checks if the next input buffer TS is discontinuous with the expected TS after data in the current buffer.
 * should be called only when partial data from prev input buf is going to be put together with
 * data from next in buf.
 */
bool_t AudioDecSvc_IsNextBufTsDiscontinuous(DecTimeStampState_t* pTsState, uint32_t ulOutbufSize,
      elite_multi_channel_pcm_fmt_blk_t* pOutputFmt)
{
   //if prev buffers had valid TS flag and next buf doesnt have then TS is discont & vice versa
   if ( pTsState->bNextOutbufTSValid ^ pTsState->bInbufTSValid ) return true;

   //if both invalid TS => TS not discontinuous
   if (!pTsState->bInbufTSValid && !pTsState->bNextOutbufTSValid) return false;

   //if both valid:

   uint32_t bytes_per_sample = (pOutputFmt->bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   //increment TS based on # of samples and sample rate
   uint32_t ulNumSampPerCh = (ulOutbufSize) / (pOutputFmt->num_channels * bytes_per_sample);
   uint32_t ulSampleRate = pOutputFmt->sample_rate;

   //get output frame duration in microsec
   uint64_t samples = pTsState->ullNextOutBufSampleCount + ulNumSampPerCh;

   uint64_t end_ts = pTsState->ullLastSyncedInBufTS +
         ((samples * (uint64_t)NUM_US_PER_SEC ) / ulSampleRate );

   int64_t diff = end_ts - pTsState->ullInbufTS;

   if (diff >= -1000 && diff <= 1000) return false;

   return true;
}

/**
 * for PCM only
 *
 * if data is taken from old and new buffers, then new buffers timestamp need to be updated by data consumed.
 * this is done only if timestamp difference is within threshold (AudioDecSvc_IsNextBufTsDiscontinuous).
 */
ADSPResult AudioDecSvc_UpdateInputTsByDataConsumed(DecTimeStampState_t* pTsState, uint32_t inputBytesConsumed, DecPcmFmt_t *pcmFmt)
{
   if (!pTsState->bInbufTSValid) return ADSP_EOK;

   uint64_t pcm_attr = pcmFmt->usBytesPerSample*pcmFmt->chan_map.nChannels*pcmFmt->ulPCMSampleRate;
   uint64_t time = inputBytesConsumed * (uint64_t)NUM_US_PER_SEC / pcm_attr;

   pTsState->ullInbufTS += time; //if there's a call to AudioDecSvc_SyncToInputTS only then it'll have effect.

   return ADSP_EOK;
}

/*
 *Function Name: AudioDecSvc_RemoveIinitialSamples
 *
 *Parameters:samples_to_remove-samples to remove at the beginning of stream
 *           valid_bytes_in_buffer-valid byted in output buffer
 *           buffer_start- starting address of buffer
 *           num_channels- number of channels
 *Description: Removed the required number of samples from beginning of output buffer. If this
 *             is greater than output buffer size, removes the whole buffer and decrements
 *             the value accordingly
 *
 *Returns: none
 *
 */
void AudioDecSvc_RemoveInitialSamples(uint32_t *samples_to_remove,
      uint32_t *valid_bytes_in_buffer,
      uint8_t * buffer_start,
      uint16_t num_channels,
      uint32_t sampling_rate,
      uint64_t *next_output_buf_ts,
      uint64_t *next_out_buf_sample_count,
      uint32_t bytes_per_sample
)
{
   uint32_t bytes_to_remove, samples_removed;
   uint32_t valid_bytes_per_channel, bytes_to_remove_per_channel;

   /* per channel address to read from and write to */
   uint8_t *addr_read, *addr_write;

   bytes_to_remove = (*samples_to_remove) * num_channels * bytes_per_sample;

   valid_bytes_per_channel = (*valid_bytes_in_buffer)/num_channels;


   if(bytes_to_remove > *valid_bytes_in_buffer)
   {
      bytes_to_remove = *valid_bytes_in_buffer;
      samples_removed = ((bytes_to_remove / bytes_per_sample)/ num_channels);
      *samples_to_remove -= samples_removed;
   }
   else
   {
      samples_removed = *samples_to_remove;
      *samples_to_remove = 0;
   }

   bytes_to_remove_per_channel = bytes_to_remove / num_channels;

   addr_read = buffer_start;
   addr_write = buffer_start;

   for (uint16_t i=0; i < num_channels; i++)
   {
      memsmove(addr_write,valid_bytes_per_channel-bytes_to_remove_per_channel,
            addr_read+bytes_to_remove_per_channel,valid_bytes_per_channel -
            bytes_to_remove_per_channel);
      addr_read += valid_bytes_per_channel;
      addr_write += (valid_bytes_per_channel - bytes_to_remove_per_channel);
   }


   //memcpy(buffer_start, buffer_start + bytes_to_remove, *valid_bytes_in_buffer - bytes_to_remove);
   *valid_bytes_in_buffer -= bytes_to_remove;

   /*We need to increment the timestamp accordingly*/
   *next_output_buf_ts += (((uint64_t)samples_removed * NUM_US_PER_SEC)/sampling_rate);
   *next_out_buf_sample_count += samples_removed;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc: Initial bytes removed:%lu", bytes_to_remove);
}


/*
 *Function Name: AudioDecSvc_RemoveTrailingSamples
 *
 *Parameters:samples_to_remove-samples to remove at the end of stream
 *           valid_bytes_in_buffer-valid byted in output buffer *
 *           num_channels- number of channels
 *Description: Removed the required number of samples from the end
 *             of output buffer.
 *
 *Returns: none*
 */
void AudioDecSvc_RemoveTrailingSamples(uint32_t samples_to_remove,
      uint32_t *valid_bytes_in_buffer,
      uint8_t *buffer_start,
      uint16_t num_channels,
      uint32_t bytes_per_sample)
{
   uint32_t bytes_to_remove;
   uint32_t valid_bytes_per_channel, bytes_to_remove_per_channel;


   /* per channel address to read from and write to */
   uint8_t *addr_read, *addr_write;


   valid_bytes_per_channel = (*valid_bytes_in_buffer)/num_channels;

   bytes_to_remove = samples_to_remove * num_channels * bytes_per_sample;
   if(bytes_to_remove > *valid_bytes_in_buffer)
   {
      bytes_to_remove = *valid_bytes_in_buffer;
   }

   bytes_to_remove_per_channel = bytes_to_remove / num_channels;

   addr_read = buffer_start;
   addr_write = buffer_start;

   for (uint16_t i=0; i < num_channels-1; i++)
   {
      addr_read += valid_bytes_per_channel;
      addr_write += (valid_bytes_per_channel - bytes_to_remove_per_channel);

      memsmove(addr_write,valid_bytes_per_channel- bytes_to_remove_per_channel,
            addr_read, valid_bytes_per_channel - bytes_to_remove_per_channel);

   }


   *valid_bytes_in_buffer -= bytes_to_remove;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc: Trailing bytes removed:%lu", bytes_to_remove);
}

/*
 *Function Name: AudioDecSvc_LogBitStreamData
 *
 *Parameters: buf_addr : pointer to buffer to be logged
 *            buf_size : buffer size in bytes
 *            session_id : session id for the log
 *            ulDecFormatId : format id for decoder
 *
 *Description: Logs the encoded bistream data
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioDecSvc_LogBitStreamData(int8_t *buf_addr, uint32_t buf_size,
      uint32_t session_id, uint32_t ulDecFormatId)
{
   ADSPResult result = ADSP_EOK;
   elite_log_info log_info_obj;

   /* Populate the packet to be sent to logging utility */
   log_info_obj.qxdm_log_code = QXDM_LOG_CODE_AUD_DEC_IN;
   log_info_obj.buf_ptr = (uint8_t *)buf_addr;
   log_info_obj.buf_size = buf_size;
   log_info_obj.session_id = session_id;
   log_info_obj.log_tap_id = AUDIOLOG_AUD_DEC_BS_IN_TAP_ID;
   log_info_obj.log_time_stamp = qurt_elite_timer_get_time();
   log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_BITSTREAM;

   log_info_obj.data_info.media_fmt_id = ulDecFormatId;

   /* Allocate the log buffer and log the packet
      If log code is disabled, log buffer allocation returns NULL
    */
   result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

   return result;
}

/*
 *Function Name: AudioDecSvc_LogPcmData
 *
 *Parameters: AudioDecSvc_t* pMe: Instance of enc svc
 *            buf_addr: Addr of buffer to be logged
 *            buf_size: Size in bytes of buffer to be logged
 *
 *Description: Logs the input PCM data to encoder service
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioDecSvc_LogPcmData(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, int8_t * buf_addr, uint32_t buf_size)
{
   ADSPResult result = ADSP_EOK;

   elite_log_info log_info_obj;
   pcm_data_info *pcm_data = &(log_info_obj.data_info.pcm_data_fmt);

   /* Populate the packet to be sent to logging utility */
   log_info_obj.qxdm_log_code = QXDM_LOG_CODE_AUD_DEC_IN;
   log_info_obj.buf_ptr = (uint8_t *)buf_addr;
   log_info_obj.buf_size = buf_size;
   log_info_obj.session_id = pInpStrm->common.ulDataLogId;
   log_info_obj.log_tap_id = AUDIOLOG_AUD_DEC_PCM_IN_TAP_ID;
   log_info_obj.log_time_stamp = qurt_elite_timer_get_time();
   log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_PCM;

   pcm_data->num_channels = pInpStrm->pcmFmt.chan_map.nChannels;
   pcm_data->sampling_rate = pInpStrm->pcmFmt.ulPCMSampleRate;
   pcm_data->bits_per_sample = pInpStrm->pcmFmt.usBytesPerSample*8;
   pcm_data->interleaved = pInpStrm->pcmFmt.usIsInterleaved;
   pcm_data->channel_mapping = pInpStrm->pcmFmt.chan_map.nChannelMap;

   /* Allocate the log buffer and log the packet
      If log code is disabled, log buffer allocation returns NULL
    */
   result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

   return result;
}

ADSPResult AudioDecSvc_LogPcmData_Out(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, int8_t * buf_addr, uint32_t buf_size)
{
   ADSPResult result = ADSP_EOK;

   elite_log_info log_info_obj;

   dec_CAPI_container_t * last_capi = AudioDecSvc_GetLastCapi(pMe);
   if(NULL == last_capi)
   {
      return ADSP_ENOMEMORY;
   }

   uint8_t capi_out_index = AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm);

   /* Populate the packet to be sent to logging utility */
   log_info_obj.qxdm_log_code = QXDM_LOG_CODE_AUD_DEC_OUT;
   log_info_obj.buf_ptr = (uint8_t *)buf_addr;
   log_info_obj.buf_size = buf_size;
   log_info_obj.session_id = pOutStrm->common.ulDataLogId;
   log_info_obj.log_time_stamp = qurt_elite_timer_get_time();

   if(CAPI_V2_FIXED_POINT == last_capi->out_data_fmt_type[0])
   {
       pcm_data_info *pcm_data = &(log_info_obj.data_info.pcm_data_fmt);
       pcm_data->num_channels = pOutStrm->out_chan_map.num_out_chan;
       pcm_data->sampling_rate = last_capi->PrevFmtBlk[capi_out_index].sample_rate;
       pcm_data->bits_per_sample = pOutStrm->output_bits_per_sample;
       pcm_data->interleaved = AUD_DEC_DEINTERLEAVED;

       if( ASM_MEDIA_FMT_ADPCM == last_capi->dec_fmt_id )
       {
           //for pcm format, pcm dec doesnt do any de-interleaving,
           //same is the case with adpcm decoding
           //so handle this as a special case
           pcm_data->interleaved = pInpStrm->pcmFmt.usIsInterleaved;
       }
       pcm_data->channel_mapping = pOutStrm->out_chan_map.out_chan;
       log_info_obj.log_tap_id = AUDIOLOG_AUD_DEC_PCM_OUT_TAP_ID;
       log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_PCM;
   }
   else
   {
       log_info_obj.log_tap_id = AUDIOLOG_AUD_DEC_BS_OUT_TAP_ID;
       log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_BITSTREAM;
       log_info_obj.data_info.media_fmt_id = last_capi->dec_fmt_id;
   }


   /* Allocate the log buffer and log the packet
      If log code is disabled, log buffer allocation returns NULL
    */
   result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

   return result;
}

ADSPResult AudioDecSvc_GetRequiredThreadStackSize(AudioDecSvc_t *pMe, uint32_t *threadStackSize)
{
   if (*threadStackSize == 0 ) *threadStackSize = PCM_STACK_SIZE;

   *threadStackSize += STACK_SIZE_DEC_PROCESS;
   if (MIN_AUDEC_THREAD_STACK_SIZE > *threadStackSize)
   {
      //minimum cumulative stack required excluding decoder process
      *threadStackSize = MIN_AUDEC_THREAD_STACK_SIZE;
   }

   return ADSP_EOK;
}

bool_t AudioDecSvc_IsLastCapiIndex(AudioDecSvc_t *pMe, uint32_t ind)
{
   return ( (ind == (DEC_SVC_MAX_CAPI-1)) || (pMe->capiContainer[ind+1] == NULL) );
}

bool_t AudioDecSvc_IsLastCapi(AudioDecSvc_t *pMe, dec_CAPI_container_t *capi_container)
{
   for (uint8_t i=0; i<DEC_SVC_MAX_CAPI; i++)
   {
      if (capi_container == pMe->capiContainer[i])
      {
         if ( AudioDecSvc_IsLastCapiIndex (pMe, i) )
         {
            return TRUE;
         }
      }
   }
   return FALSE;
}

dec_CAPI_container_t *AudioDecSvc_GetFirstCapi(AudioDecSvc_t *pMe)
{
   return pMe->capiContainer[0];
}

//There can be only one MIMO CAPI
dec_CAPI_container_t *AudioDecSvc_GetMimoCapi(AudioDecSvc_t *pMe)
{
   //currently MIMO CAPI is always the last CAPI
   return AudioDecSvc_GetLastCapi(pMe);
}

dec_CAPI_container_t *AudioDecSvc_GetLastCapi(AudioDecSvc_t *pMe)
{
   uint8_t i;
   for (i = 0; i<DEC_SVC_MAX_CAPI; i++)
   {
      if (!pMe->capiContainer[i]) break;
   }

   if(0 == i)
   {
      return NULL;
   }
   else
   {
      return pMe->capiContainer[i-1];
   }
}

void AudioDecSvc_DecErrorEvent_Clear(dec_err_event_t *dec_err_event)
{
   if (!dec_err_event) return;

   dec_err_event->nConsecutiveDecErr = 0;
}

/**
 * called during flush, shouldn't disable events if enabled at that time.
 */
void AudioDecSvc_DecErrorEvent_Reset(dec_err_event_t *dec_err_event)
{
   if (!dec_err_event) return;

   AudioDecSvc_DecErrorEvent_Clear(dec_err_event);
}

void AudioDecSvc_DecErrorEvent_RecordError(dec_err_event_t *dec_err_event)
{
   if (!dec_err_event) return;

   //Mark start time of the first failure
   if(dec_err_event->nConsecutiveDecErr == 0)
   {
      dec_err_event->errorTime =  qurt_elite_timer_get_time_in_msec();
   }

   dec_err_event->nConsecutiveDecErr++;
}

static inline bool_t AudioDecSvc_DecErrEvent_NeedToRaise(uint16_t currError, uint16_t threshError, bool_t timeOut)
{
   return ((currError > 1) && timeOut) || ((currError >= threshError) && (threshError > 0));
}

void AudioDecSvc_DecErrorEvent_CheckRaise(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, dec_err_event_t *dec_err_event)
{
   if (!dec_err_event) return;

   if (!dec_err_event->bEnableDecErrEvent) return;

   if (dec_err_event->nConsecutiveDecErr == 0) return;

   uint64_t currentTime = qurt_elite_timer_get_time_in_msec();

   //If the time between now and the first consecutive failure is greater than timeout value, raise event.
   bool_t bTimeOut = ((currentTime - dec_err_event->errorTime) > dec_err_event->msThresholdTimeout);

#ifdef DBG_DEC_ERR_EVENT
   //debugging msg
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Decoder Error count : %d, threshold %d, time %ld",
         dec_err_event->nConsecutiveDecErr,dec_err_event->nThresholdDecError,
         (int32_t)(currentTime - dec_err_event->errorTime));
#endif//DBG_DEC_ERR_EVENT


   if(AudioDecSvc_DecErrEvent_NeedToRaise(dec_err_event->nConsecutiveDecErr, dec_err_event->nThresholdDecError, bTimeOut))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Consecutive error boudanry of size %u hit", dec_err_event->nThresholdDecError);

      //Raise dec err event.
      AudioStreamMgr_GenerateClientEventFromCb(&pInpStrm->common.CallbackHandle,
            ASM_DATA_EVENT_DEC_ERR_NOTIFY,
            pInpStrm->common.CallbackHandle.unAsynClientToken,
            NULL, 0);

      AudioDecSvc_DecErrorEvent_Clear(dec_err_event);

   }
}

void AudioDecSvc_DecErrEvent_SetParam(dec_err_event_t *dec_err_event, uint8_t *pSetParamPayload)
{
   asm_dec_err_param_t *pDecErrMsg =
         (asm_dec_err_param_t *)pSetParamPayload;

   if (0 == pDecErrMsg->n_dec_err_threshold)
   {
      dec_err_event->bEnableDecErrEvent = FALSE;
      return;
   }

   dec_err_event->bEnableDecErrEvent = TRUE;
   dec_err_event->nThresholdDecError = pDecErrMsg->n_dec_err_threshold;
   dec_err_event->msThresholdTimeout = pDecErrMsg->timeout_ms;
}

void AudioDecSvc_EarlyEos_AddEntryToLastFrameDelaysArray(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, uint32_t delay_from_cur_buf)
{
#ifdef EARLY_EOS_DEBUG
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EoS [0x%lx]: AudioDecSvc_EarlyEos_AddEntryToLastFrameDelaysArray delay %lu ms", pOutStrm->common.ulDataLogId, delay_from_cur_buf/1000);
#endif

   if (delay_from_cur_buf == 0)
   {
      return; //no point adding 0 size buf delay here.
   }

   // when init samples are removed, delay_from_cur_buf doesnt match steady state.
   // hence ignore those delays.
   if (pOutStrm->early_eos_ptr->is_init_samples_removed)
   {
      return;
   }

   AudioDecSvc_EarlyEos_t *early_eos_ptr = pOutStrm->early_eos_ptr;
   early_eos_ptr->last_frame_delay_us[early_eos_ptr->last_frame_delay_buf_index] = delay_from_cur_buf;
   early_eos_ptr->last_frame_delay_buf_index = (early_eos_ptr->last_frame_delay_buf_index+1) & (AUD_DEC_SVC_LAST_FRAME_DELAY_BUF_SIZE-1);
}

/**
 * Allows for conservative estimate last few frame delays
 *
 * Also helps in reducing circ buf recreate in cases like WMA where frame size can change 30-15-15-30-15-15 ms etc.
 *
 * Also delays buffer creation until 4 frames are decoded. this helps prevent initial circ buf recreates.
 */
uint32_t AudioDecSvc_EarlyEos_GetMinFromLastFrameDelaysArray(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   uint32_t min=AUD_DEC_SVC_FRAME_DELAY_RESET_VALUE;
   bool_t any_reset = FALSE;
   AudioDecSvc_EarlyEos_t *early_eos_ptr = pOutStrm->early_eos_ptr;
   for (uint8_t i = 0; i < AUD_DEC_SVC_LAST_FRAME_DELAY_BUF_SIZE; i++)
   {
      if (early_eos_ptr->last_frame_delay_us[i] == AUD_DEC_SVC_FRAME_DELAY_RESET_VALUE)
      {
         any_reset = TRUE;
         break;
      }

      if (early_eos_ptr->last_frame_delay_us[i] < min)
      {
         min = early_eos_ptr->last_frame_delay_us[i];
      }
   }
   if (any_reset)
   {
      return AUD_DEC_SVC_FRAME_DELAY_RESET_VALUE;
   }
   return min;
}

void AudioDecSvc_EarlyEoS_ResetLastFrameDelaysArray(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   AudioDecSvc_EarlyEos_t *early_eos_ptr = pOutStrm->early_eos_ptr;
   for (uint8_t i = 0; i < AUD_DEC_SVC_LAST_FRAME_DELAY_BUF_SIZE; i++)
   {
      early_eos_ptr->last_frame_delay_us[i] = (uint32_t)AUD_DEC_SVC_FRAME_DELAY_RESET_VALUE;
   }
   early_eos_ptr->last_frame_delay_buf_index = 0;
}

/**
 * Generally it's good to fill the circ buffer
 * but when frame size decreases, more decoding may not be needed as the earlier decoded buffers will suffice the delay.
 * in this case filling circ buf causes too much delay
 * hence call this function to fill only until delay is satisfied.
 */
bool_t AudioDecSvc_EarlyEos_IsDelaySatisfied(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   if (!pOutStrm->early_eos_ptr)
   {
      return true;
   }

   AudioDecSvc_EarlyEos_t *early_eos_ptr = pOutStrm->early_eos_ptr;

   if (AudioDecSvc_CircBuf_IsDelayFull(&pOutStrm->CircBufList))
   {
      return true;
   }

   uint32_t min_delay = AudioDecSvc_EarlyEos_GetMinFromLastFrameDelaysArray(pMe, pOutStrm);
   if (min_delay == AUD_DEC_SVC_FRAME_DELAY_RESET_VALUE)
   {
      return true; //without frame delay being set, delay calc is not correct. so assume delay is satisfied.
   }

   int64_t downstream_delay = 0;
   if (pOutStrm->downstream_buf_delay_ptr) downstream_delay = *pOutStrm->downstream_buf_delay_ptr;

   int64_t delay_needed = (int64_t)early_eos_ptr->early_eos_delay_ms*1000;

   int64_t total_delay = (*pOutStrm->common.delay_ptr) + downstream_delay;

   //pOutStrm->common.delay_ptr might include all output buffer delays (2).
   //but when EoS is picked by decoder, we might have only one delay. to be conservative try to decode one extra frame.
   if (total_delay > min_delay)
   {
      total_delay = total_delay - min_delay;
   }

#ifdef EARLY_EOS_DEBUG
   int64_t delay_needed_in_circ_buf = delay_needed - total_delay;// - pOutStrm->nBufsAllocated*delay_from_cur_buf;

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EoS [0x%lx]: IsDelaySatisfied: Configured delay %lu ms. min_delay %lu ms. downstream_delay %lu ms. dec ext bufs %d. total stream delay %lu ms. delay needed thru circ buf %ld ms.",
            pOutStrm->common.ulDataLogId, early_eos_ptr->early_eos_delay_ms, min_delay/1000, (uint32_t)(downstream_delay/1000), pOutStrm->nBufsAllocated,
              (uint32_t)(total_delay/1000), (uint32_t)(delay_needed_in_circ_buf/1000));
#endif

   if (total_delay >= delay_needed)
   {
      return true;
   }

   return false;
}

void AudioDecSvc_EarlyEoS_AddBuffersToCurrBitField(AudioDecSvc_t *pMe)
{
   AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_EarlyEoS_GetStream(pMe);
   if (pOutStrm)
   {
      //after recreating if there are empty bufs, raise signal
      if (!AudioDecSvc_EarlyEos_IsDelaySatisfied(pMe, pOutStrm))
      {
         AudioDecSvc_EarlyEoS_RaiseSignal(pMe, pOutStrm);
      }
   }
}
ADSPResult AudioDecSvc_EarlyEoS_Destroy(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)

{
   if (pOutStrm->early_eos_ptr)
   {
      AudioDecSvc_EarlyEos_t *early_eos_ptr = pOutStrm->early_eos_ptr;
      pOutStrm->early_eos_ptr = NULL;
      qurt_elite_timer_delete(&early_eos_ptr->early_eos_timer);
      AudioDecSvc_EarlyEos_ClearSignal(pMe, pOutStrm);
      qurt_elite_signal_deinit(&(early_eos_ptr->fill_circ_buf_signal));
      qurt_elite_signal_deinit(&(early_eos_ptr->early_eos_timer_signal));

      qurt_elite_memory_free(early_eos_ptr);

   }
   return ADSP_EOK;
}

void AudioDecSvc_EarlyEoS_Reset(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   AudioDecSvc_EarlyEos_t *early_eos_ptr = pOutStrm->early_eos_ptr;
   //if early-EoS timer is scheduled, then this call can happen due to EoS.
   //if so, dont clear or flush.
   //if early-eos-timer triggers after EoS propagates downstream, flush is done in timer signal handler.
   if (!early_eos_ptr->early_eos_scheduled)
   {
      early_eos_ptr->early_eos_scheduled = FALSE;

      AudioDecSvc_CircBuf_Flush(&pOutStrm->CircBufList, pMe, pOutStrm);

      early_eos_ptr->reset_needed = FALSE;
   }
   else
   {
      early_eos_ptr->reset_needed = TRUE;
   }

   early_eos_ptr->is_init_samples_removed = FALSE;

   AudioDecSvc_EarlyEoS_ResetLastFrameDelaysArray(pMe, pOutStrm);
}

ADSPResult AudioDecSvc_EarlyEoS_BufferUp(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, uint32_t delay_from_cur_buf)
{
   ADSPResult result = ADSP_EOK;

   AudioDecSvc_EarlyEos_t *early_eos_ptr = pOutStrm->early_eos_ptr;

   if (NULL == early_eos_ptr)
   {
      return result;
   }

   AudioDecSvc_EarlyEos_AddEntryToLastFrameDelaysArray(pMe, pOutStrm, delay_from_cur_buf);

   uint32_t min_frame_delay = AudioDecSvc_EarlyEos_GetMinFromLastFrameDelaysArray(pMe, pOutStrm);
   if (min_frame_delay == AUD_DEC_SVC_FRAME_DELAY_RESET_VALUE)
   {
      return result; //no calculation can be done if delay values are not set.
   }

   //total_delay = delay from dec output buffers + delay from switch svc buffers + POPP delay
   //ignore PP delay as it's fixed.
   int64_t downstream_delay = 0;
   if (pOutStrm->downstream_buf_delay_ptr) downstream_delay = *pOutStrm->downstream_buf_delay_ptr;

   int64_t delay_needed = (int64_t)early_eos_ptr->early_eos_delay_ms*1000;

   //do not inc dec svc output buffer delay here because, at the time of EoS, those buffers may not be present.
   int64_t delay_needed_from_circ_buf = delay_needed - downstream_delay;// - pOutStrm->nBufsAllocated*min_frame_delay;

   //if all future bufs are of same delay as cur_buf & we decode fast enough, then delay is possible.

   int32_t num_delay_buf_in_circ_buf = (delay_needed_from_circ_buf + min_frame_delay - 1) / min_frame_delay ; //round off.

#ifdef EARLY_EOS_DEBUG
   int64_t total_delay = *pOutStrm->common.delay_ptr + downstream_delay;

   MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EoS [0x%lx]: Configured delay %lu ms. min_frame_delay %lu ms. downstream_delay %lu ms. dec ext bufs %d. total stream delay %lu ms. total needed buffers in circ buf %ld, is_init_samples_removed %u.",
         pOutStrm->common.ulDataLogId, early_eos_ptr->early_eos_delay_ms, min_frame_delay/1000, (uint32_t)(downstream_delay/1000), pOutStrm->nBufsAllocated,
           (uint32_t)(total_delay/1000), num_delay_buf_in_circ_buf, early_eos_ptr->is_init_samples_removed);
#endif
   //allocate more buffers
   result = AudioDecSvc_ReCreateCircBufs(pMe, pOutStrm, pOutStrm->maxExtBufSize, num_delay_buf_in_circ_buf);

   return result;
}

ADSPResult AudioDecSvc_EarlyEoS_SendNotif(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EOS: sending notif");
   return AudioStreamMgr_GenerateClientEventFromCb(&pOutStrm->common.CallbackHandle,
         ASM_DATA_EVENT_EARLY_EOS,
         pOutStrm->common.CallbackHandle.unAsynClientToken,
         NULL,
         0);
}

ADSPResult AudioDecSvc_EarlyEoS_Schedule(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   ADSPResult result = ADSP_EOK;

   AudioDecSvc_EarlyEos_t *early_eos_ptr = pOutStrm->early_eos_ptr;

   if (NULL == early_eos_ptr)
   {
      return ADSP_EOK;
   }
   if (early_eos_ptr->early_eos_scheduled)
   {
      return ADSP_EOK;
   }

   uint32_t min_delay = AudioDecSvc_EarlyEos_GetMinFromLastFrameDelaysArray(pMe, pOutStrm);
   int64_t time_left_for_early_eos = 0;
   if (min_delay != AUD_DEC_SVC_FRAME_DELAY_RESET_VALUE)
   {
      //total_delay = delay from dec output buffers + delay from switch svc buffers + POPP delay
      //ignore PP delay as it's fixed.
      int64_t downstream_delay = 0;
      if (pOutStrm->downstream_buf_delay_ptr) downstream_delay = *pOutStrm->downstream_buf_delay_ptr;

      int64_t total_delay = *pOutStrm->common.delay_ptr + downstream_delay;

      int64_t delay_needed = (int64_t)early_eos_ptr->early_eos_delay_ms*1000;

      time_left_for_early_eos = total_delay - delay_needed;

      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EOS [0x%lx]: time_left_for_early_eos %ld ms. total_delay %ld ms",
            pOutStrm->common.ulDataLogId, (int32_t)(time_left_for_early_eos/1000), (int32_t)(total_delay/1000));
   }

   if (time_left_for_early_eos > 0)
   {
      pMe->unCurrentBitfield |= AUD_DEC_EARLY_EOS_TIMER_SIG;
      //schedule timer
      result = qurt_elite_timer_oneshot_start_duration(&early_eos_ptr->early_eos_timer, time_left_for_early_eos);
      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Early-EOS [0x%lx]: timer set for %ld ms failed", pOutStrm->common.ulDataLogId, time_left_for_early_eos/1000);
         pMe->unCurrentBitfield &= ~AUD_DEC_EARLY_EOS_TIMER_SIG;
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EOS [0x%lx]: timer set for %ld ms success", pOutStrm->common.ulDataLogId, time_left_for_early_eos/1000);
      }
   }
   else
   {
      result = AudioDecSvc_EarlyEoS_SendNotif(pMe, pOutStrm);
   }
   early_eos_ptr->early_eos_scheduled = TRUE;

   return result;
}

AudioDecSvc_OutStream_t *AudioDecSvc_EarlyEoS_GetStream(AudioDecSvc_t* pMe)
{
   AudioDecSvc_OutStream_t *pOutStrm = NULL;
   for (uint16_t i=0; i < DEC_SVC_MAX_OUTPUT_STREAMS; i++)
   {
      if (pMe->out_streams_ptr[i])
      {
         if (pMe->out_streams_ptr[i]->early_eos_ptr) //only one stream has early-eos
         {
            pOutStrm = pMe->out_streams_ptr[i];
            break;
         }
      }
   }
   return pOutStrm;
}

ADSPResult AudioDecSvc_EarlyEoS_ProcessTimerSignal(AudioDecSvc_t* pMe, uint32_t channelBitIndex)
{
   ADSPResult result = ADSP_EOK;

   AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_EarlyEoS_GetStream(pMe);

   if (pOutStrm)
   {
      AudioDecSvc_EarlyEos_t *early_eos_ptr = pOutStrm->early_eos_ptr;

      result = AudioDecSvc_EarlyEoS_SendNotif(pMe, pOutStrm);

      qurt_elite_signal_clear(&early_eos_ptr->early_eos_timer_signal);
      pMe->unCurrentBitfield &= ~AUD_DEC_EARLY_EOS_TIMER_SIG;

      if (early_eos_ptr->reset_needed)
      {
         AudioDecSvc_CircBuf_Flush(&pOutStrm->CircBufList, pMe, pOutStrm);

         early_eos_ptr->reset_needed = FALSE;

         early_eos_ptr->early_eos_scheduled = FALSE;
      }
   }

   return result;
}

ADSPResult AudioDecSvc_EarlyEoS_Create(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, uint8_t *pSetParamPayload)
{
   ADSPResult result = ADSP_EOK;

   asm_dec_cfg_early_eos_notif_t *pEarlyEoS = (asm_dec_cfg_early_eos_notif_t*)pSetParamPayload;

   if (pEarlyEoS->time_ms > 300)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Early-EoS: given time too high %lu", pEarlyEoS->time_ms);
      return ADSP_EBADPARAM;
   }

   if (pEarlyEoS->time_ms == 0)
   {
      return AudioDecSvc_EarlyEoS_Destroy(pMe, pOutStrm);
   }

   if (pOutStrm->early_eos_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Early-EoS: config cannot be changed once set.");
      return ADSP_EALREADY;
   }

   if (DEC_SVC_PULL_MODE == pMe->ulMode)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Early-EoS: PULL Mode doesnot support Early-EOS");
      return ADSP_EBADPARAM;
   }

   if (pMe->is_rt_dec)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Early-EoS: AFE decoder doesn't support Early-EOS");
      return ADSP_EBADPARAM;
   }

   for (uint8_t i = 0; i < DEC_SVC_MAX_INPUT_STREAMS; i++)
   {
      AudioDecSvc_InpStream_t * pInpStrm = pMe->in_streams_ptr[i];

      if (pInpStrm)
      {
         if ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pInpStrm->common.perf_mode) ||
             (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION == pInpStrm->common.perf_mode) ||
            (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pInpStrm->common.perf_mode))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Early-EoS: No Stream can be in ULL/LLNP mode for early-eos");
            return ADSP_EBADPARAM;
         }
      }
   }

   if (AudioDecSvc_HasDecodingBegun(pMe))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Early-EoS: config cannot be changed once decoding has begun.");
      return ADSP_EALREADY;
   }

   for (uint16_t i=0; i < DEC_SVC_MAX_OUTPUT_STREAMS; i++)
   {
      if (pMe->out_streams_ptr[i])
      {
         if (pMe->out_streams_ptr[i]->early_eos_ptr)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Early-EoS: config not supported on multiple streams");
            return ADSP_EALREADY;
         }
      }
   }

   AudioDecSvc_EarlyEos_t *temp_eos_ptr = NULL;
   temp_eos_ptr = (AudioDecSvc_EarlyEos_t*)qurt_elite_memory_malloc(sizeof(AudioDecSvc_EarlyEos_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == temp_eos_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Out of memory allocating early-eos struct");
      return ADSP_ENOMEMORY;
   }
   memset(temp_eos_ptr, 0, sizeof(AudioDecSvc_EarlyEos_t));

   temp_eos_ptr->early_eos_delay_ms = pEarlyEoS->time_ms;

   if (ADSP_FAILED(result = qurt_elite_signal_init(&(temp_eos_ptr->fill_circ_buf_signal))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed init Early-EoS: signal result = %d!!\n", result);
      qurt_elite_memory_free(temp_eos_ptr);
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_signal_init(&(temp_eos_ptr->early_eos_timer_signal))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed init Early-EoS: signal result = %d!!\n", result);
      qurt_elite_signal_deinit(&(temp_eos_ptr->fill_circ_buf_signal));
      qurt_elite_memory_free(temp_eos_ptr);
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_channel_add_signal(&pMe->channel, &(temp_eos_ptr->fill_circ_buf_signal), AUD_DEC_EARLY_EOS_CIRC_BUF_FILL_SIG)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed adding circ buf fill signal to channel result = %d!!\n", result);
      qurt_elite_signal_deinit(&(temp_eos_ptr->fill_circ_buf_signal));
      qurt_elite_signal_deinit(&(temp_eos_ptr->early_eos_timer_signal));
      qurt_elite_memory_free(temp_eos_ptr);
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_channel_add_signal(&pMe->channel, &(temp_eos_ptr->early_eos_timer_signal), AUD_DEC_EARLY_EOS_TIMER_SIG)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed adding Early-EoS: timer signal to channel result = %d!!\n", result);
      qurt_elite_signal_deinit(&(temp_eos_ptr->fill_circ_buf_signal));
      qurt_elite_signal_deinit(&(temp_eos_ptr->early_eos_timer_signal));
      qurt_elite_memory_free(temp_eos_ptr);
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_timer_create(&(temp_eos_ptr->early_eos_timer), QURT_ELITE_TIMER_ONESHOT_DURATION,  QURT_ELITE_TIMER_USER, &(temp_eos_ptr->early_eos_timer_signal))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed creating Early-EoS: timer result = %d!!\n", result);
      qurt_elite_signal_deinit(&(temp_eos_ptr->fill_circ_buf_signal));
      qurt_elite_signal_deinit(&(temp_eos_ptr->early_eos_timer_signal));
      qurt_elite_memory_free(temp_eos_ptr);
      return result;
   }

   pOutStrm->early_eos_ptr = temp_eos_ptr;

   AudioDecSvc_EarlyEoS_ResetLastFrameDelaysArray(pMe, pOutStrm);

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Enabled Early-EoS: time-ms = %lu", temp_eos_ptr->early_eos_delay_ms);

   return result;
}


/**
 * total BW = sum of CAPI BW + svc contribution.
 */
static uint32_t AudioDecSvc_DetermineTotalBw(AudioDecSvc_t* pMe)
{
   uint32_t bw = 0;

   for (uint8_t capi_i = 0; capi_i < DEC_SVC_MAX_CAPI; capi_i++)
   {
      if (!pMe->capiContainer[capi_i]) break;

      bw += (pMe->capiContainer[capi_i]->data_bw + pMe->capiContainer[capi_i]->code_bw);
   }

   if(NULL != pMe->pUll_modules)
   {
      bw += audio_module_chain_wrapper_get_bw(pMe->pUll_modules);
   }
   return bw + pMe->svc_bw;
}

/*
 * svc BW = input BW + output BW. Input BW is considered only for PCM.
 *
 * Also this BW should ideally be InpStrm, OutStrm property. But for now, its property of AudioDecSvc_t
 */
static void AudioDecSvc_DetermineSvcBw(AudioDecSvc_t* pMe, int32_t sample_rate, int32_t num_channels, uint16_t bits_per_sample, DecPcmFmt_t *pcmInFmt)
{
   uint32_t bw = 0;
   bw += sample_rate * \
            num_channels * \
            bits_per_sample/8; //this works for compressed as well since 61937 is as good as pcm.

   for (uint8_t capi_i = 0; capi_i < DEC_SVC_MAX_CAPI; capi_i++)
   {
      if (!pMe->capiContainer[capi_i]) break;

      switch(pMe->capiContainer[capi_i]->dec_fmt_id)
      {
      case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2:
      case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3:
      {
         if (NULL != pcmInFmt)
         {
            bw += pcmInFmt->ulPCMSampleRate * pcmInFmt->usBytesPerSample * pcmInFmt->chan_map.nChannels;
            if (pcmInFmt->ulPCMSampleRate > 48000)
            {
               bw += 1*1024*1024;
            }
            if (pcmInFmt->chan_map.nChannels > 2)
            {
               bw += 1*1024*1024;
            }
         }
      }
      break;
      default:
         break;
      }
   }

   pMe->svc_bw = bw;
}

/**
 * force_vote doesn't matter if is_release=TRUE
 *
 * is_release helps in releasing BW even when aggregated BW is nonzero, useful for suspend.
 *
 * force_vote helps in voting BW due to changes in svc & not due to CAPI V2 events.
 */
ADSPResult AudioDecSvc_ProcessKppsBw(AudioDecSvc_t *pMe, bool_t is_release, bool_t force_vote)
{
   ADSPResult result = ADSP_EOK;

   uint32_t decoder_kpps=0, gen_bw = 0;

   if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->ulAdsppmClientId) )
   {
      return ADSP_EFAILED;
   }

   if (!is_release)
   {
      if ( (pMe->event_mask & AUD_DEC_SVC_EVENT__KPPS_MASK) || force_vote)
      {
         result = AudioDecSvc_AggregateKppsRequired(pMe, &decoder_kpps);
         if(ADSP_FAILED(result))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to aggregate kpps");
         }

         pMe->event_mask &= ~AUD_DEC_SVC_EVENT__KPPS_MASK;
      }
      else
      {
         decoder_kpps = pMe->prev_kpps_vote;
      }

      if ( (pMe->event_mask & AUD_DEC_SVC_EVENT__BW_MASK) || force_vote)
      {
         gen_bw = AudioDecSvc_DetermineTotalBw(pMe);

         //clear the event
         pMe->event_mask &= ~AUD_DEC_SVC_EVENT__BW_MASK;

      }
      else
      {
         gen_bw = pMe->prev_bw_vote;
      }
   }

   //If there was no event or no release-call, or no foce vote or there was no change, return.
   if (!(is_release || (pMe->prev_kpps_vote != decoder_kpps) || (pMe->prev_bw_vote != gen_bw)))
   {
      return ADSP_EOK;
   }

#if (ADSPPM_INTEGRATION==1)
   static const uint8_t NUM_REQUEST=2;
   MmpmRscParamType rscParam[NUM_REQUEST];
   MMPM_STATUS      retStats[NUM_REQUEST];
   MmpmRscExtParamType reqParam;
   uint8_t req_num=0;
   MmpmMppsReqType mmpmMppsParam;
   MmpmGenBwValType bwReqVal;
   MmpmGenBwReqType bwReq;
   uint32_t mpps=0;
   reqParam.apiType                    = MMPM_API_TYPE_SYNC;
   reqParam.pExt                       = NULL;       //for future
   reqParam.pReqArray                  = rscParam;
   reqParam.pStsArray                  = retStats;   //for most cases mmpmRes is good enough, need not check this array.
   reqParam.reqTag                     = 0;          //for async only

   /** check if mips and bw are both releases or requests. both_diff => each != request or release. */
   bool_t both_diff = ( (decoder_kpps > 0) ^ (gen_bw > 0) );
   //whether mips req is made
   bool_t is_mips_req = ( (pMe->prev_kpps_vote != decoder_kpps) || is_release);
   //whether bw req is made
   bool_t is_bw_req = ( (pMe->prev_bw_vote != gen_bw) || is_release );

   bool_t req_done = false;

   if ( is_mips_req )
   {
      pMe->prev_kpps_vote = decoder_kpps;

      mpps = (decoder_kpps+999)/1000;

      //Requesting 0 will be equivalent to releasing particular resource
      mmpmMppsParam.mppsTotal                  = mpps;
      mmpmMppsParam.adspFloorClock             = 0;
      rscParam[req_num].rscId                   = MMPM_RSC_ID_MPPS;
      rscParam[req_num].rscParam.pMppsReq       = &mmpmMppsParam;

      req_num++;
      if (both_diff || !is_bw_req) //request separately if either bw and mips are differet types (req/rel), or if there's no BW req.
      {
         reqParam.numOfReq                   = req_num;

         if (decoder_kpps == 0)
         {
            result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release MPPS by Dec (%lu). Result %lu",pMe->ulAdsppmClientId, result);
         }
         else
         {
            result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu by Dec (%lu). Result %lu", mpps, pMe->ulAdsppmClientId,result);
         }
         req_done = true;
         req_num = 0; //reset req_num as req is already made.
      }
   }

   if ( is_bw_req )
   {
      pMe->prev_bw_vote = gen_bw;

      bwReqVal.busRoute.masterPort                 = MMPM_BW_PORT_ID_ADSP_MASTER;
      bwReqVal.busRoute.slavePort                  = MMPM_BW_PORT_ID_DDR_SLAVE;
      bwReqVal.bwValue.busBwValue.bwBytePerSec     = gen_bw;
      bwReqVal.bwValue.busBwValue.usagePercentage  = 100;
      bwReqVal.bwValue.busBwValue.usageType        = MMPM_BW_USAGE_LPASS_DSP;

      bwReq.numOfBw            = 1;
      bwReq.pBandWidthArray    = &bwReqVal;

      rscParam[req_num].rscId                   = MMPM_RSC_ID_GENERIC_BW_EXT;
      rscParam[req_num].rscParam.pGenBwReq      = &bwReq;

      req_num++;
      if (both_diff || !is_mips_req) //request separately if either bw and mips are differet types (req/rel), or if there's no mips req.
      {
         reqParam.numOfReq                   = req_num;

         if (gen_bw==0)
         {
            result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW by Dec (%lu). Result %lu", pMe->ulAdsppmClientId, result);
         }
         else
         {
            result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request BW %lu KBPS by Dec (%lu). Result %lu", gen_bw/1024, pMe->ulAdsppmClientId, result);
         }
         req_done = true;
         req_num = 0; //reset req_num as req is already made.
      }
   }

   if (req_num && !req_done)
   {
      reqParam.numOfReq                   = req_num;

      if ( (decoder_kpps == 0) && (gen_bw == 0) )
      {
         result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW and MPPS by Dec (%lu). Result %lu", pMe->ulAdsppmClientId,result);
      }
      else
      {
         result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu and BW %lu KBPS by Dec (%lu). Result %lu", mpps, gen_bw/1024, pMe->ulAdsppmClientId, result);
      }
   }

#endif //#if (ADSPPM_INTEGRATION==1)
   return result;
}

/** gives for all CAPIs. but not per IO streams. */
static ADSPResult AudioDecSvc_AggregateKppsRequired(AudioDecSvc_t *pMe, uint32_t *decoder_kpps)
{
   int kpps,num_chans=0,samp_rate=0,bytes_per_samp=0;
   *decoder_kpps = 0;

   for (uint8_t i=0; i<DEC_SVC_MAX_CAPI; i++)
   {
      if (!pMe->capiContainer[i]) break;

      if (pMe->capiContainer[i] && pMe->capiContainer[i]->capi_ptr)
      {
         kpps = pMe->capiContainer[i]->kpps;
         if (0==kpps) kpps = GENERIC_DEC_KPPS;
      }
      else
      {
         /* Aggregating MPPS votes for PCM decoder using media format */
         /* scaling to a minimum of 4 MPPS to avoid very less vote to */
         /* ensure existing usecases are not disturbed much           */
         num_chans =  pMe->in_streams_ptr[0]->pcmFmt.chan_map.nChannels;
         samp_rate = pMe->in_streams_ptr[0]->pcmFmt.ulPCMSampleRate;
         bytes_per_samp = pMe->in_streams_ptr[0]->pcmFmt.usBytesPerSample;
         kpps = (PCM_DEC_KPPS_FACTOR * num_chans * samp_rate * bytes_per_samp)/1000;   //default mips (PCM)
         kpps = (kpps > PCM_DEC_MIN_KPPS) ? kpps : PCM_DEC_MIN_KPPS;
         if(NULL != pMe->pUll_modules)
         {
            kpps += (int)audio_module_chain_wrapper_get_kpps(pMe->pUll_modules);
         }
      }

      *decoder_kpps += kpps;  //aggregate kpps.
   }

   return ADSP_EOK;
}

ADSPResult AudioDecSvc_RegisterWithAdsppm(AudioDecSvc_t* pMe)
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

   result = qurt_elite_adsppm_wrapper_register(&regParam, &pMe->ulAdsppmClientId, &pMe->adsppmClientPtr);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by Dec. Result %lu. Client id %lu", result, pMe->ulAdsppmClientId);

#endif
   return result;
}

ADSPResult AudioDecSvc_DeregisterWithAdsppm(AudioDecSvc_t* pMe)
{
   ADSPResult result = ADSP_EOK;

   if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->ulAdsppmClientId) )
   {
      return ADSP_EOK;
   }

   uint32_t client_id = pMe->ulAdsppmClientId;
   result = qurt_elite_adsppm_wrapper_deregister(&pMe->ulAdsppmClientId, &pMe->adsppmClientPtr);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by Dec. Result %lu. Client id %lu", result, client_id);
   return result;
}

ADSPResult AudioDecSvc_RaiseRenderedEoSv2(AudioDecSvc_t* pMe, elite_msg_any_t *respMsg)
{
   elite_msg_data_eos_apr_t *eos_payload = NULL;
   if (respMsg)
   {
      eos_payload = (elite_msg_data_eos_apr_t *)respMsg->pPayload;
   }

   bool_t any_resp_pending = FALSE;
   for (uint16_t i=0; i < DEC_SVC_MAX_INPUT_STREAMS; i++)
   {
      if (pMe->in_streams_ptr[i])
      {
         //make sure we send event to client only if we are waiting (in flush, we mark not waiting for eos - thus eos is dropped whether it's on stream side or on device side)
         //make sure it's the same stream to which this EoS was sent
         if ( pMe->in_streams_ptr[i]->bWaitingForEoSRespFromAfe &&
               ( (eos_payload == NULL) || ((uint32_t)pMe->in_streams_ptr[i] == eos_payload->unClientToken) ))
         {
            pMe->in_streams_ptr[i]->bWaitingForEoSRespFromAfe = FALSE;
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"DecSvc [0x%lx] : raising rendered EoS event.", pMe->in_streams_ptr[i]->common.ulDataLogId);
            elite_svc_raise_rendered_eos_v2_event( &(pMe->in_streams_ptr[i]->eosInfo) );
         }

         if (pMe->in_streams_ptr[i]->bWaitingForEoSRespFromAfe)
         {
            any_resp_pending = TRUE;
         }
      }
   }

   if (!any_resp_pending) //assuming only EOS uses respQ
   {
      pMe->unCurrentBitfield &= (~AUD_DEC_RESP_SIG);
   }

   if (respMsg)
   {
      elite_msg_release_msg(respMsg);
      respMsg->pPayload = NULL;
   }

   return ADSP_EOK;
}

/* See encoder svc impl for explanation.
 *
 * delay is updated after first buf is decoded and media-fmt is known.
 * actual delay maybe lesser just as encoder, because first buf decoding may not take full-frame-size.
 */
static void AudioDecSvc_UpdateInitialDelay(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);

   //worst case delay is considered (max_data_len)
   if (pOutStrm->common.delay_ptr && last_capi && last_capi->PrevFmtBlk->sample_rate)
   {
      uint32_t dec_out_size = 0;

      //if decoder has generated output by this time then use it to calculate delay.
      //expectation is that the output frame size does not change in steady stat for real time decoder
      if(last_capi->outputs[AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm)].buf_ptr->actual_data_len)
      {
         dec_out_size = last_capi->outputs[AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm)].buf_ptr->actual_data_len;
      }
      else
      {
         dec_out_size = pOutStrm->maxExtBufSize;
      }
      uint32_t total_data = DEFAULT_NUM_OUTPUT_BUFFERS_RT * dec_out_size;

      uint32_t bytes_per_sample = (last_capi->PrevFmtBlk->bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
      uint32_t bytes_p_sec = (last_capi->PrevFmtBlk->sample_rate * last_capi->PrevFmtBlk->num_channels * bytes_per_sample);

      uint32_t delay   = (((uint64_t)total_data * NUM_US_PER_SEC) + bytes_p_sec - 1) / bytes_p_sec;

      if (*pOutStrm->common.delay_ptr != delay)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                  "Updating decoder delay from %lu to %lu",*pOutStrm->common.delay_ptr, delay);
         *pOutStrm->common.delay_ptr = delay;
      }

   }
}

static ADSPResult AudioDecSvc_PushInitialZeros(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult result = ADSP_EOK;

   uint32_t count = 0;

   //at this we are holding on to one buf. take back up.
   qurt_elite_bufmgr_node_t currentBufNode = pOutStrm->outDataBufferNode;
   elite_msg_data_buffer_t* pBuf = (elite_msg_data_buffer_t*) pOutStrm->outDataBufferNode.pBuffer;
   uint32_t size_zeros = pBuf->nActualSize;

   pOutStrm->outDataBufferNode.pBuffer = NULL;
   dec_CAPI_container_t * last_capi  = AudioDecSvc_GetLastCapi(pMe);

   for (uint32_t i = 0; i < 1; i++)
   {
      result = AudioDecSvc_ProcessPopOutBuf(pMe, pOutStrm);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder will not be able to send required amount of initial zeros. sent %lu only out of 1", count+1);
         break;
      }

      if (NULL == pOutStrm->outDataBufferNode.pBuffer)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder will not be able to send required amount of initial zeros. sent %lu only out of 1", count+1);
         break;
      }

      elite_msg_data_buffer_t *pOutBuf = (elite_msg_data_buffer_t*) (pOutStrm->outDataBufferNode.pBuffer);
      if (0 == pOutBuf->nActualSize)
      {
         memset(&pOutBuf->nDataBuf, 0, size_zeros); //max len is initialized based on decoder CAPI input threshold.
         pOutBuf->nActualSize = size_zeros;

         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "Decoder sending %lu th zero buffer of size %lu to AFE", count+1, size_zeros);

         count++;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "buffer expected to have no data, but it seems to have %lu bytes", pOutBuf->nActualSize);
      }

	  if(last_capi)
	  {
	      uint32_t bytes_per_sample = (last_capi->PrevFmtBlk[0].bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
	      uint64_t bps_o = last_capi->PrevFmtBlk[0].sample_rate * bytes_per_sample * last_capi->PrevFmtBlk[0].num_channels;
	      uint64_t ullPrebufDelay = 0;

	      if(bps_o)
	      {
	         ullPrebufDelay = ((uint64_t)size_zeros*NUM_US_PER_SEC)/bps_o;
	      }


	      //update flags and TS for the pre-buffer from the output buffer generated by decoder
	      pOutBuf->nFlag = pBuf->nFlag;
	      pOutBuf->ullTimeStamp = pBuf->ullTimeStamp- ullPrebufDelay;

	      MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Pushing initial zeros. prebuffer TS [%lu, %lu], dec buffer TS [%lu, %lu]. Delay(%lu)",
	            (uint32_t)(pOutBuf->ullTimeStamp>>32),(uint32_t)pOutBuf->ullTimeStamp,
	            (uint32_t)(pBuf->ullTimeStamp>>32),(uint32_t)pBuf->ullTimeStamp,
	            (uint32_t)ullPrebufDelay);
	  }

      AudioDecSvc_SendPcmToPeerSvc(pMe, pInpStrm, pOutStrm,  (elite_msg_data_buffer_t*) pOutStrm->outDataBufferNode.pBuffer);
   }

   pOutStrm->outDataBufferNode = currentBufNode; //restore.

   //any change in this func might need updates in AudioEncSvc_UpdateInitialDelay.

   return result;
}

ADSPResult AudioDecSvc_HandlePrebuffer(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult result = ADSP_EOK;

   //prebuffer logic
   //unlike enc svc where prebuf is needed to be sent only before first buf, here, underruns can happen at run-time. hence need to send prebuf whenever input overtakes output.
   //prebuf helps in
   //a) cases like 2.5 ms frames, where AFE needs 2 bufs back to back.
   //b) reserving processing time for decoder. i.e if processing gets over very fast for the first time and takes longer in the subsequent times, then this buf gives cushion.
   if (pMe->is_rt_dec)
   {
      dec_CAPI_container_t * last_capi  = AudioDecSvc_GetLastCapi(pMe);
      dec_CAPI_container_t * first_capi = AudioDecSvc_GetFirstCapi(pMe);
      if (last_capi)
      {
         uint8_t capi_out_index = AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm);

         uint64_t Li = pInpStrm->common.total_bytes - first_capi->in_buf[AudioDecSvc_GetInputStreamIndex(pMe,pInpStrm)].actual_data_len;
         uint64_t Lo = pOutStrm->common.total_bytes;

         uint64_t bps_i = pInpStrm->pcmFmt.ulPCMSampleRate * pInpStrm->pcmFmt.usBytesPerSample * pInpStrm->pcmFmt.chan_map.nChannels;
         uint64_t bps_o = last_capi->PrevFmtBlk[capi_out_index].sample_rate * pOutStrm->output_bytes_per_sample * pOutStrm->out_chan_map.num_out_chan;

         // if ( in_duration - out_duration ) * 1e6 > T us, push zeros. T = IO_TIME_COMPARISON_THRESHOLD_US.
         // => ( Li/bps_i - Lo/bps_o ) * 1e6 > T
         // => ( Li*bps_o - Lo*bps_i ) * 1e6 > T * (bps_o * bps_i) (avoids division)
         // => Li*bps_o*1e6 > T * (bps_o * bps_i) + Lo*bps_i*1e6
         // => Li*bps_o*1e6 > (T * bps_o + Lo *1e6) * bps_i
         // if it is first output frame then also push zeroes. If decoder is generating output in first compressed input then in_duration < output duration.

         uint64_t lhs = Li * bps_o * NUM_US_PER_SEC;
         uint64_t rhs = (IO_TIME_COMPARISON_THRESHOLD_US * bps_o + Lo * NUM_US_PER_SEC) * bps_i;
         if( ( lhs > rhs ) || (0 == pOutStrm->out_buf_cnt) )
         {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Decoder prebuffer might have run out as out data "
                  "duration < in data duration OR its first dec buffer. lhs = %lu, rhs = %lu",
                  (uint32_t)lhs, (uint32_t) rhs);

            //prebuffer
            AudioDecSvc_PushInitialZeros(pMe, pOutStrm, pInpStrm);

            //once prebuffering has been established, counters can be reset.
            pOutStrm->common.total_bytes = 0;
            pInpStrm->common.total_bytes = 0;
         }
      }
   }

   return result;
}


