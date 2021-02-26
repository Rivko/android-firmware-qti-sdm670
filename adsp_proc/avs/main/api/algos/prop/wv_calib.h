#ifndef WV_CALIB_H
#define WV_CALIB_H
/*==============================================================================
  @file wv_calib.h
  @brief This file contains WV
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


/** @h2xml_title1           {VOICE_MODULE_WV}
    @h2xml_title_agile_rev  {VOICE_MODULE_WV}
    @h2xml_title_date       {March 13, 2017} */


#define VOICE_PARAM_WV 0x00010E04

#include "adsp_begin_pack.h"

typedef struct voice_param_wv_t voice_param_wv_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_WV", VOICE_PARAM_WV}
    @h2xmlp_description {Wide Voice algorithm.\n
Fixed size for this parameter\n
Version 0 – 44 bytes}
   @h2xmlp_deprecated              {true} */

struct voice_param_wv_t {

    uint16_t adaptive_bass_boost_voiced;
    /**< @h2xmle_description {To turn this off, this flag needs to be undefined (set to 0). This can be done by commenting out the flag definition in defines.h.}
         @h2xmle_default     {0xFFFF}
         @h2xmle_range       {0x0000..0xFFFF}
         @h2xmle_policy      {Basic} */

    uint16_t fixed_bass_boost_9dB;
    /**< @h2xmle_description {To turn this off, this flag needs to be undefined. This can be done by commenting out the flag definition in defines.h.}
         @h2xmle_default     {0xFFFF}
         @h2xmle_range       {0x0000..0xFFFF}
         @h2xmle_policy      {Basic} */

    int16_t high_band_contrib;
    /**< @h2xmle_description {Increasing this factor will cause the wideband signal to have higher highband component}
         @h2xmle_default     {0x4000}
         @h2xmle_range       {0x3333..0x4000}
         @h2xmle_policy      {Basic} */

    int16_t low_band_contrib;
    /**< @h2xmle_description {Increasing this factor will cause the wideband signal to have higher lowband component}
         @h2xmle_default     {0x4000}
         @h2xmle_range       {0x4000..0x4CCC}
         @h2xmle_policy      {Basic} */

    int16_t nfact;
    /**< @h2xmle_description {Increasing this will effectively cause an increase in the amount of modulated noise that  is added to the estimated highband excitation.}
         @h2xmle_default     {0x277A}
         @h2xmle_range       {0x2300..0x2A00}
         @h2xmle_policy      {Basic} */

    int16_t noise_mix_factor_alpha;
    /**< @h2xmle_description {Increasing this will effectively cause an increase in the amount of modulated noise that  is added to the estimated highband excitation.}
         @h2xmle_default     {0x199A}
         @h2xmle_range       {0xCCC..0x2000}
         @h2xmle_policy      {Basic} */

    int16_t noise_mix_factor_beta;
    /**< @h2xmle_description {Increasing this will effectively cause an increase in the amount of modulated noise that  is added to the estimated highband excitation.}
         @h2xmle_default     {0x51F}
         @h2xmle_range       {0x333..0x666}
         @h2xmle_policy      {Basic} */

    int16_t Reserved;
    /**< @h2xmle_description {Client should set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0x0000..0x7FFF}
         @h2xmle_readOnly      {true} */

    uint32_t silence_hb_egy_attn;
    /**< @h2xmle_description {Increasing this will cause the energy of the highband in the speech pauses to be lower}
         @h2xmle_policy      {Basic} */

    int16_t smf_sens_cntrl;
    /**< @h2xmle_description {Increase in this value will cause the present frame highband gain to be smoothed less with respect to the previous frame}
         @h2xmle_default     {0x12C}
         @h2xmle_range       {0xFA..0x15E}
         @h2xmle_policy      {Basic} */

    int16_t speech_buffer_len;
    /**< @h2xmle_description {Changing this will allow segments shorter or longer than 20 msecs to be bandwidth extended. Additional changes may need to be made to make this work.}
         @h2xmle_default     {160}
         @h2xmle_policy      {Basic} */

    int16_t tilt_scal_fac;
    /**< @h2xmle_description {Increasing this parameter causes the energy of the highband for unvoiced frames with positive tilts to be higher. However the cap on the multiplicative factor limits the increase in the highband energy}
         @h2xmle_default     {0x7999}
         @h2xmle_range       {0x7333..0x7D70}
         @h2xmle_policy      {Basic} */

    int16_t Reserved1;
    /**< @h2xmle_description {Client should set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0x0000..0x7FFF}
         @h2xmle_readOnly      {true} */

    int32_t unvoiced_highband_gain_cap;
    /**< @h2xmle_description {Increase in this value will allow highband energy to have larger values}
         @h2xmle_default     {0xC800000}
         @h2xmle_range       {0xA000000..0xF000000}
         @h2xmle_policy      {Basic} */

    int16_t voiced_egy_map_alpha;
    /**< @h2xmle_description {Increase in this value will cause the highband to have more energy in voiced frames}
         @h2xmle_default     {27593}
         @h2xmle_policy      {Basic} */

    int16_t Reserved2;
    /**< @h2xmle_description {Client should set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0x0000..0x7FFF}
         @h2xmle_readOnly      {true} */

    int32_t voiced_egy_map_beta;
    /**< @h2xmle_description {Increase in this value will cause the highband to have more energy in voiced frames}
         @h2xmle_default     {0x2AC56BA}
         @h2xmle_range       {0x2800000..0x3000000}
         @h2xmle_policy      {Basic} */

    int16_t voiced_pitchgain_limit;
    /**< @h2xmle_description {Decreasing this limit will cause more frames to be classified as voiced. All such frames will have a highband extension similar to that of voiced frames}
         @h2xmle_default     {0xCCD}
         @h2xmle_range       {0xB33..0xE66}
         @h2xmle_policy      {Basic} */

    int16_t voiced_tilt_limit;
    /**< @h2xmle_description {Increasing this limit will cause more frames to be classified as voiced. All such frames will have a highband extension similar to that of voiced frames}
         @h2xmle_default     {0x199A}
         @h2xmle_range       {0xCCD..0x2000}
         @h2xmle_policy      {Basic} */

}
#include "adsp_end_pack.h"
;


#define VOICE_MODULE_WV  0x00010EE2
/**
    @h2xmlm_module       {"VOICE_MODULE_WV",
                          VOICE_MODULE_WV}
    @h2xmlm_displayName  {"RX_VOICE_WV"}
	@h2xmlm_description {Wide Voice algorithm in the voice decoder postprocessor\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_WV\n\n}

    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @h2xmlm_deprecated              {true}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_mod_enable_t::Enable}
    @h2xmle_readOnly      {true}
	
    @h2xml_Select					{voice_param_wv_t}
    @h2xmlm_InsertParameter





   @}                   <-- End of the Module -->*/
#endif
