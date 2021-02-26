#ifndef SMECNS_V2_CALIB_H
#define SMECNS_V2_CALIB_H
/*==============================================================================
  @file smecns_v2_calib.h
  @brief This file contains SMECNS_V2
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/28/17   shrani     Added h2xml comments.
 ========================================================================== */
#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"
#include "ecns_common_param_calib.h"
#include "audio_rtm_logging_param_calib.h"


/** @h2xml_title1           {VOICE_MODULE_SMECNS_V2}
    @h2xml_title_agile_rev  {VOICE_MODULE_SMECNS_V2}
    @h2xml_title_date       {March 13, 2017} */







#define VOICE_PARAM_SMECNS_V2_EC_DELAY 0x00010E63

#include "adsp_begin_pack.h"

typedef struct voice_param_smecns_v2_ec_delay_t voice_param_smecns_v2_ec_delay_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_SMECNS_V2_EC_DELAY", VOICE_PARAM_SMECNS_V2_EC_DELAY}

    @h2xmlp_description  {Fixed echo path delay in the firmware. The delay is in multiples of 8 kHz samples.\n
Maximum array size for this parameter\n
Version 0 – 4 bytes \n\n} */

struct voice_param_smecns_v2_ec_delay_t {

    int16_t Echo_Path_Delay;
    /**< @h2xmle_description {Compensates the fixed delay in echo path}
         @h2xmle_default     {1}
         @h2xmle_range       {-512..512}


         @h2xmle_policy       {Basic} */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this path to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_readOnly    {true} */


}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_SMECNS_V2_NS 0x00010E62

#include "adsp_begin_pack.h"

typedef struct voice_param_smecns_v2_ns_t voice_param_smecns_v2_ns_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_SMECNS_V2_NS", VOICE_PARAM_SMECNS_V2_NS}

    @h2xmlp_description  {Single-mic ECNS version 2 for noise suppression.\n
Maximum array size for this parameter\n
Version 0 -- 96 bytes\n\n}
@h2xmlp_version     {0}*/

struct voice_param_smecns_v2_ns_t {

    int16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion    {true}


         @h2xmle_readOnly             {true} */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly             {true} */

    int32_t VAD_Mode;
    /**< @h2xmle_description {VAD_Mode}
         @h2xmle_default     {513}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t SM_Vad_ThreshQ8;
    /**< @h2xmle_description {SM_Vad_ThreshQ8}
         @h2xmle_default     {512}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t SM_Vad_Hover;
    /**< @h2xmle_description {SM_Vad_Hover}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t SM_Vad_BlkSize1;
    /**< @h2xmle_description {SM_Vad_BlkSize1}
         @h2xmle_default     {5}
         @h2xmle_range       {1..12}


         @h2xmle_policy       {Advanced} */

    int16_t SM_Vad_BlkSize2;
    /**< @h2xmle_description {SM_Vad_BlkSize2}
         @h2xmle_default     {10}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int32_t PP_Mode;
    /**< @h2xmle_description {PP_Mode}
         @h2xmle_default     {-1073704384}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t PP_Debug_Mode;
    /**< @h2xmle_description {PP_Debug_Mode}
         @h2xmle_default     {0}
         @h2xmle_range       {-2147483648..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Mode2;
    /**< @h2xmle_description {PP_Mode2}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t SN_BlkSize1;
    /**< @h2xmle_description {SN_BlkSize1}
         @h2xmle_default     {10}
         @h2xmle_range       {1..12}


         @h2xmle_policy       {Advanced} */

    int16_t SN_BlkSize2;
    /**< @h2xmle_description {SN_BlkSize2}
         @h2xmle_default     {10}
         @h2xmle_range       {1..100}


         @h2xmle_policy       {Advanced} */

    int16_t VN_Alpha1;
    /**< @h2xmle_description {VN_Alpha1}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t VN_Alpha2;
    /**< @h2xmle_description {VN_Alpha2}
         @h2xmle_default     {32440}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t VN_Gamma;
    /**< @h2xmle_description {VN_Gamma}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t IM_Thresh;
    /**< @h2xmle_description {IM_Thresh}
         @h2xmle_default     {25000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t IM_Gamma;
    /**< @h2xmle_description {IM_Gamma}
         @h2xmle_default     {9830}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SN_Var_Gamma;
    /**< @h2xmle_description {SN_Var_Gamma}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t SN_Var_Decay;
    /**< @h2xmle_description {SN_Var_Decay}
         @h2xmle_default     {32700}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_LF_Idx;
    /**< @h2xmle_description {PP_LF_Idx}
         @h2xmle_default     {16}
         @h2xmle_range       {0..255}


         @h2xmle_policy       {Basic} */

    int16_t PP_MF_Idx;
    /**< @h2xmle_description {PP_MF_Idx}
         @h2xmle_default     {65}
         @h2xmle_range       {0..255}


         @h2xmle_policy       {Basic} */

    int16_t PP_HF_Idx;
    /**< @h2xmle_description {PP_HF_Idx}
         @h2xmle_default     {128}
         @h2xmle_range       {0..255}


         @h2xmle_policy       {Basic} */

    int16_t PP_SM_Gamma_LF;
    /**< @h2xmle_description {PP_SM_Gamma_LF}
         @h2xmle_default     {13500}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_SM_Gamma_MF;
    /**< @h2xmle_description {PP_SM_Gamma_MF}
         @h2xmle_default     {12000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_SM_Gamma_HF;
    /**< @h2xmle_description {PP_SM_Gamma_HF}
         @h2xmle_default     {11000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_SM_Min_Gain;
    /**< @h2xmle_description {PP_SM_Min_Gain}
         @h2xmle_default     {512}
         @h2xmle_range       {100..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_Comb_Gain_Alpha1;
    /**< @h2xmle_description {PP_Comb_Gain_Alpha1}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Comb_Gain_Alpha2;
    /**< @h2xmle_description {PP_Comb_Gain_Alpha2}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Gain_Alpha_Idx;
    /**< @h2xmle_description {PP_Gain_Alpha_Idx}
         @h2xmle_default     {8}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t CNI_Level;
    /**< @h2xmle_description {CNI_Level}
         @h2xmle_default     {20}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t CNI_Spec_Roll;
    /**< @h2xmle_description {CNI_Spec_Roll}
         @h2xmle_default     {32416}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_SMT_Mode;
    /**< @h2xmle_description {PP_SMT_Mode}
         @h2xmle_default     {14}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_SMT_Window;
    /**< @h2xmle_description {PP_SMT_Window}
         @h2xmle_default     {50}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_SMT_LF_Idx;
    /**< @h2xmle_description {PP_SMT_LF_Idx}
         @h2xmle_default     {24}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_SMT_MF_Idx;
    /**< @h2xmle_description {PP_SMT_MF_Idx}
         @h2xmle_default     {64}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int32_t PP_SMT_Th1;
    /**< @h2xmle_description {PP_SMT_Th1}
         @h2xmle_default     {100000000}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t PP_SMT_Th2;
    /**< @h2xmle_description {PP_SMT_Th2}
         @h2xmle_default     {950000000}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

    int32_t PP_SMT_Th3;
    /**< @h2xmle_description {PP_SMT_Th3}
         @h2xmle_default     {950000000}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t PP_SMT_NS_Gamma;
    /**< @h2xmle_description {PP_SMT_NS_Gamma}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_SMT_SVad_Th;
    /**< @h2xmle_description {PP_SMT_SVad_Th}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_SM_Gain_Boost;
    /**< @h2xmle_description {PP_SM_Gain_Boost}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t Reserved2;
    /**< @h2xmle_description {Reserved2}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly             {true} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_SMECNS_V2_EC 0x00010E61

#include "adsp_begin_pack.h"

typedef struct voice_param_smecns_v2_ec_t voice_param_smecns_v2_ec_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_SMECNS_V2_EC", VOICE_PARAM_SMECNS_V2_EC}

    @h2xmlp_description  {Single-mic ECNS version 2 for echo cancellation.\n
Maximum array size for this parameter\n
Version 0 -- 156 bytes
Version 1 extension -- 160 bytes
Version 2 extension -- 168 bytes \n\n}
@h2xmlp_version     {2}
 */

struct voice_param_smecns_v2_ec_t {

    int16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_default     {2}
         @h2xmle_range       {0..2}
         @h2xmle_isVersion    {true}


         @h2xmle_readOnly            {true}  */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly            {true} */

    int32_t Mode;
    /**< @h2xmle_description {Mode}
         @h2xmle_default     {8389421}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "8389420"}
         @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "8389421"}
         @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "8389422"}
         @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "25166636"}

         @h2xmle_policy       {Basic} */

    int16_t Input_GainQ13;
    /**< @h2xmle_description {Input_GainQ13}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_Post_GainQ11;
    /**< @h2xmle_description {AF_Post_GainQ11}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Output_GainQ11;
    /**< @h2xmle_description {Output_GainQ11}
         @h2xmle_default     {0}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_Taps;
    /**< @h2xmle_description {AF_Taps}
         @h2xmle_default     {100}
         @h2xmle_range       {4..400}


         @h2xmle_policy       {Basic} */

    int16_t AF_Taps_Qfac;
    /**< @h2xmle_description {AF_Taps_Qfac}
         @h2xmle_default     {1}
         @h2xmle_range      {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_Mu;
    /**< @h2xmle_description {AF_Mu}
         @h2xmle_default     {6}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int32_t AF_Eng_Offset;
    /**< @h2xmle_description {AF_Eng_Offset}
         @h2xmle_default     {687865856}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t AF_Mu_DT;
    /**< @h2xmle_description {AF_Mu_DT}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_Tests;
    /**< @h2xmle_description {AF_Tests}
         @h2xmle_default     {3}
         @h2xmle_range       {0..10}


         @h2xmle_policy       {Basic} */

    int16_t AF_BG_ERLE_Thresh;
    /**< @h2xmle_description {AF_BG_ERLE_Thresh}
         @h2xmle_default     {0x4000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_BG_FG_Thresh;
    /**< @h2xmle_description {AF_BG_FG_Thresh}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_Conf_Frames;
    /**< @h2xmle_description {AF_Conf_Frames}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_DT_Thresh;
    /**< @h2xmle_description {AF_DT_Thresh}
         @h2xmle_default     {29440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_DT_Thresh;
    /**< @h2xmle_description {PP_DT_Thresh}
         @h2xmle_default     {29440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_DT_Hangover;
    /**< @h2xmle_description {AF_DT_Hangover}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_DT_BiasFactor;
    /**< @h2xmle_description {AF_DT_BiasFactor}
         @h2xmle_default     {400}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_DT_Eng_Alpha;
    /**< @h2xmle_description {AF_DT_Eng_Alpha}
         @h2xmle_default     {3}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_Init_Frames;
    /**< @h2xmle_description {AF_Init_Frames}
         @h2xmle_default     {20}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_Init_DT_Frames;
    /**< @h2xmle_description {AF_Init_DT_Frames}
         @h2xmle_default     {300}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_DT_Mode;
    /**< @h2xmle_description {AF_DT_Mode}
         @h2xmle_default     {29440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_HB_Taps;
    /**< @h2xmle_description {AF_HB_Taps}
         @h2xmle_default     {100}
         @h2xmle_range       {0x4..0x190}


         @h2xmle_policy       {Basic} */

    int16_t AF_HB_Taps_Qfac;
    /**< @h2xmle_description {AF_HB_Taps_Qfac}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_HB_Mu;
    /**< @h2xmle_description {AF_HB_Mu}
         @h2xmle_default     {6}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int32_t AF_HB_Eng_Offset;
    /**< @h2xmle_description {AF_HB_Eng_Offset}
         @h2xmle_default     {687865856}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Advanced} */

    int16_t AF_HB_Mu_DT;
    /**< @h2xmle_description {AF_HB_Mu_DT}
         @h2xmle_default     {4}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_HB_Tests;
    /**< @h2xmle_description {AF_HB_Tests}
         @h2xmle_default     {3}
         @h2xmle_range       {0..10}

         @h2xmle_policy       {Basic} */

    int16_t AF_HB_BG_ERLE_Thresh;
    /**< @h2xmle_description {AF_HB_BG_ERLE_Thresh}
         @h2xmle_default     {0x4000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_HB_BG_FG_Thresh;
    /**< @h2xmle_description {AF_HB_BG_FG_Thresh}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_HB_Conf_Frames;
    /**< @h2xmle_description {AF_HB_Conf_Frames}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_HB_DT_Thresh;
    /**< @h2xmle_description {AF_HB_DT_Thresh}
         @h2xmle_default     {29440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_HB_DT_Thresh;
    /**< @h2xmle_description {PP_HB_DT_Thresh}
         @h2xmle_default     {29440}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t AF_HB_DT_Hangover;
    /**< @h2xmle_description {AF_HB_DT_Hangover}
         @h2xmle_default     {1}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_HB_DT_BiasFactor;
    /**< @h2xmle_description {AF_HB_DT_BiasFactor}
         @h2xmle_default     {400}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_HB_Init_Frames;
    /**< @h2xmle_description {AF_HB_Init_Frames}
         @h2xmle_default     {20}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t AF_HB_Init_DT_Frames;
    /**< @h2xmle_description {AF_HB_Init_DT_Frames}
         @h2xmle_default     {300}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t Reserved2;
    /**< @h2xmle_description {Reserved2}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly            {true} */

    int32_t AF_Far_VAD;
    /**< @h2xmle_description {AF_Far_VAD}
         @h2xmle_default     {32000}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t AF_HB_Far_VAD;
    /**< @h2xmle_description {AF_HB_Far_VAD}
         @h2xmle_default     {32000}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Basic} */

    int32_t PP_Far_VAD;
    /**< @h2xmle_description {PP_Far_VAD}
         @h2xmle_default     {32000}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Basic} */

    int16_t PP_Far_VAD_Hangover;
    /**< @h2xmle_description {PP_Far_VAD_Hangover}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PCD_Thresh;
    /**< @h2xmle_description {PCD_Thresh}
         @h2xmle_default     {27305}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PCD_Eng_Factor;
    /**< @h2xmle_description {PCD_Eng_Factor}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PCD_Wgt_Factor;
    /**< @h2xmle_description {PCD_Wgt_Factor}
         @h2xmle_default     {6554}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PCD_Eng_Factor2;
    /**< @h2xmle_description {PCD_Eng_Factor2}
         @h2xmle_default     {9830}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Gamma_PathChange;
    /**< @h2xmle_description {PP_Gamma_PathChange}
         @h2xmle_default     {32767}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_PathChange_AggQ;
    /**< @h2xmle_description {PP_PathChange_AggQ}
         @h2xmle_default     {3}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_NumBands;
    /**< @h2xmle_description {PP_NumBands}
         @h2xmle_default     {32}
         @h2xmle_range       {8..128}


         @h2xmle_policy       {Advanced} */

    int16_t PP_BandRes;
    /**< @h2xmle_description {PP_BandRes}
         @h2xmle_default     {0x02BC}
         @h2xmle_range       {700..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_MinGain;
    /**< @h2xmle_description {PP_MinGain}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Gamma_e_High;
    /**< @h2xmle_description {PP_Gamma_e_High}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_Gamma_e_DT;
    /**< @h2xmle_description {PP_Gamma_e_DT}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_AggQ;
    /**< @h2xmle_description {PP_AggQ}
         @h2xmle_default     {2}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_Gamma_NL;
    /**< @h2xmle_description {PP_Gamma_NL}
         @h2xmle_default     {1024}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_Rx_Factor;
    /**< @h2xmle_description {PP_Rx_Factor}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Rx_Shift;
    /**< @h2xmle_description {PP_Rx_Shift}
         @h2xmle_default     {0xFFFC}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Band_Eng;
    /**< @h2xmle_description {PP_Band_Eng}
         @h2xmle_default     {819}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Eng_Alpha_Near;
    /**< @h2xmle_description {PP_Eng_Alpha_Near}
         @h2xmle_default     {24576}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Eng_Alpha_Echo;
    /**< @h2xmle_description {PP_Eng_Alpha_Echo}
         @h2xmle_default     {24576}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Gain_Alpha;
    /**< @h2xmle_description {PP_Gain_Alpha}
         @h2xmle_default     {29491}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Clip_Thresh;
    /**< @h2xmle_description {PP_Clip_Thresh}
         @h2xmle_default     {656}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_HB_Clip_Thresh;
    /**< @h2xmle_description {PP_HB_Clip_Thresh}
         @h2xmle_default     {1092}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Clip_Factor_Init;
    /**< @h2xmle_description {PP_Clip_Factor_Init}
         @h2xmle_default     {3277}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Clip_Init_Frames;
    /**< @h2xmle_description {PP_Clip_Init_Frames}
         @h2xmle_default     {50}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_Clip_Factor;
    /**< @h2xmle_description {PP_Clip_Factor}
         @h2xmle_default     {3277}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_Clip_Eng_Alpha;
    /**< @h2xmle_description {PP_Clip_Eng_Alpha}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_HB_Clip_Eng_Alpha;
    /**< @h2xmle_description {PP_HB_Clip_Eng_Alpha}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t PP_CNI_Level;
    /**< @h2xmle_description {PP_CNI_Level}
         @h2xmle_default     {18022}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_CNI_Level_Q;
    /**< @h2xmle_description {PP_CNI_Level_Q}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t PP_CNI_Blk_Size;
    /**< @h2xmle_description {PP_CNI_Blk_Size}
         @h2xmle_default     {0xC8}
         @h2xmle_range       {1..1000}


         @h2xmle_policy       {Advanced} */

    int16_t EC_Ref_HPF_Enable;
    /**< @h2xmle_description {EC_Ref_HPF_Enable}
         @h2xmle_default     {1}
         @h2xmle_range       {0..1}


         @h2xmle_policy       {Advanced} */

    int16_t EC_Ref_HPF_Cutoff;
    /**< @h2xmle_description {EC_Ref_HPF_Cutoff}
         @h2xmle_default     {100}
         @h2xmle_range      {0x32..0x03E8}


         @h2xmle_policy       {Advanced} */

    int32_t aec_rx_channel_flag;
    /**< @h2xmle_description {aec_rx_channel_flag}
         @h2xmle_default     {0x03}
         @h2xmle_range       {0x0..0x7FFFFFFF}


         @h2xmle_policy       {Basic} */

    int32_t aec_feature_mode;
    /**< @h2xmle_description {aec_feature_mode}
         @h2xmle_default     {0}
         @h2xmle_range       {0..2147483647}


         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_SMECNS_V2_EC_STATE 0x00010E64

#include "adsp_begin_pack.h"

typedef struct voice_param_smecns_v2_ec_state_t voice_param_smecns_v2_ec_state_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_SMECNS_V2_EC_STATE", VOICE_PARAM_SMECNS_V2_EC_STATE}
    @h2xmlp_description {Provides internal status monitoring of an echo cancellation block of the single-mic ECNS version 2
parameter.\n
Maximum array size for this read-only parameter \n\n
Version 0 – 700 bytes
Version 1 extension – 708 bytes}
@h2xmlp_version     {1}

    @h2xmlp_toolPolicy  {RTM} */

struct voice_param_smecns_v2_ec_state_t {

    int16_t Version;
    /**< @h2xmle_description {Version}
         @h2xmle_isVersion    {true}    */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.} */

    uint32_t lib_version;
    /**< @h2xmle_description {lib_version}
          @h2xmle_default      {0x70010200}*/

    int16_t AF_Transfer_Flag[2];
    /**< @h2xmle_description {AF_Transfer_Flag[2]} */

    int16_t AF_Divg_Frame[2];
    /**< @h2xmle_description {AF_Divg_Frame[2]} */

    int16_t AF_PC_Flag[2];
    /**< @h2xmle_description {AF_PC_Flag[2]} */

    int16_t AF_PC_sAlert[2];
    /**< @h2xmle_description {AF_PC_sAlert[2]} */

    int16_t AF_PC_dAlert[2];
    /**< @h2xmle_description {AF_PC_dAlert[2]} */

    int16_t AF_BG_FG_Flag[2];
    /**< @h2xmle_description {AF_BG_FG_Flag[2]} */

    int16_t AF_ERLE_DT_Flag[2];
    /**< @h2xmle_description {AF_ERLE_DT_Flag[2]} */

    int16_t AF_ERLE_Flag[2];
    /**< @h2xmle_description {AF_ERLE_Flag[2]} */

    int16_t AF_Far_Vad;
    /**< @h2xmle_description {AF_Far_Vad} */

    int16_t AF_HB_Far_Vad;
    /**< @h2xmle_description {AF_HB_Far_Vad} */

    int16_t PP_Far_Vad;
    /**< @h2xmle_description {PP_Far_Vad} */

    int16_t Reserved2;
    /**< @h2xmle_description {Client must set this field to zero.} */

    int16_t AF_DT_Flag[2];
    /**< @h2xmle_description {AF_DT_Flag[2]} */

    int16_t AF_Clip_Flag;
    /**< @h2xmle_description {AF_Clip_Flag} */

    int16_t AF_HB_Clip_Flag;
    /**< @h2xmle_description {AF_HB_Clip_Flag} */

    int32_t PP_Voice_Eng_Bands[32];
    /**< @h2xmle_description {PP_Voice_Eng_Bands[32]} */

    int32_t PP_Echo_Eng_Bands[32];
    /**< @h2xmle_description {PP_Echo_Eng_Bands[32]} */

    int32_t PP_Rx_Eng_Bands[32];
    /**< @h2xmle_description {PP_Rx_Eng_Bands[32]} */

    int32_t PP_NL_Echo_Eng_Bands[32];
    /**< @h2xmle_description {PP_NL_Echo_Eng_Bands[32]} */

    int16_t PP_LP_Gain_Bands[32];
    /**< @h2xmle_description {PP_LP_Gain_Bands[32]} */

    int16_t PP_CNI_NS_Amp_Bands[32];
    /**< @h2xmle_description {PP_CNI_NS_Amp_Bands[32]} */

    int16_t PP_SWB_CNI_NS_Amp_Bands[2];
    /**< @h2xmle_description {PP_SWB_CNI_NS_Amp_Bands[2]} */

    int16_t AF_DT_Stat[2];
    /**< @h2xmle_description {AF_DT_Stat[2]} */

    int16_t PP_DT_Flag;
    /**< @h2xmle_description {PP_DT_Flag} */

    int16_t Reserved3;
    /**< @h2xmle_description {Client must set this field to zero.} */

}
#include "adsp_end_pack.h"
;



#define VOICE_PARAM_SMECNS_V2_NS_STATE 0x00010E65

#include "adsp_begin_pack.h"


typedef struct voice_param_smecns_v2_ns_state_t voice_param_smecns_v2_ns_state_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_SMECNS_V2_NS_STATE", VOICE_PARAM_SMECNS_V2_NS_STATE}
    @h2xmlp_description  {Provides internal status monitoring of single-mic ECNS version 2.\n
Maximum array size for this read-only parameter \n
Version 0 – 32 bytes \n}

    @h2xmlp_toolPolicy  {RTM}
    @h2xmlp_version     {0}*/

struct voice_param_smecns_v2_ns_state_t {

    int16_t Version;
    /**< @h2xmle_description {Version}
    @h2xmle_isVersion    {true}*/

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this field to zero.} */

    uint32_t Lib_Version;
    /**< @h2xmle_description {Lib_Version} */

    int32_t Mic_EnergyL32;
    /**< @h2xmle_description {Mic_EnergyL32} */

    int32_t Mic_SNR_L32Q8;
    /**< @h2xmle_description {Mic_SNR_L32Q8} */

    int16_t SM_Vad;
    /**< @h2xmle_description {SM_Vad} */

    int16_t Global_Vad;
    /**< @h2xmle_description {Global_Vad} */

    int32_t SM_RatioLogL32[3];
    /**< @h2xmle_description {SM_RatioLogL32[3]} */

}
#include "adsp_end_pack.h"
;








#define VOICE_MODULE_SMECNS_V2  0x00010F1F
/**
    @h2xmlm_module       {"VOICE_MODULE_SMECNS_V2",
                          VOICE_MODULE_SMECNS_V2}
    @h2xmlm_displayName  {"TX VOICE SMECNS_V2"}
	@h2xmlm_description {Single-mic ECNS version 2 in the voice processor on the Tx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_SMECNS_V2_EC_DELAY\n
VOICE_PARAM_AF_COEFFS\n
VOICE_PARAM_SMECNS_V2_EC\n
VOICE_PARAM_SMECNS_V2_NS\n
VOICE_PARAM_SMECNS_V2_NS_STATE\n
VOICE_PARAM_SMECNS_V2_EC_STATE\n
VOICE_PARAM_MUTE_DET\n
VOICE_PARAM_MUTE_DET_STATE\n
VOICE_PARAM_FLUENCE_FLEC\n
VOICE_PARAM_FLUENCE_FLEC_STATE \n
RTM_LOGGING_PARAM_ENABLE \n
RTM_PCM_LOGGING_PARAM  \n\n}

    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_mod_enable_t::Enable}
    @h2xmle_default     {1}

    @h2xml_Select					{voice_param_smecns_v2_ec_delay_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_af_coeffs_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_aec_path_struct_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_smecns_v2_ec_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_smecns_v2_ns_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_smecns_v2_ns_state_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_smecns_v2_ec_state_t}
    @h2xmlm_InsertParameter


    @h2xml_Select					 {mic_low_det_struct}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_mute_det_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_mute_det_state_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_fluence_flec_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_fluence_flec_state_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{audproc_rtm_logging_enable_payload_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{audproc_rtm_pcm_logging_payload_t}
    @h2xmlm_InsertParameter

   @}                   <-- End of the Module -->*/
#endif
