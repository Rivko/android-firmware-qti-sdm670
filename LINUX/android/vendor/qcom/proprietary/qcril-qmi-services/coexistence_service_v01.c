/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C O E X I S T E N C E _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the coex service Data structures.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //source/qcom/qct/interfaces/qmi/rel/deploy/coex/src/coexistence_service_v01.c#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Sep  8 2016 (Spin 0)
   From IDL File: coexistence_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "coexistence_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t coex_band_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_band_type_v01, freq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_band_type_v01, bandwidth),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_xsver_band_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_xsver_band_type_v01, ul_band),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_xsver_band_type_v01, dl_band),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wlan_scan_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_scan_info_type_v01, id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wlan_scan_info_type_v01, band),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_scan_info_type_v01, state),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wlan_conn_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_conn_info_type_v01, handle),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wlan_conn_info_type_v01, band),
  COEX_WLAN_MAX_WIFI_CHNLS_PER_CONN_V01,
  QMI_IDL_OFFSET8(coex_wlan_conn_info_type_v01, band) - QMI_IDL_OFFSET8(coex_wlan_conn_info_type_v01, band_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_conn_info_type_v01, state),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_conn_info_type_v01, mode),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_lte_tdd_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_lte_tdd_info_type_v01, frame_offset),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_lte_tdd_info_type_v01, tdd_config),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_lte_tdd_info_type_v01, subframe_config),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_lte_tdd_info_type_v01, ul_config),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_lte_tdd_info_type_v01, dl_config),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_filter_band_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_filter_band_info_type_v01, band_info),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_filter_band_info_type_v01, band_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_range_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_range_type_v01, min),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_range_type_v01, max),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_frame_denial_duty_cycle_params_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_frame_denial_duty_cycle_params_type_v01, max_allowed_frame_denials),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_frame_denial_duty_cycle_params_type_v01, frame_denial_window),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_band_type_and_direction_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_band_type_and_direction_v01, freq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_band_type_and_direction_v01, bandwidth),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_band_type_and_direction_v01, direction),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_band_range_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_band_range_type_v01, direction),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_band_range_type_v01, operating_dim),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_band_range_type_v01, tech),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_band_range_type_v01, freq_start),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_band_range_type_v01, freq_stop),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_tx_thlds_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_tx_thlds_type_v01, tx_pwr_thld),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_tx_thlds_type_v01, tx_bw_thld),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_tx_frame_denials_params_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_tx_frame_denials_params_type_v01, duty_cycle_params),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_tx_frame_denials_params_type_v01, max_cont_denials),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wcn_prio_params_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wcn_prio_params_type_v01, tx_thlds),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wcn_prio_params_type_v01, frame_denials),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_rb_threshold_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_rb_threshold_type_v01, start),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_rb_threshold_type_v01, end),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_tx_advanced_params_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_tx_advanced_params_type_v01, rb_thresh),
  COEX_MAX_RB_THRES_PER_CONFLICT_V01,
  QMI_IDL_OFFSET8(coex_tx_advanced_params_type_v01, rb_thresh) - QMI_IDL_OFFSET8(coex_tx_advanced_params_type_v01, rb_thresh_len),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_tx_advanced_params_type_v01, tx_power_thresh),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_pwr_limit_params_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_pwr_limit_params_type_v01, power),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_conflict_mdm_params_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_mdm_params_type_v01, wcn_prio),
  QMI_IDL_TYPE88(0, 12),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_mdm_params_type_v01, tx_adv),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_mdm_params_type_v01, pwr_lmt),
  QMI_IDL_TYPE88(0, 15),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wcn_band_range_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_wcn_band_range_type_v01, mode),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wcn_band_range_type_v01, tech),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wcn_band_range_type_v01, freq_start),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wcn_band_range_type_v01, freq_stop),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_rx_imd_protection_params_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_rx_imd_protection_params_type_v01, block_tx),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_rx_imd_protection_params_type_v01, static_backoff_tx_pwr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_bt_afh_update_range_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_bt_afh_update_range_type_v01, start_bt_channel),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_bt_afh_update_range_type_v01, end_bt_channel),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_conflict_wcn_params_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_wcn_params_type_v01, imd_protection_params),
  QMI_IDL_TYPE88(0, 18),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_wcn_params_type_v01, bt_afh_update),
  COEX_MAX_BT_AFH_CHANNEL_RANGE_V01,
  QMI_IDL_OFFSET8(coex_conflict_wcn_params_type_v01, bt_afh_update) - QMI_IDL_OFFSET8(coex_conflict_wcn_params_type_v01, bt_afh_update_len),
  QMI_IDL_TYPE88(0, 19),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_conflict_definition_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v01, wwan),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v01, wcn),
  QMI_IDL_TYPE88(0, 17),
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v01, mdm_policy),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v01, wcn_policy),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v01, wcn_params),
  QMI_IDL_TYPE88(0, 20),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v01, mdm_params),
  QMI_IDL_TYPE88(0, 16),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wlan_high_prio_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_high_prio_info_type_v01, id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wlan_high_prio_info_type_v01, band),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_high_prio_info_type_v01, state),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_lte_carrier_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_lte_carrier_info_type_v01, id),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_lte_carrier_info_type_v01, operating_dim),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_lte_carrier_info_type_v01, tdd_info),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_lte_carrier_info_type_v01, frame_boundary_offset),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_tx_antenna_state_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_tx_antenna_state_type_v01, antenna_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_tx_antenna_state_type_v01, old_state),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_tx_antenna_state_type_v01, new_state),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * coex_reset_req_msg is empty
 * static const uint8_t coex_reset_req_msg_data_v01[] = {
 * };
 */

static const uint8_t coex_reset_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_reset_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t coex_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_wwan_state) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_wwan_state_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_wwan_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_metrics_lte_bler) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_metrics_lte_bler_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_metrics_lte_bler),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_fail_condition) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_fail_condition_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_fail_condition),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_success_condition) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_success_condition_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_success_condition),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_sleep) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_sleep_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_sleep),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_wakeup) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_wakeup_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_wakeup),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_page_sync) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_page_sync_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_page_sync),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, lte_5ghz_state) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, lte_5ghz_state_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, lte_5ghz_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, tx_antenna_state) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, tx_antenna_state_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, tx_antenna_state)
};

static const uint8_t coex_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t coex_wwan_state_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_band_info) - QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_band_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_tdd_info) - QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_tdd_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_tdd_info),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_off_period) - QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_off_period_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_off_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_band_info_set) - QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_band_info_set_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_band_info_set) - QMI_IDL_OFFSET8(coex_wwan_state_ind_msg_v01, lte_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, tdscdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, tdscdma_band_info_set_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, tdscdma_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, tdscdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, tdscdma_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm_band_info_set_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, gsm_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, onex_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, onex_band_info_set_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, onex_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, onex_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, onex_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, hdr_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, hdr_band_info_set_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, hdr_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, hdr_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, hdr_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma_band_info_set_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, wcdma_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm2_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm2_band_info_set_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, gsm2_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm2_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm2_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm3_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm3_band_info_set_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, gsm3_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm3_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, gsm3_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma2_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma2_band_info_set_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, wcdma2_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma2_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma2_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, lte_carrier_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, lte_carrier_info_set_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, lte_carrier_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, lte_carrier_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, lte_carrier_info_set_len),
  QMI_IDL_TYPE88(0, 23)
};

/*
 * coex_get_wwan_state_req_msg is empty
 * static const uint8_t coex_get_wwan_state_req_msg_data_v01[] = {
 * };
 */

static const uint8_t coex_get_wwan_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_band_info) - QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_band_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_tdd_info) - QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_tdd_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_tdd_info),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_off_period) - QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_off_period_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_off_period),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_band_info_set) - QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_band_info_set_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_band_info_set) - QMI_IDL_OFFSET8(coex_get_wwan_state_resp_msg_v01, lte_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, tdscdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, tdscdma_band_info_set_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, tdscdma_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, tdscdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, tdscdma_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm_band_info_set_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, gsm_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, onex_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, onex_band_info_set_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, onex_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, onex_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, onex_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, hdr_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, hdr_band_info_set_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, hdr_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, hdr_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, hdr_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm2_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm2_band_info_set_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, gsm2_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm2_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm2_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm3_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm3_band_info_set_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, gsm3_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm3_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, gsm3_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma2_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma2_band_info_set_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, wcdma2_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma2_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma2_band_info_set_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, lte_carrier_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, lte_carrier_info_set_valid)),
  0x1C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, lte_carrier_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, lte_carrier_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, lte_carrier_info_set_len),
  QMI_IDL_TYPE88(0, 23)
};

static const uint8_t coex_set_wlan_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, scan_info) - QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, scan_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, scan_info),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, conn_info) - QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, conn_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, conn_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, high_prio_info) - QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, high_prio_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, high_prio_info),
  QMI_IDL_TYPE88(0, 22)
};

static const uint8_t coex_set_wlan_state_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_wlan_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t coex_get_wlan_scan_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_wlan_scan_state_req_msg_v01, id)
};

static const uint8_t coex_get_wlan_scan_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wlan_scan_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wlan_scan_state_resp_msg_v01, scan_info) - QMI_IDL_OFFSET8(coex_get_wlan_scan_state_resp_msg_v01, scan_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wlan_scan_state_resp_msg_v01, scan_info),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t coex_get_wlan_conn_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_wlan_conn_state_req_msg_v01, conn_handle)
};

static const uint8_t coex_get_wlan_conn_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wlan_conn_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wlan_conn_state_resp_msg_v01, conn_info) - QMI_IDL_OFFSET8(coex_get_wlan_conn_state_resp_msg_v01, conn_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wlan_conn_state_resp_msg_v01, conn_info),
  QMI_IDL_TYPE88(0, 3)
};

static const uint8_t coex_set_policy_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, policy) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, policy_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, policy),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, power_threshold) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, power_threshold_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, power_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_threshold) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_threshold_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, lte_tx_continuous_subframe_denials_threshold) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, lte_tx_continuous_subframe_denials_threshold_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, lte_tx_continuous_subframe_denials_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, lte_tx_subrame_denial_params) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, lte_tx_subrame_denial_params_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, lte_tx_subrame_denial_params),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, apt_table) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, apt_table_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, apt_table),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, controller_tx_power_limit) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, controller_tx_power_limit_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, controller_tx_power_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, wci2_power_limit) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, wci2_power_limit_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, wci2_power_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, link_path_loss_threshold) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, link_path_loss_threshold_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, link_path_loss_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_filter_alpha) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_filter_alpha_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_filter_alpha),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, filtered_rb_threshold) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, filtered_rb_threshold_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, filtered_rb_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, wci2_tx_pwrlmt_timeout) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, wci2_tx_pwrlmt_timeout_valid)),
  0x1B,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, wci2_tx_pwrlmt_timeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, controller_tx_pwrlmt_timeout) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, controller_tx_pwrlmt_timeout_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, controller_tx_pwrlmt_timeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, tx_power_threshold_for_adv_tx_notice) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, tx_power_threshold_for_adv_tx_notice_valid)),
  0x1D,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, tx_power_threshold_for_adv_tx_notice),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_threshold_for_adv_tx_notice) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_threshold_for_adv_tx_notice_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_threshold_for_adv_tx_notice),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, holdoff_timer_for_rat_conn_state) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, holdoff_timer_for_rat_conn_state_valid)),
  0x1F,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, holdoff_timer_for_rat_conn_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, filter_alpha_for_rat_power_state) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, filter_alpha_for_rat_power_state_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, filter_alpha_for_rat_power_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, tx_power_threshold_for_rat_power_state) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, tx_power_threshold_for_rat_power_state_valid)),
  0x21,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, tx_power_threshold_for_rat_power_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, holdoff_timer_for_rat_power_state) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, holdoff_timer_for_rat_power_state_valid)),
  0x22,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, holdoff_timer_for_rat_power_state)
};

static const uint8_t coex_set_policy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * coex_get_policy_req_msg is empty
 * static const uint8_t coex_get_policy_req_msg_data_v01[] = {
 * };
 */

static const uint8_t coex_get_policy_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, policy) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, policy_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, policy),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, power_threshold) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, power_threshold_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, power_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_threshold) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_threshold_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, lte_tx_continuous_subframe_denials_threshold) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, lte_tx_continuous_subframe_denials_threshold_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, lte_tx_continuous_subframe_denials_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, lte_tx_subrame_denial_params) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, lte_tx_subrame_denial_params_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, lte_tx_subrame_denial_params),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, apt_table) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, apt_table_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, apt_table),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, controller_tx_power_limit) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, controller_tx_power_limit_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, controller_tx_power_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, wci2_power_limit) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, wci2_power_limit_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, wci2_power_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, link_path_loss_threshold) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, link_path_loss_threshold_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, link_path_loss_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_filter_alpha) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_filter_alpha_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_filter_alpha),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, filtered_rb_threshold) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, filtered_rb_threshold_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, filtered_rb_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, wci2_tx_pwrlmt_timeout) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, wci2_tx_pwrlmt_timeout_valid)),
  0x1B,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, wci2_tx_pwrlmt_timeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, controller_tx_pwrlmt_timeout) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, controller_tx_pwrlmt_timeout_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, controller_tx_pwrlmt_timeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, tx_power_threshold_for_adv_tx_notice) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, tx_power_threshold_for_adv_tx_notice_valid)),
  0x1D,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, tx_power_threshold_for_adv_tx_notice),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_threshold_for_adv_tx_notice) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_threshold_for_adv_tx_notice_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_threshold_for_adv_tx_notice),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, holdoff_timer_for_rat_conn_state) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, holdoff_timer_for_rat_conn_state_valid)),
  0x1F,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, holdoff_timer_for_rat_conn_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, filter_alpha_for_rat_power_state) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, filter_alpha_for_rat_power_state_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, filter_alpha_for_rat_power_state),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, tx_power_threshold_for_rat_power_state) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, tx_power_threshold_for_rat_power_state_valid)),
  0x21,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, tx_power_threshold_for_rat_power_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, holdoff_timer_for_rat_power_state) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, holdoff_timer_for_rat_power_state_valid)),
  0x22,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, holdoff_timer_for_rat_power_state)
};

static const uint8_t coex_metrics_lte_bler_start_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_start_req_msg_v01, tb_cnt),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_start_req_msg_v01, threshold_err_tb_cnt)
};

static const uint8_t coex_metrics_lte_bler_start_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_start_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t coex_metrics_lte_bler_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_bt_only) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_bt_only_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_bt_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_bt_only) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_bt_only_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_bt_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_wifi_only) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_wifi_only_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_wifi_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_wifi_only) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_wifi_only_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_wifi_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_bt_wifi) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_bt_wifi_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_bt_wifi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_bt_wifi) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_bt_wifi_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_bt_wifi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_lte_only) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_lte_only_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, tb_cnt_lte_only),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_lte_only) - QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_lte_only_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_ind_msg_v01, errored_tb_cnt_lte_only)
};

/*
 * coex_metrics_lte_bler_stop_req_msg is empty
 * static const uint8_t coex_metrics_lte_bler_stop_req_msg_data_v01[] = {
 * };
 */

static const uint8_t coex_metrics_lte_bler_stop_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_lte_bler_stop_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t coex_metrics_lte_sinr_start_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_req_msg_v01, alpha),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_req_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_req_msg_v01, carrier_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_req_msg_v01, carrier)
};

static const uint8_t coex_metrics_lte_sinr_start_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_resp_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_resp_msg_v01, carrier_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_resp_msg_v01, carrier)
};

static const uint8_t coex_metrics_lte_sinr_read_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_req_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_req_msg_v01, carrier_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_req_msg_v01, carrier)
};

static const uint8_t coex_metrics_lte_sinr_read_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_bt_only) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_bt_only_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_bt_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_wifi_only) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_wifi_only_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_wifi_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_bt_and_wifi) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_bt_and_wifi_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_bt_and_wifi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_lte_only) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_lte_only_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_lte_only),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, carrier_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, carrier)
};

static const uint8_t coex_metrics_lte_sinr_stop_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stop_req_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stop_req_msg_v01, carrier_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stop_req_msg_v01, carrier)
};

static const uint8_t coex_metrics_lte_sinr_stop_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stop_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stop_resp_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stop_resp_msg_v01, carrier_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stop_resp_msg_v01, carrier)
};

static const uint8_t coex_set_band_filter_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_band_filter_info_req_msg_v01, bands) - QMI_IDL_OFFSET8(coex_set_band_filter_info_req_msg_v01, bands_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_band_filter_info_req_msg_v01, bands),
  COEX_WWAN_MAX_BANDS_TO_MONITOR_V01,
  QMI_IDL_OFFSET8(coex_set_band_filter_info_req_msg_v01, bands) - QMI_IDL_OFFSET8(coex_set_band_filter_info_req_msg_v01, bands_len),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t coex_set_band_filter_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_band_filter_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * coex_get_band_filter_info_req_msg is empty
 * static const uint8_t coex_get_band_filter_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t coex_get_band_filter_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_band_filter_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_band_filter_info_resp_msg_v01, bands) - QMI_IDL_OFFSET8(coex_get_band_filter_info_resp_msg_v01, bands_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_band_filter_info_resp_msg_v01, bands),
  COEX_WWAN_MAX_BANDS_TO_MONITOR_V01,
  QMI_IDL_OFFSET8(coex_get_band_filter_info_resp_msg_v01, bands) - QMI_IDL_OFFSET8(coex_get_band_filter_info_resp_msg_v01, bands_len),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t coex_condition_fail_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_condition_fail_ind_msg_v01, tx_subframe_denials_status) - QMI_IDL_OFFSET8(coex_condition_fail_ind_msg_v01, tx_subframe_denials_status_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_condition_fail_ind_msg_v01, tx_subframe_denials_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_condition_fail_ind_msg_v01, controller_tx_pwrlmt_fail_cond) - QMI_IDL_OFFSET8(coex_condition_fail_ind_msg_v01, controller_tx_pwrlmt_fail_cond_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_condition_fail_ind_msg_v01, controller_tx_pwrlmt_fail_cond),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_condition_fail_ind_msg_v01, wci2_tx_pwrlmt_fail_cond) - QMI_IDL_OFFSET8(coex_condition_fail_ind_msg_v01, wci2_tx_pwrlmt_fail_cond_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_condition_fail_ind_msg_v01, wci2_tx_pwrlmt_fail_cond)
};

static const uint8_t coex_condition_success_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_condition_success_ind_msg_v01, tx_pwrlmt_success_case) - QMI_IDL_OFFSET8(coex_condition_success_ind_msg_v01, tx_pwrlmt_success_case_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_condition_success_ind_msg_v01, tx_pwrlmt_success_case)
};

/*
 * coex_get_wci2_mws_params_req_msg is empty
 * static const uint8_t coex_get_wci2_mws_params_req_msg_data_v01[] = {
 * };
 */

static const uint8_t coex_get_wci2_mws_params_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_frame_sync_assert_offset) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_frame_sync_assert_offset_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_frame_sync_assert_offset),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_frame_sync_assert_jitter) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_frame_sync_assert_jitter_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_frame_sync_assert_jitter),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_assert_offset) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_assert_offset_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_assert_offset),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_assert_jitter) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_assert_jitter_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_assert_jitter),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_deassert_offset) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_deassert_offset_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_deassert_offset),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_deassert_jitter) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_deassert_jitter_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_rx_deassert_jitter),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_assert_offset) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_assert_offset_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_assert_offset),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_assert_jitter) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_assert_jitter_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_assert_jitter),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_deassert_offset) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_deassert_offset_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_deassert_offset),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_deassert_jitter) - QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_deassert_jitter_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wci2_mws_params_resp_msg_v01, mws_tx_deassert_jitter),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t coex_set_sleep_notification_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_sleep_notification_req_msg_v01, tech),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_set_sleep_notification_req_msg_v01, off_period_threshold)
};

static const uint8_t coex_set_sleep_notification_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_sleep_notification_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t coex_get_sleep_notification_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_sleep_notification_req_msg_v01, tech)
};

static const uint8_t coex_get_sleep_notification_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_sleep_notification_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_sleep_notification_resp_msg_v01, tech) - QMI_IDL_OFFSET8(coex_get_sleep_notification_resp_msg_v01, tech_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_sleep_notification_resp_msg_v01, tech),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_sleep_notification_resp_msg_v01, off_period_threshold) - QMI_IDL_OFFSET8(coex_get_sleep_notification_resp_msg_v01, off_period_threshold_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_sleep_notification_resp_msg_v01, off_period_threshold)
};

static const uint8_t coex_sleep_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_sleep_ind_msg_v01, tech),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_sleep_ind_msg_v01, off_period)
};

static const uint8_t coex_wakeup_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wakeup_ind_msg_v01, tech),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wakeup_ind_msg_v01, time_to_wakeup)
};

static const uint8_t coex_wcn_wake_sync_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wcn_wake_sync_req_msg_v01, scan_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_wcn_wake_sync_req_msg_v01, scan_interval) - QMI_IDL_OFFSET8(coex_wcn_wake_sync_req_msg_v01, scan_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wcn_wake_sync_req_msg_v01, scan_interval)
};

static const uint8_t coex_wcn_wake_sync_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wcn_wake_sync_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t coex_wcn_wake_sync_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_wcn_wake_sync_ind_msg_v01, page_interval),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_wcn_wake_sync_ind_msg_v01, timestamp) - QMI_IDL_OFFSET8(coex_wcn_wake_sync_ind_msg_v01, timestamp_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_wcn_wake_sync_ind_msg_v01, timestamp)
};

static const uint8_t coex_get_conflict_params_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_conflict_params_req_msg_v01, victim_tbl_offset)
};

static const uint8_t coex_get_conflict_params_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, wcn_behavior) - QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, wcn_behavior_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, wcn_behavior),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl_offset) - QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl_offset_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl_offset),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl_complete_size) - QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl_complete_size_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl_complete_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl) - QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl),
  COEX_MAX_VICTIM_TBL_ENTRIES_V01,
  QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl) - QMI_IDL_OFFSET8(coex_get_conflict_params_resp_msg_v01, victim_tbl_len),
  QMI_IDL_TYPE88(0, 21),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_conflict_params_resp_msg_v01, victim_tbl_groups) - QMI_IDL_OFFSET16RELATIVE(coex_get_conflict_params_resp_msg_v01, victim_tbl_groups_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(coex_get_conflict_params_resp_msg_v01, victim_tbl_groups),
  COEX_MAX_VICTIM_TBL_ENTRIES_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_conflict_params_resp_msg_v01, victim_tbl_groups) - QMI_IDL_OFFSET16RELATIVE(coex_get_conflict_params_resp_msg_v01, victim_tbl_groups_len)
};

static const uint8_t coex_metrics_start_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_start_req_msg_v01, tech),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_start_req_msg_v01, alpha),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_start_req_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_start_req_msg_v01, carrier_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_start_req_msg_v01, carrier)
};

static const uint8_t coex_metrics_start_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_start_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_start_resp_msg_v01, tech) - QMI_IDL_OFFSET8(coex_metrics_start_resp_msg_v01, tech_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_start_resp_msg_v01, tech),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_start_resp_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_start_resp_msg_v01, carrier_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_start_resp_msg_v01, carrier)
};

static const uint8_t coex_metrics_read_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_req_msg_v01, tech),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_req_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_read_req_msg_v01, carrier_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_req_msg_v01, carrier)
};

static const uint8_t coex_metrics_read_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, tech) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, tech_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, tech),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_bt_only) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_bt_only_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_bt_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_wifi_only) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_wifi_only_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_wifi_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_bt_and_wifi) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_bt_and_wifi_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_bt_and_wifi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_mdm_only) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_mdm_only_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, sinr_mdm_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_bt_only) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_bt_only_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_bt_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_wifi_only) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_wifi_only_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_wifi_only),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_bt_and_wifi) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_bt_and_wifi_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_bt_and_wifi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_mdm_only) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_mdm_only_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, nb_noise_mdm_only),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, carrier_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_read_resp_msg_v01, carrier)
};

static const uint8_t coex_metrics_stop_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_stop_req_msg_v01, tech),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_stop_req_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_stop_req_msg_v01, carrier_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_stop_req_msg_v01, carrier)
};

static const uint8_t coex_metrics_stop_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_stop_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_stop_resp_msg_v01, tech) - QMI_IDL_OFFSET8(coex_metrics_stop_resp_msg_v01, tech_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_stop_resp_msg_v01, tech),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_stop_resp_msg_v01, carrier) - QMI_IDL_OFFSET8(coex_metrics_stop_resp_msg_v01, carrier_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_stop_resp_msg_v01, carrier)
};

static const uint8_t coex_set_scan_freq_band_filter_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_scan_freq_band_filter_req_msg_v01, bands) - QMI_IDL_OFFSET8(coex_set_scan_freq_band_filter_req_msg_v01, bands_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_scan_freq_band_filter_req_msg_v01, bands),
  COEX_WWAN_MAX_BANDS_TO_MONITOR_V01,
  QMI_IDL_OFFSET8(coex_set_scan_freq_band_filter_req_msg_v01, bands) - QMI_IDL_OFFSET8(coex_set_scan_freq_band_filter_req_msg_v01, bands_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t coex_set_scan_freq_band_filter_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_scan_freq_band_filter_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * coex_get_scan_freq_band_filter_req_msg is empty
 * static const uint8_t coex_get_scan_freq_band_filter_req_msg_data_v01[] = {
 * };
 */

static const uint8_t coex_get_scan_freq_band_filter_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_scan_freq_band_filter_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_scan_freq_band_filter_resp_msg_v01, bands) - QMI_IDL_OFFSET8(coex_get_scan_freq_band_filter_resp_msg_v01, bands_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_scan_freq_band_filter_resp_msg_v01, bands),
  COEX_WWAN_MAX_BANDS_TO_MONITOR_V01,
  QMI_IDL_OFFSET8(coex_get_scan_freq_band_filter_resp_msg_v01, bands) - QMI_IDL_OFFSET8(coex_get_scan_freq_band_filter_resp_msg_v01, bands_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t coex_get_wlan_high_prio_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_get_wlan_high_prio_state_req_msg_v01, id)
};

static const uint8_t coex_get_wlan_high_prio_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wlan_high_prio_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_wlan_high_prio_state_resp_msg_v01, high_prio_info) - QMI_IDL_OFFSET8(coex_get_wlan_high_prio_state_resp_msg_v01, high_prio_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_get_wlan_high_prio_state_resp_msg_v01, high_prio_info),
  QMI_IDL_TYPE88(0, 22)
};

static const uint8_t coex_configure_lte_5ghz_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_configure_lte_5ghz_req_msg_v01, enable)
};

static const uint8_t coex_configure_lte_5ghz_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_configure_lte_5ghz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t coex_lte_5ghz_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_lte_5ghz_status_ind_msg_v01, lte_5ghz_active) - QMI_IDL_OFFSET8(coex_lte_5ghz_status_ind_msg_v01, lte_5ghz_active_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_lte_5ghz_status_ind_msg_v01, lte_5ghz_active),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_lte_5ghz_status_ind_msg_v01, frequency) - QMI_IDL_OFFSET8(coex_lte_5ghz_status_ind_msg_v01, frequency_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_lte_5ghz_status_ind_msg_v01, frequency),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET8(coex_lte_5ghz_status_ind_msg_v01, frequency) - QMI_IDL_OFFSET8(coex_lte_5ghz_status_ind_msg_v01, frequency_len)
};

static const uint8_t coex_antenna_switch_to_wlan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_antenna_switch_to_wlan_req_msg_v01, antenna)
};

static const uint8_t coex_antenna_switch_to_wlan_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_antenna_switch_to_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_antenna_switch_to_wlan_resp_msg_v01, grant) - QMI_IDL_OFFSET8(coex_antenna_switch_to_wlan_resp_msg_v01, grant_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_antenna_switch_to_wlan_resp_msg_v01, grant)
};

static const uint8_t coex_antenna_switch_to_mdm_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(coex_antenna_switch_to_mdm_req_msg_v01, antenna)
};

static const uint8_t coex_antenna_switch_to_mdm_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_antenna_switch_to_mdm_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t coex_tx_antenna_state_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_tx_antenna_state_ind_msg_v01, tx_antenna_state) - QMI_IDL_OFFSET8(coex_tx_antenna_state_ind_msg_v01, tx_antenna_state_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_tx_antenna_state_ind_msg_v01, tx_antenna_state),
  COEX_MAX_TX_ANTENNA_V01,
  QMI_IDL_OFFSET8(coex_tx_antenna_state_ind_msg_v01, tx_antenna_state) - QMI_IDL_OFFSET8(coex_tx_antenna_state_ind_msg_v01, tx_antenna_state_len),
  QMI_IDL_TYPE88(0, 24)
};

/* Type Table */
static const qmi_idl_type_table_entry  coex_type_table_v01[] = {
  {sizeof(coex_band_type_v01), coex_band_type_data_v01},
  {sizeof(coex_xsver_band_type_v01), coex_xsver_band_type_data_v01},
  {sizeof(coex_wlan_scan_info_type_v01), coex_wlan_scan_info_type_data_v01},
  {sizeof(coex_wlan_conn_info_type_v01), coex_wlan_conn_info_type_data_v01},
  {sizeof(coex_lte_tdd_info_type_v01), coex_lte_tdd_info_type_data_v01},
  {sizeof(coex_filter_band_info_type_v01), coex_filter_band_info_type_data_v01},
  {sizeof(coex_range_type_v01), coex_range_type_data_v01},
  {sizeof(coex_frame_denial_duty_cycle_params_type_v01), coex_frame_denial_duty_cycle_params_type_data_v01},
  {sizeof(coex_band_type_and_direction_v01), coex_band_type_and_direction_data_v01},
  {sizeof(coex_wwan_band_range_type_v01), coex_wwan_band_range_type_data_v01},
  {sizeof(coex_wwan_tx_thlds_type_v01), coex_wwan_tx_thlds_type_data_v01},
  {sizeof(coex_wwan_tx_frame_denials_params_type_v01), coex_wwan_tx_frame_denials_params_type_data_v01},
  {sizeof(coex_wcn_prio_params_type_v01), coex_wcn_prio_params_type_data_v01},
  {sizeof(coex_rb_threshold_type_v01), coex_rb_threshold_type_data_v01},
  {sizeof(coex_tx_advanced_params_type_v01), coex_tx_advanced_params_type_data_v01},
  {sizeof(coex_pwr_limit_params_type_v01), coex_pwr_limit_params_type_data_v01},
  {sizeof(coex_conflict_mdm_params_type_v01), coex_conflict_mdm_params_type_data_v01},
  {sizeof(coex_wcn_band_range_type_v01), coex_wcn_band_range_type_data_v01},
  {sizeof(coex_wwan_rx_imd_protection_params_type_v01), coex_wwan_rx_imd_protection_params_type_data_v01},
  {sizeof(coex_bt_afh_update_range_type_v01), coex_bt_afh_update_range_type_data_v01},
  {sizeof(coex_conflict_wcn_params_type_v01), coex_conflict_wcn_params_type_data_v01},
  {sizeof(coex_conflict_definition_type_v01), coex_conflict_definition_type_data_v01},
  {sizeof(coex_wlan_high_prio_info_type_v01), coex_wlan_high_prio_info_type_data_v01},
  {sizeof(coex_lte_carrier_info_type_v01), coex_lte_carrier_info_type_data_v01},
  {sizeof(coex_tx_antenna_state_type_v01), coex_tx_antenna_state_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry coex_message_table_v01[] = {
  {sizeof(coex_reset_req_msg_v01), 0},
  {sizeof(coex_reset_resp_msg_v01), coex_reset_resp_msg_data_v01},
  {sizeof(coex_indication_register_req_msg_v01), coex_indication_register_req_msg_data_v01},
  {sizeof(coex_indication_register_resp_msg_v01), coex_indication_register_resp_msg_data_v01},
  {sizeof(coex_wwan_state_ind_msg_v01), coex_wwan_state_ind_msg_data_v01},
  {sizeof(coex_get_wwan_state_req_msg_v01), 0},
  {sizeof(coex_get_wwan_state_resp_msg_v01), coex_get_wwan_state_resp_msg_data_v01},
  {sizeof(coex_set_wlan_state_req_msg_v01), coex_set_wlan_state_req_msg_data_v01},
  {sizeof(coex_set_wlan_state_resp_msg_v01), coex_set_wlan_state_resp_msg_data_v01},
  {sizeof(coex_get_wlan_scan_state_req_msg_v01), coex_get_wlan_scan_state_req_msg_data_v01},
  {sizeof(coex_get_wlan_scan_state_resp_msg_v01), coex_get_wlan_scan_state_resp_msg_data_v01},
  {sizeof(coex_get_wlan_conn_state_req_msg_v01), coex_get_wlan_conn_state_req_msg_data_v01},
  {sizeof(coex_get_wlan_conn_state_resp_msg_v01), coex_get_wlan_conn_state_resp_msg_data_v01},
  {sizeof(coex_set_policy_req_msg_v01), coex_set_policy_req_msg_data_v01},
  {sizeof(coex_set_policy_resp_msg_v01), coex_set_policy_resp_msg_data_v01},
  {sizeof(coex_get_policy_req_msg_v01), 0},
  {sizeof(coex_get_policy_resp_msg_v01), coex_get_policy_resp_msg_data_v01},
  {sizeof(coex_metrics_lte_bler_start_req_msg_v01), coex_metrics_lte_bler_start_req_msg_data_v01},
  {sizeof(coex_metrics_lte_bler_start_resp_msg_v01), coex_metrics_lte_bler_start_resp_msg_data_v01},
  {sizeof(coex_metrics_lte_bler_ind_msg_v01), coex_metrics_lte_bler_ind_msg_data_v01},
  {sizeof(coex_metrics_lte_bler_stop_req_msg_v01), 0},
  {sizeof(coex_metrics_lte_bler_stop_resp_msg_v01), coex_metrics_lte_bler_stop_resp_msg_data_v01},
  {sizeof(coex_metrics_lte_sinr_start_req_msg_v01), coex_metrics_lte_sinr_start_req_msg_data_v01},
  {sizeof(coex_metrics_lte_sinr_start_resp_msg_v01), coex_metrics_lte_sinr_start_resp_msg_data_v01},
  {sizeof(coex_metrics_lte_sinr_read_req_msg_v01), coex_metrics_lte_sinr_read_req_msg_data_v01},
  {sizeof(coex_metrics_lte_sinr_read_resp_msg_v01), coex_metrics_lte_sinr_read_resp_msg_data_v01},
  {sizeof(coex_metrics_lte_sinr_stop_req_msg_v01), coex_metrics_lte_sinr_stop_req_msg_data_v01},
  {sizeof(coex_metrics_lte_sinr_stop_resp_msg_v01), coex_metrics_lte_sinr_stop_resp_msg_data_v01},
  {sizeof(coex_set_band_filter_info_req_msg_v01), coex_set_band_filter_info_req_msg_data_v01},
  {sizeof(coex_set_band_filter_info_resp_msg_v01), coex_set_band_filter_info_resp_msg_data_v01},
  {sizeof(coex_get_band_filter_info_req_msg_v01), 0},
  {sizeof(coex_get_band_filter_info_resp_msg_v01), coex_get_band_filter_info_resp_msg_data_v01},
  {sizeof(coex_condition_fail_ind_msg_v01), coex_condition_fail_ind_msg_data_v01},
  {sizeof(coex_condition_success_ind_msg_v01), coex_condition_success_ind_msg_data_v01},
  {sizeof(coex_get_wci2_mws_params_req_msg_v01), 0},
  {sizeof(coex_get_wci2_mws_params_resp_msg_v01), coex_get_wci2_mws_params_resp_msg_data_v01},
  {sizeof(coex_set_sleep_notification_req_msg_v01), coex_set_sleep_notification_req_msg_data_v01},
  {sizeof(coex_set_sleep_notification_resp_msg_v01), coex_set_sleep_notification_resp_msg_data_v01},
  {sizeof(coex_get_sleep_notification_req_msg_v01), coex_get_sleep_notification_req_msg_data_v01},
  {sizeof(coex_get_sleep_notification_resp_msg_v01), coex_get_sleep_notification_resp_msg_data_v01},
  {sizeof(coex_sleep_ind_msg_v01), coex_sleep_ind_msg_data_v01},
  {sizeof(coex_wakeup_ind_msg_v01), coex_wakeup_ind_msg_data_v01},
  {sizeof(coex_wcn_wake_sync_req_msg_v01), coex_wcn_wake_sync_req_msg_data_v01},
  {sizeof(coex_wcn_wake_sync_resp_msg_v01), coex_wcn_wake_sync_resp_msg_data_v01},
  {sizeof(coex_wcn_wake_sync_ind_msg_v01), coex_wcn_wake_sync_ind_msg_data_v01},
  {sizeof(coex_get_conflict_params_req_msg_v01), coex_get_conflict_params_req_msg_data_v01},
  {sizeof(coex_get_conflict_params_resp_msg_v01), coex_get_conflict_params_resp_msg_data_v01},
  {sizeof(coex_metrics_start_req_msg_v01), coex_metrics_start_req_msg_data_v01},
  {sizeof(coex_metrics_start_resp_msg_v01), coex_metrics_start_resp_msg_data_v01},
  {sizeof(coex_metrics_read_req_msg_v01), coex_metrics_read_req_msg_data_v01},
  {sizeof(coex_metrics_read_resp_msg_v01), coex_metrics_read_resp_msg_data_v01},
  {sizeof(coex_metrics_stop_req_msg_v01), coex_metrics_stop_req_msg_data_v01},
  {sizeof(coex_metrics_stop_resp_msg_v01), coex_metrics_stop_resp_msg_data_v01},
  {sizeof(coex_set_scan_freq_band_filter_req_msg_v01), coex_set_scan_freq_band_filter_req_msg_data_v01},
  {sizeof(coex_set_scan_freq_band_filter_resp_msg_v01), coex_set_scan_freq_band_filter_resp_msg_data_v01},
  {sizeof(coex_get_scan_freq_band_filter_req_msg_v01), 0},
  {sizeof(coex_get_scan_freq_band_filter_resp_msg_v01), coex_get_scan_freq_band_filter_resp_msg_data_v01},
  {sizeof(coex_get_wlan_high_prio_state_req_msg_v01), coex_get_wlan_high_prio_state_req_msg_data_v01},
  {sizeof(coex_get_wlan_high_prio_state_resp_msg_v01), coex_get_wlan_high_prio_state_resp_msg_data_v01},
  {sizeof(coex_configure_lte_5ghz_req_msg_v01), coex_configure_lte_5ghz_req_msg_data_v01},
  {sizeof(coex_configure_lte_5ghz_resp_msg_v01), coex_configure_lte_5ghz_resp_msg_data_v01},
  {sizeof(coex_lte_5ghz_status_ind_msg_v01), coex_lte_5ghz_status_ind_msg_data_v01},
  {sizeof(coex_antenna_switch_to_wlan_req_msg_v01), coex_antenna_switch_to_wlan_req_msg_data_v01},
  {sizeof(coex_antenna_switch_to_wlan_resp_msg_v01), coex_antenna_switch_to_wlan_resp_msg_data_v01},
  {sizeof(coex_antenna_switch_to_mdm_req_msg_v01), coex_antenna_switch_to_mdm_req_msg_data_v01},
  {sizeof(coex_antenna_switch_to_mdm_resp_msg_v01), coex_antenna_switch_to_mdm_resp_msg_data_v01},
  {sizeof(coex_tx_antenna_state_ind_msg_v01), coex_tx_antenna_state_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object coex_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *coex_qmi_idl_type_table_object_referenced_tables_v01[] =
{&coex_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object coex_qmi_idl_type_table_object_v01 = {
  sizeof(coex_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(coex_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  coex_type_table_v01,
  coex_message_table_v01,
  coex_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry coex_service_command_messages_v01[] = {
  {QMI_COEX_RESET_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_COEX_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_COEX_GET_SUPPORTED_FIELDS_REQ_V01, QMI_IDL_TYPE16(1, 2), 5},
  {QMI_COEX_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 2), 36},
  {QMI_COEX_GET_WWAN_STATE_REQ_V01, QMI_IDL_TYPE16(0, 5), 0},
  {QMI_COEX_SET_WLAN_STATE_REQ_V01, QMI_IDL_TYPE16(0, 7), 118},
  {QMI_COEX_GET_WLAN_SCAN_STATE_REQ_V01, QMI_IDL_TYPE16(0, 9), 7},
  {QMI_COEX_GET_WLAN_CONN_STATE_REQ_V01, QMI_IDL_TYPE16(0, 11), 7},
  {QMI_COEX_SET_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 13), 123},
  {QMI_COEX_GET_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 15), 0},
  {QMI_COEX_METRICS_LTE_BLER_START_REQ_V01, QMI_IDL_TYPE16(0, 17), 14},
  {QMI_COEX_METRICS_LTE_BLER_STOP_REQ_V01, QMI_IDL_TYPE16(0, 20), 0},
  {QMI_COEX_METRICS_LTE_SINR_START_REQ_V01, QMI_IDL_TYPE16(0, 22), 14},
  {QMI_COEX_METRICS_LTE_SINR_READ_REQ_V01, QMI_IDL_TYPE16(0, 24), 7},
  {QMI_COEX_METRICS_LTE_SINR_STOP_REQ_V01, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_COEX_SET_BAND_FILTER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 28), 772},
  {QMI_COEX_GET_BAND_FILTER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 30), 0},
  {QMI_COEX_GET_WCI2_MWS_PARAMS_REQ_V01, QMI_IDL_TYPE16(0, 34), 0},
  {QMI_COEX_GET_SLEEP_NOTIFICATION_REQ_V01, QMI_IDL_TYPE16(0, 38), 7},
  {QMI_COEX_SET_SLEEP_NOTIFICATION_REQ_V01, QMI_IDL_TYPE16(0, 36), 14},
  {QMI_COEX_WCN_WAKE_SYNC_REQ_V01, QMI_IDL_TYPE16(0, 42), 11},
  {QMI_COEX_GET_CONFLICT_PARAMS_REQ_V01, QMI_IDL_TYPE16(0, 45), 7},
  {QMI_COEX_METRICS_START_REQ_V01, QMI_IDL_TYPE16(0, 47), 21},
  {QMI_COEX_METRICS_READ_REQ_V01, QMI_IDL_TYPE16(0, 49), 14},
  {QMI_COEX_METRICS_STOP_REQ_V01, QMI_IDL_TYPE16(0, 51), 14},
  {QMI_COEX_SET_SCAN_FREQ_BAND_FILTER_REQ_V01, QMI_IDL_TYPE16(0, 53), 260},
  {QMI_COEX_GET_SCAN_FREQ_BAND_FILTER_REQ_V01, QMI_IDL_TYPE16(0, 55), 0},
  {QMI_COEX_GET_WLAN_HIGH_PRIO_STATE_REQ_V01, QMI_IDL_TYPE16(0, 57), 7},
  {QMI_COEX_CONFIGURE_LTE_5GHZ_REQ_V01, QMI_IDL_TYPE16(0, 59), 4},
  {QMI_COEX_SWITCH_ANTENNA_TO_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 62), 11},
  {QMI_COEX_SWITCH_ANTENNA_TO_MDM_REQ_V01, QMI_IDL_TYPE16(0, 64), 11}
};

static const qmi_idl_service_message_table_entry coex_service_response_messages_v01[] = {
  {QMI_COEX_RESET_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_COEX_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_COEX_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_COEX_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_COEX_GET_WWAN_STATE_RESP_V01, QMI_IDL_TYPE16(0, 6), 2928},
  {QMI_COEX_SET_WLAN_STATE_RESP_V01, QMI_IDL_TYPE16(0, 8), 7},
  {QMI_COEX_GET_WLAN_SCAN_STATE_RESP_V01, QMI_IDL_TYPE16(0, 10), 26},
  {QMI_COEX_GET_WLAN_CONN_STATE_RESP_V01, QMI_IDL_TYPE16(0, 12), 87},
  {QMI_COEX_SET_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_COEX_GET_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 16), 130},
  {QMI_COEX_METRICS_LTE_BLER_START_RESP_V01, QMI_IDL_TYPE16(0, 18), 7},
  {QMI_COEX_METRICS_LTE_BLER_STOP_RESP_V01, QMI_IDL_TYPE16(0, 21), 7},
  {QMI_COEX_METRICS_LTE_SINR_START_RESP_V01, QMI_IDL_TYPE16(0, 23), 14},
  {QMI_COEX_METRICS_LTE_SINR_READ_RESP_V01, QMI_IDL_TYPE16(0, 25), 49},
  {QMI_COEX_METRICS_LTE_SINR_STOP_RESP_V01, QMI_IDL_TYPE16(0, 27), 14},
  {QMI_COEX_SET_BAND_FILTER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 29), 7},
  {QMI_COEX_GET_BAND_FILTER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 31), 779},
  {QMI_COEX_GET_WCI2_MWS_PARAMS_RESP_V01, QMI_IDL_TYPE16(0, 35), 117},
  {QMI_COEX_GET_SLEEP_NOTIFICATION_RESP_V01, QMI_IDL_TYPE16(0, 39), 21},
  {QMI_COEX_SET_SLEEP_NOTIFICATION_RESP_V01, QMI_IDL_TYPE16(0, 37), 7},
  {QMI_COEX_WCN_WAKE_SYNC_RESP_V01, QMI_IDL_TYPE16(0, 43), 7},
  {QMI_COEX_GET_CONFLICT_PARAMS_RESP_V01, QMI_IDL_TYPE16(0, 46), 3688},
  {QMI_COEX_METRICS_START_RESP_V01, QMI_IDL_TYPE16(0, 48), 21},
  {QMI_COEX_METRICS_READ_RESP_V01, QMI_IDL_TYPE16(0, 50), 91},
  {QMI_COEX_METRICS_STOP_RESP_V01, QMI_IDL_TYPE16(0, 52), 21},
  {QMI_COEX_SET_SCAN_FREQ_BAND_FILTER_RESP_V01, QMI_IDL_TYPE16(0, 54), 7},
  {QMI_COEX_GET_SCAN_FREQ_BAND_FILTER_RESP_V01, QMI_IDL_TYPE16(0, 56), 267},
  {QMI_COEX_GET_WLAN_HIGH_PRIO_STATE_RESP_V01, QMI_IDL_TYPE16(0, 58), 26},
  {QMI_COEX_CONFIGURE_LTE_5GHZ_RESP_V01, QMI_IDL_TYPE16(0, 60), 7},
  {QMI_COEX_SWITCH_ANTENNA_TO_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 63), 18},
  {QMI_COEX_SWITCH_ANTENNA_TO_MDM_RESP_V01, QMI_IDL_TYPE16(0, 65), 7}
};

static const qmi_idl_service_message_table_entry coex_service_indication_messages_v01[] = {
  {QMI_COEX_WWAN_STATE_IND_V01, QMI_IDL_TYPE16(0, 4), 2921},
  {QMI_COEX_METRICS_LTE_BLER_IND_V01, QMI_IDL_TYPE16(0, 19), 70},
  {QMI_COEX_CONDITION_FAIL_IND_V01, QMI_IDL_TYPE16(0, 32), 33},
  {QMI_COEX_CONDITION_SUCCESS_IND_V01, QMI_IDL_TYPE16(0, 33), 11},
  {QMI_COEX_SLEEP_IND_V01, QMI_IDL_TYPE16(0, 40), 14},
  {QMI_COEX_WAKEUP_IND_V01, QMI_IDL_TYPE16(0, 41), 14},
  {QMI_COEX_WCN_WAKE_SYNC_IND_V01, QMI_IDL_TYPE16(0, 44), 18},
  {QMI_COEX_LTE_5GHZ_STATUS_IND_V01, QMI_IDL_TYPE16(0, 61), 72},
  {QMI_COEX_TX_ANTENNA_STATE_IND_V01, QMI_IDL_TYPE16(0, 66), 28}
};

/*Service Object*/
struct qmi_idl_service_object coex_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x22,
  8204,
  { sizeof(coex_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(coex_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(coex_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { coex_service_command_messages_v01, coex_service_response_messages_v01, coex_service_indication_messages_v01},
  &coex_qmi_idl_type_table_object_v01,
  0x14,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type coex_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( COEX_V01_IDL_MAJOR_VERS != idl_maj_version || COEX_V01_IDL_MINOR_VERS != idl_min_version
       || COEX_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&coex_qmi_idl_service_object_v01;
}

