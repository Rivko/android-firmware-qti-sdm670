#ifndef RIGHT_IIR_TUNING_FILTER_CALIB_H
#define RIGHT_IIR_TUNING_FILTER_CALIB_H
/*==============================================================================
  @file right_iir_tuning_filter_calib.h
  @brief This file contains RIGHT_IIR_TUNING_FILTER API
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
#include "multichan_iir_tuning_filter_param_calib.h"

/** @h2xml_title1           {Rx_Right_IIR_Tuning_Filter}
    @h2xml_title_agile_rev  {Rx_Right_IIR_Tuning_Filter}
    @h2xml_title_date       {February 22, 2017} */

/**
   @h2xmlx_xmlNumberFormat {int}
*/


#define AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER 0x00010706

/**

    @h2xmlm_module       {"AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER",
                          AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER}

    @h2xmlm_displayName  {"AUDIO IIR RIGHT"}

	@h2xmlm_description {ID of the RIGHT IIR Tuning Filter module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG\n
    - #AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN\n
    - #AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS\n
    - #AUDPROC_PARAM_ID_ENABLE\n ID of the IIR Tuning Filter module on the Rx path for the right channel.\n
The parameter IDs of the IIR tuning filter module (AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
are used for the right IIR Rx tuning filter.\n
Pan parameters are not required for this per-channel IIR filter, and they are ignored by this module.\n
}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @h2xmlm_deprecated              {true}
    @{                   <-- Start of the Module -->

	@h2xml_Select  {audproc_iir_tuning_filter_enable_t}
	@h2xmlm_InsertParameter

	@h2xml_Select  {audproc_iir_tuning_filter_pregain_t}
	@h2xmlm_InsertParameter

	@h2xml_Select  {audproc_iir_filter_config_params_t}
	@h2xmlm_InsertParameter

   @}                   <-- End of the Module --> */

#endif
