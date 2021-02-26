/**
@file AudDevMgr.h
@brief This file declares some common data types used by AudioDevMgr.
 */

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/23/12   KR      Non-backward compatible API changes for Badger.  
========================================================================== */
#ifndef AUD_DEV_MGR_H
#define AUD_DEV_MGR_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Elite.h"
#include "qurt_elite.h"
#include "MixerSvc_Interface.h"
#include "AudDynaPPSvc.h"
#include "adsp_asm_session_commands.h"
#include "adsp_afe_service_commands.h"
#include "adsp_lsm_session_commands.h"
#include "MixerSvc.h"
#include "EliteMsg_AfeCustom.h"
#include "StreamRouterSvc.h"
#include "adsp_media_fmt.h"
#include "AudioStreamMgr_PrivateDefs.h"
#include "qurt_elite_adsppm_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* -----------------------------------------------------------------------
 ** Global definitions/forward declarations
 ** ----------------------------------------------------------------------- */

#define AUD_DEV_MGR_CMDQ    "admStatServ"
#define  ADM_MAX_CMD_Q_ELEMENTS  16

#define ADM_MAX_COPPS                 MT_MX_MAX_PORTS
#define ADM_COMPRESSED_MIN            100
#define ADM_COMPRESSED_MAX            113
#define ADM_COMPRESSED_MIN_RX         100
#define ADM_COMPRESSED_MAX_RX         101
#define ADM_COMPRESSED_MIN_TX         110
#define ADM_COMPRESSED_MAX_TX         111

#define ADM_RX_COPP_ID_START          0
#define ADM_RX_COPP_ID_SIZE           MT_MXAR_MAX_OUTPUT_PORTS
#define ADM_RX_COPP_ID_END            (ADM_RX_COPP_ID_START + ADM_RX_COPP_ID_SIZE - 1)

#define ADM_TX_COPP_ID_START          (ADM_RX_COPP_ID_END + 1)
#define ADM_TX_COPP_ID_SIZE           MT_MXAT_MAX_INPUT_PORTS
#define ADM_TX_COPP_ID_END            (ADM_TX_COPP_ID_START + ADM_TX_COPP_ID_SIZE - 1)

#define ADM_SAMPLING_RATE_8K          8000
#define ADM_SAMPLING_RATE_16K         16000
#define ADM_SAMPLING_RATE_32K         32000
#define ADM_SAMPLING_RATE_44100       44100
#define ADM_SAMPLING_RATE_48K         48000
#define ADM_SAMPLING_RATE_88200       88200
#define ADM_SAMPLING_RATE_176400      176400 
#define ADM_SAMPLING_RATE_192K        192000
#define ADM_SAMPLING_RATE_352800      352800
#define ADM_SAMPLING_RATE_384K        384000

#define ADM_SAMPLING_RATE_MAX         ADM_SAMPLING_RATE_384K

#define ADM_SMPLS_PER_MS_8K           8
#define ADM_SMPLS_PER_MS_16K          16
#define ADM_SMPLS_PER_MS_48K          48
#define ADM_SMPLS_PER_MS_192K         192
#define ADM_SMPLS_PER_MS_384K         384


#define ADM_FRAME_SIZE_1MS            1
#define ADM_FRAME_SIZE_5MS            5
#define ADM_FRAME_SIZE_10MS           10

#define ADM_LEGACY_NUM_AFE_FRAMES     5
#define ADM_LL_NUM_AFE_FRAMES         1

#define ADM_NUM_CHANNELS_NULL         0
#define ADM_NUM_CHANNELS_MONO         1
#define ADM_NUM_CHANNELS_STEREO       2
#define ADM_NUM_CHANNELS_EIGHT        8

#define ADM_NUM_CHANNELS_DUAL_MIC     2
#define ADM_NUM_CHANNELS_QUAD_MIC     4

#define ADM_BITS_PER_SAMPLE_16        16
#define ADM_BITS_PER_SAMPLE_24        24
#define ADM_BITS_PER_SAMPLE_32        32

#define ADM_BYTES_PER_SAMPLE_TWO      2
#define ADM_BYTES_PER_SAMPLE_FOUR     4

#define ADM_COPP_STATUS_INACTIVE      0
#define ADM_COPP_STATUS_ACTIVE        1

#define ADM_COPP_TYPE_NONE            0
#define ADM_COPP_TYPE_VPTX            1
#define ADM_COPP_TYPE_AUDIO_COPP      2

#define ADM_COPP_ID_INVALID           0xFFFF
#define ADM_INVALID_DELAY_PTR_ID      0xFFFF
#define ADM_INVALID_PRIMARY_DEVICE    0xFFFF
#define ADM_INVALID_INDEX             0xFFFF
#define ADM_INVALID_DIRECTION         0xFFFF

#define ADM_LOG_ID_BASE_RX            0x0000
#define ADM_LOG_ID_BASE_TX            0x8000
#define ADM_MAX_COPP_OPEN_CTR         0x07FF

#define CACHE_LINE_SIZE               32
#define CACHE_ALIGNMENT_MASK          (CACHE_LINE_SIZE - 1)

#define ADM_NORMAL_BUFFERING_BETWEEN_AFE_AND_CLIENT    2
#define ADM_HIGH_BUFFERING_BETWEEN_AFE_AND_CLIENT      4
#define ADM_NUM_BUFFERS_BETWEEN_MXAR_AND_COPP          2
#define ADM_HIGHEST_BUFFERING_BETWEEN_AFE_AND_CLIENT   21

#define ADM_CLIENT_NUM_ZERO 0
#define ADM_CLIENT_NUM_ONE  1

enum adm_ucsmm_mode_t
{
    ADM_UCSMM_CONNECT = 0, //Update Compressed Session Mapping Mask Connect Session and Device
    ADM_UCSMM_DISCONNECT,  //Update Compressed Session Mapping Mask Disconnect Session and Device
};

enum adm_data_mode_t
{
    ADM_DATA_PCM = 0, //Indicates PCM data
    ADM_DATA_COMPRESSED,  //Indicated Compressed data
};

enum adm_client_type_t
{
    eASM_TYPE = 0, //Indicates ASM Client
    eLSM_TYPE, //Indicates LSM Client
};

static inline bool_t isAlignedToCacheLine(uint32_t addr)
{
    return ((addr & CACHE_ALIGNMENT_MASK) == 0);
}

typedef struct {
    AudPP_MsgCbType   cbFuntion;
    void              *pContext;
} Adm_PPCallbackInfoType;

typedef struct {
    uint16_t unAfePortID;
    uint32_t unSampleRate;
    uint32_t unNumChannels;
    uint8_t  unChannelMap[8];
    uint32_t unBytesPerSample;
    uint32_t unBitWidth;
    elite_svc_handle_t     *phAfePort;
    volatile const afe_drift_info_t *pAfeDriftPtr;
} Adm_EndPointIDInfo_t;

typedef struct AdmCalPort {
    uint32_t                unCoppTopologyID;
    Adm_EndPointIDInfo_t    unEID1;
    Adm_EndPointIDInfo_t    unEID2;
    Adm_EndPointIDInfo_t    unEID3;
    uint16_t                unCoppDirection;
    elite_svc_handle_t     *phDevCopp;
    elite_svc_handle_t     *phDevCoppEcRef;
    union MtMxPortHandle_t
    {
        MtMxInPortHandle_t   *phMtMxInPort;
        MtMxOutPortHandle_t  *phMtMxOutPort;
    }mtMxPort;
    union MtMxPortHandle2_t
    {
        MtMxInPortHandle_t   *phMtMxInPort;
        MtMxOutPortHandle_t  *phMtMxOutPort;
    }mtMxPort2;
    uint16_t                unCoppStatus;
    uint16_t                unCoppType;
    uint16_t                unNumConnectedSessionsRx;
    uint16_t                unNumConnectedSessionsTx;   
    Adm_PPCallbackInfoType  structPPCallbackInfo;
    uint32_t                unMtMxSampleRate;   
    uint32_t        unCoppNumAfeFrames;     //Var to decide on number of AFE frames in the COPP Frame
    uint16_t             unDevicePerfMode;   /**< indicates the performance mode of the Device.
                                                Supported values are ADM_LEGACY_DEVICE_SESSION or
                                                ADM_LOW_LATENCY_DEVICE_SESSION or 
                                                ADM_ULTRA_LOW_LATENCY_DEVICE_SESSION. Default value is
                                                ADM_LEGACY_DEVICE_SESSION */
   uint32_t                unDirectConnSessionID; /**< indicates connected sessionID of COPP used by
                                                       ULL and LLNP sessions. This variable will have valid
                                                       value only if COPP performance mode is
                                                       ADM_ULTRA_LOW_LATENCY_DEVICE_SESSION or
                                                       ADM_LOW_LATENCY_NO_PROC_DEVICE_SESSION.*/
    bool_t                bIsCoppInLoopBackTopology;
    bool_t                bIsCoppConnectedToPseudoPort;
    uint16_t              unPrimaryDevice;   /**< Primary device to this device */
    bool_t                bIsCoppInDeviceLoopBackTopology;
} AdmCalPort_Type;

typedef struct AdmDevSessionDelayParam {
    volatile uint32_t       unCoppBufDelay;    //Connected COPP buffering delay
    volatile uint32_t       unCoppAlgDelay;    //Connected COPP algorithmic delay
    volatile uint32_t       unAFEDelay_EP1;    //Connected AFE port1 delay  (algorithmic + buffering)
    volatile uint32_t       unAFEDelay_EP2;    //Connected AFE port2 delay (algorithmic + buffering)
    volatile uint32_t       unAFEDelay_EP3;    //Connected AFE port3 delay (algorithmic + buffering) 
} AdmDevSessionDelayParam_Type;

typedef struct AdmStreamSessionInfo {
    uint32_t                unSessionSampleRate;
    uint16_t                stream_perf_mode;
    /**< indicates which performance mode this stream is opened. Supported values are:
                                             ASM_LEGACY_STREAM_SESSION,
                                             ASM_LOW_LATENCY_STREAM_SESSION,
                                             ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION,
                                             ASM_ULTRA_LOW_LATENCY_STREAM_SESSION,
                                             ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION. */
} AdmStreamSessionInfo_Type;

typedef struct AdmEndPointParams {
    uint16_t                unNumActiveCopps;
} AdmEndPointParams_Type;

typedef struct ComprCoppInfo {
    uint32_t                unCoppID;
    uint32_t                unCoppTopologyID;
    uint32_t                strtr_port_id;
    uint16_t                afe_port_id;
    elite_svc_handle_t     *phDevCopp;
    uint16_t                unCoppStatus;
    uint16_t                unCoppType;
    Adm_PPCallbackInfoType  structPPCallbackInfo;
    volatile uint32_t       unCoppBufDelay;      //Connected COPP buffering delay
    volatile uint32_t       unCoppAlgDelay;      //Connected COPP algorithmic delay
      uint16_t                         unPrimaryDevice;   /**< Primary device to this device */
      volatile const afe_drift_info_t  *pAfeDriftPtr;
} ComprCoppInfo_t;

struct StreamRouterParams_t
{
    elite_svc_handle_t*      hSvcHandle;
    uint32_t                 num_inp_connections;
    uint32_t                 num_out_connections;
    elite_svc_handle_t*      afe_port_svc_handle[STRTR_MAX_PORTS];
    uint16_t                 port_id_to_session[STRTR_MAX_PORTS];
    uint16_t                 port_id_to_delay_ptr_idx[STRTR_MAX_PORTS];   //required to identify corresponding afe delay pointer
    uint16_t                 session_id_to_delay_ptr_idx[ASM_MAX_SUPPORTED_SESSION_ID];//used to calculaute path delay
    volatile uint32_t        unAFEDelay[STRTR_MAX_PORTS];                 //Connected AFE port delay (algorithmic + buffering)
    volatile uint32_t        unStrRtrDelay[STRTR_MAX_PORTS];              //Stream Router delay (buffering)
    ComprCoppInfo_t          compr_copp_info[STRTR_MAX_PORTS];
    uint32_t                 unLogSeqId;          //Sequence ID for Tx Stream router input logging
};

typedef struct AdmStaticSvc {
    elite_svc_handle_t           hAdmSvc;
    elite_svc_handle_t*          hTxMatrix;
    elite_svc_handle_t*          hRxMatrix;
    CfgOutputPortsPayload_t      cfgOutPortsPayload;
    CfgInputPortsPayload_t       cfgInPortsPayload;
    char                         *cmdQName;
    AdmCalPort_Type              deviceCalPort[ADM_MAX_COPPS];
    AdmDevSessionDelayParam_Type *pDevSessionDelayParam;
    volatile uint32_t            unRxMtMxDelay[ASM_MAX_SUPPORTED_SESSION_ID];    //MtMx delay (buffering)
    volatile uint32_t            unTxMtMxDelay[ASM_MAX_SUPPORTED_SESSION_ID + LSM_MAX_NUM_SESSIONS];    //MtMx delay (buffering)
    uint32_t                     rxsessionMappingMask[ASM_MAX_SUPPORTED_SESSION_ID];
    uint32_t                     txsessionMappingMask[ASM_MAX_SUPPORTED_SESSION_ID + LSM_MAX_NUM_SESSIONS];
    uint16_t                     sessionIDToTxMtMxOutPortIDMap[ASM_MAX_SUPPORTED_SESSION_ID + LSM_MAX_NUM_SESSIONS];
    uint16_t                     sessionIDToRxMtMxInPortIDMap[ASM_MAX_SUPPORTED_SESSION_ID];
    uint16_t                     txMtMxOutPortIDToSessionIDMap[MT_MXAT_MAX_OUTPUT_PORTS];
    uint16_t                     rxMtMxInPortIDToSessionIDMap[MT_MXAR_MAX_INPUT_PORTS];
    uint16_t                     rxSessionIDToPrimCOPPIDMap[ASM_MAX_SUPPORTED_SESSION_ID];
    uint16_t                     txSessionIDToPrimCOPPIDMap[ASM_MAX_SUPPORTED_SESSION_ID + LSM_MAX_NUM_SESSIONS];
    qurt_elite_channel_t         admChannel;
    qurt_elite_queue_t           *admResponseQ;
    elite_apr_handle_t           admAprHandle;
    MtMxOutPortCfgParams_t       admOutPortCfgParams;
    MtMxInPortCfgParams_t        admInPortCfgParams;
    StreamRouterParams_t         rxStRtrParams;
    StreamRouterParams_t         txStRtrParams;
    uint32_t                     unNumRxAudioCoppOpenIssued;
    uint32_t                     unNumTxAudioCoppOpenIssued;
    uint32_t                     unNumVpTxCoppOpenIssued;
    AdmStreamSessionInfo_Type    rxStreamSessionInfo[ASM_MAX_SUPPORTED_SESSION_ID];
    AdmStreamSessionInfo_Type    txStreamSessionInfo[ASM_MAX_SUPPORTED_SESSION_ID + LSM_MAX_NUM_SESSIONS];
    uint32_t                     rxCompressedSessionMappingMask[ASM_MAX_SUPPORTED_SESSION_ID];
    uint32_t                     txCompressedSessionMappingMask[ASM_MAX_SUPPORTED_SESSION_ID];
    uint16_t                     rxSessionIDToPrimCompressedDevIndexMap[ASM_MAX_SUPPORTED_SESSION_ID];
    uint16_t                     txSessionIDToPrimCompressedDevIndexMap[ASM_MAX_SUPPORTED_SESSION_ID];

    //To support multi-stream per session support, the ADM will now have look-up indices for each connection.
    //ADM will abstract this information out from the audio matrix/stream router.
    //ASM will initiate a separate connection/disconnection for every stream even if part of the same session.
    uint16_t                     rxIndexToSessionStreamID[ASM_MAX_SUPPORTED_SESSION_ID];
    uint16_t                     txIndexToSessionStreamID[ASM_MAX_SUPPORTED_SESSION_ID + LSM_MAX_NUM_SESSIONS];

    uint32_t                             ulAdsppmClientId;
    qurt_elite_adsppm_client_t           *adsppmClientPtr;

    uint32_t                    prev_kpps_vote; //KPPS
    uint32_t                    prev_bw_vote; //KBps

} AdmStatSvc_InfoType;

typedef ADSPResult (*SetParamHook)(AdmStatSvc_InfoType *pAudStatAdmSvc, elite_apr_packet_t *pPkt, elite_svc_handle_t *pCoppHandle, uint32_t unCoppID);

/*-----------------------------------------------------------------------
 *Global Function Definations
 * -----------------------------------------------------------------------*/
ADSPResult  AudCreateStatAudDevMgr(uint32_t dummy, void** ppHAfeDevMgr);
int AdmAprTest(void *dummy);
int AdmAprEndTest(void *dummy);
ADSPResult AdmStartTest();
void Adm_ResetSessionMappingMask(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t sessionID, uint32_t matrixID, adm_client_type_t eClient);
void Adm_UpdateSessionMappingMask(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t sessionID, uint16_t unCoppID, uint32_t matrixID, adm_client_type_t eClient);
void Adm_RemoveCoppIDFromSessionMappingMask(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t unCoppID, uint16_t unCoppDirection);
void Adm_ResetCompressedSessionMappingMask(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t sessionID, uint32_t strrtrID, adm_client_type_t eClient);
void Adm_UpdateCompressedSessionMappingMask(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t sessionID, uint16_t unCoppID, uint32_t strRtrID, adm_ucsmm_mode_t mode, adm_client_type_t eClient);
void Adm_RemoveCoppIDFromCompressedSessionMappingMask(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t unCoppID, uint16_t unCoppDirection);
ADSPResult  Adm_CheckCoppValidity(AdmStatSvc_InfoType *pAudStatAdmSvc, uint16_t unCoppID, uint16_t unDirection);
ADSPResult Adm_DetectAndUpdatePrimaryDevicePcm(AdmStatSvc_InfoType *pAdmStatSvc);
ADSPResult Adm_DetectAndUpdatePrimaryDeviceCompressed(AdmStatSvc_InfoType *pAdmStatSvc);
uint16_t Adm_CalculateNextAvlIndex(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t direction, adm_client_type_t eClient);
uint16_t Adm_SessionStreamIDToIndex(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t direction, uint16_t sessionStreamID, adm_client_type_t eClient);
ADSPResult Adm_RegisterAdsppm(AdmStatSvc_InfoType *pAdmStatSvc);
ADSPResult Adm_DeregisterAdsppm(AdmStatSvc_InfoType *pAdmStatSvc);
ADSPResult is_valid_device_sampling_rate(uint32_t sample_rate);
ADSPResult Adm_CreatePPSvc(AdmStatSvc_InfoType *pAdmStatSvc,
                           AdmCalPort_Type *phCalPort,
                           AdmDevSessionDelayParam_Type *pSessionDelayParam,
                           uint32_t unTopology,
                           AudPP_AudProcType svcType,
                           uint32_t unLogID);

bool_t Adm_IsNativeModeRequired(uint32_t topologyId);
bool_t Adm_IsValidSampleRateForVpTxTopo(uint32_t topologyID, uint32_t sampleRate);
ADSPResult Adm_GetChannelConfigForVpTxTopo(uint32_t topologyID, uint16_t *numChannels, uint8_t *chMap);
                           
void Adm_ResetCalPort(AdmCalPort_Type *phCalPort);

void Adm_GetInternalTopoID(uint32_t *topoId, uint16_t modeOfOperation);

void Adm_InitCalPort(AdmCalPort_Type *phCalPort, void *pDevOpenCmdPayload, const uint32_t opCode);                         

/**
  Gets the device_perf_mode flag in the "flags" field of ADM_CMD_MULTI_CHANNEL_COPP_OPEN_V(>=3) commands.

  @param[in] flags     Bitfield for the flag.

  @return
  ADM_LEGACY_DEVICE_SESSION -- device copp will be opened in legacy mode\n
  ADM_LOW_LATENCY_DEVICE_SESSION -- device copp will be opened in low latency mode.
  Other values -- reserved for future use

  @dependencies
  None.
 */
static __inline uint32_t adm_get_device_perf_mode_flag_in_multi_ch_copp_open_cmd (uint32_t flags)
{
    uint32_t bit_mask = ADM_BIT_MASK_DEVICE_PERF_MODE_FLAG;
    uint16_t shift = ADM_BIT_SHIFT_DEVICE_PERF_MODE_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the device_perf_mode_flag in the flags field of ADM_CMD_MULTI_CHANNEL_COPP_OPEN_V(>=3) commands.

  @param[in,out] flags_ptr                Pointer to the mode_flags field.
  @param[in]     device_perf_mode_flag    device_perf_mode_flag:\n
                                            - refer to return values of
                                              adm_get_device_perf_mode_flag_in_multi_ch_copp_open_cmd().


  @return
  None.

  @dependencies
  None.
 */
static __inline void adm_set_device_perf_mode_flag_in_multi_ch_copp_open_cmd (uint32_t* flags_ptr, uint32_t device_perf_mode_flag)
{
    uint32_t bit_mask = ADM_BIT_MASK_DEVICE_PERF_MODE_FLAG;
    uint16_t shift = ADM_BIT_SHIFT_DEVICE_PERF_MODE_FLAG;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( device_perf_mode_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}


#ifdef __cplusplus

}
#endif //__cplusplus

#endif //#ifndef AUD_DEV_MGR_H
