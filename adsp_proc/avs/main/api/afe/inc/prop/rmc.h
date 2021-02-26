#ifndef __RMC_H__
#define __RMC_H__

/*==============================================================================
  @file aanc.h
  @brief This file contains APIs for Remove Click Noise module

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/prop/rmc.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  05/20/17    sw        (Tech Pubs) Edited descriptions and values
==============================================================================*/

/** @h2xml_title1           {Remove Click Noise Module}
    @h2xml_title_date       {December 14, 2016} */

#include "mmdefs.h"
#include "afe_common.h"

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Module ID */
#define AFE_MODULE_RMC             0x00010207

/** @h2xmlm_module      {"AFE_MODULE_RMC",AFE_MODULE_RMC}
    @h2xmlm_toolPolicy  {Calibration;RTC}
    @h2xmlm_description {Supports the Remove Click Noise feature.}
    @{                  <-- Start of the Module --> */

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_RMC_CONFIG         0x00010208

/* Minor Version of this parameter */
#define AFE_API_VERSION_RMC_CONFIG      0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_rmc_cfg_param_v1_t afe_rmc_cfg_param_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_RMC_CONFIG", AFE_PARAM_ID_RMC_CONFIG}
    @h2xmlp_description {Sets or gets RMC configuration.}
    @h2xmlp_version     {AFE_API_VERSION_RMC_CONFIG} */
#include "adsp_begin_pack.h"
struct afe_rmc_cfg_param_v1_t
{
  uint32_t rmc_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_RMC_CONFIG}
       @h2xmle_default     {AFE_API_VERSION_RMC_CONFIG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int16_t rmc_neg_on_set_detect_thresh;
  /**< @h2xmle_description {Threshold that triggers the start of negative
                            spike tracking.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x392D} */

  int16_t rmc_pos_on_set_detect_thresh;
  /**< @h2xmle_description {Threshold that triggers the start of positive
                            spike tracking.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x287A} */

  int16_t rmc_on_set_slope;
  /**< @h2xmle_description {Ratio between x(n) and x(n-2) that triggers the
                            start of spike tracking.}
       @h2xmle_range       {1..32767}
       @h2xmle_default     {0x0800} */

  int16_t rmc_neg_peak_detect_thresh;
  /**< @h2xmle_description {Threshold for determining the negative peak.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x392D} */

  int16_t rmc_pos_peak_detect_thresh;
  /**< @h2xmle_description {Threshold for determining the positive peak.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x287A} */

  int16_t rmc_neg_click_tail_thresh;
  /**< @h2xmle_description {Threshold for tracking the long smooth tail after
                            the negative peak.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x2027} */

  int16_t rmc_pos_click_tail_thresh;
  /**< @h2xmle_description {Threshold for tracking the long smooth tail after
                            the positive peak.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x1449} */

  uint16_t rmc_ramp_upDn_length;
  /**< @h2xmle_description {Length of gain ramping up or down.}
       @h2xmle_range       {2..48000}
       @h2xmle_default     {0x60} */

  uint16_t rmc_delay_length;
  /**< @h2xmle_description {Size of the delay buffer used for look-ahead
                            buffering.}
       @h2xmle_range       {0..48000}
       @h2xmle_default     {0xC0} */

  int16_t rmc_next_pos_on_set_thresh;
  /**< @h2xmle_description {Threshold for switching to positive spike detection
                            When still in negative spike detection and a
                            positive spike arrives.}
       @h2xmle_range       {0..32767}
       @h2xmle_default     {0x287A} */

  int16_t rmc_next_pos_on_set_slope;
  /**< @h2xmle_description {Ratio between x(n) and x(n-2) for switching to
                            positive spike detection when still in negative
                            spike detection and a positive spike arrives.}
       @h2xmle_range       {1..32767}
       @h2xmle_default     {0x0800} */

  int16_t rmc_gain_step_dB;
  /**< @h2xmle_description {Size of the gain step used for gain ramping up or
                            down.}
       @h2xmle_range       {-11520..0}
       @h2xmle_default     {0xFF87} */

  uint32_t rmc_peak_length;
  /**< @h2xmle_description {Length of the negative or positive peaks.}
       @h2xmle_range       {2..48000}
       @h2xmle_default     {0x90} */

  uint32_t rmc_click_tail_length;
  /**< @h2xmle_description {Length of the negative or positive tail after
                            the peaks.}
       @h2xmle_range       {3..480000}
       @h2xmle_default     {0x03C0} */

  uint32_t rmc_total_neg_mute_length;
  /**< @h2xmle_description {Length of the total muting time for a negative
                            spike.}
       @h2xmle_range       {5..480000}
       @h2xmle_default     {0x7080} */

  uint32_t rmc_total_pos_mute_length;
  /**< @h2xmle_description {Length of the total muting time for a positive
                            spike.}
       @h2xmle_range       {5..480000}
       @h2xmle_default     {0x3FC0} */

  uint32_t rmc_next_pos_decay_length;
  /**< @h2xmle_description {Length of the muting time that must be extended
                            when still in negative spike detection and a
                            positive spike arrives.}
       @h2xmle_range       {0..480000}
       @h2xmle_default     {0x3FC0} */
}
#include "adsp_end_pack.h"
;

/** @h2xml_Select         {afe_mod_enable_param_t}
    @h2xmlm_InsertParameter */

/** @} <-- End of the Module --> */

#endif /* __RMC_H__ */