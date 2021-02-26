#ifndef ST_CALIB_H
#define ST_CALIB_H
/*==============================================================================
  @file st_calib.h
  @brief This file contains ST
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
#include "voice_param_mod_enable_calib.h"


/** @h2xml_title1           {VOICE_MODULE_ST}
    @h2xml_title_agile_rev  {VOICE_MODULE_ST}
    @h2xml_title_date       {March 13, 2017} */


#define VOICE_PARAM_ST 0x00010E05

#include "adsp_begin_pack.h"

typedef struct voice_param_st_t voice_param_st_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_ST", VOICE_PARAM_ST}
    @h2xmlp_description {Slow Talk for the voice activity detection and expansion algorithm.\n
Fixed size for this parameter\n
Version 0 – 60 bytes} */

struct voice_param_st_t {

    int16_t avad_thresh;
    /**< @h2xmle_description {Threshold for generating a binary VAD decision}
         @h2xmle_default     {0x4000}
         @h2xmle_range       {0x2666..0x6666}
         @h2xmle_policy      {Advanced} */

    int16_t avad_pwr_scale;
    /**< @h2xmle_description {Indirectly lower-bounds the noise level estimate}
         @h2xmle_default     {0x0100}
         @h2xmle_range       {0x0080..0x0400}
         @h2xmle_policy      {Advanced} */

    int16_t avad_hangover;
    /**< @h2xmle_description {Avoids mid-speech clipping and reliably detects weak speech bursts at the ends of speech activity}
         @h2xmle_default     {0x0004}
         @h2xmle_range       {0x0000..0x0014}
         @h2xmle_policy      {Advanced} */

    int16_t avad_alpha;
    /**< @h2xmle_description {Controls responsiveness of the VAD}
         @h2xmle_default     {0x4000}
         @h2xmle_range       {0x0CCD..0x7332}
         @h2xmle_policy      {Advanced} */

    int16_t avad_sd_max;
    /**< @h2xmle_description {Decreasing this parameter value may help in making correct decisions during abrupt changes; however, decreasing too much may increase false alarms during long pauses/silences}
         @h2xmle_default     {0x0C00}
         @h2xmle_range       {0xA00..0x1800}
         @h2xmle_policy      {Advanced} */

    int16_t avad_sd_min;
    /**< @h2xmle_description {Decreasing this parameter value may help in making correct decisions during abrupt changes; however, decreasing too much may increase false alarms during long pauses/silences}
         @h2xmle_default     {0x0A00}
         @h2xmle_range       {0x0800..0x0C00}
         @h2xmle_policy      {Advanced} */

    int16_t avad_init_length;
    /**< @h2xmle_description {Defines the number of frames for which noise level estimate is set to a fixed value, giving enough time for the estimates to become accurate}
         @h2xmle_default     {0x0064}
         @h2xmle_range       {0x0046..0x00C8}
         @h2xmle_policy      {Advanced} */

    int16_t reserved;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0x0000..0x7FFF}
         @h2xmle_policy      {Advanced} */

    int32_t avad_max_val;
    /**< @h2xmle_description {Defines the upper limit of the noise level}
         @h2xmle_default     {0x0288}
         @h2xmle_range       {0x00CD..0x0288}
         @h2xmle_policy      {Advanced} */

    int32_t avad_init_bound;
    /**< @h2xmle_description {Defines the initial bounding value for the noise level estimate; used during the initial segment defined by the parameter AVAD_INIT_LENGTH}
         @h2xmle_default     {0x0122}
         @h2xmle_range       {0x0041..0x0122}
         @h2xmle_policy      {Advanced} */

    int16_t avad_sub_nc;
    /**< @h2xmle_description {Defines the window length for noise estimation}
         @h2xmle_default     {0x0032}
         @h2xmle_range       {0x0019..0x0064}
         @h2xmle_policy      {Advanced} */

    int16_t reserved1;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0x0000..0x7FFF}
         @h2xmle_policy      {Advanced} */

    int32_t avad_var;
    /**< @h2xmle_description {Defines the bias factor in noise estimation}
         @h2xmle_default     {0x2F0A}
         @h2xmle_range       {0x2000..0x6000}
         @h2xmle_policy      {Advanced} */

    int16_t avad_spow_min;
    /**< @h2xmle_description {Defines the minimum signal power required to update the bounds for the noise floor estimate}
         @h2xmle_default     {0x0244}
         @h2xmle_range       {0x00CD..0x0244}
         @h2xmle_policy      {Advanced} */

    int16_t avad_cfac;
    /**< @h2xmle_description {Increasing this parameter increases the noise floor learning time; can potentially increase false alarms}
         @h2xmle_default     {0x0148}
         @h2xmle_range       {0x0148..0x0666}
         @h2xmle_policy      {Advanced} */

    int32_t max_delay;
    /**< @h2xmle_description {Defines the maximum delay unit in samples}
         @h2xmle_default     {0x5DC0}
         @h2xmle_range       {-2147483648..2147483647}
         @h2xmle_policy      {Advanced} */

    int16_t reserved2;
    /**< @h2xmle_description {Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0x0000..0x7FFF}
         @h2xmle_policy      {Advanced} */

    int16_t max_exp_diff;
    /**< @h2xmle_description {Used for expansion control. Note: This parameter should not be tuned.}
         @h2xmle_default     {0x0014}
         @h2xmle_range       {0x0001..0x0064}
         @h2xmle_policy      {Advanced} */

    int16_t min_pause;
    /**< @h2xmle_description {Defines minimum speech pause duration, units in frames, needed to maintain the naturalness of speech}
         @h2xmle_default     {0x0008}
         @h2xmle_range       {0x0004..0x0010}
         @h2xmle_policy      {Advanced} */

    int16_t look_ahead;
    /**< @h2xmle_description {Defines the look-ahead size unit in frames}
         @h2xmle_default     {0x0004}
         @h2xmle_range       {0x0004..0x0008}
         @h2xmle_policy      {Advanced} */

    int32_t voice_qual_thresh1;
    /**< @h2xmle_description {Defines tunable thresholds that determine the voice quality of expanded speech; dominant for voice quality}
         @h2xmle_default     {0x66666666}
         @h2xmle_policy      {Advanced} */

    int32_t voice_qual_thresh2;
    /**< @h2xmle_description {Defines tunable thresholds that determine the voice quality of expanded speech; dominant for voice quality}
         @h2xmle_default     {0x66666666}
         @h2xmle_policy      {Advanced} */

    int32_t voice_qual_thresh_force;
    /**< @h2xmle_description {Defines tunable thresholds that determine the voice quality of expanded speech; dominant for voice quality . Note: This parameter should not be tuned.}
         @h2xmle_default     {0x66666666}
         @h2xmle_policy      {Advanced} */

}
#include "adsp_end_pack.h"
;


#define VOICE_PARAM_ST_EXP 0x00010E16

#include "adsp_begin_pack.h"

typedef struct voice_param_st_exp_t voice_param_st_exp_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_ST_EXP", VOICE_PARAM_ST_EXP}
    @h2xmlp_description {Slow Talk expansion algorithm.\n
Fixed size for this parameter\n
Version 0 – 4 bytes \n } */

struct voice_param_st_exp_t {

    int16_t EXPANSION_RATIO;
    /**< @h2xmle_description {Expansion ratio aimed by the user. Only 6 ratio supported: {100%, 110%, 120%, 130%, 140%, 150%}}
         @h2xmle_default     {0x5DC0}
         @h2xmle_policy      {Basic} */

    int16_t Reserved;
    /**< @h2xmle_description {Client must set this value to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0x0000..0x7FFF}
         @h2xmle_readOnly      {true} */

}
#include "adsp_end_pack.h"
;


#define VOICE_MODULE_ST  0x00010EE3
/**
    @h2xmlm_module       {"VOICE_MODULE_ST",
                          VOICE_MODULE_ST}
    @h2xmlm_displayName  {"RX VOICE SLOWTALK"}
	@h2xmlm_description {Slow Talk algorithm in the voice decoder postprocessor.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_ST\n
VOICE_PARAM_ST_EXP
\n\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

@h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_st_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_st_exp_t}
   @h2xmlm_InsertParameter



   @}                   <-- End of the Module -->*/
#endif
