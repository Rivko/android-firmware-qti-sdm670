#ifndef MBDRC_PARAM_CALIB_H
#define MBDRC_PARAM_CALIB_H
/*==============================================================================
  @file mbdrc_param_calib.h
  @brief This file contains MBDRC parameters
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/21/17   pag     Added h2xml comments.
 ========================================================================== */

/** @h2xml_title1           {Multiband Dynamic Range Control Parameters}
    @h2xml_title_agile_rev  {Multiband Dynamic Range Control Parameters}
    @h2xml_title_date       {December 14, 2016} */


#include "mmdefs.h"
#include "audio_common_enable_calib.h"

/*  ID of the MBDRC Enable parameter used by AUDPROC_MODULE_ID_MBDRC.*/
#define AUDPROC_PARAM_ID_MBDRC_ENABLE                              0x00010C07

/*  Structure for the enable parameter for an MBDRC module. */
typedef struct audproc_mbdrc_enable_t audproc_mbdrc_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MBDRC_ENABLE", AUDPROC_PARAM_ID_MBDRC_ENABLE}
    @h2xmlp_description {Used to configure a device}
    @h2xmlp_deprecated              {true} */

#include "adsp_begin_pack.h"

/*  Payload of the AUDPROC_PARAM_ID_MBDRC_ENABLE parameter used by the
 MBDRC module.
 */
struct audproc_mbdrc_enable_t
{
   uint32_t enable_flag;
   /**< @h2xmle_description  {Specifies whether MBDRC is enabled.}
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}
        @h2xmle_default      {0}  */
}
#include "adsp_end_pack.h"
;


/* DRC configuration structure for each sub-band of an MBDRC module. */


/** @h2xmlp_subStruct */

typedef struct audproc_subband_drc_config_params_t
{
   int16_t drc_stereo_linked_flag;
   /**< @h2xmle_description {Specifies whether all stereo channels have the same applied dynamics
        or if they process their dynamics independently.}
        @h2xmle_rangeList   {"Not linked,channels process the dynamics independently" = 0;
                             "Linked,channels have the same applied dynamics" = 1} */

   int16_t drc_mode;
   /**< @h2xmle_description {Specifies whether DRC mode is bypassed for subbands.}
        @h2xmle_rangeList   {"Disable"=0; "Enable"=1} */

   int16_t drc_down_sample_level;
   /**< @h2xmle_description {DRC down sample level.}
        @h2xmle_default     {1}
		@h2xmle_range    {1..32767}*/

   int16_t drc_delay;
   /**< @h2xmle_description {DRC delay in samples.}
        @h2xmle_range       {0..1200} */

   uint16_t drc_rms_time_avg_const;
   /**< @h2xmle_description {RMS signal energy time-averaging constant.}
        @h2xmle_range       {0..65535} */

   uint16_t drc_makeup_gain;
   /**< @h2xmle_description {DRC makeup gain in decibels.}
        @h2xmle_default     {258}
        @h2xmle_range       {258..64917} */

   /* Down expander settings */

   int16_t down_expdr_threshold;
   /**< @h2xmle_description {Down expander threshold.}
                             Its value must be: (<down_cmpsr_threshold)}
        @h2xmle_default     {1320}
        @h2xmle_range       {-32768..32767}
        @h2xmle_dataFormat  {Q7}  */

   int16_t down_expdr_slope;
   /**< @h2xmle_description {Down expander slope.}
        @h2xmle_default     {-32768}
        @h2xmle_range       {-32768..0}
        @h2xmle_dataFormat  {Q8}  */

   uint32_t down_expdr_attack;
   /**< @h2xmle_description {Down expander attack constant.}
        @h2xmle_default     {196844}
        @h2xmle_range       {196844..2147483648}
        @h2xmle_dataFormat  {Q31}  */

   uint32_t down_expdr_release;
   /**< @h2xmle_description {Down expander release constant.}
        @h2xmle_default     {19685}
        @h2xmle_range       {19685..2147483648}
        @h2xmle_dataFormat  {Q31}  */

   uint16_t down_expdr_hysteresis;
   /**< @h2xmle_description {Down expander hysteresis constant.}
        @h2xmle_default     {1}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q14}  */

   uint16_t reserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_default     {0} */

   int32_t down_expdr_min_gain_db;
   /**< @h2xmle_description {Down expander minimum gain.}
        @h2xmle_range       {-805306368..0}
        @h2xmle_dataFormat  {Q23}  */

   /* Up compressor settings */

   int16_t up_cmpsr_threshold;
   /**< @h2xmle_description {Up compressor threshold.
                             Its value must be:
                             (&gt;down_expdr_threshold) &amp;&amp; (&lt;down_cmpsr_threshold)}
        @h2xmle_default     {1320}
        @h2xmle_range       {-32768..32767}
        @h2xmle_dataFormat  {Q7}  */

   uint16_t up_cmpsr_slope;
   /**< @h2xmle_description {Up compressor slope.}
        @h2xmle_range       {0..64881}
        @h2xmle_dataFormat  {Q16}  */

   uint32_t up_cmpsr_attack;
   /**< @h2xmle_description {Up compressor attack constant.}
        @h2xmle_default     {196844}
        @h2xmle_range       {196844..2147483648}
        @h2xmle_dataFormat  {Q31}  */

   uint32_t up_cmpsr_release;
   /**< @h2xmle_description {Up compressor release constant.}
        @h2xmle_default     {19685}
        @h2xmle_range       {19685..2147483648}
        @h2xmle_dataFormat  {Q31}  */

   uint16_t up_cmpsr_hysteresis;
   /**< @h2xmle_description {Up compressor hysteresis constant.}
        @h2xmle_default     {1}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q14}  */

   /* Down compressor settings */

   int16_t down_cmpsr_threshold;
   /**< @h2xmle_description {Down compressor threshold.
                             Its value must be: (&gt;up_cmpsr_threshold)}
        @h2xmle_default     {1320}
        @h2xmle_range       {1320..11560}
        @h2xmle_dataFormat  {Q7}  */

   uint16_t down_cmpsr_slope;
   /**< @h2xmle_description {Down compressor slope.}
        @h2xmle_range       {0..64881}
        @h2xmle_dataFormat  {Q16}  */

   uint16_t reserved1;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_default     {0} */

   uint32_t down_cmpsr_attack;
   /**< @h2xmle_description {Down compressor attack constant.}
        @h2xmle_default     {196844}
        @h2xmle_range       {196844..2147483648}
        @h2xmle_dataFormat  {Q31}  */

   uint32_t down_cmpsr_release;
   /**< @h2xmle_description {Down compressor release constant.}
        @h2xmle_default     {19685}
        @h2xmle_range       {19685..2147483648}
        @h2xmle_dataFormat  {Q31}  */

   uint16_t down_cmpsr_hysteresis;
   /**< @h2xmle_description {Down compressor hysteresis constant.}
        @h2xmle_default     {1}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q14}  */

   uint16_t reserved2;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_default     {0} */

}audproc_subband_drc_config_params_t ;



/*  ID of the MBDRC Configuration parameter used by AUDPROC_MODULE_ID_MBDRC.*/
#define AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS                         0x00010C08

/* Structure for the configuration parameters for an MBDRC module. */
typedef struct audproc_mbdrc_config_params_t audproc_mbdrc_config_params_t;
/** @h2xmlp_parameter 		{"AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS",AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS}
    @h2xmlx_expandStructs	{false}
    @h2xmlm_deprecated              {true} */

#include "adsp_begin_pack.h"

struct audproc_mbdrc_config_params_t
{
   uint16_t num_bands;
   /**< @h2xmle_description  {Number of bands.}
        @h2xmle_default      {1}
        @h2xmle_range        {1..5}
         @h2xmle_readOnly     {true}	*/

   int16_t limiter_threshold;
   /**< @h2xmle_description  {Threshold in decibels for the limiter output.
                              Range: -72 to 18dB, Default:0.22dB }
        @h2xmle_range        {0..32767}
        @h2xmle_default      {4002}
        @h2xmle_dataFormat   {Q3.12} */

   int16_t limiter_makeup_gain;
   /**< @h2xmle_description  {Makeup gain in decibels for the limiter output.
                              Range: -42 to 42 dB, Default:0 dB}
        @h2xmle_range        {-9360..9360}
        @h2xmle_default      {256}
        @h2xmle_dataFormat   {Q7.8}  */

   int16_t limiter_gc;
   /**< @h2xmle_description  {Limiter gain recovery coefficient.
                              Range: 0.5 to 0.99 , Default:0.99}
        @h2xmle_range        {16384..32767}
        @h2xmle_default      {32440}
        @h2xmle_dataFormat   {Q15}  */

   int16_t limiter_delay;
   /**< @h2xmle_description  {Limiter delay in seconds.
                              Range: 0 to 10 seconds. Default 0.008 seconds}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {262}
        @h2xmle_dataFormat   {Q15}  */

   int16_t limiter_max_wait;
   /**< @h2xmle_description  {Maximum limiter waiting time in seconds.}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {262}
        @h2xmle_dataFormat   {Q15}  */

   audproc_subband_drc_config_params_t subband_drc[0];
   /**< @h2xmle_description  { Following this structure is the sub-band payload,
                               This sub-band structure must be repeated for each band.
                               After this DRC structure is configured for valid bands, the next MBDRC
                               setparams expects the sequence of sub-band MBDRC filter coefficients (the
                               length depends on the number of bands) plus the mute flag for that band
                               plus uint16 padding.\n\n
			       The sequence is as follows:\n
                               - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding \n
                               - 2 bands = 97 FIR coefficients + 2 mute flags + uint16 padding \n
                               - 3 bands = 97+33 FIR coefficients + 3 mute flags + uint16 padding \n
                               - 4 bands = 97+33+33 FIR coefficients + 4 mute flags + uint16 padding \n
                               - 5 bands = 97+33+33+33 FIR coefficients + 5 mute flags + uint16 padding \n\n

                               For improved filterbank, the sequence is as follows:\n
                               - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding \n
                               - 2 bands = 141 FIR coefficients + 2 mute flags + uint16 padding \n
                               - 3 bands = 141+81 FIR coefficients + 3 mute flags + uint16 padding \n
                               - 4 bands = 141+81+61 FIR coefficients + 4 mute flags + uint16 padding \n
                               - 5 bands = 141+81+61+61 FIR coefficients + 5 mute flags + uint16 padding \n\n }
        @h2xmle_variableArraySize {num_bands}
        */

   int16_t firFilter[0];
   /**< @h2xmle_description  { fir filter }


        @h2xmle_default      {0}
        @h2xmle_range        {-32768..32767}
        @h2xmle_variableArraySize  { "141*((num_bands + 3)/5) + 81*((num_bands+2)/5) + 61*((num_bands+1)/5) + 61*(num_bands/5)" } */

   int16_t fMuteFlag[0];
   /**< @h2xmle_description  { fMuteFlag}
        @h2xmle_default      {0}
        @h2xmle_range        {0..1}
        @h2xmle_variableArraySize {num_bands} */

   uint16_t padding[0];
   /**< @h2xmle_description  {Padding to align the structure to 4byte boundary}
        @h2xmle_default      {0} */



}
#include "adsp_end_pack.h"
;

/*  ID of the Configuration parameter used by AUDPROC_MODULE_ID_MBDRCV2 for the
    improved filter structure of the MBDRC v2 pre/postprocessing block.

    The update to this configuration structure from the original MBDRC is the
    number of filter coefficients in the filter structure. The sequence
    is as follows:

    - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding
    - 2 bands = 141 FIR coefficients + 2 mute flags + uint16 padding
    - 3 bands = 141+81 FIR coefficients + 3 mute flags + uint16 padding
    - 4 bands = 141+81+61 FIR coefficients + 4 mute flags + uint16 padding
    - 5 bands = 141+81+61+61 FIR coefficients + 5 mute flags + uint16 padding*/
#define AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2  0x0001070C
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2", AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2}
    @h2xmlp_description {Used to configure a device}
    @h2xmlx_expandStructs	{false}
    @h2xmlp_deprecated              {true}  */


/*  ID of the MBDRC Configuration parameter used by AUDPROC_MODULE_ID_MBDRCV3.

    The filter coefficient and mute flag are of type int16:
    - FIR coefficient = int16 firFilter
    - Mute flag = int16 fMuteFlag

    The sequence is as follows:
    - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding
    - 2 bands = 97 FIR coefficients + 2 mute flags + uint16 padding
    - 3 bands = 97+33 FIR coefficients + 3 mute flags + uint16 padding
    - 4 bands = 97+33+33 FIR coefficients + 4 mute flags + uint16 padding
    - 5 bands = 97+33+33+33 FIR coefficients + 5 mute flags + uint16 padding

    For improved filterbank, the sequence is as follows:
    - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding
    - 2 bands = 141 FIR coefficients + 2 mute flags + uint16 padding
    - 3 bands = 141+81 FIR coefficients + 3 mute flags + uint16 padding
    - 4 bands = 141+81+61 FIR coefficients + 4 mute flags + uint16 padding
    - 5 bands = 141+81+61+61 FIR coefficients + 5 mute flags + uint16 padding


    Sub-band DRC configuration parameters (audproc_subband_drc_config_params_t)
    To obtain legacy ADRC from MBDRC, use the calibration tool to:
    - Enable MBDRC (EnableFlag = TRUE)
    - Set number of bands to 1 (uiNumBands = 1)
    - Enable the first MBDRC band (DrcMode[0] = DRC_ENABLED = 1)
    - Clear the first band mute flag (MuteFlag[0] = 0)
    - Set the first band makeup gain to unity (compMakeUpGain[0] = 0x2000)
    - Use the legacy ADRC parameters to calibrate the rest of the MBDRC
      parameters.
*/
#define AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_V3               0x0001090C

/*  Structure for the configuration parameters for an MBDRC_V3 module. */
typedef struct audproc_mbdrc_config_params_v3_t audproc_mbdrc_config_params_v3_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_V3", AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_V3}
    @h2xmlp_description {Used to configure a device}
     @h2xmlx_expandStructs	{false}   */

#include "adsp_begin_pack.h"


struct audproc_mbdrc_config_params_v3_t
{
   uint16_t num_bands;
   /**< @h2xmle_description  {Number of bands.}
        @h2xmle_default      {1}
        @h2xmle_range        {1..5}
         @h2xmle_readOnly     {true}	*/

   int16_t limiter_threshold;
   /**< @h2xmle_description  {Threshold in decibels for the limiter output.
                              Range: -72 to 18dB, Default:0.22dB }
        @h2xmle_range        {0..32767}
        @h2xmle_default      {4002}
        @h2xmle_dataFormat   {Q3.12}  */

   int16_t limiter_makeup_gain;
   /**< @h2xmle_description  {Makeup gain in decibels for the limiter output.
                              Range: -42 to 42 dB, Default:0 dB}
        @h2xmle_range        {-9360..9360}
        @h2xmle_default      {256}
        @h2xmle_dataFormat   {Q7.8}  */

   int16_t limiter_gc;
   /**< @h2xmle_description  {Limiter gain recovery coefficient.
                              Range:0.5 to 0.99, Default:0.99}
        @h2xmle_range        {16384..32767}
        @h2xmle_default      {32440}
        @h2xmle_dataFormat   {Q15}  */

   int16_t limiter_delay;
   /**< @h2xmle_description  {Limiter delay in seconds.
                              Range:0 to 10, Default:0.008 seconds}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {262}
        @h2xmle_dataFormat   {Q15}  */

   int16_t limiter_max_wait;
   /**< @h2xmle_description  {Maximum limiter waiting time in seconds.
                              Range:0 to 10, Default:0.008 seconds}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {262}
        @h2xmle_dataFormat   {Q15}  */

   uint16_t fir_filter_length;
   /**< @h2xmle_description  {MBDRC_V3 FIR split filter length.}
        @h2xmle_rangeList    {"1 band" =   0;
                              "2 bands : Fs&lt;32Khz "=  97;
                              "3 bands : Fs&lt;32Khz "= 130;
                              "4 bands : Fs&lt;32Khz "= 163;
                              "5 bands : Fs&lt;32Khz "= 196;
                              "2 bands : Fs&gt;=32Khz"= 141;
                              "3 bands : Fs&gt;=32Khz"= 222;
                              "4 bands : Fs&gt;=32Khz"= 283;
                              "5 bands : Fs&gt;=32Khz"= 344
                              }
        @h2xmle_readOnly     {true} */

   uint16_t reserved;
   /**< @h2xmle_description  {Must be set to 0.}
        @h2xmle_default      {0} */

   audproc_subband_drc_config_params_t subband_drc[0];
   /**< @h2xmle_description  { Following this structure is the sub-band payload,
                               This sub-band structure must be repeated for each band.
                               After this DRC structure is configured for valid bands, the next MBDRC
                               setparams expects the sequence of sub-band MBDRC filter coefficients (the
                               length depends on the number of bands) plus the mute flag for that band
                               plus uint16 padding. \n\n
                               The sequence is as follows:\n
                               - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding \n
                               - 2 bands = 97 FIR coefficients + 2 mute flags + uint16 padding \n
                               - 3 bands = 97+33 FIR coefficients + 3 mute flags + uint16 padding \n
                               - 4 bands = 97+33+33 FIR coefficients + 4 mute flags + uint16 padding \n
                               - 5 bands = 97+33+33+33 FIR coefficients + 5 mute flags + uint16 padding \n\n

                               For improved filterbank, the sequence is as follows:\n
                               - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding \n
                               - 2 bands = 141 FIR coefficients + 2 mute flags + uint16 padding \n
                               - 3 bands = 141+81 FIR coefficients + 3 mute flags + uint16 padding \n
                               - 4 bands = 141+81+61 FIR coefficients + 4 mute flags + uint16 padding \n
                               - 5 bands = 141+81+61+61 FIR coefficients + 5 mute flags + uint16 padding \n\n}
        @h2xmle_variableArraySize {num_bands}
        */

   int16_t firFilter[0];
   /**< @h2xmle_description  { fir filter }


        @h2xmle_default      {0}
        @h2xmle_range        {-32768..32767}
        @h2xmle_variableArraySize  { "fir_filter_length" } */

   int16_t fMuteFlag[0];
   /**< @h2xmle_description  { fMuteFlag}
        @h2xmle_default      {0}
        @h2xmle_range        {0..1}
        @h2xmle_variableArraySize {num_bands} */

   uint16_t padding[0];
   /**< @h2xmle_description  {Padding to align the structure to 4byte boundary}
        @h2xmle_default      {0} */

}
#include "adsp_end_pack.h"
;


/** ID of the MBDRC Configuration parameter used by #AUDPROC_MODULE_ID_MBDRCV3.*/
#define AUDPROC_PARAM_ID_MBDRC_FILTER_XOVER_FREQS             0x0001090D

/** Maximum number of bands. */
#define MBDRC_MAX_BANDS                                            5

/* Structure for the MBDRC filter cross over frequencies parameter
 * for an MBDRC_V3 module. */
typedef struct audproc_mbdrc_filter_xover_freqs_t audproc_mbdrc_filter_xover_freqs_t;

/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_MBDRC_FILTER_XOVER_FREQS", AUDPROC_PARAM_ID_MBDRC_FILTER_XOVER_FREQS}
    @h2xmlp_description {Used to configure a device}  */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_MBDRC_FILTER_XOVER_FREQS parameter used
 by the MBDRC_V3 module.
 */
struct audproc_mbdrc_filter_xover_freqs_t
{
   uint32_t filter_xover_freqs[MBDRC_MAX_BANDS - 1];
   /**< @h2xmle_description  {Array of filter crossover frequencies. Based on Band number n,
                              filter_xover_freqs[MBDRC_MAX_BANDS-1] has (n-1)
                              crossover frequecies and the rest(if any) are ignored.
                              } */
}
#include "adsp_end_pack.h"
;

#endif
