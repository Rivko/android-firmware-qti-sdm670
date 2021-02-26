#ifndef POPLESS_EQUALIZER_CALIB_H
#define POPLESS_EQUALIZER_CALIB_H
/*==============================================================================
  @file popless_equalizer_calib.h
  @brief This file contains POPLESS_EQUALIZER API
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

/** @h2xml_title1           {Audio Popless Equalizer}
    @h2xml_title_agile_rev  {Audio Popless Equalizer}
    @h2xml_title_date       {February 6, 2017} */

#include  "mmdefs.h"

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition
   Module library is independent of this number, it defined here for static
   loading purpose only */
#define AUDPROC_MODULE_ID_POPLESS_EQUALIZER                    0x000108BA
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_POPLESS_EQUALIZER",
                          AUDPROC_MODULE_ID_POPLESS_EQUALIZER}
    @h2xmlm_displayName  {"AUDIO POPLESS EQUALIZER"}
    @h2xmlm_description  {ID of the Reverberation module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_EQ_ENABLE\n
    - #AUDPROC_PARAM_ID_EQ_CONFIG\n
    - #AUDPROC_PARAM_ID_EQ_NUM_BANDS\n
    - #AUDPROC_PARAM_ID_EQ_BAND_LEVELS\n
    - #AUDPROC_PARAM_ID_EQ_BAND_LEVEL_RANGE\n
    - #AUDPROC_PARAM_ID_EQ_BAND_FREQS\n
    - #AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE\n
    - #AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ\n
    - #AUDPROC_PARAM_ID_EQ_BAND_INDEX\n
    - #AUDPROC_PARAM_ID_EQ_PRESET_ID\n
    - #AUDPROC_PARAM_ID_EQ_NUM_PRESETS\n
    - #AUDPROC_PARAM_ID_EQ_PRESET_NAME\n
    - #AUDPROC_PARAM_ID_ENABLE\n

    All parameter IDs are device independent.\n\n
	( User-customized equalizer preset (with audio effects specified
    individually). )
AUDPROC_EQ_PRESET_USER_CUSTOM - (-1)\n\n

( Equalizer preset for no audio effects. )
AUDPROC_EQ_PRESET_BLANK - 0\n\n

( Equalizer preset for audio effects like a club. @newpage )
AUDPROC_EQ_PRESET_CLUB - 1\n\n

( Equalizer preset for audio effects like a dance. )
AUDPROC_EQ_PRESET_DANCE - 2\n\n

( Qualcomm equalizer preset for full bass audio effects. )
AUDPROC_EQ_PRESET_FULLBASS - 3\n\n

( Equalizer preset for bass and treble audio effects. )
AUDPROC_EQ_PRESET_BASSTREBLE - 4\n\n

( Equalizer preset for full treble audio effects. )
AUDPROC_EQ_PRESET_FULLTREBLE - 5\n\n

( Equalizer preset for audio effects that are suitable for playback through
    laptop/phone speakers.
 )
AUDPROC_EQ_PRESET_LAPTOP - 6\n\n

( Equalizer preset for audio effects that sound like a large hall. )
AUDPROC_EQ_PRESET_LHALL - 7\n\n

( Equalizer preset for audio effects that sound like a live performance. )
AUDPROC_EQ_PRESET_LIVE - 8\n\n

( Equalizer preset for audio effects that sound like a party. )
AUDPROC_EQ_PRESET_PARTY - 9\n\n

( Equalizer preset for pop audio effects. )
AUDPROC_EQ_PRESET_POP - 10\n\n

( Equalizer preset for reggae audio effects. )
AUDPROC_EQ_PRESET_REGGAE - 11\n\n

( Equalizer preset for rock audio effects. )
AUDPROC_EQ_PRESET_ROCK - 12\n\n

( Equalizer preset for Ska audio effects. )
AUDPROC_EQ_PRESET_SKA - 13\n\n

( Equalizer preset for soft pop audio effects. )
AUDPROC_EQ_PRESET_SOFT - 14\n\n

( Equalizer preset for soft rock audio effects. )
AUDPROC_EQ_PRESET_SOFTROCK - 15\n\n

( Equalizer preset for techno audio effects. )
AUDPROC_EQ_PRESET_TECHNO - 16\n\n

( User-customized equalizer preset (with audio effects specified
    individually) (OpenSL).
 )
AUDPROC_EQ_PRESET_USER_CUSTOM_AUDIO_FX - 18\n\n

( Equalizer preset for normal (generic) audio effects (OpenSL). )
AUDPROC_EQ_PRESET_NORMAL_AUDIO_FX - 19\n\n

( Equalizer preset for classical audio effects (OpenSL). )
AUDPROC_EQ_PRESET_CLASSICAL_AUDIO_FX - 20\n\n

( Equalizer preset for dance audio effects (OpenSL). )
AUDPROC_EQ_PRESET_DANCE_AUDIO_FX - 21\n\n

( Equalizer preset for no audio effects (OpenSL). )
AUDPROC_EQ_PRESET_FLAT_AUDIO_FX - 22\n\n

( Equalizer preset for folk audio effects (OpenSL). )
AUDPROC_EQ_PRESET_FOLK_AUDIO_FX - 23\n\n

( Equalizer preset for heavy metal audio effects (OpenSL). )
AUDPROC_EQ_PRESET_HEAVYMETAL_AUDIO_FX - 24\n\n

( Equalizer preset for hip hop audio effects (OpenSL). )
AUDPROC_EQ_PRESET_HIPHOP_AUDIO_FX - 25\n\n

(Equalizer preset for jazz audio effects (OpenSL). )
AUDPROC_EQ_PRESET_JAZZ_AUDIO_FX - 26\n\n

( Equalizer preset for pop audio effects (OpenSL). )
AUDPROC_EQ_PRESET_POP_AUDIO_FX - 27\n\n

( Equalizer preset for rock audio effects (OpenSL). )
AUDPROC_EQ_PRESET_ROCK_AUDIO_FX - 28\n\n
	}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
*/

/* ID of the Popless Equalizer Enable parameter used AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_ENABLE                             0x000108BB

/* Structure for the enable parameter of Popless Equalizer module. */
typedef struct audproc_eq_enable_t audproc_eq_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_ENABLE", AUDPROC_PARAM_ID_EQ_ENABLE}
    @h2xmlp_description {Enables the popless equalizer module. }  */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_ENABLE parameter used by the Popless Equalizer module.*/

struct audproc_eq_enable_t
{
   uint32_t enable_flag;
   /**< @h2xmle_description  { Specifies whether the popless equalizer is enabled. }
        @h2xmle_rangeList    {"Disable"= 0;
                              "Enable"=1}  */
}
#include "adsp_end_pack.h"
;

/* User-customized equalizer preset (with audio effects specified
    individually). */
#define  AUDPROC_EQ_PRESET_USER_CUSTOM     (-1)

/* Equalizer preset for no audio effects. */
#define  AUDPROC_EQ_PRESET_BLANK           0

/* Equalizer preset for audio effects like a club. @newpage */
#define  AUDPROC_EQ_PRESET_CLUB            1

/* Equalizer preset for audio effects like a dance. */
#define  AUDPROC_EQ_PRESET_DANCE           2

/* Qualcomm equalizer preset for full bass audio effects. */
#define  AUDPROC_EQ_PRESET_FULLBASS        3

/* Equalizer preset for bass and treble audio effects. */
#define  AUDPROC_EQ_PRESET_BASSTREBLE      4

/* Equalizer preset for full treble audio effects. */
#define  AUDPROC_EQ_PRESET_FULLTREBLE      5

/* Equalizer preset for audio effects that are suitable for playback through
    laptop/phone speakers.
 */
#define  AUDPROC_EQ_PRESET_LAPTOP          6

/* Equalizer preset for audio effects that sound like a large hall. */
#define  AUDPROC_EQ_PRESET_LHALL           7

/* Equalizer preset for audio effects that sound like a live performance. */
#define  AUDPROC_EQ_PRESET_LIVE            8

/* Equalizer preset for audio effects that sound like a party. */
#define  AUDPROC_EQ_PRESET_PARTY           9

/* Equalizer preset for pop audio effects. */
#define  AUDPROC_EQ_PRESET_POP             10

/* Equalizer preset for reggae audio effects. */
#define  AUDPROC_EQ_PRESET_REGGAE          11

/* Equalizer preset for rock audio effects. */
#define  AUDPROC_EQ_PRESET_ROCK            12

/* Equalizer preset for Ska audio effects. */
#define  AUDPROC_EQ_PRESET_SKA             13

/* Equalizer preset for soft pop audio effects. */
#define  AUDPROC_EQ_PRESET_SOFT            14

/* Equalizer preset for soft rock audio effects. */
#define  AUDPROC_EQ_PRESET_SOFTROCK        15

/* Equalizer preset for techno audio effects. */
#define  AUDPROC_EQ_PRESET_TECHNO          16

/* User-customized equalizer preset (with audio effects specified
    individually) (OpenSL).
 */
#define  AUDPROC_EQ_PRESET_USER_CUSTOM_AUDIO_FX    18

/* Equalizer preset for normal (generic) audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_NORMAL_AUDIO_FX         19

/* Equalizer preset for classical audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_CLASSICAL_AUDIO_FX      20

/* Equalizer preset for dance audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_DANCE_AUDIO_FX          21

/* Equalizer preset for no audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_FLAT_AUDIO_FX           22

/* Equalizer preset for folk audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_FOLK_AUDIO_FX           23

/* Equalizer preset for heavy metal audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_HEAVYMETAL_AUDIO_FX     24

/* Equalizer preset for hip hop audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_HIPHOP_AUDIO_FX         25

/*Equalizer preset for jazz audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_JAZZ_AUDIO_FX           26

/* Equalizer preset for pop audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_POP_AUDIO_FX            27

/**= Equalizer preset for rock audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_ROCK_AUDIO_FX           28

/* ID of the Popless Equalizer Configuration parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_CONFIG                             0x000108BC

/** @h2xmlp_subStruct */

typedef struct audproc_eq_per_band_config_t
{
	uint32_t filter_type;
	/**< @h2xmle_description  {Type of equalizer filter in the sub-band.\n }
	@h2xmle_default      {5} */

	uint32_t freq_millihertz;
	/**< @h2xmle_description  {Center or cutoff frequency of the sub-band filter in millihertz.
	Supported values: 30 to fs/2 (Nyquist rate), depending on the sampling rate of the input audio signal\n }
		@h2xmle_default      {0xEA60} */

	int32_t gain_millibels;
	/**< @h2xmle_description  {Initial gain of the sub-band filter.
	Supported values: +1500 to -1500 mdB in 100 mdB increments.\n }
	@h2xmle_default      {0x0} */

	uint32_t quality_factor;
	/**< @h2xmle_description  {Sub-band filter quality factor expressed as a Q8 number (a
	fixed-point number with a quality factor of 8). For example,
	3000/(2^8)\n }
	@h2xmle_default      {0x0100} */

	uint32_t band_idx;
	/**< @h2xmle_description  {Index of the sub-band filter.\n
	Supported values: 0 to num_bands - 1 (num_bands is specified in
	audproc_eq_config_t)}
	@h2xmle_default      {0x00} */

}audproc_eq_per_band_config_t
#include "adsp_end_pack.h"
;

/* Structure for the config parameter of Popless Equalizer module. */
typedef struct audproc_eq_config_t  audproc_eq_config_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_CONFIG", AUDPROC_PARAM_ID_EQ_CONFIG}
    @h2xmlp_description {Configures the popless equalizer module.\n
    This parameter is only used for Set Parameter calls.\n
    Following this structure are num_bands of %weak_audproc_eq_per_band_config_t
    (see Per-band equalizer parameters).\n
    The length is dependent on the num_bands value.\n
    For predefined Qualcomm and predefined OpenSL equalizers, the sequence of
    per-band parameters is not required, and num_bands must be set to 0.\n }

     @h2xmlx_expandStructs	{false}
	 @h2xmlp_toolPolicy              {Calibration}
    */

#include "adsp_begin_pack.h"

struct audproc_eq_config_t
{
   int32_t eq_pregain;
   /**< @h2xmle_description  {Gain in Q27 before any equalization processing.\n }
	@h2xmle_default      {0x08000000}*/


   int32_t preset_id;
   /**< @h2xmle_description  { Specifies either the user-customized equalizers or two sets of
        equalizers predefined by Qualcomm and OpenSL, respectively. \n
        @values\n
        - -1 -- Custom equalizer that meets Qualcomm requirements\n
        - 0 to 16 -- Equalizers that are predefined by Qualcomm\n
        - 18 -- Custom equalizer that meets OpenSL requirements\n
        - 19 to 28 -- Equalizers that are predefined by OpenSL \n }
        @h2xmle_range        {-1..28}
		@h2xmle_default      {18}*/

   uint32_t num_bands;
   /**< @h2xmle_description  { Specifies number of subbands for equalization when a custom preset ID is
        selected in the preset_id field.\n
        @values\n
        - If preset_id = -1 -- 1 to 12\n
        - If preset_id = 18 -- 5\n
        - All other preset_id settings -- 0\n }
        @h2xmle_range        {0..12}
		@h2xmle_default      {5}		*/

   audproc_eq_per_band_config_t bandEqParams[0];
	/**<@h2xmle_variableArraySize  { "num_bands" } */

}
#include "adsp_end_pack.h"
;


/* ID of the read-only Popless Equalizer Number of Bands parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_NUM_BANDS                          0x000108BD

/* Structure for the num bands parameter of Popless Equalizer module. */
typedef struct audproc_eq_num_bands_t audproc_eq_num_bands_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_NUM_BANDS", AUDPROC_PARAM_ID_EQ_NUM_BANDS}
    @h2xmlp_description {Specifies number of subbands in the current equalizer filter.\n
    This parameter is used only for Get Parameter calls: #ADM_CMD_GET_PP_PARAMS_V5 and #ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n}
	@h2xmlp_toolPolicy              {RTC_READONLY}*/

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_NUM_BANDS parameter used by the Popless Equalizer module. */

struct audproc_eq_num_bands_t
{
   uint32_t num_bands;
   /**< @h2xmle_description  { Specifies Number of subbands in the current equalizer filter.\n
        Supported values\n
        - 1 to 12 -- When equalizers are compliant to or predefined by Qualcomm\n
        - 5 -- When equalizers are compliant to or predefined by OpenSL\n }
        @h2xmle_range        {1..12}
		@h2xmle_default      {5}
        		*/

}
#include "adsp_end_pack.h"
;


/* ID of the read-only Popless Equalizer Band Levels parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER */
#define AUDPROC_PARAM_ID_EQ_BAND_LEVELS                        0x000108BE

/* Structure for the band levels parameter of Popless Equalizer module. */
typedef struct audproc_eq_band_levels_t audproc_eq_band_levels_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_BAND_LEVELS", AUDPROC_PARAM_ID_EQ_BAND_LEVELS}
    @h2xmlp_description {Specifies number of subbands in the current equalizer filter.\n
    This parameter is used only for Get Parameter calls: #ADM_CMD_GET_PP_PARAMS_V5 and #ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n}
	@h2xmlp_toolPolicy              {RTC_READONLY} */


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_BAND_LEVELS parameter used by the Popless Equalizer module. */

struct audproc_eq_band_levels_t
{
   uint32_t num_bands;
   /**< @h2xmle_description  { Specifies Number of subbands in the current equalizer filter.\n
   	    For Get Parameter calls only, the library returns the value.\n
        Supported values\n
        - 1 to 12 -- When equalizers are compliant to or predefined by Qualcomm\n
        - 5 -- When equalizers are compliant to or predefined by OpenSL\n }
        @h2xmle_range        {1..12}
		@h2xmle_default      {5}  */


   int32_t band_levels[12];
   /**< @h2xmle_description { Array of gains (in millibels) of each sub-band filter.\n
        For Get Parameter calls only, the library returns the values.\n

        The meaningful contents in the array depend on the num_bands parameter.\n
        The following example contains valid values returned by the library:\n

        band_levels[0] - band_levels[num_bands-1] } */

}
#include "adsp_end_pack.h"
;

/* ID of the read-only Popless Equalizer Band Level Range parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_BAND_LEVEL_RANGE                   0x000108BF

/* Structure for the band level range parameter of Popless Equalizer module. */
typedef struct audproc_eq_band_level_range_t audproc_eq_band_level_range_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_BAND_LEVEL_RANGE", AUDPROC_PARAM_ID_EQ_BAND_LEVEL_RANGE}
    @h2xmlp_description {This parameter is used only for Get Parameter calls: ADM_CMD_GET_PP_PARAMS_V5 and
    ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n}
	@h2xmlp_toolPolicy              {RTC_READONLY} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_BAND_LEVEL_RANGE parameter used by the Popless Equalizer module. */

struct audproc_eq_band_level_range_t
{
   int32_t min_level_millibels;
    /**< @h2xmle_description  {Specifies minimum gain of sub-band equalizer filters in the current equalizer
        configuration.\n For Get Parameter calls only, the library returns
        the value.\n}

		@h2xmle_range {-1500..-1500}
		@h2xmle_default      {-1500} */

   int32_t max_level_millibels;
   /**< @h2xmle_description  {Specifies maximun gain of sub-band equalizer filters in the current equalizer
        configuration.\n For Get Parameter calls only, the library returns
        the value.\n}

		@h2xmle_range  {1500..1500}
		@h2xmle_default      {1500} */

}
#include "adsp_end_pack.h"
;

/* ID of the read-only Popless Equalizer Band Frequencies parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_BAND_FREQS                         0x000108C0

/* Structure for the band frequency parameter of Popless Equalizer module. */
typedef struct audproc_eq_band_freqs_t audproc_eq_band_freqs_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_BAND_FREQS", AUDPROC_PARAM_ID_EQ_BAND_FREQS}
    @h2xmlp_description {This parameter is used only for Get Parameter calls: ADM_CMD_GET_PP_PARAMS_V5 and
    ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n} 
	@h2xmlp_toolPolicy              {RTC_READONLY}*/

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_BAND_FREQS parameter used by the Popless Equalizer module. */

struct audproc_eq_band_freqs_t
{
   uint32_t num_bands;
   /**< @h2xmle_description  { Specifies number of sub-band equalizer filters in the current equalizer
        configuration.\n For Get Parameter calls only, the library returns
        the value.\n
        Supported values\n
        - 1 to 12 -- When equalizers are compliant to or predefined by Qualcomm\n
        - 5 -- When equalizers are compliant to or predefined by OpenSL\n }

        @h2xmle_range        {1..12}
		@h2xmle_default      {5}  */


   uint32_t band_freqs[12];
    /**< @h2xmle_description { Array of center or cutoff frequencies of each sub-band filter, in
        millihertz.\n
        For Get parameter calls only, the library returns the values.\n

        The meaningful contents in the array depend on the num_bands parameter.\n
        The following example contains valid values returned by the library:
        indent band_freqs[0] - band_freqs[num_bands-1] \n} */

}
#include "adsp_end_pack.h"
;

/* ID of the read-only Popless Equalizer Single Band Frequency range parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE             0x000108C1

/* Structure for the band frequency range parameter of Popless Equalizer module. */
typedef struct audproc_eq_single_band_freq_range_t audproc_eq_single_band_freq_range_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE", AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE}
    @h2xmlp_description {This parameter is used only for Get Parameter calls: ADM_CMD_GET_PP_PARAMS_V5 and
    ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n} 
	@h2xmlp_toolPolicy              {RTC_READONLY}*/


#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE parameter used by the Popless Equalizer module.*/
struct audproc_eq_single_band_freq_range_t
{
   uint32_t min_freq_millihertz;
   /**< @h2xmle_description {Specifies lower frequency boundary of the sub-band equalizer filter with the
        band_index provided by a previous Set Parameter call with
        AUDPROC_PARAM_ID_EQ_BAND_INDEX.\n
        For Get Parameter calls only, the library returns the value.\n
        If the band index is not provided by a previous Set Parameter call,
        the parameters of the first sub-band with band_index = 0 are
        returned.\n Supported values  :Any value in the range of 0 to sample_rate/2 \n} */

   uint32_t max_freq_millihertz;
   /**< @h2xmle_description {Specifies upper frequency boundary of the sub-band equalizer filter with the
        band_index provided by a previous Set Parameter call with
        AUDPROC_PARAM_ID_EQ_BAND_INDEX.\n
        For Get Parameter calls only, the library returns the value.\n
        If the band index is not provided by a previous Set Parameter call,
        the parameters of the first sub-band with band_index = 0 are
        returned.\n Supported values  :Any value in the range of 0 to sample_rate/2 \n} */

}
#include "adsp_end_pack.h"
;

/* ID of the read-only Popless Equalizer Single Band Frequency parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ                   0x000108C2

/* Structure for the band frequency parameter of Popless Equalizer module. */
typedef struct audproc_eq_single_band_freq_t audproc_eq_single_band_freq_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ",AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ parameter used by the
 Popless Equalizer module.\n} 
	@h2xmlp_toolPolicy              {Calibration}*/

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ parameter used by the
 Popless Equalizer module.
 */
struct audproc_eq_single_band_freq_t
{
   uint32_t freq_millihertz;
    /**< @h2xmle_description {For Set Parameter calls only, center or cutoff frequency of the
        sub-band equalizer filter for which the band_index is requested in a
        subsequent Get Parameter call via AUDPROC_PARAM_ID_EQ_BAND_INDEX.\n

        Supported values : Any value in the range of 30 to sample_rate/2 \n } 
		@h2xmle_default      {30}*/
}
#include "adsp_end_pack.h"
;


/** ID of the Popless Equalizer Band Index parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_BAND_INDEX                         0x000108C3

/* Structure for the band index parameter of Popless Equalizer module. */
typedef struct audproc_eq_band_index_t audproc_eq_band_index_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_BAND_INDEX",AUDPROC_PARAM_ID_EQ_BAND_INDEX}
    @h2xmlp_description {It is used for both Set and Get Parameter calls.\n} */

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_BAND_INDEX parameter used by the Popless Equalizer module.  */

struct audproc_eq_band_index_t
{
   uint32_t band_index;
   /**< @h2xmle_description  {Specifies index of each band.\n
         Supported values : \n
         - 0 to 11 -- When equalizers are compliant to or predefined by Qualcomm \n
         - 0 to 4 -- When equalizers are compliant to or predefined by OpenSL \n
         If AUDPROC_PARAM_ID_EQ_BAND_INDEX is used in a Set Parameter call,
        this band_index is used to get the sub-band frequency range in the next
        Get Parameter call with AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE.\n

        If this parameter ID is used in a Get Parameter call, the band_index
        with the center or cutoff frequency is returned. The frequency is
        specified in the Set parameter of
        AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ.\n

        If the center/cutoff frequency of the requested sub-band is not set
        before a Get Parameter call via AUDPROC_PARAM_ID_EQ_BAND_INDEX, the
        default band_index of zero is returned.\n }


		@h2xmle_range {0..11}
		@h2xmle_default      {0x00}		*/

}
#include "adsp_end_pack.h"
;

/** ID of the read-only Popless Equalizer Preset ID parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_PRESET_ID                          0x000108C4

/* Structure for the preset id parameter of Popless Equalizer module. */
typedef struct audproc_eq_preset_id_t audproc_eq_preset_id_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_PRESET_ID",AUDPROC_PARAM_ID_EQ_PRESET_ID}
    @h2xmlp_description {This parameter is used only for Get Parameter calls: ADM_CMD_GET_PP_PARAMS_V5 and
    ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n} 
	@h2xmlp_toolPolicy              {RTC_READONLY}*/

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_PRESET_ID parameter used by the
 Popless Equalizer module.
 */
struct audproc_eq_preset_id_t
{
   int32_t preset_id;
   /**< @h2xmle_description  { Specifies preset ID of the current equalizer configuration.\n For Get Parameter calls only, the library returns
        the value.\n
        Supported values\n
        - -1 to 16 -- When equalizers are compliant to or predefined by Qualcomm\n
        - 18 to 28 -- When equalizers are compliant to or predefined by OpenSL\n }

        @h2xmle_range        {-1..28}  */

}
#include "adsp_end_pack.h"
;

/** ID of the read-only Popless Equalizer Number of Presets parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_NUM_PRESETS                        0x000108C5

/* Structure for the num presets parameter of Popless Equalizer module. */
typedef struct audproc_eq_num_presets_t audproc_eq_num_presets_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_NUM_PRESETS", AUDPROC_PARAM_ID_EQ_NUM_PRESETS}
    @h2xmlp_description {This parameter is used only for Get Parameter calls: ADM_CMD_GET_PP_PARAMS_V5 and
    ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n} 
	@h2xmlp_toolPolicy              {RTC_READONLY}*/

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_NUM_PRESETS parameter used by the
 Popless Equalizer module.
 */

struct audproc_eq_num_presets_t
{
   uint32_t num_presets;
    /**< @h2xmle_description  { Specifies Total number of supported presets in the current equalizer
        configuration.\n
        For Get Parameter calls only, the library returns the value.\n }

        @h2xmle_rangeList        {"When equalizers are compliant to or predefined by Qualcomm (17)"=17;"When equalizers are compliant to or predefined by OpenSL (10)"=10}
		@h2xmle_default      {10}  */
}
#include "adsp_end_pack.h"
;

/** ID of the read-only Popless Equalizer Preset Name parameter used by AUDPROC_MODULE_ID_POPLESS_EQUALIZER. */
#define AUDPROC_PARAM_ID_EQ_PRESET_NAME                        0x000108C6

/* Structure for the preset name parameter of Popless Equalizer module. */
typedef struct audproc_eq_preset_name_t audproc_eq_preset_name_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_EQ_PRESET_NAME", AUDPROC_PARAM_ID_EQ_PRESET_NAME}
    @h2xmlp_description {This parameter is used only for Get Parameter calls: ADM_CMD_GET_PP_PARAMS_V5 and
    ASM_STREAM_CMD_GET_PP_PARAMS_V2.\n} 
	@h2xmlp_toolPolicy              {RTC_READONLY}*/

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_EQ_PRESET_NAME parameter used by the
 Popless Equalizer module.
 */
struct audproc_eq_preset_name_t
{
   uint8_t preset_name[32];
   /**< @h2xmle_description  { Specifies Name of the current equalizer preset (in ASCII characters).\n
        For Get Parameter calls only, the library returns the value\n } */

}
#include "adsp_end_pack.h"
;
/** @}                   <-- End of the Module -->*/
#endif








