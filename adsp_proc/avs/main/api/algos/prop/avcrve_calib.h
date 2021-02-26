#ifndef AVCRVE_CALIB_H
#define AVCRVE_CALIB_H
/*==============================================================================
  @file avcrve_calib.h
  @brief This file contains AVCRVE
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/28/17   shrani  Added h2xml comments.
 ========================================================================== */
#include "mmdefs.h"


/** @h2xml_title1           {VOICE_MODULE_AVCRVE}
    @h2xml_title_agile_rev  {VOICE_MODULE_AVCRVE}
    @h2xml_title_date       {March 13, 2017} */


#define VOICE_PARAM_AVC_RVE_ENABLE 0x00010E15

#include "adsp_begin_pack.h"

typedef struct voice_param_avc_rve_enable_t voice_param_avc_rve_enable_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_AVC_RVE_ENABLE", VOICE_PARAM_AVC_RVE_ENABLE}
    @h2xmlp_description {Enables the AVC/RVE algorithms.\n
Fixed size for this parameter\n
Version 0 – 4 bytes\n\n } */

struct voice_param_avc_rve_enable_t {

    int16_t ModeWord;
    /**< @h2xmle_description {Mode word to decide between RVE/AVC algorithms\n
1 – AVC is enabled\n
2 – RVE is enabled\n
All other values – Both AVC/RVE are disabled\n}
         @h2xmle_default     {0}
		 @h2xmle_policy       {Basic} */

    int16_t Reserved;
    /**< @h2xmle_description {Must be set to 0}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0x0..0x0}
         @h2xmle_readOnly       {true} */

}
#include "adsp_end_pack.h"
;

#define VOICE_PARAM_AVC_RVE 0x00010E06

#include "adsp_begin_pack.h"

typedef struct voice_param_avc_rve_t voice_param_avc_rve_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_AVC_RVE", VOICE_PARAM_AVC_RVE}
    @h2xmlp_description {AVC/RVE algorithm. \n
Fixed size for this parameter  \n
Version 0 – 148 bytes \n\n } */

struct voice_param_avc_rve_t {

    int16_t FlinkThresholdList [24];
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x3815}
         @h2xmle_range       {0x0..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t FlinkTargetGainList [12];
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x1200}
         @h2xmle_range       {0x0..0x1800}

         @h2xmle_policy       {Basic} */

    int32_t RlinkBNEMin;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0xA0}
         @h2xmle_range       {0x0000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int32_t RlinkBNEMax;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0xF5E245}
         @h2xmle_range       {0x00000000..0x7FFFFFFF}

         @h2xmle_policy       {Basic} */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0..32767}

         @h2xmle_readOnly       {true} */

    int16_t RlinkSensitivityOffset;
    /**< @h2xmle_description {Forward-link AVC variation from nominal sensitivity. This offset is applied to the background noise estimate before comparing against the thresholds. Increasing this parameter makes the AVC more sensitive to background noise.}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0x0000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t FlinkHeadroom;
    /**< @h2xmle_description {Forward-link AVC headroom. This is the maximum total output gain below the threshold that is allowed.}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0x0000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t FlinkSmoothK;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0393}
         @h2xmle_range       {0x0001..0x7FFF}

         @h2xmle_policy       {Basic} */

    uint16_t RlinkBNERamp;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x4666}
         @h2xmle_range       {0x4000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaNoise;
    /**< @h2xmle_description {Smoothing for noise bands}
         @h2xmle_default     {0x0CCC}
         @h2xmle_range       {0..4767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaRec;
    /**< @h2xmle_description {Smoothing for primary channel noise estimate}
         @h2xmle_default     {0x02FF}
         @h2xmle_range       {0..2767}

         @h2xmle_policy       {Basic} */

    int16_t scaleFact_1;
    /**< @h2xmle_description {Factor for scaling single channel estimate bands greater than min}
         @h2xmle_default     {0x2000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t scaleFact_2;
    /**< @h2xmle_description {Factor for scaling second channel noise estimate}
         @h2xmle_default     {0x2000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t opMode;
    /**< @h2xmle_description {Indicates if system is run in Single mic mode (opmode = 1) or Dual mic mode (opmode = 2)}
         @h2xmle_default     {0x02}
         @h2xmle_range       {1..2}

         @h2xmle_policy       {Basic} */

    int16_t WNRThreshold;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x7FFF}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaRef;
    /**< @h2xmle_description {Smoothing for ref (far end) bands}
         @h2xmle_default     {0x0CCC}
         @h2xmle_range       {0..4767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaGainIncrease;
    /**< @h2xmle_description {Smoothing for increasing gain}
         @h2xmle_default     {0x1A6F}
         @h2xmle_range       {0..12767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaGainDecrease;
    /**< @h2xmle_description {Smoothing for decreasing gain}
         @h2xmle_default     {0xA8}
         @h2xmle_range       {0..2767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaAmp;
    /**< @h2xmle_description {Smoothing for amplitude in soft peak limiter}
         @h2xmle_default     {0x0ACF}
         @h2xmle_range       {0..2767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaPLAttack;
    /**< @h2xmle_description {Smoothing for peak limiter attack}
         @h2xmle_default     {0x4567}
         @h2xmle_range       {0..17767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaPLDecay;
    /**< @h2xmle_description {Smoothing for peak limiter decay}
         @h2xmle_default     {0x129F}
         @h2xmle_range       {0..4767}

         @h2xmle_policy       {Basic} */

    int16_t lowGainLimit;
    /**< @h2xmle_description {Lower gain limit (linear). Set to headroom for tuning the echo canceller}
         @h2xmle_default     {0x20}
         @h2xmle_range       {32..45}

         @h2xmle_policy       {Basic} */

    int16_t Headroom;
    /**< @h2xmle_description {Headroom for amplification (linear)}
         @h2xmle_default     {0x90}
         @h2xmle_range       {32..200}

         @h2xmle_policy       {Basic} */

    int16_t peakLim;
    /**< @h2xmle_description {Soft peak limiter threshold; if 0, no peak limiting}
         @h2xmle_default     {0x7FFF}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t hardPeakLim;
    /**< @h2xmle_description {Hard peak limiter amplitude; if 0, no peak limiting}
         @h2xmle_default     {0x7FFF}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t aeq_vad_threshold;
    /**< @h2xmle_description {Far-end VAD threshold}
         @h2xmle_default     {0x01}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t ref_vad_hangover_max;
    /**< @h2xmle_description {VAD hangover,10 ms increments}
         @h2xmle_default     {0x04}
         @h2xmle_range       {0..10}

         @h2xmle_policy       {Basic} */

    int16_t Reserved1;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0..32767}

         @h2xmle_readOnly       {true} */

    int16_t noiseSensThresh [4];
    /**< @h2xmle_description {Sensitivity threshold for noise before gain update, per band}
         @h2xmle_default     {0x20}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t gainMultVector [6];
    /**< @h2xmle_description {Multiplier for each filter bank gain (ratio offset)}
         @h2xmle_default     {0x1333}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;

#define VOICE_PARAM_AVC_RVE_V2 0x00010E41

#include "adsp_begin_pack.h"

typedef struct voice_param_avc_rve_v2_t voice_param_avc_rve_v2_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_AVC_RVE_V2", VOICE_PARAM_AVC_RVE_V2}
    @h2xmlp_description {AVC/RVE version 2 algorithm, which upgrades the single-mic noise estimate method to the minimum
statistics method.\n
Fixed size for this parameter - \n
Version 0 – 164 bytes \n
Version 1 extension – 180 bytes \n\n}
 @h2xmlp_version     {1}*/

struct voice_param_avc_rve_v2_t {

    int16_t version;
    /**< @h2xmle_description {Read only element.}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion    {true}

         @h2xmle_readOnly       {true} */

    int16_t Reserved1;
    /**< @h2xmle_description {Client should set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_readOnly       {true} */

    int16_t FlinkThresholdList [24];
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x3815}
         @h2xmle_range       {0x0..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t FlinkTargetGainList [12];
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x1200}
         @h2xmle_range       {0x0..0x1800}

         @h2xmle_policy       {Basic} */

    int32_t RlinkBNEMin;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0xA0}
         @h2xmle_range       {0x0000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int32_t RlinkBNEMax;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0xF5E245}
         @h2xmle_range       {0x00000000..0x7FFFFFFF}

         @h2xmle_policy       {Basic} */

    int16_t RlinkSensitivityOffset;
    /**< @h2xmle_description {Forward-link AVC variation from nominal sensitivity. This offset is applied to the background noise estimate before comparing against the thresholds. Increasing this parameter makes the AVC more sensitive to background noise.}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0x0000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t FlinkHeadroom;
    /**< @h2xmle_description {Forward-link AVC headroom. This is the maximum total output gain below the threshold that is allowed.}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0x0000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t FlinkSmoothK;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0393}
         @h2xmle_range       {0x0001..0x7FFF}

         @h2xmle_policy       {Basic} */

    uint16_t RlinkBNERamp;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x4666}
         @h2xmle_range       {0x4000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaNoise;
    /**< @h2xmle_description {Smoothing for noise bands}
         @h2xmle_default     {0x0CCC}
         @h2xmle_range       {0..4767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaRec;
    /**< @h2xmle_description {Smoothing for primary channel noise estimate}
         @h2xmle_default     {0x02FF}
         @h2xmle_range       {0..2767}

         @h2xmle_policy       {Basic} */

    int16_t scaleFact_1;
    /**< @h2xmle_description {Factor for scaling single channel estimate bands greater than min}
         @h2xmle_default     {0x2000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t scaleFact_2;
    /**< @h2xmle_description {Factor for scaling second channel noise estimate}
         @h2xmle_default     {0x2000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t opMode;
    /**< @h2xmle_description {Indicates if system is run in Single mic mode (opmode = 1) or Dual mic mode (opmode = 2)}
         @h2xmle_default     {0x02}
         @h2xmle_range       {1..2}
         @h2xmle_policy       {Basic} */

    int16_t WNRThreshold;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x7FFF}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaRef;
    /**< @h2xmle_description {Smoothing for ref (far end) bands}
         @h2xmle_default     {0x0CCC}
         @h2xmle_range       {0..4767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaGainIncrease;
    /**< @h2xmle_description {Smoothing for increasing gain}
         @h2xmle_default     {0x1A6F}
         @h2xmle_range       {0..12767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaGainDecrease;
    /**< @h2xmle_description {Smoothing for decreasing gain}
         @h2xmle_default     {0xA8}
         @h2xmle_range       {0..2767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaAmp;
    /**< @h2xmle_description {Smoothing for amplitude in soft peak limiter}
         @h2xmle_default     {0x0ACF}
         @h2xmle_range       {0..2767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaPLAttack;
    /**< @h2xmle_description {Smoothing for peak limiter attack}
         @h2xmle_default     {0x4567}
         @h2xmle_range       {0..17767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaPLDecay;
    /**< @h2xmle_description {Smoothing for peak limiter decay}
         @h2xmle_default     {0x129F}
         @h2xmle_range       {0..4767}

         @h2xmle_policy       {Basic} */

    int16_t lowGainLimit;
    /**< @h2xmle_description {Lower gain limit (linear). Set to headroom for tuning the echo canceller}
         @h2xmle_default     {0x20}
         @h2xmle_range       {32..45}

         @h2xmle_policy       {Basic} */

    int16_t Headroom;
    /**< @h2xmle_description {Headroom for amplification (linear)}
         @h2xmle_default     {0x90}
         @h2xmle_range       {32..200}

         @h2xmle_policy       {Basic} */

    int16_t peakLim;
    /**< @h2xmle_description {Soft peak limiter threshold; if 0, no peak limiting}
         @h2xmle_default     {0x7FFF}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t hardPeakLim;
    /**< @h2xmle_description {Hard peak limiter amplitude; if 0, no peak limiting}
         @h2xmle_default     {0x7FFF}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t aeq_vad_threshold;
    /**< @h2xmle_description {Far-end VAD threshold}
         @h2xmle_default     {0x04}
         @h2xmle_range       {0..10}

         @h2xmle_policy       {Basic} */

    int16_t ref_vad_hangover_max;
    /**< @h2xmle_description {VAD hangover,10 ms increments}
         @h2xmle_default     {0x20}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t noiseSensThresh [4];
    /**< @h2xmle_description {Sensitivity threshold for noise before gain update, per band}
         @h2xmle_default     {0x1333}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t gainMultVector [6];
    /**< @h2xmle_description {Multiplier for each filter bank gain (ratio offset)}
         @h2xmle_default     {}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t sne_num_blk;
    /**< @h2xmle_description {Number of block size for minimum statistic noise estimate}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    uint16_t sne_blk_size;
    /**< @h2xmle_description {The size of each block for minimum statistic noise estimate}
         @h2xmle_default     {15}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    uint16_t sne_init_alpha_Q15;
    /**< @h2xmle_description {Noise smoothing factor for minimum statistic noise estimate}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t sne_overestimation_factor_Q13;
    /**< @h2xmle_description {Over noise estimate factor for minimum statistic noise estimate}
         @h2xmle_default     {10650}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t hangoverSubBand [7];
    /**< @h2xmle_description {Hangover (number of frames to wait before decreasing gain) for each band.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t noiseSensThreshFB [3];
    /**< @h2xmle_description {Sensitivity threshold for noise before gain update, per band}
         @h2xmle_default     {0x20}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t gainMultVectorFB;
    /**< @h2xmle_description {Multiplier for each filter bank gain (ratio offset)}
         @h2xmle_default     {0x1333}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t gainAdaptNumBands;
    /**< @h2xmle_description {If Tx_Noise_reference is 8k, should be set to 4. If Tx_noise_reference is greater 8k, should be set to some value no larger than 6.}
         @h2xmle_default     {0x4}
         @h2xmle_range       {4..6}

         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;

#define VOICE_PARAM_AVC_RVE_RX 0x00010EAD

#include "adsp_begin_pack.h"

typedef struct voice_param_avc_rve_rx voice_param_avc_rve_rx;
/** @h2xmlp_parameter    {"VOICE_PARAM_AVC_RVE_RX", VOICE_PARAM_AVC_RVE_RX}
    @h2xmlp_description {AVC/RVE version algorithm, which upgrades the single-mic noise estimate method to the minimum
statistics method.\n
Fixed size for this parameter - \n
Version 0 – 72 bytes \n\n}
 @h2xmlp_version     {0}*/

struct voice_param_avc_rve_rx {

    int16_t version;
    /**< @h2xmle_description {Read only element.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion    {true}

         @h2xmle_readOnly       {true} */

    int16_t Reserved;
    /**< @h2xmle_description {Client should set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_readOnly       {true} */

    int16_t smAlphaRef;
    /**< @h2xmle_description {Smoothing for ref (far end) bands}
         @h2xmle_default     {0x0CCC}
         @h2xmle_range       {0..4767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaGainIncrease;
    /**< @h2xmle_description {Smoothing for increasing gain}
         @h2xmle_default     {0x1A6F}
         @h2xmle_range       {0..12767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaGainDecrease;
    /**< @h2xmle_description {Smoothing for decreasing gain}
         @h2xmle_default     {0xA8}
         @h2xmle_range       {0..2767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaAmp;
    /**< @h2xmle_description {Smoothing for amplitude in soft peak limiter}
         @h2xmle_default     {0x0ACF}
         @h2xmle_range       {0..2767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaPLAttack;
    /**< @h2xmle_description {Smoothing for peak limiter attack}
         @h2xmle_default     {0x4567}
         @h2xmle_range       {0..17767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaPLDecay;
    /**< @h2xmle_description {Smoothing for peak limiter decay}
         @h2xmle_default     {0x129F}
         @h2xmle_range       {0..4767}

         @h2xmle_policy       {Basic} */

    int16_t lowGainLimit;
    /**< @h2xmle_description {Lower gain limit (linear). Set to headroom for tuning the echo canceller}
         @h2xmle_default     {0x20}
         @h2xmle_range       {32..45}

         @h2xmle_policy       {Basic} */

    int16_t Headroom;
    /**< @h2xmle_description {Headroom for amplification (linear)}
         @h2xmle_default     {0x90}
         @h2xmle_range       {32..200}

         @h2xmle_policy       {Basic} */

    int16_t peakLim;
    /**< @h2xmle_description {Soft peak limiter threshold; if 0, no peak limiting}
         @h2xmle_default     {0x7FFF}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t hardPeakLim;
    /**< @h2xmle_description {Hard peak limiter amplitude; if 0, no peak limiting}
         @h2xmle_default     {0x7FFF}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t aeq_vad_threshold;
    /**< @h2xmle_description {Far-end VAD threshold}
         @h2xmle_default     {0x04}
         @h2xmle_range       {0..10}

         @h2xmle_policy       {Basic} */

    int16_t ref_vad_hangover_max;
    /**< @h2xmle_description {VAD hangover,10 ms increments}
         @h2xmle_default     {0x20}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t noiseSensThresh [4];
    /**< @h2xmle_description {Sensitivity threshold for noise before gain update, per band}
         @h2xmle_default     {0x1333}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t gainMultVector [6];
    /**< @h2xmle_description {Multiplier for each filter bank gain (ratio offset)}
         @h2xmle_default     {}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t hangoverSubBand [7];
    /**< @h2xmle_description {Hangover (number of frames to wait before decreasing gain) for each band.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t noiseSensThreshFB [3];
    /**< @h2xmle_description {Sensitivity threshold for noise before gain update, per band}
         @h2xmle_default     {0x20}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t gainMultVectorFB;
    /**< @h2xmle_description {Multiplier for each filter bank gain (ratio offset)}
         @h2xmle_default     {0x1333}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t Reserved1;
    /**< @h2xmle_description {Client must set to 0}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly       {true} */

}
#include "adsp_end_pack.h"
;

#define VOICE_PARAM_AVC_RVE_TX 0x00010EAE

#include "adsp_begin_pack.h"

typedef struct voice_param_avc_rve_tx voice_param_avc_rve_tx;
/** @h2xmlp_parameter    {"VOICE_PARAM_AVC_RVE_TX", VOICE_PARAM_AVC_RVE_TX}
    @h2xmlp_description {AVC/RVE tx algorithm, which upgrades the single-mic noise estimate method to the minimum
statistics method.\n
Fixed size for this parameter - \n
Version 0 – 112 bytes \n\n}
 @h2xmlp_version     {0}*/

struct voice_param_avc_rve_tx {

    int16_t version;
    /**< @h2xmle_description {Read only element.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion    {true}

         @h2xmle_readOnly       {true} */

    int16_t gainAdaptNumBands;
    /**< @h2xmle_description {If Tx_Noise_reference is 8k, should be set to 4. If Tx_noise_reference is greater 8k, should be set to some value no larger than 6.}
	     @h2xmle_default     {0x4}
	     @h2xmle_range       {4..6}

         @h2xmle_policy       {Basic} */
    int16_t FlinkThresholdList [24];
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x3815}
         @h2xmle_range       {0x0..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t FlinkTargetGainList [12];
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x1200}
         @h2xmle_range       {0x0..0x1800}

         @h2xmle_policy       {Basic} */

    int32_t RlinkBNEMin;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0xA0}
         @h2xmle_range       {0x0000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int32_t RlinkBNEMax;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0xF5E245}
         @h2xmle_range       {0x00000000..0x7FFFFFFF}

         @h2xmle_policy       {Basic} */

    int16_t RlinkSensitivityOffset;
    /**< @h2xmle_description {Forward-link AVC variation from nominal sensitivity. This offset is applied to the background noise estimate before comparing against the thresholds. Increasing this parameter makes the AVC more sensitive to background noise.}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0x0000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t FlinkHeadroom;
    /**< @h2xmle_description {Forward-link AVC headroom. This is the maximum total output gain below the threshold that is allowed.}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0x0000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t FlinkSmoothK;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x0393}
         @h2xmle_range       {0x0001..0x7FFF}

         @h2xmle_policy       {Basic} */

    uint16_t RlinkBNERamp;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x4666}
         @h2xmle_range       {0x4000..0x7FFF}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaNoise;
    /**< @h2xmle_description {Smoothing for noise bands}
         @h2xmle_default     {0x0CCC}
         @h2xmle_range       {0..4767}

         @h2xmle_policy       {Basic} */

    int16_t smAlphaRec;
    /**< @h2xmle_description {Smoothing for primary channel noise estimate}
         @h2xmle_default     {0x02FF}
         @h2xmle_range       {0..2767}

         @h2xmle_policy       {Basic} */

    int16_t scaleFact_1;
    /**< @h2xmle_description {Factor for scaling single channel estimate bands greater than min}
         @h2xmle_default     {0x2000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t scaleFact_2;
    /**< @h2xmle_description {Factor for scaling second channel noise estimate}
         @h2xmle_default     {0x2000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t opMode;
    /**< @h2xmle_description {Indicates if system is run in Single mic mode (opmode = 1) or Dual mic mode (opmode = 2)}
         @h2xmle_default     {0x02}
         @h2xmle_range       {1..2}
         @h2xmle_policy       {Basic} */

    int16_t WNRThreshold;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0x7FFF}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t sne_num_blk;
    /**< @h2xmle_description {Number of block size for minimum statistic noise estimate}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    uint16_t sne_blk_size;
    /**< @h2xmle_description {The size of each block for minimum statistic noise estimate}
         @h2xmle_default     {15}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    uint16_t sne_init_alpha_Q15;
    /**< @h2xmle_description {Noise smoothing factor for minimum statistic noise estimate}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t sne_overestimation_factor_Q13;
    /**< @h2xmle_description {Over noise estimate factor for minimum statistic noise estimate}
         @h2xmle_default     {10650}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;


#define VOICE_MODULE_TX_AVCRVE  0x00010F1B
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_AVCRVE",
                          VOICE_MODULE_TX_AVCRVE}
    @h2xmlm_displayName  {"TX VOICE AVC RVE"}
	@h2xmlm_description {AVC/RVE algorithm that is part of voice postprocessing on the Tx path. This module is the Tx counterpart
of VOICE_MODULE_RX_AVCRVE on the Rx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_AVC_RVE_ENABLE\n
VOICE_PARAM_AVC_RVE\n
VOICE_PARAM_AVC_RVE_V2\n
VOICE_PARAM_AVC_RVE_TX
\n\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_avc_rve_enable_t}
    @h2xmlm_InsertParameter


    @h2xml_Select					{voice_param_avc_rve_t}
    @h2xmlm_InsertParameter
    @h2xmlp_toolPolicy	{NO_SUPPORT}


   @h2xml_Select					{voice_param_avc_rve_v2_t}
   @h2xmlm_InsertParameter
   @h2xmlp_toolPolicy	{NO_SUPPORT}

    @h2xml_Select					{voice_param_avc_rve_tx}
    @h2xmlm_InsertParameter


   @}                   <-- End of the Module -->*/


#define VOICE_MODULE_RX_AVCRVE  0x00010EE4
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_AVCRVE",
                          VOICE_MODULE_RX_AVCRVE}
    @h2xmlm_displayName  {"RX VOICE AVC RVE"}
	@h2xmlm_description {AVC/RVE algorithm in the voice preprocessor on the Rx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_AVC_RVE_ENABLE\n
VOICE_PARAM_AVC_RVE\n
VOICE_PARAM_AVC_RVE_V2\n
VOICE_PARAM_AVC_RVE_RX
\n\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_avc_rve_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_avc_rve_t}
    @h2xmlm_InsertParameter
    @h2xmlp_toolPolicy	{NO_SUPPORT}

    @h2xml_Select					{voice_param_avc_rve_v2_t}
    @h2xmlm_InsertParameter
    @h2xmlp_toolPolicy	{NO_SUPPORT}

    @h2xml_Select					{voice_param_avc_rve_rx}
    @h2xmlm_InsertParameter


   @}                   <-- End of the Module -->*/
#endif
