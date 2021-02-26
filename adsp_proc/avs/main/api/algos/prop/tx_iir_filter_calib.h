#ifndef TX_IIR_FILTER_CALIB_H
#define TX_IIR_FILTER_CALIB_H
/*==============================================================================
  @file tx_iir_filter_calib.h
  @brief This file contains TX_IIR_FILTER API
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
/** @h2xml_title1           {Tx_IIR_Tuning_Filter}
    @h2xml_title_agile_rev  {Tx_IIR_Tuning_Filter}
    @h2xml_title_date       {February 9, 2017} */

/**
   @h2xmlx_xmlNumberFormat {int}
*/

/* ID of the IIR Tuning Filter module on the Tx path.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS
    - #AUDPROC_PARAM_ID_ENABLE
 */
#define AUDPROC_MODULE_ID_TX_IIR_FILTER                             0x00010C41
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_TX_IIR_FILTER",
                          AUDPROC_MODULE_ID_TX_IIR_FILTER}
    @h2xmlm_displayName  {"AUDIO TX IIR"}
	@h2xmlm_description {ID of the IIR Tuning Filter module on the Tx path.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG\n
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN\n
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS\n
    - #AUDPROC_PARAM_ID_ENABLE\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @h2xmlm_deprecated              {true}
    @{                   <-- Start of the Module -->  */

/* ID of the Tx IIR Filter Enable parameter used by
    AUDPROC_MODULE_ID_TX_IIR_FILTER.
 */
#define AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG                0x00010C42

/* ID of the Tx IIR Filter Pregain parameter used by
    AUDPROC_MODULE_ID_TX_IIR_FILTER.

 */
#define AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN                     0x00010C43

/* ID of the Tx IIR Filter Configuration parameters used by
    AUDPROC_MODULE_ID_TX_IIR_FILTER.


    This structure is followed by the IIR filter coefficients on
    the Tx path as follows:
    - Sequence of int32 ulFilterCoeffs -- Each band has five coefficients, each
      in int32 format in the order of b0, b1, b2, a1, a2.
    - Sequence of int16 sNumShiftFactor -- One int16 per band. The numerator
      shift factor is related to the Q factor of the filter coefficients.
    - Sequence of uint16 usPanSetting -- One uint16 for each band to indicate
      if the filter is applied to left (0), center (1), and right (2) channels.
 */
#define AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS                0x00010C44

/* Structure for enabling the configuration parameter for the
 IIR filter module on the Tx path. */
typedef struct audproc_tx_iir_filter_enable_cfg_t audproc_tx_iir_filter_enable_cfg_t;

/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG", AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG}
    @h2xmlp_description {Structure for enabling the configuration parameter for the
 IIR filter module on the Tx path.}
    @h2xmlp_deprecated              {true}*/
#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG parameter
 in the Tx Path IIR Tuning Filter module.
 */
struct audproc_tx_iir_filter_enable_cfg_t
{
   uint32_t enable_flag;
   /**< @h2xmle_description {Specifies whether the IIR tuning filter is enabled.}
   @h2xmle_rangeList {"Disable"=0;"Enable"=1}
   @h2xmle_policy {basic}
        */
}
#include "adsp_end_pack.h"
;


/* Structure for the pregain parameter for the
 IIR filter module on the Tx path. */
typedef struct audproc_tx_iir_filter_pre_gain_t audproc_tx_iir_filter_pre_gain_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN", AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN}
    @h2xmlp_description {Structure for the pregain parameter for the
 IIR filter module on the Tx path.}
    @h2xmlp_deprecated              {true}*/
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN parameter
 in the Tx Path IIR Tuning Filter module.
 */
struct audproc_tx_iir_filter_pre_gain_t
{
   uint16_t pre_gain;
   /**< @h2xmle_description {Linear gain (in Q13 format)}
        @h2xmle_dataFormat  {Q13}
		@h2xmle_default {0x2000}
		@h2xmle_range {0..0xFFFF}
		@h2xmle_policy {basic}
        */

   uint16_t reserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
   @h2xmle_rangeList {"0"=0}
   @h2xmle_visibility	{hide}
        */
}
#include "adsp_end_pack.h"
;

/* Structure for the configuration parameter for the
 IIR filter module on the Tx path. */
typedef struct audproc_tx_iir_filter_cfg_params_t audproc_tx_iir_filter_cfg_params_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS", AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS}
    @h2xmlp_description {ID of the Tx IIR Filter Configuration parameters used by
    AUDPROC_MODULE_ID_TX_IIR_FILTER.\n


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

/* Payload of the AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS parameter
 in the Tx Path IIR Tuning Filter module.
 */
struct audproc_tx_iir_filter_cfg_params_t
{
   uint16_t num_biquad_stages;
   /**< @h2xmle_description {Number of bands.}
   @h2xmle_range {0..20}
   @h2xmle_default {0x03}
   @h2xmle_policy {advanced}
        */



   uint16_t reserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
   @h2xmle_rangeList {"0"=0}
   @ h2xmle_readOnly {"true"} */


	int32_t ulFilterCoeffs[0];
   /**< @h2xmle_description  {ulFilterCoeffs}
        @h2xmlx_expandArray  {true}
        @h2xmle_default      {0}
		@h2xmle_policy {advanced}
        @h2xmle_variableArraySize  { "5*num_biquad_stages" } */

	int16_t sNumShiftFactor[0];
   /**< @h2xmle_description  {sNumShiftFactor}
        @h2xmlx_expandArray  {true}
        @h2xmle_default      {0x02}
		@h2xmle_policy {advanced}
        @h2xmle_variableArraySize  {num_biquad_stages } */

	uint16_t usPanSetting[0];
   /**< @h2xmle_description  {usPanSetting}
        @h2xmlx_expandArray  {true}
        @h2xmle_default      {1}
		@h2xmle_rangeList      {"TFIIR_PAN_LEFT"=0 ; "TFIIR_PAN_BOTH"=1 ; "TFIIR_PAN_RIGHT"=2}
		@h2xmle_policy {advanced}
        @h2xmle_variableArraySize  {num_biquad_stages} */

}
#include "adsp_end_pack.h"
;
/**
   @}                   <-- End of the Module -->*/
#endif
