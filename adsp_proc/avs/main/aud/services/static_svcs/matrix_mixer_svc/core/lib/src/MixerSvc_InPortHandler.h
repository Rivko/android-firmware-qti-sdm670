/**
@file MixerSvc_InPortHandler.h
@brief This file declares functions that the audio matrix mixer
       uses to handle the arrival of a buffer at its input
       ports.

*/

/*========================================================================
Copyright (c) 2010-2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_InPortHandler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/2010 AAA     Created file.
========================================================================== */
#ifndef MIXER_SVC_IN_PORT_HANDLER_H
#define MIXER_SVC_IN_PORT_HANDLER_H

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void MtMx_ProcessDataQ(This_t *me, uint32_t unInPortID);
void MtMx_MsgDataBuffer(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort);
void MtMx_InPortUpdateMediaFmtParams(This_t *me, uint32_t unInPortID, uint32_t unSampleRate, uint32_t nNumChannels, uint32_t unBytesPerSample, uint32_t unBitwidth);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //MIXER_SVC_IN_PORT_HANDLER_H
