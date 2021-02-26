/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M O D E M _ F I L E S Y S T E M _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mfs service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/mfs/src/modem_filesystem_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2 
   It was generated on: Tue Jul 23 2013 (Spin 1)
   From IDL File: modem_filesystem_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "modem_filesystem_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t mfs_put_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mfs_put_req_msg_v01, path),
  ((MFS_MAX_FILE_PATH_V01) & 0xFF), ((MFS_MAX_FILE_PATH_V01) >> 8),
  QMI_IDL_OFFSET8(mfs_put_req_msg_v01, path) - QMI_IDL_OFFSET8(mfs_put_req_msg_v01, path_len),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(mfs_put_req_msg_v01, data),
  ((MFS_MAX_ITEM_FILE_SIZE_V01) & 0xFF), ((MFS_MAX_ITEM_FILE_SIZE_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(mfs_put_req_msg_v01, data) - QMI_IDL_OFFSET16RELATIVE(mfs_put_req_msg_v01, data_len),

  0x03,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mfs_put_req_msg_v01, oflag),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mfs_put_req_msg_v01, mode)
};

static const uint8_t mfs_put_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mfs_put_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mfs_put_resp_msg_v01, efs_err_num) - QMI_IDL_OFFSET8(mfs_put_resp_msg_v01, efs_err_num_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mfs_put_resp_msg_v01, efs_err_num)
};

static const uint8_t mfs_get_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mfs_get_req_msg_v01, path),
  ((MFS_MAX_FILE_PATH_V01) & 0xFF), ((MFS_MAX_FILE_PATH_V01) >> 8),
  QMI_IDL_OFFSET8(mfs_get_req_msg_v01, path) - QMI_IDL_OFFSET8(mfs_get_req_msg_v01, path_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mfs_get_req_msg_v01, data_length) - QMI_IDL_OFFSET16RELATIVE(mfs_get_req_msg_v01, data_length_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mfs_get_req_msg_v01, data_length)
};

static const uint8_t mfs_get_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mfs_get_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mfs_get_resp_msg_v01, efs_err_num) - QMI_IDL_OFFSET8(mfs_get_resp_msg_v01, efs_err_num_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mfs_get_resp_msg_v01, efs_err_num),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mfs_get_resp_msg_v01, data) - QMI_IDL_OFFSET8(mfs_get_resp_msg_v01, data_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mfs_get_resp_msg_v01, data),
  ((MFS_MAX_ITEM_FILE_SIZE_V01) & 0xFF), ((MFS_MAX_ITEM_FILE_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(mfs_get_resp_msg_v01, data) - QMI_IDL_OFFSET8(mfs_get_resp_msg_v01, data_len)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry mfs_message_table_v01[] = {
  {sizeof(mfs_put_req_msg_v01), mfs_put_req_msg_data_v01},
  {sizeof(mfs_put_resp_msg_v01), mfs_put_resp_msg_data_v01},
  {sizeof(mfs_get_req_msg_v01), mfs_get_req_msg_data_v01},
  {sizeof(mfs_get_resp_msg_v01), mfs_get_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object mfs_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mfs_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mfs_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object mfs_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(mfs_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  mfs_message_table_v01,
  mfs_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry mfs_service_command_messages_v01[] = {
  {QMI_MFS_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_MFS_GET_SUPPORTED_FIELDS_REQ_V01, QMI_IDL_TYPE16(1, 2), 5},
  {QMI_MFS_PUT_REQ_V01, QMI_IDL_TYPE16(0, 0), 33816},
  {QMI_MFS_GET_REQ_V01, QMI_IDL_TYPE16(0, 2), 1036}
};

static const qmi_idl_service_message_table_entry mfs_service_response_messages_v01[] = {
  {QMI_MFS_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_MFS_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_MFS_PUT_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_MFS_GET_RESP_V01, QMI_IDL_TYPE16(0, 3), 32787}
};

/*Service Object*/
struct qmi_idl_service_object mfs_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x15,
  33816,
  { sizeof(mfs_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mfs_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { mfs_service_command_messages_v01, mfs_service_response_messages_v01, NULL},
  &mfs_qmi_idl_type_table_object_v01,
  0x02,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type mfs_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( MFS_V01_IDL_MAJOR_VERS != idl_maj_version || MFS_V01_IDL_MINOR_VERS != idl_min_version 
       || MFS_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&mfs_qmi_idl_service_object_v01;
}

