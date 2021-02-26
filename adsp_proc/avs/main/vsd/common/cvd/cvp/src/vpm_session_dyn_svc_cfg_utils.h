/**========================================================================
 @file vpm_session_dyn_svc_cfg_utils.h
 
 @brief This header file contains function declarations for vocproc session
 object device configuration utilities.
 
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_dyn_svc_cfg_utils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_SESSION_STATE_CTRL_UTILS_H_
#define _VPM_SESSION_STATE_CTRL_UTILS_H_

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"
#include "AFEInterface.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/


#define FARSRC 1

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

static inline bool_t is_pseudo_port_id(uint16_t port_id)
{
   return (IS_PSEUDO_PORT_AFE_ID(port_id));
}

ADSPResult vpm_create_tx_dyn_svc(vpm_session_object_t *p_session);

ADSPResult vpm_create_rx_dyn_svc(vpm_session_object_t *p_session);

ADSPResult vpm_connect_peer_svc(uint32_t sec_opcode, qurt_elite_queue_t *p_peer_svc_cmd_q,
                                qurt_elite_queue_t *p_vpm_svc_rsp_q, elite_svc_handle_t *h_src_svc);

ADSPResult vpm_configure_tx_mixer_input(vpm_session_object_t *p_session,
                                        uint32_t vmx_inport_config);

ADSPResult vpm_configure_rx_mixer_output(vpm_session_object_t *p_session,
                                         uint32_t vmx_outport_config,
                                         uint16_t sampling_rate);

ADSPResult vpm_connect_vptx_to_vmx_inp_port(qurt_elite_queue_t *p_vptx_cmd_q,
                                            qurt_elite_queue_t *p_vpm_svc_rsp_q,
                                            elite_svc_handle_t *h_vmx_inp_port);

ADSPResult vpm_close_tx_mixer_input(vpm_session_object_t *p_session);

ADSPResult vpm_close_rx_mixer_output(vpm_session_object_t *p_session);

ADSPResult vpm_destroy_dyn_svcs(elite_svc_handle_t *h_dyn_svc,
                                qurt_elite_queue_t *p_vpm_svc_rsp_q);

ADSPResult vpm_send_vfr_mode_to_dyn_svc(void *p_timing_param,
                                        qurt_elite_queue_t *p_vpm_svc_rsp_q,
                                        qurt_elite_queue_t *p_dstn_svc_cmd_q,
                                        uint32_t sec_opcode);

ADSPResult vpm_send_mute_cmd_to_dyn_svc(voice_set_soft_mute_v2_t *p_cmd_payload,
                                        qurt_elite_queue_t *p_session_rsp_q,
                                        qurt_elite_queue_t *p_dstn_svc_cmd_q);

ADSPResult vpm_connect_afe_tx(vpm_session_object_t *p_session);

ADSPResult vpm_connect_afe_far(vpm_session_object_t *p_session);

ADSPResult vpm_connect_afe_rx(vpm_session_object_t *p_session);

ADSPResult vpm_connect_aferx_to_vprx(vpm_session_object_t *p_session,
                                     uint32_t sec_opcode);

ADSPResult vpm_control_mixer_ports(vpm_session_object_t *p_session,
                                   uint32_t vmx_port_id,
                                   uint32_t vmx_direction,
                                   uint32_t sec_opcode,
                                   qurt_elite_queue_t *p_vmx_cmd_q);

ADSPResult vpm_send_run_msg_to_dyn_svc(vpm_session_object_t *p_session,
                                       uint32_t vpm_direction);


ADSPResult vpm_destroy_session_dyn_svc(vpm_session_object_t *p_session,
                                       bool_t destroy_vprx,
                                       bool_t destroy_vptx);

ADSPResult vpm_send_reconfig_msg_to_dyn_svc(vpm_session_object_t *p_session,
                                            uint32_t vpm_direction);

ADSPResult vpm_set_get_param_to_dyn_svc(vpm_session_object_t *session_obj,
                                           void *payload_address,
                                           uint32_t payload_size,
                                           uint32_t opcode,
                                           uint32_t version);

ADSPResult vpm_action_disable_vocproc(vpm_session_object_t *session_obj);

ADSPResult vpm_send_get_delay_cmd_to_dyn_svc(qurt_elite_queue_t *p_session_rsp_q,
                                             qurt_elite_queue_t *p_dstn_svc_cmd_q,
                                             void *cust_rsp_payload,
                                             uint32_t client_token);

ADSPResult vpm_set_param_v3_to_dyn_svc(vpm_session_object_t *session_obj,
                                       uint32_t cal_handle);

ADSPResult vpm_send_get_kpps_cmd_to_dyn_svc(vpm_session_object_t *session_obj);

ADSPResult vpm_send_vcpm_cfg_msg_to_dyn_svc(vpm_session_object_t *session_obj,
                                            uint32_t vpm_mem_handle,
                                            elite_apr_packet_t *p_apr_pkt);

ADSPResult vpm_action_set_mute(vpm_session_object_t *session_obj,
                               uint32_t mute_ramp_duration,
                               uint32_t vpm_direction);

ADSPResult vpm_reinit_dyn_svcs(vpm_session_object_t *session_obj,
                               bool_t is_forced_reinit);

ADSPResult vpm_send_hdvoice_enable_disable_msg(vpm_session_object_t *session_object,
                                               bool_t is_enable);

ADSPResult vpm_action_register_event(
  vpm_session_object_t *session_obj,
  uint32_t event_id
);

uint16_t vpm_get_ec_ref_port(vpm_session_object_t *p_session);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* #ifndef _VPM_SESSION_STATE_CTRL_UTILS_H_ */

