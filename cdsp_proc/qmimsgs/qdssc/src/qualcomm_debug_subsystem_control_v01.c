/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q U A L C O M M _ D E B U G _ S U B S Y S T E M _ C O N T R O L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the qdssc service Data structures.

  Copyright (c) 2014, 2019 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/qdssc/src/qualcomm_debug_subsystem_control_v01.c#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Mon May 13 2019 (Spin 0)
   From IDL File: qualcomm_debug_subsystem_control_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "qualcomm_debug_subsystem_control_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
/*
 * qdssc_get_swt_req_msg is empty
 * static const uint8_t qdssc_get_swt_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qdssc_get_swt_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_get_swt_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qdssc_get_swt_resp_msg_v01, state) - QMI_IDL_OFFSET8(qdssc_get_swt_resp_msg_v01, state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_get_swt_resp_msg_v01, state)
};

static const uint8_t qdssc_set_swt_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_swt_req_msg_v01, state)
};

static const uint8_t qdssc_set_swt_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_set_swt_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qdssc_get_entity_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_get_entity_req_msg_v01, entity_id)
};

static const uint8_t qdssc_get_entity_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_get_entity_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qdssc_get_entity_resp_msg_v01, state) - QMI_IDL_OFFSET8(qdssc_get_entity_resp_msg_v01, state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_get_entity_resp_msg_v01, state)
};

static const uint8_t qdssc_set_entity_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_entity_req_msg_v01, entity_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_entity_req_msg_v01, state)
};

static const uint8_t qdssc_set_entity_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_set_entity_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qdssc_get_entity_all_req_msg is empty
 * static const uint8_t qdssc_get_entity_all_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qdssc_get_entity_all_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_get_entity_all_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qdssc_get_entity_all_resp_msg_v01, mask) - QMI_IDL_OFFSET8(qdssc_get_entity_all_resp_msg_v01, mask_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_get_entity_all_resp_msg_v01, mask),
  8
};

static const uint8_t qdssc_set_entity_all_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_entity_all_req_msg_v01, state)
};

static const uint8_t qdssc_set_entity_all_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_set_entity_all_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qdssc_get_swevt_tag_req_msg is empty
 * static const uint8_t qdssc_get_swevt_tag_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qdssc_get_swevt_tag_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_get_swevt_tag_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qdssc_get_swevt_tag_resp_msg_v01, tag_str) - QMI_IDL_OFFSET8(qdssc_get_swevt_tag_resp_msg_v01, tag_str_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qdssc_get_swevt_tag_resp_msg_v01, tag_str),
  QDSSC_SWEVT_TAG_MAX_LEN_V01
};

static const uint8_t qdssc_get_swevt_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_get_swevt_req_msg_v01, event_id)
};

static const uint8_t qdssc_get_swevt_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_get_swevt_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qdssc_get_swevt_resp_msg_v01, state) - QMI_IDL_OFFSET8(qdssc_get_swevt_resp_msg_v01, state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_get_swevt_resp_msg_v01, state)
};

static const uint8_t qdssc_set_swevt_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_swevt_req_msg_v01, event_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_swevt_req_msg_v01, state)
};

static const uint8_t qdssc_set_swevt_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_set_swevt_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qdssc_set_rpm_swe_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_rpm_swe_req_msg_v01, log),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_rpm_swe_req_msg_v01, group),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_rpm_swe_req_msg_v01, bitmask)
};

static const uint8_t qdssc_set_rpm_swe_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_set_rpm_swe_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qdssc_set_hwe_reg_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_hwe_reg_req_msg_v01, regaddr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_hwe_reg_req_msg_v01, value)
};

static const uint8_t qdssc_set_hwe_reg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_set_hwe_reg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qdssc_get_etm_req_msg is empty
 * static const uint8_t qdssc_get_etm_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qdssc_get_etm_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_get_etm_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qdssc_get_etm_resp_msg_v01, state) - QMI_IDL_OFFSET8(qdssc_get_etm_resp_msg_v01, state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_get_etm_resp_msg_v01, state)
};

static const uint8_t qdssc_set_etm_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_etm_req_msg_v01, state)
};

static const uint8_t qdssc_set_etm_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_set_etm_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qdssc_set_rpm_etm_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qdssc_set_rpm_etm_req_msg_v01, state)
};

static const uint8_t qdssc_set_rpm_etm_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_set_rpm_etm_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qdssc_get_etm_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qdssc_get_etm_config_req_msg_v01, query_str),
  QDSSC_ETM_CONFIG_MAX_LEN_V01
};

static const uint8_t qdssc_get_etm_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_get_etm_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qdssc_get_etm_config_resp_msg_v01, settings_str) - QMI_IDL_OFFSET8(qdssc_get_etm_config_resp_msg_v01, settings_str_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qdssc_get_etm_config_resp_msg_v01, settings_str),
  QDSSC_ETM_CONFIG_MAX_LEN_V01
};

static const uint8_t qdssc_set_etm_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qdssc_set_etm_config_req_msg_v01, command_str),
  QDSSC_ETM_CONFIG_MAX_LEN_V01
};

static const uint8_t qdssc_set_etm_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qdssc_set_etm_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry qdssc_message_table_v01[] = {
  {sizeof(qdssc_get_swt_req_msg_v01), 0},
  {sizeof(qdssc_get_swt_resp_msg_v01), qdssc_get_swt_resp_msg_data_v01},
  {sizeof(qdssc_set_swt_req_msg_v01), qdssc_set_swt_req_msg_data_v01},
  {sizeof(qdssc_set_swt_resp_msg_v01), qdssc_set_swt_resp_msg_data_v01},
  {sizeof(qdssc_get_entity_req_msg_v01), qdssc_get_entity_req_msg_data_v01},
  {sizeof(qdssc_get_entity_resp_msg_v01), qdssc_get_entity_resp_msg_data_v01},
  {sizeof(qdssc_set_entity_req_msg_v01), qdssc_set_entity_req_msg_data_v01},
  {sizeof(qdssc_set_entity_resp_msg_v01), qdssc_set_entity_resp_msg_data_v01},
  {sizeof(qdssc_get_entity_all_req_msg_v01), 0},
  {sizeof(qdssc_get_entity_all_resp_msg_v01), qdssc_get_entity_all_resp_msg_data_v01},
  {sizeof(qdssc_set_entity_all_req_msg_v01), qdssc_set_entity_all_req_msg_data_v01},
  {sizeof(qdssc_set_entity_all_resp_msg_v01), qdssc_set_entity_all_resp_msg_data_v01},
  {sizeof(qdssc_get_swevt_tag_req_msg_v01), 0},
  {sizeof(qdssc_get_swevt_tag_resp_msg_v01), qdssc_get_swevt_tag_resp_msg_data_v01},
  {sizeof(qdssc_get_swevt_req_msg_v01), qdssc_get_swevt_req_msg_data_v01},
  {sizeof(qdssc_get_swevt_resp_msg_v01), qdssc_get_swevt_resp_msg_data_v01},
  {sizeof(qdssc_set_swevt_req_msg_v01), qdssc_set_swevt_req_msg_data_v01},
  {sizeof(qdssc_set_swevt_resp_msg_v01), qdssc_set_swevt_resp_msg_data_v01},
  {sizeof(qdssc_set_rpm_swe_req_msg_v01), qdssc_set_rpm_swe_req_msg_data_v01},
  {sizeof(qdssc_set_rpm_swe_resp_msg_v01), qdssc_set_rpm_swe_resp_msg_data_v01},
  {sizeof(qdssc_set_hwe_reg_req_msg_v01), qdssc_set_hwe_reg_req_msg_data_v01},
  {sizeof(qdssc_set_hwe_reg_resp_msg_v01), qdssc_set_hwe_reg_resp_msg_data_v01},
  {sizeof(qdssc_get_etm_req_msg_v01), 0},
  {sizeof(qdssc_get_etm_resp_msg_v01), qdssc_get_etm_resp_msg_data_v01},
  {sizeof(qdssc_set_etm_req_msg_v01), qdssc_set_etm_req_msg_data_v01},
  {sizeof(qdssc_set_etm_resp_msg_v01), qdssc_set_etm_resp_msg_data_v01},
  {sizeof(qdssc_set_rpm_etm_req_msg_v01), qdssc_set_rpm_etm_req_msg_data_v01},
  {sizeof(qdssc_set_rpm_etm_resp_msg_v01), qdssc_set_rpm_etm_resp_msg_data_v01},
  {sizeof(qdssc_get_etm_config_req_msg_v01), qdssc_get_etm_config_req_msg_data_v01},
  {sizeof(qdssc_get_etm_config_resp_msg_v01), qdssc_get_etm_config_resp_msg_data_v01},
  {sizeof(qdssc_set_etm_config_req_msg_v01), qdssc_set_etm_config_req_msg_data_v01},
  {sizeof(qdssc_set_etm_config_resp_msg_v01), qdssc_set_etm_config_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object qdssc_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *qdssc_qmi_idl_type_table_object_referenced_tables_v01[] =
{&qdssc_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object qdssc_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(qdssc_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  qdssc_message_table_v01,
  qdssc_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry qdssc_service_command_messages_v01[] = {
  {QMI_QDSSC_GET_SWT_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_QDSSC_SET_SWT_REQ_V01, QMI_IDL_TYPE16(0, 2), 7},
  {QMI_QDSSC_GET_ENTITY_REQ_V01, QMI_IDL_TYPE16(0, 4), 7},
  {QMI_QDSSC_SET_ENTITY_REQ_V01, QMI_IDL_TYPE16(0, 6), 14},
  {QMI_QDSSC_GET_ENTITY_ALL_REQ_V01, QMI_IDL_TYPE16(0, 8), 0},
  {QMI_QDSSC_SET_ENTITY_ALL_REQ_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_QDSSC_GET_SWEVT_TAG_REQ_V01, QMI_IDL_TYPE16(0, 12), 0},
  {QMI_QDSSC_GET_SWEVT_REQ_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_QDSSC_SET_SWEVT_REQ_V01, QMI_IDL_TYPE16(0, 16), 14},
  {QMI_QDSSC_SET_RPM_SWE_REQ_V01, QMI_IDL_TYPE16(0, 18), 22},
  {QMI_QDSSC_SET_HWE_REG_REQ_V01, QMI_IDL_TYPE16(0, 20), 14},
  {QMI_QDSSC_GET_ETM_REQ_V01, QMI_IDL_TYPE16(0, 22), 0},
  {QMI_QDSSC_SET_ETM_REQ_V01, QMI_IDL_TYPE16(0, 24), 7},
  {QMI_QDSSC_SET_RPM_ETM_REQ_V01, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_QDSSC_GET_ETM_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 28), 131},
  {QMI_QDSSC_SET_ETM_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 30), 131}
};

static const qmi_idl_service_message_table_entry qdssc_service_response_messages_v01[] = {
  {QMI_QDSSC_GET_SWT_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_QDSSC_SET_SWT_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_QDSSC_GET_ENTITY_RESP_V01, QMI_IDL_TYPE16(0, 5), 14},
  {QMI_QDSSC_SET_ENTITY_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_QDSSC_GET_ENTITY_ALL_RESP_V01, QMI_IDL_TYPE16(0, 9), 42},
  {QMI_QDSSC_SET_ENTITY_ALL_RESP_V01, QMI_IDL_TYPE16(0, 11), 7},
  {QMI_QDSSC_GET_SWEVT_TAG_RESP_V01, QMI_IDL_TYPE16(0, 13), 22},
  {QMI_QDSSC_GET_SWEVT_RESP_V01, QMI_IDL_TYPE16(0, 15), 14},
  {QMI_QDSSC_SET_SWEVT_RESP_V01, QMI_IDL_TYPE16(0, 17), 7},
  {QMI_QDSSC_SET_RPM_SWE_RESP_V01, QMI_IDL_TYPE16(0, 19), 7},
  {QMI_QDSSC_SET_HWE_REG_RESP_V01, QMI_IDL_TYPE16(0, 21), 7},
  {QMI_QDSSC_GET_ETM_RESP_V01, QMI_IDL_TYPE16(0, 23), 14},
  {QMI_QDSSC_SET_ETM_RESP_V01, QMI_IDL_TYPE16(0, 25), 7},
  {QMI_QDSSC_SET_RPM_ETM_RESP_V01, QMI_IDL_TYPE16(0, 27), 7},
  {QMI_QDSSC_GET_ETM_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 29), 138},
  {QMI_QDSSC_SET_ETM_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 31), 7}
};

/*Service Object*/
struct qmi_idl_service_object qdssc_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x33,
  138,
  { sizeof(qdssc_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qdssc_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { qdssc_service_command_messages_v01, qdssc_service_response_messages_v01, NULL},
  &qdssc_qmi_idl_type_table_object_v01,
  0x02,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type qdssc_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( QDSSC_V01_IDL_MAJOR_VERS != idl_maj_version || QDSSC_V01_IDL_MINOR_VERS != idl_min_version
       || QDSSC_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&qdssc_qmi_idl_service_object_v01;
}

