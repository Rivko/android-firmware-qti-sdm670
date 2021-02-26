#ifndef __SIDETONE_IIR_FILTER_H__
#define __SIDETONE_IIR_FILTER_H__

/*==============================================================================
  @file aanc.h
  @brief This file contains APIs for Sidetone Infinite Impulse Response Filter Module

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/prop/sidetone_iir_filter.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  05/20/17    sw        (Tech Pubs) Edited descriptions and values
==============================================================================*/

/** @h2xml_title1           {Sidetone Infinite Impulse Response Filter Module}
    @h2xml_title_date       {January 3, 2017} */

#include "mmdefs.h"
#include "afe_common.h"

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Module ID */
#define AFE_MODULE_SIDETONE_IIR_FILTER      0x00010202

/** @h2xmlm_module      {"AFE_MODULE_SIDETONE_IIR_FILTER",
                         AFE_MODULE_SIDETONE_IIR_FILTER}
    @h2xmlm_toolPolicy  {Calibration;RTC}
    @h2xmlm_description {Supports the Sidetone IIR Filter feature.}
    @{                  <-- Start of the Module --> */

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG     0x00010204

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_sidetone_iir_filter_config_params_t 
               afe_sidetone_iir_filter_config_params_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG",
                         AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG}
    @h2xmlp_description {Configures the sidetone IIR filter.} */
#include "adsp_begin_pack.h"
struct afe_sidetone_iir_filter_config_params_t
{ 
  uint16_t num_biquad_stages;
  /**< @h2xmle_description {Number of stages.}
       @h2xmle_range       {1..10}
       @h2xmle_default     {3} */

  uint16_t pregain;
  /**< @h2xmle_description {Pregain for the compensating filter response.}
       @h2xmle_dataFormat  {Q13}
       @h2xmle_default     {0x2000} */
}
#include "adsp_end_pack.h"
;

/** @h2xml_Select         {afe_mod_enable_param_t}
    @h2xmlm_InsertParameter */

/** @} <-- End of the Module --> */

#endif /* __SIDETONE_IIR_FILTER_H__ */
