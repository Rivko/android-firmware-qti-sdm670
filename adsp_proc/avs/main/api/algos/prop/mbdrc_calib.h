#ifndef _MBDRC_CALIB_H_
#define _MBDRC_CALIB_H_
/*==============================================================================
  @file mbdrc_calib.h
  @brief This file contains MBDRC
==============================================================================*/
/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who              what, where, why
 --------   ---              -------------------------------------------------------
 04/19/17   shrani/ritheshr  Added h2xml comments.
 ========================================================================== */

/** @h2xml_title1           {MBDRC_CALIB_H}
    @h2xml_title_agile_rev  {MBDRC_CALIB_H}
    @h2xml_title_date       {April 19, 2017} */


#include "mmdefs.h"
#include "audio_common_enable_calib.h"
#include "voice_param_mod_enable_calib.h"
#include "mbdrc_param_calib.h"


#define AUDPROC_MODULE_ID_MBDRC                                    0x00010C06

/**

    @h2xmlm_module       {"AUDPROC_MODULE_ID_MBDRC",
                          AUDPROC_MODULE_ID_MBDRC}
	@h2xmlm_displayName  {"AUDIO MBDRC"}
    @h2xmlm_toolPolicy   {Calibration; RTC}
    @h2xmlm_description  {Mutliband Dynamic Range Control Module}
    @h2xmlm_deprecated              {true}
    @{                   <-- Start of the Module -->
*/


/**
    @h2xml_Select					{audproc_mbdrc_enable_t}
    @h2xmlm_InsertParameter
*/

/**
    @h2xml_Select              {audproc_mbdrc_config_params_t::firFilter}
    @h2xmle_variableArraySize  { "97*((num_bands + 3)/5) + 33*((num_bands+2)/5) + 33*((num_bands+1)/5) + 33*(num_bands/5)" }
*/

/**
    @h2xml_Select					{audproc_mbdrc_config_params_t}
    @h2xmlm_InsertParameter
    @h2xmlp_parameter          {"AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS", AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS}
*/

/**
    @h2xml_Select					{audproc_subband_drc_config_params_t}
    @h2xmlm_InsertParameter
*/

/** @}                   <-- End of the Module -->*/


/*  ID of the MBDRC version 2 module pre/postprocessing block.

    This MBDRCV2 module differs from the original MBDRC
    (#AUDPROC_MODULE_ID_MBDRC) in the length of the filters used in each
    sub-band.

    This module supports the
    AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2
    parameter ID.
 */
#define AUDPROC_MODULE_ID_MBDRCV2                                0x0001070B
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_MBDRCV2",
                          AUDPROC_MODULE_ID_MBDRCV2}
    @h2xmlm_displayName  {"AUDIO MBDRC_V2"}
    @h2xmlm_toolPolicy   {Calibration; RTC}
    @h2xmlm_deprecated              {true}
    @h2xmlm_description  {Mutliband Dynamic Range Control Module V2}
    @{                   <-- Start of the Module -->
*/


/**
    @h2xml_Select					{audproc_mbdrc_enable_t}
    @h2xmlm_InsertParameter
*/

/**


    @h2xml_Select					{audproc_mbdrc_config_params_t}
    @h2xmlm_InsertParameter
    @h2xmlp_parameter          {"AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2", AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2}


*/

/**
    @h2xml_Select					{audproc_subband_drc_config_params_t}
    @h2xmlm_InsertParameter
*/

/**   @}                   <-- End of the Module -->*/



/*  ID of the Multiband Dynamic Range Control (MBDRC24Bit) module on the Tx
    and Rx paths. This parameter supports both 16-bit and 24-bit data.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_MBDRC_ENABLE
    - #AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_V3
    - #AUDPROC_PARAM_ID_ENABLE
    - #AUDPROC_PARAM_ID_MBDRC_FILTER_XOVER_FREQS
 */
#define AUDPROC_MODULE_ID_MBDRCV3                              0x0001090B

/**

    @h2xmlm_module       {"AUDPROC_MODULE_ID_MBDRCV3",
                          AUDPROC_MODULE_ID_MBDRCV3}
	@h2xmlm_displayName  {"AUDIO MBDRC_V3"}
    @h2xmlm_toolPolicy   {Calibration; RTC}
    @h2xmlm_description  {Mutliband Dynamic Range Control Module V3}
    @{                   <-- Start of the Module -->
*/


/**
    @h2xml_Select					{audproc_mbdrc_enable_t}
    @h2xmlm_InsertParameter
*/

/**
    @h2xml_Select					{audproc_mbdrc_config_params_v3_t}
    @h2xmlm_InsertParameter
*/

/**
	@h2xml_Select					{audproc_mbdrc_filter_xover_freqs_t}
   @h2xmlm_InsertParameter
*/

/**
	@h2xml_Select					{audproc_subband_drc_config_params_t}
   @h2xmlm_InsertParameter
*/

/** @}                   <-- End of the Module -->*/




/* DRC configuration structure for each sub-band of an MBDRC module. */

/** @h2xmlp_subStruct */

typedef struct voice_subband_drc_config_params_t
{
   int16_t sDrcStereoLinked;
   /**< @h2xmle_description {Options to process stereo signal with: Linked DRC “ All stereo channels have the same applied dynamics, Unlinked DRC “ Stereo channels process their dynamics independently.}
        @h2xmle_rangeList   {"Not linked,channels process the dynamics independently" = 0;
                             "Linked,channels have the same applied dynamics" = 1}
		@h2xmle_default     {1} */

   int16_t sDrcMode;
   /**< @h2xmle_description {Bypasses DRC for subbands.}
        @h2xmle_rangeList   {"Disable"=0; "Enable"=1}
		@h2xmle_default     {1} */

   int16_t sDrcDownSampleLevel;
   /**< @h2xmle_description {Update DRC gain every this number of samples instead of every sample.}
        @h2xmle_default     {1}
		@h2xmle_range       {0..16}*/

   int16_t sDrcDelay;
   /**< @h2xmle_description {Delay on the signal path in each subband DRC to apply gain. }
        @h2xmle_range       {0..1200}
		@h2xmle_default     {0xF0} */

   uint16_t usDrcRmsTav;
   /**< @h2xmle_description {Time-averaging constant for computing energy. }
        @h2xmle_range       {0..65535}
		@h2xmle_default     {0x78}		*/

   uint16_t usMakeupGain;
   /**< @h2xmle_description {Makeup gain in dB applied after DRC}
        @h2xmle_default     {0x1000}
        @h2xmle_range       {258..64917}
		@h2xmle_dataFormat  {Q3.12} 		*/

   /* Down expander settings */

   int16_t sDnExpaThresholdL;
   /**< @h2xmle_description {Downward expander threshold in dB.}
                             Its value must be: (<down_cmpsr_threshold)}
        @h2xmle_default     {0x0A27}
        @h2xmle_range       {0..11560}
        @h2xmle_dataFormat  {Q8.7}  */

   int16_t sDnExpaSlopeL;
   /**< @h2xmle_description {Down expander slope.}
        @h2xmle_default     {-256}
        @h2xmle_range       {-32768..0}
        @h2xmle_dataFormat  {Q0.16}  */

   uint32_t ulDnExpaAttack;
   /**< @h2xmle_description {Downward expander gain smooth attack time in ms}
        @h2xmle_default     {0x05BBF800}
        @h2xmle_range       {0..2147483648}
         */

   uint32_t ulDnExpaRelease;
   /**< @h2xmle_description {Downward expander gain smooth release time in ms.}
        @h2xmle_default     {0x05BBF800}
        @h2xmle_range       {0..2147483648}
          */

   uint16_t usDnExpaHysterisis;
   /**< @h2xmle_description {Downward expander gain smooth release time in ms.}
        @h2xmle_default     {0x4000}
        @h2xmle_range       {1..32690}
         */

   uint16_t usReserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_default     {0} */

   int32_t ulDnExpaMinGainDB;
   /**< @h2xmle_description {Downward expander minimum gain in dB}
        @h2xmle_range       {-805306368..0}
        @h2xmle_dataFormat  {Q9.23}
		@h2xmle_default     {-50331648} */

   /* Up compressor settings */

   int16_t sUpCompThreshold;
   /**< @h2xmle_description {Up compressor threshold.
                             Its value must be:
                             (&gt;down_expdr_threshold) &amp;&amp; (&lt;down_cmpsr_threshold)}
        @h2xmle_default     {0x0A27}
        @h2xmle_range       {0..11560}
        @h2xmle_dataFormat  {Q8.7}  */

   uint16_t sUpCompSlope;
   /**< @h2xmle_description {Up compressor slope.}
        @h2xmle_range       {0..64881}
        @h2xmle_dataFormat  {Q0.16}
		@h2xmle_default     {0x0} */

   uint32_t ulUpCompAttack;
   /**< @h2xmle_description {Upward compressor dynamic gain smooth attack time in ms.}
        @h2xmle_default     {0x05BBF800}
        @h2xmle_range       {0..2147483648}
         */

   uint32_t ulUpCompRelease;
   /**< @h2xmle_description {Upward compressor dynamic gain smooth release time in ms.}
        @h2xmle_default     {0x05BBF800}
        @h2xmle_range       {0..2147483648}
        */

   uint16_t usUpCompHysterisis;
   /**< @h2xmle_description {Upward compressor gain hysteresis in dB on the release side to avoid gain fluctuations.}
        @h2xmle_default     {0x4000}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q2.14}  */

   /* Down compressor settings */

   int16_t sDnCompThreshold;
   /**< @h2xmle_description {Downward compressor threshold in dB.
                             Its value must be: (&gt;up_cmpsr_threshold)}
        @h2xmle_default     {0x2A27}
        @h2xmle_range       {0..11560}
        @h2xmle_dataFormat  {Q8.7}
		*/

   uint16_t sDnCompSlope;
   /**< @h2xmle_description {Downward compression slope.}
        @h2xmle_range       {0..64881}
        @h2xmle_dataFormat  {Q0.16}
		@h2xmle_default     {0xFD70} */

   uint16_t usReserved1;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_default     {0} */

   uint32_t ulDnCompAttack;
   /**< @h2xmle_description {Downward compressor dynamic gain smooth attack time in ms.}
        @h2xmle_default     {0x2F0F9794}
        @h2xmle_range       {0..2147483648}
          */

   uint32_t ulDnCompRelease;
   /**< @h2xmle_description {Downward compressor dynamic gain smooth release time in ms.}
        @h2xmle_default     {0x2F0F9794}
        @h2xmle_range       {0..2147483648}
         */

   uint16_t usDnCompHysterisis;
   /**< @h2xmle_description {Downward compressor gain hysteresis in dB on release side to avoid gain fluctuation.}
        @h2xmle_default     {0x4000}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q2.14}  */

   uint16_t usReserved2;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_default     {0} */

}voice_subband_drc_config_params_t;


#define VOICE_PARAM_MBDRC  0x00010E08

typedef struct voice_param_mbdrc_t voice_param_mbdrc_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_MBDRC", VOICE_PARAM_MBDRC}
    @h2xmlp_description {Multiband dynamic range control (MBDRC) algorithm.\n
Fixed size for this parameter
Version 0 -- 12 bytes, followed by variable size DRC and FIR filter structures \n
The filter sizes are defined by configurable parameters. The maximum size for voice applications is 472
bytes. For voice, the maximum number of bands is assumed to be 3. \n}
    @h2xmlp_deprecated              {true}
 @h2xmlx_expandStructs	{false} */

#include "adsp_begin_pack.h"

struct voice_param_mbdrc_t
{
   uint16_t usNumBands;
   /**< @h2xmle_description  {Number of bands.}
        @h2xmle_default      {1}
        @h2xmle_range        {1..3}
        @h2xmle_readOnly     {true}	*/

   int16_t sLimThreshold;
   /**< @h2xmle_description  {Threshold in dB to limit peaks.}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {0x0B11}
        @h2xmle_dataFormat   {Q2.13} */

   int16_t sLimMakeupGain;
   /**< @h2xmle_description  {Makeup gain in dB to increase overall signal level after peak limiting.}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {0x100}
        @h2xmle_dataFormat   {Q7.8}  */

   int16_t sLimGc;
   /**< @h2xmle_description  {Gain recovery constant. Controls how fast or slow the gain change should recover; a high value implies slower recovery}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {0x799A}
        @h2xmle_dataFormat   {Q0.15}  */

   int16_t sLimDelay;
   /**< @h2xmle_description  {Delay buffer length in ms}
        @h2xmle_range        {0..328}
        @h2xmle_default      {0x0147}
        @h2xmle_dataFormat   {Q0.15}  */

   int16_t sLimMaxWait;
   /**< @h2xmle_description  {Maximum window size in ms, not used when delay is set to a nonzero value.}
        @h2xmle_range        {0..328}
        @h2xmle_default      {0x0147}
        @h2xmle_dataFormat   {Q0.15}  */

   voice_subband_drc_config_params_t subDrcCfg[0];
   /**< @h2xmle_description  {subDrcCfg}
        @h2xmle_variableArraySize {usNumBands}
        */

   int16_t sFilterCoeff[0];
   /**< @h2xmle_description  { sFilterCoeff}
        @h2xmle_variableArraySize  { "97*((usNumBands + 3)/5) + 33*((usNumBands + 2)/5)" }
        @h2xmle_default      {0}
		@h2xmle_range        {-32768..32767}
         */

   int16_t sMuteFlags[0];
   /**< @h2xmle_description  {sMuteFlags}
        @h2xmle_default      {0}
		@h2xmle_range        {-32768..32767}
        @h2xmle_variableArraySize {usNumBands} */

   uint16_t Reserved;
   /**< @h2xmle_description  {Client should set this field as zero}
        @h2xmle_default      {0} */

}
#include "adsp_end_pack.h"
;



#define VOICE_PARAM_MBDRC_FILTER_XOVER_FREQ 0x00010E67

#define MBDRC_MAX_BANDS 5
typedef struct voice_param_mbdrc_filter_xover_freq_t voice_param_mbdrc_filter_xover_freq_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_MBDRC_FILTER_XOVER_FREQ", VOICE_PARAM_MBDRC_FILTER_XOVER_FREQ}
    @h2xmlp_description {Monitors the MBDRC filter crossover frequencies for different sub-bands.\n
Fixed size for this parameter\n
Version 0 -- 16 bytes for five bands\n} */
#include "adsp_begin_pack.h"

struct voice_param_mbdrc_filter_xover_freq_t
{
   uint32_t filter_xover_freqs[MBDRC_MAX_BANDS-1];
   /**< @h2xmle_description  {Array of filter
crossover frequencies}
		@h2xmle_default {0}
		@h2xmle_range {0..2147483647}
        */
}
#include "adsp_end_pack.h"
;


/* DRC configuration structure for each sub-band of an MBDRC module. */

/** @h2xmlp_subStruct */

typedef struct voice_subband_drc_config_params_v2_t
{
   int16_t sDrcStereoLinked;
   /**< @h2xmle_description {Options to process stereo signal with: Linked DRC “ All stereo channels have the same applied dynamics, Unlinked DRC “ Stereo channels process their dynamics independently.}
        @h2xmle_rangeList   {"Not linked,channels process the dynamics independently" = 0;
                             "Linked,channels have the same applied dynamics" = 1}
		@h2xmle_default     {1} */

   int16_t sDrcMode;
   /**< @h2xmle_description {Bypasses DRC for subbands.}
        @h2xmle_rangeList   {"Disable"=0; "Enable"=1}
		@h2xmle_default     {1} */

   int16_t sDrcDownSampleLevel;
   /**< @h2xmle_description {Update DRC gain every this number of samples instead of every sample.}
        @h2xmle_default     {1}
		@h2xmle_range       {0..16}*/

   int16_t sDrcDelay;
   /**< @h2xmle_description {Delay on the signal path in each subband DRC to apply gain. }
        @h2xmle_range       {0..1200}
		@h2xmle_default     {0xF0} */

   uint16_t usDrcRmsTav;
   /**< @h2xmle_description {Time-averaging constant for computing energy. }
        @h2xmle_range       {0..65535}
		@h2xmle_default     {0x78}		*/

   uint16_t usMakeupGain;
   /**< @h2xmle_description {Makeup gain in dB applied after DRC}
        @h2xmle_default     {0x1000}
        @h2xmle_range       {258..64917}
		@h2xmle_dataFormat  {Q3.12} 		*/

   /* Down expander settings */

   int16_t sDnExpaThresholdL;
   /**< @h2xmle_description {Downward expander threshold in dB.}
                             Its value must be: (<down_cmpsr_threshold)}
        @h2xmle_default     {0x0F27}
        @h2xmle_range       {-32768..32767}
        @h2xmle_dataFormat  {Q8.7}  */

   int16_t sDnExpaSlopeL;
   /**< @h2xmle_description {Down expander slope.}
        @h2xmle_default     {-256}
        @h2xmle_range       {-32768..0}
        @h2xmle_dataFormat  {Q0.16}  */

   uint32_t ulDnExpaAttack;
   /**< @h2xmle_description {Downward expander gain smooth attack time in ms}
        @h2xmle_default     {0x05BBF800}
        @h2xmle_range       {0..2147483648}
         */

   uint32_t ulDnExpaRelease;
   /**< @h2xmle_description {Downward expander gain smooth release time in ms.}
        @h2xmle_default     {0x05BBF800}
        @h2xmle_range       {0..2147483648}
          */

   uint16_t usDnExpaHysterisis;
   /**< @h2xmle_description {Downward expander gain smooth release time in ms.}
        @h2xmle_default     {0x4000}
        @h2xmle_range       {1..32690}
         */

   uint16_t usReserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_default     {0} */

   int32_t ulDnExpaMinGainDB;
   /**< @h2xmle_description {Downward expander minimum gain in dB}
        @h2xmle_range       {-805306368..0}
        @h2xmle_dataFormat  {Q9.23}
		@h2xmle_default     {-50331648} */

   /* Up compressor settings */

   int16_t sUpCompThreshold;
   /**< @h2xmle_description {Up compressor threshold.
                             Its value must be:
                             (&gt;down_expdr_threshold) &amp;&amp; (&lt;down_cmpsr_threshold)}
        @h2xmle_default     {0x1E27}
        @h2xmle_range       {-32768..32767}
        @h2xmle_dataFormat  {Q8.7}  */

   uint16_t sUpCompSlope;
   /**< @h2xmle_description {Up compressor slope.}
        @h2xmle_range       {0..64881}
        @h2xmle_dataFormat  {Q0.16}
		@h2xmle_default     {0x0} */

   uint32_t ulUpCompAttack;
   /**< @h2xmle_description {Upward compressor dynamic gain smooth attack time in ms.}
        @h2xmle_default     {0x05BBF800}
        @h2xmle_range       {0..2147483648}
         */

   uint32_t ulUpCompRelease;
   /**< @h2xmle_description {Upward compressor dynamic gain smooth release time in ms.}
        @h2xmle_default     {0x05BBF800}
        @h2xmle_range       {0..2147483648}
        */

   uint16_t usUpCompHysterisis;
   /**< @h2xmle_description {Upward compressor gain hysteresis in dB on the release side to avoid gain fluctuations.}
        @h2xmle_default     {0x4000}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q2.14}  */

   /* Down compressor settings */

   int16_t sDnCompThreshold;
   /**< @h2xmle_description {Downward compressor threshold in dB.
                             Its value must be: (&gt;up_cmpsr_threshold)}
        @h2xmle_default     {0x25A7}
        @h2xmle_range       {-32768..11560}
        @h2xmle_dataFormat  {Q8.7}
		*/

   uint16_t sDnCompSlope;
   /**< @h2xmle_description {Downward compression slope.}
        @h2xmle_range       {0..64881}
        @h2xmle_dataFormat  {Q0.16}
		@h2xmle_default     {0xFD70} */

   uint16_t usReserved1;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_default     {0} */

   uint32_t ulDnCompAttack;
   /**< @h2xmle_description {Downward compressor dynamic gain smooth attack time in ms.}
        @h2xmle_default     {0x2F0F9794}
        @h2xmle_range       {0..2147483648}
          */

   uint32_t ulDnCompRelease;
   /**< @h2xmle_description {Downward compressor dynamic gain smooth release time in ms.}
        @h2xmle_default     {0x2F0F9794}
        @h2xmle_range       {0..2147483648}
         */

   uint16_t usDnCompHysterisis;
   /**< @h2xmle_description {Downward compressor gain hysteresis in dB on release side to avoid gain fluctuation.}
        @h2xmle_default     {0x4000}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q2.14}  */

   uint16_t usReserved2;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_default     {0} */

}voice_subband_drc_config_params_v2_t;

#define VOICE_PARAM_MBDRC_V2 0x00010E66

typedef struct voice_param_mbdrc_v2_t voice_param_mbdrc_v2_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_MBDRC_V2", VOICE_PARAM_MBDRC_V2}
    @h2xmlp_description {MBDRC version 2 algorithm, which supports the following features\n
Calibration for up to five bands\n
NB, WB, SWB, and FB sampling rates\n
Higher order FIR filter for SWB and FB sampling rates (up to 48 kHz processing)\n
The fixed size for this parameter is dynamic based on the number of bands and the FIR filter length, which
depends on the sampling rate (8 kHz, 16 kHz, 32 kHz, or 48 kHz).\n
In the following table, the parameters in bold correspond to sub-band DRC configuration, which is repeated
based on the number of bands (usNumBands).}
@h2xmlp_version     {0}
 @h2xmlx_expandStructs	{false}*/


#include "adsp_begin_pack.h"

struct voice_param_mbdrc_v2_t
{
   uint16_t Version;
   /**< @h2xmle_description  {Version}
        @h2xmle_default      {0}
        @h2xmle_isVersion    {true}
        @h2xmle_readOnly     {true}
        */
    uint16_t usReserved;
   /**< @h2xmle_description  {usReserved}
        @h2xmle_default      {0}
        @h2xmle_readOnly     {true}*/


   uint16_t usNumBands;
   /**< @h2xmle_description  {Number of bands.}
        @h2xmle_default      {1}
        @h2xmle_range        {1..5}
        @h2xmle_readOnly     {true}	*/

   int16_t sLimThreshold;
   /**< @h2xmle_description  {Threshold in dB to limit peaks.}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {0x0B11}
        @h2xmle_dataFormat   {Q2.13} */

   int16_t sLimMakeupGain;
   /**< @h2xmle_description  {Makeup gain in dB to increase overall signal level after peak limiting.}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {0x100}
        @h2xmle_dataFormat   {Q7.8}  */

   int16_t sLimGc;
   /**< @h2xmle_description  {Gain recovery constant. Controls how fast or slow the gain change should recover; a high value implies slower recovery}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {0x799A}
        @h2xmle_dataFormat   {Q0.15}  */

   int16_t sLimDelay;
   /**< @h2xmle_description  {Delay buffer length in ms}
        @h2xmle_range        {0..328}
        @h2xmle_default      {0x0147}
        @h2xmle_dataFormat   {Q0.15}  */

   int16_t sLimMaxWait;
   /**< @h2xmle_description  {Maximum window size in ms, not used when delay is set to a nonzero value.}
        @h2xmle_range        {0..328}
        @h2xmle_default      {0x0147}
        @h2xmle_dataFormat   {Q0.15}  */

   voice_subband_drc_config_params_v2_t subDrcCfg[0];
   /**< @h2xmle_description  {subDrcCfg}
        @h2xmle_variableArraySize {usNumBands}
        */

   int16_t filterlength;
   /**< @h2xmle_description  {The filterlength variable is hard coded based on the number of bands and voice sampling rates.\n\n
        Depending on the number of bands filterlength variable changes (when sampling rates is less than 32 kHz) \n :
	- 1 Band  : 0 \n
	- 2 Bands : 97c\n
	- 3 Bands : 130 (97 + 33) \n
	- 4 Bands : 163 (97 + 33 + 33) \n
	- 5 Bands : 196 (97 + 33 + 33 + 33) \n\n

	Depending on the number of bands filterlength variable changes (when sampling rates is less than 32 kHz) \n :
	- 1 Band  : 0 \n
	- 2 Bands : 141 \n
	- 3 Bands : 222 (141 + 81) \n
	- 4 Bands : 283 (141 + 81 + 61) \n
	- 5 Bands : 344 (141 + 81 + 61 + 61) \n\n }
        @h2xmle_default      {0}
        @h2xmle_readOnly     {true}  */

   int16_t sFilterCoeff[0];
   /**< @h2xmle_description  {sFilterCoeff}
        @h2xmle_default      {0}
	@h2xmle_range        {-32768..32767}
        @h2xmle_variableArraySize  { filterlength } */

   int16_t sMuteFlags[0];
   /**< @h2xmle_description  {sMuteFlags}
        @h2xmle_default      {0}
		@h2xmle_range        {-32768..32767}
        @h2xmle_variableArraySize {usNumBands} */

}
#include "adsp_end_pack.h"
;




#define VOICE_MODULE_MBDRC  0x00010EE6
/**
    @h2xmlm_module       {"VOICE_MODULE_MBDRC",
                          VOICE_MODULE_MBDRC}
    @h2xmlm_displayName  {"RX VOICE MBDRC"}
	@h2xmlm_description {Multiband Dynamic Range Control (MBDRC) algorithm in the voice processor on the Rx path.\n
This module supports the following parameter IDs\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_MBDRC
\n}
    @h2xmlm_deprecated              {true}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->
*/

/**
   @h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

*/

/**
   @h2xml_Select					{voice_param_mbdrc_t}
   @h2xmlm_InsertParameter
   @h2xmle_policy	                                 {advanced}
*/

/**
	@h2xml_Select					{voice_subband_drc_config_params_t}
   @h2xmlm_InsertParameter
*/



/**   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_MBDRC_V2  0x00010F23
/**
    @h2xmlm_module       {"VOICE_MODULE_MBDRC_V2",
                          VOICE_MODULE_MBDRC_V2}
    @h2xmlm_displayName  {"RX VOICE MBDRC_V2"}
	@h2xmlm_description {Multiband Dynamic Range Control (MBDRC) algorithm in the voice processor on the Rx path. This
module provides the following features:\n
Calibration for up to five bands\n
Sampling rates NB, WB, SWB, FB\n
Higher order FIR filter for SWB and FB sampling rates\n\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_MBDRC_V2\n
VOICE_PARAM_MBDRC_FILTER_XOVER_FREQ\n}
    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

*/

/** @h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

 */

/**@h2xml_Select					{voice_param_mbdrc_v2_t}
   @h2xmlm_InsertParameter
   @h2xmle_policy                                 	{advanced}
  */

/**@h2xml_Select			         	{voice_param_mbdrc_filter_xover_freq_t}
   @h2xmlm_InsertParameter
   @h2xmle_policy                               	{advanced}
   */
/**
	@h2xml_Select					{voice_subband_drc_config_params_v2_t}
   @h2xmlm_InsertParameter
*/


/**   @}                   <-- End of the Module -->*/







#endif
