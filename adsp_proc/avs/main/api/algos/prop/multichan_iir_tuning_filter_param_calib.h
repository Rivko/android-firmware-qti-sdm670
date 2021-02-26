#ifndef MULTICHAN_IIR_TUNING_FILTER_PARAM_CALIB_H
#define MULTICHAN_IIR_TUNING_FILTER_PARAM_CALIB_H
/*==============================================================================
  @file multichan_iir_tuning_filter_param_calib.h
  @brief This file contains IIR_TUNING_FILTER_PARAM API
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


#include "mmdefs.h"

/**
   @h2xmlx_xmlNumberFormat {int}
*/

	
/* ID of the IIR Tuning Filter Enable parameter used by
    AUDPROC_MODULE_ID_IIR_TUNING_FILTER.

 
 */
#define AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG            0x00010C03

/* ID of the IIR Tuning Filter Pregain parameter used by
    AUDPROC_MODULE_ID_IIR_TUNING_FILTER.
 */
#define AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN                 0x00010C04

/* ID of the IIR Tuning Filter Configuration parameters used by
    #AUDPROC_MODULE_ID_IIR_TUNING_FILTER.

    @msgpayload{audproc_iir_filter_config_params_t}
    @table{weak__audproc__iir__filter__config__params__t}
    This structure is followed by the IIR filter coefficients on
    the Tx path as follows:
    - Sequence of int32 ulFilterCoeffs -- Each band has five coefficients, each
      in int32 format in the order of b0, b1, b2, a1, a2.
    - Sequence of int16 sNumShiftFactor -- One int16 per band. The numerator
      shift factor is related to the Q factor of the filter coefficients.
    - Sequence of uint16 usPanSetting -- One uint16 for each band to indicate
      if the filter is applied to left (0), center (1), and right (2) channels.
 */
#define AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS            0x00010C05

/* Structure for an enable configuration parameter for an
 IIR tuning filter module. */
typedef struct audproc_iir_tuning_filter_enable_t audproc_iir_tuning_filter_enable;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG", AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG} 
    @h2xmlp_description {Structure for an enable configuration parameter for an
 IIR tuning filter module.}
    @h2xmlp_deprecated              {true}*/

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG
 parameter used by the IIR Tuning Filter module.
 */
struct audproc_iir_tuning_filter_enable_t
{
   uint32_t enable_flag;
   /**< @h2xmle_description {Specifies whether the IIR tuning filter is enabled.}
        @h2xmle_rangeList {"Disable"=0;"Enable"=1}
        */
   
}
#include "adsp_end_pack.h"
;

/* Structure for the pregain parameter for an IIR tuning filter module. */
typedef struct audproc_iir_tuning_filter_pregain_t audproc_iir_tuning_filter_pregain;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN", AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN}
    @h2xmlp_description {Structure for the pregain parameter for an IIR tuning filter module.}
    @h2xmlp_deprecated              {true}*/

#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN
 parameters used by the IIR Tuning Filter module.
 */
struct audproc_iir_tuning_filter_pregain_t
{
   uint16_t pregain;
/**< @h2xmle_description {Linear gain (in Q13 format)}
        @h2xmle_dataFormat  {Q13}
        */
   

   uint16_t reserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
   @h2xmle_rangeList {"0"=0}
        */
}
#include "adsp_end_pack.h"
;


/* Structure for the configuration parameter for an IIR tuning filter module. */
typedef struct audproc_iir_filter_config_params_t audproc_iir_filter_config_params;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS", AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS} 
    @h2xmlp_description {ID of the IIR Tuning Filter Configuration parameters used by
    #AUDPROC_MODULE_ID_IIR_TUNING_FILTER.\n

    This structure is followed by the IIR filter coefficients on
    the Tx path as follows:\n
    - Sequence of int32 ulFilterCoeffs -- Each band has five coefficients, each
      in int32 format in the order of b0, b1, b2, a1, a2.\n
    - Sequence of int16 sNumShiftFactor -- One int16 per band. The numerator
      shift factor is related to the Q factor of the filter coefficients.\n
    - Sequence of uint16 usPanSetting -- One uint16 for each band to indicate
      if the filter is applied to left (0), center (1), and right (2) channels.\n}

    @h2xmlp_deprecated              {true}*/
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS
 parameters used by the IIR Tuning Filter module.
 */
struct audproc_iir_filter_config_params_t
{
   uint16_t num_biquad_stages;
   /**< @h2xmle_description {Number of bands.}
        @h2xmle_range  {0..20}
		@h2xmle_default  {3}
        */
   

   uint16_t reserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
   @h2xmle_rangeList {"0"=0}
        */
		
   int32_t ulFilterCoeffs[0];
   /**< @h2xmle_description  {...}
        @h2xmlx_expandArray  {true}
        @h2xmle_default      {0}
		@h2xmle_policy {advanced}
        @h2xmle_variableArraySize  { "5*num_biquad_stages" } */ 

   int16_t sNumShiftFactor[0];		
   /**< @h2xmle_description  {...}
        @h2xmlx_expandArray  {true}
        @h2xmle_default      {0x02}
		@h2xmle_policy {advanced}
        @h2xmle_variableArraySize  {"1*num_biquad_stages" } */ 
	
   uint16_t usPanSetting[0];		
   /**< @h2xmle_description  {...}
        @h2xmlx_expandArray  {true}
        @h2xmle_default      {1}
		@h2xmle_rangeList      {"TFIIR_PAN_LEFT"=0 ; "TFIIR_PAN_BOTH"=1 ; "TFIIR_PAN_RIGHT"=2}
		@h2xmle_policy {advanced}
        @h2xmle_variableArraySize  {"1*num_biquad_stages"} */ 	
		
   
}
#include "adsp_end_pack.h"
;
#endif

