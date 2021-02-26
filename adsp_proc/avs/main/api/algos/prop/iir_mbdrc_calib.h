#ifndef IIR_MBDRC_CALIB_H
#define IIR_MBDRC_CALIB_H
/*==============================================================================
  @file iir_mbdrc_calib.h
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
 10/13/17   pag     Initial .
 ========================================================================== */

/** @h2xml_title1           {Multiband Dynamic Range Control Parameters}
    @h2xml_title_agile_rev  {Multiband Dynamic Range Control Parameters}
    */

/**
   @h2xmlx_xmlNumberFormat {int}
*/
#include "mmdefs.h"

#define MODULE_ID_IIR_MBDRC                                    0x00010943

/** 
    @h2xmlm_module       {"MODULE_ID_IIR_MBDRC", 
                          MODULE_ID_IIR_MBDRC}
    @h2xmlm_toolPolicy   {Calibration; RTC}
    @h2xmlm_description  {Mutliband Dynamic Range Control Module}
    @{                   <-- Start of the Module -->
*/

/*  ID of the MBDRC Enable parameter used by MODULE_ID_MBDRC.*/
#define PARAM_ID_IIR_MBDRC_ENABLE                              0x00010944

/*  Structure for the enable parameter for an MBDRC module. */
typedef struct iir_mbdrc_enable_t iir_mbdrc_enable_t;
/** @h2xmlp_parameter   {"PARAM_ID_IIR_MBDRC_ENABLE", PARAM_ID_IIR_MBDRC_ENABLE} 
    @h2xmlp_description {Used to configure a device} 
    @h2xmlp_version     {0}	*/

#include "adsp_begin_pack.h"

/*  Payload of the PARAM_ID_IIR_MBDRC_ENABLE parameter used by the
 MBDRC module.
 */
struct iir_mbdrc_enable_t
{
	uint32_t Version;
   /**< @h2xmle_description {Specifies the version.}
        @h2xmle_default     {0}
		@h2xmle_range       {0..32767}
		@h2xmle_readonly	{true}
        @h2xmle_isVersion   {true}            */
        
   uint32_t enable_flag;
   /**< @h2xmle_description  {Specifies whether MBDRC is enabled.}
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}  
        @h2xmle_default      {0}  */
}
#include "adsp_end_pack.h"
;



/*  ID of the MBDRC Configuration parameter used by MODULE_ID_IIR_MBDRC.*/
#define PARAM_ID_IIR_MBDRC_CONFIG_PARAMS                         0x00010945

/* DRC configuration structure for each sub-band of an MBDRC module. */
typedef struct subband_drc_config_params_t subband_drc_config_params_t;


#include "adsp_begin_pack.h"
/** @h2xmlp_subStruct */
struct subband_drc_config_params_t
{
	
   int16_t drc_mode;
   /**< @h2xmle_description {Specifies whether DRC mode is bypassed for subbands.}
        @h2xmle_rangeList   {"Disable"=0; "Enable"=1}
		@h2xmle_default     {1} */
		
   int16_t drc_linked_flag;
   /**< @h2xmle_description {Specifies whether all stereo channels have the same applied dynamics
        or if they process their dynamics independently.}
        @h2xmle_rangeList   {"Not linked,channels process the dynamics independently" = 0;
                             "Linked,channels have the same applied dynamics" = 1}
		@h2xmle_default     {0}							 */

   int16_t drc_down_sample_level;
   /**< @h2xmle_description {DRC down sample level.}
        @h2xmle_default     {1}
		@h2xmle_range    {1..16}*/

   uint16_t drc_rms_time_avg_const;
   /**< @h2xmle_description {RMS signal energy time-averaging constant.}
		@h2xmle_default     {298}
        @h2xmle_range       {0..65535}
		@h2xmle_dataFormat  {Q16}    */

   uint16_t drc_makeup_gain;
   /**< @h2xmle_description {DRC makeup gain in decibels.}
        @h2xmle_default     {4096}
        @h2xmle_range       {258..64917}
		@h2xmle_dataFormat  {Q12}    */

   /* Down expander settings */

   int16_t down_expdr_threshold;
   /**< @h2xmle_description {Down expander threshold.}
                             Its value must be: (<down_cmpsr_threshold)}
        @h2xmle_default     {3877}
        @h2xmle_range       {0..11560}
        @h2xmle_dataFormat  {Q7}  */

   int16_t down_expdr_slope;
   /**< @h2xmle_description {Down expander slope.}
        @h2xmle_default     {65434}
        @h2xmle_range       {-32768..0}
        @h2xmle_dataFormat  {Q8}  */
		
   uint16_t down_expdr_hysteresis;
   /**< @h2xmle_description {Down expander hysteresis constant.}
        @h2xmle_default     {18855}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q14}  */

   uint32_t down_expdr_attack;
   /**< @h2xmle_description {Down expander attack constant.}
        @h2xmle_default     {15690611}
        @h2xmle_range       {0..2147483648}
        @h2xmle_dataFormat  {Q31}  */

   uint32_t down_expdr_release;
   /**< @h2xmle_description {Down expander release constant.}
        @h2xmle_default     {39011832}
        @h2xmle_range       {0..2147483648}
        @h2xmle_dataFormat  {Q31}  */
		
   int32_t down_expdr_min_gain_db;
   /**< @h2xmle_description {Down expander minimum gain.}
		@h2xmle_default     {-50331648}
        @h2xmle_range       {-805306368..0}
        @h2xmle_dataFormat  {Q23}  */

   /* Up compressor settings */

   int16_t up_cmpsr_threshold;
   /**< @h2xmle_description {Up compressor threshold.
                             Its value must be:
                             (&gt;down_expdr_threshold) &amp;&amp; (&lt;down_cmpsr_threshold)}
        @h2xmle_default     {3877}
        @h2xmle_range       {0..11560}
        @h2xmle_dataFormat  {Q7}  */
		
   uint16_t up_cmpsr_slope;
   /**< @h2xmle_description {Up compressor slope.}
        @h2xmle_range       {0..64880}
		@h2xmle_default     {0}
        @h2xmle_dataFormat  {Q8}  */

   uint32_t up_cmpsr_attack;
   /**< @h2xmle_description {Up compressor attack constant.}
        @h2xmle_default     {7859688}
        @h2xmle_range       {0..2147483648}
        @h2xmle_dataFormat  {Q31}  */

   uint32_t up_cmpsr_release;
   /**< @h2xmle_description {Up compressor release constant.}
        @h2xmle_default     {7859688}
        @h2xmle_range       {0..2147483648}
        @h2xmle_dataFormat  {Q31}  */
		


   uint16_t up_cmpsr_hysteresis;
   /**< @h2xmle_description {Up compressor hysteresis constant.}
        @h2xmle_default     {18855}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q14}  */

   /* Down compressor settings */

   int16_t down_cmpsr_threshold;
   /**< @h2xmle_description {Down compressor threshold.
                             Its value must be: (&gt;up_cmpsr_threshold)}
        @h2xmle_default     {9637}
        @h2xmle_range       {0..11560}
        @h2xmle_dataFormat  {Q7}  */

   uint16_t down_cmpsr_slope;
   /**< @h2xmle_description {Down compressor slope.}
		@h2xmle_default     {62259}
		@h2xmle_range       {0..64880}
        @h2xmle_dataFormat  {Q8}  */
		
   uint16_t down_cmpsr_hysteresis;
   /**< @h2xmle_description {Down compressor hysteresis constant.}
        @h2xmle_default     {18855}
        @h2xmle_range       {1..32690}
        @h2xmle_dataFormat  {Q14}  */
		
   uint32_t down_cmpsr_attack;
   /**< @h2xmle_description {Down compressor attack constant.}
        @h2xmle_default     {77314964}
        @h2xmle_range       {0..2147483648}
        @h2xmle_dataFormat  {Q31}  */

   uint32_t down_cmpsr_release;
   /**< @h2xmle_description {Down compressor release constant.}
        @h2xmle_default     {1574244}
        @h2xmle_range       {0..2147483648}
        @h2xmle_dataFormat  {Q31}  */

		
}
#include "adsp_end_pack.h"
;


#define IIR_MAX_COEFFS_PER_BAND 10

typedef struct iir_filter_config_params_t iir_filter_config_params_t;

#include "adsp_begin_pack.h"
/** @h2xmlp_subStruct */
struct iir_filter_config_params_t
{
	uint32_t num_even_stages; 
	/**< @h2xmle_description {even filter stages;}
         @h2xmle_default     {3}
         @h2xmle_range       {0..3}
          */
	uint32_t num_odd_stages; 
	/**< @h2xmle_description {odd filter stages}
         @h2xmle_default     {2}
         @h2xmle_range       {0..2}
          */

	/* according to allpass biquad filter's transfer function:         */
	/* H_ap(z) = (b0 + b1*z^-1 + z^-2)/(1 + b1*z^-1 + b0*z^-2)         */
	/* only b0 and b1 needs to be saved					               */
	/* Besides, the 3-rd stage of even allpass filter is 1st order IIR */
	/* H_even_3rd(z) = (b0 + z^-1)/(1 + b0*z^-1)                       */
	/* only b0 needs to be saved								       */
	/* iir_coeffs store order is:									   */
	/* [b0_even1 b1_even1 | b0_even2 b1_even2 | b0_even3 b1_even3 | ...*/
	/* ...b0_odd1 b1_odd1 | b0_odd2 b1_odd2]                           */
	/* if num_even_stages are 2 , then b0_even3 = 0 and  b1_even3= 0   */
	int32_t iir_coeffs[IIR_MAX_COEFFS_PER_BAND];
	/**< @h2xmle_description {IIR filter coefficients.}
          */

}
#include "adsp_end_pack.h"
;	

typedef struct limiter_config_param_t limiter_config_param_t;


#include "adsp_begin_pack.h"
/** @h2xmlp_subStruct */
struct limiter_config_param_t
{
	
    int32_t limiter_threshold;
   /**< @h2xmle_description  {Threshold in decibels for the limiter output.
							\n For 16bit use case: limiter threshold is [-96dB  0dB].
							\n For 24bit use case: limiter threshold is [-162dB  24dB].
							\n For true 32bit use case: limiter threshold is [-162dB  0dB].
							\n If a value out of this range is configured, it will be automatically limited to the upper bound or low bound in the DSP processing.}
        @h2xmle_default      {93945856}
		@h2xmle_range        {0..2127207634}
        @h2xmle_dataFormat   {Q27} */

   int32_t limiter_makeup_gain;
   /**< @h2xmle_description  {Makeup gain in decibels for the limiter output.}
        @h2xmle_default      {256}
		@h2xmle_range        {1..32228}
        @h2xmle_dataFormat   {Q8}  */

   int32_t limiter_gc;
   /**< @h2xmle_description  {Limiter gain recovery coefficient.}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {32440}
        @h2xmle_dataFormat   {Q15}  */

   int32_t limiter_max_wait;
   /**< @h2xmle_description  {Maximum limiter waiting time in samples.}
        @h2xmle_range        {0..328}
        @h2xmle_default      {82}
        @h2xmle_dataFormat   {Q15}  */
		
   uint32_t gain_attack;      
   /**< @h2xmle_description  {Limiter gain attack time}
        @h2xmle_default      {188099735}
		@h2xmle_range        {0..2147483648}
        @h2xmle_dataFormat   {Q31}  */
		
   uint32_t gain_release;     
   /**< @h2xmle_description  {Limiter gain release time}
        @h2xmle_default      {32559427}
		@h2xmle_range        {0..2147483648}
        @h2xmle_dataFormat   {Q31}  */
		
   uint32_t attack_coef;     
   /**< @h2xmle_description  {Limiter gain attack time speed coef}
        @h2xmle_default      {32768}
		@h2xmle_range        {32768..3276800}
        @h2xmle_dataFormat   {Q15}  */
		
   uint32_t release_coef;     
   /**< @h2xmle_description  {Limiter gain release time speed coef}
        @h2xmle_default      {32768}
		@h2xmle_range        {32768..3276800}
        @h2xmle_dataFormat   {Q15}  */
		
   int32_t hard_threshold;   
   /**< @h2xmle_description  {Hard Threshold in decibels for the limiter output.
							\n For 16bit use case: limiter hard threshold is [-96dB  0dB].
							\n For 24bit use case: limiter hard threshold is [-162dB  24dB].
							\n For true 32bit use case: limiter hard threshold is [-162dB  0dB].
							\n If a value out of this range is configured, it will be automatically limited to the upper bound or low bound in the DSP processing.}
        @h2xmle_default      {93945856}
		@h2xmle_range        {0..2127207634}
        @h2xmle_dataFormat   {Q27} */
}
#include "adsp_end_pack.h"
;	

/* Structure for the configuration parameters for an MBDRC module. */
typedef struct iir_mbdrc_per_ch_config_params_t iir_mbdrc_per_ch_config_params_t;

#include "adsp_begin_pack.h"
/** @h2xmlp_subStruct */
struct iir_mbdrc_per_ch_config_params_t
{
   uint32_t channel_mask_lsb;
   /**< @h2xmle_description  {Lower 32 bits of the mask that indicates the corresponding channel 
							 whose config is to be set.\n 
							 - Set the bits corresponding to 1 to 31 channels of standard channel 
							 mapping (channels are mapped per standard channel mapping)\n 
							 - Position of the bit to set 1 (left shift)(channel_map) \n
							 - Set the Least significant digit to 1 to enable per-channel calibration \n
							 - Set the Least significant digit to 0 to disable per-channel calibration \n }
		@h2xmle_default      {4294967294}*/



   uint32_t channel_mask_msb;
   /**< @h2xmle_description  {Upper 32 bits of the mask that indicates the corresponding channel
                              whose config is to be set.
                              - Set the bits corresponding to 32 to 63 channels of standard channel
                                mapping (channels are mapped per standard channel mapping)
                              - Position of the bit to set 1 (left shift)(channel_map - 32)}
		@h2xmle_default      {4294967295}					  */
							 
							 
	limiter_config_param_t limiter;
   /**< @h2xmle_description  {...}
        @h2xmle_default      {0}
		@h2xmlx_expandStructs {false} */	

	subband_drc_config_params_t subband_drc[0];
   /**< @h2xmle_description  { Following this structure is the sub-band payload,
                               This sub-band structure must be repeated for each band.
                               After this DRC structure is configured for valid bands, the next MBDRC
                               setparams expects the sequence of sub-band MBDRC filter coefficients (the
                               length depends on the number of bands) plus the mute flag for that band
                               plus uint16 padding.
                               }
        @h2xmle_variableArraySize {"iir_mbdrc_config_params_t::num_bands"}   
        */	

	iir_filter_config_params_t iirFilter[0];
   /**< @h2xmle_description  {...}
        @h2xmle_default      {0}
        @h2xmle_variableArraySize {"(iir_mbdrc_config_params_t::num_bands)- 1"} */ 

	int32_t fMuteFlag[0];
   /**< @h2xmle_description  {...}
        @h2xmle_default      {0}
        @h2xmle_variableArraySize {"iir_mbdrc_config_params_t::num_bands"} */ 
           
}
#include "adsp_end_pack.h"
;


typedef struct iir_mbdrc_config_params_t iir_mbdrc_config_params_t;
/** @h2xmlp_parameter   {"PARAM_ID_IIR_MBDRC_CONFIG_PARAMS", PARAM_ID_IIR_MBDRC_CONFIG_PARAMS} 
    @h2xmlp_description {Used to configure a device}
    @h2xmlp_version     {0}	*/
#include "adsp_begin_pack.h"

struct iir_mbdrc_config_params_t
{
	uint32_t Version;
   /**< @h2xmle_description {Specifies the version.}
        @h2xmle_default     {0}
		@h2xmle_range       {0..32767}
		@h2xmle_readonly	{true}
        @h2xmle_isVersion   {true}            */
				
				
   uint32_t num_bands;
   /**< @h2xmle_description  {Number of bands.}
        @h2xmle_default      {1}
        @h2xmle_range        {1..10}  */	
		
		
	uint32_t limiter_mode;
   /**< @h2xmle_description {Specifies whether Limiter mode is bypassed for subbands.}
		@h2xmle_default      {1}
        @h2xmle_rangeList   {"Disable"=0; "Enable"=1} */
		
   uint32_t limiter_delay;
   /**< @h2xmle_description  {Limiter delay in samples.
                              Range: 0 to 10 samples. Default 0.008}
        @h2xmle_range        {0..327680}
        @h2xmle_default      {262}
        @h2xmle_dataFormat   {Q15}  */	
 		
   uint32_t limiter_history_winlen;
	/**< @h2xmle_description  {Length of history window}
        @h2xmle_range        {0..32767}
        @h2xmle_default      {2884}
        @h2xmle_dataFormat   {Q15}  */
		
		
   uint32_t num_config;
   /**< @h2xmle_description  {Specifies the different sets of mbdrc configurations.}
        @h2xmle_range        {1..63}
        @h2xmle_default      {1} */
		
	uint32_t drc_delay[0];   
   /**< @h2xmle_description {DRC delay in samples.}
        @h2xmle_range       {0..96000}
		@h2xmle_variableArraySize {num_bands}*/

   iir_mbdrc_per_ch_config_params_t configData[0];
   /**< @h2xmle_description {Specifies the different sets of mbdrc configurations}
        @h2xmle_variableArraySize {num_config}
        @h2xmle_default      {0} */
		

		
}
#include "adsp_end_pack.h"
;


/** ID of the MBDRC Configuration parameter used by #MODULE_ID_IIR_MBDRC.*/
#define PARAM_ID_IIR_MBDRC_FILTER_XOVER_FREQS             0x00010946

#define IIR_MBDRC_MAX_BANDS 10


/* Structure for the MBDRC filter cross over frequencies parameter
 * for an MBDRC module. */
typedef struct iir_mbdrc_per_ch_filter_xover_freqs_t iir_mbdrc_per_ch_filter_xover_freqs_t;


#include "adsp_begin_pack.h"


/* Payload of the PARAM_ID_IIR_MBDRC_FILTER_XOVER_FREQS parameter used
 by the MBDRC module.
 */
 /** @h2xmlp_subStruct */
struct iir_mbdrc_per_ch_filter_xover_freqs_t
{
	
   uint32_t channel_mask_lsb;
   /**< @h2xmle_description  {Lower 32 bits of the mask that indicates the corresponding channel whose config is to be set.\n - Set the bits corresponding to 1 to 31 channels of standard channel mapping (channels are mapped per standard channel mapping)\n - Position of the bit to set 1 (left shift)(channel_map) \n} */



   uint32_t channel_mask_msb;
   /**< @h2xmle_description  {Upper 32 bits of the mask that indicates the corresponding channel
                              whose config is to be set.
                              - Set the bits corresponding to 32 to 63 channels of standard channel
                                mapping (channels are mapped per standard channel mapping)
                              - Position of the bit to set 1 (left shift)(channel_map - 32)} */
   uint32_t	iir_mbdrc_cross_over_freqs[IIR_MBDRC_MAX_BANDS-1];
   /**< @h2xmle_description  {Array of filter crossover frequencies. Based on Band number n,
                              filter_xover_freqs[MBDRC_MAX_BANDS-1] has (n-1) 
                              crossover frequecies and the rest(if any) are ignored.
                              } */
}
#include "adsp_end_pack.h"
;

typedef struct iir_mbdrc_filter_xover_freqs_t iir_mbdrc_filter_xover_freqs_t;
/** @h2xmlp_parameter   {"PARAM_ID_IIR_MBDRC_FILTER_XOVER_FREQS", PARAM_ID_IIR_MBDRC_FILTER_XOVER_FREQS} 
    @h2xmlp_description {Used to configure a device}
	@h2xmlp_version     {0} */
#include "adsp_begin_pack.h"

struct iir_mbdrc_filter_xover_freqs_t
{
   uint32_t Version;
   /**< @h2xmle_description {Specifies the version.}
        @h2xmle_default     {0}
		@h2xmle_range       {0..32767}
 		@h2xmle_readonly	{true}
        @h2xmle_isVersion   {true}               */
		
   uint32_t num_config;
   /**< @h2xmle_description  {Specifies the different sets of mbdrc configurations.}
        @h2xmle_range        {1..63}
        @h2xmle_default      {1} */
		
	iir_mbdrc_per_ch_filter_xover_freqs_t iir_mbdrc_per_ch_cross_over_freqs[0];
	/**< @h2xmle_description {Specifies the different sets of mbdrc configurations}
        @h2xmle_variableArraySize {num_config}
        @h2xmle_default      {1} */
		
}
#include "adsp_end_pack.h"
;


/** @}                   <-- End of the Module -->*/

#endif
