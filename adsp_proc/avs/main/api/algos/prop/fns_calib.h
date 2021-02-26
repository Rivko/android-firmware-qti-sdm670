#ifndef FNS_CALIB_H
#define FNS_CALIB_H
/*==============================================================================
  @file fns_calib.h
  @brief This file contains FNS
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/11/17   shrani  Added h2xml comments.
 ========================================================================== */
#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"
#include "ecns_common_param_calib.h"


/** @h2xml_title1           {VOICE_MODULE_FNS}
    @h2xml_title_agile_rev  {VOICE_MODULE_FNS}
    @h2xml_title_date       {April 11, 2017} */



#define VOICE_PARAM_FNS_V2 0x00010E43

#include "adsp_begin_pack.h"

typedef struct voice_param_fns_v2_t voice_param_fns_v2_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FNS_V2", VOICE_PARAM_FNS_V2}

    @h2xmlp_description  {FNS version 2 algorithm. This parameter defines the structure for the Comfort Noise Injection module. \n
Maximum array size for this parameter \n
Version 0 -- 84 bytes \n\n }
    @h2xmlp_version     {0} */

struct voice_param_fns_v2_t {

    int16_t version;
    /**< @h2xmle_description {Specifies version.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion    {true}

         @h2xmle_readOnly       {true} */

    int16_t reserved;
    /**< @h2xmle_description {Client must set this field as zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly       {true} */

    int16_t fnsMode;
    /**< @h2xmle_description {Mode for enabling/disabling submodules}
         @h2xmle_default     {242}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "242"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Basic} */

    int16_t fnsInputGain;
    /**< @h2xmle_description {Input gain to FENS module}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsOutputGain;
    /**< @h2xmle_description {Output gain}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsTargetNS;
    /**< @h2xmle_description {Target noise suppression level in dB}
         @h2xmle_default     {4608}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "4608"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "4096"}

         @h2xmle_policy       {Basic} */

    int16_t fnsSalpha;
    /**< @h2xmle_description {Over-subtraction factor for stationary NS}
         @h2xmle_default     {5120}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5120"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "4096"}

         @h2xmle_policy       {Basic} */

    int16_t fnsNalpha;
    /**< @h2xmle_description {Over-subtraction factor for nonstationary NS}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "2048"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "4096"}

         @h2xmle_policy       {Basic} */

    int16_t fnsNalphaMax;
    /**< @h2xmle_description {Maximum over-subtraction factor for nonstationary NS}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "2048"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "4096"}

         @h2xmle_policy       {Basic} */

    int16_t fnsEalpha;
    /**< @h2xmle_description {Scaling factor for excess noise suppression}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsNSNRmax;
    /**< @h2xmle_description {Upper bound in dB for SNR estimation}
         @h2xmle_default     {5120}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsSNblock;
    /**< @h2xmle_description {Quarter block size for stationary NS}
         @h2xmle_default     {50}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "25"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "50"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "25"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "25"}

         @h2xmle_policy       {Basic} */

    int16_t fnsNi;
    /**< @h2xmle_description {Initialization block size for nonstationary NS}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsNPscale;
    /**< @h2xmle_description {Power scale factor for nonstationary noise update}
         @h2xmle_default     {2560}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsNLambda;
    /**< @h2xmle_description {Smoothing factor for nonstationary noise update}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsNLambdaf;
    /**< @h2xmle_description {Smoothing factor for higher level nonstationary noise update}
         @h2xmle_default     {32736}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsGsBias;
    /**< @h2xmle_description {Bias factor in dB for gain calculation}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsGsMax;
    /**< @h2xmle_description {SNR lower bound in dB for aggressive gain calculation}
         @h2xmle_default     {2000}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsSalphaHB;
    /**< @h2xmle_description {Over-subtraction factor for high-band stationary NS}
         @h2xmle_default     {5120}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5120"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "4096"}

         @h2xmle_policy       {Basic} */

    int16_t fnsNalphaMaxHB;
    /**< @h2xmle_description {Maximum over-subtraction factor for high-band nonstationary NS}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "2048"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "4096"}

         @h2xmle_policy       {Basic} */

    int16_t fnsEalphaHB;
    /**< @h2xmle_description {Scaling factor for high-band excess noise suppression}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsNLambda0;
    /**< @h2xmle_description {Smoothing factor for nonstationary noise update during speech activity}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsGsFast;
    /**< @h2xmle_description {Fast smoothing factor for postprocessor gain}
         @h2xmle_default     {9830}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsGsMed;
    /**< @h2xmle_description {Medium smoothing factor for postprocessor gain}
         @h2xmle_default     {22938}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsGsSlow;
    /**< @h2xmle_description {Slow smoothing factor for postprocessor gain}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsSwbNalpha;
    /**< @h2xmle_description {Over-subtraction factor for super wide band stationary NS}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsSwbSalpha;
    /**< @h2xmle_description {To do}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t thresh;
    /**< @h2xmle_description {Threshold for generating a binary VAD decision}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t pwrScale;
    /**< @h2xmle_description {Indirectly lower-bounds the noise level estimate}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t hangoverMax;
    /**< @h2xmle_description {Avoids mid-speech clipping and reliably detects weak speech bursts at the end of speech activity}
         @h2xmle_default     {20}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t alphaSNR;
    /**< @h2xmle_description {Controls responsiveness of the VAD}
         @h2xmle_default     {3278}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t snrDiffMax;
    /**< @h2xmle_description {Decreasing this parameter value may help in making correct decisions during abrupt changes; however, decreasing too much may increase false alarms during long pauses/silences}
         @h2xmle_default     {3072}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t snrDiffMin;
    /**< @h2xmle_description {Decreasing this parameter value may help in making correct decisions during abrupt changes; however, decreasing too much may increase false alarms during long pauses/silences}
         @h2xmle_default     {2560}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t initLength;
    /**< @h2xmle_description {Defines the number of frames for which noise level estimate is set to a fixed value}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t maxVal;
    /**< @h2xmle_description {Defines the upper limit of the noise level}
         @h2xmle_default     {648}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t initBound;
    /**< @h2xmle_description {Defines the initial bounding value for the noise level estimate; used during the initial segment defined by the parameter initLength}
         @h2xmle_default     {10}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "50"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "10"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "50"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "50"}

         @h2xmle_policy       {Basic} */

    int16_t resetBound;
    /**< @h2xmle_description {Reset bound for noise tracking}
         @h2xmle_default     {290}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t avarScale;
    /**< @h2xmle_description {Defines the bias factor in noise estimation}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t sub_Nc;
    /**< @h2xmle_description {Defines the window length for noise estimation}
         @h2xmle_default     {25}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t spowMin;
    /**< @h2xmle_description {Defines the minimum signal power required to update the bounds for the noise floor estimate}
         @h2xmle_default     {1310}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsTargetNoiseFloor;
    /**< @h2xmle_description {Comfort noise leve}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fnsSlopeNoiseFloor;
    /**< @h2xmle_description {Comfort noise slope to determine the shape of the comfort noise.}
         @h2xmle_default     {32740}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FNS_V3 0x00010E80

#include "adsp_begin_pack.h"

typedef struct voice_param_fns_v3_t voice_param_fns_v3_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FNS_V3", VOICE_PARAM_FNS_V3}

    @h2xmlp_description  {FNS version 3 algorithm. This parameter defines the structure for VOICE_MODULE_FNS_V2. FNS v3
provides better noise suppression than previous FNS modules.\n
Maximum array size for this parameter \n
Version 0 – 124 bytes \n \n}
    @h2xmlp_version      {0}*/

struct voice_param_fns_v3_t {

    int16_t version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly       {true}
         @h2xmle_isVersion    {true} */

    int16_t reserved;
    /**< @h2xmle_description {Client should set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_readOnly       {true} */

    int32_t fns_v2_feature_mode;
    /**< @h2xmle_description {Controls overall functionality mode bit}
         @h2xmle_default     {62}
         @h2xmle_range       {0..2147483647}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_GT_delayMode;
    /**< @h2xmle_description {FFT type (low delay, high delay)}
         @h2xmle_default     {0}
         @h2xmle_range       {0..1}
	 @h2xmle_readOnly     {true}
         @h2xmle_policy       {Basic} */

    int16_t fns_v2_GT_resolutionMode;
    /**< @h2xmle_description {Compressed Subband resolution mode bit flag (low, high and super)}
         @h2xmle_default     {1}
         @h2xmle_range       {0..2}
	 @h2xmle_readOnly     {true}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_GT_InputGain;
    /**< @h2xmle_description {Input gain}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_GT_OutputGain;
    /**< @h2xmle_description {Output gain}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_vad_Mode;
    /**< @h2xmle_description {VAD type mode bit}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_vadnref_Ni;
    /**< @h2xmle_description {Q0 Block length for initial non-stationary NS estimation}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_vadnref_NPscale;
    /**< @h2xmle_description {Q7.8 Speech level scale factor for non-stationary noise computation}
         @h2xmle_default     {2560}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_vadnref_NLambda;
    /**< @h2xmle_description {Q15 Smoothing factor for non-stationary noise averaging}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fns_v2_vadnref_NLambdaf;
    /**< @h2xmle_description {Q15 Higher smoothing factor for non-stationary noise averaging}
         @h2xmle_default     {32736}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_vadnref_NLambda0;
    /**< @h2xmle_description {Q15 Highest smoothing factor for non-stationary noise averaging}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_thresh;
    /**< @h2xmle_description {Q0.15 threshold for VAD detection}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_pwrScale;
    /**< @h2xmle_description {Q0.15 Multiplication factor for speech power}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_hangoverMax;
    /**< @h2xmle_description {Length of hangover window}
         @h2xmle_default     {5}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_alphaSNR;
    /**< @h2xmle_description {Q0.15 averaging factor for smoothing SNR estimate}
         @h2xmle_default     {3278}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_snrDiffMax;
    /**< @h2xmle_description {Q7.8 max limit for log SNR difference}
         @h2xmle_default     {3072}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_snrDiffMin;
    /**< @h2xmle_description {Q7.8 min limit for log SNR difference}
         @h2xmle_default     {2560}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_initLength;
    /**< @h2xmle_description {Number of initial frames for min bound control}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_maxVal;
    /**< @h2xmle_description {Q0.15 square-root of upper bound for noise power estimate}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_initBound;
    /**< @h2xmle_description {Q0.15 square-root of initial value for minimum bound}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_resetBound;
    /**< @h2xmle_description {Q0.15 square-root of reset value for minimum bound}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_avarScale;
    /**< @h2xmle_description {Q2.13 Multiplication factor for speech standard deviation}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_sub_Nc;
    /**< @h2xmle_description {Sub-window length for single-mic VAD}
         @h2xmle_default     {25}
         @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t fns_v2_avad_spowMin;
    /**< @h2xmle_description {Q0.15 square root of minimum speech power threshold}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_Mode;
    /**< @h2xmle_description {SMRMT sub-module mode bit}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_wlenInit;
    /**< @h2xmle_description {SMRMT init windows length}
         @h2xmle_default     {50}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_wlenInc;
    /**< @h2xmle_description {SMRMT window size increase counter step size}
         @h2xmle_default     {20}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_wlenMax;
    /**< @h2xmle_description {SMRMT window max size}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_band1I;
    /**< @h2xmle_description {SMRMT band1 index}
         @h2xmle_default     {12}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_band2I;
    /**< @h2xmle_description {SMRMT band2 index}
         @h2xmle_default     {17}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_alphaP;
    /**< @h2xmle_description {SMRMT probability smoothing alpha}
         @h2xmle_default     {3277}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_alphaD;
    /**< @h2xmle_description {SMRMT noise estimate learning rate}
         @h2xmle_default     {27852}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_overest_factL16Q12;
    /**< @h2xmle_description {SMRMT noise reference over-estimate factor}
         @h2xmle_default     {5300}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_smrmt_nlinitQL16;
    /**< @h2xmle_description {SMRMT initial noise floor Q factor}
         @h2xmle_default     {31}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP_Mode;
    /**< @h2xmle_description {PP sub-module mode bit}
         @h2xmle_default     {3}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP1_alphaL16Q13;
    /**< @h2xmle_description {PP1 over-subtraction factor}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP1_alpha_smLB_L16Q15;
    /**< @h2xmle_description {PP1 time averaging smoothing rate for low band}
         @h2xmle_default     {28500}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP1_alpha_smNLB_L16Q15;
    /**< @h2xmle_description {PP1 time averaging smoothing rate for high band}
         @h2xmle_default     {25000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP1_minGainL16_Q14;
    /**< @h2xmle_description {PP1 minimum gain floor}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP1_envRestFactL16Q12;
    /**< @h2xmle_description {PP1 envelope restoration factor}
         @h2xmle_default     {13000}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_TargetNS;
    /**< @h2xmle_description {PP2 Target Noise suppression level in dB}
         @h2xmle_default     {5120}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_Nalpha;
    /**< @h2xmle_description {PP2 Over-subtraction factor}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_NalphaMax;
    /**< @h2xmle_description {PP2 Max Over-subtraction factor}
         @h2xmle_default     {5120}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_NSNRmax;
    /**< @h2xmle_description {PP2 Upper bound in dB for SNR estimation}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_GsBias;
    /**< @h2xmle_description {PP2 Bias factor in dB for gain calculation}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_GsMax;
    /**< @h2xmle_description {PP2 SNR Lower bound in dB for aggressive gain calculation}
         @h2xmle_default     {2560}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_submode;
    /**< @h2xmle_description {PP2 sub module functionality mode bit control flag}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_envRest_FactL16Q12;
    /**< @h2xmle_description {PP2 envelope restoration factor}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_TargetNoiseFloor;
    /**< @h2xmle_description {Comfort noise level}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t fns_v2_PP2_SlopeNoiseFloor;
    /**< @h2xmle_description {Comfort noise slope to determine the shape of the comfort noise.}
         @h2xmle_default     {32740}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int16_t reserved1;
    /**< @h2xmle_description {Client should set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

    int32_t fns_v2_smrmt_thrB1;
    /**< @h2xmle_description {SMRMT band1 noise update threshold}
         @h2xmle_default     {250000000}
         @h2xmle_range       {0x0..0x7FFFFFFF}

         @h2xmle_policy       {Basic} */

    int32_t fns_v2_smrmt_thrB2;
    /**< @h2xmle_description {SMRMT band2 noise update threshold}
         @h2xmle_default     {200000000}
         @h2xmle_range       {0x0..0x7FFFFFFF}

         @h2xmle_policy       {Basic} */

    int32_t fns_v2_smrmt_thrB3;
    /**< @h2xmle_description {SMRMT band3 noise update threshold}
         @h2xmle_default     {180000000}
         @h2xmle_range       {0x0..0x7FFFFFFF}



         @h2xmle_policy       {Basic} */

    int32_t fns_v2_smrmt_nlinitL32;
    /**< @h2xmle_description {SMRMT band3 noise update threshold}
         @h2xmle_default     {180000000}
         @h2xmle_range       {0x0..0x7FFFFFFF}

         @h2xmle_policy       {Basic} */


}
#include "adsp_end_pack.h"
;








#define VOICE_MODULE_FNS  0x00010EEB
/**
    @h2xmlm_module       {"VOICE_MODULE_FNS",
                          VOICE_MODULE_FNS}
    @h2xmlm_displayName  {"RX VOICE FNS"}
	@h2xmlm_description {FNS algorithm in the voice postprocessor on the Rx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_FNS\n
 VOICE_PARAM_FNS_V2\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @h2xmlm_deprecated              {true}
    @{                   <-- Start of the Module -->

@h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_fns_t}
   @h2xmlm_InsertParameter
   @h2xmlp_toolPolicy	        {NO_SUPPORT}

@h2xml_Select					{ voice_param_fns_v2_t}
   @h2xmlm_InsertParameter



   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_FNS_V2  0x00010F24
/**
    @h2xmlm_module       {"VOICE_MODULE_FNS_V2",
                          VOICE_MODULE_FNS_V2}
    @h2xmlm_displayName  {"RX VOICE FNS V2"}
	@h2xmlm_description {FNS version 2 algorithm in the voice postprocessor on the Rx path. Version 2 provides better noise
suppression.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
 VOICE_PARAM_FNS_V3\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

@h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_mod_enable_t::Enable}
    @h2xmle_default     {1}

@h2xml_Select					{voice_param_fns_v3_t}
   @h2xmlm_InsertParameter


   @}                   <-- End of the Module -->*/
#endif
