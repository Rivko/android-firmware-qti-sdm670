#ifndef _AUDIO_COMMON_H_
#define _AUDIO_COMMON_H_

/*==============================================================================
  @file audio_common.h
  @brief This file contains Public APIs used by Audio modules

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/audio_common.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  04/20/17   sw         (Tech Pubs) Edited descriptions
  03/17/17   unni       Created File
==============================================================================*/

/** @h2xml_title1     {Common Public Interface}
    @h2xml_title_date {April 7, 2017} */

#include "mmdefs.h"

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_GAIN     0x00012C03
/* Remapping macro. */
#define LSM_PARAM_ID_GAIN   PARAM_ID_GAIN

/* Minor Version of this PARAM */
#define LSM_API_VERSION_GAIN    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_gain lsm_param_id_gain_t;

/** @h2xmlp_parameter   {"PARAM_ID_GAIN", PARAM_ID_GAIN}
    @h2xmlp_description {Sets the gain that is applied to incoming audio data.}
    @h2xmlp_version     {LSM_API_VERSION_GAIN} */
#include "adsp_begin_pack.h"
struct lsm_param_id_gain
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_GAIN}
       @h2xmle_default     {LSM_API_VERSION_GAIN}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int16_t gain;
  /**< @h2xmle_description {Gain that is applied to the data.}
       @h2xmle_default     {0x100}
       @h2xmle_dataFormat  {Q8} */

  uint16_t reserved;
  /**< @h2xmle_description {Used for alignment; must be set to 0.}
       @h2xmle_readonly    {true}
       @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_ENABLE     0x00012C09
/* Remapping macro. */
#define LSM_PARAM_ID_ENABLE   PARAM_ID_ENABLE

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_enable lsm_param_id_enable_t;

/** @h2xmlp_parameter   {"PARAM_ID_ENABLE", PARAM_ID_ENABLE}
    @h2xmlp_description {Enables or disables a module.} */
#include "adsp_begin_pack.h"
struct lsm_param_id_enable
{
  int16_t enable;
  /**< @h2xmle_description {Enables or disables a module.}
       @h2xmle_rangeList   {Disabled=0; Enabled=1} 
       @h2xmle_default     {0} */

  uint16_t reserved;
  /**< @h2xmle_description {Used for alignment; must be set to 0.}
       @h2xmle_readonly    {true}
       @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

#endif /* _AUDIO_COMMON_H_ */
