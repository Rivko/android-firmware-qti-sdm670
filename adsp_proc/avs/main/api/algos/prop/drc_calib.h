#ifndef DRC_CALIB_H
#define DRC_CALIB_H
/*==============================================================================
  @file drc_calib.h
  @brief This file contains DRC
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/09/17   shrani  Added h2xml comments.
 ========================================================================== */
#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"


/** @h2xml_title1           {VOICE_MODULE_DRC}
    @h2xml_title_agile_rev  {VOICE_MODULE_DRC}
    @h2xml_title_date       {APRIL 09, 2017} */


#define VOICE_PARAM_DRC 0x00010E0E

#include "adsp_begin_pack.h"

typedef struct voice_param_drc_t voice_param_drc_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_DRC", VOICE_PARAM_DRC}

    @h2xmlp_description  {Dynamic Range Control (DRC) algorithm.\n
Fixed size for this parameter \n
Version 0 – 56 bytes \n\n} */

struct voice_param_drc_t {

    int16_t Mode;
    /**< @h2xmle_description {Enable/disables the DRC feature}

         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t Delay;
    /**< @h2xmle_description {Delay on the signal path to apply gain}

         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    uint16_t rmsTavUL16Q16;
    /**< @h2xmle_description {Time-averaging constant for computing energy}

         @h2xmle_range       {0..65535}


         @h2xmle_policy       {Basic} */

    uint16_t makeupGainUL16Q12;
    /**< @h2xmle_description {Makeup gain in dB applied after DRC.}

         @h2xmle_range       {0..65535}



         @h2xmle_policy       {Basic} */

    int16_t reserved;
    /**< @h2xmle_description {Client must set this field to zero.}

         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t dnExpaThresholdL16Q7;
    /**< @h2xmle_description {Downward expander threshold in dB, Q8.7}

         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    int16_t dnExpaSlopeL16Q8;
    /**< @h2xmle_description {Downward expander slope,Q0.16}

         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    uint16_t dnExpaHysterisisUL16Q14;
    /**< @h2xmle_description {Downward expander gain hysteresis in dB on attack side to avoid gain fluctuations: Q2.14, 0 dB = 0x4000, Hysteresis(hex) = dec2hex(round(10^(Hysteresis(dB)/20) *16384))}
         @h2xmle_range       {0..65535}

         @h2xmle_policy       {Basic} */

    uint32_t dnExpaAttackUL32Q31;
    /**< @h2xmle_description {Downward expander gain smooth attack time in milliseconds }
         @h2xmle_range       {0..4294967295}

         @h2xmle_policy       {Basic} */

    uint32_t dnExpaReleaseUL32Q31;
    /**< @h2xmle_description {Downward expander gain smooth release time in milliseconds }
         @h2xmle_range       {0..4294967295}

         @h2xmle_policy       {Basic} */

    int32_t dnExpaMinGainDBL32Q23;
    /**< @h2xmle_description {Downward expander minimum gain in dB, Q9.23}
         @h2xmle_range       {-2147483648..2147483647}

         @h2xmle_policy       {Basic} */

    int16_t upCompThresholdL16Q7;
    /**< @h2xmle_description {Upward compressor threshold in dB Q8.7}
         @h2xmle_range       {-32768..32767}


         @h2xmle_policy       {Basic} */

    uint16_t upCompSlopeUL16Q16;
    /**< @h2xmle_description {Upward compression slope Q0.16}
         @h2xmle_range       {0..65535}

         @h2xmle_policy       {Basic} */

    uint32_t upCompAttackUL32Q31;
    /**< @h2xmle_description {Upward compressor dynamic gain smooth attack time in milliseconds}
         @h2xmle_range       {0..4294967295}

         @h2xmle_policy       {Basic} */

    uint32_t upCompReleaseUL32Q31;
    /**< @h2xmle_description {Upward compressor dynamic gain smooth release time in milliseconds}
         @h2xmle_range       {0..4294967295}


         @h2xmle_policy       {Basic} */

    uint16_t upCompHysterisisUL16Q14;
    /**< @h2xmle_description {Upward compressor gain hysteresis in dB on the release side to avoid gain fluctuations  Q2.14}
         @h2xmle_range       {0..65535}

         @h2xmle_policy       {Basic} */

    int16_t dnCompThresholdL16Q7;
    /**< @h2xmle_description {Downward compressor threshold in dB Q8.7}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    uint32_t dnCompAttackUL32Q31;
    /**< @h2xmle_description {Downward compressor dynamic gain smooth attack time in milliseconds}
         @h2xmle_range       {0..4294967295}


         @h2xmle_policy       {Basic} */

    uint32_t dnCompReleaseUL32Q31;
    /**< @h2xmle_description {Downward compressor dynamic gain smooth release time in milliseconds}
         @h2xmle_range       {0..4294967295}

         @h2xmle_policy       {Basic} */

    uint16_t dnCompSlopeUL16Q16;
    /**< @h2xmle_description {Downward compression slope Q0.16}
         @h2xmle_range       {0..65535}


         @h2xmle_policy       {Basic} */

    uint16_t dnCompHysterisisUL16Q14;
    /**< @h2xmle_description {Downward compressor gain hysteresis in dB on release side to avoid gain fluctuation,Q2.14}
         @h2xmle_range       {0..65535}

         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;




#define VOICE_MODULE_TX_DRC  0x00010EF3
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_DRC",
                          VOICE_MODULE_TX_DRC}
    @h2xmlm_displayName  {"TX VOICE DRC"}
	@h2xmlm_description {DRC algorithm in the voice preprocessor on the Tx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_DRC\n\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

   @h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

   @h2xml_Select					{voice_param_drc_t}
   @h2xmlm_InsertParameter


    @h2xml_Select       {voice_param_drc_t::Mode}
    @h2xmle_default     {1}


    @h2xml_Select       {voice_param_drc_t::Delay}
    @h2xmle_default     {80}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "40"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "80"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "160"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "240"}

    @h2xml_Select       {voice_param_drc_t::rmsTavUL16Q16}
    @h2xmle_default     {299}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "716"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "299"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1484"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "993"}

    @h2xml_Select       {voice_param_drc_t::makeupGainUL16Q12}

    @h2xmle_default     {4096}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "16306"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "4096"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "16306"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "16306"}

    @h2xml_Select       {voice_param_drc_t::reserved}

    @h2xmle_default     {0}

    @h2xml_Select       {voice_param_drc_t::dnExpaThresholdL16Q7}

    @h2xmle_default      {3239}

    @h2xml_Select       {voice_param_drc_t::dnExpaSlopeL16Q8}

    @h2xmle_default     {-383}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "-1022"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "-383"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "-1022"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "-1022"}

    @h2xml_Select       {voice_param_drc_t::dnExpaHysterisisUL16Q14}

     @h2xmle_default    {16384}

    @h2xml_Select       {voice_param_drc_t::dnExpaAttackUL32Q31}
    @h2xmle_default     {1604514739}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "516314934"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1604514739"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "142678726"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "142678726"}

    @h2xml_Select       {voice_param_drc_t::dnExpaReleaseUL32Q31}
    @h2xmle_default     {1604514739}

   @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "516314934"}
   @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1604514739"}
   @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "142678726"}
   @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "142678726"}

    @h2xml_Select       {voice_param_drc_t::dnExpaMinGainDBL32Q23}

   @h2xmle_default     {-50331648}


    @h2xml_Select       {voice_param_drc_t::upCompThresholdL16Q7}

    @h2xmle_default   {3239}


    @h2xml_Select       {voice_param_drc_t::upCompSlopeUL16Q16}

    @h2xmle_default     {0}

    @h2xml_Select       {voice_param_drc_t::upCompAttackUL32Q31}

    @h2xmle_default     {5897467}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "5897467"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5897467"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "2950761"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1967625"}


    @h2xml_Select       {voice_param_drc_t::upCompReleaseUL32Q31}

     @h2xmle_default     {5897467}

     @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "5897467"}
     @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5897467"}
     @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "2950761"}
     @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1967625"}

    @h2xml_Select       {voice_param_drc_t::upCompHysterisisUL16Q14}

    @h2xmle_default     {18855}

    @h2xml_Select       {voice_param_drc_t::dnCompThresholdL16Q7}

    @h2xmle_default     {9063}


    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "6951"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "9063"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "6951"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "6951"}

     @h2xml_Select       {voice_param_drc_t::dnCompAttackUL32Q31}
     @h2xmle_default     {1604514739}

     @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "516314934"}
     @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1604514739"}
     @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "142678726"}
     @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "142678726"}



    @h2xml_Select       {voice_param_drc_t::dnCompReleaseUL32Q31}

     @h2xmle_default     {1604514739}
     @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "516314934"}
     @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1604514739"}
     @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "142678726"}
     @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "142678726"}

    @h2xml_Select       {voice_param_drc_t::dnCompSlopeUL16Q16}

    @h2xmle_default     {64879}

    @h2xml_Select       {voice_param_drc_t::dnCompHysterisisUL16Q14}

     @h2xmle_default     {16384}


@}                   <-- End of the Module -->*/



#define VOICE_MODULE_RX_DRC  0x00010EF2
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_DRC",
                          VOICE_MODULE_RX_DRC}
    @h2xmlm_displayName  {"RX VOICE DRC"}
	@h2xmlm_description {DRC algorithm in the voice preprocessor on the Rx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_DRC\n\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_drc_t}
    @h2xmlm_InsertParameter

    @h2xml_Select       {voice_param_drc_t::Mode}
    @h2xmle_default     {1}


    @h2xml_Select       {voice_param_drc_t::Delay}
    @h2xmle_default     {80}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "40"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "80"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "160"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "240"}

    @h2xml_Select       {voice_param_drc_t::rmsTavUL16Q16}
    @h2xmle_default     {299}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "598"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "299"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1484"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "993"}

    @h2xml_Select       {voice_param_drc_t::makeupGainUL16Q12}

    @h2xmle_default     {4096}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "16306"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "4096"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "16306"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "16306"}

    @h2xml_Select       {voice_param_drc_t::reserved}

    @h2xmle_default     {0}

    @h2xml_Select       {voice_param_drc_t::dnExpaThresholdL16Q7}

    @h2xmle_default      {3239}

    @h2xml_Select       {voice_param_drc_t::dnExpaSlopeL16Q8}

    @h2xmle_default     {-383}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "-1022"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "-383"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "-1022"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "-1022"}

    @h2xml_Select       {voice_param_drc_t::dnExpaHysterisisUL16Q14}

    @h2xmle_default     {16384}

    @h2xml_Select       {voice_param_drc_t::dnExpaAttackUL32Q31}
    @h2xmle_default     {1604514739}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "2010199605"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1604514739"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1067661045"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "789550996"}

    @h2xml_Select       {voice_param_drc_t::dnExpaReleaseUL32Q31}
    @h2xmle_default     {1604514739}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "2010199605"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1604514739"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1067661045"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "789550996"}

    @h2xml_Select       {voice_param_drc_t::dnExpaMinGainDBL32Q23}

    @h2xmle_default   {-50331648}


    @h2xml_Select       {voice_param_drc_t::upCompThresholdL16Q7}

    @h2xmle_default   {3239}


    @h2xml_Select       {voice_param_drc_t::upCompSlopeUL16Q16}

    @h2xmle_default     {0}

    @h2xml_Select       {voice_param_drc_t::upCompAttackUL32Q31}
    @h2xmle_default     {5897467}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "5897467"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5897467"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "2950761"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1967625"}

    @h2xml_Select       {voice_param_drc_t::upCompReleaseUL32Q31}
    @h2xmle_default     {5897467}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "5897468"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "5897467"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "2950761"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1967625"}

    @h2xml_Select       {voice_param_drc_t::upCompHysterisisUL16Q14}

    @h2xmle_default     {18855}

    @h2xml_Select       {voice_param_drc_t::dnCompThresholdL16Q7}

    @h2xmle_default     {9063}

    @h2xml_Select       {voice_param_drc_t::dnCompAttackUL32Q31}
    @h2xmle_default     {1604514739}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "2010199605"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1604514739"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1067661045"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "789550996"}

    @h2xml_Select       {voice_param_drc_t::dnCompReleaseUL32Q31}
    @h2xmle_default     {1604514739}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "2010199605"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "1604514739"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1067661045"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "789550996"}

    @h2xml_Select       {voice_param_drc_t::dnCompSlopeUL16Q16}

    @h2xmle_default     {64879}

    @h2xml_Select       {voice_param_drc_t::dnCompHysterisisUL16Q14}

     @h2xmle_default     {16384}




@}                   <-- End of the Module -->*/



#endif
