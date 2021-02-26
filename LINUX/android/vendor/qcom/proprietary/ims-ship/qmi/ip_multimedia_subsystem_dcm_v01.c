/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        I P _ M U L T I M E D I A _ S U B S Y S T E M _ D C M _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the imsdcm service Data structures.

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Wed May 17 2017 (Spin 0)
   From IDL File: ip_multimedia_subsystem_dcm_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "ip_multimedia_subsystem_dcm_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t imsdcm_addr_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(imsdcm_addr_type_v01, family_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(imsdcm_addr_type_v01, ipaddr),
  IMS_DCM_IP_ADDR_STRING_LEN_MAX_V01,
  QMI_IDL_OFFSET8(imsdcm_addr_type_v01, ipaddr) - QMI_IDL_OFFSET8(imsdcm_addr_type_v01, ipaddr_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t imsdcm_link_addr_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(imsdcm_link_addr_type_v01, port),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(imsdcm_link_addr_type_v01, family_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(imsdcm_link_addr_type_v01, ipaddr),
  IMS_DCM_IP_ADDR_STRING_LEN_MAX_V01,
  QMI_IDL_OFFSET8(imsdcm_link_addr_type_v01, ipaddr) - QMI_IDL_OFFSET8(imsdcm_link_addr_type_v01, ipaddr_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_dcm_pdp_information_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ims_dcm_pdp_information_v01, apn_name),
  IMS_DCM_APN_STRING_LEN_MAX_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_information_v01, apn_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_information_v01, rat_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_information_v01, family_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_information_v01, profile_num),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_dcm_rtcp_jitter_threshold_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_rtcp_jitter_threshold_v01, rtcp_jitter_good),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_rtcp_jitter_threshold_v01, rtcp_jitter_acceptable),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_rtcp_jitter_threshold_v01, rtcp_jitter_fair),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_rtcp_jitter_threshold_v01, rtcp_jitter_bad),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_dcm_rtp_frame_loss_threshold_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_rtp_frame_loss_threshold_v01, rtp_frame_loss_good),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_rtp_frame_loss_threshold_v01, rtp_frame_loss_acceptable),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_rtp_frame_loss_threshold_v01, rtp_frame_loss_fair),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_rtp_frame_loss_threshold_v01, rtp_frame_loss_bad),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_dcm_handover_media_report_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_handover_media_report_v01, rtcp_jitter_ul),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_handover_media_report_v01, rtcp_jitter_dl),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_handover_media_report_v01, rtp_frame_loss_ul),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_handover_media_report_v01, rtp_frame_loss_dl),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ims_dcm_tz_info_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_tz_info_v01, dcm_tm_sec),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_tz_info_v01, dcm_tm_min),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_tz_info_v01, dcm_tm_hour),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_tz_info_v01, dcm_tm_mday),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_tz_info_v01, dcm_tm_mon),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_tz_info_v01, dcm_tm_year),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_tz_info_v01, dcm_tm_wday),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_tz_info_v01, dcm_tm_tzone),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_tz_info_v01, dcm_epoch_time),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t ims_dcm_pdp_activate_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_info),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_req_seq_no) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_req_seq_no_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_req_seq_no),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_subs_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_subs_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_subs_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_slot_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_slot_id_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, pdp_slot_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, eInstance_id_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_req_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_pdp_activate_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, pdp_req_seq_no) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, pdp_req_seq_no_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, pdp_req_seq_no),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, eInstance_id_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_rsp_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_pdp_activate_ind_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, pdp_error),
  QMI_IDL_TYPE88(1, 0),

  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, pdp_req_seq_no) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, pdp_req_seq_no_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, pdp_req_seq_no),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, addr_info) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, addr_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, addr_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, eInstance_id_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_activate_ind_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_log_request_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_log_request_msg_v01, ims_dcm_log_msg) - QMI_IDL_OFFSET8(ims_dcm_log_request_msg_v01, ims_dcm_log_msg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_log_request_msg_v01, ims_dcm_log_msg),
  ((IMS_DCM_LOG_STRING_LEN_MAX_V01) & 0xFF), ((IMS_DCM_LOG_STRING_LEN_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(ims_dcm_log_request_msg_v01, ims_dcm_log_msg) - QMI_IDL_OFFSET8(ims_dcm_log_request_msg_v01, ims_dcm_log_msg_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ims_dcm_log_request_msg_v01, eInstance_id) - QMI_IDL_OFFSET16RELATIVE(ims_dcm_log_request_msg_v01, eInstance_id_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ims_dcm_log_request_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_log_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_log_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_log_rsp_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_log_rsp_msg_v01, eInstance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_log_rsp_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_register_app_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_register_app_state_req_msg_v01, ims_dcm_bluetooth_event) - QMI_IDL_OFFSET8(ims_dcm_register_app_state_req_msg_v01, ims_dcm_bluetooth_event_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_register_app_state_req_msg_v01, ims_dcm_bluetooth_event),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_register_app_state_req_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_register_app_state_req_msg_v01, eInstance_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_register_app_state_req_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_register_app_state_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_register_app_state_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_register_app_state_rsp_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_register_app_state_rsp_msg_v01, eInstance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_register_app_state_rsp_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_app_state_ind_bt_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_app_state_ind_bt_msg_v01, bt_state) - QMI_IDL_OFFSET8(ims_dcm_app_state_ind_bt_msg_v01, bt_state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_app_state_ind_bt_msg_v01, bt_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_app_state_ind_bt_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_app_state_ind_bt_msg_v01, eInstance_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_app_state_ind_bt_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_app_state_ind_logging_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_app_state_ind_logging_msg_v01, ims_dcm_logging_event_supported) - QMI_IDL_OFFSET8(ims_dcm_app_state_ind_logging_msg_v01, ims_dcm_logging_event_supported_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_app_state_ind_logging_msg_v01, ims_dcm_logging_event_supported),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_app_state_ind_logging_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_app_state_ind_logging_msg_v01, eInstance_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_app_state_ind_logging_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_pdp_deactivate_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_req_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_req_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_req_msg_v01, eInstance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_req_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_pdp_deactivate_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_rsp_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_rsp_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_rsp_msg_v01, eInstance_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_deactivate_rsp_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_pdp_get_ip_address_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_req_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_req_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_req_msg_v01, eInstance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_req_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_pdp_get_ip_address_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_rsp_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_rsp_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_rsp_msg_v01, eInstance_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_rsp_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_pdp_get_ip_address_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_ind_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_ind_msg_v01, addr_info) - QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_ind_msg_v01, addr_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_ind_msg_v01, addr_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_ind_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_ind_msg_v01, eInstance_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_pdp_get_ip_address_ind_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_modem_link_add_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_modem_link_add_req_msg_v01, link_local_ip_addr),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t ims_dcm_modem_link_add_rsp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_modem_link_add_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ims_dcm_address_change_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_address_change_ind_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_address_change_ind_msg_v01, addr_info) - QMI_IDL_OFFSET8(ims_dcm_address_change_ind_msg_v01, addr_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_address_change_ind_msg_v01, addr_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_address_change_ind_msg_v01, eInstance_id) - QMI_IDL_OFFSET8(ims_dcm_address_change_ind_msg_v01, eInstance_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_address_change_ind_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_get_wifi_quality_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, pdp_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, rssi_threshold_add),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, rssi_threshold_drop),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, rssi_threshold_close),

  0x05,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, rssi_timer_close),

  0x06,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, rssi_timer_far),

  0x07,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, rssi_avg_timer_on_attach),

  0x08,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, rssi_avg_timer_on_camp),

  0x09,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, rssi_sampling_timer),

  0x0A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, rssi_average_interval),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x0B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_req_msg_v01, curattachstatus)
};

static const uint8_t ims_dcm_get_wifi_quality_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_rsp_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_get_wifi_quality_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_ind_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_wifi_quality_ind_msg_v01, qualityreport)
};

static const uint8_t ims_dcm_stop_wifi_quality_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_stop_wifi_quality_req_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_stop_wifi_quality_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_stop_wifi_quality_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_stop_wifi_quality_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_stop_wifi_quality_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_stop_wifi_quality_rsp_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_update_wifi_quality_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_update_wifi_quality_req_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_update_wifi_quality_req_msg_v01, attachstatus)
};

static const uint8_t ims_dcm_update_wifi_quality_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_update_wifi_quality_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_update_wifi_quality_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_update_wifi_quality_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_update_wifi_quality_rsp_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_get_ho_measurement_init_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, pdp_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, sampling_timer_value),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, monitor_timer_value),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rssi_threshold_high),

  0x05,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rssi_threshold_low),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, source_rat_type) - QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, source_rat_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, source_rat_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, target_rat_type) - QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, target_rat_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, target_rat_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rtcp_jitter_threshold) - QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rtcp_jitter_threshold_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rtcp_jitter_threshold),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rtp_frame_loss_threshold) - QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rtp_frame_loss_threshold_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rtp_frame_loss_threshold),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rssi_threshold_repoint) - QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rssi_threshold_repoint_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rssi_threshold_repoint),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rssi_threshold_call_quality) - QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rssi_threshold_call_quality_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_req_msg_v01, rssi_threshold_call_quality)
};

static const uint8_t ims_dcm_get_ho_measurement_init_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_get_ho_measurement_init_rsp_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_ho_measurement_start_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_start_req_msg_v01, pdp_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_start_req_msg_v01, source_rat_type),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_start_req_msg_v01, target_rat_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_start_req_msg_v01, module_name)
};

static const uint8_t ims_dcm_ho_measurement_start_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_start_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_ho_measurement_start_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_ho_measurement_start_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_start_rsp_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_ho_measurement_stop_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_stop_req_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_stop_req_msg_v01, module_name)
};

static const uint8_t ims_dcm_ho_measurement_stop_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_stop_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_ho_measurement_stop_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_ho_measurement_stop_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_stop_rsp_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_ho_measurement_report_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, pdp_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, module_name),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, handover_report),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, current_rssi_value) - QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, current_rssi_value_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, current_rssi_value),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, handover_media_report) - QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, handover_media_report_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, handover_media_report),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, voice_call_report) - QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, voice_call_report_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_report_ind_msg_v01, voice_call_report)
};

static const uint8_t ims_dcm_ho_measurement_status_update_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_status_update_req_msg_v01, pdp_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_status_update_req_msg_v01, source_rat_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_status_update_req_msg_v01, target_rat_type)
};

static const uint8_t ims_dcm_ho_measurement_status_update_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_status_update_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_ho_measurement_status_update_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_ho_measurement_status_update_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_status_update_rsp_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_ho_measurement_uninit_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_uninit_req_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_ho_measurement_uninit_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_uninit_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ims_dcm_ho_measurement_uninit_rsp_msg_v01, pdp_id) - QMI_IDL_OFFSET8(ims_dcm_ho_measurement_uninit_rsp_msg_v01, pdp_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_ho_measurement_uninit_rsp_msg_v01, pdp_id)
};

static const uint8_t ims_dcm_wlan_tz_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_wlan_tz_req_msg_v01, pdp_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_wlan_tz_req_msg_v01, req_seq_no)
};

static const uint8_t ims_dcm_wlan_tz_rsp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_wlan_tz_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_wlan_tz_rsp_msg_v01, pdp_id),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_wlan_tz_rsp_msg_v01, req_seq_no),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x05,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_wlan_tz_rsp_msg_v01, tz_info),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t ims_dcm_sub_destroy_instance_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_sub_destroy_instance_req_msg_v01, eInstance_id)
};

static const uint8_t ims_dcm_sub_destroy_instance_rsp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_sub_destroy_instance_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ims_dcm_service_enable_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(ims_dcm_service_enable_req_msg_v01, services_type_mask)
};

static const uint8_t ims_dcm_service_enable_rsp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ims_dcm_service_enable_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  imsdcm_type_table_v01[] = {
  {sizeof(imsdcm_addr_type_v01), imsdcm_addr_type_data_v01},
  {sizeof(imsdcm_link_addr_type_v01), imsdcm_link_addr_type_data_v01},
  {sizeof(ims_dcm_pdp_information_v01), ims_dcm_pdp_information_data_v01},
  {sizeof(ims_dcm_rtcp_jitter_threshold_v01), ims_dcm_rtcp_jitter_threshold_data_v01},
  {sizeof(ims_dcm_rtp_frame_loss_threshold_v01), ims_dcm_rtp_frame_loss_threshold_data_v01},
  {sizeof(ims_dcm_handover_media_report_v01), ims_dcm_handover_media_report_data_v01},
  {sizeof(ims_dcm_tz_info_v01), ims_dcm_tz_info_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry imsdcm_message_table_v01[] = {
  {sizeof(ims_dcm_pdp_activate_req_msg_v01), ims_dcm_pdp_activate_req_msg_data_v01},
  {sizeof(ims_dcm_pdp_activate_rsp_msg_v01), ims_dcm_pdp_activate_rsp_msg_data_v01},
  {sizeof(ims_dcm_pdp_activate_ind_msg_v01), ims_dcm_pdp_activate_ind_msg_data_v01},
  {sizeof(ims_dcm_log_request_msg_v01), ims_dcm_log_request_msg_data_v01},
  {sizeof(ims_dcm_log_rsp_msg_v01), ims_dcm_log_rsp_msg_data_v01},
  {sizeof(ims_dcm_register_app_state_req_msg_v01), ims_dcm_register_app_state_req_msg_data_v01},
  {sizeof(ims_dcm_register_app_state_rsp_msg_v01), ims_dcm_register_app_state_rsp_msg_data_v01},
  {sizeof(ims_dcm_app_state_ind_bt_msg_v01), ims_dcm_app_state_ind_bt_msg_data_v01},
  {sizeof(ims_dcm_app_state_ind_logging_msg_v01), ims_dcm_app_state_ind_logging_msg_data_v01},
  {sizeof(ims_dcm_pdp_deactivate_req_msg_v01), ims_dcm_pdp_deactivate_req_msg_data_v01},
  {sizeof(ims_dcm_pdp_deactivate_rsp_msg_v01), ims_dcm_pdp_deactivate_rsp_msg_data_v01},
  {sizeof(ims_dcm_pdp_get_ip_address_req_msg_v01), ims_dcm_pdp_get_ip_address_req_msg_data_v01},
  {sizeof(ims_dcm_pdp_get_ip_address_rsp_msg_v01), ims_dcm_pdp_get_ip_address_rsp_msg_data_v01},
  {sizeof(ims_dcm_pdp_get_ip_address_ind_msg_v01), ims_dcm_pdp_get_ip_address_ind_msg_data_v01},
  {sizeof(ims_dcm_modem_link_add_req_msg_v01), ims_dcm_modem_link_add_req_msg_data_v01},
  {sizeof(ims_dcm_modem_link_add_rsp_msg_v01), ims_dcm_modem_link_add_rsp_msg_data_v01},
  {sizeof(ims_dcm_address_change_ind_msg_v01), ims_dcm_address_change_ind_msg_data_v01},
  {sizeof(ims_dcm_get_wifi_quality_req_msg_v01), ims_dcm_get_wifi_quality_req_msg_data_v01},
  {sizeof(ims_dcm_get_wifi_quality_rsp_msg_v01), ims_dcm_get_wifi_quality_rsp_msg_data_v01},
  {sizeof(ims_dcm_get_wifi_quality_ind_msg_v01), ims_dcm_get_wifi_quality_ind_msg_data_v01},
  {sizeof(ims_dcm_stop_wifi_quality_req_msg_v01), ims_dcm_stop_wifi_quality_req_msg_data_v01},
  {sizeof(ims_dcm_stop_wifi_quality_rsp_msg_v01), ims_dcm_stop_wifi_quality_rsp_msg_data_v01},
  {sizeof(ims_dcm_update_wifi_quality_req_msg_v01), ims_dcm_update_wifi_quality_req_msg_data_v01},
  {sizeof(ims_dcm_update_wifi_quality_rsp_msg_v01), ims_dcm_update_wifi_quality_rsp_msg_data_v01},
  {sizeof(ims_dcm_get_ho_measurement_init_req_msg_v01), ims_dcm_get_ho_measurement_init_req_msg_data_v01},
  {sizeof(ims_dcm_get_ho_measurement_init_rsp_msg_v01), ims_dcm_get_ho_measurement_init_rsp_msg_data_v01},
  {sizeof(ims_dcm_ho_measurement_start_req_msg_v01), ims_dcm_ho_measurement_start_req_msg_data_v01},
  {sizeof(ims_dcm_ho_measurement_start_rsp_msg_v01), ims_dcm_ho_measurement_start_rsp_msg_data_v01},
  {sizeof(ims_dcm_ho_measurement_stop_req_msg_v01), ims_dcm_ho_measurement_stop_req_msg_data_v01},
  {sizeof(ims_dcm_ho_measurement_stop_rsp_msg_v01), ims_dcm_ho_measurement_stop_rsp_msg_data_v01},
  {sizeof(ims_dcm_ho_measurement_report_ind_msg_v01), ims_dcm_ho_measurement_report_ind_msg_data_v01},
  {sizeof(ims_dcm_ho_measurement_status_update_req_msg_v01), ims_dcm_ho_measurement_status_update_req_msg_data_v01},
  {sizeof(ims_dcm_ho_measurement_status_update_rsp_msg_v01), ims_dcm_ho_measurement_status_update_rsp_msg_data_v01},
  {sizeof(ims_dcm_ho_measurement_uninit_req_msg_v01), ims_dcm_ho_measurement_uninit_req_msg_data_v01},
  {sizeof(ims_dcm_ho_measurement_uninit_rsp_msg_v01), ims_dcm_ho_measurement_uninit_rsp_msg_data_v01},
  {sizeof(ims_dcm_wlan_tz_req_msg_v01), ims_dcm_wlan_tz_req_msg_data_v01},
  {sizeof(ims_dcm_wlan_tz_rsp_msg_v01), ims_dcm_wlan_tz_rsp_msg_data_v01},
  {sizeof(ims_dcm_sub_destroy_instance_req_msg_v01), ims_dcm_sub_destroy_instance_req_msg_data_v01},
  {sizeof(ims_dcm_sub_destroy_instance_rsp_msg_v01), ims_dcm_sub_destroy_instance_rsp_msg_data_v01},
  {sizeof(ims_dcm_service_enable_req_msg_v01), ims_dcm_service_enable_req_msg_data_v01},
  {sizeof(ims_dcm_service_enable_rsp_msg_v01), ims_dcm_service_enable_rsp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object imsdcm_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *imsdcm_qmi_idl_type_table_object_referenced_tables_v01[] =
{&imsdcm_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object imsdcm_qmi_idl_type_table_object_v01 = {
  sizeof(imsdcm_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(imsdcm_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  imsdcm_type_table_v01,
  imsdcm_message_table_v01,
  imsdcm_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry imsdcm_service_command_messages_v01[] = {
  {QMI_IMS_DCM_PDP_ACTIVATE_REQ_V01, QMI_IDL_TYPE16(0, 0), 148},
  {QMI_IMS_DCM_PDP_DEACTIVATE_REQ_V01, QMI_IDL_TYPE16(0, 9), 11},
  {QMI_IMS_DCM_GET_IP_ADDRESS_REQ_V01, QMI_IDL_TYPE16(0, 11), 11},
  {QMI_IMS_DCM_LINK_ADDR_REQ_V01, QMI_IDL_TYPE16(0, 14), 50},
  {QMI_IMS_DCM_GET_WIFI_QUALITY_REQ_V01, QMI_IDL_TYPE16(0, 17), 74},
  {QMI_IMS_DCM_STOP_WIFI_QUALITY_REQ_V01, QMI_IDL_TYPE16(0, 20), 4},
  {QMI_IMS_DCM_UPDATE_WIFI_QUALITY_REQ_V01, QMI_IDL_TYPE16(0, 22), 11},
  {QMI_IMS_DCM_HO_MEASUREMENT_INIT_REQ_V01, QMI_IDL_TYPE16(0, 24), 98},
  {QMI_IMS_DCM_HO_MEASUREMENT_START_REQ_V01, QMI_IDL_TYPE16(0, 26), 25},
  {QMI_IMS_DCM_HO_MEASUREMENT_STOP_REQ_V01, QMI_IDL_TYPE16(0, 28), 11},
  {QMI_IMS_DCM_HO_MEASUREMENT_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 31), 18},
  {QMI_IMS_DCM_HO_MEASUREMENT_UNINIT_REQ_V01, QMI_IDL_TYPE16(0, 33), 4},
  {QMI_IMS_DCM_REGISTER_APP_STATE_REQ_V01, QMI_IDL_TYPE16(0, 5), 11},
  {QMI_IMS_DCM_LOG_REQ_V01, QMI_IDL_TYPE16(0, 3), 1036},
  {QMI_IMS_DCM_WLAN_TZ_REQ_V01, QMI_IDL_TYPE16(0, 35), 11},
  {QMI_IMS_DCM_SUB_DESTROY_INSTANCE_REQ_V01, QMI_IDL_TYPE16(0, 37), 7},
  {QMI_IMS_DCM_SERVICE_ENABLE_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 39), 11}
};

static const qmi_idl_service_message_table_entry imsdcm_service_response_messages_v01[] = {
  {QMI_IMS_DCM_PDP_ACTIVATE_RSP_V01, QMI_IDL_TYPE16(0, 1), 25},
  {QMI_IMS_DCM_PDP_DEACTIVATE_RSP_V01, QMI_IDL_TYPE16(0, 10), 18},
  {QMI_IMS_DCM_GET_IP_ADDRESS_RSP_V01, QMI_IDL_TYPE16(0, 12), 18},
  {QMI_IMS_DCM_LINK_ADDR_RSP_V01, QMI_IDL_TYPE16(0, 15), 7},
  {QMI_IMS_DCM_GET_WIFI_QUALITY_RSP_V01, QMI_IDL_TYPE16(0, 18), 11},
  {QMI_IMS_DCM_STOP_WIFI_QUALITY_RSP_V01, QMI_IDL_TYPE16(0, 21), 11},
  {QMI_IMS_DCM_UPDATE_WIFI_QUALITY_RSP_V01, QMI_IDL_TYPE16(0, 23), 11},
  {QMI_IMS_DCM_HO_MEASUREMENT_INIT_RSP_V01, QMI_IDL_TYPE16(0, 25), 11},
  {QMI_IMS_DCM_HO_MEASUREMENT_START_RSP_V01, QMI_IDL_TYPE16(0, 27), 11},
  {QMI_IMS_DCM_HO_MEASUREMENT_STOP_RSP_V01, QMI_IDL_TYPE16(0, 29), 11},
  {QMI_IMS_DCM_HO_MEASUREMENT_STATUS_RSP_V01, QMI_IDL_TYPE16(0, 32), 11},
  {QMI_IMS_DCM_HO_MEASUREMENT_UNINIT_RSP_V01, QMI_IDL_TYPE16(0, 34), 11},
  {QMI_IMS_DCM_REGISTER_APP_STATE_RSP_V01, QMI_IDL_TYPE16(0, 6), 14},
  {QMI_IMS_DCM_LOG_RSP_V01, QMI_IDL_TYPE16(0, 4), 14},
  {QMI_IMS_DCM_WLAN_TZ_RSP_V01, QMI_IDL_TYPE16(0, 36), 45},
  {QMI_IMS_DCM_SUB_DESTROY_INSTANCE_RSP_V01, QMI_IDL_TYPE16(0, 38), 7},
  {QMI_IMS_DCM_SERVICE_ENABLE_STATUS_RSP_V01, QMI_IDL_TYPE16(0, 40), 7}
};

static const qmi_idl_service_message_table_entry imsdcm_service_indication_messages_v01[] = {
  {QMI_IMS_DCM_PDP_ACTIVATE_IND_V01, QMI_IDL_TYPE16(0, 2), 73},
  {QMI_IMS_DCM_GET_IP_ADDRESS_IND_V01, QMI_IDL_TYPE16(0, 13), 59},
  {QMI_IMS_DCM_ADDRESS_CHANGE_IND_V01, QMI_IDL_TYPE16(0, 16), 59},
  {QMI_IMS_DCM_WIFI_QUALITY_IND_V01, QMI_IDL_TYPE16(0, 19), 11},
  {QMI_IMS_DCM_HO_MEASUREMENT_REPORT_IND_V01, QMI_IDL_TYPE16(0, 30), 51},
  {QMI_IMS_DCM_APP_STATE_BT_IND_V01, QMI_IDL_TYPE16(0, 7), 14},
  {QMI_IMS_DCM_APP_STATE_LOGGING_IND_V01, QMI_IDL_TYPE16(0, 8), 11}
};

/*Service Object*/
struct qmi_idl_service_object imsdcm_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x0302,
  1036,
  { sizeof(imsdcm_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(imsdcm_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(imsdcm_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { imsdcm_service_command_messages_v01, imsdcm_service_response_messages_v01, imsdcm_service_indication_messages_v01},
  &imsdcm_qmi_idl_type_table_object_v01,
  0x0F,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type imsdcm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( IMSDCM_V01_IDL_MAJOR_VERS != idl_maj_version || IMSDCM_V01_IDL_MINOR_VERS != idl_min_version
       || IMSDCM_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&imsdcm_qmi_idl_service_object_v01;
}

