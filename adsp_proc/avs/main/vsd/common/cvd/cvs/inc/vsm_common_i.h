#ifndef _VSM_COMMON_I_H_
#define _VSM_COMMON_I_H_
/*
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/inc/vsm_common_i.h#2 $
  $Author: pwbldsvc $
*/

/* This file mainly contains the function definitions of utility and action routines used by VSM.
*/

#include "Elite.h"
#include "qurt_elite.h"
#include "vsm_header_i.h"

/******************************Utilities **************
*******************************************************/

bool_t VSM_utility_ssr_matches_passive_clients_domain(
  uint8_t domain_id,
  vsm_session_object_t* session_object
);

ADSPResult VSM_utility_remove_passive_client_info(
  uint16_t client_addr,
  uint16_t client_port,
  vsm_session_object_t* session_object
);

void VSM_utility_update_passive_client_info(
  elite_apr_packet_t* apr_pkt,
  vsm_session_object_t* session_object
);

bool_t VSM_utility_if_passive_client_already_exists(
  elite_apr_packet_t* apr_pkt,
  vsm_session_object_t* session_object
);

int32_t vsm_utility_determine_closest_supported_pp_sr (
  uint32_t enc_dec_sr,
  uint32_t* ret_pp_sr
);

int32_t vsm_utility_evs_sanitize_sampling_rates (
  uint32_t requested_rx_sampling_rate,
  uint32_t requested_tx_sampling_rate,
  uint32_t* ret_dec_sampling_rate,
  uint32_t* ret_enc_sampling_rate
);

bool_t vsm_utility_var_voc_sr_is_valid (
  uint32_t sr
);

int32_t vsm_utility_determine_target_sampling_rates (
  uint32_t media_id,
  uint32_t requested_var_voc_rx_sampling_rate,
  uint32_t requested_var_voc_tx_sampling_rate,
  uint32_t* ret_dec_sr,
  uint32_t* ret_enc_sr,
  uint32_t* ret_rx_pp_sr,
  uint32_t* ret_tx_pp_sr
);

bool_t vsm_utility_verify_param_data_size_aligned (
  uint32_t param_size
);

ADSPResult vsm_utility_playback_control_stop( vsm_session_object_t* session_object );

ADSPResult vsm_utility_playback_control_start( vsm_session_object_t* session_object );

ADSPResult vsm_utility_record_control_stop( vsm_session_object_t* session_object );

ADSPResult vsm_utility_record_control_start( vsm_session_object_t* session_object );

bool_t vsm_utility_evs_validate_enc_channel_aware_mode(
  uint8_t fec_offset,
  uint8_t fer_rate
);

bool_t vsm_utility_evs_validate_enc_operating_mode (
  uint32_t mode,
  uint32_t bandwidth,
  uint32_t requested_var_voc_tx_sampling_rate
);

bool_t vsm_utility_validate_apr_payload_size( elite_apr_packet_t* apr_pkt );

void vsm_utility_cache_enc_operating_mode (
  vsm_session_object_t* session_object,
  void* payload
);

void vsm_utility_cache_channel_aware_mode (
  vsm_session_object_t* session_object,
  void* payload
);

void vsm_utility_cache_enc_rate ( 
  vsm_session_object_t* session_object, 
  void* payload
);

void vsm_utility_cache_minmax_rate (
  vsm_session_object_t* session_object, 
  void* payload
);

void vsm_utility_cache_rate_modulation (
  vsm_session_object_t* session_object,
  void* payload
);

void vsm_utility_cache_dtx_mode (
  vsm_session_object_t* session_object,
  bool_t is_full_ctrl,
  void* payload
);

void vsm_utility_get_cached_dtx_mode (
  vsm_session_object_t* session_object,
  vsm_enc_set_dtx_mode_t* out_args
);

void vsm_utility_cache_voc_property (
  vsm_session_object_t* session_object, 
  void* payload,
  vsm_voc_property_enum_t property
);

void vsm_utility_free_session_object(
  vsm_session_object_t** session_ptr
);

bool_t vsm_utility_is_voc_op_mode_event_received( vsm_session_object_t* session_object );

void vsm_utility_allocate_timing_payload(
  vsm_session_object_t* session_object,
  int32_t** ret_ptr
);

ADSPResult vsm_utility_mailbox_timer_get_time (
  uint64_t* ret_time_us
);

bool_t vsm_utility_media_id_is_nb_sr (
  uint32_t media_id
);

int32_t vsm_utility_determine_target_rx_pp_sr_from_feature (
  uint32_t feature_id,
  uint32_t media_id
);

bool_t vsm_utility_vocoder_is_variable (
  uint32_t media_id /* Assumes the media_id is valid. */
);

bool_t vsm_utility_media_id_is_valid (
  uint32_t media_id
);
 
void vsm_utility_destroy_service(
  vsm_session_object_t* session_object,
  elite_svc_handle_t* svc_handle_ptr
);

void vsm_utility_initiate_fullcontrol_client_info(
  vsm_session_object_t* session_obj,
  vss_istream_cmd_create_full_control_session_t* payload
);

void vsm_utility_initialize_session_variables ( 
  vsm_session_object_t* session_obj
);

void vsm_utility_reset_cached_vocoder_properties ( 
  vsm_session_object_t* session_obj
);

bool_t vsm_utility_is_hdvoice_feature_supported(
   uint32_t feature_id
);

bool_t vsm_is_calibration_required (
  vsm_session_object_t* session_obj
);

uint32_t vsm_utility_validate_and_update_ui_prop_version(
  uint32_t* cached_version,
  uint32_t  new_version
);

ADSPResult vsm_utility_log_vocoder_packet (
  vsm_session_object_t* session_object,
  elite_apr_packet_t* packet
);

ADSPResult vsm_utility_log_enc_packet(
  vsm_session_object_t* session_object,
  elite_apr_packet_t* packet
);

ADSPResult vsm_utility_log_dec_packet(
  vsm_session_object_t* session_object,
  elite_apr_packet_t* packet
);

ADSPResult vsm_utility_get_max_mailbox_pkt_size (
  uint32_t media_id,
  uint32_t* ret_max_enc_pkt_size,
  uint32_t* ret_max_dec_pkt_size
);

bool vsm_utility_is_mailbox_voc_req_pending(
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* circ_buf,
  uint32_t circ_buf_mem_size,
  uint32_t voc_req_unit_size
);

ADSPResult vsm_utility_get_mailbox_enc_buffer(
  vsm_session_object_t* session_obj,
  vss_ipktexg_mailbox_enc_request_t* enc_req,
  vss_ipktexg_mailbox_enc_packet_t** ret_enc_buffer
);

ADSPResult vsm_utility_get_mailbox_dec_buffer(
  vsm_session_object_t* session_obj,
  vss_ipktexg_mailbox_dec_request_t* dec_req,
  vss_ipktexg_mailbox_dec_packet_t** ret_dec_buffer
);

ADSPResult vsm_utility_complete_pending_voc_request(
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* circ_buf,
  uint32_t circ_buf_mem_size,
  uint32_t voc_req_unit_size
);

ADSPResult vsm_action_send_dtmf_gen_msg_to_afe(
  vsm_session_object_t* session_obj,
  uint8_t * mailbox_dec_buf_ptr,
  int32_t mailbox_dec_buf_size,
  int8_t mailbox_buffer_index
);

ADSPResult vsm_utility_log_dtmf_mailbox_pkt_gen_no_data_pkt(
  vsm_session_object_t* session_obj,
  vss_ipktexg_mailbox_dec_packet_t* mailbox_dec_packet,
  uint32_t apr_packet_token,
  elite_apr_packet_t** dec_packet
);

/*****************************************************
 ******************VSM ACTIONS************************
 *****************************************************/

ADSPResult vsm_action_cleanup_ssr_domain_passive_clients(
  uint8_t domain_id,
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_update_cached_system_sampling_rates(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_tty_mode(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_oobtty_mode(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_update_tty_mode(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_destroy_vocproc(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_stop_vocproc(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_start_vocproc(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_timing_to_vocproc(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_create_vocproc(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_playback_set_system_config(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_stop_record(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_start_record(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_disconnect_afe(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_connect_afe(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_encoder_reset(
  vsm_session_object_t* session_object
);


ADSPResult vsm_action_set_voc_param(
  vsm_session_object_t* session_object,
  vss_istream_voc_param_data_t* payload_ptr
);

ADSPResult vsm_action_set_param(
  vsm_session_object_t* session_object,
  int32_t* get_param_ptr,
  uint32_t payload_size,
  uint32_t version
);

ADSPResult vsm_action_get_param(
  vsm_session_object_t* session_object,
  int32_t* get_param_ptr,
  bool_t is_inband,
  uint32_t version
);

ADSPResult vsm_action_send_start_stop_host_pcm(
  vsm_session_object_t* session_object,
  elite_apr_packet_t* apr_pkt
);

ADSPResult vsm_action_set_oob_pkt_exchange_config(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_forward_command_to_venc(
  vsm_session_object_t* session_object,
  uint32_t opcode
);

ADSPResult vsm_action_set_enc_rate(
  vsm_session_object_t* session_object,
  uint32_t rate
);

void vsm_action_free_session_resources(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_clear_oob_configuration(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_reconfig_mailbox_timing( 
  vsm_session_object_t* session_object
);

void vsm_action_ssr_clear_event_class_registrations_oob_tty( 
  vsm_session_object_t* session_object, 
  uint8_t ssr_domain_id 
);

ADSPResult vsm_action_clear_mailbox_configuration(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_get_avsync_delays(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_voice_timing(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_get_kpps(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_run_pause_services(
  vsm_session_object_t* session_object,
  uint32_t opcode
);

ADSPResult vsm_action_run_stop_ports(
  vsm_session_object_t* session_object,
  uint32_t opcode
);

ADSPResult vsm_action_set_tx_rx_mute(
  vsm_session_object_t* session_object,
  uint16_t direction,
  uint16_t tx_ramp_duration,
  uint16_t rx_ramp_duration
);

ADSPResult vsm_action_set_ui_properties(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_hdvoice_enable_disable(
  vsm_session_object_t* session_object,
  bool_t is_enable
);

ADSPResult vsm_action_calibrate_static(
  vsm_session_object_t* session_object
);

void vsm_action_toggle_csm (
  vsm_session_object_t* session_obj,
  bool_t status
);

/* BACKWARD COMPATIBILITY */
ADSPResult vsm_action_calibrate_common(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_packet_exchange_mode(
  vsm_session_object_t* session_object,
  uint32_t mode
);

ADSPResult vsm_action_set_cached_minmax_rate (
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_cached_channel_aware_mode (
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_cached_enc_operating_mode (
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_cached_rate_modulation (
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_cached_dtx_mode(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_cached_voc_params (
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_cached_properties(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_reinit_stream(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_close_mixer_input_output(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_cleanup_create_errors(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_register_event(
  vsm_session_object_t* session_object,
  uint32_t event_id
);

ADSPResult vsm_action_update_mixer_port_mapping(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_detach_to_vocproc(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_mute_notification(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_ready_event(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_eamr_mode_event (
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_evs_bandwidth_event (
  vsm_session_object_t* session_object 
);

ADSPResult vsm_action_send_avsync_rx_delay_event(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_enable_notifications_to_clients(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_not_ready_event(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_disable_notifications_to_clients(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_reconfig_event(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_system_config_response(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_attach_to_vocproc(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_rx_dtmf_detect_config(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_media_type(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_connect_rx_downstream(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_set_pp_sampling_rate(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_reconfig_tx_mixer(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_reassign_mixer_inout_ports(
  vsm_session_object_t* session_object,
  bool_t is_attach
);

ADSPResult vsm_action_create_mixer_input_ouput(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_create_dynamic_services(
  vsm_session_object_t* session_object
);

ADSPResult vsm_action_send_avsync_tx_timestamp (
  vsm_session_object_t* session_obj,
  uint64_t timestamp_us
);

ADSPResult vsm_action_tx_inband_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
);

ADSPResult vsm_action_rx_inband_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
);

ADSPResult vsm_action_tx_oob_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
);

ADSPResult vsm_action_rx_oob_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
);

ADSPResult vsm_action_tx_mailbox_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
);

ADSPResult vsm_action_rx_mailbox_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
);

ADSPResult vsm_action_publish_evs_params (
  vsm_session_object_t* session_obj
);

#endif  //_VSM_COMMON_I_H_
