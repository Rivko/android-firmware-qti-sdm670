/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        A U T H E N T I C A T I O N _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the auth service Data structures.

  (c) 2011-2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Mon Apr 17 2017 (Spin 0)
   From IDL File: authentication_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "authentication_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t auth_aka_params_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(auth_aka_params_v01, rand),
  QMI_AUTH_AKA_RAND_MAX_V01,
  QMI_IDL_OFFSET8(auth_aka_params_v01, rand) - QMI_IDL_OFFSET8(auth_aka_params_v01, rand_len),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(auth_aka_params_v01, autn),
  QMI_AUTH_AKA_AUTN_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(auth_aka_params_v01, autn) - QMI_IDL_OFFSET16RELATIVE(auth_aka_params_v01, autn_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t auth_aka_algo_result_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(auth_aka_algo_result_v01, aka_handle),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(auth_aka_algo_result_v01, aka_status),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t auth_aka_response_data_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(auth_aka_response_data_v01, digest),
  QMI_AUTH_AKA_DIGEST_MAX_V01,
  QMI_IDL_OFFSET8(auth_aka_response_data_v01, digest) - QMI_IDL_OFFSET8(auth_aka_response_data_v01, digest_len),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(auth_aka_response_data_v01, aka_data),
  QMI_AUTH_AKA_DATA_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(auth_aka_response_data_v01, aka_data) - QMI_IDL_OFFSET16RELATIVE(auth_aka_response_data_v01, aka_data_len),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * auth_reset_req_msg is empty
 * static const uint8_t auth_reset_req_msg_data_v01[] = {
 * };
 */

static const uint8_t auth_reset_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_reset_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t auth_start_eap_session_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(auth_start_eap_session_req_msg_v01, eap_method_mask) - QMI_IDL_OFFSET8(auth_start_eap_session_req_msg_v01, eap_method_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(auth_start_eap_session_req_msg_v01, eap_method_mask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(auth_start_eap_session_req_msg_v01, user_id) - QMI_IDL_OFFSET8(auth_start_eap_session_req_msg_v01, user_id_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(auth_start_eap_session_req_msg_v01, user_id),
  QMI_AUTH_EAP_IDENTITY_MAX_CHAR_V01,
  QMI_IDL_OFFSET8(auth_start_eap_session_req_msg_v01, user_id) - QMI_IDL_OFFSET8(auth_start_eap_session_req_msg_v01, user_id_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(auth_start_eap_session_req_msg_v01, eap_meta_identity) - QMI_IDL_OFFSET16RELATIVE(auth_start_eap_session_req_msg_v01, eap_meta_identity_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(auth_start_eap_session_req_msg_v01, eap_meta_identity),
  QMI_AUTH_EAP_IDENTITY_MAX_CHAR_V01,
  QMI_IDL_OFFSET16RELATIVE(auth_start_eap_session_req_msg_v01, eap_meta_identity) - QMI_IDL_OFFSET16RELATIVE(auth_start_eap_session_req_msg_v01, eap_meta_identity_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(auth_start_eap_session_req_msg_v01, eap_sim_aka_algo) - QMI_IDL_OFFSET16RELATIVE(auth_start_eap_session_req_msg_v01, eap_sim_aka_algo_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(auth_start_eap_session_req_msg_v01, eap_sim_aka_algo)
};

static const uint8_t auth_start_eap_session_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_start_eap_session_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t auth_send_eap_packet_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_FIRST_EXTENDED |  QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS |  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS,
  QMI_IDL_OFFSET8(auth_send_eap_packet_req_msg_v01, eap_request_pkt),
  ((QMI_AUTH_EAP_REQ_PACKET_MAX_V01) & 0xFF), ((QMI_AUTH_EAP_REQ_PACKET_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(auth_send_eap_packet_req_msg_v01, eap_request_pkt) - QMI_IDL_OFFSET8(auth_send_eap_packet_req_msg_v01, eap_request_pkt_len)
};

static const uint8_t auth_send_eap_packet_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(auth_send_eap_packet_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_FIRST_EXTENDED |  QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS |  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS,
  QMI_IDL_OFFSET8(auth_send_eap_packet_resp_msg_v01, eap_response_pkt),
  ((QMI_AUTH_EAP_RESP_PACKET_MAX_V01) & 0xFF), ((QMI_AUTH_EAP_RESP_PACKET_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(auth_send_eap_packet_resp_msg_v01, eap_response_pkt) - QMI_IDL_OFFSET8(auth_send_eap_packet_resp_msg_v01, eap_response_pkt_len)
};

static const uint8_t auth_eap_session_result_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(auth_eap_session_result_ind_msg_v01, eap_result)
};

/*
 * auth_get_eap_session_keys_req_msg is empty
 * static const uint8_t auth_get_eap_session_keys_req_msg_data_v01[] = {
 * };
 */

static const uint8_t auth_get_eap_session_keys_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(auth_get_eap_session_keys_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_FIRST_EXTENDED |  QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS |  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS,
  QMI_IDL_OFFSET8(auth_get_eap_session_keys_resp_msg_v01, session_key),
  ((QMI_AUTH_SESSION_KEY_MAX_V01) & 0xFF), ((QMI_AUTH_SESSION_KEY_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(auth_get_eap_session_keys_resp_msg_v01, session_key) - QMI_IDL_OFFSET8(auth_get_eap_session_keys_resp_msg_v01, session_key_len)
};

/*
 * auth_end_eap_session_req_msg is empty
 * static const uint8_t auth_end_eap_session_req_msg_data_v01[] = {
 * };
 */

static const uint8_t auth_end_eap_session_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_end_eap_session_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t auth_run_aka_algo_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(auth_run_aka_algo_req_msg_v01, aka_ver),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(auth_run_aka_algo_req_msg_v01, aka_params) - QMI_IDL_OFFSET8(auth_run_aka_algo_req_msg_v01, aka_params_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_run_aka_algo_req_msg_v01, aka_params),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t auth_run_aka_algo_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_run_aka_algo_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(auth_run_aka_algo_resp_msg_v01, aka_handle)
};

static const uint8_t auth_aka_algo_result_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_aka_algo_result_ind_msg_v01, aka_algo_result),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(auth_aka_algo_result_ind_msg_v01, aka_response_data) - QMI_IDL_OFFSET8(auth_aka_algo_result_ind_msg_v01, aka_response_data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_aka_algo_result_ind_msg_v01, aka_response_data),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t auth_set_subscription_binding_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(auth_set_subscription_binding_req_msg_v01, bind_subs)
};

static const uint8_t auth_set_subscription_binding_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_set_subscription_binding_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * auth_get_bind_subscription_req_msg is empty
 * static const uint8_t auth_get_bind_subscription_req_msg_data_v01[] = {
 * };
 */

static const uint8_t auth_get_bind_subscription_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_get_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(auth_get_bind_subscription_resp_msg_v01, bind_subscription) - QMI_IDL_OFFSET8(auth_get_bind_subscription_resp_msg_v01, bind_subscription_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(auth_get_bind_subscription_resp_msg_v01, bind_subscription)
};

static const uint8_t auth_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(auth_indication_register_req_msg_v01, report_eap_notification_code) - QMI_IDL_OFFSET8(auth_indication_register_req_msg_v01, report_eap_notification_code_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(auth_indication_register_req_msg_v01, report_eap_notification_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(auth_indication_register_req_msg_v01, report_eap_error_code) - QMI_IDL_OFFSET8(auth_indication_register_req_msg_v01, report_eap_error_code_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(auth_indication_register_req_msg_v01, report_eap_error_code),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(auth_indication_register_req_msg_v01, report_eap_auth_reject) - QMI_IDL_OFFSET8(auth_indication_register_req_msg_v01, report_eap_auth_reject_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(auth_indication_register_req_msg_v01, report_eap_auth_reject)
};

static const uint8_t auth_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t auth_eap_notification_code_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(auth_eap_notification_code_ind_msg_v01, eap_notification_code)
};

static const uint8_t auth_eap_error_code_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(auth_eap_error_code_ind_msg_v01, eap_error_code)
};

/*
 * auth_eap_auth_rej_ind_msg is empty
 * static const uint8_t auth_eap_auth_rej_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t auth_send_eap_packet_ext_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_FIRST_EXTENDED |  QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS |  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS,
  QMI_IDL_OFFSET8(auth_send_eap_packet_ext_req_msg_v01, eap_request_ext_pkt),
  ((QMI_AUTH_EAP_REQ_PACKET_EXT_MAX_V01) & 0xFF), ((QMI_AUTH_EAP_REQ_PACKET_EXT_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(auth_send_eap_packet_ext_req_msg_v01, eap_request_ext_pkt) - QMI_IDL_OFFSET8(auth_send_eap_packet_ext_req_msg_v01, eap_request_ext_pkt_len)
};

static const uint8_t auth_send_eap_packet_ext_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(auth_send_eap_packet_ext_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_FIRST_EXTENDED |  QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS |  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS,
  QMI_IDL_OFFSET8(auth_send_eap_packet_ext_resp_msg_v01, eap_response_ext_pkt),
  ((QMI_AUTH_EAP_RESP_PACKET_EXT_MAX_V01) & 0xFF), ((QMI_AUTH_EAP_RESP_PACKET_EXT_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(auth_send_eap_packet_ext_resp_msg_v01, eap_response_ext_pkt) - QMI_IDL_OFFSET8(auth_send_eap_packet_ext_resp_msg_v01, eap_response_ext_pkt_len)
};

static const uint8_t auth_get_eap_auth_credentials_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(auth_get_eap_auth_credentials_req_msg_v01, cred_request_mask)
};

static const uint8_t auth_get_eap_auth_credentials_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(auth_get_eap_auth_credentials_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(auth_get_eap_auth_credentials_resp_msg_v01, msk) - QMI_IDL_OFFSET8(auth_get_eap_auth_credentials_resp_msg_v01, msk_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(auth_get_eap_auth_credentials_resp_msg_v01, msk),
  ((QMI_AUTH_EAP_INFO_MAX_V01) & 0xFF), ((QMI_AUTH_EAP_INFO_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(auth_get_eap_auth_credentials_resp_msg_v01, msk) - QMI_IDL_OFFSET8(auth_get_eap_auth_credentials_resp_msg_v01, msk_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(auth_get_eap_auth_credentials_resp_msg_v01, ext_msk) - QMI_IDL_OFFSET16RELATIVE(auth_get_eap_auth_credentials_resp_msg_v01, ext_msk_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(auth_get_eap_auth_credentials_resp_msg_v01, ext_msk),
  ((QMI_AUTH_EAP_INFO_MAX_V01) & 0xFF), ((QMI_AUTH_EAP_INFO_MAX_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(auth_get_eap_auth_credentials_resp_msg_v01, ext_msk) - QMI_IDL_OFFSET16RELATIVE(auth_get_eap_auth_credentials_resp_msg_v01, ext_msk_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(auth_get_eap_auth_credentials_resp_msg_v01, session_id) - QMI_IDL_OFFSET16RELATIVE(auth_get_eap_auth_credentials_resp_msg_v01, session_id_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(auth_get_eap_auth_credentials_resp_msg_v01, session_id),
  ((QMI_AUTH_EAP_INFO_MAX_V01) & 0xFF), ((QMI_AUTH_EAP_INFO_MAX_V01) >> 8),
  QMI_IDL_OFFSET16RELATIVE(auth_get_eap_auth_credentials_resp_msg_v01, session_id) - QMI_IDL_OFFSET16RELATIVE(auth_get_eap_auth_credentials_resp_msg_v01, session_id_len)
};

/* Type Table */
static const qmi_idl_type_table_entry  auth_type_table_v01[] = {
  {sizeof(auth_aka_params_v01), auth_aka_params_data_v01},
  {sizeof(auth_aka_algo_result_v01), auth_aka_algo_result_data_v01},
  {sizeof(auth_aka_response_data_v01), auth_aka_response_data_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry auth_message_table_v01[] = {
  {sizeof(auth_reset_req_msg_v01), 0},
  {sizeof(auth_reset_resp_msg_v01), auth_reset_resp_msg_data_v01},
  {sizeof(auth_start_eap_session_req_msg_v01), auth_start_eap_session_req_msg_data_v01},
  {sizeof(auth_start_eap_session_resp_msg_v01), auth_start_eap_session_resp_msg_data_v01},
  {sizeof(auth_send_eap_packet_req_msg_v01), auth_send_eap_packet_req_msg_data_v01},
  {sizeof(auth_send_eap_packet_resp_msg_v01), auth_send_eap_packet_resp_msg_data_v01},
  {sizeof(auth_eap_session_result_ind_msg_v01), auth_eap_session_result_ind_msg_data_v01},
  {sizeof(auth_get_eap_session_keys_req_msg_v01), 0},
  {sizeof(auth_get_eap_session_keys_resp_msg_v01), auth_get_eap_session_keys_resp_msg_data_v01},
  {sizeof(auth_end_eap_session_req_msg_v01), 0},
  {sizeof(auth_end_eap_session_resp_msg_v01), auth_end_eap_session_resp_msg_data_v01},
  {sizeof(auth_run_aka_algo_req_msg_v01), auth_run_aka_algo_req_msg_data_v01},
  {sizeof(auth_run_aka_algo_resp_msg_v01), auth_run_aka_algo_resp_msg_data_v01},
  {sizeof(auth_aka_algo_result_ind_msg_v01), auth_aka_algo_result_ind_msg_data_v01},
  {sizeof(auth_set_subscription_binding_req_msg_v01), auth_set_subscription_binding_req_msg_data_v01},
  {sizeof(auth_set_subscription_binding_resp_msg_v01), auth_set_subscription_binding_resp_msg_data_v01},
  {sizeof(auth_get_bind_subscription_req_msg_v01), 0},
  {sizeof(auth_get_bind_subscription_resp_msg_v01), auth_get_bind_subscription_resp_msg_data_v01},
  {sizeof(auth_indication_register_req_msg_v01), auth_indication_register_req_msg_data_v01},
  {sizeof(auth_indication_register_resp_msg_v01), auth_indication_register_resp_msg_data_v01},
  {sizeof(auth_eap_notification_code_ind_msg_v01), auth_eap_notification_code_ind_msg_data_v01},
  {sizeof(auth_eap_error_code_ind_msg_v01), auth_eap_error_code_ind_msg_data_v01},
  {sizeof(auth_eap_auth_rej_ind_msg_v01), 0},
  {sizeof(auth_send_eap_packet_ext_req_msg_v01), auth_send_eap_packet_ext_req_msg_data_v01},
  {sizeof(auth_send_eap_packet_ext_resp_msg_v01), auth_send_eap_packet_ext_resp_msg_data_v01},
  {sizeof(auth_get_eap_auth_credentials_req_msg_v01), auth_get_eap_auth_credentials_req_msg_data_v01},
  {sizeof(auth_get_eap_auth_credentials_resp_msg_v01), auth_get_eap_auth_credentials_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object auth_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *auth_qmi_idl_type_table_object_referenced_tables_v01[] =
{&auth_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object auth_qmi_idl_type_table_object_v01 = {
  sizeof(auth_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(auth_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  auth_type_table_v01,
  auth_message_table_v01,
  auth_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry auth_service_command_messages_v01[] = {
  {QMI_AUTH_RESET_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_AUTH_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 18), 12},
  {QMI_AUTH_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_AUTH_GET_SUPPORTED_FIELDS_REQ_V01, QMI_IDL_TYPE16(1, 2), 5},
  {QMI_AUTH_START_EAP_SESSION_REQ_V01, QMI_IDL_TYPE16(0, 2), 532},
  {QMI_AUTH_SEND_EAP_PACKET_REQ_V01, QMI_IDL_TYPE16(0, 4), 261},
  {QMI_AUTH_GET_EAP_SESSION_KEYS_REQ_V01, QMI_IDL_TYPE16(0, 7), 0},
  {QMI_AUTH_END_EAP_SESSION_REQ_V01, QMI_IDL_TYPE16(0, 9), 0},
  {QMI_AUTH_RUN_AKA_ALGO_REQ_V01, QMI_IDL_TYPE16(0, 11), 519},
  {QMI_AUTH_SET_SUBSCRIPTION_BINDING_REQ_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_AUTH_GET_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 16), 0},
  {QMI_AUTH_SEND_EAP_PACKET_EXT_REQ_V01, QMI_IDL_TYPE16(0, 23), 4101},
  {QMI_AUTH_GET_EAP_AUTH_CREDENTIALS_REQ_V01, QMI_IDL_TYPE16(0, 25), 7}
};

static const qmi_idl_service_message_table_entry auth_service_response_messages_v01[] = {
  {QMI_AUTH_RESET_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_AUTH_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 19), 7},
  {QMI_AUTH_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_AUTH_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_AUTH_START_EAP_SESSION_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_AUTH_SEND_EAP_PACKET_RESP_V01, QMI_IDL_TYPE16(0, 5), 268},
  {QMI_AUTH_GET_EAP_SESSION_KEYS_RESP_V01, QMI_IDL_TYPE16(0, 8), 268},
  {QMI_AUTH_END_EAP_SESSION_RESP_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_AUTH_RUN_AKA_ALGO_RESP_V01, QMI_IDL_TYPE16(0, 12), 14},
  {QMI_AUTH_SET_SUBSCRIPTION_BINDING_RESP_V01, QMI_IDL_TYPE16(0, 15), 7},
  {QMI_AUTH_GET_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 17), 14},
  {QMI_AUTH_SEND_EAP_PACKET_EXT_RESP_V01, QMI_IDL_TYPE16(0, 24), 4108},
  {QMI_AUTH_GET_EAP_AUTH_CREDENTIALS_RESP_V01, QMI_IDL_TYPE16(0, 26), 790}
};

static const qmi_idl_service_message_table_entry auth_service_indication_messages_v01[] = {
  {QMI_AUTH_EAP_SESSION_RESULT_IND_V01, QMI_IDL_TYPE16(0, 6), 4},
  {QMI_AUTH_AKA_ALGO_RESULT_IND_V01, QMI_IDL_TYPE16(0, 13), 523},
  {QMI_AUTH_EAP_NOTIFICATION_CODE_IND_V01, QMI_IDL_TYPE16(0, 20), 5},
  {QMI_AUTH_EAP_ERROR_CODE_IND_V01, QMI_IDL_TYPE16(0, 21), 5},
  {QMI_AUTH_EAP_AUTH_REJ_IND_V01, QMI_IDL_TYPE16(0, 22), 0}
};

/*Service Object*/
struct qmi_idl_service_object auth_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x07,
  8204,
  { sizeof(auth_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(auth_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(auth_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { auth_service_command_messages_v01, auth_service_response_messages_v01, auth_service_indication_messages_v01},
  &auth_qmi_idl_type_table_object_v01,
  0x0A,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type auth_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( AUTH_V01_IDL_MAJOR_VERS != idl_maj_version || AUTH_V01_IDL_MINOR_VERS != idl_min_version
       || AUTH_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&auth_qmi_idl_service_object_v01;
}

