/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        A O S T L M _ A U D I T _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the aostlm_audit service Data structures.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/qmimsgs.adsp/2.6/aostlm_audit/src/aostlm_audit_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2 
   It was generated on: Fri Jan 23 2015 (Spin 0)
   From IDL File: aostlm_audit_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "aostlm_audit_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
/* 
 * aostlm_audit_req_msg is empty
 * static const uint8_t aostlm_audit_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t aostlm_audit_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(aostlm_audit_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(aostlm_audit_resp_msg_v01, audit_report) - QMI_IDL_OFFSET8(aostlm_audit_resp_msg_v01, audit_report_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(aostlm_audit_resp_msg_v01, audit_report),
  ((AOSTLM_MAX_AUDIT_MSG_LENGTH_V01) & 0xFF), ((AOSTLM_MAX_AUDIT_MSG_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(aostlm_audit_resp_msg_v01, audit_report) - QMI_IDL_OFFSET8(aostlm_audit_resp_msg_v01, audit_report_len)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry aostlm_audit_message_table_v01[] = {
  {sizeof(aostlm_audit_req_msg_v01), 0},
  {sizeof(aostlm_audit_resp_msg_v01), aostlm_audit_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object aostlm_audit_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *aostlm_audit_qmi_idl_type_table_object_referenced_tables_v01[] =
{&aostlm_audit_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object aostlm_audit_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(aostlm_audit_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  aostlm_audit_message_table_v01,
  aostlm_audit_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry aostlm_audit_service_command_messages_v01[] = {
  {AOSTLM_AUDIT_REQ_V01, QMI_IDL_TYPE16(0, 0), 0}
};

static const qmi_idl_service_message_table_entry aostlm_audit_service_response_messages_v01[] = {
  {AOSTLM_AUDIT_RESP_V01, QMI_IDL_TYPE16(0, 1), 3084}
};

/*Service Object*/
struct qmi_idl_service_object aostlm_audit_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x3E,
  3084,
  { sizeof(aostlm_audit_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(aostlm_audit_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { aostlm_audit_service_command_messages_v01, aostlm_audit_service_response_messages_v01, NULL},
  &aostlm_audit_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type aostlm_audit_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( AOSTLM_AUDIT_V01_IDL_MAJOR_VERS != idl_maj_version || AOSTLM_AUDIT_V01_IDL_MINOR_VERS != idl_min_version 
       || AOSTLM_AUDIT_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&aostlm_audit_qmi_idl_service_object_v01;
}

