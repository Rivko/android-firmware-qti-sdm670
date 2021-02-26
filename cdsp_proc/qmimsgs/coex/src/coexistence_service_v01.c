/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C O E X I S T E N C E _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the coex service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/coex/src/coexistence_service_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5 
   It was generated on: Tue Oct  8 2013 (Spin 0)
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

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_page_sync) - QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_page_sync_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_indication_register_req_msg_v01, report_coex_page_sync)
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

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma_band_info_set_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_wwan_state_ind_msg_v01, wcdma_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_wwan_state_ind_msg_v01, wcdma_band_info_set_len),
  QMI_IDL_TYPE88(0, 8)
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

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set),
  COEX_WWAN_MAX_BANDS_PER_TECH_V01,
  QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set) - QMI_IDL_OFFSET16RELATIVE(coex_get_wwan_state_resp_msg_v01, wcdma_band_info_set_len),
  QMI_IDL_TYPE88(0, 8)
};

static const uint8_t coex_set_wlan_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, scan_info) - QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, scan_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, scan_info),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, conn_info) - QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, conn_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_set_wlan_state_req_msg_v01, conn_info),
  QMI_IDL_TYPE88(0, 3)
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

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_threshold_for_adv_tx_notice) - QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_threshold_for_adv_tx_notice_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_set_policy_req_msg_v01, rb_threshold_for_adv_tx_notice)
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

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_threshold_for_adv_tx_notice) - QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_threshold_for_adv_tx_notice_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_get_policy_resp_msg_v01, rb_threshold_for_adv_tx_notice)
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
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_req_msg_v01, alpha)
};

static const uint8_t coex_metrics_lte_sinr_start_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_start_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* 
 * coex_metrics_lte_sinr_read_req_msg is empty
 * static const uint8_t coex_metrics_lte_sinr_read_req_msg_data_v01[] = {
 * };
 */
  
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

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_lte_only) - QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_lte_only_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_read_resp_msg_v01, sinr_lte_only)
};

/* 
 * coex_metrics_lte_sinr_stop_req_msg is empty
 * static const uint8_t coex_metrics_lte_sinr_stop_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t coex_metrics_lte_sinr_stop_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stop_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
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
  {sizeof(coex_band_type_and_direction_v01), coex_band_type_and_direction_data_v01}
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
  {sizeof(coex_metrics_lte_sinr_read_req_msg_v01), 0},
  {sizeof(coex_metrics_lte_sinr_read_resp_msg_v01), coex_metrics_lte_sinr_read_resp_msg_data_v01},
  {sizeof(coex_metrics_lte_sinr_stop_req_msg_v01), 0},
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
  {sizeof(coex_wcn_wake_sync_ind_msg_v01), coex_wcn_wake_sync_ind_msg_data_v01}
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
  {QMI_COEX_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 2), 28},
  {QMI_COEX_GET_WWAN_STATE_REQ_V01, QMI_IDL_TYPE16(0, 5), 0},
  {QMI_COEX_SET_WLAN_STATE_REQ_V01, QMI_IDL_TYPE16(0, 7), 99},
  {QMI_COEX_GET_WLAN_SCAN_STATE_REQ_V01, QMI_IDL_TYPE16(0, 9), 7},
  {QMI_COEX_GET_WLAN_CONN_STATE_REQ_V01, QMI_IDL_TYPE16(0, 11), 7},
  {QMI_COEX_SET_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 13), 101},
  {QMI_COEX_GET_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 15), 0},
  {QMI_COEX_METRICS_LTE_BLER_START_REQ_V01, QMI_IDL_TYPE16(0, 17), 14},
  {QMI_COEX_METRICS_LTE_BLER_STOP_REQ_V01, QMI_IDL_TYPE16(0, 20), 0},
  {QMI_COEX_METRICS_LTE_SINR_START_REQ_V01, QMI_IDL_TYPE16(0, 22), 7},
  {QMI_COEX_METRICS_LTE_SINR_READ_REQ_V01, QMI_IDL_TYPE16(0, 24), 0},
  {QMI_COEX_METRICS_LTE_SINR_STOP_REQ_V01, QMI_IDL_TYPE16(0, 26), 0},
  {QMI_COEX_SET_BAND_FILTER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 28), 772},
  {QMI_COEX_GET_BAND_FILTER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 30), 0},
  {QMI_COEX_GET_WCI2_MWS_PARAMS_REQ_V01, QMI_IDL_TYPE16(0, 34), 0},
  {QMI_COEX_GET_SLEEP_NOTIFICATION_REQ_V01, QMI_IDL_TYPE16(0, 38), 7},
  {QMI_COEX_SET_SLEEP_NOTIFICATION_REQ_V01, QMI_IDL_TYPE16(0, 36), 14},
  {QMI_COEX_WCN_WAKE_SYNC_REQ_V01, QMI_IDL_TYPE16(0, 42), 11}
};

static const qmi_idl_service_message_table_entry coex_service_response_messages_v01[] = {
  {QMI_COEX_RESET_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_COEX_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_COEX_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_COEX_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_COEX_GET_WWAN_STATE_RESP_V01, QMI_IDL_TYPE16(0, 6), 1616},
  {QMI_COEX_SET_WLAN_STATE_RESP_V01, QMI_IDL_TYPE16(0, 8), 7},
  {QMI_COEX_GET_WLAN_SCAN_STATE_RESP_V01, QMI_IDL_TYPE16(0, 10), 26},
  {QMI_COEX_GET_WLAN_CONN_STATE_RESP_V01, QMI_IDL_TYPE16(0, 12), 87},
  {QMI_COEX_SET_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_COEX_GET_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 16), 108},
  {QMI_COEX_METRICS_LTE_BLER_START_RESP_V01, QMI_IDL_TYPE16(0, 18), 7},
  {QMI_COEX_METRICS_LTE_BLER_STOP_RESP_V01, QMI_IDL_TYPE16(0, 21), 7},
  {QMI_COEX_METRICS_LTE_SINR_START_RESP_V01, QMI_IDL_TYPE16(0, 23), 7},
  {QMI_COEX_METRICS_LTE_SINR_READ_RESP_V01, QMI_IDL_TYPE16(0, 25), 42},
  {QMI_COEX_METRICS_LTE_SINR_STOP_RESP_V01, QMI_IDL_TYPE16(0, 27), 7},
  {QMI_COEX_SET_BAND_FILTER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 29), 7},
  {QMI_COEX_GET_BAND_FILTER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 31), 779},
  {QMI_COEX_GET_WCI2_MWS_PARAMS_RESP_V01, QMI_IDL_TYPE16(0, 35), 117},
  {QMI_COEX_GET_SLEEP_NOTIFICATION_RESP_V01, QMI_IDL_TYPE16(0, 39), 21},
  {QMI_COEX_SET_SLEEP_NOTIFICATION_RESP_V01, QMI_IDL_TYPE16(0, 37), 7},
  {QMI_COEX_WCN_WAKE_SYNC_RESP_V01, QMI_IDL_TYPE16(0, 43), 7}
};

static const qmi_idl_service_message_table_entry coex_service_indication_messages_v01[] = {
  {QMI_COEX_WWAN_STATE_IND_V01, QMI_IDL_TYPE16(0, 4), 1609},
  {QMI_COEX_METRICS_LTE_BLER_IND_V01, QMI_IDL_TYPE16(0, 19), 70},
  {QMI_COEX_CONDITION_FAIL_IND_V01, QMI_IDL_TYPE16(0, 32), 33},
  {QMI_COEX_CONDITION_SUCCESS_IND_V01, QMI_IDL_TYPE16(0, 33), 11},
  {QMI_COEX_SLEEP_IND_V01, QMI_IDL_TYPE16(0, 40), 14},
  {QMI_COEX_WAKEUP_IND_V01, QMI_IDL_TYPE16(0, 41), 14},
  {QMI_COEX_WCN_WAKE_SYNC_IND_V01, QMI_IDL_TYPE16(0, 44), 18}
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
  0x0A,
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

