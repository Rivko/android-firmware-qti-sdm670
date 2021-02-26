/*
-----------------------------------------------------------------
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
---------------------------------------------------------------
*/
#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "service_registry_locator_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t servreg_loc_entry_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(servreg_loc_entry_v01, name),
  QMI_SERVREG_LOC_NAME_LENGTH_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(servreg_loc_entry_v01, instance_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(servreg_loc_entry_v01, service_data_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(servreg_loc_entry_v01, service_data),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qmi_servreg_loc_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_servreg_loc_indication_register_req_msg_v01, enable_database_updated_indication) - QMI_IDL_OFFSET8(qmi_servreg_loc_indication_register_req_msg_v01, enable_database_updated_indication_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_indication_register_req_msg_v01, enable_database_updated_indication)
};

static const uint8_t qmi_servreg_loc_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_servreg_loc_get_domain_list_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_req_msg_v01, service_name),
  QMI_SERVREG_LOC_NAME_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_req_msg_v01, domain_offset) - QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_req_msg_v01, domain_offset_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_req_msg_v01, domain_offset)
};

static const uint8_t qmi_servreg_loc_get_domain_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, total_domains) - QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, total_domains_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, total_domains),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, db_rev_count) - QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, db_rev_count_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, db_rev_count),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, domain_list) - QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, domain_list_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, domain_list),
  QMI_SERVREG_LOC_LIST_LENGTH_V01,
  QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, domain_list) - QMI_IDL_OFFSET8(qmi_servreg_loc_get_domain_list_resp_msg_v01, domain_list_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t qmi_servreg_loc_register_service_list_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_servreg_loc_register_service_list_req_msg_v01, domain_name),
  QMI_SERVREG_LOC_NAME_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_register_service_list_req_msg_v01, service_list),
  QMI_SERVREG_LOC_LIST_LENGTH_V01,
  QMI_IDL_OFFSET8(qmi_servreg_loc_register_service_list_req_msg_v01, service_list) - QMI_IDL_OFFSET8(qmi_servreg_loc_register_service_list_req_msg_v01, service_list_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t qmi_servreg_loc_register_service_list_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_register_service_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qmi_servreg_loc_database_updated_ind_msg is empty
 * static const uint8_t qmi_servreg_loc_database_updated_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_servreg_loc_pfr_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_servreg_loc_pfr_req_msg_v01, serv_name),
  QMI_SERVREG_LOC_NAME_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_servreg_loc_pfr_req_msg_v01, pd_failure_reason),
  ((QMI_SERVREG_LOC_PFR_LEN_V01) & 0xFF), ((QMI_SERVREG_LOC_PFR_LEN_V01) >> 8)
};

static const uint8_t qmi_servreg_loc_pfr_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_servreg_loc_pfr_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  servreg_loc_type_table_v01[] = {
  {sizeof(servreg_loc_entry_v01), servreg_loc_entry_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry servreg_loc_message_table_v01[] = {
  {sizeof(qmi_servreg_loc_indication_register_req_msg_v01), qmi_servreg_loc_indication_register_req_msg_data_v01},
  {sizeof(qmi_servreg_loc_indication_register_resp_msg_v01), qmi_servreg_loc_indication_register_resp_msg_data_v01},
  {sizeof(qmi_servreg_loc_get_domain_list_req_msg_v01), qmi_servreg_loc_get_domain_list_req_msg_data_v01},
  {sizeof(qmi_servreg_loc_get_domain_list_resp_msg_v01), qmi_servreg_loc_get_domain_list_resp_msg_data_v01},
  {sizeof(qmi_servreg_loc_register_service_list_req_msg_v01), qmi_servreg_loc_register_service_list_req_msg_data_v01},
  {sizeof(qmi_servreg_loc_register_service_list_resp_msg_v01), qmi_servreg_loc_register_service_list_resp_msg_data_v01},
  {sizeof(qmi_servreg_loc_database_updated_ind_msg_v01), 0},
  {sizeof(qmi_servreg_loc_pfr_req_msg_v01), qmi_servreg_loc_pfr_req_msg_data_v01},
  {sizeof(qmi_servreg_loc_pfr_resp_msg_v01), qmi_servreg_loc_pfr_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object servreg_loc_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *servreg_loc_qmi_idl_type_table_object_referenced_tables_v01[] =
{&servreg_loc_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object servreg_loc_qmi_idl_type_table_object_v01 = {
  sizeof(servreg_loc_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(servreg_loc_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  servreg_loc_type_table_v01,
  servreg_loc_message_table_v01,
  servreg_loc_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry servreg_loc_service_command_messages_v01[] = {
  {QMI_SERVREG_LOC_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 4},
  {QMI_SERVREG_LOC_GET_DOMAIN_LIST_REQ_V01, QMI_IDL_TYPE16(0, 2), 74},
  {QMI_SERVREG_LOC_REGISTER_SERVICE_LIST_REQ_V01, QMI_IDL_TYPE16(0, 4), 2439},
  {QMI_SERVREG_LOC_PFR_REQ_V01, QMI_IDL_TYPE16(0, 7), 326}
};

static const qmi_idl_service_message_table_entry servreg_loc_service_response_messages_v01[] = {
  {QMI_SERVREG_LOC_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_SERVREG_LOC_GET_DOMAIN_LIST_RESP_V01, QMI_IDL_TYPE16(0, 3), 2389},
  {QMI_SERVREG_LOC_REGISTER_SERVICE_LIST_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_SERVREG_LOC_PFR_RESP_V01, QMI_IDL_TYPE16(0, 8), 7}
};

static const qmi_idl_service_message_table_entry servreg_loc_service_indication_messages_v01[] = {
  {QMI_SERVREG_LOC_DATABASE_UPDATED_IND_V01, QMI_IDL_TYPE16(0, 6), 0}
};

/*Service Object*/
struct qmi_idl_service_object servreg_loc_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x40,
  2439,
  { sizeof(servreg_loc_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(servreg_loc_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(servreg_loc_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { servreg_loc_service_command_messages_v01, servreg_loc_service_response_messages_v01, servreg_loc_service_indication_messages_v01},
  &servreg_loc_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type servreg_loc_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SERVREG_LOC_V01_IDL_MAJOR_VERS != idl_maj_version || SERVREG_LOC_V01_IDL_MINOR_VERS != idl_min_version
       || SERVREG_LOC_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&servreg_loc_qmi_idl_service_object_v01;
}

