#ifndef SMECNS_CALIB_H
#define SMECNS_CALIB_H
/*==============================================================================
  @file smecns_calib.h
  @brief This file contains SMECNS
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/08/17   shrani  Added h2xml comments.
 ========================================================================== */
#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"
#include "ecns_common_param_calib.h"


/** @h2xml_title1           {VOICE_MODULE_SMECNS}
    @h2xml_title_agile_rev  {VOICE_MODULE_SMECNS}
    @h2xml_title_date       {APRIL 08, 2017} */


#define VOICE_PARAM_SMECNS 0x00010E01

#include "adsp_begin_pack.h"

typedef struct voice_param_smecns_t voice_param_smecns_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_SMECNS", VOICE_PARAM_SMECNS}

    @h2xmlp_description  {Single-mic ECNS.\n
Maximum array size for this parameter \n
Version 0 – 92 bytes \n
Note Single-mic ECNS also supports a versioned command, VOICE_PARAM_SMECNS_EXT. \n\n}
    @h2xmlp_version     {0}

    @h2xmlp_toolPolicy  {NO_SUPPORT}*/

struct voice_param_smecns_t {

    uint16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion    {true}
         @h2xmle_readOnly    {true}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Basic} */

    uint16_t Mode;
    /**< @h2xmle_description {Mode word controls the activation of various blocks of EC}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t tuning_mode;
    /**< @h2xmle_description {Speech frame can be replaced by 1 kHz sinusoidal frame depending on the internal status of EEC set by tuning_mode}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Basic} */


    int16_t HPF_coeffs[6];
    /**< @h2xmle_description {Determines high pass filter. No adjustments should be made.}
         @h2xmle_default     {14661}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_limit;
    /**< @h2xmle_description {Saturator for the echo reference.}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "32767"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "32767"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "32767"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "32767"}

         @h2xmle_policy       {Basic} */

    int16_t echo_path_delay;
    /**< @h2xmle_description {Compensates the fixed delay in echo path}
         @h2xmle_default     {0}
         @h2xmle_range       {-100..400}


         @h2xmle_policy       {Basic} */

    int16_t output_gain;
    /**< @h2xmle_description {Output gain}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t input_gain;
    /**< @h2xmle_description {Input gain}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_twoalpha;
    /**< @h2xmle_description {Step size to update the coefficients of AF}
         @h2xmle_default     {2699}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_erl;
    /**< @h2xmle_description {Adaptive step size control for AF filter}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_taps;
    /**< @h2xmle_description {Number of taps of AF foreground filter}
         @h2xmle_default     {64}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Basic} */

    int16_t AF_preset_coefs;
    /**< @h2xmle_description {Indicates whether a preset of AF coefficients should be done}
         @h2xmle_default     {2}
         @h2xmle_range       {0..3}


         @h2xmle_policy       {Basic} */

    int16_t AF_offset;
    /**< @h2xmle_description {Minimum power level used in computation}
         @h2xmle_default     {767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_erl_bg;
    /**< @h2xmle_description {Adaptive step size control for AF background filter}
         @h2xmle_default     {64}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_taps_bg;
    /**< @h2xmle_description {Number of taps of AF background filter}
         @h2xmle_default     {32}
         @h2xmle_range       {0..64}


         @h2xmle_policy       {Basic} */

    int16_t PCD_threshold;
    /**< @h2xmle_description {Controls sensitivity to detect of path changes}
         @h2xmle_default     {18000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t minimum_erl;
    /**< @h2xmle_description {Minimum adaptive step size}
         @h2xmle_default     {64}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t erl_step;
    /**< @h2xmle_description {erl_step}
         @h2xmle_default     {18000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_far;
    /**< @h2xmle_description {SPDET_far}
         @h2xmle_default     {20000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_mic;
    /**< @h2xmle_description {SPDET_mic}
         @h2xmle_default     {20000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_xclip;
    /**< @h2xmle_description {Controls the activation of nonlinear echo suppression.}
         @h2xmle_default     {128}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_tail_alpha;
    /**< @h2xmle_description {Represents decay time in energy of echo tail of impulse response.}
         @h2xmle_default     {4000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_tail_portion;
    /**< @h2xmle_description {Represents the level of the echo tail present in power estimated by AF filter.}
         @h2xmle_default     {4000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_alpha;
    /**< @h2xmle_description {DENS_gamma_e_alpha}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_high;
    /**< @h2xmle_description {Additional muting gain applied in DES during far-end only.}
         @h2xmle_default     {512}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_dt;
    /**< @h2xmle_description {Additional muting gain applied in DES during doubletalk.}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_low;
    /**< @h2xmle_description {DENS_gamma_e_low}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_rescue;
    /**< @h2xmle_description {DENS_gamma_e_rescue}
         @h2xmle_default     {1024}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_spdet_near;
    /**< @h2xmle_description {DENS_spdet_near}
         @h2xmle_default     {1024}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_spdet_act;
    /**< @h2xmle_description {DENS_spdet_act}
         @h2xmle_default     {768}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_n;
    /**< @h2xmle_description {DENS_gamma_n}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_NFE_blocksize;
    /**< @h2xmle_description {DENS_NFE_blocksize}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_limit_NS;
    /**< @h2xmle_description {Controls the maximum amount of noise suppression.}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_NL_atten;
    /**< @h2xmle_description {Controls the amount of nonlinear echo suppression.}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_CNI_level;
    /**< @h2xmle_description {DENS_CNI_level}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_echo_ratio;
    /**< @h2xmle_description {WB_echo_ratio}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_gamma_n;
    /**< @h2xmle_description {Oversubtraction factor and bias compensation for noise estimation. Used with DENS_limit_NS.}
         @h2xmle_default     {1024}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_gamma_e;
    /**< @h2xmle_description {This parameter adjusts the aggressiveness of EC in the high band (4 ~ 8 kHz). A higher value is more aggressive and suppresses more high-band echo.Q-format - Q4.11WB_gamma_E = 2048 * gammaWhere gamma is in the range [0,15]}
         @h2xmle_default     {768}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t max_noise_floor;
    /**< @h2xmle_description {max_noise_floor}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t det_threshold;
    /**< @h2xmle_description {det_threshold}
         @h2xmle_default     {99}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t Reserved1;
    /**< @h2xmle_description {Client should set this param to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly    {true}


         @h2xmle_policy       {Basic} */

    uint16_t Reserved2;
    /**< @h2xmle_description {Client should set this param to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly    {true}


         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_SMECNS_EXT 0x00010E27

#include "adsp_begin_pack.h"

typedef struct voice_param_smecns_ext_t voice_param_smecns_ext_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_SMECNS_EXT", VOICE_PARAM_SMECNS_EXT}

    @h2xmlp_description  {Single-mic ECNS with versioning.\n
Maximum array size for this parameter \n
Version 0 – 96 bytes \n
Version 1 extension – 112 bytes \n\n }
 @h2xmlp_version     {1}*/

struct voice_param_smecns_ext_t {

    uint16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly    {true}
         @h2xmle_isVersion    {true}


         @h2xmle_policy       {Basic} */

    uint16_t Mode;
    /**< @h2xmle_description {Mode word controls the activation of various blocks of EC}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t tuning_mode;
    /**< @h2xmle_description {Speech frame can be replaced by 1 kHz sinusoidal frame depending on the internal status of EEC set by tuning_mode}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t HPF_coeffs[6];
    /**< @h2xmle_description {Determines high pass filter. No adjustments should be made.}
         @h2xmle_default     {14661}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */



    int16_t AF_limit;
    /**< @h2xmle_description {Saturator for the echo reference.}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t echo_path_delay;
    /**< @h2xmle_description {Compensates the fixed delay in echo path}
         @h2xmle_default     {0}
         @h2xmle_range       {-100..400}


         @h2xmle_policy       {Basic} */

    int16_t output_gain;
    /**< @h2xmle_description {Output gain}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t input_gain;
    /**< @h2xmle_description {Input gain}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_twoalpha;
    /**< @h2xmle_description {Step size to update the coefficients of AF}
         @h2xmle_default     {2699}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_erl;
    /**< @h2xmle_description {Adaptive step size control for AF filter}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_taps;
    /**< @h2xmle_description {Number of taps of AF foreground filter}
         @h2xmle_default     {64}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Basic} */

    int16_t AF_preset_coefs;
    /**< @h2xmle_description {Indicates whether a preset of AF coefficients should be done}
         @h2xmle_default     {2}
         @h2xmle_range       {0..3}


         @h2xmle_policy       {Basic} */

    int16_t AF_offset;
    /**< @h2xmle_description {Minimum power level used in computation}
         @h2xmle_default     {767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_erl_bg;
    /**< @h2xmle_description {Adaptive step size control for AF background filter}
         @h2xmle_default     {64}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_taps_bg;
    /**< @h2xmle_description {Number of taps of AF background filter}
         @h2xmle_default     {32}
         @h2xmle_range       {0..64}


         @h2xmle_policy       {Basic} */

    int16_t PCD_threshold;
    /**< @h2xmle_description {Controls sensitivity to detect of path changes}
         @h2xmle_default     {18000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t minimum_erl;
    /**< @h2xmle_description {Minimum adaptive step size}
         @h2xmle_default     {64}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t erl_step;
    /**< @h2xmle_description {..}
         @h2xmle_default     {18000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_far;
    /**< @h2xmle_description {..}
         @h2xmle_default     {20000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_mic;
    /**< @h2xmle_description {..}
         @h2xmle_default     {20000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_xclip;
    /**< @h2xmle_description {Controls the activation of nonlinear echo suppression.}
         @h2xmle_default     {128}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_tail_alpha;
    /**< @h2xmle_description {Represents decay time in energy of echo tail of impulse response.}
         @h2xmle_default     {4000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_tail_portion;
    /**< @h2xmle_description {Represents the level of the echo tail present in power estimated by AF filter.}
         @h2xmle_default     {4000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_alpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_high;
    /**< @h2xmle_description {Additional muting gain applied in DES during far-end only.}
         @h2xmle_default     {512}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_dt;
    /**< @h2xmle_description {Additional muting gain applied in DES during doubletalk.}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_low;
    /**< @h2xmle_description {..}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_e_rescue;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1024}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_spdet_near;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1024}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_spdet_act;
    /**< @h2xmle_description {..}
         @h2xmle_default     {768}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_gamma_n;
    /**< @h2xmle_description {Oversubtraction factor and bias compensation for noise estimation. Used with DENS_limit_NS.}
         @h2xmle_default     {256}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_NFE_blocksize;
    /**< @h2xmle_description {..}
         @h2xmle_default     {200}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_limit_NS;
    /**< @h2xmle_description {Controls the maximum amount of noise suppression.}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_NL_atten;
    /**< @h2xmle_description {Controls the amount of nonlinear echo suppression.}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t DENS_CNI_level;
    /**< @h2xmle_description {..}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_echo_ratio;
    /**< @h2xmle_description {..}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_gamma_n;
    /**< @h2xmle_description {Oversubtraction factor and bias compensation for high band noise estimation}
         @h2xmle_default     {1024}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_gamma_e;
    /**< @h2xmle_description {This parameter adjusts the aggressiveness of EC in the high band (4 ~ 8 kHz). A higher value is more aggressive and suppresses more high-band echo.Q-format - Q4.11WB_gamma_E = 2048 * gammaWhere gamma is in the range [0,15]}
         @h2xmle_default     {768}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t max_noise_floor;
    /**< @h2xmle_description {..}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t det_threshold;
    /**< @h2xmle_description {..}
         @h2xmle_default     {99}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t WB_tail_alpha;
    /**< @h2xmle_description {Represents decay time in energy of echo tail of impulse response for highband.}
         @h2xmle_default     {12000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t WB_tail_portion;
    /**< @h2xmle_description {Represents the level of the echo tail present in power estimated by AF filter for highband.}
         @h2xmle_default     {6000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t Reserved1;
    /**< @h2xmle_description {Client should set this to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly    {true}


         @h2xmle_policy       {Basic} */

    uint16_t AF_PostGain;
    /**< @h2xmle_description {..}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t AF_High_limit;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t AF_High_taps;
    /**< @h2xmle_description {..}
         @h2xmle_default     {64}
         @h2xmle_range       {0..400}


         @h2xmle_policy       {Basic} */

    uint16_t AF_High_twoalpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {2560}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t AF_High_erl;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t AF_High_offset;
    /**< @h2xmle_description {..}
         @h2xmle_default     {250}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t WB_Echo_Scale;
    /**< @h2xmle_description {..}
         @h2xmle_default     {767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t Rx_Ref_Gain;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_SMECNS_PRESET 0x00010E02

#include "adsp_begin_pack.h"

typedef struct voice_param_smecns_preset_t voice_param_smecns_preset_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_SMECNS_PRESET", VOICE_PARAM_SMECNS_PRESET}

    @h2xmlp_description  {SMECNS preset coefficients for single-mic ECNS.
Maximum array size for this parameter\n
Version 0 – 1604 bytes \n\n} */

struct voice_param_smecns_preset_t {

    uint16_t NumPresetFilterTaps;
    /**< @h2xmle_description {Number of taps of AF foreground filter for highband}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    uint16_t Reserved;
    /**< @h2xmle_description {Client should set this to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly    {true}


         @h2xmle_policy       {Basic} */

    uint32_t PresetFilterCoeffs[0];
    /**< @h2xmle_description {Array of preset coefficients}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_variableArraySize  { "NumPresetFilterTaps" }


         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;



#define VOICE_MODULE_SMECNS  0x00010EE0
/**
    @h2xmlm_module       {"VOICE_MODULE_SMECNS",
                          VOICE_MODULE_SMECNS}
	@h2xmlm_description {Single-mic ECNS in the voice processor on the Tx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_SMECNS (supported only for backward compatibility)\n
VOICE_PARAM_SMECNS_PRESET\n
VOICE_PARAM_AF_COEFFS\n
VOICE_PARAM_FNS\n
VOICE_PARAM_SMECNS_EXT\n\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_mod_enable_t::Enable}
    @h2xmle_default     {1}

    @h2xml_Select					{voice_param_smecns_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_smecns_preset_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_af_coeffs_sm_ecns_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_fns_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_smecns_ext_t}
    @h2xmlm_InsertParameter



   @}                   <-- End of the Module -->*/


#endif
