/*========================================================================

*//** @file AudioEncSvc_Includes.h
This file contains Elite Audio encoder service include components.

Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_enc_svc/src/AudioEncSvc_Includes.h#2 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
09/29/10   RKC      Created file.

========================================================================== */

#ifndef _AUDIOENSVC_INC_H_
#define _AUDIOENSVC_INC_H_

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_AprIf.h"

#include "Elite_CAPI_V2.h"
#include "Elite_CAPI_V2_private_params.h"
#include "capi_v2_adsp_error_code_converter.h"
#include "Elite_fwk_extns_compr_over_pcm.h"

#include "AudioEncSvc_CapiV2CallbackHandler.h"
#include "AudioEncSvc.h"
#include <audio_basic_op_ext.h>

#include "adsp_asm_api.h"
#include "adsp_media_fmt.h"
#include "Interleaver.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "byte_conversion.h"
#include "avtimer_drv_api.h"
#include "EliteLoggingUtils_i.h"
#include "qurt_elite_adsppm_wrapper.h"
#include "adsp_avs_common_api.h"
#include "adsp_private_api.h"

#include "AudioEncSvcImc.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

static const uint32_t BITS_IN_BYTE = 8;
static const uint32_t AUD_ENC_CMD_SIG = 0x80000000;
static const uint32_t AUD_ENC_BUF_SIG = 0x40000000; //output
static const uint32_t AUD_ENC_DATA_SIG = 0x20000000; //input
static const uint32_t AUD_ENC_NUM_Q = 3;
static const uint64_t AUD_MAX_INT64 = (0x7FFFFFFFFFFFFFFFULL);
static const uint32_t AUD_ENC_STATE_EOS = 1;
static const uint32_t AUD_ENC_MASK_EVEN = 0xFFFFFFFE;
static const uint32_t AUD_ENC_MASK_MULTIPLE_OF_8 = 0xFFFFFFF8;
static const uint32_t AUD_ENC_MASK_MSB_24 = 0xFFFFFF00;
static const uint64_t NUM_US_PER_SEC = 1000000;
static const uint64_t NUM_MS_PER_SEC = 1000;
static const uint32_t MASK_32BYTE_ALIGNMENT = 0x1F;
static const uint32_t MAX_NUM_CHANNELS = 8;
static const uint32_t NUM_SAMPLES_IN_IEC61937_PREAMBLE = 4;

// maximum data messages in data queue. Must consider data buffers + EOS messages.
static const uint32_t MAX_DATA_Q_ELEMENTS = 16;
// max number of dataQ elements. this should be max time any encoder takes to process in ms.
static const uint32_t MAX_DATA_Q_ELEMENTS_RT = 128;

// maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

//worst case number of buffers in queue
static const uint32_t MAX_BUF_Q_ELEMENTS = 32;

// queue must be sized to be larger than max # of msgs, and a power of 2.
static const uint32_t BUF_Q_ELEMENTS_SIZE = 16;

static const uint32_t BYTES_PER_SAMPLE_TWO = 2;
static const uint32_t BYTES_PER_SAMPLE_FOUR = 4;

// thread settings. Make configurable? Probably this is good enough.
static const uint32_t THREAD_PRIO = 100;

//Max allowable difference between the actual and expected timestamps in microseconds
static const int64_t TIMESTAMP_TOLERANCE = 1000;

//max number of concurrent CAPIs
static const uint8_t ENC_SVC_MAX_CAPI = 2;

#define PCM_ENC_KPPS_FACTOR    6 /* Number of transactions of pcm data*/
#define PCM_ENC_MIN_KPPS    4000 /* Minimum KPPS for PCM encoder */

//KPPS requirement
#define GENERIC_ENC_KPPS    13000

/* Tapping point ID's for Encoder service*/
#define AUDIOLOG_AUD_ENC_PCM_IN_TAP_ID    0x0001
#define AUDIOLOG_AUD_ENC_BS_OUT_TAP_ID    0x0002

//buffer size in ms for regular latency
#define BUF_SIZE_REG_LATENCY_MS   5
#define BUF_SIZE_LOW_LATENCY_MS   1

/** Event masks */
#define AUD_ENC_SVC_EVENT__KPPS_MASK   (0x80000000)
#define AUD_ENC_SVC_EVENT__KPPS_SHIFT  (31)
#define AUD_ENC_SVC_EVENT__BW_MASK     (0x40000000)
#define AUD_ENC_SVC_EVENT__BW_SHIFT    (30)
#define AUD_ENC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK   (0x10000000)
#define AUD_ENC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_SHIFT  (28)

#define ENC_MAX(x, y) (x > y ? x : y)
/*----------------------------------------------------------------------------
* Global Data Definitions
* -------------------------------------------------------------------------*/
typedef enum Enc_CAPI_Type
{
   ENC_CAPI_TYPE_AUTO,
   ENC_CAPI_TYPE_DEPACKETIZER, //61937
   ENC_CAPI_TYPE_PACKETIZER, //61937
   ENC_CAPI_TYPE_COP_PACKETIZER, //COP
   ENC_CAPI_TYPE_COP_DEPACKETIZER //COP
} Enc_CAPI_Type;

typedef enum Enc_Endianness_Type
{
   ENC_LITTLE_ENDIAN,
   ENC_BIG_ENDIAN
} Enc_Endianness_Type;

extern uint32_t asmMemoryMapClient;

typedef struct audio_enc_svc_capi_v2_media_fmt_t
{
   capi_v2_set_get_media_format_t main;
   union
   {
      capi_v2_standard_data_format_t std;
      capi_v2_raw_compressed_data_format_t raw_fmt;
   };
} audio_enc_svc_capi_v2_media_fmt_t;



struct EncSvc_InpBufParams_t
{
    capi_v2_buf_t                       in_buf;
    capi_v2_stream_data_t               inputs;
    capi_v2_stream_data_t               *inputs_ptr[1];
} ;

struct EncSvc_OutBufParams_t
{
    capi_v2_buf_t                       out_buf;
    capi_v2_stream_data_t               outputs;
    capi_v2_stream_data_t               *outputs_ptr[1];

    //Number of frames filled onto the output buffer
    uint16_t                    usNumFramesInOutBuf;

    //Used to store framesPerBuffer given by client and thereafter
    //it acts an index to fit in best possible encode frames <=N and >1
    uint16_t                    usTargetFramesPerBuf;

    //Flag to indicate if metadata has to added, 1:Yes, 0:No
    bool_t                      bFrameMetaDataEnable;

    uint32_t                    maxBufSize; //max allocated size. max_len in out_buf is used for intermediate calc (esp. for last-capi)
} ;

//Parameters related to encoder timestamp
//All timestamp values are in units of microseconds
struct TimeStampParams_t
{
   //time stamp to be used for the last output buffer
   uint64_t                     ullTimeStamp;
   //time stamp for the last encoded frame
   uint64_t                     ullFrameTimeStamp;
   //time stamp of last PCM buffer from upstream
   uint64_t                     ullLastBufferTimeStamp;
   //timestamp expected for current buffer based on previous buffer timestamp
   uint64_t                     ullExpectedTimeStamp;
   //flag to indicate if time stamp of last PCM buffer is valid
   bool_t                       isLastBufferTimeStampValid;
   //flag to indicate if time stamp of last frame valid
   bool_t                       isTimeStampValid;
   //flag indicating if there was a mismatch between received buffer timestamp
   //and expected timestamp
   bool_t                       timestamp_mismatch;
   //flag to indicate if the latest copy was from beginning of new buffer
   int64_t                      timestamp_offset;
   // The incoming timesamp may be negative if the stream is working in relative timestamps
   // mode and the PP delay is nonzero. In that case, add this constant offset to make the
   // timestamp of the first buffer start with zero.
   bool_t                       isLastCopyFromNewBuffer;
   //stores the samples in internal buffer before copying over from input buffer
   uint32_t                     samples_in_internal_buf_before_copy;
};

// Memory blob for all queues used by Encoder Svc
struct EncSvc_MsgQueueBlobs_t
{
   QURT_ELITE_ALIGN( uint8_t ucDataQ[ QURT_ELITE_QUEUE_GET_REQUIRED_BYTES( MAX_DATA_Q_ELEMENTS)],
                8 );  /**< Memory  holding data queue. Aligned to 8 byte boundary */
   QURT_ELITE_ALIGN( uint8_t ucCmdQ[ QURT_ELITE_QUEUE_GET_REQUIRED_BYTES( MAX_CMD_Q_ELEMENTS)],
                8 );  /**< Memory holding commnd queue. Aligned to 8 byte boundary */
   QURT_ELITE_ALIGN( uint8_t ucGpQ[ QURT_ELITE_QUEUE_GET_REQUIRED_BYTES( BUF_Q_ELEMENTS_SIZE)],
                8 );  /**< Memory holding GP queue. Aligned to 8 byte boundary */
} ;

typedef ADSPResult (*enc_capi_destroy_fn)(capi_v2_t **capi_ptr_ptr);

/** parameters applicable to the whole encoder svc. */
typedef struct enc_init_time_get_params_t
{
   uint32_t max_stack_size;
} enc_init_time_get_params_t;

typedef struct enc_capi_port_index_t
{
      uint16_t index;
      bool_t   valid;

} enc_capi_port_index_t;

static const capi_v2_framework_extension_id_t enc_svc_supported_fmwk_extns[] = { {FWK_EXTN_COMPR_OVER_PCM} };

#define ENC_SVC_MAX_SUPPORTED_FMWK_EXTNS (sizeof(enc_svc_supported_fmwk_extns)/sizeof(enc_svc_supported_fmwk_extns[0]))

/** parameters applicable to the each CAPI. */
typedef struct enc_CAPI_init_time_get_params_t
{
   // ------ input
   enc_capi_port_index_t input_port_index;
   enc_capi_port_index_t output_port_index;

   // ------ output
   capi_v2_port_data_threshold_t in_port_thresh;
   capi_v2_port_data_threshold_t out_port_thresh;
   capi_v2_stack_size_t stack_size;

} enc_CAPI_init_time_get_params_t;

typedef enum enc_CAPI_V2_presence
{
   ENC_CAPI_V2_PRESENCE_UNKNOWN=0,
   ENC_CAPI_V2_PRESENCE_NOT_PRESENT,
   ENC_CAPI_V2_PRESENCE_PRESENT
}enc_CAPI_V2_presence;

typedef enum enc_AMDB_presence
{
   ENC_AMDB_PRESENCE_NOT_PRESENT = 0,
   ENC_AMDB_PRESENCE_PRESENT_AS_CAPI_V1,
   ENC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2,
   ENC_AMDB_PRESENCE_PRESENT_AS_STUB
}enc_AMDB_presence;


typedef struct enc_CAPI_init_params_t
{
   // ------ input
   uint32_t enc_cfg_id;
   capi_v2_event_callback_info_t cb_info;
   void *amdb_handle;

   // ------ output
   enc_capi_destroy_fn  enc_destroy_fn;
   enc_CAPI_V2_presence  capi_v2_presence;

} enc_CAPI_init_params_t;

/**
 * below fields support CAPI_V2_DATA_FORMAT_INVALID_VAL when invalid.
 */
struct EncMediaFmt_t
{
   uint32_t       sample_rate;
   uint32_t       bits_per_sample;
   uint32_t       bytes_per_sample;     //number of bytes for sample of output 2,3,4 for PCM formats only.
                                        //for other formats output bits is obtained from CAPI. Input bits p s is available in media fmt.
   uint16_t       num_channels;
   uint8_t        chan_map[CAPI_V2_MAX_CHANNELS];
   
   Enc_Endianness_Type	endianness;

   uint32_t       fmt_id;  //needed only for depacketizer use case
};

struct enc_CAPI_container_t
{
   capi_v2_t                           *capi_ptr;

   enc_CAPI_callback_obj_t             capi_callback_obj;
   enc_capi_destroy_fn                 enc_destroy_fn;

   uint32_t                            kpps; //total kpps of this capi for all its ports. needed to check for change.
   uint32_t                            code_bw;
   uint32_t                            data_bw;

   uint32_t                            in_port_thresh;
   uint32_t                            out_port_thresh;

   bool_t                              media_fmt_event;

   audio_enc_svc_capi_v2_media_fmt_t   out_med_fmt; //after CAPI raises out media fmt, it's stored here.
         //out_media_fmt for packetizers, PCM should be similar to out_med_fmt_per_enc_cfg.
         //for encoder, out_med_fmt is bit stream.

   uint32_t                            enc_cfg_id; //enc fmt id the enc is created with. for depack->detected enc cfg is in out_med_fmt
   EncSvc_InpBufParams_t               in_buf_params;
   EncSvc_OutBufParams_t               out_buf_params;

   //Stores the minimum number of samples (inclu all ch) that encoder needs to encode
   //a frame, basically encoder input size in Samples. for pcm 240 is returned by vcapi but this var is not used.
   uint32_t                             unEncInpFrameSize; //TODO: CAPI v2 don't use this. this can be used only for current CAPI V1 under the wrapper.

   capi_v2_num_needed_framework_extensions_t num_fmwk_extns;
   capi_v2_framework_extension_id_t    fmwk_extns[ENC_SVC_MAX_SUPPORTED_FMWK_EXTNS];

   uint32_t                            bytes_consumed_per_frame; //updated after encoding a frame
};

typedef struct enc_push_mode_t enc_push_mode_t;

struct AudioEncSvc_t
{
   //handle to give out to others
   elite_svc_handle_t                   serviceHandle;
   eEncoderMode                         ulMode;
   //peer service handle
   elite_svc_handle_t                   *pPeerSvc;
   //mask for MQ's owned by this obj
   qurt_elite_channel_t                 channel;
   //Fields with 1 in this indicate the signals that we are awaiting at that moment
   uint32_t                             unCurrentBitfield;
   //Fields with 1 in this indicate the those signals are set at that moment
   uint32_t                             unChannelStatus;
   //container for CAPIs
   enc_CAPI_container_t                 *capiContainer[ENC_SVC_MAX_CAPI];
   enc_CAPI_container_t                 *mainCapiContainer;
   //Message structs for Input Data Msg, client Buffer Msg
   elite_msg_any_t                      CmdMsg;
   elite_msg_any_t                      InpDataMsg;
   //the following two vars should be mutually exclusive:
   //OutDataMsg is used for processing output buffers from client,
   //outDataBufferNode is for output buffers allocated by encoder service itself
   elite_msg_any_t                      OutDataMsg;
   qurt_elite_bufmgr_node_t             outDataBufferNode;
   //Shared Memory Node for Client buffers
   elite_mem_shared_memory_map_t        SharedMemMapTypeNode;
   elite_mem_shared_memory_map_t        SharedMemMapTypeNodeBackUp;
   //Params structure to store the offset and sizes related to IO buffers
   //EncSvcBufParams_t                    encBufParams;
   //Parameters related to encoder timestamp
   TimeStampParams_t                    encTSParams;
   //Used to differentiate between External and Internal EOS
   //and based on this EOS ACK is either sent to client to ASM
   bool_t                               isExternalEos;
   //flag to indicate whether we should wait for enc cfg blk
   bool_t                               bWaitForEncCfg;
   //flag to indicate whether we are waiting for input format
   bool_t                               bWaitForInpMediaFmt;

   //flag to indicate end of stream
   bool_t                               endOfStream;

   bool_t                               isPaused;

   bool_t                               waitForFlushReadBufs;

   //flags to indicate native mode
   bool_t                               isNativeChannels;
   bool_t                               isNativeSamplerate;

   // AudioEncSvc store a copy of call back data
   AudioStreamMgr_CallBackHandleType    CbData;
   // read buffer received count: for loopback/AFE -> this is not accurate due to realloc
   uint32_t                             buf_recv_cnt;
   // read buffer done count: used for debug & to identify first encoded buf in AFE encoding cases.
   uint32_t                             buf_done_cnt;

   uint32_t                             ulDataLogId;
   // data logging id. upper 16 bits: apr port id. lower 16 bits: seq num

   uint32_t                             outbufs_allocated;

   //push mode related
   enc_push_mode_t                      *push_mode_ptr;

   qurt_elite_queue_t                   *event_q_ptr;         /**< Messages will be queued here to raise asynchronous events
                                                              with the static service. */

   uint16_t                              perf_mode;        /**< performance mode in which the encoder is opened
                                                                Supported values are ASM_LEGACY_STREAM_SESSION &
                                                                ASM_LOW_LATENCY_STREAM_SESSION. This mode is
                                                                currently only applicable to PCM playback */
   bool_t                                 no_apr_enc;
                        /**<Flag to indicate if encoder should allocate output buffers by itself (instead of using APR client buffers)
                         * 1 means local output buffers need to be allocated (transcode loopback case)
                         * 0 means output buffers are provided by apr client */
   bool_t                                 is_rt_enc;
                        /** is enc instantiated in real-time path AFE)*/
   eEncoderIOFormatConvType               io_fmt_conv;

   uint32_t                               event_mask;   //event mask indicating which event was raised from CAPI (see description above AUD_ENC_SVC_EVENT__KPPS_MASK)

   elite_multi_channel_pcm_fmt_blk_t    in_med_fmt;  //coming from input media format block.
   EncMediaFmt_t                        out_med_fmt_per_enc_cfg;
      //output media fmt is as per enc cfg
      //the reason why we cannot query CAPI is because CAPI's output is bit stream in general, but we need the corresponding PCM's SR, bps etc
      //it's not possible to query that from CAPI. So info obtained from enc cfg blk is stored here.
      //note that enc cfg block is set_param, when enc_cfg block is not present (depacketizer) or when it's native mode, input media fmt is set
      //Also encoder doesn't support out-media fmt (SR,CM) different from in-media fmt (SR,CM); bps can be different (like in PCM).

   uint32_t                             prev_kpps_vote;     //total KPPS of the decoder.
   uint32_t                             svc_bw;     //BW from decoder svc.
   uint32_t                             prev_bw_vote;   //previous bw vote inclu capi vote

   uint32_t                             ulAdsppmClientId;
   qurt_elite_adsppm_client_t           *adsppmClientPtr;

   volatile uint32_t                   *delay_ptr;    //shared delay ptr. microseconds. **currently only for AFE where a client-token based method is NOT used. hence needs update when used for ASM**

   imc_src_dst_info_t                  *imc_info_ptr; //pointer to destination IMC struct
};

void AudioEncSvc_CopyInpDatatoInternalBuf(AudioEncSvc_t* pMe, enc_CAPI_container_t *capi_container, bool_t *compr_need_more);
ADSPResult AudioEncSvc_CreateCAPIs(AudioEncSvc_t *pMe, AudioEncSvcInitParams_t *pInitParams, enc_init_time_get_params_t *enc_params);
ADSPResult AudioEncSvc_UpdateEncConfig(AudioEncSvc_t *pMe, uint8_t *payload, uint32_t payload_size, uint32_t frames_per_buf);
ADSPResult AudioEncSvc_SetEncBitRate(AudioEncSvc_t *pMe, asm_bitrate_param_t *pEncBitRateParam);
ADSPResult AudioEncSvc_SetParam(AudioEncSvc_t* pMe);
void AudioEncSvc_ResetTimeStampValues(TimeStampParams_t* pTS);
ADSPResult AudioEncSvc_GetOutputBuffer(AudioEncSvc_t* pMe,
                                              elite_apr_packet_t ** ppAprPacket,
                                              asm_data_cmd_read_v2_t ** ppOutBuf);
void AudioEncSvc_FillMetaDataInOutputBuffer(AudioEncSvc_t* pMe,
                                                   uint32_t ulInpPcmSize,
                                                   uint32_t ulEncOutSize,
      asm_data_cmd_read_v2_t *pEncOut,
      uint32_t unOutBufWrOffset);

void AudioEncSvc_FillMetaDataCompressedInOutputBuffer(AudioEncSvc_t* pMe,
                                                   uint32_t ulInpPcmSize,
                                                   uint32_t ulEncOutSize,
      asm_data_cmd_read_v2_t *pEncOut,
      uint32_t unOutBufWrOffset);

ADSPResult AudioEncSvc_ReleaseOutputBuffer(AudioEncSvc_t* pMe,
                                                  uint32_t ulOutBufSize,
                                                  ADSPResult errCode
                                                  );
ADSPResult AudioEncSvc_SendOutBufToPeerSvc (AudioEncSvc_t* pMe);
void AudioEncSvc_InitOutDataBuf(elite_msg_data_buffer_t *pBuf, uint32_t maxSize);
void AudioEncSvc_DestroyOutBufs(AudioEncSvc_t *pMe);
ADSPResult AudioEncSvc_CreateSvcQueues(AudioEncSvc_t *pMe, bool_t is_out_dataQ_needed);
ADSPResult AudioEncSvc_CreateSvcDataQQueue(AudioEncSvc_t *pMe);
void AudioEncSvc_Destroy (AudioEncSvc_t* pMe);
ADSPResult AudioEncSvc_InitInpBufParams(EncSvc_InpBufParams_t *in_buf);
ADSPResult AudioEncSvc_DestroyInpBuf(AudioEncSvc_t* pMe);
ADSPResult AudioEncSvc_InitOutBufParams(EncSvc_OutBufParams_t *pBufParams);
ADSPResult AudioEncSvc_InitInpOutBufParamsOfAllCAPIs(AudioEncSvc_t *pMe);
ADSPResult AudioEncSvc_FlushInpQueue(AudioEncSvc_t *pMe);
ADSPResult AudioEncSvc_FlushOutQueue(AudioEncSvc_t *pMe);
ADSPResult AudioEncSvc_ProcessMediaTypeMsg(AudioEncSvc_t *pMe);
ADSPResult AudioEncSvc_FillPCMMetadata(AudioEncSvc_t* pMe, enc_CAPI_container_t *last_capi);
ADSPResult AudioEncSvc_FillTimeStampState(AudioEncSvc_t* pMe, enc_CAPI_container_t *first_capi);
ADSPResult AudioEncSvc_UpdateTimeStampState(AudioEncSvc_t* pMe,
                                            uint32_t consumed_samples,
                                            uint32_t num_channels,
                                            uint32_t sampling_rate );

ADSPResult AudioEncSvc_CopyWithInterleaving(AudioEncSvc_t* pMe,
                                            uint32_t num_channels,
                                            uint32_t samples_to_copy,
                                            int8_t  *src_ptr,
                                            int8_t  *dest_ptr,
                                            uint32_t bytes_per_samp_out,
                                            uint32_t bytes_per_samp_in,
                                            uint32_t out_Q_format,
                                            uint32_t out_bits_per_sample);

ADSPResult AudioEncSvc_FillMetadataAndReleaseOutputBuf(AudioEncSvc_t *pMe);

static inline bool_t AudioEncSvc_IsPcmFmt(uint32_t encCfgId)
{
   return ( (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 == encCfgId) ||
           (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 == encCfgId) ||
         (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 == encCfgId));

}
static inline bool_t AudioEncSvc_IsOutBufFromClient(AudioEncSvc_t *pMe)
{
   return (0 == pMe->no_apr_enc);
}

static inline bool_t AudioEncSvc_IsLastCapiIndex(AudioEncSvc_t *pMe, uint32_t ind)
{
   return ( (ind == (ENC_SVC_MAX_CAPI-1)) || (pMe->capiContainer[ind+1] == NULL) );
}

ADSPResult AudioEncSvc_LogInputData(int8_t *buf_addr, uint32_t buf_size, AudioEncSvc_t* pMe);
ADSPResult AudioEncSvc_LogOutputData(int8_t *buf_addr, uint32_t buf_size, AudioEncSvc_t* pMe);

ADSPResult AudioEncSvc_UpdatePcmEncCfg(AudioEncSvc_t *pMe, void* pPcmConfig, bool_t isNativeModeAllowed);
ADSPResult AudioEncSvc_UpdateNonPcmEncCfg(AudioEncSvc_t *pMe, void* pConfig);

static inline int32_t AudioEncSvc_GetThreadPriority(AudioEncSvc_t* pMe)
{
   int32_t nPriority = elite_get_thrd_prio(ELITETHREAD_DYNA_ENC_SVC_PRIO_ID);
   if ( (ASM_LOW_LATENCY_STREAM_SESSION == pMe->perf_mode) ||
        (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pMe->perf_mode) || pMe->is_rt_enc)
   {
      nPriority = elite_get_thrd_prio(ELITETHREAD_DYNA_LL_ENC_SVC_PRIO_ID);
   }
   return nPriority;
}

static inline uint32_t AudioEncSvc_GetPcmOutBufSize(uint16_t perf_mode, uint32_t sample_rate, uint16_t bytes_per_sample, uint16_t num_ch)
{
   //Calculate size based on the current media format
   uint32_t unAfeFrameSizeInSamples;
   elite_svc_get_frame_size(sample_rate, &unAfeFrameSizeInSamples);
   uint32_t unDecOutBufSizeBytes = bytes_per_sample * num_ch * (unAfeFrameSizeInSamples);

   //5 AFE-frames for regular latency, 1 for low-latency
   unDecOutBufSizeBytes = (ASM_LEGACY_STREAM_SESSION == perf_mode)? (BUF_SIZE_REG_LATENCY_MS*unDecOutBufSizeBytes) : (BUF_SIZE_LOW_LATENCY_MS*unDecOutBufSizeBytes);
   return unDecOutBufSizeBytes;
}

bool_t AudioEncSvc_IsLastCapi(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container);
enc_CAPI_container_t *AudioEncSvc_GetFirstCapi(AudioEncSvc_t *pMe);
enc_CAPI_container_t *AudioEncSvc_GetLastCapi(AudioEncSvc_t *pMe);
ADSPResult AudioEncSvc_NotifyPeerSvcWithMediaFmtUpdate (AudioEncSvc_t* pMe,
      uint32_t nSampleRateAfter,
      uint32_t nChannelsAfter,
      uint16_t *ucChannelMappingAfter,
      uint16_t output_bits_per_sample);

int  AudioEncSvc_CheckAndConvertDecErrCodeToEncErrCode(int code);

ADSPResult AudioEncSvc_RegisterWithAdsppm(AudioEncSvc_t* pMe);
ADSPResult AudioEncSvc_DeregisterWithAdsppm(AudioEncSvc_t* pMe);
ADSPResult AudioEncSvc_ProcessKppsBw(AudioEncSvc_t *pMe, bool_t is_release, bool_t force_vote);
ADSPResult AudioEncSvc_ProcessEosCmd(AudioEncSvc_t* pMe);
bool_t AudioEncSvc_IsProcessB4Eos(AudioEncSvc_t* pMe);
ADSPResult AudioEncSvc_CheckReallocateExternalBuffer(AudioEncSvc_t *pMe);

ADSPResult AudioEncSvc_HandleFwkExtnBeforeProcess(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container, uint32_t pcm_bytes_consumed_per_frame, bool_t has_bytes_consumed_changed);
ADSPResult AudioEncSvc_HandleFwkExtnAtInit(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container);
ADSPResult AudioEncSvc_HandleFwkExtnAtMediaFmt(AudioEncSvc_t *pMe, enc_CAPI_container_t *capi_container);
void AudioEncSvc_InitMediaFmt(capi_v2_standard_data_format_t *std_media_fmt);
ADSPResult AudioEncSvc_GetExtOutputBuf(AudioEncSvc_t *pMe);

/**
 * InpDataMsg messages may not always be data msg
 */
static inline ADSPResult AudioEncSvc_ReleaseInputBuf(AudioEncSvc_t *pMe, ADSPResult ret_code)
{
   ADSPResult result = ADSP_EOK;

   if (pMe->InpDataMsg.pPayload)
   {
      result = elite_msg_finish_msg(&pMe->InpDataMsg, ret_code);
      if (ADSP_SUCCEEDED(result))
      {
         pMe->InpDataMsg.pPayload = NULL;
      }
   }
   return result;
}

ADSPResult AudioEncSvc_PushInitialZeros(AudioEncSvc_t *pMe);

#endif /* _AUDIOENSVC_INC_H_ */
