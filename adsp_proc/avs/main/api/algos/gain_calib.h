#ifndef GAIN_CALIB_H
#define GAIN_CALIB_H
/*==============================================================================
  @file gain_calib.h
  @brief This file contains GAIN
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



/** @h2xml_title1           {VOICE_MODULE_GAIN}
    @h2xml_title_agile_rev  {VOICE_MODULE_GAIN}
    @h2xml_title_date       {April 11, 2017} */


#define VOICE_PARAM_GAIN 0x00010E13

#include "adsp_begin_pack.h"

typedef struct voice_param_gain_t voice_param_gain_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_GAIN", VOICE_PARAM_GAIN}
    @h2xmlp_description {Gain algorithm.\n
Fixed size for this parameter \n
 Version 0 - 4 bytes\n\n} */

struct voice_param_gain_t {

    uint16_t Gain;
    /**< @h2xmle_description {Controls Tx mic gain}
         @h2xmle_default     {0x2000}
         @h2xmle_range       {0..0xFFFF}

         @h2xmle_policy       {Basic} */

    int16_t Reserved;
    /**< @h2xmle_description {Client must this field to zero.}
         @h2xmle_default     {0}
	 @h2xmle_readonly	 {true} */

}
#include "adsp_end_pack.h"
;


#define VOICE_MODULE_TX_MIC_GAIN  0x00010EF7
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_MIC_GAIN",
                          VOICE_MODULE_TX_MIC_GAIN}
    @h2xmlm_displayName  {"TX VOICE MIC GAIN"}
	@h2xmlm_description {Voice mic gain in the voice preprocessor on the Tx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_GAIN
\n}
   @h2xmlm_ToolPolicy 				{Calibration; RTC}
   @{                   <-- Start of the Module -->

   @h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_gain_t}
   @h2xmlm_InsertParameter
   @h2xml_Select           {voice_param_gain_t::Gain}

   @h2xmle_default         {0x2AAC}

   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_RX_GAIN  0x00010F20
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_GAIN",
                          VOICE_MODULE_RX_GAIN}
    @h2xmlm_displayName  {"RX GAIN"}
	@h2xmlm_description {Gain of voice downlink path processing.\n
This module supports the following parameter IDs\n
VOICE_PARAM_GAIN\n \n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @h2xmlm_deprecated              {true}
    @{                   <-- Start of the Module -->

   @h2xml_Select					{voice_param_gain_t}
   @h2xmlm_InsertParameter
   @h2xml_Select           {voice_param_gain_t::Gain}

   @h2xmle_default         {0x2000}



   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_RX_SPKR_GAIN  0x00010EF9
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_SPKR_GAIN",
                          VOICE_MODULE_RX_SPKR_GAIN}
    @h2xmlm_displayName  {"RX VOICE SPKR GAIN"}
	@h2xmlm_description {Voice speaker gain in the voice preprocessor on the Tx path.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_GAIN
\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

@h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_gain_t}
   @h2xmlm_InsertParameter
   @h2xml_Select           {voice_param_gain_t::Gain}
   @h2xmle_default         {0x2000}



   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_RX_DEC_GAIN  0x00010EFA
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_DEC_GAIN",
                          VOICE_MODULE_RX_DEC_GAIN}
    @h2xmlm_displayName  {"RX VOICE DEC GAIN"}
	@h2xmlm_description {Voice decoder gain in the voice preprocessor on the Rx path.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_GAIN
\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

@h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_gain_t}
   @h2xmlm_InsertParameter
   @h2xml_Select           {voice_param_gain_t::Gain}
   @h2xmle_default         {0x2000}



   @}                   <-- End of the Module -->*/





#endif
