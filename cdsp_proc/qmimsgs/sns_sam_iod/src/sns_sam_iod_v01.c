/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S N S _ S A M _ I O D _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the SNS_SAM_IOD_SVC service Data structures.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/qmimsgs.adsp/2.6/sns_sam_iod/src/sns_sam_iod_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.4 
   It was generated on: Thu May  7 2015 (Spin 1)
   From IDL File: sns_sam_iod_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "sns_sam_iod_v01.h"
#include "sns_sam_common_v01.h"
#include "sns_common_v01.h"


/*Type Definitions*/
static const uint8_t sns_sam_iod_sunset_sunrise_ts_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_sunset_sunrise_ts_s_v01, sunset_ts),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_sunset_sunrise_ts_s_v01, sunrise_ts),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sns_sam_iod_report_data_s_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_report_data_s_v01, timestamp),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_report_data_s_v01, output_class),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_report_data_s_v01, reason_unknown),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_report_data_s_v01, confidence),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t sns_sam_iod_enable_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_enable_req_msg_v01, report_period),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_enable_req_msg_v01, timeofday),

  0x03,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_iod_enable_req_msg_v01, sunset_sunrise_ts),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_iod_enable_req_msg_v01, notify_suspend) - QMI_IDL_OFFSET8(sns_sam_iod_enable_req_msg_v01, notify_suspend_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_iod_enable_req_msg_v01, notify_suspend),
  QMI_IDL_TYPE88(2, 1)
};

static const uint8_t sns_sam_iod_enable_resp_msg_data_v01[] = {
  2,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_iod_enable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_iod_enable_resp_msg_v01, instance_id) - QMI_IDL_OFFSET8(sns_sam_iod_enable_resp_msg_v01, instance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_enable_resp_msg_v01, instance_id)
};

static const uint8_t sns_sam_iod_disable_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_disable_req_msg_v01, instance_id)
};

static const uint8_t sns_sam_iod_disable_resp_msg_data_v01[] = {
  2,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_iod_disable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_iod_disable_resp_msg_v01, instance_id) - QMI_IDL_OFFSET8(sns_sam_iod_disable_resp_msg_v01, instance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_disable_resp_msg_v01, instance_id)
};

static const uint8_t sns_sam_iod_get_report_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_get_report_req_msg_v01, instance_id)
};

static const uint8_t sns_sam_iod_get_report_resp_msg_data_v01[] = {
  2,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_iod_get_report_resp_msg_v01, resp),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_iod_get_report_resp_msg_v01, instance_id) - QMI_IDL_OFFSET8(sns_sam_iod_get_report_resp_msg_v01, instance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_get_report_resp_msg_v01, instance_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_iod_get_report_resp_msg_v01, report_data) - QMI_IDL_OFFSET8(sns_sam_iod_get_report_resp_msg_v01, report_data_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_iod_get_report_resp_msg_v01, report_data),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t sns_sam_iod_report_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_report_ind_msg_v01, instance_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_report_ind_msg_v01, timestamp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_iod_report_ind_msg_v01, report_data),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t sns_sam_iod_error_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_error_ind_msg_v01, error),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_error_ind_msg_v01, instance_id)
};

static const uint8_t sns_sam_iod_update_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, instance_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, sunset_sunrise_ts) - QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, sunset_sunrise_ts_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, sunset_sunrise_ts),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, confidence_threshold_indoor) - QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, confidence_threshold_indoor_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, confidence_threshold_indoor),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, confidence_threshold_outdoor) - QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, confidence_threshold_outdoor_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_update_req_msg_v01, confidence_threshold_outdoor)
};

static const uint8_t sns_sam_iod_update_resp_msg_data_v01[] = {
  2,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_iod_update_resp_msg_v01, resp),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_iod_update_resp_msg_v01, instance_id) - QMI_IDL_OFFSET8(sns_sam_iod_update_resp_msg_v01, instance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_iod_update_resp_msg_v01, instance_id)
};

/* Type Table */
static const qmi_idl_type_table_entry  SNS_SAM_IOD_SVC_type_table_v01[] = {
  {sizeof(sns_sam_iod_sunset_sunrise_ts_s_v01), sns_sam_iod_sunset_sunrise_ts_s_data_v01},
  {sizeof(sns_sam_iod_report_data_s_v01), sns_sam_iod_report_data_s_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry SNS_SAM_IOD_SVC_message_table_v01[] = {
  {sizeof(sns_sam_iod_enable_req_msg_v01), sns_sam_iod_enable_req_msg_data_v01},
  {sizeof(sns_sam_iod_enable_resp_msg_v01), sns_sam_iod_enable_resp_msg_data_v01},
  {sizeof(sns_sam_iod_disable_req_msg_v01), sns_sam_iod_disable_req_msg_data_v01},
  {sizeof(sns_sam_iod_disable_resp_msg_v01), sns_sam_iod_disable_resp_msg_data_v01},
  {sizeof(sns_sam_iod_get_report_req_msg_v01), sns_sam_iod_get_report_req_msg_data_v01},
  {sizeof(sns_sam_iod_get_report_resp_msg_v01), sns_sam_iod_get_report_resp_msg_data_v01},
  {sizeof(sns_sam_iod_report_ind_msg_v01), sns_sam_iod_report_ind_msg_data_v01},
  {sizeof(sns_sam_iod_error_ind_msg_v01), sns_sam_iod_error_ind_msg_data_v01},
  {sizeof(sns_sam_iod_update_req_msg_v01), sns_sam_iod_update_req_msg_data_v01},
  {sizeof(sns_sam_iod_update_resp_msg_v01), sns_sam_iod_update_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object SNS_SAM_IOD_SVC_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *SNS_SAM_IOD_SVC_qmi_idl_type_table_object_referenced_tables_v01[] =
{&SNS_SAM_IOD_SVC_qmi_idl_type_table_object_v01, &sns_sam_common_qmi_idl_type_table_object_v01, &sns_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object SNS_SAM_IOD_SVC_qmi_idl_type_table_object_v01 = {
  sizeof(SNS_SAM_IOD_SVC_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(SNS_SAM_IOD_SVC_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  SNS_SAM_IOD_SVC_type_table_v01,
  SNS_SAM_IOD_SVC_message_table_v01,
  SNS_SAM_IOD_SVC_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry SNS_SAM_IOD_SVC_service_command_messages_v01[] = {
  {SNS_SAM_IOD_CANCEL_REQ_V01, QMI_IDL_TYPE16(2, 0), 0},
  {SNS_SAM_IOD_VERSION_REQ_V01, QMI_IDL_TYPE16(2, 2), 0},
  {SNS_SAM_IOD_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 0), 33},
  {SNS_SAM_IOD_DISABLE_REQ_V01, QMI_IDL_TYPE16(0, 2), 4},
  {SNS_SAM_IOD_GET_REPORT_REQ_V01, QMI_IDL_TYPE16(0, 4), 4},
  {SNS_SAM_IOD_UPDATE_REQ_V01, QMI_IDL_TYPE16(0, 8), 29},
  {SNS_SAM_IOD_GET_ATTRIBUTES_REQ_V01, QMI_IDL_TYPE16(1, 0), 0}
};

static const qmi_idl_service_message_table_entry SNS_SAM_IOD_SVC_service_response_messages_v01[] = {
  {SNS_SAM_IOD_CANCEL_RESP_V01, QMI_IDL_TYPE16(2, 1), 5},
  {SNS_SAM_IOD_VERSION_RESP_V01, QMI_IDL_TYPE16(2, 3), 17},
  {SNS_SAM_IOD_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 1), 9},
  {SNS_SAM_IOD_DISABLE_RESP_V01, QMI_IDL_TYPE16(0, 3), 9},
  {SNS_SAM_IOD_GET_REPORT_RESP_V01, QMI_IDL_TYPE16(0, 5), 32},
  {SNS_SAM_IOD_UPDATE_RESP_V01, QMI_IDL_TYPE16(0, 9), 9},
  {SNS_SAM_IOD_GET_ATTRIBUTES_RESP_V01, QMI_IDL_TYPE16(1, 1), 79}
};

static const qmi_idl_service_message_table_entry SNS_SAM_IOD_SVC_service_indication_messages_v01[] = {
  {SNS_SAM_IOD_REPORT_IND_V01, QMI_IDL_TYPE16(0, 6), 34},
  {SNS_SAM_IOD_ERROR_IND_V01, QMI_IDL_TYPE16(0, 7), 8}
};

/*Service Object*/
struct qmi_idl_service_object SNS_SAM_IOD_SVC_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  SNS_QMI_SVC_ID_54_V01,
  79,
  { sizeof(SNS_SAM_IOD_SVC_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(SNS_SAM_IOD_SVC_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(SNS_SAM_IOD_SVC_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { SNS_SAM_IOD_SVC_service_command_messages_v01, SNS_SAM_IOD_SVC_service_response_messages_v01, SNS_SAM_IOD_SVC_service_indication_messages_v01},
  &SNS_SAM_IOD_SVC_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type SNS_SAM_IOD_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SNS_SAM_IOD_SVC_V01_IDL_MAJOR_VERS != idl_maj_version || SNS_SAM_IOD_SVC_V01_IDL_MINOR_VERS != idl_min_version 
       || SNS_SAM_IOD_SVC_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&SNS_SAM_IOD_SVC_qmi_idl_service_object_v01;
}

