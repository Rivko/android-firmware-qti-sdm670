#ifndef DMECNS_CALIB_H
#define DMECNS_CALIB_H
/*==============================================================================
  @file dmecns_calib.h
  @brief This file contains DMECNS
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
#include "ecns_common_param_calib.h"
#include "voice_param_mod_enable_calib.h"


/** @h2xml_title1           {VOICE_MODULE_DMECNS}
    @h2xml_title_agile_rev  {VOICE_MODULE_DMECNS}
    @h2xml_title_date       {April 11, 2017} */


#define VOICE_PARAM_DMECNS 0x00010E11

#include "adsp_begin_pack.h"

typedef struct voice_param_dmecns_t voice_param_dmecns_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_DMECNS", VOICE_PARAM_DMECNS}

    @h2xmlp_description  {Dual-mic ECNS.\n
Maximum array size for this parameter \n
Version 0 --  448 bytes}
    @h2xmlp_toolPolicy   {NO_SUPPORT}

    @*/

struct voice_param_dmecns_t {

    int16_t Mode;
    /**< @h2xmle_description {Mode word controls the activation of various blocks of EC}
         @h2xmle_default     {-1549}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "-1549"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "-1549"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "0X0"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0X0"}

         @h2xmle_policy       {Basic} */

    int16_t tuning_mode;
    /**< @h2xmle_description {Speech frame can be replaced by 1 kHz sinusoidal frame depending on the internal status of EEC set by tuning_mode}
         @h2xmle_default     {64}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "64"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "64"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "0X0"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0X0"}

         @h2xmle_policy       {Basic} */

    int16_t echo_path_delay;
    /**< @h2xmle_description {Compensates the fixed delay in echo path}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF1_taps;
    /**< @h2xmle_description {Number of taps of AF filter}
         @h2xmle_default     {80}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Basic} */

    int16_t AF1_twoalpha;
    /**< @h2xmle_description {Step size to update the coefficients of the AF}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Basic} */

    int16_t AF1_erl;
    /**< @h2xmle_description {Adaptive step size control for AF filter}
         @h2xmle_default     {500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF1_preset_coefs;
    /**< @h2xmle_description {Indicates whether a preset of AF coefficients should be done}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF1_offset;
    /**< @h2xmle_description {Minimum power level used in computation}
         @h2xmle_default     {776}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_taps;
    /**< @h2xmle_description {Number of taps of AF filter}
         @h2xmle_default     {80}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_twoalpha;
    /**< @h2xmle_description {Step size to update the coefficients of the AF}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_erl;
    /**< @h2xmle_description {Adaptive step size control for AF filter}
         @h2xmle_default     {250}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_preset_coefs;
    /**< @h2xmle_description {Indicates whether a preset of AF coefficients should be done}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_offset;
    /**< @h2xmle_description {Minimum power level used in computation}
         @h2xmle_default     {776}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t PCD_twoalpha;
    /**< @h2xmle_description {Step size to update coefficients of path change detector}
         @h2xmle_default     {655}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t PCD_offset;
    /**< @h2xmle_description {Minimum power level used in computation}
         @h2xmle_default     {776}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t PCD_threshold;
    /**< @h2xmle_description {Controls sensitivity to detect of path changes}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_WgThreshold;
    /**< @h2xmle_description {Wind gush detection threshold}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_MpThreshold;
    /**< @h2xmle_description {Speech tracking threshold}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SF_init_table0 [8];
    /**< @h2xmle_description {Values preloaded based on nvSFFilterCoeffMic0}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SF_init_table1 [8];
    /**< @h2xmle_description {Values preloaded based on nvSFFilterCoeffMic1}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SF_taps;
    /**< @h2xmle_description {Number of taps of spatial filter}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SF_twoalpha;
    /**< @h2xmle_description {Convergence step size for Spatial filter}
         @h2xmle_default     {655}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoAlphaRev;
    /**< @h2xmle_description {Attenuation decay time in nonlinear echo cancellation}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoYcomp;
    /**< @h2xmle_description {Controls height of initial attenuation in nonlinear echo cancellation}
         @h2xmle_default     {32736}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_WbThreshold;
    /**< @h2xmle_description {Within beam threshold}
         @h2xmle_default     {4608}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaHi;
    /**< @h2xmle_description {Additional muting gain applied in DES during far-end only}
         @h2xmle_default     {10240}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaLo;
    /**< @h2xmle_description {Additional muting gain applied in DES during near-end only or double talk}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGammaS;
    /**< @h2xmle_description {Oversubtraction factor for noise estimation; used with DNNS_NoiseGainMinS}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGammaN;
    /**< @h2xmle_description {Oversubtraction factor for noise estimation; used with DNNS_NoiseGainMinN}
         @h2xmle_default     {11520}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGainMinS;
    /**< @h2xmle_description {Controls maximum amount of noise suppression}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGainMinN;
    /**< @h2xmle_description {Controls maximum amount of noise suppression}
         @h2xmle_default     {1536}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseBiasComp;
    /**< @h2xmle_description {Bias compensation for stationary noise suppression}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_AcThreshold;
    /**< @h2xmle_description {Near-end speech detection threshold}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_echo_ratio_2mic;
    /**< @h2xmle_description {Coupling factor between high band and low band echo}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Flen_1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {15}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Flen_2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {15}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Delay_1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Delay_2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Vad_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {24576}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Pow_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {512}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Meu_1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Meu_2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t FixCalFactorMic1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t FixCalFactorMic2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_Gamma_E;
    /**< @h2xmle_description {Oversubtraction factor for high band echo suppression}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_Gamma_SN;
    /**< @h2xmle_description {Oversubtraction factor for high band stationary noise suppression}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_Gamma_NN;
    /**< @h2xmle_description {Oversubtraction factor for high band nonstationary noise suppression}
         @h2xmle_default     {1024}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t NS_Fac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {11572}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t NS_BlockSize;
    /**< @h2xmle_description {..}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CS_OutputGain;
    /**< @h2xmle_description {Gain at the output of the Fluence algorithm on the TX path}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Mode;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt0_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt0_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1966}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt0_Delta;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt1_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt1_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1966}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt1_Delta;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt2_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt2_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {600}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt2_Delta;
    /**< @h2xmle_description {..}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt3_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt3_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt0_Mu_fac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt1_Mu_fac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt3_Mu_fac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt3_Mu_fac2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt4_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {24}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt4_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1638}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Cover_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Wind_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {13106}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_CD_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Vad2_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {22938}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Threshold;
    /**< @h2xmle_description {..}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_NS_Hover;
    /**< @h2xmle_description {..}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Pwr_SmAlpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Det_ESmAlpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {82}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_NS_EgOffset;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Sm_Ratio;
    /**< @h2xmle_description {..}
         @h2xmle_default     {328}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Det_Coefs[5];
    /**< @h2xmle_description {..}
         @h2xmle_default     {338}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Th1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {800}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Th2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WNR_Fq;
    /**< @h2xmle_description {..}
         @h2xmle_default     {800}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WNR_Dfc;
    /**< @h2xmle_description {..}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WNR_Sm_AlphaInc;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WNR_Sm_AlphaDec;
    /**< @h2xmle_description {..}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Fixed_Over_Est;
    /**< @h2xmle_description {..}
         @h2xmle_default     {19472}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover3;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover4;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover5;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_Level;
    /**< @h2xmle_description {..}
         @h2xmle_default     {10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_InA;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_InB;
    /**< @h2xmle_description {..}
         @h2xmle_default     {-3000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_A;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_B;
    /**< @h2xmle_description {..}
         @h2xmle_default     {-2500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Fx_DL;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Fvad_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Pscale;
    /**< @h2xmle_description {..}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Hover;
    /**< @h2xmle_description {..}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_AlphaS;
    /**< @h2xmle_description {..}
         @h2xmle_default     {3278}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_SdMax;
    /**< @h2xmle_description {..}
         @h2xmle_default     {3072}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_SdMin;
    /**< @h2xmle_description {..}
         @h2xmle_default     {2560}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_InitL;
    /**< @h2xmle_description {..}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_MaxVal;
    /**< @h2xmle_description {..}
         @h2xmle_default     {648}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_InitB;
    /**< @h2xmle_description {..}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_ResetB;
    /**< @h2xmle_description {..}
         @h2xmle_default     {410}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Avar;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Nc;
    /**< @h2xmle_description {..}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_SpMin;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1310}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_CFac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {328}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Bias;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Bias_Inp;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t NV_maxVadCount;
    /**< @h2xmle_description {..}
         @h2xmle_default     {35}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SM_NoiseGammaS;
    /**< @h2xmle_description {..}
         @h2xmle_default     {12288}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_PrSecAlpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {9830}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_PrBias;
    /**< @h2xmle_description {..}
         @h2xmle_default     {38}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_PrSecThresh;
    /**< @h2xmle_description {..}
         @h2xmle_default     {13106}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_NoiseGammaN;
    /**< @h2xmle_description {..}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_far;
    /**< @h2xmle_description {..}
         @h2xmle_default     {20000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_mic;
    /**< @h2xmle_description {..}
         @h2xmle_default     {20000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_x_clip;
    /**< @h2xmle_description {..}
         @h2xmle_default     {512}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NL_atten;
    /**< @h2xmle_description {..}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_CNI_level;
    /**< @h2xmle_description {..}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaAlpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaRescue;
    /**< @h2xmle_description {..}
         @h2xmle_default     {28672}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaDt;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_taps_bg;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_erl_bg;
    /**< @h2xmle_description {..}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t ePCD_threshold;
    /**< @h2xmle_description {..}
         @h2xmle_default     {18000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t minimum_erl;
    /**< @h2xmle_description {..}
         @h2xmle_default     {64}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t erl_step;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16800}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGammaInit;
    /**< @h2xmle_description {..}
         @h2xmle_default     {4608}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t MF_NoiseGammaFac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {14592}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t E_NoiseGammaFac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCountParam;
    /**< @h2xmle_description {..}
         @h2xmle_default     {500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_FF_Coeffs0[32];
    /**< @h2xmle_description {..}
         @h2xmle_default     {65}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_FF_Coeffs1[32];
    /**< @h2xmle_description {..}
         @h2xmle_default     {90}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "-151"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "90"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "-151"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "-151"}

         @h2xmle_policy       {Basic} */

    int16_t Reserved1;
    /**< @h2xmle_description {Client should set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int16_t Reserved2;
    /**< @h2xmle_description {Client should set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int16_t Reserved3;
    /**< @h2xmle_description {Client should set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int16_t Reserved4;
    /**< @h2xmle_description {Client should set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int16_t Reserved5;
    /**< @h2xmle_description {Client should set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FLECNS 0x00010E20

#include "adsp_begin_pack.h"

typedef struct voice_param_flecns_t voice_param_flecns_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLECNS", VOICE_PARAM_FLECNS}

    @h2xmlp_description  {Fluence dual-mic and tri-mic ECNS.\n
Maximum array size for this parameter \n
Version 0 – 468 bytes \n} */

struct voice_param_flecns_t {

    int16_t Mode;
    /**< @h2xmle_description {Mode word controls the activation of various blocks of EC}
         @h2xmle_default     {-1549}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "-1549"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "-1549"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "0X0"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0X0"}

         @h2xmle_policy       {Basic} */

    int16_t tuning_mode;
    /**< @h2xmle_description {Speech frame can be replaced by 1 kHz sinusoidal frame depending on the internal status of EEC set by tuning_mode}
         @h2xmle_default     {64}
         @h2xmle_range       {-32768..32767}

         @h2xmle_defaultDependency   {  Samplerate = "8000",  default = "64"}
         @h2xmle_defaultDependency   {  Samplerate = "16000",  default = "64"}
         @h2xmle_defaultDependency   {  Samplerate = "32000",  default = "0X0"}
         @h2xmle_defaultDependency   {  Samplerate = "48000",  default = "0X0"}

         @h2xmle_policy       {Basic} */

    int16_t echo_path_delay;
    /**< @h2xmle_description {Compensates the fixed delay in echo path}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF1_taps;
    /**< @h2xmle_description {Number of taps of AF filter}
         @h2xmle_default     {80}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF1_twoalpha;
    /**< @h2xmle_description {Step size to update the coefficients of the AF}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF1_erl;
    /**< @h2xmle_description {Adaptive step size control for AF filter}
         @h2xmle_default     {500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF1_preset_coefs;
    /**< @h2xmle_description {Indicates whether a preset of AF coefficients should be done}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF1_offset;
    /**< @h2xmle_description {Minimum power level used in computation}
         @h2xmle_default     {776}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_taps;
    /**< @h2xmle_description {Number of taps of AF filter}
         @h2xmle_default     {80}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_twoalpha;
    /**< @h2xmle_description {Step size to update the coefficients of the AF}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_erl;
    /**< @h2xmle_description {Adaptive step size control for AF filter}
         @h2xmle_default     {250}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_preset_coefs;
    /**< @h2xmle_description {Indicates whether a preset of AF coefficients should be done}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF2_offset;
    /**< @h2xmle_description {Minimum power level used in computation}
         @h2xmle_default     {776}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t PCD_twoalpha;
    /**< @h2xmle_description {Step size to update coefficients of path change detector}
         @h2xmle_default     {655}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t PCD_offset;
    /**< @h2xmle_description {Minimum power level used in computation}
         @h2xmle_default     {776}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t PCD_threshold;
    /**< @h2xmle_description {Controls sensitivity to detect of path changes}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_WgThreshold;
    /**< @h2xmle_description {Wind gush detection threshold}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_MpThreshold;
    /**< @h2xmle_description {Speech tracking threshold}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SF_init_table0 [8];
    /**< @h2xmle_description {Values preloaded based on nvSFFilterCoeffMic0}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SF_init_table1 [8];
    /**< @h2xmle_description {Values preloaded based on nvSFFilterCoeffMic1}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SF_taps;
    /**< @h2xmle_description {Number of taps of spatial filter}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SF_twoalpha;
    /**< @h2xmle_description {Convergence step size for Spatial filter}
         @h2xmle_default     {655}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoAlphaRev;
    /**< @h2xmle_description {Attenuation decay time in nonlinear echo cancellation}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoYcomp;
    /**< @h2xmle_description {Controls height of initial attenuation in nonlinear echo cancellation}
         @h2xmle_default     {32736}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_WbThreshold;
    /**< @h2xmle_description {Within beam threshold}
         @h2xmle_default     {4608}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaHi;
    /**< @h2xmle_description {Additional muting gain applied in DES during far-end only}
         @h2xmle_default     {10240}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaLo;
    /**< @h2xmle_description {Additional muting gain applied in DES during near-end only or double talk}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGammaS;
    /**< @h2xmle_description {Oversubtraction factor for noise estimation; used with DNNS_NoiseGainMinS}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGammaN;
    /**< @h2xmle_description {Oversubtraction factor for noise estimation; used with DNNS_NoiseGainMinN}
         @h2xmle_default     {11520}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGainMinS;
    /**< @h2xmle_description {Controls maximum amount of noise suppression}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGainMinN;
    /**< @h2xmle_description {Controls maximum amount of noise suppression}
         @h2xmle_default     {1536}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseBiasComp;
    /**< @h2xmle_description {Bias compensation for stationary noise suppression}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_AcThreshold;
    /**< @h2xmle_description {Near-end speech detection threshold}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_echo_ratio_2mic;
    /**< @h2xmle_description {Coupling factor between high band and low band echo}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Flen_1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {15}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Flen_2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {15}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Delay_1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Delay_2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {3}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Vad_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {24576}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Pow_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {512}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Meu_1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCODEC_Meu_2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t FixCalFactorMic1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t FixCalFactorMic2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_Gamma_E;
    /**< @h2xmle_description {Oversubtraction factor for high band echo suppression}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_Gamma_SN;
    /**< @h2xmle_description {Oversubtraction factor for high band stationary noise suppression}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WB_Gamma_NN;
    /**< @h2xmle_description {Oversubtraction factor for high band nonstationary noise suppression}
         @h2xmle_default     {1024}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t NS_Fac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {11572}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t NS_BlockSize;
    /**< @h2xmle_description {..}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CS_OutputGain;
    /**< @h2xmle_description {Gain at the output of the Fluence algorithm on the TX path}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Mode;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt0_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt0_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1966}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt0_Delta;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt1_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt1_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1966}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt1_Delta;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt2_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt2_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {600}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt2_Delta;
    /**< @h2xmle_description {..}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt3_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt3_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt0_Mu_fac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt1_Mu_fac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt3_Mu_fac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt3_Mu_fac2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt4_M;
    /**< @h2xmle_description {..}
         @h2xmle_default     {24}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Filt4_Mu;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1638}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Cover_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Wind_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {13106}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_CD_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CB_Vad2_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {22938}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Threshold;
    /**< @h2xmle_description {..}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_NS_Hover;
    /**< @h2xmle_description {..}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Pwr_SmAlpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Det_EsmAlpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {82}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_NS_EgOffset;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Sm_Ratio;
    /**< @h2xmle_description {..}
         @h2xmle_default     {328}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Det_Coefs [5];
    /**< @h2xmle_description {..}
         @h2xmle_default     {338}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Th1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {800}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_Th2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WNR_Fq;
    /**< @h2xmle_description {..}
         @h2xmle_default     {800}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WNR_Dfc;
    /**< @h2xmle_description {..}
         @h2xmle_default     {20}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WNR_Sm_AlphaInc;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WNR_Sm_AlphaDec;
    /**< @h2xmle_description {..}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Fixed_Over_Est;
    /**< @h2xmle_description {..}
         @h2xmle_default     {19472}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover1;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover3;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover4;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Hover5;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_Level;
    /**< @h2xmle_description {..}
         @h2xmle_default     {10}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_InA;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_InB;
    /**< @h2xmle_description {..}
         @h2xmle_default     {-3000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_A;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Cf_B;
    /**< @h2xmle_description {..}
         @h2xmle_default     {-2500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Fx_DL;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Fvad_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Th;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Pscale;
    /**< @h2xmle_description {..}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Hover;
    /**< @h2xmle_description {..}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_AlphaS;
    /**< @h2xmle_description {..}
         @h2xmle_default     {3278}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_SdMax;
    /**< @h2xmle_description {..}
         @h2xmle_default     {3072}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_SdMin;
    /**< @h2xmle_description {..}
         @h2xmle_default     {2560}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_InitL;
    /**< @h2xmle_description {..}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_MaxVal;
    /**< @h2xmle_description {..}
         @h2xmle_default     {648}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_InitB;
    /**< @h2xmle_description {..}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_ResetB;
    /**< @h2xmle_description {..}
         @h2xmle_default     {410}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Avar;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Nc;
    /**< @h2xmle_description {..}
         @h2xmle_default     {6554}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_SpMin;
    /**< @h2xmle_description {..}
         @h2xmle_default     {1310}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Sc_Cfac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {328}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Bias;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_Bias_Inp;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32767}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t NV_maxVadCount;
    /**< @h2xmle_description {..}
         @h2xmle_default     {35}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SM_NoiseGammaS;
    /**< @h2xmle_description {..}
         @h2xmle_default     {12288}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_PrSecAlpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {9830}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_PrBias;
    /**< @h2xmle_description {..}
         @h2xmle_default     {38}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_PrSecThresh;
    /**< @h2xmle_description {..}
         @h2xmle_default     {13106}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t WND_NoiseGammaN;
    /**< @h2xmle_description {..}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_far;
    /**< @h2xmle_description {..}
         @h2xmle_default     {20000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_mic;
    /**< @h2xmle_description {..}
         @h2xmle_default     {20000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SPDET_x_clip;
    /**< @h2xmle_description {..}
         @h2xmle_default     {512}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NL_atten;
    /**< @h2xmle_description {..}
         @h2xmle_default     {100}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_CNI_level;
    /**< @h2xmle_description {..}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaAlpha;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaRescue;
    /**< @h2xmle_description {..}
         @h2xmle_default     {28672}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_EchoGammaDt;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_taps_bg;
    /**< @h2xmle_description {..}
         @h2xmle_default     {32}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_erl_bg;
    /**< @h2xmle_description {..}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t ePCD_threshold;
    /**< @h2xmle_description {..}
         @h2xmle_default     {18000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t minimum_erl;
    /**< @h2xmle_description {..}
         @h2xmle_default     {64}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t erl_step;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16800}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NoiseGammaInit;
    /**< @h2xmle_description {..}
         @h2xmle_default     {4608}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t MF_NoiseGammaFac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {14592}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t E_NoiseGammaFac;
    /**< @h2xmle_description {..}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t VCountParam;
    /**< @h2xmle_description {..}
         @h2xmle_default     {500}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_FF_Coeffs0 [32];
    /**< @h2xmle_description {..}
         @h2xmle_default     {65}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t IS_FF_Coeffs1 [32];
    /**< @h2xmle_description {..}
         @h2xmle_default     {0xFF07}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t ENH_Vad_Th2;
    /**< @h2xmle_description {..}
         @h2xmle_default     {28672}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t HM_Mode;
    /**< @h2xmle_description {..}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t HM_Thresh;
    /**< @h2xmle_description {..}
         @h2xmle_default     {3246}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t HM_HangMax;
    /**< @h2xmle_description {..}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t HM_Gamma;
    /**< @h2xmle_description {..}
         @h2xmle_default     {18022}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t HM_CalFactor;
    /**< @h2xmle_description {..}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Clean_Thresh;
    /**< @h2xmle_description {..}
         @h2xmle_default     {150}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Clean_Block;
    /**< @h2xmle_description {..}
         @h2xmle_default     {200}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t CS_OutputGain_BTSM;
    /**< @h2xmle_description {..}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Reserved1;
    /**< @h2xmle_description {Client must set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int16_t Reserved2;
    /**< @h2xmle_description {Client must set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int16_t Reserved3;
    /**< @h2xmle_description {Client must set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int16_t Reserved4;
    /**< @h2xmle_description {Client must set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Reserved5;
    /**< @h2xmle_description {Client must set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

    int16_t Reserved6;
    /**< @h2xmle_description {Client must set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_readOnly       {true} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FL_STATE 0x00010E1D

#include "adsp_begin_pack.h"

typedef struct voice_param_fl_state_t voice_param_fl_state_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FL_STATE", VOICE_PARAM_FL_STATE}
    @h2xmlp_description {Fluence dual-mic and Fluence active noise cancellation (ANC) tri-mic ECNS.\n
Maximum array size for this read-only parameter \n
Version 0 – 240 bytes \n}

    @h2xmlp_toolPolicy  {RTM} */

struct voice_param_fl_state_t {

    int16_t fbcf_vad0;
    /**< @h2xmle_description {fbcf_vad0} */

    int16_t fbcf_vad1;
    /**< @h2xmle_description {fbcf_vad1} */

    int16_t fbcf_filt_len;
    /**< @h2xmle_description {fbcf_filt_len} */

    int16_t fwcf_filt_len;
    /**< @h2xmle_description {fwcf_filt_len} */

    int32_t fbcf_coeffsl_32q28_0 [15];
    /**< @h2xmle_description {fbcf_coeffsl_32q28_0 [15]} */

    int32_t reserved1;
    /**< @h2xmle_description {reserved1} */

    int32_t fbcf_coeffsl_32q28_1 [15];
    /**< @h2xmle_description {fbcf_coeffsl_32q28_1 [15]} */

    int32_t reserved2;
    /**< @h2xmle_description {reserved2} */

    int32_t fwcf_coeffsl_32q24 [15];
    /**< @h2xmle_description {fwcf_coeffsl_32q24 [15]} */

    int16_t is_state;
    /**< @h2xmle_description {is_state} */

    int16_t is_indicator;
    /**< @h2xmle_description {is_indicator} */

    int16_t is_indicator_inp;
    /**< @h2xmle_description {is_indicator_inp} */

    int16_t is_vad;
    /**< @h2xmle_description {is_vad} */

    int16_t is_clean_det;
    /**< @h2xmle_description {is_clean_det} */

    int16_t is_aec_vad;
    /**< @h2xmle_description {is_aec_vad} */

    int32_t is_calfactor;
    /**< @h2xmle_description {is_calfactor} */

    int32_t dnns_estatus;
    /**< @h2xmle_description {dnns_estatus} */

    int32_t dnns_status;
    /**< @h2xmle_description {dnns_status} */

    int32_t wnd_meas;
    /**< @h2xmle_description {wnd_meas} */

    int32_t wnd_score;
    /**< @h2xmle_description {wnd_score} */

    int16_t wnd_pri_wind_q15;
    /**< @h2xmle_description {wnd_pri_wind_q15} */

    int16_t wnd_smratio;
    /**< @h2xmle_description {wnd_smratio} */

    int32_t wnr_freqc;
    /**< @h2xmle_description {wnr_freqc} */

    int16_t hm_vad;
    /**< @h2xmle_description {hm_vad} */

    int16_t reserved3;
    /**< @h2xmle_description {reserved3} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_FLECNS_EXT 0x00010E1F

#include "adsp_begin_pack.h"

typedef struct voice_param_flecns_ext_t voice_param_flecns_ext_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLECNS_EXT", VOICE_PARAM_FLECNS_EXT}

    @h2xmlp_description  {Fluence dual-mic and tri-mic ECNS with versioning. \n
Maximum array size for this parameter \n
Version 0 – 92 bytes \n}
@h2xmlp_version     {0} */

struct voice_param_flecns_ext_t {

    int16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}
         @h2xmle_isVersion    {true}


         @h2xmle_readOnly       {true} */

    int16_t Clean_Hangover;
    /**< @h2xmle_description {Clean_Hangover}
         @h2xmle_default     {50}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t BG_Cln_Thresh_Shift;
    /**< @h2xmle_description {BG_Cln_Thresh_Shift}
         @h2xmle_default     {15}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t BG_NS_Thresh_Shift;
    /**< @h2xmle_description {BG_NS_Thresh_Shift}
         @h2xmle_default     {9}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t BG_Alpha;
    /**< @h2xmle_description {BG_Alpha}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t Mic_Gain_Shift;
    /**< @h2xmle_description {Mic_Gain_Shift}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t Fvad_Hover;
    /**< @h2xmle_description {Fvad_Hover}
         @h2xmle_default     {30}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t IS_Mode;
    /**< @h2xmle_description {IS_Mode}
         @h2xmle_default     {768}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t V4_Mode;
    /**< @h2xmle_description {V4_Mode}
         @h2xmle_default     {28928}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t CS_AFpostGain1;
    /**< @h2xmle_description {CS_AFpostGain1}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t CS_AFpostGain2;
    /**< @h2xmle_description {CS_AFpostGain2}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t E_NoiseGammaSFac;
    /**< @h2xmle_description {E_NoiseGammaSFac}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SM_NoiseGainMinS;
    /**< @h2xmle_description {SM_NoiseGainMinS}
         @h2xmle_default     {1536}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGammaS_Clean;
    /**< @h2xmle_description {DNNS_NoiseGammaS_Clean}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGammaN_Clean;
    /**< @h2xmle_description {DNNS_NoiseGammaN_Clean}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGainMinS_Clean;
    /**< @h2xmle_description {DNNS_NoiseGainMinS_Clean}
         @h2xmle_default     {10496}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGainMinN_Clean;
    /**< @h2xmle_description {DNNS_NoiseGainMinN_Clean}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SM_NoiseGammaS_Clean;
    /**< @h2xmle_description {SM_NoiseGammaS_Clean}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SM_NoiseGainMinS_Clean;
    /**< @h2xmle_description {SM_NoiseGainMinS_Clean}
         @h2xmle_default     {10496}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGammaS_Turbo;
    /**< @h2xmle_description {DNNS_NoiseGammaS_Turbo;}
         @h2xmle_default     {12288}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGammaN_Turbo;
    /**< @h2xmle_description {DNNS_NoiseGammaN_Turbo;}
         @h2xmle_default     {12288}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGainMinS_Turbo;
    /**< @h2xmle_description {DNNS_NoiseGainMinS_Turbo;}
         @h2xmle_default     {1536}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGainMinN_Turbo;
    /**< @h2xmle_description {DNNS_NoiseGainMinN_Turbo;}
         @h2xmle_default     {1536}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SM_NoiseGammaS_Turbo;
    /**< @h2xmle_description {SM_NoiseGammaS_Turbo;}
         @h2xmle_default     {12288}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SM_NoiseGainMinS_Turbo;
    /**< @h2xmle_description {SM_NoiseGainMinS_Turbo;}
         @h2xmle_default     {1536}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGammaS_HighBG;
    /**< @h2xmle_description {DNNS_NoiseGammaS_HighBG;}
         @h2xmle_default     {9216}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGammaN_HighBG;
    /**< @h2xmle_description {DNNS_NoiseGammaN_HighBG;}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGainMinS_HighBG;
    /**< @h2xmle_description {DNNS_NoiseGainMinS_HighBG}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGainMinN_HighBG;
    /**< @h2xmle_description {DNNS_NoiseGainMinN_HighBG}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SM_NoiseGammaS_HighBG;
    /**< @h2xmle_description {SM_NoiseGammaS_HighBG;}
         @h2xmle_default     {10240}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SM_NoiseGainMinS_HighBG;
    /**< @h2xmle_description {SM_NoiseGainMinS_HighBG;}
         @h2xmle_default     {2048}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGamma_Step;
    /**< @h2xmle_description {DNNS_NoiseGamma_Step;}
         @h2xmle_default     {80}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NoiseGainMin_Step;
    /**< @h2xmle_description {DNNS_NoiseGainMin_Step;}
         @h2xmle_default     {256}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NR2_Alpha;
    /**< @h2xmle_description {DNNS_NR2_Alpha;}
         @h2xmle_default     {32440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NR2_Scale;
    /**< @h2xmle_description {DNNS_NR2_Scale;}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t DNNS_NR2_Thresh;
    /**< @h2xmle_description {DNNS_NR2_Thresh;}
         @h2xmle_default     {19660}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t DNNS_NR2_Beta;
    /**< @h2xmle_description {DNNS_NR2_Beta;}
         @h2xmle_default     {29490}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t WB_NR2_Scale;
    /**< @h2xmle_description {WB_NR2_Scale;}
         @h2xmle_default     {4096}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF1_high_taps;
    /**< @h2xmle_description {AF1_high_taps;}
         @h2xmle_default     {80}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF1_high_twoalpha;
    /**< @h2xmle_description {AF1_high_twoalpha}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF1_high_erl;
    /**< @h2xmle_description {AF1_high_erl;}
         @h2xmle_default     {250}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF1_high_offset;
    /**< @h2xmle_description {AF1_high_offset;}
         @h2xmle_default     {776}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t WB_Echo_Scale;
    /**< @h2xmle_description {WB_Echo_Scale;}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t WB_EchoAlphaRev;
    /**< @h2xmle_description {WB_EchoAlphaRev;}
         @h2xmle_default     {12288}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t WB_EchoYcomp;
    /**< @h2xmle_description {WB_EchoYcomp;}
         @h2xmle_default     {32000}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t Rx_Ref_Gain;
    /**< @h2xmle_description {Rx_Ref_Gain;}
         @h2xmle_default     {8192}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

}
#include "adsp_end_pack.h"
;



#define VOICE_MODULE_DMECNS  0x00010EE1
/**
    @h2xmlm_module       {"VOICE_MODULE_DMECNS",
                          VOICE_MODULE_DMECNS}
    @h2xmlm_displayName  {"TX VOICE DMECNS"}
	@h2xmlm_description {Dual-mic ECNS in the voice processor on the Tx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_DMECNS (supported only for backward compatability)\n
VOICE_PARAM_FLECNS\n
VOICE_PARAM_AF_COEFFS\n
VOICE_PARAM_FL_STATE\n
VOICE_PARAM_FLECNS_EXT
\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_mod_enable_t::Enable}
    @h2xmle_default     {1}

    @h2xml_Select					{voice_param_dmecns_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_flecns_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_af_coeffs_dm_fluence_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_fl_state_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_flecns_ext_t}
    @h2xmlm_InsertParameter


   @}                   <-- End of the Module -->*/
#endif
