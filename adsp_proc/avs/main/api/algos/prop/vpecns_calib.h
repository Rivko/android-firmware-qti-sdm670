#ifndef VPECNS_CALIB_H
#define VPECNS_CALIB_H
/*==============================================================================
  @file vpecns_calib.h
  @brief This file contains VPECNS
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


/** @h2xml_title1           {VOICE_MODULE_VPECNS
    @h2xml_title_agile_rev  {VOICE_MODULE_VPECNS
    @h2xml_title_date       {March 13, 2017} */


#define VOICE_PARAM_VPECNS 0x00010E34

#include "adsp_begin_pack.h"

typedef struct voice_param_vpecns_t voice_param_vpecns_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_VPECNS", VOICE_PARAM_VPECNS}

    @h2xmlp_description  {Voice Plus dual-mic ECNS.\n
Maximum array size for this parameter\n
Version 0 – 80 bytes \n}
@h2xmlp_version     {0} */

struct voice_param_vpecns_t {

    uint16_t version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {0}
         @h2xmle_range       {0..65535}
         @h2xmle_isVersion    {true}


         @h2xmle_readOnly       {true} */

    uint16_t reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..65535}


         @h2xmle_readOnly       {true} */

    uint16_t Mode;
    /**< @h2xmle_description {Mode word to enable/disable sub-modules in the algorithm}
         @h2xmle_default     {63987}
         @h2xmle_range       {0..65535}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "63987"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "63987"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Basic} */

    uint16_t Mode2;
    /**< @h2xmle_description {Additional mode word for advanced options}
         @h2xmle_default     {0}
         @h2xmle_range       {0..65535}


         @h2xmle_policy       {Advanced} */

    int16_t EC_path_delay;
    /**< @h2xmle_description {Acoustic echo path delay compensation}
         @h2xmle_default     {0}
         @h2xmle_range       {-100..400}


         @h2xmle_policy       {Basic} */

    uint16_t LEC1_taps;
    /**< @h2xmle_description {Number of taps for LEC1 (mic1)}
         @h2xmle_default     {100}
         @h2xmle_range       {0..200}


         @h2xmle_policy       {Basic} */

    uint16_t LEC1_el_rate;
    /**< @h2xmle_description {Learning rate for LEC1}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t LEC2_taps;
    /**< @h2xmle_description {Number of taps for LEC2 (mic2)}
         @h2xmle_default     {100}
         @h2xmle_range       {0..200}


         @h2xmle_policy       {Basic} */

    uint16_t LEC2_el_rate;
    /**< @h2xmle_description {Learning rate for LEC2}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_ecAlphar;
    /**< @h2xmle_description {Echo suppression based on decay rate of estimated echo tail}
         @h2xmle_default     {12288}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_ecCompy;
    /**< @h2xmle_description {Echo suppression based on power of the estimated echo tail}
         @h2xmle_default     {32000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_ecFarGamma;
    /**< @h2xmle_description {Echo suppression aggressiveness during far-end speech}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_ecNearGamma;
    /**< @h2xmle_description {Echo suppression aggressiveness during near-end speech}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_nsGamma_SN;
    /**< @h2xmle_description {Stationary noise suppression aggressiveness}
         @h2xmle_default     {9216}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_nsGamma_NN;
    /**< @h2xmle_description {Non-stationary noise suppression aggressiveness}
         @h2xmle_default     {10240}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_nsGain_SN;
    /**< @h2xmle_description {Maximum amount of stationary noise suppression}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_nsGain_NN;
    /**< @h2xmle_description {Maximum amount of non-stationary noise suppression}
         @h2xmle_default     {1536}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_nsGamma_bias;
    /**< @h2xmle_description {Bias compensation to balance stationary and non-stationary noise suppression}
         @h2xmle_default     {9216}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t SF_vad_thresh;
    /**< @h2xmle_description {Voice activity detection threshold for spatial filter}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t Gain_mic1;
    /**< @h2xmle_description {Input gain for mic 1}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t Gain_mic2;
    /**< @h2xmle_description {Input gain for mic 2}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_WB_ecFarGamma;
    /**< @h2xmle_description {Echo suppression aggressiveness for high-band (4-8kHz)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_WB_nsGamma_SN;
    /**< @h2xmle_description {Stationary noise suppression aggressiveness for high band (4-8kHz)}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_WB_nsGamma_NN;
    /**< @h2xmle_description {Non-stationary noise suppression aggressiveness for high band (4-8kHz)}
         @h2xmle_default     {1024}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t PP_nsref_factor;
    /**< @h2xmle_description {Noise reference over-estimation}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t Gain_out;
    /**< @h2xmle_description {Output gain}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t Gain_agm;
    /**< @h2xmle_description {Over-estimation of noise mic signal before spatial filter}
         @h2xmle_default     {19472}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_nlec_gamma;
    /**< @h2xmle_description {Non-linear echo suppression aggressiveness}
         @h2xmle_default     {512}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t PP_ec_cni_level;
    /**< @h2xmle_description {Comfort noise injection level}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_ecDTGamma;
    /**< @h2xmle_description {Echo suppression aggressiveness during double-talk}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t NS_clean_thresh;
    /**< @h2xmle_description {Clean speech detection threshold}
         @h2xmle_default     {150}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t Mode3;
    /**< @h2xmle_description {Additional mode word for advanced options}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t Gain_postLEC_mic1;
    /**< @h2xmle_description {LEC 1 post gain (before noise suppression)}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t Gain_postLEC_mic2;
    /**< @h2xmle_description {LEC 2 post gain (before noise suppression)}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t PP_Clean_nsGamma_SN;
    /**< @h2xmle_description {Stationary noise suppression aggressiveness when clean speech is detected}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t PP_Clean_nsGamma_NN;
    /**< @h2xmle_description {Non-Stationary noise suppression aggressiveness when clean speech is detected}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    uint16_t LEC_HB_taps;
    /**< @h2xmle_description {Number of taps for high band LEC}
         @h2xmle_default     {100}
         @h2xmle_range       {0..200}


         @h2xmle_policy       {Basic} */

    uint16_t LEC_HB_el_rate;
    /**< @h2xmle_description {High band LEC learning rate}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_WB_ecAlphar;
    /**< @h2xmle_description {Echo suppression based on decay rate of estimated echo tail for high band (4-8kHz)}
         @h2xmle_default     {12288}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t PP_WB_ecCompy;
    /**< @h2xmle_description {Echo suppression based on power of the estimated echo tail for high band (4-8kHz)}
         @h2xmle_default     {32000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;



#define VOICE_MODULE_VPECNS  0x00010F16
/**
    @h2xmlm_module       {"VOICE_MODULE_VPECNS", VOICE_MODULE_VPECNS}
    @h2xmlm_displayName  {"TX VOICE MODULE VPECNS"}
	@h2xmlm_description {Dual-mic Voice Plus ECNS in the voice processor on the Tx path.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_VPECNS\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_mod_enable_t::Enable}
    @h2xmle_default     {1}

    @h2xml_Select					{voice_param_vpecns_t}
    @h2xmlm_InsertParameter

   @}                   <-- End of the Module -->*/
#endif
