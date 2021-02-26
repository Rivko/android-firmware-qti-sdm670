#ifndef _LISTEN_COMMON_PARAMS_H_
#define _LISTEN_COMMON_PARAMS_H_

/*==============================================================================
  @file listen_common_params.h
  @brief This file contains common parameters for listen libraries

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/prop/listen_common_params.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  07/20/17   sw         (Tech Pubs) Edited descriptions
  05/12/17   unni       Created File
==============================================================================*/

/** @h2xml_title1     {Common Parameters for LSM Libraries}
    @h2xml_title_date {June 1, 2017} */

#include "mmdefs.h"

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_ENDPOINT_DETECT_THRESHOLD    0x00012C01
/* Remapping Macro */
#define LSM_PARAM_ID_ENDPOINT_DETECT_THRESHOLD \
        PARAM_ID_ENDPOINT_DETECT_THRESHOLD

/* Minor Version of this PARAM */
#define LSM_API_VERSION_EPD_THRESHOLD   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_epd_threshold lsm_param_id_epd_threshold_t;

/** @h2xmlp_parameter   {"PARAM_ID_ENDPOINT_DETECT_THRESHOLD",
                         PARAM_ID_ENDPOINT_DETECT_THRESHOLD}
    @h2xmlp_description {Sets the beginning-of-speech and end-of-speech
                         thresholds.}
    @h2xmlp_version     {LSM_API_VERSION_EPD_THRESHOLD} */
#include "adsp_begin_pack.h"
struct lsm_param_id_epd_threshold
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_EPD_THRESHOLD}
       @h2xmle_default     {LSM_API_VERSION_EPD_THRESHOLD}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t epd_begin_threshold;
  /**< @h2xmle_description {Threshold for the beginning of speech.}
       @h2xmle_default     {0x02710000}
       @h2xmle_dataFormat  {Q20} */

  uint32_t epd_end_threshold;
  /**< @h2xmle_description {Threshold for the end of speech.}
       @h2xmle_default     {0xFA9B62B7}
       @h2xmle_dataFormat  {Q20} */

}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_OPERATION_MODE   0x00012C02
/* Remapping Macro */
#define LSM_PARAM_ID_OPERATION_MODE   PARAM_ID_OPERATION_MODE

/* Minor Version of this PARAM */
#define LSM_API_VERSION_OPERATION_MODE    0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_operation_mode lsm_param_id_operation_mode_t;

/** @h2xmlp_parameter   {"PARAM_ID_OPERATION_MODE",
                         PARAM_ID_OPERATION_MODE}
    @h2xmlp_description {Sets the operation mode for the Detection Engine.}
    @h2xmlp_version     {LSM_API_VERSION_OPERATION_MODE}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct lsm_param_id_operation_mode
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..LSM_API_VERSION_OPERATION_MODE}
       @h2xmle_default     {LSM_API_VERSION_OPERATION_MODE}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint16_t mode;
  /**< @h2xmle_description {Flags that configure the Detection Engine to
                            run in different modes.}

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {detectionMode}
       @h2xmle_description {Keyword or Pattern Detection mode.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1}
       @h2xmle_bitFieldEnd
       
       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {verificationMode}
       @h2xmle_description {User Verification mode.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd
       
       @h2xmle_bitField    {0x00000004}
       @h2xmle_bitName     {eventMode}
       @h2xmle_description {Detection events are sent to the HLOS}
       @h2xmle_rangeList   {Success=0;Always=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd   */

  uint16_t reserved;
  /**< @h2xmle_description {Used for alignment; must be set to 0.}
       @h2xmle_readonly    {true} */

}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/
/* Unique Paramete id */
#define PARAM_ID_MIN_CONFIDENCE_LEVELS  0x00012C07
/* Remapping macro. */
#define LSM_PARAM_ID_MIN_CONFIDENCE_LEVELS PARAM_ID_MIN_CONFIDENCE_LEVELS

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_min_confidence_levels
               lsm_param_id_min_confidence_levels_t;

/** @h2xmlp_parameter   {"PARAM_ID_MIN_CONFIDENCE_LEVELS",
                         PARAM_ID_MIN_CONFIDENCE_LEVELS}
    @h2xmlp_description {Sets the minimum confidence levels for registered
                         keyword models and active user models.}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */

#include "adsp_begin_pack.h"
struct lsm_param_id_min_confidence_levels
{
  uint8_t num_active_models;
  /**< @h2xmle_description {Sum of all keyword models plus the active user
                            models.}
       @h2xmle_range       {1..20}
       @h2xmle_default     {1} */

  uint8_t confidence_levels[0];
  /**< @h2xmle_description {Array of confidence levels for each keyword and
                            active user pair. \n
                            \n
                            This array is of size num_active_models. Each
                            element is 1 byte long. \n
                            \n
                            Assuming N keywords and M active user models are
                            included in the model, the first N elements of
                            this parameter correspond to the confidence of N
                            keywords. The N+1 to N+M elements control the
                            confidence level of the M active user model. Each
                            element maps confidence values for entries (keyword
                            or user specific keyword) whose order is specified
                            in the sound model.}
       @h2xmle_variableArraySize {num_active_models} */
}
#include "adsp_end_pack.h"
;

#endif /* _LISTEN_COMMON_PARAMS_H_ */
