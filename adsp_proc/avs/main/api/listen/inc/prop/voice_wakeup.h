#ifndef _VOICE_WAKEUP_H_
#define _VOICE_WAKEUP_H_

/*==============================================================================
  @file voice_wakeup.h
  @brief This file contains Public APIs for Detection Engine Module

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/prop/voice_wakeup.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  07/01/17   sw         (Tech Pubs) Edited descriptions
  03/17/17   unni       Created File
==============================================================================*/

/** @h2xml_title1     {Detection Engine Module}
    @h2xml_title_date {April 7, 2017} */

#include "mmdefs.h"
#include "audio_common.h"
#include "detection_engine.h"
#include "listen_common_params.h"

/*==============================================================================
   Constants
==============================================================================*/
/* Unique Paramete id */
#define PARAM_ID_KWD_USER_ENABLE_MODE  0x00012C17
/* Remapping macro. */
#define LSM_PARAM_ID_KWD_USER_ENABLE_MODE PARAM_ID_KWD_USER_ENABLE_MODE

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_kwd_user_enable lsm_param_id_kwd_user_enable_t;

/** @h2xmlp_parameter   {"PARAM_ID_KWD_USER_ENABLE_MODE",
                         PARAM_ID_KWD_USER_ENABLE_MODE}
    @h2xmlp_description {Selectively enables a keyword or user.}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */

#include "adsp_begin_pack.h"
struct lsm_param_id_kwd_user_enable
{
  uint8_t num_active_models;
  /**< @h2xmle_description {Sum of all keyword models plus active user models.}
       @h2xmle_range       {1..20}
       @h2xmle_default     {1} */

  uint8_t keyword_user_enables[0];
  /**< @h2xmle_description {Array that enumerates the enable (1) or disable (0)
                            status for each keyword and active user pair. \n
                            \n
                            Immediately following this array is another array
                            of size num_active_models. Each element is 1 byte
                            in length. \n
                            \n
                            Assuming N keywords and M active user models are
                            included in the model, the first N elements of this
                            parameter correspond to enabling or disabling N
                            keywords. The N+1 to N+M elements control enabling
                            or disabling the M active user model. Each element
                            maps enable (1) or disable (0) values for entries
                            (keywords or user-specific keywords) whose order is
                            specified in the sound model.}
       @h2xmle_variableArraySize {num_active_models} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramete id */
#define PARAM_ID_SWMAD_ENABLE   0x00012C1A
/* Remapping macro. */
#define LSM_PARAM_ID_SWMAD_ENABLE   PARAM_ID_SWMAD_ENABLE 

/* Minor version of this PARAM */
#define LSM_API_VERSION_SWMAD_ENABLE    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_swmad_enable lsm_param_id_swmad_enable_t;

/** @h2xmlp_parameter   {"PARAM_ID_SWMAD_ENABLE", PARAM_ID_SWMAD_ENABLE}
    @h2xmlp_description {Enables Software Mic Activity Detection (SWMAD).}
    @h2xmlp_version     {LSM_API_VERSION_SWMAD_ENABLE} */
#include "adsp_begin_pack.h"
struct lsm_param_id_swmad_enable
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_SWMAD_ENABLE}
       @h2xmle_default     {LSM_API_VERSION_SWMAD_ENABLE}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t enable;
  /**< @h2xmle_description {Enables or disables SWMAD.}
       @h2xmle_rangeList   {Disabled=0; Enabled=1}
       @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/
/* Unique Parameter id */
#define PARAM_ID_SWMAD_CFG 0x00012C18
/* Remapping macro. */
#define LSM_PARAM_ID_SWMAD_CFG PARAM_ID_SWMAD_CFG 

/* Minor version of this PARAM */
#define LSM_API_VERSION_SWMAD_CFG   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_swmad_cfg lsm_param_id_swmad_cfg_t;

/** @h2xmlp_parameter   {"PARAM_ID_SWMAD_CFG", PARAM_ID_SWMAD_CFG}
    @h2xmlp_description {Configures SWMAD parameters.}
    @h2xmlp_version     {LSM_API_VERSION_SWMAD_CFG} */
#include "adsp_begin_pack.h"
struct lsm_param_id_swmad_cfg
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_SWMAD_CFG}
       @h2xmle_default     {LSM_API_VERSION_SWMAD_CFG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t sw_mad_thresh_Q23;
  /**< @h2xmle_description {Score threshold for the binary classifier.}
       @h2xmle_range       {-2147483648..2139095040}
       @h2xmle_dataFormat  {Q23}
       @h2xmle_default     {-511573} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Parameter id */
#define PARAM_ID_SWMAD_MODEL    0x00012C19
/* Remapping macro. */
#define LSM_PARAM_ID_SWMAD_MODEL    PARAM_ID_SWMAD_MODEL

/*==============================================================================
   Type definitions
==============================================================================*/

/** @h2xmlp_parameter   {"PARAM_ID_SWMAD_MODEL", PARAM_ID_SWMAD_MODEL}
    @h2xmlp_description {Registers the SWMAD model. This model contains the
                         unique sound characteristics or signatures used by
                         the algorithm hosted in the module.}*/
struct param_id_swmad_model
{
  uint8_t blob[0];
  /**< @h2xmle_description {Array that represents the sound model. The size of
                            the array is the actual size of the sound model.}
       @h2xmle_elementType  {rawData}
       @h2xmle_displayType  {file}
       @h2xmle_defaultFile  {swmad_default.bin} */
};

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramete id */
#define PARAM_ID_POLLING_ENABLE   0x00012C1B
/* Remapping macro. */
#define LSM_PARAM_ID_POLLING_ENABLE   PARAM_ID_POLLING_ENABLE 

/* Minor version of this PARAM */
#define LSM_API_VERSION_POLLING_ENABLE    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_polling_enable lsm_param_id_polling_enable_t;

/** @h2xmlp_parameter   {"PARAM_ID_POLLING_ENABLE", PARAM_ID_POLLING_ENABLE}
    @h2xmlp_description {Sets the behavior of the algorithm when returning
                         detection events. \n
                         \n
                         If polling is disabled, the algorithm returns only
                         detection events. If polling is enabled, the algorithm
                         returns both detection and rejection events.}
    @h2xmlp_version     {LSM_API_VERSION_POLLING_ENABLE} */
#include "adsp_begin_pack.h"
struct lsm_param_id_polling_enable
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_POLLING_ENABLE}
       @h2xmle_default     {LSM_API_VERSION_POLLING_ENABLE}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t polling_enable;
  /**< @h2xmle_description {Enables or disables polling.}
       @h2xmle_rangeList   {Disabled=0; Enabled=1}
       @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/
/* Unique Parameter id */
#define LSM_PARAM_ID_VOICE_WAKEUP_CUSTOM_CONFIG      0x00012C20
/* Remapping macro. */
#define PARAM_ID_VOICE_WAKEUP_CUSTOM_CONFIG \
        LSM_PARAM_ID_VOICE_WAKEUP_CUSTOM_CONFIG

/* Minor version of this PARAM */
#define LSM_API_VERSION_VOICE_WAKEUP_CUSTOM_CONFIG   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_voice_wakeup_custom_config
               lsm_param_id_voice_wakeup_custom_config_t;
/** @h2xmlp_parameter   {"PARAM_ID_VOICE_WAKEUP_CUSTOM_CONFIG",
                         PARAM_ID_VOICE_WAKEUP_CUSTOM_CONFIG}
    @h2xmlp_description {Passes the custom configuration to the Detection
                         Engine.}
    @h2xmlp_version     {LSM_API_VERSION_VOICE_WAKEUP_CUSTOM_CONFIG}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct lsm_param_id_voice_wakeup_custom_config
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_VOICE_WAKEUP_CUSTOM_CONFIG}
       @h2xmle_default     {LSM_API_VERSION_VOICE_WAKEUP_CUSTOM_CONFIG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t hist_buffer_duration_msec;
  /**< @h2xmle_description {Specifies the history buffer duration in
                            milliseconds. \n
                            \n
                            The history buffer is used to hold the data prior
                            to keyword detection and also the keyword. The
                            duration is limited by the memory available in
                            the system.}
       @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition.
   Module library is independent of this number, it defined here for static
   loading purpose only */
#define LSM_MODULE_VOICE_WAKEUP_V3        0x00012C1C
/* Remapping Macro */
#define MODULE_ID_VOICE_WAKEUP_V3 LSM_MODULE_VOICE_WAKEUP_V3

/*==============================================================================
   Module definition
==============================================================================*/

/** @h2xmlm_module       {"MODULE_ID_VOICE_WAKEUP_V3",MODULE_ID_VOICE_WAKEUP_V3}
    @h2xmlm_toolPolicy   {Calibration;RTC}
    @h2xmlm_description  {Detection Engine feature that supports multiple
                          keywords and multiple users.}
    @{                   <-- Start of the Module --> 

    @h2xml_Select        {"PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_REGISTER"}
    @h2xmlm_InsertParameter
    @h2xml_Select        {"PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_DEREGISTER"}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_gain}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_epd_threshold}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_operation_mode}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_min_confidence_levels}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_kwd_user_enable}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_swmad_enable}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_swmad_cfg}
    @h2xmlm_InsertParameter
    @h2xml_Select        {param_id_swmad_model}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_polling_enable}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_voice_wakeup_custom_config}
    @h2xmlm_InsertParameter

    @} <-- End of the Module --> */

/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition.
   Module library is independent of this number, it defined here for static
   loading purpose only */
#define LSM_MODULE_VOICE_WAKEUP_V2        0x00012C0D
/* Remapping Macro */
#define MODULE_ID_VOICE_WAKEUP LSM_MODULE_VOICE_WAKEUP_V2

/*==============================================================================
   Module definition
==============================================================================*/

/** @h2xmlm_module       {"MODULE_ID_VOICE_WAKEUP_V2",MODULE_ID_VOICE_WAKEUP}
    @h2xmlm_toolPolicy   {Calibration;RTC}
    @h2xmlm_description  {Detection Engine feature that supports multiple
                          keywords and multiple users.}
    @h2xmlm_deprecated   {true}
    @{                   <-- Start of the Module -->

    @h2xml_Select        {"PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_REGISTER"}
    @h2xmlm_InsertParameter
    @h2xml_Select        {"PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_DEREGISTER"}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_gain}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_epd_threshold}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_operation_mode}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_min_confidence_levels}
    @h2xmlm_InsertParameter

    @} <-- End of the Module --> */

#endif /* _VOICE_WAKEUP_H_ */
