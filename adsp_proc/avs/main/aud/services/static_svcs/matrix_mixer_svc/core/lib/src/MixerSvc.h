/**
@file MixerSvc.h
@brief This file declares some common data types used by Audio
       Matrix Mixer.
 */

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/01/2010 AAA      Created file.
==========================================================================*/

#ifndef MATRIX_MIXER_H
#define MATRIX_MIXER_H

#include "EliteLoggingUtils_i.h"
#include "adsp_adm_api.h"
#include "MixerSvc_Interface.h"
#include "stream_dev_drift_calc.h"
#include "AFEInterface.h"
#include "qurt_elite_adsppm_wrapper.h"
#include "MixerSvc_AvSync.h"
#include "adsp_mtmx_strtr_api.h"
#include "MixerSvc_Limiter.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//#define MT_MX_EXTRA_DEBUG
#define NUM_CHANNELS_MONO                                         1
#define NUM_CHANNELS_STEREO                                       2
#define MT_MX_NUM_CHANNELS_ONE                                    1
#define MT_MX_NUM_CHANNELS_TWO                                    2
#define MT_MX_NUM_CHANNELS_THREE                                  3
#define MT_MX_NUM_CHANNELS_FOUR                                   4
#define MT_MX_NUM_CHANNELS_FIVE                                   5
#define MT_MX_NUM_CHANNELS_SIX                                    6
#define MT_MX_NUM_CHANNELS_SEVEN                                  7
#define MT_MX_NUM_CHANNELS_EIGHT                                  8
#define MAX_CHANNEL_MAPPING_NUMBER                                34

#define MT_MX_DEFAULT_BYTES_PER_SAMPLE                            2
#define MT_MX_BYTES_PER_SAMPLE_TWO                                2
#define MT_MX_BYTES_PER_SAMPLE_FOUR                               4
#define MT_MX_BIT_WIDTH_16                                        16
#define MT_MX_BIT_WIDTH_24                                        24
#define MT_MX_BIT_WIDTH_32                                        32
#define MT_MX_DEFAULT_BIT_WIDTH                                   MT_MX_BIT_WIDTH_16


/*****************************************************************/
/* IMPORTANT: New input port states must be added such that any  */
/* state before input port starts processing PCM buffers should  */
/* have a value less than INPUT_PORT_STATE_ACTIVE.               */
/*****************************************************************/
#define INPUT_PORT_STATE_INACTIVE                                 0 /* port not in use */
#define INPUT_PORT_STATE_INIT                                     1 /* port just created, hasn't received any cmds/msgs, can process only cmds, cannot process dataQ in this state */
#define INPUT_PORT_STATE_WAITING_TO_BE_READY                      2 /* after processing first RUN cmd (right after INIT) but waiting to be able to process dataQ (due to RUN cmd specifying a future time or a positive relative offset) */
#define INPUT_PORT_STATE_READY                                    3 /* after processing first RUN cmd (right after INIT) and now able to process dataQ, can process only MEDIA_TYPE msg on dataQ*/
#define INPUT_PORT_STATE_WAITING_TO_BE_ACTIVE                     4 /* after processing steady-state RUN cmd but waiting to be able tp process dataQ (due to RUN cmd specifying a future time or a positive relative offset) */
/* input port can process PCM data buffers in the below states. */
#define INPUT_PORT_STATE_ACTIVE                                   8 /* after processing first MEDIA_TYPE msg, can now process PCM buffers on dataQ */
#define INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER              9
#define INPUT_PORT_STATE_PAUSED                                   10 /* port in pause state */

#define OUTPUT_PORT_STATE_INACTIVE                                0
#define OUTPUT_PORT_STATE_ACTIVE                                  1
#define OUTPUT_PORT_STATE_INIT                                    2
#define OUTPUT_PORT_STATE_WAITING_TO_BE_ACTIVE                    3
#define OUTPUT_PORT_STATE_PAUSED                                  4
#define OUTPUT_PORT_STATE_DISABLED                                5

#define INPUT_BUFFER_RELEASED                                     0
#define INPUT_BUFFER_HELD                                         1

#define MT_MX_MAX_PORTS                                           26
#define MT_MX_MAX_INPUT_PORTS                                     16
#define MT_MXAR_MAX_INPUT_PORTS                                   16
#define MT_MXAT_MAX_INPUT_PORTS                                   9
#define MT_MXAR_MAX_OUTPUT_PORTS                                  9
#define MT_MXAT_MAX_OUTPUT_PORTS                                  10

#define MT_MX_MAX_OUTPUT_PORTS                                    (MT_MX_MAX_PORTS - MT_MX_MAX_INPUT_PORTS)

//This will be used to identify that a stream session is ULL and will be used in the map routing command.
#define MT_MX_ULL_SESSION                                         (MT_MX_MAX_PORTS + 1)
#define MT_MX_LLNP_SESSION                                        ((MT_MX_MAX_PORTS*2) + 1)
#define MT_MX_ULLPP_SESSION                                       ((MT_MX_MAX_PORTS*3) + 1)

#define MT_MX_INPUT_PORT_UNDERFLOW_REPORT_STATUS_DO_NOT_REPORT    0
#define MT_MX_INPUT_PORT_UNDERFLOW_REPORT_STATUS_REPORT           1

#define MT_MX_OUTPUT_PORT_OVERFLOW_REPORT_STATUS_DO_NOT_REPORT    0
#define MT_MX_OUTPUT_PORT_OVERFLOW_REPORT_STATUS_REPORT    1

#define MT_MX_INPUT_PORT_STRMASK_CLEARED                          0
#define MT_MX_INPUT_PORT_STRMASK_BIT_REMOVED                      1
#define MT_MX_INPUT_PORT_STRMASK_BIT_ADDED                        2

#define MT_MX_OUTPUT_PORT_INPORTSMASK_CLEARED                     0
#define MT_MX_OUTPUT_PORT_INPORTSMASK_BIT_REMOVED                 1
#define MT_MX_OUTPUT_PORT_INPORTSMASK_BIT_ADDED                   2

#define MT_MX_NUM_SAMPLES_TO_ADD_OR_DROP_PER_BUFFER               1
#define MT_MX_INPUT_PORT_NO_ADJUSTMENT_OF_SAMPLES                 0
#define MT_MX_INPUT_PORT_ADD_SAMPLES                              1
#define MT_MX_INPUT_PORT_DROP_SAMPLES                             2

#define MT_MX_SAMPLING_RATE_48000                                 48000

#define MT_MX_FRAME_DURATION_1000US                               1000
#define MT_MX_FRAME_DURATION_5000US                               5000
#define MT_MX_FRAME_DURATION_10000US                              10000

#define MT_MX_LEGACY_NUM_AFE_FRAMES                               5
#define MT_MX_LL_NUM_AFE_FRAMES                                   1

#define MAX_DATA_Q_ELEMENTS                                       16
#define MAX_INPUT_CMD_Q_ELEMENTS                                  8
#define MAX_INDEX_CHANNEL_MIXER_SET_PARAM                         8
#define UNITY_Q14                                                 16384

#define MT_MX_MAX_SAMPLING_RATE                                   (384000)

//Following are the KPPS (= MPPS / 1000) requirement of MTMX

// MTMX requirement for 48kHz, stereo, 16-bit cases for both legacy and low latency cases.
#define MTMX_KPPS_48K_STEREO_16BIT_LEG_LAT (2000)
#define MTMX_KPPS_48K_STEREO_16BIT_LOW_LAT (3000)
#define MTMX_KPPS_48K_STEREO_24BIT_LEG_LAT (3000)
#define MTMX_KPPS_48K_STEREO_24BIT_LOW_LAT (4000)

// MTMX requirement for 192kHz, stereo, 16-bit cases for both legacy and low latency cases.
#define MTMX_KPPS_192K_STEREO_16BIT_LEG_LAT (4000)
#define MTMX_KPPS_192K_STEREO_16BIT_LOW_LAT (5000)
#define MTMX_KPPS_192K_STEREO_24BIT_LEG_LAT (5000)
#define MTMX_KPPS_192K_STEREO_24BIT_LOW_LAT (6000)

//Number of elements in the PSPD Q's
#define MTMX_NUM_PSPD_Q_ELEM        (1)

typedef enum eChMixerType
{
    PSPD_CHMIXER_NONE = 0,
    PSPD_CHMIXER_QCOM,
}eChMixerType;

//Utility macros
#define MTMX_FREE(ptr) \
        do { \
            qurt_elite_memory_free(ptr); \
            ptr = NULL; \
        } while(0)

#define MTMX_ALIGNED_FREE(ptr) \
        do { \
            qurt_elite_memory_aligned_free(ptr); \
            ptr = NULL; \
        } while(0)

#define MT_MX_SAMPLES_TO_USEC(num_of_samples, samples_per_second) \
        (((samples_per_second) == 0) ? (0) : (((uint64_t)(num_of_samples) * 1000000) / (samples_per_second)))

typedef struct mt_mx_struct_ramp_gain_t  mt_mx_struct_ramp_gain_t;
struct mt_mx_struct_ramp_gain_t
{
    uint32_t                   unRampSizeInSamples;
    uint16_t                   unStepSizeInSamples;
    uint16_t                   unRampCurve;
    uint32_t                   unNumRemSteps;
    uint16_t                   unTargetGain[MT_MX_NUM_CHANNELS_EIGHT];
    int16_t                    nGainIncDecStep[MT_MX_NUM_CHANNELS_EIGHT];
    uint16_t                   unCurrentGain[MT_MX_NUM_CHANNELS_EIGHT];
    bool_t                     bShouldRampGainBeApplied;
    bool_t                     bIsRunningInSlowRampModeSR[MT_MX_NUM_CHANNELS_EIGHT];
    uint16_t                   unNumSamplesWaitToIncDecSR[MT_MX_NUM_CHANNELS_EIGHT];
    uint16_t                   unCurrentSamplesWaitCounterSR[MT_MX_NUM_CHANNELS_EIGHT];
    bool_t                     bIsChannelMuted[MT_MX_NUM_CHANNELS_EIGHT];
    uint16_t                   unCachedTargetGainAfterUnMute[MT_MX_NUM_CHANNELS_EIGHT];
};

typedef struct mt_mx_struct_channel_mixer_t  mt_mx_struct_channel_mixer_t;
struct mt_mx_struct_channel_mixer_t
{
    bool_t                                    bIsQcomChannelMixerLibCreated;
    bool_t                                    bCanChMixerBeOptimizedOut;
    uint32_t                                 unCachedPsPdSetParamID;
    void                                      *punCachedPsPdSetParam;
};


typedef struct mt_mx_struct_pspd_t mt_mx_struct_pspd_t;
struct mt_mx_struct_pspd_t
{
    elite_svc_handle_t             thread_param;
    int8_t                         *pPspdOutBuf;
    uint32_t                       unPspdOutBufSize;
    volatile uint32_t              unPspdKpps; /* Var used by the PSPD to store req KPPS */
    volatile uint32_t              unPspdBw; /* Var used by the PSPD to store req BW in Bps */
    elite_msg_data_buffer_v2_t     inp_buf;
    elite_msg_data_buffer_v2_t     out_buf;
    QURT_ELITE_ALIGN(char cmd_resp_q[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MTMX_NUM_PSPD_Q_ELEM)], 8);
    QURT_ELITE_ALIGN(char inp_data_q[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MTMX_NUM_PSPD_Q_ELEM)], 8);
    QURT_ELITE_ALIGN(char out_data_q[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MTMX_NUM_PSPD_Q_ELEM)], 8);
};

typedef struct MatrixInPortInfoType MatrixInPortInfoType;
struct MatrixInPortInfoType
{
    //General properties
    MtMxInPortHandle_t       inPortHandle;                                            //Handle to the input port
    char                      dataQName[QURT_ELITE_DEFAULT_NAME_LEN];  //Data queue name
    elite_msg_any_t     myDataQMsg;
    elite_msg_data_buffer_t* pInputBuf;                                   //Input PCM buffer
    int32_t                   inPortState;                                          //Holds the current state of the i/p
    int32_t                   inPortStatePrToStateChange;               //Holds the state of the i/p port  prior to a port state change.
    bool_t                    bIsPortLive;                          //Indicates whether the input port is live (TRUE) or non-live (FALSE)
    bool_t                    bIsInPortBurstMode;            //Indicates whether the input port is in burst mode (TRUE) or non-burst mode (FALSE)
    Elite_CbType          eliteCb;                                //Callback structure provided by static svc
    uint16_t                 unPortPerfMode;    //Performance mode in which the input port is operating. Supported values are
    //ADM_LEGACY_DEVICE_SESSION & ADM_LOW_LATENCY_DEVICE_SESSION
    //For ADM_LOW_LATENCY_DEVICE_SESSION, 1msec local buffer is allocated
    //For ADM_LEGACY_DEVICE_SESSION, 5msec local buffer is allocated

    //Masks
    uint32_t                 strMask;                              //Mask that shows the outputs that the current input feeds
    uint32_t                 activeOutPortsMask;           //Mask that indicates which output ports (out of all that this input is feeding to) are in active state
    uint32_t                 outputReqPendingMask;     //Mask that shows the outputs currently waiting for samples from this input.
    uint32_t                 eosPendingMask;                //Mask that indicates which output ports (out of all that this input is feeding to) have an EOS pending
    uint32_t                 eosPendingAckMask;             //Mask that indicates which output ports are waiting for ack from AFE for EoS.

    //Local buffer and filling logic
    uint32_t                 unNumSamplesPerChReqToFillLocalBuf; //Number of samples (per channel) required to fill local buffer
    int8_t                     *pCurrentSample;               //Points to the current sample being used from the upstream peer svc's output buffer
    int                         nNumRemainingSamplesPerCh;  //Number of samples remaining in the upstream peer svc's output buffer
    int                         nNumSamplesUsedPerCh;          //Number of samples used up from the upstream peer svc's output buffer
    uint32_t                 unTopPrioOutPort;         //ID of the highest priority o/p port that this i/p port is feeding to
    int8_t*                   pWrLoc;                          //Write pointer of local buffer
    int8_t*                   pStartLoc;                      //Start pointer
    uint32_t                 numBufRcvd;                 //Counter keeping track of # of i/p bufs rcvd by this i/p port so far
    uint32_t                 numBufReturned;          //Counter keeping track of # of i/p bufs returned by this i/p port so far
    bool_t                    bInBufStatus;                 //Flag to indicate of a request for new input buffer has been sent
    bool_t                    bIsLocalBufFull;
    bool_t                    bIsInBufShort;
    bool_t                    bIsLocalBufEmpty;
    bool_t                    bIsAccumulationPending;
    bool_t                    bIsNonLiveInputPortWaitingToAcc;
    bool_t                    bHasFirstSampleAccumulated;
    adsp_time      ullPreviousSamplesInUsec;          //Keeps track of inputData samples in usec remaining to process from the previous buffer.

    //EOS
    bool_t                    bIsLastMsgEOS;
    bool_t                    bIsEOSHeld;
    elite_msg_any_t           gaplessEOSmsg;  // Stores the Gapless EOS message temporarily until all the connected o/p ports sent it downstream.
    bool_t                    bIsGaplessEOSMsgWaitingToBeSentDS; //A flag to indicate whether an i/p port wants to send Gapless EOS downstream.
    uint64_t                 ullStaleEosSamples;
    bool_t                   bIsSessionUnderEOS;
    elite_msg_data_eos_info_t            eosInfo;

    //Media type
    uint32_t                 unAfeFrameSizeInSamples; //Hold the unit frame size in samples as defined by AFE
    adsp_time                unFrameDurationInUsec;      // Frame duration in microseconds
    uint32_t                 unSamplesPer1Msec;          // Samples per 1 milli second
    uint32_t                 unSampleRate;             // Sample Rate
    uint32_t                 unNewSampleRate;                   //New sample Rate: Used when media fmt is rcvd in steady-state and i/p port's local buf cannot be init'ed immediately
    uint32_t                 unTargetSampleRate;                //Target sample rate to expect. This is set in case there is a POPP sampling rate change initiated that will
    //eventually change the sampling rate of the i/p port
    elite_msg_any_t     msgsToReturn[MAX_DATA_Q_ELEMENTS];//Temporarily, store the messages to return to the PP later on. This will be used when there is a port
    //state change on the i/p port of RX matrix. Buffers will be returned to upstream PP service once the
    //new media type has been received. This is done to avoid a drainage of the pipeline in the case of FTRT.
    uint16_t                 unNumMsgsToReturn;//Keeps track of how many messages to return.
    uint32_t                 unNumChannels;              //Number of channels
    uint32_t                 unNewNumChannels;       //New number of channels: Used when media fmt is rcvd in steady-state and i/p port's local buf cannot be init'ed immediately
    uint8_t                   unChannelMapping[MT_MX_NUM_CHANNELS_EIGHT];        //Channel mapping
    uint8_t                   unNewChannelMapping[MT_MX_NUM_CHANNELS_EIGHT]; //New channel mapping: Used when media fmt is rcvd in steady-state and
    //i/p port's local buf cannot be init'ed immediately
    uint32_t                 unChMapToChRevLUMask[MAX_CHANNEL_MAPPING_NUMBER];
    // An array of masks which indicates the following
    // unChMapToChRevLUMask[0] = Channel Map Value "1" --> Which channel?
    // unChMapToChRevLUMask[1] = Channel Map Value "2" --> Which channel?
    //                          ...
    // unChMapToChRevLUMask[7] = Channel Map Value "8" --> Which channel?
    //                          ...
    // unChMapToChRevLUMask[11]= Channel Map Value "12"--> Which channel?
    int                        nInterleaveFlag;              //Interleaved/non_interleaved data
    int                        unBitwidth;                     //16/24/32 bit
    uint32_t                   unNewBitwidth;                  //New bit-width, 16/24/32 bit:Used when media fmt is rcvd in steady-state and i/p port's local buf cannot be init'ed immediately
    uint16_t                unBytesPerSample;         //# of bytes per sample
    uint16_t                unNewBytesPerSample;  //New # of bytes per sample: Used when media fmt is rcvd in steady-state and i/p port's local buf cannot be init'ed immediately
    uint32_t                unInPortPerChBufSize;        //Input port local buffer per channel size
    uint32_t                unInPortBufSize;                 //Input port local buffer size
    bool_t                  bShouldLocalBufBeReInit;  //Boolean indicating whether this input port needs to be reinit
    uint32_t                unNumAfeFrames;           //Number of AFE frames in matrix frame
    uint32_t                unNewNumAfeFrames;        //New number of AFE frames in matrix frame
    bool_t                   bIsBufferReinitPending;          //Indicates if buffer reinitialization is pending in this port
    bool_t                  bDoesInputPortNeedToTransitionAwayFromReady; //Does this i/p port need to transition away from ready?

    //AV-Sync
    mt_mx_inport_avsync_t *pInPortAvSync;      //Pointer to the AV-Sync structure for this input port

    //PSPD (Per-Stream, Per-Device)
    mt_mx_struct_pspd_t                structPspd[MT_MX_MAX_OUTPUT_PORTS]; //Stucture holding information about the PSPD thread
    mt_mx_struct_channel_mixer_t structChanMixer[MT_MX_MAX_OUTPUT_PORTS]; //Structure holding information about the PSPD Channel Mixer library
    mt_mx_struct_ramp_gain_t        structRampGain[MT_MX_MAX_OUTPUT_PORTS]; // structure holds info about PSPD ramp gains
    qurt_elite_channel_t                  pspd_channel; //Channel to manage PSPD threads under this input port. For each PSPD thread 3 bits are used - cmd, inp, out
                                                                                     //9 output ports per input port would require 27 signal bits
    uint32_t                                     unCurrentWaitMask; //A combinational mask which indicates which bits of the above channel is this i/p port waiting on

    //Misc.
    uint32_t                  unDataLogId;           //Data logging id: In case of stream-side port, upper 16 bits: asm apr port id lower 16 bits seq num.
    uint8_t                     unUnderflowReportStatus;  //Whether to report underflow or wait for valid data
    bool_t                      bIsRegisteredForUnderflowEvents;
    bool_t                      bInputPortJustMovedFromHeldToActive;
    uint64_t                  ullIncomingBufferTS;                                    //Incoming buffer TS in usec
    bool_t                      bIsTimeStampValid;
    bool_t                      bForceCheckTimeStampValidity;
    bool_t                      bShouldSessionTimeBeDerivedFromNextTS;
    bool_t                      bShouldSessionTimeBeResync;                    //Indicates that the session time must be resynced. Applicable for gapless EOS or Resync Command
    adsp_time               ullTimeStampAtCopy;                                   //Timestamp in microseconds of the current input buffer that is being copied (offset from the beginning).
    adsp_time               ullTimeStampAtAccumulation;                   //Timestamp in microseconds of the current input buffer that is being accumulated (offset from the beginning).
    adsp_time               ullTimeStampProcessed;                              //Timestamp in microseconds of the buffer that was last rendered.
    volatile uint32_t   *punMtMxInDelay;                                         //Address to the MXAT i/p port delay. MXAR will ignore this
    uint64_t                   ullNumSamplesForStcUpdate;                     //Keeps track of the total number of samples processed, used to calculate and set the Session Time Clock
    adsp_time                 ullTimeStampForStcBaseInUsec;                //Keeps track of the first buffer TS received after run command
    uint32_t                   unDataRcvdInSamples;
    adsp_time               unDataRcvdInUsec;
    uint64_t                   ullSessionTime;
    uint64_t                   ullAbsoluteTime;
    avtimer_drv_handle_t avt_drv_handle;                                        //Handle to avtimer (If AvSync is stubbed out, this is still used; If AvSync is not stubbed out, AvSyncLib will store handle)
    bool_t                     bIsAdmLoopback;
};

typedef struct MatrixOutPortInfoType MatrixOutPortInfoType;
struct MatrixOutPortInfoType
{
    //General properties
    MtMxOutPortHandle_t outPortHandle;   //Handle to the output port
    qurt_elite_queue_t*     bufQ;                 //Handle to the buffer Q of the port
    char                            bufQName[QURT_ELITE_DEFAULT_NAME_LEN];     //Buffer Q name
    uint32_t                       unMaxBufQCapacity;      //Max number of buffers the buf Q can hold
    elite_svc_handle_t*      pDownstreamPeerHandle; //Ptr to downstream peer's handle
    elite_msg_data_buffer_t* pOutputBuf;             //Ptr to port's output buffer
    int                               outPortState;                            //Holds the current state of the o/p
    int                               nOutputMode;                  //Mode in which output port is operating: push mode or pull mode
    uint32_t                       numBufRcvd;                    //Number of o/p buffers received
    uint32_t                       numBufSent;                    //Number of o/p buffers sent downstream
    int64_t*                    pAccBuf; //Pointer to output port's accumulator buffer (64-bit buffer)
    Elite_CbType                eliteCb;                //Callback structure provided by static svc
    bool_t                          bIsFirstOutBufYetToBeSent;
    bool_t                          bIsTopPriorityInputPortLive;     //A flag to indicate that the o/p port's top priority i/p port is Live
    uint16_t                       unPortPerfMode;    //Performance mode in which the output port is operating. Supported values are
    //ADM_LEGACY_DEVICE_SESSION & ADM_LOW_LATENCY_DEVICE_SESSION
    //For ADM_LOW_LATENCY_DEVICE_SESSION, 1msec local buffer is allocated
    //For ADM_LEGACY_DEVICE_SESSION, 5msec local buffer is allocated

    //Media type
    uint32_t                     unAfeFrameSizeInSamples; //Hold the unit frame size in samples as defined by AFE
    adsp_time                    unFrameDurationInUsec;   // Frame duration in microseconds
    uint32_t                     unSampleRate;                 //Sampling Frequency
    uint32_t                     unNumChannels;              //Number of channels
    uint8_t                      unChannelMapping[MT_MX_NUM_CHANNELS_EIGHT];    //Channel mapping
    uint32_t                     unBytesPerSample;          //Number of bytes per sample
    uint32_t                     unBufSize;                       //Output port's buffer size in samples.
    uint32_t                     unBufSizeInBytes;                       //Output port's buffer size in bytes.
    uint32_t                     unOutPortPerChBufSize; //Output port's buffer size per channel
    uint32_t                     unInterleaveFlag;            //Flag indicating whether output is interleaved or not.
    uint32_t                     unBitwidth;             // PCM samples bit width.
    uint32_t                     unNumAfeFrames;           //Number of AFE frames in matrix frame
    uint32_t                     unNewNumAfeFrames;        //New number of AFE frames in matrix frame

    //Masks
    uint32_t                     inPortsMask;                      //Mask that indicates which input ports feed to this output port.
    uint32_t                     accInPortsMask;                 //Mask that indicates which input ports accumulated to this output port's accBuf
    uint32_t                     accBufAvailabilityMask;       //Mask that indicates which input ports are allowed to accumulate to this output port's accBuf
    uint32_t                     inPortsWaitingToAccMask; //Mask that indicates which input ports are waiting to accumulate to this output port's accBuf
    //NOTE: Live i/p ports should not be setting this mask. It is intended for non-live i/p ports ONLY
    uint32_t                     inPortsTopPriorityMask;     //Mask that indicates which of the input ports connected to this o/p port has the highest
    //(top) priority. The highest priority i/p always drives the action in the case of an i/p driven model (TX matrix)
    //If multiple i/ps connect to one o/p port, then live ports gets priority over non-live ports
    //Tie breaker: First come first privelege to decide on the top priority i/p port

    //Re-init/Re-config
    uint32_t                    unNumOutputBufs;                           //Number of o/p buffers to be allocated for this output port
    uint32_t                    unNumOutputBufsPendingRelease;  //Number of o/p buffers that this port needs to wait on before reconfiguring this o/p port
    uint32_t                    unOutPortReconfigMode;                //The reconfig mode that this o/p port was configured for.
    bool_t                       bIsOutPortWaitingForRscsToSendOut;//A flag to indicate that the o/p port needs more resources to send out the data already accumulated in its acc. buffer
    uint8_t                     unNativeModeFlags; // Flag used to indicate native mode operation.
    bool_t                       bIsOutPortInNativeModePendingReInit;//A flag to indicate that an o/p port that is operating in native mode is pending Re-init
    bool_t                       bIsBufferReinitPending;   //Indicates if buffer reinitialization is pending in this port
    bool_t                       bIsOutPortPendingReleaseBuffers; //This indicates if this o/p port is pending any buffers to be released
    uint32_t                    unNumChannelsPrRecfg, unBytesPerSamplePrRecfg, unSampleRatePrRecfg;
    uint8_t                      usChannelMappingPrRecfg[MT_MX_NUM_CHANNELS_EIGHT];
    int                             outPortStatusPriorToReconfig; //Holds the o/p port status prior to reconfig.

    //AV-Sync
    mt_mx_outport_avsync_t *pOutPortAvSync;      //Pointer to the AV-Sync structure for this output port

    //Misc.
    uint32_t                         unDataLogId;                                                                 //Data logging id: In case of stream-side port, upper 16 bits: asm apr port id lower 16 bits seq num.
    bool_t                             bIsRegisteredForOverflowEvents;
    uint8_t                            unOverflowReportStatus;                                          //Status to report overflow
    bool_t                             bIsOutPortDelivTimeSyncDataWithAbsoluteTS; //A flag to indicate that this o/p port is sending down time synchronized data and absolute timestamps
    avtimer_drv_handle_t avt_drv_handle;                                                            //Handle to avtimer
    volatile uint32_t          *punMtMxOutDelay;                                                  //Address to the MXAR o/p port delay. MXAT will ignore this. (AV-Sync).
    volatile uint32_t          *punAFEDelay;                                                            //Address to the downstream AFE delay (AV-Sync).
    volatile uint32_t          *punCoppBufDelay;                                                    //Address to the downstream COPP buffering delay (AV-Sync).
    volatile uint32_t          *punCoppAlgDelay;                                                    //Address to the downstream COPP algorithmic delay (AV-Sync).
    adsp_time                      llSessionTime;                                                             //Session time of the stream in microseconds
    uint64_t                          ullPreviousWCTime;                                                  //Wallclock time when previous output buffer was sent down
    uint32_t                          unConnInPortIdWithValidTs;     //Index of the first connected input port with a valid timestamp. This input port timestamp should be propagated downstream.
                                                                                                      //Set this to MT_MX_INVALID_PORT_ID upon initialization or if there isn't an input port with a valid timestamp
    mt_mx_struct_limiter_t    structLimiter; //Structure for Limiter module needed for smooth limiting to mixer output
};

// This contains all the required data for a service instance. Like the data members of a class...
struct This_t
{
    //General properties
    uint32_t                 mtMxID;                        // Mt Mx instance identifier
    elite_svc_handle_t        serviceHandle;              //Handle to give out to other services
    MatrixInPortInfoType    *inPortParams[MT_MX_MAX_INPUT_PORTS];        //Array of ptr to structures that hold info of all input ports
    MatrixOutPortInfoType *outPortParams[MT_MX_MAX_OUTPUT_PORTS];  //Array of ptr to structures that hold info of all output ports
    uint32_t                 maxInPortID;                       //Max input port ID
    uint32_t                 maxOutPortID;                    //Max output port ID
    char                      cmdQName[QURT_ELITE_DEFAULT_NAME_LEN];  //Command queue name
    bool_t                    bIsMxAtOperatingInBurstMode;   //This would indicate if the MXAT is operating in burst mode

    //Channels, bitmasks etc
    qurt_elite_channel_t     channel;                   //Mask for MQ's owned by this obj
    uint32_t                 unDataBitfield;                // pChannel sub-mask containing data Qs.
    uint32_t                 unCurrentBitfield;           //Matrix's overall pChannel wait mask.
    uint32_t                 unBufBitfield;                 // pChannel sub-mask containing buf Qs.
    uint32_t                 inPortIDMap[32];            //Array that maps i/p port IDs to their bit position in the channel.
    uint32_t                 outPortIDMap[32];         //Array that maps o/p port IDs to their bit position in the channel.
    uint32_t                 unChannelStatus;          //Bitfield returned from qurt_elite_channel_wait indicating which blocked Q's have msgs.

    //Masks
    uint32_t                 steadyStateInPortsMask;        //Input ports that: a. are active, b. have non-zero strMask and c. and are feeding to at least one active o/p port.

    //Misc.
    elite_qxdm_log_code         qxdm_log_code;                  //QXDM log code for the Matrix mixer service

    //MPPS/BW
    uint32_t                                             unKpps;                                    //KPPS for this matrix
    uint32_t                                             unBandWidth;                        //BW needs for this matrix
    uint32_t                                             unAdsppmClientId;
    qurt_elite_adsppm_client_t           *pAdsppmClient;
};

ADSPResult AudCreateMatrixMixerSvc (uint32_t inputParam, void **handle, MtMxParams_t* pMtMxParams);
ADSPResult MtMx_CustomMsgHandler(void* pInstance, elite_msg_any_t* pMsg);

//AV-Sync functions
ADSPResult MxAr_AVSyncSetAFEDriftPtrInputDerivedFromOutput(This_t *me);
ADSPResult MxAr_OutPortToHonorInPortsTimestamps(This_t *me, uint32_t unOutPortID);
ADSPResult MxAr_CommonInPortProcessingRoutinePostSteps(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_InitInputAvSync(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_DeInitInputAvSync(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_InitOutputAvSync(This_t *me, uint32_t unOutPortID);
ADSPResult MtMx_DeInitOutputAvSync(This_t *me, uint32_t unOutPortID);
ADSPResult MtMx_AvSyncRequestHW(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_AvSyncReleaseHW(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_InPortToHonorTimestamp(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_InPortToCheckReInitSampleSlipLibrary(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_InPortSetS2DPtrSampleSlipLibrary(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_InPortResetSampleSlipLibrary(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_CheckSSLibStatus_3(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_CheckSSLibStatus_4(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_HandleSampleAddDrops_1(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_HandleSampleAddDrops_2(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_HandleSampleAddDrops_3(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_ClearHoldDuration(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_ProcessMsgDataBuffer(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_AddInterpolatedSamples(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_DropSamples(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_ResetAvSyncStatsDrift(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_ResetAvSyncSampleStats(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_ResetStrClkSync(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_SetupDefaultRenderWindow(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_SetDefaultRenderWindow(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_SetDefaultRenderWindows(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_CheckIfInportMovesAnyOutportToActiveState(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_SetInOutDriftPtr(This_t *me, uint32_t unInPortID, uint32_t unOutPortID);
ADSPResult MtMx_FillInPortLocalBuf(This_t *me, uint32_t unInPortID, uint32_t numSamplesReqPerCh);
ADSPResult MtMx_MsgDataBufferHold(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort);
ADSPResult MtMx_HoldCommonRoutine(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort);
ADSPResult MtMx_SetInputDriftPtr(This_t *me, uint32_t unInPortID, volatile const afe_drift_info_t *pAfeDriftPtr);
ADSPResult MtMx_SetOutputDriftPtr(This_t *me, uint32_t unOutPortID, volatile const afe_drift_info_t *pAfeDriftPtr);
ADSPResult MtMx_AdjustSessionClk(This_t *me, elite_msg_any_t *pMsg);
ADSPResult MtMx_SetRateMatchingParamHandler(This_t *me, elite_msg_any_t *pMsg);
ADSPResult MxAr_Run(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxRunType *pPayload);
ADSPResult MxAt_Run(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxRunType *pPayload);
ADSPResult MxAr_InPortSetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort);
ADSPResult MxAr_InPortGetParamHandler(This_t *me, elite_msg_any_t *pMsg, EliteMsg_CustomMtMxGetSetParamType *pPayload, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort);
ADSPResult MtMx_UpdateExpectedST(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_ResetSessionTimeClock(This_t *me, uint32_t unInPortID);
ADSPResult MxArUpdateInputPortSessionTime(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort, MatrixOutPortInfoType *pCurrentOutPort, uint32_t ullCurrentDevicePathDelay);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //MATRIX_MIXER_H
