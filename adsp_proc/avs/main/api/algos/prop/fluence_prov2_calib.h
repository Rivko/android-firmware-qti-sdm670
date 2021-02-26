#ifndef FLUENCE_PROV2_CALIB_H
#define FLUENCE_PROV2_CALIB_H
/*==============================================================================
  @file fluence_prov2_calib.h
  @brief This file contains FLUENCE_PROV2
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
#include "audio_rtm_logging_param_calib.h"


/** @h2xml_title1           {VOICE_MODULE_FLUENCE_PRO}
    @h2xml_title_agile_rev  {VOICE_MODULE_FLUENCE_PRO}
    @h2xml_title_date       {April 11, 2017} */





#define VOICE_PARAM_FLUENCE_LSP 0x00010EAA

#include "adsp_begin_pack.h"

typedef struct voice_param_fluence_lsp_t voice_param_fluence_lsp_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FLUENCE_LSP", VOICE_PARAM_FLUENCE_LSP}

    @h2xmlp_description  {Minimum Variance Distortionless Response (MVDR) parameter for linear spatial filtering. This read/write
parameter is used to pass far-field MVDR parameters.\n
Maximum array size for this parameter \n
Version 0 – 128 bytes \n\n}
@h2xmlp_version     {0} */

struct voice_param_fluence_lsp_t {

    int16_t Version;
    /**< @h2xmle_description {Version.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}
         @h2xmle_isVersion    {true}



         @h2xmle_readOnly       {true} */

    int16_t Reserved;
    /**< @h2xmle_description {Client should set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}



         @h2xmle_readOnly       {true} */

    int32_t LSP_Flag;
    /**< @h2xmle_description {Mode word for Linear Spatial processing}
         @h2xmle_default     {0}
         @h2xmle_range       {0..2147483647}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_numStrVect;
    /**< @h2xmle_description {Number of steering vectors for LSP processing}
         @h2xmle_default     {2}
         @h2xmle_range       {0..5}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_strVEnableFlag [5];
    /**< @h2xmle_description {Enable flag for LSP steering vector}
         @h2xmle_default     {1}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int32_t LSP_ecDiagLoadPerctL32Q24;
    /**< @h2xmle_description {Diagonal loading factor in percentage for the EC LSP processing}
         @h2xmle_default     {0}
         @h2xmle_range       {0..2147483647}



         @h2xmle_policy       {Advanced} */

    int32_t LSP_bfDiagLoadPerctL32Q24;
    /**< @h2xmle_description {Diagonal loading factor in percentage for the NS LSP processing}
         @h2xmle_default     {0}
         @h2xmle_range       {0..2147483647}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_initialSkipNumFrm;
    /**< @h2xmle_description {Number of initial frames to skip during LSP processing update}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_initialAttenuationL16Q13;
    /**< @h2xmle_description {Initial attenuation to apply before the LSP processing converges}
         @h2xmle_default     {2048}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_ecStartIndx;
    /**< @h2xmle_description {Starting frequency bin index for EC LSP processing}
         @h2xmle_default     {1}
         @h2xmle_range       {0..129}


         @h2xmle_policy       {Advanced} */

    int16_t LSP_ecEndIndx;
    /**< @h2xmle_description {Starting frequency bin index for EC LSP processing}
         @h2xmle_default     {128}
         @h2xmle_range       {0..129}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_ecAccumuNumFrm;
    /**< @h2xmle_description {Number of frames to accumulate covariance matrix for EC LSP processing}
         @h2xmle_default     {10}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_ecWatchNumFrm;
    /**< @h2xmle_description {Max Number of frames to watch for updating covariance for EC LSP processing}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_bfStartIndx;
    /**< @h2xmle_description {Starting frequency bin index for NS LSP processing}
         @h2xmle_default     {1}
         @h2xmle_range       {0..129}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_bfEndIndx;
    /**< @h2xmle_description {Ending frequency bin index for NS LSP processing}
         @h2xmle_default     {128}
         @h2xmle_range       {0..129}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_bfAccumuNumFrm;
    /**< @h2xmle_description {Number of frames to accumulate covariance matrix for NS LSP processing}
         @h2xmle_default     {50}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_bfWatchNumFrm;
    /**< @h2xmle_description {Max Number of frames to watch for updating covariance for NS LSP processing}
         @h2xmle_default     {100}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_matrixInvtPerFrm;
    /**< @h2xmle_description {Number of bins per frame for matrix inverse computation}
         @h2xmle_default     {8}
         @h2xmle_range       {1..32}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_nrefStartIndx;
    /**< @h2xmle_description {Starting frequency bin index for noise reference post-processing}
         @h2xmle_default     {1}
         @h2xmle_range       {0..129}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_nrefEndIndx;
    /**< @h2xmle_description {Ending frequency bin index for noise reference post-processing}
         @h2xmle_default     {128}
         @h2xmle_range       {1..256}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_statNrefStatcsNumFrm;
    /**< @h2xmle_description {Number of frames for updating Stationary noise ref Statistics}
         @h2xmle_default     {80}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int32_t LSP_statNrefOverEstL32Q16;
    /**< @h2xmle_description {Over-estimation factor for stationary noise reference}
         @h2xmle_default     {16384}
         @h2xmle_range       {0..2147483647}



         @h2xmle_policy       {Advanced} */

    int32_t LSP_ssCompensatFactorL32Q16;
    /**< @h2xmle_description {Compensation factor for post-processing gain}
         @h2xmle_default     {93061}
         @h2xmle_range       {0..2147483647}



         @h2xmle_policy       {Advanced} */

    int32_t LSP_postGainBoostL32Q16;
    /**< @h2xmle_description {Post gain boost for post-processing gain computation}
         @h2xmle_default     {262144}
         @h2xmle_range       {0..2147483647}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_ssMinGainL16Q14;
    /**< @h2xmle_description {Min Gain for NS post-processing}
         @h2xmle_default     {4096}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_postGainAttL16Q14;
    /**< @h2xmle_description {Attack time for the NS processing post gain}
         @h2xmle_default     {3000}
         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Advanced} */

    int16_t LSP_postGainDecL16Q14;
    /**< @h2xmle_description {Decay time for the NS processing post gain}
         @h2xmle_default     {10000}
         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Advanced} */

    int16_t LSP_outputHPFilterL16Q14 [26];
    /**< @h2xmle_description {Gain Factor for output HPF frequency bin}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}



         @h2xmle_policy       {Advanced} */

    int16_t Reserved6;
    /**< @h2xmle_description {Client should set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0..32767}


         @h2xmle_readOnly       {true} */

}
#include "adsp_end_pack.h"
;






/** @h2xmlp_subStruct */
typedef struct w_t
{
	int32_t IDT;
	/**< @h2xmle_description {Direction/Mic Metadata} */


	int32_t c[0];
	/**< @h2xmle_description {Complex coefficients}
	     @h2xmle_variableArraySize {"2*FSTV::N"}  */

}w_t ;

#define VOICE_PARAM_FSTV   0x00010EA9



typedef struct voice_param_fstv_t voice_param_fstv_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FSTV", VOICE_PARAM_FSTV}
    @h2xmlp_description {Fluence steering vector (FSTV). This parameter is used to pass the pretraining steering vector to the Fluence library.\n
Maximum array size for this parameter \n
Version 0 -- 30784 bytes\n}

 @h2xmlx_expandStructs	{false} */


#include "adsp_begin_pack.h"
struct voice_param_fstv_t  {

	int16_t N;
	/**< @h2xmle_description {Number of coefficients}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0x0..0x0100}
         @h2xmle_policy       {Basic} */

	int16_t N_W;
	/**< @h2xmle_description {Number of coefficient arrays}
         @h2xmle_default     {0x0}
         @h2xmle_range       {0x0..0x000F}
         @h2xmle_policy       {Basic} */

	w_t W[0];
	/**< @h2xmle_description {Array of coefficients.}
	 @h2xmle_variableArraySize {N_W}
         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;

#define VOICE_MODULE_FLUENCE_PRO  0x00010F17
/**
    @h2xmlm_module       {"VOICE_MODULE_FLUENCE_PRO",
                          VOICE_MODULE_FLUENCE_PRO}
    @h2xmlm_displayName  {"TX VOICE FLUENCE PRO"}
	@h2xmlm_description {Fluence Pro version 2 quad-mic ECNS in the voice processor on the Tx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_AF_COEFFS\n
VOICE_PARAM_ECHO_PATH_DELAY\n
VOICE_PARAM_FLUENCE_EC_SPK\n
VOICE_PARAM_FLUENCE_NS_SPK\n
VOICE_PARAM_FLUENCE_EC_STATE\n
VOICE_PARAM_FLUENCE_NS_SPK_STATE\n
VOICE_PARAM_FLUENCE_LSP\n
VOICE_PARAM_FSTV\n
VOICE_PARAM_MUTE_DET\n
VOICE_PARAM_FLUENCE_FLEC\n
VOICE_PARAM_FLUENCE_FLEC_STATE\n
RTM_LOGGING_PARAM_ENABLE \n
RTM_PCM_LOGGING_PARAM  \n\n}

    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_mod_enable_t::Enable}
    @h2xmle_default     {1}

   @h2xml_Select					{voice_param_af_coeffs_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_aec_path_struct_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_echo_path_delay_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_fluence_ec_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_fluence_ns_spk_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_fp_state_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_fluence_ns_spk_state_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_fluence_lsp_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_fstv_t}
   @h2xmlm_InsertParameter


   @h2xml_Select					 {w_t}
   @h2xmlm_InsertParameter


   @h2xml_Select					{voice_param_mute_det_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{mic_low_det_struct}
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
