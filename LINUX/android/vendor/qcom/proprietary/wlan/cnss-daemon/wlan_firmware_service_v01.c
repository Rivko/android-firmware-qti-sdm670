/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        W L A N _ F I R M W A R E _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the wlfw service Data structures.

  Copyright (c) 2015-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Wed Mar  6 2019 (Spin 0)
   From IDL File: wlan_firmware_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "wlan_firmware_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t wlfw_ce_tgt_pipe_cfg_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ce_tgt_pipe_cfg_s_v01, pipe_num),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ce_tgt_pipe_cfg_s_v01, pipe_dir),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ce_tgt_pipe_cfg_s_v01, nentries),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ce_tgt_pipe_cfg_s_v01, nbytes_max),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ce_tgt_pipe_cfg_s_v01, flags),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_ce_svc_pipe_cfg_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ce_svc_pipe_cfg_s_v01, service_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ce_svc_pipe_cfg_s_v01, pipe_dir),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ce_svc_pipe_cfg_s_v01, pipe_num),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_shadow_reg_cfg_s_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(wlfw_shadow_reg_cfg_s_v01, id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(wlfw_shadow_reg_cfg_s_v01, offset),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_shadow_reg_v2_cfg_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_shadow_reg_v2_cfg_s_v01, addr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_rri_over_ddr_cfg_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_rri_over_ddr_cfg_s_v01, base_addr_low),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_rri_over_ddr_cfg_s_v01, base_addr_high),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_msi_cfg_s_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(wlfw_msi_cfg_s_v01, ce_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(wlfw_msi_cfg_s_v01, msi_vector),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_memory_region_info_s_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_memory_region_info_s_v01, region_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_memory_region_info_s_v01, size),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_memory_region_info_s_v01, secure_flag),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_mem_cfg_s_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_mem_cfg_s_v01, offset),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_mem_cfg_s_v01, size),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_mem_cfg_s_v01, secure_flag),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_mem_seg_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_mem_seg_s_v01, size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_mem_seg_s_v01, type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_mem_seg_s_v01, mem_cfg),
  QMI_WLFW_MAX_NUM_MEM_CFG_V01,
  QMI_IDL_OFFSET8(wlfw_mem_seg_s_v01, mem_cfg) - QMI_IDL_OFFSET8(wlfw_mem_seg_s_v01, mem_cfg_len),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_mem_seg_resp_s_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_mem_seg_resp_s_v01, addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_mem_seg_resp_s_v01, size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_mem_seg_resp_s_v01, type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_mem_seg_resp_s_v01, restore),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_rf_chip_info_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_rf_chip_info_s_v01, chip_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_rf_chip_info_s_v01, chip_family),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_rf_board_info_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_rf_board_info_s_v01, board_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_soc_info_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_soc_info_s_v01, soc_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t wlfw_fw_version_info_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_fw_version_info_s_v01, fw_version),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wlfw_fw_version_info_s_v01, fw_build_timestamp),
  QMI_WLFW_MAX_TIMESTAMP_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t wlfw_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, fw_ready_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, fw_ready_enable_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, fw_ready_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, initiate_cal_download_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, initiate_cal_download_enable_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, initiate_cal_download_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, initiate_cal_update_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, initiate_cal_update_enable_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, initiate_cal_update_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, msa_ready_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, msa_ready_enable_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, msa_ready_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, pin_connect_result_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, pin_connect_result_enable_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, pin_connect_result_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, client_id) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, client_id_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, client_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, request_mem_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, request_mem_enable_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, request_mem_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, fw_mem_ready_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, fw_mem_ready_enable_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, fw_mem_ready_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, fw_init_done_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, fw_init_done_enable_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, fw_init_done_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, rejuvenate_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, rejuvenate_enable_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, rejuvenate_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, xo_cal_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, xo_cal_enable_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, xo_cal_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, cal_done_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, cal_done_enable_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, cal_done_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, qdss_trace_req_mem_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, qdss_trace_req_mem_enable_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, qdss_trace_req_mem_enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, qdss_trace_save_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, qdss_trace_save_enable_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, qdss_trace_save_enable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, qdss_trace_free_enable) - QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, qdss_trace_free_enable_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_req_msg_v01, qdss_trace_free_enable)
};

static const uint8_t wlfw_ind_register_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ind_register_resp_msg_v01, fw_status) - QMI_IDL_OFFSET8(wlfw_ind_register_resp_msg_v01, fw_status_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_ind_register_resp_msg_v01, fw_status)
};

/*
 * wlfw_fw_ready_ind_msg is empty
 * static const uint8_t wlfw_fw_ready_ind_msg_data_v01[] = {
 * };
 */

/*
 * wlfw_msa_ready_ind_msg is empty
 * static const uint8_t wlfw_msa_ready_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t wlfw_pin_connect_result_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_pin_connect_result_ind_msg_v01, pwr_pin_result) - QMI_IDL_OFFSET8(wlfw_pin_connect_result_ind_msg_v01, pwr_pin_result_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_pin_connect_result_ind_msg_v01, pwr_pin_result),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_pin_connect_result_ind_msg_v01, phy_io_pin_result) - QMI_IDL_OFFSET8(wlfw_pin_connect_result_ind_msg_v01, phy_io_pin_result_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_pin_connect_result_ind_msg_v01, phy_io_pin_result),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_pin_connect_result_ind_msg_v01, rf_pin_result) - QMI_IDL_OFFSET8(wlfw_pin_connect_result_ind_msg_v01, rf_pin_result_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_pin_connect_result_ind_msg_v01, rf_pin_result)
};

static const uint8_t wlfw_wlan_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_wlan_mode_req_msg_v01, mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_wlan_mode_req_msg_v01, hw_debug) - QMI_IDL_OFFSET8(wlfw_wlan_mode_req_msg_v01, hw_debug_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_wlan_mode_req_msg_v01, hw_debug)
};

static const uint8_t wlfw_wlan_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_wlan_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_wlan_cfg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_wlan_cfg_req_msg_v01, host_version) - QMI_IDL_OFFSET8(wlfw_wlan_cfg_req_msg_v01, host_version_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wlfw_wlan_cfg_req_msg_v01, host_version),
  QMI_WLFW_MAX_STR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_wlan_cfg_req_msg_v01, tgt_cfg) - QMI_IDL_OFFSET8(wlfw_wlan_cfg_req_msg_v01, tgt_cfg_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_wlan_cfg_req_msg_v01, tgt_cfg),
  QMI_WLFW_MAX_NUM_CE_V01,
  QMI_IDL_OFFSET8(wlfw_wlan_cfg_req_msg_v01, tgt_cfg) - QMI_IDL_OFFSET8(wlfw_wlan_cfg_req_msg_v01, tgt_cfg_len),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, svc_cfg) - QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, svc_cfg_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(wlfw_wlan_cfg_req_msg_v01, svc_cfg),
  QMI_WLFW_MAX_NUM_SVC_V01,
  QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, svc_cfg) - QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, svc_cfg_len),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, shadow_reg) - QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, shadow_reg_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(wlfw_wlan_cfg_req_msg_v01, shadow_reg),
  QMI_WLFW_MAX_NUM_SHADOW_REG_V01,
  QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, shadow_reg) - QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, shadow_reg_len),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, shadow_reg_v2) - QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, shadow_reg_v2_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(wlfw_wlan_cfg_req_msg_v01, shadow_reg_v2),
  QMI_WLFW_MAX_NUM_SHADOW_REG_V2_V01,
  QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, shadow_reg_v2) - QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, shadow_reg_v2_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, rri_over_ddr_cfg) - QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, rri_over_ddr_cfg_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(wlfw_wlan_cfg_req_msg_v01, rri_over_ddr_cfg),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, msi_cfg) - QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, msi_cfg_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(wlfw_wlan_cfg_req_msg_v01, msi_cfg),
  QMI_WLFW_MAX_NUM_CE_V01,
  QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, msi_cfg) - QMI_IDL_OFFSET16RELATIVE(wlfw_wlan_cfg_req_msg_v01, msi_cfg_len),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t wlfw_wlan_cfg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_wlan_cfg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * wlfw_cap_req_msg is empty
 * static const uint8_t wlfw_cap_req_msg_data_v01[] = {
 * };
 */

static const uint8_t wlfw_cap_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, chip_info) - QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, chip_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, chip_info),
  QMI_IDL_TYPE88(0, 10),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, board_info) - QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, board_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, board_info),
  QMI_IDL_TYPE88(0, 11),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, soc_info) - QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, soc_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, soc_info),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, fw_version_info) - QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, fw_version_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, fw_version_info),
  QMI_IDL_TYPE88(0, 13),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, fw_build_id) - QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, fw_build_id_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wlfw_cap_resp_msg_v01, fw_build_id),
  QMI_WLFW_MAX_BUILD_ID_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_cap_resp_msg_v01, num_macs) - QMI_IDL_OFFSET16RELATIVE(wlfw_cap_resp_msg_v01, num_macs_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_cap_resp_msg_v01, num_macs),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_cap_resp_msg_v01, voltage_mv) - QMI_IDL_OFFSET16RELATIVE(wlfw_cap_resp_msg_v01, voltage_mv_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_cap_resp_msg_v01, voltage_mv)
};

static const uint8_t wlfw_bdf_download_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, valid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, file_id) - QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, file_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, file_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, total_size) - QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, total_size_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, total_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, seg_id) - QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, seg_id_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, seg_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, data_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, data),
  ((QMI_WLFW_MAX_DATA_SIZE_V01) & 0xFF), ((QMI_WLFW_MAX_DATA_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_bdf_download_req_msg_v01, data_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_bdf_download_req_msg_v01, end) - QMI_IDL_OFFSET16RELATIVE(wlfw_bdf_download_req_msg_v01, end_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_bdf_download_req_msg_v01, end),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_bdf_download_req_msg_v01, bdf_type) - QMI_IDL_OFFSET16RELATIVE(wlfw_bdf_download_req_msg_v01, bdf_type_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_bdf_download_req_msg_v01, bdf_type)
};

static const uint8_t wlfw_bdf_download_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_bdf_download_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_cal_report_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_report_req_msg_v01, meta_data),
  QMI_WLFW_MAX_NUM_CAL_V01,
  QMI_IDL_OFFSET8(wlfw_cal_report_req_msg_v01, meta_data) - QMI_IDL_OFFSET8(wlfw_cal_report_req_msg_v01, meta_data_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cal_report_req_msg_v01, xo_cal_data) - QMI_IDL_OFFSET8(wlfw_cal_report_req_msg_v01, xo_cal_data_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_report_req_msg_v01, xo_cal_data)
};

static const uint8_t wlfw_cal_report_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_cal_report_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_initiate_cal_download_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_initiate_cal_download_ind_msg_v01, cal_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_initiate_cal_download_ind_msg_v01, total_size) - QMI_IDL_OFFSET8(wlfw_initiate_cal_download_ind_msg_v01, total_size_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_initiate_cal_download_ind_msg_v01, total_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_initiate_cal_download_ind_msg_v01, cal_data_location) - QMI_IDL_OFFSET8(wlfw_initiate_cal_download_ind_msg_v01, cal_data_location_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_initiate_cal_download_ind_msg_v01, cal_data_location)
};

static const uint8_t wlfw_cal_download_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, valid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, file_id) - QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, file_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, file_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, total_size) - QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, total_size_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, total_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, seg_id) - QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, seg_id_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, seg_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, data_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, data),
  ((QMI_WLFW_MAX_DATA_SIZE_V01) & 0xFF), ((QMI_WLFW_MAX_DATA_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_cal_download_req_msg_v01, data_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_cal_download_req_msg_v01, end) - QMI_IDL_OFFSET16RELATIVE(wlfw_cal_download_req_msg_v01, end_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_cal_download_req_msg_v01, end),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_cal_download_req_msg_v01, cal_data_location) - QMI_IDL_OFFSET16RELATIVE(wlfw_cal_download_req_msg_v01, cal_data_location_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_cal_download_req_msg_v01, cal_data_location)
};

static const uint8_t wlfw_cal_download_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_cal_download_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_initiate_cal_update_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_initiate_cal_update_ind_msg_v01, cal_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_initiate_cal_update_ind_msg_v01, total_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_initiate_cal_update_ind_msg_v01, cal_data_location) - QMI_IDL_OFFSET8(wlfw_initiate_cal_update_ind_msg_v01, cal_data_location_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_initiate_cal_update_ind_msg_v01, cal_data_location)
};

static const uint8_t wlfw_cal_update_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_update_req_msg_v01, cal_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_update_req_msg_v01, seg_id)
};

static const uint8_t wlfw_cal_update_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, file_id) - QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, file_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, file_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, total_size) - QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, total_size_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, total_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, seg_id) - QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, seg_id_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, seg_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, data_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, data),
  ((QMI_WLFW_MAX_DATA_SIZE_V01) & 0xFF), ((QMI_WLFW_MAX_DATA_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_cal_update_resp_msg_v01, data_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_cal_update_resp_msg_v01, end) - QMI_IDL_OFFSET16RELATIVE(wlfw_cal_update_resp_msg_v01, end_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_cal_update_resp_msg_v01, end),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_cal_update_resp_msg_v01, cal_data_location) - QMI_IDL_OFFSET16RELATIVE(wlfw_cal_update_resp_msg_v01, cal_data_location_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_cal_update_resp_msg_v01, cal_data_location)
};

static const uint8_t wlfw_msa_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_msa_info_req_msg_v01, msa_addr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_msa_info_req_msg_v01, size)
};

static const uint8_t wlfw_msa_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_msa_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_msa_info_resp_msg_v01, mem_region_info),
  QMI_WLFW_MAX_NUM_MEMORY_REGIONS_V01,
  QMI_IDL_OFFSET8(wlfw_msa_info_resp_msg_v01, mem_region_info) - QMI_IDL_OFFSET8(wlfw_msa_info_resp_msg_v01, mem_region_info_len),
  QMI_IDL_TYPE88(0, 6)
};

/*
 * wlfw_msa_ready_req_msg is empty
 * static const uint8_t wlfw_msa_ready_req_msg_data_v01[] = {
 * };
 */

static const uint8_t wlfw_msa_ready_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_msa_ready_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_ini_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_ini_req_msg_v01, enablefwlog) - QMI_IDL_OFFSET8(wlfw_ini_req_msg_v01, enablefwlog_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_ini_req_msg_v01, enablefwlog)
};

static const uint8_t wlfw_ini_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_ini_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_athdiag_read_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_athdiag_read_req_msg_v01, offset),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_athdiag_read_req_msg_v01, mem_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_athdiag_read_req_msg_v01, data_len)
};

static const uint8_t wlfw_athdiag_read_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_athdiag_read_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_athdiag_read_resp_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_athdiag_read_resp_msg_v01, data_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_athdiag_read_resp_msg_v01, data),
  ((QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01) & 0xFF), ((QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(wlfw_athdiag_read_resp_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_athdiag_read_resp_msg_v01, data_len)
};

static const uint8_t wlfw_athdiag_write_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_athdiag_write_req_msg_v01, offset),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_athdiag_write_req_msg_v01, mem_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_athdiag_write_req_msg_v01, data),
  ((QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01) & 0xFF), ((QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(wlfw_athdiag_write_req_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_athdiag_write_req_msg_v01, data_len)
};

static const uint8_t wlfw_athdiag_write_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_athdiag_write_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_vbatt_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_vbatt_req_msg_v01, voltage_uv)
};

static const uint8_t wlfw_vbatt_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_vbatt_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_mac_addr_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_mac_addr_req_msg_v01, mac_addr) - QMI_IDL_OFFSET8(wlfw_mac_addr_req_msg_v01, mac_addr_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_mac_addr_req_msg_v01, mac_addr),
  QMI_WLFW_MAC_ADDR_SIZE_V01
};

static const uint8_t wlfw_mac_addr_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_mac_addr_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_host_cap_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, num_clients) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, num_clients_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, num_clients),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, wake_msi) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, wake_msi_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, wake_msi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, gpios) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, gpios_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, gpios),
  QMI_WLFW_MAX_NUM_GPIO_V01,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, gpios) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, gpios_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, nm_modem) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, nm_modem_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, nm_modem),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, bdf_support) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, bdf_support_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, bdf_support),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, bdf_cache_support) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, bdf_cache_support_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, bdf_cache_support),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, m3_support) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, m3_support_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, m3_support),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, m3_cache_support) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, m3_cache_support_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, m3_cache_support),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_filesys_support) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_filesys_support_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_filesys_support),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_cache_support) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_cache_support_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_cache_support),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_done) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_done_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_done),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, mem_bucket) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, mem_bucket_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, mem_bucket),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, mem_cfg_mode) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, mem_cfg_mode_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, mem_cfg_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_duration) - QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_duration_valid)),
  0x1D,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(wlfw_host_cap_req_msg_v01, cal_duration)
};

static const uint8_t wlfw_host_cap_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_host_cap_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_request_mem_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_request_mem_ind_msg_v01, mem_seg),
  QMI_WLFW_MAX_NUM_MEM_SEG_V01,
  QMI_IDL_OFFSET8(wlfw_request_mem_ind_msg_v01, mem_seg) - QMI_IDL_OFFSET8(wlfw_request_mem_ind_msg_v01, mem_seg_len),
  QMI_IDL_TYPE88(0, 8)
};

static const uint8_t wlfw_respond_mem_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_respond_mem_req_msg_v01, mem_seg),
  QMI_WLFW_MAX_NUM_MEM_SEG_V01,
  QMI_IDL_OFFSET8(wlfw_respond_mem_req_msg_v01, mem_seg) - QMI_IDL_OFFSET8(wlfw_respond_mem_req_msg_v01, mem_seg_len),
  QMI_IDL_TYPE88(0, 9)
};

static const uint8_t wlfw_respond_mem_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_respond_mem_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * wlfw_fw_mem_ready_ind_msg is empty
 * static const uint8_t wlfw_fw_mem_ready_ind_msg_data_v01[] = {
 * };
 */

/*
 * wlfw_fw_init_done_ind_msg is empty
 * static const uint8_t wlfw_fw_init_done_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t wlfw_rejuvenate_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, cause_for_rejuvenation) - QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, cause_for_rejuvenation_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, cause_for_rejuvenation),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, requesting_sub_system) - QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, requesting_sub_system_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, requesting_sub_system),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, line_number) - QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, line_number_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, line_number),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, function_name) - QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, function_name_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(wlfw_rejuvenate_ind_msg_v01, function_name),
  QMI_WLFW_FUNCTION_NAME_LEN_V01
};

/*
 * wlfw_rejuvenate_ack_req_msg is empty
 * static const uint8_t wlfw_rejuvenate_ack_req_msg_data_v01[] = {
 * };
 */

static const uint8_t wlfw_rejuvenate_ack_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_rejuvenate_ack_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_dynamic_feature_mask_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_req_msg_v01, mask) - QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_req_msg_v01, mask_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_req_msg_v01, mask)
};

static const uint8_t wlfw_dynamic_feature_mask_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_resp_msg_v01, prev_mask) - QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_resp_msg_v01, prev_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_resp_msg_v01, prev_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_resp_msg_v01, curr_mask) - QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_resp_msg_v01, curr_mask_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_dynamic_feature_mask_resp_msg_v01, curr_mask)
};

static const uint8_t wlfw_m3_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_m3_info_req_msg_v01, addr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_m3_info_req_msg_v01, size)
};

static const uint8_t wlfw_m3_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_m3_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_xo_cal_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_xo_cal_ind_msg_v01, xo_cal_data)
};

/*
 * wlfw_cal_done_ind_msg is empty
 * static const uint8_t wlfw_cal_done_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t wlfw_qdss_trace_req_mem_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_req_mem_ind_msg_v01, mem_seg),
  QMI_WLFW_MAX_NUM_MEM_SEG_V01,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_req_mem_ind_msg_v01, mem_seg) - QMI_IDL_OFFSET8(wlfw_qdss_trace_req_mem_ind_msg_v01, mem_seg_len),
  QMI_IDL_TYPE88(0, 8)
};

static const uint8_t wlfw_qdss_trace_mem_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_mem_info_req_msg_v01, mem_seg),
  QMI_WLFW_MAX_NUM_MEM_SEG_V01,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_mem_info_req_msg_v01, mem_seg) - QMI_IDL_OFFSET8(wlfw_qdss_trace_mem_info_req_msg_v01, mem_seg_len),
  QMI_IDL_TYPE88(0, 9)
};

static const uint8_t wlfw_qdss_trace_mem_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_mem_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_qdss_trace_save_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_save_ind_msg_v01, source),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_save_ind_msg_v01, total_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_save_ind_msg_v01, mem_seg) - QMI_IDL_OFFSET8(wlfw_qdss_trace_save_ind_msg_v01, mem_seg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_save_ind_msg_v01, mem_seg),
  QMI_WLFW_MAX_NUM_MEM_SEG_V01,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_save_ind_msg_v01, mem_seg) - QMI_IDL_OFFSET8(wlfw_qdss_trace_save_ind_msg_v01, mem_seg_len),
  QMI_IDL_TYPE88(0, 9),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_qdss_trace_save_ind_msg_v01, file_name) - QMI_IDL_OFFSET16RELATIVE(wlfw_qdss_trace_save_ind_msg_v01, file_name_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(wlfw_qdss_trace_save_ind_msg_v01, file_name),
  QMI_WLFW_MAX_STR_LEN_V01
};

static const uint8_t wlfw_qdss_trace_data_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_data_req_msg_v01, seg_id)
};

static const uint8_t wlfw_qdss_trace_data_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, total_size) - QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, total_size_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, total_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, seg_id) - QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, seg_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, seg_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, data_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, data),
  ((QMI_WLFW_MAX_DATA_SIZE_V01) & 0xFF), ((QMI_WLFW_MAX_DATA_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_qdss_trace_data_resp_msg_v01, data_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_qdss_trace_data_resp_msg_v01, end) - QMI_IDL_OFFSET16RELATIVE(wlfw_qdss_trace_data_resp_msg_v01, end_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_qdss_trace_data_resp_msg_v01, end)
};

static const uint8_t wlfw_qdss_trace_config_download_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, total_size) - QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, total_size_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, total_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, seg_id) - QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, seg_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, seg_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, data_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, data),
  ((QMI_WLFW_MAX_DATA_SIZE_V01) & 0xFF), ((QMI_WLFW_MAX_DATA_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, data) - QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_req_msg_v01, data_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(wlfw_qdss_trace_config_download_req_msg_v01, end) - QMI_IDL_OFFSET16RELATIVE(wlfw_qdss_trace_config_download_req_msg_v01, end_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(wlfw_qdss_trace_config_download_req_msg_v01, end)
};

static const uint8_t wlfw_qdss_trace_config_download_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_config_download_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_qdss_trace_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_mode_req_msg_v01, mode) - QMI_IDL_OFFSET8(wlfw_qdss_trace_mode_req_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_mode_req_msg_v01, mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_mode_req_msg_v01, option) - QMI_IDL_OFFSET8(wlfw_qdss_trace_mode_req_msg_v01, option_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_mode_req_msg_v01, option)
};

static const uint8_t wlfw_qdss_trace_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t wlfw_qdss_trace_free_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_qdss_trace_free_ind_msg_v01, mem_seg) - QMI_IDL_OFFSET8(wlfw_qdss_trace_free_ind_msg_v01, mem_seg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_free_ind_msg_v01, mem_seg),
  QMI_WLFW_MAX_NUM_MEM_SEG_V01,
  QMI_IDL_OFFSET8(wlfw_qdss_trace_free_ind_msg_v01, mem_seg) - QMI_IDL_OFFSET8(wlfw_qdss_trace_free_ind_msg_v01, mem_seg_len),
  QMI_IDL_TYPE88(0, 9)
};

static const uint8_t wlfw_shutdown_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_shutdown_req_msg_v01, shutdown) - QMI_IDL_OFFSET8(wlfw_shutdown_req_msg_v01, shutdown_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlfw_shutdown_req_msg_v01, shutdown)
};

static const uint8_t wlfw_shutdown_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_shutdown_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * wlfw_antenna_switch_req_msg is empty
 * static const uint8_t wlfw_antenna_switch_req_msg_data_v01[] = {
 * };
 */

static const uint8_t wlfw_antenna_switch_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_antenna_switch_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_antenna_switch_resp_msg_v01, antenna) - QMI_IDL_OFFSET8(wlfw_antenna_switch_resp_msg_v01, antenna_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_antenna_switch_resp_msg_v01, antenna)
};

static const uint8_t wlfw_antenna_grant_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(wlfw_antenna_grant_req_msg_v01, grant) - QMI_IDL_OFFSET8(wlfw_antenna_grant_req_msg_v01, grant_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(wlfw_antenna_grant_req_msg_v01, grant)
};

static const uint8_t wlfw_antenna_grant_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlfw_antenna_grant_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  wlfw_type_table_v01[] = {
  {sizeof(wlfw_ce_tgt_pipe_cfg_s_v01), wlfw_ce_tgt_pipe_cfg_s_data_v01},
  {sizeof(wlfw_ce_svc_pipe_cfg_s_v01), wlfw_ce_svc_pipe_cfg_s_data_v01},
  {sizeof(wlfw_shadow_reg_cfg_s_v01), wlfw_shadow_reg_cfg_s_data_v01},
  {sizeof(wlfw_shadow_reg_v2_cfg_s_v01), wlfw_shadow_reg_v2_cfg_s_data_v01},
  {sizeof(wlfw_rri_over_ddr_cfg_s_v01), wlfw_rri_over_ddr_cfg_s_data_v01},
  {sizeof(wlfw_msi_cfg_s_v01), wlfw_msi_cfg_s_data_v01},
  {sizeof(wlfw_memory_region_info_s_v01), wlfw_memory_region_info_s_data_v01},
  {sizeof(wlfw_mem_cfg_s_v01), wlfw_mem_cfg_s_data_v01},
  {sizeof(wlfw_mem_seg_s_v01), wlfw_mem_seg_s_data_v01},
  {sizeof(wlfw_mem_seg_resp_s_v01), wlfw_mem_seg_resp_s_data_v01},
  {sizeof(wlfw_rf_chip_info_s_v01), wlfw_rf_chip_info_s_data_v01},
  {sizeof(wlfw_rf_board_info_s_v01), wlfw_rf_board_info_s_data_v01},
  {sizeof(wlfw_soc_info_s_v01), wlfw_soc_info_s_data_v01},
  {sizeof(wlfw_fw_version_info_s_v01), wlfw_fw_version_info_s_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry wlfw_message_table_v01[] = {
  {sizeof(wlfw_ind_register_req_msg_v01), wlfw_ind_register_req_msg_data_v01},
  {sizeof(wlfw_ind_register_resp_msg_v01), wlfw_ind_register_resp_msg_data_v01},
  {sizeof(wlfw_fw_ready_ind_msg_v01), 0},
  {sizeof(wlfw_msa_ready_ind_msg_v01), 0},
  {sizeof(wlfw_pin_connect_result_ind_msg_v01), wlfw_pin_connect_result_ind_msg_data_v01},
  {sizeof(wlfw_wlan_mode_req_msg_v01), wlfw_wlan_mode_req_msg_data_v01},
  {sizeof(wlfw_wlan_mode_resp_msg_v01), wlfw_wlan_mode_resp_msg_data_v01},
  {sizeof(wlfw_wlan_cfg_req_msg_v01), wlfw_wlan_cfg_req_msg_data_v01},
  {sizeof(wlfw_wlan_cfg_resp_msg_v01), wlfw_wlan_cfg_resp_msg_data_v01},
  {sizeof(wlfw_cap_req_msg_v01), 0},
  {sizeof(wlfw_cap_resp_msg_v01), wlfw_cap_resp_msg_data_v01},
  {sizeof(wlfw_bdf_download_req_msg_v01), wlfw_bdf_download_req_msg_data_v01},
  {sizeof(wlfw_bdf_download_resp_msg_v01), wlfw_bdf_download_resp_msg_data_v01},
  {sizeof(wlfw_cal_report_req_msg_v01), wlfw_cal_report_req_msg_data_v01},
  {sizeof(wlfw_cal_report_resp_msg_v01), wlfw_cal_report_resp_msg_data_v01},
  {sizeof(wlfw_initiate_cal_download_ind_msg_v01), wlfw_initiate_cal_download_ind_msg_data_v01},
  {sizeof(wlfw_cal_download_req_msg_v01), wlfw_cal_download_req_msg_data_v01},
  {sizeof(wlfw_cal_download_resp_msg_v01), wlfw_cal_download_resp_msg_data_v01},
  {sizeof(wlfw_initiate_cal_update_ind_msg_v01), wlfw_initiate_cal_update_ind_msg_data_v01},
  {sizeof(wlfw_cal_update_req_msg_v01), wlfw_cal_update_req_msg_data_v01},
  {sizeof(wlfw_cal_update_resp_msg_v01), wlfw_cal_update_resp_msg_data_v01},
  {sizeof(wlfw_msa_info_req_msg_v01), wlfw_msa_info_req_msg_data_v01},
  {sizeof(wlfw_msa_info_resp_msg_v01), wlfw_msa_info_resp_msg_data_v01},
  {sizeof(wlfw_msa_ready_req_msg_v01), 0},
  {sizeof(wlfw_msa_ready_resp_msg_v01), wlfw_msa_ready_resp_msg_data_v01},
  {sizeof(wlfw_ini_req_msg_v01), wlfw_ini_req_msg_data_v01},
  {sizeof(wlfw_ini_resp_msg_v01), wlfw_ini_resp_msg_data_v01},
  {sizeof(wlfw_athdiag_read_req_msg_v01), wlfw_athdiag_read_req_msg_data_v01},
  {sizeof(wlfw_athdiag_read_resp_msg_v01), wlfw_athdiag_read_resp_msg_data_v01},
  {sizeof(wlfw_athdiag_write_req_msg_v01), wlfw_athdiag_write_req_msg_data_v01},
  {sizeof(wlfw_athdiag_write_resp_msg_v01), wlfw_athdiag_write_resp_msg_data_v01},
  {sizeof(wlfw_vbatt_req_msg_v01), wlfw_vbatt_req_msg_data_v01},
  {sizeof(wlfw_vbatt_resp_msg_v01), wlfw_vbatt_resp_msg_data_v01},
  {sizeof(wlfw_mac_addr_req_msg_v01), wlfw_mac_addr_req_msg_data_v01},
  {sizeof(wlfw_mac_addr_resp_msg_v01), wlfw_mac_addr_resp_msg_data_v01},
  {sizeof(wlfw_host_cap_req_msg_v01), wlfw_host_cap_req_msg_data_v01},
  {sizeof(wlfw_host_cap_resp_msg_v01), wlfw_host_cap_resp_msg_data_v01},
  {sizeof(wlfw_request_mem_ind_msg_v01), wlfw_request_mem_ind_msg_data_v01},
  {sizeof(wlfw_respond_mem_req_msg_v01), wlfw_respond_mem_req_msg_data_v01},
  {sizeof(wlfw_respond_mem_resp_msg_v01), wlfw_respond_mem_resp_msg_data_v01},
  {sizeof(wlfw_fw_mem_ready_ind_msg_v01), 0},
  {sizeof(wlfw_fw_init_done_ind_msg_v01), 0},
  {sizeof(wlfw_rejuvenate_ind_msg_v01), wlfw_rejuvenate_ind_msg_data_v01},
  {sizeof(wlfw_rejuvenate_ack_req_msg_v01), 0},
  {sizeof(wlfw_rejuvenate_ack_resp_msg_v01), wlfw_rejuvenate_ack_resp_msg_data_v01},
  {sizeof(wlfw_dynamic_feature_mask_req_msg_v01), wlfw_dynamic_feature_mask_req_msg_data_v01},
  {sizeof(wlfw_dynamic_feature_mask_resp_msg_v01), wlfw_dynamic_feature_mask_resp_msg_data_v01},
  {sizeof(wlfw_m3_info_req_msg_v01), wlfw_m3_info_req_msg_data_v01},
  {sizeof(wlfw_m3_info_resp_msg_v01), wlfw_m3_info_resp_msg_data_v01},
  {sizeof(wlfw_xo_cal_ind_msg_v01), wlfw_xo_cal_ind_msg_data_v01},
  {sizeof(wlfw_cal_done_ind_msg_v01), 0},
  {sizeof(wlfw_qdss_trace_req_mem_ind_msg_v01), wlfw_qdss_trace_req_mem_ind_msg_data_v01},
  {sizeof(wlfw_qdss_trace_mem_info_req_msg_v01), wlfw_qdss_trace_mem_info_req_msg_data_v01},
  {sizeof(wlfw_qdss_trace_mem_info_resp_msg_v01), wlfw_qdss_trace_mem_info_resp_msg_data_v01},
  {sizeof(wlfw_qdss_trace_save_ind_msg_v01), wlfw_qdss_trace_save_ind_msg_data_v01},
  {sizeof(wlfw_qdss_trace_data_req_msg_v01), wlfw_qdss_trace_data_req_msg_data_v01},
  {sizeof(wlfw_qdss_trace_data_resp_msg_v01), wlfw_qdss_trace_data_resp_msg_data_v01},
  {sizeof(wlfw_qdss_trace_config_download_req_msg_v01), wlfw_qdss_trace_config_download_req_msg_data_v01},
  {sizeof(wlfw_qdss_trace_config_download_resp_msg_v01), wlfw_qdss_trace_config_download_resp_msg_data_v01},
  {sizeof(wlfw_qdss_trace_mode_req_msg_v01), wlfw_qdss_trace_mode_req_msg_data_v01},
  {sizeof(wlfw_qdss_trace_mode_resp_msg_v01), wlfw_qdss_trace_mode_resp_msg_data_v01},
  {sizeof(wlfw_qdss_trace_free_ind_msg_v01), wlfw_qdss_trace_free_ind_msg_data_v01},
  {sizeof(wlfw_shutdown_req_msg_v01), wlfw_shutdown_req_msg_data_v01},
  {sizeof(wlfw_shutdown_resp_msg_v01), wlfw_shutdown_resp_msg_data_v01},
  {sizeof(wlfw_antenna_switch_req_msg_v01), 0},
  {sizeof(wlfw_antenna_switch_resp_msg_v01), wlfw_antenna_switch_resp_msg_data_v01},
  {sizeof(wlfw_antenna_grant_req_msg_v01), wlfw_antenna_grant_req_msg_data_v01},
  {sizeof(wlfw_antenna_grant_resp_msg_v01), wlfw_antenna_grant_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object wlfw_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *wlfw_qmi_idl_type_table_object_referenced_tables_v01[] =
{&wlfw_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object wlfw_qmi_idl_type_table_object_v01 = {
  sizeof(wlfw_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(wlfw_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  wlfw_type_table_v01,
  wlfw_message_table_v01,
  wlfw_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry wlfw_service_command_messages_v01[] = {
  {QMI_WLFW_IND_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 66},
  {QMI_WLFW_WLAN_MODE_REQ_V01, QMI_IDL_TYPE16(0, 5), 11},
  {QMI_WLFW_WLAN_CFG_REQ_V01, QMI_IDL_TYPE16(0, 7), 866},
  {QMI_WLFW_CAP_REQ_V01, QMI_IDL_TYPE16(0, 9), 0},
  {QMI_WLFW_BDF_DOWNLOAD_REQ_V01, QMI_IDL_TYPE16(0, 11), 6182},
  {QMI_WLFW_CAL_REPORT_REQ_V01, QMI_IDL_TYPE16(0, 13), 28},
  {QMI_WLFW_CAL_DOWNLOAD_REQ_V01, QMI_IDL_TYPE16(0, 16), 6185},
  {QMI_WLFW_CAL_UPDATE_REQ_V01, QMI_IDL_TYPE16(0, 19), 14},
  {QMI_WLFW_MSA_INFO_REQ_V01, QMI_IDL_TYPE16(0, 21), 18},
  {QMI_WLFW_MSA_READY_REQ_V01, QMI_IDL_TYPE16(0, 23), 0},
  {QMI_WLFW_INI_REQ_V01, QMI_IDL_TYPE16(0, 25), 4},
  {QMI_WLFW_ATHDIAG_READ_REQ_V01, QMI_IDL_TYPE16(0, 27), 21},
  {QMI_WLFW_ATHDIAG_WRITE_REQ_V01, QMI_IDL_TYPE16(0, 29), 6163},
  {QMI_WLFW_VBATT_REQ_V01, QMI_IDL_TYPE16(0, 31), 11},
  {QMI_WLFW_MAC_ADDR_REQ_V01, QMI_IDL_TYPE16(0, 33), 9},
  {QMI_WLFW_HOST_CAP_REQ_V01, QMI_IDL_TYPE16(0, 35), 194},
  {QMI_WLFW_RESPOND_MEM_REQ_V01, QMI_IDL_TYPE16(0, 38), 548},
  {QMI_WLFW_REJUVENATE_ACK_REQ_V01, QMI_IDL_TYPE16(0, 43), 0},
  {QMI_WLFW_DYNAMIC_FEATURE_MASK_REQ_V01, QMI_IDL_TYPE16(0, 45), 11},
  {QMI_WLFW_M3_INFO_REQ_V01, QMI_IDL_TYPE16(0, 47), 18},
  {QMI_WLFW_QDSS_TRACE_MEM_INFO_REQ_V01, QMI_IDL_TYPE16(0, 52), 548},
  {QMI_WLFW_QDSS_TRACE_DATA_REQ_V01, QMI_IDL_TYPE16(0, 55), 7},
  {QMI_WLFW_SHUTDOWN_REQ_V01, QMI_IDL_TYPE16(0, 62), 4},
  {QMI_WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_REQ_V01, QMI_IDL_TYPE16(0, 57), 6167},
  {QMI_WLFW_QDSS_TRACE_MODE_REQ_V01, QMI_IDL_TYPE16(0, 59), 18},
  {QMI_WLFW_ANTENNA_SWITCH_REQ_V01, QMI_IDL_TYPE16(0, 64), 0},
  {QMI_WLFW_ANTENNA_GRANT_REQ_V01, QMI_IDL_TYPE16(0, 66), 11}
};

static const qmi_idl_service_message_table_entry wlfw_service_response_messages_v01[] = {
  {QMI_WLFW_IND_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 18},
  {QMI_WLFW_WLAN_MODE_RESP_V01, QMI_IDL_TYPE16(0, 6), 7},
  {QMI_WLFW_WLAN_CFG_RESP_V01, QMI_IDL_TYPE16(0, 8), 7},
  {QMI_WLFW_CAP_RESP_V01, QMI_IDL_TYPE16(0, 10), 214},
  {QMI_WLFW_BDF_DOWNLOAD_RESP_V01, QMI_IDL_TYPE16(0, 12), 7},
  {QMI_WLFW_CAL_REPORT_RESP_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_WLFW_CAL_DOWNLOAD_RESP_V01, QMI_IDL_TYPE16(0, 17), 7},
  {QMI_WLFW_CAL_UPDATE_RESP_V01, QMI_IDL_TYPE16(0, 20), 6188},
  {QMI_WLFW_MSA_INFO_RESP_V01, QMI_IDL_TYPE16(0, 22), 37},
  {QMI_WLFW_MSA_READY_RESP_V01, QMI_IDL_TYPE16(0, 24), 7},
  {QMI_WLFW_INI_RESP_V01, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_WLFW_ATHDIAG_READ_RESP_V01, QMI_IDL_TYPE16(0, 28), 6156},
  {QMI_WLFW_ATHDIAG_WRITE_RESP_V01, QMI_IDL_TYPE16(0, 30), 7},
  {QMI_WLFW_VBATT_RESP_V01, QMI_IDL_TYPE16(0, 32), 7},
  {QMI_WLFW_MAC_ADDR_RESP_V01, QMI_IDL_TYPE16(0, 34), 7},
  {QMI_WLFW_HOST_CAP_RESP_V01, QMI_IDL_TYPE16(0, 36), 7},
  {QMI_WLFW_RESPOND_MEM_RESP_V01, QMI_IDL_TYPE16(0, 39), 7},
  {QMI_WLFW_REJUVENATE_ACK_RESP_V01, QMI_IDL_TYPE16(0, 44), 7},
  {QMI_WLFW_DYNAMIC_FEATURE_MASK_RESP_V01, QMI_IDL_TYPE16(0, 46), 29},
  {QMI_WLFW_M3_INFO_RESP_V01, QMI_IDL_TYPE16(0, 48), 7},
  {QMI_WLFW_QDSS_TRACE_MEM_INFO_RESP_V01, QMI_IDL_TYPE16(0, 53), 7},
  {QMI_WLFW_QDSS_TRACE_DATA_RESP_V01, QMI_IDL_TYPE16(0, 56), 6174},
  {QMI_WLFW_SHUTDOWN_RESP_V01, QMI_IDL_TYPE16(0, 63), 7},
  {QMI_WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_RESP_V01, QMI_IDL_TYPE16(0, 58), 7},
  {QMI_WLFW_QDSS_TRACE_MODE_RESP_V01, QMI_IDL_TYPE16(0, 60), 7},
  {QMI_WLFW_ANTENNA_SWITCH_RESP_V01, QMI_IDL_TYPE16(0, 65), 18},
  {QMI_WLFW_ANTENNA_GRANT_RESP_V01, QMI_IDL_TYPE16(0, 67), 7}
};

static const qmi_idl_service_message_table_entry wlfw_service_indication_messages_v01[] = {
  {QMI_WLFW_FW_READY_IND_V01, QMI_IDL_TYPE16(0, 2), 0},
  {QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01, QMI_IDL_TYPE16(0, 15), 21},
  {QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01, QMI_IDL_TYPE16(0, 18), 21},
  {QMI_WLFW_MSA_READY_IND_V01, QMI_IDL_TYPE16(0, 3), 0},
  {QMI_WLFW_PIN_CONNECT_RESULT_IND_V01, QMI_IDL_TYPE16(0, 4), 21},
  {QMI_WLFW_REQUEST_MEM_IND_V01, QMI_IDL_TYPE16(0, 37), 1124},
  {QMI_WLFW_FW_MEM_READY_IND_V01, QMI_IDL_TYPE16(0, 40), 0},
  {QMI_WLFW_FW_INIT_DONE_IND_V01, QMI_IDL_TYPE16(0, 41), 0},
  {QMI_WLFW_REJUVENATE_IND_V01, QMI_IDL_TYPE16(0, 42), 144},
  {QMI_WLFW_XO_CAL_IND_V01, QMI_IDL_TYPE16(0, 49), 4},
  {QMI_WLFW_CAL_DONE_IND_V01, QMI_IDL_TYPE16(0, 50), 0},
  {QMI_WLFW_QDSS_TRACE_REQ_MEM_IND_V01, QMI_IDL_TYPE16(0, 51), 1124},
  {QMI_WLFW_QDSS_TRACE_SAVE_IND_V01, QMI_IDL_TYPE16(0, 54), 581},
  {QMI_WLFW_QDSS_TRACE_FREE_IND_V01, QMI_IDL_TYPE16(0, 61), 548}
};

/*Service Object*/
struct qmi_idl_service_object wlfw_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x45,
  6188,
  { sizeof(wlfw_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(wlfw_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(wlfw_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { wlfw_service_command_messages_v01, wlfw_service_response_messages_v01, wlfw_service_indication_messages_v01},
  &wlfw_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type wlfw_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( WLFW_V01_IDL_MAJOR_VERS != idl_maj_version || WLFW_V01_IDL_MINOR_VERS != idl_min_version
       || WLFW_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&wlfw_qmi_idl_service_object_v01;
}

