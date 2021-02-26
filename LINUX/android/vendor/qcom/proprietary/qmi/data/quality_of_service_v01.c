/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q U A L I T Y _ O F _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the qos service Data structures.

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.5
   It was generated on: Tue May 12 2015 (Spin 0)
   From IDL File: quality_of_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "quality_of_service_v01.h"
#include "common_v01.h"
#include "data_common_v01.h"


/*Type Definitions*/
static const uint8_t qos_report_nw_supp_prof_ch_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_report_nw_supp_prof_ch_type_v01, network_supported_qos_profile_change_reporting),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(qos_report_nw_supp_prof_ch_type_v01, ext_technology_preference),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_nw_supp_qos_prof_type_data_v01[] = {
  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(qos_nw_supp_qos_prof_type_v01, tech_pref),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_nw_supp_qos_prof_type_v01, profile_count),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_nw_supp_qos_prof_type_v01, profile_value),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_status_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_status_info_type_v01, qos_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_status_info_type_v01, qos_status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_status_info_type_v01, qos_event),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_flow_oper_tx_q_level_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_oper_tx_q_level_info_type_v01, current_new_data_cnt),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_oper_tx_q_level_info_type_v01, wm_free_cnt),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_oper_tx_q_level_info_type_v01, total_pending_cnt),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_flow_oper_RMAC3_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_flow_oper_RMAC3_info_type_v01, ps_headroom_payload_size),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_flow_oper_RMAC3_info_type_v01, bucket_level_payload_size),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_flow_oper_RMAC3_info_type_v01, t2p_inflow_payload_size),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t error_codes_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(error_codes_type_v01, bit_number),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(error_codes_type_v01, error_value),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_ipv6_addr_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_ipv6_addr_type_v01, ipv6_address),
  QMI_QOS_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_ipv6_addr_type_v01, prefix_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_ipv4_addr_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_ipv4_addr_type_v01, ipv4_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_ipv4_addr_type_v01, subnet_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_ipv4_tos_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_ipv4_tos_type_v01, val),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_ipv4_tos_type_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_ipv4_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_ipv4_info_type_v01, valid_params),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_ipv4_info_type_v01, src_addr),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_ipv4_info_type_v01, dest_addr),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_ipv4_info_type_v01, tos),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_ipv6_trf_cls_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_ipv6_trf_cls_type_v01, val),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_ipv6_trf_cls_type_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_ipv6_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_ipv6_info_type_v01, valid_params),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_ipv6_info_type_v01, src_addr),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_ipv6_info_type_v01, dest_addr),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_ipv6_info_type_v01, trf_cls),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_ipv6_info_type_v01, flow_label),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_ip_header_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(qos_ip_header_type_v01, ip_version),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_ip_header_type_v01, v4_info),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_ip_header_type_v01, v6_info),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_port_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_port_type_v01, port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_port_type_v01, range),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_port_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_port_info_type_v01, valid_params),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_port_info_type_v01, src_port_info),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_port_info_type_v01, dest_port_info),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_icmp_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_icmp_info_type_v01, valid_params),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_icmp_info_type_v01, type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_icmp_info_type_v01, code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_ipsec_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_ipsec_info_type_v01, valid_params),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_ipsec_info_type_v01, spi),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_xport_header_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_xport_header_type_v01, xport_protocol),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_xport_header_type_v01, tcp_info),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_xport_header_type_v01, udp_info),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_xport_header_type_v01, icmp_info),
  QMI_IDL_TYPE88(0, 15),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_xport_header_type_v01, esp_info),
  QMI_IDL_TYPE88(0, 16),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_xport_header_type_v01, ah_info),
  QMI_IDL_TYPE88(0, 16),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_filter_rule_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_filter_rule_type_v01, ip_info),
  QMI_IDL_TYPE88(0, 12),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_filter_rule_type_v01, xport_info),
  QMI_IDL_TYPE88(0, 17),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_filter_rule_resp_ind_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_filter_rule_resp_ind_type_v01, fltr_info),
  QMI_IDL_TYPE88(0, 18),
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_filter_rule_resp_ind_type_v01, filter_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_filter_rule_resp_ind_type_v01, filter_precedence),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_flow_data_rate_min_max_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_flow_data_rate_min_max_type_v01, data_rate_max),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_flow_data_rate_min_max_type_v01, guaranteed_rate),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_flow_data_rate_token_bucket_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_data_rate_token_bucket_type_v01, peak_rate),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_data_rate_token_bucket_type_v01, token_rate),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_data_rate_token_bucket_type_v01, bucket_size),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_flow_pkt_err_rate_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_flow_pkt_err_rate_v01, ip_flow_pkt_error_rate_multiplier),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_flow_pkt_err_rate_v01, ip_flow_pkt_error_rate_exponent),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_flow_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, flow_valid_params),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_trf_cls),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, data_rate_min_max),
  QMI_IDL_TYPE88(0, 20),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, data_rate_token_bucket),
  QMI_IDL_TYPE88(0, 21),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_latency),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_jitter),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_pkt_error_rate),
  QMI_IDL_TYPE88(0, 22),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_min_policed_packet_size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_max_allowed_packet_size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_3gpp_residual_bit_error_rate),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_3gpp_traffic_handling_priority),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_3gpp2_profile_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_3gpp2_flow_priority),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_3gpp_im_cn_flag),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_3gpp_sig_ind),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_type_v01, ip_flow_lte_qci),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qos_flow_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_info_type_v01, qos_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_flow_info_type_v01, new_flow),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_flow_info_type_v01, state_change),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * qos_reset_req_msg is empty
 * static const uint8_t qos_reset_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qos_reset_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_reset_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_set_event_report_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_set_event_report_req_msg_v01, global_flow_reporting) - QMI_IDL_OFFSET8(qos_set_event_report_req_msg_v01, global_flow_reporting_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_set_event_report_req_msg_v01, global_flow_reporting),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_set_event_report_req_msg_v01, nw_supp_qos_prof) - QMI_IDL_OFFSET8(qos_set_event_report_req_msg_v01, nw_supp_qos_prof_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_set_event_report_req_msg_v01, nw_supp_qos_prof),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t qos_set_event_report_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_set_event_report_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_event_report_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_event_report_ind_msg_v01, nw_supp_qos_prof_ind) - QMI_IDL_OFFSET8(qos_event_report_ind_msg_v01, nw_supp_qos_prof_ind_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_event_report_ind_msg_v01, nw_supp_qos_prof_ind),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t qos_request_qos_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_request_qos_req_msg_v01, qos_req_opcode) - QMI_IDL_OFFSET8(qos_request_qos_req_msg_v01, qos_req_opcode_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_request_qos_req_msg_v01, qos_req_opcode)
};

static const uint8_t qos_request_qos_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_request_qos_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_request_qos_resp_msg_v01, qos_identifier),
  QMI_QOS_MAX_QOS_SPECS_V01,
  QMI_IDL_OFFSET8(qos_request_qos_resp_msg_v01, qos_identifier) - QMI_IDL_OFFSET8(qos_request_qos_resp_msg_v01, qos_identifier_len)
};

static const uint8_t qos_release_qos_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_release_qos_req_msg_v01, qos_identifier),
  QMI_QOS_MAX_QOS_SPECS_V01,
  QMI_IDL_OFFSET8(qos_release_qos_req_msg_v01, qos_identifier) - QMI_IDL_OFFSET8(qos_release_qos_req_msg_v01, qos_identifier_len)
};

static const uint8_t qos_release_qos_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_release_qos_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_suspend_qos_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_suspend_qos_req_msg_v01, qos_identifier),
  QMI_QOS_MAX_QOS_SPECS_V01,
  QMI_IDL_OFFSET8(qos_suspend_qos_req_msg_v01, qos_identifier) - QMI_IDL_OFFSET8(qos_suspend_qos_req_msg_v01, qos_identifier_len)
};

static const uint8_t qos_suspend_qos_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_suspend_qos_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_resume_qos_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_resume_qos_req_msg_v01, qos_identifier),
  QMI_QOS_MAX_QOS_SPECS_V01,
  QMI_IDL_OFFSET8(qos_resume_qos_req_msg_v01, qos_identifier) - QMI_IDL_OFFSET8(qos_resume_qos_req_msg_v01, qos_identifier_len)
};

static const uint8_t qos_resume_qos_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_resume_qos_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_modify_qos_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_modify_qos_req_msg_v01, request_primary_modify) - QMI_IDL_OFFSET8(qos_modify_qos_req_msg_v01, request_primary_modify_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_modify_qos_req_msg_v01, request_primary_modify)
};

static const uint8_t qos_modify_qos_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_modify_qos_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_get_granted_qos_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_get_granted_qos_req_msg_v01, qos_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_get_granted_qos_req_msg_v01, request_primary) - QMI_IDL_OFFSET8(qos_get_granted_qos_req_msg_v01, request_primary_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_get_granted_qos_req_msg_v01, request_primary)
};

static const uint8_t qos_get_granted_qos_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_granted_qos_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_get_granted_qos_resp_msg_v01, extended_error_code) - QMI_IDL_OFFSET8(qos_get_granted_qos_resp_msg_v01, extended_error_code_valid)),
  0xE0,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_get_granted_qos_resp_msg_v01, extended_error_code)
};

static const uint8_t qos_get_qos_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_get_qos_status_req_msg_v01, qos_id)
};

static const uint8_t qos_get_qos_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_qos_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(qos_get_qos_status_resp_msg_v01, qos_status)
};

static const uint8_t qos_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_status_ind_msg_v01, qos_status_info),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_status_ind_msg_v01, reason_code) - QMI_IDL_OFFSET8(qos_status_ind_msg_v01, reason_code_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_status_ind_msg_v01, reason_code)
};

/*
 * qos_get_qos_network_status_req_msg is empty
 * static const uint8_t qos_get_qos_network_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qos_get_qos_network_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_qos_network_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_get_qos_network_status_resp_msg_v01, network_qos_support_status)
};

static const uint8_t qos_network_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_network_status_ind_msg_v01, network_qos_support_status)
};

static const uint8_t qos_get_nw_supported_qos_profiles_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_req_msg_v01, ext_technology_preference) - QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_req_msg_v01, ext_technology_preference_valid)),
  0x10,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_req_msg_v01, ext_technology_preference)
};

static const uint8_t qos_get_nw_supported_qos_profiles_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_resp_msg_v01, nw_supp_qos_profiles),
  QMI_QOS_MAX_QOS_SPECS_V01,
  QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_resp_msg_v01, nw_supp_qos_profiles) - QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_resp_msg_v01, nw_supp_qos_profiles_len),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_resp_msg_v01, ext_error_info) - QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_resp_msg_v01, ext_error_info_valid)),
  0xE0,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_get_nw_supported_qos_profiles_resp_msg_v01, ext_error_info)
};

static const uint8_t qos_primary_qos_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_primary_qos_event_ind_msg_v01, event)
};

static const uint8_t qos_set_client_ip_pref_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(qos_set_client_ip_pref_req_msg_v01, ip_preference)
};

static const uint8_t qos_set_client_ip_pref_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_set_client_ip_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_bind_data_port_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_bind_data_port_req_msg_v01, ep_id) - QMI_IDL_OFFSET8(qos_bind_data_port_req_msg_v01, ep_id_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_bind_data_port_req_msg_v01, ep_id),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_bind_data_port_req_msg_v01, mux_id) - QMI_IDL_OFFSET8(qos_bind_data_port_req_msg_v01, mux_id_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_bind_data_port_req_msg_v01, mux_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_bind_data_port_req_msg_v01, data_port) - QMI_IDL_OFFSET8(qos_bind_data_port_req_msg_v01, data_port_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qos_bind_data_port_req_msg_v01, data_port)
};

static const uint8_t qos_bind_data_port_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_bind_data_port_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_perform_flow_operation_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, qos_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, requested_operation) - QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, requested_operation_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, requested_operation),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, inactivity_timer) - QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, inactivity_timer_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, inactivity_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, is_primary) - QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, is_primary_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_req_msg_v01, is_primary)
};

static const uint8_t qos_perform_flow_operation_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, tx_q_info) - QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, tx_q_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, tx_q_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, rmac3_info) - QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, rmac3_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, rmac3_info),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, tx_status) - QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, tx_status_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, tx_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, inactivity_timer) - QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, inactivity_timer_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, inactivity_timer),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, error) - QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, error_valid)),
  0xE1,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, error),
  QMI_QOS_MAX_FLOW_BIT_MASK_V01,
  QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, error) - QMI_IDL_OFFSET8(qos_perform_flow_operation_resp_msg_v01, error_len),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t qos_get_filter_params_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_get_filter_params_req_msg_v01, qos_id)
};

static const uint8_t qos_get_filter_params_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_filter_params_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_bind_subscription_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_bind_subscription_req_msg_v01, bind_subs)
};

static const uint8_t qos_bind_subscription_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qos_get_bind_subscription_req_msg is empty
 * static const uint8_t qos_get_bind_subscription_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qos_get_bind_subscription_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_get_bind_subscription_resp_msg_v01, bind_subscription) - QMI_IDL_OFFSET8(qos_get_bind_subscription_resp_msg_v01, bind_subscription_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_get_bind_subscription_resp_msg_v01, bind_subscription)
};

static const uint8_t qos_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_indication_register_req_msg_v01, report_global_qos_flows) - QMI_IDL_OFFSET8(qos_indication_register_req_msg_v01, report_global_qos_flows_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qos_indication_register_req_msg_v01, report_global_qos_flows)
};

static const uint8_t qos_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qos_request_qos_ex_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_request_qos_ex_req_msg_v01, tx_qos_flow) - QMI_IDL_OFFSET8(qos_request_qos_ex_req_msg_v01, tx_qos_flow_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_request_qos_ex_req_msg_v01, tx_qos_flow),
  QMI_QOS_MAX_QOS_FLOWS_V01,
  QMI_IDL_OFFSET8(qos_request_qos_ex_req_msg_v01, tx_qos_flow) - QMI_IDL_OFFSET8(qos_request_qos_ex_req_msg_v01, tx_qos_flow_len),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, rx_qos_flow) - QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, rx_qos_flow_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_request_qos_ex_req_msg_v01, rx_qos_flow),
  QMI_QOS_MAX_QOS_FLOWS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, rx_qos_flow) - QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, rx_qos_flow_len),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, tx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, tx_qos_filter_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_request_qos_ex_req_msg_v01, tx_qos_filter),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, tx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, tx_qos_filter_len),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, rx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, rx_qos_filter_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_request_qos_ex_req_msg_v01, rx_qos_filter),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, rx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_req_msg_v01, rx_qos_filter_len),
  QMI_IDL_TYPE88(0, 18)
};

static const uint8_t qos_request_qos_ex_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, qos_id) - QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, qos_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, qos_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_flow_error) - QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_flow_error_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_flow_error),
  QMI_QOS_MAX_QOS_FLOWS_V01,
  QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_flow_error) - QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_flow_error_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, rx_qos_flow_error) - QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, rx_qos_flow_error_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, rx_qos_flow_error),
  QMI_QOS_MAX_QOS_FLOWS_V01,
  QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, rx_qos_flow_error) - QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, rx_qos_flow_error_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_filter_error) - QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_filter_error_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_filter_error),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_filter_error) - QMI_IDL_OFFSET8(qos_request_qos_ex_resp_msg_v01, tx_qos_filter_error_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_resp_msg_v01, rx_qos_filter_error) - QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_resp_msg_v01, rx_qos_filter_error_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qos_request_qos_ex_resp_msg_v01, rx_qos_filter_error),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_resp_msg_v01, rx_qos_filter_error) - QMI_IDL_OFFSET16RELATIVE(qos_request_qos_ex_resp_msg_v01, rx_qos_filter_error_len)
};

static const uint8_t qos_global_qos_flow_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_global_qos_flow_ind_msg_v01, qos_flow_state),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_global_qos_flow_ind_msg_v01, tx_qos_flow_granted) - QMI_IDL_OFFSET8(qos_global_qos_flow_ind_msg_v01, tx_qos_flow_granted_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_global_qos_flow_ind_msg_v01, tx_qos_flow_granted),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_global_qos_flow_ind_msg_v01, rx_qos_flow_granted) - QMI_IDL_OFFSET8(qos_global_qos_flow_ind_msg_v01, rx_qos_flow_granted_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_global_qos_flow_ind_msg_v01, rx_qos_flow_granted),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, tx_qos_filters) - QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, tx_qos_filters_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_global_qos_flow_ind_msg_v01, tx_qos_filters),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, tx_qos_filters) - QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, tx_qos_filters_len),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, rx_qos_filters) - QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, rx_qos_filters_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_global_qos_flow_ind_msg_v01, rx_qos_filters),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, rx_qos_filters) - QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, rx_qos_filters_len),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, flow_type) - QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, flow_type_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qos_global_qos_flow_ind_msg_v01, flow_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, bearer_id) - QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, bearer_id_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qos_global_qos_flow_ind_msg_v01, bearer_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, fc_seq_num) - QMI_IDL_OFFSET16RELATIVE(qos_global_qos_flow_ind_msg_v01, fc_seq_num_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qos_global_qos_flow_ind_msg_v01, fc_seq_num)
};

static const uint8_t qos_modify_qos_ex_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_req_msg_v01, qos_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_modify_qos_ex_req_msg_v01, tx_qos_flow) - QMI_IDL_OFFSET8(qos_modify_qos_ex_req_msg_v01, tx_qos_flow_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_req_msg_v01, tx_qos_flow),
  QMI_QOS_MAX_QOS_FLOWS_V01,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_req_msg_v01, tx_qos_flow) - QMI_IDL_OFFSET8(qos_modify_qos_ex_req_msg_v01, tx_qos_flow_len),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, rx_qos_flow) - QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, rx_qos_flow_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_modify_qos_ex_req_msg_v01, rx_qos_flow),
  QMI_QOS_MAX_QOS_FLOWS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, rx_qos_flow) - QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, rx_qos_flow_len),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, tx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, tx_qos_filter_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_modify_qos_ex_req_msg_v01, tx_qos_filter),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, tx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, tx_qos_filter_len),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, rx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, rx_qos_filter_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_modify_qos_ex_req_msg_v01, rx_qos_filter),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, rx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_req_msg_v01, rx_qos_filter_len),
  QMI_IDL_TYPE88(0, 18)
};

static const uint8_t qos_modify_qos_ex_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_flow_error) - QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_flow_error_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_flow_error),
  QMI_QOS_MAX_QOS_FLOWS_V01,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_flow_error) - QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_flow_error_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, rx_qos_flow_error) - QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, rx_qos_flow_error_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, rx_qos_flow_error),
  QMI_QOS_MAX_QOS_FLOWS_V01,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, rx_qos_flow_error) - QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, rx_qos_flow_error_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_filter_error) - QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_filter_error_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_filter_error),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_filter_error) - QMI_IDL_OFFSET8(qos_modify_qos_ex_resp_msg_v01, tx_qos_filter_error_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_resp_msg_v01, rx_qos_filter_error) - QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_resp_msg_v01, rx_qos_filter_error_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qos_modify_qos_ex_resp_msg_v01, rx_qos_filter_error),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_resp_msg_v01, rx_qos_filter_error) - QMI_IDL_OFFSET16RELATIVE(qos_modify_qos_ex_resp_msg_v01, rx_qos_filter_error_len)
};

static const uint8_t qos_get_qos_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qos_get_qos_info_req_msg_v01, qos_id)
};

static const uint8_t qos_get_qos_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, flow_status) - QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, flow_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, flow_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, tx_qos_flow) - QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, tx_qos_flow_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, tx_qos_flow),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, rx_qos_flow) - QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, rx_qos_flow_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qos_get_qos_info_resp_msg_v01, rx_qos_flow),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, tx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, tx_qos_filter_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_get_qos_info_resp_msg_v01, tx_qos_filter),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, tx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, tx_qos_filter_len),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, rx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, rx_qos_filter_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qos_get_qos_info_resp_msg_v01, rx_qos_filter),
  QMI_QOS_MAX_QOS_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, rx_qos_filter) - QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, rx_qos_filter_len),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, ext_error_info) - QMI_IDL_OFFSET16RELATIVE(qos_get_qos_info_resp_msg_v01, ext_error_info_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qos_get_qos_info_resp_msg_v01, ext_error_info)
};

/* Type Table */
static const qmi_idl_type_table_entry  qos_type_table_v01[] = {
  {sizeof(qos_report_nw_supp_prof_ch_type_v01), qos_report_nw_supp_prof_ch_type_data_v01},
  {sizeof(qos_nw_supp_qos_prof_type_v01), qos_nw_supp_qos_prof_type_data_v01},
  {sizeof(qos_status_info_type_v01), qos_status_info_type_data_v01},
  {sizeof(qos_flow_oper_tx_q_level_info_type_v01), qos_flow_oper_tx_q_level_info_type_data_v01},
  {sizeof(qos_flow_oper_RMAC3_info_type_v01), qos_flow_oper_RMAC3_info_type_data_v01},
  {sizeof(error_codes_type_v01), error_codes_type_data_v01},
  {sizeof(qos_ipv6_addr_type_v01), qos_ipv6_addr_type_data_v01},
  {sizeof(qos_ipv4_addr_type_v01), qos_ipv4_addr_type_data_v01},
  {sizeof(qos_ipv4_tos_type_v01), qos_ipv4_tos_type_data_v01},
  {sizeof(qos_ipv4_info_type_v01), qos_ipv4_info_type_data_v01},
  {sizeof(qos_ipv6_trf_cls_type_v01), qos_ipv6_trf_cls_type_data_v01},
  {sizeof(qos_ipv6_info_type_v01), qos_ipv6_info_type_data_v01},
  {sizeof(qos_ip_header_type_v01), qos_ip_header_type_data_v01},
  {sizeof(qos_port_type_v01), qos_port_type_data_v01},
  {sizeof(qos_port_info_type_v01), qos_port_info_type_data_v01},
  {sizeof(qos_icmp_info_type_v01), qos_icmp_info_type_data_v01},
  {sizeof(qos_ipsec_info_type_v01), qos_ipsec_info_type_data_v01},
  {sizeof(qos_xport_header_type_v01), qos_xport_header_type_data_v01},
  {sizeof(qos_filter_rule_type_v01), qos_filter_rule_type_data_v01},
  {sizeof(qos_filter_rule_resp_ind_type_v01), qos_filter_rule_resp_ind_type_data_v01},
  {sizeof(qos_flow_data_rate_min_max_type_v01), qos_flow_data_rate_min_max_type_data_v01},
  {sizeof(qos_flow_data_rate_token_bucket_type_v01), qos_flow_data_rate_token_bucket_type_data_v01},
  {sizeof(qos_flow_pkt_err_rate_v01), qos_flow_pkt_err_rate_data_v01},
  {sizeof(qos_flow_type_v01), qos_flow_type_data_v01},
  {sizeof(qos_flow_info_type_v01), qos_flow_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry qos_message_table_v01[] = {
  {sizeof(qos_reset_req_msg_v01), 0},
  {sizeof(qos_reset_resp_msg_v01), qos_reset_resp_msg_data_v01},
  {sizeof(qos_set_event_report_req_msg_v01), qos_set_event_report_req_msg_data_v01},
  {sizeof(qos_set_event_report_resp_msg_v01), qos_set_event_report_resp_msg_data_v01},
  {sizeof(qos_event_report_ind_msg_v01), qos_event_report_ind_msg_data_v01},
  {sizeof(qos_request_qos_req_msg_v01), qos_request_qos_req_msg_data_v01},
  {sizeof(qos_request_qos_resp_msg_v01), qos_request_qos_resp_msg_data_v01},
  {sizeof(qos_release_qos_req_msg_v01), qos_release_qos_req_msg_data_v01},
  {sizeof(qos_release_qos_resp_msg_v01), qos_release_qos_resp_msg_data_v01},
  {sizeof(qos_suspend_qos_req_msg_v01), qos_suspend_qos_req_msg_data_v01},
  {sizeof(qos_suspend_qos_resp_msg_v01), qos_suspend_qos_resp_msg_data_v01},
  {sizeof(qos_resume_qos_req_msg_v01), qos_resume_qos_req_msg_data_v01},
  {sizeof(qos_resume_qos_resp_msg_v01), qos_resume_qos_resp_msg_data_v01},
  {sizeof(qos_modify_qos_req_msg_v01), qos_modify_qos_req_msg_data_v01},
  {sizeof(qos_modify_qos_resp_msg_v01), qos_modify_qos_resp_msg_data_v01},
  {sizeof(qos_get_granted_qos_req_msg_v01), qos_get_granted_qos_req_msg_data_v01},
  {sizeof(qos_get_granted_qos_resp_msg_v01), qos_get_granted_qos_resp_msg_data_v01},
  {sizeof(qos_get_qos_status_req_msg_v01), qos_get_qos_status_req_msg_data_v01},
  {sizeof(qos_get_qos_status_resp_msg_v01), qos_get_qos_status_resp_msg_data_v01},
  {sizeof(qos_status_ind_msg_v01), qos_status_ind_msg_data_v01},
  {sizeof(qos_get_qos_network_status_req_msg_v01), 0},
  {sizeof(qos_get_qos_network_status_resp_msg_v01), qos_get_qos_network_status_resp_msg_data_v01},
  {sizeof(qos_network_status_ind_msg_v01), qos_network_status_ind_msg_data_v01},
  {sizeof(qos_get_nw_supported_qos_profiles_req_msg_v01), qos_get_nw_supported_qos_profiles_req_msg_data_v01},
  {sizeof(qos_get_nw_supported_qos_profiles_resp_msg_v01), qos_get_nw_supported_qos_profiles_resp_msg_data_v01},
  {sizeof(qos_primary_qos_event_ind_msg_v01), qos_primary_qos_event_ind_msg_data_v01},
  {sizeof(qos_set_client_ip_pref_req_msg_v01), qos_set_client_ip_pref_req_msg_data_v01},
  {sizeof(qos_set_client_ip_pref_resp_msg_v01), qos_set_client_ip_pref_resp_msg_data_v01},
  {sizeof(qos_bind_data_port_req_msg_v01), qos_bind_data_port_req_msg_data_v01},
  {sizeof(qos_bind_data_port_resp_msg_v01), qos_bind_data_port_resp_msg_data_v01},
  {sizeof(qos_perform_flow_operation_req_msg_v01), qos_perform_flow_operation_req_msg_data_v01},
  {sizeof(qos_perform_flow_operation_resp_msg_v01), qos_perform_flow_operation_resp_msg_data_v01},
  {sizeof(qos_get_filter_params_req_msg_v01), qos_get_filter_params_req_msg_data_v01},
  {sizeof(qos_get_filter_params_resp_msg_v01), qos_get_filter_params_resp_msg_data_v01},
  {sizeof(qos_bind_subscription_req_msg_v01), qos_bind_subscription_req_msg_data_v01},
  {sizeof(qos_bind_subscription_resp_msg_v01), qos_bind_subscription_resp_msg_data_v01},
  {sizeof(qos_get_bind_subscription_req_msg_v01), 0},
  {sizeof(qos_get_bind_subscription_resp_msg_v01), qos_get_bind_subscription_resp_msg_data_v01},
  {sizeof(qos_indication_register_req_msg_v01), qos_indication_register_req_msg_data_v01},
  {sizeof(qos_indication_register_resp_msg_v01), qos_indication_register_resp_msg_data_v01},
  {sizeof(qos_request_qos_ex_req_msg_v01), qos_request_qos_ex_req_msg_data_v01},
  {sizeof(qos_request_qos_ex_resp_msg_v01), qos_request_qos_ex_resp_msg_data_v01},
  {sizeof(qos_global_qos_flow_ind_msg_v01), qos_global_qos_flow_ind_msg_data_v01},
  {sizeof(qos_modify_qos_ex_req_msg_v01), qos_modify_qos_ex_req_msg_data_v01},
  {sizeof(qos_modify_qos_ex_resp_msg_v01), qos_modify_qos_ex_resp_msg_data_v01},
  {sizeof(qos_get_qos_info_req_msg_v01), qos_get_qos_info_req_msg_data_v01},
  {sizeof(qos_get_qos_info_resp_msg_v01), qos_get_qos_info_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object qos_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *qos_qmi_idl_type_table_object_referenced_tables_v01[] =
{&qos_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &data_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object qos_qmi_idl_type_table_object_v01 = {
  sizeof(qos_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(qos_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  qos_type_table_v01,
  qos_message_table_v01,
  qos_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry qos_service_command_messages_v01[] = {
  {QMI_QOS_RESET_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_QOS_SET_EVENT_REPORT_REQ_V01, QMI_IDL_TYPE16(0, 2), 10},
  {QMI_QOS_REQUEST_QOS_REQ_V01, QMI_IDL_TYPE16(0, 5), 4},
  {QMI_QOS_RELEASE_QOS_REQ_V01, QMI_IDL_TYPE16(0, 7), 44},
  {QMI_QOS_SUSPEND_QOS_REQ_V01, QMI_IDL_TYPE16(0, 9), 44},
  {QMI_QOS_RESUME_QOS_REQ_V01, QMI_IDL_TYPE16(0, 11), 44},
  {QMI_QOS_MODIFY_QOS_REQ_V01, QMI_IDL_TYPE16(0, 13), 4},
  {QMI_QOS_GET_GRANTED_QOS_REQ_V01, QMI_IDL_TYPE16(0, 15), 11},
  {QMI_QOS_GET_QOS_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 17), 7},
  {QMI_QOS_GET_QOS_NETWORK_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 20), 0},
  {QMI_QOS_GET_NW_SUPPORTED_QOS_PROFILES_REQ_V01, QMI_IDL_TYPE16(0, 23), 5},
  {QMI_QOS_SET_CLIENT_IP_PREF_REQ_V01, QMI_IDL_TYPE16(0, 26), 4},
  {QMI_QOS_BIND_DATA_PORT_REQ_V01, QMI_IDL_TYPE16(0, 28), 20},
  {QMI_QOS_GET_FILTER_PARAMS_REQ_V01, QMI_IDL_TYPE16(0, 32), 7},
  {QMI_QOS_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 34), 7},
  {QMI_QOS_GET_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 36), 0},
  {QMI_QOS_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 38), 4},
  {QMI_QOS_REQUEST_QOS_EX_REQ_V01, QMI_IDL_TYPE16(0, 40), 5888},
  {QMI_QOS_MODIFY_QOS_EX_REQ_V01, QMI_IDL_TYPE16(0, 43), 5895},
  {QMI_QOS_GET_QOS_INFO_REQ_V01, QMI_IDL_TYPE16(0, 45), 7},
  {QMI_QOS_PERFORM_FLOW_OPERATION_REQ_V01, QMI_IDL_TYPE16(0, 30), 25}
};

static const qmi_idl_service_message_table_entry qos_service_response_messages_v01[] = {
  {QMI_QOS_RESET_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_QOS_SET_EVENT_REPORT_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_QOS_REQUEST_QOS_RESP_V01, QMI_IDL_TYPE16(0, 6), 51},
  {QMI_QOS_RELEASE_QOS_RESP_V01, QMI_IDL_TYPE16(0, 8), 7},
  {QMI_QOS_SUSPEND_QOS_RESP_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_QOS_RESUME_QOS_RESP_V01, QMI_IDL_TYPE16(0, 12), 7},
  {QMI_QOS_MODIFY_QOS_RESP_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_QOS_GET_GRANTED_QOS_RESP_V01, QMI_IDL_TYPE16(0, 16), 12},
  {QMI_QOS_GET_QOS_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 18), 11},
  {QMI_QOS_GET_QOS_NETWORK_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 21), 11},
  {QMI_QOS_GET_NW_SUPPORTED_QOS_PROFILES_RESP_V01, QMI_IDL_TYPE16(0, 24), 66},
  {QMI_QOS_SET_CLIENT_IP_PREF_RESP_V01, QMI_IDL_TYPE16(0, 27), 7},
  {QMI_QOS_BIND_DATA_PORT_RESP_V01, QMI_IDL_TYPE16(0, 29), 7},
  {QMI_QOS_GET_FILTER_PARAMS_RESP_V01, QMI_IDL_TYPE16(0, 33), 7},
  {QMI_QOS_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 35), 7},
  {QMI_QOS_GET_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 37), 14},
  {QMI_QOS_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 39), 7},
  {QMI_QOS_REQUEST_QOS_EX_RESP_V01, QMI_IDL_TYPE16(0, 41), 414},
  {QMI_QOS_MODIFY_QOS_EX_RESP_V01, QMI_IDL_TYPE16(0, 44), 407},
  {QMI_QOS_GET_QOS_INFO_RESP_V01, QMI_IDL_TYPE16(0, 46), 4952},
  {QMI_QOS_PERFORM_FLOW_OPERATION_RESP_V01, QMI_IDL_TYPE16(0, 31), 61}
};

static const qmi_idl_service_message_table_entry qos_service_indication_messages_v01[] = {
  {QMI_QOS_EVENT_REPORT_IND_V01, QMI_IDL_TYPE16(0, 4), 8},
  {QMI_QOS_STATUS_IND_V01, QMI_IDL_TYPE16(0, 19), 13},
  {QMI_QOS_NETWORK_STATUS_IND_V01, QMI_IDL_TYPE16(0, 22), 4},
  {QMI_QOS_PRIMARY_QOS_EVENT_IND_V01, QMI_IDL_TYPE16(0, 25), 5},
  {QMI_QOS_GLOBAL_QOS_FLOW_IND_V01, QMI_IDL_TYPE16(0, 42), 4964}
};

/*Service Object*/
struct qmi_idl_service_object qos_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x04,
  5895,
  { sizeof(qos_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qos_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qos_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { qos_service_command_messages_v01, qos_service_response_messages_v01, qos_service_indication_messages_v01},
  &qos_qmi_idl_type_table_object_v01,
  0x0C,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type qos_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( QOS_V01_IDL_MAJOR_VERS != idl_maj_version || QOS_V01_IDL_MINOR_VERS != idl_min_version
       || QOS_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&qos_qmi_idl_service_object_v01;
}

