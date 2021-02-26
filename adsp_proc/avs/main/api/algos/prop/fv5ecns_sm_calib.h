#ifndef FV5ECNS_SM_CALIB_H
#define FV5ECNS_SM_CALIB_H
/*==============================================================================
  @file fv5ecns_sm_calib.h
  @brief This file contains FV5ECNS_SM
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

/** @h2xml_title1           {VOICE_MODULE_FLUENCE_SM}
    @h2xml_title_agile_rev  {VOICE_MODULE_FLUENCE_SM}
    @h2xml_title_date       {April 11, 2017} */


#define VOICE_MODULE_FLUENCE_SM  0x00010F09
/**
    @h2xmlm_module       {"VOICE_MODULE_FLUENCE_SM",
                          VOICE_MODULE_FLUENCE_SM}
    @h2xmlm_displayName  {"TX VOICE FLUENCE SM"}
	@h2xmlm_description {Single-mic Fluence version 5 ECNS in the voice processor on the Tx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_FLUENCE_EC\n
VOICE_PARAM_AF_COEFFS\n
VOICE_PARAM_ECHO_PATH_DELAY\n
VOICE_PARAM_FLUENCE_NS\n
VOICE_PARAM_FLUENCE_EC_STATE\n
VOICE_PARAM_FLUENCE_NS_STATE\n
VOICE_PARAM_FLUENCE_SPF_COEFF\n
VOICE_PARAM_MUTE_DET\n
VOICE_PARAM_MUTE_DET_STATE\n
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

@h2xml_Select					{voice_param_fpecns_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_af_coeffs_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_aec_path_struct_t}
   @h2xmlm_InsertParameter


@h2xml_Select					{voice_param_echo_path_delay_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_fv5ecns_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{ voice_param_fp_state_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_fv5_state_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_fv5_spf_coeff_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_mute_det_t}
   @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_mute_det_state_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{mic_low_det_struct}
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
