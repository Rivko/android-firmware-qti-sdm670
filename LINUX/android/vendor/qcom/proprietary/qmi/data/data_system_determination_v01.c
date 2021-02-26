/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A _ S Y S T E M _ D E T E R M I N A T I O N _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the dsd service Data structures.

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Thu Dec 28 2017 (Spin 0)
   From IDL File: data_system_determination_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "data_system_determination_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t dsd_system_status_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_info_type_v01, technology),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_info_type_v01, rat_value),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_info_type_v01, so_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_apn_avail_sys_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_apn_avail_sys_info_type_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_apn_avail_sys_info_type_v01, apn_avail_sys),
  QMI_DSD_MAX_AVAIL_SYS_V01,
  QMI_IDL_OFFSET8(dsd_apn_avail_sys_info_type_v01, apn_avail_sys) - QMI_IDL_OFFSET8(dsd_apn_avail_sys_info_type_v01, apn_avail_sys_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_apn_pref_sys_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_apn_pref_sys_info_type_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_apn_pref_sys_info_type_v01, curr_pref_sys),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_apn_pref_sys_info_type_v01, recommended_pref_sys),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_global_pref_sys_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_global_pref_sys_info_type_v01, curr_pref_sys),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_global_pref_sys_info_type_v01, recommended_pref_sys),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_ipv6_addr_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_ipv6_addr_type_v01, ipv6_address),
  QMI_DSD_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_ipv6_addr_type_v01, prefix_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_apn_pref_sys_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_apn_pref_sys_type_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_apn_pref_sys_type_v01, pref_sys),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_apn_name_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_apn_name_type_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_apn_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_apn_info_type_v01, apn_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_apn_info_type_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_ident_bssid_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_bssid_type_v01, bssid),
  QMI_DSD_MAC_ADDR_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_bssid_type_v01, channel_list),
  QMI_DSD_MAX_CHANNELS_V01,
  QMI_IDL_OFFSET8(dsd_wifi_ident_bssid_type_v01, channel_list) - QMI_IDL_OFFSET8(dsd_wifi_ident_bssid_type_v01, channel_list_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_ident_ssid_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_ssid_type_v01, ssid),
  QMI_DSD_MAX_SSID_LEN_V01,
  QMI_IDL_OFFSET8(dsd_wifi_ident_ssid_type_v01, ssid) - QMI_IDL_OFFSET8(dsd_wifi_ident_ssid_type_v01, ssid_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_ssid_type_v01, is_hidden),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_ssid_type_v01, bssid_list),
  QMI_DSD_MAX_BSSIDS_V01,
  QMI_IDL_OFFSET8(dsd_wifi_ident_ssid_type_v01, bssid_list) - QMI_IDL_OFFSET8(dsd_wifi_ident_ssid_type_v01, bssid_list_len),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_ident_rssi_threshold_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_rssi_threshold_type_v01, rssi_high),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_rssi_threshold_type_v01, rssi_low),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_ident_sinr_threshold_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_sinr_threshold_type_v01, sinr_high),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_sinr_threshold_type_v01, sinr_low),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_ident_bss_load_threshold_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_bss_load_threshold_type_v01, station_count_high),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_bss_load_threshold_type_v01, station_count_low),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_bss_load_threshold_type_v01, channel_utilization_high),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_bss_load_threshold_type_v01, channel_utilization_low),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_bss_load_threshold_type_v01, available_admission_capacity_high),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_ident_bss_load_threshold_type_v01, available_admission_capacity_low),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_blacklist_bssid_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_blacklist_bssid_type_v01, bssid),
  QMI_DSD_MAC_ADDR_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_bss_load_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_bss_load_type_v01, station_count),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_bss_load_type_v01, channel_utilization),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_bss_load_type_v01, available_admission_capacity),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_phy_rate_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_phy_rate_type_v01, downlink_phy_rate),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_phy_rate_type_v01, uplink_phy_rate),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_packet_error_rate_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_packet_error_rate_type_v01, downlink_packet_error_rate),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_packet_error_rate_type_v01, uplink_packet_error_rate),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_meas_info_per_channel_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_channel_type_v01, channel),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_channel_type_v01, threshold_state),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_channel_type_v01, valid_params),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_channel_type_v01, rssi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_channel_type_v01, sinr),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_channel_type_v01, bss_load),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_channel_type_v01, phy_rate),
  QMI_IDL_TYPE88(0, 15),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_channel_type_v01, packet_error_rate),
  QMI_IDL_TYPE88(0, 16),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_channel_type_v01, network_mode),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_meas_info_per_bssid_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_bssid_type_v01, bssid),
  QMI_DSD_MAC_ADDR_LEN_V01,

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_info_per_bssid_type_v01, channel_info),
  QMI_IDL_TYPE88(0, 17),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_wifi_meas_secondary_bssid_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_secondary_bssid_info_type_v01, secondary_channel),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_secondary_bssid_info_type_v01, bandwidth_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_lte_cell_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, serving_cell_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, pci),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, freq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, dl_bandwidth),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_type_v01, band_info),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_lte_scell_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_lte_scell_info_type_v01, cell_info),
  QMI_IDL_TYPE88(0, 20),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_scell_info_type_v01, scell_state),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_lte_scell_info_type_v01, ul_enabled),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_3gpp_mcc_mnc_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_3gpp_mcc_mnc_type_v01, mcc),
  QMI_DSD_MCC_MNC_SIZE_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_3gpp_mcc_mnc_type_v01, mnc),
  QMI_DSD_MCC_MNC_SIZE_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t dsd_apn_type_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_apn_type_info_type_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_apn_type_info_type_v01, apn_type_mask),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * dsd_get_system_status_req_msg is empty
 * static const uint8_t dsd_get_system_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_system_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys) - QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys),
  QMI_DSD_MAX_AVAIL_SYS_V01,
  QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys) - QMI_IDL_OFFSET8(dsd_get_system_status_resp_msg_v01, avail_sys_len),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_avail_sys_info_len),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, global_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, global_pref_sys_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_get_system_status_resp_msg_v01, global_pref_sys),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_pref_sys_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_get_system_status_resp_msg_v01, apn_pref_sys),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_get_system_status_resp_msg_v01, apn_pref_sys_len),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t dsd_system_status_change_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, limit_so_mask_change_ind) - QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, limit_so_mask_change_ind_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, limit_so_mask_change_ind),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_data_system_status_changes) - QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_data_system_status_changes_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_data_system_status_changes),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_pref_tech_change_only) - QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_pref_tech_change_only_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_system_status_change_req_msg_v01, report_pref_tech_change_only)
};

static const uint8_t dsd_system_status_change_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_system_status_change_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_system_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys) - QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys),
  QMI_DSD_MAX_AVAIL_SYS_V01,
  QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys) - QMI_IDL_OFFSET8(dsd_system_status_ind_msg_v01, avail_sys_len),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_avail_sys_info) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_avail_sys_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_system_status_ind_msg_v01, apn_avail_sys_info),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_avail_sys_info) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_avail_sys_info_len),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, global_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, global_pref_sys_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_system_status_ind_msg_v01, global_pref_sys),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_pref_sys_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_system_status_ind_msg_v01, apn_pref_sys),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_pref_sys) - QMI_IDL_OFFSET16RELATIVE(dsd_system_status_ind_msg_v01, apn_pref_sys_len),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t dsd_bind_subscription_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_bind_subscription_req_msg_v01, bind_subs)
};

static const uint8_t dsd_bind_subscription_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * dsd_get_bind_subscription_req_msg is empty
 * static const uint8_t dsd_get_bind_subscription_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_bind_subscription_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_bind_subscription_resp_msg_v01, bind_subscription) - QMI_IDL_OFFSET8(dsd_get_bind_subscription_resp_msg_v01, bind_subscription_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_bind_subscription_resp_msg_v01, bind_subscription)
};

static const uint8_t dsd_wlan_available_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wlan_ap_mac_address),
  QMI_DSD_MAC_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wlan_ipv4_address) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wlan_ipv4_address_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wlan_ipv4_address),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wlan_ipv6_address) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wlan_ipv6_address_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wlan_ipv6_address),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wqe_status) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wqe_status_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wqe_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv4_address_1) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv4_address_1_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv4_address_1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv4_address_2) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv4_address_2_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv4_address_2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv6_address_1) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv6_address_1_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv6_address_1),
  QMI_DSD_IPV6_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv6_address_2) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv6_address_2_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, dns_ipv6_address_2),
  QMI_DSD_IPV6_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv4_address_1) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv4_address_1_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv4_address_1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv4_address_2) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv4_address_2_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv4_address_2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv6_address_1) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv6_address_1_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv6_address_1),
  QMI_DSD_IPV6_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv6_address_2) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv6_address_2_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, epdg_ipv6_address_2),
  QMI_DSD_IPV6_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, ssid) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, ssid_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, ssid),
  QMI_DSD_MAX_SSID_LEN_V01,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, ssid) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, ssid_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, channel) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, channel_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, channel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, channel_bandwidth) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, channel_bandwidth_valid)),
  0x1D,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, channel_bandwidth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wqe_profile_type) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wqe_profile_type_valid)),
  0x1E,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wqe_profile_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wifi_assoc_type) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wifi_assoc_type_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, wifi_assoc_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, network_mode) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, network_mode_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, network_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, connection_status) - QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, connection_status_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_available_req_msg_v01, connection_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_wlan_available_req_msg_v01, secondary_channel) - QMI_IDL_OFFSET16RELATIVE(dsd_wlan_available_req_msg_v01, secondary_channel_valid)),
  0x22,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_wlan_available_req_msg_v01, secondary_channel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_wlan_available_req_msg_v01, bandwidth_type) - QMI_IDL_OFFSET16RELATIVE(dsd_wlan_available_req_msg_v01, bandwidth_type_valid)),
  0x23,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_wlan_available_req_msg_v01, bandwidth_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_wlan_available_req_msg_v01, is_default_route) - QMI_IDL_OFFSET16RELATIVE(dsd_wlan_available_req_msg_v01, is_default_route_valid)),
  0x24,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_wlan_available_req_msg_v01, is_default_route),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_wlan_available_req_msg_v01, hessid) - QMI_IDL_OFFSET16RELATIVE(dsd_wlan_available_req_msg_v01, hessid_valid)),
  0x25,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_wlan_available_req_msg_v01, hessid),
  QMI_DSD_MAX_HESSID_LEN_V01
};

static const uint8_t dsd_wlan_available_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wlan_available_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_wlan_not_available_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wlan_not_available_req_msg_v01, wqe_status) - QMI_IDL_OFFSET8(dsd_wlan_not_available_req_msg_v01, wqe_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wlan_not_available_req_msg_v01, wqe_status)
};

static const uint8_t dsd_wlan_not_available_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wlan_not_available_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_set_wlan_preference_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_set_wlan_preference_req_msg_v01, wlan_preference)
};

static const uint8_t dsd_set_wlan_preference_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_wlan_preference_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * dsd_get_wlan_preference_req_msg is empty
 * static const uint8_t dsd_get_wlan_preference_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_wlan_preference_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_wlan_preference_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_wlan_preference_resp_msg_v01, wlan_preference) - QMI_IDL_OFFSET8(dsd_get_wlan_preference_resp_msg_v01, wlan_preference_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_wlan_preference_resp_msg_v01, wlan_preference)
};

static const uint8_t dsd_set_apn_preferred_system_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_apn_preferred_system_req_msg_v01, apn_pref_sys),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t dsd_set_apn_preferred_system_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_apn_preferred_system_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * dsd_get_modem_power_cost_req_msg is empty
 * static const uint8_t dsd_get_modem_power_cost_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_modem_power_cost_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_modem_power_cost_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_modem_power_cost_resp_msg_v01, power_cost) - QMI_IDL_OFFSET8(dsd_get_modem_power_cost_resp_msg_v01, power_cost_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_modem_power_cost_resp_msg_v01, power_cost)
};

/*
 * dsd_pdn_policy_start_txn_req_msg is empty
 * static const uint8_t dsd_pdn_policy_start_txn_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_pdn_policy_start_txn_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_pdn_policy_start_txn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_pdn_policy_start_txn_resp_msg_v01, txn_id) - QMI_IDL_OFFSET8(dsd_pdn_policy_start_txn_resp_msg_v01, txn_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_pdn_policy_start_txn_resp_msg_v01, txn_id)
};

static const uint8_t dsd_add_pdn_policy_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, txn_id),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, supported_system_priority_list),
  QMI_DSD_MAX_SYSTEMS_V01,
  QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, supported_system_priority_list) - QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, supported_system_priority_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, is_default) - QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, is_default_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, is_default),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, override_type) - QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, override_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_add_pdn_policy_req_msg_v01, override_type)
};

static const uint8_t dsd_add_pdn_policy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_add_pdn_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_modify_pdn_policy_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, txn_id),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, supported_system_priority_list) - QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, supported_system_priority_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, supported_system_priority_list),
  QMI_DSD_MAX_SYSTEMS_V01,
  QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, supported_system_priority_list) - QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, supported_system_priority_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, is_default) - QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, is_default_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, is_default),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, override_type) - QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, override_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_modify_pdn_policy_req_msg_v01, override_type)
};

static const uint8_t dsd_modify_pdn_policy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_modify_pdn_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_delete_pdn_policy_by_apn_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_delete_pdn_policy_by_apn_req_msg_v01, txn_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_delete_pdn_policy_by_apn_req_msg_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01
};

static const uint8_t dsd_delete_pdn_policy_by_apn_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_delete_pdn_policy_by_apn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_get_pdn_policy_apn_list_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_apn_list_req_msg_v01, txn_id)
};

static const uint8_t dsd_get_pdn_policy_apn_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_apn_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_pdn_policy_apn_list_resp_msg_v01, apn_list) - QMI_IDL_OFFSET8(dsd_get_pdn_policy_apn_list_resp_msg_v01, apn_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_apn_list_resp_msg_v01, apn_list),
  QMI_DSD_MAX_APNS_V01,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_apn_list_resp_msg_v01, apn_list) - QMI_IDL_OFFSET8(dsd_get_pdn_policy_apn_list_resp_msg_v01, apn_list_len),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t dsd_get_pdn_policy_settings_for_apn_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_req_msg_v01, txn_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_req_msg_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01
};

static const uint8_t dsd_get_pdn_policy_settings_for_apn_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, apn_name) - QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, apn_name_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, supported_system_priority_list) - QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, supported_system_priority_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, supported_system_priority_list),
  QMI_DSD_MAX_SYSTEMS_V01,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, supported_system_priority_list) - QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, supported_system_priority_list_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, is_default) - QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, is_default_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, is_default),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, override_type) - QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, override_type_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01, override_type)
};

static const uint8_t dsd_pdn_policy_end_txn_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_pdn_policy_end_txn_req_msg_v01, txn_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_pdn_policy_end_txn_req_msg_v01, txn_exec_type)
};

static const uint8_t dsd_pdn_policy_end_txn_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_pdn_policy_end_txn_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_set_apn_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_apn_info_req_msg_v01, apn_info),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_apn_info_req_msg_v01, apn_invalid) - QMI_IDL_OFFSET8(dsd_set_apn_info_req_msg_v01, apn_invalid_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_set_apn_info_req_msg_v01, apn_invalid)
};

static const uint8_t dsd_set_apn_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_apn_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_get_apn_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_apn_info_req_msg_v01, apn_type)
};

static const uint8_t dsd_get_apn_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_apn_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_apn_info_resp_msg_v01, apn_name) - QMI_IDL_OFFSET8(dsd_get_apn_info_resp_msg_v01, apn_name_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_get_apn_info_resp_msg_v01, apn_name),
  QMI_DSD_MAX_APN_LEN_V01
};

static const uint8_t dsd_notify_data_settings_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, data_service_switch) - QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, data_service_switch_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, data_service_switch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, data_service_roaming_switch) - QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, data_service_roaming_switch_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, data_service_roaming_switch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, rat_preference) - QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, rat_preference_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, rat_preference),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, wifi_switch) - QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, wifi_switch_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_notify_data_settings_req_msg_v01, wifi_switch)
};

static const uint8_t dsd_notify_data_settings_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_notify_data_settings_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * dsd_get_data_settings_req_msg is empty
 * static const uint8_t dsd_get_data_settings_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_data_settings_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, data_service_switch) - QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, data_service_switch_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, data_service_switch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, data_service_roaming_switch) - QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, data_service_roaming_switch_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, data_service_roaming_switch),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, rat_preference) - QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, rat_preference_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, rat_preference),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, wifi_switch) - QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, wifi_switch_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_get_data_settings_resp_msg_v01, wifi_switch)
};

static const uint8_t dsd_thermal_info_change_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_thermal_info_change_ind_msg_v01, thermal_action) - QMI_IDL_OFFSET8(dsd_thermal_info_change_ind_msg_v01, thermal_action_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_thermal_info_change_ind_msg_v01, thermal_action)
};

/*
 * dsd_get_thermal_mitigation_info_req_msg is empty
 * static const uint8_t dsd_get_thermal_mitigation_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_thermal_mitigation_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_thermal_mitigation_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_thermal_mitigation_info_resp_msg_v01, thermal_action) - QMI_IDL_OFFSET8(dsd_get_thermal_mitigation_info_resp_msg_v01, thermal_action_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_thermal_mitigation_info_resp_msg_v01, thermal_action)
};

static const uint8_t dsd_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_thermal_info_changes) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_thermal_info_changes_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_thermal_info_changes),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wifi_meas_trigger) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wifi_meas_trigger_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wifi_meas_trigger),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wqe_profile_type_changes) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wqe_profile_type_changes_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wqe_profile_type_changes),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wqe_profile_met) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wqe_profile_met_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wqe_profile_met),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_nat_keep_alive_info) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_nat_keep_alive_info_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_nat_keep_alive_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_roaming_status_change) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_roaming_status_change_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_roaming_status_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wwan_conn_state_type) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wwan_conn_state_type_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_wwan_conn_state_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_lte_cell_info_change) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_lte_cell_info_change_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_lte_cell_info_change),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_current_dds) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_current_dds_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_current_dds),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_vacate_wifi_band) - QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_vacate_wifi_band_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_indication_register_req_msg_v01, report_vacate_wifi_band)
};

static const uint8_t dsd_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_start_wifi_meas_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, wifi_meas_id),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, meas_param),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, report_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, rssi_threshold) - QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, rssi_threshold_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, rssi_threshold),
  QMI_IDL_TYPE88(0, 10),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, sinr_threshold) - QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, sinr_threshold_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, sinr_threshold),
  QMI_IDL_TYPE88(0, 11),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, bss_load_threshold) - QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, bss_load_threshold_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, bss_load_threshold),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, ident_param_list) - QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, ident_param_list_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, ident_param_list),
  QMI_DSD_MAX_SSIDS_V01,
  QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, ident_param_list) - QMI_IDL_OFFSET8(dsd_start_wifi_meas_ind_msg_v01, ident_param_list_len),
  QMI_IDL_TYPE88(0, 9),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, sampling_timer) - QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, sampling_timer_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_start_wifi_meas_ind_msg_v01, sampling_timer),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, report_interval) - QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, report_interval_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_start_wifi_meas_ind_msg_v01, report_interval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, alpha) - QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, alpha_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_start_wifi_meas_ind_msg_v01, alpha),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, time_to_trigger) - QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, time_to_trigger_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_start_wifi_meas_ind_msg_v01, time_to_trigger),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, blacklisted_bssids) - QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, blacklisted_bssids_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_start_wifi_meas_ind_msg_v01, blacklisted_bssids),
  QMI_DSD_MAX_BSSIDS_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, blacklisted_bssids) - QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, blacklisted_bssids_len),
  QMI_IDL_TYPE88(0, 13),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, wlan_fw_action) - QMI_IDL_OFFSET16RELATIVE(dsd_start_wifi_meas_ind_msg_v01, wlan_fw_action_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(dsd_start_wifi_meas_ind_msg_v01, wlan_fw_action)
};

static const uint8_t dsd_stop_wifi_meas_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_stop_wifi_meas_ind_msg_v01, wifi_meas_id)
};

static const uint8_t dsd_wifi_meas_report_config_status_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_config_status_req_msg_v01, wifi_meas_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_config_status_req_msg_v01, status)
};

static const uint8_t dsd_wifi_meas_report_config_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_config_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_wifi_meas_report_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, wifi_meas_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, tx_id),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, report_type),

  0x04,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, is_last_report_of_transaction),

  0x05,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, ssid),
  QMI_DSD_MAX_SSID_LEN_V01,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, ssid) - QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, ssid_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, per_bssid_info) - QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, per_bssid_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, per_bssid_info),
  QMI_DSD_MAX_BSSIDS_PER_REPORT_V01,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, per_bssid_info) - QMI_IDL_OFFSET8(dsd_wifi_meas_report_req_msg_v01, per_bssid_info_len),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_wifi_meas_report_req_msg_v01, sec_bssid_info) - QMI_IDL_OFFSET16RELATIVE(dsd_wifi_meas_report_req_msg_v01, sec_bssid_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_wifi_meas_report_req_msg_v01, sec_bssid_info),
  QMI_DSD_MAX_BSSIDS_PER_REPORT_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_wifi_meas_report_req_msg_v01, sec_bssid_info) - QMI_IDL_OFFSET16RELATIVE(dsd_wifi_meas_report_req_msg_v01, sec_bssid_info_len),
  QMI_IDL_TYPE88(0, 19)
};

static const uint8_t dsd_wifi_meas_report_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_meas_report_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_set_apn_call_type_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(dsd_set_apn_call_type_req_msg_v01, apn),
  QMI_DSD_MAX_APN_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_set_apn_call_type_req_msg_v01, apn_call_type)
};

static const uint8_t dsd_set_apn_call_type_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_apn_call_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_set_wqe_profile_type_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_set_wqe_profile_type_req_msg_v01, wqe_profile_type)
};

static const uint8_t dsd_set_wqe_profile_type_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_wqe_profile_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_wqe_profile_type_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_wqe_profile_type_ind_msg_v01, set_wqe_profile_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wqe_profile_type_ind_msg_v01, iccid) - QMI_IDL_OFFSET8(dsd_wqe_profile_type_ind_msg_v01, iccid_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wqe_profile_type_ind_msg_v01, iccid),
  QMI_DSD_MAX_ICCID_LEN_V01,
  QMI_IDL_OFFSET8(dsd_wqe_profile_type_ind_msg_v01, iccid) - QMI_IDL_OFFSET8(dsd_wqe_profile_type_ind_msg_v01, iccid_len)
};

static const uint8_t dsd_wqe_profile_type_met_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_wqe_profile_type_met_ind_msg_v01, met_wqe_profile_type)
};

static const uint8_t dsd_nat_keep_alive_info_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, trigger_nat_keep_alive),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, dest_v4_address) - QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, dest_v4_address_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, dest_v4_address),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, dest_ipv6_address) - QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, dest_ipv6_address_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, dest_ipv6_address),
  QMI_DSD_IPV6_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, dest_port) - QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, dest_port_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, dest_port),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, source_port) - QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, source_port_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, source_port),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, timer_value) - QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, timer_value_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_nat_keep_alive_info_ind_msg_v01, timer_value)
};

static const uint8_t dsd_nat_keep_alive_config_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_nat_keep_alive_config_status_req_msg_v01, status)
};

static const uint8_t dsd_nat_keep_alive_config_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_nat_keep_alive_config_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_set_wqe_profile_quality_status_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_set_wqe_profile_quality_status_req_msg_v01, wqe_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_wqe_profile_quality_status_req_msg_v01, quality_status) - QMI_IDL_OFFSET8(dsd_set_wqe_profile_quality_status_req_msg_v01, quality_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_set_wqe_profile_quality_status_req_msg_v01, quality_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_wqe_profile_quality_status_req_msg_v01, status_code) - QMI_IDL_OFFSET8(dsd_set_wqe_profile_quality_status_req_msg_v01, status_code_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_set_wqe_profile_quality_status_req_msg_v01, status_code)
};

static const uint8_t dsd_set_wqe_profile_quality_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_wqe_profile_quality_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_set_quality_measurement_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_lte_rsrp_low) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_lte_rsrp_low_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_lte_rsrp_low),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_lte_rsrp_mid) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_lte_rsrp_mid_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_lte_rsrp_mid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_lte_rsrp_high) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_lte_rsrp_high_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_lte_rsrp_high),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_rssi_threshold_low) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_rssi_threshold_low_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_rssi_threshold_low),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_rssi_threshold_high) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_rssi_threshold_high_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_rssi_threshold_high),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_sinr_threshold_low) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_sinr_threshold_low_valid)),
  0x15,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_sinr_threshold_low),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_sinr_threshold_high) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_sinr_threshold_high_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, in_call_wifi_sinr_threshold_high),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_lte_rsrp_low) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_lte_rsrp_low_valid)),
  0x17,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_lte_rsrp_low),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_lte_rsrp_mid) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_lte_rsrp_mid_valid)),
  0x18,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_lte_rsrp_mid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_lte_rsrp_high) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_lte_rsrp_high_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_lte_rsrp_high),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_rssi_threshold_low) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_rssi_threshold_low_valid)),
  0x1A,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_rssi_threshold_low),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_rssi_threshold_high) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_rssi_threshold_high_valid)),
  0x1B,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_rssi_threshold_high),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_sinr_threshold_low) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_sinr_threshold_low_valid)),
  0x1C,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_sinr_threshold_low),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_sinr_threshold_high) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_sinr_threshold_high_valid)),
  0x1D,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, idle_wifi_sinr_threshold_high),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, ecio_1x_threshold_low) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, ecio_1x_threshold_low_valid)),
  0x1E,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, ecio_1x_threshold_low),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, ecio_1x_threshold_high) - QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, ecio_1x_threshold_high_valid)),
  0x1F,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_req_msg_v01, ecio_1x_threshold_high)
};

static const uint8_t dsd_set_quality_measurement_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_quality_measurement_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_wqe_profile_init_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, wqe_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, wifi_rssi_threshold_low) - QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, wifi_rssi_threshold_low_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, wifi_rssi_threshold_low),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, wifi_rssi_threshold_high) - QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, wifi_rssi_threshold_high_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, wifi_rssi_threshold_high),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, iccid) - QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, iccid_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, iccid),
  QMI_DSD_MAX_ICCID_LEN_V01,
  QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, iccid) - QMI_IDL_OFFSET8(dsd_wqe_profile_init_ind_msg_v01, iccid_len)
};

static const uint8_t dsd_roaming_status_change_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_roaming_status_change_ind_msg_v01, technology),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_roaming_status_change_ind_msg_v01, roaming_status) - QMI_IDL_OFFSET8(dsd_roaming_status_change_ind_msg_v01, roaming_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_roaming_status_change_ind_msg_v01, roaming_status)
};

/*
 * dsd_get_current_roaming_status_info_req_msg is empty
 * static const uint8_t dsd_get_current_roaming_status_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_current_roaming_status_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_current_roaming_status_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_current_roaming_status_info_resp_msg_v01, roaming_status) - QMI_IDL_OFFSET8(dsd_get_current_roaming_status_info_resp_msg_v01, roaming_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dsd_get_current_roaming_status_info_resp_msg_v01, roaming_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_current_roaming_status_info_resp_msg_v01, technology) - QMI_IDL_OFFSET8(dsd_get_current_roaming_status_info_resp_msg_v01, technology_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_current_roaming_status_info_resp_msg_v01, technology)
};

static const uint8_t dsd_wwan_connection_state_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wwan_connection_state_ind_msg_v01, wwan_conn_state)
};

static const uint8_t dsd_set_wwan_activity_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_set_wwan_activity_req_msg_v01, activity_type)
};

static const uint8_t dsd_set_wwan_activity_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_wwan_activity_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * dsd_get_lte_cell_info_req_msg is empty
 * static const uint8_t dsd_get_lte_cell_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_lte_cell_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_lte_cell_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_lte_cell_info_resp_msg_v01, pcell_info) - QMI_IDL_OFFSET8(dsd_get_lte_cell_info_resp_msg_v01, pcell_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_lte_cell_info_resp_msg_v01, pcell_info),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_lte_cell_info_resp_msg_v01, scell_info) - QMI_IDL_OFFSET8(dsd_get_lte_cell_info_resp_msg_v01, scell_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_lte_cell_info_resp_msg_v01, scell_info),
  QMI_DSD_MAX_LTE_NUM_SCELL_V01,
  QMI_IDL_OFFSET8(dsd_get_lte_cell_info_resp_msg_v01, scell_info) - QMI_IDL_OFFSET8(dsd_get_lte_cell_info_resp_msg_v01, scell_info_len),
  QMI_IDL_TYPE88(0, 21)
};

static const uint8_t dsd_lte_cell_info_change_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_change_ind_msg_v01, pcell_info),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_change_ind_msg_v01, scell_info),
  QMI_DSD_MAX_LTE_NUM_SCELL_V01,
  QMI_IDL_OFFSET8(dsd_lte_cell_info_change_ind_msg_v01, scell_info) - QMI_IDL_OFFSET8(dsd_lte_cell_info_change_ind_msg_v01, scell_info_len),
  QMI_IDL_TYPE88(0, 21)
};

static const uint8_t dsd_set_3gpp_hplmn_list_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_3gpp_hplmn_list_req_msg_v01, hplmn_list),
  QMI_DSD_HPLMN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(dsd_set_3gpp_hplmn_list_req_msg_v01, hplmn_list) - QMI_IDL_OFFSET8(dsd_set_3gpp_hplmn_list_req_msg_v01, hplmn_list_len),
  QMI_IDL_TYPE88(0, 22),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_set_3gpp_hplmn_list_req_msg_v01, excp_hplmn_list) - QMI_IDL_OFFSET16RELATIVE(dsd_set_3gpp_hplmn_list_req_msg_v01, excp_hplmn_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_set_3gpp_hplmn_list_req_msg_v01, excp_hplmn_list),
  QMI_DSD_EXCP_HPLMN_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_set_3gpp_hplmn_list_req_msg_v01, excp_hplmn_list) - QMI_IDL_OFFSET16RELATIVE(dsd_set_3gpp_hplmn_list_req_msg_v01, excp_hplmn_list_len),
  QMI_IDL_TYPE88(0, 22)
};

static const uint8_t dsd_set_3gpp_hplmn_list_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_3gpp_hplmn_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * dsd_get_3gpp_hplmn_list_req_msg is empty
 * static const uint8_t dsd_get_3gpp_hplmn_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_3gpp_hplmn_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_3gpp_hplmn_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_3gpp_hplmn_list_resp_msg_v01, hplmn_list) - QMI_IDL_OFFSET8(dsd_get_3gpp_hplmn_list_resp_msg_v01, hplmn_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_3gpp_hplmn_list_resp_msg_v01, hplmn_list),
  QMI_DSD_HPLMN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(dsd_get_3gpp_hplmn_list_resp_msg_v01, hplmn_list) - QMI_IDL_OFFSET8(dsd_get_3gpp_hplmn_list_resp_msg_v01, hplmn_list_len),
  QMI_IDL_TYPE88(0, 22),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dsd_get_3gpp_hplmn_list_resp_msg_v01, excp_hplmn_list) - QMI_IDL_OFFSET16RELATIVE(dsd_get_3gpp_hplmn_list_resp_msg_v01, excp_hplmn_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(dsd_get_3gpp_hplmn_list_resp_msg_v01, excp_hplmn_list),
  QMI_DSD_EXCP_HPLMN_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(dsd_get_3gpp_hplmn_list_resp_msg_v01, excp_hplmn_list) - QMI_IDL_OFFSET16RELATIVE(dsd_get_3gpp_hplmn_list_resp_msg_v01, excp_hplmn_list_len),
  QMI_IDL_TYPE88(0, 22)
};

static const uint8_t dsd_switch_dds_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_switch_dds_req_msg_v01, subscription),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_switch_dds_req_msg_v01, dds_switch_type) - QMI_IDL_OFFSET8(dsd_switch_dds_req_msg_v01, dds_switch_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_switch_dds_req_msg_v01, dds_switch_type)
};

static const uint8_t dsd_switch_dds_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_switch_dds_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_switch_dds_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_switch_dds_ind_msg_v01, dds_switch_result)
};

/*
 * dsd_get_current_dds_req_msg is empty
 * static const uint8_t dsd_get_current_dds_req_msg_data_v01[] = {
 * };
 */

static const uint8_t dsd_get_current_dds_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_get_current_dds_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_current_dds_resp_msg_v01, dds) - QMI_IDL_OFFSET8(dsd_get_current_dds_resp_msg_v01, dds_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_current_dds_resp_msg_v01, dds),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_get_current_dds_resp_msg_v01, dds_switch_type) - QMI_IDL_OFFSET8(dsd_get_current_dds_resp_msg_v01, dds_switch_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_get_current_dds_resp_msg_v01, dds_switch_type)
};

static const uint8_t dsd_current_dds_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_current_dds_ind_msg_v01, dds),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_current_dds_ind_msg_v01, dds_switch_type) - QMI_IDL_OFFSET8(dsd_current_dds_ind_msg_v01, dds_switch_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_current_dds_ind_msg_v01, dds_switch_type)
};

static const uint8_t dsd_set_apn_type_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_apn_type_req_msg_v01, apn_type_info),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dsd_set_apn_type_req_msg_v01, apn_type_preference_mask) - QMI_IDL_OFFSET8(dsd_set_apn_type_req_msg_v01, apn_type_preference_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_set_apn_type_req_msg_v01, apn_type_preference_mask)
};

static const uint8_t dsd_set_apn_type_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_set_apn_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_dsd_configure_vowifi_service_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_dsd_configure_vowifi_service_req_msg_v01, fqdn) - QMI_IDL_OFFSET8(qmi_dsd_configure_vowifi_service_req_msg_v01, fqdn_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_dsd_configure_vowifi_service_req_msg_v01, fqdn),
  QMI_DSD_MAX_FQDN_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_dsd_configure_vowifi_service_req_msg_v01, vowifi_ipv4_addr) - QMI_IDL_OFFSET16RELATIVE(qmi_dsd_configure_vowifi_service_req_msg_v01, vowifi_ipv4_addr_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_dsd_configure_vowifi_service_req_msg_v01, vowifi_ipv4_addr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_dsd_configure_vowifi_service_req_msg_v01, vowifi_ipv6_addr) - QMI_IDL_OFFSET16RELATIVE(qmi_dsd_configure_vowifi_service_req_msg_v01, vowifi_ipv6_addr_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_dsd_configure_vowifi_service_req_msg_v01, vowifi_ipv6_addr),
  QMI_DSD_IPV6_ADDR_LEN_V01
};

static const uint8_t qmi_dsd_configure_vowifi_service_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_dsd_configure_vowifi_service_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_wifi_service_cap_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_service_cap_req_msg_v01, wifi_srv_cap_mask)
};

static const uint8_t dsd_wifi_service_cap_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_service_cap_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dsd_wifi_band_vacate_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_band_vacate_ind_msg_v01, wifi_band_vacate_request)
};

static const uint8_t dsd_wifi_band_vacate_result_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dsd_wifi_band_vacate_result_req_msg_v01, wifi_band_vacate_result)
};

static const uint8_t dsd_wifi_band_vacate_result_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dsd_wifi_band_vacate_result_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  dsd_type_table_v01[] = {
  {sizeof(dsd_system_status_info_type_v01), dsd_system_status_info_type_data_v01},
  {sizeof(dsd_apn_avail_sys_info_type_v01), dsd_apn_avail_sys_info_type_data_v01},
  {sizeof(dsd_apn_pref_sys_info_type_v01), dsd_apn_pref_sys_info_type_data_v01},
  {sizeof(dsd_global_pref_sys_info_type_v01), dsd_global_pref_sys_info_type_data_v01},
  {sizeof(dsd_ipv6_addr_type_v01), dsd_ipv6_addr_type_data_v01},
  {sizeof(dsd_apn_pref_sys_type_v01), dsd_apn_pref_sys_type_data_v01},
  {sizeof(dsd_apn_name_type_v01), dsd_apn_name_type_data_v01},
  {sizeof(dsd_apn_info_type_v01), dsd_apn_info_type_data_v01},
  {sizeof(dsd_wifi_ident_bssid_type_v01), dsd_wifi_ident_bssid_type_data_v01},
  {sizeof(dsd_wifi_ident_ssid_type_v01), dsd_wifi_ident_ssid_type_data_v01},
  {sizeof(dsd_wifi_ident_rssi_threshold_type_v01), dsd_wifi_ident_rssi_threshold_type_data_v01},
  {sizeof(dsd_wifi_ident_sinr_threshold_type_v01), dsd_wifi_ident_sinr_threshold_type_data_v01},
  {sizeof(dsd_wifi_ident_bss_load_threshold_type_v01), dsd_wifi_ident_bss_load_threshold_type_data_v01},
  {sizeof(dsd_wifi_blacklist_bssid_type_v01), dsd_wifi_blacklist_bssid_type_data_v01},
  {sizeof(dsd_wifi_bss_load_type_v01), dsd_wifi_bss_load_type_data_v01},
  {sizeof(dsd_wifi_phy_rate_type_v01), dsd_wifi_phy_rate_type_data_v01},
  {sizeof(dsd_wifi_packet_error_rate_type_v01), dsd_wifi_packet_error_rate_type_data_v01},
  {sizeof(dsd_wifi_meas_info_per_channel_type_v01), dsd_wifi_meas_info_per_channel_type_data_v01},
  {sizeof(dsd_wifi_meas_info_per_bssid_type_v01), dsd_wifi_meas_info_per_bssid_type_data_v01},
  {sizeof(dsd_wifi_meas_secondary_bssid_info_type_v01), dsd_wifi_meas_secondary_bssid_info_type_data_v01},
  {sizeof(dsd_lte_cell_info_type_v01), dsd_lte_cell_info_type_data_v01},
  {sizeof(dsd_lte_scell_info_type_v01), dsd_lte_scell_info_type_data_v01},
  {sizeof(dsd_3gpp_mcc_mnc_type_v01), dsd_3gpp_mcc_mnc_type_data_v01},
  {sizeof(dsd_apn_type_info_type_v01), dsd_apn_type_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry dsd_message_table_v01[] = {
  {sizeof(dsd_get_system_status_req_msg_v01), 0},
  {sizeof(dsd_get_system_status_resp_msg_v01), dsd_get_system_status_resp_msg_data_v01},
  {sizeof(dsd_system_status_change_req_msg_v01), dsd_system_status_change_req_msg_data_v01},
  {sizeof(dsd_system_status_change_resp_msg_v01), dsd_system_status_change_resp_msg_data_v01},
  {sizeof(dsd_system_status_ind_msg_v01), dsd_system_status_ind_msg_data_v01},
  {sizeof(dsd_bind_subscription_req_msg_v01), dsd_bind_subscription_req_msg_data_v01},
  {sizeof(dsd_bind_subscription_resp_msg_v01), dsd_bind_subscription_resp_msg_data_v01},
  {sizeof(dsd_get_bind_subscription_req_msg_v01), 0},
  {sizeof(dsd_get_bind_subscription_resp_msg_v01), dsd_get_bind_subscription_resp_msg_data_v01},
  {sizeof(dsd_wlan_available_req_msg_v01), dsd_wlan_available_req_msg_data_v01},
  {sizeof(dsd_wlan_available_resp_msg_v01), dsd_wlan_available_resp_msg_data_v01},
  {sizeof(dsd_wlan_not_available_req_msg_v01), dsd_wlan_not_available_req_msg_data_v01},
  {sizeof(dsd_wlan_not_available_resp_msg_v01), dsd_wlan_not_available_resp_msg_data_v01},
  {sizeof(dsd_set_wlan_preference_req_msg_v01), dsd_set_wlan_preference_req_msg_data_v01},
  {sizeof(dsd_set_wlan_preference_resp_msg_v01), dsd_set_wlan_preference_resp_msg_data_v01},
  {sizeof(dsd_get_wlan_preference_req_msg_v01), 0},
  {sizeof(dsd_get_wlan_preference_resp_msg_v01), dsd_get_wlan_preference_resp_msg_data_v01},
  {sizeof(dsd_set_apn_preferred_system_req_msg_v01), dsd_set_apn_preferred_system_req_msg_data_v01},
  {sizeof(dsd_set_apn_preferred_system_resp_msg_v01), dsd_set_apn_preferred_system_resp_msg_data_v01},
  {sizeof(dsd_get_modem_power_cost_req_msg_v01), 0},
  {sizeof(dsd_get_modem_power_cost_resp_msg_v01), dsd_get_modem_power_cost_resp_msg_data_v01},
  {sizeof(dsd_pdn_policy_start_txn_req_msg_v01), 0},
  {sizeof(dsd_pdn_policy_start_txn_resp_msg_v01), dsd_pdn_policy_start_txn_resp_msg_data_v01},
  {sizeof(dsd_add_pdn_policy_req_msg_v01), dsd_add_pdn_policy_req_msg_data_v01},
  {sizeof(dsd_add_pdn_policy_resp_msg_v01), dsd_add_pdn_policy_resp_msg_data_v01},
  {sizeof(dsd_modify_pdn_policy_req_msg_v01), dsd_modify_pdn_policy_req_msg_data_v01},
  {sizeof(dsd_modify_pdn_policy_resp_msg_v01), dsd_modify_pdn_policy_resp_msg_data_v01},
  {sizeof(dsd_delete_pdn_policy_by_apn_req_msg_v01), dsd_delete_pdn_policy_by_apn_req_msg_data_v01},
  {sizeof(dsd_delete_pdn_policy_by_apn_resp_msg_v01), dsd_delete_pdn_policy_by_apn_resp_msg_data_v01},
  {sizeof(dsd_get_pdn_policy_apn_list_req_msg_v01), dsd_get_pdn_policy_apn_list_req_msg_data_v01},
  {sizeof(dsd_get_pdn_policy_apn_list_resp_msg_v01), dsd_get_pdn_policy_apn_list_resp_msg_data_v01},
  {sizeof(dsd_get_pdn_policy_settings_for_apn_req_msg_v01), dsd_get_pdn_policy_settings_for_apn_req_msg_data_v01},
  {sizeof(dsd_get_pdn_policy_settings_for_apn_resp_msg_v01), dsd_get_pdn_policy_settings_for_apn_resp_msg_data_v01},
  {sizeof(dsd_pdn_policy_end_txn_req_msg_v01), dsd_pdn_policy_end_txn_req_msg_data_v01},
  {sizeof(dsd_pdn_policy_end_txn_resp_msg_v01), dsd_pdn_policy_end_txn_resp_msg_data_v01},
  {sizeof(dsd_set_apn_info_req_msg_v01), dsd_set_apn_info_req_msg_data_v01},
  {sizeof(dsd_set_apn_info_resp_msg_v01), dsd_set_apn_info_resp_msg_data_v01},
  {sizeof(dsd_get_apn_info_req_msg_v01), dsd_get_apn_info_req_msg_data_v01},
  {sizeof(dsd_get_apn_info_resp_msg_v01), dsd_get_apn_info_resp_msg_data_v01},
  {sizeof(dsd_notify_data_settings_req_msg_v01), dsd_notify_data_settings_req_msg_data_v01},
  {sizeof(dsd_notify_data_settings_resp_msg_v01), dsd_notify_data_settings_resp_msg_data_v01},
  {sizeof(dsd_get_data_settings_req_msg_v01), 0},
  {sizeof(dsd_get_data_settings_resp_msg_v01), dsd_get_data_settings_resp_msg_data_v01},
  {sizeof(dsd_thermal_info_change_ind_msg_v01), dsd_thermal_info_change_ind_msg_data_v01},
  {sizeof(dsd_get_thermal_mitigation_info_req_msg_v01), 0},
  {sizeof(dsd_get_thermal_mitigation_info_resp_msg_v01), dsd_get_thermal_mitigation_info_resp_msg_data_v01},
  {sizeof(dsd_indication_register_req_msg_v01), dsd_indication_register_req_msg_data_v01},
  {sizeof(dsd_indication_register_resp_msg_v01), dsd_indication_register_resp_msg_data_v01},
  {sizeof(dsd_start_wifi_meas_ind_msg_v01), dsd_start_wifi_meas_ind_msg_data_v01},
  {sizeof(dsd_stop_wifi_meas_ind_msg_v01), dsd_stop_wifi_meas_ind_msg_data_v01},
  {sizeof(dsd_wifi_meas_report_config_status_req_msg_v01), dsd_wifi_meas_report_config_status_req_msg_data_v01},
  {sizeof(dsd_wifi_meas_report_config_status_resp_msg_v01), dsd_wifi_meas_report_config_status_resp_msg_data_v01},
  {sizeof(dsd_wifi_meas_report_req_msg_v01), dsd_wifi_meas_report_req_msg_data_v01},
  {sizeof(dsd_wifi_meas_report_resp_msg_v01), dsd_wifi_meas_report_resp_msg_data_v01},
  {sizeof(dsd_set_apn_call_type_req_msg_v01), dsd_set_apn_call_type_req_msg_data_v01},
  {sizeof(dsd_set_apn_call_type_resp_msg_v01), dsd_set_apn_call_type_resp_msg_data_v01},
  {sizeof(dsd_set_wqe_profile_type_req_msg_v01), dsd_set_wqe_profile_type_req_msg_data_v01},
  {sizeof(dsd_set_wqe_profile_type_resp_msg_v01), dsd_set_wqe_profile_type_resp_msg_data_v01},
  {sizeof(dsd_wqe_profile_type_ind_msg_v01), dsd_wqe_profile_type_ind_msg_data_v01},
  {sizeof(dsd_wqe_profile_type_met_ind_msg_v01), dsd_wqe_profile_type_met_ind_msg_data_v01},
  {sizeof(dsd_nat_keep_alive_info_ind_msg_v01), dsd_nat_keep_alive_info_ind_msg_data_v01},
  {sizeof(dsd_nat_keep_alive_config_status_req_msg_v01), dsd_nat_keep_alive_config_status_req_msg_data_v01},
  {sizeof(dsd_nat_keep_alive_config_status_resp_msg_v01), dsd_nat_keep_alive_config_status_resp_msg_data_v01},
  {sizeof(dsd_set_wqe_profile_quality_status_req_msg_v01), dsd_set_wqe_profile_quality_status_req_msg_data_v01},
  {sizeof(dsd_set_wqe_profile_quality_status_resp_msg_v01), dsd_set_wqe_profile_quality_status_resp_msg_data_v01},
  {sizeof(dsd_set_quality_measurement_info_req_msg_v01), dsd_set_quality_measurement_info_req_msg_data_v01},
  {sizeof(dsd_set_quality_measurement_info_resp_msg_v01), dsd_set_quality_measurement_info_resp_msg_data_v01},
  {sizeof(dsd_wqe_profile_init_ind_msg_v01), dsd_wqe_profile_init_ind_msg_data_v01},
  {sizeof(dsd_roaming_status_change_ind_msg_v01), dsd_roaming_status_change_ind_msg_data_v01},
  {sizeof(dsd_get_current_roaming_status_info_req_msg_v01), 0},
  {sizeof(dsd_get_current_roaming_status_info_resp_msg_v01), dsd_get_current_roaming_status_info_resp_msg_data_v01},
  {sizeof(dsd_wwan_connection_state_ind_msg_v01), dsd_wwan_connection_state_ind_msg_data_v01},
  {sizeof(dsd_set_wwan_activity_req_msg_v01), dsd_set_wwan_activity_req_msg_data_v01},
  {sizeof(dsd_set_wwan_activity_resp_msg_v01), dsd_set_wwan_activity_resp_msg_data_v01},
  {sizeof(dsd_get_lte_cell_info_req_msg_v01), 0},
  {sizeof(dsd_get_lte_cell_info_resp_msg_v01), dsd_get_lte_cell_info_resp_msg_data_v01},
  {sizeof(dsd_lte_cell_info_change_ind_msg_v01), dsd_lte_cell_info_change_ind_msg_data_v01},
  {sizeof(dsd_set_3gpp_hplmn_list_req_msg_v01), dsd_set_3gpp_hplmn_list_req_msg_data_v01},
  {sizeof(dsd_set_3gpp_hplmn_list_resp_msg_v01), dsd_set_3gpp_hplmn_list_resp_msg_data_v01},
  {sizeof(dsd_get_3gpp_hplmn_list_req_msg_v01), 0},
  {sizeof(dsd_get_3gpp_hplmn_list_resp_msg_v01), dsd_get_3gpp_hplmn_list_resp_msg_data_v01},
  {sizeof(dsd_switch_dds_req_msg_v01), dsd_switch_dds_req_msg_data_v01},
  {sizeof(dsd_switch_dds_resp_msg_v01), dsd_switch_dds_resp_msg_data_v01},
  {sizeof(dsd_switch_dds_ind_msg_v01), dsd_switch_dds_ind_msg_data_v01},
  {sizeof(dsd_get_current_dds_req_msg_v01), 0},
  {sizeof(dsd_get_current_dds_resp_msg_v01), dsd_get_current_dds_resp_msg_data_v01},
  {sizeof(dsd_current_dds_ind_msg_v01), dsd_current_dds_ind_msg_data_v01},
  {sizeof(dsd_set_apn_type_req_msg_v01), dsd_set_apn_type_req_msg_data_v01},
  {sizeof(dsd_set_apn_type_resp_msg_v01), dsd_set_apn_type_resp_msg_data_v01},
  {sizeof(qmi_dsd_configure_vowifi_service_req_msg_v01), qmi_dsd_configure_vowifi_service_req_msg_data_v01},
  {sizeof(qmi_dsd_configure_vowifi_service_resp_msg_v01), qmi_dsd_configure_vowifi_service_resp_msg_data_v01},
  {sizeof(dsd_wifi_service_cap_req_msg_v01), dsd_wifi_service_cap_req_msg_data_v01},
  {sizeof(dsd_wifi_service_cap_resp_msg_v01), dsd_wifi_service_cap_resp_msg_data_v01},
  {sizeof(dsd_wifi_band_vacate_ind_msg_v01), dsd_wifi_band_vacate_ind_msg_data_v01},
  {sizeof(dsd_wifi_band_vacate_result_req_msg_v01), dsd_wifi_band_vacate_result_req_msg_data_v01},
  {sizeof(dsd_wifi_band_vacate_result_resp_msg_v01), dsd_wifi_band_vacate_result_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object dsd_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *dsd_qmi_idl_type_table_object_referenced_tables_v01[] =
{&dsd_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object dsd_qmi_idl_type_table_object_v01 = {
  sizeof(dsd_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(dsd_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  dsd_type_table_v01,
  dsd_message_table_v01,
  dsd_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry dsd_service_command_messages_v01[] = {
  {QMI_DSD_WLAN_AVAILABLE_REQ_V01, QMI_IDL_TYPE16(0, 9), 250},
  {QMI_DSD_WLAN_NOT_AVAILABLE_REQ_V01, QMI_IDL_TYPE16(0, 11), 7},
  {QMI_DSD_SET_WLAN_PREFERENCE_REQ_V01, QMI_IDL_TYPE16(0, 13), 7},
  {QMI_DSD_GET_WLAN_PREFERENCE_REQ_V01, QMI_IDL_TYPE16(0, 15), 0},
  {QMI_DSD_GET_SYSTEM_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01, QMI_IDL_TYPE16(0, 2), 12},
  {QMI_DSD_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_DSD_GET_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 7), 0},
  {QMI_DSD_SET_APN_PREFERRED_SYSTEM_REQ_V01, QMI_IDL_TYPE16(0, 17), 108},
  {QMI_DSD_GET_MODEM_POWER_COST_REQ_V01, QMI_IDL_TYPE16(0, 19), 0},
  {QMI_DSD_PDN_POLICY_START_TXN_REQ_V01, QMI_IDL_TYPE16(0, 21), 0},
  {QMI_DSD_ADD_PDN_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 23), 137},
  {QMI_DSD_MODIFY_PDN_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 25), 137},
  {QMI_DSD_DELETE_PDN_POLICY_BY_APN_REQ_V01, QMI_IDL_TYPE16(0, 27), 110},
  {QMI_DSD_GET_PDN_POLICY_APN_LIST_REQ_V01, QMI_IDL_TYPE16(0, 29), 7},
  {QMI_DSD_GET_PDN_POLICY_SETTINGS_FOR_APN_REQ_V01, QMI_IDL_TYPE16(0, 31), 110},
  {QMI_DSD_PDN_POLICY_END_TXN_REQ_V01, QMI_IDL_TYPE16(0, 33), 14},
  {QMI_DSD_SET_APN_INFO_REQ_V01, QMI_IDL_TYPE16(0, 35), 112},
  {QMI_DSD_GET_APN_INFO_REQ_V01, QMI_IDL_TYPE16(0, 37), 7},
  {QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01, QMI_IDL_TYPE16(0, 39), 19},
  {QMI_DSD_GET_DATA_SETTING_REQ_V01, QMI_IDL_TYPE16(0, 41), 0},
  {QMI_DSD_GET_THERMAL_MITIGATION_INFO_REQ_V01, QMI_IDL_TYPE16(0, 44), 0},
  {QMI_DSD_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 46), 40},
  {QMI_DSD_WIFI_MEAS_REPORT_CONFIG_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 50), 14},
  {QMI_DSD_WIFI_MEAS_REPORT_REQ_V01, QMI_IDL_TYPE16(0, 52), 2469},
  {QMI_DSD_SET_APN_CALL_TYPE_REQ_V01, QMI_IDL_TYPE16(0, 54), 114},
  {QMI_DSD_SET_WQE_PROFILE_TYPE_REQ_V01, QMI_IDL_TYPE16(0, 56), 11},
  {QMI_DSD_NAT_KEEP_ALIVE_CONFIG_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 61), 7},
  {QMI_DSD_SET_WQE_PROFILE_QUALITY_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 63), 21},
  {QMI_DSD_SET_QUALITY_MEASUREMENT_INFO_REQ_V01, QMI_IDL_TYPE16(0, 65), 80},
  {QMI_DSD_GET_CURRENT_ROAMING_STATUS_INFO_REQ_V01, QMI_IDL_TYPE16(0, 69), 0},
  {QMI_DSD_SET_WWAN_ACTIVITY_REQ_V01, QMI_IDL_TYPE16(0, 72), 7},
  {QMI_DSD_GET_LTE_CELL_INFO_REQ_V01, QMI_IDL_TYPE16(0, 74), 0},
  {QMI_DSD_SET_3GPP_HPLMN_LIST_REQ_V01, QMI_IDL_TYPE16(0, 77), 520},
  {QMI_DSD_GET_3GPP_HPLMN_LIST_REQ_V01, QMI_IDL_TYPE16(0, 79), 0},
  {QMI_DSD_SWITCH_DDS_REQ_V01, QMI_IDL_TYPE16(0, 81), 14},
  {QMI_DSD_GET_CURRENT_DDS_REQ_V01, QMI_IDL_TYPE16(0, 84), 0},
  {QMI_DSD_SET_APN_TYPE_REQ_V01, QMI_IDL_TYPE16(0, 87), 123},
  {QMI_DSD_CONFIGURE_VOWIFI_SERVICE_REQ_V01, QMI_IDL_TYPE16(0, 89), 229},
  {QMI_DSD_WIFI_SERVICE_CAP_REQ_V01, QMI_IDL_TYPE16(0, 91), 11},
  {QMI_DSD_WIFI_BAND_VACATE_RESULT_REQ_V01, QMI_IDL_TYPE16(0, 94), 7}
};

static const qmi_idl_service_message_table_entry dsd_service_response_messages_v01[] = {
  {QMI_DSD_WLAN_AVAILABLE_RESP_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_DSD_WLAN_NOT_AVAILABLE_RESP_V01, QMI_IDL_TYPE16(0, 12), 7},
  {QMI_DSD_SET_WLAN_PREFERENCE_RESP_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_DSD_GET_WLAN_PREFERENCE_RESP_V01, QMI_IDL_TYPE16(0, 16), 14},
  {QMI_DSD_GET_SYSTEM_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 1), 7419},
  {QMI_DSD_SYSTEM_STATUS_CHANGE_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_DSD_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 6), 7},
  {QMI_DSD_GET_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 8), 14},
  {QMI_DSD_SET_APN_PREFERRED_SYSTEM_RESP_V01, QMI_IDL_TYPE16(0, 18), 7},
  {QMI_DSD_GET_MODEM_POWER_COST_RESP_V01, QMI_IDL_TYPE16(0, 20), 14},
  {QMI_DSD_PDN_POLICY_START_TXN_RESP_V01, QMI_IDL_TYPE16(0, 22), 14},
  {QMI_DSD_ADD_PDN_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 24), 7},
  {QMI_DSD_MODIFY_PDN_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_DSD_DELETE_PDN_POLICY_BY_APN_RESP_V01, QMI_IDL_TYPE16(0, 28), 7},
  {QMI_DSD_GET_PDN_POLICY_APN_LIST_RESP_V01, QMI_IDL_TYPE16(0, 30), 1526},
  {QMI_DSD_GET_PDN_POLICY_SETTINGS_FOR_APN_RESP_V01, QMI_IDL_TYPE16(0, 32), 137},
  {QMI_DSD_PDN_POLICY_END_TXN_RESP_V01, QMI_IDL_TYPE16(0, 34), 7},
  {QMI_DSD_SET_APN_INFO_RESP_V01, QMI_IDL_TYPE16(0, 36), 7},
  {QMI_DSD_GET_APN_INFO_RESP_V01, QMI_IDL_TYPE16(0, 38), 110},
  {QMI_DSD_NOTIFY_DATA_SETTING_RESP_V01, QMI_IDL_TYPE16(0, 40), 7},
  {QMI_DSD_GET_DATA_SETTING_RESP_V01, QMI_IDL_TYPE16(0, 42), 26},
  {QMI_DSD_GET_THERMAL_MITIGATION_INFO_RESP_V01, QMI_IDL_TYPE16(0, 45), 14},
  {QMI_DSD_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 47), 7},
  {QMI_DSD_WIFI_MEAS_REPORT_CONFIG_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 51), 7},
  {QMI_DSD_WIFI_MEAS_REPORT_RESP_V01, QMI_IDL_TYPE16(0, 53), 7},
  {QMI_DSD_SET_APN_CALL_TYPE_RESP_V01, QMI_IDL_TYPE16(0, 55), 7},
  {QMI_DSD_SET_WQE_PROFILE_TYPE_RESP_V01, QMI_IDL_TYPE16(0, 57), 7},
  {QMI_DSD_NAT_KEEP_ALIVE_CONFIG_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 62), 7},
  {QMI_DSD_SET_WQE_PROFILE_QUALITY_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 64), 7},
  {QMI_DSD_SET_QUALITY_MEASUREMENT_INFO_RESP_V01, QMI_IDL_TYPE16(0, 66), 7},
  {QMI_DSD_GET_CURRENT_ROAMING_STATUS_INFO_RESP_V01, QMI_IDL_TYPE16(0, 70), 18},
  {QMI_DSD_SET_WWAN_ACTIVITY_RESP_V01, QMI_IDL_TYPE16(0, 73), 7},
  {QMI_DSD_GET_LTE_CELL_INFO_RESP_V01, QMI_IDL_TYPE16(0, 75), 229},
  {QMI_DSD_SET_3GPP_HPLMN_LIST_RESP_V01, QMI_IDL_TYPE16(0, 78), 7},
  {QMI_DSD_GET_3GPP_HPLMN_LIST_RESP_V01, QMI_IDL_TYPE16(0, 80), 527},
  {QMI_DSD_SWITCH_DDS_RESP_V01, QMI_IDL_TYPE16(0, 82), 7},
  {QMI_DSD_GET_CURRENT_DDS_RESP_V01, QMI_IDL_TYPE16(0, 85), 21},
  {QMI_DSD_SET_APN_TYPE_RESP_V01, QMI_IDL_TYPE16(0, 88), 7},
  {QMI_DSD_CONFIGURE_VOWIFI_SERVICE_RESP_V01, QMI_IDL_TYPE16(0, 90), 7},
  {QMI_DSD_WIFI_SERVICE_CAP_RESP_V01, QMI_IDL_TYPE16(0, 92), 7},
  {QMI_DSD_WIFI_BAND_VACATE_RESULT_RESP_V01, QMI_IDL_TYPE16(0, 95), 7}
};

static const qmi_idl_service_message_table_entry dsd_service_indication_messages_v01[] = {
  {QMI_DSD_SYSTEM_STATUS_IND_V01, QMI_IDL_TYPE16(0, 4), 7412},
  {QMI_DSD_THERMAL_INFO_CHANGE_IND_V01, QMI_IDL_TYPE16(0, 43), 7},
  {QMI_DSD_START_WIFI_MEAS_IND_V01, QMI_IDL_TYPE16(0, 48), 5469},
  {QMI_DSD_STOP_WIFI_MEAS_IND_V01, QMI_IDL_TYPE16(0, 49), 7},
  {QMI_DSD_WQE_PROFILE_TYPE_IND_V01, QMI_IDL_TYPE16(0, 58), 25},
  {QMI_DSD_WQE_PROFILE_TYPE_MET_IND_V01, QMI_IDL_TYPE16(0, 59), 11},
  {QMI_DSD_NAT_KEEP_ALIVE_INFO_IND_V01, QMI_IDL_TYPE16(0, 60), 47},
  {QMI_DSD_WQE_PROFILE_INIT_IND_V01, QMI_IDL_TYPE16(0, 67), 31},
  {QMI_DSD_ROAMING_STATUS_CHANGE_IND_V01, QMI_IDL_TYPE16(0, 68), 11},
  {QMI_DSD_WWAN_CONNECTION_STATE_IND_V01, QMI_IDL_TYPE16(0, 71), 7},
  {QMI_DSD_LTE_CELL_INFO_CHANGE_IND_V01, QMI_IDL_TYPE16(0, 76), 222},
  {QMI_DSD_SWITCH_DDS_IND_V01, QMI_IDL_TYPE16(0, 83), 7},
  {QMI_DSD_CURRENT_DDS_IND_V01, QMI_IDL_TYPE16(0, 86), 14},
  {QMI_DSD_WIFI_BAND_VACATE_IND_V01, QMI_IDL_TYPE16(0, 93), 7}
};

/*Service Object*/
struct qmi_idl_service_object dsd_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x2A,
  7419,
  { sizeof(dsd_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(dsd_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(dsd_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { dsd_service_command_messages_v01, dsd_service_response_messages_v01, dsd_service_indication_messages_v01},
  &dsd_qmi_idl_type_table_object_v01,
  0x24,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type dsd_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( DSD_V01_IDL_MAJOR_VERS != idl_maj_version || DSD_V01_IDL_MINOR_VERS != idl_min_version
       || DSD_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&dsd_qmi_idl_service_object_v01;
}

