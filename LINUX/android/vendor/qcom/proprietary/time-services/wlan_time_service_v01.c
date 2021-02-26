/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        W L A N _ T I M E _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the wlan_time service Data structures.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Wed Oct 12 2016 (Spin 0)
   From IDL File: wlan_time_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "wlan_time_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t apps_set_time_on_wlan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(apps_set_time_on_wlan_req_msg_v01, base),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(apps_set_time_on_wlan_req_msg_v01, time),

  0x03,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(apps_set_time_on_wlan_req_msg_v01, timetick),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(apps_set_time_on_wlan_req_msg_v01, reserved)
};

static const uint8_t apps_set_time_on_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(apps_set_time_on_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t apps_get_time_from_wlan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(apps_get_time_from_wlan_req_msg_v01, base)
};

static const uint8_t apps_get_time_from_wlan_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(apps_get_time_from_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(apps_get_time_from_wlan_resp_msg_v01, base),

  0x04,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(apps_get_time_from_wlan_resp_msg_v01, time),

  0x05,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(apps_get_time_from_wlan_resp_msg_v01, timetick),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x06,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(apps_get_time_from_wlan_resp_msg_v01, reserved)
};

static const uint8_t wlan_stop_updates_to_apps_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(wlan_stop_updates_to_apps_req_msg_v01, base),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(wlan_stop_updates_to_apps_req_msg_v01, update)
};

static const uint8_t wlan_stop_updates_to_apps_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(wlan_stop_updates_to_apps_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_wlan_qmi_delay_cal_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmi_wlan_qmi_delay_cal_req_msg_v01, dummy)
};

static const uint8_t qmi_wlan_qmi_delay_cal_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_wlan_qmi_delay_cal_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmi_wlan_qmi_delay_cal_resp_msg_v01, dummy)
};

static const uint8_t qmi_apps_stop_update_to_wlan_message_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_apps_stop_update_to_wlan_message_v01, base),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_apps_stop_update_to_wlan_message_v01, update)
};

static const uint8_t qmi_wlan_set_time_on_apps_message_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_wlan_set_time_on_apps_message_v01, base)
};

static const uint8_t qmi_wlan_get_time_from_apps_message_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_wlan_get_time_from_apps_message_v01, base)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry wlan_time_message_table_v01[] = {
  {sizeof(apps_set_time_on_wlan_req_msg_v01), apps_set_time_on_wlan_req_msg_data_v01},
  {sizeof(apps_set_time_on_wlan_resp_msg_v01), apps_set_time_on_wlan_resp_msg_data_v01},
  {sizeof(apps_get_time_from_wlan_req_msg_v01), apps_get_time_from_wlan_req_msg_data_v01},
  {sizeof(apps_get_time_from_wlan_resp_msg_v01), apps_get_time_from_wlan_resp_msg_data_v01},
  {sizeof(wlan_stop_updates_to_apps_req_msg_v01), wlan_stop_updates_to_apps_req_msg_data_v01},
  {sizeof(wlan_stop_updates_to_apps_resp_msg_v01), wlan_stop_updates_to_apps_resp_msg_data_v01},
  {sizeof(qmi_wlan_qmi_delay_cal_req_msg_v01), qmi_wlan_qmi_delay_cal_req_msg_data_v01},
  {sizeof(qmi_wlan_qmi_delay_cal_resp_msg_v01), qmi_wlan_qmi_delay_cal_resp_msg_data_v01},
  {sizeof(qmi_apps_stop_update_to_wlan_message_v01), qmi_apps_stop_update_to_wlan_message_data_v01},
  {sizeof(qmi_wlan_set_time_on_apps_message_v01), qmi_wlan_set_time_on_apps_message_data_v01},
  {sizeof(qmi_wlan_get_time_from_apps_message_v01), qmi_wlan_get_time_from_apps_message_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object wlan_time_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *wlan_time_qmi_idl_type_table_object_referenced_tables_v01[] =
{&wlan_time_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object wlan_time_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(wlan_time_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  wlan_time_message_table_v01,
  wlan_time_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry wlan_time_service_command_messages_v01[] = {
  {QMI_WLAN_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_WLAN_GET_SUPPORTED_FIELDS_REQ_V01, QMI_IDL_TYPE16(1, 2), 5},
  {QMI_APPS_SET_TIME_ON_WLAN_BASE_REQ_V01, QMI_IDL_TYPE16(0, 0), 40},
  {QMI_APPS_GET_TIME_FROM_WLAN_BASE_REQ_V01, QMI_IDL_TYPE16(0, 2), 7},
  {QMI_WLAN_STOP_UPDATES_TO_APPS_REQ_V01, QMI_IDL_TYPE16(0, 4), 11},
  {QMI_WLAN_QMI_DELAY_CAL_REQ_V01, QMI_IDL_TYPE16(0, 6), 11}
};

static const qmi_idl_service_message_table_entry wlan_time_service_response_messages_v01[] = {
  {QMI_WLAN_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_WLAN_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_APPS_SET_TIME_ON_WLAN_BASE_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_APPS_GET_TIME_FROM_WLAN_BASE_RESP_V01, QMI_IDL_TYPE16(0, 3), 47},
  {QMI_WLAN_STOP_UPDATES_TO_APPS_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_WLAN_QMI_DELAY_CAL_RESP_V01, QMI_IDL_TYPE16(0, 7), 18}
};

static const qmi_idl_service_message_table_entry wlan_time_service_indication_messages_v01[] = {
  {QMI_APPS_STOP_UPDATES_TO_WLAN_IND_V01, QMI_IDL_TYPE16(0, 8), 11},
  {QMI_WLAN_SET_TIME_ON_APPS_IND_V01, QMI_IDL_TYPE16(0, 9), 7},
  {QMI_WLAN_GET_TIME_FROM_APPS_IND_V01, QMI_IDL_TYPE16(0, 10), 7}
};

/*Service Object*/
struct qmi_idl_service_object wlan_time_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x41E,
  8204,
  { sizeof(wlan_time_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(wlan_time_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(wlan_time_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { wlan_time_service_command_messages_v01, wlan_time_service_response_messages_v01, wlan_time_service_indication_messages_v01},
  &wlan_time_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type wlan_time_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( WLAN_TIME_V01_IDL_MAJOR_VERS != idl_maj_version || WLAN_TIME_V01_IDL_MINOR_VERS != idl_min_version
       || WLAN_TIME_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&wlan_time_qmi_idl_service_object_v01;
}

