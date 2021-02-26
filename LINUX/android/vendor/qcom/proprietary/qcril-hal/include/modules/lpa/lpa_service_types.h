/******************************************************************************
@file    lpa_service_types.h
@brief   qcril uim lpa service types

---------------------------------------------------------------------------

Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
---------------------------------------------------------------------------
******************************************************************************/
#pragma once

/*===========================================================================

                           TYPES

===========================================================================*/

typedef enum
{
  LPA_SERVICE_UNKNOWN_EVENT_ID   = 0,
  LPA_SERVICE_ADD_PROFILE        = 1,
  LPA_SERVICE_ENABLE_PROFILE     = 2,
  LPA_SERVICE_DISABLE_PROFILE    = 3,
  LPA_SERVICE_DELETE_PROFILE     = 4,
  LPA_SERVICE_EUICC_MEMORY_RESET = 5,
  LPA_SERVICE_GET_PROFILE        = 6,
  LPA_SERVICE_UPDATE_NICKNAME    = 7,
  LPA_SERVICE_GET_EID            = 8,
  LPA_SERVICE_USER_CONSENT       = 9,
  LPA_SERVICE_SRV_ADDR_OPERATION = 10
} lpa_service_user_event_type;

typedef enum
{
  LPA_SERVICE_RESULT_SUCCESS = 0,
  LPA_SERVICE_RESULT_FAILURE = 1,
  LPA_SERVICE_RESULT_CONFRM_CODE_MISSING = 2
} lpa_service_result_type;

typedef enum
{
  LPA_SERVICE_PROFILE_CLASS_TEST         = 0,
  LPA_SERVICE_PROFILE_CLASS_PROVISIONING = 1,
  LPA_SERVICE_PROFILE_CLASS_OPERATIONAL  = 2
} lpa_service_profile_class_type;

typedef enum
{
  LPA_SERVICE_PROFILE_TYPE_DISABLE_NOT_ALLOWED = 1,
  LPA_SERVICE_PROFILE_TYPE_DELETE_NOT_ALLOWED  = 2,
  LPA_SERVICE_PROFILE_TYPE_DELETE_ON_DISABLE   = 4
} lpa_service_profile_policy_mask;

typedef enum
{
  LPA_SERVICE_EUICC_RESET_TEST_PROFILES           = 1,
  LPA_SERVICE_EUICC_RESET_OPERATIONAL_PROFILES    = 2,
  LPA_SERVICE_EUICC_RESET_TO_DEFAULT_SMDP_ADDRESS = 4
} lpa_service_euicc_reset_mask;

typedef enum
{
  LPA_SERVICE_SERVER_ADDRESS_GET_OPERATION = 1,
  LPA_SERVICE_SERVER_ADDRESS_SET_OPERATION = 2
} lpa_service_srv_addr_op_type;

typedef enum
{
  LPA_SERVICE_PROFILE_STATE_UNKNOWN  = 0,
  LPA_SERVICE_PROFILE_STATE_ACTIVE   = 1,
  LPA_SERVICE_PROFILE_STATE_INACTIVE = 2
} lpa_service_profile_state_type;

typedef enum
{
  LPA_SERVICE_ICON_TYPE_UNKNOWN = 0,
  LPA_SERVICE_ICON_TYPE_JPEG    = 1,
  LPA_SERVICE_ICON_TYPE_PNG     = 2
} lpa_service_icon_type;

typedef enum
{
  LPA_SERVICE_ADD_PROFILE_STATUS_NONE                  = 0,
  LPA_SERVICE_ADD_PROFILE_STATUS_ERROR                 = 1,
  LPA_SERVICE_ADD_PROFILE_STATUS_DOWNLOAD_PROGRESS     = 2,
  LPA_SERVICE_ADD_PROFILE_STATUS_INSTALLATION_PROGRESS = 3,
  LPA_SERVICE_ADD_PROFILE_STATUS_INSTALLATION_COMPLETE = 4,
  LPA_SERVICE_ADD_PROFILE_STATUS_GET_USER_CONSENT      = 5
} lpa_service_add_profile_status_type;

typedef enum
{
  LPA_SERVICE_ADD_PROFILE_FAILURE_CAUSE_NONE    = 0,
  LPA_SERVICE_ADD_PROFILE_FAILURE_CAUSE_GENERIC = 1,
  LPA_SERVICE_ADD_PROFILE_FAILURE_CAUSE_SIM     = 2,
  LPA_SERVICE_ADD_PROFILE_FAILURE_CAUSE_NETWORK = 3,
  LPA_SERVICE_ADD_PROFILE_FAILURE_CAUSE_MEMORY  = 4
} lpa_service_add_profile_failure_cause;

typedef struct
{
  const char   * headerName;
  const char   * headerValue;
} lpa_service_http_custom_header_req_type;

typedef struct
{
  char   * headerName;
  char   * headerValue;
} lpa_service_http_custom_header_type;

typedef struct
{
  int32_t                               tokenId;
  uint8_t                             * payload;
  uint32_t                              payload_len;
  char                                * contentType;
  lpa_service_http_custom_header_type * customHeaders;
  uint8_t                               no_of_headers;
  char                                * url;
} lpa_service_http_transaction_ind_type;

typedef struct
{
  int32_t                                   tokenId;
  lpa_service_result_type                   result;
  const uint8_t                           * payload;
  uint32_t                                  payload_len;
  lpa_service_http_custom_header_req_type * customHeaders;
  uint8_t                                   no_of_headers;
} lpa_service_http_transaction_req_type;

typedef struct
{
  lpa_service_srv_addr_op_type     opCode;
  const char                     * smdpAddress;
} lpa_service_srv_addr_op_req_type;

typedef struct
{
  lpa_service_user_event_type           event;
  const char                          * activationCode;   /* UIM_LPA_ADD_PROFILE */
  const char                          * confirmationCode; /* UIM_LPA_ADD_PROFILE */
  const char                          * nickname;         /* UIM_LPA_UPDATE_NICKNAME */
  const uint8_t                       * iccid;            /* UIM_LPA_ENABLE_PROFILE
                                                             UIM_LPA_DISABLE_PROFILE
                                                             UIM_LPA_DELETE_PROFILE
                                                             UIM_LPA_UPDATE_NICKNAME */
  uint32_t                              iccid_len;
  int32_t                               resetMask;        /* UIM_LPA_EUICC_MEMORY_RESET */
  bool                                  userOk;           /* UIM_LPA_USER_CONSENT */
  lpa_service_srv_addr_op_req_type      srvOpReq;         /* UIM_LPA_SRV_ADDR_OPERATION */
} lpa_service_user_req_type;

typedef struct
{
  lpa_service_profile_state_type    state;
  uint8_t                         * iccid;
  uint32_t                          iccid_len;
  char                            * profileName;
  char                            * nickName;
  char                            * spName;
  lpa_service_icon_type             iconType;
  uint8_t                         * icon;
  uint32_t                          icon_len;
  lpa_service_profile_class_type    profileClass;
  lpa_service_profile_policy_mask   profilePolicy;
} lpa_service_profile_info_type;

typedef struct
{
  char  * smdpAddress;
  char  * smdsAddress;
} lpa_service_srv_addr_op_resp_type;

typedef struct
{
  lpa_service_user_event_type         event;
  lpa_service_result_type             result;
  uint8_t                           * eid;
  uint32_t                            eid_len;
  lpa_service_profile_info_type     * profiles;
  uint8_t                             no_of_profiles;
  lpa_service_srv_addr_op_resp_type   srvAddr;
} lpa_service_user_resp_type;

typedef struct
{
  lpa_service_add_profile_status_type    status;
  lpa_service_add_profile_failure_cause  cause;
  int32_t                                progress;
  lpa_service_profile_policy_mask        policyMask;
  bool                                   userConsentRequired;
} lpa_service_add_profile_progress_ind_type;
