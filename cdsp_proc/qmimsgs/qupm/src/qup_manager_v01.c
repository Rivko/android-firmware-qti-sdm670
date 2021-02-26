/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q U P _ M A N A G E R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the qupm service Data structures.

  Copyright (c) 2014 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/qmimsgs.adsp/2.6/qupm/src/qup_manager_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.13 
   It was generated on: Wed Sep 24 2014 (Spin 0)
   From IDL File: qup_manager_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "qup_manager_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t qupm_ready_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qupm_ready_req_msg_v01, qup_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qupm_ready_req_msg_v01, flags) - QMI_IDL_OFFSET8(qupm_ready_req_msg_v01, flags_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qupm_ready_req_msg_v01, flags)
};

static const uint8_t qupm_ready_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qupm_ready_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qupm_ready_resp_msg_v01, status) - QMI_IDL_OFFSET8(qupm_ready_resp_msg_v01, status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qupm_ready_resp_msg_v01, status)
};

static const uint8_t qupm_take_ownership_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qupm_take_ownership_req_msg_v01, qup_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qupm_take_ownership_req_msg_v01, flags) - QMI_IDL_OFFSET8(qupm_take_ownership_req_msg_v01, flags_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qupm_take_ownership_req_msg_v01, flags)
};

static const uint8_t qupm_take_ownership_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qupm_take_ownership_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qupm_take_ownership_resp_msg_v01, status) - QMI_IDL_OFFSET8(qupm_take_ownership_resp_msg_v01, status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qupm_take_ownership_resp_msg_v01, status)
};

static const uint8_t qupm_give_ownership_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qupm_give_ownership_req_msg_v01, qup_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qupm_give_ownership_req_msg_v01, flags) - QMI_IDL_OFFSET8(qupm_give_ownership_req_msg_v01, flags_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qupm_give_ownership_req_msg_v01, flags)
};

static const uint8_t qupm_give_ownership_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qupm_give_ownership_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qupm_give_ownership_resp_msg_v01, status) - QMI_IDL_OFFSET8(qupm_give_ownership_resp_msg_v01, status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qupm_give_ownership_resp_msg_v01, status)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry qupm_message_table_v01[] = {
  {sizeof(qupm_ready_req_msg_v01), qupm_ready_req_msg_data_v01},
  {sizeof(qupm_ready_resp_msg_v01), qupm_ready_resp_msg_data_v01},
  {sizeof(qupm_take_ownership_req_msg_v01), qupm_take_ownership_req_msg_data_v01},
  {sizeof(qupm_take_ownership_resp_msg_v01), qupm_take_ownership_resp_msg_data_v01},
  {sizeof(qupm_give_ownership_req_msg_v01), qupm_give_ownership_req_msg_data_v01},
  {sizeof(qupm_give_ownership_resp_msg_v01), qupm_give_ownership_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object qupm_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *qupm_qmi_idl_type_table_object_referenced_tables_v01[] =
{&qupm_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object qupm_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(qupm_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  qupm_message_table_v01,
  qupm_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry qupm_service_command_messages_v01[] = {
  {QMI_QUPM_READY_REQ_V01, QMI_IDL_TYPE16(0, 0), 14},
  {QMI_QUPM_TAKE_OWNERSHIP_REQ_V01, QMI_IDL_TYPE16(0, 2), 14},
  {QMI_QUPM_GIVE_OWNERSHIP_REQ_V01, QMI_IDL_TYPE16(0, 4), 14}
};

static const qmi_idl_service_message_table_entry qupm_service_response_messages_v01[] = {
  {QMI_QUPM_READY_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_QUPM_TAKE_OWNERSHIP_RESP_V01, QMI_IDL_TYPE16(0, 3), 14},
  {QMI_QUPM_GIVE_OWNERSHIP_RESP_V01, QMI_IDL_TYPE16(0, 5), 14}
};

/*Service Object*/
struct qmi_idl_service_object qupm_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x303,
  14,
  { sizeof(qupm_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qupm_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { qupm_service_command_messages_v01, qupm_service_response_messages_v01, NULL},
  &qupm_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type qupm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( QUPM_V01_IDL_MAJOR_VERS != idl_maj_version || QUPM_V01_IDL_MINOR_VERS != idl_min_version 
       || QUPM_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&qupm_qmi_idl_service_object_v01;
}

