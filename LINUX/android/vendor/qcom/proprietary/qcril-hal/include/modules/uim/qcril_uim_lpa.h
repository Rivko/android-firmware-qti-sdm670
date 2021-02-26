#ifndef QCRIL_UIM_LPA_H
#define QCRIL_UIM_LPA_H
/*===========================================================================

  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "modules/uim/qcril_uim.h"
#include "modules/uim/UimLpaReqMessage.h"

/*===========================================================================

                           TYPES

===========================================================================*/

typedef enum
{
  QCRIL_UIM_LPA_UNKNOWN_EVENT_ID   = 0,
  QCRIL_UIM_LPA_ADD_PROFILE        = 1,
  QCRIL_UIM_LPA_ENABLE_PROFILE     = 2,
  QCRIL_UIM_LPA_DISABLE_PROFILE    = 3,
  QCRIL_UIM_LPA_DELETE_PROFILE     = 4,
  QCRIL_UIM_LPA_EUICC_MEMORY_RESET = 5,
  QCRIL_UIM_LPA_GET_PROFILE        = 6,
  QCRIL_UIM_LPA_UPDATE_NICKNAME    = 7,
  QCRIL_UIM_LPA_GET_EID            = 8,
  QCRIL_UIM_LPA_USER_CONSENT       = 9,
  QCRIL_UIM_LPA_SRV_ADDR_OPERATION = 10
} qcril_uim_lpa_user_req_event_type;

typedef enum
{
  QCRIL_UIM_LPA_SERVER_ADDRESS_GET_OPERATION = 1,
  QCRIL_UIM_LPA_SERVER_ADDRESS_SET_OPERATION = 2
} qcril_uim_lpa_srv_addr_op_type;

typedef enum
{
  QCRIL_UIM_LPA_RESULT_SUCCESS = 0,
  QCRIL_UIM_LPA_RESULT_FAILURE = 1,
  QCRIL_UIM_LPA_RESULT_CONFRM_CODE_MISSING = 2
} qcril_uim_lpa_result_type;

typedef enum
{
  QCRIL_UIM_LPA_PROFILE_CLASS_TEST         = 0,
  QCRIL_UIM_LPA_PROFILE_CLASS_PROVISIONING = 1,
  QCRIL_UIM_LPA_PROFILE_CLASS_OPERATIONAL  = 2
} qcril_uim_lpa_profile_class_type;

typedef enum
{
  QCRIL_UIM_LPA_PROFILE_TYPE_DISABLE_NOT_ALLOWED = 1,
  QCRIL_UIM_LPA_PROFILE_TYPE_DELETE_NOT_ALLOWED  = 2,
  QCRIL_UIM_LPA_PROFILE_TYPE_DELETE_ON_DISABLE   = 4
} qcril_uim_lpa_profile_policy_mask;

typedef enum
{
  QCRIL_UIM_LPA_EUICC_RESET_TEST_PROFILES           = 1,
  QCRIL_UIM_LPA_EUICC_RESET_OPERATIONAL_PROFILES    = 2,
  QCRIL_UIM_LPA_EUICC_RESET_TO_DEFAULT_SMDP_ADDRESS = 4
} qcril_uim_lpa_euicc_reset_mask;

typedef enum
{
  QCRIL_UIM_LPA_PROFILE_STATE_UNKNOWN  = 0,
  QCRIL_UIM_LPA_PROFILE_STATE_ACTIVE   = 1,
  QCRIL_UIM_LPA_PROFILE_STATE_INACTIVE = 2
} qcril_uim_lpa_profile_state_type;

typedef enum
{
  QCRIL_UIM_LPA_ICON_TYPE_UNKNOWN = 0,
  QCRIL_UIM_LPA_ICON_TYPE_JPEG    = 1,
  QCRIL_UIM_LPA_ICON_TYPE_PNG     = 2
} qcril_uim_lpa_icon_type;

typedef struct
{
  qcril_uim_lpa_srv_addr_op_type   opCode;
  char                           * smdpAddress;
} qcril_uim_lpa_srv_addr_op_req_type;

typedef struct
{
  qcril_uim_lpa_user_req_event_type     event;
  char                                * activationCode;   /* UIM_LPA_ADD_PROFILE */
  char                                * confirmationCode; /* UIM_LPA_ADD_PROFILE */
  char                                * nickname;         /* UIM_LPA_UPDATE_NICKNAME */
  uint8_t                             * iccid;            /* UIM_LPA_ENABLE_PROFILE
                                                             UIM_LPA_DISABLE_PROFILE
                                                             UIM_LPA_DELETE_PROFILE
                                                             UIM_LPA_UPDATE_NICKNAME */
  uint32_t                              iccid_len;
  int32_t                               resetMask;        /* UIM_LPA_EUICC_MEMORY_RESET */
  bool                                  userOk;           /* UIM_LPA_USER_CONSENT */
  qcril_uim_lpa_srv_addr_op_req_type    srvOpReq;         /* UIM_LPA_SRV_ADDR_OPERATION */
} qcril_uim_lpa_user_req_type;

typedef struct
{
  qcril_uim_lpa_profile_state_type      state;
  uint8_t                             * iccid;
  uint32_t                              iccid_len;
  char                                * profileName;
  char                                * nickName;
  char                                * spName;
  qcril_uim_lpa_icon_type               iconType;
  uint8_t                             * icon;
  uint32_t                              icon_len;
  qcril_uim_lpa_profile_class_type      profileClass;
  qcril_uim_lpa_profile_policy_mask     profilePolicy;
} qcril_uim_lpa_profile_info_type;

typedef struct
{
  char  * smdpAddress;
  char  * smdsAddress;
} qcril_uim_lpa_srv_addr_op_resp_type;

typedef struct
{
  qcril_uim_lpa_user_req_event_type     event;
  qcril_uim_lpa_result_type             result;
  uint8_t                             * eid;
  uint32_t                              eid_len;
  qcril_uim_lpa_profile_info_type     * profiles;
  uint8_t                               no_of_profiles;
  qcril_uim_lpa_srv_addr_op_resp_type   srvAddr;
} qcril_uim_lpa_user_resp_type;

/*=========================================================================

  FUNCTION:  qcril_uim_lpa_process_qmi_callback

===========================================================================*/
void qcril_uim_lpa_process_qmi_callback
(
  qmi_uim_rsp_id_type                 msg_id,
  qcril_uim_callback_params_type    * resp_ptr,
  void                              * orig_req_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_lpa_user_request

===========================================================================*/
boolean qcril_uim_lpa_user_request
(
  std::shared_ptr<UimLpaReqMessage> req_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_lpa_add_profile_ind

===========================================================================*/
void qcril_uim_lpa_add_profile_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_lpa_cleanup_cached_iccids

===========================================================================*/
void qcril_uim_lpa_cleanup_cached_iccids();

#endif /* QCRIL_UIM_LPA_H */
