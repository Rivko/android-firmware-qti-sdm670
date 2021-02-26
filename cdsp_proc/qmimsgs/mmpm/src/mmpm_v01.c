/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M M P M _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the dsppm_svc service Data structures.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/mmpm/src/mmpm_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Aug 17 2018 (Spin 0)
   From IDL File: mmpm_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mmpm_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t mmpm_register_param_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_register_param_type_v01, coreId),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_register_param_type_v01, instanceId),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mmpm_register_param_type_v01, clientName),
  MAX_CLIENT_NAME_SIZE_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_register_param_type_v01, clientType),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mmpm_clk_req_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_clk_req_type_v01, clkId),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_clk_req_type_v01, clkFreq),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_clk_req_type_v01, freqMatch),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mmpm_bw_req_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_bw_req_type_v01, masterPort),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_bw_req_type_v01, slavePort),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_bw_req_type_v01, bytesPerSec),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_bw_req_type_v01, usagePercent),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_bw_req_type_v01, usageType),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mmpm_mpps_req_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_mpps_req_type_v01, mppsTotal),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_mpps_req_type_v01, adspFloorClock),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mmpm_mips_req_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_mips_req_type_v01, mipsTotal),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_mips_req_type_v01, mipsPerThread),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_mips_req_type_v01, codeLocation),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_mips_req_type_v01, reqOperation),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mmpm_rsc_status_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_rsc_status_type_v01, rscId),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_rsc_status_type_v01, status),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t mmpm_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mmpm_register_req_msg_v01, regParam),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t mmpm_register_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mmpm_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mmpm_register_resp_msg_v01, clientId) - QMI_IDL_OFFSET8(mmpm_register_resp_msg_v01, clientId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_register_resp_msg_v01, clientId)
};

static const uint8_t mmpm_deregister_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_deregister_req_msg_v01, clientId)
};

static const uint8_t mmpm_deregister_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mmpm_deregister_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mmpm_deregister_resp_msg_v01, mmpmStatus) - QMI_IDL_OFFSET8(mmpm_deregister_resp_msg_v01, mmpmStatus_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_deregister_resp_msg_v01, mmpmStatus)
};

static const uint8_t mmpm_request_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, clientId),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, rscId),
  MAX_NUM_RESOURCES_V01,
  QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, rscId) - QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, rscId_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, regProgSpeed) - QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, regProgSpeed_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, regProgSpeed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, coreClockReq) - QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, coreClockReq_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, coreClockReq),
  MAX_NUM_REQUESTS_V01,
  QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, coreClockReq) - QMI_IDL_OFFSET8(mmpm_request_req_msg_v01, coreClockReq_len),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, genBwReq) - QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, genBwReq_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mmpm_request_req_msg_v01, genBwReq),
  MAX_NUM_REQUESTS_V01,
  QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, genBwReq) - QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, genBwReq_len),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, sleepLatReq) - QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, sleepLatReq_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mmpm_request_req_msg_v01, sleepLatReq),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, mipsReq) - QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, mipsReq_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mmpm_request_req_msg_v01, mipsReq),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, mppsReq) - QMI_IDL_OFFSET16RELATIVE(mmpm_request_req_msg_v01, mppsReq_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mmpm_request_req_msg_v01, mppsReq),
  QMI_IDL_TYPE88(0, 3)
};

static const uint8_t mmpm_request_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mmpm_request_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mmpm_request_resp_msg_v01, reqReturnStatus) - QMI_IDL_OFFSET8(mmpm_request_resp_msg_v01, reqReturnStatus_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mmpm_request_resp_msg_v01, reqReturnStatus),
  MAX_NUM_RESOURCES_V01,
  QMI_IDL_OFFSET8(mmpm_request_resp_msg_v01, reqReturnStatus) - QMI_IDL_OFFSET8(mmpm_request_resp_msg_v01, reqReturnStatus_len),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t mmpm_release_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_release_req_msg_v01, clientId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mmpm_release_req_msg_v01, rscId),
  MAX_NUM_RESOURCES_V01,
  QMI_IDL_OFFSET8(mmpm_release_req_msg_v01, rscId) - QMI_IDL_OFFSET8(mmpm_release_req_msg_v01, rscId_len)
};

static const uint8_t mmpm_release_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mmpm_release_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mmpm_release_resp_msg_v01, reqReturnStatus) - QMI_IDL_OFFSET8(mmpm_release_resp_msg_v01, reqReturnStatus_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mmpm_release_resp_msg_v01, reqReturnStatus),
  MAX_NUM_RESOURCES_V01,
  QMI_IDL_OFFSET8(mmpm_release_resp_msg_v01, reqReturnStatus) - QMI_IDL_OFFSET8(mmpm_release_resp_msg_v01, reqReturnStatus_len),
  QMI_IDL_TYPE88(0, 5)
};

/* Type Table */
static const qmi_idl_type_table_entry  dsppm_svc_type_table_v01[] = {
  {sizeof(mmpm_register_param_type_v01), mmpm_register_param_type_data_v01},
  {sizeof(mmpm_clk_req_type_v01), mmpm_clk_req_type_data_v01},
  {sizeof(mmpm_bw_req_type_v01), mmpm_bw_req_type_data_v01},
  {sizeof(mmpm_mpps_req_type_v01), mmpm_mpps_req_type_data_v01},
  {sizeof(mmpm_mips_req_type_v01), mmpm_mips_req_type_data_v01},
  {sizeof(mmpm_rsc_status_type_v01), mmpm_rsc_status_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry dsppm_svc_message_table_v01[] = {
  {sizeof(mmpm_register_req_msg_v01), mmpm_register_req_msg_data_v01},
  {sizeof(mmpm_register_resp_msg_v01), mmpm_register_resp_msg_data_v01},
  {sizeof(mmpm_deregister_req_msg_v01), mmpm_deregister_req_msg_data_v01},
  {sizeof(mmpm_deregister_resp_msg_v01), mmpm_deregister_resp_msg_data_v01},
  {sizeof(mmpm_request_req_msg_v01), mmpm_request_req_msg_data_v01},
  {sizeof(mmpm_request_resp_msg_v01), mmpm_request_resp_msg_data_v01},
  {sizeof(mmpm_release_req_msg_v01), mmpm_release_req_msg_data_v01},
  {sizeof(mmpm_release_resp_msg_v01), mmpm_release_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object dsppm_svc_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *dsppm_svc_qmi_idl_type_table_object_referenced_tables_v01[] =
{&dsppm_svc_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object dsppm_svc_qmi_idl_type_table_object_v01 = {
  sizeof(dsppm_svc_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(dsppm_svc_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  dsppm_svc_type_table_v01,
  dsppm_svc_message_table_v01,
  dsppm_svc_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry dsppm_svc_service_command_messages_v01[] = {
  {QMI_DSPPM_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 47},
  {QMI_DSPPM_DEREGISTER_REQ_V01, QMI_IDL_TYPE16(0, 2), 7},
  {QMI_DSPPM_REQUEST_REQ_V01, QMI_IDL_TYPE16(0, 4), 1119},
  {QMI_DSPPM_RELEASE_REQ_V01, QMI_IDL_TYPE16(0, 6), 43}
};

static const qmi_idl_service_message_table_entry dsppm_svc_service_response_messages_v01[] = {
  {QMI_DSPPM_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_DSPPM_DEREGISTER_RESP_V01, QMI_IDL_TYPE16(0, 3), 14},
  {QMI_DSPPM_REQUEST_RESP_V01, QMI_IDL_TYPE16(0, 5), 75},
  {QMI_DSPPM_RELEASE_RESP_V01, QMI_IDL_TYPE16(0, 7), 75}
};

/*Service Object*/
struct qmi_idl_service_object dsppm_svc_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  DSPPM_QMI_SVC_ID_V01,
  1119,
  { sizeof(dsppm_svc_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(dsppm_svc_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { dsppm_svc_service_command_messages_v01, dsppm_svc_service_response_messages_v01, NULL},
  &dsppm_svc_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type dsppm_svc_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( DSPPM_SVC_V01_IDL_MAJOR_VERS != idl_maj_version || DSPPM_SVC_V01_IDL_MINOR_VERS != idl_min_version
       || DSPPM_SVC_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&dsppm_svc_qmi_idl_service_object_v01;
}

