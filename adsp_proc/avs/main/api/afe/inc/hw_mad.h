#ifndef _AFE_HW_MAD_H_
#define _AFE_HW_MAD_H_

/*==============================================================================
  @file hw_mad.h
  @brief This file contains Public APIs for Hardware MAD (Mic-Activitiy 
         Detection) Module

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/hw_mad.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------  
  06/19/17    sw        (Tech Pubs) Edited descriptions
  05/12/17   unni       Created File
==============================================================================*/

/** @h2xml_title1           {Hardware MAD Module}
    @h2xml_title_date       {May 12, 2017} */

#include "mmdefs.h"
#include "afe_common.h"

/*------------------------------------------------------------------------------
   Module ID
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Module ID */
#define AFE_MODULE_HW_MAD   0x00010230

/** @h2xmlm_module      {"AFE_MODULE_HW_MAD",AFE_MODULE_HW_MAD}
    @h2xmlm_toolPolicy  {Calibration;RTC}
    @h2xmlm_description {Supports the Hardware Mic Activity Detection (MAD)
                         feature.}
    @h2xmlm_deprecated  {true}
    @{                   <-- Start of the Module --> */

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_HW_MAD_CFG   0x00010231

/* Minor Version of this PARAM */
#define AFE_API_VERSION_HW_MAD_CONFIG   0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_hw_mad_cfg_param_v1_t afe_hw_mad_cfg_param_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_HW_MAD_CFG", AFE_PARAM_ID_HW_MAD_CFG}
    @h2xmlp_description {Configures hardware MAD.}
    @h2xmlp_version     {AFE_API_VERSION_HW_MAD_CONFIG} */
#include "adsp_begin_pack.h"
struct afe_hw_mad_cfg_param_v1_t
{
  uint32_t hw_mad_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_HW_MAD_CONFIG}
       @h2xmle_default     {AFE_API_VERSION_HW_MAD_CONFIG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint16_t audio_sleep_time_ms;
  /**< @h2xmle_description {Sleep time for the MAD audio hardware (in
                            milliseconds).}
       @h2xmle_range       {0..60000}
       @h2xmle_default     {0} */

  uint16_t beacon_sleep_time_ms;
  /**< @h2xmle_description {Sleep time for the MAD beacon hardware (in
                            milliseconds).}
       @h2xmle_range       {0..60000}
       @h2xmle_default     {0} */

    uint16_t us_sleep_time_ms;
  /**< @h2xmle_description {Sleep time for the MAD ultrasound hardware (in
                            milliseconds).}
       @h2xmle_range       {0..60000}
       @h2xmle_default     {0} */

  int16_t reserved;
  /**< @h2xmle_description {Used for alignment; must be set to 0.}
       @h2xmle_readonly    {true}
       @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Parameter id */
#define AFE_PARAM_ID_HW_MAD_CTRL    0x00010232

/* Minor version of this PARAM */
#define AFE_API_VERSION_HW_MAD_CTRL   0x1

/* Bit to enable/disable hardware MAD audio */
#define AFE_HW_MAD_AUDIO_BIT    0x0

/* Bit to enable/disable MAD beacon */
#define AFE_HW_MAD_BEACON_BIT   0x1

/* Bit to enable/disable MAD ultrasound */
#define AFE_HW_MAD_ULT_BIT    0x2

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_hw_mad_ctrl_param_v1_t afe_hw_mad_ctrl_param_v1_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_HW_MAD_CTRL", AFE_PARAM_ID_HW_MAD_CTRL}
    @h2xmlp_description {Controls hardware MAD.}
    @h2xmlp_version     {AFE_API_VERSION_HW_MAD_CTRL}
    @h2xmlp_toolPolicy  {NO_SUPPORT} */
#include "adsp_begin_pack.h"
struct afe_hw_mad_ctrl_param_v1_t
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_HW_MAD_CTRL}
       @h2xmle_default     {AFE_API_VERSION_HW_MAD_CTRL}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint16_t mad_type;
  /**< @h2xmle_description {Type of MAD hardware block.}
       @h2xmle_range       {0..0x7}
       
       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {"Audio"}
       @h2xmle_description {Enables hardware MAD audio.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {"Beacon"}
       @h2xmle_description {Enables the hardware MAD beacon.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000004}
       @h2xmle_bitName     {"Ultrasound"}
       @h2xmle_description {Enables hardware MAD ultrasound.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0}
       @h2xmle_bitFieldEnd */

  uint16_t mad_enable;
  /**< @h2xmle_description {Enables hardware MAD paths for audio,
                            ultrasound, and beacon.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {0} */

}
#include "adsp_end_pack.h"
;

/** @} <-- End of the Module --> */

#endif /* _AFE_HW_MAD_H_ */

