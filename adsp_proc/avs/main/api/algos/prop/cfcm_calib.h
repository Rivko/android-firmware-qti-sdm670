#ifndef CFCM_CALIB_H
#define CFCM_CALIB_H
/*==============================================================================
  @file cfcm_calib.h
  @brief This file contains CFCM API
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

/** @h2xml_title1           {CFCM}
    @h2xml_title_agile_rev  {CFCM}
    @h2xml_title_date       {February 27, 2017} */
#include "mmdefs.h"
#include "audio_common_param_calib.h"
#define AUDPROC_MODULE_ID_CFCM                            0x00010933
/** @h2xmlm_module       {"AUDPROC_MODULE_ID_CFCM",
                          AUDPROC_MODULE_ID_CFCM}
    @h2xmlm_displayName  {"AUDIO CFCM"}
	@h2xmlm_description {ID of Crossover filtering and Channel mixing module.\n

    This module supports the following parameter IDs:\n
    -#AUDPROC_PARAM_ID_CFCM_ENABLE\n
    -#AUDPROC_PARAM_ID_CFCM_CONFIG_FILTERBANK\n
    -#AUDPROC_PARAM_ID_CFCM_CONFIG_PARAMS\n
    -#AUDPROC_PARAM_ID_LIB_VERSION\n
}
 @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->   */



#define AUDPROC_PARAM_ID_CFCM_ENABLE                      0x00010934

/* Structure for enabling CFCM module. */

typedef struct audproc_cfcm_enable_t audproc_cfcm_enable_t;
/** @h2xmlp_parameter   {"AU DPROC_PARAM_ID_CFCM_ENABLE", AUDPROC_PARAM_ID_CFCM_ENABLE}
    @h2xmlp_description {ID of the enable parameter used by AUDPROC_MODULE_ID_CFCM.\nPayload of the AUDPROC_PARAM_ID_CFCM_ENABLE parameter\n}
	*/
#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_CFCM_ENABLE parameter
 */
struct audproc_cfcm_enable_t
{

      uint32_t cfcm_enable;
	  /**< @h2xmle_description  {Specifies whether cfcm is enabled. }
        @h2xmle_rangeList    {"Disable"=0;
                              "Enable"=1}
        @h2xmle_default      {0}*/
}
#include "adsp_end_pack.h"
;


#define AUDPROC_PARAM_ID_CFCM_CONFIG_FILTERBANK                      0x00010935

/* Structure for CFCM config filterbank parameters. */
typedef struct audproc_cfcm_config_filterbank_t audproc_cfcm_config_filterbank_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_CFCM_CONFIG_FILTERBANK", AUDPROC_PARAM_ID_CFCM_CONFIG_FILTERBANK}
    @h2xmlp_description {ID of the CFCM config filterbank parameters used by AUDPROC_MODULE_ID_CFCM.\nPayload of the AUDPROC_PARAM_ID_CFCM_CONFIG_FILTERBANK\n}
	@h2xmlp_version     {0}  */
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_CFCM_CONFIG_FILTERBANK
 */
struct audproc_cfcm_config_filterbank_t
{
    uint32_t   version;
	/**< @h2xmle_description  {Version of this param id}
         @h2xmle_isVersion    {true}
         @h2xmle_readOnly     {true} */


    uint32_t   cfcm_crossover_freq;
	/**< @h2xmle_description  {crossover frequency}
         @h2xmle_default      {1000}
        */

    uint32_t   cfcm_filterbank_order;
	/**< @h2xmle_description  {order of filter bank}
         @h2xmle_default      {5}  */

    uint32_t   cfcm_num_allpass_stages[2];
	/**< @h2xmle_description  {allpass stages}
         @h2xmle_defaultList  {2, 1}
        */

    int32_t    cfcm_iir_coeffs[10];
	/**< @h2xmle_description  {iir filter coefficients}
         @h2xmle_defaultList  {126487926, -258475280, -117705787, 0, 0, 0, 111381350, -243497943, 0, 0} */
}
#include "adsp_end_pack.h"
;


#define AUDPROC_PARAM_ID_CFCM_CONFIG_PARAMS                      0x00010936

/* Structure for CFCM config parameters. */
typedef struct audproc_cfcm_config_params_t audproc_cfcm_config_params_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_CFCM_CONFIG_PARAMS", AUDPROC_PARAM_ID_CFCM_CONFIG_PARAMS}
    @h2xmlp_description {ID of the CFCM config parameters used by AUDPROC_MODULE_ID_CFCM.\nPayload of the AUDPROC_PARAM_ID_CFCM_CONFIG_PARAMS\n}
	@h2xmlp_version     {0}*/
#include "adsp_begin_pack.h"


/* Payload of the AUDPROC_PARAM_ID_CFCM_CONFIG_PARAMS
 */
struct audproc_cfcm_config_params_t
{
    uint32_t    version;
	/**< @h2xmle_description  {Version of this param id}
          @h2xmle_isVersion    {true}
          @h2xmle_readOnly     {true} */

    uint32_t    cfcm_lf_mixing_switch;
	/**< @h2xmle_description  {mixing switch }
         @h2xmle_default      {0} */

    int32_t     cfcm_receiver_path_gain;
	/**< @h2xmle_description  {receiver gain}
          @h2xmle_default      {0x7FFFFFF} */

    int32_t     cfcm_speaker_path_gain;
	/**< @h2xmle_description  {speaker gain }
        @h2xmle_default      {0x7FFFFFF} */

}
#include "adsp_end_pack.h"
;

/**
	@h2xml_Select					{audproc_lib_version_t}
    @h2xmlm_InsertParameter
*/

 /**  @}                   <-- End of the Module --> */

#endif
