
/*========================================================================

This file contains structure definitions for Audio Decoder Service.

Copyright (c) 2010-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_Structs.h#2 $

when       who     what, where, why
--------   ---     --------------------------   -----------------------------
12/16/09    SJ      Created file.

========================================================================== */
#ifndef AUDIODECSVC_STRUCT_H
#define AUDIODECSVC_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "AudioDecSvc.h"
#include "AudioStreamMgr.h"
#include "adsp_private_api_ext.h"
#include "Elite_CAPI_V2.h"
#include "AudioDecSvc_CapiV2CallbackHandler.h"
#include "Elite_CAPI_V2_private_params.h"
#include "capi_v2_adsp_error_code_converter.h"
#include "qurt_elite_adsppm_wrapper.h"
#include "Elite_fwk_extns_compr_over_pcm.h"
#include "adsp_avs_common_api.h"
#include "AudioDecSvc_EventUtils.h"
#include "audio_module_chain_wrapper.h"
#include "AudioDecSvcImc.h"


#define  AUD_DEC_CMD_SIG   (0x80000000)
#define  AUD_DEC_RESP_SIG  (0x20000000)

//maximum number of input streams (input signal mask should be changed when this is changed)
#define DEC_SVC_MAX_INPUT_STREAMS   1

//maximum number of output streams (output signal mask should be changed when this is changed)
#define DEC_SVC_MAX_OUTPUT_STREAMS  1

//Maximum buffer in circular buffer list
#if (DEC_SVC_MAX_OUTPUT_STREAMS>1)

#define DEC_SVC_MIMO_MAX_BUF_IN_CIRC_BUFFER   5

#else

#define DEC_SVC_MIMO_MAX_BUF_IN_CIRC_BUFFER   1

#endif //DEC_SVC_MAX_OUTPUT_STREAMS

#define AUD_DEC_INP_DATA_SIG_SHIFT     26 //26 for input (24,25 are left vacant now)
#define AUD_DEC_OUT_DATA_SIG_SHIFT     18 //18 for output (19,20,21,22,23 are vacant now)

#define AUD_DEC_INP_INDEX_TO_DATA_SIG(ind)    ( ((uint32_t)0x1) << (AUD_DEC_INP_DATA_SIG_SHIFT+ind))
#define AUD_DEC_OUT_INDEX_TO_DATA_SIG(ind)    ( ((uint32_t)0x1) << (AUD_DEC_OUT_DATA_SIG_SHIFT+ind))

#define AUD_DEC_INP_SIG_BIT_IND_TO_INDEX(sigBitInd) ( sigBitInd - AUD_DEC_INP_DATA_SIG_SHIFT )
#define AUD_DEC_OUT_SIG_BIT_IND_TO_INDEX(sigBitInd)  ( sigBitInd - AUD_DEC_OUT_DATA_SIG_SHIFT )

#define AUD_DEC_OUT_INDEX_TO_SIG_BIT_IND(ind)   (AUD_DEC_OUT_DATA_SIG_SHIFT+ind)

#define AUD_DEC_EARLY_EOS_TIMER_SIG         (0x00008000)
//lower prio than rest of data queue & timer
#define AUD_DEC_EARLY_EOS_CIRC_BUF_FILL_SIG  (0x00004000)

#define  PCM_FORMAT_MAX_NUM_CHANNEL  8
#define  PCM_FORMAT_MAX_SAMPLING_RATE_HZ  384000

/**
 * Currently max messages with data are:media fmt
 *
 * meta data always waits for output buffers.
 * meta data + early-eos wont work.
 */
#define AUD_DEC_SVC_MAX_MSG_WITH_DATA  1

#define SECTION(a) __attribute__((__section__(a)))

#define AUD_DEC_SVC_MAX_BUF_Q_ELEMENTS 32 //Worst case number of buffers needed

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
static const uint32_t BITS_IN_BYTE = 8;
static const uint32_t BITS_PER_SAMPLE = 16;
static const uint32_t AUD_DEC_INTERLEAVED = 1;
static const uint32_t AUD_DEC_DEINTERLEAVED = 0;
static const int64_t  SAMPLES_PER_FRAME_X_1000000 = 1000000L;
static const uint32_t NUM_US_PER_SEC = 1000000L;

/// Round a number to the nearest multiple of 4 towards the direction of infinity.
#define ROUNDUP_MULTIPLE4(x)    ( ( ( (x)+3)>>2 ) << 2 )

// maximum data messages in data queue. Must consider data buffers + EOS messages.
static const uint32_t MAX_INP_DATA_Q_ELEMENTS = 16;
// maximum number of data Q elements in real-time mode (AFE).
// it should be max amount of time in ms that dec will take to decode.
static const uint32_t MAX_INP_DATA_Q_ELEMENTS_RT = 128;

// maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

// How many buffers in output buffer queue? May need to make this configurable...
static const uint32_t MAX_OUT_BUF_Q_ELEMENTS = 8;
// number of output buffers (default).
static const uint32_t DEFAULT_NUM_OUTPUT_BUFFERS = 2;
//more buffers needed to meet real-time deadlines.
static const uint32_t DEFAULT_NUM_OUTPUT_BUFFERS_RT = 3;
// number of output buffers for PCM decoder.
static const uint32_t DEFAULT_NUM_PCM_DEC_OUTPUT_BUFFERS = 1;
// number of output buffers with metadata. this is high because metadata is transferred from dec -> PP -> matrix.
// we need 2 buffers on dec (pcm and metadata), and a buffer each on PP and matrix.
static const uint32_t NUM_ADDITIONAL_OUTPUT_BUFFERS_WITH_METADATA = 2;
// queue must be sized to be larger than max # of msgs, and a power of 2.
static const uint32_t BUF_Q_ELEMENTS_SIZE = 8;

static const uint32_t BYTES_PER_SAMPLE_TWO = 2;
static const uint32_t BYTES_PER_SAMPLE_FOUR = 4;

// number of Response queue elements.
static const uint32_t MAX_RESP_Q_ELEMENTS = 2;

// buffer size. Assumed that service knows what size buffer it needs to deliver.
// Based on the format the buffer size will be allocated using below constants
static const uint32_t AUD_DEC_DEFAULT_OUT_BUF_SIZE = 1024;

static const uint32_t AUD_DEC_PCM_OUT_BUF_SIZE = 840*4*2; // 840 * 4
// 840 is the least common multiple of 1,2,3,4,5,6,7,8.
// The factor 2 is chose to make sure that a reasonable amount
// of samples are delivered. A multiple of # of bytes * max number of
// channels
// now with run-time buffer creation  this allocation is freed as soon as media-fmt comes.
static const uint32_t MASK_32BYTE_ALIGNMENT = 0x1F;

static const uint32_t AUD_DEC_SVC_LAST_FRAME_DELAY_BUF_SIZE = 4;     //must be power of 2
static const uint32_t AUD_DEC_SVC_FRAME_DELAY_RESET_VALUE = 0xFFFFFFFF;

#define PCM_DEC_KPPS_FACTOR    6 /* Number of transactions of pcm data(in->scratch, scratch->out)+2 */
#define PCM_DEC_MIN_KPPS    4000 /* Minimum KPPS for PCM decoder */

#define GENERIC_DEC_KPPS    13000

//No. of possible speaker positions
#define MAX_SPKR_POS             (16)

/** buffer size in AFE-frames for regular latency
 * higher size will increase latency & memory.
 */
#define BUF_SIZE_REG_LATENCY   (5)

#define BUF_SIZE_LOW_LATENCY (1)

//max number of concurrent CAPIs
#define DEC_SVC_MAX_CAPI   3

/**
 * bandwidth required for single stream decode
 * values are so due to legacy implementation.
 *
 * assuming 48kHz sampling, 2 channels, 2 bytes per channel.
 * in bytes per second.
 */
#define SINGLE_STREAM_DECODED_BW  (48000 * 2 * 2)

#define PCM_STACK_SIZE 2048
#define MIN_AUDEC_THREAD_STACK_SIZE 5084 // minimum audio decoder thread stack size excluding decoder stack
#define STACK_SIZE_DEC_PROCESS 512   //decoder stack size excluding core process function stack

/** Event mask is useful in 2  cases
 * 1. when event is not per port
 * 2. when event is per port, but the event handling is done for all ports at once.
 *
 * It's not useful when event is not per port and event cannot be handled at once, because
 * it's not known when to clear the mask*/
#define AUD_DEC_SVC_EVENT__KPPS_MASK   (0x80000000)
#define AUD_DEC_SVC_EVENT__KPPS_SHIFT  (31)
#define AUD_DEC_SVC_EVENT__BW_MASK     (0x40000000)
#define AUD_DEC_SVC_EVENT__BW_SHIFT    (30)
#define AUD_DEC_SVC_EVENT__METADATA_AVAILABLE_MASK   (0x20000000)
#define AUD_DEC_SVC_EVENT__METADATA_AVAILABLE_SHIFT  (29)
#define AUD_DEC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK   (0x10000000)
#define AUD_DEC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_SHIFT  (28)


//#define EARLY_EOS_DEBUG

/* -----------------------------------------------------------------------
 ** Global definitions/forward declarations
 ** ----------------------------------------------------------------------- */
typedef enum Dec_CAPI_Type
{
   DEC_CAPI_TYPE_AUTO,
   DEC_CAPI_TYPE_DEPACKETIZER,
   DEC_CAPI_TYPE_CONVERTER,
   DEC_CAPI_TYPE_PASS_THROUGH,
   DEC_CAPI_TYPE_PACKETIZER,
   DEC_CAPI_TYPE_COP_PACKETIZER, //COP
   DEC_CAPI_TYPE_COP_DEPACKETIZER
} Dec_CAPI_Type;

typedef enum Dec_Endianness_Type
{
   DEC_LITTLE_ENDIAN,
   DEC_BIG_ENDIAN
} Dec_Endianness_Type;

typedef struct AudioDecSvc_t AudioDecSvc_t;
typedef struct AudioDecSvc_InpStream_t AudioDecSvc_InpStream_t;

struct DecTimeStampState_t
{
   uint64_t        ullInbufTS;         /**< Time stamp of the input buffer that we
                                            are currently holding on to */
   bool_t          bInbufTSValid;      /**< Time stamp valid flag of the input buffer
                                            that we are currently holding on to */
   uint64_t        ullNextOutBufTS;    /**< Time stamp of the next output buffer
                                            to be sent out to PP */
   uint64_t        ullNextOutBufSampleCount;
   /**< Sample count corresponding to ullNextOutBufTS
    *   this value resets to zero every time
    *   AudioDecSvc_SyncToInputTS is called.*/
   uint64_t        ullLastSyncedInBufTS;
   /**< TS of the input buffer that was last sync'ed in
    * AudioDecSvc_SyncToInputTS*/
   bool_t          bNextOutbufTSValid; /**< Time stamp valid flag of the next
                                            output buffer to be sent out to PP */
   bool_t          bNextOutbufTsOld;   /**< This flag indicates that the TS used for
                                            next output buffer is based on the last
                                            input buffer, not the current one */
   bool_t          bIsTSResync;        /**< This flag indicates that the TS needs to
                                            resync to the next input buffer */
} ;

struct silence_removal_t
{
   uint32_t        initial_samples_to_remove;      /**< Number of silence samples to remove
                                                         at the beginning of file*/
   uint32_t        trailing_samples_to_remove;     /**< Number of silence samples to remove
                                                         at the end of file*/
   bool_t          last_buffer;                    /**< Flag indicating if the current client buffer is the last buffer*/

} ;

struct DecChannelMap_t
{
   uint32_t  nChannels;                 /** Number of channels mapped */
   uint8_t   nChannelMap[CAPI_V2_MAX_CHANNELS];          /** channel positions mapped */
};

struct DecPcmFmt_t
{
   uint32_t                             ulPCMSampleRate;
   uint16_t                             usBitsPerSample;
   uint16_t                             usIsInterleaved;
   uint32_t                             usBytesPerSample;
   DecChannelMap_t                      chan_map;
   Dec_Endianness_Type                  endianness;
};

/** Describes the input channels that get routed to a given
 *  output channel */
struct dec_in_chan_map_t
{
   //# of input channels that route to this output channel
   uint32_t                             num_in_chan;
   //list of input channel positions that route to this output channel
   uint8_t                              in_chan[PCM_FORMAT_MAX_NUM_CHANNEL];
} ;

struct dec_out_chan_map_t
{
   //client given output channel mapping
   DecChannelMap_t                      max_map;
   //# of output channels actually going to downstream svc
   uint32_t                             num_out_chan;
   //list of output channel positions
   uint8_t                              out_chan[PCM_FORMAT_MAX_NUM_CHANNEL];
   //details of input channels routed to each of the output channels above
   dec_in_chan_map_t                    in_chan_map[PCM_FORMAT_MAX_NUM_CHANNEL];
   //flag to indicate whether downmixing of input is required
   bool_t                               downmix_required;
   //flag to indicate whether output chan map command was received.
   bool_t                               is_out_chan_map_received;
} ;

/** metadata transfer related */
struct dec_metadata_xfer_t
{
   uint8_t                              is_waiting_to_push_metadata; /**< after waking up from wait, whether decoder is waiting to push meta-data down.
                                                                           true after an attempt to send meta-data but out buf was not available. */
   uint32_t                             curr_wait_mask_backup;       /**< back up copy of the mask to be restored after sending meta-data */
};
/*decoder error event related*/
struct dec_err_event_t
{
   uint64_t                             errorTime;
   //flag to enable/disable  DEC_ERROR notification event
   bool_t                               bEnableDecErrEvent;
   //Number of consecutive errors from decoder process.
   uint16_t                             nConsecutiveDecErr;
   //Maximum number of consecutive errors before raising event, if enabled
   uint16_t                             nThresholdDecError;
   //Maximum miliseconds of timeout
   uint16_t                             msThresholdTimeout;
};

typedef ADSPResult (*dec_capi_destroy_fn)(capi_v2_t **capi_ptr_ptr);

/** parameters applicable to the whole decoder svc. */
typedef struct dec_init_time_get_params_t
{
   uint32_t max_stack_size;
} dec_init_time_get_params_t;

typedef struct dec_capi_port_index_t
{
   uint16_t index;
   bool_t   valid;

} dec_capi_port_index_t;
/** parameters applicable to the each CAPI. */
typedef struct dec_CAPI_init_time_get_params_t
{
   // ------ input
   dec_capi_port_index_t input_port_index;
   dec_capi_port_index_t output_port_index;

   // ------ output
   capi_v2_port_data_threshold_t in_port_thresh;
   capi_v2_port_data_threshold_t out_port_thresh;
   capi_v2_stack_size_t stack_size;
   capi_v2_max_metadata_size_t metadata_size;
   capi_v2_output_media_format_size_t out_med_fmt_size;

} dec_CAPI_init_time_get_params_t;

typedef enum dec_CAPI_V2_presence
{
   DEC_CAPI_V2_PRESENCE_UNKNOWN=0,
   DEC_CAPI_V2_PRESENCE_NOT_PRESENT,
   DEC_CAPI_V2_PRESENCE_PRESENT
}dec_CAPI_V1_presence;

typedef enum dec_AMDB_presence
{
   DEC_AMDB_PRESENCE_NOT_PRESENT = 0,
   DEC_AMDB_PRESENCE_PRESENT_AS_CAPI_V1,
   DEC_AMDB_PRESENCE_PRESENT_AS_CAPI_V2,
   DEC_AMDB_PRESENCE_PRESENT_AS_STUB,

}dec_AMDB_presence;

typedef struct dec_CAPI_init_params_t
{
   // ------ input
   uint32_t uMediaFmt;
   uint32_t id2; //valid for converter case only. zero for other cases.
   capi_v2_event_callback_info_t cb_info;
   dec_capi_port_index_t input_port_index;
   dec_capi_port_index_t output_port_index;
   void *amdb_handle;

   // ------ output
   dec_capi_destroy_fn  dec_destroy_fn;
   dec_CAPI_V2_presence  capi_v2_presence;

} dec_CAPI_init_params_t;

typedef struct dec_pull_mode_t dec_pull_mode_t;

static const capi_v2_framework_extension_id_t dec_svc_supported_fmwk_extns[] = { {FWK_EXTN_COMPR_OVER_PCM} };

#define DEC_SVC_MAX_SUPPORTED_FMWK_EXTNS (sizeof(dec_svc_supported_fmwk_extns)/sizeof(dec_svc_supported_fmwk_extns[0]))

struct dec_CAPI_container_t
{
   capi_v2_t                           *capi_ptr;
   Dec_CAPI_Type                       capi_type;   //type of the CAPI. helpful in adjusting time stamp by the service.
   capi_v2_buf_t                       in_buf[DEC_SVC_MAX_INPUT_STREAMS];
   capi_v2_buf_t                       out_buf[DEC_SVC_MAX_OUTPUT_STREAMS];

   capi_v2_stream_data_t               inputs[DEC_SVC_MAX_INPUT_STREAMS];
   capi_v2_stream_data_t               outputs[DEC_SVC_MAX_OUTPUT_STREAMS];

   capi_v2_stream_data_t               *inputs_ptr[DEC_SVC_MAX_INPUT_STREAMS];
   capi_v2_stream_data_t               *outputs_ptr[DEC_SVC_MAX_OUTPUT_STREAMS];

   int32_t                             bytes_logged;        //num of bytes from current CAPI buffer that are already logged.
   uint32_t                            dec_fmt_id;          //format id:
   void                                *scratch_out_buf;    //scratch output buffer: is allocated as one buf for all CAPI outputs & is of max out size.
   uint32_t                            scratch_buf_size; //could be diff from maxExtBufSize as this is max of out sizes
   elite_multi_channel_pcm_fmt_blk_t   PrevFmtBlk[DEC_SVC_MAX_OUTPUT_STREAMS];       //store the previous format blk for each output stream
   data_format_t                       out_data_fmt_type[DEC_SVC_MAX_OUTPUT_STREAMS];      //data type of the output
   dec_CAPI_callback_obj_t             capi_callback_obj;
   dec_capi_destroy_fn                 dec_destroy_fn;
   void*                               amdb_handle; //store AMDB handle for querying dlinfo
   uint32_t                            kpps; //total kpps of this capi for all its ports. needed to check for change.
   uint32_t                            code_bw;
   uint32_t                            data_bw;

   uint32_t                            in_port_event_new_size[DEC_SVC_MAX_INPUT_STREAMS];
   uint32_t                            out_port_event_new_size[DEC_SVC_MAX_OUTPUT_STREAMS];
   bool_t                              metadata_available_event[DEC_SVC_MAX_OUTPUT_STREAMS];
   bool_t                              media_fmt_event[DEC_SVC_MAX_OUTPUT_STREAMS];

   capi_v2_num_needed_framework_extensions_t num_fmwk_extns;
   capi_v2_framework_extension_id_t    fmwk_extns[DEC_SVC_MAX_SUPPORTED_FMWK_EXTNS];

};

struct AudioDecSvc_CommonStream_t
{
   elite_svc_handle_t                     serviceHandle;
   uint16_t                               stream_type;  //primary/secondary etc
   uint8_t                                stream_id; //0 is invalid
   AudioDecSvcIOStreamType                io_type;

   AudioStreamMgr_CallBackHandleType      CallbackHandle;

   volatile uint32_t                    *delay_ptr;         //delay ptr shared by ASM/AFE. for input streams delay will be zero. microseconds.
   uint32_t                             ulDataLogId;       // data logging id: <12 bit int. apr port id ><12 bit seq num><8 bit for flush etc>
   uint16_t                             perf_mode;        /**< performance mode in which the decoder is opened
                                                             Supported values are ASM_LEGACY_STREAM_SESSION &
                                                             ASM_LOW_LATENCY_STREAM_SESSION. This mode is
                                                             currently only applicable to PCM playback */
   qurt_elite_queue_t                   *pEventQ;   // Messages will be queued here to raise asynchronous events with the static service.
   uint32_t                             eventToken;       // This is the token which will be used to identify this instance in any asynchronous event raised.

   uint64_t                             total_bytes; //amount of data received (input strm) / transmitted (output strm) in bytes. Applicable only for PCM/compressed packetized.
};

/**
 * One item in circ buf contains one data buffer and
 * a few other elite messages that have to be sent before the data (in order)
 */
struct AudioDecSvc_CircBufItem_t
{
   elite_msg_data_buffer_t *data;
   elite_msg_any_t          msg[AUD_DEC_SVC_MAX_MSG_WITH_DATA];
};

/* Rules
 * if tail is equals to head ; there is no valid buffer in list = empty
 * if tail is not equals to head ; buffer presents at tail will swapped with decoder output buffer and tail will be circularly incremented
 * if process is to be called and no decoder output buffer is available then use buffer present at head in list and circularly increment the head
 * after incrementing the head if head is equals to tail then increment the tail (dropping the oldest buffer)
 */
struct AudioDecSvc_Circular_Buffer_List_t
{
   AudioDecSvc_CircBufItem_t   *buflist;
   uint16_t                    numBufs;
   uint16_t                    tail; //points to oldest filled data, 0 based index.
   uint16_t                    head; //points to empty buffer, 0 based index.
   //filled buffers are from tail to head-1. and empty from head to tail-1.
};

enum AudioDecSvc_EosType_t
{
   AUD_DEC_EOS_NONE=0,
   AUD_DEC_EOS_V1,
   AUD_DEC_EOS_V2,
};

struct AudioDecSvc_InpStream_t
{
   AudioDecSvc_CommonStream_t           common;              //keep this on top so that handle will be the first element.

   elite_msg_any_t                      inpDataQMsg;

   elite_mem_shared_memory_map_t        inp_buf_params;
   elite_mem_shared_memory_map_t        inp_buf_params_backup;  /* Backup inp_buf_params for cache invalidation in FreeInputDataCmd */

   dec_err_event_t                      dec_err_event;
   AudioDecSvc_EosType_t                endOfStream;
   elite_msg_data_eos_info_t            eosInfo;
   bool_t                               bEndOfFrame;
   bool_t                               bWaitingForEoSRespFromAfe;
   //flag to enable/disable  SR/CM change notification event
   bool_t                               bEnableSrCmEvent;
   //info about decoder error event
   bool_t                               is_frame_rate_set;
   bool_t                               WasPrevDecResNeedMore;
   bool_t                               no_apr_dec;    /** indicates that this decoder doesn't have an APR client but an internal client to provide input data */

   DecPcmFmt_t                          pcmFmt;    //PCM CAPI does not take #channel or sampling rate as inputs

   // saving this information here in service
   DecTimeStampState_t                  TsState; /**< time stamp state structure */
   silence_removal_t                    silence_removal; /**< silence removal state structure*/

   uint32_t                             buf_recv_cnt;    // write buffer recvd count
   uint32_t                             buf_done_cnt;    // write buffer done count
   elite_msg_data_eos_info_t            DtmfEndToneInfo;   //store the client and src address to send the tone ended event
   dec_pull_mode_t                      *pull_mode_ptr; //pull_mode related parameters
   eDecoderIOFormatConvType             io_fmt_conv;
};

struct AudioDecSvc_EarlyEos_t
{
   uint32_t                             early_eos_delay_ms;
   qurt_elite_signal_t                  fill_circ_buf_signal;
   qurt_elite_signal_t                  early_eos_timer_signal;
   qurt_elite_timer_t                   early_eos_timer;

   bool_t                               early_eos_scheduled;
   bool_t                               reset_needed; //after EoS if timer fires

   bool_t                               is_init_samples_removed; //optimization

   /**
    * Allows for conservative estimate last few frame delays: if frame size drops from large to small, then we will not be able to meet early-eos delay
    * choosing small increasing chance of ensuring we will meet early-eos delay.
    *
    * Also helps in reducing circ buf recreate in cases like WMA where frame size can change 30-15-15-30-15-15 ms etc.
    *
    * is_init_samples_removed -> at this time delay is not stored.
    */
   uint32_t                             last_frame_delay_us[AUD_DEC_SVC_LAST_FRAME_DELAY_BUF_SIZE];
   uint8_t                              last_frame_delay_buf_index;

};

struct AudioDecSvc_OutStream_t
{
   AudioDecSvc_CommonStream_t           common;

   qurt_elite_queue_t                   *pOutBufQ;
   AudioDecSvc_Circular_Buffer_List_t   CircBufList;
   int                                  nBufsAllocated;         /**< Keep track of number of output bufs in circulation */
   uint32_t                             maxExtBufSize;          /**< ext buf or out buf. (could be diff from scratch_buf_size) */
   elite_svc_handle_t                   *pDownStreamSvc;        /**< down stream peer service handle */

   qurt_elite_bufmgr_node_t             outDataBufferNode;

   uint32_t                             decoderConverterMode;   /**< decoder converter mode */
   dec_out_chan_map_t                   out_chan_map;
   uint32_t                             out_buf_cnt;            /**< output buffer generated count */

   uint16_t                             output_bits_per_sample; /**< bits per sample as received from ASM Read/Write open CMDs */
   uint32_t                             output_bytes_per_sample;

   dec_metadata_xfer_t                  metadata_xfr;           /**< helper struct for metadata transfer */

   AudioDecSvc_EarlyEos_t               *early_eos_ptr;

   volatile uint32_t*                   downstream_buf_delay_ptr; //pointer to downstream buffer delay, us

};

//TODO: error check to make sure when multi-capi is used, only one input stream possible currently.
//TODO: whenever MIMO dec is opened, check whether the fmt is compatible with existing fmt_ids
//TODO: there can be only one MIMO dec & it has to be the last CAPI: error check
//TODO: if capi chaining with MIMO dec, then it can support only one input stream.

struct AudioDecSvc_t
{
   AudioDecSvc_InpStream_t              *in_streams_ptr[DEC_SVC_MAX_INPUT_STREAMS];   //even in default mode (SISO), 2 streams are assumed, however, both have same stream_id.
   AudioDecSvc_OutStream_t              *out_streams_ptr[DEC_SVC_MAX_OUTPUT_STREAMS]; //and in such cases more than 2 streams are assumed to be not created.

   qurt_elite_thread_t                  thread_id;
   qurt_elite_queue_t                   *pCmdQ;
   qurt_elite_queue_t                   *pRespQ;        //Response queue

   dec_CAPI_container_t                 *capiContainer[DEC_SVC_MAX_CAPI];
   dec_CAPI_container_t                 *mainCapiContainer;  //TODO: Currently set-param doesn't have a way of knowing which of the multiple CAPIs the it's supposed to be set-to,
   //in future, we need a way to identify the decoder on which it has to be set.
   //Until then set-enc-dec will be on main CAPI container.
   //similarly, other features like metadata transfer,
   qurt_elite_channel_t                 channel;             //hold MQ's owned by this instance
   uint32_t                             unCurrentBitfield;
   elite_msg_any_t                      cmdMsg;
   uint32_t                             unPreviousBitfield; //useful to handle connect->pause->suspend->flush->run & its combinations
   bool_t                               isPrevBitfieldValid;

   bool_t                               need_to_send_eos;   //need to send eos to all connected down-stream services

   uint32_t                             prev_kpps_vote;     //total KPPS of the decoder.
   uint32_t                             svc_bw;             //BW from decoder svc.
   uint32_t                             prev_bw_vote;       //previous bw vote inclu capi vote

   eDecoderMode                         ulMode;
   uint32_t                             event_mask;   //event mask indicating which event was raised from CAPI (see description above AUD_DEC_SVC_EVENT__KPPS_MASK)

   uint32_t                             ulAdsppmClientId;
   qurt_elite_adsppm_client_t           *adsppmClientPtr;

   bool_t                               is_rt_dec; //identifies real-time path.

   aud_dec_svc_event_node_t             *event_list;

   AudioModuleCapiWrapper_t             *pUll_modules;

   imc_src_dst_info_t                   *imc_info_ptr; //pointer to source imc struct
};

static inline void AudioDecSvc_SetPrevBitField(AudioDecSvc_t *pMe)
{
   if (!pMe->isPrevBitfieldValid)
   {
      pMe->isPrevBitfieldValid = true;
      pMe->unPreviousBitfield = pMe->unCurrentBitfield;
   }
}

static inline uint32_t AudioDecSvc_GetAllInputStreamMask(AudioDecSvc_t *pMe)
{
   uint32_t mask=0;
   for (uint16_t i=0; i < DEC_SVC_MAX_INPUT_STREAMS; i++)
   {
      if (pMe->in_streams_ptr[i]) mask |= AUD_DEC_INP_INDEX_TO_DATA_SIG(i);
   }
   return mask;
}

static inline uint32_t AudioDecSvc_GetAllOutputStreamMask(AudioDecSvc_t *pMe)
{
   uint32_t mask=0;
   for (uint16_t i=0; i < DEC_SVC_MAX_OUTPUT_STREAMS; i++)
   {
      if (pMe->out_streams_ptr[i] &&
    		  pMe->out_streams_ptr[i]->pDownStreamSvc) mask |= AUD_DEC_OUT_INDEX_TO_DATA_SIG(i);
   }
   return mask;
}

static inline uint32_t  AudioDecSvc_GetInputStreamMask(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   for (uint16_t i=0; i < DEC_SVC_MAX_INPUT_STREAMS; i++)
   {
      if (pMe->in_streams_ptr[i] == pInpStrm) return AUD_DEC_INP_INDEX_TO_DATA_SIG(i);
   }

   return 0;
}

static inline uint32_t  AudioDecSvc_GetOutputStreamMask(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   for (uint16_t i=0; i < DEC_SVC_MAX_OUTPUT_STREAMS; i++)
   {
      if (pMe->out_streams_ptr[i] == pOutStrm) return AUD_DEC_OUT_INDEX_TO_DATA_SIG(i);
   }

   return 0;
}

static inline void AudioDecSvc_StopListenToMask(AudioDecSvc_t *pMe, uint32_t mask)
{
	pMe->unCurrentBitfield &= ~mask;
}

static inline void AudioDecSvc_StartListenToMask(AudioDecSvc_t *pMe, uint32_t mask)
{
	pMe->unCurrentBitfield |= mask;
}

static inline void AudioDecSvc_ListenToInput(AudioDecSvc_t *pMe)
{
   uint32_t omask = AudioDecSvc_GetAllOutputStreamMask(pMe);
   uint32_t imask = AudioDecSvc_GetAllInputStreamMask(pMe);
   AudioDecSvc_StopListenToMask(pMe, omask); //stop listening to output
   AudioDecSvc_StartListenToMask(pMe, imask); //start listening to input
}

static inline void AudioDecSvc_ListenToOutput(AudioDecSvc_t *pMe)
{
   uint32_t imask = AudioDecSvc_GetAllInputStreamMask(pMe);
   uint32_t omask = AudioDecSvc_GetAllOutputStreamMask(pMe);
   AudioDecSvc_StopListenToMask(pMe, imask); //stop listening to input
   AudioDecSvc_StartListenToMask(pMe, omask); //start listening to output
}

static inline void AudioDecSvc_ListenToControl(AudioDecSvc_t *pMe)
{
   pMe->unCurrentBitfield = AUD_DEC_CMD_SIG | (pMe->unCurrentBitfield & (AUD_DEC_EARLY_EOS_TIMER_SIG|AUD_DEC_RESP_SIG));
   //continue to listen to early-eos timer or resp queue if listening now.
}

static inline bool_t AudioDecSvc_IsListenToControlOnly(AudioDecSvc_t *pMe)
{
   uint32_t control_mask = AUD_DEC_CMD_SIG | AUD_DEC_RESP_SIG | AUD_DEC_EARLY_EOS_TIMER_SIG;

   if (pMe->unCurrentBitfield & control_mask) return true;

   return false;
}

static inline bool_t AudioDecSvc_IsPcmFmt(uint32_t fmt_id)
{
   return ( (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 == fmt_id) ||
           (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 == fmt_id) ||
         (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 == fmt_id) );
}

ADSPResult AudioDecSvc_ProcessPCM(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInStrm, AudioDecSvc_OutStream_t *pOutStrm,
      uint8_t **ppucIn, uint32_t *pRemBytesInSrcBuf,
      uint32_t *numBytesProcessed, bool_t isPackedChSpacing, uint32_t usableOutBufSize,bool_t isInvalidateCache);
ADSPResult AudioDecSvc_FinishSuspend(AudioDecSvc_t* pMe);

//Returns primary or primary_secondary stream in mimo case otherwise first input stream.
static inline AudioDecSvc_InpStream_t *AudioDecSvc_GetDefaultInputStream(AudioDecSvc_t *pMe)
{
   for(uint8_t ii =0 ; ii < DEC_SVC_MAX_INPUT_STREAMS ; ii++)
   {
      if(NULL != pMe->in_streams_ptr[ii])
      {
         if(ASM_STREAM_TYPE_SECONDARY != pMe->in_streams_ptr[ii]->common.stream_type) return pMe->in_streams_ptr[ii];
      }
   }
   return pMe->in_streams_ptr[0];
}

static inline AudioDecSvc_OutStream_t *AudioDecSvc_GetDefaultOutputStream(AudioDecSvc_t *pMe)
{
   return pMe->out_streams_ptr[0];
}

static inline uint8_t AudioDecSvc_GetInputStreamIndex(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   for(uint8_t ii =0 ; ii < DEC_SVC_MAX_INPUT_STREAMS ; ii++)
   {
      if(pInpStrm == pMe->in_streams_ptr[ii])
      {
         return ii;
      }
   }
   return 0;
}

static inline uint8_t AudioDecSvc_GetOutputStreamIndex(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   for(uint8_t ii =0 ; ii < DEC_SVC_MAX_OUTPUT_STREAMS ; ii++)
   {
      if(pOutStrm == pMe->out_streams_ptr[ii])
      {
         return ii;
      }
   }
   return 0;
}

static inline uint32_t AudioDecSvc_AddBufDelayAndGetClientToken(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm, uint32_t buf_size,
      elite_multi_channel_pcm_fmt_blk_t* pOutputFmt)
{
   //for AFE dec (real-time), delay is only due to initial zero buffers. run-time updates (as done in FTRT path), not required.
   if (pMe->is_rt_dec)
   {
      return 0;
   }

   uint32_t sample_rate = pOutputFmt->sample_rate;
   uint32_t num_channels = pOutputFmt->num_channels;
   uint32_t bytes_per_sample = (pOutputFmt->bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   uint32_t bytes_p_sec = (sample_rate * num_channels * bytes_per_sample);

   if (bytes_p_sec == 0)
   {
      return 0;
   }

   uint32_t delay = (((uint64_t)buf_size*1000000L) + bytes_p_sec - 1) / bytes_p_sec;

   *pOutStrm->common.delay_ptr += delay; //add delay when the buf goes down & also store as client token.

   uint32_t client_token = delay;

   return client_token;

}

static inline void AudioDecSvc_SubtractBufDelayUsingClientToken(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm, uint32_t *client_token_ptr)
{
   if (pMe->is_rt_dec)
   {
      return ;
   }

   uint32_t delay = *client_token_ptr;

   *pOutStrm->common.delay_ptr = (uint32_t) ((int64_t)(*pOutStrm->common.delay_ptr) - (int64_t)delay);

   *client_token_ptr = 0;
}

int AudioDecSvc_ConnectToService(AudioDecSvc_t* pMe);
int AudioDecSvc_DisconnectFromService(AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_ReturnUnsupported (AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_Flush(AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_Run(AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_Pause(AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_StartSuspend(AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_SetParam(AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_SetPpParam(AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_GetPpParam(AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_DtmfHandler(AudioDecSvc_t *pMe);
void AudioDecSvc_SendDtmfToneEndedEvent(AudioDecSvc_t *pMe);
ADSPResult AudioDecSvc_RegisterAprEvents (AudioDecSvc_t* pMe);

ADSPResult AudioDecSvc_ProcessInputDataQ(AudioDecSvc_t* pMe, uint32_t channelBitIndex);
ADSPResult AudioDecSvc_ProcessOutputDataQ(AudioDecSvc_t* pMe, uint32_t channelBitIndex);
ADSPResult AudioDecSvc_ProcessCircBufFill(AudioDecSvc_t *pMe, uint32_t channelBitIndex);

static inline uint32_t AudioDecSvc_GetPcmOutBufSize(uint16_t perf_mode, uint32_t sample_rate, uint16_t bytes_per_sample, uint16_t num_ch)
{
   //Calculate size based on the current Format Sample Rate

   uint32_t unAfeFrameSizeInSamples;
   elite_svc_get_frame_size(sample_rate,&unAfeFrameSizeInSamples);
   uint32_t unDecOutBufSizeBytes = bytes_per_sample * num_ch * (unAfeFrameSizeInSamples);
   unDecOutBufSizeBytes = (ASM_LEGACY_STREAM_SESSION == perf_mode)? (BUF_SIZE_REG_LATENCY*unDecOutBufSizeBytes) : (BUF_SIZE_LOW_LATENCY*unDecOutBufSizeBytes); //5 AFE-frames for regular latency
   return unDecOutBufSizeBytes;
}

static inline bool_t AudioDecSvc_IsBufFromOutBufQ(AudioDecSvc_OutStream_t *pOutStrm)
{
   return (pOutStrm->outDataBufferNode.pReturnQ != NULL); // a buffer from outout buf Q will have retrurn Q set to bufQ.
   //if not from output bufQ, then it's from circ buf.
}




#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDCMNUTIL_H
