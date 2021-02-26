/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S L I M B U S _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the slimbus service Data structures.

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/slimbus/src/slimbus_v01.c#3 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Oct 28 2016 (Spin 0)
   From IDL File: slimbus_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "slimbus_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t slimbus_select_inst_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(slimbus_select_inst_req_msg_v01, instance),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(slimbus_select_inst_req_msg_v01, mode) - QMI_IDL_OFFSET8(slimbus_select_inst_req_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(slimbus_select_inst_req_msg_v01, mode)
};

static const uint8_t slimbus_select_inst_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(slimbus_select_inst_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t slimbus_power_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(slimbus_power_req_msg_v01, pm_req),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(slimbus_power_req_msg_v01, resp_type) - QMI_IDL_OFFSET8(slimbus_power_req_msg_v01, resp_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(slimbus_power_req_msg_v01, resp_type)
};

static const uint8_t slimbus_power_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(slimbus_power_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * slimbus_check_framer_req_msg is empty
 * static const uint8_t slimbus_check_framer_req_msg_data_v01[] = {
 * };
 */

static const uint8_t slimbus_check_framer_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(slimbus_check_framer_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * slimbus_send_deferred_response_req_msg is empty
 * static const uint8_t slimbus_send_deferred_response_req_msg_data_v01[] = {
 * };
 */

static const uint8_t slimbus_send_deferred_response_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(slimbus_send_deferred_response_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry slimbus_message_table_v01[] = {
  {sizeof(slimbus_select_inst_req_msg_v01), slimbus_select_inst_req_msg_data_v01},
  {sizeof(slimbus_select_inst_resp_msg_v01), slimbus_select_inst_resp_msg_data_v01},
  {sizeof(slimbus_power_req_msg_v01), slimbus_power_req_msg_data_v01},
  {sizeof(slimbus_power_resp_msg_v01), slimbus_power_resp_msg_data_v01},
  {sizeof(slimbus_check_framer_req_msg_v01), 0},
  {sizeof(slimbus_check_framer_resp_msg_v01), slimbus_check_framer_resp_msg_data_v01},
  {sizeof(slimbus_send_deferred_response_req_msg_v01), 0},
  {sizeof(slimbus_send_deferred_response_resp_msg_v01), slimbus_send_deferred_response_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object slimbus_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *slimbus_qmi_idl_type_table_object_referenced_tables_v01[] =
{&slimbus_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object slimbus_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(slimbus_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  slimbus_message_table_v01,
  slimbus_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry slimbus_service_command_messages_v01[] = {
  {QMI_SLIMBUS_SELECT_INSTANCE_REQ_V01, QMI_IDL_TYPE16(0, 0), 14},
  {QMI_SLIMBUS_POWER_REQ_V01, QMI_IDL_TYPE16(0, 2), 14},
  {QMI_SLIMBUS_CHECK_FRAMER_REQ_V01, QMI_IDL_TYPE16(0, 4), 0},
  {QMI_SLIMBUS_SEND_DEFERRED_RESPONSE_REQ_V01, QMI_IDL_TYPE16(0, 6), 0}
};

static const qmi_idl_service_message_table_entry slimbus_service_response_messages_v01[] = {
  {QMI_SLIMBUS_SELECT_INSTANCE_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_SLIMBUS_POWER_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_SLIMBUS_CHECK_FRAMER_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_SLIMBUS_SEND_DEFERRED_RESPONSE_RESP_V01, QMI_IDL_TYPE16(0, 7), 7}
};

/*Service Object*/
struct qmi_idl_service_object slimbus_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x0301,
  14,
  { sizeof(slimbus_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(slimbus_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { slimbus_service_command_messages_v01, slimbus_service_response_messages_v01, NULL},
  &slimbus_qmi_idl_type_table_object_v01,
  0x02,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type slimbus_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SLIMBUS_V01_IDL_MAJOR_VERS != idl_maj_version || SLIMBUS_V01_IDL_MINOR_VERS != idl_min_version
       || SLIMBUS_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&slimbus_qmi_idl_service_object_v01;
}


