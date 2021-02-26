#ifndef AIG_CALIB_H
#define AIG_CALIB_H
/*==============================================================================
  @file aig_calib.h
  @brief This file contains AIG API
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

/** @h2xml_title1           {Adaptive Input Gain}
    @h2xml_title_agile_rev  {AIG}
    @h2xml_title_date       {April 19, 2017} */

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
#define AUDPROC_MODULE_ID_AIG                         0x00010716
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_AIG",
                          AUDPROC_MODULE_ID_AIG}
    @h2xmlm_displayName  {"AUDIO TX AIG"}
    @h2xmlm_toolPolicy   {Calibration; RTC}
    @h2xmlm_description  {Audio Adaptive Input Gain Module}
    @{                   <-- Start of the Module -->
*/
#define AUDPROC_PARAM_ID_AIG_ENABLE                   0x00010717

/* Structure for enabling the configuration parameter for
 AUDIO AIG enable. */
typedef struct Audio_AigEnable_t Audio_AigEnable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_AIG_ENABLE", AUDPROC_PARAM_ID_AIG_ENABLE}
    @h2xmlp_description {Used to enable/disable AIG module} */
#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_AIG_ENABLE parameter in the AIG
 module.
 */
struct Audio_AigEnable_t
{

   uint32_t enable;
   /**< @h2xmle_description  {Enable/Disable AIG module}
        @h2xmle_rangeList    {"Disable" = 0;
                              "Enable"  = 1}
        @h2xmle_default      {0} */
}
#include "adsp_end_pack.h"
;
#define AUDPROC_PARAM_ID_AIG_CONFIG                   0x00010718

/*==============================================================================
   Type Definitions
==============================================================================*/

/* Structure for Audio Aig configuration parameters */
typedef struct Audio_AigParam_t Audio_AigParam_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_AIG_CONFIG", AUDPROC_PARAM_ID_AIG_CONFIG}
    @h2xmlp_description {Used to set parameters for AIG module} */

#include "adsp_begin_pack.h"

/* Payload of the AUDIO_PARAM_AIG parameters in the AIG module.
 */
struct Audio_AigParam_t
{

   uint16_t mode;
    /**< @h2xmle_description  {Select between Static/Adaptive input gain modes}
         @h2xmle_rangeList    {"Static Input Gain (SIG)" = 0;
                               "Adaptive Input Gain (AIG)" = 1}
	@h2xmle_range       {0..2}
         @h2xmle_default      {0} */

   int16_t staticGainL16Q12;
    /**< @h2xmle_description  {Static input gain when the mode field is set to 1}
          @h2xmle_default     {0x1000}
         @h2mle_dataFormat   {Q12}
	 @h2xmle_range       {0..32767}
    */

   int16_t initialGainDBL16Q7;
    /**< @h2xmle_description  {Initial value from which the adaptive gain update starts}
          @h2xmle_default     {0x0}
         @h2xmle_dataFormat   {Q7}
	  @h2xmle_range       {-11520..11520}
    */
   int16_t idealRMSDBL16Q7;
    /**< @h2xmle_description  {Average RMS level that the AIG tries to achieve}
          @h2xmle_default     {0x19A8}
	  @h2xmle_range       {0..32767}
    */

   int32_t noiseGateL32;
    /**< @h2xmle_description  {Threshold below which a signal is considered as noise and AIG is not
                           applied}
         @h2xmle_default     {0x0432}
	 @h2xmle_range       {1..1073741824}
    */

   int32_t minGainL32Q15;
    /**< @h2xmle_description  {Minimum gain that can be provided by the AIG}
          @h2xmle_default     {0x8000}
         @h2xmle_dataFormat   {Q15}
	 @h2xmle_range       {1..1036215144}
    */

   int32_t maxGainL32Q15;
    /**< @h2xmle_description  {Maximum gain that can be provided by the AIG}
          @h2xmle_default     {0x8000}
         @h2xmle_dataFormat   {Q15}
	 @h2xmle_range       {1..1036215144}
    */

   uint32_t gainAtRtUL32Q31;
    /**< @h2xmle_description  {Attack/release time for the AIG update}
          @h2xmle_default     {0x240665}
         @h2xmle_dataFormat   {Q31}
	 @h2xmle_range       {0..2147483648}
    */

   uint32_t longGainAtRtUL32Q31;
    /**< @h2xmle_description  {Long attack/release time while updating gain for noise/silence}
          @h2xmle_default     {0x24F}
         @h2xmle_dataFormat   {Q31}
	  @h2xmle_range       {0..2147483648}
 */

uint32_t rmsTavUL32Q32;
    /**< @h2xmle_description  {RMS smoothing time constant used for long-term RMS estimate}
          @h2xmle_default     {0x1CD5E}
         @h2xmle_dataFormat   {Q32}
	  @h2xmle_range       {0..4294967295}
    */

   uint32_t gainUpdateStartTimMsUL32Q0;
    /**< @h2xmle_description  {Waiting time before which the AIG applies the adaptive gain update}
          @h2xmle_default     {0x0}
         @h2xmle_dataFormat   {Q0}
	  @h2xmle_range       {0..4294967295}
    */

}
#include "adsp_end_pack.h"
;
/** @}                   <-- End of the Module -->*/




#define VOICE_PARAM_AIG 0x00010E17

#include "adsp_begin_pack.h"

typedef struct voice_param_aig_t voice_param_aig_t;
/** @h2xmlp_parameter    {"VOICE_PARAM_AIG", VOICE_PARAM_AIG}
    @h2xmlp_description {Adaptive Input Gain (AIG) algorithm on the Tx and Rx paths.\n
	Fixed size for this parameter \n
	Version 0 – 36 bytes\n} */

struct voice_param_aig_t {

        int16_t aigMode;
    /**< @h2xmle_description {Mode control to apply SIG or AIG. Enable also has to be set to enable AIG}
         @h2xmle_default     {1}
         @h2xmle_range       {0..2}





         @h2xmle_policy       {Basic} */

    int16_t staticGainL16Q12;
    /**< @h2xmle_description {Static input gain when aigMode is set to 0: Q3.12, 0 dB = 0x1000, staticGainL16Q12 = dec2hex(round(10^(staticGaindB /20) * 4096))}

         @h2xmle_range       {0..32767}


         @h2xmle_policy       {Basic} */

    int16_t initialGainDBL16Q7;
    /**< @h2xmle_description {Initial value that the adaptive gain update starts from: dB Q7, 0 dB = 0x0000, initialGainDBL16Q7 = dec2hex(round(initialGaindB *128))}
         @h2xmle_default     {0}
         @h2xmle_range       {-11520..11520}

         @h2xmle_policy       {Basic} */

    int16_t idealRMSDBL16Q7;
    /**< @h2xmle_description {Average RMS level that AIG attempts to achieve: Q8.7, -24 dB = 0x2128, idealRMSDBL16Q7 = dec2hex(round(( idealRMSdB + 300 * log10(2)) *128))}

         @h2xmle_range       {0..32767}



         @h2xmle_policy       {Basic} */

    int32_t noiseGateL32;
    /**< @h2xmle_description {Threshold below which signal is considered as noise and AIG targets for an idealRmsDB of 0 dB: -60 dB = 0x00000432, noiseGateL32 = dec2hex(round(10^( (noiseGatedB + 300*log10(2)) /10)))}

         @h2xmle_range       {1..1073741824}


         @h2xmle_policy       {Basic} */

    int32_t minGainL32Q15;
    /**< @h2xmle_description {Minimum gain that can be provided by AIG: Q16.15, 0 dB = 0x00008000, minGainL32Q15 = dec2hex(round(10^(minGaindB/20)* 32768))}

         @h2xmle_range       {1..1036215144}

         @h2xmle_policy       {Basic} */

    int32_t maxGainL32Q15;
    /**< @h2xmle_description {Maximum gain that can be provided by AIG: Q16.15, 0 dB = 0x00008000, maxGainL32Q15 = dec2hex(round(10^(maxGaindB/20)* 32768))}
         @h2xmle_default     {46286}
         @h2xmle_range       {1..1036215144}



         @h2xmle_policy       {Basic} */

    uint32_t gainAtRtUL32Q31;
    /**< @h2xmle_description {Attack/release time for AIG update: Q1.31, 5000 ms = 0x0001CD5C at 8 kHz, 10000 ms = 0x0000E6AF at 8kHz, gainAtRtUL32Q31 = dec2hex(round((1-exp(-2200 /fs/gainAtRtms))*2^31))}
         @h2xmle_default     {0x240665}
         @h2xmle_range       {0..2147483648}



         @h2xmle_policy       {Basic} */

    uint32_t longGainAtRtUL32Q31;
    /**< @h2xmle_description {Long attack/release time while updating gain for noise/silence: Q1.31, 100000 ms = 0x00001712 at 8 kHz, longGainAtRtUL32Q31 = dec2hex(round((1-exp(-2200 /fs/longGainAtRtms)) * 2^31))}
         @h2xmle_default     {0x049D}
         @h2xmle_range       {0..2147483648}



         @h2xmle_policy       {Basic} */

    uint32_t rmsTavUL32Q32;
    /**< @h2xmle_description {RMS smoothing time constant used for long-term RMS estimate: Q0.32, 5000ms = 0x0039AB9 at 8 kHz, rmsTavUL16Q16 = dec2hex(round((1-exp(-2200/fs/rmsTavms)) *2^32))}
         @h2xmle_default     {0x01CD5E}
         @h2xmle_range       {0..4294967295}



         @h2xmle_policy       {Basic} */

    uint32_t gainUpdateStartTimMsUL32Q0;
    /**< @h2xmle_description {The waiting time before which AIG starts to apply adaptive gain update: Q32.0, 1000 ms = 0x00003E8, gainUpdateStartTimeMsUL32Q0 = round(gainUpdateStartTimems)}

         @h2xmle_range       {0..30000}



         @h2xmle_policy       {Basic} */

}
#include "adsp_end_pack.h"
;



#define VOICE_MODULE_RX_AIG  0x00010EFF
/**
    @h2xmlm_module       {"VOICE_MODULE_RX_AIG",
                          VOICE_MODULE_RX_AIG}
    @h2xmlm_displayName  {"RX VOICE AIG"}
	@h2xmlm_description {Adaptive input gain control algorithm in the voice preprocessor voice preprocessor on the Rx path.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_AIG\n}

    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_aig_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_aig_t::aigMode}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "1"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1"}

    @h2xml_Select					{voice_param_aig_t::staticGainL16Q12}

    @h2xmle_default                 {16384}

    @h2xml_Select					{voice_param_aig_t::idealRMSDBL16Q7}

    @h2xmle_default                 {8488}

    @h2xml_Select					{voice_param_aig_t::noiseGateL32}

    @h2xmle_default                 {1074}

    @h2xml_Select					{voice_param_aig_t::minGainL32Q15}

    @h2xmle_default                 {32768}


    @h2xml_Select					{voice_param_aig_t::maxGainL32Q15}

    @h2xmle_default                 {65381}

    @h2xml_Select					{voice_param_aig_t::gainAtRtUL32Q31}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0x120477"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0x120477"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0x9028D"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0x601C5"}

    @h2xml_Select					{voice_param_aig_t::longGainAtRtUL32Q31}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0xE6AF"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0xE6AF"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0x7358"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0x4CE5"}

    @h2xml_Select					{voice_param_aig_t::rmsTavUL32Q32}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0x039AB9"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0x039AB9"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0x1CD5E"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0x13394"}

    @h2xml_Select					{voice_param_aig_t::gainUpdateStartTimMsUL32Q0}

    @h2xmle_default                  {0}






   @}                   <-- End of the Module -->*/



#define VOICE_MODULE_TX_AIG  0x00010F00
/**
    @h2xmlm_module       {"VOICE_MODULE_TX_AIG",
                          VOICE_MODULE_TX_AIG}
    @h2xmlm_displayName  {"TX VOICE AIG"}
	@h2xmlm_description {Adaptive input gain control algorithm in the voice preprocessor voice preprocessor on the Tx path.\n
This module supports the following parameter IDs:\n
VOICE_PARAM_MOD_ENABLE\n
VOICE_PARAM_AIG\n}

    @h2xmlm_ToolPolicy 				{Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select					{voice_param_mod_enable_t}
    @h2xmlm_InsertParameter

    @h2xml_Select					{voice_param_aig_t}
    @h2xmlm_InsertParameter


     @h2xml_Select					{voice_param_aig_t::aigMode}

     @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "1"}
     @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0"}
     @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "1"}
     @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "1"}

    @h2xml_Select					{voice_param_aig_t::staticGainL16Q12}

    @h2xmle_default                 {4096}
    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "4096"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "16384"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "4096"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "4096"}




    @h2xml_Select					{voice_param_aig_t::idealRMSDBL16Q7}
    @h2xmle_default     {6568}

     @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "6568"}
     @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "8488"}
     @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "6568"}
     @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "6568"}

    @h2xml_Select					{voice_param_aig_t::noiseGateL32}

    @h2xmle_default     {1074}

    @h2xml_Select					{voice_param_aig_t::minGainL32Q15}

    @h2xmle_default                 {32768}


    @h2xml_Select					{voice_param_aig_t::maxGainL32Q15}

    @h2xmle_default     {0x8000}



    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "46286"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "65381"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "46286"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "46286"}

    @h2xml_Select					{voice_param_aig_t::gainAtRtUL32Q31}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0x240665"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0x120477"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0x9028D"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0x601C5"}

    @h2xml_Select					{voice_param_aig_t::longGainAtRtUL32Q31}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0x049D"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0xE6AF"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0x0127"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0x00C5"}

    @h2xml_Select					{voice_param_aig_t::rmsTavUL32Q32}

    @h2xmle_defaultDependency   {   Samplerate = "8000",  default = "0x01CD5E"}
    @h2xmle_defaultDependency   {   Samplerate = "16000",  default = "0x039AB9"}
    @h2xmle_defaultDependency   {   Samplerate = "32000",  default = "0x7358"}
    @h2xmle_defaultDependency   {   Samplerate = "48000",  default = "0x4CE5"}

    @h2xml_Select					{voice_param_aig_t::gainUpdateStartTimMsUL32Q0}

    @h2xmle_default                  {0}



@}                   <-- End of the Module -->*/






#endif
