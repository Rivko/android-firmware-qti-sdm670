/**
@file MixerSvc_MsgHandlers.h
@brief This file declares functions that the audio matrix mixer
       uses to handle various control commands it receives.

*/

/*========================================================================
Copyright (c) 2010-2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_MsgHandlers.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/2010 AAA     Created file.
========================================================================== */
#ifndef MIXER_SVC_MSG_HANDLERS_H
#define MIXER_SVC_MSG_HANDLERS_H

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

ADSPResult MtMx_CfgInputPorts(This_t *me, elite_msg_any_t* pMsg);
ADSPResult MtMx_CfgOutputPorts(This_t *me, elite_msg_any_t* pMsg);
ADSPResult MtMx_Pause(This_t *me, elite_msg_any_t* pMsg);
ADSPResult MtMx_Run(This_t *me, elite_msg_any_t* pMsg);
ADSPResult MtMx_Flush(This_t *me, elite_msg_any_t* pMsg);
ADSPResult MtMx_Suspend(This_t *me, elite_msg_any_t* pMsg);
ADSPResult MtMx_DestroyYourself(void* pInstance, elite_msg_any_t* pMsg);
ADSPResult MtMx_MapRoutings(This_t *me, elite_msg_any_t* pMsg);
ADSPResult MtMx_MsgMediaType(This_t *me, uint32_t unInPortID, elite_msg_any_t myDataQMsg);
void MtMx_MsgEos(This_t *me, uint32_t unInPortID, elite_msg_any_t myDataQMsg);
void MtMx_ProcessMarkBuffer(This_t *me, uint32_t unInPortID, elite_msg_any_t myDataQMsg, uint16_t status);
ADSPResult MtMx_RegisterUnderflowOverflowEvent(This_t *me, elite_msg_any_t *pMsg);
ADSPResult MtMx_RampGains(This_t *me, elite_msg_any_t *pMsg);
ADSPResult MtMx_Mute(This_t *me, elite_msg_any_t *pMsg);
ADSPResult MtMx_PortStateChange(This_t *me, elite_msg_any_t* pMsg);
ADSPResult MtMx_SetParamHandler(This_t *me, elite_msg_any_t *pMsg);
ADSPResult MtMx_GetParamHandler(This_t *me, elite_msg_any_t *pMsg);
void MtMx_RunOutputPort(This_t *me, uint32_t unPort2Run);
ADSPResult MtMx_SetPrimaryOutputPort(This_t *me, elite_msg_any_t *pMsg);
ADSPResult MtMx_SetPsPdParamHandler(This_t *me, elite_msg_any_t *pMsg);
void MtMx_MsgResync(This_t *me, uint32_t unInPortID, elite_msg_any_t myDataQMsg);
ADSPResult MtMx_SetOutputPortParamHandler(This_t *me, elite_msg_any_t *pMsg);
ADSPResult MtMx_GetOutputPortParamHandler(This_t *me, elite_msg_any_t *pMsg);

void MtMx_SetLimiterModeOnOutputPorts(This_t *me);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //MIXER_SVC_MSG_HANDLERS_H
