/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S E R V I C E _ R E G I S T R Y _ N O T I F I E R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the servreg_notif service Data structures.

  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2
   It was generated on: Fri Apr 17 2015 (Spin 0)
   From IDL File: service_registry_notifier_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "service_registry_notifier_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t qmi_servreg_notif_register_listener_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_notif_register_listener_req_msg_v01, enable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_servreg_notif_register_listener_req_msg_v01, service_name),
  QMI_SERVREG_NOTIF_NAME_LENGTH_V01
};

static const uint8_t qmi_servreg_notif_register_listener_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_servreg_notif_register_listener_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_servreg_notif_register_listener_resp_msg_v01, curr_state) - QMI_IDL_OFFSET8(qmi_servreg_notif_register_listener_resp_msg_v01, curr_state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_notif_register_listener_resp_msg_v01, curr_state)
};

static const uint8_t qmi_servreg_notif_query_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_servreg_notif_query_state_req_msg_v01, service_name),
  QMI_SERVREG_NOTIF_NAME_LENGTH_V01
};

static const uint8_t qmi_servreg_notif_query_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_servreg_notif_query_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_servreg_notif_query_state_resp_msg_v01, curr_state) - QMI_IDL_OFFSET8(qmi_servreg_notif_query_state_resp_msg_v01, curr_state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_notif_query_state_resp_msg_v01, curr_state)
};

static const uint8_t qmi_servreg_notif_state_updated_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_notif_state_updated_ind_msg_v01, curr_state),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_servreg_notif_state_updated_ind_msg_v01, service_name),
  QMI_SERVREG_NOTIF_NAME_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_notif_state_updated_ind_msg_v01, transaction_id)
};

static const uint8_t qmi_servreg_notif_set_ack_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_servreg_notif_set_ack_req_msg_v01, service_name),
  QMI_SERVREG_NOTIF_NAME_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_notif_set_ack_req_msg_v01, transaction_id)
};

static const uint8_t qmi_servreg_notif_set_ack_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_servreg_notif_set_ack_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry servreg_notif_message_table_v01[] = {
  {sizeof(qmi_servreg_notif_register_listener_req_msg_v01), qmi_servreg_notif_register_listener_req_msg_data_v01},
  {sizeof(qmi_servreg_notif_register_listener_resp_msg_v01), qmi_servreg_notif_register_listener_resp_msg_data_v01},
  {sizeof(qmi_servreg_notif_query_state_req_msg_v01), qmi_servreg_notif_query_state_req_msg_data_v01},
  {sizeof(qmi_servreg_notif_query_state_resp_msg_v01), qmi_servreg_notif_query_state_resp_msg_data_v01},
  {sizeof(qmi_servreg_notif_state_updated_ind_msg_v01), qmi_servreg_notif_state_updated_ind_msg_data_v01},
  {sizeof(qmi_servreg_notif_set_ack_req_msg_v01), qmi_servreg_notif_set_ack_req_msg_data_v01},
  {sizeof(qmi_servreg_notif_set_ack_resp_msg_v01), qmi_servreg_notif_set_ack_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object servreg_notif_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *servreg_notif_qmi_idl_type_table_object_referenced_tables_v01[] =
{&servreg_notif_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object servreg_notif_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(servreg_notif_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  servreg_notif_message_table_v01,
  servreg_notif_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry servreg_notif_service_command_messages_v01[] = {
  {QMI_SERVREG_NOTIF_REGISTER_LISTENER_REQ_V01, QMI_IDL_TYPE16(0, 0), 71},
  {QMI_SERVREG_NOTIF_QUERY_STATE_REQ_V01, QMI_IDL_TYPE16(0, 2), 67},
  {QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_REQ_V01, QMI_IDL_TYPE16(0, 5), 72}
};

static const qmi_idl_service_message_table_entry servreg_notif_service_response_messages_v01[] = {
  {QMI_SERVREG_NOTIF_REGISTER_LISTENER_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_SERVREG_NOTIF_QUERY_STATE_RESP_V01, QMI_IDL_TYPE16(0, 3), 14},
  {QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_RESP_V01, QMI_IDL_TYPE16(0, 6), 7}
};

static const qmi_idl_service_message_table_entry servreg_notif_service_indication_messages_v01[] = {
  {QMI_SERVREG_NOTIF_STATE_UPDATED_IND_V01, QMI_IDL_TYPE16(0, 4), 79}
};

/*Service Object*/
struct qmi_idl_service_object servreg_notif_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x42,
  79,
  { sizeof(servreg_notif_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(servreg_notif_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(servreg_notif_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { servreg_notif_service_command_messages_v01, servreg_notif_service_response_messages_v01, servreg_notif_service_indication_messages_v01},
  &servreg_notif_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type servreg_notif_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SERVREG_NOTIF_V01_IDL_MAJOR_VERS != idl_maj_version || SERVREG_NOTIF_V01_IDL_MINOR_VERS != idl_min_version
       || SERVREG_NOTIF_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&servreg_notif_qmi_idl_service_object_v01;
}

