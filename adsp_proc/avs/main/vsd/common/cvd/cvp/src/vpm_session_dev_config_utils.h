/**========================================================================
 @file vpm_session_dev_config_utils.h
 
 @brief This header file contains function declarations for vocproc session
 object device configuration utilities.
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_dev_config_utils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_SESSION_DEV_CFG_UTILS_H_
#define _VPM_SESSION_DEV_CFG_UTILS_H_

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"

/* -----------------------------------------------------------------------
** Function declarations
** ----------------------------------------------------------------------- */


ADSPResult vpm_parse_device_config_wv_mod(vpm_session_object_t *session_obj,
                                          vpm_device_config_entry_t *device_config_entry,
                                          uint32_t *temp_wv_sr);

ADSPResult vpm_parse_device_config_feature_mod(vpm_session_object_t *session_obj,
                                               vpm_device_config_entry_t *device_config_entry);

ADSPResult vpm_parse_sound_device_mod(vpm_session_object_t *session_obj,
                                      vpm_device_config_entry_t *sound_device_mod);

ADSPResult vpm_parse_clk_ctrl_mod(vpm_session_object_t* session_obj,
                                  vpm_device_config_entry_t* clk_ctrl_mod);

ADSPResult vpm_post_parse_device_config (vpm_session_object_t* session_obj,
                                         uint32_t temp_wv_sr);

ADSPResult vpm_clear_feature_config(vpm_session_object_t *session_obj);

ADSPResult vpm_get_clock_scale_factors( vpm_session_object_t *session_obj,
                                        vss_icommon_rsp_set_system_config_t *set_system_config_rsp );

ADSPResult vpm_clear_sound_device_info(vpm_session_object_t *session_obj);

ADSPResult vpm_clear_clock_control_info(vpm_session_object_t *session_obj);


ADSPResult vpm_register_device_config_cmd_ctrl(vpm_session_object_t *session_obj);

ADSPResult vpm_deregister_device_config_cmd_ctrl(vpm_session_object_t *session_obj);

#endif /* #ifndef _VPM_SESSION_DEV_CFG_UTILS_H_ */

