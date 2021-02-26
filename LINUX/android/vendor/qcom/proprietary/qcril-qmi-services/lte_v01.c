/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        L T E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the lte service Data structures.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Wed May 25 2016 (Spin 1)
   From IDL File: lte_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "lte_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t qmi_lte_disc_plmn_id_s_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_plmn_id_s_type_v01, plmn_id),
  3,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmi_lte_disc_prose_app_id_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_prose_app_id_v01, prose_app_id_name),
  ((QMI_LTE_DISC_PA_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_PA_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t value_128_t_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(value_128_t_v01, value_1_64),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(value_128_t_v01, value_65_128),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmi_lte_disc_announcing_policy_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_announcing_policy_info_type_v01, PlmnId),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_announcing_policy_info_type_v01, T4005_ValidityTime),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_announcing_policy_info_type_v01, Range),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmi_lte_disc_monitoring_policy_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_monitoring_policy_info_type_v01, plmn_id),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_monitoring_policy_info_type_v01, T4005_ValidityTime),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_monitoring_policy_info_type_v01, RemainingTime),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qmi_lte_indication_reg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_indication_reg_req_msg_v01, indication_bitmask)
};

static const uint8_t qmi_lte_indication_reg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_indication_reg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_lte_disc_set_lted_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_req_msg_v01, OsId),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_req_msg_v01, DedicatedApnName) - QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_req_msg_v01, DedicatedApnName_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_req_msg_v01, DedicatedApnName),
  QMI_LTE_DISC_DEDICATED_APN_NAME_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_req_msg_v01, AnnouncingPolicyList) - QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_req_msg_v01, AnnouncingPolicyList_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_req_msg_v01, AnnouncingPolicyList),
  QMI_LTE_DISC_ANNOUNCING_POLICY_LIST_MAX_V01,
  QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_req_msg_v01, AnnouncingPolicyList) - QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_req_msg_v01, AnnouncingPolicyList_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_config_req_msg_v01, MonitoringPolicyList) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_config_req_msg_v01, MonitoringPolicyList_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_set_lted_config_req_msg_v01, MonitoringPolicyList),
  QMI_LTE_DISC_MONITORING_POLICY_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_config_req_msg_v01, MonitoringPolicyList) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_config_req_msg_v01, MonitoringPolicyList_len),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_config_req_msg_v01, BAKPassword) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_config_req_msg_v01, BAKPassword_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_set_lted_config_req_msg_v01, BAKPassword),
  QMI_LTE_DISC_PSK_MAX_LEN_V01,
  QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_config_req_msg_v01, BAKPassword) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_config_req_msg_v01, BAKPassword_len)
};

static const uint8_t qmi_lte_disc_set_lted_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qmi_lte_disc_get_lted_config_req_msg is empty
 * static const uint8_t qmi_lte_disc_get_lted_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_lte_disc_get_lted_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_config_resp_msg_v01, DedicatedApnName) - QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_config_resp_msg_v01, DedicatedApnName_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_config_resp_msg_v01, DedicatedApnName),
  QMI_LTE_DISC_DEDICATED_APN_NAME_MAX_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_config_resp_msg_v01, AnnouncingPolicy) - QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_config_resp_msg_v01, AnnouncingPolicy_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_config_resp_msg_v01, AnnouncingPolicy),
  QMI_LTE_DISC_ANNOUNCING_POLICY_LIST_MAX_V01,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_config_resp_msg_v01, AnnouncingPolicy) - QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_config_resp_msg_v01, AnnouncingPolicy_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_get_lted_config_resp_msg_v01, MonitoringPolicy) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_get_lted_config_resp_msg_v01, MonitoringPolicy_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_get_lted_config_resp_msg_v01, MonitoringPolicy),
  QMI_LTE_DISC_MONITORING_POLICY_LIST_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_get_lted_config_resp_msg_v01, MonitoringPolicy) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_get_lted_config_resp_msg_v01, MonitoringPolicy_len),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t qmi_lte_disc_set_lted_category_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_category_req_msg_v01, OsAppId) - QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_category_req_msg_v01, OsAppId_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_category_req_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_category_req_msg_v01, lted_category) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_set_lted_category_req_msg_v01, lted_category_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_set_lted_category_req_msg_v01, lted_category)
};

static const uint8_t qmi_lte_disc_set_lted_category_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_set_lted_category_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmi_lte_disc_get_lted_category_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_category_req_msg_v01, OsAppId) - QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_category_req_msg_v01, OsAppId_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_category_req_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8)
};

static const uint8_t qmi_lte_disc_get_lted_category_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_category_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_category_resp_msg_v01, lted_category) - QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_category_resp_msg_v01, lted_category_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_lted_category_resp_msg_v01, lted_category)
};

static const uint8_t qmi_lte_disc_terminate_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_terminate_req_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8)
};

static const uint8_t qmi_lte_disc_terminate_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_terminate_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qmi_lte_disc_get_service_status_req_msg is empty
 * static const uint8_t qmi_lte_disc_get_service_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_lte_disc_get_service_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_service_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_get_service_status_resp_msg_v01, PublishAllowed) - QMI_IDL_OFFSET8(qmi_lte_disc_get_service_status_resp_msg_v01, PublishAllowed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_service_status_resp_msg_v01, PublishAllowed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_get_service_status_resp_msg_v01, SubscribeAllowed) - QMI_IDL_OFFSET8(qmi_lte_disc_get_service_status_resp_msg_v01, SubscribeAllowed_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_get_service_status_resp_msg_v01, SubscribeAllowed)
};

static const uint8_t qmi_lte_disc_publish_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_publish_req_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, DiscoveryType) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, DiscoveryType_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_publish_req_msg_v01, DiscoveryType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, Expression) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, Expression_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_publish_req_msg_v01, Expression),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, PublishDuration) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, PublishDuration_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_publish_req_msg_v01, PublishDuration),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, ExpressionValidityTime) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, ExpressionValidityTime_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_publish_req_msg_v01, ExpressionValidityTime),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, MetaData) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_req_msg_v01, MetaData_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_publish_req_msg_v01, MetaData),
  ((QMI_LTE_DISC_METADATA_MAX_V01) & 0xFF), ((QMI_LTE_DISC_METADATA_MAX_V01) >> 8)
};

static const uint8_t qmi_lte_disc_publish_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_publish_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_publish_resp_msg_v01, OsAppId) - QMI_IDL_OFFSET8(qmi_lte_disc_publish_resp_msg_v01, OsAppId_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_publish_resp_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_resp_msg_v01, Expression) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_resp_msg_v01, Expression_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_publish_resp_msg_v01, Expression),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_resp_msg_v01, ExpressionResult) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_publish_resp_msg_v01, ExpressionResult_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_publish_resp_msg_v01, ExpressionResult)
};

static const uint8_t qmi_lte_disc_subscribe_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_subscribe_req_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_req_msg_v01, DiscoveryType) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_req_msg_v01, DiscoveryType_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_subscribe_req_msg_v01, DiscoveryType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_req_msg_v01, Expression) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_req_msg_v01, Expression_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_subscribe_req_msg_v01, Expression),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_req_msg_v01, SubscribeDuration) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_req_msg_v01, SubscribeDuration_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_subscribe_req_msg_v01, SubscribeDuration),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_req_msg_v01, ExpressionValidityTime) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_req_msg_v01, ExpressionValidityTime_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_subscribe_req_msg_v01, ExpressionValidityTime)
};

static const uint8_t qmi_lte_disc_subscribe_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_subscribe_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_subscribe_resp_msg_v01, OsAppId) - QMI_IDL_OFFSET8(qmi_lte_disc_subscribe_resp_msg_v01, OsAppId_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_subscribe_resp_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_resp_msg_v01, Expression) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_resp_msg_v01, Expression_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_subscribe_resp_msg_v01, Expression),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_resp_msg_v01, ExpressionResult) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_subscribe_resp_msg_v01, ExpressionResult_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_subscribe_resp_msg_v01, ExpressionResult)
};

static const uint8_t qmi_lte_disc_cancel_publish_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_cancel_publish_req_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_cancel_publish_req_msg_v01, Expression) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_cancel_publish_req_msg_v01, Expression_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_cancel_publish_req_msg_v01, Expression),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t qmi_lte_disc_cancel_publish_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_cancel_publish_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_cancel_publish_resp_msg_v01, OsAppId) - QMI_IDL_OFFSET8(qmi_lte_disc_cancel_publish_resp_msg_v01, OsAppId_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_cancel_publish_resp_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8)
};

static const uint8_t qmi_lte_disc_cancel_subscribe_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_cancel_subscribe_req_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_cancel_subscribe_req_msg_v01, Expression) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_cancel_subscribe_req_msg_v01, Expression_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_cancel_subscribe_req_msg_v01, Expression),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t qmi_lte_disc_cancel_subscribe_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_disc_cancel_subscribe_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_cancel_subscribe_resp_msg_v01, OsAppId) - QMI_IDL_OFFSET8(qmi_lte_disc_cancel_subscribe_resp_msg_v01, OsAppId_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_cancel_subscribe_resp_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8)
};

static const uint8_t qmi_lte_disc_broadcast_notification_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_broadcast_notification_ind_msg_v01, PublishAllowed) - QMI_IDL_OFFSET8(qmi_lte_disc_broadcast_notification_ind_msg_v01, PublishAllowed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_broadcast_notification_ind_msg_v01, PublishAllowed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_broadcast_notification_ind_msg_v01, SubscribeAllowed) - QMI_IDL_OFFSET8(qmi_lte_disc_broadcast_notification_ind_msg_v01, SubscribeAllowed_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_disc_broadcast_notification_ind_msg_v01, SubscribeAllowed)
};

static const uint8_t qmi_lte_disc_match_notification_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_match_notification_ind_msg_v01, OsAppId) - QMI_IDL_OFFSET8(qmi_lte_disc_match_notification_ind_msg_v01, OsAppId_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_match_notification_ind_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, Expression) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, Expression_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_match_notification_ind_msg_v01, Expression),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, MatchedExpression) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, MatchedExpression_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_match_notification_ind_msg_v01, MatchedExpression),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, MatchEventState) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, MatchEventState_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_match_notification_ind_msg_v01, MatchEventState),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, MetaDataIndex) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, MetaDataIndex_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_match_notification_ind_msg_v01, MetaDataIndex),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, MetaData) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_match_notification_ind_msg_v01, MetaData_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_match_notification_ind_msg_v01, MetaData),
  ((QMI_LTE_DISC_METADATA_MAX_V01) & 0xFF), ((QMI_LTE_DISC_METADATA_MAX_V01) >> 8)
};

static const uint8_t qmi_lte_disc_notification_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_disc_notification_ind_msg_v01, OsAppId) - QMI_IDL_OFFSET8(qmi_lte_disc_notification_ind_msg_v01, OsAppId_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmi_lte_disc_notification_ind_msg_v01, OsAppId),
  ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) & 0xFF), ((QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01) >> 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, AuthorizationResult) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, AuthorizationResult_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_notification_ind_msg_v01, AuthorizationResult),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, Expression) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, Expression_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_notification_ind_msg_v01, Expression),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, ExpressionResult) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, ExpressionResult_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_notification_ind_msg_v01, ExpressionResult),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, PublishStatusPerExpr) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, PublishStatusPerExpr_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_notification_ind_msg_v01, PublishStatusPerExpr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, SubscribeStatusPerExpr) - QMI_IDL_OFFSET16RELATIVE(qmi_lte_disc_notification_ind_msg_v01, SubscribeStatusPerExpr_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmi_lte_disc_notification_ind_msg_v01, SubscribeStatusPerExpr)
};

static const uint8_t qmi_lte_bind_subscription_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_bind_subscription_req_msg_v01, subscription)
};

static const uint8_t qmi_lte_bind_subscription_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qmi_lte_get_subscription_info_req_msg is empty
 * static const uint8_t qmi_lte_get_subscription_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qmi_lte_get_subscription_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_lte_get_subscription_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_get_subscription_info_resp_msg_v01, subscription) - QMI_IDL_OFFSET8(qmi_lte_get_subscription_info_resp_msg_v01, subscription_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_get_subscription_info_resp_msg_v01, subscription),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_get_subscription_info_resp_msg_v01, LteDiscCapability) - QMI_IDL_OFFSET8(qmi_lte_get_subscription_info_resp_msg_v01, LteDiscCapability_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_get_subscription_info_resp_msg_v01, LteDiscCapability)
};

static const uint8_t qmi_lte_subscription_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_subscription_info_ind_msg_v01, subscription) - QMI_IDL_OFFSET8(qmi_lte_subscription_info_ind_msg_v01, subscription_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_subscription_info_ind_msg_v01, subscription),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_lte_subscription_info_ind_msg_v01, LteDiscCapability) - QMI_IDL_OFFSET8(qmi_lte_subscription_info_ind_msg_v01, LteDiscCapability_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_lte_subscription_info_ind_msg_v01, LteDiscCapability)
};

/*
 * qmi_lte_disc_psk_expired_ind_msg is empty
 * static const uint8_t qmi_lte_disc_psk_expired_ind_msg_data_v01[] = {
 * };
 */

/* Type Table */
static const qmi_idl_type_table_entry  lte_type_table_v01[] = {
  {sizeof(qmi_lte_disc_plmn_id_s_type_v01), qmi_lte_disc_plmn_id_s_type_data_v01},
  {sizeof(qmi_lte_disc_prose_app_id_v01), qmi_lte_disc_prose_app_id_data_v01},
  {sizeof(value_128_t_v01), value_128_t_data_v01},
  {sizeof(qmi_lte_disc_announcing_policy_info_type_v01), qmi_lte_disc_announcing_policy_info_type_data_v01},
  {sizeof(qmi_lte_disc_monitoring_policy_info_type_v01), qmi_lte_disc_monitoring_policy_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry lte_message_table_v01[] = {
  {sizeof(qmi_lte_indication_reg_req_msg_v01), qmi_lte_indication_reg_req_msg_data_v01},
  {sizeof(qmi_lte_indication_reg_resp_msg_v01), qmi_lte_indication_reg_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_set_lted_config_req_msg_v01), qmi_lte_disc_set_lted_config_req_msg_data_v01},
  {sizeof(qmi_lte_disc_set_lted_config_resp_msg_v01), qmi_lte_disc_set_lted_config_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_get_lted_config_req_msg_v01), 0},
  {sizeof(qmi_lte_disc_get_lted_config_resp_msg_v01), qmi_lte_disc_get_lted_config_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_set_lted_category_req_msg_v01), qmi_lte_disc_set_lted_category_req_msg_data_v01},
  {sizeof(qmi_lte_disc_set_lted_category_resp_msg_v01), qmi_lte_disc_set_lted_category_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_get_lted_category_req_msg_v01), qmi_lte_disc_get_lted_category_req_msg_data_v01},
  {sizeof(qmi_lte_disc_get_lted_category_resp_msg_v01), qmi_lte_disc_get_lted_category_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_terminate_req_msg_v01), qmi_lte_disc_terminate_req_msg_data_v01},
  {sizeof(qmi_lte_disc_terminate_resp_msg_v01), qmi_lte_disc_terminate_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_get_service_status_req_msg_v01), 0},
  {sizeof(qmi_lte_disc_get_service_status_resp_msg_v01), qmi_lte_disc_get_service_status_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_publish_req_msg_v01), qmi_lte_disc_publish_req_msg_data_v01},
  {sizeof(qmi_lte_disc_publish_resp_msg_v01), qmi_lte_disc_publish_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_subscribe_req_msg_v01), qmi_lte_disc_subscribe_req_msg_data_v01},
  {sizeof(qmi_lte_disc_subscribe_resp_msg_v01), qmi_lte_disc_subscribe_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_cancel_publish_req_msg_v01), qmi_lte_disc_cancel_publish_req_msg_data_v01},
  {sizeof(qmi_lte_disc_cancel_publish_resp_msg_v01), qmi_lte_disc_cancel_publish_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_cancel_subscribe_req_msg_v01), qmi_lte_disc_cancel_subscribe_req_msg_data_v01},
  {sizeof(qmi_lte_disc_cancel_subscribe_resp_msg_v01), qmi_lte_disc_cancel_subscribe_resp_msg_data_v01},
  {sizeof(qmi_lte_disc_broadcast_notification_ind_msg_v01), qmi_lte_disc_broadcast_notification_ind_msg_data_v01},
  {sizeof(qmi_lte_disc_match_notification_ind_msg_v01), qmi_lte_disc_match_notification_ind_msg_data_v01},
  {sizeof(qmi_lte_disc_notification_ind_msg_v01), qmi_lte_disc_notification_ind_msg_data_v01},
  {sizeof(qmi_lte_bind_subscription_req_msg_v01), qmi_lte_bind_subscription_req_msg_data_v01},
  {sizeof(qmi_lte_bind_subscription_resp_msg_v01), qmi_lte_bind_subscription_resp_msg_data_v01},
  {sizeof(qmi_lte_get_subscription_info_req_msg_v01), 0},
  {sizeof(qmi_lte_get_subscription_info_resp_msg_v01), qmi_lte_get_subscription_info_resp_msg_data_v01},
  {sizeof(qmi_lte_subscription_info_ind_msg_v01), qmi_lte_subscription_info_ind_msg_data_v01},
  {sizeof(qmi_lte_disc_psk_expired_ind_msg_v01), 0}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object lte_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *lte_qmi_idl_type_table_object_referenced_tables_v01[] =
{&lte_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object lte_qmi_idl_type_table_object_v01 = {
  sizeof(lte_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(lte_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  lte_type_table_v01,
  lte_message_table_v01,
  lte_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry lte_service_command_messages_v01[] = {
  {QMI_LTE_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 11},
  {QMI_LTE_DISC_SET_LTED_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 2), 1298},
  {QMI_LTE_DISC_GET_LTED_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 4), 0},
  {QMI_LTE_DISC_SET_LTED_CATEGORY_REQ_V01, QMI_IDL_TYPE16(0, 6), 2058},
  {QMI_LTE_DISC_GET_CATEGORY_REQ_V01, QMI_IDL_TYPE16(0, 8), 2051},
  {QMI_LTE_DISC_TERMINATE_REQ_V01, QMI_IDL_TYPE16(0, 10), 2051},
  {QMI_LTE_DISC_GET_SERVICE_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 12), 0},
  {QMI_LTE_DISC_PUBLISH_REQ_V01, QMI_IDL_TYPE16(0, 14), 20512},
  {QMI_LTE_DISC_SUBSCRIBE_REQ_V01, QMI_IDL_TYPE16(0, 16), 4125},
  {QMI_LTE_DISC_CANCEL_PUBLISH_REQ_V01, QMI_IDL_TYPE16(0, 18), 4104},
  {QMI_LTE_DISC_CANCEL_SUBSCRIBE_REQ_V01, QMI_IDL_TYPE16(0, 20), 4104},
  {QMI_LTE_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 25), 7},
  {QMI_LTE_GET_SUBSCRIPTION_INFO_REQ_V01, QMI_IDL_TYPE16(0, 27), 0}
};

static const qmi_idl_service_message_table_entry lte_service_response_messages_v01[] = {
  {QMI_LTE_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LTE_DISC_SET_LTED_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_LTE_DISC_GET_LTED_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 5), 1218},
  {QMI_LTE_DISC_SET_LTED_CATEGORY_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_LTE_DISC_GET_CATEGORY_RESP_V01, QMI_IDL_TYPE16(0, 9), 14},
  {QMI_LTE_DISC_TERMINATE_RESP_V01, QMI_IDL_TYPE16(0, 11), 7},
  {QMI_LTE_DISC_GET_SERVICE_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 13), 15},
  {QMI_LTE_DISC_PUBLISH_RESP_V01, QMI_IDL_TYPE16(0, 15), 4118},
  {QMI_LTE_DISC_SUBSCRIBE_RESP_V01, QMI_IDL_TYPE16(0, 17), 4118},
  {QMI_LTE_DISC_CANCEL_PUBLISH_RESP_V01, QMI_IDL_TYPE16(0, 19), 2058},
  {QMI_LTE_DISC_CANCEL_SUBSCRIBE_RESP_V01, QMI_IDL_TYPE16(0, 21), 2058},
  {QMI_LTE_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_LTE_GET_SUBSCRIPTION_INFO_RESP_V01, QMI_IDL_TYPE16(0, 28), 18}
};

static const qmi_idl_service_message_table_entry lte_service_indication_messages_v01[] = {
  {QMI_LTE_DISC_NOTIFICATION_IND_V01, QMI_IDL_TYPE16(0, 24), 4126},
  {QMI_LTE_DISC_BROADCAST_NOTIFICATION_IND_V01, QMI_IDL_TYPE16(0, 22), 8},
  {QMI_LTE_DISC_MATCH_NOTIFICATION_IND_V01, QMI_IDL_TYPE16(0, 23), 22553},
  {QMI_LTE_SUBSCRIPTION_INFO_IND_V01, QMI_IDL_TYPE16(0, 29), 11},
  {QMI_LTE_DISC_PSK_EXPIRED_IND_V01, QMI_IDL_TYPE16(0, 30), 0}
};

/*Service Object*/
struct qmi_idl_service_object lte_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x46,
  22553,
  { sizeof(lte_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(lte_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(lte_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { lte_service_command_messages_v01, lte_service_response_messages_v01, lte_service_indication_messages_v01},
  &lte_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type lte_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( LTE_V01_IDL_MAJOR_VERS != idl_maj_version || LTE_V01_IDL_MINOR_VERS != idl_min_version
       || LTE_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&lte_qmi_idl_service_object_v01;
}

