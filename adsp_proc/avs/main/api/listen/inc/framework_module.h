#ifndef _FRAMEWORK_MODULE_
#define _FRAMEWORK_MODULE_

/*==============================================================================
  @file detection_engine_type.h
  @brief This file contains Public APIs for Modules using LSM Framework Module

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/framework_module.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  04/20/17   sw         (Tech Pubs) Edited descriptions and some values.
  03/20/17   unni       Created File
==============================================================================*/

/** @h2xml_title1          {LSM Framework Module}
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
#define LSM_MODULE_FRAMEWORK    0x00012C0E

/** @h2xmlm_module      {"LSM_MODULE_FRAMEWORK",LSM_MODULE_FRAMEWORK}
    @h2xmlm_toolPolicy  {NO_SUPPORT}
    @h2xmlm_description {Supports the LSM Framework feature.}
    @{                  <-- Start of the Module --> */

/*==============================================================================
   Constants
==============================================================================*/
/* Unique Parameter id */
#define LSM_PARAM_ID_CONNECT_TO_PORT    0x00012C04

/* Minor version of this PARAM */
#define LSM_API_VERSION_CONNECT_TO_PORT   0x1

/* ID of the port used in LSM_PARAM_ID_CONNECT_TO_PORT when the LSM connects
    to the ADM.  */
#define PORT_ID_FOR_CONNECTING_TO_ADM   0xADCB

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_connect_port lsm_param_id_connect_port_t;
/** @h2xmlp_parameter   {"LSM_PARAM_ID_CONNECT_TO_PORT",
                         LSM_PARAM_ID_CONNECT_TO_PORT}
    @h2xmlp_description {Connects the LSM session to an AFE port.}
    @h2xmlp_version     {LSM_API_VERSION_CONNECT_TO_PORT}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct lsm_param_id_connect_port
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_CONNECT_TO_PORT}
       @h2xmle_default     {LSM_API_VERSION_CONNECT_TO_PORT}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint16_t port_id;
  /**< @h2xmle_description {ID of the AFE port from which the Detection Engine
                            receives the data. To connect the LSM to the Audio
                            Device Manager, set this value to 0xADCB.}
       @h2xmle_default     {0xADCB} */

  uint16_t reserved;
  /**< @h2xmle_description {Used for alignment; must be set to 0.}
       @h2xmle_readonly    {true}
       @h2xmle_default     {0}  */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/
/* Unique Parameter id */
#define LSM_PARAM_ID_FWK_MODE_CONFIG    0x00012C27

/* Minor version of this PARAM */
#define LSM_API_VERSION_FWK_MODE_CONFIG   0x1

/* LSM service on the aDSP sends the old LSM_SESSION_EVENT_DETECTION_STATUS_V2
   without timestamp information. */
#define LSM_NON_TIME_STAMP_MODE 0
/* LSM service on the aDSP sends a new LSM_SESSION_EVENT_DETECTION_STATUS_V3
   with timestamp information. */
#define LSM_TIME_STAMP_MODE 1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_fwk_mode_config lsm_param_id_fwk_mode_config_t;
/** @h2xmlp_parameter   {"LSM_PARAM_ID_FWK_MODE_CONFIG",
                         LSM_PARAM_ID_FWK_MODE_CONFIG}
    @h2xmlp_description {Used by clients to set the mode to LSM. \n
                         \n
                         If this parameter is not sent, by default the LSM sends
                         LSM_SESSION_EVENT_DETECTION_STATUS_V2 without
                         timestamp information.}
    @h2xmlp_version     {LSM_API_VERSION_FWK_MODE_CONFIG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct lsm_param_id_fwk_mode_config
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_FWK_MODE_CONFIG}
       @h2xmle_default     {LSM_API_VERSION_FWK_MODE_CONFIG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t mode;
  /**< @h2xmle_description {Specifies whether the aDSP sends the old detection
                            event or a new detection event.}
       @h2xmle_rangeList   {"Non-timestamp mode"=LSM_NON_TIME_STAMP_MODE;
                            "Timestamp mode"=LSM_TIME_STAMP_MODE} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/
/* Unique Parameter id */
#define LSM_PARAM_ID_MEDIA_FMT    0x00012C1E

/* Minor version of this PARAM */
#define LSM_API_VERSION_MEDIA_FMT   0x2

/* Maximum number of channels. */
#define LSM_MAX_NUM_CHANNELS 8

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_media_fmt_v2 lsm_param_id_media_fmt_v2_t;
/** @h2xmlp_parameter   {"LSM_PARAM_ID_MEDIA_FMT", LSM_PARAM_ID_MEDIA_FMT}
    @h2xmlp_description {Provides media format information for the LSM session.}
    @h2xmlp_version     {LSM_API_VERSION_MEDIA_FMT}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct lsm_param_id_media_fmt_v2
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_MEDIA_FMT}
       @h2xmle_default     {LSM_API_VERSION_MEDIA_FMT}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t sampling_rate;
  /**< @h2xmle_description {Sampling rate (in Hz) at which this device operates.}
       @h2xmle_rangeList   {"16 kHz"=16000; "48000 kHz"=48000}
       @h2xmle_default     {16000} */

  uint16_t num_channels;
  /**< @h2xmle_description {Number of channels.}
       @h2xmle_rangeList   {Mono=1; Stereo=2; "3 ch"=3; "Quad"=4; "5 ch"=5;
                            "6 ch"=6; "7 ch"=7; "8 ch"=8}
       @h2xmle_default     {1} */

  uint16_t bit_width;
  /**< @h2xmle_description {Bit width of the sample.}
       @h2xmle_rangeList   {"16-bit"=16,"24-bit"=24}
       @h2xmle_default     {16} */

  uint8_t channel_mapping[LSM_MAX_NUM_CHANNELS];
  /**< @h2xmle_description {Channel mapping of the LSM session.}
       @h2xmle_range       {1..9}
       @h2xmle_default     {1} */
}
#include "adsp_end_pack.h"
;

/** @} <-- End of the Module --> */

#endif /* _FRAMEWORK_MODULE_ */
