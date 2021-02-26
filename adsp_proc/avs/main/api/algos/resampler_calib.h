#ifndef RESAMPLER_CALIB_H
#define RESAMPLER_CALIB_H
/*==============================================================================
  @file resampler_calib.h
  @brief This file contains RESAMPLER API
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


/** @h2xml_title1           {Audio RESAMPLER}
    @h2xml_title_agile_rev  {Audio RESAMPLER}
    @h2xml_title_date       {February 9, 2017} */

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
#define AUDPROC_MODULE_ID_RESAMPLER   0x00010719
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_RESAMPLER",
                          AUDPROC_MODULE_ID_RESAMPLER}
    @h2xmlm_displayName  {"AUDIO HIGH THD RESAMPLER"}

    @h2xmlm_description  {ID of the RESAMPLER module.\n

   This module supports the AUDPROC_PARAM_ID_DYNAMIC_RESAMPLER_MODE_CONFIG.\n }

    @h2xmlm_toolPolicy              {Calibration; RTC}

    @{                   <-- Start of the Module -->
*/
/* ID of the Dynamic Resampler Mode Configuration parameter used by AUDPROC_MODULE_ID_RESAMPLER. */
#define AUDPROC_PARAM_ID_DYNAMIC_RESAMPLER_MODE_CONFIG   0x00010911

/* Structure for setting the dynamic mode configuration for the Resampler. */
typedef struct audproc_dynamic_resampler_mode_config_t audproc_dynamic_resampler_mode_config_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_DYNAMIC_RESAMPLER_MODE_CONFIG", AUDPROC_PARAM_ID_DYNAMIC_RESAMPLER_MODE_CONFIG}
    @h2xmlp_description {Specifies the mode of operation for the dynamic re-sampler.}  */

struct audproc_dynamic_resampler_mode_config_t
{
   uint8_t dynamic_mode;
   /**< @h2xmle_description  {  Specifies the mode of operation for the dynamic re-sampler.}
         @h2xmle_rangeList   {"Generic resampling"= 0;
                              "Dynamic resampling"=1}
        @h2xmle_default      {0}  */


   uint8_t delay_type;
   /**< @h2xmle_description  { Specifies the delay type for the Dynamic Resampling mode.\n
        This delay_type value has significance only if the dynamic_mode value
        is set to 1.\n }
         @h2xmle_rangeList   {"High delay with smooth transition"= 0;
                              "Low delay with visible transitional phase distortion"=1}
        @h2xmle_default      {0}  */


   uint16_t reserved;
   /**< @h2xmle_description  { Clients must set this field to 0.}
         @h2xmle_rangeList   {"0"= 0}
         @h2xmle_visibility	 {hide}
         @h2xmle_default      {0}  */

}
#include "adsp_end_pack.h"
;

/** @}                   <-- End of the Module -->*/
#endif






















