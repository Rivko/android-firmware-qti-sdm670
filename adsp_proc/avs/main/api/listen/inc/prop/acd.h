#ifndef _ACD_H_
#define _ACD_H_

/*==============================================================================
  @file acd.h
  @brief This file contains Public APIs for Audio Context Detection Module

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/prop/acd.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  07/01/17   sw         (Tech Pubs) Edited descriptions.
  03/17/17   unni   Created File
==============================================================================*/

/** @h2xml_title1          {Audio Context Detection Module}
    @h2xml_title_date      {March 17, 2017} */

#include "mmdefs.h"
#include "audio_common.h"
#include "detection_engine.h"
#include "listen_common_params.h"

/*------------------------------------------------------------------------------
   Module ID
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition.
   Module library is independent of this number, it defined here for static
   loading purpose only */
#define LSM_MODULE_ACD        0x00012C22
/* Remapping Macro */
#define MODULE_ID_ACD LSM_MODULE_ACD

/** @h2xmlm_module       {"MODULE_ID_ACD",MODULE_ID_ACD}
    @h2xmlm_toolPolicy   {Calibration;RTC}
    @h2xmlm_description  {Supports the Audio Context Detection feature.}
    @{                   <-- Start of the Module -->

    @h2xml_Select        {lsm_param_id_epd_threshold}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_operation_mode}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_gain}
    @h2xmlm_InsertParameter
    @h2xml_Select        {lsm_param_id_min_confidence_levels}
    @h2xmlm_InsertParameter
    @h2xml_Select        {"PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_REGISTER"}
    @h2xmlm_InsertParameter
    @h2xml_Select        {"PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_DEREGISTER"}
    @h2xmlm_InsertParameter

    @}                   <-- End of the Module -->*/
#endif /* _ACD_H_ */
