/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        B A N D W I D T H _ L I M I T S _ M A N A G E M E N T _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the blm service Data structures.

  
  Copyright (c) 2015 Qualcomm Technologies, Inc.  
  All Rights Reserved
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //source/qcom/qct/interfaces/qmi/blm/main/latest/src/bandwidth_limits_management_v01.c#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.6 
   It was generated on: Thu Aug  6 2015 (Spin 1)
   From IDL File: bandwidth_limits_management_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "bandwidth_limits_management_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t blm_monitor_bw_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(blm_monitor_bw_req_msg_v01, ab_low_threshold),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(blm_monitor_bw_req_msg_v01, ab_high_threshold),

  0x03,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(blm_monitor_bw_req_msg_v01, ib_low_threshod),

  0x04,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(blm_monitor_bw_req_msg_v01, ib_high_threshold),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x05,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(blm_monitor_bw_req_msg_v01, mitigation_level)
};

static const uint8_t blm_monitor_bw_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(blm_monitor_bw_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t blm_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(blm_indication_register_req_msg_v01, enable_bw_usage_level) - QMI_IDL_OFFSET8(blm_indication_register_req_msg_v01, enable_bw_usage_level_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(blm_indication_register_req_msg_v01, enable_bw_usage_level)
};

static const uint8_t blm_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(blm_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t blm_bw_level_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(blm_bw_level_ind_msg_v01, ab_level),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(blm_bw_level_ind_msg_v01, ib_level)
};

/* 
 * blm_get_max_mitigation_level_req_msg is empty
 * static const uint8_t blm_get_max_mitigation_level_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t blm_get_max_mitigation_level_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(blm_get_max_mitigation_level_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(blm_get_max_mitigation_level_resp_msg_v01, max_mitigation_level) - QMI_IDL_OFFSET8(blm_get_max_mitigation_level_resp_msg_v01, max_mitigation_level_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(blm_get_max_mitigation_level_resp_msg_v01, max_mitigation_level)
};

/* 
 * blm_get_info_req_msg is empty
 * static const uint8_t blm_get_info_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t blm_get_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ab_low_threshold) - QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ab_low_threshold_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ab_low_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ab_high_threshold) - QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ab_high_threshold_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ab_high_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ib_low_threshod) - QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ib_low_threshod_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ib_low_threshod),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ib_high_threshold) - QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ib_high_threshold_valid)),
  0x13,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ib_high_threshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, mitigation_level) - QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, mitigation_level_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, mitigation_level),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ab_level) - QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ab_level_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ab_level),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ib_level) - QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ib_level_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(blm_get_info_resp_msg_v01, ib_level)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry blm_message_table_v01[] = {
  {sizeof(blm_monitor_bw_req_msg_v01), blm_monitor_bw_req_msg_data_v01},
  {sizeof(blm_monitor_bw_resp_msg_v01), blm_monitor_bw_resp_msg_data_v01},
  {sizeof(blm_indication_register_req_msg_v01), blm_indication_register_req_msg_data_v01},
  {sizeof(blm_indication_register_resp_msg_v01), blm_indication_register_resp_msg_data_v01},
  {sizeof(blm_bw_level_ind_msg_v01), blm_bw_level_ind_msg_data_v01},
  {sizeof(blm_get_max_mitigation_level_req_msg_v01), 0},
  {sizeof(blm_get_max_mitigation_level_resp_msg_v01), blm_get_max_mitigation_level_resp_msg_data_v01},
  {sizeof(blm_get_info_req_msg_v01), 0},
  {sizeof(blm_get_info_resp_msg_v01), blm_get_info_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object blm_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *blm_qmi_idl_type_table_object_referenced_tables_v01[] =
{&blm_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object blm_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(blm_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  blm_message_table_v01,
  blm_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry blm_service_command_messages_v01[] = {
  {QMI_BLM_MONITOR_BW_REQ_V01, QMI_IDL_TYPE16(0, 0), 51},
  {QMI_BLM_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 2), 4},
  {QMI_BLM_GET_MAX_MITIGATION_LEVEL_REQ_V01, QMI_IDL_TYPE16(0, 5), 0},
  {QMI_BLM_GET_INFO_REQ_V01, QMI_IDL_TYPE16(0, 7), 0}
};

static const qmi_idl_service_message_table_entry blm_service_response_messages_v01[] = {
  {QMI_BLM_MONITOR_BW_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_BLM_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_BLM_GET_MAX_MITIGATION_LEVEL_RESP_V01, QMI_IDL_TYPE16(0, 6), 14},
  {QMI_BLM_GET_INFO_RESP_V01, QMI_IDL_TYPE16(0, 8), 72}
};

static const qmi_idl_service_message_table_entry blm_service_indication_messages_v01[] = {
  {QMI_BLM_BW_LEVEL_IND_V01, QMI_IDL_TYPE16(0, 4), 14}
};

/*Service Object*/
struct qmi_idl_service_object blm_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x43,
  72,
  { sizeof(blm_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(blm_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(blm_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { blm_service_command_messages_v01, blm_service_response_messages_v01, blm_service_indication_messages_v01},
  &blm_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type blm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( BLM_V01_IDL_MAJOR_VERS != idl_maj_version || BLM_V01_IDL_MINOR_VERS != idl_min_version 
       || BLM_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&blm_qmi_idl_service_object_v01;
}

