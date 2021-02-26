/**
@file MixerSvc_Util.h
@brief This file declares various utility functions that the
       audio matrix mixer uses.
 */

/*========================================================================
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_Util.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/2010 AAA     Created file.
========================================================================== */
#ifndef MIXER_SVC_UTIL_H
#define MIXER_SVC_UTIL_H

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"
#include "audio_basic_op.h"
#include "audio_basic_op_ext.h"
#include "adsp_private_api_ext.h"
#include "AudDevMgr_PrivateDefs.h"
#include "adsp_asm_data_commands.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*--------------------Defines-----------------------*/

//Bit Width Shift Factors
#define BYTE_INTER_CONV_SHIFT_FACT     (PCM_32BIT_Q_FORMAT - ELITE_32BIT_PCM_Q_FORMAT) /*Q31 - Q27*/
#define BYTE_UPDOWN_CONV_SHIFT_FACT_32 (PCM_32BIT_Q_FORMAT - PCM_16BIT_Q_FORMAT) /*Q31 - Q15 */

//Matrix Gain Macros
#define MT_MX_GAIN_SHIFT_FACT          (-13)
#define MT_MX_GAIN_SHIFT_FACT_ABS      (-MT_MX_GAIN_SHIFT_FACT)
#define MT_MX_UNITY_GAIN               (0x2000)
#define MT_MX_RND_FACTOR_GAIN_SHIFT    (0x00001000L)

/*Rounding Factors for bit width conversion
  _V_ indicates a combined 64 bit round factor for vector operations */
#define MT_MX_RND_FACTOR_Q27_TO_Q15                   (0x00000800L)
#define MT_MX_RND_FACTOR_Q31_TO_Q15                   (0x00008000L)
#define MT_MX_RND_FACTOR_Q31_TO_Q27                   (0x00000008L)
#define MT_MX_RND_FACTOR_V_Q27_TO_Q15                 (0x0000080000000800LL)
#define MT_MX_RND_FACTOR_V_Q31_TO_Q15                 (0x0000800000008000LL)
#define MT_MX_RND_FACTOR_V_Q31_TO_Q27                 (0x0000000800000008LL)
#define MT_MX_RND_FACTOR_V_Q31_TO_Q27_STEREO_MONO_AVG (0x0000001000000010LL)
#define MT_MX_RND_FACTOR_V_Q27_TO_Q15_STEREO_MONO_AVG (0x0000100000001000LL)
#define MT_MX_RND_FACTOR_V_Q31_TO_Q15_STEREO_MONO_AVG (0x0001000000010000LL)

#define AUDIOLOG_MTMX_IN_TAP_ID        (0x0001)
#define MT_MX_INVALID_PORT_ID          (99)

enum bit_conv_mode_t
{
    BIT_CONV_IN_OUT_EQUAL = 0,
    BIT_CONV_IN16_TO_OUT24,
    BIT_CONV_IN24_TO_OUT16,
    BIT_CONV_IN16_TO_OUT32,
    BIT_CONV_IN32_TO_OUT16,
    BIT_CONV_IN24_TO_OUT32,
    BIT_CONV_IN32_TO_OUT24,
    BIT_CONV_IN_OUT_INVALID
};

/*--------------------Functions-----------------------*/
void MtMx_CalculateStreamDelay(This_t *me, uint32_t portID);

inline bool_t MtMx_InportIsLocalBufFull(MatrixInPortInfoType *pCurrentInPort)
{
   return pCurrentInPort->bIsLocalBufFull;
}

inline bool_t MtMx_InportIsLocalBufPartiallyFull(MatrixInPortInfoType *pCurrentInPort)
{
   return (!pCurrentInPort->bIsLocalBufEmpty && !pCurrentInPort->bIsLocalBufFull);
}

inline void MtMx_CalculateTopPriorityOutputPort(This_t *me, uint32_t unInPortID)
{
   me->inPortParams[unInPortID]->unTopPrioOutPort =
         Q6_R_ct0_R(me->inPortParams[unInPortID]->strMask);
 if (ADM_MATRIX_ID_AUDIO_TX == me->mtMxID)
   {
      // Changing the priority of the output port does not change the Tx delay.
   }
   else
   {
      MtMx_CalculateStreamDelay(me, unInPortID);
   }
}

inline void MtMx_ClearInputPortChannelStatus(This_t *me, uint32_t unInPortID)
{
   me->unChannelStatus = me->unChannelStatus &
         ~qurt_elite_queue_get_channel_bit(me->inPortParams[unInPortID]->inPortHandle.portHandle.dataQ);
}

inline void MtMx_AddInputPortToWaitMask(This_t *me, uint32_t unInPortID)
{
   if (!me->inPortParams[unInPortID]->bIsEOSHeld)
   {
      me->unCurrentBitfield = me->unCurrentBitfield |
            qurt_elite_queue_get_channel_bit(me->inPortParams[unInPortID]->inPortHandle.portHandle.dataQ);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "MtMx #%lu: Not adding i/p port [%lu] to waitmask because EOS is held",
            me->mtMxID, unInPortID);
   }
}

inline void MtMx_RemoveInputPortFromWaitMask(This_t *me, uint32_t unInPortID)
{
   me->unCurrentBitfield = me->unCurrentBitfield &
         ~qurt_elite_queue_get_channel_bit(me->inPortParams[unInPortID]->inPortHandle.portHandle.dataQ);
}

inline void MtMx_ClearOutputPortChannelStatus(This_t *me, uint32_t unOutPortID)
{
   me->unChannelStatus = me->unChannelStatus &
         ~qurt_elite_queue_get_channel_bit(me->outPortParams[unOutPortID]->bufQ);
}

inline void MtMx_AddOutputPortToWaitMask(This_t *me, uint32_t unOutPortID)
{
   me->unCurrentBitfield = me->unCurrentBitfield |
         qurt_elite_queue_get_channel_bit(me->outPortParams[unOutPortID]->bufQ);
}

inline void MtMx_RemoveOutputPortFromWaitMask(This_t *me, uint32_t unOutPortID)
{
   me->unCurrentBitfield = me->unCurrentBitfield &
         ~qurt_elite_queue_get_channel_bit(me->outPortParams[unOutPortID]->bufQ);
}

inline bool_t MtMx_BitWidthNativityIsEnabled(uint8_t unNativeModeFlags)
{
   return ((((unNativeModeFlags & ADM_BIT_MASK_NATIVE_MODE) >> ADM_BIT_SHIFT_NATIVE_MODE_BIT_WIDTH) & 1) ? TRUE : FALSE);
}

inline bool_t MtMx_ChannelNativityIsEnabled(uint8_t unNativeModeFlags)
{
   return ((((unNativeModeFlags & ADM_BIT_MASK_NATIVE_MODE) >> ADM_BIT_SHIFT_NATIVE_MODE_CHANNELS) & 1) ? TRUE : FALSE);
}

inline int32_t MtMx_CalculateRoundingFactor(int32_t shift_factor)
{
	if(shift_factor >= 0) /* There is no truncation for the shift left operation */
	{
		return 0;
	}
	return (int32_t)(1 << ((-shift_factor) - 1));
}

typedef struct
{
    uint32_t*    pAddr;                   /**< Physical memory address. This field is NULL. */
    uint16_t     data_payload_size;       /** Common data payload size for entire structure. */
    uint32_t     unModuleId1;             /**< ID of the module. */
    uint32_t     unParamId1;              /**< ID of the parameter. */
    uint16_t     usParamSize1;            /**< Data size of this parameter ID/module ID combination. */
    uint16_t     usReserved1;             /**< Reserved for 32-bit alignment. */
    struct asm_session_mtmx_strtr_param_render_decision_t sMtMxRenderingDecision;
        /**< Parameter is a structure which contains the drift pointer information to be passed on to the COPP. */
    uint32_t     unModuleId2;             /**< ID of the module. */
    uint32_t     unParamId2;              /**< ID of the parameter. */
    uint16_t     usParamSize2;            /**< Data size of this parameter ID/module ID combination. */
    uint16_t     usReserved2;             /**< Reserved for 32-bit alignment. */
    struct asm_session_mtmx_strtr_param_clock_recovery_t sMtMxClkRecoveryMethod;
        /**< Parameter is a structure which contains the sample slip enable information to be passed on to the COPP. */
} mtmx_ratematching_set_params;

void MtMx_InitInPortLocalBufPtrs(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_FillInPortLocalBufHold(This_t *me, uint32_t unInPortID, uint32_t numSamplesReqPerCh);
ADSPResult MxAr_ApplyGainAndAccumulateSamples(This_t *me, uint32_t unInPortID);
ADSPResult MxAt_ApplyGainAndAccumulateSamples(This_t *me, uint32_t unInPortID);
void MtMx_RecfgInPorts(This_t *me, uint32_t unOutPortID);
void MtMx_InPortToUpdateItsActiveOutPortsMask(This_t *me, uint32_t unInPortID);
void MtMx_OutPortToUpdateActiveOutPortsMasks(This_t *me, uint32_t unOutPortID);
void MxAt_OutPortToUpdateOutputReqPendingMasks(This_t *me, uint32_t unOutPortID);
void MtMx_AddOutPortToActiveOutPortsMask(This_t *me, uint32_t unOutPortID);
void MtMx_InPortToUpdateWaitMask(This_t *me, uint32_t unInPortID);
void MtMx_OutPortToUpdateWaitMask(This_t *me, uint32_t unOutPortID);
void MxAr_ProcessPendingPullOutPorts(This_t *me);
void MtMx_SetOPRMaskOnInputPortsConnectedToPendingPullPorts(This_t *me);
void AudDestroyMatrixMixerSvc (This_t* me);
void MxAr_CommonInPortProcessingRoutine(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort);
void MxAt_CommonInPortProcessingRoutine(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort);
void MxAt_NewInPortToUpdateInPortsTopPriorityMaskOfOutPort(This_t *me, uint32_t unInPortID, uint32_t unOutPortID);
void MxAt_ClosingInPortToUpdateInPortsTopPriorityMaskOfOutPort(This_t *me, uint32_t unInPortID);
void MtMx_ClosingInPortToUpdateMtMxMasksOfOutPort(This_t *me, uint32_t unInPortID);
void MxAt_UpdateBurstModeOfMxAt(This_t *me);
ADSPResult MtMx_DuplicateEosMsg( This_t *me, uint32_t unInPortID, uint32_t unOutPortID, elite_msg_any_t *pInMsg, elite_msg_any_t *pOutMsg);
void MtMx_UpdateInPortsMaskToReflectStrMaskChange(This_t *me, uint32_t unInPortID, uint32_t unMode, uint32_t unBitPosition);
void MtMx_UpdateStrMaskToReflectInPortsMaskChange(This_t *me, uint32_t unOutPortID, uint32_t unMode, uint32_t unBitPosition);
void MtMx_ApplyOutputBufferTS(This_t *me, uint32_t unOutPortID, elite_msg_data_buffer_t *pOutputBuf);
ADSPResult MtMx_ReInitLocalBuffer(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_OutportAllocBuffers(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
ADSPResult MtMx_OutportSendMediaTypeMsgDS(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
void MtMx_MoveOutportToActiveState(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
void MtMx_SetupInputToOutputGainParams(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, adm_session_copp_gain_v5_t *pSessionCOPPGain);
void MtMx_SetupInputToOutputMuteParams(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, adm_cmd_matrix_mute_v5_t *pMutePayload);
void MxAt_OutPortToUpdateAccBufAvailabilityMask(This_t *me, uint32_t unOutPortID);
void MtMx_PrintDebugMediaTypeInformationOutputPort(This_t *me, uint32_t unOutPortID);
void MtMx_PrintDebugMediaTypeInformationInputPort(This_t *me, uint32_t unInPortID);
void MxAt_UpdateMediaTypeForNativeModeOutputPort(This_t *me, uint32_t unOutPortID);
void MxAr_UpdateMediaTypeForNativeModeOutputPort(This_t *me, uint32_t unOutPortID);
void MtMx_CheckIfAnyConnOutPortsInNativeModeNeedReInit(This_t *me, uint32_t unInPortID);
ADSPResult MtMx_ReInitNativeModeOutport(This_t *me, uint32_t unOutPortID);
void MxAt_OutPortToCheckForOverflowEvents(This_t *me, uint32_t unOutPortID);
void MtMx_DetectIfPushPortsNeedsToSendData(This_t *me);
void MtMx_DetectIfPullPortsNeedsToSendData(This_t *me);
void MxAt_DetectIfPendingNonLivePortsNeedsToSendData(This_t *me);
void MxAt_DetectIfPendingNonLivePortsNeedsToMarkPendingAccOnOp(This_t *me);
void MtMx_OpPortReCfg_StoreCurrentCfg(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
void MxAr_OpPortReCfg_PostSteps(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
void MxAt_OpPortReCfg_PostSteps(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
ADSPResult MtMx_OpPortReCfg_CheckAndSendMT(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
bool_t MtMx_OpPortReCfg_CheckIfReCfgNeeded(This_t *me, uint32_t unOutPortID, MatrixOutPortInfoType *pCurrentOutPort);
ADSPResult MtMx_LogPcmData(This_t *me, uint32_t unInPortID, int8_t *pCurrentSample);
void MtMx_ForceDisableOutputPortInBadState(This_t *me, uint32_t unOutPortID);
void MtMx_CopyChannelMap(uint8_t *psSrcMap, uint8_t *psDstMap, uint32_t unNumChannels);
bool_t MtMx_CheckIfInputPortCanTransitionAwayFromReady(This_t* me, uint32_t unInPortID);
ADSPResult MtMx_DetectMixedLatencyConnections(This_t *me);
void MtMx_CheckForMixedLatencyAndUpdatePendingBufferReinit(This_t *me, uint16_t unInpPortId, uint16_t unOutPortId);
ADSPResult MtMx_ReInitOutputPortBuffers(This_t *me, uint16_t unOutPortId);
ADSPResult MtMx_ReInitInputPortBuffers(This_t *me, uint16_t unInpPortId);
bool_t MtMx_CheckforMixedConxInConnectedInpOutPorts(This_t *me,uint16_t unInpPortId, uint16_t unOutPortId);
ADSPResult MtMx_SetReqKppsAndBW(This_t *me);
void MtMx_ScanOutputPortsForInputsWithValidTimestamps(This_t *me);
void MtMx_SetOutputBufferTimestamp(This_t *me, uint32_t unOutPortID, elite_msg_data_buffer_t *pOutputBuf);
ADSPResult MtMx_PspdInitChanQ(MatrixInPortInfoType *pCurrentInPort);
bool_t MtMx_PspdSvcCreated(mt_mx_struct_pspd_t *pCurrentPspd);
ADSPResult MtMx_PspdProcCmd(MatrixInPortInfoType *pCurrentInPort, MatrixOutPortInfoType *pCurrentOutPort, mt_mx_struct_pspd_t *pCurrentPspd);
void MtMx_ClosingInPortToCheckDestroyPspdThreads(This_t *me, uint32_t unInPortID);
void MtMx_ClosingOutPortToCheckDestroyPspdThreads(This_t *me, uint32_t unOutPortID);
ADSPResult MtMx_PspdReInitBuffers(This_t *me, uint32_t unInPortID, uint32_t unOutPortID);
void MtMx_OutPortCallPspdReInitBuffers(This_t *me, uint32_t unOutPortID);
ADSPResult MtMx_PspdSendAndWaitForResp(elite_msg_any_t *pMsg, qurt_elite_queue_t *pSendQ, qurt_elite_queue_t *pRecvQ);
void MtMx_ClosingInPortToCheckDestroyPspdLibraries(This_t *me, uint32_t unInPortID);
void MtMx_ClosingOutPortToCheckDestroyPspdLibraries(This_t *me, uint32_t unOutPortID);
void MtMx_InPortToCheckReInitPspdLibraries(This_t *me, uint32_t unInPortID);
void MtMx_OutPortToCheckReInitPspdLibraries(This_t *me, uint32_t unOutPortID);
void MtMx_InOutPortsToCheckReInitPspdLibraries(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, bool_t bOptzCheck);
ADSPResult MtMx_RegisterWithAdsppm(This_t* me);
ADSPResult MtMx_RaiseAdsppmKppsAndBwUpdateEvent(This_t* me, bool_t bKppsVoteNeeded, bool_t bBwVoteNeeded);
void MtMx_DeRegisterFromAdsppm(This_t* me);
void MtMx_SetInPortFrameDuration(This_t *me, uint32_t unInPortID);
void MtMx_SetOutPortFrameDuration(This_t *me, uint32_t unOutPortID);
void MtMx_CalculateFrameDuration(const uint32_t frame_size, const uint32_t sampling_rate, adsp_time *frame_duration);
void MtMx_AdjustBaseValueStcUpdate(This_t *me, uint32_t unInPortID, uint32_t unSampleRate);
ADSPResult MtMx_CheckRaiseRenderedEoSv2(This_t* me, elite_msg_any_t *respMsg, uint32_t unInPortID, uint32_t unOutPortID, bool_t forceRaise);
ADSPResult MtMx_ClearEoSMsg(This_t* me, uint32_t unInPortID, bool_t clearOnlyEoSV2);

static inline uint32_t MtMx_GetClientTokenFromInOutPortId(uint32_t unInPortID, uint32_t unOutPortID)
{
   return ((unInPortID & 0x0000FFFF)<<16) | (unOutPortID & 0x0000FFFF);
}

static inline void MtMx_GetInOutPortIdFromClientToken(uint32_t clientToken, uint32_t *unInPortID, uint32_t *unOutPortID)
{
   *unInPortID  = (clientToken & 0xFFFF0000) >>16;
   *unOutPortID = (clientToken & 0x0000FFFF);
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //MIXER_SVC_UTIL_H
