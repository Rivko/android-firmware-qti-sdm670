/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        W L A N _ L O C A T I O N _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the wls service Data structures.

  Copyright (c) 2014-2018 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/wls/src/wlan_location_service_v01.c#8 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Wed Mar 21 2018 (Spin 0)
   From IDL File: wlan_location_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "wlan_location_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t wls_freq_list_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wls_freq_list_t_v01, country_code),
  WLS_MAX_COUNTRY_CODE_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(wls_freq_list_t_v01, freq_list),
  WLS_MAX_FREQ_LIST_LEN_V01,
  QMI_IDL_OFFSET8(wls_freq_list_t_v01, freq_list) - QMI_IDL_OFFSET8(wls_freq_list_t_v01, freq_list_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wls_ssid_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ssid_t_v01, ssid),
  WLS_MAX_SSID_LEN_V01,
  QMI_IDL_OFFSET8(wls_ssid_t_v01, ssid) - QMI_IDL_OFFSET8(wls_ssid_t_v01, ssid_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wls_ap_base_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_base_t_v01, bssid),
  WLS_MAX_BSSID_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_base_t_v01, ssid),
  WLS_MAX_SSID_LEN_V01,
  QMI_IDL_OFFSET8(wls_ap_base_t_v01, ssid) - QMI_IDL_OFFSET8(wls_ap_base_t_v01, ssid_len),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_base_t_v01, frequency),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_base_t_v01, rssi),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wls_ap_t_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, base),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, cur_connected),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, valid_mask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, tx_power),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, tx_rate),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, tsf),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, age),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, wifi_802_11_mc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, msap_supported),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, msap),
  12,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, is_active),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, dwell_time_ms),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, band_center_freq_1),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, band_center_freq_2),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, phy_mode),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, tsf_offset),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, scan_mod_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, scan_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, flags),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_t_v01, scan_cache),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wls_ap_chre_t_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(wls_ap_chre_t_v01, ess),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_chre_t_v01, sec_mode),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_chre_t_v01, tx_nss),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_chre_t_v01, tx_mcs),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_chre_t_v01, rx_nss),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_ap_chre_t_v01, rx_mcs),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wls_rtt_oem_channel_info_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_oem_channel_info_s_v01, chan_number),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_oem_channel_info_s_v01, mhz),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_oem_channel_info_s_v01, band_center_freq1),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_oem_channel_info_s_v01, band_center_freq2),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_oem_channel_info_s_v01, info),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_oem_channel_info_s_v01, reg_info_1),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_oem_channel_info_s_v01, reg_info_2),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wls_ap_chain_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_chain_info_t_v01, radio_chain_mask),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ap_chain_info_t_v01, rssi_chain),
  WLS_MAX_RADIO_CHAIN_V01,

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * wls_ver_req_msg is empty
 * static const uint8_t wls_ver_req_msg_data_v01[] = {
 * };
 */

static const uint8_t wls_ver_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wls_version) - QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wls_version_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wls_version),
  WLS_MAX_VER_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_chip_oem) - QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_chip_oem_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_chip_oem),
  WLS_MAX_CHIP_VER_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_chip_name) - QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_chip_name_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_chip_name),
  WLS_MAX_CHIP_VER_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_chip_id) - QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_chip_id_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_chip_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_host_ver) - QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_host_ver_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_host_ver),
  WLS_MAX_VER_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_fw_ver) - QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_fw_ver_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, wlan_fw_ver),
  WLS_MAX_VER_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, err_id) - QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, err_id_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_ver_resp_msg_v01, err_id)
};

static const uint8_t wls_status_query_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_status_query_req_msg_v01, req_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_status_query_req_msg_v01, wlan_intf_type) - QMI_IDL_OFFSET8(wls_status_query_req_msg_v01, wlan_intf_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_status_query_req_msg_v01, wlan_intf_type)
};

static const uint8_t wls_status_query_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_status_query_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_status_query_resp_msg_v01, req_id) - QMI_IDL_OFFSET8(wls_status_query_resp_msg_v01, req_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_status_query_resp_msg_v01, req_id)
};

static const uint8_t wls_disc_scan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, req_id),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, is_active),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, dwell_time_ms),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, time_out_sec),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, freq_list) - QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, freq_list_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, freq_list),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, ssid_list) - QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, ssid_list_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, ssid_list),
  WLS_MAX_SSID_NUM_V01,
  QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, ssid_list) - QMI_IDL_OFFSET8(wls_disc_scan_req_msg_v01, ssid_list_len),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_disc_scan_req_msg_v01, use_cached) - QMI_IDL_OFFSET16RELATIVE(wls_disc_scan_req_msg_v01, use_cached_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wls_disc_scan_req_msg_v01, use_cached),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_disc_scan_req_msg_v01, scan_type) - QMI_IDL_OFFSET16RELATIVE(wls_disc_scan_req_msg_v01, scan_type_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(wls_disc_scan_req_msg_v01, scan_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_disc_scan_req_msg_v01, dwell_mode) - QMI_IDL_OFFSET16RELATIVE(wls_disc_scan_req_msg_v01, dwell_mode_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(wls_disc_scan_req_msg_v01, dwell_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_disc_scan_req_msg_v01, radio_chain_pref) - QMI_IDL_OFFSET16RELATIVE(wls_disc_scan_req_msg_v01, radio_chain_pref_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(wls_disc_scan_req_msg_v01, radio_chain_pref)
};

static const uint8_t wls_disc_scan_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_disc_scan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_disc_scan_resp_msg_v01, req_id) - QMI_IDL_OFFSET8(wls_disc_scan_resp_msg_v01, req_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_disc_scan_resp_msg_v01, req_id)
};

static const uint8_t wls_sub_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_sub_req_msg_v01, req_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_sub_req_msg_v01, is_enable) - QMI_IDL_OFFSET8(wls_sub_req_msg_v01, is_enable_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_sub_req_msg_v01, is_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_sub_req_msg_v01, sub_type) - QMI_IDL_OFFSET8(wls_sub_req_msg_v01, sub_type_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wls_sub_req_msg_v01, sub_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_sub_req_msg_v01, wlan_intf_type) - QMI_IDL_OFFSET8(wls_sub_req_msg_v01, wlan_intf_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_sub_req_msg_v01, wlan_intf_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_sub_req_msg_v01, snoop_mode) - QMI_IDL_OFFSET8(wls_sub_req_msg_v01, snoop_mode_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_sub_req_msg_v01, snoop_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_sub_req_msg_v01, motion_detection_enable) - QMI_IDL_OFFSET8(wls_sub_req_msg_v01, motion_detection_enable_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_sub_req_msg_v01, motion_detection_enable)
};

static const uint8_t wls_sub_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_sub_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_sub_resp_msg_v01, err_id) - QMI_IDL_OFFSET8(wls_sub_resp_msg_v01, err_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_sub_resp_msg_v01, err_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_sub_resp_msg_v01, req_id) - QMI_IDL_OFFSET8(wls_sub_resp_msg_v01, req_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_sub_resp_msg_v01, req_id)
};

static const uint8_t wls_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, event_type) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, event_type_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, event_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, req_id) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, req_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, req_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, host_is_on) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, host_is_on_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, host_is_on),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, fw_svc_is_on) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, fw_svc_is_on_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, fw_svc_is_on),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, is_connected) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, is_connected_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, is_connected),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, handoff_happened) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, handoff_happened_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, handoff_happened),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, intf_id) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, intf_id_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, intf_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, intf_mode) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, intf_mode_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, intf_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, connected_ap_info) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, connected_ap_info_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, connected_ap_info),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, handoff_ap_info) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, handoff_ap_info_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, handoff_ap_info),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_event_ind_msg_v01, supported_scan_freq_list) - QMI_IDL_OFFSET8(wls_event_ind_msg_v01, supported_scan_freq_list_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_event_ind_msg_v01, supported_scan_freq_list),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_event_ind_msg_v01, rtt_oem_channel_list) - QMI_IDL_OFFSET16RELATIVE(wls_event_ind_msg_v01, rtt_oem_channel_list_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(wls_event_ind_msg_v01, rtt_oem_channel_list),
  WLS_MAX_NUM_RTT_CHAN_V01,
  QMI_IDL_OFFSET16RELATIVE(wls_event_ind_msg_v01, rtt_oem_channel_list) - QMI_IDL_OFFSET16RELATIVE(wls_event_ind_msg_v01, rtt_oem_channel_list_len),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t wls_scan_result_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, req_id) - QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, req_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, req_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, scanned_freq_list) - QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, scanned_freq_list_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, scanned_freq_list),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, ap_array) - QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, ap_array_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, ap_array),
  WLS_MAX_AP_NUM_V01,
  QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, ap_array) - QMI_IDL_OFFSET8(wls_scan_result_ind_msg_v01, ap_array_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, scan_is_completed) - QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, scan_is_completed_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wls_scan_result_ind_msg_v01, scan_is_completed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, extscan_is_completed) - QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, extscan_is_completed_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wls_scan_result_ind_msg_v01, extscan_is_completed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, scan_type_performed) - QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, scan_type_performed_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(wls_scan_result_ind_msg_v01, scan_type_performed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, ap_array_chre) - QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, ap_array_chre_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(wls_scan_result_ind_msg_v01, ap_array_chre),
  WLS_MAX_AP_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, ap_array_chre) - QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, ap_array_chre_len),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, ap_array_chain) - QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, ap_array_chain_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(wls_scan_result_ind_msg_v01, ap_array_chain),
  WLS_MAX_AP_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, ap_array_chain) - QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, ap_array_chain_len),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, radio_chain_pref) - QMI_IDL_OFFSET16RELATIVE(wls_scan_result_ind_msg_v01, radio_chain_pref_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(wls_scan_result_ind_msg_v01, radio_chain_pref)
};

static const uint8_t wls_err_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_err_ind_msg_v01, err_id) - QMI_IDL_OFFSET8(wls_err_ind_msg_v01, err_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_err_ind_msg_v01, err_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_err_ind_msg_v01, err_info) - QMI_IDL_OFFSET8(wls_err_ind_msg_v01, err_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wls_err_ind_msg_v01, err_info),
  WLS_MAX_ERR_INFO_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_err_ind_msg_v01, req_id) - QMI_IDL_OFFSET8(wls_err_ind_msg_v01, req_id_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_err_ind_msg_v01, req_id)
};

static const uint8_t wls_rtt_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_rtt_req_msg_v01, rtt_req_blob) - QMI_IDL_OFFSET8(wls_rtt_req_msg_v01, rtt_req_blob_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_req_msg_v01, rtt_req_blob),
  ((WLS_MAX_RTT_REQ_BLOB_LEN_V01) & 0xFF), ((WLS_MAX_RTT_REQ_BLOB_LEN_V01) >> 8),
  QMI_IDL_OFFSET8(wls_rtt_req_msg_v01, rtt_req_blob) - QMI_IDL_OFFSET8(wls_rtt_req_msg_v01, rtt_req_blob_len)
};

static const uint8_t wls_rtt_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wls_rtt_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_rtt_resp_msg_v01, err_id) - QMI_IDL_OFFSET8(wls_rtt_resp_msg_v01, err_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_resp_msg_v01, err_id)
};

static const uint8_t wls_rtt_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wls_rtt_ind_msg_v01, rtt_ind_blob) - QMI_IDL_OFFSET8(wls_rtt_ind_msg_v01, rtt_ind_blob_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wls_rtt_ind_msg_v01, rtt_ind_blob),
  ((WLS_MAX_RTT_IND_BLOB_LEN_V01) & 0xFF), ((WLS_MAX_RTT_IND_BLOB_LEN_V01) >> 8),
  QMI_IDL_OFFSET8(wls_rtt_ind_msg_v01, rtt_ind_blob) - QMI_IDL_OFFSET8(wls_rtt_ind_msg_v01, rtt_ind_blob_len)
};

/* Type Table */
static const qmi_idl_type_table_entry  wls_type_table_v01[] = {
  {sizeof(wls_freq_list_t_v01), wls_freq_list_t_data_v01},
  {sizeof(wls_ssid_t_v01), wls_ssid_t_data_v01},
  {sizeof(wls_ap_base_t_v01), wls_ap_base_t_data_v01},
  {sizeof(wls_ap_t_v01), wls_ap_t_data_v01},
  {sizeof(wls_ap_chre_t_v01), wls_ap_chre_t_data_v01},
  {sizeof(wls_rtt_oem_channel_info_s_v01), wls_rtt_oem_channel_info_s_data_v01},
  {sizeof(wls_ap_chain_info_t_v01), wls_ap_chain_info_t_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry wls_message_table_v01[] = {
  {sizeof(wls_ver_req_msg_v01), 0},
  {sizeof(wls_ver_resp_msg_v01), wls_ver_resp_msg_data_v01},
  {sizeof(wls_status_query_req_msg_v01), wls_status_query_req_msg_data_v01},
  {sizeof(wls_status_query_resp_msg_v01), wls_status_query_resp_msg_data_v01},
  {sizeof(wls_disc_scan_req_msg_v01), wls_disc_scan_req_msg_data_v01},
  {sizeof(wls_disc_scan_resp_msg_v01), wls_disc_scan_resp_msg_data_v01},
  {sizeof(wls_sub_req_msg_v01), wls_sub_req_msg_data_v01},
  {sizeof(wls_sub_resp_msg_v01), wls_sub_resp_msg_data_v01},
  {sizeof(wls_event_ind_msg_v01), wls_event_ind_msg_data_v01},
  {sizeof(wls_scan_result_ind_msg_v01), wls_scan_result_ind_msg_data_v01},
  {sizeof(wls_err_ind_msg_v01), wls_err_ind_msg_data_v01},
  {sizeof(wls_rtt_req_msg_v01), wls_rtt_req_msg_data_v01},
  {sizeof(wls_rtt_resp_msg_v01), wls_rtt_resp_msg_data_v01},
  {sizeof(wls_rtt_ind_msg_v01), wls_rtt_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object wls_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *wls_qmi_idl_type_table_object_referenced_tables_v01[] =
{&wls_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object wls_qmi_idl_type_table_object_v01 = {
  sizeof(wls_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(wls_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  wls_type_table_v01,
  wls_message_table_v01,
  wls_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry wls_service_command_messages_v01[] = {
  {QMI_WLS_VER_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_WLS_STS_QUERY_REQ_V01, QMI_IDL_TYPE16(0, 2), 14},
  {QMI_WLS_SCAN_REQ_V01, QMI_IDL_TYPE16(0, 4), 706},
  {QMI_WLS_SUB_REQ_V01, QMI_IDL_TYPE16(0, 6), 40},
  {QMI_WLS_RTT_REQ_V01, QMI_IDL_TYPE16(0, 11), 4101}
};

static const qmi_idl_service_message_table_entry wls_service_response_messages_v01[] = {
  {QMI_WLS_VER_RESP_V01, QMI_IDL_TYPE16(0, 1), 191},
  {QMI_WLS_STS_QUERY_RESP_V01, QMI_IDL_TYPE16(0, 3), 14},
  {QMI_WLS_SCAN_RESP_V01, QMI_IDL_TYPE16(0, 5), 14},
  {QMI_WLS_SUB_RESP_V01, QMI_IDL_TYPE16(0, 7), 21},
  {QMI_WLS_RTT_RESP_V01, QMI_IDL_TYPE16(0, 12), 14}
};

static const qmi_idl_service_message_table_entry wls_service_indication_messages_v01[] = {
  {QMI_WLS_STS_IND_V01, QMI_IDL_TYPE16(0, 8), 5764},
  {QMI_WLS_EVENT_IND_V01, QMI_IDL_TYPE16(0, 8), 5764},
  {QMI_WLS_SCAN_RESULT_IND_V01, QMI_IDL_TYPE16(0, 9), 11115},
  {QMI_WLS_ERR_IND_V01, QMI_IDL_TYPE16(0, 10), 48},
  {QMI_WLS_RTT_IND_V01, QMI_IDL_TYPE16(0, 13), 4101}
};

/*Service Object*/
struct qmi_idl_service_object wls_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x003A,
  11115,
  { sizeof(wls_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(wls_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(wls_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { wls_service_command_messages_v01, wls_service_response_messages_v01, wls_service_indication_messages_v01},
  &wls_qmi_idl_type_table_object_v01,
  0x07,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type wls_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( WLS_V01_IDL_MAJOR_VERS != idl_maj_version || WLS_V01_IDL_MINOR_VERS != idl_min_version
       || WLS_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&wls_qmi_idl_service_object_v01;
}

