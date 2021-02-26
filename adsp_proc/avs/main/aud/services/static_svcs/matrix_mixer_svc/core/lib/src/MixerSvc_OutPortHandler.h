/**
@file MixerSvc_OutPortHandler.h
@brief This file declares functions that the audio matrix mixer
       uses to handle the arrival of a buffer at its input
       ports.

*/

/*========================================================================
Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_OutPortHandler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/2010 AAA     Created file.
========================================================================== */
#ifndef MIXER_SVC_OUT_PORT_HANDLER_H
#define MIXER_SVC_OUT_PORT_HANDLER_H

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void MxAr_ProcessBufQ(This_t *me, uint32_t unOutPortID);
void MxAt_ProcessBufQ(This_t *me, uint32_t unOutPortID);
void MxAt_ProcessOutputPortWakeUp(This_t *me, uint32_t unOutPortID);
void MxAt_OutPortToProcessIfNonLiveInputPortsNeedToAcc(This_t *me, uint32_t unOutPortID);
void MtMx_OutPortToCheckForPendingEOS(This_t *me, uint32 unOutPortID);
void MtMx_OutPortToUpdateOutputReqPendingMask(This_t *me, uint32_t unOutPortID);
ADSPResult MtMx_OpPortPopBuffer(This_t *me, uint32_t unOutPortID);
ADSPResult MtMx_OpPortReCfg(This_t *me, uint32_t unOutPortID);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //MIXER_SVC_OUT_PORT_HANDLER_H
