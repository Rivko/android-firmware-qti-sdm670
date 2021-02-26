#ifndef AUDIOSPHERE_CALIB_H
#define AUDIOSPHERE_CALIB_H
/*==============================================================================
  @file audiosphere_calib.h
  @brief This file contains AUDIOSPHERE API
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 03/21/17   pag     Added h2xml comments.
 ========================================================================== */
#include "mmdefs.h"
/** @h2xml_title1           {AUDIOSPHERE}
    @h2xml_title_agile_rev  {AUDIOSPHERE}
    @h2xml_title_date       {February 3, 2017} */


/* ID of the AudioSphere module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_AUDIOSPHERE_ENABLE
    - #AUDPROC_PARAM_ID_AUDIOSPHERE_STRENGTH
    - #AUDPROC_PARAM_ID_AUDIOSPHERE_CONFIG_MODE
    - #AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_STEREO_INPUT
    - #AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_MULTICHANNEL_INPUT
    - #AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_STEREO_INPUT
    - #AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_MULTICHANNEL_INPUT
    - #AUDPROC_PARAM_ID_AUDIOSPHERE_OPERATING_INPUT_MEDIA_INFO
    - #AUDPROC_PARAM_ID_ENABLE
 */
/*  Refer to topology recommendations TR4 and TR10. */
#define AUDPROC_MODULE_ID_AUDIOSPHERE                            0x00010916



#define AUDPROC_PARAM_ID_AUDIOSPHERE_ENABLE                      0x00010917


#define AUDPROC_PARAM_ID_AUDIOSPHERE_STRENGTH                     0x00010918


#define AUDPROC_PARAM_ID_AUDIOSPHERE_CONFIG_MODE                  0x00010919

/* ID of the AudioSphere Filter Coefficients parameter used by
    #AUDPROC_MODULE_ID_AUDIOSPHERE for stereo input.

    @msgpayload{audproc_audiosphere_filt_coeffs_t}
    @tablens{weak__audproc__audiosphere__filt__coeffs__t}

    This structure is followed by two arrays of 32-bit coefficients. The first
    array is real coefficients, and the second array is the imaginary part of
    the complex coefficients. @newpage
 */
#define AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_STEREO_INPUT     0x0001091A

/* ID of the AudioSphere Filter Coefficients parameter used by
    #AUDPROC_MODULE_ID_AUDIOSPHERE for multichannel input.

    @msgpayload{audproc_audiosphere_filt_coeffs_t}
    @tablens{weak__audproc__audiosphere__filt__coeffs__t}

    This structure is followed by two arrays of 32-bit coefficients. The first
    array is real coefficients, and the second array is the imaginary part of
    the complex coefficients. @newpage
 */
#define AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_MULTICHANNEL_INPUT  0x0001091B

/* ID of the AudioSphere Filter Design parameter used by
    #AUDPROC_MODULE_ID_AUDIOSPHERE for stereo input.

    @msgpayload{audproc_audiosphere_filter_design_t}
    @table{weak__audproc__audiosphere__filter__design__t} @newpage
 */
#define AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_STEREO_INPUT      0x0001091C

/* ID of the AudioSphere filter design parameter used by
    #AUDPROC_MODULE_ID_AUDIOSPHERE for multichannel input.

    @msgpayload{audproc_audiosphere_filter_design_t}
    @table{weak__audproc__audiosphere__filter__design__t} @newpage
 */
#define AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_MULTICHANNEL_INPUT  0x0001091D

/* ID of the AudioSphere Operating Input Media Information parameter
    used by #AUDPROC_MODULE_ID_AUDIOSPHERE.

    @msgpayload{audproc_audiosphere_operating_input_media_info_t}
    @table{weak__audproc__audiosphere__operating__input__media__info__t} @newpage
 */
#define AUDPROC_PARAM_ID_AUDIOSPHERE_OPERATING_INPUT_MEDIA_INFO           0x0001091E

/* Structure for the enable parameter of AudioSphere module. */
typedef struct audproc_audiosphere_enable_t audproc_audiosphere_enable_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_AUDIOSPHERE_ENABLE", AUDPROC_PARAM_ID_AUDIOSPHERE_ENABLE}
    @h2xmlp_description {ID of the AudioSphere Enable parameter used by
    AUDPROC_MODULE_ID_AUDIOSPHERE.\nPayload of the AUDPROC_PARAM_ID_AUDIOSPHERE_ENABLE parameter used by the
 AudioSphere module.}  */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_ENABLE parameter used by the
 AudioSphere module.
 */
struct audproc_audiosphere_enable_t
{
   uint32_t enable_flag;
   /**< @h2xmle_description  {Specifies whether the AudioSphere module is enabled.}
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}  */

}
#include "adsp_end_pack.h"
;


/* Structure for the strength parameter of AUDIOSPHERE module. */
typedef struct audproc_audiosphere_strength_t audproc_audiosphere_strength_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_AUDIOSPHERE_STRENGTH", AUDPROC_PARAM_ID_AUDIOSPHERE_STRENGTH}
    @h2xmlp_description {ID of the AudioSphere Strength parameter used by
    #AUDPROC_MODULE_ID_AUDIOSPHERE.\nPayload of the AUDPROC_PARAM_ID_AUDIOSPHERE_STRENGTH parameter used by the
 AudioSphere module.}  */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_STRENGTH parameter used by the
 AudioSphere module.
 */
struct audproc_audiosphere_strength_t
{
   uint32_t strength;
   /**< @h2xmle_description  {Specifies the effects of the audiosphere.\n 0 -- No effects \n 1000 -- Maximum effects}
        @h2xmle_range    {0..1000}
		@h2xmle_default {0x000003E8} */
}
#include "adsp_end_pack.h"
;

/* Structure for the config mode parameter of AUDIOSPHERE module. */
typedef struct audproc_audiosphere_config_mode_t audproc_audiosphere_config_mode_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_AUDIOSPHERE_CONFIG_MODE", AUDPROC_PARAM_ID_AUDIOSPHERE_CONFIG_MODE}
    @h2xmlp_description {ID of the AudioSphere Configuration mode parameter used by
    #AUDPROC_MODULE_ID_AUDIOSPHERE.\n Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_CONFIG_MODE parameter used by the
 AudioSphere module. }
  @h2xmlp_version     {0} */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_CONFIG_MODE parameter used by the
 AudioSphere module.
 */
struct audproc_audiosphere_config_mode_t
{
   uint32_t version;
   /**< @h2xmle_description  {Specifies the version information of this parameter.\n (currently, only 0 is supported)}
		@h2xmle_default {0}
		@h2xmle_readonly	{true}
        @h2xmle_isVersion    {true}*/

   uint32_t config_mode;
   /**< @h2xmle_description  {Specifies the mode of operation for the AudioSphere.}
        @h2xmle_rangeList    {"Static mode"=0}
		@h2xmle_default {0} */

}
#include "adsp_end_pack.h"
;

typedef struct audproc_audiosphere_filt_coeffs_t audproc_audiosphere_filt_coeffs_t;
/* @weakgroup weak_audproc_audiosphere_filt_coeffs_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS parameter used by the
 AudioSphere module. */
/* Following this structure is a payload that is a sequence of int32 Imaginary
    coeffs -- 2-D array[num_filters][fft_size/2+1].
    The total number of cint2x32 coefficients is defined by
    (num_filters*(fft_size/2+1)).
    In this case, the total number is 2052, where num_filters is 4 and
    fft_size is 1024.
 */

#include "adsp_begin_pack.h"

struct audproc_audiosphere_filt_coeffs_t
{
   uint32_t version;
   /**< @h2xmle_description  {Specifies the version information of this parameter.(currently, only 0 is supported)}
	@h2xmle_default {0}
	@h2xmle_readonly	{true}
        @h2xmle_isVersion    {true}        */


   uint32_t sample_rate;
   /**< @h2xmle_description  {Specifies the sample rate of the filter coefficients.(48000 Hz - AudioSphere-supported sample rate)}
        @h2xmle_rangeList    {"48000"=48000}
	@h2xmle_default {48000}		*/


   uint32_t fft_size;
   /**< @h2xmle_description  {Specifies the length of FFT.}
        @h2xmle_rangeList    {"1024"=1024}
	@h2xmle_default {1024}		*/

	int32_t real_coeffs[0];
	/**< @h2xmle_description  {...}
        @h2xmlx_expandArray  {true}
        @h2xmle_variableArraySize  {"4*(fft_size/2+1)"}
		@h2xmle_defaultFile {audiosphere_real_coeffs.bin}  */

	int32_t image_coeffs[0];
	/**< @h2xmle_description  {...}
        @h2xmlx_expandArray  {true}
        @h2xmle_variableArraySize  {"4*(fft_size/2+1)"}
		@h2xmle_defaultFile {audiosphere_image_coeffs.bin}  */
}
#include "adsp_end_pack.h"
;

typedef struct audproc_audiosphere_filt_coeffs_t audproc_audiosphere_filt_coeffs_stereo_t ;
typedef struct audproc_audiosphere_filt_coeffs_t audproc_audiosphere_filt_coeffs_multichannel_t ;

/* Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN parameter used by the
 AudioSphere module.
 */
typedef struct audproc_audiosphere_filter_design_t audproc_audiosphere_filter_design_t;

#include "adsp_begin_pack.h"
struct audproc_audiosphere_filter_design_t
{
   uint32_t version;
   /**< @h2xmle_description  {Specifies the version information of this parameter.(currently, only 0 is supported)}
        @h2xmle_range    {0..4294967295}
		@h2xmle_default {0}
        @h2xmle_isVersion    {true}        */

   uint32_t speaker_distance;
   /**< @h2xmle_description  {Specifies the distance (in millimeters) between speakers on the
        handset (in mm). This value is used for designing the filter
        coefficients.}
        @h2xmle_range    {0..4294967295}
		@h2xmle_default {133}		*/


   uint32_t head_distance;
    /**< @h2xmle_description  {Specifies the distance (in millimeters) between center of the head and
        the device. This value is used for designing the filter coefficients.}
        @h2xmle_range    {0..4294967295}
		@h2xmle_default {420}		*/


   uint32_t back_angle;
   /**< @h2xmle_description  {Specifies the angle of the rear virtual speakers. This value is used
        for designing the filter coefficients.\n Any positive value in degrees (0 is the forward direction) }
        @h2xmle_range    {0..4294967295}
		@h2xmle_default {100}		*/


   uint32_t front_angle;
   /**< @h2xmle_description  {Specifies the angle of the front virtual speakers. This value is used
        for designing the filter coefficients.\n

        Any positive value in degrees (0 is the forward direction) }
        @h2xmle_range    {0..4294967295}
		@h2xmle_default {45}		*/


   int32_t max_freq_gain;
   /**< @h2xmle_description  {Specifies the maximum frequency gain (in decibels) provided by the
        corresponding filter coefficients.\n

        Any value in dB  }
		@h2xmle_default {8}		*/

}
#include "adsp_end_pack.h"
;
typedef struct audproc_audiosphere_filter_design_t audproc_audiosphere_filter_design_stereo_t ;
typedef struct audproc_audiosphere_filter_design_t audproc_audiosphere_filter_design_multichannel_t ;


/* Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_OPERATING_INPUT_MEDIA_INFO
 parameter used by the  AudioSphere module.
 */
typedef struct audproc_audiosphere_operating_input_media_info_t audproc_audiosphere_operating_input_media_info_t ; 
 
 /** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_AUDIOSPHERE_OPERATING_INPUT_MEDIA_INFO", AUDPROC_PARAM_ID_AUDIOSPHERE_OPERATING_INPUT_MEDIA_INFO}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_OPERATING_INPUT_MEDIA_INFO
 parameter used by the  AudioSphere module.}
@h2xmlp_toolPolicy {RTC}
@h2xmlp_readOnly   {true}
@h2xmlp_version     {0} */
#include "adsp_begin_pack.h"

struct audproc_audiosphere_operating_input_media_info_t
{
   uint32_t version;
   /**< @h2xmle_description  {Specifies the version information of this parameter.(currently, only 0 is supported)}
        @h2xmle_range    {0..4294967295}
    	@h2xmle_default {0}
        @h2xmle_isVersion    {true}        */


   uint32_t num_input_channel;
   /**< @h2xmle_description  {Number of input channels to the AudioSphere module.\n(Any other value -- Reserved for future use @tablebulletend)}
        @h2xmle_rangeList    {"No valid media format is received (Default)"=0;"Stereo input"=2;"5.1 input"=6;
		"7.1 input"=8}
		@h2xmle_default {0}		*/


   uint32_t sample_rate;
   /**< @h2xmle_description  {Sample rate at which the AudioSphere module is operating.\n (currently, only 48000 Hz is
          supported)}
        @h2xmle_rangeList    {"No valid media format is received (Default)"=0;"48000"=48000}
		@h2xmle_default {0}		*/

}
#include "adsp_end_pack.h"
;
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_AUDIOSPHERE",
                          AUDPROC_MODULE_ID_AUDIOSPHERE}

    @h2xmlm_displayName  {"AUDIOSPHERE"}

    @h2xmlm_description  {ID of the Audiosphere module.\n

      This module supports the following parameter IDs:\n
      - #AUDPROC_PARAM_ID_AUDIOSPHERE_ENABLE
      - #AUDPROC_PARAM_ID_AUDIOSPHERE_STRENGTH
      - #AUDPROC_PARAM_ID_AUDIOSPHERE_CONFIG_MODE
      - #AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_STEREO_INPUT
      - #AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_MULTICHANNEL_INPUT
      - #AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_STEREO_INPUT
      - #AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_MULTICHANNEL_INPUT
      - #AUDPROC_PARAM_ID_AUDIOSPHERE_OPERATING_INPUT_MEDIA_INFO
      - #AUDPROC_PARAM_ID_ENABLE
      All parameter IDs are device independent.\n}

     @h2xmlm_toolPolicy              {Calibration; RTC}

    @{                   <-- Start of the Module -->




	@h2xml_Select					{audproc_audiosphere_enable_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_audiosphere_strength_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_audiosphere_config_mode_t}
    @h2xmlm_InsertParameter

	@h2xml_Select  {audproc_audiosphere_filt_coeffs_stereo_t}
	@h2xmlp_parameter   {"AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_STEREO_INPUT", AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_STEREO_INPUT}
    @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS parameter used by the
	AudioSphere module.\n Following this structure is a payload that is a sequence of int32 Imaginary
    coeffs -- 2-D array[num_filters][fft_size/2+1].
    The total number of cint2x32 coefficients is defined by
    (num_filters*(fft_size/2+1)).
    In this case, the total number is 2052, where num_filters is 4 and
    fft_size is 1024. }
    @h2xmlp_version     {0}
    @h2xmlm_InsertParameter


	@h2xml_Select  {audproc_audiosphere_filt_coeffs_multichannel_t}
	@h2xmlp_parameter          {"AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_MULTICHANNEL_INPUT",AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS_MULTICHANNEL_INPUT}
	@h2xmlp_description {Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_FILT_COEFFS parameter used by the
	AudioSphere module.\n Following this structure is a payload that is a sequence of int32 Imaginary
    coeffs -- 2-D array[num_filters][fft_size/2+1].
    The total number of cint2x32 coefficients is defined by
    (num_filters*(fft_size/2+1)).
    In this case, the total number is 2052, where num_filters is 4 and
	fft_size is 1024. }
        @h2xmlp_version     {0}
	@h2xmlm_InsertParameter

	@h2xml_Select  {audproc_audiosphere_filter_design_stereo_t}
	@h2xmlp_parameter   {"AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_STEREO_INPUT", AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_STEREO_INPUT}
        @h2xmlp_description {Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN parameter used by the
	AudioSphere module.}
        @h2xmlp_version     {0}
	@h2xmlm_InsertParameter


	@h2xml_Select  {audproc_audiosphere_filter_design_multichannel_t}
	@h2xmlp_parameter          {"AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_MULTICHANNEL_INPUT", AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN_MULTICHANNEL_INPUT}
	@h2xmlp_description {Payload of the AUDPROC_PARAM_ID_AUDIOSPHERE_FILTER_DESIGN parameter used by the
	AudioSphere module.}
        @h2xmlp_version     {0}
	@h2xmlm_InsertParameter

	@h2xml_Select					{audproc_audiosphere_operating_input_media_info_t}
        @h2xmlm_InsertParameter




   @}                   <-- End of the Module --> */


#endif
