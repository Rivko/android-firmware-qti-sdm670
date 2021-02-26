/*============================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/


#ifndef DOLBY_MS12_H
#define DOLBY_MS12_H

#include "qap_api.h"
#include "qap_defs.h"
#include "qap_module.h"

#if __cplusplus
extern "C" {
#endif

typedef enum {
    MS12_SESSION_CFG_MANUAL_MIXER_CTL, /* Manual Mixer Mode */
    MS12_SESSION_CFG_STANDALONE_EXEC,  /* IIDK mode */
    MS12_SESSION_CFG_DAP_ENABLE,       /* DAP enable configuration */
    MS12_SESSION_CFG_DAP_ENABLE_SPEAKER,   /* DAP enable speaker configuration
                                            - 0: Disable
                                            - 1: Enable */
    MS12_SESSION_CFG_DAP_ENABLE_HEADPHONE, /* DAP enable headphone configuration
                                            - 0: Disable
                                            - 1: Enable */
    MS12_SESSION_CFG_DAP_VB_ENABLE,    /* DAPv2 Virtual Bass enable (additional delay)
                                            - 0: DAPv2 Virtual Bass is not enabled (default)
                                            - 1: DAPv2 Virtual Bass is enabled */
    MS12_SESSION_CFG_DIALOG_NORM,      /* Dialog Normalization config
                                            - Range: 0-127
                                            - Default: 108(-27dB) */
    MS12_SESSION_CFG_COMPR_PROF,       /* [pcm] Compressor profile
                                            - 0: clipping protection only
                                            - 1: film-standard, default
                                            - 2: film-light
                                            - 3: music-standard
                                            - 4: music-light
                                            - 5: speech */
    MS12_SESSION_CFG_AC4_MODE,         /* AC4 decoding mode
                                            - 0: Single input stream, single decode
                                            - 1: Single input stream, dual decode, single decoder instance
                                                 (decoder internal mixing)
                                                 NOTE: This is an experimental mode, and not tested!
                                            - 2: Single input stream, dual decode, dual decoder instance
                                                 (decoder external mixing, default for single input)
                                            - 3: Dual input stream, dual decode (default for dual input) */
    MS12_SESSION_CFG_PCM_DMX_OUTPUT_ACTIVE,            /* Downmix output active */
    MS12_SESSION_CFG_PCM_DMX_OUTPUT_DUAL_MONO_MODE,    /* Downmix output dual mono reproduction */
    MS12_SESSION_CFG_PCM_DMX_OUTPUT_COMPRESSION_MODE,  /* Downmix output compression mode */
    MS12_SESSION_CFG_PCM_DMX_OUTPUT_DMX_TYPE,          /* Downmix output downmix type */
    MS12_SESSION_CFG_PCM_DMX_OUTPUT_DRC_CUT,           /* Downmix DRC cut factor */
    MS12_SESSION_CFG_PCM_DMX_OUTPUT_DRC_BOOST,         /* Downmix DRC boost factor */
    MS12_SESSION_CFG_PCM_MC_OUTPUT_ACTIVE,             /* Multichannel output active */
    MS12_SESSION_CFG_PCM_MC_OUTPUT_DUAL_MONO_MODE,     /* Multichannel output dual mono reproduction */
    MS12_SESSION_CFG_PCM_MC_OUTPUT_COMPRESSION_MODE,   /* Multichannel output compression mode */
    MS12_SESSION_CFG_PCM_MC_OUTPUT_DRC_CUT,            /* Multichannel DRC cut factor */
    MS12_SESSION_CFG_PCM_MC_OUTPUT_DRC_BOOST,          /* Multichannel DRC boost factor */
    MS12_SESSION_CFG_PCM_MC_OUTPUT_5_1_DMX,            /* Multichannel 7.1 to 5.1 downmix flag */
    MS12_SESSION_CFG_DDP_SUBSTREAM_SELECT,             /* DD+ substream selection */
    MS12_SESSION_CFG_AD_MIXING_ENABLED,                /* AD mixing on/off */
    MS12_SESSION_CFG_AD_MIXING_USER_PREF,              /* AD mixing user preference */
    MS12_SESSION_CFG_SYSSOUND_MIXING_ENABLED,          /* System sound mixing on/off */
} dlb_ms12_session_param_t;

typedef enum {
    MS12_DAP_CFG_CAL_BOOST,            /* DAP Calibration Boost
                                            - Range  : 0 - 192
                                            - default: 0 */
    MS12_DAP_CFG_DMX,                  /* DAP Downmix Mode
                                            - 0: Lt/Rt (Default)
                                            - 1: Lo/Ro */
    MS12_DAP_CFG_GAINS,                /* DAP Gains - postgains
                                            - Range  : -2080...480
                                            - Default: 0 */
    MS12_DAP_CFG_SUR_DEC_ENAB,         /* Surround Decoder
                                            - 0: disabled
                                            - 1: enabled (default) */
    MS12_DAP_CFG_SUR_VIRT,             /* Surround Virtualizer
                                            - virtualizer_enable (0,1, default 1)
                                            - headphone_reverb (-2080...192, default 0)
                                            - speaker_angle (5..30, default 10)
                                            - speaker_start (20...2000, default 20)
                                            - surround_boost (0...96, default 96) */
    MS12_DAP_CFG_GR_EQ,                /* Graphical Equalizer
                                            - eq_enable (0,1, default 0)
                                            - eq_nb_bands (1...20, default 10)
                                            - eq_band_center (20...20000, default {32, 64, 125, 250,500, 1000, 2000, 4000, 8000, 16000})
                                            - eq_band_target (-576...576, default {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} */
    MS12_DAP_CFG_OPTIMIZER,            /* Optimizer
                                            - optimizer_enable (0,1, default 0)
                                            - opt_nb_bands (1...20, default 10)
                                            - opt_band_center_freq (20...20000,
                                                  default {32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000})
                                            - opt_band_gains[MAX_CHANNELS] (-480...480,
                                                  default {{10*0, 10*0, 10*0, 10*0, 10*0, 10*0, 10*0, 10*0})
                                            - (8 maximum channels assumed) */
    MS12_DAP_CFG_BASS_ENH,             /* Bass Enhancer
                                            - bass_enable (0,1, default 0)
                                            - bass_boost (0...384, default 192)
                                            - bass_cutoff (20...20000, default 200)
                                            - bass_width (2..64, default 16) */
    MS12_DAP_CFG_REG,                  /* DAP Regulator
                                            - regulator_enable (0,1, default 1)
                                            - regulator_mode (0,1, default 0)
                                            - regulator_overdrive (0...192, default 0)
                                            - regulator_timbre (0...16, default 16)
                                            - regulator_distortion (0...144, default 96)
                                            - reg_nb_bands (1...20, default 2)
                                            - reg_band_center (20...20000, default {20,20000}
                                            - reg_low_thresholds (-2080...0, default {-192, -192})
                                            - reg_high_thresholds (-2080...0, default {0, 0})
                                            - reg_isolated_bands (0,1 default {0,0}) */
    MS12_DAP_CFG_VB,                   /* Virtual Bass
                                            - virtual_bass_mode (0...3, default 0)
                                            - virtual_bass_low_src_freq (30...90, default 35)
                                            - virtual_bass_high_src_freq (90...270, default 160)
                                            - virtual_bass_overall_gain(-480...0, default 0)
                                            - virtual_bass_slope_gain (-3...0, default 0)
                                            - virtual_bass_subgains[3] (-480...0, default {-32,-144,-192})
                                            - virtual_bass_low_mix_freq (0...375, default 94)
                                            - virtual_bass_high_mix_freq (281...938, default 469) */
    MS12_DAP_CFG_MI_STEER,             /* Media Intelligence Steering
                                            - mi_ieq_enable (0,1, default 0)
                                            - mi_dv_enable (0,1, default 0)
                                            - mi_de_enable (0,1, default 0)
                                            - mi_surround_enable (0,1, default 0) */
    MS12_DAP_CFG_LEVELER,              /* DAP Volume Leveler
                                            - leveler_enable (0,1, default 0)
                                            - leveler_amount (0...10, default 7)
                                            - leveler_ignore_il (0, 1, default 0) */
    MS12_DAP_CFG_IEQ,                  /* DAP Intelligent Equalizer
                                            - ieq_enable (0,1, default 0)
                                            - ieq_amount (0...16, default 10)
                                            - ieq_nb_bands (1...20, default 10)
                                            - ieq_band_center (20...20000,
                                                 default {32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000})
                                            - ieq_band_target (-480...480, default {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} */
    MS12_DAP_CFG_DIAG_ENH,             /* Dialog Enhancer
                                            - de_enable (0,1, default 0)
                                            - de_amount (0...16, default 0)
                                            - de_ducking (0...16, default 0) */
    MS12_DAP_CFG_DRC,                  /* DAP Dynamic Range Compression Mode (for multichannel and DAP output)
                                            - 0: Line (Default)
                                            - 1: RF */
} dlb_ms12_dap_param_t;

typedef struct dlb_mixing_metadata {
    bool mixmdate;        /* This parameter is set to 1 when mixing
                             metadata is present in the frame.*/

    /* Gain values for all the primary channels */
    bool extpgmscle;      /* This parameter is set to 1 when scaling
                             params for all the primary channels are present */
    uint32_t extpgmscl;   /* This parameter is used to control the level of
                             all channels of the primary audio.
                             Valid Range: 0 (mute), 1-63 (-50 dB to +12dB, in 1 dB steps).*/
    uint8_t mixdef;       /* This field shall be set to 11b if gain control of individual
                             primary channel is required, else 00b */

    /* Individual Primary channel Gain values */
    bool extpgmlscle;     /* This parameter is set to 1 when scaling params for
                             L primary audio channel is present */
    uint8_t extpgmlscl;   /* This parameter is used to control the level of
                             L primary audio channel
                             Valid Range: 0-15 (15 =  mute).*/
    bool extpgmcscle;     /* This parameter is set to 1 when scaling params for
                             C primary audio channel is present */
    uint8_t extpgmcscl;   /* This parameter is used to control the level of
                             C primary audio channel
                             Valid Range: 0-15 (15 =  mute).*/
    bool extpgmrscle;     /* This parameter is set to 1 when scaling params for
                             R primary audio channel is present */
    uint8_t extpgmrscl;   /* This parameter is used to control the level of
                             R primary audio channel
                             Valid Range: 0-15 (15 =  mute).*/
    bool extpgmlsscle;    /* This parameter is set to 1 when scaling params for
                             Ls primary audio channel is present */
    uint8_t extpgmlsscl;  /* This parameter is used to control the level of
                             Ls primary audio channel
                             Valid Range: 0-15 (15 =  mute).*/
    bool extpgmrsscle;    /* This parameter is set to 1 when scaling params for
                             Rs primary audio channel is present */
    uint8_t extpgmrsscl;  /* This parameter is used to control the level of
                             Rs primary audio channel
                             Valid Range: 0-15 (15 =  mute).*/
    bool extpgmlfescle;   /* This parameter is set to 1 when scaling params for
                             LFE primary audio channel is present */
    uint8_t extpgmlfescl; /* This parameter is used to control the level of
                             LFE primary audio channel
                             Valid Range: 0-15 (15 =  mute).*/

    /* Scaling params when primary is downmixed */
    bool dmixscle;        /* This parameter is set to 1 when scaling params for
                             primary audio channels prior to downmixing */
    uint8_t dmixscl;      /* When the primary audio decoder is configured to output a two-channel
                             downmix of a primary audio stream that contains more than two channels,
                             it is no longer possible to control the level of individual primary
                             audio channels. This parameter allows additional gain to be applied to
                             the primary audio when it has been downmixed to two channels.The total
                             gain applied to the two-channel downmix is equal to the sum of the gain
                             indicated by this parameter and the gain indicated by the extpgmscl parameter
                             Valid Range: 0-15 (15 =  mute).*/
    bool addche;          /* This parameter is set to 1 when gain metadata to control the level of the
                             sixth and seventh full-range primary audio channels is present in the frame.*/

    /* Scaling params for Auxiliary Channels */
    bool extpgmaux1scle;  /* This parameter is set to 1 when scaling params for
                             sixth full-range channel primary audio channel is present */
    uint8_t extpgmaux1scl;/* This parameter is used to control the level of
                             sixth full-range channel primary audio channel
                             Valid Range: 0-15 (15 =  mute).*/
    bool extpgmaux2scle;  /* This parameter is set to 1 when scaling params for
                             sixth full-range channel primary audio channel is present */
    uint8_t extpgmaux2scl;/* This parameter is used to control the level of
                             sixth full-range channel primary audio channel
                             Valid Range: 0-15 (15 =  mute).*/

    /* Panning params */
    bool paninfoe;       /* This parameter is set to 1 when panning metadata is present in the stream. */
    uint32_t panmean;    /* This parameter is used to pan a mono secondary audio stream within the
                            soundfield of the primary audio. A value of 0 degrees indicates that the
                            secondary audio is panned to the C speaker position.
                            Valid Range: 0.239 (0 to 358.5 degrees) */
} dlb_mixing_metadata_t;
#if __cplusplus
}  // extern "C"
#endif

#endif //DOLBY_MS12_H
