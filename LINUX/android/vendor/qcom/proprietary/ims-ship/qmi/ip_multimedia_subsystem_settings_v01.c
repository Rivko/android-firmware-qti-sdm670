/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        I P _ M U L T I M E D I A _ S U B S Y S T E M _ S E T T I N G S _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the imss service Data structures.

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc. 
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //source/qcom/qct/interfaces/qmi/rel/deploy/imss/src/ip_multimedia_subsystem_settings_v01.c#16 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Wed Jun 14 2017 (Spin 0)
   From IDL File: ip_multimedia_subsystem_settings_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "common_v01.h"
#include "ip_multimedia_subsystem_ims_common_v01.h"


/*Type Definitions*/
static const uint8_t ims_settings_pol_man_rat_apn_info_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v01, rat),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v01, apn_type_apn_index),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v01, service_mask),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v01, auth_type_security_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v01, ip_type_info),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_settings_pol_mgr_rat_apn_fb_sp_info_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_rat_apn_fb_sp_info_v01, pol_mgr_rat_apn_fallback),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_rat_apn_fb_sp_info_v01, pol_mgr_service_priority_wwan),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_settings_pol_mgr_apn_name_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_apn_name_v01, pol_mgr_apn_name),
  IMS_SETTINGS_POL_MGR_APN_NAME_STR_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_settings_qdj_optimization2_info_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qdj_optimization2_info_v01, qdj_optimization2_enabled),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qdj_optimization2_info_v01, qdj_go_through_threshold),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qdj_optimization2_info_v01, qdj_drop_threshold),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_settings_vt_h264_info_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_vt_h264_info_v01, dynamic_pt_mode0),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_vt_h264_info_v01, dynamic_pt_mode1),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_settings_pol_mgr_rat_ipsec_info_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_rat_ipsec_info_v01, ipsec_enable_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_rat_ipsec_info_v01, ipsec_enable),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_settings_csd_codec_info_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_csd_codec_info_v01, codec_info),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_csd_codec_info_v01, codec_profile_number),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_csd_codec_info_v01, pay_load_number),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_settings_pol_man_rat_apn_info_v02_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v02_v01, rat),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v02_v01, apn_type_apn_index),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v02_v01, auth_type_security_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v02_v01, ip_type_info),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_man_rat_apn_info_v02_v01, service_mask_extend),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_settings_rtt_optional_data_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rtt_optional_data_v01, total_message_size),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rtt_optional_data_v01, current_message_sequence_number),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rtt_optional_data_v01, highest_message_sequence_number),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t ims_settings_set_sip_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, sip_local_port) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, sip_local_port_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, sip_local_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_sip_reg) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_sip_reg_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_sip_reg),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, subscribe_timer) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, subscribe_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, subscribe_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_t1) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_t1_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_t1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_t2) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_t2_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_t2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tf) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tf_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tf),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, sigcomp_enabled) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, sigcomp_enabled_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, sigcomp_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tj) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tj_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tj),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tj_ext) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tj_ext_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tj_ext),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, keepalive_enabled) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, keepalive_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, keepalive_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, nat_rto_timer) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, nat_rto_timer_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, nat_rto_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, sip_timer_operator_mode_a) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, sip_timer_operator_mode_a_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, sip_timer_operator_mode_a),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tb_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tb_value_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tb_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, gruu_enabled) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, gruu_enabled_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, gruu_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, transport_switch_enabled) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, transport_switch_enabled_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, transport_switch_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, tcp_max_backoff_timer_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, tcp_max_backoff_timer_value_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, tcp_max_backoff_timer_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, gzip_decoding_outbuffer_multiplier) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, gzip_decoding_outbuffer_multiplier_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, gzip_decoding_outbuffer_multiplier),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_td_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_td_value_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_td_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_t4) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_t4_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_t4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_ta_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_ta_value_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_ta_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_te_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_te_value_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_te_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tg_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tg_value_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tg_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_th_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_th_value_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_th_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_ti_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_ti_value_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_ti_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tk_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tk_value_valid)),
  0x28,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tk_value),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tc_value) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tc_value_valid)),
  0x29,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_req_msg_v01, timer_tc_value)
};

static const uint8_t ims_settings_set_sip_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_sip_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_sip_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_reg_mgr_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_req_msg_v01, regmgr_config_pcscf_port) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_req_msg_v01, regmgr_config_pcscf_port_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_req_msg_v01, regmgr_config_pcscf_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_req_msg_v01, regmgr_primary_cscf) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_req_msg_v01, regmgr_primary_cscf_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_req_msg_v01, regmgr_primary_cscf),
  IMS_SETTINGS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_reg_mgr_config_req_msg_v01, ims_test_mode_enabled) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_reg_mgr_config_req_msg_v01, ims_test_mode_enabled_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_reg_mgr_config_req_msg_v01, ims_test_mode_enabled)
};

static const uint8_t ims_settings_set_reg_mgr_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_sms_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sms_config_req_msg_v01, sms_format) - QMI_IDL_OFFSET8(ims_settings_set_sms_config_req_msg_v01, sms_format_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_sms_config_req_msg_v01, sms_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sms_config_req_msg_v01, sms_over_ip_network_indication) - QMI_IDL_OFFSET8(ims_settings_set_sms_config_req_msg_v01, sms_over_ip_network_indication_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sms_config_req_msg_v01, sms_over_ip_network_indication),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sms_config_req_msg_v01, phone_context_uri) - QMI_IDL_OFFSET8(ims_settings_set_sms_config_req_msg_v01, phone_context_uri_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_sms_config_req_msg_v01, phone_context_uri),
  IMS_SETTINGS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_sms_config_req_msg_v01, sms_psi) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_sms_config_req_msg_v01, sms_psi_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_sms_config_req_msg_v01, sms_psi),
  IMS_SETTINGS_SMS_PSI_LEN_V01
};

static const uint8_t ims_settings_set_sms_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_sms_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sms_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_sms_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_sms_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_user_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_user_config_req_msg_v01, ims_domain) - QMI_IDL_OFFSET8(ims_settings_set_user_config_req_msg_v01, ims_domain_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_user_config_req_msg_v01, ims_domain),
  IMS_SETTINGS_STRING_LEN_MAX_V01
};

static const uint8_t ims_settings_set_user_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_user_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_user_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_user_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_user_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_voip_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, session_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, session_expiry_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, session_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, min_session_expiry) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, min_session_expiry_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, min_session_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_enable) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_enable_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, scr_amr_enable) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, scr_amr_enable_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, scr_amr_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, scr_amr_wb_enable) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, scr_amr_wb_enable_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, scr_amr_wb_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_mode) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_mode_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_mode) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_mode_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_octet_align) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_octet_align_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_octet_align),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_octet_align) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_octet_align_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_octet_align),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, ringing_timer) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, ringing_timer_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, ringing_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, ringback_timer) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, ringback_timer_valid)),
  0x1A,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, ringback_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, rtp_rtcp_inactivity_timer) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, rtp_rtcp_inactivity_timer_valid)),
  0x1B,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, rtp_rtcp_inactivity_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_mode_str) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_mode_str_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_mode_str),
  IMS_SETTINGS_VOIP_AMR_MODE_STR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_mode_str) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_mode_str_valid)),
  0x1D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, amr_wb_mode_str),
  IMS_SETTINGS_VOIP_AMR_WB_MODE_STR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, voip_silent_redial_enabled) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, voip_silent_redial_enabled_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, voip_silent_redial_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, voip_preferred_rtp_payload_type) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, voip_preferred_rtp_payload_type_valid)),
  0x1F,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, voip_preferred_rtp_payload_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, voip_config_confURI) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, voip_config_confURI_valid)),
  0x20,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_req_msg_v01, voip_config_confURI),
  IMS_SETTINGS_VOIP_CONFIG_CONF_URI_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voip_config_req_msg_v01, e911_silent_redial_enabled) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voip_config_req_msg_v01, e911_silent_redial_enabled_valid)),
  0x21,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_voip_config_req_msg_v01, e911_silent_redial_enabled)
};

static const uint8_t ims_settings_set_voip_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voip_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_voip_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_voip_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_presence_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, publish_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, publish_expiry_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, publish_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, publish_extended_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, publish_extended_expiry_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, publish_extended_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, minimum_publish_interval) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, minimum_publish_interval_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, minimum_publish_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capability_poll_list_subscription_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capability_poll_list_subscription_expiry_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capability_poll_list_subscription_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capability_discovery_enable) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capability_discovery_enable_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capability_discovery_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capabilites_cache_expiration) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capabilites_cache_expiration_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capabilites_cache_expiration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, availability_cache_expiration) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, availability_cache_expiration_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, availability_cache_expiration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capability_poll_interval) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capability_poll_interval_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, capability_poll_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, max_subcription_list_entries) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, max_subcription_list_entries_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, max_subcription_list_entries),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, volte_user_opted_in_status) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, volte_user_opted_in_status_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, volte_user_opted_in_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, last_publish_etag) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, last_publish_etag_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, last_publish_etag),
  IMS_SETTINGS_PRESENCE_PUBLISH_ETAG_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, last_published_time) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, last_published_time_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_req_msg_v01, last_published_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_presence_config_req_msg_v01, last_negotiated_published_expire) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_presence_config_req_msg_v01, last_negotiated_published_expire_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_presence_config_req_msg_v01, last_negotiated_published_expire),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_presence_config_req_msg_v01, gzip_enabled) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_presence_config_req_msg_v01, gzip_enabled_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_presence_config_req_msg_v01, gzip_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_presence_config_req_msg_v01, presence_notify_wait_duration) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_presence_config_req_msg_v01, presence_notify_wait_duration_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_presence_config_req_msg_v01, presence_notify_wait_duration),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_presence_config_req_msg_v01, publish_error_recovery_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_presence_config_req_msg_v01, publish_error_recovery_timer_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_presence_config_req_msg_v01, publish_error_recovery_timer)
};

static const uint8_t ims_settings_set_presence_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_presence_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_presence_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_presence_ext_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_req_msg_v01, publish_error_recovery_timer) - QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_req_msg_v01, publish_error_recovery_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_req_msg_v01, publish_error_recovery_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_req_msg_v01, publish_user_agent) - QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_req_msg_v01, publish_user_agent_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_req_msg_v01, publish_user_agent),
  IMS_SETTINGS_PRESENCE_USERAGENT_LEN_V01
};

static const uint8_t ims_settings_set_presence_ext_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_presence_ext_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_media_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, h264_profile) - QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, h264_profile_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, h264_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, h264_level) - QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, h264_level_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, h264_level),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_bitrate) - QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_bitrate_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_bitrate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_frames_per_second) - QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_frames_per_second_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_frames_per_second),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_resolution) - QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_resolution_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_resolution),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_codec) - QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_codec_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_media_config_req_msg_v01, video_codec)
};

static const uint8_t ims_settings_set_media_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_media_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_media_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_media_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_media_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_qipcall_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, vt_calling_enabled) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, vt_calling_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, vt_calling_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, mobile_data_enabled) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, mobile_data_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, mobile_data_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, volte_enabled) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, volte_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, volte_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, emerg_call_timer) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, emerg_call_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, emerg_call_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, vt_quality_selector) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, vt_quality_selector_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, vt_quality_selector),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, speech_start_port) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, speech_start_port_valid)),
  0x15,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, speech_start_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, speech_end_port) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, speech_end_port_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, speech_end_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_wb_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_wb_octet_aligned_dynamic_pt_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_wb_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_octet_aligned_dynamic_pt_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_bandwidth_efficient_dynamic_pt_valid)),
  0x1A,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, dtmf_wb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, dtmf_wb_dynamic_pt_valid)),
  0x1B,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, dtmf_wb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, dtmf_nb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, dtmf_nb_dynamic_pt_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, dtmf_nb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_default_mode) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_default_mode_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, amr_default_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, video_rtp_port_start) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, video_rtp_port_start_valid)),
  0x1E,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, video_rtp_port_start),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, video_rtp_port_end) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, video_rtp_port_end_valid)),
  0x1F,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, video_rtp_port_end),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, emerg_call_timer_wlan) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, emerg_call_timer_wlan_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_req_msg_v01, emerg_call_timer_wlan)
};

static const uint8_t ims_settings_set_qipcall_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_reg_mgr_extended_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, reregistration_delay) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, reregistration_delay_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, reregistration_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, t_delay) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, t_delay_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, t_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, reg_retry_base_time) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, reg_retry_base_time_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, reg_retry_base_time),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, reg_retry_max_time) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, reg_retry_max_time_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_req_msg_v01, reg_retry_max_time)
};

static const uint8_t ims_settings_set_reg_mgr_extended_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_extended_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_pol_mgr_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_rat_apn_info) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_rat_apn_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_rat_apn_info),
  IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01,
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_rat_apn_fb_sp_info) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_rat_apn_fb_sp_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_rat_apn_fb_sp_info),
  IMS_SETTINGS_POL_MGR_RAT_APN_FB_SIZE_V01,
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_allowed_services_wlan) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_allowed_services_wlan_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_allowed_services_wlan),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_add_all_fts) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_add_all_fts_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_add_all_fts),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_acs_priority) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_acs_priority_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_acs_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_isim_priority) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_isim_priority_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_isim_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_nv_priority) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_nv_priority_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_nv_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_pco_priority) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_pco_priority_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_pco_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_ims_service_status) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_ims_service_status_valid)),
  0x18,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_ims_service_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_apn_name) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_apn_name_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_req_msg_v01, pol_mgr_apn_name),
  IMS_SETTINGS_POL_MGR_APN_SIZE_V01,
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t ims_settings_set_pol_mgr_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_rcs_sm_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_req_msg_v01, standalone_message_auth) - QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_req_msg_v01, standalone_message_auth_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_req_msg_v01, standalone_message_auth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_req_msg_v01, standalone_message_max_size) - QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_req_msg_v01, standalone_message_max_size_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_req_msg_v01, standalone_message_max_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_req_msg_v01, standalone_message_explorer_uri) - QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_req_msg_v01, standalone_message_explorer_uri_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_req_msg_v01, standalone_message_explorer_uri),
  IMS_SETTINGS_RCS_SM_EXPLORER_URI_LEN_V01
};

static const uint8_t ims_settings_set_rcs_sm_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_rcs_sm_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_ut_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, disable_ut) - QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, disable_ut_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, disable_ut),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_apn_name) - QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_apn_name_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_apn_name),
  IMS_SETTINGS_UT_APN_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_ip_addr_type) - QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_ip_addr_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_ip_addr_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_apn_name_apn2) - QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_apn_name_apn2_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_apn_name_apn2),
  IMS_SETTINGS_UT_APN_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_ip_addr_type_apn2) - QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_ip_addr_type_apn2_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ut_config_req_msg_v01, ut_ip_addr_type_apn2)
};

static const uint8_t ims_settings_set_ut_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_ut_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ut_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_ut_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_ut_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_client_provisioning_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_client_provisioning) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_client_provisioning_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_client_provisioning),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_volte) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_volte_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_volte),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_vt) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_vt_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_vt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_presence) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_presence_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_presence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_preference) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_preference_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_preference),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_roaming) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_roaming_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_roaming),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_vowifi) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_vowifi_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, enable_vowifi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, vowifi_provId) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, vowifi_provId_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, vowifi_provId),
  IMS_SETTINGS_VOIP_VOWIFI_MDN_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_roam_preference) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_roam_preference_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_req_msg_v01, wifi_call_roam_preference)
};

static const uint8_t ims_settings_set_client_provisioning_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_client_provisioning_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_set_apcs_complete_config_req_msg is empty
 * static const uint8_t ims_settings_set_apcs_complete_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_set_apcs_complete_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_apcs_complete_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_apcs_complete_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_apcs_complete_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_apcs_complete_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_sip_config_req_msg is empty
 * static const uint8_t ims_settings_get_sip_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_sip_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, sip_local_port) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, sip_local_port_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, sip_local_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_sip_reg) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_sip_reg_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_sip_reg),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, subscribe_timer) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, subscribe_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, subscribe_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_t1) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_t1_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_t1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_t2) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_t2_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_t2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tf) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tf_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tf),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, sigcomp_enabled) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, sigcomp_enabled_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, sigcomp_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tj) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tj_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tj),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tj_ext) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tj_ext_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tj_ext),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, keepalive_enabled) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, keepalive_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, keepalive_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, nat_rto_timer) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, nat_rto_timer_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, nat_rto_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, sip_timer_operator_mode_a) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, sip_timer_operator_mode_a_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, sip_timer_operator_mode_a),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tb_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tb_value_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tb_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, gruu_enabled) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, gruu_enabled_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, gruu_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, transport_switch_enabled) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, transport_switch_enabled_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, transport_switch_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, tcp_max_backoff_timer_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, tcp_max_backoff_timer_value_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, tcp_max_backoff_timer_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, gzip_decoding_outbuffer_multiplier) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, gzip_decoding_outbuffer_multiplier_valid)),
  0x21,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, gzip_decoding_outbuffer_multiplier),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_td_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_td_value_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_td_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_t4) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_t4_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_t4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_ta_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_ta_value_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_ta_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_te_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_te_value_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_te_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tg_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tg_value_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tg_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_th_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_th_value_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_th_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_ti_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_ti_value_valid)),
  0x28,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_ti_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tk_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tk_value_valid)),
  0x29,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tk_value),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tc_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tc_value_valid)),
  0x2A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_config_rsp_msg_v01, timer_tc_value)
};

/*
 * ims_settings_get_reg_mgr_config_req_msg is empty
 * static const uint8_t ims_settings_get_reg_mgr_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_reg_mgr_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, regmgr_config_pcscf_port) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, regmgr_config_pcscf_port_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, regmgr_config_pcscf_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, regmgr_primary_cscf) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, regmgr_primary_cscf_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_config_rsp_msg_v01, regmgr_primary_cscf),
  IMS_SETTINGS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_reg_mgr_config_rsp_msg_v01, ims_test_mode) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_reg_mgr_config_rsp_msg_v01, ims_test_mode_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_reg_mgr_config_rsp_msg_v01, ims_test_mode)
};

/*
 * ims_settings_get_sms_config_req_msg is empty
 * static const uint8_t ims_settings_get_sms_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_sms_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, sms_format) - QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, sms_format_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, sms_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, sms_over_ip_network_indication) - QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, sms_over_ip_network_indication_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, sms_over_ip_network_indication),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, phone_context_uri) - QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, phone_context_uri_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_sms_config_rsp_msg_v01, phone_context_uri),
  IMS_SETTINGS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_sms_config_rsp_msg_v01, sms_psi) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_sms_config_rsp_msg_v01, sms_psi_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_sms_config_rsp_msg_v01, sms_psi),
  IMS_SETTINGS_SMS_PSI_LEN_V01
};

/*
 * ims_settings_get_user_config_req_msg is empty
 * static const uint8_t ims_settings_get_user_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_user_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_user_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_user_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_user_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_user_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_user_config_rsp_msg_v01, ims_domain) - QMI_IDL_OFFSET8(ims_settings_get_user_config_rsp_msg_v01, ims_domain_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_user_config_rsp_msg_v01, ims_domain),
  IMS_SETTINGS_STRING_LEN_MAX_V01
};

/*
 * ims_settings_get_voip_config_req_msg is empty
 * static const uint8_t ims_settings_get_voip_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_voip_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, session_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, session_expiry_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, session_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, min_session_expiry) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, min_session_expiry_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, min_session_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_wb_enable) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_wb_enable_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_wb_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, scr_amr_enable) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, scr_amr_enable_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, scr_amr_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, scr_amr_wb_enable) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, scr_amr_wb_enable_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, scr_amr_wb_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_mode) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_mode_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_wb_mode) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_wb_mode_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_wb_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_octet_align) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_octet_align_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_octet_align),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_wb_octet_align) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_wb_octet_align_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, amr_wb_octet_align),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, ringing_timer) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, ringing_timer_valid)),
  0x1A,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, ringing_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, ringback_timer) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, ringback_timer_valid)),
  0x1B,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, ringback_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, rtp_rtcp_inactivity_timer) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, rtp_rtcp_inactivity_timer_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, rtp_rtcp_inactivity_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, voip_silent_redial_enabled) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, voip_silent_redial_enabled_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, voip_silent_redial_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, voip_preferred_rtp_payload_type) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, voip_preferred_rtp_payload_type_valid)),
  0x1E,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, voip_preferred_rtp_payload_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, voip_config_confURI) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, voip_config_confURI_valid)),
  0x1F,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, voip_config_confURI),
  IMS_SETTINGS_VOIP_CONFIG_CONF_URI_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, e911_silent_redial_enabled) - QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, e911_silent_redial_enabled_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_config_rsp_msg_v01, e911_silent_redial_enabled)
};

/*
 * ims_settings_get_presence_config_req_msg is empty
 * static const uint8_t ims_settings_get_presence_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_presence_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, publish_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, publish_expiry_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, publish_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, publish_extended_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, publish_extended_expiry_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, publish_extended_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, minimum_publish_interval) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, minimum_publish_interval_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, minimum_publish_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capability_poll_list_subscription_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capability_poll_list_subscription_expiry_timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capability_poll_list_subscription_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capability_discovery_enable) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capability_discovery_enable_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capability_discovery_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capabilites_cache_expiration) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capabilites_cache_expiration_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capabilites_cache_expiration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, availability_cache_expiration) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, availability_cache_expiration_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, availability_cache_expiration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capability_poll_interval) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capability_poll_interval_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, capability_poll_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, max_subcription_list_entries) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, max_subcription_list_entries_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, max_subcription_list_entries),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, volte_user_opted_in_status) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, volte_user_opted_in_status_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, volte_user_opted_in_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, last_publish_etag) - QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, last_publish_etag_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_presence_config_rsp_msg_v01, last_publish_etag),
  IMS_SETTINGS_PRESENCE_PUBLISH_ETAG_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, last_published_time) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, last_published_time_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_presence_config_rsp_msg_v01, last_published_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, last_negotiated_published_expire) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, last_negotiated_published_expire_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_presence_config_rsp_msg_v01, last_negotiated_published_expire),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, gzip_enabled) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, gzip_enabled_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_presence_config_rsp_msg_v01, gzip_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, presence_notify_wait_duration) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, presence_notify_wait_duration_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_presence_config_rsp_msg_v01, presence_notify_wait_duration),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, publish_error_recovery_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_presence_config_rsp_msg_v01, publish_error_recovery_timer_valid)),
  0x20,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_presence_config_rsp_msg_v01, publish_error_recovery_timer)
};

/*
 * ims_settings_get_presence_ext_config_req_msg is empty
 * static const uint8_t ims_settings_get_presence_ext_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_presence_ext_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, publish_error_recovery_timer) - QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, publish_error_recovery_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, publish_error_recovery_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, publish_user_agent) - QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, publish_user_agent_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_presence_ext_config_rsp_msg_v01, publish_user_agent),
  IMS_SETTINGS_PRESENCE_USERAGENT_LEN_V01
};

/*
 * ims_settings_get_media_config_req_msg is empty
 * static const uint8_t ims_settings_get_media_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_media_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, h264_profile) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, h264_profile_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, h264_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, h264_level) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, h264_level_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, h264_level),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_bitrate) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_bitrate_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_bitrate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_frames_per_second) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_frames_per_second_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_frames_per_second),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_resolution) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_resolution_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_resolution),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_codec) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_codec_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, video_codec),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, lipsync_drop_upper_limit) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, lipsync_drop_upper_limit_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, lipsync_drop_upper_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, lipsync_drop_lower_limit) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, lipsync_drop_lower_limit_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, lipsync_drop_lower_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, rtp_mtu_size) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, rtp_mtu_size_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, rtp_mtu_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_time_warping_enabled) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_time_warping_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_time_warping_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_iba_max) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_iba_max_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_iba_max),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_frames_at_start) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_frames_at_start_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_frames_at_start),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_delay) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_delay_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_min_delay) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_min_delay_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_min_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_optimization2_info) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_optimization2_info_valid)),
  0x1F,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_optimization2_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_frames_at_run) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_frames_at_run_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_frames_at_run),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_bumped_up_delay) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_bumped_up_delay_valid)),
  0x21,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_max_bumped_up_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_jitter_increment) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_jitter_increment_valid)),
  0x22,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_jitter_increment),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_target_underflow) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_target_underflow_valid)),
  0x23,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_target_underflow),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_default_jitter) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_default_jitter_valid)),
  0x24,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, qdj_default_jitter),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, gmin) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, gmin_valid)),
  0x25,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, gmin),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, tx_system_delay) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, tx_system_delay_valid)),
  0x26,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, tx_system_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, rx_system_delay) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, rx_system_delay_valid)),
  0x27,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, rx_system_delay),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, audio_offload) - QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, audio_offload_valid)),
  0x28,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_media_config_rsp_msg_v01, audio_offload)
};

/*
 * ims_settings_get_qipcall_config_req_msg is empty
 * static const uint8_t ims_settings_get_qipcall_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_qipcall_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, vt_calling_enabled) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, vt_calling_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, vt_calling_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, mobile_data_enabled) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, mobile_data_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, mobile_data_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, volte_enabled) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, volte_enabled_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, volte_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, emerg_call_timer) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, emerg_call_timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, emerg_call_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, vt_quality_selector) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, vt_quality_selector_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, vt_quality_selector),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, speech_start_port) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, speech_start_port_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, speech_start_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, speech_end_port) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, speech_end_port_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, speech_end_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_wb_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_wb_octet_aligned_dynamic_pt_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_wb_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_octet_aligned_dynamic_pt_valid)),
  0x1A,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_bandwidth_efficient_dynamic_pt_valid)),
  0x1B,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, dtmf_wb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, dtmf_wb_dynamic_pt_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, dtmf_wb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, dtmf_nb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, dtmf_nb_dynamic_pt_valid)),
  0x1D,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, dtmf_nb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_default_mode) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_default_mode_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, amr_default_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, video_rtp_port_start) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, video_rtp_port_start_valid)),
  0x1F,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, video_rtp_port_start),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, video_rtp_port_end) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, video_rtp_port_end_valid)),
  0x20,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, video_rtp_port_end),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, emerg_call_timer_wlan) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, emerg_call_timer_wlan_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_config_rsp_msg_v01, emerg_call_timer_wlan)
};

/*
 * ims_settings_get_reg_mgr_extended_config_req_msg is empty
 * static const uint8_t ims_settings_get_reg_mgr_extended_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_reg_mgr_extended_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, reregistration_delay) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, reregistration_delay_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, reregistration_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, t_delay) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, t_delay_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, t_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, reg_retry_base_time) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, reg_retry_base_time_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, reg_retry_base_time),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, reg_retry_max_time) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, reg_retry_max_time_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01, reg_retry_max_time)
};

/*
 * ims_settings_get_pol_mgr_config_req_msg is empty
 * static const uint8_t ims_settings_get_pol_mgr_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_pol_mgr_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_rat_apn_info) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_rat_apn_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_rat_apn_info),
  IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01,
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_rat_apn_fb_sp_info) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_rat_apn_fb_sp_info_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_rat_apn_fb_sp_info),
  IMS_SETTINGS_POL_MGR_RAT_APN_FB_SIZE_V01,
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_allowed_services_wlan) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_allowed_services_wlan_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_allowed_services_wlan),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_add_all_fts) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_add_all_fts_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_add_all_fts),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_acs_priority) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_acs_priority_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_acs_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_isim_priority) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_isim_priority_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_isim_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_nv_priority) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_nv_priority_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_nv_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_pco_priority) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_pco_priority_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_pco_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_ims_service_status) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_ims_service_status_valid)),
  0x19,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_ims_service_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_apn_name) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_apn_name_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_apn_name),
  IMS_SETTINGS_POL_MGR_APN_SIZE_V01,
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_rat_apn_info_extend) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_rat_apn_info_extend_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_pol_mgr_config_rsp_msg_v01, pol_mgr_rat_apn_info_extend),
  IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01,
  QMI_IDL_TYPE88(0, 7)
};

/*
 * ims_settings_get_rcs_sm_config_req_msg is empty
 * static const uint8_t ims_settings_get_rcs_sm_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_rcs_sm_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, standalone_message_auth) - QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, standalone_message_auth_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, standalone_message_auth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, standalone_message_max_size) - QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, standalone_message_max_size_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, standalone_message_max_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, standalone_message_explorer_uri) - QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, standalone_message_explorer_uri_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_sm_config_rsp_msg_v01, standalone_message_explorer_uri),
  IMS_SETTINGS_RCS_SM_EXPLORER_URI_LEN_V01
};

/*
 * ims_settings_get_ut_config_req_msg is empty
 * static const uint8_t ims_settings_get_ut_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_ut_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, disable_ut) - QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, disable_ut_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, disable_ut),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_apn_name) - QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_apn_name_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_apn_name),
  IMS_SETTINGS_UT_APN_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_ip_addr_type) - QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_ip_addr_type_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_ip_addr_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_apn_name_apn2) - QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_apn_name_apn2_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_apn_name_apn2),
  IMS_SETTINGS_UT_APN_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_ip_addr_type_apn2) - QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_ip_addr_type_apn2_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ut_config_rsp_msg_v01, ut_ip_addr_type_apn2)
};

/*
 * ims_settings_get_client_provisioning_config_req_msg is empty
 * static const uint8_t ims_settings_get_client_provisioning_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_client_provisioning_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_client_provisioning) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_client_provisioning_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_client_provisioning),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_volte) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_volte_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_volte),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_vt) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_vt_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_vt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_presence) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_presence_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_presence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_preference) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_preference_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_preference),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_roaming) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_roaming_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_roaming),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_vowifi) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_vowifi_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, enable_vowifi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, vowifi_provId) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, vowifi_provId_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, vowifi_provId),
  IMS_SETTINGS_VOIP_VOWIFI_MDN_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_roam_preference) - QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_roam_preference_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_client_provisioning_config_rsp_msg_v01, wifi_call_roam_preference)
};

/*
 * ims_settings_get_sip_read_only_config_req_msg is empty
 * static const uint8_t ims_settings_get_sip_read_only_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_sip_read_only_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, timer_t4) - QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, timer_t4_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, timer_t4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, tcp_threshold_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, tcp_threshold_value_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, tcp_threshold_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, compact_form_enabled) - QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, compact_form_enabled_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, compact_form_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, settings_auth_scheme) - QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, settings_auth_scheme_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, settings_auth_scheme),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, settings_initial_auth_config) - QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, settings_initial_auth_config_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, settings_initial_auth_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, auth_header_value) - QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, auth_header_value_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_sip_read_only_config_rsp_msg_v01, auth_header_value),
  IMS_SETTINGS_CONFIG_PROXY_ROUTE_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_sip_read_only_config_rsp_msg_v01, proxy_route_value) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_sip_read_only_config_rsp_msg_v01, proxy_route_value_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_sip_read_only_config_rsp_msg_v01, proxy_route_value),
  IMS_SETTINGS_CONFIG_PROXY_ROUTE_LEN_V01
};

/*
 * ims_settings_get_network_read_only_config_req_msg is empty
 * static const uint8_t ims_settings_get_network_read_only_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_network_read_only_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, ipv6_enabled) - QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, ipv6_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, ipv6_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, ip_sec_int_scheme) - QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, ip_sec_int_scheme_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, ip_sec_int_scheme),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, ip_sec_enc_algo) - QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, ip_sec_enc_algo_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, ip_sec_enc_algo),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, msrp_pkt_size) - QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, msrp_pkt_size_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_network_read_only_config_rsp_msg_v01, msrp_pkt_size)
};

/*
 * ims_settings_get_voip_read_only_config_req_msg is empty
 * static const uint8_t ims_settings_get_voip_read_only_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_voip_read_only_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, voip_config_expires) - QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, voip_config_expires_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, voip_config_expires),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, voip_session_timer_enabled) - QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, voip_session_timer_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voip_read_only_config_rsp_msg_v01, voip_session_timer_enabled)
};

/*
 * ims_settings_get_user_read_only_config_req_msg is empty
 * static const uint8_t ims_settings_get_user_read_only_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_user_read_only_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, reg_config_userName) - QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, reg_config_userName_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, reg_config_userName),
  IMS_SETTINGS_REG_CONFIG_USER_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, reg_config_privateURI) - QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, reg_config_privateURI_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_user_read_only_config_rsp_msg_v01, reg_config_privateURI),
  IMS_SETTINGS_REG_CONFIG_PRIVATE_URI_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_user_read_only_config_rsp_msg_v01, reg_config_displayName) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_user_read_only_config_rsp_msg_v01, reg_config_displayName_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_FIRST_EXTENDED |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_FLAGS_UTF16_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_user_read_only_config_rsp_msg_v01, reg_config_displayName),
  IMS_SETTINGS_REG_CONFIG_DISPLAY_NAME_LEN_V01
};

/*
 * ims_settings_get_reg_mgr_read_only_config_req_msg is empty
 * static const uint8_t ims_settings_get_reg_mgr_read_only_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_reg_mgr_read_only_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, settings_regmgr_mode_config) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, settings_regmgr_mode_config_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, settings_regmgr_mode_config),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, regmgr_pdp_profilename) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, regmgr_pdp_profilename_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01, regmgr_pdp_profilename),
  IMS_SETTINGS_REG_PDPD_PROFILE_NAME_LEN_V01
};

/*
 * ims_settings_get_rcs_auto_config_read_only_config_req_msg is empty
 * static const uint8_t ims_settings_get_rcs_auto_config_read_only_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcsOnly_device_type) - QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcsOnly_device_type_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcsOnly_device_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_pdp_profilename) - QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_pdp_profilename_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_pdp_profilename),
  IMS_SETTINGS_REG_PDPD_PROFILE_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, internet_pdp_profilename) - QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, internet_pdp_profilename_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, internet_pdp_profilename),
  IMS_SETTINGS_REG_PDPD_PROFILE_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, pco_config_priority) - QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, pco_config_priority_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, pco_config_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, isim_config_priority) - QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, isim_config_priority_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, isim_config_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, preconfig_priority) - QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, preconfig_priority_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, preconfig_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, autoconfig_priority) - QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, autoconfig_priority_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, autoconfig_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_lte_ft_list) - QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_lte_ft_list_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_lte_ft_list),
  ((IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01) & 0xFF), ((IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01) >> 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_hspa_ft_list) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_hspa_ft_list_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_hspa_ft_list),
  ((IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01) & 0xFF), ((IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01) >> 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_wifi_ft_list) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_wifi_ft_list_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, rcs_wifi_ft_list),
  ((IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01) & 0xFF), ((IMS_SETTINGS_RCS_FEATURE_TAG_LIST_LEN_V01) >> 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, disable_auto_config) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, disable_auto_config_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01, disable_auto_config)
};

/*
 * ims_settings_get_rcs_imscore_auto_config_read_only_config_req_msg is empty
 * static const uint8_t ims_settings_get_rcs_imscore_auto_config_read_only_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, rcs_timer_t1) - QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, rcs_timer_t1_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, rcs_timer_t1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, rcs_timer_t2) - QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, rcs_timer_t2_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, rcs_timer_t2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, rcs_timer_t4) - QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, rcs_timer_t4_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01, rcs_timer_t4)
};

static const uint8_t ims_settings_config_ind_reg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, sip_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, sip_config_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, sip_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, reg_mgr_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, reg_mgr_config_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, reg_mgr_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, sms_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, sms_config_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, sms_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, user_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, user_config_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, user_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, voip_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, voip_config_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, voip_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, presence_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, presence_config_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, presence_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, media_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, media_config_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, media_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, qipcall_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, qipcall_config_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, qipcall_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, sip_read_only_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, sip_read_only_config_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, sip_read_only_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, network_read_only_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, network_read_only_config_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, network_read_only_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, reg_mgr_extended_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, reg_mgr_extended_config_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, reg_mgr_extended_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, pol_mgr_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, pol_mgr_config_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, pol_mgr_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, presence_ext_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, presence_ext_config_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, presence_ext_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rcs_sm_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rcs_sm_config_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rcs_sm_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ut_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ut_config_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ut_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, client_provisioning_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, client_provisioning_config_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, client_provisioning_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, vt_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, vt_config_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, vt_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, acs_network_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, acs_network_config_valid)),
  0x21,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, acs_network_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, handover_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, handover_config_valid)),
  0x22,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, handover_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, qipcall_vice_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, qipcall_vice_config_valid)),
  0x23,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, qipcall_vice_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, pol_mgr_rat_config) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, pol_mgr_rat_config_valid)),
  0x24,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, pol_mgr_rat_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, voice_dynamic_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, voice_dynamic_config_enabled_valid)),
  0x25,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, voice_dynamic_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, codec_dynamic_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, codec_dynamic_config_enabled_valid)),
  0x26,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, codec_dynamic_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, video_dynamic_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, video_dynamic_config_enabled_valid)),
  0x27,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, video_dynamic_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, emer_dynamic_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, emer_dynamic_config_enabled_valid)),
  0x28,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, emer_dynamic_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, test_dynamic_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, test_dynamic_config_enabled_valid)),
  0x29,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, test_dynamic_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rtp_dynamic_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rtp_dynamic_config_enabled_valid)),
  0x2A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rtp_dynamic_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ims_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ims_config_enabled_valid)),
  0x2B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ims_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ims_common_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ims_common_config_enabled_valid)),
  0x2C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ims_common_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ims_service_enable_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ims_service_enable_config_enabled_valid)),
  0x2D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, ims_service_enable_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rcs_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rcs_config_enabled_valid)),
  0x2E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rcs_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, xcap_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, xcap_config_enabled_valid)),
  0x2F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, xcap_config_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, qipcall_evs_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, qipcall_evs_config_enabled_valid)),
  0x30,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, qipcall_evs_config_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rtt_config_enabled) - QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rtt_config_enabled_valid)),
  0x31,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_req_msg_v01, rtt_config_enabled)
};

static const uint8_t ims_settings_config_ind_reg_rsp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_config_ind_reg_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ims_settings_sip_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, sip_local_port) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, sip_local_port_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, sip_local_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_sip_reg) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_sip_reg_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_sip_reg),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, subscribe_timer) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, subscribe_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, subscribe_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_t1) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_t1_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_t1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_t2) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_t2_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_t2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tf) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tf_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tf),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, sigcomp_enabled) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, sigcomp_enabled_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, sigcomp_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tj) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tj_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tj),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tj_ext) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tj_ext_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tj_ext),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, keepalive_enabled) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, keepalive_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, keepalive_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, nat_rto_timer) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, nat_rto_timer_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, nat_rto_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, sip_timer_operator_mode_a) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, sip_timer_operator_mode_a_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, sip_timer_operator_mode_a),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tb_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tb_value_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tb_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, gruu_enabled) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, gruu_enabled_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, gruu_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, transport_switch_enabled) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, transport_switch_enabled_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, transport_switch_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, tcp_max_backoff_timer_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, tcp_max_backoff_timer_value_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, tcp_max_backoff_timer_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, gzip_decoding_outbuffer_multiplier) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, gzip_decoding_outbuffer_multiplier_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, gzip_decoding_outbuffer_multiplier),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_td_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_td_value_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_td_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_t4) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_t4_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_t4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_ta_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_ta_value_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_ta_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_te_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_te_value_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_te_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tg_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tg_value_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tg_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_th_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_th_value_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_th_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_ti_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_ti_value_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_ti_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tk_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tk_value_valid)),
  0x28,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tk_value),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tc_value) - QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tc_value_valid)),
  0x29,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_config_ind_msg_v01, timer_tc_value)
};

static const uint8_t ims_settings_reg_mgr_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_config_ind_msg_v01, regmgr_config_pcscf_port) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_config_ind_msg_v01, regmgr_config_pcscf_port_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_config_ind_msg_v01, regmgr_config_pcscf_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_config_ind_msg_v01, regmgr_primary_cscf) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_config_ind_msg_v01, regmgr_primary_cscf_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_config_ind_msg_v01, regmgr_primary_cscf),
  IMS_SETTINGS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_reg_mgr_config_ind_msg_v01, ims_test_mode) - QMI_IDL_OFFSET16RELATIVE(ims_settings_reg_mgr_config_ind_msg_v01, ims_test_mode_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_reg_mgr_config_ind_msg_v01, ims_test_mode)
};

static const uint8_t ims_settings_sms_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sms_config_ind_msg_v01, sms_format) - QMI_IDL_OFFSET8(ims_settings_sms_config_ind_msg_v01, sms_format_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_sms_config_ind_msg_v01, sms_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sms_config_ind_msg_v01, sms_over_ip_network_indication) - QMI_IDL_OFFSET8(ims_settings_sms_config_ind_msg_v01, sms_over_ip_network_indication_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sms_config_ind_msg_v01, sms_over_ip_network_indication),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sms_config_ind_msg_v01, phone_context_uri) - QMI_IDL_OFFSET8(ims_settings_sms_config_ind_msg_v01, phone_context_uri_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_sms_config_ind_msg_v01, phone_context_uri),
  IMS_SETTINGS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_sms_config_ind_msg_v01, sms_psi) - QMI_IDL_OFFSET16RELATIVE(ims_settings_sms_config_ind_msg_v01, sms_psi_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_sms_config_ind_msg_v01, sms_psi),
  IMS_SETTINGS_SMS_PSI_LEN_V01
};

static const uint8_t ims_settings_user_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_user_config_ind_msg_v01, ims_domain) - QMI_IDL_OFFSET8(ims_settings_user_config_ind_msg_v01, ims_domain_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_user_config_ind_msg_v01, ims_domain),
  IMS_SETTINGS_STRING_LEN_MAX_V01
};

static const uint8_t ims_settings_voip_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, session_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, session_expiry_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, session_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, min_session_expiry) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, min_session_expiry_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, min_session_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_wb_enable) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_wb_enable_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_wb_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, scr_amr_enable) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, scr_amr_enable_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, scr_amr_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, scr_amr_wb_enable) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, scr_amr_wb_enable_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, scr_amr_wb_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_mode) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_mode_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_wb_mode) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_wb_mode_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_wb_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_octet_align) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_octet_align_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_octet_align),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_wb_octet_align) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_wb_octet_align_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, amr_wb_octet_align),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, ringing_timer) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, ringing_timer_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, ringing_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, ringback_timer) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, ringback_timer_valid)),
  0x1A,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, ringback_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, rtp_rtcp_inactivity_timer) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, rtp_rtcp_inactivity_timer_valid)),
  0x1B,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, rtp_rtcp_inactivity_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, voip_silent_redial_enabled) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, voip_silent_redial_enabled_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, voip_silent_redial_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, voip_preferred_rtp_payload_type) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, voip_preferred_rtp_payload_type_valid)),
  0x1D,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, voip_preferred_rtp_payload_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, voip_config_confURI) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, voip_config_confURI_valid)),
  0x1E,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, voip_config_confURI),
  IMS_SETTINGS_VOIP_CONFIG_CONF_URI_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, e911_silent_redial_enabled) - QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, e911_silent_redial_enabled_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voip_config_ind_msg_v01, e911_silent_redial_enabled)
};

static const uint8_t ims_settings_presence_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, publish_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, publish_expiry_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, publish_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, publish_extended_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, publish_extended_expiry_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, publish_extended_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, minimum_publish_interval) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, minimum_publish_interval_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, minimum_publish_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capability_poll_list_subscription_expiry_timer) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capability_poll_list_subscription_expiry_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capability_poll_list_subscription_expiry_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capability_discovery_enable) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capability_discovery_enable_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capability_discovery_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capabilites_cache_expiration) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capabilites_cache_expiration_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capabilites_cache_expiration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, availability_cache_expiration) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, availability_cache_expiration_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, availability_cache_expiration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capability_poll_interval) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capability_poll_interval_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, capability_poll_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, max_subcription_list_entries) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, max_subcription_list_entries_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, max_subcription_list_entries),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, volte_user_opted_in_status) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, volte_user_opted_in_status_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, volte_user_opted_in_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, last_publish_etag) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, last_publish_etag_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, last_publish_etag),
  IMS_SETTINGS_PRESENCE_PUBLISH_ETAG_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, last_published_time) - QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, last_published_time_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_config_ind_msg_v01, last_published_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_presence_config_ind_msg_v01, last_negotiated_published_expire) - QMI_IDL_OFFSET16RELATIVE(ims_settings_presence_config_ind_msg_v01, last_negotiated_published_expire_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_presence_config_ind_msg_v01, last_negotiated_published_expire),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_presence_config_ind_msg_v01, gzip_enabled) - QMI_IDL_OFFSET16RELATIVE(ims_settings_presence_config_ind_msg_v01, gzip_enabled_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_presence_config_ind_msg_v01, gzip_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_presence_config_ind_msg_v01, presence_notify_wait_duration) - QMI_IDL_OFFSET16RELATIVE(ims_settings_presence_config_ind_msg_v01, presence_notify_wait_duration_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_presence_config_ind_msg_v01, presence_notify_wait_duration),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_presence_config_ind_msg_v01, publish_error_recovery_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_presence_config_ind_msg_v01, publish_error_recovery_timer_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_presence_config_ind_msg_v01, publish_error_recovery_timer)
};

static const uint8_t ims_settings_presence_ext_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_ext_config_ind_msg_v01, publish_error_recovery_timer) - QMI_IDL_OFFSET8(ims_settings_presence_ext_config_ind_msg_v01, publish_error_recovery_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_ext_config_ind_msg_v01, publish_error_recovery_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_ext_config_ind_msg_v01, publish_user_agent) - QMI_IDL_OFFSET8(ims_settings_presence_ext_config_ind_msg_v01, publish_user_agent_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_presence_ext_config_ind_msg_v01, publish_user_agent),
  IMS_SETTINGS_PRESENCE_USERAGENT_LEN_V01
};

static const uint8_t ims_settings_media_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, h264_profile) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, h264_profile_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, h264_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, h264_level) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, h264_level_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, h264_level),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_bitrate) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_bitrate_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_bitrate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_frames_per_second) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_frames_per_second_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_frames_per_second),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_resolution) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_resolution_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_resolution),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_codec) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_codec_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, video_codec),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, lipsync_drop_upper_limit) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, lipsync_drop_upper_limit_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, lipsync_drop_upper_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, lipsync_drop_lower_limit) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, lipsync_drop_lower_limit_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, lipsync_drop_lower_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, rtp_mtu_size) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, rtp_mtu_size_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, rtp_mtu_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_time_warping_enabled) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_time_warping_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_time_warping_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_iba_max) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_iba_max_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_iba_max),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_frames_at_start) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_frames_at_start_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_frames_at_start),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_delay) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_delay_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_min_delay) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_min_delay_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_min_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_optimization2_info) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_optimization2_info_valid)),
  0x1E,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_optimization2_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_frames_at_run) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_frames_at_run_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_frames_at_run),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_bumped_up_delay) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_bumped_up_delay_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_max_bumped_up_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_jitter_increment) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_jitter_increment_valid)),
  0x21,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_jitter_increment),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_target_underflow) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_target_underflow_valid)),
  0x22,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_target_underflow),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_default_jitter) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_default_jitter_valid)),
  0x23,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, qdj_default_jitter),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, gmin) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, gmin_valid)),
  0x24,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, gmin),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, tx_system_delay) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, tx_system_delay_valid)),
  0x25,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, tx_system_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, rx_system_delay) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, rx_system_delay_valid)),
  0x26,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, rx_system_delay),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, audio_offload) - QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, audio_offload_valid)),
  0x27,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_media_config_ind_msg_v01, audio_offload)
};

static const uint8_t ims_settings_qipcall_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, vt_calling_enabled) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, vt_calling_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, vt_calling_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, mobile_data_enabled) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, mobile_data_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, mobile_data_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, volte_enabled) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, volte_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, volte_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, emerg_call_timer) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, emerg_call_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, emerg_call_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, vt_quality_selector) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, vt_quality_selector_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, vt_quality_selector),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, speech_start_port) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, speech_start_port_valid)),
  0x15,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, speech_start_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, speech_end_port) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, speech_end_port_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, speech_end_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_wb_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_wb_octet_aligned_dynamic_pt_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_wb_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_octet_aligned_dynamic_pt_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_bandwidth_efficient_dynamic_pt_valid)),
  0x1A,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, dtmf_wb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, dtmf_wb_dynamic_pt_valid)),
  0x1B,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, dtmf_wb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, dtmf_nb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, dtmf_nb_dynamic_pt_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, dtmf_nb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_default_mode) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_default_mode_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, amr_default_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, video_rtp_port_start) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, video_rtp_port_start_valid)),
  0x1E,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, video_rtp_port_start),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, video_rtp_port_end) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, video_rtp_port_end_valid)),
  0x1F,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, video_rtp_port_end),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, emerg_call_timer_wlan) - QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, emerg_call_timer_wlan_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_config_ind_msg_v01, emerg_call_timer_wlan)
};

static const uint8_t ims_settings_reg_mgr_extended_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, reregistration_delay) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, reregistration_delay_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, reregistration_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, t_delay) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, t_delay_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, t_delay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, reg_retry_base_time) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, reg_retry_base_time_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, reg_retry_base_time),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, reg_retry_max_time) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, reg_retry_max_time_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_extended_config_ind_msg_v01, reg_retry_max_time)
};

static const uint8_t ims_settings_pol_mgr_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_rat_apn_info) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_rat_apn_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_rat_apn_info),
  IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01,
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_rat_apn_fb_sp_info) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_rat_apn_fb_sp_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_rat_apn_fb_sp_info),
  IMS_SETTINGS_POL_MGR_RAT_APN_FB_SIZE_V01,
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_allowed_services_wlan) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_allowed_services_wlan_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_allowed_services_wlan),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_add_all_fts) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_add_all_fts_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_add_all_fts),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_acs_priority) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_acs_priority_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_acs_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_isim_priority) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_isim_priority_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_isim_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_nv_priority) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_nv_priority_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_nv_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_pco_priority) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_pco_priority_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_pco_priority),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_ims_service_status) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_ims_service_status_valid)),
  0x18,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_ims_service_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_apn_name) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_apn_name_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_apn_name),
  IMS_SETTINGS_POL_MGR_APN_SIZE_V01,
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_rat_apn_info_extend) - QMI_IDL_OFFSET16RELATIVE(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_rat_apn_info_extend_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_pol_mgr_config_ind_msg_v01, pol_mgr_rat_apn_info_extend),
  IMS_SETTINGS_POL_MGR_RAT_APN_SIZE_V01,
  QMI_IDL_TYPE88(0, 7)
};

static const uint8_t ims_settings_rcs_sm_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rcs_sm_config_ind_msg_v01, standalone_message_auth) - QMI_IDL_OFFSET8(ims_settings_rcs_sm_config_ind_msg_v01, standalone_message_auth_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rcs_sm_config_ind_msg_v01, standalone_message_auth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rcs_sm_config_ind_msg_v01, standalone_message_max_size) - QMI_IDL_OFFSET8(ims_settings_rcs_sm_config_ind_msg_v01, standalone_message_max_size_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rcs_sm_config_ind_msg_v01, standalone_message_max_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rcs_sm_config_ind_msg_v01, standalone_message_explorer_uri) - QMI_IDL_OFFSET8(ims_settings_rcs_sm_config_ind_msg_v01, standalone_message_explorer_uri_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_rcs_sm_config_ind_msg_v01, standalone_message_explorer_uri),
  IMS_SETTINGS_RCS_SM_EXPLORER_URI_LEN_V01
};

static const uint8_t ims_settings_ut_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_disabled) - QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_disabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_disabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_apn_name) - QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_apn_name_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_apn_name),
  IMS_SETTINGS_UT_APN_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_ip_addr_type) - QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_ip_addr_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_ip_addr_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_apn_name_apn2) - QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_apn_name_apn2_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_apn_name_apn2),
  IMS_SETTINGS_UT_APN_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_ip_addr_type_apn2) - QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_ip_addr_type_apn2_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ut_config_ind_msg_v01, ut_ip_addr_type_apn2)
};

static const uint8_t ims_settings_client_provisioning_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, client_provisioning_enabled) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, client_provisioning_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, client_provisioning_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, volte_enabled) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, volte_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, volte_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, vt_enabled) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, vt_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, vt_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, presence_enabled) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, presence_enabled_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, presence_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_preference) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_preference_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_preference),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_roaming) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_roaming_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_roaming),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, vowifi_enabled) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, vowifi_enabled_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, vowifi_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, vowifi_provId) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, vowifi_provId_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, vowifi_provId),
  IMS_SETTINGS_VOIP_VOWIFI_MDN_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_roam_preference) - QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_roam_preference_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_client_provisioning_config_ind_msg_v01, wifi_call_roam_preference)
};

static const uint8_t ims_settings_sip_read_only_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, timer_t4) - QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, timer_t4_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, timer_t4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, tcp_threshold_value) - QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, tcp_threshold_value_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, tcp_threshold_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, compact_form_enabled) - QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, compact_form_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, compact_form_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, settings_auth_scheme) - QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, settings_auth_scheme_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, settings_auth_scheme),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, settings_initial_auth_config) - QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, settings_initial_auth_config_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, settings_initial_auth_config),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, auth_header_value) - QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, auth_header_value_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_sip_read_only_config_ind_msg_v01, auth_header_value),
  IMS_SETTINGS_CONFIG_PROXY_ROUTE_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_sip_read_only_config_ind_msg_v01, proxy_route_value) - QMI_IDL_OFFSET16RELATIVE(ims_settings_sip_read_only_config_ind_msg_v01, proxy_route_value_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_sip_read_only_config_ind_msg_v01, proxy_route_value),
  IMS_SETTINGS_CONFIG_PROXY_ROUTE_LEN_V01
};

static const uint8_t ims_settings_network_read_only_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, ipv6_enabled) - QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, ipv6_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, ipv6_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, ip_sec_int_scheme) - QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, ip_sec_int_scheme_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, ip_sec_int_scheme),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, ip_sec_enc_algo) - QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, ip_sec_enc_algo_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, ip_sec_enc_algo),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, msrp_pkt_size) - QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, msrp_pkt_size_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_network_read_only_config_ind_msg_v01, msrp_pkt_size)
};

static const uint8_t ims_settings_set_vt_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, vt_3g_h263_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, vt_3g_h263_dynamic_pt_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, vt_3g_h263_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, vt_4g_h263_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, vt_4g_h263_dynamic_pt_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, vt_4g_h263_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, num_vt_4g_h264_config) - QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, num_vt_4g_h264_config_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, num_vt_4g_h264_config),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, vt_4g_h264_info) - QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, vt_4g_h264_info_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_vt_config_req_msg_v01, vt_4g_h264_info),
  IMS_SETTINGS_VT_4G_H264_CONFIG_SIZE_V01,
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t ims_settings_set_vt_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_vt_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_vt_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_vt_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_vt_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_vt_config_req_msg is empty
 * static const uint8_t ims_settings_get_vt_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_vt_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, vt_3g_h263_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, vt_3g_h263_dynamic_pt_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, vt_3g_h263_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, vt_4g_h263_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, vt_4g_h263_dynamic_pt_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, vt_4g_h263_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, num_vt_4g_h264_config) - QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, num_vt_4g_h264_config_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, num_vt_4g_h264_config),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, vt_4g_h264_info) - QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, vt_4g_h264_info_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_vt_config_rsp_msg_v01, vt_4g_h264_info),
  IMS_SETTINGS_VT_4G_H264_CONFIG_SIZE_V01,
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t ims_settings_vt_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, vt_3g_h263_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, vt_3g_h263_dynamic_pt_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, vt_3g_h263_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, vt_4g_h263_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, vt_4g_h263_dynamic_pt_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, vt_4g_h263_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, num_vt_4g_h264_config) - QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, num_vt_4g_h264_config_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, num_vt_4g_h264_config),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, vt_4g_h264_info) - QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, vt_4g_h264_info_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_vt_config_ind_msg_v01, vt_4g_h264_info),
  IMS_SETTINGS_VT_4G_H264_CONFIG_SIZE_V01,
  QMI_IDL_TYPE88(0, 4)
};

/*
 * ims_settings_get_acs_network_config_req_msg is empty
 * static const uint8_t ims_settings_get_acs_network_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_acs_network_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, ir94_video_calling_service_enabled) - QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, ir94_video_calling_service_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, ir94_video_calling_service_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, rcs_messaging_enabled) - QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, rcs_messaging_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_acs_network_config_rsp_msg_v01, rcs_messaging_enabled)
};

static const uint8_t ims_settings_acs_network_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_acs_network_config_ind_msg_v01, ir94_video_calling_service_enabled) - QMI_IDL_OFFSET8(ims_settings_acs_network_config_ind_msg_v01, ir94_video_calling_service_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_acs_network_config_ind_msg_v01, ir94_video_calling_service_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_acs_network_config_ind_msg_v01, rcs_messaging_enabled) - QMI_IDL_OFFSET8(ims_settings_acs_network_config_ind_msg_v01, rcs_messaging_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_acs_network_config_ind_msg_v01, rcs_messaging_enabled)
};

static const uint8_t ims_settings_set_acs_network_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_acs_network_config_req_msg_v01, enable_rcs_messaging) - QMI_IDL_OFFSET8(ims_settings_set_acs_network_config_req_msg_v01, enable_rcs_messaging_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_acs_network_config_req_msg_v01, enable_rcs_messaging)
};

static const uint8_t ims_settings_set_acs_network_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_acs_network_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_acs_network_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_acs_network_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_acs_network_config_rsp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_set_handover_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_wwan_hys_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_wwan_hys_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_wwan_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_1x_hys_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_1x_hys_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_1x_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wwan_to_wlan_hys_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wwan_to_wlan_hys_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wwan_to_wlan_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, ims_pdn_ho_enabled) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, ims_pdn_ho_enabled_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, ims_pdn_ho_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHO1xQualTH) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHO1xQualTH_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHO1xQualTH),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOLTEQualTH1) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOLTEQualTH1_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOLTEQualTH1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOLTEQualTH2) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOLTEQualTH2_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOLTEQualTH2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOLTEQualTH3) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOLTEQualTH3_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOLTEQualTH3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOWIFIQualTHA) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOWIFIQualTHA_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOWIFIQualTHA),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOWIFIQualTHB) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOWIFIQualTHB_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIMSHOWIFIQualTHB),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, ho_disabled_mask) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, ho_disabled_mask_valid)),
  0x1A,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, ho_disabled_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_RSRQ_Src) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_RSRQ_Src_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_RSRQ_Src),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_SNR_Src) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_SNR_Src_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_SNR_Src),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_RSRQ_Target) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_RSRQ_Target_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_RSRQ_Target),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_SNR_Target) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_SNR_Target_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_LTE_DL_SNR_Target),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_Wifi_Rove_Out) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_Wifi_Rove_Out_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_Wifi_Rove_Out),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_Wifi_Rove_In) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_Wifi_Rove_In_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_Wifi_Rove_In),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_Wifi_Repoint) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_Wifi_Repoint_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, iIR92_Wifi_Repoint),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_3gpp_idle_mode_hys_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_3gpp_idle_mode_hys_timer_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_3gpp_idle_mode_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_3gpp2_idle_mode_hys_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_3gpp2_idle_mode_hys_timer_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_3gpp2_idle_mode_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_1x_idle_mode_hys_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_1x_idle_mode_hys_timer_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_1x_idle_mode_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_voip_cell_pref_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_voip_cell_pref_timer_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_voip_cell_pref_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_voip_wifi_pref_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_voip_wifi_pref_timer_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_voip_wifi_pref_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_vt_cell_pref_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_vt_cell_pref_timer_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_vt_cell_pref_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_vt_wifi_pref_timer) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_vt_wifi_pref_timer_valid)),
  0x28,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_req_msg_v01, wlan_to_lte_vt_wifi_pref_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_handover_config_req_msg_v01, wwan_to_wlan_all_mode_hysteresis_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_handover_config_req_msg_v01, wwan_to_wlan_all_mode_hysteresis_timer_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_handover_config_req_msg_v01, wwan_to_wlan_all_mode_hysteresis_timer)
};

static const uint8_t ims_settings_set_handover_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_handover_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_handover_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_handover_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_handover_config_req_msg is empty
 * static const uint8_t ims_settings_get_handover_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_handover_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_wwan_hys_timer) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_wwan_hys_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_wwan_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_1x_hys_timer) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_1x_hys_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_1x_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wwan_to_wlan_hys_timer) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wwan_to_wlan_hys_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wwan_to_wlan_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, ims_pdn_ho_enabled) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, ims_pdn_ho_enabled_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, ims_pdn_ho_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHO1xQualTH) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHO1xQualTH_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHO1xQualTH),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOLTEQualTH1) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOLTEQualTH1_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOLTEQualTH1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOLTEQualTH2) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOLTEQualTH2_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOLTEQualTH2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOLTEQualTH3) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOLTEQualTH3_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOLTEQualTH3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOWIFIQualTHA) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOWIFIQualTHA_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOWIFIQualTHA),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOWIFIQualTHB) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOWIFIQualTHB_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIMSHOWIFIQualTHB),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, ho_disabled_mask) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, ho_disabled_mask_valid)),
  0x1B,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, ho_disabled_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_RSRQ_Src) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_RSRQ_Src_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_RSRQ_Src),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_SNR_Src) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_SNR_Src_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_SNR_Src),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_RSRQ_Target) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_RSRQ_Target_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_RSRQ_Target),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_SNR_Target) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_SNR_Target_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_LTE_DL_SNR_Target),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_Wifi_Rove_Out) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_Wifi_Rove_Out_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_Wifi_Rove_Out),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_Wifi_Rove_In) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_Wifi_Rove_In_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_Wifi_Rove_In),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_Wifi_Repoint) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_Wifi_Repoint_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, iIR92_Wifi_Repoint),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_3gpp_idle_mode_hys_timer) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_3gpp_idle_mode_hys_timer_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_3gpp_idle_mode_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_3gpp2_idle_mode_hys_timer) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_3gpp2_idle_mode_hys_timer_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_3gpp2_idle_mode_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_1x_idle_mode_hys_timer) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_1x_idle_mode_hys_timer_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_1x_idle_mode_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_voip_cell_pref_timer) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_voip_cell_pref_timer_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_voip_cell_pref_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_voip_wifi_pref_timer) - QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_voip_wifi_pref_timer_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_voip_wifi_pref_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_vt_cell_pref_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_vt_cell_pref_timer_valid)),
  0x28,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_vt_cell_pref_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_vt_wifi_pref_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_vt_wifi_pref_timer_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_handover_config_rsp_msg_v01, wlan_to_lte_vt_wifi_pref_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_handover_config_rsp_msg_v01, wwan_to_wlan_all_mode_hysteresis_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_handover_config_rsp_msg_v01, wwan_to_wlan_all_mode_hysteresis_timer_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_handover_config_rsp_msg_v01, wwan_to_wlan_all_mode_hysteresis_timer)
};

static const uint8_t ims_settings_handover_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_wwan_hys_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_wwan_hys_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_wwan_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_1x_hys_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_1x_hys_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_1x_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wwan_to_wlan_hys_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wwan_to_wlan_hys_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wwan_to_wlan_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, ims_pdn_ho_enabled) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, ims_pdn_ho_enabled_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, ims_pdn_ho_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHO1xQualTH) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHO1xQualTH_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHO1xQualTH),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOLTEQualTH1) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOLTEQualTH1_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOLTEQualTH1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOLTEQualTH2) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOLTEQualTH2_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOLTEQualTH2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOLTEQualTH3) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOLTEQualTH3_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOLTEQualTH3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOWIFIQualTHA) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOWIFIQualTHA_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOWIFIQualTHA),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOWIFIQualTHB) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOWIFIQualTHB_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIMSHOWIFIQualTHB),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, ho_disabled_mask) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, ho_disabled_mask_valid)),
  0x1A,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, ho_disabled_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_RSRQ_Src) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_RSRQ_Src_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_RSRQ_Src),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_SNR_Src) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_SNR_Src_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_SNR_Src),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_RSRQ_Target) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_RSRQ_Target_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_RSRQ_Target),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_SNR_Target) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_SNR_Target_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_LTE_DL_SNR_Target),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_Wifi_Rove_Out) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_Wifi_Rove_Out_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_Wifi_Rove_Out),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_Wifi_Rove_In) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_Wifi_Rove_In_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_Wifi_Rove_In),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_Wifi_Repoint) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_Wifi_Repoint_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, iIR92_Wifi_Repoint),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_3gpp_idle_mode_hys_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_3gpp_idle_mode_hys_timer_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_3gpp_idle_mode_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_3gpp2_idle_mode_hys_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_3gpp2_idle_mode_hys_timer_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_3gpp2_idle_mode_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_1x_idle_mode_hys_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_1x_idle_mode_hys_timer_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_1x_idle_mode_hys_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_voip_cell_pref_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_voip_cell_pref_timer_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_voip_cell_pref_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_voip_wifi_pref_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_voip_wifi_pref_timer_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_voip_wifi_pref_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_vt_cell_pref_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_vt_cell_pref_timer_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_vt_cell_pref_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_vt_wifi_pref_timer) - QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_vt_wifi_pref_timer_valid)),
  0x28,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_handover_config_ind_msg_v01, wlan_to_lte_vt_wifi_pref_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_handover_config_ind_msg_v01, wwan_to_wlan_all_mode_hysteresis_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_handover_config_ind_msg_v01, wwan_to_wlan_all_mode_hysteresis_timer_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_handover_config_ind_msg_v01, wwan_to_wlan_all_mode_hysteresis_timer)
};

static const uint8_t ims_settings_set_qipcall_vice_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_vice_config_req_msg_v01, qipcall_vice_enabled) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_vice_config_req_msg_v01, qipcall_vice_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_vice_config_req_msg_v01, qipcall_vice_enabled)
};

static const uint8_t ims_settings_set_qipcall_vice_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_vice_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_vice_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_vice_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_vice_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_qipcall_vice_config_req_msg is empty
 * static const uint8_t ims_settings_get_qipcall_vice_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_qipcall_vice_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_vice_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_vice_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_vice_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_vice_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_vice_config_rsp_msg_v01, qipcall_vice_enabled) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_vice_config_rsp_msg_v01, qipcall_vice_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_vice_config_rsp_msg_v01, qipcall_vice_enabled)
};

static const uint8_t ims_settings_qipcall_vice_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_vice_config_ind_msg_v01, qipcall_vice_enabled) - QMI_IDL_OFFSET8(ims_settings_qipcall_vice_config_ind_msg_v01, qipcall_vice_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_vice_config_ind_msg_v01, qipcall_vice_enabled)
};

static const uint8_t ims_settings_set_pol_mgr_rat_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_rat_config_req_msg_v01, rat_ipsec_enabled) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_rat_config_req_msg_v01, rat_ipsec_enabled_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_rat_config_req_msg_v01, rat_ipsec_enabled),
  IMS_SETTINGS_POL_MGR_RAT_MAX_V01,
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t ims_settings_set_pol_mgr_rat_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_rat_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_rat_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_rat_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_pol_mgr_rat_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_pol_mgr_rat_config_req_msg is empty
 * static const uint8_t ims_settings_get_pol_mgr_rat_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_pol_mgr_rat_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_rat_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_rat_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_rat_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_rat_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_rat_config_rsp_msg_v01, rat_ipsec_enabled) - QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_rat_config_rsp_msg_v01, rat_ipsec_enabled_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_pol_mgr_rat_config_rsp_msg_v01, rat_ipsec_enabled),
  IMS_SETTINGS_POL_MGR_RAT_MAX_V01,
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t ims_settings_pol_mgr_rat_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_pol_mgr_rat_config_ind_msg_v01, rat_ipsec_enabled) - QMI_IDL_OFFSET8(ims_settings_pol_mgr_rat_config_ind_msg_v01, rat_ipsec_enabled_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_pol_mgr_rat_config_ind_msg_v01, rat_ipsec_enabled),
  IMS_SETTINGS_POL_MGR_RAT_MAX_V01,
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t ims_settings_set_config_csd_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_config_csd_req_msg_v01, profile_number),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_config_csd_req_msg_v01, codec_data) - QMI_IDL_OFFSET8(ims_settings_set_config_csd_req_msg_v01, codec_data_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_config_csd_req_msg_v01, codec_data),
  IMS_SETTINGS_CSD_CODEC_MAX_V01,
  QMI_IDL_OFFSET8(ims_settings_set_config_csd_req_msg_v01, codec_data) - QMI_IDL_OFFSET8(ims_settings_set_config_csd_req_msg_v01, codec_data_len),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t ims_settings_set_config_csd_rsp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_config_csd_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ims_settings_set_sip_new_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_t1) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_t1_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_t1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_t2) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_t2_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_t2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_t4) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_t4_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_t4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_ta) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_ta_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_ta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tb) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tb_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tb),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tc) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tc_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_td) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_td_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_td),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_te) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_te_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_te),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tf) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tf_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tf),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tg) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tg_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tg),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_th) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_th_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_th),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_ti) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_ti_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_ti),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tj) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tj_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tj),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tk) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tk_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tk),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, keepalive_enabled) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, keepalive_enabled_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, keepalive_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tcallsetup) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tcallsetup_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_req_msg_v01, timer_tcallsetup)
};

static const uint8_t ims_settings_set_sip_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_sip_new_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_sip_new_config_req_msg is empty
 * static const uint8_t ims_settings_get_sip_new_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_sip_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_t1) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_t1_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_t1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_t2) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_t2_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_t2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_t4) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_t4_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_t4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_ta) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_ta_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_ta),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tb) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tb_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tb),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tc) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tc_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_td) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_td_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_td),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_te) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_te_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_te),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tf) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tf_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tf),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tg) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tg_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tg),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_th) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_th_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_th),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_ti) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_ti_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_ti),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tj) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tj_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tj),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tk) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tk_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tk),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, keepalive_enabled) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, keepalive_enabled_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, keepalive_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tcallsetup) - QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tcallsetup_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_sip_new_config_rsp_msg_v01, timer_tcallsetup)
};

static const uint8_t ims_settings_set_presence_new_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_timer) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_extended_timer) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_extended_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_extended_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_src_throttle_timer) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_src_throttle_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_src_throttle_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_error_recovery_timer) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_error_recovery_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, publish_error_recovery_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, list_subscription_expiry) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, list_subscription_expiry_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, list_subscription_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, capabilites_cache_expiry) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, capabilites_cache_expiry_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, capabilites_cache_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, availability_cache_expiry) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, availability_cache_expiry_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, availability_cache_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, capability_poll_interval) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, capability_poll_interval_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, capability_poll_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, max_enties_in_list_subscribe) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, max_enties_in_list_subscribe_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, max_enties_in_list_subscribe),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, capability_discovery_enabled) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, capability_discovery_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, capability_discovery_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, gzip_enabled) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, gzip_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, gzip_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, user_agent) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, user_agent_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_req_msg_v01, user_agent),
  IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01
};

static const uint8_t ims_settings_set_presence_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_presence_new_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_presence_new_config_req_msg is empty
 * static const uint8_t ims_settings_get_presence_new_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_presence_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_timer) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_extended_timer) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_extended_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_extended_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_src_throttle_timer) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_src_throttle_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_src_throttle_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_error_recovery_timer) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_error_recovery_timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, publish_error_recovery_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, list_subscription_expiry) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, list_subscription_expiry_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, list_subscription_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, capabilites_cache_expiry) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, capabilites_cache_expiry_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, capabilites_cache_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, availability_cache_expiry) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, availability_cache_expiry_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, availability_cache_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, capability_poll_interval) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, capability_poll_interval_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, capability_poll_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, max_enties_in_list_subscribe) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, max_enties_in_list_subscribe_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, max_enties_in_list_subscribe),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, capability_discovery_enabled) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, capability_discovery_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, capability_discovery_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, gzip_enabled) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, gzip_enabled_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, gzip_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, user_agent) - QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, user_agent_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_presence_new_config_rsp_msg_v01, user_agent),
  IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01
};

static const uint8_t ims_settings_presence_new_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_timer) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_extended_timer) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_extended_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_extended_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_src_throttle_timer) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_src_throttle_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_src_throttle_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_error_recovery_timer) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_error_recovery_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, publish_error_recovery_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, list_subscription_expiry) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, list_subscription_expiry_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, list_subscription_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, capabilites_cache_expiry) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, capabilites_cache_expiry_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, capabilites_cache_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, availability_cache_expiry) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, availability_cache_expiry_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, availability_cache_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, capability_poll_interval) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, capability_poll_interval_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, capability_poll_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, max_enties_in_list_subscribe) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, max_enties_in_list_subscribe_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, max_enties_in_list_subscribe),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, capability_discovery_enabled) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, capability_discovery_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, capability_discovery_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, gzip_enabled) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, gzip_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, gzip_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, user_agent) - QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, user_agent_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_presence_new_config_ind_msg_v01, user_agent),
  IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01
};

static const uint8_t ims_settings_set_rcs_new_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_req_msg_v01, auto_config_user_agent) - QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_req_msg_v01, auto_config_user_agent_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_req_msg_v01, auto_config_user_agent),
  IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_req_msg_v01, xdm_client_user_agent) - QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_req_msg_v01, xdm_client_user_agent_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_req_msg_v01, xdm_client_user_agent),
  IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, client_vendor) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, client_vendor_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_rcs_new_config_req_msg_v01, client_vendor),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, client_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, client_version_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_rcs_new_config_req_msg_v01, client_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, terminal_vendor) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, terminal_vendor_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_rcs_new_config_req_msg_v01, terminal_vendor),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, terminal_model) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, terminal_model_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_rcs_new_config_req_msg_v01, terminal_model),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, terminal_sw_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, terminal_sw_version_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_rcs_new_config_req_msg_v01, terminal_sw_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, rcs_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, rcs_version_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_rcs_new_config_req_msg_v01, rcs_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, provisioning_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, provisioning_version_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_rcs_new_config_req_msg_v01, provisioning_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, default_sms_app) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, default_sms_app_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_rcs_new_config_req_msg_v01, default_sms_app),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, default_vvm_app) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_rcs_new_config_req_msg_v01, default_vvm_app_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_rcs_new_config_req_msg_v01, default_vvm_app)
};

static const uint8_t ims_settings_set_rcs_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_rcs_new_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_rcs_new_config_req_msg is empty
 * static const uint8_t ims_settings_get_rcs_new_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_rcs_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, auto_config_user_agent) - QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, auto_config_user_agent_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, auto_config_user_agent),
  IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, xdm_client_user_agent) - QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, xdm_client_user_agent_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_rcs_new_config_rsp_msg_v01, xdm_client_user_agent),
  IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, client_vendor) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, client_vendor_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_new_config_rsp_msg_v01, client_vendor),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, client_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, client_version_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_new_config_rsp_msg_v01, client_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, terminal_vendor) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, terminal_vendor_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_new_config_rsp_msg_v01, terminal_vendor),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, terminal_model) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, terminal_model_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_new_config_rsp_msg_v01, terminal_model),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, terminal_sw_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, terminal_sw_version_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_new_config_rsp_msg_v01, terminal_sw_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, rcs_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, rcs_version_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_new_config_rsp_msg_v01, rcs_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, provisioning_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, provisioning_version_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_new_config_rsp_msg_v01, provisioning_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, default_sms_app) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, default_sms_app_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_new_config_rsp_msg_v01, default_sms_app),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, default_vvm_app) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_rcs_new_config_rsp_msg_v01, default_vvm_app_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_rcs_new_config_rsp_msg_v01, default_vvm_app)
};

static const uint8_t ims_settings_rcs_new_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rcs_new_config_ind_msg_v01, auto_config_user_agent) - QMI_IDL_OFFSET8(ims_settings_rcs_new_config_ind_msg_v01, auto_config_user_agent_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_rcs_new_config_ind_msg_v01, auto_config_user_agent),
  IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rcs_new_config_ind_msg_v01, xdm_client_user_agent) - QMI_IDL_OFFSET8(ims_settings_rcs_new_config_ind_msg_v01, xdm_client_user_agent_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_rcs_new_config_ind_msg_v01, xdm_client_user_agent),
  IMS_SETTINGS_RCS_USER_AGENT_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, client_vendor) - QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, client_vendor_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_rcs_new_config_ind_msg_v01, client_vendor),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, client_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, client_version_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_rcs_new_config_ind_msg_v01, client_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, terminal_vendor) - QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, terminal_vendor_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_rcs_new_config_ind_msg_v01, terminal_vendor),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, terminal_model) - QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, terminal_model_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_rcs_new_config_ind_msg_v01, terminal_model),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, terminal_sw_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, terminal_sw_version_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_rcs_new_config_ind_msg_v01, terminal_sw_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, rcs_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, rcs_version_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_rcs_new_config_ind_msg_v01, rcs_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, provisioning_version) - QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, provisioning_version_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(ims_settings_rcs_new_config_ind_msg_v01, provisioning_version),
  IMS_SETTINGS_RCS_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, default_sms_app) - QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, default_sms_app_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_rcs_new_config_ind_msg_v01, default_sms_app),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, default_vvm_app) - QMI_IDL_OFFSET16RELATIVE(ims_settings_rcs_new_config_ind_msg_v01, default_vvm_app_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_rcs_new_config_ind_msg_v01, default_vvm_app)
};

static const uint8_t ims_settings_set_reg_mgr_new_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, reg_irat_hystersis_timer) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, reg_irat_hystersis_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, reg_irat_hystersis_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, reg_retry_base_time) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, reg_retry_base_time_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, reg_retry_base_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, reg_retry_max_time) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, reg_retry_max_time_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, reg_retry_max_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, regmgr_primary_pcscf) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, regmgr_primary_pcscf_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_req_msg_v01, regmgr_primary_pcscf),
  IMS_SETTINGS_RM_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_reg_mgr_new_config_req_msg_v01, regmgr_pcscf_port) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_reg_mgr_new_config_req_msg_v01, regmgr_pcscf_port_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_reg_mgr_new_config_req_msg_v01, regmgr_pcscf_port)
};

static const uint8_t ims_settings_set_reg_mgr_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_reg_mgr_new_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_reg_mgr_new_config_req_msg is empty
 * static const uint8_t ims_settings_get_reg_mgr_new_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_reg_mgr_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, reg_irat_hystersis_timer) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, reg_irat_hystersis_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, reg_irat_hystersis_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, reg_retry_base_time) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, reg_retry_base_time_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, reg_retry_base_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, reg_retry_max_time) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, reg_retry_max_time_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, reg_retry_max_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, regmgr_primary_pcscf) - QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, regmgr_primary_pcscf_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, regmgr_primary_pcscf),
  IMS_SETTINGS_RM_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, regmgr_pcscf_port) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, regmgr_pcscf_port_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_reg_mgr_new_config_rsp_msg_v01, regmgr_pcscf_port)
};

static const uint8_t ims_settings_reg_mgr_new_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, reg_irat_hystersis_timer) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, reg_irat_hystersis_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, reg_irat_hystersis_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, reg_retry_base_time) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, reg_retry_base_time_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, reg_retry_base_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, reg_retry_max_time) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, reg_retry_max_time_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, reg_retry_max_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, regmgr_primary_pcscf) - QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, regmgr_primary_pcscf_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_reg_mgr_new_config_ind_msg_v01, regmgr_primary_pcscf),
  IMS_SETTINGS_RM_STRING_LEN_MAX_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_reg_mgr_new_config_ind_msg_v01, regmgr_pcscf_port) - QMI_IDL_OFFSET16RELATIVE(ims_settings_reg_mgr_new_config_ind_msg_v01, regmgr_pcscf_port_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_reg_mgr_new_config_ind_msg_v01, regmgr_pcscf_port)
};

static const uint8_t ims_settings_set_xcap_new_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, ip_addr_type_wwan) - QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, ip_addr_type_wwan_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, ip_addr_type_wwan),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, xcap_apn_wwan) - QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, xcap_apn_wwan_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, xcap_apn_wwan),
  IMS_SETTINGS_XCAP_APN_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, ip_addr_type_wlan) - QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, ip_addr_type_wlan_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, ip_addr_type_wlan),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, xcap_apn_wlan) - QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, xcap_apn_wlan_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_req_msg_v01, xcap_apn_wlan),
  IMS_SETTINGS_XCAP_APN_NAME_LEN_V01
};

static const uint8_t ims_settings_set_xcap_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_xcap_new_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_xcap_new_config_req_msg is empty
 * static const uint8_t ims_settings_get_xcap_new_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_xcap_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, ip_addr_type_wwan) - QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, ip_addr_type_wwan_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, ip_addr_type_wwan),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, xcap_apn_wwan) - QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, xcap_apn_wwan_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, xcap_apn_wwan),
  IMS_SETTINGS_XCAP_APN_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, ip_addr_type_wlan) - QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, ip_addr_type_wlan_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, ip_addr_type_wlan),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, xcap_apn_wlan) - QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, xcap_apn_wlan_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_xcap_new_config_rsp_msg_v01, xcap_apn_wlan),
  IMS_SETTINGS_XCAP_APN_NAME_LEN_V01
};

static const uint8_t ims_settings_xcap_new_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, ip_addr_type_wwan) - QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, ip_addr_type_wwan_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, ip_addr_type_wwan),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, xcap_apn_wwan) - QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, xcap_apn_wwan_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, xcap_apn_wwan),
  IMS_SETTINGS_XCAP_APN_NAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, ip_addr_type_wlan) - QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, ip_addr_type_wlan_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, ip_addr_type_wlan),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, xcap_apn_wlan) - QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, xcap_apn_wlan_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_xcap_new_config_ind_msg_v01, xcap_apn_wlan),
  IMS_SETTINGS_XCAP_APN_NAME_LEN_V01
};

static const uint8_t ims_settings_set_sms_new_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_req_msg_v01, sms_psi) - QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_req_msg_v01, sms_psi_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_req_msg_v01, sms_psi),
  IMS_SETTINGS_IMS_SMS_PSI_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_req_msg_v01, sms_format) - QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_req_msg_v01, sms_format_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_req_msg_v01, sms_format)
};

static const uint8_t ims_settings_set_sms_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_sms_new_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_sms_new_config_req_msg is empty
 * static const uint8_t ims_settings_get_sms_new_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_sms_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, sms_psi) - QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, sms_psi_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, sms_psi),
  IMS_SETTINGS_IMS_SMS_PSI_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, sms_format) - QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, sms_format_valid)),
  0x12,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_sms_new_config_rsp_msg_v01, sms_format)
};

static const uint8_t ims_settings_sms_new_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sms_new_config_ind_msg_v01, sms_psi) - QMI_IDL_OFFSET8(ims_settings_sms_new_config_ind_msg_v01, sms_psi_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_sms_new_config_ind_msg_v01, sms_psi),
  IMS_SETTINGS_IMS_SMS_PSI_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_sms_new_config_ind_msg_v01, sms_format) - QMI_IDL_OFFSET8(ims_settings_sms_new_config_ind_msg_v01, sms_format_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_sms_new_config_ind_msg_v01, sms_format)
};

static const uint8_t ims_settings_set_voice_dynamic_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, audio_feature_tag) - QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, audio_feature_tag_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, audio_feature_tag),
  IMS_SETTINGS_FEATURE_TAG_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, session_expires) - QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, session_expires_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, session_expires),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, min_session_expiry) - QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, min_session_expiry_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, min_session_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, conference_uri) - QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, conference_uri_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_req_msg_v01, conference_uri),
  IMS_SETTINGS_VOICE_CONFRENCE_URI_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, silent_redial_enable) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, silent_redial_enable_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_voice_dynamic_config_req_msg_v01, silent_redial_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, caller_id_mode) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, caller_id_mode_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_voice_dynamic_config_req_msg_v01, caller_id_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, ringing_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, ringing_timer_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_voice_dynamic_config_req_msg_v01, ringing_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, ringback_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, ringback_timer_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_voice_dynamic_config_req_msg_v01, ringback_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, rtp_link_aliveness_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, rtp_link_aliveness_timer_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_voice_dynamic_config_req_msg_v01, rtp_link_aliveness_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, vice_enabled) - QMI_IDL_OFFSET16RELATIVE(ims_settings_set_voice_dynamic_config_req_msg_v01, vice_enabled_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_set_voice_dynamic_config_req_msg_v01, vice_enabled)
};

static const uint8_t ims_settings_set_voice_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_voice_dynamic_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_voice_dynamic_config_req_msg is empty
 * static const uint8_t ims_settings_get_voice_dynamic_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_voice_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, audio_feature_tag) - QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, audio_feature_tag_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, audio_feature_tag),
  IMS_SETTINGS_FEATURE_TAG_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, session_expires) - QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, session_expires_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, session_expires),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, min_session_expiry) - QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, min_session_expiry_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, min_session_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, conference_uri) - QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, conference_uri_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_voice_dynamic_config_rsp_msg_v01, conference_uri),
  IMS_SETTINGS_VOICE_CONFRENCE_URI_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, silent_redial_enable) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, silent_redial_enable_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_voice_dynamic_config_rsp_msg_v01, silent_redial_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, caller_id_mode) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, caller_id_mode_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_voice_dynamic_config_rsp_msg_v01, caller_id_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, ringing_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, ringing_timer_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_voice_dynamic_config_rsp_msg_v01, ringing_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, ringback_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, ringback_timer_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_voice_dynamic_config_rsp_msg_v01, ringback_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, rtp_link_aliveness_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, rtp_link_aliveness_timer_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_voice_dynamic_config_rsp_msg_v01, rtp_link_aliveness_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, vice_enabled) - QMI_IDL_OFFSET16RELATIVE(ims_settings_get_voice_dynamic_config_rsp_msg_v01, vice_enabled_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_get_voice_dynamic_config_rsp_msg_v01, vice_enabled)
};

static const uint8_t ims_settings_voice_dynamic_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, audio_feature_tag) - QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, audio_feature_tag_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, audio_feature_tag),
  IMS_SETTINGS_FEATURE_TAG_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, session_expires) - QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, session_expires_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, session_expires),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, min_session_expiry) - QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, min_session_expiry_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, min_session_expiry),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, conference_uri) - QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, conference_uri_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_voice_dynamic_config_ind_msg_v01, conference_uri),
  IMS_SETTINGS_VOICE_CONFRENCE_URI_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, silent_redial_enable) - QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, silent_redial_enable_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_voice_dynamic_config_ind_msg_v01, silent_redial_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, caller_id_mode) - QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, caller_id_mode_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_voice_dynamic_config_ind_msg_v01, caller_id_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, ringing_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, ringing_timer_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_voice_dynamic_config_ind_msg_v01, ringing_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, ringback_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, ringback_timer_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_voice_dynamic_config_ind_msg_v01, ringback_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, rtp_link_aliveness_timer) - QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, rtp_link_aliveness_timer_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_voice_dynamic_config_ind_msg_v01, rtp_link_aliveness_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, vice_enabled) - QMI_IDL_OFFSET16RELATIVE(ims_settings_voice_dynamic_config_ind_msg_v01, vice_enabled_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_settings_voice_dynamic_config_ind_msg_v01, vice_enabled)
};

static const uint8_t ims_settings_set_codec_dynamic_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_octet_aligned_dynamic_pt_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_octet_aligned_dynamic_pt_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_bandwidth_efficient_dynamic_pt_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, dtmf_wb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, dtmf_wb_dynamic_pt_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, dtmf_wb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, dtmf_nb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, dtmf_nb_dynamic_pt_valid)),
  0x15,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, dtmf_nb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_mode_set) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_mode_set_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_mode_set),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_mode_set) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_mode_set_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_mode_set),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_default_mode) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_default_mode_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_default_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_enabled) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, scr_amr_enabled) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, scr_amr_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, scr_amr_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, scr_amr_wb_enabled) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, scr_amr_wb_enabled_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, scr_amr_wb_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, evs_primary) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, evs_primary_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, evs_primary),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, evs_sdp_codec) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, evs_sdp_codec_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, evs_sdp_codec),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, qvp_rtp_sdp_bw) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, qvp_rtp_sdp_bw_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, qvp_rtp_sdp_bw),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_mode_set_mask) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_mode_set_mask_valid)),
  0x1F,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_mode_set_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_mode_set_mask) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_mode_set_mask_valid)),
  0x20,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_req_msg_v01, amr_wb_mode_set_mask)
};

static const uint8_t ims_settings_set_codec_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_codec_dynamic_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_codec_dynamic_config_req_msg is empty
 * static const uint8_t ims_settings_get_codec_dynamic_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_codec_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_octet_aligned_dynamic_pt_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_octet_aligned_dynamic_pt_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_bandwidth_efficient_dynamic_pt_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, dtmf_wb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, dtmf_wb_dynamic_pt_valid)),
  0x15,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, dtmf_wb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, dtmf_nb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, dtmf_nb_dynamic_pt_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, dtmf_nb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_mode_set) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_mode_set_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_mode_set),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_mode_set) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_mode_set_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_mode_set),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_default_mode) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_default_mode_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_default_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_enabled) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, scr_amr_enabled) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, scr_amr_enabled_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, scr_amr_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, scr_amr_wb_enabled) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, scr_amr_wb_enabled_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, scr_amr_wb_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, evs_primary) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, evs_primary_valid)),
  0x1D,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, evs_primary),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, evs_sdp_codec) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, evs_sdp_codec_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, evs_sdp_codec),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, qvp_rtp_sdp_bw) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, qvp_rtp_sdp_bw_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, qvp_rtp_sdp_bw),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_mode_set_mask) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_mode_set_mask_valid)),
  0x20,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_mode_set_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_mode_set_mask) - QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_mode_set_mask_valid)),
  0x21,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_codec_dynamic_config_rsp_msg_v01, amr_wb_mode_set_mask)
};

static const uint8_t ims_settings_codec_dynamic_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_octet_aligned_dynamic_pt_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_octet_aligned_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_octet_aligned_dynamic_pt_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_octet_aligned_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_bandwidth_efficient_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_bandwidth_efficient_dynamic_pt_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_bandwidth_efficient_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, dtmf_wb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, dtmf_wb_dynamic_pt_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, dtmf_wb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, dtmf_nb_dynamic_pt) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, dtmf_nb_dynamic_pt_valid)),
  0x15,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, dtmf_nb_dynamic_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_mode_set) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_mode_set_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_mode_set),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_mode_set) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_mode_set_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_mode_set),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_default_mode) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_default_mode_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_default_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_enabled) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, scr_amr_enabled) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, scr_amr_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, scr_amr_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, scr_amr_wb_enabled) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, scr_amr_wb_enabled_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, scr_amr_wb_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, evs_primary) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, evs_primary_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, evs_primary),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, evs_sdp_codec) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, evs_sdp_codec_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, evs_sdp_codec),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, qvp_rtp_sdp_bw) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, qvp_rtp_sdp_bw_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, qvp_rtp_sdp_bw),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_mode_set_mask) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_mode_set_mask_valid)),
  0x1F,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_mode_set_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_mode_set_mask) - QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_mode_set_mask_valid)),
  0x20,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_codec_dynamic_config_ind_msg_v01, amr_wb_mode_set_mask)
};

static const uint8_t ims_settings_set_video_dynamic_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_req_msg_v01, video_feature_tag) - QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_req_msg_v01, video_feature_tag_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_req_msg_v01, video_feature_tag),
  IMS_SETTINGS_FEATURE_TAG_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_req_msg_v01, vt_quality_selector) - QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_req_msg_v01, vt_quality_selector_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_req_msg_v01, vt_quality_selector)
};

static const uint8_t ims_settings_set_video_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_video_dynamic_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_video_dynamic_config_req_msg is empty
 * static const uint8_t ims_settings_get_video_dynamic_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_video_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, video_feature_tag) - QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, video_feature_tag_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, video_feature_tag),
  IMS_SETTINGS_FEATURE_TAG_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, vt_quality_selector) - QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, vt_quality_selector_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_video_dynamic_config_rsp_msg_v01, vt_quality_selector)
};

static const uint8_t ims_settings_video_dynamic_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_video_dynamic_config_ind_msg_v01, video_feature_tag) - QMI_IDL_OFFSET8(ims_settings_video_dynamic_config_ind_msg_v01, video_feature_tag_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_video_dynamic_config_ind_msg_v01, video_feature_tag),
  IMS_SETTINGS_FEATURE_TAG_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_video_dynamic_config_ind_msg_v01, vt_quality_selector) - QMI_IDL_OFFSET8(ims_settings_video_dynamic_config_ind_msg_v01, vt_quality_selector_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_video_dynamic_config_ind_msg_v01, vt_quality_selector)
};

static const uint8_t ims_settings_set_emer_dynamic_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_call_setup_timer) - QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_call_setup_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_call_setup_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_wlan_call_setup_timer) - QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_wlan_call_setup_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_wlan_call_setup_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_silent_redial_enabled) - QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_silent_redial_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_silent_redial_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_wwan_call_connect_timer) - QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_wwan_call_connect_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_wwan_call_connect_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_wlan_call_connect_timer) - QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_wlan_call_connect_timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_req_msg_v01, e911_wlan_call_connect_timer)
};

static const uint8_t ims_settings_set_emer_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_emer_dynamic_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_emer_dynamic_config_req_msg is empty
 * static const uint8_t ims_settings_get_emer_dynamic_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_emer_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_call_setup_timer) - QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_call_setup_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_call_setup_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_wlan_call_setup_timer) - QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_wlan_call_setup_timer_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_wlan_call_setup_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_silent_redial_enabled) - QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_silent_redial_enabled_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_silent_redial_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_wwan_call_connect_timer) - QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_wwan_call_connect_timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_wwan_call_connect_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_wlan_call_connect_timer) - QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_wlan_call_connect_timer_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_emer_dynamic_config_rsp_msg_v01, e911_wlan_call_connect_timer)
};

static const uint8_t ims_settings_emer_dynamic_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_call_setup_timer) - QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_call_setup_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_call_setup_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_wlan_call_setup_timer) - QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_wlan_call_setup_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_wlan_call_setup_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_silent_redial_enabled) - QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_silent_redial_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_silent_redial_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_wwan_call_connect_timer) - QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_wwan_call_connect_timer_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_wwan_call_connect_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_wlan_call_connect_timer) - QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_wlan_call_connect_timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_emer_dynamic_config_ind_msg_v01, e911_wlan_call_connect_timer)
};

static const uint8_t ims_settings_set_test_dynamic_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_req_msg_v01, e911_testmode_enabled) - QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_req_msg_v01, e911_testmode_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_req_msg_v01, e911_testmode_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_req_msg_v01, conformance_test_enabled) - QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_req_msg_v01, conformance_test_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_req_msg_v01, conformance_test_enabled)
};

static const uint8_t ims_settings_set_test_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_test_dynamic_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_test_dynamic_config_req_msg is empty
 * static const uint8_t ims_settings_get_test_dynamic_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_test_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, e911_testmode_enabled) - QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, e911_testmode_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, e911_testmode_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, conformance_test_enabled) - QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, conformance_test_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_test_dynamic_config_rsp_msg_v01, conformance_test_enabled)
};

static const uint8_t ims_settings_test_dynamic_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_test_dynamic_config_ind_msg_v01, e911_testmode_enabled) - QMI_IDL_OFFSET8(ims_settings_test_dynamic_config_ind_msg_v01, e911_testmode_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_test_dynamic_config_ind_msg_v01, e911_testmode_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_test_dynamic_config_ind_msg_v01, conformance_test_enabled) - QMI_IDL_OFFSET8(ims_settings_test_dynamic_config_ind_msg_v01, conformance_test_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_test_dynamic_config_ind_msg_v01, conformance_test_enabled)
};

static const uint8_t ims_settings_set_rtp_dynamic_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, speech_start_port) - QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, speech_start_port_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, speech_start_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, speech_stop_port) - QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, speech_stop_port_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, speech_stop_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, video_start_port) - QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, video_start_port_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, video_start_port),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, video_end_port) - QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, video_end_port_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_req_msg_v01, video_end_port)
};

static const uint8_t ims_settings_set_rtp_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_rtp_dynamic_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_rtp_dynamic_config_req_msg is empty
 * static const uint8_t ims_settings_get_rtp_dynamic_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_rtp_dynamic_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, speech_start_port) - QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, speech_start_port_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, speech_start_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, speech_stop_port) - QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, speech_stop_port_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, speech_stop_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, video_start_port) - QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, video_start_port_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, video_start_port),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, video_end_port) - QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, video_end_port_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_rtp_dynamic_config_rsp_msg_v01, video_end_port)
};

static const uint8_t ims_settings_rtp_dynamic_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, speech_start_port) - QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, speech_start_port_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, speech_start_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, speech_stop_port) - QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, speech_stop_port_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, speech_stop_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, video_start_port) - QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, video_start_port_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, video_start_port),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, video_end_port) - QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, video_end_port_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rtp_dynamic_config_ind_msg_v01, video_end_port)
};

static const uint8_t ims_settings_set_ims_new_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_ims_new_config_req_msg_v01, ims_user_agent),
  ((IMS_SETTINGS_IMS_USER_AGENT_LEN_V01) & 0xFF), ((IMS_SETTINGS_IMS_USER_AGENT_LEN_V01) >> 8)
};

static const uint8_t ims_settings_set_ims_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_ims_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_ims_new_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_ims_new_config_req_msg is empty
 * static const uint8_t ims_settings_get_ims_new_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_ims_new_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_new_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_new_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_ims_new_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_ims_new_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_new_config_rsp_msg_v01, ims_user_agent) - QMI_IDL_OFFSET8(ims_settings_get_ims_new_config_rsp_msg_v01, ims_user_agent_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_ims_new_config_rsp_msg_v01, ims_user_agent),
  ((IMS_SETTINGS_IMS_USER_AGENT_LEN_V01) & 0xFF), ((IMS_SETTINGS_IMS_USER_AGENT_LEN_V01) >> 8)
};

static const uint8_t ims_settings_ims_new_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_ims_new_config_ind_msg_v01, ims_user_agent),
  ((IMS_SETTINGS_IMS_USER_AGENT_LEN_V01) & 0xFF), ((IMS_SETTINGS_IMS_USER_AGENT_LEN_V01) >> 8)
};

static const uint8_t ims_settings_set_ims_common_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_req_msg_v01, allowed_rat_mask) - QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_req_msg_v01, allowed_rat_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_req_msg_v01, allowed_rat_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_req_msg_v01, phone_context_uri) - QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_req_msg_v01, phone_context_uri_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_req_msg_v01, phone_context_uri),
  IMS_SETTINGS_IMS_STRING_LEN_MAX_V01
};

static const uint8_t ims_settings_set_ims_common_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_ims_common_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_ims_common_config_req_msg is empty
 * static const uint8_t ims_settings_get_ims_common_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_ims_common_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, allowed_rat_mask) - QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, allowed_rat_mask_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, allowed_rat_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, phone_context_uri) - QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, phone_context_uri_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_ims_common_config_rsp_msg_v01, phone_context_uri),
  IMS_SETTINGS_IMS_STRING_LEN_MAX_V01
};

static const uint8_t ims_settings_ims_common_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_common_config_ind_msg_v01, allowed_rat_mask) - QMI_IDL_OFFSET8(ims_settings_ims_common_config_ind_msg_v01, allowed_rat_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_common_config_ind_msg_v01, allowed_rat_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_common_config_ind_msg_v01, phone_context_uri) - QMI_IDL_OFFSET8(ims_settings_ims_common_config_ind_msg_v01, phone_context_uri_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_ims_common_config_ind_msg_v01, phone_context_uri),
  IMS_SETTINGS_IMS_STRING_LEN_MAX_V01
};

static const uint8_t ims_settings_set_ims_service_enable_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, volte_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, volte_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, volte_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, videotelephony_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, videotelephony_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, videotelephony_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, mobile_data_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, mobile_data_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, mobile_data_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, service_mask_by_network_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, service_mask_by_network_enabled_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, service_mask_by_network_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, wifi_calling_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, wifi_calling_enabled_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, wifi_calling_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, call_mode_preference) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, call_mode_preference_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, call_mode_preference),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, wifi_calling_in_roaming_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, wifi_calling_in_roaming_enabled_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, wifi_calling_in_roaming_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, wifi_provisioning_id) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, wifi_provisioning_id_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, wifi_provisioning_id),
  IMS_SETTINGS_WIFI_PROVISION_ID_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, ims_service_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, ims_service_enabled_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, ims_service_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, ut_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, ut_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, ut_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, sms_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, sms_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, sms_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, dan_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, dan_enabled_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, dan_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, ussd_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, ussd_enabled_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, ussd_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, mwi_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, mwi_enabled_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, mwi_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, presence_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, presence_enabled_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, presence_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, autoconfig_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, autoconfig_enabled_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, autoconfig_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, xdm_client_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, xdm_client_enabled_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, xdm_client_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, rcs_messaging_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, rcs_messaging_enabled_valid)),
  0x21,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, rcs_messaging_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, call_mode_pref_roam_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, call_mode_pref_roam_enabled_valid)),
  0x22,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, call_mode_pref_roam_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, rtt_enabled) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, rtt_enabled_valid)),
  0x23,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_req_msg_v01, rtt_enabled)
};

static const uint8_t ims_settings_set_ims_service_enable_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_ims_service_enable_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_ims_service_enable_config_req_msg is empty
 * static const uint8_t ims_settings_get_ims_service_enable_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_ims_service_enable_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, volte_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, volte_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, volte_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, videotelephony_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, videotelephony_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, videotelephony_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, mobile_data_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, mobile_data_enabled_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, mobile_data_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, service_mask_by_network_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, service_mask_by_network_enabled_valid)),
  0x14,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, service_mask_by_network_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, wifi_calling_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, wifi_calling_enabled_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, wifi_calling_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, call_mode_preference) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, call_mode_preference_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, call_mode_preference),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, wifi_calling_in_roaming_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, wifi_calling_in_roaming_enabled_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, wifi_calling_in_roaming_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, wifi_provisioning_id) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, wifi_provisioning_id_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, wifi_provisioning_id),
  IMS_SETTINGS_WIFI_PROVISION_ID_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, ims_service_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, ims_service_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, ims_service_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, ut_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, ut_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, ut_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, sms_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, sms_enabled_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, sms_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, dan_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, dan_enabled_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, dan_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, ussd_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, ussd_enabled_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, ussd_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, mwi_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, mwi_enabled_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, mwi_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, presence_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, presence_enabled_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, presence_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, autoconfig_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, autoconfig_enabled_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, autoconfig_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, xdm_client_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, xdm_client_enabled_valid)),
  0x21,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, xdm_client_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, rcs_messaging_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, rcs_messaging_enabled_valid)),
  0x22,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, rcs_messaging_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, call_mode_pref_roam_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, call_mode_pref_roam_enabled_valid)),
  0x23,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, call_mode_pref_roam_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, rtt_enabled) - QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, rtt_enabled_valid)),
  0x24,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_ims_service_enable_config_rsp_msg_v01, rtt_enabled)
};

static const uint8_t ims_settings_ims_service_enable_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, volte_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, volte_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, volte_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, videotelephony_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, videotelephony_enabled_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, videotelephony_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, mobile_data_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, mobile_data_enabled_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, mobile_data_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, service_mask_by_network_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, service_mask_by_network_enabled_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, service_mask_by_network_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, wifi_calling_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, wifi_calling_enabled_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, wifi_calling_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, call_mode_preference) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, call_mode_preference_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, call_mode_preference),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, wifi_calling_in_roaming_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, wifi_calling_in_roaming_enabled_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, wifi_calling_in_roaming_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, wifi_provisioning_id) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, wifi_provisioning_id_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, wifi_provisioning_id),
  IMS_SETTINGS_WIFI_PROVISION_ID_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, ims_service_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, ims_service_enabled_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, ims_service_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, ut_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, ut_enabled_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, ut_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, sms_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, sms_enabled_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, sms_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, dan_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, dan_enabled_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, dan_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, ussd_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, ussd_enabled_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, ussd_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, mwi_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, mwi_enabled_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, mwi_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, presence_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, presence_enabled_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, presence_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, autoconfig_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, autoconfig_enabled_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, autoconfig_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, xdm_client_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, xdm_client_enabled_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, xdm_client_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, rcs_messaging_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, rcs_messaging_enabled_valid)),
  0x21,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, rcs_messaging_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, call_mode_pref_roam_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, call_mode_pref_roam_enabled_valid)),
  0x22,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, call_mode_pref_roam_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, rtt_enabled) - QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, rtt_enabled_valid)),
  0x23,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_ims_service_enable_config_ind_msg_v01, rtt_enabled)
};

static const uint8_t ims_settings_set_qipcall_evs_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_req_msg_v01, evs_primary) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_req_msg_v01, evs_primary_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_req_msg_v01, evs_primary),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_req_msg_v01, evs_sdp_codec) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_req_msg_v01, evs_sdp_codec_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_req_msg_v01, evs_sdp_codec),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_req_msg_v01, qvp_rtp_sdp_bw) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_req_msg_v01, qvp_rtp_sdp_bw_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_req_msg_v01, qvp_rtp_sdp_bw)
};

static const uint8_t ims_settings_set_qipcall_evs_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_set_qipcall_evs_config_rsp_msg_v01, settings_resp)
};

/*
 * ims_settings_get_qipcall_evs_config_req_msg is empty
 * static const uint8_t ims_settings_get_qipcall_evs_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_settings_get_qipcall_evs_config_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, settings_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, evs_primary) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, evs_primary_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, evs_primary),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, evs_sdp_codec) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, evs_sdp_codec_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, evs_sdp_codec),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, qvp_rtp_sdp_bw) - QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, qvp_rtp_sdp_bw_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_get_qipcall_evs_config_rsp_msg_v01, qvp_rtp_sdp_bw)
};

static const uint8_t ims_settings_qipcall_evs_config_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_evs_config_ind_msg_v01, evs_primary) - QMI_IDL_OFFSET8(ims_settings_qipcall_evs_config_ind_msg_v01, evs_primary_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_evs_config_ind_msg_v01, evs_primary),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_evs_config_ind_msg_v01, evs_sdp_codec) - QMI_IDL_OFFSET8(ims_settings_qipcall_evs_config_ind_msg_v01, evs_sdp_codec_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_evs_config_ind_msg_v01, evs_sdp_codec),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_qipcall_evs_config_ind_msg_v01, qvp_rtp_sdp_bw) - QMI_IDL_OFFSET8(ims_settings_qipcall_evs_config_ind_msg_v01, qvp_rtp_sdp_bw_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_settings_qipcall_evs_config_ind_msg_v01, qvp_rtp_sdp_bw)
};

static const uint8_t ims_settings_send_rtt_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_send_rtt_req_msg_v01, rtt_data),
  IMS_SETTINGS_RTT_DATA_LEN_MAX_V01,
  QMI_IDL_OFFSET8(ims_settings_send_rtt_req_msg_v01, rtt_data) - QMI_IDL_OFFSET8(ims_settings_send_rtt_req_msg_v01, rtt_data_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_send_rtt_req_msg_v01, rtt_optional_data) - QMI_IDL_OFFSET8(ims_settings_send_rtt_req_msg_v01, rtt_optional_data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_send_rtt_req_msg_v01, rtt_optional_data),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_send_rtt_req_msg_v01, rtt_reference_number) - QMI_IDL_OFFSET8(ims_settings_send_rtt_req_msg_v01, rtt_reference_number_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_send_rtt_req_msg_v01, rtt_reference_number)
};

static const uint8_t ims_settings_send_rtt_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_send_rtt_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_send_rtt_resp_msg_v01, settings_resp) - QMI_IDL_OFFSET8(ims_settings_send_rtt_resp_msg_v01, settings_resp_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ims_settings_send_rtt_resp_msg_v01, settings_resp)
};

static const uint8_t ims_settings_rtt_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rtt_ind_msg_v01, rtt_data),
  IMS_SETTINGS_RTT_DATA_LEN_MAX_V01,
  QMI_IDL_OFFSET8(ims_settings_rtt_ind_msg_v01, rtt_data) - QMI_IDL_OFFSET8(ims_settings_rtt_ind_msg_v01, rtt_data_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rtt_ind_msg_v01, rtt_optional_data) - QMI_IDL_OFFSET8(ims_settings_rtt_ind_msg_v01, rtt_optional_data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_settings_rtt_ind_msg_v01, rtt_optional_data),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_settings_rtt_ind_msg_v01, rtt_reference_number) - QMI_IDL_OFFSET8(ims_settings_rtt_ind_msg_v01, rtt_reference_number_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_settings_rtt_ind_msg_v01, rtt_reference_number)
};

static const uint8_t ims_set_bind_subscription_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_set_bind_subscription_req_msg_v01, subscription_type)
};

static const uint8_t ims_set_bind_subscription_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_set_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_set_bind_subscription_resp_msg_v01, common_resp) - QMI_IDL_OFFSET8(ims_set_bind_subscription_resp_msg_v01, common_resp_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_set_bind_subscription_resp_msg_v01, common_resp)
};

/*
 * ims_get_active_subscription_status_req_msg is empty
 * static const uint8_t ims_get_active_subscription_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ims_get_active_subscription_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, common_resp) - QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, common_resp_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, common_resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, active_on_primary_subscription) - QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, active_on_primary_subscription_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, active_on_primary_subscription),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, active_on_secondary_subscription) - QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, active_on_secondary_subscription_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, active_on_secondary_subscription),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, active_on_tertiary_subscription) - QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, active_on_tertiary_subscription_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_get_active_subscription_status_resp_msg_v01, active_on_tertiary_subscription)
};

static const uint8_t ims_active_subscription_ind_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_active_subscription_ind_v01, isActive)
};

/* Type Table */
static const qmi_idl_type_table_entry  imss_type_table_v01[] = {
  {sizeof(ims_settings_pol_man_rat_apn_info_v01), ims_settings_pol_man_rat_apn_info_data_v01},
  {sizeof(ims_settings_pol_mgr_rat_apn_fb_sp_info_v01), ims_settings_pol_mgr_rat_apn_fb_sp_info_data_v01},
  {sizeof(ims_settings_pol_mgr_apn_name_v01), ims_settings_pol_mgr_apn_name_data_v01},
  {sizeof(ims_settings_qdj_optimization2_info_v01), ims_settings_qdj_optimization2_info_data_v01},
  {sizeof(ims_settings_vt_h264_info_v01), ims_settings_vt_h264_info_data_v01},
  {sizeof(ims_settings_pol_mgr_rat_ipsec_info_v01), ims_settings_pol_mgr_rat_ipsec_info_data_v01},
  {sizeof(ims_settings_csd_codec_info_v01), ims_settings_csd_codec_info_data_v01},
  {sizeof(ims_settings_pol_man_rat_apn_info_v02_v01), ims_settings_pol_man_rat_apn_info_v02_data_v01},
  {sizeof(ims_settings_rtt_optional_data_v01), ims_settings_rtt_optional_data_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry imss_message_table_v01[] = {
  {sizeof(ims_settings_set_sip_config_req_msg_v01), ims_settings_set_sip_config_req_msg_data_v01},
  {sizeof(ims_settings_set_sip_config_rsp_msg_v01), ims_settings_set_sip_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_reg_mgr_config_req_msg_v01), ims_settings_set_reg_mgr_config_req_msg_data_v01},
  {sizeof(ims_settings_set_reg_mgr_config_rsp_msg_v01), ims_settings_set_reg_mgr_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_sms_config_req_msg_v01), ims_settings_set_sms_config_req_msg_data_v01},
  {sizeof(ims_settings_set_sms_config_rsp_msg_v01), ims_settings_set_sms_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_user_config_req_msg_v01), ims_settings_set_user_config_req_msg_data_v01},
  {sizeof(ims_settings_set_user_config_rsp_msg_v01), ims_settings_set_user_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_voip_config_req_msg_v01), ims_settings_set_voip_config_req_msg_data_v01},
  {sizeof(ims_settings_set_voip_config_rsp_msg_v01), ims_settings_set_voip_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_presence_config_req_msg_v01), ims_settings_set_presence_config_req_msg_data_v01},
  {sizeof(ims_settings_set_presence_config_rsp_msg_v01), ims_settings_set_presence_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_presence_ext_config_req_msg_v01), ims_settings_set_presence_ext_config_req_msg_data_v01},
  {sizeof(ims_settings_set_presence_ext_config_rsp_msg_v01), ims_settings_set_presence_ext_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_media_config_req_msg_v01), ims_settings_set_media_config_req_msg_data_v01},
  {sizeof(ims_settings_set_media_config_rsp_msg_v01), ims_settings_set_media_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_qipcall_config_req_msg_v01), ims_settings_set_qipcall_config_req_msg_data_v01},
  {sizeof(ims_settings_set_qipcall_config_rsp_msg_v01), ims_settings_set_qipcall_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_reg_mgr_extended_config_req_msg_v01), ims_settings_set_reg_mgr_extended_config_req_msg_data_v01},
  {sizeof(ims_settings_set_reg_mgr_extended_config_rsp_msg_v01), ims_settings_set_reg_mgr_extended_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_pol_mgr_config_req_msg_v01), ims_settings_set_pol_mgr_config_req_msg_data_v01},
  {sizeof(ims_settings_set_pol_mgr_config_rsp_msg_v01), ims_settings_set_pol_mgr_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_rcs_sm_config_req_msg_v01), ims_settings_set_rcs_sm_config_req_msg_data_v01},
  {sizeof(ims_settings_set_rcs_sm_config_rsp_msg_v01), ims_settings_set_rcs_sm_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_ut_config_req_msg_v01), ims_settings_set_ut_config_req_msg_data_v01},
  {sizeof(ims_settings_set_ut_config_rsp_msg_v01), ims_settings_set_ut_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_client_provisioning_config_req_msg_v01), ims_settings_set_client_provisioning_config_req_msg_data_v01},
  {sizeof(ims_settings_set_client_provisioning_config_rsp_msg_v01), ims_settings_set_client_provisioning_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_apcs_complete_config_req_msg_v01), 0},
  {sizeof(ims_settings_set_apcs_complete_config_rsp_msg_v01), ims_settings_set_apcs_complete_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_sip_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_sip_config_rsp_msg_v01), ims_settings_get_sip_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_reg_mgr_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_reg_mgr_config_rsp_msg_v01), ims_settings_get_reg_mgr_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_sms_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_sms_config_rsp_msg_v01), ims_settings_get_sms_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_user_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_user_config_rsp_msg_v01), ims_settings_get_user_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_voip_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_voip_config_rsp_msg_v01), ims_settings_get_voip_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_presence_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_presence_config_rsp_msg_v01), ims_settings_get_presence_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_presence_ext_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_presence_ext_config_rsp_msg_v01), ims_settings_get_presence_ext_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_media_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_media_config_rsp_msg_v01), ims_settings_get_media_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_qipcall_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_qipcall_config_rsp_msg_v01), ims_settings_get_qipcall_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_reg_mgr_extended_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_reg_mgr_extended_config_rsp_msg_v01), ims_settings_get_reg_mgr_extended_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_pol_mgr_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_pol_mgr_config_rsp_msg_v01), ims_settings_get_pol_mgr_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_rcs_sm_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_rcs_sm_config_rsp_msg_v01), ims_settings_get_rcs_sm_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_ut_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_ut_config_rsp_msg_v01), ims_settings_get_ut_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_client_provisioning_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_client_provisioning_config_rsp_msg_v01), ims_settings_get_client_provisioning_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_sip_read_only_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_sip_read_only_config_rsp_msg_v01), ims_settings_get_sip_read_only_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_network_read_only_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_network_read_only_config_rsp_msg_v01), ims_settings_get_network_read_only_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_voip_read_only_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_voip_read_only_config_rsp_msg_v01), ims_settings_get_voip_read_only_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_user_read_only_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_user_read_only_config_rsp_msg_v01), ims_settings_get_user_read_only_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_reg_mgr_read_only_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_reg_mgr_read_only_config_rsp_msg_v01), ims_settings_get_reg_mgr_read_only_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_rcs_auto_config_read_only_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_v01), ims_settings_get_rcs_auto_config_read_only_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_rcs_imscore_auto_config_read_only_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_v01), ims_settings_get_rcs_imscore_auto_config_read_only_config_rsp_msg_data_v01},
  {sizeof(ims_settings_config_ind_reg_req_msg_v01), ims_settings_config_ind_reg_req_msg_data_v01},
  {sizeof(ims_settings_config_ind_reg_rsp_msg_v01), ims_settings_config_ind_reg_rsp_msg_data_v01},
  {sizeof(ims_settings_sip_config_ind_msg_v01), ims_settings_sip_config_ind_msg_data_v01},
  {sizeof(ims_settings_reg_mgr_config_ind_msg_v01), ims_settings_reg_mgr_config_ind_msg_data_v01},
  {sizeof(ims_settings_sms_config_ind_msg_v01), ims_settings_sms_config_ind_msg_data_v01},
  {sizeof(ims_settings_user_config_ind_msg_v01), ims_settings_user_config_ind_msg_data_v01},
  {sizeof(ims_settings_voip_config_ind_msg_v01), ims_settings_voip_config_ind_msg_data_v01},
  {sizeof(ims_settings_presence_config_ind_msg_v01), ims_settings_presence_config_ind_msg_data_v01},
  {sizeof(ims_settings_presence_ext_config_ind_msg_v01), ims_settings_presence_ext_config_ind_msg_data_v01},
  {sizeof(ims_settings_media_config_ind_msg_v01), ims_settings_media_config_ind_msg_data_v01},
  {sizeof(ims_settings_qipcall_config_ind_msg_v01), ims_settings_qipcall_config_ind_msg_data_v01},
  {sizeof(ims_settings_reg_mgr_extended_config_ind_msg_v01), ims_settings_reg_mgr_extended_config_ind_msg_data_v01},
  {sizeof(ims_settings_pol_mgr_config_ind_msg_v01), ims_settings_pol_mgr_config_ind_msg_data_v01},
  {sizeof(ims_settings_rcs_sm_config_ind_msg_v01), ims_settings_rcs_sm_config_ind_msg_data_v01},
  {sizeof(ims_settings_ut_config_ind_msg_v01), ims_settings_ut_config_ind_msg_data_v01},
  {sizeof(ims_settings_client_provisioning_config_ind_msg_v01), ims_settings_client_provisioning_config_ind_msg_data_v01},
  {sizeof(ims_settings_sip_read_only_config_ind_msg_v01), ims_settings_sip_read_only_config_ind_msg_data_v01},
  {sizeof(ims_settings_network_read_only_config_ind_msg_v01), ims_settings_network_read_only_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_vt_config_req_msg_v01), ims_settings_set_vt_config_req_msg_data_v01},
  {sizeof(ims_settings_set_vt_config_rsp_msg_v01), ims_settings_set_vt_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_vt_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_vt_config_rsp_msg_v01), ims_settings_get_vt_config_rsp_msg_data_v01},
  {sizeof(ims_settings_vt_config_ind_msg_v01), ims_settings_vt_config_ind_msg_data_v01},
  {sizeof(ims_settings_get_acs_network_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_acs_network_config_rsp_msg_v01), ims_settings_get_acs_network_config_rsp_msg_data_v01},
  {sizeof(ims_settings_acs_network_config_ind_msg_v01), ims_settings_acs_network_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_acs_network_config_req_msg_v01), ims_settings_set_acs_network_config_req_msg_data_v01},
  {sizeof(ims_settings_set_acs_network_config_rsp_msg_v01), ims_settings_set_acs_network_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_handover_config_req_msg_v01), ims_settings_set_handover_config_req_msg_data_v01},
  {sizeof(ims_settings_set_handover_config_rsp_msg_v01), ims_settings_set_handover_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_handover_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_handover_config_rsp_msg_v01), ims_settings_get_handover_config_rsp_msg_data_v01},
  {sizeof(ims_settings_handover_config_ind_msg_v01), ims_settings_handover_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_qipcall_vice_config_req_msg_v01), ims_settings_set_qipcall_vice_config_req_msg_data_v01},
  {sizeof(ims_settings_set_qipcall_vice_config_rsp_msg_v01), ims_settings_set_qipcall_vice_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_qipcall_vice_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_qipcall_vice_config_rsp_msg_v01), ims_settings_get_qipcall_vice_config_rsp_msg_data_v01},
  {sizeof(ims_settings_qipcall_vice_config_ind_msg_v01), ims_settings_qipcall_vice_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_pol_mgr_rat_config_req_msg_v01), ims_settings_set_pol_mgr_rat_config_req_msg_data_v01},
  {sizeof(ims_settings_set_pol_mgr_rat_config_rsp_msg_v01), ims_settings_set_pol_mgr_rat_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_pol_mgr_rat_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_pol_mgr_rat_config_rsp_msg_v01), ims_settings_get_pol_mgr_rat_config_rsp_msg_data_v01},
  {sizeof(ims_settings_pol_mgr_rat_config_ind_msg_v01), ims_settings_pol_mgr_rat_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_config_csd_req_msg_v01), ims_settings_set_config_csd_req_msg_data_v01},
  {sizeof(ims_settings_set_config_csd_rsp_msg_v01), ims_settings_set_config_csd_rsp_msg_data_v01},
  {sizeof(ims_settings_set_sip_new_config_req_msg_v01), ims_settings_set_sip_new_config_req_msg_data_v01},
  {sizeof(ims_settings_set_sip_new_config_rsp_msg_v01), ims_settings_set_sip_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_sip_new_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_sip_new_config_rsp_msg_v01), ims_settings_get_sip_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_set_presence_new_config_req_msg_v01), ims_settings_set_presence_new_config_req_msg_data_v01},
  {sizeof(ims_settings_set_presence_new_config_rsp_msg_v01), ims_settings_set_presence_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_presence_new_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_presence_new_config_rsp_msg_v01), ims_settings_get_presence_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_presence_new_config_ind_msg_v01), ims_settings_presence_new_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_rcs_new_config_req_msg_v01), ims_settings_set_rcs_new_config_req_msg_data_v01},
  {sizeof(ims_settings_set_rcs_new_config_rsp_msg_v01), ims_settings_set_rcs_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_rcs_new_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_rcs_new_config_rsp_msg_v01), ims_settings_get_rcs_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_rcs_new_config_ind_msg_v01), ims_settings_rcs_new_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_reg_mgr_new_config_req_msg_v01), ims_settings_set_reg_mgr_new_config_req_msg_data_v01},
  {sizeof(ims_settings_set_reg_mgr_new_config_rsp_msg_v01), ims_settings_set_reg_mgr_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_reg_mgr_new_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_reg_mgr_new_config_rsp_msg_v01), ims_settings_get_reg_mgr_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_reg_mgr_new_config_ind_msg_v01), ims_settings_reg_mgr_new_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_xcap_new_config_req_msg_v01), ims_settings_set_xcap_new_config_req_msg_data_v01},
  {sizeof(ims_settings_set_xcap_new_config_rsp_msg_v01), ims_settings_set_xcap_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_xcap_new_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_xcap_new_config_rsp_msg_v01), ims_settings_get_xcap_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_xcap_new_config_ind_msg_v01), ims_settings_xcap_new_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_sms_new_config_req_msg_v01), ims_settings_set_sms_new_config_req_msg_data_v01},
  {sizeof(ims_settings_set_sms_new_config_rsp_msg_v01), ims_settings_set_sms_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_sms_new_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_sms_new_config_rsp_msg_v01), ims_settings_get_sms_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_sms_new_config_ind_msg_v01), ims_settings_sms_new_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_voice_dynamic_config_req_msg_v01), ims_settings_set_voice_dynamic_config_req_msg_data_v01},
  {sizeof(ims_settings_set_voice_dynamic_config_rsp_msg_v01), ims_settings_set_voice_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_voice_dynamic_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_voice_dynamic_config_rsp_msg_v01), ims_settings_get_voice_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_voice_dynamic_config_ind_msg_v01), ims_settings_voice_dynamic_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_codec_dynamic_config_req_msg_v01), ims_settings_set_codec_dynamic_config_req_msg_data_v01},
  {sizeof(ims_settings_set_codec_dynamic_config_rsp_msg_v01), ims_settings_set_codec_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_codec_dynamic_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_codec_dynamic_config_rsp_msg_v01), ims_settings_get_codec_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_codec_dynamic_config_ind_msg_v01), ims_settings_codec_dynamic_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_video_dynamic_config_req_msg_v01), ims_settings_set_video_dynamic_config_req_msg_data_v01},
  {sizeof(ims_settings_set_video_dynamic_config_rsp_msg_v01), ims_settings_set_video_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_video_dynamic_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_video_dynamic_config_rsp_msg_v01), ims_settings_get_video_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_video_dynamic_config_ind_msg_v01), ims_settings_video_dynamic_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_emer_dynamic_config_req_msg_v01), ims_settings_set_emer_dynamic_config_req_msg_data_v01},
  {sizeof(ims_settings_set_emer_dynamic_config_rsp_msg_v01), ims_settings_set_emer_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_emer_dynamic_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_emer_dynamic_config_rsp_msg_v01), ims_settings_get_emer_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_emer_dynamic_config_ind_msg_v01), ims_settings_emer_dynamic_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_test_dynamic_config_req_msg_v01), ims_settings_set_test_dynamic_config_req_msg_data_v01},
  {sizeof(ims_settings_set_test_dynamic_config_rsp_msg_v01), ims_settings_set_test_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_test_dynamic_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_test_dynamic_config_rsp_msg_v01), ims_settings_get_test_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_test_dynamic_config_ind_msg_v01), ims_settings_test_dynamic_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_rtp_dynamic_config_req_msg_v01), ims_settings_set_rtp_dynamic_config_req_msg_data_v01},
  {sizeof(ims_settings_set_rtp_dynamic_config_rsp_msg_v01), ims_settings_set_rtp_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_rtp_dynamic_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_rtp_dynamic_config_rsp_msg_v01), ims_settings_get_rtp_dynamic_config_rsp_msg_data_v01},
  {sizeof(ims_settings_rtp_dynamic_config_ind_msg_v01), ims_settings_rtp_dynamic_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_ims_new_config_req_msg_v01), ims_settings_set_ims_new_config_req_msg_data_v01},
  {sizeof(ims_settings_set_ims_new_config_rsp_msg_v01), ims_settings_set_ims_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_ims_new_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_ims_new_config_rsp_msg_v01), ims_settings_get_ims_new_config_rsp_msg_data_v01},
  {sizeof(ims_settings_ims_new_config_ind_msg_v01), ims_settings_ims_new_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_ims_common_config_req_msg_v01), ims_settings_set_ims_common_config_req_msg_data_v01},
  {sizeof(ims_settings_set_ims_common_config_rsp_msg_v01), ims_settings_set_ims_common_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_ims_common_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_ims_common_config_rsp_msg_v01), ims_settings_get_ims_common_config_rsp_msg_data_v01},
  {sizeof(ims_settings_ims_common_config_ind_msg_v01), ims_settings_ims_common_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_ims_service_enable_config_req_msg_v01), ims_settings_set_ims_service_enable_config_req_msg_data_v01},
  {sizeof(ims_settings_set_ims_service_enable_config_rsp_msg_v01), ims_settings_set_ims_service_enable_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_ims_service_enable_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_ims_service_enable_config_rsp_msg_v01), ims_settings_get_ims_service_enable_config_rsp_msg_data_v01},
  {sizeof(ims_settings_ims_service_enable_config_ind_msg_v01), ims_settings_ims_service_enable_config_ind_msg_data_v01},
  {sizeof(ims_settings_set_qipcall_evs_config_req_msg_v01), ims_settings_set_qipcall_evs_config_req_msg_data_v01},
  {sizeof(ims_settings_set_qipcall_evs_config_rsp_msg_v01), ims_settings_set_qipcall_evs_config_rsp_msg_data_v01},
  {sizeof(ims_settings_get_qipcall_evs_config_req_msg_v01), 0},
  {sizeof(ims_settings_get_qipcall_evs_config_rsp_msg_v01), ims_settings_get_qipcall_evs_config_rsp_msg_data_v01},
  {sizeof(ims_settings_qipcall_evs_config_ind_msg_v01), ims_settings_qipcall_evs_config_ind_msg_data_v01},
  {sizeof(ims_settings_send_rtt_req_msg_v01), ims_settings_send_rtt_req_msg_data_v01},
  {sizeof(ims_settings_send_rtt_resp_msg_v01), ims_settings_send_rtt_resp_msg_data_v01},
  {sizeof(ims_settings_rtt_ind_msg_v01), ims_settings_rtt_ind_msg_data_v01},
  {sizeof(ims_set_bind_subscription_req_msg_v01), ims_set_bind_subscription_req_msg_data_v01},
  {sizeof(ims_set_bind_subscription_resp_msg_v01), ims_set_bind_subscription_resp_msg_data_v01},
  {sizeof(ims_get_active_subscription_status_req_msg_v01), 0},
  {sizeof(ims_get_active_subscription_status_resp_msg_v01), ims_get_active_subscription_status_resp_msg_data_v01},
  {sizeof(ims_active_subscription_ind_v01), ims_active_subscription_ind_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object imss_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *imss_qmi_idl_type_table_object_referenced_tables_v01[] =
{&imss_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &ip_multimedia_subsystem_ims_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object imss_qmi_idl_type_table_object_v01 = {
  sizeof(imss_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(imss_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  imss_type_table_v01,
  imss_message_table_v01,
  imss_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry imss_service_command_messages_v01[] = {
  {QMI_IMS_SETTINGS_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_IMS_SETTINGS_GET_SUPPORTED_FIELDS_REQ_V01, QMI_IDL_TYPE16(1, 2), 5},
  {QMI_IMS_SETTINGS_SET_SIP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 0), 163},
  {QMI_IMS_SETTINGS_SET_REG_MGR_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 2), 267},
  {QMI_IMS_SETTINGS_SET_SMS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 4), 397},
  {QMI_IMS_SETTINGS_SET_USER_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 6), 258},
  {QMI_IMS_SETTINGS_SET_VOIP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 8), 265},
  {QMI_IMS_SETTINGS_GET_SIP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 30), 0},
  {QMI_IMS_SETTINGS_GET_REG_MGR_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 32), 0},
  {QMI_IMS_SETTINGS_GET_SMS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 34), 0},
  {QMI_IMS_SETTINGS_GET_USER_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 36), 0},
  {QMI_IMS_SETTINGS_GET_VOIP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 38), 0},
  {QMI_IMS_SETTINGS_CONFIG_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 72), 136},
  {QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 10), 224},
  {QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 40), 0},
  {QMI_IMS_SETTINGS_SET_MEDIA_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 14), 37},
  {QMI_IMS_SETTINGS_GET_MEDIA_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 44), 0},
  {QMI_IMS_SETTINGS_SET_QIPCALL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 16), 87},
  {QMI_IMS_SETTINGS_GET_QIPCALL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 46), 0},
  {QMI_IMS_SETTINGS_GET_SIP_READ_ONLY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 58), 0},
  {QMI_IMS_SETTINGS_GET_NETWORK_READ_ONLY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 60), 0},
  {QMI_IMS_SETTINGS_GET_VOIP_READ_ONLY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 62), 0},
  {QMI_IMS_SETTINGS_GET_USER_READ_ONLY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 64), 0},
  {QMI_IMS_SETTINGS_GET_REG_MGR_READ_ONLY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 66), 0},
  {QMI_IMS_SETTINGS_GET_RCS_AUTO_CONFIG_READ_ONLY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 68), 0},
  {QMI_IMS_SETTINGS_GET_RCS_IMSCORE_AUTO_CONFIG_READ_ONLY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 70), 0},
  {QMI_IMS_SETTINGS_SET_REG_MGR_EXTENDED_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 18), 20},
  {QMI_IMS_SETTINGS_GET_REG_MGR_EXTENDED_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 48), 0},
  {QMI_IMS_SETTINGS_SET_POL_MGR_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 20), 461},
  {QMI_IMS_SETTINGS_GET_POL_MGR_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 50), 0},
  {QMI_IMS_SETTINGS_SET_PRESENCE_EXT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 12), 90},
  {QMI_IMS_SETTINGS_GET_PRESENCE_EXT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 42), 0},
  {QMI_IMS_SETTINGS_SET_RCS_SM_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 22), 141},
  {QMI_IMS_SETTINGS_GET_RCS_SM_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 52), 0},
  {QMI_IMS_SETTINGS_SET_UT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 24), 152},
  {QMI_IMS_SETTINGS_GET_UT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 54), 0},
  {QMI_IMS_SETTINGS_SET_CLIENT_PROVISIONING_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 26), 179},
  {QMI_IMS_SETTINGS_GET_CLIENT_PROVISIONING_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 56), 0},
  {QMI_IMS_SETTINGS_SET_APCS_COMPLETE_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 28), 0},
  {QMI_IMS_SETTINGS_SET_VT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 90), 57},
  {QMI_IMS_SETTINGS_GET_VT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 92), 0},
  {QMI_IMS_SETTINGS_GET_ACS_NETWORK_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 95), 0},
  {QMI_IMS_SETTINGS_SET_ACS_NETWORK_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 98), 4},
  {QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 100), 186},
  {QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 102), 0},
  {QMI_IMS_SETTINGS_SET_QIPCALL_VICE_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 105), 4},
  {QMI_IMS_SETTINGS_GET_QIPCALL_VICE_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 107), 0},
  {QMI_IMS_SETTINGS_SET_POL_MGR_RAT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 110), 7},
  {QMI_IMS_SETTINGS_GET_POL_MGR_RAT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 112), 0},
  {QMI_IMS_SETTINGS_SET_CSD_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 115), 32},
  {QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 117), 109},
  {QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 119), 0},
  {QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 121), 201},
  {QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 123), 0},
  {QMI_IMS_SETTINGS_SET_RCS_SM_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 126), 394},
  {QMI_IMS_SETTINGS_GET_RCS_SM_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 128), 0},
  {QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 131), 278},
  {QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 133), 0},
  {QMI_IMS_SETTINGS_SET_XCAP_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 136), 218},
  {QMI_IMS_SETTINGS_GET_XCAP_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 138), 0},
  {QMI_IMS_SETTINGS_SET_SMS_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 141), 134},
  {QMI_IMS_SETTINGS_GET_SMS_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 143), 0},
  {QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 146), 306},
  {QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 148), 0},
  {QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 151), 104},
  {QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 153), 0},
  {QMI_IMS_SETTINGS_SET_VIDEO_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 156), 137},
  {QMI_IMS_SETTINGS_GET_VIDEO_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 158), 0},
  {QMI_IMS_SETTINGS_SET_EMER_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 161), 32},
  {QMI_IMS_SETTINGS_GET_EMER_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 163), 0},
  {QMI_IMS_SETTINGS_SET_TEST_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 166), 8},
  {QMI_IMS_SETTINGS_GET_TEST_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 168), 0},
  {QMI_IMS_SETTINGS_SET_RTP_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 171), 20},
  {QMI_IMS_SETTINGS_GET_RTP_DYNAMIC_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 173), 0},
  {QMI_IMS_SETTINGS_SET_IMS_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 176), 1026},
  {QMI_IMS_SETTINGS_GET_IMS_NEW_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 178), 0},
  {QMI_IMS_SETTINGS_SET_IMS_COMMON_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 181), 269},
  {QMI_IMS_SETTINGS_GET_IMS_COMMON_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 183), 0},
  {QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 186), 108},
  {QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 188), 0},
  {QMI_IMS_SETTINGS_SET_QIPCALL_EVS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 191), 19},
  {QMI_IMS_SETTINGS_GET_QIPCALL_EVS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 193), 0},
  {QMI_IMS_SETTINGS_SEND_RTT_REQ_V01, QMI_IDL_TYPE16(0, 196), 145},
  {QMI_IMS_SET_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 199), 7},
  {QMI_IMS_GET_ACTIVE_SUBSCRIPTION_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 201), 0}
};

static const qmi_idl_service_message_table_entry imss_service_response_messages_v01[] = {
  {QMI_IMS_SETTINGS_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_IMS_SETTINGS_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_IMS_SETTINGS_SET_SIP_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 1), 11},
  {QMI_IMS_SETTINGS_SET_REG_MGR_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 3), 11},
  {QMI_IMS_SETTINGS_SET_SMS_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 5), 11},
  {QMI_IMS_SETTINGS_SET_USER_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 7), 11},
  {QMI_IMS_SETTINGS_SET_VOIP_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 9), 11},
  {QMI_IMS_SETTINGS_GET_SIP_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 31), 174},
  {QMI_IMS_SETTINGS_GET_REG_MGR_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 33), 278},
  {QMI_IMS_SETTINGS_GET_SMS_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 35), 408},
  {QMI_IMS_SETTINGS_GET_USER_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 37), 269},
  {QMI_IMS_SETTINGS_GET_VOIP_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 39), 208},
  {QMI_IMS_SETTINGS_CONFIG_IND_REG_RSP_V01, QMI_IDL_TYPE16(0, 73), 7},
  {QMI_IMS_SETTINGS_SET_PRESENCE_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 11), 11},
  {QMI_IMS_SETTINGS_GET_PRESENCE_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 41), 235},
  {QMI_IMS_SETTINGS_SET_MEDIA_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 15), 11},
  {QMI_IMS_SETTINGS_GET_MEDIA_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 45), 129},
  {QMI_IMS_SETTINGS_SET_QIPCALL_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 17), 11},
  {QMI_IMS_SETTINGS_GET_QIPCALL_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 47), 98},
  {QMI_IMS_SETTINGS_GET_SIP_READ_ONLY_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 59), 557},
  {QMI_IMS_SETTINGS_GET_NETWORK_READ_ONLY_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 61), 42},
  {QMI_IMS_SETTINGS_GET_VOIP_READ_ONLY_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 63), 20},
  {QMI_IMS_SETTINGS_GET_USER_READ_ONLY_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 65), 400},
  {QMI_IMS_SETTINGS_GET_REG_MGR_READ_ONLY_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 67), 52},
  {QMI_IMS_SETTINGS_GET_RCS_AUTO_CONFIG_READ_ONLY_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 69), 919},
  {QMI_IMS_SETTINGS_GET_RCS_IMSCORE_AUTO_CONFIG_READ_ONLY_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 71), 32},
  {QMI_IMS_SETTINGS_SET_REG_MGR_EXTENDED_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 19), 11},
  {QMI_IMS_SETTINGS_GET_REG_MGR_EXTENDED_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 49), 31},
  {QMI_IMS_SETTINGS_SET_POL_MGR_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 21), 11},
  {QMI_IMS_SETTINGS_GET_POL_MGR_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 51), 605},
  {QMI_IMS_SETTINGS_SET_PRESENCE_EXT_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 13), 11},
  {QMI_IMS_SETTINGS_GET_PRESENCE_EXT_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 43), 101},
  {QMI_IMS_SETTINGS_SET_RCS_SM_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 23), 11},
  {QMI_IMS_SETTINGS_GET_RCS_SM_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 53), 152},
  {QMI_IMS_SETTINGS_SET_UT_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 25), 11},
  {QMI_IMS_SETTINGS_GET_UT_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 55), 163},
  {QMI_IMS_SETTINGS_SET_CLIENT_PROVISIONING_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 27), 11},
  {QMI_IMS_SETTINGS_GET_CLIENT_PROVISIONING_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 57), 190},
  {QMI_IMS_SETTINGS_SET_APCS_COMPLETE_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 29), 11},
  {QMI_IMS_SETTINGS_SET_VT_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 91), 11},
  {QMI_IMS_SETTINGS_GET_VT_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 93), 68},
  {QMI_IMS_SETTINGS_GET_ACS_NETWORK_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 96), 19},
  {QMI_IMS_SETTINGS_SET_ACS_NETWORK_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 99), 11},
  {QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 101), 11},
  {QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 103), 197},
  {QMI_IMS_SETTINGS_SET_QIPCALL_VICE_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 106), 11},
  {QMI_IMS_SETTINGS_GET_QIPCALL_VICE_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 108), 15},
  {QMI_IMS_SETTINGS_SET_POL_MGR_RAT_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 111), 11},
  {QMI_IMS_SETTINGS_GET_POL_MGR_RAT_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 113), 18},
  {QMI_IMS_SETTINGS_SET_CSD_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 116), 7},
  {QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 118), 11},
  {QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 120), 120},
  {QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 122), 11},
  {QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 124), 212},
  {QMI_IMS_SETTINGS_SET_RCS_SM_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 127), 11},
  {QMI_IMS_SETTINGS_GET_RCS_SM_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 129), 405},
  {QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 132), 11},
  {QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 134), 289},
  {QMI_IMS_SETTINGS_SET_XCAP_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 137), 11},
  {QMI_IMS_SETTINGS_GET_XCAP_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 139), 229},
  {QMI_IMS_SETTINGS_SET_SMS_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 142), 11},
  {QMI_IMS_SETTINGS_GET_SMS_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 144), 145},
  {QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 147), 11},
  {QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 149), 317},
  {QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 152), 11},
  {QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 154), 115},
  {QMI_IMS_SETTINGS_SET_VIDEO_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 157), 11},
  {QMI_IMS_SETTINGS_GET_VIDEO_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 159), 148},
  {QMI_IMS_SETTINGS_SET_EMER_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 162), 11},
  {QMI_IMS_SETTINGS_GET_EMER_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 164), 43},
  {QMI_IMS_SETTINGS_SET_TEST_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 167), 11},
  {QMI_IMS_SETTINGS_GET_TEST_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 169), 19},
  {QMI_IMS_SETTINGS_SET_RTP_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 172), 11},
  {QMI_IMS_SETTINGS_GET_RTP_DYNAMIC_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 174), 31},
  {QMI_IMS_SETTINGS_SET_IMS_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 177), 11},
  {QMI_IMS_SETTINGS_GET_IMS_NEW_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 179), 1037},
  {QMI_IMS_SETTINGS_SET_IMS_COMMON_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 182), 11},
  {QMI_IMS_SETTINGS_GET_IMS_COMMON_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 184), 280},
  {QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 187), 11},
  {QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 189), 119},
  {QMI_IMS_SETTINGS_SET_QIPCALL_EVS_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 192), 11},
  {QMI_IMS_SETTINGS_GET_QIPCALL_EVS_CONFIG_RSP_V01, QMI_IDL_TYPE16(0, 194), 30},
  {QMI_IMS_SETTINGS_SEND_RTT_RESP_V01, QMI_IDL_TYPE16(0, 197), 11},
  {QMI_IMS_SET_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 200), 14},
  {QMI_IMS_GET_ACTIVE_SUBSCRIPTION_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 202), 26}
};

static const qmi_idl_service_message_table_entry imss_service_indication_messages_v01[] = {
  {QMI_IMS_SETTINGS_SIP_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 74), 163},
  {QMI_IMS_SETTINGS_REG_MGR_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 75), 267},
  {QMI_IMS_SETTINGS_SMS_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 76), 397},
  {QMI_IMS_SETTINGS_USER_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 77), 258},
  {QMI_IMS_SETTINGS_VOIP_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 78), 197},
  {QMI_IMS_SETTINGS_PRESENCE_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 79), 224},
  {QMI_IMS_SETTINGS_MEDIA_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 81), 118},
  {QMI_IMS_SETTINGS_QIPCALL_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 82), 87},
  {QMI_IMS_SETTINGS_SIP_READ_ONLY_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 88), 546},
  {QMI_IMS_SETTINGS_NETWORK_READ_ONLY_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 89), 31},
  {QMI_IMS_SETTINGS_REG_MGR_EXTENDED_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 83), 20},
  {QMI_IMS_SETTINGS_POL_MGR_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 84), 594},
  {QMI_IMS_SETTINGS_PRESENCE_EXT_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 80), 90},
  {QMI_IMS_SETTINGS_RCS_SM_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 85), 141},
  {QMI_IMS_SETTINGS_UT_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 86), 152},
  {QMI_IMS_SETTINGS_CLIENT_PROVISIONING_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 87), 179},
  {QMI_IMS_SETTINGS_VT_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 94), 57},
  {QMI_IMS_SETTINGS_ACS_NETWORK_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 97), 8},
  {QMI_IMS_SETTINGS_HANDOVER_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 104), 186},
  {QMI_IMS_SETTINGS_QIPCALL_VICE_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 109), 4},
  {QMI_IMS_SETTINGS_POL_MGR_RAT_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 114), 7},
  {QMI_IMS_SETTINGS_PRESENCE_NEW_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 125), 201},
  {QMI_IMS_SETTINGS_RCS_NEW_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 130), 394},
  {QMI_IMS_SETTINGS_REG_MGR_NEW_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 135), 278},
  {QMI_IMS_SETTINGS_SMS_NEW_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 145), 134},
  {QMI_IMS_SETTINGS_VOICE_DYNAMIC_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 150), 306},
  {QMI_IMS_SETTINGS_CODEC_DYNAMIC_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 155), 104},
  {QMI_IMS_SETTINGS_VIDEO_DYNAMIC_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 160), 137},
  {QMI_IMS_SETTINGS_EMER_DYNAMIC_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 165), 32},
  {QMI_IMS_SETTINGS_TEST_DYNAMIC_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 170), 8},
  {QMI_IMS_SETTINGS_RTP_DYNAMIC_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 175), 20},
  {QMI_IMS_SETTINGS_IMS_NEW_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 180), 1026},
  {QMI_IMS_SETTINGS_IMS_COMMON_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 185), 269},
  {QMI_IMS_SETTINGS_IMS_SERVICE_ENABLE_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 190), 108},
  {QMI_IMS_SETTINGS_XCAP_NEW_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 140), 218},
  {QMI_IMS_SETTINGS_QIPCALL_EVS_CONFIG_IND_V01, QMI_IDL_TYPE16(0, 195), 19},
  {QMI_IMS_SETTINGS_RTT_RCVD_DATA_IND_V01, QMI_IDL_TYPE16(0, 198), 145},
  {QMI_IMS_ACTIVE_SUBSCRIPTION_IND_V01, QMI_IDL_TYPE16(0, 203), 4}
};

/*Service Object*/
struct qmi_idl_service_object imss_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x12,
  8204,
  { sizeof(imss_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(imss_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(imss_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { imss_service_command_messages_v01, imss_service_response_messages_v01, imss_service_indication_messages_v01},
  &imss_qmi_idl_type_table_object_v01,
  0x3B,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type imss_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( IMSS_V01_IDL_MAJOR_VERS != idl_maj_version || IMSS_V01_IDL_MINOR_VERS != idl_min_version
       || IMSS_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&imss_qmi_idl_service_object_v01;
}

