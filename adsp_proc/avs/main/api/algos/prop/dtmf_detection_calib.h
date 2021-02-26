#ifndef DTMF_DETECTION_CALIB_H
#define DTMF_DETECTION_CALIB_H
/*==============================================================================
  @file dtmf_detection_calib.h
  @brief This file contains DTMF_DETECTION
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


/** @h2xml_title1           {VOICE_MODULE_DTMF_DETECTION}
    @h2xml_title_agile_rev  {VOICE_MODULE_DTMF_DETECTION}
    @h2xml_title_date       {March 13, 2017} */


#define VOICE_PARAM_DTMF_DETECTION 0x00010E0C

#include "adsp_begin_pack.h"

typedef struct voice_param_dtmf_detection_t voice_param_dtmf_detection_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_DTMF_DETECTION", VOICE_PARAM_DTMF_DETECTION}
    @h2xmlp_description {DTMF detection algorithm.\n
	Fixed size for this parameter\n
	Version 0 – 36 bytes\n} */

struct voice_param_dtmf_detection_t {

    int16_t MagnitudeThresh;
    /**< @h2xmle_description {Tx DTMF detection magnitude threshold. This corresponds to a minimum energy level of -29 dBm in each of the two tones in the input signal. On the Rx side, a lesser nominal value is suggested, as the DTMF detection is carried out on the output of the decoder.}
         @h2xmle_default     {0x05}
         @h2xmle_range       {0x0000..0x3FFF}

         @h2xmle_policy       {Basic} */

    int16_t FwTwistFactor;
    /**< @h2xmle_description {Tx DTMF detection forward twist factor. Used when the energy in the highest-magnitude tone in Band 1 has a higher energy than the highest-magnitude tone in Band 2.}
         @h2xmle_default     {0x09}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t RevTwistFactor;
    /**< @h2xmle_description {Tx DTMF detection reverse twist factor. Used when the energy in the highest-magnitude tone in Band 2 has a higher energy than the highest-magnitude tone in Band 1.}
         @h2xmle_default     {0x29}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t FreqRatio1;
    /**< @h2xmle_description {Tx DTMF detection maximum ratio between two frequencies in the same band}
         @h2xmle_default     {0x07}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t FreqRatio2;
    /**< @h2xmle_description {Tx DTMF detection maximum ratio between two frequencies in the same band}
         @h2xmle_default     {0x0A}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t FreqRatio3;
    /**< @h2xmle_description {Tx DTMF detection maximum ratio between two frequencies in the same band}
         @h2xmle_default     {0x06}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t FreqRatio4;
    /**< @h2xmle_description {Tx DTMF detection maximum ratio between two frequencies in the same band}
         @h2xmle_default     {0x04}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t FreqRatio5;
    /**< @h2xmle_description {Tx DTMF detection maximum ratio between two frequencies in the same band}
         @h2xmle_default     {0x09}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t FreqRatio6;
    /**< @h2xmle_description {Tx DTMF detection maximum ratio between two frequencies in the same band}
         @h2xmle_default     {0x0C}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t FreqRatio7;
    /**< @h2xmle_description {Tx DTMF detection maximum ratio between two frequencies in the same band}
         @h2xmle_default     {0x0D}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t FreqRatio8;
    /**< @h2xmle_description {Tx DTMF detection maximum ratio between two frequencies in the same band}
         @h2xmle_default     {0x11}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t DetectionThresh1;
    /**< @h2xmle_description {Tx DTMF detection Threshold1. Used in the energy ratio test for band 1. This parameter is a Q15 unsigned number.}
         @h2xmle_default     {0x093C}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t DetectionThresh2;
    /**< @h2xmle_description {Tx DTMF detection Threshold2. Used in the energy ratio test for band 2. This parameter is a Q15 unsigned number.}
         @h2xmle_default     {0x2666}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t DetectionThresh31;
    /**< @h2xmle_description {Threshold31, used in an energy ratio test.}
         @h2xmle_default     {0x05}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t DetectionThresh32;
    /**< @h2xmle_description {Threshold32, used in an energy ratio test}
         @h2xmle_default     {0x04}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t ParamL;
    /**< @h2xmle_description {To do}
         @h2xmle_default     {0x08}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t ParamM;
    /**< @h2xmle_description {To do}
         @h2xmle_default     {0x05}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

    int16_t ParamN;
    /**< @h2xmle_description {To do}
         @h2xmle_default     {0x02}
         @h2xmle_range       {-32768..32767}

         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;



#define VOICE_MODULE_DTMF_DETECTION  0x00010EEC
/**
    @h2xmlm_module       {"VOICE_MODULE_DTMF_DETECTION",
                          VOICE_MODULE_DTMF_DETECTION}
    @h2xmlm_displayName  {"RX VOICE DTMF DETECTION"}
	@h2xmlm_description {DTMF detection algorithm in the voice postprocessor on the Rx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_DTMF_DETECTION\n\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xmlp_toolPolicy	        {NO_SUPPORT }

    @h2xml_Select					{voice_param_dtmf_detection_t}
    @h2xmlm_InsertParameter

    @}                   <-- End of the Module -->*/


#define VOICE_MODULE_TX_DTMF_DETECTION  0x00010F01
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_DTMF_DETECTION",
                          VOICE_MODULE_TX_DTMF_DETECTION}
    @h2xmlm_displayName  {"TX VOICE DTMF DETECTION"}
	@h2xmlm_description {DTMF detection algorithm in the voice postprocessor on the Tx path.\n
This module supports the following parameter IDs \n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_DTMF_DETECTION\n\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xmlp_toolPolicy	        {NO_SUPPORT}

    @h2xml_Select					{voice_param_dtmf_detection_t}
    @h2xmlm_InsertParameter
    @h2xmlp_toolPolicy	        {NO_SUPPORT}




   @}                   <-- End of the Module -->*/
#endif
