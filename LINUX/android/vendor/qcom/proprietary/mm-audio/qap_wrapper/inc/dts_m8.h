/*============================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/


#ifndef DTS_M8_H
#define DTS_M8_H

#include "qap_api.h"
#include "qap_defs.h"
#include "qap_module.h"

#if __cplusplus
extern "C" {
#endif

/* Add New Params for dts */
typedef enum {
    DTS_SESSION_CFG_AD_MIXING_ENABLED,                /* AD mixing on/off */
    DTS_SESSION_CFG_AD_MIXING_USER_PREF,              /* AD mixing user preference */
    DTS_SESSION_CFG_SYSSOUND_MIXING_ENABLED,          /* System sound mixing on/off */
} dts_m8_session_param_t;

/* TODO: Add params related dap for DTS */
typedef enum {
    DTS_DAP_CFG_CAL_BOOST,            /* DAP Calibration Boost */
} dts_ms8_dap_param_t;

/* Dts m8 metadata struct */
typedef struct dts_m8_gain_metadata {
    bool is_metadata_enable;
    bool is_primary_audio_gain_valid;
    bool is_secondary_pann_gain_valid;
    bool is_post_mix_gain_valid;

    uint32_t primary_audio_channel_mask;
    uint32_t primary_audio_gain[DTS_MAX_CHANNELS]; //Q15
    uint32_t secondary_pann_gain[DTS_MAX_CHANNELS]; //Q15
    uint32_t post_mix_gain; //Q20
} dts_m8_gain_metadata_t;

/* TODO: Define metadata struct as per DTS */
#if __cplusplus
}  // extern "C"
#endif

#endif //DTS_M8_H
