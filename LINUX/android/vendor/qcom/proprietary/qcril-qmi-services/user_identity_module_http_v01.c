/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        U S E R _ I D E N T I T Y _ M O D U L E _ H T T P _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the uim_http service Data structures.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Tue Mar  8 2016 (Spin 0)
   From IDL File: user_identity_module_http_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "user_identity_module_http_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t uim_http_header_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(uim_http_header_type_v01, name),
  UIM_HTTP_HEADER_NAME_MAX_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(uim_http_header_type_v01, value),
  ((UIM_HTTP_HEADER_VALUE_MAX_LEN_V01) & 0xFF), ((UIM_HTTP_HEADER_VALUE_MAX_LEN_V01) >> 8),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t uim_http_segment_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(uim_http_segment_info_type_v01, total_size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(uim_http_segment_info_type_v01, segment_offset),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t uim_http_req_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(uim_http_req_info_type_v01, content_type),
  ((UIM_HTTP_HEADER_VALUE_MAX_LEN_V01) & 0xFF), ((UIM_HTTP_HEADER_VALUE_MAX_LEN_V01) >> 8),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(uim_http_req_info_type_v01, custom_header),
  UIM_HTTP_CUST_HEADER_MAX_COUNT_V01,
  QMI_IDL_OFFSET16RELATIVE(uim_http_req_info_type_v01, custom_header) - QMI_IDL_OFFSET16RELATIVE(uim_http_req_info_type_v01, custom_header_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * uim_http_reset_req_msg is empty
 * static const uint8_t uim_http_reset_req_msg_data_v01[] = {
 * };
 */

static const uint8_t uim_http_reset_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(uim_http_reset_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t uim_http_transaction_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(uim_http_transaction_req_msg_v01, result),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(uim_http_transaction_req_msg_v01, token_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(uim_http_transaction_req_msg_v01, headers) - QMI_IDL_OFFSET8(uim_http_transaction_req_msg_v01, headers_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(uim_http_transaction_req_msg_v01, headers),
  UIM_HTTP_CUST_HEADER_MAX_COUNT_V01,
  QMI_IDL_OFFSET8(uim_http_transaction_req_msg_v01, headers) - QMI_IDL_OFFSET8(uim_http_transaction_req_msg_v01, headers_len),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_req_msg_v01, segment_info) - QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_req_msg_v01, segment_info_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(uim_http_transaction_req_msg_v01, segment_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_req_msg_v01, payload_body) - QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_req_msg_v01, payload_body_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(uim_http_transaction_req_msg_v01, payload_body),
  ((UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01) & 0xFF), ((UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_req_msg_v01, payload_body) - QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_req_msg_v01, payload_body_len)
};

static const uint8_t uim_http_transaction_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(uim_http_transaction_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t uim_http_transaction_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(uim_http_transaction_ind_msg_v01, token_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(uim_http_transaction_ind_msg_v01, url) - QMI_IDL_OFFSET8(uim_http_transaction_ind_msg_v01, url_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(uim_http_transaction_ind_msg_v01, url),
  ((UIM_HTTP_URL_MAX_LEN_V01) & 0xFF), ((UIM_HTTP_URL_MAX_LEN_V01) >> 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_ind_msg_v01, headers) - QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_ind_msg_v01, headers_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(uim_http_transaction_ind_msg_v01, headers),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_ind_msg_v01, segment_info) - QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_ind_msg_v01, segment_info_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(uim_http_transaction_ind_msg_v01, segment_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_ind_msg_v01, payload_body) - QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_ind_msg_v01, payload_body_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(uim_http_transaction_ind_msg_v01, payload_body),
  ((UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01) & 0xFF), ((UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_ind_msg_v01, payload_body) - QMI_IDL_OFFSET16RELATIVE(uim_http_transaction_ind_msg_v01, payload_body_len)
};

/* Type Table */
static const qmi_idl_type_table_entry  uim_http_type_table_v01[] = {
  {sizeof(uim_http_header_type_v01), uim_http_header_type_data_v01},
  {sizeof(uim_http_segment_info_type_v01), uim_http_segment_info_type_data_v01},
  {sizeof(uim_http_req_info_type_v01), uim_http_req_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry uim_http_message_table_v01[] = {
  {sizeof(uim_http_reset_req_msg_v01), 0},
  {sizeof(uim_http_reset_resp_msg_v01), uim_http_reset_resp_msg_data_v01},
  {sizeof(uim_http_transaction_req_msg_v01), uim_http_transaction_req_msg_data_v01},
  {sizeof(uim_http_transaction_resp_msg_v01), uim_http_transaction_resp_msg_data_v01},
  {sizeof(uim_http_transaction_ind_msg_v01), uim_http_transaction_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object uim_http_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *uim_http_qmi_idl_type_table_object_referenced_tables_v01[] =
{&uim_http_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object uim_http_qmi_idl_type_table_object_v01 = {
  sizeof(uim_http_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(uim_http_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  uim_http_type_table_v01,
  uim_http_message_table_v01,
  uim_http_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry uim_http_service_command_messages_v01[] = {
  {QMI_UIM_HTTP_RESET_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_UIM_HTTP_TRANSACTION_REQ_V01, QMI_IDL_TYPE16(0, 2), 4992}
};

static const qmi_idl_service_message_table_entry uim_http_service_response_messages_v01[] = {
  {QMI_UIM_HTTP_RESET_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_UIM_HTTP_TRANSACTION_RESP_V01, QMI_IDL_TYPE16(0, 3), 7}
};

static const qmi_idl_service_message_table_entry uim_http_service_indication_messages_v01[] = {
  {QMI_UIM_HTTP_TRANSACTION_IND_V01, QMI_IDL_TYPE16(0, 4), 5758}
};

/*Service Object*/
struct qmi_idl_service_object uim_http_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x47,
  5758,
  { sizeof(uim_http_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(uim_http_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(uim_http_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { uim_http_service_command_messages_v01, uim_http_service_response_messages_v01, uim_http_service_indication_messages_v01},
  &uim_http_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type uim_http_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( UIM_HTTP_V01_IDL_MAJOR_VERS != idl_maj_version || UIM_HTTP_V01_IDL_MINOR_VERS != idl_min_version
       || UIM_HTTP_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&uim_http_qmi_idl_service_object_v01;
}

