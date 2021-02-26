/**========================================================================
 @file vpm_session_utils.h
 
 @brief This header file contains function declarations for vocproc session
 object configuration utilites
 
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_utils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_SESSION_UTILS_H_
#define _VPM_SESSION_UTILS_H_

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"
#include "adsp_vcmn_api.h"

/* -----------------------------------------------------------------------
** Function declarations
** ----------------------------------------------------------------------- */

/* Assumes the media_id is valid. */
static inline bool_t vpm_media_id_is_var_sr(uint32_t media_id)
{
   bool_t rc;

   switch (media_id)
   {
      case VSS_MEDIA_ID_EVS:
      {
         rc = TRUE;
         break;
      }
      default:
      {
         rc = FALSE;
         break;
      }
   }

   return rc;
}

ADSPResult vpm_check_topo_commit_state(vpm_session_object_t *session_obj);

static inline void vpm_set_cmd_rsp_pending_status(vpm_session_object_t *session_obj,
                                                  bool_t is_cmd_rsp_pending)
{
   /* Init the response control params */
   session_obj->session_rsp_ctrl.is_prev_cmd_rsp_pending = is_cmd_rsp_pending;

   session_obj->session_rsp_ctrl.status = ADSP_EOK;
   session_obj->session_rsp_ctrl.num_cmd_issued = 0;
   session_obj->session_rsp_ctrl.num_rsp_received = 0;
   session_obj->session_rsp_ctrl.num_rsp_failed = 0;
   session_obj->session_rsp_ctrl.p_ack_payload = NULL;
   session_obj->session_rsp_ctrl.is_ack_payload_type_apr = FALSE;
   session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = NULL;
   session_obj->session_rsp_ctrl.num_unsupported_ecode = 0;
}

static inline void vpm_create_next_goal_ctrl(vpm_session_object_t *session_obj,
                                             cvp_goal_enum_t next_goal)
{
   session_obj->state_ctrl.goal = next_goal;
   session_obj->state_ctrl.action = VPM_ACTION_ENUM_NONE;
   session_obj->state_ctrl.status = ADSP_EFAILED;
   session_obj->state_ctrl.is_goal_completed = FALSE;
}

static inline void vpm_complete_goal(vpm_session_object_t *session_obj,
                                     uint32_t goal_status)
{
   session_obj->state_ctrl.goal = CVP_GOAL_ENUM_NONE;
   session_obj->state_ctrl.action = VPM_ACTION_ENUM_NONE;
   session_obj->state_ctrl.status = goal_status;
   session_obj->state_ctrl.is_goal_completed = TRUE;
}

void vpm_set_session_cmd_pending_status(uint32_t vpm_session_idx, bool_t is_pending);

void vpm_check_goal_completion_status(vpm_session_object_t *session_obj);

void vpm_update_device_active_set(vpm_session_object_t *session_obj);

bool_t vpm_sr_is_valid(uint32_t sample_rate);

bool_t vpm_is_hdvoice_feature_supported(uint32_t feature_id);

bool_t vpm_media_id_is_nb_sr(uint32_t media_id);

ADSPResult vpm_get_mute_mode(vpm_session_object_t *session_obj,
                             voice_mute_path_t *mute_mode);

ADSPResult vpm_construct_session(vpm_session_object_t **ret_session_obj,
                                 elite_msg_any_t *p_msg_pkt);

ADSPResult vpm_extract_topology_params(vpm_session_object_t *session_obj,
                                       bool_t error_check_only_flag);

bool_t vpm_is_dev_channel_info_modified(vpm_session_object_t *session_obj);

bool_t vpm_is_reinit_required(vpm_session_object_t *session_obj);

bool_t vpm_is_calibration_required(vpm_session_object_t *session_obj);

ADSPResult vpm_destroy_session(vpm_session_object_t *session_obj);

ADSPResult vpm_get_free_session_idx(voice_proc_mgr_t *pMe,
                                    uint32_t *session_idx);


ADSPResult vpm_add_session(voice_proc_mgr_t *pMe,
                           vpm_session_object_t *session_obj,
                           uint32_t session_idx);

ADSPResult vpm_set_session_params(vpm_session_object_t *session_obj);


ADSPResult vpm_soundfocus_validate_angles(vss_isoundfocus_cmd_set_sectors_t *set_sectors);

ADSPResult vpm_create_session_static_info(voice_proc_mgr_t *pMe);

ADSPResult vpm_destroy_session_static_info(voice_proc_mgr_t *pMe);

ADSPResult vpm_create_full_session_cmd_ctrl(voice_proc_mgr_t *pMe);

ADSPResult vpm_create_full_session_v2_cmd_ctrl(voice_proc_mgr_t *pMe);

ADSPResult vpm_create_full_session_v3_cmd_ctrl(voice_proc_mgr_t *pMe);

ADSPResult vpm_topology_commit_cmd_ctrl(vpm_session_object_t *session_obj);

ADSPResult vpm_destroy_session_cmd_ctrl(vpm_session_object_t *session_obj);

ADSPResult vpm_enable_cmd_ctrl(vpm_session_object_t *session_obj,
                               bool_t modem_enable);

ADSPResult vpm_disable_cmd_ctrl(vpm_session_object_t *session_obj,
                                bool_t modem_disable);

ADSPResult vpm_reinit_cmd_ctrl(vpm_session_object_t *session_obj);

uint32_t vpm_get_sampling_rate(uint32_t media_type_format,
                               uint32_t feature_id,
                               uint16_t evs_sampling_rate);
                               
ADSPResult vpm_parse_generic_module_id_params_and_set_param(vpm_session_object_t *session_obj,
                                                void *payload_address,
                                                uint32_t payload_size,
                                                bool_t *is_generic_module_params);
                                                
ADSPResult vpm_parse_generic_module_id_params_and_set_param_v2(vpm_session_object_t *session_obj,
                                                void *payload_address,
                                                uint32_t payload_size,
                                                bool_t *is_generic_module_params);
                                                
                                               
ADSPResult vpm_parse_generic_module_id_params_and_get_param(vpm_session_object_t *session_obj,
                                                void *payload_address,
                                                uint32_t payload_size,
                                                bool_t *is_generic_module_params);
ADSPResult vpm_parse_generic_module_id_params_and_get_param_v2(vpm_session_object_t *session_obj,
                                                void *payload_address,
                                                uint32_t payload_size,
                                                bool_t *is_generic_module_params);
#endif /* #ifndef _VPM_SESSION_UTILS_H_ */


