/*========================================================================

 *//** @file AudioEncSvc_Util.cpp
 This file contains Elite Audio encoder service include components.

Copyright (c) 2010-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
 Edit History

 $Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_enc_svc/src/AudioEncSvc_Util.cpp#3 $


 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 09/29/10   RKC      Created file.
 02/07/18   npendeka  Modified
 ========================================================================== */

#include "AudioEncSvc_Includes.h"
#include "AudioEncSvc_PushMode.h"
#include "AudioEncSvc_CapiV2Util.h"
#include <stringl/stringl.h>
#include "inter_module_comm.h"

static char AUDIO_ENC_SVC_DATA_Q_NAME[] = "DESd";
static char AUDIO_ENC_SVC_CMD_Q_NAME[] = "DESc";
static char AUDIO_ENC_SVC_BUF_Q_NAME[] = "DESb";

static const uint32_t MAX_ENC_OUT_BUF = 2;
static const uint32_t MAX_ENC_OUT_BUF_IN_AFE = 3;

static const uint32_t AUD_ENC_INTERLEAVED = 1;
//static const uint32_t AUD_ENC_DEINTERLEAVED = 0;

static const uint32_t MIN_SAMPLE_FREQ = 8000;
static const uint32_t MIN_CHANNEL = 1;
static const uint32_t MIN_BYTES_PER_SAMPLE = 2;

/*--------------------------------------------------------------------------
 Global Variables
 ----------------------------------------------------------------------------*/

#define LAST_CAPI       TRUE
#define NOT_LAST_CAPI   FALSE
#define MAX_BUF_Q_ELEMENTS 2
/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
static ADSPResult AudioEncSvc_CreateEncCAPI(AudioEncSvc_t* pMe, Enc_CAPI_Type enc_capi_type, uint32_t enc_cfg_id,
      AudioEncSvcInitParams_t* pInitParams, bool_t is_last_capi, enc_init_time_get_params_t *enc_params);
static ADSPResult AudioEncSvc_CreateInpBuf(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container, uint32_t buf_size);
static ADSPResult AudioEncSvc_CreateOutBuf(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container, bool_t is_last_capi, bool_t is_no_apr_enc, uint32_t buf_size);
static ADSPResult AudioEncSvc_HandlePortDataThreshChangeEvent(AudioEncSvc_t *pMe);
static ADSPResult AudioEncSvc_RecreateBuffers(AudioEncSvc_t *pMe,
      enc_capi_port_index_t in_port_index, enc_capi_port_index_t out_port_index,
      uint8_t capi_index);

static void AudioEncSvc_DetermineSvcBw(AudioEncSvc_t* pMe, elite_multi_channel_pcm_fmt_blk_t *pPcmFormatBlock);
static void AudioEncSvc_UpdateInitialDelay(AudioEncSvc_t *pMe);

void AudioEncSvc_InitMediaFmt(capi_v2_standard_data_format_t *std_media_fmt)
{
   std_media_fmt->bits_per_sample = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->bitstream_format = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->data_interleaving = CAPI_V2_INVALID_INTERLEAVING;
   std_media_fmt->data_is_signed = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->num_channels = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->q_factor = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   std_media_fmt->sampling_rate = CAPI_V2_DATA_FORMAT_INVALID_VAL;

   for (uint32_t j=0; (j<CAPI_V2_MAX_CHANNELS); j++)
   {
      std_media_fmt->channel_type[j] = (uint16_t)CAPI_V2_DATA_FORMAT_INVALID_VAL;
   }
}

/*===========================================================================
 FUNCTION       AudioEncSvc_CopyInpDatatoInternalBuf()

 DESCRIPTION
 1 Copies input data from dataQ buffer to internal buffer
 2 Updates the indices maintained for internal buffer to indicate the valid
 input samples in it, and if there is some more data to be copied from
 internal buffer it is also updated in unRemSamplesInDataBuf
 3 If all the samples in the input buffer are copied release the DataQ msg
 4 If mono or input data it is simple memcpy and if it is stereo and
 noninterleaved it is copied as interleaved into the input buffer
 All encoder CAPI's are fed with interleaved data and it is upto the
 CAPI to deinterleave if required

 PARAMETERS
 Input:  pointer to AudioEncSvc_t

 RETURN VALUE
 result : None
 ===========================================================================*/
void AudioEncSvc_CopyInpDatatoInternalBuf(AudioEncSvc_t* pMe, enc_CAPI_container_t *capi_container, bool_t *compr_need_more)
{
   int num_channels;
   int enc_in_bits_per_sample;

   uint32_t unInpBufRdIdx;
   uint32_t unSamplesToCopy, unSamplesInDataBuf;
   int8_t *pi8SrcPtr;
   int8_t *pi8DstPtr;

   //get input buffer from data msg payload
   elite_msg_data_buffer_t* pInpBuf = (elite_msg_data_buffer_t*) pMe->InpDataMsg.pPayload;

   //return w/o doing anything if there is no date left in input buffer
   if (!pInpBuf || (0 == pInpBuf->nActualSize))
   {
      return;
   }

   //in the compressed input case we dont need to copy more
   //since there is some data in internal buffer
   *compr_need_more = FALSE;

   enc_in_bits_per_sample=pMe->in_med_fmt.bits_per_sample;
   uint32_t enc_in_bytes_per_sample = (enc_in_bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   uint32_t enc_svc_input_bytes_per_sample = (pMe->in_med_fmt.bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   //Remaining samples in the data buffer to be copied to internal buffer
   unSamplesInDataBuf = (pInpBuf->nActualSize / enc_svc_input_bytes_per_sample);

   //check if this is a new buffer: only new buffer starts from offset zero.
   if (pInpBuf->nOffset == 0)
   {
      pMe->encTSParams.isLastCopyFromNewBuffer = TRUE;
   }

   //keep track of number of PCM samples already in internal buffer. Needed for timestamp calculation
   pMe->encTSParams.samples_in_internal_buf_before_copy = capi_container->in_buf_params.in_buf.actual_data_len / enc_in_bytes_per_sample;

   //Available empty space in the internal buffer
   unSamplesToCopy = ((capi_container->in_buf_params.in_buf.max_data_len - capi_container->in_buf_params.in_buf.actual_data_len) / enc_in_bytes_per_sample);

   //check if available input is less than the empty space in pcm buffer
   unSamplesToCopy = (unSamplesInDataBuf <= unSamplesToCopy) ? unSamplesInDataBuf : unSamplesToCopy;

   if( unSamplesToCopy == 0 ) return;

   num_channels = pMe->out_med_fmt_per_enc_cfg.num_channels;

   //Write offset to internal buffer and read offset for data buffer
   unInpBufRdIdx = pInpBuf->nOffset; //bytes

   //if deinterleaved stereo then set read index accordingly
   if (!pMe->in_med_fmt.is_interleaved)
   {
      unInpBufRdIdx = unInpBufRdIdx / num_channels;
   }

   //Set up input and output buffer pointers with correct indices
   pi8SrcPtr = ((int8_t *) &(pInpBuf->nDataBuf)) + (unInpBufRdIdx);
   pi8DstPtr = ((int8_t *) (capi_container->in_buf_params.in_buf.data_ptr)) + capi_container->in_buf_params.in_buf.actual_data_len;

   //output is Q31.
   AudioEncSvc_CopyWithInterleaving(pMe, num_channels, unSamplesToCopy, (int8_t *) pi8SrcPtr, (int8_t*) pi8DstPtr, enc_in_bytes_per_sample,
         enc_svc_input_bytes_per_sample, (enc_in_bytes_per_sample * 8 - 1), enc_in_bytes_per_sample * 8);

   //update the remaining sample in data buffer
   pInpBuf->nOffset += (unSamplesToCopy * enc_svc_input_bytes_per_sample);
   pInpBuf->nActualSize -= (unSamplesToCopy * enc_svc_input_bytes_per_sample);
   //update the available pcm samples in internal buffer
   capi_container->in_buf_params.in_buf.actual_data_len += (unSamplesToCopy * enc_in_bytes_per_sample);

   //if we have copied all the data from data buffer, release it
   if (0 == pInpBuf->nActualSize)
   {
      AudioEncSvc_ReleaseInputBuf(pMe, ADSP_EOK);
   }
   return;
}

ADSPResult AudioEncSvc_CreateCAPIs(AudioEncSvc_t *pMe, AudioEncSvcInitParams_t *pInitParams, enc_init_time_get_params_t *enc_params)
{
   uint32_t enc_cfg_id = pInitParams->ulConfigId;
   ADSPResult result = ADSP_EOK;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Creating CAPIs. io fmt conv mode %d. format id 0x%lx",
         pMe->io_fmt_conv, enc_cfg_id);

   switch (pMe->io_fmt_conv)
   {
   case ENC_SVC_IO_FORMAT_CONV_TYPE_AUTO:               //decode raw to PCM
   {
      result = AudioEncSvc_CreateEncCAPI(pMe, ENC_CAPI_TYPE_AUTO, enc_cfg_id, pInitParams, LAST_CAPI, enc_params);
      pMe->mainCapiContainer = pMe->capiContainer[0];
      break;
   }
   case ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW:       //depacketize
   {
      result = AudioEncSvc_CreateEncCAPI(pMe, ENC_CAPI_TYPE_DEPACKETIZER, enc_cfg_id, pInitParams, LAST_CAPI, enc_params);
      pMe->mainCapiContainer = pMe->capiContainer[0];
      break;
   }
   case ENC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_61937:       //encode->packetize.
   {
      result = AudioEncSvc_CreateEncCAPI(pMe, ENC_CAPI_TYPE_AUTO, enc_cfg_id, pInitParams, NOT_LAST_CAPI, enc_params);
      pMe->mainCapiContainer = pMe->capiContainer[0];

      if (ADSP_SUCCEEDED(result))
      {
         result = AudioEncSvc_CreateEncCAPI(pMe, ENC_CAPI_TYPE_PACKETIZER, enc_cfg_id, pInitParams, LAST_CAPI, enc_params);
      }
      break;
   }
   case ENC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_COP:
   {
      result = AudioEncSvc_CreateEncCAPI(pMe, ENC_CAPI_TYPE_AUTO, enc_cfg_id, pInitParams, NOT_LAST_CAPI, enc_params);
      pMe->mainCapiContainer = pMe->capiContainer[0];
      if (ADSP_SUCCEEDED(result))
      {
         result = AudioEncSvc_CreateEncCAPI(pMe, ENC_CAPI_TYPE_COP_PACKETIZER, enc_cfg_id, pInitParams, LAST_CAPI, enc_params);
      }
      break;
   }
   case ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_61937:     //pass through
   case ENC_SVC_IO_FORMAT_CONV_TYPE_RAW_TO_61937:       //packetize
   case ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_PCM:       //depacketizer->decodes
   case ENC_SVC_IO_FORMAT_CONV_TYPE_61937_CONV_61937:   //depacketizer->conv->packetize
   case ENC_SVC_IO_FORMAT_CONV_TYPE_COP_TO_PCM:
   {
      return ADSP_EUNSUPPORTED;
   }
   default:
      return ADSP_EUNSUPPORTED;
   }

   if (ADSP_SUCCEEDED(result))
   {
      //buffers (re)creation must be done only after all CAPIs are created
      //because buffer size depends on prev & current CAPI thresh.
      pMe->event_mask |= AUD_ENC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK;
      result = AudioEncSvc_HandlePortDataThreshChangeEvent(pMe);
   }

   if (ADSP_FAILED(result)) return ADSP_EFAILED;

   return ADSP_EOK;
}

static ADSPResult AudioEncSvc_CreatePcmEnc(AudioEncSvc_t* pMe, enc_CAPI_container_t *capi_container,
      AudioEncSvcInitParams_t *pInitParams, bool_t is_last_capi, enc_init_time_get_params_t *enc_params)
{
   ADSPResult result = ADSP_EOK;

   uint32_t output_bytes_per_sample =  (pInitParams->bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   pMe->out_med_fmt_per_enc_cfg.bits_per_sample = pInitParams->bits_per_sample; //init with this, later enc cfg can change it
   pMe->out_med_fmt_per_enc_cfg.bytes_per_sample =  output_bytes_per_sample;

   uint32_t unEncBufSizeBytes = AudioEncSvc_GetPcmOutBufSize(pInitParams->perf_mode,
         48000, output_bytes_per_sample, 2);

   //initialize output buffer params
   EncSvc_OutBufParams_t *pOutBufParams = &(capi_container->out_buf_params);

   if(pInitParams->encMode == ENC_SVC_PUSH_MODE)
   {
      result =  AudioEncSvc_UpdatePcmEncCfg(pMe, &pInitParams->push_mode.EncCfg.pcm, FALSE); //native mode not allowed

      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc failed to set media fmt for push mode");
         return result;
      }

      unEncBufSizeBytes = AudioEncSvc_GetPcmOutBufSize(pInitParams->perf_mode,
            pInitParams->push_mode.EncCfg.pcm.sample_rate, output_bytes_per_sample, pInitParams->push_mode.EncCfg.pcm.num_channels);
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PCM encoder perf mode is %d, input buffer size is %lu bytes",
         pInitParams->perf_mode, unEncBufSizeBytes);


   if (pMe->no_apr_enc)
   {
      pOutBufParams->bFrameMetaDataEnable = FALSE;
   }
   else
   {
      pOutBufParams->bFrameMetaDataEnable = pInitParams->bFrameMetaDataEnable;
   }

   capi_container->in_port_thresh = capi_container->out_port_thresh = unEncBufSizeBytes;

   pMe->event_mask |= AUD_ENC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK;
   result = AudioEncSvc_HandlePortDataThreshChangeEvent(pMe);

   return result;
}


/*
 *Function Name:AudioEncSvc_CreateEncCAPI
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *            AudioEncSvcInitParams_t* pInitParams: Initialization params
 *
 *Description: Creates an encoder CAPI instance
 *
 *Returns: ADSPResult
 *
 */
static ADSPResult AudioEncSvc_CreateEncCAPI(AudioEncSvc_t* pMe, Enc_CAPI_Type enc_capi_type, uint32_t enc_cfg_id,
      AudioEncSvcInitParams_t* pInitParams, bool_t is_last_capi, enc_init_time_get_params_t *enc_params)
{
   ADSPResult result = ADSP_EOK;
   uint8_t capi_index ;

   //find a slot in CAPI container array
   for (capi_index = 0; capi_index < ENC_SVC_MAX_CAPI; capi_index++)
   {
      if (NULL == pMe->capiContainer[capi_index])
      {
         break;
      }
   }
   if(ENC_SVC_MAX_CAPI == capi_index)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "All CAPIs occupied, cannot create any new CAPI!");
      return ADSP_EFAILED;

   }

   pMe->capiContainer[capi_index] = (enc_CAPI_container_t*) qurt_elite_memory_malloc(sizeof(enc_CAPI_container_t), QURT_ELITE_HEAP_DEFAULT);

   if (NULL == pMe->capiContainer[capi_index])
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No memory creating container for encoder CAPI");
      return ADSP_ENOMEMORY;
   }
   memset(pMe->capiContainer[capi_index], 0, sizeof(enc_CAPI_container_t));
   AudioEncSvc_InitMediaFmt(&(pMe->capiContainer[capi_index]->out_med_fmt.std));

   pMe->capiContainer[capi_index]->enc_cfg_id = enc_cfg_id;

   if (AudioEncSvc_IsPcmFmt(enc_cfg_id) && (enc_capi_type == ENC_CAPI_TYPE_AUTO))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Creating pcm encoder with fmt id 0x%lx", enc_cfg_id);
      pMe->mainCapiContainer = pMe->capiContainer[capi_index];
      return AudioEncSvc_CreatePcmEnc(pMe, pMe->capiContainer[capi_index], pInitParams, is_last_capi, enc_params);
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Creating an encoder CAPI at index %u. CAPI type %d(0-auto, 1-depack, 2-pack, 3-cop_depack, 4-cop_pack, 5-cop-pack, 6-cop-depack), last capi %u",
         capi_index, enc_capi_type, is_last_capi);

   enc_CAPI_init_time_get_params_t init_time_get_params;
   memset(&init_time_get_params, 0, sizeof(enc_CAPI_init_time_get_params_t));

   enc_CAPI_init_params_t capi_init_param;
   capi_init_param.enc_cfg_id = enc_cfg_id;
   capi_init_param.cb_info = audio_enc_svc_get_capi_v2_callback_handler(pMe, capi_index);
   capi_init_param.enc_destroy_fn = NULL;
   capi_init_param.capi_v2_presence = ENC_CAPI_V2_PRESENCE_NOT_PRESENT;

   /*
    Create input_bits_per_sample CAPI param. Use the bps from ASM in initparams
    to set the value If encoder doesn't support the asked bps, next lower bps
    should be used to set. Loop should contiune till the encoder returns success.
    Currently, only two possible values 16, 24.
    */

   switch (enc_capi_type)
   {
   case    ENC_CAPI_TYPE_AUTO:
   {
      //check if CAPI is present in AMDB.
      enc_AMDB_presence amdb_presence = audio_enc_svc_get_amdb_presence(enc_capi_type, capi_init_param.enc_cfg_id, 0, &capi_init_param.amdb_handle);

      if(ENC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMDB CAPI V2 found for fmt 0x%lx. Trying to create CAPI V2", enc_cfg_id);
         result = audio_enc_svc_create_init_auto_capi_v2(&(pMe->capiContainer[capi_index]->capi_ptr), pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "fmt 0x%lx is present as an unsupported type in AMDB, failing capi creation.", capi_init_param.enc_cfg_id);
         result = ADSP_EFAILED;
      }

      //typically enc capi don't raise output media fmt. so assign default values.
      pMe->capiContainer[capi_index]->out_med_fmt.main.format_header.data_format = CAPI_V2_RAW_COMPRESSED;
      pMe->capiContainer[capi_index]->out_med_fmt.raw_fmt.bitstream_format = capi_init_param.enc_cfg_id;
      break;
   }
   case    ENC_CAPI_TYPE_DEPACKETIZER:
   {
      //check if CAPI is present in AMDB.
      capi_init_param.enc_cfg_id = AVS_MODULE_ID_DEPACKETIZER_IEC61937;
      enc_AMDB_presence amdb_presence = audio_enc_svc_get_amdb_presence(enc_capi_type, capi_init_param.enc_cfg_id, 0, &capi_init_param.amdb_handle);

      if(ENC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMDB CAPI V2 found for fmt 0x%lx. Trying to create CAPI V2", enc_cfg_id);
         result = audio_enc_svc_create_init_depack_capi_v2(&pMe->capiContainer[capi_index]->capi_ptr, pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "module 0x%lx is present as an unsupported type in AMDB, failing capi creation.", AVS_MODULE_ID_DEPACKETIZER_IEC61937);
         result = ADSP_EFAILED;
      }

      //typically enc capi don't raise output media fmt. so assign default values.
      pMe->capiContainer[capi_index]->out_med_fmt.main.format_header.data_format = CAPI_V2_RAW_COMPRESSED;
      pMe->capiContainer[capi_index]->out_med_fmt.raw_fmt.bitstream_format = enc_cfg_id;

      break;
   }
   case    ENC_CAPI_TYPE_PACKETIZER:
   {
      //check if CAPI is present in AMDB.
      enc_AMDB_presence amdb_presence = audio_enc_svc_get_amdb_presence(enc_capi_type, capi_init_param.enc_cfg_id, 0, &capi_init_param.amdb_handle);

      if(ENC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMDB CAPI V2 found for fmt 0x%lx. Trying to create CAPI V2", enc_cfg_id);
         result = audio_enc_svc_create_init_pack_capi_v2(&(pMe->capiContainer[capi_index]->capi_ptr), pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "module 0x%lx is present as an unsupported type in AMDB, failing capi creation.", capi_init_param.enc_cfg_id);
         result = ADSP_EFAILED;
      }

      //typically enc capi don't raise output media fmt. so assign default values.
      pMe->capiContainer[capi_index]->out_med_fmt.main.format_header.data_format = CAPI_V2_IEC61937_PACKETIZED;
      pMe->capiContainer[capi_index]->out_med_fmt.std.bitstream_format = capi_init_param.enc_cfg_id;
      pMe->capiContainer[capi_index]->out_med_fmt.std.bits_per_sample = pInitParams->bits_per_sample;
      break;
   }
   case  ENC_CAPI_TYPE_COP_PACKETIZER:
   {
      //check if CAPI is present in AMDB.
      enc_AMDB_presence amdb_presence = audio_enc_svc_get_amdb_presence(ENC_CAPI_TYPE_PACKETIZER, pInitParams->cop_packetizer_module_id, 0, &capi_init_param.amdb_handle);

      if(ENC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2 == amdb_presence)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Trying to create COP packetizer CAPI V2 module-id 0x%lx ", pInitParams->cop_packetizer_module_id);
         result = audio_enc_svc_create_init_cop_pack_capi_v2(&pMe->capiContainer[capi_index]->capi_ptr, pInitParams, &capi_init_param);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "cop packetizer module 0x%lx is present as an unsupported type in AMDB, failing capi creation.", pInitParams->cop_packetizer_module_id);
         result = ADSP_EFAILED;
      }

      //typically enc capi don't raise output media fmt. so assign default values.
      pMe->capiContainer[capi_index]->out_med_fmt.main.format_header.data_format = CAPI_V2_COMPR_OVER_PCM_PACKETIZED;
      pMe->capiContainer[capi_index]->out_med_fmt.std.bitstream_format = capi_init_param.enc_cfg_id; //same bit stream as main CAPI
      break;
   }
   default:
   {
      return ADSP_EBADPARAM;
   }
   }

   if (ADSP_FAILED(result) || (NULL == (pMe->capiContainer[capi_index]->capi_ptr)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cannot create encoder CAPI at index %u or no memory. result %d", capi_index, result);
      return result;
   }

#ifdef DBG_BUFFER_ADDRESSES
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR Enc obj: 0x%8x",
         pMe->capiContainer[capi_index]->enc_capi_ptr);
#endif /* DBG_BUFFER_ADDRESSES */


   //store the capi destroy function
   pMe->capiContainer[capi_index]->enc_destroy_fn = capi_init_param.enc_destroy_fn;

   init_time_get_params.input_port_index.index = 0;
   init_time_get_params.input_port_index.valid = true;
   init_time_get_params.output_port_index.index = 0;
   init_time_get_params.output_port_index.valid = true;
   result = audio_enc_svc_get_init_time_properties(pMe->capiContainer[capi_index]->capi_ptr, &init_time_get_params, pMe->capiContainer[capi_index]);

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc error in getting properties");
      return result;
   }


   //keep the largest stack size needed.
   if (init_time_get_params.stack_size.size_in_bytes > enc_params->max_stack_size)
   {
      enc_params->max_stack_size = init_time_get_params.stack_size.size_in_bytes;
   }

   pMe->out_med_fmt_per_enc_cfg.bits_per_sample = pInitParams->bits_per_sample; //init with this, later enc cfg can change it
   pMe->out_med_fmt_per_enc_cfg.bytes_per_sample =  (pInitParams->bits_per_sample > 16) ?
         BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   pMe->capiContainer[capi_index]->in_port_thresh = init_time_get_params.in_port_thresh.threshold_in_bytes;
   pMe->capiContainer[capi_index]->out_port_thresh = init_time_get_params.out_port_thresh.threshold_in_bytes;

   //initialize output buffer params
   EncSvc_OutBufParams_t *pOutBufParams = &pMe->capiContainer[capi_index]->out_buf_params;
   if (is_last_capi)
   {
      pOutBufParams->bFrameMetaDataEnable = pInitParams->bFrameMetaDataEnable;
   }
   else
   {
      pOutBufParams->bFrameMetaDataEnable = FALSE;
   }

   if (pMe->no_apr_enc)
   {
      pOutBufParams->bFrameMetaDataEnable = FALSE;
   }

   pOutBufParams->usTargetFramesPerBuf = 0;

   if(enc_capi_type == ENC_CAPI_TYPE_DEPACKETIZER)
   {
      pMe->capiContainer[capi_index]->out_buf_params.usTargetFramesPerBuf = pInitParams->frames_per_buffer;
   }

   if (pMe->no_apr_enc)
   {
      pMe->capiContainer[capi_index]->out_buf_params.usTargetFramesPerBuf = 1;
   }

   AudioEncSvc_HandleFwkExtnAtInit(pMe, pMe->capiContainer[capi_index]);

   return ADSP_EOK;
}

ADSPResult AudioEncSvc_UpdatePcmEncCfg(AudioEncSvc_t *pMe, void* pPcmConfig, bool_t isNativeModeAllowed)
{
   ADSPResult result = ADSP_EOK;

   enc_CAPI_container_t *main_capi = pMe->mainCapiContainer;

   asm_multi_channel_pcm_enc_cfg_v2_t *pPcmConfig_v2 = (asm_multi_channel_pcm_enc_cfg_v2_t*)pPcmConfig;

   //Check the common parameters of v2, v3 and v4
   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PCM Enc Cfg 0x%lx. ch=%u,SR=%lu,bps=%u, native mode allowed %d", main_capi->enc_cfg_id,
         pPcmConfig_v2->num_channels, pPcmConfig_v2->sample_rate, pPcmConfig_v2->bits_per_sample, isNativeModeAllowed);

   MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PCM Enc cfg channel mapping %u,%u,%u,%u,%u,%u,%u,%u", pPcmConfig_v2->channel_mapping[0], pPcmConfig_v2->channel_mapping[1],
         pPcmConfig_v2->channel_mapping[2], pPcmConfig_v2->channel_mapping[3], pPcmConfig_v2->channel_mapping[4], pPcmConfig_v2->channel_mapping[5],
         pPcmConfig_v2->channel_mapping[6], pPcmConfig_v2->channel_mapping[7]);

   if ((!pPcmConfig_v2->is_signed) || (pPcmConfig_v2->num_channels > 8)
         || ((!isNativeModeAllowed) && (pPcmConfig_v2->num_channels == 0)) || ((!isNativeModeAllowed) && (pPcmConfig_v2->sample_rate == 0)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error:Received invalid parameters in PCM config block");
      return ADSP_EBADPARAM;
   }

   pMe->out_med_fmt_per_enc_cfg.endianness = ENC_LITTLE_ENDIAN;

   uint16_t pcmOutBytesPerSample;
   if (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 == main_capi->enc_cfg_id)
   {
      if((16 == pPcmConfig_v2->bits_per_sample) || (24 == pPcmConfig_v2->bits_per_sample))
      {
         pcmOutBytesPerSample = (pPcmConfig_v2->bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: Received invalid bits per sample_[%lu].",
               pPcmConfig_v2->bits_per_sample);
         return ADSP_EBADPARAM;
      }
   }
   else if (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 == main_capi->enc_cfg_id)
   {
      asm_multi_channel_pcm_enc_cfg_v3_t *pPcmConfig_v3 = (asm_multi_channel_pcm_enc_cfg_v3_t*)pPcmConfig;
      if(((16 == pPcmConfig_v3->bits_per_sample) || (24 == pPcmConfig_v3->bits_per_sample)) &&
            (pPcmConfig_v3->sample_word_size >= pPcmConfig_v3->bits_per_sample))
      {
         uint32_t bytesPerSample = pPcmConfig_v3->sample_word_size / 8;
         if ((bytesPerSample * 8 != pPcmConfig_v3->sample_word_size) || (bytesPerSample > 4))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "sample word size %u not byte aligned or is > 32.", pPcmConfig_v3->sample_word_size);
            return ADSP_EBADPARAM;
         }
         pcmOutBytesPerSample = bytesPerSample;
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sample word size is %u.", pPcmConfig_v3->sample_word_size);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: received invalid bits per sample_[%lu], or sample word size [%lu].",
               pPcmConfig_v3->bits_per_sample, pPcmConfig_v3->sample_word_size);
         return ADSP_EBADPARAM;
      }
   }
   else if (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 == main_capi->enc_cfg_id)
   {
      asm_multi_channel_pcm_enc_cfg_v4_t *pPcmConfig_v4 = (asm_multi_channel_pcm_enc_cfg_v4_t*)pPcmConfig;
      if(((16 == pPcmConfig_v4->bits_per_sample) || (24 == pPcmConfig_v4->bits_per_sample) || (32 == pPcmConfig_v4->bits_per_sample)) &&
            (pPcmConfig_v4->sample_word_size >= pPcmConfig_v4->bits_per_sample))
      {
         uint32_t bytesPerSample = pPcmConfig_v4->sample_word_size / 8;
         if ((bytesPerSample * 8 != pPcmConfig_v4->sample_word_size) || (bytesPerSample > 4))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "sample word size %u not byte aligned or is > 32.", pPcmConfig_v4->sample_word_size);
            return ADSP_EBADPARAM;
         }
         pcmOutBytesPerSample = bytesPerSample;

         if((pPcmConfig_v4->endianness != 0) && (pPcmConfig_v4->endianness != 1))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "endianness type %u not supported.", pPcmConfig_v4->endianness);
            return ADSP_EBADPARAM;
         }

         pMe->out_med_fmt_per_enc_cfg.endianness = (pPcmConfig_v4->endianness == 0) ? ENC_LITTLE_ENDIAN : ENC_BIG_ENDIAN;
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sample word size is %u, Endianness type is %u (0-little, 1-big)",
               pPcmConfig_v4->sample_word_size, pPcmConfig_v4->endianness);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: received invalid bits per sample_[%lu], or sample word size [%lu].",
               pPcmConfig_v4->bits_per_sample, pPcmConfig_v4->sample_word_size);
         return ADSP_EBADPARAM;
      }
   }
   else
   {
      return ADSP_EBADPARAM;
   }

   pMe->out_med_fmt_per_enc_cfg.bits_per_sample = pPcmConfig_v2->bits_per_sample;
   pMe->out_med_fmt_per_enc_cfg.bytes_per_sample = pcmOutBytesPerSample;
   pMe->out_med_fmt_per_enc_cfg.num_channels = pPcmConfig_v2->num_channels;
   pMe->out_med_fmt_per_enc_cfg.sample_rate = pPcmConfig_v2->sample_rate;
   memscpy(pMe->out_med_fmt_per_enc_cfg.chan_map, sizeof(pMe->out_med_fmt_per_enc_cfg.chan_map), pPcmConfig_v2->channel_mapping, sizeof(pPcmConfig_v2->channel_mapping));

   return result;
}

/**
 * when enc cfg block is received output media is initialized as follows.
 * later on CAPI may raise an event & output media fmt may get updated.
 * in COP packetizer, the media fmt is assigned right during creation, in order to prevent overwriting 0 check is made below.
 *
 * this implementation serves initialization of output media fmt if CAPI doesn't raise event.
 */
static void init_all_capi_out_media_fmt_at_enc_cfg(AudioEncSvc_t *pMe)
{
   for (uint8_t i = 0; i < ENC_SVC_MAX_CAPI; i++)
   {
      if (NULL == pMe->capiContainer[i])
      {
         break;
      }

      if ( (CAPI_V2_FIXED_POINT == pMe->capiContainer[i]->out_med_fmt.main.format_header.data_format) ||
            (CAPI_V2_IEC61937_PACKETIZED == pMe->capiContainer[i]->out_med_fmt.main.format_header.data_format) ||
            (CAPI_V2_COMPR_OVER_PCM_PACKETIZED == pMe->capiContainer[i]->out_med_fmt.main.format_header.data_format))
      {
         if (0 == pMe->capiContainer[i]->out_med_fmt.std.bits_per_sample)
         {
            pMe->capiContainer[i]->out_med_fmt.std.bits_per_sample = pMe->out_med_fmt_per_enc_cfg.bits_per_sample;
         }

         if (0 == pMe->capiContainer[i]->out_med_fmt.std.num_channels)
         {
            pMe->capiContainer[i]->out_med_fmt.std.num_channels = pMe->out_med_fmt_per_enc_cfg.num_channels;
            memscpy(pMe->capiContainer[i]->out_med_fmt.std.channel_type, sizeof(pMe->capiContainer[i]->out_med_fmt.std.channel_type),
                  pMe->out_med_fmt_per_enc_cfg.chan_map, sizeof(pMe->out_med_fmt_per_enc_cfg.chan_map));
         }

         if (0 == pMe->capiContainer[i]->out_med_fmt.std.sampling_rate)
         {
            pMe->capiContainer[i]->out_med_fmt.std.sampling_rate = pMe->out_med_fmt_per_enc_cfg.sample_rate;
         }

         pMe->capiContainer[i]->out_med_fmt.std.data_interleaving = CAPI_V2_DEINTERLEAVED_PACKED;
         pMe->capiContainer[i]->out_med_fmt.std.data_is_signed = 1;
         pMe->capiContainer[i]->out_med_fmt.std.q_factor = pMe->out_med_fmt_per_enc_cfg.bytes_per_sample == BYTES_PER_SAMPLE_TWO ? PCM_16BIT_Q_FORMAT: ELITE_32BIT_PCM_Q_FORMAT;
      }
   }
}

/*
 *Function Name:AudioEncSvc_UpdateEncConfig
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *            asm_enc_cfg_blk_param_v2_t *pEncCfgParam: Encoder config params
 *
 *Description: Updates encoder CAPI with encoder config sent by client

 * Encoder config is sent for encoders only (main capi). not for packetizers or depacketizers.
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_UpdateEncConfig(AudioEncSvc_t *pMe, uint8_t *payload, uint32_t payload_size, uint32_t frames_per_buf)
{
   enc_CAPI_container_t *main_capi = pMe->mainCapiContainer;
   enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

   //ignore frames per buf if we are not going to send to APR client.
   if (pMe->no_apr_enc)
   {
      last_capi->out_buf_params.usTargetFramesPerBuf = 1;
   }
   else
   {
      //TODO: this kind of hacks won't work fine with dynamic loading.
      if ( (ASM_MEDIA_FMT_WMA_V8 == main_capi->enc_cfg_id) ||
            AudioEncSvc_IsPcmFmt(main_capi->enc_cfg_id))
      {
         //For WMA entire output buffer consists multiple frames packed into single packet
         //So number of packets per buffer is always 1
         main_capi->out_buf_params.usTargetFramesPerBuf = 1;
      }
      else
      {
         last_capi->out_buf_params.usTargetFramesPerBuf = frames_per_buf;
      }
   }

   //read the format blocks
   uint32_t ulEncCfgId = main_capi->enc_cfg_id;
   uint8_t* pConfigBlk = payload;
   int32_t result = ADSP_EOK;

   switch (ulEncCfgId)
   {
   case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2:
   case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3:
   case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4:
   {
      result = AudioEncSvc_UpdatePcmEncCfg(pMe, (void*)pConfigBlk, TRUE); //native mode allowed.
      if (ADSP_EOK != result)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to set encoder config block for pcm");
         return result;
      }
      break;
   }
   default:
   {
      result = AudioEncSvc_UpdateNonPcmEncCfg(pMe, (void*)pConfigBlk);
      if (ADSP_EOK != result)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to set encoder config block for non-pcm");
         return result;
      }
      break;
   }
   }

   if ( 0 == pMe->out_med_fmt_per_enc_cfg.sample_rate )
   {
      pMe->isNativeSamplerate = true;
   }
   if  (0 == pMe->out_med_fmt_per_enc_cfg.num_channels)
   {
      pMe->isNativeChannels = true;
   }

   //initialize output media type (for PCM, packetized) to be same as out_med_fmt_per_enc_cfg
   //after this point, CAPI out-media-fmt event may change it
   init_all_capi_out_media_fmt_at_enc_cfg(pMe);

   //TODO: the above switch case will go once we have asm_custom_enc_cfg_t-like struct for all fmts.

   if(!AudioEncSvc_IsPcmFmt(ulEncCfgId))
   {
      result = aud_enc_svc_set_param(main_capi->capi_ptr, CAPI_PARAM_ENC_CFG_BLK,
            (int8_t*)payload, payload_size);

      if (ADSP_EOK != result)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to set encoder config block params!");
         if (result == ADSP_EUNSUPPORTED)
         {
            result = ADSP_EOK;
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Encoder doesn't support setting encoder cfg block!!! Ignoring the error!!!");
         }
      }
   }

   AudioEncSvc_ProcessKppsBw(pMe, FALSE, TRUE);

   AudioEncSvc_HandlePortDataThreshChangeEvent(pMe);

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "UpdateEncConfig completed with isNativeChannels %u, isNativeSamplerate %lu, out_channels %lu, out_sampling_rate %lu",
         pMe->isNativeChannels, pMe->isNativeSamplerate, pMe->out_med_fmt_per_enc_cfg.num_channels, pMe->out_med_fmt_per_enc_cfg.sample_rate);


   return result;
}

/*
 *Function Name:AudioEncSvc_SetParam
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Processes encoder configuration block
 *
 *Returns: ADSPResult
 *
 */

ADSPResult AudioEncSvc_SetParam(AudioEncSvc_t* pMe)
{
   //check the param ID first
   elite_msg_param_header_t *pParamHdr = (elite_msg_param_header_t *) pMe->CmdMsg.pPayload;
   uint32_t ulParamId = pParamHdr->unParamId;
   int32_t result = ADSP_EOK,capi_index=0;

   enc_CAPI_container_t *main_capi = pMe->mainCapiContainer;

   switch (ulParamId)
   {
   case ELITEMSG_PARAM_ID_CAL:
   {
      elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->CmdMsg.pPayload;
      asm_stream_cmd_set_encdec_param_t *pEncDecParam = (asm_stream_cmd_set_encdec_param_t*) pParamMsg->pnParamData;
      int8_t *pSetParamPayload = ((int8_t*) pEncDecParam + sizeof(asm_stream_cmd_set_encdec_param_t));

      switch (pEncDecParam->param_id)
      {
      case ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK_V2:
      {
         if (pEncDecParam->param_size < sizeof(asm_enc_cfg_blk_param_v2_t))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SET_PARAM_ENC_CFG param size not big enough!");
            result = ADSP_EBADPARAM;
         }
         if (pMe->bWaitForEncCfg)
         {
            asm_enc_cfg_blk_param_v2_t *pEncCfgBlk = (asm_enc_cfg_blk_param_v2_t*) pSetParamPayload;
            ADSPResult res = AudioEncSvc_UpdateEncConfig(pMe, ((uint8_t*)pEncCfgBlk)+sizeof(asm_enc_cfg_blk_param_v2_t), pEncCfgBlk->enc_cfg_blk_size, pEncCfgBlk->frames_per_buf);
            if (ADSP_EOK == res)
            {
               pMe->bWaitForEncCfg = FALSE;
               //start listening to cmd and data queues now
               pMe->unCurrentBitfield = AUD_ENC_CMD_SIG | AUD_ENC_DATA_SIG;
               //ack the msg
               result = ADSP_EOK;
            }
            else
            {
               result = res;
            }
         }
         else
         {
            //cannot process enc cfg more than once
            result = ADSP_ENOTREADY;
         }
         break;
      }
      case ASM_PARAM_ID_ENCDEC_BITRATE:
      {
         if (pMe->bWaitForEncCfg)
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudEncSvc cannot change bitrate without encoder config!");
            result = ADSP_ENOTREADY;
         }
         else
         {
            result = aud_enc_svc_set_param(main_capi->capi_ptr, pEncDecParam->param_id, pSetParamPayload, pEncDecParam->param_size);
            //don't flush buffers as that would cause discontinuity.
         }
         break;
      }
      default:
      {
         if(main_capi->capi_ptr != NULL)
         {
            //Pass on to let encoder CAPI decide if this is supported
            result = aud_enc_svc_set_param(main_capi->capi_ptr, pEncDecParam->param_id, pSetParamPayload, pEncDecParam->param_size);
         }
      }
      break;
      }
      break;
   }
   case AVS_ENCODER_PARAM_ID_ENC_CFG_BLK:
   {
      elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->CmdMsg.pPayload;
      avs_enc_cfg_blk_param_t *pEncDecParam = (avs_enc_cfg_blk_param_t*) pParamMsg->pnParamData;

      if (pMe->bWaitForEncCfg)
      {
         ADSPResult res = AudioEncSvc_UpdateEncConfig(pMe, ((uint8_t*)pEncDecParam)+sizeof(avs_enc_cfg_blk_param_t), pEncDecParam->enc_cfg_blk_size, 1);
         if (ADSP_EOK == res)
         {
            pMe->bWaitForEncCfg = FALSE;
            //start listening to cmd and data queues now
            pMe->unCurrentBitfield = AUD_ENC_CMD_SIG | AUD_ENC_DATA_SIG;
            //ack the msg
            result = ADSP_EOK;
         }
         else
         {
            result = res;
         }
      }
      else
      {
         //cannot process enc cfg more than once
         result = ADSP_ENOTREADY;
      }
      break;
   }
   case AVS_ENCODER_PARAM_ID_ENC_BITRATE:
   {
      elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->CmdMsg.pPayload;
      if (pMe->bWaitForEncCfg)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudEncSvc cannot change bitrate without encoder config!");
         result = ADSP_ENOTREADY;
      }
      else
      {
         if (pParamMsg->unSize >= sizeof(avs_enc_bitrate_param_t))
         {
            //transalte to ASM structs because CAPIs may not understand the AVS struct.
            avs_enc_bitrate_param_t *pBitRate = (avs_enc_bitrate_param_t*)pParamMsg->pnParamData;
            asm_bitrate_param_t asmBitRate;
            asmBitRate.bitrate = pBitRate->enc_bitrate;

            result = aud_enc_svc_set_param(main_capi->capi_ptr, ASM_PARAM_ID_ENCDEC_BITRATE, (int8_t*)&asmBitRate, sizeof(asm_bitrate_param_t));
            //don't flush buffers as that would cause discontinuity.
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudEncSvc bit rate size doesn't match");
            result = ADSP_EBADPARAM;
         }
      }
      break;
   }
   case AVS_ENCDEC_PARAM_ID_DEC_TO_ENC_COMMUNICATION:
   {
	  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVS_ENCDEC_PARAM_ID_DEC_TO_ENC_COMMUNICATION set param encoder");
      elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->CmdMsg.pPayload;
      if (pParamMsg->unSize < sizeof(dec_to_enc_communication_payload_t))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: enc_set_param()- received wrong key size for IMC communication");
         result = ADSP_EFAILED;
         break;
      }
      result = AudioEncSvc_IMCCheckInit(pMe, pParamMsg);
      break;
   }
   case AVS_ENCODER_PARAM_ID_BIT_RATE_LEVEL_MAP:
   {
	   elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->CmdMsg.pPayload;
	   if (pParamMsg->unSize < sizeof(bitrate_level_map_payload_t))
	   {
	      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: enc_set_param()- received wrong bit rate map size for IMC communication");
		  result = ADSP_EFAILED;
		  break;
	    }
       result = aud_enc_svc_set_param(main_capi->capi_ptr, ulParamId,  (int8_t*)pParamMsg->pnParamData, pParamMsg->unSize);
	   break;
   }
   case AVS_ENCODER_PARAM_ID_FRM_SIZE_CONTROL:
   {
	   elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->CmdMsg.pPayload;
	   if (pParamMsg->unSize < sizeof(avs_enc_frame_size_control_param_t))
	   {
	      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: enc_set_param()- received wrong frame size control parameter size");
		  result = ADSP_EFAILED;
		  break;
	    }
       result = aud_enc_svc_set_param(main_capi->capi_ptr, ulParamId,  (int8_t*)pParamMsg->pnParamData, pParamMsg->unSize);
	   break;
   }
   default:
   {
      elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->CmdMsg.pPayload;
      //Pass on to let encoder CAPI decide if this is supported
       for(capi_index=0; capi_index < ENC_SVC_MAX_CAPI; capi_index++)
       {
		   if ((pMe->capiContainer[capi_index] != NULL ) && (pMe->capiContainer[capi_index]->capi_ptr != NULL ))
		   {
			   result = aud_enc_svc_set_param(pMe->capiContainer[capi_index]->capi_ptr, ulParamId, (int8_t*)pParamMsg->pnParamData, pParamMsg->unSize);
		   }
	   }
   }
   }
   return elite_msg_finish_msg(&pMe->CmdMsg, result);
}

/*
 *Function Name:AudioEncSvc_ResetTimeStampValues
 *
 *Parameters: TimeStampParams_t* pTS: Instance of timestamp structure
 *
 *Description: Initialize timestamp parameters
 *
 *Returns: none
 *
 */
void AudioEncSvc_ResetTimeStampValues(TimeStampParams_t* pTS)
{
   //Initialize the timestamp value to some huge number
   pTS->ullLastBufferTimeStamp = AUD_MAX_INT64;
   pTS->ullTimeStamp = AUD_MAX_INT64;
   pTS->isTimeStampValid = FALSE;
   pTS->isLastBufferTimeStampValid = FALSE;
   pTS->timestamp_mismatch = FALSE;
   pTS->ullExpectedTimeStamp = 0;
   pTS->timestamp_offset = 0;
}

/*
 *Function Name: AudioEncSvc_GetOutputBuffer
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *            elite_apr_packet_t ** ppAprPacket: pointer to
 *                                   APR packet handle
 *            asm_data_cmd_read_v2_t ** ppOutBuf: pointer to
 *                                   output buffer handle
 *
 *Description: Returns the APR packet handle and output buffer handle
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_GetOutputBuffer(AudioEncSvc_t* pMe, elite_apr_packet_t ** ppAprPacket, asm_data_cmd_read_v2_t ** ppOutBuf)
{
   QURT_ELITE_ASSERT(ppAprPacket);
   QURT_ELITE_ASSERT(ppOutBuf);

   *ppAprPacket = (elite_apr_packet_t *) (pMe->OutDataMsg.pPayload);
   *ppOutBuf = NULL;

   elite_apr_if_get_payload((void**) (ppOutBuf), *ppAprPacket);
   if (NULL == *ppOutBuf)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "No output buffer to write encoded streams");
      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}

/*
 *Function Name: AudioEncSvc_FillMetaDataInOutputBuffer
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *            uint32_t ulInpPcmSize: Input PCM size consumed for encoding the current frame
 *            uint32_t ulEncOutSize: Output frame size of encoder in bytes
 *            pointer to Read Payload
 *
 *Description: Fills the metaframe info in the output buffer
 *             uint32_t offset;           // Offset to the encoded frame
 *             uint32_t ulEncodePcmSamples; // Number of PCM samples per channel encoded in this frame
 *             uint32_t timestamp_msw;     //Upper 32 bits of 64-bit session time of first sample in buffer, in microseconds
 *             uint32_t timestamp_lsw;     //Lower 32 bits of 64-bit session time of first sample in buffer, in microseconds
 *             uint32_t flags;            //Timestamp validity is set in bit 31
 *
 *             Used incase multiple frames are embedded in a single client output buffer
 */
void AudioEncSvc_FillMetaDataInOutputBuffer(AudioEncSvc_t* pMe, uint32_t ulInpPcmSize, uint32_t ulEncOutSize, asm_data_cmd_read_v2_t *pEncOut,
      uint32_t unOutBufWrOffset)
{
   enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);
   //Get the pointer to Enc Svc IO buf params
   EncSvc_OutBufParams_t *pencBufParams = &last_capi->out_buf_params;
   uint16_t usFrameNum = pencBufParams->usNumFramesInOutBuf;
   //Add Metadata for the successfully encoded frame, move it to a function
   asm_data_read_buf_metadata_v2_t *pMetaFrameInfo = (asm_data_read_buf_metadata_v2_t*) (pMe->SharedMemMapTypeNode.unVirtAddr
         + usFrameNum * sizeof(asm_data_read_buf_metadata_v2_t));
   //Offset at which this frame data is present in buffer
   pMetaFrameInfo->offset = unOutBufWrOffset;
   //Encoded bytes correspoding to this frame
   pMetaFrameInfo->frm_size = ulEncOutSize;
   //PCM samples cosumed to encode the current frame
   pMetaFrameInfo->num_encoded_pcm_samples = ulInpPcmSize;
   //timestamp for this frame is where last frame encoding left off
   pMetaFrameInfo->timestamp_lsw = (uint32_t) pMe->encTSParams.ullFrameTimeStamp;
   pMetaFrameInfo->timestamp_msw = (uint32_t) (pMe->encTSParams.ullFrameTimeStamp >> 32);
   //Set the flags to indicate timstamp field
   pMetaFrameInfo->flags = 0;
   asm_set_timestamp_valid_flag(&(pMetaFrameInfo->flags), pMe->encTSParams.isTimeStampValid);

   return;
}

/*
 *Function Name: AudioEncSvc_FillMetaDataCompressedInOutputBuffer
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *            uint32_t ulInpPcmSize: Input PCM size consumed for encoding the current frame
 *            uint32_t ulEncOutSize: Ouput frame size of encoder in bytes
 *            sm_data_cmd_read_t *pEncOut : pointer to Read Payload
 *
 *Description: Fills the metaframe info in the output buffer
 *             uint32_t format_id;        //format ID of the compressed data*
 *             uint32_t data_offset;      // Offset to the encoded frame
 *             uint32_t data_size;        // size of the encoded frame
 *             uint32_t command_offset;   // offset for command
 *             uint32_t command_size;     // command size
 *             uint32_t num_encoded_pcm_samples; // Number of PCM samples per channel encoded in this frame
 *             uint32_t timestamp_msw;     //Upper 32 bits of 64-bit session time of first sample in buffer, in microseconds
 *             uint32_t timestamp_lsw;     //Lower 32 bits of 64-bit session time of first sample in buffer, in microseconds
 *             uint32_t flags;            //Timestamp validity is set in bit 31
 *
 *             Used incase multiple frames are embedded in a single client output buffer
 */
void AudioEncSvc_FillMetaDataCompressedInOutputBuffer(AudioEncSvc_t* pMe, uint32_t ulInpPcmSize, uint32_t ulEncOutSize, asm_data_cmd_read_v2_t *pEncOut,
      uint32_t unOutBufWrOffset)
{
   enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);
   //Get the pointer to Enc Svc IO buf params
   EncSvc_OutBufParams_t *pencBufParams = &last_capi->out_buf_params;
   uint16_t usFrameNum = pencBufParams->usNumFramesInOutBuf;
   asm_data_read_buf_metadata_compressed_v2_t *pMetaFrameInfo = (asm_data_read_buf_metadata_compressed_v2_t *) (pMe->SharedMemMapTypeNode.unVirtAddr
         + usFrameNum * sizeof(asm_data_read_buf_metadata_compressed_v2_t));

   //initialize to 0
   memset(pMetaFrameInfo, 0, sizeof(asm_data_read_buf_metadata_compressed_v2_t));

   pMetaFrameInfo->bitstream_id = 0;
   //for now only supporting one stream
   pMetaFrameInfo->format_id = last_capi->out_med_fmt.raw_fmt.bitstream_format;
   //Offset at which this frame data is present in buffer
   pMetaFrameInfo->data_offset = unOutBufWrOffset;
   //Encoded bytes correspoding to this frame
   pMetaFrameInfo->data_size = ulEncOutSize;
   //PCM samples cosumed to encode the current frame
   pMetaFrameInfo->num_encoded_pcm_samples = ulInpPcmSize;
   //timestamp for this frame is where last frame encoding left off
   pMetaFrameInfo->timestamp_lsw = (uint32_t) pMe->encTSParams.ullFrameTimeStamp;
   pMetaFrameInfo->timestamp_msw = (uint32_t) (pMe->encTSParams.ullFrameTimeStamp >> 32);
   //Set the flags to indicate timstamp field
   pMetaFrameInfo->flags = 0;
   asm_set_timestamp_valid_flag(&(pMetaFrameInfo->flags), pMe->encTSParams.isTimeStampValid);

   return;
}

/*
 *Function Name: AudioEncSvc_ReleaseOutputBuffer
 *
 *Parameters: AudioEncSvc_t* pMe: encoder service state
 *            uint32_t ulOutBufSize: output buffer size to be filled in ack msg
 *            ADSPResult errCode: error code to fill in ack msg
 *
 *Description: Sends a read_done event to client and free the APR message
 *             coming from client
 *
 *Returns: ADSPResult
 *
 */
//#define AUDENCDEC_TIMESTAMP_LOGS
ADSPResult AudioEncSvc_ReleaseOutputBuffer(AudioEncSvc_t* pMe, uint32_t ulOutBufSize, ADSPResult errCode)
{
   uint32_t offset = 0;
   elite_apr_packet_t *pAprPacket;
   asm_data_cmd_read_v2_t *pEncOut;
   AudioEncSvc_GetOutputBuffer(pMe, &pAprPacket, &pEncOut);

   enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

   asm_data_event_read_done_v2_t readDoneEvt;

#ifdef AUDENCDEC_TIMESTAMP_LOGS
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Enc: Outgoing buffer TS: %lu %lu. valid=%d, ulOutBufSize=%ld",
         (uint32_t)(pMe->encTSParams.ullTimeStamp>>32),
         (uint32_t)pMe->encTSParams.ullTimeStamp, pMe->encTSParams.isTimeStampValid,ulOutBufSize);
#endif

   //fill up buf addr
   readDoneEvt.buf_addr_lsw = pEncOut->buf_addr_lsw;
   readDoneEvt.buf_addr_msw = pEncOut->buf_addr_msw;
   readDoneEvt.mem_map_handle = pEncOut->mem_map_handle;

   //Fill time stamp info and timestamp flag
   readDoneEvt.timestamp_lsw = pMe->encTSParams.ullTimeStamp;
   readDoneEvt.timestamp_msw = (pMe->encTSParams.ullTimeStamp >> 32);

   readDoneEvt.flags = 0;

   if (last_capi->out_buf_params.bFrameMetaDataEnable)
   {
      elite_apr_packet_t *pAprPacket = (elite_apr_packet_t*) (pMe->OutDataMsg.pPayload);
      uint32_t uAprOpCode = elite_apr_if_get_opcode(pAprPacket);

      uint32_t metadata_size = sizeof(asm_data_read_buf_metadata_v2_t);
      if (ASM_DATA_CMD_READ_COMPRESSED_V2 == uAprOpCode)
      {
         metadata_size = sizeof(asm_data_read_buf_metadata_compressed_v2_t);
      }

      //If metadata is requested calculate non zero offset and its flag
      offset = (last_capi->out_buf_params.usTargetFramesPerBuf) * metadata_size;
      asm_set_frame_metadata_flag(&readDoneEvt.flags, 1);
   }

   //fill offset and encoded bytes size
   readDoneEvt.offset = offset;

   if (ulOutBufSize >= offset)
   {
      readDoneEvt.enc_frames_total_size = (ulOutBufSize - offset);
   }
   else
   {
      readDoneEvt.enc_frames_total_size = 0;
   }

   int8_t * buf_addr = (int8_t *) (pMe->SharedMemMapTypeNode.unVirtAddr) + offset;
   uint32_t buf_size = readDoneEvt.enc_frames_total_size;

   /* Call to log the output encoded bit-stream data */
   if (buf_size)
   {
      AudioEncSvc_LogOutputData(buf_addr, buf_size, pMe);
   }

   //copy the UID present in data read command
   readDoneEvt.seq_id = pEncOut->seq_id;

   //Fill number of frames filled in the client buffer
   readDoneEvt.num_frames = last_capi->out_buf_params.usNumFramesInOutBuf;
   if (!readDoneEvt.enc_frames_total_size)
   {
      readDoneEvt.num_frames = 0;
   }
   else if (AudioEncSvc_IsPcmFmt(last_capi->enc_cfg_id))
   {
      //PCM always contains one frame per buffer irrespective
      //of encoder config setting
      readDoneEvt.num_frames = 1;
   }

   //time stamp is valid only if we have any encoded data
   asm_set_timestamp_valid_flag(&readDoneEvt.flags, pMe->encTSParams.isTimeStampValid);
   if (!readDoneEvt.num_frames || !readDoneEvt.enc_frames_total_size)
   {
      asm_set_timestamp_valid_flag(&readDoneEvt.flags, 0);
   }

   //fix this typecast; may be status field needs to be changed to lStatus in ISOD
   readDoneEvt.status = (uint32_t) errCode;

   if (0 != pMe->SharedMemMapTypeNodeBackUp.unVirtAddr)
   {
      elite_mem_map_release_shm(&(pMe->SharedMemMapTypeNodeBackUp));

      memset(&pMe->SharedMemMapTypeNodeBackUp, 0, sizeof(elite_mem_shared_memory_map_t));
   }

   //Return SC buffer to client. ASM_DATA_CMD_READ_V2, ack opCode will be filled by ASM function
   ADSPResult result = AudioStreamMgr_GenerateAck(pAprPacket, errCode, (void*) &readDoneEvt, sizeof(readDoneEvt), 0);

   //set payload of gpQMsg to null to indicate that we are not holding on to any
   //output buffer
   pMe->OutDataMsg.pPayload = NULL;

   last_capi->out_buf_params.out_buf.actual_data_len = 0;
   last_capi->out_buf_params.out_buf.max_data_len = 0;
   last_capi->out_buf_params.out_buf.data_ptr = NULL;

   pMe->buf_done_cnt++;

   return result;
}

/**
 * note: SendOutBufToPeerSvc is called from AudioEncSvc_PushInitialZeros
 * where some fields in pMe may need to be left intact.
 * if any new field is reset in this function, then remember to back it up in pushInitialZeros.
 */
ADSPResult AudioEncSvc_SendOutBufToPeerSvc(AudioEncSvc_t* pMe)
{
   ADSPResult nResult = ADSP_EOK;

   enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

   elite_msg_data_buffer_t *pOutBuf = (elite_msg_data_buffer_t*) (pMe->outDataBufferNode.pBuffer);

   if (pOutBuf == NULL) return ADSP_EOK;

   // send the output buffer downstream
   AudioEncSvc_InitOutDataBuf(pOutBuf, (uint32_t) last_capi->out_buf_params.out_buf.actual_data_len);

   AudioEncSvc_LogOutputData((int8_t*)&(pOutBuf->nDataBuf),pOutBuf->nActualSize, pMe);

   //TODO: put timestamp in output buffer
   //asm_set_timestamp_valid_flag(&pOutBuf->nFlag, pMe->TsState.bNextOutbufTSValid);
   //pOutBuf->ullTimeStamp = pMe->TsState.ullNextOutBufTS;

   //TODO: update next output time stamp
   //(void)AudioEncSvc_UpdateNextOutbufTS(pOutBuf->nActualSize, &pMe->PrevFmtBlk, &pMe->TsState,
   //                                     pMe->ulDecFormatId);

   qurt_elite_bufmgr_node_t outBufMgrNode = pMe->outDataBufferNode;
   elite_msg_any_t *pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(&outBufMgrNode, ELITE_DATA_BUFFER, NULL, /* do not need response */
         0, /* token */
         0 /* do not care response result*/
   );

   nResult = qurt_elite_queue_push_back(pMe->pPeerSvc->dataQ, (uint64_t*) pPeerDataQMsg);

   (pMe->buf_done_cnt)++;

   // send pOutputBuf to downstream service.
   if (ADSP_FAILED(nResult))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping!\n");
      (void) elite_msg_push_payload_to_returnq(pMe->serviceHandle.gpQ, (elite_msg_any_payload_t*) (pMe->outDataBufferNode.pBuffer));
   }

   pMe->outDataBufferNode.pBuffer = NULL;
   last_capi->out_buf_params.out_buf.data_ptr = NULL;
   last_capi->out_buf_params.out_buf.actual_data_len = 0;
   last_capi->out_buf_params.out_buf.max_data_len = 0;
   last_capi->out_buf_params.usNumFramesInOutBuf = 0;

#ifdef DBG_AUDIO_ENCODE
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sent(%d) bytes to stream router\n", pOutBuf->nActualSize);
#endif

   return nResult;
}

void AudioEncSvc_InitOutDataBuf(elite_msg_data_buffer_t *pBuf, uint32_t actualSize)
{
   pBuf->nFlag = 0;
   pBuf->ullTimeStamp = 0;
   pBuf->nOffset = 0;
   pBuf->nActualSize = actualSize;
   pBuf->pResponseQ = NULL;
   pBuf->unClientToken = 0;
   pBuf->unResponseResult = 0;
}

void AudioEncSvc_DestroyExtBufs(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container)
{
   ADSPResult result;
   qurt_elite_bufmgr_node_t bufNode;

   if (NULL != pMe->outDataBufferNode.pBuffer)
   {
      (void) elite_msg_push_payload_to_returnq(pMe->serviceHandle.gpQ, (elite_msg_any_payload_t*) (pMe->outDataBufferNode.pBuffer));
      pMe->outDataBufferNode.pBuffer = NULL;
   }

   // Drain the buffers
   while (qurt_elite_channel_poll(qurt_elite_queue_get_channel(pMe->serviceHandle.gpQ),
         qurt_elite_queue_get_channel_bit(pMe->serviceHandle.gpQ)))
   {
      // retrieve the buffer
      result = qurt_elite_queue_pop_front(pMe->serviceHandle.gpQ, (uint64_t*)&bufNode );

      if (ADSP_EOK == result)
      {
         // Free the buffer
         qurt_elite_memory_free(bufNode.pBuffer);
         pMe->outbufs_allocated--;
      }
      // shouldn't reach this point.
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error %d in destroying buffers in Encoder svc!!\n", result);
      }
   }
}

void AudioEncSvc_DestroyAllExtBufs(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container)
{
   while (0 < pMe->outbufs_allocated) //Destroy all the allocated buffers.
   {
      (void) qurt_elite_channel_wait(qurt_elite_queue_get_channel(pMe->serviceHandle.gpQ),
            qurt_elite_queue_get_channel_bit(pMe->serviceHandle.gpQ));

      AudioEncSvc_DestroyExtBufs(pMe, capi_container);
   }

   capi_container->out_buf_params.out_buf.data_ptr = NULL;
   capi_container->out_buf_params.out_buf.actual_data_len = 0;
   capi_container->out_buf_params.out_buf.max_data_len = 0;
   capi_container->out_buf_params.usNumFramesInOutBuf = 0;
   capi_container->out_buf_params.usTargetFramesPerBuf = 0;

   pMe->outbufs_allocated = 0;
}

void AudioEncSvc_DestroyOutBufs(AudioEncSvc_t *pMe)
{
   // Wait for all buffers to be returned. This has to be a blocking operation, so use pselect.
   uint8_t total_capis=0;

   for (uint8_t i=0; i < ENC_SVC_MAX_CAPI; i++)
   {
      if (!pMe->capiContainer[i]) break;
      total_capis++;
   }

   //we created CAPI output buffer for all but last CAPI, hence we need to delete only for all but last one.
   for (uint8_t i=0; i < total_capis-1; i++)
   {
      if (!pMe->capiContainer[i]) break;

      if (pMe->capiContainer[i]->out_buf_params.out_buf.data_ptr)
      {
         qurt_elite_memory_free(pMe->capiContainer[i]->out_buf_params.out_buf.data_ptr);
         pMe->capiContainer[i]->out_buf_params.out_buf.data_ptr = NULL;
      }

      pMe->capiContainer[i]->out_buf_params.out_buf.actual_data_len = 0;
      pMe->capiContainer[i]->out_buf_params.out_buf.max_data_len = 0;
      pMe->capiContainer[i]->out_buf_params.usNumFramesInOutBuf = 0;
      pMe->capiContainer[i]->out_buf_params.usTargetFramesPerBuf = 0;
   }

   if (total_capis>0){
   AudioEncSvc_DestroyAllExtBufs(pMe, pMe->capiContainer[total_capis-1]);
   }
}

ADSPResult AudioEncSvc_CheckReallocateExternalBuffer(AudioEncSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_data_buffer_t *pBuffer = (elite_msg_data_buffer_t*)pMe->outDataBufferNode.pBuffer;
   enc_CAPI_container_t *lastCapi = AudioEncSvc_GetLastCapi(pMe);

   if ( (NULL == pBuffer) || (NULL == lastCapi))
   {
      return result;
   }

   //if buf size doesn't match then recreate.
   if((uint32_t)pBuffer->nMaxSize != lastCapi->out_buf_params.maxBufSize)
   {
      // Free the buffer
      qurt_elite_memory_free(pBuffer);
      pMe->outbufs_allocated--;

      pMe->outDataBufferNode.pBuffer = NULL;

      //nBufsAllocated buffers are already allocated, need to allocate one more.
      result = AudioEncSvc_CreateOutBuf(pMe, lastCapi,
            TRUE, pMe->no_apr_enc, lastCapi->out_buf_params.maxBufSize);

      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Buffer recreate failed %d", result);
      }
   }

   return result;
}

static uint32_t power_of_2(uint32_t n)
{
   n = n - 1;
   n |= (n >> 1);
   n |= (n >> 2);
   n |= (n >> 4);
   n |= (n >> 8);
   n |= (n >> 16);
   n += 1;
   return n;
}

/*
 *Function AudioEncSvc_CreateSvcDataQQueue
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Creates input data queues and adds it to the channel.
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_CreateSvcDataQQueue(AudioEncSvc_t *pMe)
{
   // assign memory location for queues.
   int count;
   char aDataQName[QURT_ELITE_DEFAULT_NAME_LEN];    //data queue name

   count = qurt_elite_atomic_get(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;
   qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs);

   snprintf(aDataQName, QURT_ELITE_DEFAULT_NAME_LEN, "%s%x", AUDIO_ENC_SVC_DATA_Q_NAME, count);

   uint32_t num_elements = MAX_DATA_Q_ELEMENTS;

   //only for RT path (AFE) decide num of buf based on capi
   if (pMe->is_rt_enc)
   {
      enc_CAPI_container_t *first_capi = AudioEncSvc_GetFirstCapi(pMe);
      uint32_t worst_case_frame_len = first_capi->in_port_thresh / ((MIN_SAMPLE_FREQ * MIN_CHANNEL * MIN_BYTES_PER_SAMPLE)/1000);
      if (worst_case_frame_len != 0)
      {
         num_elements = power_of_2(worst_case_frame_len); //assume one ms buffers
      }
      //encoder wouldn't take MAX_DATA_Q_ELEMENTS_RT ms to process realistically
      if (num_elements > MAX_DATA_Q_ELEMENTS_RT)
      {
         num_elements = MAX_DATA_Q_ELEMENTS_RT;
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioEncSvc number of dataQ elements is determined to be %lu", num_elements);

   ADSPResult result;
   if (ADSP_FAILED(result = qurt_elite_queue_create(aDataQName,
         num_elements, &(pMe->serviceHandle.dataQ))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc failed to create service data queue!! \n");
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->serviceHandle.dataQ,AUD_ENC_DATA_SIG)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioEncSvc failed to add service data queue to channel w/ result = %d!!\n", result);
      return result;
   }

   return ADSP_EOK;
}

/*
 *Function Name:AudioEncSvc_CreateSvcQueues
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *Parameters: is_out_dataQ_needed whether output dataQ (gpQ or bufQ) is needed (push mode doesn't need)
 *
 *Description: Creates cmd, and output data queues and adds them to the channel.
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_CreateSvcQueues(AudioEncSvc_t *pMe, bool_t is_out_dataQ_needed)
{
   // assign memory location for queues.
   pMe->serviceHandle.dataQ = NULL;

   // set up channel
   qurt_elite_channel_init(&pMe->channel);

   // static variable counter to make queue and thread name strings unique
   // limit to 16 bits so it will roll over and and only cost 4 characters in hexadecimal.
   // Queues in existence likely to have unique names, but not required...
   int count;
   char aCmdQName[QURT_ELITE_DEFAULT_NAME_LEN];     //command queue name
   char aOutQName[QURT_ELITE_DEFAULT_NAME_LEN];     //output buffer queue name

   count = qurt_elite_atomic_get(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;
   qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs);

   // name the queues. Name is concatenated with 5 items:
   // - unique 3 letter code for service
   // - letter abbreviating which queue, e.g. 'd' 'c' 'b' for data, cmd, buf.
   // - number in case there are multiple queues of the same type, e.g. 2 buf q's
   // - count (up to 5 digits)
   // e.g. the first instance of this service would name its dataQ "/EPLd00".and cmd q "/EPLc00"
   snprintf(aCmdQName, QURT_ELITE_DEFAULT_NAME_LEN, "%s%x", AUDIO_ENC_SVC_CMD_Q_NAME, count);
   snprintf(aOutQName, QURT_ELITE_DEFAULT_NAME_LEN, "%s%x", AUDIO_ENC_SVC_BUF_Q_NAME, count);

   ADSPResult result;
   if (ADSP_FAILED(result = qurt_elite_queue_create(aCmdQName,
         MAX_CMD_Q_ELEMENTS, &(pMe->serviceHandle.cmdQ)))

         || (is_out_dataQ_needed && ADSP_FAILED(result = qurt_elite_queue_create(aOutQName,
               BUF_Q_ELEMENTS_SIZE, &(pMe->serviceHandle.gpQ)))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc failed to create service queues!! \n");
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->serviceHandle.cmdQ,AUD_ENC_CMD_SIG))
         || (is_out_dataQ_needed && ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->serviceHandle.gpQ,AUD_ENC_BUF_SIG))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioEncSvc failed to add service queues to channel w/ result = %d!!\n", result);
      return result;
   }

   //fill service ID
   pMe->serviceHandle.unSvcId = ELITE_DYN_ENCODER_SVCID;

   return ADSP_EOK;
}

/*
 *Function Name:AudioEncSvc_CreateInpBuf
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Creates internal input buffer.
 *
 *Returns: ADSPResult
 *
 */
static ADSPResult AudioEncSvc_CreateInpBuf(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container, uint32_t buf_size)
{
   // create internal input bitstream buffer
   uint8_t *in_buf = (uint8_t *) qurt_elite_memory_malloc(buf_size, QURT_ELITE_HEAP_DEFAULT);
   if ( NULL==in_buf)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create internal input buffer!");
      return ADSP_ENOMEMORY;
   }

#ifdef DBG_BUFFER_ADDRESSES
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR Enc internal PCM buffer: 0x%8x, size %d",
         in_buf, buf_size);
#endif /* DBG_BUFFER_ADDRESSES */

   capi_container->in_buf_params.in_buf.actual_data_len = 0;
   //Set DataLenReq fields here, as they are constant
   capi_container->in_buf_params.in_buf.max_data_len = buf_size;
   //Set the data fields for input node of CAPI
   capi_container->in_buf_params.in_buf.data_ptr    = (int8_t *) (in_buf);

   capi_container->in_buf_params.inputs.buf_ptr = &capi_container->in_buf_params.in_buf;
   capi_container->in_buf_params.inputs.bufs_num = 1;
   capi_container->in_buf_params.inputs.flags.end_of_frame = false;
   capi_container->in_buf_params.inputs.flags.is_timestamp_valid = false;
   capi_container->in_buf_params.inputs.flags.marker_eos = false;
   capi_container->in_buf_params.inputs.flags.marker_1 = 0;
   capi_container->in_buf_params.inputs.flags.marker_2 = 0;
   capi_container->in_buf_params.inputs.flags.marker_3 = 0;
   capi_container->in_buf_params.inputs.flags.reserved = 0;
   capi_container->in_buf_params.inputs.timestamp = 0;

   capi_container->in_buf_params.inputs_ptr[0] = &capi_container->in_buf_params.inputs;

   //max in size is not stored elsewhere as capi_container->in_buf_params.in_buf.max_data_len is not changed.

   //initialize input buffer
   (void) AudioEncSvc_InitInpBufParams(&capi_container->in_buf_params);

   return ADSP_EOK;
}

/*
 *Function Name:AudioEncSvc_CreateOutBuf
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Creates output buffers and push them onto output bufQ.
 *
 *Returns: ADSPResult
 *
 */
static ADSPResult AudioEncSvc_CreateOutBuf(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container, bool_t is_last_capi, bool_t is_no_apr_enc, uint32_t buf_size)
{
   if (!is_last_capi)
   {
      if (NULL == capi_container->out_buf_params.out_buf.data_ptr)
      {
         void *pOutputBuf = qurt_elite_memory_malloc(buf_size, QURT_ELITE_HEAP_DEFAULT);
         if (!pOutputBuf)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create output buffer for AudEncSvc.It requires %lu bytes",buf_size);
            return ADSP_ENOMEMORY;
         }

         memset(pOutputBuf, 0, buf_size);

         capi_container->out_buf_params.out_buf.data_ptr = (int8_t*)pOutputBuf;
         capi_container->out_buf_params.out_buf.actual_data_len = 0;
      }
   }

   if (is_last_capi && is_no_apr_enc)
   {
      uint32_t reqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(buf_size);

      uint32_t num_buf = MAX_ENC_OUT_BUF;
      if (pMe->is_rt_enc)
      {
         num_buf = MAX_ENC_OUT_BUF_IN_AFE;
      }
      if (pMe->outbufs_allocated < num_buf)
      {
         num_buf = num_buf - pMe->outbufs_allocated;
         // Allocate and queue up the output buffers.
         for (uint32_t i = 0; i < num_buf; i++)
         {
            //allocate the databuffer payload (metadata + pcm buffer size)
            void *pOutputBuf = qurt_elite_memory_malloc(reqSize, QURT_ELITE_HEAP_DEFAULT);

            if (!pOutputBuf)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to allocate output data buffer for AudEncSvc.It requires %lu bytes",reqSize);
               return ADSP_ENOMEMORY;
            }

#ifdef DBG_BUFFER_ADDRESSES
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR encoder output buffer: 0x%8x, size %d", pOutputBuf, reqSize);
#endif /* DBG_BUFFER_ADDRESSES */

            memset(pOutputBuf, 0, reqSize);

            //save the max size in each output data buffer structure
            elite_msg_data_buffer_t *pOutDataBuf = (elite_msg_data_buffer_t*) pOutputBuf;
            pOutDataBuf->nMaxSize = buf_size;

            ADSPResult result;
            if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(pMe->serviceHandle.gpQ, (elite_msg_any_payload_t*) pOutputBuf)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to fill Encoder Svc buffer queue!! \n");
               //free this PCM buffer here because main destroy function doesn't have the ptr, since push to returnq failed.
               qurt_elite_memory_free(pOutputBuf);
               return result;
            }
            pMe->outbufs_allocated++;
         }
      }
   }

   if (is_last_capi)
   {
      capi_container->out_buf_params.out_buf.data_ptr = NULL;
      capi_container->out_buf_params.out_buf.actual_data_len = 0;
   }

   capi_container->out_buf_params.maxBufSize = buf_size;
   capi_container->out_buf_params.out_buf.max_data_len = buf_size;
   //max out size is not stored elsewhere as capi_container->out_buf_params.out_buf.max_data_len is not changed (except for last capi).

   capi_container->out_buf_params.outputs.buf_ptr = &capi_container->out_buf_params.out_buf;
   capi_container->out_buf_params.outputs.bufs_num = 1;
   capi_container->out_buf_params.outputs.flags.end_of_frame = 0;
   capi_container->out_buf_params.outputs.flags.is_timestamp_valid = false;
   capi_container->out_buf_params.outputs.flags.marker_eos = 0;
   capi_container->out_buf_params.outputs.flags.marker_1 = 0;
   capi_container->out_buf_params.outputs.flags.marker_2 = 0;
   capi_container->out_buf_params.outputs.flags.marker_3 = 0;
   capi_container->out_buf_params.outputs.flags.reserved = 0;
   capi_container->out_buf_params.outputs.timestamp = 0;

   capi_container->out_buf_params.outputs_ptr[0] = &capi_container->out_buf_params.outputs;

   (void) AudioEncSvc_InitOutBufParams(&capi_container->out_buf_params);

   return ADSP_EOK;
}

static ADSPResult AudioEncSvc_RecreateBuffers(AudioEncSvc_t *pMe,
      enc_capi_port_index_t in_port_index, enc_capi_port_index_t out_port_index,
      uint8_t capi_index)
{
   ADSPResult result = ADSP_EOK;

   uint32_t in_size = 0, out_size = 0;

   if (in_port_index.valid)
   {
      //input size is prev output size + current input size.
      //TODO: for CAPI in the middle, we don't need the prev+current at both previous and current CAPIs.
      //  We need either prev+current in only one place or prev & current in each CAPIs.
      in_size = pMe->capiContainer[capi_index]->in_port_thresh;
      if (capi_index >= 1)
      {
         in_size += pMe->capiContainer[capi_index-1]->out_port_thresh;
      }

      EncSvc_InpBufParams_t *pBufParams = &pMe->capiContainer[capi_index]->in_buf_params;

      if (pBufParams->in_buf.max_data_len != in_size)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "CAPI index %lu in buffer recreate old %lu, new %lu",
               capi_index, pBufParams->in_buf.max_data_len, in_size);

         if (pBufParams->in_buf.data_ptr)
         {
            qurt_elite_memory_free(pBufParams->in_buf.data_ptr);
            pBufParams->in_buf.data_ptr = NULL;
         }

         //deinit other relevant params
         pBufParams->in_buf.max_data_len = 0;
         pBufParams->in_buf.actual_data_len = 0;

         //if last CAPI create ext output buffers as well.
         result = AudioEncSvc_CreateInpBuf(pMe, pMe->capiContainer[capi_index], in_size);
      }
   }

   if (out_port_index.valid)
   {
      EncSvc_OutBufParams_t *pBufParams = &pMe->capiContainer[capi_index]->out_buf_params;
      bool_t is_last_capi = AudioEncSvc_IsLastCapiIndex(pMe, capi_index);

      out_size = pMe->capiContainer[capi_index]->out_port_thresh;
      //if not last CAPI, output size is sum of current output + next input size.
      //TODO: for CAPI in the middle, we don't need the prev+current at both previous and current CAPIs.
      //  We need either prev+current in only one place or prev & current in each CAPIs.
      if (!is_last_capi)
      {
         out_size += pMe->capiContainer[capi_index+1]->in_port_thresh;
      }

      bool_t is_recreate_needed = (pBufParams->maxBufSize != out_size);

      if (is_recreate_needed)
      {
         MSG_6(MSG_SSID_QDSP6, DBG_MED_PRIO, "CAPI index %lu. possible out buffer recreate old %lu, new %lu [is_last_capi %u, no_apr_enc %u, is_afe_enc %u]",
               capi_index, pBufParams->maxBufSize, out_size, is_last_capi, pMe->no_apr_enc, pMe->is_rt_enc);
      }

      //for APR enc & last-CAPI, no need to recreate as bufs are from client.
      if (is_last_capi && pMe->no_apr_enc)
      {
         if (is_recreate_needed)
         {
            AudioEncSvc_DestroyExtBufs(pMe, pMe->capiContainer[capi_index]);
         }
      }

      if (!is_last_capi)
      {
         if (is_recreate_needed)
         {
            if (pMe->capiContainer[capi_index]->out_buf_params.out_buf.data_ptr)
            {
               qurt_elite_memory_free(pMe->capiContainer[capi_index]->out_buf_params.out_buf.data_ptr);
               pMe->capiContainer[capi_index]->out_buf_params.out_buf.data_ptr = NULL;
            }

            pMe->capiContainer[capi_index]->out_buf_params.out_buf.actual_data_len = 0;
            pMe->capiContainer[capi_index]->out_buf_params.out_buf.max_data_len = 0;
            pMe->capiContainer[capi_index]->out_buf_params.usNumFramesInOutBuf = 0;
            pMe->capiContainer[capi_index]->out_buf_params.usTargetFramesPerBuf = 0;
         }
      }

      if (is_recreate_needed)
      {
         result = AudioEncSvc_CreateOutBuf(pMe, pMe->capiContainer[capi_index],
               is_last_capi, pMe->no_apr_enc, out_size);
      }
   }

   return result;
}

static ADSPResult AudioEncSvc_HandlePortDataThreshChangeEvent(AudioEncSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;

   if (!(pMe->event_mask|AUD_ENC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK))
   {
      return ADSP_EOK;
   }

   enc_capi_port_index_t in_capi_port_index;
   enc_capi_port_index_t out_capi_port_index;
   in_capi_port_index.index = 0; in_capi_port_index.valid = false;
   out_capi_port_index.index = 0; out_capi_port_index.valid = false;

   //buf will be recreated only if size changes.

   for (uint8_t capi_index=0; capi_index < ENC_SVC_MAX_CAPI; capi_index++)
   {
      if(NULL == pMe->capiContainer[capi_index]) break;

      out_capi_port_index.index = 0; out_capi_port_index.valid = false;

      if (pMe->capiContainer[capi_index]->in_port_thresh)
      {
         in_capi_port_index.index = 0; in_capi_port_index.valid = true;

         result |= AudioEncSvc_RecreateBuffers(pMe, in_capi_port_index, out_capi_port_index, capi_index);

         int enc_in_bits_per_sample = pMe->in_med_fmt.bits_per_sample;
         uint32_t enc_in_bytes_per_sample = (enc_in_bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

         uint32_t prevEncFmSize = pMe->capiContainer[capi_index]->unEncInpFrameSize;
         pMe->capiContainer[capi_index]->unEncInpFrameSize =
               pMe->capiContainer[capi_index]->in_port_thresh/enc_in_bytes_per_sample;

         if (pMe->capiContainer[capi_index]->unEncInpFrameSize != prevEncFmSize)
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "CAPI index %lu Port data threshold event (or CAPI creation) resulted in change in unEncInpFrameSize from %lu to %lu",
                  capi_index, prevEncFmSize, pMe->capiContainer[capi_index]->unEncInpFrameSize);
         }
      }

      in_capi_port_index.index = 0; in_capi_port_index.valid = false;

      if (pMe->capiContainer[capi_index]->out_port_thresh)
      {
         out_capi_port_index.index = 0; out_capi_port_index.valid = true;

         result |= AudioEncSvc_RecreateBuffers(pMe, in_capi_port_index, out_capi_port_index, capi_index);
      }
   }

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"failed to recreate buffers");
      result = ADSP_EFAILED;
   }

   //clear all events (even if error)
   pMe->event_mask &= (~AUD_ENC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK);

   return result;
}


/*
 *Function Name:AudioEncSvc_InitInpBufParams
 *
 *Parameters: EncSvc_InpBufParams_t *pBufParams: input buffer params handle
 *
 *Description: Resets internal input buffer and its params
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_InitInpBufParams(EncSvc_InpBufParams_t *in_buf)
{
   memset(in_buf->in_buf.data_ptr, 0, in_buf->in_buf.max_data_len);

   //init only those params that can change during life time of the instance
   in_buf->in_buf.actual_data_len = 0;

   return ADSP_EOK;
}

/*
 *Function Name:AudioEncSvc_DestroyInpBuf
 *
 *Parameters: EncSvc_InpBufParams_t *pBufParams: input buffer params handle
 *
 *Description: Destroys internal input buffer and deinitializes its parameters
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_DestroyInpBuf(AudioEncSvc_t *pMe)
{
   EncSvc_InpBufParams_t *pBufParams;

   for (uint8_t i=0; i<ENC_SVC_MAX_CAPI; i++)
   {
      if (!pMe->capiContainer[i]) break;

      pBufParams = &pMe->capiContainer[i]->in_buf_params;

      if (pBufParams->in_buf.data_ptr)
      {
         qurt_elite_memory_free(pBufParams->in_buf.data_ptr);
         pBufParams->in_buf.data_ptr = NULL;
      }

      //deinit other relevant params
      pBufParams->in_buf.max_data_len = 0;
      pBufParams->in_buf.actual_data_len = 0;
   }

   return ADSP_EOK;
}

/*
 *Function Name:AudioEncSvc_InitOutBufParams
 *
 *Parameters: EncSvc_OutBufParams_t *pBufParams: output buffer params handle
 *
 *Description: Resets output buffer parameters
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_InitOutBufParams(EncSvc_OutBufParams_t *pBufParams)
{
   //init only those params that can change during life time of the instance
   pBufParams->out_buf.actual_data_len = 0;
   pBufParams->usNumFramesInOutBuf = 0;

   return ADSP_EOK;
}

ADSPResult AudioEncSvc_InitInpOutBufParamsOfAllCAPIs(AudioEncSvc_t *pMe)
{
   //reset the CAPI buffers.
   for (uint8_t i=0; i<ENC_SVC_MAX_CAPI; i++)
   {
      if (!pMe->capiContainer[i]) break;
      (void)AudioEncSvc_InitInpBufParams(& pMe->capiContainer[i]->in_buf_params);
      (void)AudioEncSvc_InitOutBufParams(& pMe->capiContainer[i]->out_buf_params);
   }
   return ADSP_EOK;
}

/*
 *Function Name:AudioEncSvc_FlushInpQueue
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Flushes inpput data queue
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_FlushInpQueue(AudioEncSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   uint32_t num_buf = 0;
   if (pMe->serviceHandle.dataQ)
   {
      while(TRUE)
      {
         //check if there is an dataq msg thats already popped out
         if (pMe->InpDataMsg.pPayload)
         {
            if(pMe->InpDataMsg.unOpCode == ELITE_DATA_MARK_BUFFER)
            {
               //Raise the Mark Buffer Consumption Event
               elite_msg_data_mark_buffer_t *pMarkBufferPayload = (elite_msg_data_mark_buffer_t *)pMe->InpDataMsg.pPayload;
               asm_data_event_mark_buffer_v2_t eventPayload;
               eventPayload.token_lsw = pMarkBufferPayload->token_lsw;
               eventPayload.token_msw = pMarkBufferPayload->token_msw;
               eventPayload.result = ASM_DATA_EVENT_MARK_BUFFER_DISCARDED;
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Encoder: Mark buffer discarded, raising event");
               result = AudioStreamMgr_GenerateClientEventFromCb(&pMe->CbData,
                     ASM_DATA_EVENT_MARK_BUFFER_V2,
                     0, &eventPayload, sizeof(eventPayload));
            }

            num_buf++;
            AudioEncSvc_ReleaseInputBuf(pMe, ADSP_ENOTREADY);
         }

         // Drain any queued buffers
         if ( qurt_elite_channel_poll( qurt_elite_queue_get_channel(pMe->serviceHandle.dataQ),
               qurt_elite_queue_get_channel_bit(pMe->serviceHandle.dataQ) ))
         {
            result = qurt_elite_queue_pop_front(pMe->serviceHandle.dataQ, (uint64_t*) &pMe->InpDataMsg);

            if (ADSP_FAILED(result) )
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc_FlushInpQueue, breaking when num_buf is %lu. result %ld", num_buf, result);
               break;
            }
            else
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc_FlushInpQueue, popped %p", pMe->InpDataMsg.pPayload);
            }
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioEncSvc_FlushInpQueue, queue-poll resulted in nothing");
            break;
         }
      }
   }
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Encoder: AudioEncSvc_FlushInpQueue, released %lu buffers", num_buf);

   return result;
}

/*
 *Function Name:AudioEncSvc_FlushOutQueue
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Flushes output data queue
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_FlushOutQueue(AudioEncSvc_t *pMe)
{
   enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);
   if (pMe->serviceHandle.gpQ)
   {
      if (AudioEncSvc_IsOutBufFromClient(pMe))
      {
         //first check if we have any output buffers already popped out
         if (pMe->OutDataMsg.pPayload)
         {
            //Flush the shared mem map region
            elite_mem_flush_cache(&(pMe->SharedMemMapTypeNode));

            AudioEncSvc_ReleaseOutputBuffer(pMe, (uint32_t) last_capi->out_buf_params.out_buf.actual_data_len, ADSP_EOK);
         }
      }
      else
      {
         if (pMe->outDataBufferNode.pBuffer)
         {
            AudioEncSvc_SendOutBufToPeerSvc(pMe);
         }
      }

      // This next chunk of code needs a story...
      // On prepare for disconnect we do not want to flush the entire output queue because the client
      // may still be trying to push empty buffers to the encoder. We have a race condition here.
      // If the client is pushing buffers faster than we can pop them then this will result in an inf loop.
      // The output queue is flushed on disconnection. If we are handling a prepare for disconnect cmd then
      // we may wait to flush the output buffers on disconnection.
      bool_t bShouldFlushEntireOutQueue= TRUE;
      if ((ELITE_CUSTOM_MSG == pMe->CmdMsg.unOpCode) && (NULL != pMe->CmdMsg.pPayload))
      {
         elite_msg_custom_header_t *pPayload = (elite_msg_custom_header_t *)pMe->CmdMsg.pPayload;
         bShouldFlushEntireOutQueue =
               (ELITEMSG_CUSTOM_ENC_PREPARE_DISCONNECT == pPayload->unSecOpCode) ? FALSE : TRUE;
      }

      if (bShouldFlushEntireOutQueue)
      {
         if (AudioEncSvc_IsOutBufFromClient(pMe))
         {
            //Now release any data buffers in the queue...
            while (ADSP_EOK == qurt_elite_queue_pop_front(pMe->serviceHandle.gpQ, (uint64_t*) &pMe->OutDataMsg))
            {
               if (ADSP_EOK != AudioEncSvc_ReleaseOutputBuffer(pMe, 0, ADSP_EOK))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to release bufQ message!");
               }
            }
         }
         //no need to release any output buf from queue since they are empty and also belong to enc.
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Waiting to release all data buffers in queue until close!");
      }
   }

   pMe->outDataBufferNode.pBuffer = NULL;
   pMe->OutDataMsg.pPayload = NULL;
   last_capi->out_buf_params.out_buf.data_ptr = NULL;
   last_capi->out_buf_params.out_buf.actual_data_len = 0;

   return ADSP_EOK;
}

/*
 *Function Name:AudioEncSvc_CheckSendSrCmEvent
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *            elite_multi_channel_pcm_fmt_blk_t *pInPcmFmtBlk
 *
 *Description: Sends SR/CM change event
 *
 *Returns: ADSPResult
 *
 */
static void AudioEncSvc_CheckSendSrCmEvent(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container, elite_multi_channel_pcm_fmt_blk_t *pInPcmFmtBlk)
{
   bool_t isNativeMode = pMe->isNativeChannels || pMe->isNativeSamplerate;
   uint32_t ulEncCfgId = capi_container->enc_cfg_id;
   uint32_t ulCurrChannels = pInPcmFmtBlk->num_channels;
   uint32_t ulCurrSampleRate = pInPcmFmtBlk->sample_rate;
   uint32_t ulBitsPerSample = pInPcmFmtBlk->bits_per_sample;

   //&& (ulCurrChannels != pMe->ulPrevNumChannels || ulCurrSampleRate != pMe->ulPrevSampleRate) - is handled by caller.
   if (isNativeMode && (AudioEncSvc_IsPcmFmt(ulEncCfgId)) && pMe->CbData.pAsyncRespQ)
   {
      //form the payload for the event
      asm_data_event_enc_sr_cm_change_notify_t SrCmEvent;

      SrCmEvent.sample_rate = ulCurrSampleRate;
      SrCmEvent.num_channels = ulCurrChannels;
      SrCmEvent.bits_per_sample = ulBitsPerSample;

      memset(&SrCmEvent.channel_mapping, PCM_CHANNEL_INVALID, sizeof(SrCmEvent.channel_mapping));
      //Multichannel: copy channel mapping from format block
      memscpy(&SrCmEvent.channel_mapping, pInPcmFmtBlk->num_channels * sizeof(uint8_t),
            pInPcmFmtBlk->channel_mapping, pInPcmFmtBlk->num_channels * sizeof(uint8_t));

      (void) AudioStreamMgr_GenerateClientEventFromCb(&pMe->CbData, ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY, pMe->CbData.unAsynClientToken, &SrCmEvent,
            sizeof(SrCmEvent));

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Generated ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY; \
         now waiting for ASM_STREAM_CMD_FLUSH_READBUFS");

      //wait for flush_readbufs command before doing any more data processing
      pMe->waitForFlushReadBufs = true;
      //listen only to command queue
      pMe->unCurrentBitfield = AUD_ENC_CMD_SIG;
   }
}

/*
 *Function Name:AudioEncSvc_ProcessMediaTypeMsg
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Processes data media type message from
 *             upstream service
 *
 *Returns: ADSPResult
 *
 */

ADSPResult AudioEncSvc_ProcessMediaTypeMsg(AudioEncSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   enc_CAPI_container_t *first_capi = AudioEncSvc_GetFirstCapi(pMe);
   enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

   //get the mediaFormat structure
   elite_msg_data_media_type_apr_t *pMediaFmt = (elite_msg_data_media_type_apr_t*) (pMe->InpDataMsg.pPayload);
   elite_multi_channel_pcm_fmt_blk_t *pInPcmFmtBlk;
   bool_t sr_cm_updated = FALSE, output_media_fmt_changed = FALSE;

   /* sanity checks */
   if (ELITEMSG_DATA_MEDIA_TYPE_APR != pMediaFmt->unMediaTypeFormat || (ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM != pMediaFmt->unMediaFormatID))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported MEDIA FORMAT 0x%8lx, formatID 0x%lx", (pMediaFmt->unMediaTypeFormat), (pMediaFmt->unMediaFormatID));
      result = ADSP_EBADPARAM;
      goto __bailout;
   }

   pInPcmFmtBlk = (elite_multi_channel_pcm_fmt_blk_t*) elite_msg_get_media_fmt_blk(pMediaFmt);

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudEncSvc Input media format numchannels %u, samplingRate %lu, Interleave %u, bps=%u", pInPcmFmtBlk->num_channels,
         pInPcmFmtBlk->sample_rate, pInPcmFmtBlk->is_interleaved, pInPcmFmtBlk->bits_per_sample);

   //input media format for first CAPI
   audio_enc_svc_capi_v2_media_fmt_t in_media_fmt;
   in_media_fmt.std.bits_per_sample = pMe->out_med_fmt_per_enc_cfg.bits_per_sample;
   in_media_fmt.std.data_interleaving = CAPI_V2_INTERLEAVED;
   in_media_fmt.std.data_is_signed = 1;
   in_media_fmt.std.q_factor = (pMe->out_med_fmt_per_enc_cfg.bits_per_sample - 1); //enc svc gives data to CAPI in Q15 or Q31
   //channel & SR info comes from input media fmt must match enc-cfg unless enc-cfg specifies as native mode.

   //in case of compressed record, no encode config block is received.
   //so need to set the sampling rate, num channels to values in format block
   //there's no CAPI_PARAM_ENC_CFG_BLK struct for depacketizer, input-media-fmt suffices.
   if (pMe->io_fmt_conv == ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW)
   {
      in_media_fmt.main.format_header.data_format = CAPI_V2_IEC61937_PACKETIZED; //input to capi is packetized.
      in_media_fmt.std.bitstream_format = CAPI_V2_DATA_FORMAT_INVALID_VAL;
      pMe->out_med_fmt_per_enc_cfg.bits_per_sample = pInPcmFmtBlk->bits_per_sample;
      pMe->out_med_fmt_per_enc_cfg.bytes_per_sample = pInPcmFmtBlk->bits_per_sample==16?2:4;

      //for 61937-to-raw use case, enc cfg set now, so init output media fmt now.
      init_all_capi_out_media_fmt_at_enc_cfg(pMe);

      pMe->isNativeChannels = true;
      pMe->isNativeSamplerate = true;
   }
   else
   {
      in_media_fmt.main.format_header.data_format = CAPI_V2_FIXED_POINT;
      in_media_fmt.std.bitstream_format = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2;
   }

   if (!pMe->isNativeChannels)
   {
      if (pInPcmFmtBlk->num_channels != pMe->out_med_fmt_per_enc_cfg.num_channels)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error. Number of channels in media type %d does not match output number of channels %lu",
               pInPcmFmtBlk->num_channels, pMe->out_med_fmt_per_enc_cfg.num_channels);
         result = ADSP_EFAILED;
         goto __bailout;
      }
      else
      {
         for (uint32_t i = 0; i < pMe->out_med_fmt_per_enc_cfg.num_channels; i++)
         {
            if (pMe->out_med_fmt_per_enc_cfg.chan_map[i] != pInPcmFmtBlk->channel_mapping[i])
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error. Channel mapping in media type msg does not match output channel mapping");
               result = ADSP_EFAILED;
               goto __bailout;
            }
         }
      }
   }
   else //if(isNativeChannels)
   {
      //set channel count to what upstream svc sent
      if (pMe->out_med_fmt_per_enc_cfg.num_channels != pInPcmFmtBlk->num_channels)
      {
         pMe->out_med_fmt_per_enc_cfg.num_channels = pInPcmFmtBlk->num_channels;
         sr_cm_updated = TRUE;
      }
      memscpy(pMe->out_med_fmt_per_enc_cfg.chan_map, sizeof(pMe->out_med_fmt_per_enc_cfg.chan_map),pInPcmFmtBlk->channel_mapping,sizeof(pInPcmFmtBlk->channel_mapping));
   }

   if (!pMe->isNativeSamplerate)
   {
      if (pInPcmFmtBlk->sample_rate != pMe->out_med_fmt_per_enc_cfg.sample_rate)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error. Sampling rate in media type msg %lu does not match output sampling rate %lu", pInPcmFmtBlk->sample_rate,
               pMe->out_med_fmt_per_enc_cfg.sample_rate);
         result = ADSP_EFAILED;
         goto __bailout;
      }
   }
   else
   {
      //set the sampling rate = what upstream svc sent
      if (pMe->out_med_fmt_per_enc_cfg.sample_rate != pInPcmFmtBlk->sample_rate)
      {
         pMe->out_med_fmt_per_enc_cfg.sample_rate = pInPcmFmtBlk->sample_rate;
         sr_cm_updated = TRUE;
      }
   }


   if ( sr_cm_updated )
   {
      output_media_fmt_changed = TRUE; //if SR_CM updated, mark as output media fmt changed.

      //All CAPIs may not raise output media fmt. So initialize to be same as input before setting.
      //only channel, SR might have changed.
      //init only if not zero.
      for (uint8_t i = 0; i < ENC_SVC_MAX_CAPI; i++)
      {
         if (NULL == pMe->capiContainer[i])
         {
            break;
         }

         if ( (CAPI_V2_FIXED_POINT == pMe->capiContainer[i]->out_med_fmt.main.format_header.data_format) ||
               (CAPI_V2_IEC61937_PACKETIZED == pMe->capiContainer[i]->out_med_fmt.main.format_header.data_format) ||
               (CAPI_V2_COMPR_OVER_PCM_PACKETIZED == pMe->capiContainer[i]->out_med_fmt.main.format_header.data_format))
         {
            if (0 == pMe->capiContainer[i]->out_med_fmt.std.num_channels)
            {
               memscpy(pMe->capiContainer[i]->out_med_fmt.std.channel_type, sizeof(pMe->capiContainer[i]->out_med_fmt.std.channel_type),
                     pMe->out_med_fmt_per_enc_cfg.chan_map, sizeof(pMe->out_med_fmt_per_enc_cfg.chan_map));
               pMe->capiContainer[i]->out_med_fmt.std.num_channels = pMe->out_med_fmt_per_enc_cfg.num_channels;
            }

            if (0 == pMe->capiContainer[i]->out_med_fmt.std.sampling_rate)
            {
               pMe->capiContainer[i]->out_med_fmt.std.sampling_rate = pMe->out_med_fmt_per_enc_cfg.sample_rate;
            }
         }
      }

      if(!AudioEncSvc_IsPcmFmt(pMe->capiContainer[0]->enc_cfg_id)) //not needed for pcm encoder
      {
         //send media fmt to first CAPI
         memscpy(in_media_fmt.std.channel_type, sizeof(in_media_fmt.std.channel_type), pMe->out_med_fmt_per_enc_cfg.chan_map, sizeof(pMe->out_med_fmt_per_enc_cfg.chan_map));
         in_media_fmt.std.num_channels = pMe->out_med_fmt_per_enc_cfg.num_channels;
         in_media_fmt.std.sampling_rate = pMe->out_med_fmt_per_enc_cfg.sample_rate;
         in_media_fmt.std.data_interleaving = CAPI_V2_INTERLEAVED;
         in_media_fmt.std.data_is_signed = 1;
         in_media_fmt.std.q_factor = (pMe->out_med_fmt_per_enc_cfg.bits_per_sample - 1);
         result = aud_enc_svc_set_media_fmt(first_capi->capi_ptr, &in_media_fmt, CAPI_V2_INPUT_MEDIA_FORMAT);
      }

   }

   memscpy(&pMe->in_med_fmt, sizeof(pMe->in_med_fmt), pInPcmFmtBlk, sizeof(elite_multi_channel_pcm_fmt_blk_t));

   //propagate media fmt across CAPIs
   for (uint8_t i = 0; i < ENC_SVC_MAX_CAPI-1; i++)
   {
      if (pMe->capiContainer[i])
      {
         //previous CAPI output media fmt is set on later CAPI
         if (pMe->capiContainer[i+1])
         {
            result = aud_enc_svc_set_media_fmt(pMe->capiContainer[i+1]->capi_ptr, &pMe->capiContainer[i]->out_med_fmt, CAPI_V2_INPUT_MEDIA_FORMAT);
            pMe->capiContainer[i]->media_fmt_event = FALSE;
            AudioEncSvc_HandleFwkExtnAtMediaFmt(pMe, pMe->capiContainer[i+1]);
         }
      }
   }

   for (uint32_t capi_index = 0; capi_index < ENC_SVC_MAX_CAPI; capi_index++)
   {
      if (NULL == pMe->capiContainer[capi_index]) break;

      if(AudioEncSvc_IsPcmFmt(pMe->capiContainer[capi_index]->enc_cfg_id)) break;

      //Note: this call returns success only for CAPI v1 as CAPI V2 doesn't implement this param.
      //For CAPI V2 threshold based approach must be used.
      //once CAPI V1 is deprecated complete we can remove this call & references to CAPI_PARAM_SAMPLES_PER_FRAME
      result = aud_enc_svc_get_param(pMe->capiContainer[capi_index]->capi_ptr, CAPI_PARAM_SAMPLES_PER_FRAME,
            (int8_t*)&pMe->capiContainer[capi_index]->unEncInpFrameSize,
            sizeof(pMe->capiContainer[capi_index]->unEncInpFrameSize));

      if (ADSP_FAILED(result) || (0 == pMe->capiContainer[capi_index]->unEncInpFrameSize))
      {
         int enc_in_bits_per_sample = pMe->in_med_fmt.bits_per_sample;
         uint32_t enc_in_bytes_per_sample = (enc_in_bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

         pMe->capiContainer[capi_index]->unEncInpFrameSize =
               pMe->capiContainer[capi_index]->in_port_thresh/enc_in_bytes_per_sample; //TODO: need to divide by num_chan?

         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Get param for samples per frame failed or returned 0. assuming %lu",
               pMe->capiContainer[capi_index]->unEncInpFrameSize);

      }
   }

   //successfully processed input media format, so need not wait for it anymore
   if (pMe->bWaitForInpMediaFmt)
   {
      pMe->bWaitForInpMediaFmt = FALSE;
   }

   AudioEncSvc_DetermineSvcBw(pMe, pInPcmFmtBlk);
   //force update as the svc BW code updated.
   result = AudioEncSvc_ProcessKppsBw(pMe, FALSE, TRUE);

   //pcm encoder won't raise event for threshold change, so needs to be set here
   if(AudioEncSvc_IsPcmFmt(pMe->capiContainer[0]->enc_cfg_id))
   {
      uint32_t bytes_per_sample = (pMe->in_med_fmt.bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
      pMe->capiContainer[0]->in_port_thresh = AudioEncSvc_GetPcmOutBufSize(pMe->perf_mode,
            pMe->in_med_fmt.sample_rate, bytes_per_sample, pMe->in_med_fmt.num_channels);

      pMe->capiContainer[0]->out_port_thresh = pMe->capiContainer[0]->in_port_thresh;

   }

   AudioEncSvc_HandlePortDataThreshChangeEvent(pMe);

   /** for push mode sr_cm_updated is false, because native mode is not enabled */
   if (sr_cm_updated)
   {
      AudioEncSvc_CheckSendSrCmEvent(pMe, last_capi, pInPcmFmtBlk);
   }

   if (output_media_fmt_changed || last_capi->media_fmt_event)
   {
      AudioEncSvc_NotifyPeerSvcWithMediaFmtUpdate(pMe, last_capi->out_med_fmt.std.sampling_rate, last_capi->out_med_fmt.std.num_channels,
            last_capi->out_med_fmt.std.channel_type, last_capi->out_med_fmt.std.bits_per_sample);
      last_capi->media_fmt_event = FALSE;
   }

   __bailout:
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in media format msg ");
   }

   elite_msg_finish_msg(&pMe->InpDataMsg, result);
   pMe->InpDataMsg.pPayload = NULL;

   return result;
}

/*
 *Function Name:AudioEncSvc_FillPCMMetadata
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Fills metadata info into output buffer
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_FillPCMMetadata(AudioEncSvc_t* pMe, enc_CAPI_container_t *last_capi)
{
   ADSPResult result = ADSP_EOK;
   elite_apr_packet_t *pAprPacket;
   asm_data_cmd_read_v2_t *pEncOut;
   uint32_t enc_buf_size;

   AudioEncSvc_GetOutputBuffer(pMe, &pAprPacket, &pEncOut);

   //reset the outbuf write offset since this is used to set frame start offset in metadata
   uint32_t unOutBufWrOffset = last_capi->out_buf_params.usTargetFramesPerBuf * sizeof(asm_data_read_buf_metadata_v2_t);

   enc_buf_size = last_capi->out_buf_params.out_buf.actual_data_len - unOutBufWrOffset;

   //For PCM the frame timestamp is the same as buffer timestamp
   pMe->encTSParams.ullFrameTimeStamp = pMe->encTSParams.ullTimeStamp;

   int num_channels = pMe->out_med_fmt_per_enc_cfg.num_channels;
   uint16_t bytes_per_sample = pMe->out_med_fmt_per_enc_cfg.bytes_per_sample;

   AudioEncSvc_FillMetaDataInOutputBuffer(pMe, enc_buf_size / (num_channels * bytes_per_sample), enc_buf_size, pEncOut, unOutBufWrOffset);

   return result;
}

/*
 *Function Name:AudioEncSvc_FillTimeStampState
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Fills timestamp info into output buffer
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_FillTimeStampState(AudioEncSvc_t* pMe, enc_CAPI_container_t *first_capi)
{
   uint32_t timestamp_valid;
   elite_msg_data_buffer_t* pInpBuf = (elite_msg_data_buffer_t*) pMe->InpDataMsg.pPayload;
   TimeStampParams_t *pTS = (TimeStampParams_t *) &pMe->encTSParams;
   int samples_in_buf, num_channels, sampling_rate; // bits_per_sample;
   int64_t timestamp_diff;

   uint32_t bytes_per_sample;

   num_channels = pMe->in_med_fmt.num_channels;
   sampling_rate = pMe->in_med_fmt.sample_rate;

   bytes_per_sample = (pMe->in_med_fmt.bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   samples_in_buf = pInpBuf->nActualSize / bytes_per_sample;

   //check to see if timestamp is valid
   timestamp_valid = asm_get_timestamp_valid_flag(pInpBuf->nFlag);
   int64_t incoming_timestamp = (int64_t) pInpBuf->ullTimeStamp;

#ifdef AUDENCDEC_TIMESTAMP_LOGS
   MSG_4(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "Enc: incoming buffer TS: %lu %lu, valid=%lu,size=%d",
         (uint32_t)(pInpBuf->ullTimeStamp>>32), (uint32_t)pInpBuf->ullTimeStamp, timestamp_valid,(unsigned int)pInpBuf->nActualSize);
#endif

   //set mismatch flag to FALSE to begin with
   pTS->timestamp_mismatch = FALSE;

   //if this is the first buffer after initialization
   //both LastBufferTimeStamp and TimeStamp need to be set
   if (pTS->ullLastBufferTimeStamp == AUD_MAX_INT64)
   {
      if (timestamp_valid)
      {
         // Correct for possible negative timestamp in the first buffer.
         if (incoming_timestamp < 0)
         {
            pTS->timestamp_offset = -incoming_timestamp;
         }
         else
         {
            pTS->timestamp_offset = 0;
         }

         pTS->ullLastBufferTimeStamp = incoming_timestamp + pTS->timestamp_offset;
         pTS->isLastBufferTimeStampValid = TRUE;
         pTS->ullTimeStamp = pTS->ullLastBufferTimeStamp;
         pTS->ullFrameTimeStamp = pTS->ullTimeStamp;
         pTS->isTimeStampValid = TRUE;
      }
      else
      {
         pTS->isLastBufferTimeStampValid = FALSE;
         pTS->isTimeStampValid = FALSE;
      }
   }
   else
   {
      //Not the first buffer.
      if (pTS->isLastBufferTimeStampValid != timestamp_valid)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Timestamp validity mismatch because prev buffer validity=%u, current buf validity=%lu",
               pTS->isLastBufferTimeStampValid, timestamp_valid);
         pTS->timestamp_mismatch = TRUE;
      }
      //leave TimeStamp as it is and update LastBufferTimeStamp
      if (timestamp_valid)
      {
         pTS->ullLastBufferTimeStamp = incoming_timestamp + pTS->timestamp_offset;

         //check the difference with expected value and see if it is within permissible limits if last TS was also valid.
         if (pTS->isLastBufferTimeStampValid)
         {
            timestamp_diff = (int64_t) pTS->ullLastBufferTimeStamp - (int64_t) pTS->ullExpectedTimeStamp;
            if ((timestamp_diff < -TIMESTAMP_TOLERANCE) || (timestamp_diff > TIMESTAMP_TOLERANCE))
            {
               pTS->timestamp_mismatch = TRUE;
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "TS discontinuity detected. Delta = %ld us", (int32_t) timestamp_diff);
            }
         }
         pTS->isLastBufferTimeStampValid = TRUE;
      }
      else
      {
         pTS->isLastBufferTimeStampValid = FALSE;
      }
   }

   //Now update expected timestamp for next input buffer
   if (timestamp_valid && (num_channels > 0) && (sampling_rate > 0))
   {
      pTS->ullExpectedTimeStamp = pMe->encTSParams.ullLastBufferTimeStamp + ((int64_t) samples_in_buf * NUM_US_PER_SEC) / (num_channels * sampling_rate);
   }

   return ADSP_EOK;
}

/*
 *Function Name:AudioEncSvc_UpdateTimeStampState
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *            uint32_t consumed_samples: number of samples consumed from
 *            last input buffer
 *            uint32_t num_channels: number of output channels
 *            uint32_t sampling_rate: output sampling rate
 *
 *Description: updates timestamp state based on consumed samples
 *
 *Returns: ADSPResult*
 */
ADSPResult AudioEncSvc_UpdateTimeStampState(AudioEncSvc_t* pMe, uint32_t consumed_samples, uint32_t num_channels, uint32_t sampling_rate)
{
   if ((num_channels > 0) && (sampling_rate > 0))
   {
      pMe->encTSParams.ullTimeStamp = pMe->encTSParams.ullLastBufferTimeStamp + ((int64_t) consumed_samples * NUM_US_PER_SEC) / (num_channels * sampling_rate);
      pMe->encTSParams.isTimeStampValid = pMe->encTSParams.isLastBufferTimeStampValid;
   }
   else
   {
      pMe->encTSParams.isTimeStampValid = FALSE;
   }

   return ADSP_EOK;
}

/*
 *Function Name:AudioEncSvc_CopyWithInterleaving
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *            uint32_t num_channels: number of output channels
 *            uint32_t samples_to_copy: number of samples to copy
 *            uint16 *src_ptr: starting address to start copying from
 *            uint16 *dest_ptr:starting address to start coping into
 *            uint32_t bytes_per_sample_out : 16 or 24 or 32. (Q15, Q23, Q31)
 *            uint32_t bytes_per_sample_in : 16 or 32.(Q15, Q27)
 *            uint32_t out_Q_format Q15, Q23, Q31
 *
 * input can be interleaved or not.
 *
 *Description: Copies required number of samples from source to destination
 *             with interleaving
 *
 *Returns: ADSPResult*
 */
ADSPResult AudioEncSvc_CopyWithInterleaving(AudioEncSvc_t* pMe, uint32_t num_channels, uint32_t samples_to_copy, int8_t *src_ptr, int8_t *dest_ptr,
      uint32_t bytes_per_sample_out, uint32_t bytes_per_sample_in, uint32_t out_Q_format, uint32_t out_bits_per_sample)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_data_buffer_t* pInpBuf = (elite_msg_data_buffer_t*) pMe->InpDataMsg.pPayload;

   uint32_t num_samples_per_chan = (samples_to_copy / num_channels);

   int32_t offset_in_bytes = ((pInpBuf->nActualSize + pInpBuf->nOffset) / num_channels); //channel spacing (used only for deinterleaved)
   int32_t offset_in_samples = (offset_in_bytes / bytes_per_sample_in);

   if (2 == bytes_per_sample_in)
   {
      result = byte_convertor_with_intlv_16_in(src_ptr, dest_ptr, num_samples_per_chan, num_channels, bytes_per_sample_out * 8, out_Q_format,
            out_bits_per_sample, pMe->in_med_fmt.is_interleaved, offset_in_samples);
   }
   else if (4 == bytes_per_sample_in)
   {
      if(pMe->in_med_fmt.bits_per_sample == 32)
      {
         result = byte_convertor_with_intlv_32_in_Q31(src_ptr, dest_ptr, num_samples_per_chan, num_channels, bytes_per_sample_out * 8, out_Q_format,
               out_bits_per_sample, pMe->in_med_fmt.is_interleaved, offset_in_samples);
      }
      else if(pMe->in_med_fmt.bits_per_sample == 24)
      {
         result = byte_convertor_with_intlv_32_in(src_ptr, dest_ptr, num_samples_per_chan, num_channels, bytes_per_sample_out * 8, out_Q_format,
               out_bits_per_sample, pMe->in_med_fmt.is_interleaved, offset_in_samples);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc: invalid input bits per sample %lu", pMe->in_med_fmt.bits_per_sample);
         return ADSP_EFAILED;
      }
   }

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioEncSvc:Error bytes_per_sample_in = %lu, bytes_per_sample_out = %lu", bytes_per_sample_in,
            bytes_per_sample_out);
   }

   return result;
}

/*
 *Function Name:AudioEncSvc_FillMetadataAndReleaseOutputBuf
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *
 *Description: Fills timestamp info (if PCM) and releases output buffer
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_FillMetadataAndReleaseOutputBuf(AudioEncSvc_t* pMe)
{

   enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

   uint32_t enc_output_size = last_capi->out_buf_params.out_buf.actual_data_len;

   //If PCM need to fill metadata since it is done
   //only at the time of releasing buffer
   if (AudioEncSvc_IsPcmFmt(last_capi->enc_cfg_id) && last_capi->out_buf_params.bFrameMetaDataEnable)
   {
      AudioEncSvc_FillPCMMetadata(pMe, last_capi);
   }

   //Flush the shared mem map region
   elite_mem_flush_cache(&(pMe->SharedMemMapTypeNode));

   //discard the remaining input pcm samples and release the output buffer
   AudioEncSvc_ReleaseOutputBuffer(pMe, enc_output_size, ADSP_EOK);

   return ADSP_EOK;
}

/*
 *Function Name: AudioEncSvc_LogInputData
 *
 *Parameters: buf_addr : pointer to buffer to be logged
 *            buf_size : buffer size in bytes
 *            session_id : session id for the log
 *            ulEncCfgId : media format id for encoder
 *
 *Description: Logs the encoded or PCM data
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_LogInputData(int8_t *buf_addr, uint32_t buf_size, AudioEncSvc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   elite_log_info log_info_obj;

   uint32_t session_id = pMe->ulDataLogId;

   /* Populate the packet to be sent to logging utility */
   log_info_obj.qxdm_log_code = QXDM_LOG_CODE_AUD_ENC_IN;
   log_info_obj.buf_ptr = (uint8_t *) buf_addr;
   log_info_obj.buf_size = buf_size;
   log_info_obj.session_id = session_id;
   log_info_obj.log_tap_id = AUDIOLOG_AUD_ENC_PCM_IN_TAP_ID;
   log_info_obj.log_time_stamp = qurt_elite_timer_get_time();

   //if input is PCM (not 61937)
   if (pMe->io_fmt_conv != ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW)
   {
      log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_PCM;

      pcm_data_info *pcm_data = &(log_info_obj.data_info.pcm_data_fmt);

      pcm_data->num_channels = pMe->in_med_fmt.num_channels;
      pcm_data->sampling_rate = pMe->in_med_fmt.sample_rate;
      pcm_data->bits_per_sample = pMe->in_med_fmt.bits_per_sample;
      pcm_data->interleaved = pMe->in_med_fmt.is_interleaved;
      pcm_data->channel_mapping = pMe->in_med_fmt.channel_mapping;
   }
   else
   {
      //if input is not PCM, then it's 61937
      log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_BITSTREAM;
      log_info_obj.data_info.media_fmt_id = 0;
   }

   /* Allocate the log buffer and log the packet
    If log code is disabled, log buffer allocation returns NULL
    */
   result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

   return result;
}

/*
 *Function Name: AudioEncSvc_LogOutputData
 *
 *Parameters: buf_addr : pointer to buffer to be logged
 *            buf_size : buffer size in bytes
 *            session_id : session id for the log
 *            ulEncCfgId : media format id for encoder
 *
 *Description: Logs the encoded or PCM data
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_LogOutputData(int8_t *buf_addr, uint32_t buf_size, AudioEncSvc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   elite_log_info log_info_obj;

   enc_CAPI_container_t  *last_capi = AudioEncSvc_GetLastCapi(pMe);

   uint32_t session_id = pMe->ulDataLogId;
   uint32_t ulEncCfgId = last_capi->enc_cfg_id;

   /* Populate the packet to be sent to logging utility */
   log_info_obj.qxdm_log_code = QXDM_LOG_CODE_AUD_ENC_OUT;
   log_info_obj.buf_ptr = (uint8_t *) buf_addr;
   log_info_obj.buf_size = buf_size;
   log_info_obj.session_id = session_id;
   log_info_obj.log_tap_id = AUDIOLOG_AUD_ENC_BS_OUT_TAP_ID;
   log_info_obj.log_time_stamp = qurt_elite_timer_get_time();

   /** log as PCM if logging is for output and it's PCM encode format
    * OR if logging is for input and it's not compressed format
    */
   if (AudioEncSvc_IsPcmFmt(ulEncCfgId))
   {
      log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_PCM;

      pcm_data_info *pcm_data = &(log_info_obj.data_info.pcm_data_fmt);

      /* Get the sampling rate and number of channels in for the input. */

      pcm_data->num_channels = pMe->out_med_fmt_per_enc_cfg.num_channels;
      pcm_data->sampling_rate = pMe->out_med_fmt_per_enc_cfg.sample_rate;
      pcm_data->bits_per_sample = pMe->out_med_fmt_per_enc_cfg.bytes_per_sample * 8;
      pcm_data->interleaved = TRUE; //output is always interleaved.
      pcm_data->channel_mapping = pMe->out_med_fmt_per_enc_cfg.chan_map;
   }
   else
   {
      log_info_obj.data_fmt = ELITE_LOG_DATA_FMT_BITSTREAM;
      if ((pMe->io_fmt_conv != ENC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_61937) &&
            (pMe->io_fmt_conv != ENC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_COP))
      {
         //for regular encode, or 61937 to raw, this is right.
         log_info_obj.data_info.media_fmt_id = ulEncCfgId;
      }
      else
      {
         //for pcm to 61937, media fmt is 0
         log_info_obj.data_info.media_fmt_id = 0;
      }
   }

   /* Allocate the log buffer and log the packet
    If log code is disabled, log buffer allocation returns NULL
    */
   result = elite_allocbuf_and_log_data_pkt(&log_info_obj);

   return result;
}

bool_t AudioEncSvc_IsLastCapi(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container)
{
   for (uint8_t i=0; i<ENC_SVC_MAX_CAPI; i++)
   {
      if (capi_container == pMe->capiContainer[i])
      {
         if ( (i == (ENC_SVC_MAX_CAPI-1)) || (pMe->capiContainer[i+1] == NULL) )
         {
            return TRUE;
         }
      }
   }
   return FALSE;
}

enc_CAPI_container_t *AudioEncSvc_GetFirstCapi(AudioEncSvc_t *pMe)
{
   return pMe->capiContainer[0];
}

enc_CAPI_container_t *AudioEncSvc_GetLastCapi(AudioEncSvc_t *pMe)
{
   uint8_t i;
   for (i = 0; i<ENC_SVC_MAX_CAPI; i++)
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

ADSPResult AudioEncSvc_NotifyPeerSvcWithMediaFmtUpdate (AudioEncSvc_t* pMe,
      uint32_t nSampleRateAfter,
      uint32_t nChannelsAfter,
      uint16_t *ucChannelMappingAfter,
      uint16_t output_bits_per_sample)
{
   elite_msg_any_t msg;
   ADSPResult nResult=ADSP_EOK;

   if (NULL == pMe->pPeerSvc) return nResult;

   uint32_t nPayloadSize = sizeof(elite_msg_data_media_type_apr_t) + sizeof(elite_multi_channel_pcm_fmt_blk_t);
   if ( ADSP_FAILED( elite_msg_create_msg(&msg,&nPayloadSize,
         ELITE_DATA_MEDIA_TYPE,
         NULL, 0,0 ) ))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to create media fmt message");
      return ADSP_EFAILED;
   }

   elite_msg_data_media_type_apr_t *pCmdMsgPayload = (elite_msg_data_media_type_apr_t*) (msg.pPayload);
   pCmdMsgPayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
   pCmdMsgPayload->unMediaFormatID   = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

   elite_multi_channel_pcm_fmt_blk_t *pPcmFormatBlock =
         (elite_multi_channel_pcm_fmt_blk_t*) ((uint8_t*)(pCmdMsgPayload)+sizeof(elite_msg_data_media_type_apr_t));
   pPcmFormatBlock->num_channels = nChannelsAfter;

   if(16 < output_bits_per_sample)
   {
      pPcmFormatBlock->bits_per_sample = 32;
   }
   else
   {
      pPcmFormatBlock->bits_per_sample = 16;
   }

   pPcmFormatBlock->sample_rate = nSampleRateAfter;
   pPcmFormatBlock->is_signed = 1;
   pPcmFormatBlock->is_interleaved = AUD_ENC_INTERLEAVED;

   if (ucChannelMappingAfter)
   {
      memscpy( pPcmFormatBlock->channel_mapping, sizeof(pPcmFormatBlock->channel_mapping), ucChannelMappingAfter, nChannelsAfter);
   }
   else
   {
      memset(pPcmFormatBlock->channel_mapping, 0, sizeof(pPcmFormatBlock->channel_mapping));
   }

   nResult = qurt_elite_queue_push_back(pMe->pPeerSvc->dataQ, (uint64_t*)&msg );

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "Reconfig media format from encoder: FMTID 0x%lx, CH %d, BPS %d, SR %lu, isSigned %d, isInterleaved %d, Result %d",
         pCmdMsgPayload->unMediaFormatID,pPcmFormatBlock->num_channels,
         pPcmFormatBlock->bits_per_sample,pPcmFormatBlock->sample_rate,
         pPcmFormatBlock->is_signed,pPcmFormatBlock->is_interleaved,nResult);

   if (pMe->is_rt_enc)
   {
      AudioEncSvc_UpdateInitialDelay(pMe);
   }

   return nResult;
}

ADSPResult AudioEncSvc_RegisterWithAdsppm(AudioEncSvc_t* pMe)
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

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by Enc. Result %lu. Client id %lu", result, pMe->ulAdsppmClientId);

#endif
   return result;
}

ADSPResult AudioEncSvc_DeregisterWithAdsppm(AudioEncSvc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->ulAdsppmClientId) )
   {
      return ADSP_EOK;
   }

   uint32_t client_id =  pMe->ulAdsppmClientId;
   result = qurt_elite_adsppm_wrapper_deregister(&pMe->ulAdsppmClientId, &pMe->adsppmClientPtr);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by Enc. Result %lu. Client id %lu", result, client_id);
   return result;
}

/**
 * total BW = sum of CAPI BW + svc contribution.
 */
static uint32_t AudioEncSvc_DetermineTotalBw(AudioEncSvc_t* pMe)
{
   uint32_t bw = 0;

   for (uint8_t capi_i = 0; capi_i < ENC_SVC_MAX_CAPI; capi_i++)
   {
      if (!pMe->capiContainer[capi_i]) break;

      bw += (pMe->capiContainer[capi_i]->data_bw + pMe->capiContainer[capi_i]->code_bw);
   }
   return bw + pMe->svc_bw;
}


/*
 * svc BW = input BW + output BW. Input BW is considered only for PCM.
 *
 * Also this BW should ideally be InpStrm, OutStrm property. But for now, its property of AudioEncSvc_t
 */
static void AudioEncSvc_DetermineSvcBw(AudioEncSvc_t* pMe, elite_multi_channel_pcm_fmt_blk_t *pPcmFormatBlock)
{
   uint32_t bw = 0;
   if (NULL != pPcmFormatBlock)
   {
      bw += pPcmFormatBlock->sample_rate * \
            pPcmFormatBlock->num_channels * \
            pPcmFormatBlock->bits_per_sample/8; //this works for compressed as well since 61937 is as good as pcm.
   }

   for (uint8_t capi_i = 0; capi_i < ENC_SVC_MAX_CAPI; capi_i++)
   {
      if (!pMe->capiContainer[capi_i]) break;

      switch(pMe->capiContainer[capi_i]->enc_cfg_id)
      {
      case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2:
      case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3:
      case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4:
      {
         uint32_t num_chan = pMe->out_med_fmt_per_enc_cfg.num_channels;
         uint32_t sample_rate = pMe->out_med_fmt_per_enc_cfg.sample_rate;
         uint32_t bps = pMe->out_med_fmt_per_enc_cfg.bits_per_sample;

         bw += bps / 8 * sample_rate * num_chan;

         if (sample_rate > 48000)
         {
            bw += 6*1024*1024;
         }
         if (num_chan > 2)
         {
            bw += 7*1024*1024;
         }
      }
      break;
      default:
         break;
      }
   }

   pMe->svc_bw = bw;
}


static ADSPResult AudioEncSvc_AggregateKppsRequired(AudioEncSvc_t *pMe, uint32_t *encoder_kpps)
{
   int kpps;
   *encoder_kpps = 0;

   for (uint8_t i=0; i<ENC_SVC_MAX_CAPI; i++)
   {
      if (!pMe->capiContainer[i]) break;

      if (pMe->capiContainer[i] && pMe->capiContainer[i]->capi_ptr)
      {
         kpps = pMe->capiContainer[i]->kpps;
         if (0==kpps) kpps = GENERIC_ENC_KPPS;
      }
      else if(AudioEncSvc_IsPcmFmt(pMe->capiContainer[i]->enc_cfg_id))
      {
         /* Aggregating MPPS votes for PCM encoder using media format */
         /* Scaling to a minimum of 4 MPPS to avoid very less vote */
         uint32_t bytes_per_sample = pMe->in_med_fmt.bits_per_sample;
         kpps = (PCM_ENC_KPPS_FACTOR * pMe->in_med_fmt.num_channels * pMe->in_med_fmt.sample_rate * bytes_per_sample)/1000;
         kpps = (kpps > PCM_ENC_MIN_KPPS) ? kpps : PCM_ENC_MIN_KPPS;
      }
      else
      {
         kpps = GENERIC_ENC_KPPS;   //default mips
      }

      *encoder_kpps += kpps;  //aggregate kpps.
   }

   return ADSP_EOK;
}

/**
 * force_vote doesn't matter if is_release=TRUE
 *
 * is_release helps in releasing BW even when aggregated BW is nonzero, useful for suspend.
 *
 * force_vote helps in voting BW due to changes in svc & not due to CAPI V2 events.
 */
ADSPResult AudioEncSvc_ProcessKppsBw(AudioEncSvc_t *pMe, bool_t is_release, bool_t force_vote)
{
   ADSPResult result = ADSP_EOK;

   uint32_t encoder_kpps=0, gen_bw = 0;

   if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->ulAdsppmClientId) )
   {
      return ADSP_EFAILED;
   }

   if (!is_release)
   {
      if ( (pMe->event_mask & AUD_ENC_SVC_EVENT__KPPS_MASK) || force_vote)
      {
         result = AudioEncSvc_AggregateKppsRequired(pMe, &encoder_kpps);
         if(ADSP_FAILED(result))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to aggregate kpps");
         }

         pMe->event_mask &= ~AUD_ENC_SVC_EVENT__KPPS_MASK;
      }
      else
      {
         encoder_kpps = pMe->prev_kpps_vote;
      }

      if ( (pMe->event_mask & AUD_ENC_SVC_EVENT__BW_MASK) || force_vote)
      {
         gen_bw = AudioEncSvc_DetermineTotalBw(pMe);

         //clear the event
         pMe->event_mask &= ~AUD_ENC_SVC_EVENT__BW_MASK;

      }
      else
      {
         gen_bw = pMe->prev_bw_vote;
      }
   }

   //If there was no event or no release-call, or no foce vote or there was no change, return.
   if (!(is_release || (pMe->prev_kpps_vote != encoder_kpps) || (pMe->prev_bw_vote != gen_bw)))
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
   bool_t both_diff = ( (encoder_kpps > 0) ^ (gen_bw > 0) );
   //whether mips req is made
   bool_t is_mips_req = (pMe->prev_kpps_vote != encoder_kpps) || is_release ;
   //whether bw req is made
   bool_t is_bw_req = ( (pMe->prev_bw_vote != gen_bw) || is_release );

   bool_t req_done = false;

   if ( is_mips_req )
   {
      pMe->prev_kpps_vote = encoder_kpps;

      mpps = (encoder_kpps+999)/1000;

      //Requesting 0 will be equivalent to releasing particular resource
      mmpmMppsParam.mppsTotal                  = mpps;
      mmpmMppsParam.adspFloorClock             = 0;
      rscParam[req_num].rscId                   = MMPM_RSC_ID_MPPS;
      rscParam[req_num].rscParam.pMppsReq       = &mmpmMppsParam;

      req_num++;

      if (both_diff || !is_bw_req) //request separately if either bw and mips are differet types (req/rel), or if there's no BW req.
      {
         reqParam.numOfReq                   = req_num;

         if (encoder_kpps == 0)
         {
            result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release MPPS by Enc (%lu). Result %lu",pMe->ulAdsppmClientId, result);
         }
         else
         {
            result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu by Enc (%lu). Result %lu", mpps, pMe->ulAdsppmClientId,result);
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
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW by Enc (%lu). Result %lu", pMe->ulAdsppmClientId, result);
         }
         else
         {
            result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request BW %lu KBPS by Enc (%lu). Result %lu", gen_bw/1024, pMe->ulAdsppmClientId, result);
         }
         req_done = true;
         req_num = 0; //reset req_num as req is already made.
      }
   }

   if (req_num && !req_done)
   {
      reqParam.numOfReq                   = req_num;

      if ( (encoder_kpps == 0) && (gen_bw == 0) )
      {
         result = qurt_elite_adsppm_wrapper_release(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW and MPPS by Enc (%lu). Result %lu", pMe->ulAdsppmClientId,result);
      }
      else
      {
         result = qurt_elite_adsppm_wrapper_request(pMe->ulAdsppmClientId, &pMe->adsppmClientPtr, &reqParam);
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu and BW %lu KBPS by Enc (%lu). Result %lu", mpps, gen_bw/1024, pMe->ulAdsppmClientId, result);
      }
   }

#endif //#if (ADSPPM_INTEGRATION==1)
   return result;
}

ADSPResult AudioEncSvc_HandleFwkExtnBeforeProcess(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container, uint32_t pcm_bytes_consumed_per_frame, bool_t has_bytes_consumed_changed)
{
   ADSPResult result = ADSP_EOK;
   for (uint16_t i = 0; i < capi_container->num_fmwk_extns.num_extensions; i++)
   {
      switch(capi_container->fmwk_extns[i].id)
      {
      case FWK_EXTN_COMPR_OVER_PCM:
      {
         if (has_bytes_consumed_changed)
         {
            fwk_extn_param_id_compr_over_pcm_pcm_frame_length_payload_t frame;
            uint32_t bytes_per_sample = ((pMe->in_med_fmt.bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO);
            frame.pcm_frame_length_per_ch = pcm_bytes_consumed_per_frame / (bytes_per_sample * pMe->in_med_fmt.num_channels);
            ADSPResult res = aud_enc_svc_set_param(capi_container->capi_ptr, FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_FRAME_LENGTH, (int8_t*)&frame, sizeof(frame));
            if (ADSP_FAILED(res))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "setting fwk extn param FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_FRAME_LENGTH failed");
            }

            bool_t buf_was_popped = pMe->outDataBufferNode.pBuffer != NULL;

            //setting PCM frame length can change the port-threshold.
            AudioEncSvc_HandlePortDataThreshChangeEvent(pMe);

            if (buf_was_popped && (pMe->outDataBufferNode.pBuffer == NULL))
            {
               AudioEncSvc_GetExtOutputBuf(pMe);
            }

         }
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FWK EXTN 0x%lx not implemented before process", capi_container->fmwk_extns[i].id);
      }
      }
   }

   return result;
}

ADSPResult AudioEncSvc_HandleFwkExtnAtInit(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container)
{
   ADSPResult result = ADSP_EOK;
   for (uint16_t i = 0; i < capi_container->num_fmwk_extns.num_extensions; i++)
   {
      switch(capi_container->fmwk_extns[i].id)
      {
      case FWK_EXTN_COMPR_OVER_PCM:
      {
         fwk_extn_param_id_compr_over_pcm_config_zero_padding_t cfg = {0};
         cfg.zero_padding_config = 0;//FWK_EXTN_CONST_COMPR_OVER_PCM_DISABLE_ZERO_PAD;

         ADSPResult res = aud_enc_svc_set_param(capi_container->capi_ptr, FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_CONFIG_ZERO_PADDING, (int8_t*)&cfg, sizeof(cfg));
         if (ADSP_FAILED(res))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "setting fwk extn param FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_CONFIG_ZERO_PADDING failed");
         }
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FWK EXTN 0x%lx not implemented at init", capi_container->fmwk_extns[i].id);
      }
      }
   }
   return result;
}

ADSPResult AudioEncSvc_HandleFwkExtnAtMediaFmt(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container)
{
   ADSPResult result = ADSP_EOK;
   for (uint16_t i = 0; i < capi_container->num_fmwk_extns.num_extensions; i++)
   {
      switch(capi_container->fmwk_extns[i].id)
      {
      case FWK_EXTN_COMPR_OVER_PCM:
      {
         fwk_extn_param_id_compr_over_pcm_pcm_media_format_t pcm;
         pcm.bits_per_sample = pMe->in_med_fmt.bits_per_sample;
         memscpy(pcm.channel_type, sizeof(pcm.channel_type), pMe->in_med_fmt.channel_mapping, sizeof(pMe->in_med_fmt.channel_mapping));
         pcm.channels = pMe->in_med_fmt.num_channels;
         pcm.sampling_rate = pMe->in_med_fmt.sample_rate;
         ADSPResult res = aud_enc_svc_set_param(capi_container->capi_ptr, FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_MEDIA_FORMAT, (int8_t*)&pcm, sizeof(pcm));
         if (ADSP_FAILED(res))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "setting fwk extn param FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_MEDIA_FORMAT failed");
         }

         //also set worst case pcm frame length decided based on input thresh of first capi
         //setting this will cause COP CAPI to raise output port thresh event - ensures output buffer is created with rigth size for sending zeros to AFE.
         enc_CAPI_container_t *first_capi = AudioEncSvc_GetFirstCapi(pMe);
         fwk_extn_param_id_compr_over_pcm_pcm_frame_length_payload_t frame;

         uint32_t bytes_per_sample = ((pMe->in_med_fmt.bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO);
         frame.pcm_frame_length_per_ch = first_capi->in_port_thresh / (bytes_per_sample * pMe->in_med_fmt.num_channels);
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "setting fwk extn param FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_FRAME_LENGTH with value %lu (assumed to be worst case frame len)", frame.pcm_frame_length_per_ch);
         res = aud_enc_svc_set_param(capi_container->capi_ptr, FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_FRAME_LENGTH, (int8_t*)&frame, sizeof(frame));
         if (ADSP_FAILED(res))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "setting fwk extn param FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_FRAME_LENGTH failed");
         }

         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FWK EXTN 0x%lx not implemented at in-media fmt", capi_container->fmwk_extns[i].id);
      }
      }
   }
   return result;
}

/**
 * Info about delay calculation:
 * even though one zero buf is sent, delay is calculated based on outbufs_allocated
 * this is because delay is updated at media fmt, at which time one frame was collected and encoded.
 * the time this should take is 2 * frame-size. Including zero buf (outbufs_allocated), 3 buf delays seen
 *
 * delay is updated even before output media fmt is processed. this is ok as in enc, output media fmt doesn't change after it's set.
 *
 * worst case delay is considered (max_data_len)
 *
 * if encoder doesn't take frame-size amount of time to encode, then actual delay is going to be lesser.
 *
 */
static void AudioEncSvc_UpdateInitialDelay(AudioEncSvc_t *pMe)
{
   //delay in steady state is only due to initial zeros.
   //ideally this delay must be removed when all buffers come back to enc svc; but currently no such use case exists.
   if (pMe->delay_ptr)
   {
      enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);

      if ( (CAPI_V2_FIXED_POINT == last_capi->out_med_fmt.main.format_header.data_format) ||
            (CAPI_V2_IEC61937_PACKETIZED == last_capi->out_med_fmt.main.format_header.data_format) ||
            (CAPI_V2_COMPR_OVER_PCM_PACKETIZED == last_capi->out_med_fmt.main.format_header.data_format))
      {
         uint32_t total_data = pMe->outbufs_allocated * last_capi->out_buf_params.out_buf.max_data_len;

         uint32_t bytes_per_sample = (last_capi->out_med_fmt.std.bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
         uint32_t bytes_p_sec = (last_capi->out_med_fmt.std.sampling_rate * last_capi->out_med_fmt.std.num_channels * bytes_per_sample);

         uint32_t delay   = (((uint64_t)total_data * NUM_US_PER_SEC) + bytes_p_sec - 1) / bytes_p_sec;

         if (*pMe->delay_ptr != delay)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                  "Updating encoder delay from %lu to %lu",*pMe->delay_ptr, delay);
            *pMe->delay_ptr = delay;
         }
      }
   }
}

/**
 * initial zeros are pushed after first buf is processed. until then AFE will underrun.
 * in case first buf processing happens early, then these zeros will help in reserving the processing time for next frames.
 * the zeros also help in fractional frame size such as 2.5 ms frame.
 *
 */
ADSPResult AudioEncSvc_PushInitialZeros(AudioEncSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   enc_CAPI_container_t *last_capi = AudioEncSvc_GetLastCapi(pMe);
   uint32_t count = 0;

   //at this time we are holding on to one buf. take back up.
   qurt_elite_bufmgr_node_t currentBufNode = pMe->outDataBufferNode;
   uint32_t size_zeros = last_capi->out_buf_params.out_buf.actual_data_len;
   EncSvc_OutBufParams_t outparams = last_capi->out_buf_params;
   pMe->outDataBufferNode.pBuffer = NULL;

   for (uint32_t i = 0; i < 1; i++) //only one zero buf, since we send this down at media-fmt.
   {
      AudioEncSvc_GetExtOutputBuf(pMe);

      if (pMe->outDataBufferNode.pBuffer == NULL)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Encoder will not be able to send required amount of initial zeros. sent %lu only out of 1",count+1);
         break;
      }

      if (0 == last_capi->out_buf_params.out_buf.actual_data_len)
      {
         elite_msg_data_buffer_t *pOutBuf = (elite_msg_data_buffer_t*) (pMe->outDataBufferNode.pBuffer);
         memset(&pOutBuf->nDataBuf, 0, size_zeros);
         last_capi->out_buf_params.out_buf.actual_data_len = size_zeros;

         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "Encoder sending %lu th zero buffer of size %lu to AFE", count+1, last_capi->out_buf_params.out_buf.actual_data_len);

         count++;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "buffer expected to have no data, but it seems to have %lu bytes", last_capi->out_buf_params.out_buf.actual_data_len);
      }

      AudioEncSvc_SendOutBufToPeerSvc(pMe);
   }

   //any change in this func might need updates in AudioEncSvc_UpdateInitialDelay.

   //restore.
   pMe->outDataBufferNode = currentBufNode;
   last_capi->out_buf_params = outparams;

   return result;
}
