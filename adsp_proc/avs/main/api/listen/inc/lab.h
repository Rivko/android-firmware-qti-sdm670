#ifndef _LOOK_AHEAD_BUFFER_
#define _LOOK_AHEAD_BUFFER_

/*==============================================================================
  @file detection_engine_type.h
  @brief This file contains Public APIs for Modules using Look Ahead Buffering
         Feature

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/lab.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  04/20/17   sw         (Tech Pubs) Edited descriptions.
  03/20/17   unni       Created File
==============================================================================*/

/** @h2xml_title1          {Lookahead Buffering Module}
    @h2xml_title_date      {April 7, 2017} */

#include "mmdefs.h"
/*------------------------------------------------------------------------------
   Module ID
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition.
   Module library is independent of this number, it defined here for static
   loading purpose only */
#define LSM_MODULE_LAB    0x00012C08

/** @h2xmlm_module       {"LSM_MODULE_LAB",LSM_MODULE_LAB}
    @h2xmlm_toolPolicy   {NO_SUPPORT}
    @h2xmlm_description  {Supports the Lookahead Buffering feature.}
    @{                   <-- Start of the Module --> */

/*==============================================================================
   Constants
==============================================================================*/
/* Unique Parameter id */
#define LSM_PARAM_ID_LAB_CONFIG    0x00012C0A

/* Minor version of this PARAM */
#define LSM_API_VERSION_LAB_CONFIG   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_lab_config_v1 lsm_param_id_lab_config_v1_t;
/** @h2xmlp_parameter   {"LSM_PARAM_ID_LAB_CONFIG", LSM_PARAM_ID_LAB_CONFIG}
    @h2xmlp_description {Configures settings for lookahead buffering.}
    @h2xmlp_version     {LSM_API_VERSION_LAB_CONFIG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct lsm_param_id_lab_config_v1
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_LAB_CONFIG}
       @h2xmle_default     {LSM_API_VERSION_LAB_CONFIG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t wake_up_latency_ms;
  /**< @h2xmle_description {Time (in milliseconds) the client processor takes
                            to respond to a keyword detection event and set up
                            the data path for lookahead buffering.} */
}
#include "adsp_end_pack.h"
;

/** @} <-- End of the Module --> */

#endif /* _LOOK_AHEAD_BUFFER_ */
