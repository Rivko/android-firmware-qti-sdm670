#ifndef FIR_CALIB_H
#define FIR_CALIB_H
/*==============================================================================
  @file fir_calib.h
  @brief This file contains FIR
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


/** @h2xml_title1           {VOICE_MODULE_FIR}
    @h2xml_title_agile_rev  {VOICE_MODULE_FIR}
    @h2xml_title_date       {APRIL 08, 2017} */


#define VOICE_PARAM_FIR 0x00010E0A

#include "adsp_begin_pack.h"

typedef struct voice_param_fir_t voice_param_fir_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_FIR", VOICE_PARAM_FIR}

    @h2xmlp_description  {FIR tuning filter algorithm for voice.
This parameter contains a field that indicates the number of filter taps, followed by an FIR filter coefficient
structure. The supported Q factor is Q14. The maximum number of FIR taps for voice applications is 128
taps.\n
Maximum size for this parameter\n
Version 0 – 260 bytes \n\n} */

struct voice_param_fir_t {

    uint16_t NumFilterTaps;
    /**< @h2xmle_description {Number of filter taps}
         @h2xmle_default     {12}
         @h2xmle_range       {1..128}

         @h2xmle_policy       {Basic} */

    int16_t FilterCoeffs[0];
    /**< @h2xmle_description {These make the filter. Individual values are not typically set directly.}
         @h2xmle_default     {16384}
         @h2xmle_range       {-32768..32767}
         @h2xmle_variableArraySize  { "NumFilterTaps" }

        @h2xmle_defaultList {16384, 0, 0, 0, , 0, 0, 0, 0, 0, 0, 0, 0 }

       @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;





#define VOICE_MODULE_TX_FIR  0x00010EE9
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_FIR",
                          VOICE_MODULE_TX_FIR}
    @h2xmlm_displayName  {"TX VOICE FIR"}
	@h2xmlm_description {FIR algorithm in the voice processor on the Tx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_FIR\n\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_fir_t}
    @h2xmlm_InsertParameter

   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_RX_FIR  0x00010EEA
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_FIR",
                          VOICE_MODULE_RX_FIR}
    @h2xmlm_displayName  {"RX VOICE FIR"}
	@h2xmlm_description {FIR algorithm in the voice processor on the Rx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_FIR\n\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

@h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_fir_t}
   @h2xmlm_InsertParameter



    @h2xml_Select       {voice_param_fir_t::FilterCoeffs}
    @h2xmle_defaultList {16384, 0, 0, 0, , 0, 0, 0, 0, 0, 0, 0, 0 }


   @}                   <-- End of the Module -->*/


#endif
