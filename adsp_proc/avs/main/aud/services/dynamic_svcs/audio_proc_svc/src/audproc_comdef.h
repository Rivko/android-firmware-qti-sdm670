/*========================================================================
ELite

This file declares AudDynaPPSvc. This is a service for Postprocessing demo.
It receives and processes both control and data command. Part of the interface
might be just temporary solution before additional finalized interface change
available.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
1/3/2011   PC      Removing TSM and making POPP appi compliant
11/17/10   ss      Introducing APPI Rev B
07/06/10   SS      Adding EANS for Audio rec path
05/15/10   JG      Created file.

========================================================================== */

#ifndef ELITE_AUDYNAPP_CMNDEF_H
#define ELITE_AUDYNAPP_CMNDEF_H


/*----------------------------------------------------------------------------
 * Include files
 * -------------------------------------------------------------------------*/
#include "AudDynaPPSvc.h"
#include "audproc_event_api_utils.h"
#include "Elite.h"
#include "adsp_asm_api.h"
#include "adsp_adm_api.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_PrivateDefs.h"
#include "AudDevMgr_AprIf.h"
#include "adsp_media_fmt.h"
#include "EliteTopology_db_if.h"
#include "EliteTopology_db.h"
#include "audproc_topohandler.h"
#include "audpp_common.h"
#include "audproc_topo_bg_thread.h"

#include "EliteLoggingUtils_i.h"
#include "qurt_elite_adsppm_wrapper.h"

// #define AUDPROC_SIM
#ifdef AUDPROC_SIM
#include "stdio.h"
#include "stdlib.h"
#endif // AUDPROC_SIM


#define MAX_GAIN_MB 1200
#define MIN_GAIN_MB -8500
#define PAUSE_MIN_PERIOD 20   //20 milliseonds
#define PAUSE_MAX_PERIOD 50   //50 milliseconds
#define PAUSE_MIN_STEP 0
#define PAUSE_MAX_STEP 50000  //50 milliseconds
#define PAUSE_TIMEOUT_PERIOD 60000 //60 milliseconds
#define BUF_NUM_AFE_FRAME_LOW_LATENCY   1

static const uint16_t KEEP_ORIGINAL_SAMPING_RATE = 0;

// BITMASK for getting the status of buffer.

static const int32_t ncTsmBufferSize=2048*2;
// In bytes, sufficient for 10ms content of 2 channel, 48KHz

static const uint32_t NUM_CHANNELS_INVALID = 0;
static const uint32_t NUM_CHANNELS_MONO = 1;
static const uint32_t NUM_CHANNELS_STEREO = 2;
static const uint32_t NUM_CHANNELS_5_1 = 6;
static const uint32_t NUM_CHANNELS_7_1 = 8;
static const uint32_t MAX_INPUT_CHANNELS = NUM_CHANNELS_7_1;
static const uint32_t MAX_OUTPUT_CHANNELS = NUM_CHANNELS_7_1;
static const uint8_t  mono_channel_mapping[8] = { PCM_CHANNEL_C , 0, 0, 0, 0, 0, 0, 0};
static const uint8_t  non_mono_channel_mapping[8] = { PCM_CHANNEL_L, PCM_CHANNEL_R, PCM_CHANNEL_C, PCM_CHANNEL_LS, PCM_CHANNEL_RS, PCM_CHANNEL_LFE, PCM_CHANNEL_CS, PCM_CHANNEL_LB };

static const uint32_t BITS_PER_SAMPLE_INVALID = 0;
static const uint32_t BYTES_PER_SAMPLE_TWO = 2;
static const uint32_t BYTES_PER_SAMPLE_FOUR = 4;

#define SHIFT_AND_OR(x,y) ((x) | ((x) >> (y)))
#define ROUND_TO_NEXT_POWER_OF_2(x) (SHIFT_AND_OR(SHIFT_AND_OR(SHIFT_AND_OR(SHIFT_AND_OR(SHIFT_AND_OR((x)-1,1),2),4),8),16) + 1)

// maximum data messages in data queue.
static const uint32_t MAX_EOS_MESSAGES = ASM_MAX_NUM_SUPPORTED_SESSIONS; // Assuming only one EOS per session.
static const uint32_t MAX_MEDIA_TYPE_MESSAGES = 1; // Reasoning: Multiple media type messages may be sent only on the Tx path, and it is unlikely to get more than one along with maximum number of EOS's
static const uint32_t MAX_DATA_BUF_MESSAGES = 30; // Maximum Output number of buffers of the matrix. This value happens when the max delay is used in the loopback mode.
static const uint32_t MAX_DATA_MESSAGES = MAX_EOS_MESSAGES + MAX_MEDIA_TYPE_MESSAGES + MAX_DATA_BUF_MESSAGES;

static const uint32_t MAX_DATA_Q_ELEMENTS = ROUND_TO_NEXT_POWER_OF_2(MAX_DATA_MESSAGES);

// maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 64;

//msg queue has to be bigger than number of elements, and a power of 2.
static const uint32_t BUF_Q_ELEMENTS_SIZE  = 8 ;

//number of audpp input ports as of now
static const uint32_t AUDPP_MAX_INPUT_PORTS = 2;
static const uint32_t AUDPP_PRIMARY_IN_PORT = 0;
static const uint32_t AUDPP_PRIMARY_OUT_PORT = 0;

static const uint32_t AUDPP_MAX_PENDING_DATA_MSGS = 10;

/// SET bit indicating input buffer is consumed
#define SET_INPUT_BUF_BIT(x)  ( (x) |= (BitMaskInBufferStatus) )
// clear bit to indicate input buffer is not consumed
#define CLR_INPUT_BUF_BIT(x)  ( (x) &= (~BitMaskInBufferStatus) )

/// SET bit indicating output buffer is consumed
#define SET_OUTPUT_BUF_BIT(x)  ( (x) |= BitMaskOutBufferStatus )
// clear bit to indicate output buffer is not consumed
#define CLR_OUTPUT_BUF_BIT(x)  ( (x) &= (~BitMaskOutBufferStatus))

/// Round a number to the nearest multiple of 4 towards the direction of infinity.
#define ROUNDUP_MULTIPLE4(x)    ( ( ( (x)+3)>>2 ) << 2 )

#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

//service needs MPPS of 5
#define AUDPP_SVC_KPPS 5000

static const uint32_t CACHE_LINE_SIZE = 32; // Must be a power of 2
static const uint32_t CACHE_ALIGNMENT_MASK = (CACHE_LINE_SIZE - 1);

static inline bool_t isAlignedToCacheLine(uint32_t addr)
{
   return ((addr & CACHE_ALIGNMENT_MASK) == 0);
}

enum AudPP_BufferSourceType
{
   SRC_INVALID = -1,
   SRC_UPSTREAM = 0,
   SRC_SC = 1,
};

/// Following are possible service states for POSTPROC service

enum AudPP_StateType
{
   DYNA_SVC_PP_STATE_PAUSE,
   DYNA_SVC_PP_STATE_RUN,
   DYNA_SVC_PP_STATE_FATAL_ERROR,
   DYNA_SVC_PP_NUM_STATES         // a count of the supported states.
};

struct topo_struct;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

// Postprocessing service embedded configuration information
typedef struct _AudDynaPPSvcCfg_t
{
   AudPP_AudProcType dynaPPSvcType;
   // type of AudPP chain, could be:
   // For Postproc: POPP, PDOPP or COPP, PGOPP
   // For PreProc: POPreP, COPreP etc.
   uint32_t dynaPPSvcTopologyID;
   // topology ID for PP chain

   uint16_t dynaPPSvcOutSizeInSample;
   // output frame size in Sample (per channel)
   uint16_t dynaPPSvcOutDurationInUsec;
   // output frame duration in micro sec
   uint16_t dynaPPSvcOutBufNumAfeFrames;
   //number of AFE frames in the output Buffer
   AudPPSvcOutputMediaTypeParams_t outputParams;

   bool_t      initializeWithCompressedFormat;

   uint16_t    perf_mode;
} AudDynaPPSvcCfg_t;


typedef struct _AudPPStatus_t
{
   AudPP_StateType dynaPPSvcState;
   // service state for AudPP state transition
   bool_t isSleeping;        // If true, PP does not process the data queue

   AudPP_BufInfo_t outputBufInfo;

   // ptr to input data msg
   elite_msg_data_buffer_t* pOutDataMsgDataBuf;
   // ptr to output msg data buf

   qurt_elite_bufmgr_node_t   outBufMgrNode;

   // for data source from Scorpion,
   // shared memory between SC & Q6 used.
   uint32_t unDataBitfield[AUDPP_MAX_INPUT_PORTS];
   // short-cut for setting pChannel to listen for data msg arrival on an input port
   uint32_t unBufBitfield;
   // short-cut for setting pChannel to listen for buf msg arrival
   uint32_t unCommandOnlyBitField;
   // short-cut for setting pChannel to listen for commands only
   uint32_t unCurrentBitfield;
   // short-cut for setting pChannel to listen for softPause ExpireSignal
   uint32_t unSoftPauseSignalOnlyBitfield;
    // short-cut for setting pChannel to listen for commands Qs only
   uint32_t unCmdQSignalOnlyBitfield;
	// short-cut for setting pChannel to listen for commands Qs only
   uint32_t unDataQSignalOnlyBitfield;
   //counter on the number of buffers remains to be delivered after receiving pause
   int32_t  nSoftPauseBufCounter;
   // holds the current bitfield to listen to
   bool_t m_fKeepProcessingonEos;
   bool_t rampOnResume;

   FILE*    fdbg;                       // Debug output - used only for simulation
   int16_t*   dbgptr;                     // Debug output - used only for simulation

} AudPPStatus_t;

typedef struct _AudPP_FormatConvType
{
   uint16_t instanceId;
   uint32_t numInputChannels;
   uint32_t numOutputChannels;
   bool_t deinterleave;

   // Scratch buffer
   int8_t *pTmpBuffer;
   uint32_t tmpBufSize;
} AudPP_FormatConvType;

// This contains all the required data for this service instance.
typedef struct _audpp_buf_delay_mgr
{
   AudPP_AudProcType svc_type;
   volatile uint32_t *buf_delay_ptr;
   uint32_t current_delay;
} audpp_buf_delay_mgr;
void audpp_buf_delay_mgr_init(audpp_buf_delay_mgr *obj, AudPP_AudProcType svc_type, volatile uint32_t *buf_delay_ptr);
void audpp_buf_delay_mgr_buf_duration_change(audpp_buf_delay_mgr *obj, uint32_t out_size_in_sample, uint32_t sampling_rate);
void audpp_buf_delay_mgr_buffer_freed(audpp_buf_delay_mgr *obj, uint32_t buf_delay);
void audpp_buf_delay_mgr_update_delay(audpp_buf_delay_mgr *obj, uint32_t buf_delay);
uint32_t audpp_buf_delay_mgr_get_current_delay(audpp_buf_delay_mgr *obj);


typedef struct AudPP_DataMsg_Cache_t
{
   uint32_t num_pending_data_msgs; 
   elite_msg_any_t* pCachedInDataMsg;

}AudPP_DataMsg_Cache_t;


typedef struct _AudPPInputPort_t
{
   elite_svc_handle_t handle;
   char              dataQName[QURT_ELITE_DEFAULT_NAME_LEN];

   bool_t			mediaFormatReceived;
   bool_t			mediaFormatHeld;
   bool_t			eosReceived;
   // Indicates whether we have received a valid media format command or not.
   AdspAudioMediaFormatInfo_t inputMediaFormatInfo;
   // media format (PCM) information for the input packet
   uint32_t			inputBufferCount;

   AudPP_BufInfo_t inputBufInfo;

   AudPP_BufInfo_t prevInputBufInfo;

   elite_msg_any_t* pInDataMsg;

   AudPP_DataMsg_Cache_t  elite_data_msg_cache;
   // ptr to input data msg
   elite_mem_shared_memory_map_t inputBufMemNode;


   AudPP_FormatConvType   formatConverter;

   bool_t                 formatConverterInitialized;
   // Converts the number of channels and interleaving

} AudPPInputPort_t;

//typedef struct _AudPPOutputPort_t
//{
//} AudPPOutputPort_t;

// This contains all the required data for this service instance.
typedef struct _ThisAudDynaPPSvc_t
{
	elite_svc_handle_t serviceHandle;
   AudPPInputPort_t *in_port[AUDPP_MAX_INPUT_PORTS];
   uint32_t		numActiveInPorts;
   uint32_t		numActiveOutPorts;
   // handle to give out to communicate
   // with static svc or SC
   qurt_elite_queue_t*  bufQ;
   // output buffer queue
   int               nBufsAllocated;
   // number of output bufs in circulation
   uint32_t          outputBufSizeInBytes;
   // Size of each output buffer
   elite_svc_handle_t *pDownstreamPeer;
   // This service only supports 1 downstream peer
   // data queue name
   char              cmdQName[QURT_ELITE_DEFAULT_NAME_LEN];
   // command queue name
   char              bufQName[QURT_ELITE_DEFAULT_NAME_LEN];
   // output buffer queue name
   uint16_t          objId;

   qurt_elite_channel_t channel;
   // channel for listening to mqs

   qurt_elite_timer_t         softPauseTimer;
   qurt_elite_signal_t        softPauseExpireSignal;
   qurt_elite_signal_t        BGThenableCmdQSignal;
   qurt_elite_signal_t        BGThenableDataQSignal;
   bool_t                softPauseExpireSignalInitialized;
   EliteMsg_CustomPPPausePrepareType      *pPauseMsgPayload;

   AudDynaPPSvcCfg_t ppCfgInfo;
   // PP configuration information
   // Indicates whether we have received a valid media format command or not.
   // media format (PCM) information for the input packet
   AdspAudioMediaFormatInfo_t outputMediaFormatInfo;
   // media format (PCM) information for the output packet
   AudPPStatus_t          audPPStatus;
   // Status information, which can change on every iteration of the workloop

   topo_struct *pTopologyObj;    // Pointer to the topology object

   uint32_t memoryMapClient;          // The memory map client passed in by ASM and ADM

   elite_qxdm_log_code qxdm_log_code_in;   //QXDM log code for input the PP service

   elite_qxdm_log_code qxdm_log_code_out;	//QXDM log code for output of PP service

   volatile uint32_t      *pAlgorithmicDelay;// Pointer to the memory location where the current algorithmic delay in microseconds will be stored.

   uint32_t topologyBandwidth;
   uint32_t topologyKpps;
   uint32_t bufferingDelay; // The current buffering delay in microseconds
   audpp_buf_delay_mgr    bufDelayMgr;  // Handles the calculation of the buffering delay

   uint32_t     ulDataLogId;      // data logging id: . upper 16 bits: apr port id. lower 16 bits: seq num.

   AudioStreamMgr_CallBackHandleType    CbData;  // AudPP store a copy of call back data

   // Event handling from the topo layer
   topo_event_client_t topo_event_client;
   // The PP service bandwidth depends on the media type. So the bandwidth must get calculated at the end of the media type update
   // function. In order to prevent multiple bandwidth update events raised by callbacks during the media type update, the
   // is_processing_set_media_type flag is used to prevent events from being sent. Exactly one event will be sent for bandwidth
   // update if the bandwidth has changed when the bandwidth is calculated at the end of the media type update function.
   // If the input media type has changes in the number of channels and the sampling rate, the sampling rate will be set to
   // the resampler first, causing a media type update event from the resampler. After this the media format with the changed
   // number of channels will be sent to the topology. This will cause another change in the output media type. This flag
   // is also used to prevent multiple such events.
   bool_t is_processing_set_media_type;
   // If the output media type changes while in a call to topo_process, the topo layer will raise an output media update event
   // and return immediately. The PP service layer must then send any partial output buffer and perform all the actions
   // associated with an output media update. The topo_process function will update the output buffer offsets when it returns,
   // so the output media change processing must not be done in the callback that is raised inside the topo_process function.
   // To achieve this behavior, the is_processing_data flag is set. The is_media_format_change_pending flag is set if the
   // callback was called in topo_process. It is then handled after topo_process returns.
   bool_t is_processing_data;
   bool_t is_media_format_change_pending;
   AdspAudioMediaFormatInfo_t new_media_format;
   uint32_t                             prev_kpps_vote;     //total KPPS of the decoder.
   uint32_t                             prev_bw_vote;   //previous bw vote inclu capi vote

   uint32_t                             ulAdsppmClientId;
   qurt_elite_adsppm_client_t           *adsppmClientPtr;

   uint32_t								output_buffer_count;
   bool_t	is_input_output_realtime;		// This variable is set by ASM/ADM to indicate if data
      	   	   	   	   	   	   	   	        //feeding into PP service is realtime and rendering is also realtime.
                                            //In existing use cases, this is set in ASM loopback
   void *bg_th_handle; 
   topo_bg_th_cb_info_t    bg_th_cb_info;
   audproc_event_node_t	*event_root_node;
} ThisAudDynaPPSvc_t;

extern const topo_event_vtable_t topo_event_vtable;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

// Packet Handling functions
ADSPResult AudPP_ParseMediaTypePacket(ThisAudDynaPPSvc_t *me, elite_msg_data_media_type_apr_t *pMediaFormatData, AdspAudioMediaFormatInfo_t *pMediaFmt,uint32_t port_id);
ADSPResult AudPP_DataProc_MediaFormat( ThisAudDynaPPSvc_t* me);
ADSPResult AudPP_MainThsetInputMediaInfo(ThisAudDynaPPSvc_t* me, const AdspAudioMediaFormatInfo_t *pInputMediaFormat,uint32_t port_id);
ADSPResult AudPP_DataProc_EosFormat( ThisAudDynaPPSvc_t* me,uint32_t port_id);
ADSPResult AudPP_DataProc_DataBufFormatProcSetUp(ThisAudDynaPPSvc_t* me,uint32_t port_id);
void AudPP_DataProc_DataBufFormatProc(ThisAudDynaPPSvc_t* me);
void AudPP_DataProc_ProcessNewPacket(ThisAudDynaPPSvc_t *me, elite_msg_any_t *pInDataMsg,uint32_t port_id);

void AudPP_DataProc_ProcessNewPacket_BGth_active(ThisAudDynaPPSvc_t *me, elite_msg_any_t *pInDataMsg,
	                                                          uint32_t port_id);

void AudPP_DataProc_ProcessAPRPacket(ThisAudDynaPPSvc_t *me);
void AudPP_SoftPauseTimerExpireHandler(ThisAudDynaPPSvc_t *me);
ADSPResult AudPP_SoftPause_Start(ThisAudDynaPPSvc_t *me);
ADSPResult AudPP_GenerateEos(ThisAudDynaPPSvc_t *me);
ADSPResult AudPP_ProcessOutputFormatMsg(ThisAudDynaPPSvc_t *me, elite_msg_any_t *pMsg);
ADSPResult AudPP_ProcessSetOutputSamplingRate(ThisAudDynaPPSvc_t *me, elite_msg_any_t *pMsg);
ADSPResult AudPP_AprMsgCallback(void *pCtxt, elite_msg_any_t msg);
ADSPResult AudPP_sendMediaTypeMsg(ThisAudDynaPPSvc_t *me);
ADSPResult AudPP_VoteBw(ThisAudDynaPPSvc_t* me);
void AudPP_UpdateOutputMediaFormat(ThisAudDynaPPSvc_t *me, const AdspAudioMediaFormatInfo_t *new_format);

ADSPResult AudPP_Alloc_DataMsg_Cache(ThisAudDynaPPSvc_t *me);
void AudPP_Dealloc_DataMsg_Cache(ThisAudDynaPPSvc_t *me);
void AudPP_DataProc_ReleasePendingPackets(ThisAudDynaPPSvc_t *me);

// Functions for setting parameters
ADSPResult AudPP_setInputMediaInfo(ThisAudDynaPPSvc_t* me, const AdspAudioMediaFormatInfo_t *pInputMediaFormat,uint32_t port_id);
ADSPResult AudPP_setOutputMediaInfo(ThisAudDynaPPSvc_t* me, const AudPPSvcOutputMediaTypeParams_t *pParams);
bool_t AudPP_MediaFormatsEqual(const AdspAudioMediaFormatInfo_t *pMF1, const AdspAudioMediaFormatInfo_t *pMF2);

// Buffer management functions
void AudPP_FreeCurrentInputBuffer(ThisAudDynaPPSvc_t* me, uint32_t port_id);
void AudPP_FreeCurrentOutputBuffer(ThisAudDynaPPSvc_t* me);
void AudPP_CheckForOutstandingBuffers(ThisAudDynaPPSvc_t* me);
ADSPResult AudPP_FlushDataQueue(ThisAudDynaPPSvc_t *me, uint32_t port_id);
void AudPP_deliverCurrentOutputBuffer(ThisAudDynaPPSvc_t* me, uint16_t numChannels = NUM_CHANNELS_INVALID,  uint16_t bitspersample = BITS_PER_SAMPLE_INVALID);
void AudPP_PackChannelData(const ThisAudDynaPPSvc_t* me, const uint16_t numChannels, const uint16_t bitspersample);
ADSPResult AudPP_TryGetOutputBuffer(ThisAudDynaPPSvc_t* me);
ADSPResult AudPP_AllocateOutputBuffers(ThisAudDynaPPSvc_t *me, const uint32_t bufSize, const uint32_t numBufs);
uint32_t AudPP_NumOutputBuffersRequired(ThisAudDynaPPSvc_t *me);
ADSPResult AudPP_CheckAndAdjustPCMBufSize(ThisAudDynaPPSvc_t *me);
ADSPResult AudPP_CheckAndAdjustCompressedBufSize(ThisAudDynaPPSvc_t *me, uint32_t actualSize, uint32_t maxSize, uint32_t inSizeInSamples);
ADSPResult AudPP_DiscardMarkBuffer(ThisAudDynaPPSvc_t *me,elite_msg_any_t *dataQMsg);

// State management functions
void AudPP_GotoSleepState(ThisAudDynaPPSvc_t *me);
void AudPP_GotoActiveState(ThisAudDynaPPSvc_t *me);
bool_t AudPP_isSleeping(const ThisAudDynaPPSvc_t *me);
bool_t AudPP_isPaused(const ThisAudDynaPPSvc_t *me);
bool_t AudPP_isDisconnected(const ThisAudDynaPPSvc_t *me);
void AudPP_GotoFatalState(ThisAudDynaPPSvc_t *me);
bool_t AudPP_isInFatalState(const ThisAudDynaPPSvc_t *me);
ADSPResult AudPP_SuspendPrepare(const ThisAudDynaPPSvc_t *me);
ADSPResult AudPP_RunPrepare(const ThisAudDynaPPSvc_t *me);

// Buffer format conversion functions
ADSPResult AudPP_InitFormatConv(AudPP_FormatConvType *pConv, const uint16_t instanceId);
ADSPResult AudPP_SetupFormatConv(AudPP_FormatConvType *pConv, uint32_t inputChannels, uint32_t outputChannels, bool_t deinterleave, uint32_t samplingRate);
void AudPP_DestroyFormatConv(AudPP_FormatConvType *pConv);
ADSPResult AudPP_ProcessFormatConv(AudPP_FormatConvType *pConv, int8_t *pBuffer, uint32_t samplesPerChannel, uint32_t bytes_per_sample);
ADSPResult AudPP_ResetFormatConv(AudPP_FormatConvType *pConv);


// util function
bool_t switchInGapless( ThisAudDynaPPSvc_t *me);

// Port utility functions
bool_t AudPP_isPrimaryInputPort(uint32_t port_id);
bool_t AudPP_isPortRunning(ThisAudDynaPPSvc_t* pMe,uint32_t port_id);

//PP initialization functions
ADSPResult AudPP_InitializePPFeatures(ThisAudDynaPPSvc_t* me);
void AudPP_ReInitCompressTopoWithOutputSampleSize(ThisAudDynaPPSvc_t* me);

bool_t topo_is_module_present(void *obj_ptr, const uint32_t module_id);

/**
 * Utility function to check if the packet is a data buffer
 * packet or not.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] pMsg
 * The message to be checked.
 *
 * @return bool_t
 * TRUE if the message is a data buffer
 * FALSE otherwise.
 */
static inline bool_t AudPP_isDataBuffer(const elite_msg_any_t *pMsg)
{
   if (ELITE_DATA_BUFFER == pMsg->unOpCode)
   {
      return TRUE;
   }

   return FALSE;
}

static inline bool_t AudPP_isEosBuffer(const elite_msg_any_t *pMsg)
{
   if (ELITE_DATA_EOS == pMsg->unOpCode)
   {
      return TRUE;
   }

   return FALSE;
}

static inline bool_t AudPP_isMarkBuffer(const elite_msg_any_t *pMsg)
{
   if (ELITE_DATA_MARK_BUFFER == pMsg->unOpCode)
   {
      return TRUE;
   }

   return FALSE;
}

static inline bool_t isCompressedFormat(const AdspAudioMediaFormatInfo_t *pMediaFormat)
{
    return ((pMediaFormat->bitstreamFormat != ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2) &&
            (pMediaFormat->bitstreamFormat != ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3) &&
            (pMediaFormat->bitstreamFormat != ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4));
}

static inline bool_t isCompressedInput(const ThisAudDynaPPSvc_t* me,uint32_t port_id)
{
    return isCompressedFormat(&me->in_port[port_id]->inputMediaFormatInfo);
}

// For AudPP logging

#define AUDIOLOG_AUD_PP_IN_TAP_ID       0x0001
#define AUDIOLOG_AUD_PP_OUT_TAP_ID      0x0002

ADSPResult AudPP_LogData(ThisAudDynaPPSvc_t* me,uint32_t port_id);
ADSPResult AudPP_LogData_Output(ThisAudDynaPPSvc_t* me,uint32_t port_id);

ADSPResult AudPP_pptype_to_topo_db_client_type(AudPP_AudProcType pp_type, elite_topo_db_client_type* client_type);

ADSPResult AudPP_RegisterWithAdsppm(ThisAudDynaPPSvc_t* pMe);
ADSPResult AudPP_DeregisterWithAdsppm(ThisAudDynaPPSvc_t* pMe);
ADSPResult AudPP_ProcessKppsBw(ThisAudDynaPPSvc_t *pMe, bool_t is_release, bool_t force_vote);
uint32_t AudPP_AggregateKpps(ThisAudDynaPPSvc_t *pMe);
uint32_t AudPP_AggregateBw(ThisAudDynaPPSvc_t *pMe);

#endif // #ifndef ELITE_AUDYNAPP_CMNDEF_H
