#ifndef IIR_CALIB_H
#define IIR_CALIB_H
/*==============================================================================
  @file iir_calib.h
  @brief This file contains IIR
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



/** @h2xml_title1           {VOICE_MODULE_IIR}
    @h2xml_title_agile_rev  {VOICE_MODULE_IIR}
    @h2xml_title_date       {March 28, 2017} */



#define VOICE_PARAM_IIR 0x00010E09

#include "adsp_begin_pack.h"

typedef struct voice_param_iir_t voice_param_iir_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_IIR", VOICE_PARAM_IIR}
    @h2xmlp_description  {IIR Tuning Filter algorithm for voice.
Fixed size for this parameter \n
Version 0 – 4 bytes, followed by variable-sized IIR filter structures \n
The filter sizes are defined by configurable parameters. The maximum size for voice applications is 224
bytes. For voice, the maximum number of filter stages is assumed to be 10. If the number of stages is an
odd number, 2 bytes of padding must be added at the end of the payload. \n\n } */

struct voice_param_iir_t {

    uint16_t NumBiquadStages;
    /**< @h2xmle_description {Number of filter stages. Higher value gives more filter resolution. But, at higher values need to look out for poor sound using Yulewalker.}
         @h2xmle_default     {2}
         @h2xmle_range       {1..10}

         @h2xmle_policy       {Basic} */

    uint16_t PreGain;
    /**< @h2xmle_description {Sets the gain before the filter}
         @h2xmle_default     {8192}
         @h2xmle_range       {0..32767}

         @h2xmle_policy       {Basic} */

    int32_t FilterCoeffs[0];
    /**< @h2xmle_description {These make the filter. Individual values are not typically set directly. These should come from the IIR Designer.}
         @h2xmle_default     {1073741824}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_variableArraySize  { "5*NumBiquadStages" }

         @h2xmle_policy       {Basic} */

    int16_t NumShiftFactor[0];
    /**< @h2xmle_description {These make the filter. Individual values are not typically set directly. These should come from the IIR Designer.}
         @h2xmle_default     {2}
         @h2xmle_range       {0..32767}

         @h2xmle_variableArraySize  { "NumBiquadStages" }

         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;



#define VOICE_MODULE_RX_IIR  0x00010EE8
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_IIR",
                          VOICE_MODULE_RX_IIR}
    @h2xmlm_displayName  {"RX VOICE IIR"}
	@h2xmlm_description {IIR algorithm in the voice processor on the Rx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_IIR\n\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{ voice_param_iir_t}
    @h2xmlm_InsertParameter

    @h2xml_Select       {voice_param_iir_t::NumBiquadStages}


    @h2xml_Select       {voice_param_iir_t::PreGain}


    @h2xml_Select       {voice_param_iir_t::FilterCoeffs}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "2014770557"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1204769422"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1073741824"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1073741824"}

    @h2xml_Select       {voice_param_iir_t::NumShiftFactor}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "1"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "2"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "2"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "2"}



   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_TX_IIR  0x00010EE7
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_IIR",
                          VOICE_MODULE_TX_IIR}
    @h2xmlm_displayName  {"TX VOICE IIR"}
	@h2xmlm_description {IIR algorithm in the voice processor on the Tx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_IIR\n\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{ voice_param_iir_t}
    @h2xmlm_InsertParameter

   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_IIR_MIC1  0x00010EF0
/**
    @h2xmlm_module       {"VOICE_MODULE_IIR_MIC1",
                          VOICE_MODULE_IIR_MIC1}
    @h2xmlm_displayName  {"TX VOICE IIR MIC1"}
	@h2xmlm_description {IIR algorithm in the voice processor on the Tx mic 1 input path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
 VOICE_PARAM_IIR\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{ voice_param_iir_t}
    @h2xmlm_InsertParameter

    @}                   <-- End of the Module -->*/

 #define VOICE_MODULE_IIR_MIC2  0x00010EF1
/**
    @h2xmlm_module       {"VOICE_MODULE_IIR_MIC2",
                          VOICE_MODULE_IIR_MIC2}
    @h2xmlm_displayName  {"TX VOICE IIR MIC2"}
	@h2xmlm_description {IIR algorithm in the voice processor on the Tx mic 2 input path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_IIR\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_iir_t}
    @h2xmlm_InsertParameter


@}                   <-- End of the Module -->*/

#define VOICE_MODULE_IIR_MIC3  0x00010F03
/**
    @h2xmlm_module       {"VOICE_MODULE_IIR_MIC3",
                          VOICE_MODULE_IIR_MIC3}
    @h2xmlm_displayName  {"TX VOICE IIR MIC3"}
	@h2xmlm_description {IIR algorithm in the voice processor on the Tx mic 3 input path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
 VOICE_PARAM_IIR\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{ voice_param_iir_t}
    @h2xmlm_InsertParameter

 @}                   <-- End of the Module -->*/

 #define VOICE_MODULE_IIR_MIC4  0x00010F04
/**
    @h2xmlm_module       {"VOICE_MODULE_IIR_MIC4",
                          VOICE_MODULE_IIR_MIC4}
    @h2xmlm_displayName  {"TX VOICE IIR MIC4"}
	@h2xmlm_description {IIR algorithm in the voice processor on the Tx mic 4 input path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
 VOICE_PARAM_IIR\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{ voice_param_iir_t}
    @h2xmlm_InsertParameter

@}                   <-- End of the Module -->*/



#define VOICE_MODULE_TX_HPF  0x00010F11
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_HPF",
                          VOICE_MODULE_TX_HPF}
    @h2xmlm_displayName  {"TX VOICE HPF"}
	@h2xmlm_description {HPF 12 IIR algorithm in the voice processor on the Tx path.\n
For each HPF, it's a 2nd order IIR (one-biquad). \n
Coefficients are saved in colume in the format of [b0  b1  b2  a1  a2].  A0 is assumed to be 1*2^Qfactor \n.
Coefficients Qfactor for all numerators(b) and denominators are Q30.\n\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_IIR\n\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_iir_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_iir_t::NumBiquadStages}

    @h2xmle_default     {1}


    @h2xml_Select       {voice_param_iir_t::FilterCoeffs }

    @h2xmle_default {0x40000000}

    @h2xmle_defaultDependency   { Samplerate = "8000",  defaultList="{{0x3E3F4E5D, 0x83816345, 0x3E3F4E5D, 0x838DAD95, 0x3C8AE70B}"}
    @h2xmle_defaultDependency   { Samplerate = "16000",  defaultList="{{0x3F1E189F, 0x81C3CEC2, 0x3F1E189F, 0x81C6EC2, 0x3E3F4EA9}"}
    @h2xmle_defaultDependency   { Samplerate = "32000",  defaultList="{{0x3F8EA7F4, 0x80E2B018, 0x3F8EA7F4, 0x80E378D4, 0x3F1E18A4}"}
    @h2xmle_defaultDependency   { Samplerate = "48000",  defaultList="{{0x3FB45999, 0x80974CCF, 0x3FB45999, 0x8097A63A, 0x3F690C9D}"}



    @h2xml_Select       {voice_param_iir_t::NumShiftFactor}

    @h2xmle_default     {2}


@}                   <-- End of the Module -->*/


#define VOICE_MODULE_RX_HPF  0x00010F12
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_HPF",
                          VOICE_MODULE_RX_HPF}
    @h2xmlm_displayName  {"RX VOICE HPF"}
	@h2xmlm_description {HPF 12 IIR algorithm in the voice processor on the Rx path.\n
For each HPF, it's a 2nd order IIR (one-biquad). \n
Coefficients are saved in colume in the format of [b0  b1  b2  a1  a2].  A0 is assumed to be 1*2^Qfactor \n.
Coefficients Qfactor for all numerators(b) and denominators are Q30.\n\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_IIR\n}
   @h2xmlm_toolPolicy 				{Calibration; RTC}
   @{                   <-- Start of the Module -->

   @h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_iir_t}
    @h2xmlm_InsertParameter

    @h2xml_Select       {voice_param_iir_t::NumBiquadStages}

    @h2xmle_default     {1}



    @h2xml_Select       {voice_param_iir_t::FilterCoeffs}

    @h2xmle_default {0x40000000}

    @h2xmle_defaultDependency   { Samplerate = "8000",  defaultList="{{0x3E3F4E5D, 0x83816345, 0x3E3F4E5D, 0x838DAD95, 0x3C8AE70B}"}
    @h2xmle_defaultDependency   { Samplerate = "16000",  defaultList="{{0x3F1E189F, 0x81C3CEC2, 0x3F1E189F, 0x81C6EC2, 0x3E3F4EA9}"}
    @h2xmle_defaultDependency   { Samplerate = "32000",  defaultList="{{0x3F8EA7F4, 0x80E2B018, 0x3F8EA7F4, 0x80E378D4, 0x3F1E18A4}"}
    @h2xmle_defaultDependency   { Samplerate = "48000",  defaultList="{{0x3FB45999, 0x80974CCF, 0x3FB45999, 0x8097A63A, 0x3F690C9D}"}


    @h2xml_Select       {voice_param_iir_t::NumShiftFactor}

    @h2xmle_default     {2}


@}                   <-- End of the Module -->*/

#endif
