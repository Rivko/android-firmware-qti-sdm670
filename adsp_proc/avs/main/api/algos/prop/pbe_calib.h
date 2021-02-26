#ifndef PBE_CALIB_H
#define PBE_CALIB_H
/*==============================================================================
  @file pbe_calib.h
  @brief This file contains PBE API
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who                 what, where, why
 --------   ---                 -------------------------------------------------------
 04/18/17   shrani/ritheshr     Added h2xml comments.
 ========================================================================== */

/** @h2xml_title1           {Psychoacoustic Bass Enhancement (PBE)}
    @h2xml_title_agile_rev  {Psychoacoustic Bass Enhancement (PBE)}
    @h2xml_title_date       {April 18, 2017} */



#include "mmdefs.h"
#include "voice_param_mod_enable_calib.h"


/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition
   Module library is independent of this number, it defined here for static
   loading purpose only */
#define AUDPROC_MODULE_ID_PBE                              0x00010C2A
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_PBE",
                          AUDPROC_MODULE_ID_PBE}
    @h2xmlm_displayName  {"AUDIO PBE"}
      @h2xmlm_description  {ID of the Reverberation module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_PBE_ENABLE\n
    - #AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG\n
    - #AUDPROC_PARAM_ID_ENABLE\n
    Refer to custom topology recommendations TR1, TR2 and TR7.\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
*/

/* ID of the PBE Enable Configuration parameter used by AUDPROC_MODULE_ID_PBE. */
#define AUDPROC_PARAM_ID_PBE_ENABLE                       0x00010C2B

/* Structure for the enable configuration parameter for PBE enable. */
typedef struct audproc_pbe_enable_t audproc_pbe_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_PBE_ENABLE", AUDPROC_PARAM_ID_PBE_ENABLE}
    @h2xmlp_description {Enables the PBE module}  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_PBE_ENABLE parameter used by the PBE module. */

struct audproc_pbe_enable_t
{
   uint32_t enable_flag;
   /**< @h2xmle_description { Specifies whether the PBE module is enabled.}
        @h2xmle_rangeList      {"Disable"=0; "Enable"=1} */

}
#include "adsp_end_pack.h"
;


/* ID of the PBE Configuration parameter used by AUDPROC_MODULE_ID_PBE. */
#define AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG                0x00010C49

/* Structure for PBE configuration parameters. */
typedef struct audproc_pbe_params_t audproc_pbe_params_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG",AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG}
    @h2xmlp_description {Configures the device.This structure is followed by the filter coefficients.\n
The length is dependent on the number of bands:\n
    - Sequence of int32 p1LowpassCoeffsLQ30 -- Low pass filter coefficients if
      uiXoverFltOrder = 3 {length = 5 * 2}. Else, {length = 5}.\n\n
    - Sequence of int32 p1HighpassCoeffsQ30 -- High pass filter coefficients
      if uiXoverFltOrder = 3 {length = 5 * 2}. Else, {length = 5}.\n\n
    - Sequence of int32 p1BandpassCoeffsQ30 -- Band pass filter coefficients
      if length is 5 * uiBandpassFltOrder. Each band has five coefficients,
      and each coefficient is in int32 format in the order of b0, b1, b2, a1,
      a2.\n\n
    - Sequence of int32 p1BassShelfCoeffsQ30 -- Bass shelf filter coefficients
      of length 5. Each coefficient is in int32 format in the order of b0, b1,
      b2, a1, a2.\n\n
    - Sequence of int32 p1TrebleShelfCoeffsQ30 -- Treble shelf filter
      coefficients of length 5. Each coefficient is in int32 format in the
      order of b0, b1, b2, a1, a2.\n\n } */


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG parameters used by the PBE module. */

struct audproc_pbe_params_t
{
   int16_t real_bass_mix_q15;
   /**< @h2xmle_description {Mix amount of real bass harmonics and virtual bass harmonics.}
		  @h2xmle_default 	{13107}
		  @h2xmle_defaultDependency 	{  Samplerate="8000",  default="19661"}
		  @h2xmle_defaultDependency 	{  Samplerate="16000",  default="19661"}
		  @h2xmle_defaultDependency 	{  Samplerate="32000",  default="13107"}
		  @h2xmle_defaultDependency 	{  Samplerate="48000",  default="13107"}
        @h2xmle_range       {0..32767}
        @h2xmle_dataFormat   {Q15} */


   int16_t bass_color_ctrl_q15;
    /**< @h2xmle_description {Color control of virtual bass.}
        @h2xmle_default     {9830}
        @h2xmle_range      {0..32767}
        @h2xmle_dataFormat   {Q15} */

   uint16_t main_chain_delay;
   /**< @h2xmle_description {Sample delay in the non-bass path to compensate an algorithm delay in the bass path.}
		  @h2xmle_default 	{240}
		  @h2xmle_defaultDependency 	{  Samplerate="8000",  default="40"}
		  @h2xmle_defaultDependency 	{  Samplerate="16000",  default="80"}
		  @h2xmle_defaultDependency 	{  Samplerate="32000",  default="160"}
		  @h2xmle_defaultDependency 	{  Samplerate="48000",  default="240"}
        @h2xmle_range       {0x00000000..0x00001800}	*/


   uint16_t xover_filter_order;
    /**< @h2xmle_description { Low-pass and HPF order.}
         @h2xmle_default     {1}
         @h2xmle_range       {1..3}
         @h2xmle_rangelist   {"6 dB/oct" = 1;
                              "12 dB/oct" = 2;
                              "18 dB/oct (tablebulletend)" = 3} */

   uint16_t bpass_filter_order;
   /**< @h2xmle_description { Low-pass and HPF order.}
         @h2xmle_default     {2}
         @h2xmle_range       {1..3}

         @h2xmle_rangelist   {"6 dB/oct" = 1;
                              "12 dB/oct" = 2;
                              "18 dB/oct (tablebulletend)" = 3} */

   int16_t drc_delay;
    /**< @h2xmle_description { Delay on the signal path to which to apply DRC gain.}
		   @h2xmle_default 	{ 240}
		   @h2xmle_defaultDependency 	{ Samplerate="8000",  default="40"}
		   @h2xmle_defaultDependency 	{  Samplerate="16000",  default="80"}
		   @h2xmle_defaultDependency 	{  Samplerate="32000",  default="160"}
		   @h2xmle_defaultDependency 	{  Samplerate="48000",  default="240"}
         @h2xmle_range       {0x00000000..0x00000780} */


   uint16_t rms_time_avg_ul16q16;
   /**< @h2xmle_description {Short-term root mean square energy estimation time.}
		   @h2xmle_default 	{ 1484}
		   @h2xmle_defaultDependency 	{  Samplerate="8000",  default="8419"}
		   @h2xmle_defaultDependency 	{  Samplerate="16000",  default="4354"}
		   @h2xmle_defaultDependency 	{  Samplerate="32000",  default="2214"}
		   @h2xmle_defaultDependency 	{ Samplerate="48000",  default="1484"}
         @h2xmle_range       {0..0xFFFF}
         @h2xmle_dataFormat   {Q16} */



   int16_t expdr_threshold_l16q8;
   /**< @h2xmle_description {Downward expansion threshold. Range:-80-cmpsr_threashold_l16q7. Default: -35
                             Values are set using  drcUI_set_threshold() in Q7 }
         @h2xmle_default     {7080}
         @h2xmle_range       {0x00000F27..0x00002D27}
         @h2xmle_dataFormat   {Q7} */


   uint16_t expdr_slope_l16q8;
    /**< @h2xmle_description { Downward expansion ratio.
                               Values are set using drcUI_set_expa_ratio() in Q8 }
         @h2xmle_default     {154}
         @h2xmle_range       {0..65535}
         @h2xmle_dataFormat   {Q8} */


   int16_t cmpsr_threashold_l16q7;
   /**< @h2xmle_description {Downward compression threshold. Range:expdr_threshold_l16q8-0.0. Default: -20.0
                             Values are set using  drcUI_set_threshold() in Q7 }
         @h2xmle_default     {9000}
         @h2xmle_range       {0x00000F27..0x00002D27}
          @h2xmle_dataFormat   {Q7} */


   uint16_t cmpsr_slope_ul16q16;
    /**< @h2xmle_description {Downward compression ratio. Range 1.0 to 30.0. Default: 10.0}
         @h2xmle_default     { 58982 }
         @h2xmle_range       {0x00000000..0x0000F777}
          @h2xmle_dataFormat   {Q16} */

   uint16_t makeup_gain_ul16q12;
    /**< @h2xmle_description {Makeup gain. Range -18.0 to 18.0. Default: 6.0}
         @h2xmle_default     {24576}
         @h2xmle_range       {0x00000203..0x00007F17}
          @h2xmle_dataFormat   {Q12} */


   uint32_t cmpsr_attack_ul32q31;
    /**< @h2xmle_description { Downward compression gain smooth attack time. Range: 0.0 to 500.0, Default:10.0 }
		   @h2xmle_default 	{ 77314964}
		   @h2xmle_defaultDependency 	{ Samplerate="8000",  default="424087652"}
		   @h2xmle_defaultDependency 	{ Samplerate="16000",  default="223694491"}
		   @h2xmle_defaultDependency 	{ Samplerate="32000",  default="114922270"}
		   @h2xmle_defaultDependency 	{ Samplerate="48000",  default="77314964"}
         @h2xmle_range       {0x00000000..0xFFFFFFFF}
         @h2xmle_dataFormat   {Q31}*/

   uint32_t cmpsr_release_ul32q31;
    /**< @h2xmle_description { Downward compression gain smooth release time.Range: 0.0 to 5000.0, Default:100.0}
		   @h2xmle_default	{7859688}
		   @h2xmle_defaultDependency 	{  Samplerate="8000",  default="46728739"}
		   @h2xmle_defaultDependency 	{  Samplerate="16000",  default="23492872"}
		   @h2xmle_defaultDependency 	{  Samplerate="32000",  default="11778739"}
		   @h2xmle_defaultDependency 	{  Samplerate="48000",  default="7859688"}
         @h2xmle_range       {0x00000000..0xFFFFFFFF}
          @h2xmle_dataFormat   {Q31}*/

   uint32_t expdr_attack_ul32q31;
    /**< @h2xmle_description { Downward expansion gain smooth attack time.Range: 0.0 to 500.0, Default:10.0}
         @h2xmle_default     {9820112}

		   @h2xmle_defaultDependency 	{  Samplerate="8000",  default="424087652"}
		   @h2xmle_defaultDependency 	{ Samplerate="16000",  default="223694491"}
		   @h2xmle_defaultDependency 	{  Samplerate="32000",  default="114922270"}
		   @h2xmle_defaultDependency 	{  Samplerate="48000",  default="77314964"}
         @h2xmle_range       {0x00000000..0xFFFFFFFF}
          @h2xmle_dataFormat   {Q31}*/

   uint32_t expdr_release_ul32q31;
    /**< @h2xmle_description { Downward expansion gain smooth release time.Range: 0.0 to 5000.0, Default:100.0}
		   @h2xmle_default 	{7859688}
		   @h2xmle_defaultDependency 	{Samplerate="8000",  default="46728739"}
		   @h2xmle_defaultDependency 	{Samplerate="16000",  default="23492872"}
		   @h2xmle_defaultDependency 	{ Samplerate="32000",  default="11778739"}
		   @h2xmle_defaultDependency 	{ Samplerate="48000",  default="7859688"}
         @h2xmle_range       {0x00000000..0xFFFFFFFF}
         @h2xmle_dataFormat   {Q31}*/

   int16_t limiter_bass_threshold_q12;//capi_v2_pbe_config.cpp, line:105,500;
    /**< @h2xmle_description {Output level of bass content.
                               For value x in db, it had to be converted using
                               f(x) = 10^(-3/20) express in Q12 format}
		   @h2xmle_default 	{ 23190}
		   @h2xmle_defaultDependency 	{ Samplerate="8000",  default="16422"}
		   @h2xmle_defaultDependency 	{Samplerate="16000",  default="16422"}
		   @h2xmle_defaultDependency 	{ Samplerate="32000",  default="16049"}
		   @h2xmle_defaultDependency 	{ Samplerate="48000",  default="16049"}
         @h2xmle_range        {0x00000000..0x00007FFF}
         @h2xmle_dataFormat  {Q12}*/

   int16_t limiter_high_threshold_q12;
    /**< @h2xmle_description {Output level of non-bass content. Default:-3dB}
		   @h2xmle_default	{23190}
		   @h2xmle_defaultDependency 	{Samplerate="8000",  default="16422"}
		   @h2xmle_defaultDependency 	{ Samplerate="16000",  default="16422"}
		   @h2xmle_defaultDependency 	{ Samplerate="32000",  default="16049"}
		   @h2xmle_defaultDependency 	{ Samplerate="48000",  default="16049"}
         @h2xmle_range       {0x00000000..0x00007FFF}
          @h2xmle_dataFormat   {Q12}*/

   int16_t limiter_bass_makeup_gain_q8;
    /**< @h2xmle_description {Extra boost gain for bass content.}
         @h2xmle_default     {32440 }
         @h2xmle_range       {0x00000100..0x00007FFF}
         @h2xmle_dataFormat   {Q8} */

   int16_t limiter_high_makeup_gain_q8;
  /**< @h2xmle_description {Extra boost gain for non-bass content.}
        @h2xmle_default     {32440 }
         @h2xmle_range       {0x00000100..0x00007FFF}
          @h2xmle_dataFormat   {Q8} */

   int16_t limiter_bass_grc_q15;
   /**< @h2xmle_description {Limiter gain recovery constant for bass content.}
         @h2xmle_default     {32440 }
         @h2xmle_range       {0..32767}
          @h2xmle_dataFormat   {Q15} */

   int16_t limiter_high_grc_q15;
    /**< @h2xmle_description {Limiter gain recovery constant for non-bass content.}
         @h2xmle_default     {32440 }
         @h2xmle_range       {0..32767}
          @h2xmle_dataFormat   {Q15} */

   int16_t limiter_delay_q15;
    /**< @h2xmle_description {downward expansion threshold.range: 0.0 to 10.0, default:5.0}
         @h2xmle_default     {163}
         @h2xmle_range       {0x00000020..0x00000666}
         @h2xmle_dataformat   {q15} */


   uint16_t reserved;
    /**< @h2xmle_description {This field must be set to 0}
         @h2xmle_readOnly     {true}  */


   int32_t pbeFiltCoeffsQ30[0];
   /**< @h2xmle_description  {Filter coefficients for PBE}
        @h2xmlx_expandStructs {false}
	@h2xmle_range       {0x80000000..0x7FFFFFFF}
        @h2xmle_variableArraySize  { "((xover_filter_order)/3 + 1)*5*2 + 5*bpass_filter_order + 5 + 5" }

	@h2xmle_defaultList   {0x0035711A,0x0035711A,0x00000000,0xC06AE236,0x00000000,0x3FCA8EE5,0xC035711B,0x00000000,0xC06AE236,0x00000000,0x00358632,0x00000000,0xffca79ce,0x8039A627,0x3FC7314D,0x0035761C,0x00000000,0xFFCA89E4,0x8060CD19,0x3FA18775,0x3FCA7751,0x80D64C61,0x3F5FEECD,0x80D75671,0x3F2B702D,0x1020FF91,0xDFF466DA,0x0FEB4D85,0x806BD6E8,0x3F94DD08} */

}
#include "adsp_end_pack.h"
;

/** @}                   <-- End of the Module -->*/




#define VOICE_PARAM_PBE 0x00010E07

#include "adsp_begin_pack.h"

typedef struct voice_param_pbe_t voice_param_pbe_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_PBE", VOICE_PARAM_PBE}

    @h2xmlp_description  {PBE algorithm.\n
Fixed size for this parameter - \n
Version 0 -- 56 bytes, followed by a variable size IIR filter structure \n\n
The filter sizes are defined by configurable parameters. The maximum size for voice applications is 236
bytes. The filter order is assumed to be 3 for high, low, and band-pass filters. \n} */

struct voice_param_pbe_t {

    int16_t iRealBassMixQ15;
    /**< @h2xmle_description {Mix amount of real bass harmonics and virtual bass harmonics.}
         @h2xmle_default     {19661}
         @h2xmle_range       {0..32767}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "19661"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "19661"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "13107"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "13107"}

         @h2xmle_policy       {Basic} */

    int16_t iBassColorControlQ15;
    /**< @h2xmle_description {Color control of virtual bass.}
         @h2xmle_default     {9830}
         @h2xmle_range      {0..32767}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "9830"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "9830"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "9830"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "9830"}

         @h2xmle_policy       {Basic} */


    uint16_t uiMainChainDelay;
    /**< @h2xmle_description {Sample delay in the non-bass path to compensate an algorithm delay in the bass path.}
         @h2xmle_default     {80}
         @h2xmle_range       {0x00000000..0x00001800}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "40"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "80"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "160"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "240"}

         @h2xmle_policy       {Basic} */

    uint16_t uiXoverFltOrder;
    /**< @h2xmle_description {Low-pass and HPF order.}
         @h2xmle_default     {1}
         @h2xmle_range       {1..3}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "1"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "1"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "1"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "1"}

         @h2xmle_policy       {Basic} */

    uint16_t uiBandpassFltOrder;
    /**< @h2xmle_description {Low-pass and HPF order.}
         @h2xmle_default     {2}
         @h2xmle_range       {1..3}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "2"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "2"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "2"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "2"}

         @h2xmle_policy       {Basic} */

    int16_t iDrcDelay;
    /**< @h2xmle_description {Delay on the signal path to which to apply DRC gain.}
         @h2xmle_default     {80}
         @h2xmle_range       {0x00000000..0x00000780}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "40"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "80"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "160"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "240"}

         @h2xmle_policy       {Basic} */

    uint16_t uiRmsTavUL16Q16;
    /**< @h2xmle_description {Short-term root mean square energy estimation time.}
         @h2xmle_default     {4354}
         @h2xmle_range       {0..32767}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "8419"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "4354"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "2214"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "1484"}

         @h2xmle_policy       {Basic} */

    int16_t iExpThresholdL16Q7;
    /**< @h2xmle_description {Downward expansion threshold. Range:-80-cmpsr_threashold_l16q7. Default: -35. Values are set using  drcUI_set_threshold() in Q7}
         @h2xmle_default     {7079}
         @h2xmle_range       {0x00000F27..0x00002D27}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "7079"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "7079"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "7079"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "7079"}

         @h2xmle_policy       {Basic} */

    uint16_t uiExpSlopeL16Q8;
    /**< @h2xmle_description {Downward expansion ratio.Values are set using drcUI_set_expa_ratio() in Q8}
         @h2xmle_default     {65366}
          @h2xmle_range       {0x00000000..0x0000FFFF}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "65366"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "65366"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "65366"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "65366"}

         @h2xmle_policy       {Basic} */

    int16_t iCompThresholdL16Q7;
    /**< @h2xmle_description {Downward compression threshold. Range:expdr_threshold_l16q8-0.0. Default: -20.0 Values are set using  drcUI_set_threshold() in Q7}
         @h2xmle_default     {8999}
       @h2xmle_range       {0x00000F27..0x00002D27}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "8999"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "8999"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "8999"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "8999"}

         @h2xmle_policy       {Basic} */

    uint16_t uiCompSlopeUL16Q16;
    /**< @h2xmle_description {Downward compression ratio. Range 1.0 to 30.0. Default: 10.0}
         @h2xmle_default     {58982}
         @h2xmle_range       {0x00000000..0x0000F777}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "58982"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "58982"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "58982"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "58982"}

         @h2xmle_policy       {Basic} */

    uint16_t uiMakeupGainUL16Q12;
    /**< @h2xmle_description {Makeup gain. Range -18.0 to 18.0. Default: 6.0}
         @h2xmle_default     {4096}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "4096"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "4096"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "4096"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "4096"}
         @h2xmle_range      {0x00000203..0x00007F17}
         @h2xmle_policy       {Basic} */

    uint32_t ulCompAttackUL32Q31;
    /**< @h2xmle_description {Downward compression gain smooth attack time. Range: 0.0 to 500.0, Default:10.0}
         @h2xmle_default     {223694491}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "424087652"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "223694491"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "114922270"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "77314964"}
         @h2xmle_range       {0x00000000..0xFFFFFFFF}
         @h2xmle_policy       {Basic} */

    uint32_t ulCompReleaseUL32Q31;
    /**< @h2xmle_description {Downward compression gain smooth release time.Range: 0.0 to 5000.0, Default:100.0}
         @h2xmle_default     {23492872}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "46728739"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "23492872"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "11778739"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "7859688"}
          @h2xmle_range       {0..4294967295}
         @h2xmle_policy       {Basic} */

    uint32_t uiExpAttackUL32Q31;
    /**< @h2xmle_description {Downward expansion gain smooth attack time.Range: 0.0 to 500.0, Default:10.0}
         @h2xmle_default     {223694491}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "424087652"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "223694491"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "114922270"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "77314964"}
          @h2xmle_range        {0x00000000..0xFFFFFFFF}
         @h2xmle_policy       {Basic} */

    uint32_t uiExpaReleaseUL32Q31;
    /**< @h2xmle_description {Downward expansion gain smooth release time.Range: 0.0 to 5000.0, Default:100.0}
         @h2xmle_default     {23492872}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "46728739"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "23492872"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "11778739"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "7859688"}
          @h2xmle_range       {0..4294967295}
         @h2xmle_policy       {Basic} */

    int16_t iBassLimThresholdQ12;
    /**< @h2xmle_description {Output level of bass content.For value x in db, it had to be converted using f(x) = 10^(-3/20) express in Q12 format.}
         @h2xmle_default     {16422}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "16422"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "16422"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "16049"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "16049"}
          @h2xmle_range        {0x00000000..0x00007FFF}
         @h2xmle_policy       {Basic} */

    int16_t iHighLimThresholdQ12;
    /**< @h2xmle_description {Output level of non-bass content. Default:-3dB}
         @h2xmle_default     {16422}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "16422"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "16422"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "16049"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "16049"}
         @h2xmle_range        {0x00000000..0x00007FFF}
         @h2xmle_policy       {Basic} */

    int16_t iBassLimMakeUpGainQ8;
    /**< @h2xmle_description {Extra boost gain for bass content.}
         @h2xmle_default     {256}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "256"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "256"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "256"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "256"}
         @h2xmle_range       {0x00000100..0x00007FFF}
         @h2xmle_policy       {Basic} */

    int16_t iHighLimMakeUpGainQ8;
    /**< @h2xmle_description {Extra boost gain for non-bass content.}
         @h2xmle_default     {256}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "256"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "256"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "256"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "256"}
         @h2xmle_range       {0x00000100..0x00007FFF}
         @h2xmle_policy       {Basic} */

    int16_t iLimBassGRCQ15;
    /**< @h2xmle_description {Limiter gain recovery constant for bass content.}
         @h2xmle_default     {32440}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "32440"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "32440"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "32440"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "32440"}
          @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t iLimHighGRCQ15;
    /**< @h2xmle_description {Limiter gain recovery constant for non-bass content.}
         @h2xmle_default     {29491}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "29491"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "29491"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "29491"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "29491"}
          @h2xmle_range       {0..32767}
         @h2xmle_policy       {Basic} */

    int16_t iLimDelayQ15;
    /**< @h2xmle_description {downward expansion threshold.range: 0.0 to 10.0, default:5.0}
         @h2xmle_default     {163}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "163"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "163"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "32"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "32"}
        @h2xmle_range       {0x00000020..0x00000666}
         @h2xmle_policy       {Basic} */

    int16_t Reserved;
    /**< @h2xmle_description {This field must be set to 0}
         @h2xmle_default     {0}
         @h2xmle_defaultDependency   { Samplerate = "8000",  default = "0"}
         @h2xmle_defaultDependency   { Samplerate = "16000",  default = "0"}
         @h2xmle_defaultDependency   { Samplerate = "32000",  default = "0"}
         @h2xmle_defaultDependency   { Samplerate = "48000",  default = "0"}

         @h2xmle_policy       {Basic} */

    int32_t pbeFiltCoeffsQ30[0];
    /**< @h2xmle_description  {Filter coefficients for PBE}
        @h2xmlx_expandStructs {false}
        @h2xmle_variableArraySize  { "((uiXoverFltOrder)/3 + 1)*5*2 + 5*uiBandpassFltOrder + 5 + 5" }

	@h2xmle_defaultList  {0x02E1DA81, 0x02E1DA81,0x00000000,0xC5C3B503,0x00000000,0x3D1E257E,0xC2E1DA82,0x00000000,0xC5C3B503,0x00000000,0x02F31D7B,0x00000000,0xFD0CE285,0x83CAFB35,0x3CDF71D2,0x02E39E9E,0x00000000,0xFD1C6162,0x86EC3357,0x3AE42ACC,0x3CE44DCF,0x8C3499A5,0x376B3DA5,0x8CF99121,0x351482F1,0x10E69663,0xE1A43CF4,0x0E0150E6,0x86532C2B,0x3A38F812}

        @h2xmle_defaultDependency   { Samplerate = "8000",  defaultList="{0x02E1DA81, 0x02E1DA81,0x00000000,0xC5C3B503,0x00000000,0x3D1E257E,0xC2E1DA82,0x00000000,0xC5C3B503,0x00000000,0x02F31D7B,0x00000000,0xFD0CE285,0x83CAFB35,0x3CDF71D2,0x02E39E9E,0x00000000,0xFD1C6162,0x86EC3357,0x3AE42ACC,0x3CE44DCF,0x8C3499A5,0x376B3DA5,0x8CF99121,0x351482F1,0x10E69663,0xE1A43CF4,0x0E0150E6,0x86532C2B,0x3A38F812}"}

	@h2xmle_defaultDependency   { Samplerate = "16000",  defaultList="{0x009F4A73,0x009F4A73,0x00000000,0xC13E94E8,0x00000000,0x3F60B58C,0xC09F4A74,0x00000000,0xC13E94E8,0x00000000,0x00A00885,0x00000000,0xFF5FF77B,0x80B16408,0x3F56284B,0x009F737A,0x00000000,0xFF608C86,0x812E9F31,0x3EE63A9B,0x3F5E6D36,0x8284BFF5,0x3E23044D,0x828DFB2A,0x3D8AACB9,0x103C914A,0xE02D31F1,0x0F9C85EB,0x81459DC9,0x3EC0AB5D}"}

	@h2xmle_defaultDependency   { Samplerate = "32000",  defaultList="{0x00500855,0x00500855,0x00000000,0xC0A010AC,0x00000000,0x3FAFF7AA,0xC0500856,0x00000000,0xC0A010AC,0x00000000,0x005037CE,0x00000000,0xFFAFC832,0x805707B0,0x3FAADCA9,0x00501357,0x00000000,0xFFAFECA9,0x8092BC8E,0x3F727FBD,0x3FAF92FB,0x8141B0E4,0x3F104C6F,0x81440590,0x3EC23416,0x1027D35B,0xE0025701,0x0FD769D9,0x80A205B2,0x3F5F8E83}"}

	@h2xmle_defaultDependency   { Samplerate = "48000",  defaultList="{0x0035711A,0x0035711A,0x00000000,0xC06AE236,0x00000000,0x3FCA8EE5,0xC035711B,0x00000000,0xC06AE236,0x00000000,0x00358632,0x00000000,0xFFCA79CE,0x8039A627,0x3FC7314D,0x0035761C,0x00000000,0xFFCA89E4,0x8060CD19,0x3FA18775,0x3FCA7751,0x80D64C61,0x3F5FEECD,0x80D75671,0x3F2B702D,0x1020FF91,0xDFF466DA,0x0FEB4D85,0x806BD6E8,0x3F94DD08}"}

        @h2xmle_policy       {Basic}*/

}
#include "adsp_end_pack.h"
;




#define VOICE_MODULE_PBE  0x00010EE5
/**
    @h2xmlm_module       {"VOICE_MODULE_PBE",
                          VOICE_MODULE_PBE}
    @h2xmlm_displayName  {"RX VOICE PBE"}
	@h2xmlm_description {PBE algorithm in the voice processor on the Rx path.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_PBE
\n}
    @h2xmlm_toolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

@h2xml_Select					{voice_param_mod_enable_t}
   @h2xmlm_InsertParameter

@h2xml_Select					{voice_param_pbe_t}
   @h2xmlm_InsertParameter



@}                   <-- End of the Module -->*/

#endif
