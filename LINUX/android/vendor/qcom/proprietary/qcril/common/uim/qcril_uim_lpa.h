#ifndef QCRIL_UIM_LPA_H
#define QCRIL_UIM_LPA_H
/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/16   vr      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "user_identity_module_v01.h"
#include "qcril_uim.h"

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
} qcril_uim_lpa_user_event_type;

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
  QCRIL_UIM_LPA_SERVER_ADDRESS_GET_OPERATION = 1,
  QCRIL_UIM_LPA_SERVER_ADDRESS_SET_OPERATION = 2
} qcril_uim_lpa_srv_addr_op_type;

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

typedef enum
{
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_NONE                  = 0,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_ERROR                 = 1,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_DOWNLOAD_PROGRESS     = 2,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_PROGRESS = 3,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_COMPLETE = 4,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_GET_USER_CONSENT      = 5
} qcril_uim_lpa_add_profile_status_type;

typedef enum
{
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NONE    = 0,
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_GENERIC = 1,
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_SIM     = 2,
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NETWORK = 3,
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_MEMORY  = 4
} qcril_uim_lpa_add_profile_failure_cause;

typedef struct
{
  qcril_uim_lpa_srv_addr_op_type   opCode;
  const char                     * smdpAddress;
} qcril_uim_lpa_srv_addr_op_req_type;

typedef struct
{
  qcril_uim_lpa_user_event_type         event;
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
  qcril_uim_lpa_user_event_type         event;
  qcril_uim_lpa_result_type             result;
  uint8_t                             * eid;
  uint32_t                              eid_len;
  qcril_uim_lpa_profile_info_type     * profiles;
  uint8_t                               no_of_profiles;
  qcril_uim_lpa_srv_addr_op_resp_type   srvAddr;
} qcril_uim_lpa_user_resp_type;

typedef struct
{
  qcril_uim_lpa_add_profile_status_type    status;
  qcril_uim_lpa_add_profile_failure_cause  cause;
  int32_t                                  progress;
  qcril_uim_lpa_profile_policy_mask        policyMask;
  bool                                     userConsentRequired;
} qcril_uim_lpa_add_profile_progress_ind_type;


/*===========================================================================

                           FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_lpa_cleanup_cached_iccids

===========================================================================*/
void qcril_uim_lpa_cleanup_cached_iccids
(
);

/*===========================================================================
  FUNCTION  qcril_qmi_uim_lpa_add_profile_ind_hdlr
===========================================================================*/
int qcril_qmi_uim_lpa_add_profile_ind_hdlr
(
  uim_add_profile_ind_msg_v01               * qcci_data_ptr,
  qmi_uim_indication_data_type              * qmi_data_ptr
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

  FUNCTION:  qcril_uim_lpa_process_qmi_callback

===========================================================================*/
void qcril_uim_lpa_process_qmi_callback
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_lpa_user_request

===========================================================================*/
void qcril_uim_lpa_user_request
(
  void                            * token,
  qcril_uim_lpa_user_req_type     * user_req
);

/*===========================================================================

FUNCTION:  qcril_uim_lpa_user_response

===========================================================================*/
void qcril_uim_lpa_user_response
(
  void                         * token_ptr,
  qcril_uim_lpa_user_resp_type * user_resp_ptr
);

/*===========================================================================

FUNCTION:  qcril_uim_lpa_add_profile_progress_ind

===========================================================================*/
void qcril_uim_lpa_add_profile_progress_ind
(
  qcril_uim_lpa_add_profile_progress_ind_type * prog_ind_ptr
);

#endif /* QCRIL_UIM_LPA_H */
