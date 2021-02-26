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
08/17/16   HB      Created file for proprietary information.
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "ds1_calib.h"
#include "dts_hpx_calib.h"
#include "adsp_core_api.h"
#include "adsp_vpm_api.h"
#include "AudDevMgr.h"
#include "EliteTopology_db_if.h"
#include "EliteCmnTopology_db.h"
#include "AudDevMgr_PrivateDefs.h"

void Adm_GetInternalTopoID(uint32_t *topoId, uint16_t modeOfOperation)
{
    if((ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP == *topoId) &&
         ((ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_RX_PATH_COPP == modeOfOperation) ||
           (ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_LOOPBACK_COPP == modeOfOperation)||
           (ADM_CMD_DEVICE_OPEN_MODE_OF_OPERATION_DEVICE_LOOPBACK == modeOfOperation)))
    {
    	*topoId = ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_RX;
    }

    if((ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP == *topoId) &&
         ((ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_TX_PATH_LIVE_COPP == modeOfOperation) ||
           (ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_TX_PATH_NON_LIVE_COPP == modeOfOperation)))
    {
    	*topoId = ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_TX;
    }
}

bool_t Adm_IsNativeModeRequired(uint32_t topologyId)
{
    ADSPResult result1 = ADSP_EOK, result2 = ADSP_EOK;

    // Operate Matrix mixer in native mode if DS1AP module is present in the topology
    result1 = topo_db_is_module_present(TOPO_DB_CLIENT_TYPE_COPP, topologyId, AUDPROC_MODULE_ID_DS1AP);
    result2 = elite_cmn_topo_db_is_module_present(1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT, topologyId, AUDPROC_MODULE_ID_DS1AP);
    if(ADSP_EOK == result1 || ADSP_EOK == result2)
    {
        return TRUE;
    }

    // Operate Matrix mixer in native mode if AudioSphere module is present in the topology
    result1 = topo_db_is_module_present(TOPO_DB_CLIENT_TYPE_COPP, topologyId, AUDPROC_MODULE_ID_AUDIOSPHERE);
    result2 = elite_cmn_topo_db_is_module_present(1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT, topologyId, AUDPROC_MODULE_ID_AUDIOSPHERE);
    if(ADSP_EOK == result1 || ADSP_EOK == result2)
    {
        return TRUE;
    }

    // Operate Matrix mixer in native mode if DTS HPX module is present in the topology
    result1 = topo_db_is_module_present(TOPO_DB_CLIENT_TYPE_COPP, topologyId, AUDPROC_MODULE_ID_DTS_HPX_POSTMIX);
    result2 = elite_cmn_topo_db_is_module_present(1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT, topologyId, AUDPROC_MODULE_ID_DTS_HPX_POSTMIX);
    if(ADSP_EOK == result1 || ADSP_EOK == result2)
    {
        return TRUE;
    }
    return FALSE;
}

bool_t Adm_IsValidSampleRateForVpTxTopo(uint32_t topologyID, uint32_t sampleRate)
{
    //Check for validity of sampling rate
    if((VPM_TX_SM_ECNS == topologyID) ||
            (VPM_TX_DM_FLUENCE == topologyID) ||
            (VPM_TX_DM_VPECNS == topologyID))
    {
        //VPM_TX_SM_ECNS, VPM_TX_DM_FLUENCE and VPM_TX_VPECNS support only 8 kHz and 16 kHz
        if ((ADM_SAMPLING_RATE_8K != sampleRate) &&
                (ADM_SAMPLING_RATE_16K != sampleRate))
        {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Invalid sampling rate [%lu] "
                    "for VPTX topology 0x%lX", sampleRate, topologyID);
            return FALSE;
        }
    }
    else
    {
        //All other topologies support only 8 kHz, 16 kHz, 32kHz and 48 kHz
        if ((ADM_SAMPLING_RATE_8K != sampleRate) &&
                (ADM_SAMPLING_RATE_16K != sampleRate) &&
				(ADM_SAMPLING_RATE_32K != sampleRate) &&
                (ADM_SAMPLING_RATE_48K != sampleRate))
        {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Invalid sampling rate [%lu] "
                    "for VPTX topology 0x%lX", sampleRate, topologyID);
            return FALSE;
        }
    }
    return TRUE;
}

ADSPResult Adm_GetChannelConfigForVpTxTopo(uint32_t topologyID, uint16_t *numChannels, uint8_t *chMap)
{
    switch(topologyID)
    {
    case VPM_TX_NONE:
    case VPM_TX_SM_ECNS:
    case VPM_TX_SM_ECNS_V2:
    case VPM_TX_SM_FLUENCEV5:
    case VPM_TX_CUSTOM_SM_ECNS_1:
    case VPM_TX_CUSTOM_SM_ECNS_2:
    case VPM_TX_CUSTOM_SM_ECNS_3:
    {
        *numChannels = ADM_NUM_CHANNELS_MONO;
        chMap[0] = PCM_CHANNEL_C;
        break;
    }
    case VPM_TX_DM_FLUENCE:
    case VPM_TX_DM_FLUENCEV5:
    case VPM_TX_DM_VPECNS:
    case VPM_TX_DM_FLUENCEV5_BROADSIDE:
    case VPM_TX_CUSTOM_DM_ECNS_1:
    case VPM_TX_CUSTOM_DM_ECNS_2:
    case VPM_TX_CUSTOM_DM_ECNS_3:
    {
        *numChannels = ADM_NUM_CHANNELS_DUAL_MIC;
        chMap[0] = PCM_CHANNEL_L;
        chMap[1] = PCM_CHANNEL_R;
        break;
    }
    case VPM_TX_QM_FLUENCE_PROV2:
    case VPM_TX_CUSTOM_QM_ECNS_1:
    case VPM_TX_CUSTOM_QM_ECNS_2:
    case VPM_TX_CUSTOM_QM_ECNS_3:
    {
        *numChannels = ADM_NUM_CHANNELS_QUAD_MIC;
        chMap[0] = PCM_CHANNEL_L;
        chMap[1] = PCM_CHANNEL_R;
        chMap[2] = PCM_CHANNEL_C;
        chMap[3] = PCM_CHANNEL_LS;
        break;
    }
    default:
    {
        return ADSP_EUNSUPPORTED;
    }
    }
    return ADSP_EOK;
}
