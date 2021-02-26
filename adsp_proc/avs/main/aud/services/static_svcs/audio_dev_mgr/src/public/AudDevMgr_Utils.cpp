/**
@file AudDevMgr_Utils.cpp
@brief This file contains utility functions for ADM implementation.

 */

/*===========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/17/16   HB      Created file.
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudDevMgr.h"
#include "adsp_vpm_api.h"

void Adm_GetInternalTopoID(uint32_t *topoId, uint16_t modeOfOperation)
{
	return;
}

bool_t Adm_IsNativeModeRequired(uint32_t topologyId)
{
    return FALSE;
}

bool_t Adm_IsValidSampleRateForVpTxTopo(uint32_t topologyID, uint32_t sampleRate)
{
    //Check for validity of sampling rate
    //All topologies support only 8 kHz, 16 kHz and 48 kHz
    if ((ADM_SAMPLING_RATE_8K != sampleRate) &&
            (ADM_SAMPLING_RATE_16K != sampleRate) &&
            (ADM_SAMPLING_RATE_48K != sampleRate))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Invalid sampling rate [%lu] "
                "for VPTX topology 0x%lX", sampleRate, topologyID);
        return FALSE;
    }
    return TRUE;
}

ADSPResult Adm_GetChannelConfigForVpTxTopo(uint32_t topologyID, uint16_t *numChannels, uint8_t *chMap)
{
    switch(topologyID)
    {
    case VPM_TX_NONE:
    {
        *numChannels = ADM_NUM_CHANNELS_MONO;
        chMap[0] = PCM_CHANNEL_C;
        break;
    }
    default:
    {
        return ADSP_EUNSUPPORTED;
    }
    }
    return ADSP_EOK;
}
