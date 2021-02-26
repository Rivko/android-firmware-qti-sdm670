#ifndef VOL_CTRL_CALIB_H
#define VOL_CTRL_CALIB_H
/*==============================================================================
  @file vol_ctrl_calib.h
  @brief This file contains VOL_CTRL API
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

/** @h2xml_title1           {Audio Volume Control}
    @h2xml_title_agile_rev  {Audio Volume Control}
    @h2xml_title_date       {February 9, 2017} */

#include  "mmdefs.h"
#include  "vol_ctrl_param_calib.h"

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Global unique Module ID definition
   Module library is independent of this number, it defined here for static
   loading purpose only */
/* ID of the Volume Control module pre/postprocessing block. */
#define AUDPROC_MODULE_ID_VOL_CTRL                                  0x00010BFE
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_VOL_CTRL",
                          AUDPROC_MODULE_ID_VOL_CTRL}
    @h2xmlm_displayName  {"AUDIO SOFT STEP VOLUME"}
    @h2xmlm_description  {ID of the volume control module.\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN\n
    - #AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG\n
    - #AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS\n
    - #AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS\n
    - #AUDPROC_PARAM_ID_MULTICHANNEL_GAIN\n
    - #AUDPROC_PARAM_ID_MULTICHANNEL_MUTE\n

    All parameter IDs are device independent.\n}

    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->
*/
/**

    @h2xml_Select					{audproc_volume_ctrl_master_gain_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_mute_config_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_soft_step_volume_params_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_soft_pause_params_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_channel_type_gain_pair_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_multichannel_gain_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_channel_type_mute_pair_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_multichannel_mute_t}
    @h2xmlm_InsertParameter
*/

/** @}                   <-- End of the Module --> */


#define AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2                0x00010910
/**
    @h2xmlm_module       {"AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2",
                          AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2}
    @h2xmlm_displayName  {"AUDIO SOFT STEP VOLUME INSTANCE_2"}
    @h2xmlm_description  {ID of the Volume Control module pre/postprocessing block, instance 2. Use this module together with
AUDPROC_MODULE_ID_VOL_CTRL to have two volume controls in a pre/postprocessing chain.\n
This module supports the same parameter IDs that AUDPROC_MODULE_ID_VOL_CTRL supports:\n

    This module supports the following parameter IDs:\n
    - #AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN\n
    - #AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG\n
    - #AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS\n
    - #AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS\n
    - #AUDPROC_PARAM_ID_MULTICHANNEL_GAIN\n
    - #AUDPROC_PARAM_ID_MULTICHANNEL_MUTE\n

    All parameter IDs are device independent.\n}
    @h2xmlm_toolPolicy              {Calibration; RTC}
    @{                   <-- Start of the Module -->


    @h2xml_Select					{audproc_volume_ctrl_master_gain_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_mute_config_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_soft_step_volume_params_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_soft_pause_params_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_channel_type_gain_pair_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_multichannel_gain_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_channel_type_mute_pair_t}
    @h2xmlm_InsertParameter

	@h2xml_Select					{audproc_volume_ctrl_multichannel_mute_t}
    @h2xmlm_InsertParameter


   @}                   <-- End of the Module -->*/


#endif




