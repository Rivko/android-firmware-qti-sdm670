/**
@file MixerSvc_ChanMixUtils.h
@brief This file declares channel mixer utility functions that 
       the audio matrix mixer uses.
*/

/*===========================================================================
Copyright (c) 2013-2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_ChanMixUtils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/10/2014  kr      Re-factoring, De-coupling PSPD and ChannelMixer.
11/25/2013 rkc     Created file.
========================================================================== */

#ifndef MIXER_SVC_CHANMIX_UTILS_H
#define MIXER_SVC_CHANMIX_UTILS_H

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"
#include "audio_basic_op.h"
#include "audio_basic_op_ext.h"
#include "MixerSvc_Util.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void MtMx_InOutPortsToCheckReInitChannelMixerLibrary(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, bool_t bOptzCheck);
void MtMx_DestroyPspdChannelMixer(This_t *me, uint32_t unInPortID, uint32_t unOutPortID);
ADSPResult MtMx_SetChannelMixerCfg(This_t *me, uint32_t unInPortID, uint32_t unOutPortID);
ADSPResult MtMx_SetChannelMixerCoef(This_t *me, uint32_t unInPortID, uint32_t unOutPortID, audproc_chmixer_param_id_coeff_t *pCoefPayload);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //MIXER_SVC_CHANMIX_UTILS_H

